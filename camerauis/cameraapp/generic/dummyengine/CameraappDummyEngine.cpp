/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Camera Application Engine implementation still capturing,*
*/



// INCLUDE FILES

#include <CaeEngine.h>
#include "CameraappDummyEngine.h"              // Engine implementation header.
#include "CamLogger.h"
#include "CameraappDummyCallbackActive.h"


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CCaeEngine::NewL
// Two-phased constructor. Calls CCaeEngineImp's NewL().
// -----------------------------------------------------------------------------
//
CCaeEngine* CCameraappDummyEngine::NewL()
    {
    return CCameraappDummyEngineImp::NewL();
    }


// -----------------------------------------------------------------------------
// CCameraappDummyEngineImp::CCameraappDummyEngineImp
// Default constructor.
// -----------------------------------------------------------------------------
//
CCameraappDummyEngineImp::CCameraappDummyEngineImp()
    {
    }

void CCameraappDummyEngineImp::SetCamAppEngineObserver( MCamAppEngineObserver& aObserver )
    {
    iObserver = &aObserver;
    if ( !iCallBackActive )
        {
        TRAPD( ignore, iCallBackActive = CCameraappDummyCallbackActive::NewL( *iObserver ) )
        if ( ignore )
            { 
            // Do nothing ( removes build warning )
            }                     
        }
    }

// -----------------------------------------------------------------------------
// CCameraappDummyEngineImp::~CCameraappDummyEngineImp
// Destructor.
// -----------------------------------------------------------------------------
//
CCameraappDummyEngineImp::~CCameraappDummyEngineImp()
    {
    if ( iCallBackActive )
        {
        iCallBackActive->Cancel();
        delete iCallBackActive;
        }
    }

void CCameraappDummyEngineImp::InitL( TBool /*aCreateSnapImage*/ )
    {
    if ( iCallBackActive )
        {
        iCallBackActive->InitComplete();
        }
    }

void CCameraappDummyEngineImp::Reserve()
    {
    if ( iCallBackActive )
        {
        iCallBackActive->InitComplete();
        }
    }

void CCameraappDummyEngineImp::StartViewFinderBitmapsL( TSize& /*aSize*/ )
    {
    iIsViewFinding = ETrue;
    if ( !iCallBackActive )
        {
        iCallBackActive = CCameraappDummyCallbackActive::NewL( *iObserver );
        }

    if ( iCallBackActive )
        {
        iCallBackActive->SendViewFinderFrames();
        }
    }

void CCameraappDummyEngineImp::StopViewFinder()
    {
    if ( iIsViewFinding )
        {
        iIsViewFinding = EFalse;
        if ( iCallBackActive )
            {
            iCallBackActive->StopViewFinderFrames();
            }
        }
    }

TBool CCameraappDummyEngineImp::IsViewFinding() const
    {
    return iIsViewFinding;
    }

void CCameraappDummyEngineImp::SetCaeStillBurstObserver( MCaeStillBurstObserver& aObserver )
    {
    if ( iCallBackActive )
        {
        iCallBackActive->SetBurstObserver( &aObserver );
        }
    }

void CCameraappDummyEngineImp::StopStillBurstCapture()
    {
    if ( iCallBackActive )
        {
        iCallBackActive->StopBurst();
        }  
    }


TInt CCameraappDummyEngineImp::SetStillCaptureImageCountL( TInt aImageCount )
    {
    iImageCount = aImageCount;
    return KErrNone;
    }

void CCameraappDummyEngineImp::PrepareStillCaptureL(
                                            const TSize& /*aSize*/, 
                                            CCamera::TFormat /*aFormat*/,  
                                            TInt /*aCompressionQuality*/, 
                                            const TRect& /*aCropRect*/,
                                            TSize& /*aSnapSize*/ )
    {
    iVideoPrepared = EFalse;
    if ( iCallBackActive )
        {
        iCallBackActive->StillPrepareComplete();
        }       
    }

void CCameraappDummyEngineImp::CaptureStill()
    {
    if ( iImageCount < 1 || !iCallBackActive )
        {
        return;
        }
    if ( iImageCount == 1 )
        {
        iCallBackActive->CaptureStill();
        }
    else
        {       
        iCallBackActive->CaptureBurst( iImageCount );
        }
    }

void CCameraappDummyEngineImp::StopVideoRecording()
    {
    if ( iCallBackActive )
        {
        iCallBackActive->VideoRecordingStopped();
        }
    }

void CCameraappDummyEngineImp::PauseVideoRecording()
    {
    if ( iCallBackActive )
        {
        iCallBackActive->VideoRecordingPaused();
        }
    }

void CCameraappDummyEngineImp::StartVideoRecording()
    {
    if ( iCallBackActive )
        {
        iCallBackActive->VideoRecordingResumed();
        }    
    }

void CCameraappDummyEngineImp::ResumeVideoRecording()
    {
    if ( iCallBackActive )
        {
        iCallBackActive->VideoRecordingResumed();
        }
    }

void CCameraappDummyEngineImp::SetVideoRecordingFileNameL( 
                                const TDesC& /*aVideoClipFileName*/ )
    {
    if ( iVideoPrepared && iCallBackActive )
        {
        iCallBackActive->VideoPrepareComplete();
        }  
    }

void CCameraappDummyEngineImp::PrepareVideoRecordingL(
                                        const TSize&  /*aFrameSize*/, 
                                        TReal32       /*aFrameRate*/, 
                                        TInt          /*aBitRate*/, 
                                        TBool         /*aAudioEnabled*/,
                                        TInt          /*aAudioBitRate*/, 
                                        const TDesC8& /*aMimeType*/, 
                                        const TDesC&  /*aPreferredSupplier*/, 
                                        const TDesC8& /*aVideoType*/, 
                                        const TDesC8& /*aAudioType*/ )
    {
    iVideoPrepared = ETrue;
    if ( iCallBackActive )
        {
        iCallBackActive->VideoPrepareComplete();
        }  
    }

// -----------------------------------------------------------------------------
// CCameraappDummyEngineImp::ConstructL
// Symbian 2nd phase constructor that can leave.
// -----------------------------------------------------------------------------
//
void CCameraappDummyEngineImp::ConstructL()
    {
    iImageCount = 1;
    }




void CCameraappDummyEngineImp::GetInfo( TCamAppEngineInfo& aInfo ) const
    {
    aInfo.iMaxDigitalZoom = 20;
    aInfo.iFlashModesSupported = CCamera::EFlashNone | 
    							 CCamera::EFlashAuto | 
    							 CCamera::EFlashForced;
    							 
    aInfo.iWhiteBalanceModesSupported = CCamera::EWBAuto |
    									CCamera::EWBDaylight |
    									CCamera::EWBCloudy |
    									CCamera::EWBTungsten |
    									CCamera::EWBFluorescent;
    }


void CCameraappDummyEngineImp::SetZoomValueL( TInt aZoomValue )    
    {
    iZoomValue = aZoomValue;

    TBuf <100> buf;
    buf.Format( _L("__zm %d, val %d__"), iZoomMode, iZoomValue );
    User::InfoPrint( buf );   
    }



TInt CCameraappDummyEngineImp::ZoomValue() const
    { 
    return iZoomValue; 
    }
    
    
void CCameraappDummyEngineImp::SetZoomModeL( CCaeEngine::TZoomMode aZoomMode )    
    {
    iZoomMode = aZoomMode;
    
    TBuf <100> buf;
    buf.Format( _L("__zm %d, val %d__"), iZoomMode, iZoomValue );
    User::InfoPrint( buf );
    }

void CCameraappDummyEngineImp::GetOrderL( RCaeOrderedFeatureList& /*aOrderedFeatureList*/ )
    {
    }
   
void CCameraappDummyEngineImp::SetOrderL( const RCaeOrderedFeatureList& /*aOrderedFeatureList*/ )
    {
    }
   
void CCameraappDummyEngineImp::SetSnapImageSourceL( TSnapImageSource /*aSnapImageSource*/ )
    {
    }
    
void CCameraappDummyEngineImp::SetImageCodecsL( TUid/*aDecoderUid*/, TUid /*aEncoderUid*/ )
    {
    }    

TInt CCameraappDummyEngineImp::CamerasAvailable()
    {
    return 2;
    }


TInt CCameraappDummyEngineImp::EnumerateVideoFrameSizeL( const TSize& /*aSize*/ ) { return 0; };
        
TInt CCameraappDummyEngineImp::CCameraHandle() const { return 1; };

void CCameraappDummyEngineImp::EnableVideoRecording() {};
        
void CCameraappDummyEngineImp::DisableVideoRecording() {};

void CCameraappDummyEngineImp::SetCaeExtensionModeL( TBool aExtModeActive, TBool aCreateSnapImage )
    {
    (void)aExtModeActive; // removes compiler warning
    (void)aCreateSnapImage; // removes compiler warning
    }

void CCameraappDummyEngineImp::ProcessExtViewFinderFrameReadyL( CFbsBitmap& aFrame )
    {
    (void)aFrame; // removes compiler warning
    }

void CCameraappDummyEngineImp::ProcessExtSnapImageL( CFbsBitmap& aSnapImage )
    {
    (void)aSnapImage; // removes compiler warning
    }        

void CCameraappDummyEngineImp::ProcessExtCapturedImageL( CFbsBitmap& aBitmap, TBool aLastImage )
    {
    (void)aBitmap; // removes compiler warning
    (void)aLastImage; // removes compiler warning
    }

void CCameraappDummyEngineImp::ProcessExtCapturedImageL( HBufC8* aImageData, TBool aLastImage )
    {
    (void)aImageData; // removes compiler warning
    (void)aLastImage; // removes compiler warning
    }

void CCameraappDummyEngineImp::ProcessExtCapturedImageL( TDesC8& aImageDataDes, TBool aLastImage )
    {
    (void)aImageDataDes; //removes compiler warning
    (void)aLastImage; // removes compiler warning
    }

void CCameraappDummyEngineImp::ProcessExtCancel()
    {
    }

void CCameraappDummyEngineImp::SkippedExtensionListL( RCaeOrderedFeatureList& aSkippedExtensions )
    {
    (void)aSkippedExtensions; // removes compiler warning
    }

void CCameraappDummyEngineImp::SetSkippedExtensionList( RCaeOrderedFeatureList& aSkippedExtensions )
    {
    (void)aSkippedExtensions; // removes compiler warning
    }

TInt CCameraappDummyEngineImp::SetAsyncVideoStopMode( TBool aAsyncVideoStopEnabled ) 
    {
    (void)aAsyncVideoStopEnabled; // removes compiler warning
    return KErrNone; 
    }

// -----------------------------------------------------------------------------
// CCaeEngineImp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCameraappDummyEngineImp* CCameraappDummyEngineImp::NewL()
    {
    CCameraappDummyEngineImp* self = new( ELeave ) CCameraappDummyEngineImp();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//  End of File  
