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
* Description:  Emualtor support for CCamCameraController part of cameraapp*
*/



#include "CamCameraCallback.h"
#include "CameraappTestBitmapManager.h"
#include <ecam.h>
#include <AknIconUtils.h>
#include <fbs.h>

_LIT(KImageFile, "c:\\private\\101F857A\\capture.jpg");


/*
* CCamCameraCallBack* CCamCameraCallBack::NewL
*/
CCamCameraCallBack* CCamCameraCallBack::NewL( 
                            MCameraObserver& aController )
    {
    CCamCameraCallBack* self = new( ELeave ) CCamCameraCallBack( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

/*
* CCamCameraCallBack::CCamCameraCallBack
*/
CCamCameraCallBack::CCamCameraCallBack( 
                            MCameraObserver& aController )
    : CActive( EPriorityLow ),
      iController( aController )
    {
    }

/*
* CCamCameraCallBack::ConstructL()
*/    
void CCamCameraCallBack::ConstructL()
    {
    CActiveScheduler::Add( this );
    iBitmapManager = CCameraappTestBitmapManager::NewL();
    }

/*
* CCamCameraCallBack::DoCancel
*/
void CCamCameraCallBack::DoCancel()
    {
    // Status update flags
    iViewFinderFrame = EFalse;
    iStillPrepareComplete = EFalse;
    iReserveComplete = EFalse;
    iPowerOnComplete = EFalse;
    iCaptureImage = EFalse;
    
    // Error status associated with each request
    iReserveError = KErrNone;
    iPowerOnError = KErrNone;
    }

/*
* CCamCameraCallBack::~CCamCameraCallBack
*/
CCamCameraCallBack::~CCamCameraCallBack()
    {  
    delete iBitmapManager;
    Cancel();
    }

/*
* CCamCameraCallBack::Cancel
*/
/*
void CCamCameraCallBack::Cancel()
    {
    CActive::Cancel();
    DoCancel();
    }
*/

/*
* CCamCameraCallBack::RunL
*/
void CCamCameraCallBack::RunL()
    {
    if ( iStillPrepareComplete )
        {
        DoStillPrepareComplete();
        }
    else if ( iReserveComplete )
        {
        DoReserveComplete();
        }
    else if ( iPowerOnComplete )
        {
        DoPowerOnComplete();
        }
    else if ( iCaptureImage )
        {
        DoCaptureImageL();
        }
    else
        {
        }
    if ( iViewFinderFrame )
        {
        DoViewFinderFrame();
        }
    }

/*
* CCamCameraCallBack::SetActiveAndCompleteRequest
*/
void CCamCameraCallBack::SetActiveAndCompleteRequest()
    {
    SetActive();
    TRequestStatus* statusPtr = &iStatus;
    User::RequestComplete( statusPtr, KErrNone );
    }


/*
* CCamCameraCallBack::ReserveCompleteRequest
*/
void 
CCamCameraCallBack::ReserveCompleteRequest( const TInt aError )
    {
    iReserveError = aError;
    iReserveComplete = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();    
        }    
    }

/*
* CCamCameraCallBack::PowerOnCompleteRequest
*/
void 
CCamCameraCallBack::PowerOnCompleteRequest( const TInt aError )
    {
    iPowerOnError = aError;
    iPowerOnComplete = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();    
        }    
    }
    

/*
* CCamCameraCallBack::StillPrepareComplete
*/
void CCamCameraCallBack::StillPrepareComplete()
    {
    iStillPrepareComplete = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

/*
* CCamCameraCallBack::SendViewFinderFrames
*/ 
void CCamCameraCallBack::SendViewFinderFrames()
    {
    iViewFinderFrame = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

/*
* CCamCameraCallBack::StopViewFinderFrames
*/    
void CCamCameraCallBack::StopViewFinderFrames()
	{
	iViewFinderFrame = EFalse;
	}

/*
* CCamCameraCallBack::RequestCaptureImage
*/
void CCamCameraCallBack::RequestCaptureImage()
    {
    iCaptureImage = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

/*
* CCamCameraCallBack::DoReserveComplete
*/
void CCamCameraCallBack::DoReserveComplete()
    {
    iController.ReserveComplete( iReserveError );
    iReserveComplete = EFalse;
    }

/*
* CCamCameraCallBack::DoPowerOnComplete
*/
void CCamCameraCallBack::DoPowerOnComplete()
    {
    iController.PowerOnComplete( iPowerOnError );
    iPowerOnComplete = EFalse;
    }


/*
* CCamCameraCallBack::DoStillPrepareComplete
*/
void CCamCameraCallBack::DoStillPrepareComplete()
    {
    iStillPrepareComplete = EFalse;
    }


/*
* CCamCameraCallBack::DoViewFinderFrame
*/
void CCamCameraCallBack::DoViewFinderFrame()
    {
    iController.ViewFinderFrameReady( iBitmapManager->NextBitmap() ); 
    
    if ( iViewFinderFrame )
        {
        if ( !IsActive() )
            {
            SetActiveAndCompleteRequest();
            }
        }
    
    }

/*
* CCamCameraCallBack::DoCaptureImageL
*/
void CCamCameraCallBack::DoCaptureImageL()
    {
    iCaptureImage = EFalse;
    TInt imageIndex = 0;
    
    // Connect to fs
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    // Open file and get size
    RFile file;
    TInt size = 0;
    User::LeaveIfError( file.Open( fs, KImageFile, EFileRead ) );
    CleanupClosePushL( file );    
    User::LeaveIfError( file.Size( size ) );
    
    // Allocate memory
    HBufC8* data = HBufC8::NewL( size );
    TPtr8 dataDes = data->Des();    
    
    
    // Read file content    
    TInt read = file.Read( dataDes, size );
    
    // Clean up
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PopAndDestroy( &fs );
    
    // Give data to controller
    iController.ImageReady( NULL, data, KErrNone );       
    }


//  End of File  
