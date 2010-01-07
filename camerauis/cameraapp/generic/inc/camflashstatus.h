
#ifndef CAMFLASHSTATUS_H
#define CAMFLASHSTATUS_H/*
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
* Description:  Header file for CCamFlashStatus class.
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
*
*
*/



// ===========================================================================
// INCLUDES

#include <e32base.h>
#include "mcamcameraobserver.h"


// ===========================================================================
// CONSTANTS

// Flash icon blinking delay (duration of icon on/off state)
const TInt KFlashIconBlinkDelay = 300*1000; // [microseconds]

// Enumerated flash charging states. Should probably be 
// defined in the driver's headers and included from there.
enum TFlashChargeStatus
    {
    EFlashCharged       = 0x1,
    EFlashRequired      = 0x2,
    EFlashChargeTimeout = 0x4  
    };

/*
// Flash P&S UID & key
#ifndef  CAMERAAPP_SIMULATE_XENON_FLASH
const TInt KFlashPSUid = 0xA0002371;
#else
// Use application UID to simulate the flash charging publish
// and subscribe property. This way, no extra capabilities are needed
// for creating the property.
const TInt KFlashPSUid = 0x101FFA86;
#endif // CAMERAAPP_SIMULATE_XENON_FLASH
const TInt KFlashPSKey = 0;
*/


// ===========================================================================
// FORWARD DECLARATIONS

// 
//class CCamPropertyWatcher;
class CCamAppController;


// ===========================================================================
// CLASS DECLARATION

/**
* Interface to notify observer of flash icon visibility.
* This is used to create icon blinking effect when the flash
* is being recharged.
*/
class MFlashStatusObserver
    {
    public:        
        /*
        * Flash icon visibility has changed.
        * @param aVisible new visibility status
        */  
        virtual void FlashIconVisible( TBool aVisible ) = 0;      
        
        /**
        * Called every time when camera driver reports error in
        * flash recharging.                
        */
        virtual void FlashError() = 0;        
        
    };

/**
* Class to handle flash status and flash error status.
*
* - Keeps track of the flash recharge and flash error status
* - Creates events for visibility of the battery icon and uses
*   this to create blinking effect when the flash is being
*   charged
* - Gives an event when a charging error is received from
*   the camera driver. In this case, the UI can display
*   an error message.
*
*/
class CCamFlashStatus: public CBase,
                       public MCamCameraObserver
                     //public MPropertyObserver
    {
    public: // Construction and destruction
    
        /**
        * Two-phased constructor
        */
        static CCamFlashStatus* NewL( CCamAppController& aController );
        
        /**
        * Destructor
        */
        ~CCamFlashStatus();
        
    public: // New functions
    
        /**
        * Subscribes to flash icon blink events. At one time, only one 
        * observer can be listening for the events. An observer can
        * unregister itself by giving NULL as aObserver parameter
        * value.
        * 
        * @param aObserver pointer to observer or NULL
        */
        void Subscribe( MFlashStatusObserver* aObserver );   
        
        /**
        * Returns true, if flash is fully charged, otherwise false
        */
        TBool FlashReady() const;
        
        /**
        * Returns true if there has been an error recharging the flash
        */
        TBool FlashError() const;  
        
        /**
        * Set the value returned by FlashError. This may be needed
        * before application pretended exit.
        * @param aFlashError
        */                    
        void SetFlashError( TBool aFlashError );
                                
        /**
        * Returns true, if flash recharge error has already been
        * reported to user. This eliminates the error displayed
        * multiple times (for example, when switching between
        * image and video modes)
        */
        TBool ErrorCallbacksEnabled() const;
        
        /**
        * Enable/disable flash error callbacks. Should be set to ETrue
        * when error message is displayed to user, and to EFalse
        * during pretended application exit.
        * @param aErrorReported
        */        
        void EnableErrorCallbacks( TBool aCallbacksEnabled );                        
                        
        /**
        * Returns true, if flash icon should be displayed, false
        * otherwise. This cannot be used to determine the charging
        * status of the flash.
        */
        TBool FlashIconVisible() const;
        
    //public: // From MPropertyObserver
        /**
        * The value of a watched property has changed
        * @param aCategory The category of the property
        * @param aKey the Identifier of the property
        */    
        //void HandlePropertyChangedL( const TUid& aCategory, const TUint aKey );   

    public: // From MCamCameraObserver

        /**
        * Handle camera event
        */
        virtual void HandleCameraEventL( TInt              aStatus, 
                                         TCamCameraEventId aEventId, 
                                         TAny*             aEventData = NULL );
        
    private: 
    
        /** 
        * C++ default constructor
        */
        CCamFlashStatus( CCamAppController& aController );
        
        /**
        * Second phase constructor
        */
        void ConstructL();

        /**
        * Check if blinking needs to be stopped or started.
        */
        void CheckBlinkingStatus();
        
        /**
        * Start flash icon blinking timer
        */
        void StartBlinking();
        
        /**
        * Stop flash icon blinking timer, return
        * the icon to invisible state.
        */
        void StopBlinking();

        /**
        * Check if flash setting is off.
        * Flash not ready events are ignored if flash setting is off.
        */
        TBool IsFlashOff() const;
        
        /**
        * Notifies observer about error, if necessary
        */
        void NotifyObserverAboutFlashError();
        
        /**
        * Updates blink status of the flash icon. 
        * @param aThis
        */
        static TInt BlinkTimerCallback( TAny* aSelf );     
        
    private:      
        
        // Flash readiness to take a picture        
        TBool iFlashReady;
        
        // Flash is required to take a photo under current
        // lighting conditions.
        TBool iFlashRequired;
        
        // Flash recharging error status.
        // If set to true, there has been an error in recharging
        // the flash, and flash functionality is no more functional.
        TBool iFlashError;      
          
        // Determines whether cllabacks
        TBool iErrorCallbacksEnabled;
                
        // Flash status observer. Typically the container where
        // the flash icon resides.
        MFlashStatusObserver* iObserver;
    
        // Timer used to create blinking effect of the flash icon
        CPeriodic* iBlinkTimer;    
        
        TBool iBlinking;
        
        // Icon visibility 
        // true - visible, EFalse - not visible
        TBool iBlinkStatus;        
        
        // Used for watching the flash P&S key
        //CCamPropertyWatcher* iFlashPropertyWatcher;
        
        // Register for Camera events through controller.
        CCamAppController& iController;
    };

#endif // CAMFLASHSTATUS_H
