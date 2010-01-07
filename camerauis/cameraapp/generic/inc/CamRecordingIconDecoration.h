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




#ifndef CAMRECORDINGICONDECORATION_H
#define CAMRECORDINGICONDECORATION_H

// INCLUDES
#include <e32base.h>
#include "CamObservable.h"
#include "CamControllerObservers.h"

// FORWARD DECLARATIONS
class MObserver;
class CCamObserverHandler;
class CBitmapContext;
class CFbsBitmap;
class CPeriodic;
class CCamAppController;

// CLASS DECLARATION
class CCamRecordingIconDecoration : public CBase,
                                    public MCamObservable,
                                    public MCamControllerObserver
    {
    private:
        // The internal recording states used
        enum TCamRecordState
            {
            ECamNotRecording,
            ECamRecording,
            ECamRecordPaused
            };
    
    public:
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aPosition The postition of the icon
        * @param aObserver Any interested observer
        * @return New instance of class
        */
    	static CCamRecordingIconDecoration* NewL( const TRect& aPosition, 
    	                                          CCamAppController& aController );

        /**
        * Destructor
        */
        ~CCamRecordingIconDecoration();
        
    protected:
        /**
        * Constructor
        */
        CCamRecordingIconDecoration( CCamAppController& aController );

        /**
        * Second-phase constructor
        * @param aPosition The postition of the icon
        * @param aController Reference to the controller
        */
        void ConstructL( const TRect& aPosition );
        
    public: // New methods
        /**
        * Draws the recording icon
        * @param aGc The context to use while drawing
        * @param aDrawRect Ignored
        */
        void Draw(CBitmapContext& aGc, const TRect& aDrawRect);

        /**
        * Callback for pause flash timer.
        * @since 2.8
        * @param aVideoContainer- the CCamVideoPreCaptureContainer
        * @return KErrNone
        */
        static TInt PauseFlashCallBack( TAny* aRecordingIconDecorator );

        /**
        * Toggles the paused icon called from PauseFlashCallBack
        * @since 2.8
        */
        void TogglePauseIconFlash();
    
    private: // New methods
        /**
        * Changes iRecordState, tells observers to update
        * @param aNewState The new state to change to
        */
        void ChangeRecordState(TCamRecordState aNewState);
            
    public: // From MCamObservable
        /**
        * From MCamObservable
        * @param aObserver Observer to register
        * @since 3.0
        */
        void RegisterObserverL(MCamObserver* aObserver);

        /**
        * From MCamObservable
        * @param aObserver Observer to deregister
        * @since 3.0
        */
        void DeregisterObserver(MCamObserver* aObserver);

        /**
        * From MCamObservable
        * @param aEvent Event code to broadcast
        * @since 3.0
        */
        void BroadcastEvent(TCamObserverEvent aEvent);
        
    public: // From MCamControllerObserver
        /**
        * Handle an event from CCamAppControllerBase.
        * @since 2.8
        * @param aEvent the controller event that has occurred
        * @param aError any reported error
        */
        virtual void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );
                                            
    private: // Data
        // Handles requests to the MCamObservable interface
        CCamObserverHandler* iObserverHandler;    
        
        // Where to put the icon
        TPoint iPosition;   
        TRect iSize;

        // Bitmap and mask for record icon
        CFbsBitmap* iRecBitmap;
        CFbsBitmap* iRecBitmapMask;

        // Bitmap amd mask for record paused icon
        CFbsBitmap* iRecPauseBitmap;
        CFbsBitmap* iRecPauseBitmapMask;

        // Current recording state
        TCamRecordState iRecordState;

        // Whether to show paused icon (toggled via periodic timer)
        TBool iShowPaused;

        // Periodic timer used to toggle paused icon
        CPeriodic* iPauseFlashPeriodic;
        
        // Pointer to the controller
        CCamAppController& iController;
    };
    

#endif // CAMRECORDINGICONDECORATION_H
