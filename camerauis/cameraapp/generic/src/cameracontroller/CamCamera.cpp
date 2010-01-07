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
* Description:  
*                
*
*/




/*
* Refer CCamCamera.h
* Implementing the Interfaces defined in CCamCamera class duplicating the definitions
* for Emulator Ennvironment
*/

#include <e32cmn.h>
#include "CamCamera.h"  // Interface declarations
#include <ecam.h>
#include "CamCamera.hrh" // Constants and Possible Events definitions
#include "camcameracontroller.pan"     // Panic category and codes (use cameracontroller's)
#include "camcameracontrollertypes.h"
#include <AknIconUtils.h>
#include <fbs.h>
#include "CamCameraCallback.h"
#include "CameraappTestBitmapManager.h"


using namespace NCamCameraController;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCamCamera* 
CCamCamera::NewL( MCameraObserver2& aObserver, 
                  TInt              aCameraIndex, 
                  TInt              aPriority )
  {
  CCamCamera* self = new (ELeave) CCamCamera
                                    ( aObserver, 
                                      aCameraIndex, 
                                      aPriority
                                    );
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();
  return self;
  }

// ---------------------------------------------------------------------------
// New2L
// ---------------------------------------------------------------------------
//
CCamCamera* 
CCamCamera::New2L( MCameraObserver2& aObserver,
                   TInt              aCameraIndex,
                   TInt              aPriority )
  {
  return CCamCamera::NewL( aObserver, aCameraIndex, aPriority );
  }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCamCamera* 
CCamCamera::NewL( MCameraObserver& aObserver, 
                  TInt             aCameraIndex )
  {
  CCamCamera* self = new (ELeave) CCamCamera( aObserver, aCameraIndex );
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();
  return self;
  }

// ---------------------------------------------------------------------------
// New2L
// ---------------------------------------------------------------------------
//
CCamCamera* 
CCamCamera::New2L( MCameraObserver& /*aObserver   */,
                   TInt             /*aCameraIndex*/ )
  {
  // Also CCamera implementation rejects New2L + MCameraObserver(1)
  User::Leave( KErrNotSupported );
  return NULL;
  }

// ---------------------------------------------------------------------------
// ~CCamCamera
// ---------------------------------------------------------------------------
//
/*
* CCamCamera::NewDuplicateL
*/
CCamCamera* 
CCamCamera::NewDuplicateL( MCameraObserver2& /*aObserver*/,
                           TInt              /*aCameraHandle*/ )
  {
  User::Leave( KErrNotSupported );
  return NULL;
  }

// ---------------------------------------------------------------------------
// NewDuplicate2L
// ---------------------------------------------------------------------------
//
CCamCamera* 
CCamCamera::NewDuplicate2L( MCameraObserver2& /*aObserver*/,
                            TInt              /*aCameraHandle*/ )
  {
  User::Leave( KErrNotSupported );
  return NULL;
  }


// ---------------------------------------------------------------------------
// NewDuplicateL
// ---------------------------------------------------------------------------
//
CCamCamera* 
CCamCamera::NewDuplicateL( MCameraObserver& /*aObserver*/,
                           TInt             /*aCameraHandle*/ )
  {
  User::Leave( KErrNotSupported );
  return NULL;
  }

// ---------------------------------------------------------------------------
// NewDuplicate2L
// ---------------------------------------------------------------------------
//
CCamCamera* 
CCamCamera::NewDuplicate2L( MCameraObserver& /*aObserver*/,
                            TInt             /*aCameraHandle*/ )
  {
  User::Leave( KErrNotSupported );
  return NULL;
  }

// ---------------------------------------------------------------------------
// ~CCamCamera
// ---------------------------------------------------------------------------
//
CCamCamera::~CCamCamera()
  {
  iPhotoResolutions.Reset();
  delete iCallBackActive;
  delete iBitmapManager;
  if( iVfPeriodic )
    {                
    iVfPeriodic->Cancel();
    delete iVfPeriodic;
    }
  }





// ---------------------------------------------------------------------------
// CamerasAvailable
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::CamerasAvailable()
  {
  return 2;
  }


// ---------------------------------------------------------------------------
// Reserve
// ---------------------------------------------------------------------------
//
void 
CCamCamera::Reserve()
  {
  TInt status = KErrNone;
  if ( ECamIdle == iState )
    {
    iState |= ECamReserved;
    }
  else
    {
    status = KErrInUse;
    }
    
  User::After( KDelayReserve );

  if ( iCallBackActive )
    {
    iCallBackActive->ReserveCompleteRequest( status );
    }
  }


// ---------------------------------------------------------------------------
// PowerOn
// ---------------------------------------------------------------------------
//
void 
CCamCamera::PowerOn()
  {
  TInt status = KErrNone;

  if      ( ECamIdle     == iState ) status = KErrNotReady;
  else if ( ECamReserved != iState ) status = KErrInUse;
  else                               iState |= ECamPowerOn;

  User::After( KDelayPowerOn );

  if ( iCallBackActive )
    {
    iCallBackActive->PowerOnCompleteRequest( status );
    }
  }

// ---------------------------------------------------------------------------
// Release
// ---------------------------------------------------------------------------
//
void 
CCamCamera::Release()
  {
  iState = ECamIdle;
  }
  
// ---------------------------------------------------------------------------
// PowerOff
// ---------------------------------------------------------------------------
//
 void CCamCamera::PowerOff()
  {
  iState = ECamIdle | ECamReserved;

  User::After( KDelayPowerOff );
  }


// ---------------------------------------------------------------------------
// Handle
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::Handle()
  {
  return KCameraHandle;
  };

// ---------------------------------------------------------------------------
// SetZoomFactorL
// ---------------------------------------------------------------------------
//
void  
CCamCamera::SetZoomFactorL( TInt aZoomFactor )
  {
  if( Rng( iInfo.iMinZoom, aZoomFactor, iInfo.iMaxZoom ) )
    {
    iZoomFactor = aZoomFactor;
    }
    else
    {
    User::Leave( KErrNotSupported );
    }
  }

// ---------------------------------------------------------------------------
// ZoomFactor
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::ZoomFactor() const
  {
  return iZoomFactor;
  }

// ---------------------------------------------------------------------------
// SetDigitalZoomFactorL
// ---------------------------------------------------------------------------
//
void  
CCamCamera::SetDigitalZoomFactorL( TInt aDigitalZoomFactor )
  {
  if( Rng( 0, aDigitalZoomFactor, iInfo.iMaxDigitalZoom ) )
    {
    iDigitalZoom = aDigitalZoomFactor;
    }
  else
    {
    User::Leave( KErrNotSupported );
    }
  }

// ---------------------------------------------------------------------------
// DigitalZoomFactor
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::DigitalZoomFactor() const
  {
  return iDigitalZoom;
  }

// ---------------------------------------------------------------------------
// SetContrastL
// ---------------------------------------------------------------------------
//
void  
CCamCamera::SetContrastL( TInt aContrast )
  {
  if( EContrastAuto == aContrast )
    {
    iContrast = EContrastAuto;
    }
  else if( Rng( -100, aContrast, 100 ) )
    {
    iContrast = aContrast;
    }
  else
    {
    User::Leave( KErrNotSupported );  
    }
  }

// ---------------------------------------------------------------------------
// Contrast
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::Contrast() const
  {
  return iContrast;
  }


// ---------------------------------------------------------------------------
// SetBrightnessL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::SetBrightnessL(TInt aBrightness)
  {
  if( EContrastAuto == aBrightness )
    {
    iBrightness = EBrightnessAuto;
    }
  else if( Rng( -100, aBrightness, 100 ) )
    {
    iBrightness = aBrightness;
    }
  else
    {
    User::Leave( KErrNotSupported );  
    }
  }

// ---------------------------------------------------------------------------
// Brightness
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::Brightness() const
  {
  return iBrightness;
  }

// ---------------------------------------------------------------------------
// SetFlashL
// ---------------------------------------------------------------------------
//
void
CCamCamera::SetFlashL( TFlash aFlash )
  {
  if( CCamCamera::EFlashNone == aFlash // Zero, so need special attention
   || KFlashModeSupport      &  aFlash ) iFlash = aFlash;
  else                                   User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// Flash
// ---------------------------------------------------------------------------
//
CCamCamera::TFlash 
CCamCamera::Flash() const
  {
  return iFlash;
  }

// ---------------------------------------------------------------------------
// SetExposureL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::SetExposureL(TExposure aExposure )
  {
  if( CCamCamera::EExposureAuto == aExposure 
   || KExposureSupport          &  aExposure ) iExposure = aExposure;
  else                                         User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// Exposure
// ---------------------------------------------------------------------------
//
CCamCamera::TExposure 
CCamCamera::Exposure() const
  {
  return iExposure;
  }

// ---------------------------------------------------------------------------
// SetWhiteBalanceL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::SetWhiteBalanceL( TWhiteBalance aWhiteBalance )
  {
  if( CCamCamera::EWBAuto == aWhiteBalance
   || KWBSupport          &  aWhiteBalance ) iWhiteBalance = aWhiteBalance;
  else                                       User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// WhiteBalance
// ---------------------------------------------------------------------------
//
CCamCamera::TWhiteBalance 
CCamCamera::WhiteBalance() const
  {
  return iWhiteBalance;
  }


// ---------------------------------------------------------------------------
// StartViewFinderDirectL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StartViewFinderDirectL( RWsSession& /*aWs*/,
                                   CWsScreenDevice& /*aScreenDevice*/,
                                   RWindowBase& /*aWindow*/,
                                   TRect& /*aScreenRect*/ )
  {
  User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// StartViewFinderDirectL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StartViewFinderDirectL( RWsSession& /*aWs*/,
                                    CWsScreenDevice& /*aScreenDevice*/,
                                    RWindowBase& /*aWindow*/,
                                    TRect& /*aScreenRect*/,
                                    TRect& /*aClipRect*/ )
  {
  User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// StartViewFinderBitmapsL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StartViewFinderBitmapsL( TSize& aSize )
  {  
  iViewfinderSize = aSize;

  iVfPeriodic->Start( KVfFrameInterval, 
                      KVfFrameInterval, 
                      TCallBack( ViewfinderCallBack, this ) 
                     );

  iState |= ECamVFOn;
  }

// ---------------------------------------------------------------------------
// StartViewFinderBitmapsL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StartViewFinderBitmapsL( TSize& aSize,
                                     TRect& aClipRect )
  {
  StartViewFinderBitmapsL( aSize );
  aSize     = iViewfinderSize;
  aClipRect = TRect( iViewfinderSize );  
  }

// ---------------------------------------------------------------------------
// StartViewFinderL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StartViewFinderL( TFormat /*aImageFormat*/,
                              TSize&  /*aSize*/ )
  {
  User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// StartViewFinderL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StartViewFinderL( TFormat /*aImageFormat*/,
                              TSize&  /*aSize*/,
                              TRect&  /*aClipRect*/ )
  {
  User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// StopViewFinder
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StopViewFinder()
  {
  // Stop the viewfinder frames
  iVfPeriodic->Cancel();

  iState &= ~ECamVFOn;
  }

// ---------------------------------------------------------------------------
// ViewFinderActive
// ---------------------------------------------------------------------------
//
TBool 
CCamCamera::ViewFinderActive() const
  {
  return ( iState & ECamVFOn );
  }

// ---------------------------------------------------------------------------
// SetViewFinderMirrorL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::SetViewFinderMirrorL( TBool aMirror )
  {
  iViewFinderMirror = aMirror;
  }

// ---------------------------------------------------------------------------
// ViewFinderMirror
// ---------------------------------------------------------------------------
//
TBool 
CCamCamera::ViewFinderMirror() const
  {
  return iViewFinderMirror;
  }

// ---------------------------------------------------------------------------
// PrepareImageCaptureL
// ---------------------------------------------------------------------------
//
 void 
CCamCamera::PrepareImageCaptureL( TFormat aImageFormat, TInt aSizeIndex )
  {
  TRect temp;
  PrepareImageCaptureL( aImageFormat, aSizeIndex, temp );
  }

// ---------------------------------------------------------------------------
// PrepareImageCaptureL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::PrepareImageCaptureL( TFormat      aImageFormat, 
                                  TInt         aSizeIndex, 
                                  const TRect& /*aClipRect*/ )
  {
  if( ECamPowerOn & iState )
    {
    TSize size;
    EnumerateCaptureSizes( size, aSizeIndex, aImageFormat );
    iState |= ECamImageOn;
    }
  else
    {
    User::Leave( KErrNotReady );
    }
  }

// ---------------------------------------------------------------------------
// CaptureImage
// ---------------------------------------------------------------------------
//
void 
CCamCamera::CaptureImage()
  {
  if ( iCallBackActive )
    {
    iCallBackActive->RequestCaptureImage();
    }
  }

// ---------------------------------------------------------------------------
// CancelCaptureImage
// ---------------------------------------------------------------------------
//
void 
CCamCamera::CancelCaptureImage()
  {
  
  // In Wins we dont exactly start any async method
  // to capture the image. 
  }

// ---------------------------------------------------------------------------
// EnumerateCaptureSizes
// ---------------------------------------------------------------------------
///
void 
CCamCamera::EnumerateCaptureSizes( TSize&  aSize,
                                   TInt    aSizeIndex,
                                   TFormat aFormat ) const
  {
  switch( aFormat )
    {
    // -----------------------------------------------------
    case EFormatJpeg:
      //
      // switch( aSizeIndex )
      //  {
      aSize = iPhotoResolutions[ aSizeIndex ];                
      // case 0:  aSize = KImageSizeJpegCam1; break;
      // default: aSize = TSize();        break;       
      //  }
      break;
    // -----------------------------------------------------
    case EFormatExif:      
      // switch( aSizeIndex )
      //  {      
      aSize = iPhotoResolutions[ aSizeIndex ];        
      //  case 0:  aSize = KImageSizeExifCam1; break;
      //  default: aSize = TSize();        break;       
      //   }
      break;
    // -----------------------------------------------------
    default:
      aSize = TSize();
      break;
    // -----------------------------------------------------
    }
  }

// ---------------------------------------------------------------------------
// PrepareVideoCaptureL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::PrepareVideoCaptureL( TFormat /*aFormat*/,
                                  TInt /*aSizeIndex*/,
                                  TInt /*aRateIndex*/,
                                  TInt /*aBuffersToUse*/,
                                  TInt /*aFramesPerBuffer*/ )
  {
  User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// PrepareVideoCaptureL
// ---------------------------------------------------------------------------
//
void 
CCamCamera::PrepareVideoCaptureL( TFormat /*aFormat*/,
                                  TInt /*aSizeIndex*/,
                                  TInt /*aRateIndex*/,
                                  TInt /*aBuffersToUse*/,
                                  TInt /*aFramesPerBuffer*/,
                                  const TRect& /*aClipRect*/ )
  {
  User::Leave( KErrNotSupported );
  }

// ---------------------------------------------------------------------------
// StartVideoCapture
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StartVideoCapture()
  {
  Panic( ECamCameraControllerUnsupported );
  }

// ---------------------------------------------------------------------------
// StopVideoCapture
// ---------------------------------------------------------------------------
//
void 
CCamCamera::StopVideoCapture()
  {
  Panic( ECamCameraControllerUnsupported );
  }

// ---------------------------------------------------------------------------
// VideoCaptureActive
// ---------------------------------------------------------------------------
//
TBool 
CCamCamera::VideoCaptureActive() const
  {
  return EFalse;
  }

// ---------------------------------------------------------------------------
// EnumerateVideoFrameSizes
// ---------------------------------------------------------------------------
//
void 
CCamCamera::EnumerateVideoFrameSizes( TSize& /*aSize*/,
                                      TInt /*aSizeIndex*/,
                                      TFormat /*aFormat*/ ) const
  {
  // Video not supported
  Panic( ECamCameraControllerUnsupported );
  }

// ---------------------------------------------------------------------------
// EnumerateVideoFrameRates
// ---------------------------------------------------------------------------
//
void 
CCamCamera::EnumerateVideoFrameRates(
            TReal32& /*aRate*/,
            TInt /*aRateIndex*/,
            TFormat /*aFormat*/,
            TInt /*aSizeIndex*/,
            CCamCamera::TExposure /*aExposure*/ ) const
  {
  // Video not supported
  Panic( ECamCameraControllerUnsupported );
  }


// ---------------------------------------------------------------------------
// GetFrameSize
// ---------------------------------------------------------------------------
//
void 
CCamCamera::GetFrameSize( TSize& /*aSize*/ ) const
  {
  // Video not supported
  Panic( ECamCameraControllerUnsupported );
  }

// ---------------------------------------------------------------------------
// FrameRate
// ---------------------------------------------------------------------------
//
TReal32 
CCamCamera::FrameRate() const
  {
  // Video not supported
  Panic( ECamCameraControllerUnsupported );

  return 0;
  }

// ---------------------------------------------------------------------------
// BuffersInUse
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::BuffersInUse() const
  {
  // Not sure
  return 1;
  } 

// ---------------------------------------------------------------------------
// FramesPerBuffer
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::FramesPerBuffer() const
  {
  return 1;
  }

// ---------------------------------------------------------------------------
// SetJpegQuality
// ---------------------------------------------------------------------------
//
void 
CCamCamera::SetJpegQuality( TInt aQuality )
  {
  iQuality = Min( 100, Max( 0, aQuality ) );
  }

// ---------------------------------------------------------------------------
// JpegQuality
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::JpegQuality() const
  {
  return iQuality;
  }

// ---------------------------------------------------------------------------
// CustomInterface
// ---------------------------------------------------------------------------
//
TAny* 
CCamCamera::CustomInterface( TUid /*aInterface*/ )
  {
  return NULL;
  }

// ---------------------------------------------------------------------------
// CameraInfo
// ---------------------------------------------------------------------------
//
void  
CCamCamera::CameraInfo( TCameraInfo& aInfo ) const
  {
	aInfo.iHardwareVersion             = TVersion( 0, 0, 2007 ); /** Version number and name of camera hardware. */
	aInfo.iSoftwareVersion             = TVersion( 0, 0, 2007 ); /** Version number and name of camera software (device driver). */
	aInfo.iOrientation                 = TCameraInfo::EOrientationUnknown;
    aInfo.iOptionsSupported            = KOptionsSupport;
	aInfo.iFlashModesSupported         = KFlashModeSupport;
	aInfo.iExposureModesSupported      = KExposureSupport;
	aInfo.iWhiteBalanceModesSupported  = KWBSupport;
	aInfo.iMinZoom                     = 0;
	aInfo.iMaxZoom                     = 100;
	aInfo.iMaxDigitalZoom              = 100;
	aInfo.iMinZoomFactor               = 1.0;
	aInfo.iMaxZoomFactor               = 10.0;
	aInfo.iMaxDigitalZoomFactor        = 10.0;
	// aInfo.iNumImageSizesSupported      = 1;	
	aInfo.iNumImageSizesSupported      = iPhotoResolutions.Count();
	aInfo.iImageFormatsSupported       = (iCameraIndex == 0)? KImageFormatSupportCAM1 : KImageFormatSupportCAM2;;
	aInfo.iNumVideoFrameSizesSupported = 1;
	aInfo.iNumVideoFrameRatesSupported = 1;
	aInfo.iVideoFrameFormatsSupported  = (iCameraIndex == 0)? KVideoFormatSupportCAM1 : KVideoFormatSupportCAM2;;
	aInfo.iMaxFramesPerBufferSupported = 1;
	aInfo.iMaxBuffersSupported         = 2;
  }


// ---------------------------------------------------------------------------
// InitPhotoResolutionsL
// ---------------------------------------------------------------------------
//
void CCamCamera::InitPhotoResolutionsL()
    {
    iPhotoResolutions.Append( TSize(  640,  480 ) ); // VGA
    iPhotoResolutions.Append( TSize(  800,  600 ) ); // SVGA
    iPhotoResolutions.Append( TSize( 1024,  768 ) ); // XGA
    iPhotoResolutions.Append( TSize( 1152,  864 ) ); // 1 MP
    iPhotoResolutions.Append( TSize( 1280,  960 ) ); // 1.3 MP
    iPhotoResolutions.Append( TSize( 1600, 1200 ) ); // 2 MP
    iPhotoResolutions.Append( TSize( 2048, 1536 ) ); // 3 MP
    iPhotoResolutions.Append( TSize( 2592, 1944 ) ); // 5 MP
    iPhotoResolutions.Append( TSize( 3264, 2448 ) ); // 8 MP
    iPhotoResolutions.Append( TSize( 4000, 3000 ) ); // 12 MP
    iPhotoResolutions.Append( TSize( 3264, 1832 ) ); // 6 MP 16:9
    iPhotoResolutions.Append( TSize( 4000, 2248 ) ); // 9 MP 16:9
    iPhotoResolutions.Append( TSize(  354,  288 ) ); // CIF
    iPhotoResolutions.Append( TSize(  320,  240 ) ); // QVGA
    }


// ---------------------------------------------------------------------------
// ViewfinderCallBack
// ---------------------------------------------------------------------------
//
TInt 
CCamCamera::ViewfinderCallBack( TAny* aSelf )
  {
  CCamCamera* self = static_cast<CCamCamera*>( aSelf );
  if( self && self->iObserver )
    {
    self->iObserver->ViewFinderFrameReady( self->iBitmapManager->NextBitmap() );         
    }
  return KErrNone;    
  }
    
// ===========================================================================
// Constructors


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamCamera::ConstructL()
  {
  InitPhotoResolutionsL();
  iBitmapManager  = CCameraappTestBitmapManager::NewL();
  iVfPeriodic     = CPeriodic::NewL( EPriorityLow );
  iCallBackActive = CCamCameraCallBack::NewL( *iObserver );

  CameraInfo( iInfo );
  }


// ---------------------------------------------------------------------------
// 1st phase constructor
// CCamCamera::CCamCamera
// ---------------------------------------------------------------------------
//
CCamCamera::CCamCamera( MCameraObserver& aObserver, TInt aCameraIndex )
  : iObserver   ( &aObserver   ),
    iState      ( ECamIdle     ),
    iCameraIndex( aCameraIndex )
  {
  }

// ---------------------------------------------------------------------------
// CCamCamera::CCamCamera
//
// ---------------------------------------------------------------------------
//
CCamCamera::CCamCamera( MCameraObserver2& aObserver,TInt aCameraIndex, TInt aPriority )
  : iObserver2  ( &aObserver   ),
    iState      ( ECamIdle     ),
    iCameraIndex( aCameraIndex ),
    iPriority   ( aPriority    )
  {
  }


void CCamCamera::Construct2L(MCameraObserver& /*aObserver*/, TInt /*aCameraIndex*/)
    {
    // not implemented
    }

void CCamCamera::Construct2DupL(MCameraObserver& /*aObserver*/, TInt /*aCameraHandle*/)
    {
    // not implemented
    }

void CCamCamera::Construct2L(MCameraObserver2& /*aObserver*/, TInt /*aCameraIndex*/, TInt /*aPriority*/)
    {
    // not implemented
    }

void CCamCamera::Construct2DupL(MCameraObserver2& /*aObserver*/, TInt /*aCameraHandle*/)
    {
    // not implemented
    }


// end of file


