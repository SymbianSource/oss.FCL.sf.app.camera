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



// INCLUDE FILES
#include "CameraappDummyCallbackActive.h"
#include "CameraappTestBitmapManager.h"
#include <CaeEngine.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCameraappDummyCallbackActive::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCameraappDummyCallbackActive* CCameraappDummyCallbackActive::NewL( 
                            MCamAppEngineObserver& aController )
    {
    CCameraappDummyCallbackActive* self = new( ELeave ) CCameraappDummyCallbackActive( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CCameraappDummyCallbackActive::~CCameraappDummyCallbackActive()
    {  
    delete iBitmapManager;
    }

void CCameraappDummyCallbackActive::Cancel()
    {
    CActive::Cancel();
    DoCancel();
    }

void CCameraappDummyCallbackActive::SetBurstObserver( MCaeStillBurstObserver* aBurstObserver )
    {
    iBurstObserver = aBurstObserver;
    }

CCameraappDummyCallbackActive::CCameraappDummyCallbackActive( 
                            MCamAppEngineObserver& aController )
    : CActive( EPriorityLow ),
      iController( aController )
    {
    }

void CCameraappDummyCallbackActive::InitComplete( )
    {
    iInitComplete = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();    
        }    
    }

void CCameraappDummyCallbackActive::StillPrepareComplete( )
    {
    iStillPrepareComplete = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

void CCameraappDummyCallbackActive::VideoPrepareComplete( )
    {
    iVideoPrepareComplete = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

void CCameraappDummyCallbackActive::SendViewFinderFrames( )
    {
    iViewFinderFrame = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }
    
void CCameraappDummyCallbackActive::StopViewFinderFrames()
	{
	iViewFinderFrame = EFalse;
	}

void CCameraappDummyCallbackActive::CaptureStill()
    {
    iCaptureStill = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

void CCameraappDummyCallbackActive::CaptureBurst( TInt aImageCount )
    {
    iStopBurst = EFalse;
    iCaptureBurst = ETrue;
    iBurstCount = aImageCount;
    iBurstCaptureImage = 0;
    iBurstSnapImage = 0;
    iCompletedBurst = 0;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

void CCameraappDummyCallbackActive::StopBurst()
    {
    iStopBurst = ETrue;
    }

void CCameraappDummyCallbackActive::VideoRecordingStopped()
    {
    iVideoStopped = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

void CCameraappDummyCallbackActive::VideoRecordingPaused()
    {
    iVideoPaused = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

void CCameraappDummyCallbackActive::VideoRecordingResumed()
    {
    iVideoResumed = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

void CCameraappDummyCallbackActive::SetActiveAndCompleteRequest()
    {
    SetActive();
    TRequestStatus* statusPtr = &iStatus;
    User::RequestComplete( statusPtr, KErrNone );
    }

void CCameraappDummyCallbackActive::DoInitComplete( )
    {
    iController.McaeoInitComplete( KErrNone );
    iInitComplete = EFalse;
    }

// -----------------------------------------------------------------------------
// CCameraappDummyCallbackActive::StillPrepareComplete
// Prepare CCamAppControllerBase and CamAppEngine for current mode
// -----------------------------------------------------------------------------
//
void CCameraappDummyCallbackActive::DoStillPrepareComplete()
    {
    iController.McaeoStillPrepareComplete( KErrNone );
    iStillPrepareComplete = EFalse;
    }

// -----------------------------------------------------------------------------
// CCameraappDummyCallbackActive::VideoPrepareComplete
// Prepare CCamAppControllerBase and CamAppEngine for current mode
// -----------------------------------------------------------------------------
//
void CCameraappDummyCallbackActive::DoVideoPrepareComplete()
    {
    iController.McaeoVideoPrepareComplete( KErrNone );

    TTimeIntervalMicroSeconds timeElapsed( 1 );
    TTimeIntervalMicroSeconds timeRem( 100000000 );
    iController.McaeoVideoRecordingTimes( timeElapsed, timeRem, KErrNone );
    iVideoPrepareComplete = EFalse;
    }

// -----------------------------------------------------------------------------
// CCameraappDummyCallbackActive::ViewFinderFrame
// Start the viewfinder
// -----------------------------------------------------------------------------
//
void CCameraappDummyCallbackActive::DoViewFinderFrame()
    {
    iController.McaeoViewFinderFrameReady( iBitmapManager->NextBitmap(), KErrNone ); 
    if ( iViewFinderFrame )
        {
        if ( !IsActive() )
            {
            SetActiveAndCompleteRequest();
            }
        }
    }

void CCameraappDummyCallbackActive::DoCaptureStillL()
    {
    iCaptureStill = EFalse;
    iController.McaeoSnapImageReady( iBitmapManager->Bitmap( 2 ), KErrNone );
    HBufC8* testData = HBufC8::NewL( KTestData( ).Length() );
    testData->Des().Copy( KTestData );
    // ownership of testData transferred to iController
    iController.McaeoStillImageReady( NULL, testData, KErrNone );
    }

void CCameraappDummyCallbackActive::DoCaptureBurstL()
    {
    if ( !iBurstObserver )
        {
        return;
        }   
    // if we are still taking burst images
    if ( iBurstCaptureImage < iBurstCount )
        {   
        if ( iStopBurst )
            {
            iBurstCount = iBurstCaptureImage;
            }
        else
            {
            iBurstObserver->McaesboStillBurstCaptureMoment( KErrNone );
            if (!iStopBurst )
                {
                iBurstCaptureImage++;
                }
            }
        if ( !IsActive() )
            {
            SetActiveAndCompleteRequest();
            }
        }
    // if we are sending back snap images
    else if ( iBurstSnapImage < iBurstCount )
        {
        iBurstSnapImage++;
        iController.McaeoSnapImageReady( iBitmapManager->Bitmap( 2 ), KErrNone );
        if ( !IsActive() )
            {
            SetActiveAndCompleteRequest();
            }
        }
    // if we are sending back still images
    else if ( iCompletedBurst < iBurstCount )
        {
        iCompletedBurst++;
        // ownership of testData transferred to iController
        HBufC8* testData = HBufC8::NewL( KTestData( ).Length() );
        testData->Des().Copy( KTestData );
        iController.McaeoStillImageReady( NULL, testData, KErrNone );
        if ( !IsActive() )
            {
            SetActiveAndCompleteRequest();
            }
        }
    else // all finished
        {
        iCaptureBurst = EFalse;
        iBurstObserver->McaesboStillBurstComplete( iBurstCount, KErrNone );
        iBurstCount = 0;
        iCompletedBurst = 0;
        iBurstSnapImage = 0;
        iBurstCaptureImage = 0;
        }    
    }

void CCameraappDummyCallbackActive::DoVideoPaused()
    {
    iVideoPaused = EFalse;
    iController.McaeoVideoRecordingPaused( KErrNone );
    }

void CCameraappDummyCallbackActive::DoVideoResumed()
    {
    iVideoResumed = EFalse;
    iController.McaeoVideoRecordingOn( KErrNone );
    }

void CCameraappDummyCallbackActive::DoVideoStopped()
    {
    iVideoStopped = EFalse;
    iController.McaeoVideoRecordingComplete( KErrNone );
    }

void CCameraappDummyCallbackActive::ConstructL()
    {
    CActiveScheduler::Add( this );
    iBitmapManager = CCameraappTestBitmapManager::NewL();
    }

// -----------------------------------------------------------------------------
// CCameraappDummyCallbackActive::DoCancel
// Cancels the active object
// -----------------------------------------------------------------------------
//
void CCameraappDummyCallbackActive::DoCancel()
    {
    iInitComplete = EFalse;
    iStillPrepareComplete = EFalse;
    iVideoPrepareComplete = EFalse;
    iViewFinderFrame = EFalse;
    iVideoPaused = EFalse;
    iVideoResumed = EFalse;
    }

void CCameraappDummyCallbackActive::RunL()
    {
    if ( iInitComplete )
        {
        DoInitComplete();
        }
    else if ( iStillPrepareComplete )
        {
        DoStillPrepareComplete();
        }
    else if ( iVideoPrepareComplete )
        {
        DoVideoPrepareComplete();
        }
    else if ( iCaptureStill )
        {
        DoCaptureStillL();
        }
    else if ( iCaptureBurst )
        {
        DoCaptureBurstL();
        }
    else if ( iVideoPaused )
        {
        DoVideoPaused();
        }
    else if ( iVideoResumed )
        {
        DoVideoResumed();
        }
    else if ( iVideoStopped )
        {
        DoVideoStopped();
        }
    else
        {
        }
        
   	if ( iViewFinderFrame )
        {
        DoViewFinderFrame();
        }
    }
//  End of File  
