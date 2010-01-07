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
* Description:  CCamSelfTimer class for Camera*
*/



#ifndef CAMSELFTIMER_H
#define CAMSELFTIMER_H

#include "CamAppController.h"
#include "CamSelfTimerObserver.h"
#include "CamAppUiBase.h"   // For TCamSelfTimerFunctions
#include "CamPSI.h" 

//////////



// INCLUDES

// MACROS

// DATA TYPES

// Represents the selftimer state.  Allows up to 3 different active states, 
// plus the standby (inactive) state.
enum TCamSelfTimer  
    {
    ECamSelfTimerActive1,   
    ECamSelfTimerActive2,   
    ECamSelfTimerActive3,
    ECamSelfTimerStandby
    };


// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Implements self-timer functionality 
*
*  @since 2.8
*/
class CCamSelfTimer : public CBase
    {   
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCamSelfTimer* NewL( CCamAppController& aController );
        
        /**
        * Destructor.
        */
        virtual ~CCamSelfTimer();

    public: // New functions
        
        /**
        * Called by client to start the self timer functionatlity.
        * Starts the timer counting down.
        * @since 2.8
        */
        void StartSelfTimer();

        /**
        * Called by client to set the current capture mode.  This is needed
        * to ensure the Self-Timer operation, if started, would call the 
        * correct capture (for video or photo) on timer completion.
        * @since 2.8
        * @param aMode The capture mode (video/image)
        * @param aMode The image capture mode (single/burst/timelapse)
        * @param aFunc The period data set to use
        */
        void SetModeL( TCamCameraMode         aMode, 
                      TCamImageCaptureMode   aImageMode,
                      TCamSelfTimerFunctions aFunc );
        

        /**
        * Cancels any oustanding self-timer operation, and resets internal
        * state to standby/idle state.
        * @since 2.8
        */
        void Cancel();

        /**
        * Returns whether the self-timer is currently active (counting down)
        * @since 2.8
        * @return TBool - ETrue if active, EFalse if in standby/idle.
        */
        TBool IsActive();

        /**
        * Add a self timer observer
        * @since 2.8
        * @param aObserver Pointer to object implementing MCamSelfTimerObserver
        */
        void AddObserverL( const MCamSelfTimerObserver* aObserver );

        /**
        * Remove self timer observer
        * @since 2.8
        * @param aObserver Pointer to object implementing MCamSelfTimerObserver
        */
        void RemoveObserver( const MCamSelfTimerObserver* aObserver );

        /**
        * Send event to self timer observers
        * @since 2.8
        * @param aEvent event to send to observers
        */
        void NotifyObservers( const TCamSelfTimerEvent aEvent );

    public: // Functions from base classes
       
    protected:  // New functions        

    protected:  // Functions from base classes
        
    private:

        /**
        * C++ default constructor.
        */
        CCamSelfTimer( CCamAppController& aController );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Allows the self-timer object to change it's internal state based upon
        * the parameter passed in.
        * @since 2.8
        * @param aState, The internal state to move to.
        */
        void ChangeState( TCamSelfTimer aState); 

        /**
        * The timer callback used by the periodic timer.  Will call the 
        * CCamSelfTimer's Tick() method.
        * @since 2.8
        * @param aPtr A pointer to the instance of CCamSelfTimer
        * @return KErrNone
        */
        static TInt TimerCallback( TAny* aPtr );

        /**
        * Called by the timer callback function to progress the internal 
        * self-timer state accordingly.  
        * @since 2.8
        */
        void Tick();

        /**
        * Turns the indicators (LED/icon/tone) on or off depending on the 
        * parameter passed in.
        * @since 2.8
        * @param aOn If ETrue, turns the indication(s) on, else turns them off
        */
        void DoIndication( TBool aOn );

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Main timer
        CPeriodic* iPeriodicTimer;
        
        // Callback used for timer
        TCallBack iCallback;
        
        // Current state of the SelfTimer object
        TCamSelfTimer iState;               

        // Whether LED is currently on or off
        TBool iIndication;  

        // Specifies whether capturing video or still.
        TCamCameraMode         iMode; 
        // Image submode (single/burst/timelapse)
        TCamImageCaptureMode   iImageMode;

         
        // Tracks the time remaining before capture when timer is active.
        TTimeIntervalMicroSeconds32 iTimeRemaining;

        // Tracks the period between "ticks" when timer is active
        TTimeIntervalMicroSeconds32 iTimerTickPeriod; 

        // Camera Controller used to start autofocus/capture operations
        CCamAppController& iController;

        // array of self timer observer objects
        RPointerArray<MCamSelfTimerObserver> iObservers;

        // Holds the period details from the PSI
        TSelfTimerSetting iSettings;

        // Tracks the time that has elapsed since last indicator state change
        TInt iTimeElapsedSinceIndication;

    public:     // Friend classes        
    protected:  // Friend classes        
    private:    // Friend classes                
    };

#endif      // CAMSELFTIMER_H   
            
// End of File














