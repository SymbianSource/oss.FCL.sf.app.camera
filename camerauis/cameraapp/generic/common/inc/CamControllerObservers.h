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
* Description:  Defines abstract API's for the controller observers*
*/




#ifndef CAMCONTROLLEROBSERVERS_H
#define CAMCONTROLLEROBSERVERS_H

 

/**
*  Controller events that are reported to observers through the
* MCamControllerObserver interface
*
*  @since 2.8
*/
enum TCamControllerEvent
    {
    ECamEventControllerReady, // New request can be issued to CCamAppController

    ECamEventEngineStateChanged,
    ECamEventOperationStateChanged,
    ECamEventSnapshotReady,
    ECamEventCaptureComplete,
    ECamEventRecordComplete,
    ECamEventCounterUpdated,
    ECamEventZoomStateChanged,
    ECamEventFocusCancelled,
    ECamEventFlashStateChanged,
    ECamEventSceneStateChanged,
    ECamEventAudioMuteStateChanged,
    ECamEventContAFStateChanged, 
    ECamEventVideoFileTypeChanged,
    ECamEventImageQualityChanged,
    ECamEventVideoQualityChanged,
    ECamEventSaveLocationChanged,
    ECamEventSaveComplete,
    ECamEventSaveCancelled, 
    ECamEventVideoPauseTimeout,
    ECamEventMediaFileChanged,
    ECamEventExitRequested,
    ECamEventCallStateChanged,
    ECamEventMemoryCardHotswap,
    ECamEventCameraChanged,
    ECamEventSetupStateChanged,
    ECamEventZoomSetupChanged,
    ECamEventMacroSwitchChanged,
    ECamEventSliderOpen,
    ECamEventSliderClosed,
    ECamEventInvalidMemoryCard,
    ECamEventVfOverlayChanged, // Yet only for CAMERAAPP_PRODUCT_SUPPORTS_VF_GRID
	ECamEventUserModeChanged,
    ECamEventHistogramChanged,

	ECamEventLocationSettingChanged,
    ECamEventRestoreCameraSettings,
    ECamEventExposureChanged,
    ECamEventVideoStabChanged,
    ECamEventImageData,
    ECamEventFaceTrackingStateChanged,
    ECamEventInitReady,
    };

// FORWARD DECLARATIONS
class CFbsBitmap;

// CLASS DECLARATION

/**
* Abstract API for viewfinder observer. 
* Derived classes may register as viewfinder observers to get
* related notifications from CCamAppController.
*
*  @since 2.8
*/
//class MCamViewFinderObserver
//    {
//    public:
//        /**
//        * New viewfinder frame is available 
//        * @since 2.8
//        * @param aFrame pointer to the bitmap. This pointer is valid until
//        *               the call returns. Ownership not transferred.
//        * @param aFrozen whether or not the viewfinder has been frozen
//        */
//        virtual void ShowViewFinderFrame( const CFbsBitmap* aFrame, TBool aFrozen ) = 0;
//    };

/**
* Abstract API for controller observer. 
* Derived classes may register as controller observers to
* get related notifications about state changes in CCamAppController and
* engine notifications.
*
*  @since 2.8
*/
class MCamControllerObserver
    {
    public:
        /**
        * Handle an event from CCamAppController.
        * @since 2.8
        * @param aEvent the controller event that has occurred
        * @param aError any reported error
        */
        virtual void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError ) = 0;
    };

#endif      // CAMCONTROLLEROBSERVERS_H
            
// End of File
