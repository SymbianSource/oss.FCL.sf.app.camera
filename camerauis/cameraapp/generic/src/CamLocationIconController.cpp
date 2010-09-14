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
* Description:  Implemantation for CCamLocationIconController class.
*
*/


#include "CamLocationIconController.h"

#include <e32base.h>
#include <e32property.h>                // RProperty
#ifndef __WINSCW__
  #include <locationtrailpskeys.h>      // P&S keys
#endif
#include <AknsConstants.h>              // KAknsMessageSkinChange 
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <cameraapp.mbg>

#include "CamAppController.h"
#include "CamIndicator.h"
#include "CamIndicatorResourceReader.h"
#include "CamIndicatorData.h"

#include "CamPropertyWatcher.h"
#include "CamUtility.h"
#include "CamSettingsInternal.hrh"

#include "CamControllerObservers.h"

// ---------------------------------------------------------------------------
// CCamLocationIconController::CCamLocationIconController
// ---------------------------------------------------------------------------
//
CCamLocationIconController::CCamLocationIconController( CCamAppController& aController,
                                                        MCamLocationIconObserver& aObserver, 
                                                        TBool aCallbackActive ) :
    iController( aController ), iObserver( aObserver ), iCallbackActive( aCallbackActive )
    {
    }

// ---------------------------------------------------------------------------
// CCamLocationIconController::~CCamLocationIconController
// ---------------------------------------------------------------------------
//
CCamLocationIconController::~CCamLocationIconController()
  {
    PRINT ( _L("Camera => CCamLocationIconController::~CCamLocationIconController") );
     	
    iLocationIndicators.ResetAndDestroy();
    iLocationIndicators.Close();
    delete iLocationStateWatcher;

    iController.RemoveControllerObserver( this );
    
    PRINT ( _L("Camera <= CCamLocationIconController::~CCamLocationIconController") ); 	   
    }

// ---------------------------------------------------------------------------
// CCamLocationIconController::NewL
// ---------------------------------------------------------------------------
//
CCamLocationIconController* CCamLocationIconController::NewL(  CCamAppController& aController, 
                                                               MCamLocationIconObserver& aObserver, 
                                                               TBool aCallbackActive, TRect& aRect )
    {
    PRINT ( _L("Camera => CCamLocationIconController::NewL") );		
    
    CCamLocationIconController* self = 
        new (ELeave) CCamLocationIconController( aController, aObserver, aCallbackActive );
    CleanupStack::PushL( self );
    self->ConstructL(aRect);
    CleanupStack::Pop(); // self
    PRINT ( _L("Camera <= CCamLocationIconController::NewL") );
    return self;    
    }

// ---------------------------------------------------------------------------
// CCamLocationIconController::ConstructL
// ---------------------------------------------------------------------------
//
void CCamLocationIconController::ConstructL(TRect& aRect)
    {
    PRINT ( _L("Camera => CCamLocationIconController::ConstructL") );	
    iEnv = CEikonEnv::Static();
    iRect.SetRect(aRect.iTl, aRect.Size());
    //Change this later when the location active object has been finished!!
    iLocationState = KLocationSettingStateOff;
    CreateLocationIndicatorL();
    
#ifndef __WINSCW__
    // Construct property watchers for location indicator status
    iLocationStateWatcher = CCamPropertyWatcher::NewL( *this, KPSUidLocationTrail, KLocationTrailState );

    // Subscribe to the properties 
    iLocationStateWatcher->Subscribe();
#endif

    // Read initial values for location status
    ReadCurrentState();
    
    iController.AddControllerObserverL( this );
    
    PRINT ( _L( "Camera <= CCamLocationIconController::ConstructL" ) );	
    }
    
    
//----------------------------------------------------------
// CCamLocationIconController::CreateLocationIndicatorL
//----------------------------------------------------------
void CCamLocationIconController::CreateLocationIndicatorL()
    {
	RArray<TInt> icons;
	CleanupClosePushL( icons );
	User::LeaveIfError( icons.Append( EMbmCameraappQgn_indi_cam4_geotag_off ) );
	User::LeaveIfError( icons.Append( EMbmCameraappQgn_indi_cam4_geotag_off_mask ) );
	User::LeaveIfError( icons.Append( EMbmCameraappQgn_indi_cam4_geotag_on ) );
	User::LeaveIfError( icons.Append( EMbmCameraappQgn_indi_cam4_geotag_on_mask ) );
	TInt count = icons.Count();
    // Get the icon rect
    TRect iconRect = Rect();
		
    CCamIndicator* indicator;
	for( TInt i = 0; i < count; i+=2 )
		{
        indicator = CCamIndicator::NewL( iconRect );
        CleanupStack::PushL( indicator );
        indicator->AddIconL( icons[i], icons[i+1] ); // Bitmap & mask.
        indicator->SetRect( iconRect );
       
        User::LeaveIfError( iLocationIndicators.Append( indicator ) );
        CleanupStack::Pop( indicator );
		}

	CleanupStack::PopAndDestroy( &icons );

    }


// ---------------------------------------------------------------------------
// CCamLocationIconController::SetCallbackActive
// ---------------------------------------------------------------------------
//    
void CCamLocationIconController::SetCallbackActive( TBool aActive )
    {
    iCallbackActive = aActive;
    }


// ---------------------------------------------------------------------------
// CCamLocationIconController::UpdateRect
// ---------------------------------------------------------------------------
//    
void CCamLocationIconController::UpdateRect()
    {
    TInt i;
    TRect newRect = Rect();
    TInt count = iLocationIndicators.Count();
    
	const TCamCameraMode mode = iController.CurrentMode();
	const TCamCameraMode targetmode = iController.TargetMode();
	TBool ShowIcon = ETrue;
	if ( (targetmode != mode)
          || (( ECamControllerImage != targetmode )
           && ( ECamControllerVideo != targetmode )) ) 
	    {
	    ShowIcon = EFalse;
		}
		
    for ( i = 0; i < count; i++ )
        {
      	
        if ( ShowIcon ) 
            {
            iLocationIndicators[i]->DisplayIcon();
            } 
        else 
            {
            iLocationIndicators[i]->ClearIcon();
            }
        iLocationIndicators[i]->SetRect( newRect );
        }
    NotifyObserver();
    }

// ---------------------------------------------------------------------------
// CCamLocationIconController::SetLocationState
// ---------------------------------------------------------------------------
//
void CCamLocationIconController::SetLocationState( TInt aState )
    {    	
    PRINT( _L("Camera => CCamLocationIconController::SetLocationState") );	    	

    // Possible location trail states are 0,1,2 and 3
    // but the GPS data is available only with the state 3
    // so that is when the icon is changed to on state.
    // Possible values
    // - TrailStopped (value 0 - off-icon)
    // - TrailStarted - No valid GPS data (value 1 - off-icon)
    // - TrailStarted - Searching GPS device (value 2 - off-icon)
    // - TrailStarted - GPS data available (value 3 - on-icon)
    if ( KCamLocationTrailGPSDataAvail == aState ) 
        {
    	iLocationState = KLocationSettingStateOn;
        }
    else
        {
    	iLocationState = KLocationSettingStateOff;
        }

    NotifyObserver();
        
    PRINT( _L("Camera <= CCamLocationIconController::SetLocationState") );	        
    }    
 
// ---------------------------------------------------------------------------
// CCamLocationIconController::Rect
// Returns the rectangle defining the position and size
// of the location icon
// ----------------------------------------------------
//
TRect CCamLocationIconController::Rect() const
	{
    TAknLayoutRect camIndicatorPane;
	TAknLayoutRect layoutRect;
	
    TSize screenSize;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, screenSize );
    TRect extent = TRect( TPoint(), screenSize );
	
	const TCamCameraMode targetmode = iController.TargetMode();
	const TCamCaptureOperation operation =  iController.CurrentVideoOperation();
	if ( ECamControllerVideo == targetmode )
		{
		if ( ( ECamCapturing == operation )||
		     ( ECamPausing == operation )  ||
			 ( ECamPaused == operation )   ||
			 ( ECamResuming == operation ))
			{
	        if ( AknLayoutUtils::PenEnabled() ) 
	            {
                if ( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    camIndicatorPane.LayoutRect(
                        extent,
                        AknLayoutScalable_Apps::vid4_progress_pane( 1 ) );
                    layoutRect.LayoutRect(
                        camIndicatorPane.Rect(),
                        AknLayoutScalable_Apps::vid4_progress_pane_g5( 0 ) );
                    }
                else  // portrait
                    {
                    camIndicatorPane.LayoutRect(
                        extent,
                        AknLayoutScalable_Apps::vid4_progress_pane( 0 ) );
                    layoutRect.LayoutRect(
                        camIndicatorPane.Rect(),
                        AknLayoutScalable_Apps::vid4_progress_pane_g5( 1 ) );
                    }
	            }
	        else
	            {
	            if ( Layout_Meta_Data::IsLandscapeOrientation() )
	                {
	                camIndicatorPane.LayoutRect(
	                    extent,
	                    AknLayoutScalable_Apps::vid6_indi_pane( 0 ) );
	                layoutRect.LayoutRect(
	                    camIndicatorPane.Rect(),
	                    AknLayoutScalable_Apps::vid6_indi_pane_g4( 1 ) );
	                }
	            else  // portrait
	                {
	                camIndicatorPane.LayoutRect(
	                    extent,
	                    AknLayoutScalable_Apps::vid6_indi_pane( 3 ) );
	                layoutRect.LayoutRect(
	                    camIndicatorPane.Rect(),
	                    AknLayoutScalable_Apps::vid6_indi_pane_g4( 3 ) );
	                }
	            }
			}
		else
			{
            if ( AknLayoutUtils::PenEnabled() ) 
                {
                if ( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    camIndicatorPane.LayoutRect( extent,
                             AknLayoutScalable_Apps::vid4_progress_pane( 1 ) ); // vid4_indicators_pane should be used
                    layoutRect.LayoutRect( camIndicatorPane.Rect(),
                        AknLayoutScalable_Apps::vid4_indicators_pane_g5( 0 ) );       
                    }
                else  // portrait
                    {
                    camIndicatorPane.LayoutRect(
                        extent,
                        AknLayoutScalable_Apps::vid4_indicators_pane( 0 ) );
                    layoutRect.LayoutRect(
                        camIndicatorPane.Rect(),
                        AknLayoutScalable_Apps::vid4_indicators_pane_g5( 1 ) );
                    }
                }
            else
                {
                if ( Layout_Meta_Data::IsLandscapeOrientation() )
                    {
                    camIndicatorPane.LayoutRect(
                        extent,
                        AknLayoutScalable_Apps::vid6_indi_pane( 0 ) );
                    layoutRect.LayoutRect(
                        camIndicatorPane.Rect(),
                        AknLayoutScalable_Apps::vid6_indi_pane_g4( 0 ) );
                    }
                else  // portrait
                    {
                    camIndicatorPane.LayoutRect(
                        extent,
                        AknLayoutScalable_Apps::vid6_indi_pane( 3 ) );
                    layoutRect.LayoutRect(
                        camIndicatorPane.Rect(),
                        AknLayoutScalable_Apps::vid6_indi_pane_g4( 2 ) );
                    }
                }
			}
		}
	else
		{
        if ( AknLayoutUtils::PenEnabled() ) 
            {
    	    if ( Layout_Meta_Data::IsLandscapeOrientation() )
    	        {
    	        camIndicatorPane.LayoutRect(
    	            extent,
    	            AknLayoutScalable_Apps::cam4_indicators_pane( 1 ) );
    	        layoutRect.LayoutRect(
    	            camIndicatorPane.Rect(),
    	            AknLayoutScalable_Apps::cam4_indicators_pane_g3( 0 ) );
    	        }
    	    else  // portrait
    	        {
    	        camIndicatorPane.LayoutRect(
    	            extent,
    	            AknLayoutScalable_Apps::cam4_indicators_pane( 0 ) );
    	        layoutRect.LayoutRect(
    	            camIndicatorPane.Rect(),
    	            AknLayoutScalable_Apps::cam4_indicators_pane_g3( 1 ) );
    	        }
            }
        else
            {
            if ( Layout_Meta_Data::IsLandscapeOrientation() )
                {
                camIndicatorPane.LayoutRect(
                    extent,
                    AknLayoutScalable_Apps::cam6_indi_pane( 0 ) );
                layoutRect.LayoutRect(
                    camIndicatorPane.Rect(),
                    AknLayoutScalable_Apps::cam6_indi_pane_g3( 0 ) );
                }
            else  // portrait
                {
                camIndicatorPane.LayoutRect(
                    extent,
                    AknLayoutScalable_Apps::cam6_indi_pane( 3 ) );
                layoutRect.LayoutRect(
                    camIndicatorPane.Rect(),
                    AknLayoutScalable_Apps::cam6_indi_pane_g3( 2 ) );
                }
            }
		}
    return layoutRect.Rect();
	}

// ---------------------------------------------------------------------------
// CCamLocationIconController::Draw
// ---------------------------------------------------------------------------
//
void CCamLocationIconController::Draw( CBitmapContext& aGc ) const
    {
    PRINT( _L("Camera => CCamLocationIconController::Draw") );

    // Make sure no brush is being used
    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );    
    iLocationIndicators[iLocationState]->Draw( aGc );

    PRINT( _L("Camera <= CCamLocationIconController::Draw") );
    }    


// ---------------------------------------------------------------------------
// CCamLocationIconController::NotifyObserver
// ---------------------------------------------------------------------------    
//
void CCamLocationIconController::NotifyObserver()
    {
    if( iCallbackActive )
        {
        iObserver.LocationIconUpdated();
        }
    }

// ---------------------------------------------------------------------------
// CCamLocationIconController::ReadCurrentState
// ---------------------------------------------------------------------------
//
void CCamLocationIconController::ReadCurrentState()
    {
    PRINT( _L("Camera => CCamLocationIconController::ReadCurrentState") ); 	
    
    TInt locationState = KLocationSettingStateOff;
#ifndef __WINSCW__
    TInt err = KErrNone;

    // Get the location state
    err = iLocationStateWatcher->Get( locationState );
    if( KErrNone == err && -1 != locationState && KCamLocationTrailStopped <= locationState && KCamLocationTrailGPSDataAvail >= locationState )
        {
        // We got an acceptable value
        SetLocationState( locationState );
        }
    else
        {
        // Default to off value
        SetLocationState( KLocationSettingStateOff );
        }
#endif // __WINSCW__

    NotifyObserver();
    
    PRINT( _L("Camera <= CCamLocationIconController::ReadCurrentState") ); 	
    }
    
// ---------------------------------------------------------------------------
// CCamLocationIconController::HandlePropertyChangedL
// ---------------------------------------------------------------------------    
//
#ifndef __WINSCW__    
void CCamLocationIconController::HandlePropertyChangedL( const TUid& aCategory, const TUint aKey )
    {
    PRINT( _L("Camera => CCamLocationIconController::HandlePropertyChangedL") ); 	    	

    TInt value = KLocationSettingStateOff;
    if( KPSUidLocationTrail == aCategory && KLocationTrailState == aKey )
        {
        TInt err = iLocationStateWatcher->Get( value );
        if( KErrNone == err && -1 != value && KCamLocationTrailStopped <= value && KCamLocationTrailGPSDataAvail >= value )
            {
            SetLocationState( value );
            }
        }
    else
        {
        // Do nothing. For Lint.
        }

    PRINT( _L("Camera <= CCamLocationIconController::HandlePropertyChangedL") );
    }
#else
void CCamLocationIconController::HandlePropertyChangedL( const TUid& /*aCategory*/, const TUint /*aKey*/ )
  {
  // Empty
  }
#endif // __WINSCW__


// ---------------------------------------------------------------------------    
// HandleControllerEventL
//
// Update the rect for any controller event
// ---------------------------------------------------------------------------    
//
void 
CCamLocationIconController::HandleControllerEventL( TCamControllerEvent aEvent,
                                                    TInt                /*aError*/ )
	{
	PRINT( _L("Camera => CCamLocationIconController::HandleControllerEventL") );
	if ( ECamEventExitRequested != aEvent )
        {
        TBool locIconVisible( EFalse );
        locIconVisible = 
            iController.IntegerSettingValue( ECamSettingItemRecLocation ) == ECamLocationOn;

        if ( locIconVisible ) 
    	    {
    	    PRINT( _L("Camera => UpdateRect()") );
	        UpdateRect();
	        PRINT( _L("Camera <= UpdateRect()") );
    	    }
    	}
    PRINT( _L("Camera <= CCamLocationIconController::HandleControllerEventL") );
	}

