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
* Description:  Implementation of Wrapper for Audio Player Utility
*
*/


// ===========================================================================  
// includes
#include <mdaaudiosampleplayer.h>
#include <AudioPreference.h>

#include "Cam.hrh"
#include "CamPanic.h"
#include "camlogging.h"
#include "CamUtility.h" // for resource id's of sounds
#include "mcamplayerobserver.h"
#include "camaudioplayerwrapper.h"
#include "CamPerformance.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "camaudioplayerwrapperTraces.h"
#endif



// ===========================================================================  
// Constants
const TReal32 KCamSoundVol = 0.75f; 
// Migrated directly from old sound player.
#ifndef __WINSCW__
  static const TUint KAudioPriority = KAudioPriorityCameraTone;
#else
  static const TUint KAudioPriority = KAudioPriorityVideoRecording;
#endif // __WINSCW__                                                                                              


// ===========================================================================  
// public constructors and destructor

// ---------------------------------------------------------------------------
// NewL <<static>>
// ---------------------------------------------------------------------------
//
CCamAudioPlayerWrapper* 
CCamAudioPlayerWrapper::NewL( MCamPlayerObserver& aObserver, 
                              TInt                aSoundId )
  {
  PRINT( _L("Camera => CCamAudioPlayerWrapper::NewL()") );
  CCamAudioPlayerWrapper* self = 
    new (ELeave) CCamAudioPlayerWrapper( aObserver, aSoundId );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  PRINT( _L("Camera <= CCamAudioPlayerWrapper::NewL()") );
  return self;
  }

// ---------------------------------------------------------------------------
// destructor <<virtual>>
// ---------------------------------------------------------------------------
//
CCamAudioPlayerWrapper::~CCamAudioPlayerWrapper()
  {
  if( iPlayer )
    {
    iPlayer->Stop();
    iPlayer->Close();
    delete iPlayer;
    iPlayer = NULL;
    }
  }



// ===========================================================================  
// from CCamPlayerWrapperBase

// ---------------------------------------------------------------------------
// Id <<virtual>><<const>>
// ---------------------------------------------------------------------------
//
TInt 
CCamAudioPlayerWrapper::Id() const
  {
  return iSoundId;
  }

// ---------------------------------------------------------------------------
// IsEqualSound <<virtual>><<const>>
//
// Compare if two sounds are equal. 
// If the ids or filenames match, the sounds are considered to be equal.
// ---------------------------------------------------------------------------
//
TBool
CCamAudioPlayerWrapper::IsEqualSound( TInt aSoundId ) const
  {
  TInt eq( EFalse );
  TRAP_IGNORE( 
    {
    eq = aSoundId == iSoundId
      || MapSoundId2FilenameL( aSoundId ) == MapSoundId2FilenameL( iSoundId );
    });
  return eq;
  }


// ---------------------------------------------------------------------------
// Play <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerWrapper::Play( TBool aCallback )
  {
  PRINT2( _L("Camera => CCamAudioPlayerWrapper::Play, callback:%d iState[%s]"), 
          aCallback, 
          KCamAudioPlayerWrapperStateNames[iState] );

  switch( iState )
    {
    case CCamAudioPlayerWrapper::EReady:
      iPlayRequested = EFalse;
      iState         = CCamAudioPlayerWrapper::EPlaying;
      iCallback      = aCallback;
      iPlayer->SetVolume( static_cast<TInt>( iPlayer->MaxVolume()* KCamSoundVol ) );
      iPlayer->Play();
      break;

    case CCamAudioPlayerWrapper::EInitializing:
      iPlayRequested = ETrue;
      iCallback      = aCallback;
      break;

    case CCamAudioPlayerWrapper::ECorrupt:
      // Notify observer rigth away as init has failed.
      iCallback      = aCallback;
      NotifyPlayComplete( KErrCorrupt );
      break;

    case CCamAudioPlayerWrapper::EPlaying:
      PRINT( _L("Camera <> WARNING: Still playing old sound, need to stop early") );
      iPlayer->Stop();
      // Notify observer that playing was interrupted (if callback was requested for previous play).
      // Sets state to idle so that we are ready to play again.
      NotifyPlayComplete( KErrAbort );
      // Now we can repeat the call as the state is right.
      Play( aCallback ); 
      break;
    case CCamAudioPlayerWrapper::EIdle:
    default:
      //Set state and ignore otherwise...
      PRINT( _L("Camera <> CCamAudioPlayerWrapper::Play invalid state") );
      iState = CCamAudioPlayerWrapper::ECorrupt;
      break;
    }

  PRINT( _L("Camera <= CCamAudioPlayerWrapper::Play") );
  }


// ---------------------------------------------------------------------------
// CancelPlay <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerWrapper::CancelPlay()
  {
  PRINT1( _L("Camera => CCamAudioPlayerWrapper::CancelPlay, id:%d"), iSoundId );
  switch( iState )
    {
    case CCamAudioPlayerWrapper::EPlaying:
      PRINT( _L("Camera <> CCamAudioPlayerWrapper::CancelPlay .. iState == EPlaying, call Stop()..") );
      iPlayer->Stop();
      NotifyPlayComplete( KErrAbort );
      break;
    default:
      break;  
    }

  // Make sure pending Play() is cancelled.
  iPlayRequested = EFalse;      
  PRINT( _L("Camera <= CCamAudioPlayerWrapper::CancelPlay") );
  }


// ===========================================================================  
// from MMdaAudioPlayerCallback
  
// ---------------------------------------------------------------------------
// MapcInitComplete <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerWrapper
::MapcInitComplete(       TInt                       aStatus, 
                    const TTimeIntervalMicroSeconds& /*aDuration*/ )
  {
  PRINT2( _L("Camera => CCamAudioPlayerWrapper::MapcInitComplete, id:%d status:%d"), iSoundId, aStatus );
  if( KErrNone != aStatus )
    {
    // If we encounter an error in init phase,
    // we'll notify observer once Play gets called.
    iState = CCamAudioPlayerWrapper::ECorrupt;
    }
  else
    {
    iState = CCamAudioPlayerWrapper::EReady;
    }

  // If play has been called before init has finished
  // try to play now. If we had an error in init,
  // Play() will handle this based on iState.
  if( iPlayRequested )
    {
    Play( iCallback );
    }
  PRINT( _L("Camera <= CCamAudioPlayerWrapper::MapcInitComplete") );
  }

// ---------------------------------------------------------------------------
// MapcPlayComplete <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerWrapper::MapcPlayComplete( TInt aStatus )
  {
  NotifyPlayComplete( aStatus );
  }




// ===========================================================================  
// other public

// ---------------------------------------------------------------------------
// MapSoundId2FilenameL <<static>>
// ---------------------------------------------------------------------------
//
TPtrC
CCamAudioPlayerWrapper::MapSoundId2FilenameL( TInt aSoundId )
  {
  PRINT1( _L("Camera => CCamAudioPlayerWrapper::MapSoundId2FilenameL, id:%d"), aSoundId );
    
  TPtrC filename( KNullDesC );

  switch( aSoundId )
    {
    case ECamVideoStartSoundId:    filename.Set( KCamVideoStartTone );    break;
    case ECamVideoStopSoundId:     filename.Set( KCamVideoStopTone  );    break;
    case ECamVideoPauseSoundId:    filename.Set( KCamVideoPauseTone );    break;
    case ECamVideoResumeSoundId:   filename.Set( KCamVideoStartTone );    break;
    case ECamAutoFocusFailed:      User::Leave( KErrNotSupported );       break;
    case ECamAutoFocusComplete:    filename.Set( KCamAutoFocusComplete ); break;
    case ECamStillCaptureSoundId1: filename.Set( KCamCaptureTone1 );      break;
    case ECamStillCaptureSoundId2: filename.Set( KCamCaptureTone2 );      break;
    case ECamStillCaptureSoundId3: filename.Set( KCamCaptureTone3 );      break;
    case ECamStillCaptureSoundId4: filename.Set( KCamCaptureTone4 );      break;
    case ECamBurstCaptureSoundId1: filename.Set( KCamBurstCaptureTone1 );      break;
    case ECamBurstCaptureSoundId2: filename.Set( KCamBurstCaptureTone2 );      break;
    case ECamBurstCaptureSoundId3: filename.Set( KCamBurstCaptureTone3 );      break;
    case ECamBurstCaptureSoundId4: filename.Set( KCamBurstCaptureTone4 );      break;
    
    case ECamSelfTimerSoundId:     filename.Set( KCamSelfTimerTone );      break;

    default:
      __ASSERT_DEBUG( EFalse, CamPanic( ECamPanicNotSupported ) );
      break;
    }

  PRINT1( _L("Camera <= CCamAudioPlayerWrapper::MapSoundId2FilenameL, return [%S]"), &filename );
  return filename;
  }



// ===========================================================================  
// other private

// ---------------------------------------------------------------------------
// InitL
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerWrapper::InitL()
  {
  PRINT( _L("Camera => CCamAudioPlayerWrapper::InitL") );
  if( CCamAudioPlayerWrapper::EIdle == iState )
    {
    iState = CCamAudioPlayerWrapper::EInitializing;
    // Our process uses multiple players. Ensure they
    // (or their internal controllers) share the same
    // heap so we don't run out of memory chunks.
    iPlayer->UseSharedHeap();
    iPlayer->OpenFileL( MapSoundId2FilenameL( iSoundId ) );
    }
  else
    {
    CamPanic( ECamPanicInvalidState );
    PRINT( _L("Camera <> CCamAudioPlayerWrapper::InitL invalid state") );
    iState = CCamAudioPlayerWrapper::ECorrupt;
    User::Leave(KErrCorrupt);
    }
  PRINT( _L("Camera <= CCamAudioPlayerWrapper::InitL") );
  }


// ---------------------------------------------------------------------------
// NotifyPlayComplete
// ---------------------------------------------------------------------------
//
void
CCamAudioPlayerWrapper::NotifyPlayComplete( TInt aStatus )
  {
  PRINT1( _L("Camera => CCamAudioPlayerWrapper::NotifyPlayComplete, status:%d"), aStatus );
  switch( iState ) 
    {
    // Problem initializing.
    // Init is not tried again as it could slow down the capturing.
    case CCamAudioPlayerWrapper::ECorrupt:
      break;

    // Normal case where the playing has completed.
    // Set state to EReady to indicate we are ready 
    // for new play request. 
    case CCamAudioPlayerWrapper::EPlaying:
      iState = CCamAudioPlayerWrapper::EReady;
      break;

    // Other states not allowed here.
    default:
      // Make this player ready that we can play new sound.
      PRINT( _L("Camera <> CCamAudioPlayerWrapper::NotifyPlayComplete invalid state") );
      iState = CCamAudioPlayerWrapper::EReady;
      break;
    }

  // Clear flag just in case.
  iPlayRequested = EFalse;

  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAUDIOPLAYERWRAPPER_NOTIFYPLAYCOMPLETE, "e_CAM_APP_CAPTURE_SOUND_PLAY 0" ); //CCORAPP_CAPTURE_SOUND_PLAY_END

  // Notify observer with our sound id and received status.
  if( iCallback )
    iObserver.PlayComplete( aStatus, iSoundId );

  PRINT( _L("Camera <= CCamAudioPlayerWrapper::NotifyPlayComplete") );
  }

// ===========================================================================  
// private constructors

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerWrapper::ConstructL()
  {
  PRINT( _L("Camera => CCamAudioPlayerWrapper::ConstructL()") );
  iPlayer = CMdaAudioPlayerUtility::NewL( 
              *this, 
               KAudioPriority ,
               TMdaPriorityPreference( KAudioPrefCamera ) );

  InitL();
  PRINT( _L("Camera <= CCamAudioPlayerWrapper::ConstructL()") );
  }


// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamAudioPlayerWrapper
::CCamAudioPlayerWrapper( MCamPlayerObserver& aObserver, 
                          TInt                aSoundId )
  : iObserver     ( aObserver                     ),
    iSoundId      ( aSoundId                      ),
    iPlayer       ( NULL                          ),
    iState        ( CCamAudioPlayerWrapper::EIdle ),
    iPlayRequested( EFalse                        ),
    iCallback     ( EFalse                        )
  {
  }

// ===========================================================================  
// end of file

