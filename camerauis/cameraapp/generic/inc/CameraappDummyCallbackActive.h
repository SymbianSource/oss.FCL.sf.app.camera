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
* Description:  Dummy callback active object class for Cameraapp Unit Tests*
*/




#ifndef CAMERAAPPDUMMYCALLBACKACTIVE_H
#define CAMERAAPPDUMMYCALLBACKACTIVE_H

//  INCLUDES
#include <e32base.h>
// CONSTANTS

// MACROS
//#define ?macro ?macro_def

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CCameraappTestBitmapManager; 
class MCamAppEngineObserver;
class MCaeStillBurstObserver;

/**
* Handle intercommunication between camcorder UI and engine.
*
*  @since 2.8
*/
class CCameraappDummyCallbackActive : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCameraappDummyCallbackActive* NewL( MCamAppEngineObserver& aController );
        
        /**
        * Destructor.
        */
        virtual ~CCameraappDummyCallbackActive();

    public: // Functions from CActive

        void Cancel();

    public:

        void SetBurstObserver( MCaeStillBurstObserver* aBurstObserver );
        
        /**
        * Prepare CCamAppControllerBase and CamAppEngine for current mode
        */
        void InitComplete();

        /**
        * Prepare for Still capture
        */
        void StillPrepareComplete();

        /**
        * Prepare for video capture
        */
        void VideoPrepareComplete();

        /**
        * Initiate Still Image Capture
        */
        void SendViewFinderFrames();
        
        void StopViewFinderFrames();

        void CaptureStill();

        void CaptureBurst( TInt aImageCount );

        void StopBurst();

        void VideoRecordingStopped();

        void VideoRecordingPaused();

        void VideoRecordingResumed();

    private:

        CCameraappDummyCallbackActive( MCamAppEngineObserver& aController );

        void ConstructL( );

        void SetActiveAndCompleteRequest();

        void DoInitComplete();

        void DoStillPrepareComplete();

        void DoVideoPrepareComplete();

        void DoViewFinderFrame();

        void DoCaptureStillL();

        void DoCaptureBurstL();

        void DoVideoPaused();
        
        void DoVideoResumed();

        void DoVideoStopped();

        /**
        * Cancels the active object
        */
        void DoCancel();

        void RunL();

    private:
        MCamAppEngineObserver& iController;
        MCaeStillBurstObserver* iBurstObserver;
        CCameraappTestBitmapManager* iBitmapManager;
        TBool iInitComplete;
        TBool iStillPrepareComplete;
        TBool iVideoPrepareComplete;
        TBool iViewFinderFrame;
        TBool iCaptureStill;
        TBool iCaptureBurst;
        TBool iVideoPaused;
        TBool iVideoResumed;
        TBool iVideoStopped;
        TInt iBurstCount;
        TBool iInitFail;
        TBool iStopBurst;
        TInt iBurstCaptureImage;
        TInt iBurstSnapImage;
        TInt iCompletedBurst;
    };

#endif      // CAMERAAPPDUMMYCALLBACKACTIVE_H
            
// End of File
