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
* Description:  Implemantation for CCamFlashStatus class.
*
*/


// #include "CamPropertyWatcher.h"
// #include "CamUtility.h"
#include "camlogging.h"
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "camcameraevents.h"
#include "CamAppController.h"
#include "camflashstatus.h"

static const TUint KCameraEventInterest = ECamCameraEventClassFlashStatus;

// ---------------------------------------------------------------------------
// CCamFlashStatus::CCamFlashStatus
// ---------------------------------------------------------------------------
//
CCamFlashStatus::CCamFlashStatus( CCamAppController& aController ):
    iErrorCallbacksEnabled( ETrue ),
    iController( aController )
    {    
    }

// ---------------------------------------------------------------------------
// CCamFlashStatus::~CCamFlashStatus
// ---------------------------------------------------------------------------
//    
CCamFlashStatus::~CCamFlashStatus()
    {  
    iController.RemoveCameraObserver( this );

    if( iBlinkTimer )
        {
        iBlinkTimer->Cancel();
        delete iBlinkTimer;
        }
    }

// ---------------------------------------------------------------------------
// CCamFlashStatus::NewL
// ---------------------------------------------------------------------------
//
CCamFlashStatus*
CCamFlashStatus::NewL( CCamAppController& aController )
    {
    CCamFlashStatus* self = 
        new (ELeave) CCamFlashStatus( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;        
    }


// ---------------------------------------------------------------------------
// CCamFlashStatus::ConstructL
// ---------------------------------------------------------------------------
//
void    
CCamFlashStatus::ConstructL()
    {
    iBlinkTimer = CPeriodic::NewL( EPriorityLow );

    iController.AddCameraObserverL( this, KCameraEventInterest );
    iFlashError    = EFalse;
    iFlashRequired = EFalse;
    iFlashReady    = EFalse;
    }
       
// ---------------------------------------------------------------------------
// CCamFlashStatus::Subscribe
// ---------------------------------------------------------------------------
//    
void
CCamFlashStatus::Subscribe( MFlashStatusObserver* aObserver )
    {
    PRINT1( _L("Camera => CCamFlashStatus::Subscribe, observer ptr:%d"), aObserver );
    iObserver = aObserver;
    
    // If there is need to blink, but no observer before,
    // we need to recheck the blinking status when observer subscribes.
    CheckBlinkingStatus();

    PRINT ( _L("Camera <= CCamFlashStatus::Subscribe") );
    }
    
    
// ---------------------------------------------------------------------------
// CCamFlashStatus::FlashReady
// ---------------------------------------------------------------------------
//    
TBool
CCamFlashStatus::FlashReady() const
    {
    return iFlashReady;
    }
    
// ---------------------------------------------------------------------------
// CCamFlashStatus::FlashError
// ---------------------------------------------------------------------------
//    
TBool
CCamFlashStatus::FlashError() const
    {
    return iFlashError;
    }
    
// ---------------------------------------------------------------------------
// CCamFlashStatus::SetFlashError
// ---------------------------------------------------------------------------
//     
void
CCamFlashStatus::SetFlashError( TBool aFlashError )
    {
    iFlashError = aFlashError;
    }    
    
// ---------------------------------------------------------------------------
// CCamFlashStatus::ErrorCallbacksEnabled
// ---------------------------------------------------------------------------
//    
TBool
CCamFlashStatus::ErrorCallbacksEnabled() const
    {
    return iErrorCallbacksEnabled;
    }
    
// ---------------------------------------------------------------------------
// CCamFlashStatus::EnableErrorCallback
// ---------------------------------------------------------------------------
//    
void
CCamFlashStatus::EnableErrorCallbacks( TBool aCallbacksEnabled )
    {
    iErrorCallbacksEnabled = aCallbacksEnabled;
    }

// ---------------------------------------------------------------------------
// CCamFlashStatus::FlashIconVisible
// ---------------------------------------------------------------------------
//    
TBool
CCamFlashStatus::FlashIconVisible() const
    {
    return iBlinkStatus;
    }


// ---------------------------------------------------------------------------
// HandleCameraEventL <<virtual>>
//
// ---------------------------------------------------------------------------
//
void 
CCamFlashStatus::HandleCameraEventL( TInt              aStatus, 
                                     TCamCameraEventId aEventId, 
                                     TAny*             aEventData /*= NULL*/ )
  {
  PRINT( _L("Camera => CCamFlashStatus::HandleCameraEventL") );

  switch( aEventId )
    {
    // ---------------------------------------------------
    // Flash ready.
    case ECamCameraEventFlashReady:
      {
      PRINT ( _L("Camera <> CCamFlashStatus: Flash ready") );
      iFlashError = aStatus;
      iFlashReady = (KErrNone == aStatus);
      break;
      }
    // ---------------------------------------------------
    // Flash not ready, e.g. recharging.
    case ECamCameraEventFlashNotReady:
      {
      PRINT ( _L("Camera <> CCamFlashStatus: Flash NOT ready") );
      iFlashError = aStatus;
      iFlashReady = EFalse;
      break;
      }
    // ---------------------------------------------------
    default:
      {
      PRINT ( _L("Camera <> CCamFlashStatus: unhandled event!") );
      break;
      }
    // ---------------------------------------------------
    }

  // Check if need to start/stop blinking      
  CheckBlinkingStatus();
  
  // Notify error
  if( iFlashError )
    {
    NotifyObserverAboutFlashError();
    }

  PRINT( _L("Camera <= CCamFlashStatus::HandleCameraEventL") );
  }
  

// ---------------------------------------------------------------------------
// CheckBlinkingStatus
//
// ---------------------------------------------------------------------------
//
void
CCamFlashStatus::CheckBlinkingStatus()
  {
  // If blinking is on, check if it needs to be stopped
  if( iBlinking )
    {
    // No observer OR flash ready OR flash error OR flash off 
    // => stop blinking
    if( !iObserver 
     ||  iFlashReady
     ||  iFlashError
     ||  IsFlashOff() )
      {
      StopBlinking();
      }
    }
  // If blinking is off, check if it needs to be started.
  else
    {
    // Observer joined AND flash not ready AND no error AND flash used 
    // => start blinking
    if( iObserver && !iFlashReady && !iFlashError && !IsFlashOff() )
      {
      StartBlinking();
      }
    }
  }



// ---------------------------------------------------------------------------
// CCamFlashStatus::StartBlinking
// ---------------------------------------------------------------------------
//   
void
CCamFlashStatus::StartBlinking()
  {
  // Start blinking only if some observer is displaying the status
  if( iObserver )
    {
    PRINT( _L("Camera =><= CamFlashStatus::StartBlinking - starting blinking") )
    iBlinking    = ETrue;
    iBlinkStatus = EFalse;
    // Make first callback to make the flash icon visible
    BlinkTimerCallback( this );     

    // Start the blinking timer    
    iBlinkTimer->Cancel();
    iBlinkTimer->Start( KFlashIconBlinkDelay,
                        KFlashIconBlinkDelay,
                        TCallBack( BlinkTimerCallback, this) );                                                            
      
    }
  }

// ---------------------------------------------------------------------------
// CCamFlashStatus::StopBlinking
// ---------------------------------------------------------------------------
//
void 
CCamFlashStatus::StopBlinking()
  {
  PRINT( _L("Camera =><= CamFlashStatus::StopBlinking - stopping blinking") )
  
  // Stop blinking
  iBlinking = EFalse;
  iBlinkTimer->Cancel();
                                                                          
  if( iBlinkStatus )
    {
    // The flash icon is visible. Make one more callback to
    // hide it.
    BlinkTimerCallback( this );    
    }
  }
   

// ---------------------------------------------------------------------------
// CCamFlashStatus::BlinkTimerCallback
// ---------------------------------------------------------------------------
//   
TInt
CCamFlashStatus::BlinkTimerCallback( TAny* aSelf )
  {
  PRINT( _L("Camera => CamFlashStatus::BlinkTimerCallback") )
  
  CCamFlashStatus* self = static_cast<CCamFlashStatus*>( aSelf );
  if( self )
    {
    // Invert icon visibility
    TBool newBlinkStatus = self->iBlinkStatus ? EFalse : ETrue;
    self->iBlinkStatus = newBlinkStatus;
    
    // Inform observer of change
    if( self->iObserver )
      {
      self->iObserver->FlashIconVisible( newBlinkStatus );
      }
    }    
   
  PRINT( _L("Camera <= CamFlashStatus::BlinkTimerCallback") )
  return KErrNone;     
  }


// ---------------------------------------------------------------------------
// FlashOff
// ---------------------------------------------------------------------------
//
TBool 
CCamFlashStatus::IsFlashOff() const
  {
  TInt           key  = KErrNotFound;
  TCamCameraMode mode = iController.CurrentMode();

  switch( mode )
    {
    case ECamControllerImage: key = ECamSettingItemDynamicPhotoFlash; break;
    case ECamControllerVideo: key = ECamSettingItemDynamicVideoFlash; break;
    default:
      break;
    }

  TBool off = ( KErrNotFound == key // Not prepared or standby mode.
             || ECamFlashOff == iController.IntegerSettingValue( key )
              );

  return off;
  }

// ---------------------------------------------------------------------------
// CCamFlashStatus::NotifyObserverAboutFlashError
// ---------------------------------------------------------------------------
//    
void
CCamFlashStatus::NotifyObserverAboutFlashError()
  {
  if( iErrorCallbacksEnabled && iObserver )
    {
    // Give error callback to the observer. 
    iObserver->FlashError();
    }    
  }
    

// End of file

