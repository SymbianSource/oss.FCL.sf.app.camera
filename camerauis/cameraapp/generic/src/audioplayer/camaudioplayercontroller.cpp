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
* Description:  Implementation of Controller for Sound / Tone Wrappers
*
*/



// ===========================================================================  
// includes
#include <e32base.h>
#include <aknappui.h>       // CAknAppUi
#include <aknsoundsystem.h> // CAknKeySoundSystem

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "camlogging.h"

#include "CamSettingsInternal.hrh"       // TCamCameraMode, TCamSettingItemIds
#include "CamAppController.h"
#include "mcamplayerobserver.h"
#include "mcamplayerwrapper.h"
#include "camaudioplayerwrapper.h"
#include "camaudioplayercontroller.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "camaudioplayercontrollerTraces.h"
#endif



// ===========================================================================
// Constants
namespace NCamAudioPlayerController
  {
  static const TInt KImageCaptureSoundId = KMinTInt;


  // Cached sounds which are loaded once entering image mode.
  // No need to include capture sound, it is selected based
  // on the setting.
  static const TInt KImageCachedSounds[] =
    {
    // Our own id for image capture sound,
    // which can change.
    // Also avoid compilation error, if AF beep not supported.
    KImageCaptureSoundId,
    ECamAutoFocusComplete
    };
  
  // Cached sounds which are loaded once entering video mode.
  static const TInt KVideoCachedSounds[] =
    {
    ECamVideoStartSoundId,
    ECamVideoStopSoundId,
    ECamVideoPauseSoundId,
    ECamVideoResumeSoundId
    };
  }

using namespace NCamAudioPlayerController;

// ===========================================================================
// public constructors and destructor

// ---------------------------------------------------------------------------
// NewL <<static>>
// ---------------------------------------------------------------------------
//
CCamAudioPlayerController* 
CCamAudioPlayerController::NewL( CCamAppController&  aController,
                                 MCamPlayerObserver& aObserver )
  {
  CCamAudioPlayerController* self = 
    new (ELeave) CCamAudioPlayerController( aController, aObserver );
    
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;  
  }

// ---------------------------------------------------------------------------
// destructor <<virtual>>
// ---------------------------------------------------------------------------
//
CCamAudioPlayerController::~CCamAudioPlayerController()
  {
  iController.RemoveControllerObserver( this );
  iController.RemoveSettingsObserver( this );

  Reset();
  iPlayers.Close();

  iKeySoundSystem = NULL; // not own
  }




// ===========================================================================
// From MCamControllerObserver

// ---------------------------------------------------------------------------
// HandleControllerEventL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController
::HandleControllerEventL( TCamControllerEvent aEvent,
                          TInt                aError )
  {
  PRINT( _L("Camera => CCamAudioPlayerController::HandleControllerEventL") );
  switch( aEvent )
    {
    case ECamEventEngineStateChanged:
      {
      PRINT( _L("Camera <> CCamAudioPlayerController: case ECamEventEngineStateChanged") );
      if( KErrNone == aError )
        {
        TCamCameraMode mode = iController.CurrentMode();
        PRINT2( _L("Camera <> CCamAudioPlayerController: KErrNone, old mode[%s] new mode[%s]"), KCamModeNames[iCameraMode], KCamModeNames[mode] );
        if( mode != iCameraMode )
          {
          // Changing to video or image mode.
          // Cache the players needed.
          // If this fails we just retry once a sound is really needed.
          if     ( ECamControllerImage    == mode 
                || ECamControllerVideo    == mode ) { Setup( mode ); }
          // Changing to shutdown mode
          // Release all players.
          else if( ECamControllerShutdown == mode || ECamControllerIdle == mode ) 
        	  {
        	  Setup( mode );
        	  }
          // Other mode entered, no action
          else                                      { /* no action */ }
          }
        }
      else
        {
        PRINT1( _L("Camera <> CCamAudioPlayerController: error in (%d), call Reset.."), aError );
        Reset();
        }
      break;
      }

    default:
      {
      break;
      }
    }
  PRINT( _L("Camera <= CCamAudioPlayerController::HandleControllerEventL") );
  }



// ===========================================================================
// From MCamSettingsModelObserver

// ---------------------------------------------------------------------------
// IntSettingChangedL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController
::IntSettingChangedL( TCamSettingItemIds aSettingItem, 
                      TInt               /*aSettingValue*/ )
  {
  PRINT( _L("Camera => CCamAudioPlayerController::IntSettingChangedL") );
  switch( aSettingItem )
    {
    case ECamSettingItemPhotoCaptureTone:
      {
      PRINT( _L("Camera <> CCamAudioPlayerController: case ECamSettingItemPhotoCaptureTone") );

      // The setting value is *not* of TCamSoundId type.
      // See CCamAppController::CaptureToneId().
      const TCamSoundId newSoundId = iController.CaptureToneId();
      SetupCaptureSoundPlayerL( newSoundId );

      break;
      }
    default:
      {
      break;
      }
    }

  PRINT( _L("Camera <= CCamAudioPlayerController::IntSettingChangedL") );
  }


// ---------------------------------------------------------------------------
// TextSettingChangedL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController
::TextSettingChangedL(       TCamSettingItemIds /*aSettingItem*/, 
                       const TDesC&             /*aSettingValue*/ )
  {
  // these events are ignored
  }



// ===========================================================================
// new public

// ---------------------------------------------------------------------------
// PlaySound
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::PlaySound( TInt  aSoundId,
                                      TBool aCallback )
  {
  TRAPD( status,
    {
    // Get cached player, if available.
    // Otherwise create new one.
    CCamPlayerWrapperBase* player = GetPlayerL( aSoundId );
    player->Play( aCallback );
    });

  if( KErrNone != status )
    {
    if( KErrNoMemory == status )
      {
      // If OOM, release all players.
      // Create new ones again when needed.
      Reset();
      }
    if( aCallback )
      {
      // If could not instantiate a player, notify observer right away.
      iObserver.PlayComplete( status, aSoundId );
      }
    }
  }


// ---------------------------------------------------------------------------
// CancelAllPlaying
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::CancelAllPlaying()
  {
  PRINT( _L("Camera => CCamAudioPlayerController::CancelAllPlaying") );

  for( TInt i = 0; i < iPlayers.Count(); i++ )
    {
    iPlayers[i]->CancelPlay();
    }

  PRINT( _L("Camera <= CCamAudioPlayerController::CancelAllPlaying") );
  }

// ---------------------------------------------------------------------------
// CancelPlaying
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::CancelPlaying( TInt aSoundId )
  {
  TInt index = FindPlayerIndex( aSoundId );
  if( KErrNotFound != index )
    {
    CCamPlayerWrapperBase* player = iPlayers[index];
    player->CancelPlay();
    }
  }


// ---------------------------------------------------------------------------
// InitKeySoundSystem
// ---------------------------------------------------------------------------
// 
void
CCamAudioPlayerController::InitKeySoundSystem()
  {
  PRINT( _L("Camera => CCamAudioPlayerController::InitKeySoundSystem") );
  iKeySoundSystem = static_cast<CAknAppUi*>( CEikonEnv::Static()->AppUi() )->KeySounds();
  /* No sounds played via iKeySoundSystem. No need to add sound list.
  if( !iKeySoundSystem )
    {
    iKeySoundSystem = static_cast<CAknAppUi*>( CEikonEnv::Static()->AppUi() )->KeySounds();
    if ( iKeySoundSystem )
      {
      TRAPD( status, iKeySoundSystem->AddAppSoundInfoListL( R_CAM_SOUND_LIST ) );
      PRINT1( _L("Camera <> CCamAudioPlayerController: status:%d"), status );
      if( KErrNone          != status
       && KErrAlreadyExists != status
        )
        {
        // Set unusable
        iKeySoundSystem = NULL;
        }  
      }
    }
  */
  PRINT( _L("Camera <= CCamAudioPlayerController::InitKeySoundSystem") );
  }

// ---------------------------------------------------------------------------
// EnableAllKeySounds
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::EnableAllKeySounds()
  {
  if ( iKeySoundSystem && iKeySoundsDisabled )
    {
    iKeySoundSystem->ReleaseContext();
    iKeySoundSystem->PopContext();
    iKeySoundsDisabled = EFalse;
    }
  }


// ---------------------------------------------------------------------------
// DisableAllKeySoundsL
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::DisableAllKeySoundsL()
  {
  if ( iKeySoundSystem && !iKeySoundsDisabled )    
    {
    iKeySoundSystem->PushContextL( R_AVKON_SILENT_SKEY_LIST );
    iKeySoundSystem->BringToForeground();
    iKeySoundSystem->LockContext();
    iKeySoundsDisabled = ETrue;
    }
  }




// ===========================================================================
// new private

// ---------------------------------------------------------------------------
// FindPlayerIndex
// ---------------------------------------------------------------------------
//
TInt 
CCamAudioPlayerController::FindPlayerIndex( TInt aSoundId ) const
  {
  TInt found = KErrNotFound;

  // Check that id is valid.
  if( aSoundId > 0 )
    {
    TInt current = iPlayers.Count(); 
    while( --current >= 0 )
      {
      // Reuse player, if the sound is the same despite
      // sound id being different.
      // if( aSoundId == iPlayers[current]->Id() )
      if( iPlayers[current]->IsEqualSound( aSoundId ) )
        {
        found   = current;
        current = -1;
        }
      }
    }
  return found;
  }
    
// ---------------------------------------------------------------------------
// GetPlayerL
// ---------------------------------------------------------------------------
//
CCamPlayerWrapperBase*
CCamAudioPlayerController::GetPlayerL( TInt aSoundId )
  {
  PRINT1( _L("Camera => CCamAudioPlayerController::GetPlayerL, sound id:%d"), aSoundId );
  CCamPlayerWrapperBase* player( NULL );
  
  if( KImageCaptureSoundId == aSoundId )
    {
    player = GetPlayerL( iController.CaptureToneId() );
    }
  else
    {
    TInt index = FindPlayerIndex( aSoundId );
  
    if( KErrNotFound != index )
      {
      player = iPlayers[index];
      }
    else
      {
      OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAUDIOPLAYERCONTROLLER_GETPLAYERL, "e_CAM_APP_CAPTURE_SOUND_LOAD 1" ); //CCORAPP_CAPTURE_SOUND_LOAD_START
      
      player = CCamAudioPlayerWrapper::NewL( iObserver, aSoundId );
  
      CleanupStack::PushL( player );
      iPlayers.AppendL( player );
      CleanupStack::Pop( player );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMAUDIOPLAYERCONTROLLER_GETPLAYERL, "e_CAM_APP_CAPTURE_SOUND_LOAD 0" ); //CCORAPP_CAPTURE_SOUND_LOAD_END
      }
    }

  PRINT( _L("Camera <= CCamAudioPlayerController::GetPlayerL") );
  return player;
  }



// ---------------------------------------------------------------------------
// RemovePlayer
// ---------------------------------------------------------------------------
//
void
CCamAudioPlayerController::RemovePlayer( TInt aSoundId )
  {
  TInt index = FindPlayerIndex( aSoundId );
  if( KErrNotFound != index )
    {
    CCamPlayerWrapperBase* player = iPlayers[index];
    iPlayers.Remove( index );
    delete player;
    player = NULL;
    }
  }

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::Setup( TCamCameraMode aCameraMode )
  {
  PRINT( _L("Camera => CCamAudioPlayerController::Setup") );
  Reset();

  TInt status( KErrNone );  
  switch( aCameraMode )
    {
    case ECamControllerImage:
      {
      TRAP( status, SetupForImageL() );
      break;
      }

    case ECamControllerVideo:
      {
      TRAP( status, SetupForVideoL() );
      break;
      }

    case ECamControllerIdle:
    case ECamControllerShutdown:
    default:
      {
      // Already reset, no sounds needed now.
      break;
      }
    }

  // If any problems, try reload sounds once really needed, so reset now.
  if( KErrNone != status ) 
    Reset();
  else
    iCameraMode = aCameraMode;

  PRINT( _L("Camera <= CCamAudioPlayerController::Setup") );
  }

// ---------------------------------------------------------------------------
// SetupForVideoL
// ---------------------------------------------------------------------------
//
void
CCamAudioPlayerController::SetupForVideoL()
  {
  PRINT( _L("Camera => CCamAudioPlayerController::SetupForVideoL") );
  InitKeySoundSystem();

  TInt count( sizeof( KVideoCachedSounds ) / sizeof( TInt ) );
  PRINT1( _L("Camera <> CCamAudioPlayerController: count of players to load: %d"), count );
  for( TInt i = 0; i < count; i++ )
    {
    GetPlayerL( KVideoCachedSounds[i] );
    }
  PRINT( _L("Camera <= CCamAudioPlayerController::SetupForVideoL") );
  }

// ---------------------------------------------------------------------------
// SetupForImageL
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::SetupForImageL()
  {
  PRINT( _L("Camera => CCamAudioPlayerController::SetupForImageL") );
  TInt count( sizeof( KImageCachedSounds ) / sizeof( TInt ) );

  PRINT1( _L("Camera <> CCamAudioPlayerController: count of players to load: %d"), count );
  for( TInt i = 0; i < count; i++ )
    {
    GetPlayerL( KImageCachedSounds[i] );
    }
  PRINT( _L("Camera => CCamAudioPlayerController::SetupForImageL") );
  }

// ---------------------------------------------------------------------------
// SetupCaptureSoundPlayerL
// ---------------------------------------------------------------------------
//
void
CCamAudioPlayerController::SetupCaptureSoundPlayerL( TInt aNewSoundId )
  {
  PRINT2( _L("Camera => CCamAudioPlayerController::SetupCaptureSoundPlayerL, old sound:%d new sound:%d"), iCaptureSoundId, aNewSoundId );

  if( aNewSoundId != iCaptureSoundId )
    {
    // Free old player, if exists.
    RemovePlayer( iCaptureSoundId );
  
    // If loading new fails, retry when sound is needed.
    iCaptureSoundId = -1;
    GetPlayerL( aNewSoundId ); 
    iCaptureSoundId = aNewSoundId;
    }
  PRINT ( _L("Camera <= CCamAudioPlayerController::SetupCaptureSoundPlayerL") );
  }

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::Reset()
  {
  PRINT ( _L("Camera => CCamAudioPlayerController::Reset") );
  iPlayers.ResetAndDestroy();


  iCameraMode     = ECamControllerIdle;
  iCaptureSoundId = -1;
  PRINT ( _L("Camera <= CCamAudioPlayerController::Reset") );
  }



// ===========================================================================
// private constructors

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void 
CCamAudioPlayerController::ConstructL()
  {
  iController.AddControllerObserverL( this );
  iController.AddSettingsObserverL( this );
  }

// ---------------------------------------------------------------------------
// constructor
// ---------------------------------------------------------------------------
//
CCamAudioPlayerController
::CCamAudioPlayerController( CCamAppController&  aController,
                             MCamPlayerObserver& aObserver )
  : iController       ( aController            ),
    iObserver         ( aObserver              ),
    iCameraMode       ( ECamControllerIdle ),
    iCaptureSoundId   ( -1                     ),
    iKeySoundsDisabled( EFalse                 )
  {
  }

// ===========================================================================  
// end of file

