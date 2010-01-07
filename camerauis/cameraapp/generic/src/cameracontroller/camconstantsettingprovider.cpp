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
* Description:  CCamCameraControllerActive class implementation.
*                
*
*/

#include "camcameracontrollerflags.hrh"

#include <ecam.h>
#ifdef CAMERAAPP_CAPI_V2
  #include <ecamadvsettings.h> // KECamFineResolutionFactor
#endif

#include "camlogging.h"
#include "camcameracontroller.pan"
#include "camcameracontrollertypes.h"
#include "camconstantsettingprovider.h"
#include "camsettingconversion.h"
#include "CamAppController.h"
#include "campointerutility.inl"

using namespace NCamCameraController;

// ===========================================================================
// Local data

static const CCamera::TFormat       KImageFormat            = CCamera::EFormatExif;
static const TSize                  KImageSize              = TSize(1600,1200); // 2MPix, supported in most products
static const TInt                   KImageFactor            = 100;

static const CCamera::TFormat       KImageFormatSecondary   = CCamera::EFormatFbsBitmapColor16M;
static const TSize                  KImageSizeSecondary     = TSize( 320,240 );
static const TInt                   KImageFactorSecondary   = 100;

static const TSize                  KViewfinderSize         = TSize(320, 240);
static const TCamViewfinderMode     KViewFinderMode         = ECamViewfinderBitmap;

static const TSize                  KViewfinderSizeSecondary   = TSize( 240, 180 );

static const CCamera::TFormat       KSnapshotFormat         = CCamera::EFormatFbsBitmapColor16M;
static const TSize                  KSnapshotSize           = TSize(320, 240);
static const TBool                  KSnapshotMaintainAspect = EFalse;

static const CCamera::TFlash        KFlashSetting           = CCamera::EFlashAuto;

static const CCamera::TExposure     KEvModeSetting          = CCamera::EExposureAuto;
#ifdef CAMERAAPP_CAPI_V2
  static const TInt                   KEvValueSetting       = 2;
#endif

static const TInt                   KISOValue               = 100;
static const CCamera::TWhiteBalance KWbModeSetting          = CCamera::EWBAuto;
static const CCamera::TBrightness   KBrightnessValue        = CCamera::EBrightnessAuto;
static const CCamera::TContrast     KContrastValue          = CCamera::EContrastAuto;
static const TInt                   KDigitalZoomValue       = 1;
static const TInt                   KOpticalZoomValue       = 1;

#ifdef CAMERAAPP_CAPI_V2
  static const CCamera::CCameraAdvancedSettings::TFocusRange KFocusRange = 
    CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
#endif // CAMERAAPP_CAPI_V2

// -------------------------------------
// Video by CAE settings:
// - MMS video setting, as it's the same in all products in use.
#ifdef CAMERAAPP_CAE_FOR_VIDEO

static const TSize KVideoFrameSize                      = TSize(176, 144);
static const TReal KVideoFrameRate                      = 15.0f;
static const TInt  KVideoVideoBitRate                   = 64000;
static const TInt  KVideoAudioBitRate                   = 12200;
static const TBool KVideoAudioOn                        = ETrue;

_LIT8( KVideoMimeType,  "video/3gpp"      );
_LIT ( KVideoSupplier,  "Nokia"           );
_LIT8( KVideoVideoType, "video/H263-2000" );
_LIT8( KVideoAudioType, " AMR"            );

#endif // CAMERAAPP_CAE_FOR_VIDEO
// -------------------------------------


// ===========================================================================
// Local methods


// ===========================================================================
// Methods

// static 
CCamConstantSettingProvider* 
CCamConstantSettingProvider::NewL( CCamAppController& aController )
  {
  CCamConstantSettingProvider* self = 
      CCamConstantSettingProvider::NewLC( aController );
  CleanupStack::Pop( self );
  return self;  
  }

// static 
CCamConstantSettingProvider* 
CCamConstantSettingProvider::NewLC( CCamAppController& aController )
  {
  CCamConstantSettingProvider* self = 
      new (ELeave) CCamConstantSettingProvider( aController );
  CleanupStack::PushL( self );
  self->ConstructL();
  return self;  
  }

// virtual 
CCamConstantSettingProvider::~CCamConstantSettingProvider()
  {
  iPendingSettingChanges.Close();
  }
  
// ===========================================================================
// From MCamSettingProvider

// ---------------------------------------------------------------------------
// virtual 
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::ProvideCameraParamL( 
    const NCamCameraController::TCamCameraParamId& aParamId,
          TAny*                                    aParamData )
  {
  __ASSERT_DEBUG( aParamData, Panic( ECamNullPointer ) );
  switch( aParamId )
    {
    // -----------------------------------------------------------------------
    case ECameraParamImage:
      {
      TPckgBuf<TCamParamsImage>* ptr = 
          static_cast<TPckgBuf<TCamParamsImage>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );
      TCamParamsImage& params( (*ptr)() ); 
      
      if( ECamActiveCameraPrimary == iController.ActiveCamera() )
        {                     
        // Primary camera parameters
        params.iFormat        = KImageFormat;
        params.iSize          = KImageSize;
        params.iQualityFactor = KImageFactor;
        }
      else
        {                     
        // Secondary camera parameters
        params.iFormat        = KImageFormatSecondary;
        params.iSize          = KImageSizeSecondary;
        params.iQualityFactor = KImageFactorSecondary;
        }                                    
      
      break;
      }
    // -----------------------------------------------------------------------
#ifdef CAMERAAPP_CAE_FOR_VIDEO
    case ECameraParamVideoCae:
      {
      TPckgBuf<TCamParamsVideoCae>* ptr = 
          static_cast<TPckgBuf<TCamParamsVideoCae>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );

      TCamParamsVideoCae& params( (*ptr)() ); 

      params.iFrameSize    = KVideoFrameSize;
      params.iFrameRate    = KVideoFrameRate;
      params.iVideoBitRate = KVideoVideoBitRate;
      params.iAudioBitRate = KVideoAudioBitRate;
      params.iAudioOn      = KVideoAudioOn;
      params.iMimeType.Set ( KVideoMimeType  );
      params.iSupplier.Set ( KVideoSupplier  );
      params.iVideoType.Set( KVideoVideoType );
      params.iAudioType.Set( KVideoAudioType );

      break;
      }
#endif // CAMERAAPP_CAE_FOR_VIDEO
    // -----------------------------------------------------------------------
    case ECameraParamVfMode:
      {
      TPckgBuf<TCamViewfinderMode>* ptr = 
          static_cast<TPckgBuf<TCamViewfinderMode>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );
      TCamViewfinderMode& params( (*ptr)() ); 
      params = KViewFinderMode;
      break;
      }
    // -----------------------------------------------------------------------
    case ECameraParamSnapshot: 
      {
      TPckgBuf<TCamParamsSnapshot>* ptr = 
          static_cast<TPckgBuf<TCamParamsSnapshot>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );
      TCamParamsSnapshot& params( (*ptr)() ); 

      params.iSize           = KSnapshotSize;
      params.iFormat         = KSnapshotFormat;
      params.iMaintainAspect = KSnapshotMaintainAspect;
      break;
      }
    // -----------------------------------------------------------------------
    default:
      {
      User::Leave( KErrNotSupported );
      break;
      }
    // -----------------------------------------------------------------------
    }
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::ProvideCameraSettingL( 
    const NCamCameraController::TCamCameraSettingId& aSettingId,
          TAny*                                      aSettingData )
  { 
  switch( aSettingId )
    {
    case ECameraSettingQualityFactor:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KImageFactor;
      break;
      }
    case ECameraSettingFlash:
      {
      CCamera::TFlash* ptr = 
        static_cast<CCamera::TFlash*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KFlashSetting;
      break;
      }
    case ECameraSettingExposure:
      {
      TPckgBuf<TCamSettingDataExposure>* ptr = 
        static_cast<TPckgBuf<TCamSettingDataExposure>*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );

      TCamSettingDataExposure& evData( (*ptr)() );
      evData.iExposureMode = KEvModeSetting;
#ifdef CAMERAAPP_CAPI_V2
      evData.iExposureStep = KEvValueSetting * KECamFineResolutionFactor;
#else
      evData.iExposureStep = 0;
#endif
      break;
      }
    case ECameraSettingLightSensitivity:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KISOValue;
      break;
      }
    case ECameraSettingWhiteBalance:
      {
      TPckgBuf<TCamSettingDataWhiteBalance>* ptr = 
        static_cast<TPckgBuf<TCamSettingDataWhiteBalance>*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );

      TCamSettingDataWhiteBalance& wbData( (*ptr)() );
      wbData.iWhiteBalanceMode = KWbModeSetting;
      break;
      }
    case ECameraSettingBrightness:
      {
      CCamera::TBrightness* ptr = 
          static_cast<CCamera::TBrightness*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KBrightnessValue;
      break;
      }
    case ECameraSettingContrast:
      {
      CCamera::TContrast* ptr = 
          static_cast<CCamera::TContrast*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KContrastValue;
      break;
      }
    case ECameraSettingDigitalZoom:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KDigitalZoomValue;
      break;
      }
    case ECameraSettingOpticalZoom:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KOpticalZoomValue;
      break;
      }

#ifdef CAMERAAPP_CAPI_V2
    case ECameraSettingFocusRange:
      {
      CCamera::CCameraAdvancedSettings::TFocusRange* ptr =
          static_cast<CCamera::CCameraAdvancedSettings::TFocusRange*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = KFocusRange;
      break;
      }
#endif // CAMERAAPP_CAPI_V2
    case ECameraSettingAudioMute:
      {
      TBool* ptr = static_cast<TBool*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = EFalse;
      break;
      }
    case ECameraSettingContAF:  
      {
      TBool* ptr = static_cast<TBool*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = ETrue;
      break;
      }
    case ECameraSettingFileMaxSize:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = 0; // not limited
      break;
      }
    case ECameraSettingFileName:
      {
      TPtr* ptr = static_cast<TPtr*>( aSettingData );
      CheckNonNullL( ptr, KErrNotFound );
      *ptr = iController.CurrentFullFileName();
      break;
      }
    default:
      {
      User::Leave( KErrNotSupported );
      break;
      }
    }
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::ProvidePendingSettingChangesL( 
    RArray<NCamCameraController::TCamCameraSettingId>& aSettingIds )
  {
  PRINT( _L("Camera => CCamConstantSettingProvider::ProvidePendingSettingChangesL") );
  aSettingIds.Reset();
  aSettingIds.ReserveL( iPendingSettingChanges.Count() );
  for( TInt i = 0; i < iPendingSettingChanges.Count(); i++ )
    {
    aSettingIds.AppendL( iPendingSettingChanges[i] );
    }    
  PRINT1( _L("Camera <> %d settings changed"), aSettingIds.Count() );


  // Update: Reset only when asked. Enables retries.
  // iPendingSettingChanges.Reset();
  PRINT( _L("Camera <= CCamConstantSettingProvider::ProvidePendingSettingChangesL") );
  }


// ---------------------------------------------------------------------------
// PendingSettingChangeCount
// ---------------------------------------------------------------------------
//
TInt 
CCamConstantSettingProvider::PendingSettingChangeCount() const
  {
  return iPendingSettingChanges.Count();
  }


// ===========================================================================
// From MCamSettingsModelObserver
/*
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::IntegerSettingChangedL( 
    TCamSettingItemIds aSettingItem, 
    TInt//               aSettingValue
    )
  {
  TCamCameraIntSettingId cameraId = 
      Map2CameraControllerSettingId( aSettingItem );

  if( ECameraSettingNone != cameraId 
   && KErrNotFound == iPendingSettingChanges.Find( cameraId ) )
    {
    iPendingSettingChanges.AppendL( cameraId )
    }
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::TextSettingChangedL( 
          TCamSettingItemIds aSettingItem, 
    const TDesC&//             aSettingValue
    )
  {
  TCamCameraIntSettingId cameraId = 
      Map2CameraControllerSettingId( aSettingIds[i] );

  if( ECameraSettingNone != cameraId )
    {
    iPendingSettingChanges.Append( cameraId )
    }
  }
*/
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::Reset()
  {
  iPendingSettingChanges.Reset();
  }
  
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider
::AddPendingSettingChangesL( const RArray<TCamSettingItemIds>& aUiSettingIds )
  {
  iPendingSettingChanges.ReserveL( aUiSettingIds.Count() 
                                 + iPendingSettingChanges.Count() );

  for( TInt i = 0; i < aUiSettingIds.Count(); i++ )
    {
    AddPendingSettingChangeL( aUiSettingIds[i] );
    }
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider
::AddPendingSettingChangeL( TCamSettingItemIds aUiSettingId )
  {
  TCamCameraSettingId cameraId = 
      CCamSettingConversion::Map2CameraControllerSettingId( aUiSettingId );

  // Add each setting only once
  if( ECameraSettingNone != cameraId 
   && KErrNotFound       == iPendingSettingChanges.Find( cameraId ) )
    {
    iPendingSettingChanges.AppendL( cameraId );
    }
  }

// ---------------------------------------------------------------------------
// SetSupportedISORatesL
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::SetSupportedISORatesL( const RArray<TInt>& aSupportedIsoRates )
  {
  iSupportedISORates = aSupportedIsoRates;
  if ( iSupportedISORates.Count() != 0 ) 
    {
    iValidISORates = ETrue;
    }
  }

// ===========================================================================
// private


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamConstantSettingProvider::ConstructL()
  {
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamConstantSettingProvider
::CCamConstantSettingProvider( CCamAppController& aController )
  : iController( aController ),
    iPendingSettingChanges()
  {
  }

