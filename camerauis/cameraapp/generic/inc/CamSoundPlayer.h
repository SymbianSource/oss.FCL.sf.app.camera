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



#ifndef CAMSOUNDPLAYER_H
#define CAMSOUNDPLAYER_H

//  INCLUDES
#include <MdaAudioSamplePlayer.h>
#include "Cam.hrh" // For TCamSoundId

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknKeySoundSystem;

// CLASS DECLARATION


/**
* Observer class for getting notified when sound playback completes.
*/
class MCamSoundPlayerObserver
    {
    public:
        /**
        * CCamSoundPlayer has finished sound playback
        * and callback was requested (aEnableCallback was true
        * in CCamSoundPlayer::PlaySound()).
        * @since 2.8
        */
        virtual void PlaySoundComplete() = 0;
    };

/**
*  This class is used to play sounds and tones for the Camera app
*
*  @since 2.8
*/
class CCamSoundPlayer : public CBase,
                        public MMdaAudioPlayerCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aObs Observer to be informed when sound playback is complete.
        */
        static CCamSoundPlayer* NewL( MCamSoundPlayerObserver* aObs );
        
        /**
        * Destructor.
        */
        virtual ~CCamSoundPlayer();

    public: // New functions
        
        /**
        * Initialises the sound player.  
        * @since 2.8
        */
        void InitialiseL();

        /**
        * Starts procedure to play the specified sound
        * @since 2.8 
        * @param aSound The sound to play
        * @param aEnableCallback Whether to call back when play complete
        */
        void PlaySound( TCamSoundId aSound, TBool aEnableCallback );

      
        /**
        * Called to disable certain key press clicks
        * @since 2.8 
        */
        void DisableSelectionKeySoundL();

        /**
        * Called to enable certain key press clicks
        * @since 2.8 
        */
        void EnableSelectionKeySound();

        /**
        * Called to disable all key sounds, usually when video recording 
        * is active
        * @since 2.8 
        */
        void DisableAllKeySoundsL();

        /**
        * Called to enable all key sounds, usually when video recording 
        * is has stopped
        * @since 2.8 
        */
        void EnableAllKeySounds();
        

    public: // Functions from base classes
        /**
        * From MMdaAudioPlayerCallback.
        * @since 2.8
        */
        void MapcInitComplete( TInt aError, 
            const TTimeIntervalMicroSeconds& aDuration );

        /**
        * From MMdaAudioPlayerCallback.
        * @since 2.8
        */
        void MapcPlayComplete( TInt aError );
        
    private:

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aObs Observer to be informed when sound playback is complete.        
        */
        CCamSoundPlayer( MCamSoundPlayerObserver* aObs );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Starts procedure to play audio (WAV) files. Internal function
        * @since 2.8 
        * @param aFile The file to open
        * @param aEnableCallback Whether to call back when play complete
        */
        void StartPlaySound( const TDesC& aFile, const TBool aEnableCallback );

    public:     // Data

        // Audio player utility for WAV sounds (eg Video Recording)
        CMdaAudioPlayerUtility* iAudioPlayer;

        // Key sound system for Self-timer sound and camera shutter.
        CAknKeySoundSystem* iKeySoundSystem;

        // Observer to notify when playback completes
        MCamSoundPlayerObserver* iObserver;

        // Whether file open is still in progress
        TBool iOpenFileInProgress;
    
        // Whether a callback for current sound has been requested
        TBool iEnableCallback;

        // Whether this class has been initialised yet
        TBool iInitialised;

        // Whether selection keys have been silenced
        TBool iSelectionKeySilent;

        // Whether all keys have been silenced
        TBool iAllKeysSilent;        

        };

#endif      // CAMSOUNDPLAYER_H   
            
// End of File
