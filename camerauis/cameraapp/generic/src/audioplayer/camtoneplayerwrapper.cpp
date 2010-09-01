/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Tone Wrapper implementation
*
*/



#include <mdaaudiotoneplayer.h>
#include <AudioPreference.h>

#include "CamPanic.h"
#include "camlogging.h"
#include "mcamplayerobserver.h"
#include "camtoneplayerwrapper.h"




// ===========================================================================  
// public constructors and destructor

// ---------------------------------------------------------------------------
// NewL <<static>>
//
// ---------------------------------------------------------------------------
//
CCamTonePlayerWrapper* 
CCamTonePlayerWrapper::NewL( MCamPlayerObserver& aObserver )
  {
  CCamTonePlayerWrapper* self = 
    new (ELeave) CCamTonePlayerWrapper( aObserver );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;
  }

// ---------------------------------------------------------------------------
// destructor <<virtual>>
//
// ---------------------------------------------------------------------------
//
CCamTonePlayerWrapper::~CCamTonePlayerWrapper()
  {    
  CancelPlay();

  delete iTone;
  iTone = NULL;
  }


// ===========================================================================  
// from MMdaAudioToneObserver
  
// ---------------------------------------------------------------------------
// MatoPrepareComplete <<virtual>>
//
// ---------------------------------------------------------------------------
//
void 
CCamTonePlayerWrapper::MatoPrepareComplete( TInt aStatus )
  {
  PRINT( _L("Camera => CCamTonePlayerWrapper::MatoPrepareComplete") );
  if( KErrNone == aStatus )
    {
    iState = CCamTonePlayerWrapper::EPlaying;
    iTone->Play();
#ifdef __WINSCW__
    // The tone player does not give a callback on the emulator.
    // Emulate a callback here.
    MatoPlayComplete( KErrNone );
#endif // __WINSCW__
    }
  else if( iCallback )
    {
    NotifyPlayComplete( aStatus );
    }
  else
    {
    // No action
    }

  PRINT( _L("Camera <= CCamTonePlayerWrapper::MatoPrepareComplete") );
  }


// ---------------------------------------------------------------------------
// MatoPlayComplete <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamTonePlayerWrapper::MatoPlayComplete( TInt aStatus )
  {
  NotifyPlayComplete( aStatus );
  }



// ===========================================================================  
// other public

// ---------------------------------------------------------------------------
// PlayTone
// ---------------------------------------------------------------------------
//
void 
CCamTonePlayerWrapper::PlayTone( TInt    aToneInHz, 
                                 TInt    aLengthInUs, 
                                 TReal32 aVolumeInPercent,
                                 TBool   aCallback )
  {
  PRINT2( _L("Camera => CCamTonePlayerWrapper::PlayTone, callback:%d iState[%s]"), 
          aCallback, 
          KCamTonePlayerWrapperStateNames[iState] );

  iCallback = aCallback;

  switch( iState )
    {
    // ---------------------------------
    case CCamTonePlayerWrapper::EIdle:
        {
        iState = CCamTonePlayerWrapper::EInitializing;
        iTone->SetVolume( aVolumeInPercent * iTone->MaxVolume() ); 
        iTone->PrepareToPlayTone( aToneInHz, aLengthInUs );

        break;
        }
    // ---------------------------------
    case CCamTonePlayerWrapper::EInitializing:
    case CCamTonePlayerWrapper::EPlaying:
        {
        // Do notification, if asked.
        // This play request is simply skipped,
        // as there's tone playing on the way right now.
        if( aCallback ) 
          {
          iObserver.PlayComplete( KErrInUse, -1 );   //iState-1 is another suggestion.
          }  
        break;
        }
    default:
      CamPanic( ECamPanicInvalidState );
      break;
    // ---------------------------------
    }

  PRINT( _L("Camera <= CCamAudioPlayerWrapper::Play") );
  }


// ---------------------------------------------------------------------------
// CancelPlay
// ---------------------------------------------------------------------------
//
void 
CCamTonePlayerWrapper::CancelPlay()
  {
  if( iTone )
    {
    switch( iState )
      {
      case CCamTonePlayerWrapper::EPlaying:
        iTone->CancelPlay();
        // Because of the observer callback function MMdaAudioToneObserver::MatoPrepareComplete() is not called,
        // so set state to idle directly
        iState = CCamTonePlayerWrapper::EIdle;
        break;
      case CCamTonePlayerWrapper::EInitializing:
        iTone->CancelPrepare();
        // Because of the observer callback function MMdaAudioToneObserver::MatoPrepareComplete() is not called,
        // so set state to idle directly
        iState = CCamTonePlayerWrapper::EIdle;
        break;
      case CCamTonePlayerWrapper::EIdle:
      default: 
        break;
      }
    }
  }



// ===========================================================================  
// other private

// ---------------------------------------------------------------------------
// NotifyPlayComplete
// ---------------------------------------------------------------------------
//
void
CCamTonePlayerWrapper::NotifyPlayComplete( TInt aStatus )
  {
  PRINT( _L("Camera => CCamTonePlayerWrapper::NotifyPlayComplete") );

  // Play is complete, return to idle state.
  iState = CCamTonePlayerWrapper::EIdle;

  // Notify observer received status.
  // Sound id is not usable here.
  if( iCallback )
    iObserver.PlayComplete( aStatus, -1 );

  PRINT( _L("Camera <= CCamTonePlayerWrapper::NotifyPlayComplete") );
  }



// ===========================================================================  
// private constructors

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void 
CCamTonePlayerWrapper::ConstructL()
  {
  iTone = CMdaAudioToneUtility::NewL( 
            *this, 
            NULL, 
            KAudioPriorityCameraTone,
            TMdaPriorityPreference( KAudioPrefCamera ) );
  }


// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CCamTonePlayerWrapper
::CCamTonePlayerWrapper( MCamPlayerObserver& aObserver )
  : iObserver     ( aObserver                     ),
    iTone         ( NULL                          ),
    iState        ( CCamTonePlayerWrapper::EIdle  ),
    iCallback     ( EFalse                        )
  {
  }

// ===========================================================================  
// end of file
