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
* Description:  Audio Player Controller
*
*/


#ifndef CAM_AUDIOPLAYERCONTROLLER_H
#define CAM_AUDIOPLAYERCONTROLLER_H

// ===========================================================================
// Includes
#include <e32base.h>
#include "CamControllerObservers.h"
#include "mcamsettingsmodelobserver.h"

// ===========================================================================
// Forward declarations
class CAknKeySoundSystem;
class CCamAppController;
class MCamAudioPlayerObserver;
class CCamPlayerWrapperBase;
class CCamTonePlayerWrapper;

// ===========================================================================
// Class definitions

/**
* Audio Player Controller class
*
* Controls tone and audio players and caches needed
* players based on Camera capture mode to enable
* fast playing of needed sounds/tones.
*
*/
class CCamAudioPlayerController : public CBase,
                                  public MCamControllerObserver,
                                  public MCamSettingsModelObserver
  {
  // =======================================================
  // Methods

  // -------------------------------------------------------
  // constructors and destructor
  public:

    static CCamAudioPlayerController* NewL( CCamAppController&  aController,
                                            MCamPlayerObserver& aObserver );
    virtual ~CCamAudioPlayerController();

  // -------------------------------------------------------
  // from MCamControllerObserver
  public: 
    
    virtual void HandleControllerEventL( TCamControllerEvent aEvent, TInt aStatus );

  // -------------------------------------------------------
  // from MCamSettingsModelObserver
  public: 
  
    virtual void IntSettingChangedL ( TCamSettingItemIds aSettingItem, TInt aSettingValue );
    virtual void TextSettingChangedL( TCamSettingItemIds aSettingItem, const TDesC& aSettingValue );

  // -------------------------------------------------------
  // new public
  public:
    
    /**
    * @param aCallback If set ETrue, calling method this results in one 
    *                  callback to MCamAudioPlayerObserver::PlayComplete
    */
    void PlaySound( TInt  aSoundId, 
                    TBool aCallback );

    /**
    * @param aCallback If set ETrue, calling method this results in one 
    *                  callback to MCamAudioPlayerObserver::PlayComplete
    */
    void PlayTone( TInt    aToneInHz, 
                   TInt    aLengthInUs, 
                   TReal32 aVolumeInPercent, 
                   TBool   aCallback );

    /**
    * Cancel all ongoing playing activity.
    */
    void CancelAllPlaying();
    
    /**
    * Cancel given sound playing, if ongoing.
    * @aSoundId Id of the sound to be cancelled.
    */
    void CancelPlaying( TInt aSoundId );
    
    void InitKeySoundSystem();
    void EnableAllKeySounds();
    void DisableAllKeySoundsL();


  // -------------------------------------------------------
  // new private
  private:

    TInt                   FindPlayerIndex( TInt aSoundId ) const;
    CCamPlayerWrapperBase* GetPlayerL     ( TInt aSoundId );
    void                   RemovePlayer   ( TInt aSoundId );

    void Setup( TCamCameraMode aCameraMode );
    void SetupForVideoL();
    void SetupForImageL();
    void SetupCaptureSoundPlayerL( TInt aNewSoundId );
    void Reset();
  
  // -------------------------------------------------------
  // private constructors
  private:

    void ConstructL();
    
    CCamAudioPlayerController( CCamAppController&  aController,
                               MCamPlayerObserver& aObserver );

  // =======================================================
  // Data
  private:

    CCamAppController&                   iController;
    MCamPlayerObserver&                  iObserver;

    RPointerArray<CCamPlayerWrapperBase> iPlayers;
    CCamTonePlayerWrapper*               iTonePlayer;

    TInt                                 iCameraMode;     // TCamCameraMode value
    TInt                                 iCaptureSoundId; // Sound id or -1 if not set

    CAknKeySoundSystem*                  iKeySoundSystem; // not own
    TBool                                iKeySoundsDisabled;

  // =======================================================
  };

#endif // CAM_AUDIOPLAYERCONTROLLER_H

// ===========================================================================
// end of file


