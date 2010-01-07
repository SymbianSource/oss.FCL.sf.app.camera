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
* Description:  Handles the playing of sounds and tones for the Camera App*
*/



// INCLUDE FILES
#include <AknSoundSystem.h>     //CAknKeySoundSystem

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include <avkon.rsg>
#include <StringLoader.h>
#include <AudioPreference.h>
#include <AknAppUi.h>

#include "CamSoundPlayer.h"
#include "Cam.hrh"
#include "camutility.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#if 0 // Old constants that may be used again in the future
const TInt KCamInCallToneHz = 1760;         
const TInt KCamInCallToneLen= 450000;       
_LIT( KCamSelfTimerSoundFile , "z:\\system\\sounds\\simple\\cameraappSelfTimer.rng" );
#endif 

const TReal32 KCamInCallToneVol = 0.75f;    

_LIT( KCamAutoFocusComplete, "z:\\system\\sounds\\digital\\cameraappFocusSucc.wav" );
//_LIT( KCamAutoFocusFailed,   "c:\\system\\sounds\\digital\\focus2.wav" ); NOT DEFINED YET


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamSoundPlayer::CCamSoundPlayer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamSoundPlayer::CCamSoundPlayer( MCamSoundPlayerObserver* aObs )
    {
    iObserver = aObs; 
    }

// -----------------------------------------------------------------------------
// CCamSoundPlayer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamSoundPlayer::ConstructL()
    {
    iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this, 
                                                 KAudioPriorityCameraTone,
                                                 TMdaPriorityPreference( KAudioPrefCamera ) );

    iTonePlayer = CMdaAudioToneUtility::NewL( *this, 
                                              NULL, 
                                              KAudioPriorityVideoRecording,
                                              TMdaPriorityPreference( KAudioPrefCamera ) );
    }

// -----------------------------------------------------------------------------
// CCamSoundPlayer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamSoundPlayer* CCamSoundPlayer::NewL( MCamSoundPlayerObserver* aObs )
    {
    CCamSoundPlayer* self = new( ELeave ) CCamSoundPlayer( aObs );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::~CCamSoundPlayer
// Destructor
// ---------------------------------------------------------------------------
//
CCamSoundPlayer::~CCamSoundPlayer()
  {
  PRINT( _L("Camera => ~CCamSoundPlayer") );
  if ( iAudioPlayer )
    {
    iAudioPlayer->Close();
    delete iAudioPlayer;
    }
  
  CancelTonePlayer();    
  delete iTonePlayer;
  PRINT( _L("Camera <= ~CCamSoundPlayer") );
  }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::InitialiseL
// Initialize sound player.
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::InitialiseL()
    {
    if ( iInitialised )
        {
        return;
        }

    iKeySoundSystem = static_cast<CAknAppUi*>( CEikonEnv::Static()->AppUi() )->KeySounds();

    if ( !iKeySoundSystem )
        {
        return;
        }

    TRAPD( error, iKeySoundSystem->AddAppSoundInfoListL( R_CAM_SOUND_LIST ) );

    if ( error && error != KErrAlreadyExists )
        {
        User::Leave( error );
        }

    iInitialised = ETrue;
    }



// -----------------------------------------------------------------------------
// CCamSoundPlayer::PlaySound
// Plays the requested sound
// -----------------------------------------------------------------------------
//
void CCamSoundPlayer::PlaySound( TCamSoundId aSoundId,
                                 TBool aEnableCallback )
    {
    PRINT( _L("Camera => CCamSoundPlayer::PlaySound") );  
      
    // Get into a known state before we begin
    switch( aSoundId )    
        {
        case ECamVideoStartSoundId:
            {
            StartPlaySound( KCamVideoStartTone(), aEnableCallback );
            break;
            }
    
        case ECamVideoStopSoundId:
            {
            StartPlaySound( KCamVideoStopTone(), aEnableCallback );
            break;
            }

        case ECamVideoPauseSoundId:
            {
            StartPlaySound( KCamVideoPauseTone(), aEnableCallback );
            break;
            }

        case ECamVideoResumeSoundId:
            {
            StartPlaySound( KCamVideoStartTone(), aEnableCallback );
            break;
            }

        case ECamAutoFocusFailed:
            {
            PRINT( _L("Camera <> PlaySound - ECamAutoFocusFailed") );
            // Do nothing as no failed WAV sound has been specified         	
            break;
            }

        case ECamAutoFocusComplete:
            {              
            PRINT( _L("Camera <> PlaySound - ECamAutoFocusComplete") );  
            StartPlaySound( KCamAutoFocusComplete(), aEnableCallback );         	
            break;
            }              

		case ECamStillCaptureSoundId1:
            {
            StartPlaySound( KCamCaptureTone1(), aEnableCallback );         	
            break;
            } 
            
      	case ECamStillCaptureSoundId2:
            {
            StartPlaySound( KCamCaptureTone2(), aEnableCallback );         	
            break;
            } 
        
        case ECamStillCaptureSoundId3:
            {
            StartPlaySound( KCamCaptureTone3(), aEnableCallback );         	
            break;
            } 
            
       case ECamStillCaptureSoundId4:
            {
            StartPlaySound( KCamCaptureTone4(), aEnableCallback );         	
            break;
            } 
#if 0
        case ECamInCallToneId:
            {
            PlayTone( KCamInCallToneHz, KCamInCallToneLen, KCamInCallToneVol, aEnableCallback );
            break;
            }
#endif
    
        default:
            {
            // Other sounds are played using the key sound system                
#if !( defined(__WINS__) || defined(__WINSCW__) )
#if 0
            iKeySoundSystem->PlaySound( aSoundId );
#endif            
#endif
            iEnableCallback = EFalse;
            break;
            }
        }  
    PRINT( _L("Camera <= CCamSoundPlayer::PlaySound") );            
    }

// -----------------------------------------------------------------------------
// CCamSoundPlayer::PlayTone
// Plays the requested tone
// -----------------------------------------------------------------------------
//
void CCamSoundPlayer::PlayTone( TInt aToneHz, TInt aLenMicSec, TReal32 aVolume, TBool aEnableCallback )
    {
    if ( !iOpenFileInProgress )
        {   
        iOpenFileInProgress = ETrue;
        TInt64 len = aLenMicSec;
        iTonePlayer->PrepareToPlayTone( aToneHz, TTimeIntervalMicroSeconds( len ) );
        iTonePlayer->SetVolume( static_cast<TInt>( aVolume * iTonePlayer->MaxVolume() ) );
        }
    else
        {
        if ( iEnableCallback )
            {
            iObserver->PlaySoundComplete();
            iEnableCallback = EFalse;
            }
        }  

    iEnableCallback = aEnableCallback;
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::StartPlaySound
// Starts the opening (and playback) of specified wav file
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::StartPlaySound( const TDesC& aFile, const TBool aEnableCallback )
    {   
    PRINT(_L("CCamSoundPlayer::StartPlaySound") )
    iAudioPlayer->Stop();
    iAudioPlayer->Close();

    TInt err = KErrNone;
    iEnableCallback = aEnableCallback;

    TRAP( err, iAudioPlayer->OpenFileL( aFile ) );
    if ( !err )
        {
        iOpenFileInProgress = ETrue;
        }
    else
        {
        // The last OpenFileL call hasn't completed yet.
        err = KErrInUse;
        }
    
    PRINT1(_L("CCamSoundPlayer::StartPlaySound err (%d)"), err )
    
    if ( err && aEnableCallback )    
        {
        // Cannot play sound, so notify client.
        iObserver->PlaySoundComplete();
        iEnableCallback = EFalse;
        }    
    }

// ---------------------------------------------------------------------------
// CCamSoundPlayer::StartPlayTone
// Attempts to begin playing the specified tone
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::StartPlayTone( const TDesC& aFile, const TBool aEnableCallback )
    {
    // Check if we are already trying to open/play a tone, and stop it 
    // if so
    if ( iOpenFileInProgress )
        {
        iTonePlayer->CancelPrepare();
        iOpenFileInProgress = EFalse;
        if ( iEnableCallback )
            {
            iObserver->PlaySoundComplete();
            iEnableCallback = EFalse;
            }
        }
    else if ( iTonePlayInProgress )
        {
        iTonePlayer->CancelPlay();
        iTonePlayInProgress = EFalse;
        if ( iEnableCallback )
            {
            iObserver->PlaySoundComplete();
            iEnableCallback = EFalse;
            }
        }    
    else
        {
        // empty statement to remove Lint error.
        }

    iEnableCallback = aEnableCallback;

    iOpenFileInProgress = ETrue;
    iTonePlayer->PrepareToPlayFileSequence( aFile );
    iTonePlayer->SetVolume( static_cast<TInt>( KCamInCallToneVol * iTonePlayer->MaxVolume() ) );
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::MapcInitComplete
// CMdaAudioPlayerUtility initialization complete
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::MapcInitComplete( TInt aError, 
                                       const TTimeIntervalMicroSeconds& /*aDuration*/ )
    {
    iOpenFileInProgress = EFalse;

    if ( !aError )
        {
        iAudioPlayer->Play();
        }
    else
        {
        if ( iEnableCallback )
            {
            iObserver->PlaySoundComplete();
            iEnableCallback = EFalse;
            }
        }   
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::MapcPlayComplete
// Playback complete, notify observer
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::MapcPlayComplete( TInt /*aError*/ )
    {
    if ( iEnableCallback && iObserver)
        {
        iObserver->PlaySoundComplete();
        iEnableCallback = EFalse;
        }
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::MatoPrepareComplete
// Tone prepare complete.
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::MatoPrepareComplete( TInt aError )
    {
    iOpenFileInProgress = EFalse;

    if ( !aError )
        {
        iTonePlayInProgress = ETrue;
        iTonePlayer->Play();
#ifdef __WINS__
// The tone player does not give a call back on the emulator so
// we have to force a callback here
    MatoPlayComplete( KErrNone );
#endif // __WINS__
        }
    else
        {
        if ( iEnableCallback )
            {
            iObserver->PlaySoundComplete();
            iEnableCallback = EFalse;
            }
        }
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::MatoPlayComplete
// Tone playback complete.
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::MatoPlayComplete( TInt /*aError*/ )
    {
    iTonePlayInProgress = EFalse;
    if ( iEnableCallback )
        {
        iObserver->PlaySoundComplete();
        iEnableCallback = EFalse;
        }
    }



// ---------------------------------------------------------------------------
// CCamSoundPlayer::DisableSelectionKeySoundL
// Called to disable certain key press clicks
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::DisableSelectionKeySoundL()
    {
    if ( iKeySoundSystem && !iSelectionKeySilent )
        {
        iKeySoundSystem->PushContextL( R_CAM_SILENT_OK_KEY_LIST ); 
        iSelectionKeySilent = ETrue;
        }
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::EnableSelectionKeySound
// Called to enable certain key press clicks
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::EnableSelectionKeySound()
    {
    if ( iKeySoundSystem && iSelectionKeySilent )
        {
        iKeySoundSystem->PopContext();
        iSelectionKeySilent = EFalse;
        }
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::DisableAllKeySoundsL
// Called to disable all key sounds, usually when video recording 
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::DisableAllKeySoundsL()
    {
    if ( iKeySoundSystem && !iAllKeysSilent )    
        {
        iKeySoundSystem->PushContextL( R_AVKON_SILENT_SKEY_LIST );
        iKeySoundSystem->BringToForeground();
        iKeySoundSystem->LockContext();
        iAllKeysSilent = ETrue;
        }
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::EnableAllKeySoundsL
// Called to enable all key sounds, usually when video recording 
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::EnableAllKeySounds()
    {
    if ( iKeySoundSystem && iAllKeysSilent )
        {
        iKeySoundSystem->ReleaseContext();
        iKeySoundSystem->PopContext();
        iAllKeysSilent = EFalse;
        }
    }


// ---------------------------------------------------------------------------
// CCamSoundPlayer::CancelTonePlayer
// Cancels any outstanding tone player activity
// ---------------------------------------------------------------------------
//
void CCamSoundPlayer::CancelTonePlayer()
    {
    if ( iOpenFileInProgress && iTonePlayer )
        {   
        iTonePlayer->CancelPrepare();
        iOpenFileInProgress = EFalse;

        if ( iEnableCallback && iObserver )
            {
            iObserver->PlaySoundComplete();
            iEnableCallback = EFalse;
            }
        }
        
    if ( iTonePlayInProgress && iTonePlayer )
        {
        iTonePlayer->CancelPlay();        
        iTonePlayInProgress = EFalse;

        if ( iEnableCallback && iObserver )
            {
            iObserver->PlaySoundComplete();
            iEnableCallback = EFalse;
            }        
        }
    }
    


//  End of File  
