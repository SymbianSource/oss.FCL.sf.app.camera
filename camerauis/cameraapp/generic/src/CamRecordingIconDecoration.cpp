/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Camera UI recording icon decoration*
*/



#include <AknIconUtils.h>
#include <fbs.h>

#include <cameraapp.mbg>

#include "CamRecordingIconDecoration.h"
#include "CamObserverHandler.h"
#include "Cam.hrh"
#include "CamUtility.h"
#include "CamAppController.h"
#include "CamPanic.h"

const TInt KCamVidPauseFlashInterval = 800000;

// -----------------------------------------------------------------------------
// CCamRecordingIconDecoration::CCamRecordingIconDecoration()
// Default constructor
// -----------------------------------------------------------------------------
//
CCamRecordingIconDecoration
::CCamRecordingIconDecoration( CCamAppController& aController )
  : iController( aController )
  {
  }

// -----------------------------------------------------------------------------
// CCamRecordingIconDecoration::NewL
// Factory method to safely create a new instance
// -----------------------------------------------------------------------------
//
CCamRecordingIconDecoration* 
CCamRecordingIconDecoration::NewL( const TRect& aPosition, 
                                   CCamAppController& aController)
  {
  CCamRecordingIconDecoration* me = 
      new (ELeave) CCamRecordingIconDecoration( aController );

  CleanupStack::PushL( me );
  me->ConstructL( aPosition );
  CleanupStack::Pop( me );

  return me;
  }

// -----------------------------------------------------------------------------
// CCamRecordingIconDecoration::ConstructL
// Initializes a new instance
// -----------------------------------------------------------------------------
//
void 
CCamRecordingIconDecoration::ConstructL( const TRect& aPosition )
  {
  iPosition = aPosition.iTl;
  iSize = TRect( TPoint(0,0), aPosition.Size() );
  
  iObserverHandler = CCamObserverHandler::NewL();
  
  iPauseFlashPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );

  // Register as a controller observer    
  iController.AddControllerObserverL(this);
  
  // Load in the icons
  TFileName resFileName;
  CamUtility::ResourceFileName( resFileName );
  TPtrC resname = resFileName;
  
  AknIconUtils::CreateIconL( iRecBitmap, 
                             iRecBitmapMask, 
                             resname, 
                             EMbmCameraappQgn_indi_vid4_record, 
                             EMbmCameraappQgn_indi_vid4_record_mask );
  AknIconUtils::SetSize( iRecBitmap, aPosition.Size() );

  AknIconUtils::CreateIconL( iRecPauseBitmap, 
                             iRecPauseBitmapMask, 
                             resname, 
                             EMbmCameraappQgn_indi_vid4_pause, 
                             EMbmCameraappQgn_indi_vid4_pause_mask );
  AknIconUtils::SetSize( iRecPauseBitmap, aPosition.Size() );
  
  // Set iRecordState to the correct value
  switch( iController.CurrentVideoOperation() )
    {
    case ECamCapturing:
        iRecordState = ECamRecording;
        break;
    case ECamPaused:
        iRecordState = ECamRecordPaused;
        break;
    default:
        iRecordState = ECamNotRecording;
        break;
    }
  }

// -----------------------------------------------------------------------------
// CCamRecordingIconDecoration::~CCamRecordingIconDecoration()
// Destructor
// -----------------------------------------------------------------------------
//
CCamRecordingIconDecoration::~CCamRecordingIconDecoration()
  {
  PRINT( _L("Camera => ~CCamRecordingIconDecoration") );
  iController.RemoveControllerObserver(this);
  
  delete iObserverHandler;
  
  if( iPauseFlashPeriodic )
    {
    iPauseFlashPeriodic->Cancel();
    }
  delete iPauseFlashPeriodic;
  
  delete iRecBitmap;
  delete iRecBitmapMask;
  delete iRecPauseBitmap;
  delete iRecPauseBitmapMask;
  PRINT( _L("Camera <= ~CCamRecordingIconDecoration") );
  }
    
// -----------------------------------------------------------------------------
// CCamRecordingIconDecoration::Draw()
// Draw the decoration
// -----------------------------------------------------------------------------
//
void CCamRecordingIconDecoration::Draw(CBitmapContext& aGc, const TRect& /*aDrawRect*/)
    {
    if ( iRecordState == ECamRecording )
        {
		aGc.BitBltMasked(iPosition, iRecBitmap, iSize, iRecBitmapMask, ETrue);
        }
    else if ( iRecordState == ECamRecordPaused )
        {
        if( iShowPaused )
            {
    		aGc.BitBltMasked(iPosition, iRecPauseBitmap, iSize, iRecPauseBitmapMask, ETrue);
            }
        }
    else
        {
        }
    }

// ---------------------------------------------------------
// CCamRecordingIconDecoration::HandleControllerEventL
// Listens for controller events
// ---------------------------------------------------------
//
void 
CCamRecordingIconDecoration
::HandleControllerEventL( TCamControllerEvent aEvent, TInt /*aError*/ )
  {
  if( aEvent == ECamEventOperationStateChanged )
    {
    TCamCaptureOperation operation = iController.CurrentOperation();
    
    // Update based on new controller state
    if( ECamCapturing == operation )
      {
      ChangeRecordState(ECamRecording);
      }
    else if( ECamPaused == operation )
      {
      ChangeRecordState(ECamRecordPaused);
      iShowPaused = ETrue;

      // Kick off the pause flash timer
      if( !iPauseFlashPeriodic->IsActive() )
        {
        TCallBack cb( PauseFlashCallBack, this );
        iPauseFlashPeriodic->Start( KCamVidPauseFlashInterval,
                                    KCamVidPauseFlashInterval,
                                    cb );
        }
      }
    else
      {
      ChangeRecordState( ECamNotRecording );
      }
  
    // Cancel the timer if need be
    if( iRecordState != ECamRecordPaused )         
      {
      if( iPauseFlashPeriodic->IsActive() )
        {
        iPauseFlashPeriodic->Cancel();
        }
      }
    }
  }

// ---------------------------------------------------------
// CCamRecordingIconDecoration::ChangeRecordState
// Changes iRecordState, tells observers to update
// ---------------------------------------------------------
//
void CCamRecordingIconDecoration::ChangeRecordState(TCamRecordState aNewState)
    {
    if ( iRecordState != aNewState )
        {
        BroadcastEvent(ECamObserverEventRecordingIconUpdated);
        }
    iRecordState = aNewState;
    }
    
// ---------------------------------------------------------
// CCamRecordingIconDecoration::PauseFlashCallBack
// Callback function from periodic timer used to flash paused icon every 0.8 secs
// ---------------------------------------------------------
//
TInt CCamRecordingIconDecoration::PauseFlashCallBack( TAny* aRecordingIconDecoration )
    {
    __ASSERT_DEBUG( aRecordingIconDecoration, CamPanic( ECamPanicNullPointer ) );
    static_cast <CCamRecordingIconDecoration*> ( aRecordingIconDecoration )->
                                                    TogglePauseIconFlash();
    return KErrNone;
    }

// ---------------------------------------------------------
// CCamRecordingIconDecoration::TogglePauseIconFlash
// Updates paused icon state in response to periodic timer 
// ---------------------------------------------------------
//
void CCamRecordingIconDecoration::TogglePauseIconFlash()
    {
    PRINT(_L("CCamRecordingIconDecoration::TogglePauseIconFlash: broadcasting ECamObserverEventRecordingIconUpdated"));
    iShowPaused = !iShowPaused;
    BroadcastEvent(ECamObserverEventRecordingIconUpdated);
    }

// ---------------------------------------------------------
// CCamRecordingIconDecoration::RegisterObserverL
// Registers an observer
// ---------------------------------------------------------
//
void CCamRecordingIconDecoration::RegisterObserverL(MCamObserver* aObserver)
    {
    iObserverHandler->RegisterObserverL(aObserver);
    }

// ---------------------------------------------------------
// CCamRecordingIconDecoration::DeregisterObserver
// Deregisters an observer
// ---------------------------------------------------------
//
void CCamRecordingIconDecoration::DeregisterObserver(MCamObserver* aObserver)
    {
    iObserverHandler->DeregisterObserver(aObserver);
    }

// ---------------------------------------------------------
// CCamRecordingIconDecoration::BroadcastEvent
// Broadcasts an event code to all observers
// ---------------------------------------------------------
//
void CCamRecordingIconDecoration::BroadcastEvent(TCamObserverEvent aEvent)
    {
    iObserverHandler->BroadcastEvent(aEvent);
    }

