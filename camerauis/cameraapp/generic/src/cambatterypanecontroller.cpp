/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implemantation for CCamBatteryPaneController class.
*
*/


#include "cambatterypanecontroller.h"

#include <e32base.h>
#include <e32property.h>                // RProperty
#include <hwrmpowerstatesdkpskeys.h>    // P&S keys
#include <eikdef.h>                     // KEikColorResourceChange
#include <AknsConstants.h>              // KAknsMessageSkinChange 

#include "cambatterypanedrawer.h"
#include "CamPropertyWatcher.h"
#include "CamUtility.h"
#include "CamAppUi.h"


// ---------------------------------------------------------------------------
// CCamBatteryPaneController::CCamBatteryPaneController
// ---------------------------------------------------------------------------
//
CCamBatteryPaneController::CCamBatteryPaneController(
    MCamBatteryPaneObserver& aObserver, TBool aCallbackActive ) :
    iObserver( aObserver ), iCallbackActive( aCallbackActive )
    {
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::~CCamBatteryPaneController
// ---------------------------------------------------------------------------
//
CCamBatteryPaneController::~CCamBatteryPaneController()
  {
  PRINT ( _L("Camera => ~CCamBatteryPaneController") );
  
  if (iTicker)
    {
    iTicker->Cancel();
    }
  delete iTicker;	
  delete iDrawer;	 
  delete iBatteryStrengthWatcher;    
  delete iBatteryChargingWatcher;
  
  PRINT ( _L("Camera <= ~CCamBatteryPaneController") ); 	   
  }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::NewL
// ---------------------------------------------------------------------------
//
CCamBatteryPaneController* CCamBatteryPaneController::NewL(
    MCamBatteryPaneObserver& aObserver,
    TBool aCallbackActive )
    {
    PRINT ( _L("Camera => CCamBatteryPaneController::NewL") );		
    
    CCamBatteryPaneController* self = new (ELeave) CCamBatteryPaneController(
            aObserver, aCallbackActive );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    PRINT ( _L("Camera <= CCamBatteryPaneController::NewL") );
    return self;    
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::ConstructL
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneController::ConstructL()
    {
    PRINT ( _L("Camera => CCamBatteryPaneController::ConstructL") );	
    
    iDrawer = CCamBatteryPaneDrawer::NewL();
    iTicker = CPeriodic::NewL( CActive::EPriorityHigh );
    
    // Construct property watchers for battery level and charging status
    iBatteryStrengthWatcher = CCamPropertyWatcher::NewL( *this, KPSUidHWRMPowerState, KHWRMBatteryLevel );  
    iBatteryChargingWatcher = CCamPropertyWatcher::NewL( *this, KPSUidHWRMPowerState, KHWRMChargingStatus );
      
    // Subscribe to the properties 
    iBatteryStrengthWatcher->Subscribe();
    iBatteryChargingWatcher->Subscribe();   
    
    // Read initial values for battery level and charging status
    ReadCurrentState();
    
    PRINT ( _L( "Camera <= CCamBatteryPaneController::ConstructL" ) );	
    }


// ---------------------------------------------------------------------------
// CCamBatteryPaneController::Pause
// ---------------------------------------------------------------------------
//    
void CCamBatteryPaneController::Pause( TBool aPause )
    {
    PRINT1 ( _L("Camera => CCamBatteryPaneController::Pause( %d)"), aPause );	
    if( aPause )
      {
      // Pause battery pane controller  
      if( !iPaused )
        {  
        // Prevent callbacks
        TBool oldCbState = iCallbackActive; 
        iCallbackActive = EFalse;
        // Stop recharging
        if( iRecharging )
          {
          StopRecharging(); 
          }
        // Cancel notifications of battery events      
        iBatteryStrengthWatcher->Cancel();
        iBatteryChargingWatcher->Cancel();  
        iPaused = ETrue;
        // Restore callback state
        iCallbackActive = oldCbState;
        } 
      }
    else
      {
      // Restart battery pane controller  
      if( iPaused )          
        {
        // Re-subscribe to battery events  
        iBatteryStrengthWatcher->Subscribe();
        iBatteryChargingWatcher->Subscribe();
        ReadCurrentState();
        iPaused = EFalse;
        }
      }  
    PRINT( _L("Camera <= CCamBatteryPaneController::Pause") );	  
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::SetCallbackActive
// ---------------------------------------------------------------------------
//    
void CCamBatteryPaneController::SetCallbackActive( TBool aActive )
    {
    iCallbackActive = aActive;
    }


// ---------------------------------------------------------------------------
// CCamBatteryPaneController::IsBatteryLow
// ---------------------------------------------------------------------------
//    
TBool CCamBatteryPaneController::IsBatteryLow()
    {
    return ( ( iBatteryStrength >= 0 )&& ( iBatteryStrength <= 2 )
            && !iRecharging );
    }



// ---------------------------------------------------------------------------
// CCamBatteryPaneController::SetBatteryStrength
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneController::SetBatteryStrength( TInt aStrength )
    {    	
    PRINT( _L("Camera => CCamBatteryPaneController::SetBatteryStrength") );	    	
    
    // Drawer uses battery strength for indexing a table, so make
    // sure it stays within valid boundaries
    if( aStrength > KMaxBatteryStrength )
        {
        iBatteryStrength = KMaxBatteryStrength;
        }
    else if( aStrength < KMinBatteryStrength )
        {
        iBatteryStrength = KMinBatteryStrength;        
        }
    else
        {
        iBatteryStrength = aStrength;    
        }
        
    if( !iRecharging )
        {        
        iDrawer->SetBatteryStrength( iBatteryStrength );
        NotifyObserver();
        }
        
    PRINT( _L("Camera <= CCamBatteryPaneController::SetBatteryStrength") );	        
    }    
 
// ---------------------------------------------------------------------------
// CCamBatteryPaneController::SetLocation
// ---------------------------------------------------------------------------    
//
void CCamBatteryPaneController::SetLocation( TPoint aLocation )
    {
    iDrawer->SetLocation( aLocation );
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::Rect
// ---------------------------------------------------------------------------    
//
TRect CCamBatteryPaneController::Rect() const
	{
	return iDrawer->Rect();	
	}

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::Draw
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneController::Draw( CBitmapContext& aGc ) const
//void CCamBatteryPaneController::Draw( CWindowGc& aGc ) const
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi());
    
    // When battery recharge animation runs over, indicator needs clearing
    // PostCap view has redraw functions of its own, so it doesn't need this
    if ( iRechargeBatteryStrength == 0 && 
         iRecharging &&
         !appUi->IsBurstEnabled() &&
		 appUi->CurrentViewState() != ECamViewStatePostCapture && 
         !(appUi->IsSecondCameraEnabled() && appUi->IsQwerty2ndCamera() ) )
        {    
        iDrawer->ClearBattery( aGc );
        }
    
    iDrawer->Draw( aGc );
    }    

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::StartRecharging
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneController::StartRecharging()
    {
    PRINT( _L("Camera => CCamBatteryPaneController::StartRecharging") );  	
    
    // Stop current timer
    iTicker->Cancel();
    
    // Initialize
    iRecharging = ETrue;
    iRechargeBatteryStrength = KMinBatteryStrength; 
      
    // Give initial strength value to drawer and notify observer
    iDrawer->SetBatteryStrength( iRechargeBatteryStrength );
    NotifyObserver();
    
    // Start the animation
    iTicker->Start( KBatteryRechargeTick, KBatteryRechargeTick, TCallBack(TickerCallback, this) );
    
    PRINT( _L("Camera => CCamBatteryPaneController::StartRecharging") );  
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::StopRecharging
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneController::StopRecharging()
    {
    PRINT( _L("Camera => CCamBatteryPaneController::StopRecharging") ); 	
    
    // Stop the timer and give the original battery
    // strength to drawer class
    iTicker->Cancel();
    iRecharging = EFalse;
    iDrawer->SetBatteryStrength( iBatteryStrength );
    NotifyObserver();
    
    PRINT( _L("Camera <= CCamBatteryPaneController::StopRecharging") ); 
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::NotifyObserver
// ---------------------------------------------------------------------------    
//
void CCamBatteryPaneController::NotifyObserver()
    {
    if( iCallbackActive )
        {
        iObserver.BatteryPaneUpdated();
        }
    }
    
// ---------------------------------------------------------------------------
// CCamBatteryPaneController::UpdateRechargeBatteryStrength
// ---------------------------------------------------------------------------    
//
void CCamBatteryPaneController::UpdateRechargeBatteryStrength()
    {    
    PRINT( _L("Camera => CCamBatteryPaneController::UpdateRechargeBatteryStrength") );	
    
    iRechargeBatteryStrength++;
    if( iRechargeBatteryStrength > KMaxBatteryStrength )
        {
        iRechargeBatteryStrength = KMinBatteryStrength;        
        }
    iDrawer->SetBatteryStrength( iRechargeBatteryStrength );
    
    NotifyObserver();   
    
    PRINT( _L("Camera <= CCamBatteryPaneController::UpdateRechargeBatteryStrength") );	     
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::TickerCallback
// ---------------------------------------------------------------------------    
//
TInt CCamBatteryPaneController::TickerCallback( TAny* aThis )
    {    	
	CCamBatteryPaneController* self = static_cast<CCamBatteryPaneController*>( aThis );
	if( self )
		{
		self->UpdateRechargeBatteryStrength();		
		}    
    return ETrue;    
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::ReadCurrentState
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneController::ReadCurrentState()
    {
    PRINT( _L("Camera => CCamBatteryPaneController::ReadCurrentState") ); 	
    
    TInt batteryStrength = 0;
    TInt chargingStatus = 0;
    TInt err = KErrNone;

    // Disable callbacks to avoid two separate callbacks for charging
    // status and battery level
    TBool callbackActive = iCallbackActive;
    iCallbackActive = EFalse;
    
    // Get the battery strength    
    err = iBatteryStrengthWatcher->Get( batteryStrength );
    if( KErrNone == err && -1 != batteryStrength )
        {
        // We got an acceptable value
        SetBatteryStrength( batteryStrength );
        }
    else
        {
        // Default to lowest value
        SetBatteryStrength( KMinBatteryStrength );
        }    
    
    // Get current recharging state       
    err = iBatteryChargingWatcher->Get( chargingStatus );
    if ( KErrNone == err )
        {
        HandleChargingStatusChange( chargingStatus );
        }
    else
        {        
        StopRecharging();                
        }        
    
    // Restore callbacks and notify the observer
    iCallbackActive = callbackActive;
    NotifyObserver();                
    
    PRINT( _L("Camera <= CCamBatteryPaneController::ReadCurrentState") ); 	
    }
    

// ---------------------------------------------------------------------------
// CCamBatteryPaneController::HandleChargingStatusChange
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneController::HandleChargingStatusChange( TInt aStatus )
    {
    PRINT( _L("Camera => CCamBatteryPaneController::HandleChargingStatusChange") );	    	
    	
    if( EChargingStatusCharging == aStatus || 
        EChargingStatusAlmostComplete == aStatus ||
        EChargingStatusChargingContinued == aStatus )
        {
        // The battery is being recharged
        
        if( !iRecharging )
            {
            // Start the recharging animation
            StartRecharging();
            }
        }
    else
        {
        // The battery is currently not being recharged
        if( iRecharging )
            {
            // Stop the recharging animation
            StopRecharging();
            }        
        }    
        
    PRINT( _L("Camera <= CCamBatteryPaneController::HandleChargingStatusChange") );	        
    }
    
// ---------------------------------------------------------------------------
// CCamBatteryPaneController::HandlePropertyChangedL
// ---------------------------------------------------------------------------    
//
void CCamBatteryPaneController::HandlePropertyChangedL( const TUid& aCategory, const TUint aKey )
    {
    PRINT( _L("Camera => CCamBatteryPaneController::HandlePropertyChangedL") ); 	    	
    
    TInt value;
    if( KPSUidHWRMPowerState == aCategory && KHWRMBatteryLevel == aKey )
        {
        TInt err = iBatteryStrengthWatcher->Get( value );
        if( KErrNone == err && -1 != value )
            {
            SetBatteryStrength( value );
            }
        }
    else if( KPSUidHWRMPowerState == aCategory && KHWRMChargingStatus == aKey )
        {
        TInt err = iBatteryChargingWatcher->Get( value );
        if( KErrNone == err && -1 != value )
            {
            HandleChargingStatusChange( value );
            }
        }
    else
        {
        // Do nothing. For Lint.
        }

    PRINT( _L("Camera <= CCamBatteryPaneController::HandlePropertyChangedL") );
    }
    
// ---------------------------------------------------------------------------
// CCamBatteryPaneController::HandleResourceChange
// ---------------------------------------------------------------------------    
//   
void CCamBatteryPaneController::HandleResourceChange( TInt aType )
    {
    PRINT( _L("Camera => CCamBatteryPaneController::HandleResourceChange") );	
    
    if( KAknsMessageSkinChange == aType ||
        KEikColorResourceChange == aType )
        {
        iDrawer->HandleResourceChange( aType );
        }
        
		PRINT( _L("Camera <= CCamBatteryPaneController::HandleResourceChange") );	        
    }

// End of file

