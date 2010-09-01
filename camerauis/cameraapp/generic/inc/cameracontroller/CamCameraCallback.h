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
* Description:  Emulator version of CCamera*
*/



#ifndef CAMCAMERACALLBACK_H
#define CAMCAMERACALLBACK_H

//  INCLUDES
#include <e32base.h>
#include <fbs.h>

class CCameraappTestBitmapManager; 
class MCameraObserver;


class CCamCameraCallBack : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCamCameraCallBack* NewL( MCameraObserver& aController );
        
        /**
        * Destructor.
        */
        virtual ~CCamCameraCallBack();

    public: // Functions from CActive

        // void Cancel();

    public: // New Methods

        /**
        * Complete reserve request.
        */
        void ReserveCompleteRequest( const TInt aError );
        
        /**
        * Complete power-on request.
        */
        void PowerOnCompleteRequest( const TInt aError );
        
        /**
        * Prepare for Still capture
        */
        void StillPrepareComplete();

        /**
        * Initiate Still Image Capture
        */
        void SendViewFinderFrames();
        
        /**
        * Stop Sending View Finder Frames
        */
        void StopViewFinderFrames();
        
        /**
        * Capture Image and Call MCameraObserver::ImageReady .
        */
        void RequestCaptureImage();

    private: // New Methods

        /*
        * Constructor
        */ 
        CCamCameraCallBack( MCameraObserver& aController );
        
        // 2nd phase constructor
        void ConstructL( );
        
        // We call SetActive and check the handling of all requests
        void SetActiveAndCompleteRequest();
        
        // Complete Reserve request by calling MCameraObserver::ReserveComplete
        void DoReserveComplete();

        // Complete Reserve request by calling MCameraObserver::PowerOnComplete
        void DoPowerOnComplete();
        
        void DoStillPrepareComplete();

        // Transfer view finder frames request by calling MCameraObserver::ViewFinderFrameReady
        void DoViewFinderFrame();

        // Complete Reserve request by calling MCameraObserver::ImageReady
        void DoCaptureImageL();
        
        void Reset();
        

        /**
        * Cancels the active object
        */
        void DoCancel();
        
        /*
        * All requests are handled one by one in this method 
        */
        void RunL();

    private:
        MCameraObserver& iController;
        
        CCameraappTestBitmapManager* iBitmapManager;
        // Request complete status flags
        TBool iStillPrepareComplete;
        TBool iReserveComplete;
        TBool iPowerOnComplete;
        TBool iCaptureImage;
        TBool iViewFinderFrame;
        
        // Errors caused during request
        TInt iReserveError;
        TInt iPowerOnError; 
    };

#endif      // CAMCAMERACALLBACKACTIVE_H
            
