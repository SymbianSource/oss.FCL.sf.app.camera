/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Setting Provider class implementation
*
*/


// ===========================================================================
// Includes

#include "camcameracontrollerflags.hrh"
#include <ecam.h>
#ifdef CAMERAAPP_CAPI_V2
  #include <ecamadvsettings.h> // KECamFineResolutionFactor
#endif

#include <ECamOrientationCustomInterface2.h>

#include "camlogging.h"
#include "camcameracontroller.pan"
#include "CamSettingsInternal.hrh"
#include "camcameracontrollertypes.h"
#include "camsettingprovider.h"
#include "CamSettingsModel.h"
#include "camsettingconversion.h"
#include "CamAppController.h"
#include "campointerutility.inl"
#include "CameraUiConfigManager.h"

#include "CamAppUi.h"
#include "CamUtility.h"
#include "CamPSI.h"

#include "CamVideoQualityLevel.h"


// ===========================================================================
// Types

using namespace NCamCameraController;

#ifdef CAMERAAPP_CAPI_V2_ADV
  typedef CCamera::CCameraAdvancedSettings CAS;
#endif
#ifdef CAMERAAPP_CAPI_V2_IP
  typedef CCamera::CCameraImageProcessing  CIP;
#endif

// ===========================================================================
// Local data

static const TInt               KSettingArrayGranularity = 8;

static const TSize              KViewfinderSize         = TSize(320, 240);
static const TBool              KSnapshotMaintainAspect = EFalse;
static const TInt               KEngineMultiplier       = 10;

// ===========================================================================
// Methods

// static
CCamSettingProvider*
CCamSettingProvider::NewL( CCamAppController& aController,
                           MCamSettingsModel* aSettingsModel )
  {
  CCamSettingProvider* self =
      CCamSettingProvider::NewLC( aController, aSettingsModel );
  CleanupStack::Pop( self );
  return self;
  }

// static
CCamSettingProvider*
CCamSettingProvider::NewLC( CCamAppController& aController,
                            MCamSettingsModel* aSettingsModel )
  {
  CCamSettingProvider* self =
      new (ELeave) CCamSettingProvider( aController, aSettingsModel );
  CleanupStack::PushL( self );
  self->ConstructL();
  return self;
  }

// virtual
CCamSettingProvider::~CCamSettingProvider()
  {
  iPendingSettingChanges.Close();
  }


// ===========================================================================
// From MCamSettingProvider

// ---------------------------------------------------------------------------
// virtual ProvideCameraParamL
//
// ---------------------------------------------------------------------------
//
void
CCamSettingProvider::ProvideCameraParamL(
    const NCamCameraController::TCamCameraParamId& aParamId,
          TAny*                                      aParamData )
  {
  __ASSERT_DEBUG( aParamData, Panic( ECamNullPointer ) );
  switch( aParamId )
    {
    // -----------------------------------------------------
    case ECameraParamImage:
      {
      TPckgBuf<TCamParamsImage>* ptr =
          static_cast<TPckgBuf<TCamParamsImage>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );
      TCamParamsImage& params( (*ptr)() );
      CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() );
      
      // Both primary and secondary camera use Exif format
      params.iFormat = CCamera::EFormatExif;  

      TCamPhotoSizeId PhotoSizeIndex = iSettingsModel->CurrentPhotoResolution();
      TSize PhotoSize = iSettingsModel->ImageResolutionFromIndex( PhotoSizeIndex );
      params.iSize     = PhotoSize;
      params.iQualityFactor = iSettingsModel->CurrentPhotoCompression();
      break;
      }
    // -----------------------------------------------------
#ifdef CAMERAAPP_CAE_FOR_VIDEO
    case ECameraParamVideoCae:
      {
      TPckgBuf<TCamParamsVideoCae>* ptr =
          static_cast<TPckgBuf<TCamParamsVideoCae>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );

      TCamParamsVideoCae& params( (*ptr)() );


      TInt videoQuality = iSettingsModel->IntegerSettingValue( ECamSettingItemVideoQuality );

      PRINT1( _L("Camera <> CCamSettingProvider::ProvideCameraParamL - video quality (index): %d"), videoQuality );
      PRINT1( _L("Camera <> CCamSettingProvider::ProvideCameraParamL - quality count: %d"), iSettingsModel->VideoQualityArray().Count() );

      CCamVideoQualityLevel& level = *( iSettingsModel->VideoQualityArray() )[videoQuality];
      TInt res = level.VideoResolution();
      PRINT1( _L("Camera <> CCamSettingProvider::ProvideCameraParamL - video resolution (index): %d"), res );
      PRINT1( _L("Camera <> CCamSettingProvider::ProvideCameraParamL - resolution count: %d"), iSettingsModel->VideoResolutions().Count() );

      // Video resolutions array in Setting Model is set up from
      // R_CAM_VIDEO_RESOLUTION_ARRAY resource. New resolutions
      // need to be added there also, in addition to configuration files.
      TSize resolution =  ( iSettingsModel->VideoResolutions() )[ res ];
      params.iFrameSize    = resolution;
      params.iFrameRate    = level.FrameRate();
      params.iVideoBitRate = level.VideoBitRate();
      params.iAudioBitRate = level.AudioBitRate();
      params.iAudioOn      = iSettingsModel->IntegerSettingValue( ECamSettingItemVideoAudioRec );
      params.iMimeType.Set ( level.VideoMimeType() );
      params.iSupplier.Set ( level.PreferredSupplier() );
      params.iVideoType.Set( level.VideoFileType() );
      params.iAudioType.Set( level.AudioType() );

      TInt framerate = -1;
      TCamSceneId videoscene = static_cast< TCamSceneId > (
              iSettingsModel->IntegerSettingValue(ECamSettingItemDynamicVideoScene) );

      framerate = iSettingsModel->SceneSettingValue(videoscene, ECamSettingItemSceneFramerate);

      // Change the framerate if scene has defined it to be > 0
      if ( framerate > 0 )
          {
          params.iFrameRate = framerate;
          }

      PRINT1( _L("Camera <> CCamSettingProvider::ProvideCameraParamL ECameraParamVideoCae framerate: %d"), framerate )

      break;
      }
#endif // CAMERAAPP_CAE_FOR_VIDEO
    // -----------------------------------------------------
    case ECameraParamVfMode:
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      TPckgBuf<TCamViewfinderMode>* ptr =
          static_cast<TPckgBuf<TCamViewfinderMode>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );
      TCamViewfinderMode& params( (*ptr)() );
      TBool isPrimaryCamera = appUi->ActiveCamera() == ECamActiveCameraPrimary;

      if ( iController.IsDirectScreenVFSupported( isPrimaryCamera ) )
        {// primary camera or 2nd camera DSVF
        PRINT( _L("Setting Provider <> provide ECameraParamVfMode == ECamViewfinderDirect") );
        params = ECamViewfinderDirect;
        }
      else
        {// primary camera or 2nd camera bitmap view finder
        PRINT( _L("Setting Provider <> provide ECameraParamVfMode == ECamViewfinderBitmap") );
        params = ECamViewfinderBitmap;
        }
      break;
      }
    // -----------------------------------------------------
    case ECameraParamVfBitmap:
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>(
            CEikonEnv::Static()->AppUi() );
        PRINT2( _L("Setting Provider <> provide ECameraParamVfBitmap. CurrentMode: (%d) TargetMode: (%d)"), iController.CurrentMode(), appUi->TargetMode() );
        TPckgBuf<TCamParamsVfBitmap>* ptr =
          static_cast<TPckgBuf<TCamParamsVfBitmap>*>( aParamData );
        CheckNonNullL( ptr, KErrNotFound );
        TCamParamsVfBitmap& params( (*ptr)() );

        TDisplayMode displayMode =
            CEikonEnv::Static()->ScreenDevice()->DisplayMode();
        CCamera::TFormat viewfinderFormat =
            CCamSettingConversion::Map2CameraFormat( displayMode );

        const TCamCameraMode cameraMode =
        ( appUi->TargetMode() != ECamControllerIdle)
            ? appUi->TargetMode()
            : iController.CurrentMode();
        const TInt           resolution = (cameraMode == ECamControllerVideo)
                                        ? iController.GetCurrentVideoResolution()
                                        : iController.GetCurrentImageResolution();
        params.iRect   = CamUtility::ViewfinderLayout( cameraMode, resolution );

        params.iFormat = viewfinderFormat;

        // Mirror viewfinder for secondary camera
        params.iMirrorImage = ECamActiveCameraSecondary == appUi->ActiveCamera();

        break;
        }
    // -----------------------------------------------------
    case ECameraParamVfDirect:
      {
      // Not supported yet.
      User::Leave( KErrNotSupported );
      break;
      }
    // -----------------------------------------------------
    case ECameraParamSnapshot:
      {
      TPckgBuf<TCamParamsSnapshot>* ptr =
          static_cast<TPckgBuf<TCamParamsSnapshot>*>( aParamData );
      CheckNonNullL( ptr, KErrNotFound );
      TCamParamsSnapshot& params( (*ptr)() );

      // Fetch the snapshot size
      TDisplayMode   displayMode = CEikonEnv::Static()->ScreenDevice()->DisplayMode();
      TCamCameraMode cameraMode  = iController.CurrentMode();
      TInt           resolution  = (cameraMode == ECamControllerVideo)
                                      ? iController.GetCurrentVideoResolution()
                                      : iController.GetCurrentImageResolution();
      params.iSize           = CamUtility::ViewfinderLayout( cameraMode, resolution ).Size();
      params.iFormat         = CCamSettingConversion::Map2CameraFormat(displayMode);
      params.iMaintainAspect = KSnapshotMaintainAspect;
      break;
      }
    // -----------------------------------------------------
    default:
      {
      User::Leave( KErrNotSupported );
      break;
      }
    // -----------------------------------------------------
    }
  }


// ---------------------------------------------------------------------------
// virtual ProvideCameraSettingL
//
// ---------------------------------------------------------------------------
//
void
CCamSettingProvider::ProvideCameraSettingL(
    const NCamCameraController::TCamCameraSettingId& aSettingId,
          TAny*                                      aSettingData )
  {
  CheckNonNullL( aSettingData, KErrArgument );

  TCamCameraMode mode( iController.CurrentMode() );

  switch( aSettingId )
    {
    // -----------------------------------------------------
    // Flash
    case ECameraSettingFlash:
    case ECameraUserSceneSettingFlash:
      {
      TCamFlashId flashId = static_cast<TCamFlashId> (iSettingsModel->
                     IntegerSettingValue( ECamSettingItemDynamicPhotoFlash ) );
      if ( mode == ECamControllerVideo )
        {
        flashId = static_cast<TCamFlashId> (iSettingsModel->
                     IntegerSettingValue( ECamSettingItemDynamicVideoFlash ) );
        }
      CCamera::TFlash CamFlashId = CCamSettingConversion::Map2CameraFlash( flashId );
      CCamera::TFlash* ptr = static_cast<CCamera::TFlash*>( aSettingData );
      *ptr = CamFlashId;
      break;
      }
    // -----------------------------------------------------
    // Exposure (EV mode + EV compensation step)
    case ECameraSettingExposure:
    case ECameraUserSceneSettingExposure:
      {
      TPckgBuf<TCamSettingDataExposure>* ptr =
        static_cast<TPckgBuf<TCamSettingDataExposure>*>( aSettingData );
      TCamSettingDataExposure& evData( (*ptr)() );

      TInt sceneItemId = ( ECamControllerVideo == mode )
                         ? ECamSettingItemDynamicVideoScene
                         : ECamSettingItemDynamicPhotoScene;

      TInt sceneId = iSettingsModel->IntegerSettingValue( sceneItemId );

      TCamSceneAEMode evMode =
        static_cast<TCamSceneAEMode>(
          iSettingsModel->SceneSettingValue( sceneId, ECamSettingItemSceneAEMode ) );

      evData.iExposureMode = CCamSettingConversion::Map2CameraExposureMode( evMode );
      PRINT1( _L("Camera <> provide EV mode: %016b"), evData.iExposureMode );
#ifdef CAMERAAPP_CAPI_V2
      TInt evKey;
      if ( ECameraUserSceneSettingExposure != aSettingId )
        {
        evKey   = ( ECamControllerVideo == mode )
                   ? ECamSettingItemDynamicVideoExposure
                   : ECamSettingItemDynamicPhotoExposure;
        }
      else
        {
        evKey   = ECamSettingItemUserSceneExposure;
        }

      TInt evValue = iSettingsModel->IntegerSettingValue( evKey );
      if( iEvStepAsUiValue != 0 )
        evData.iExposureStep = (evValue * KECamFineResolutionFactor) / iEvStepAsUiValue;
      else
        evData.iExposureStep = 0;
      PRINT1( _L("Camera <> provide EV compensation: %d"), evData.iExposureStep );
#else
      evData.iExposureStep = 0;
#endif
      break;
      }
    // -----------------------------------------------------
    // Light sensitivity (ISO value)
    case ECameraSettingLightSensitivity:
    case ECameraUserSceneSettingLightSensitivity:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      TInt isoKey;

      if ( ECameraUserSceneSettingLightSensitivity != aSettingId )
        {
        isoKey   =  ( ECamControllerVideo == mode )
                    ? ECamSettingItemDynamicVideoLightSensitivity
                    : ECamSettingItemDynamicPhotoLightSensitivity;
        }
      else
        {
        isoKey = ECamSettingItemUserSceneLightSensitivity;
        }
      // TCamLightSensitivityId type value returned
      TCamLightSensitivityId isoId =
        static_cast<TCamLightSensitivityId>( iSettingsModel->IntegerSettingValue( isoKey ) );

      *ptr = CamUtility::MapLightSensitivity2IsoValueL( isoId, iSupportedISORates );
      PRINT1( _L("Camera <> provide ISO rate: %d"), *ptr );
      break;
      }
    // -----------------------------------------------------
    // White balance
    case ECameraSettingWhiteBalance:
    case ECameraUserSceneSettingWhiteBalance:
      {
      TPckgBuf<TCamSettingDataWhiteBalance>* ptr =
        static_cast<TPckgBuf<TCamSettingDataWhiteBalance>*>( aSettingData );
      TInt key;

      if ( ECameraUserSceneSettingWhiteBalance != aSettingId )
        {
        key = ( ECamControllerVideo == mode )
               ? ECamSettingItemDynamicVideoWhiteBalance
               : ECamSettingItemDynamicPhotoWhiteBalance;
        }
      else
        {
        key = ECamSettingItemUserSceneWhitebalance;
        }

      TCamSettingDataWhiteBalance& wbData( (*ptr)() );

      TCamWhiteBalanceId WB = static_cast<TCamWhiteBalanceId>(iSettingsModel->IntegerSettingValue( key ));
      CCamera::TWhiteBalance CamWB = CCamSettingConversion::Map2CameraWb( WB );

      wbData.iWhiteBalanceMode = CamWB;
      break;
      }
#ifdef CAMERAAPP_CAPI_V2_IP
    // -----------------------------------------------------
    // Colour effect (colour filter: sepia/b&w/vivid/..)
    case ECameraSettingColourEffect:
    case ECameraUserSceneSettingColourEffect:
      {
      CIP::TEffect* ptr = static_cast<CIP::TEffect*>( aSettingData );

      TInt key;

      if ( ECameraUserSceneSettingColourEffect != aSettingId )
        {
        key = ( ECamControllerVideo == mode )
               ? ECamSettingItemDynamicVideoColourFilter
               : ECamSettingItemDynamicPhotoColourFilter;
        }
      else
        {
        key = ECamSettingItemUserSceneColourFilter;
        }

      TCamColourFilterId uiValue =
        static_cast<TCamColourFilterId>( iSettingsModel->IntegerSettingValue( key ) );

      CIP::TEffect effect = CCamSettingConversion::Map2CameraEffect( uiValue );

      PRINT1( _L("Camera <> provide colour effect: 0x%08x"), effect );
      *ptr = effect;
      break;
      }
    // -----------------------------------------------------
    // Sharpness
    case ECameraSettingSharpness:
    case ECameraUserSceneSettingSharpness:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );

      TInt key;

      if ( ECameraUserSceneSettingSharpness != aSettingId )
        {
        key = ECamSettingItemDynamicPhotoImageSharpness;
        }
      else
        {
        key = ECamSettingItemUserSceneImageSharpness;
        }
      TCamImageSharpnessId uiValue =
        static_cast<TCamImageSharpnessId>(
          iSettingsModel->IntegerSettingValue( key ) );

      TInt sharpness = CamUtility::MapSharpnessId2SharpnessValueL( uiValue );
      PRINT1( _L("Camera <> provide sharpness: %d"), sharpness );
      *ptr = sharpness;

      break;
      }
#endif // CAMERAAPP_CAPI_V2_IP
    // -----------------------------------------------------
    // Brightness
    case ECameraSettingBrightness:
    case ECameraUserSceneSettingBrightness:
      {
      CCamera::TBrightness* ptr =
          static_cast<CCamera::TBrightness*>( aSettingData );

      TInt key;

      if ( ECameraUserSceneSettingBrightness != aSettingId )
        {
        key = ( ECamControllerVideo == mode )
               ? ECamSettingItemDynamicVideoBrightness
               : ECamSettingItemDynamicPhotoBrightness;
        }
      else
        {
        key = ECamSettingItemUserSceneBrightness;
        }

      TInt SettingsBrightness = iSettingsModel->IntegerSettingValue( key );
      CCamera::TBrightness CamBrightness = CCamera::TBrightness( KEngineMultiplier * SettingsBrightness );

      PRINT1( _L("Camera <> provide brightness: %d"), CamBrightness );
      *ptr = CamBrightness;
      break;
      }
    // -----------------------------------------------------
    // Contrast
    case ECameraSettingContrast:
    case ECameraUserSceneSettingContrast:
      {
      CCamera::TContrast* ptr = static_cast<CCamera::TContrast*>( aSettingData );
      TInt key;

      if ( ECameraUserSceneSettingContrast != aSettingId )
        {
        key = ( ECamControllerVideo == mode )
               ? ECamSettingItemDynamicVideoContrast
               : ECamSettingItemDynamicPhotoContrast;
        }
      else
        {
        key = ECamSettingItemUserSceneContrast;
        }

      TInt SettingsContrast = iSettingsModel->IntegerSettingValue( key );
      CCamera::TContrast CamContrast = CCamera::TContrast( KEngineMultiplier * SettingsContrast );
      PRINT1( _L("Camera <> provide contrast: %d"), CamContrast );
      *ptr = CamContrast;
      break;
      }
    // -----------------------------------------------------
    // Digital zoom
    case ECameraSettingDigitalZoom:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      CheckNonNullL( ptr, KErrArgument );
      *ptr = iController.ZoomValue();
      break;
      }
    // -----------------------------------------------------
    // Optical zoom
    case ECameraSettingOpticalZoom:
      {
      User::Leave( KErrNotSupported );
      //iSettingsModel->IntegerSettingValue( ECamSettingItemPhotoOpticalZoom );
      break;
      }
#ifdef CAMERAAPP_CAPI_V2_ADV
    case ECameraSettingFocusRange:
      {
      // User::Leave( KErrNotSupported );
      CAS::TFocusRange* ptr = static_cast<CAS::TFocusRange*>( aSettingData );

      // Get current scene, from which the focus range value should be obtained
      TInt currentScene = iSettingsModel->IntegerSettingValue( ECamSettingItemDynamicPhotoScene );
      if ( currentScene  == ECamSceneUser )
        {
        // In case of user scene, use the base scene instead
        currentScene = iSettingsModel->IntegerSettingValue( ECamSettingItemUserSceneBasedOnScene );
        }
      PRINT1( _L("Camera <> current scene: %d"), currentScene );

      // Get the autofocus mode in camera internal format
      TInt afMode = iSettingsModel->SceneSettingValue( currentScene, ECamSettingItemSceneAFMode );

      PRINT1( _L("Camera <> Autofocusmode for current scene: %d"), afMode );

      TCamSettingsOnOff facetrack =
      static_cast<TCamSettingsOnOff>(
          iSettingsModel->IntegerSettingValue( ECamSettingItemFaceTracking ) );
      if ( iController.UiConfigManagerPtr() && iController.UiConfigManagerPtr()->IsFaceTrackingSupported() &&
           ( ECamSettOn == facetrack ) )
          {
          // Use CCamera::CCameraAdvancedSettings::EFocusRangeAuto if in facetracking mode.
          *ptr = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
          }
      else
          {
          // Convert it to TFocusRange
          *ptr = CCamSettingConversion::Map2CameraAutofocus( static_cast<TCamSceneAFMode>( afMode ) );
          PRINT1( _L("Camera <> Autofocusmode mapped for CCamera: %d"), *ptr );
          }
      //CheckNonNullL( ptr, KErrNotFound );
      //*ptr = KFocusRange;//iSettingsModel->IntegerSettingValue( ECamSettingItemDynamicPhotoFocusRange );
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingCaptureLimit:
      {
      TInt* ptr = static_cast<TInt*>( aSettingData );
      *ptr = iController.CaptureLimitSetting();

      PRINT1( _L("Camera <> provide capture limit: %d"), *ptr );
      break;
      }
    // -----------------------------------------------------
    // Stabilization setting (for video)
    case ECameraSettingStabilization:
      {
      TCamSettingDataStabilization& stabilization =
        ( *static_cast<TPckgBuf<TCamSettingDataStabilization>*>( aSettingData ) )();

      TCamSettingsOnOff settingOn =
        static_cast<TCamSettingsOnOff>( iSettingsModel->IntegerSettingValue( ECamSettingItemVideoStab ) );

      // These could be made product specific..
      if( ECamSettOn == settingOn )
        {
        PRINT( _L("Camera <> Stabilization is set ON") );
        stabilization.iMode       = CAS::EStabilizationModeAuto;
        stabilization.iEffect     = CAS::EStabilizationAuto;
        stabilization.iComplexity = CAS::EStabilizationComplexityAuto;
        }
      else
        {
        PRINT( _L("Camera <> Stabilization is set OFF") );
        stabilization.iMode       = CAS::EStabilizationModeOff;
        stabilization.iEffect     = CAS::EStabilizationOff;
        stabilization.iComplexity = CAS::EStabilizationComplexityAuto;
        }
      break;
      }
#endif // CAMERAAPP_CAPI_V2_ADV
    // -----------------------------------------------------
    // Device orientation setting
    case ECameraSettingOrientation:
      {
      MCameraOrientation::TOrientation* ptr =
        static_cast<MCameraOrientation::TOrientation*>( aSettingData );

      TCamImageOrientation orientationSetting =
        static_cast<TCamImageOrientation>( iController.ImageOrientation() );
      PRINT1( _L("Camera <> Got image orientation from ui: [%s]"), KCamOrientationNames[orientationSetting] );

      *ptr = CCamSettingConversion::Map2CameraOrientation( orientationSetting );
      PRINT1( _L("Camera <> provide camera orientation: 0x%02X"), *ptr );
      break;
      }
    // -----------------------------------------------------
    // Video audio mute setting
    case ECameraSettingAudioMute:
      {
      TCamSettingsOnOff mute =
        static_cast<TCamSettingsOnOff>(
          iSettingsModel->IntegerSettingValue( ECamSettingItemVideoAudioRec ) );

      TBool* ptr = static_cast<TBool*>( aSettingData );
      *ptr = (ECamSettOn == mute);
      PRINT1( _L("Camera <> provide mute on: %d"), *ptr );
      break;
      }
    // -----------------------------------------------------
    // Continuous autofocus setting
    case ECameraSettingContAF: 
      {
      TCamSettingsOnOff caf = static_cast<TCamSettingsOnOff>( 
         iSettingsModel->IntegerSettingValue( ECamSettingItemContinuousAutofocus ) );

      TBool* ptr = static_cast<TBool*>( aSettingData );
      *ptr = (ECamSettOn == caf);
      PRINT1( _L("Camera <> provide continuous autofocus on: %d"), *ptr );      
      break;
      }
    // -----------------------------------------------------
    // facetracking setting
    case ECameraSettingFacetracking:
      {
      TCamSettingsOnOff facetrack =
        static_cast<TCamSettingsOnOff>(
          iSettingsModel->IntegerSettingValue( ECamSettingItemFaceTracking ) );
      TBool* ptr = static_cast<TBool*>( aSettingData );
      *ptr = (ECamSettOn == facetrack);
      PRINT1( _L("Camera <> provide facetracking on: %d"), *ptr );
      break;
      }
    // -----------------------------------------------------
    // Video max size in bytes
    case ECameraSettingFileMaxSize:
      {
      TCamVideoClipLength setting =
        static_cast<TCamVideoClipLength>(
          iSettingsModel->IntegerSettingValue( ECamSettingItemVideoClipLength ) );

      TInt* ptr = static_cast<TInt*>( aSettingData );

      *ptr = ( ECamVideoClipShort == setting )
             ? CamUtility::MaxMmsSizeInBytesL()
             : 0; // 0 means not limited

      PRINT1( _L("Camera <> provide max file size: %d"), *ptr );
      break;
      }
    // -----------------------------------------------------
    // Video filename
    case ECameraSettingFileName:
      {
      TPtr* ptr = static_cast<TPtr*>( aSettingData );
      //*ptr = iController.CurrentFullFileName();
      *ptr = iController.CurrentVideoFileName();
      break;
      }
    // -----------------------------------------------------
    default:
      {
#ifndef CAMERAAPP_EMULATOR_BUILD
      User::Leave( KErrNotSupported );
#endif // not CAMERAAPP_EMULATOR_BUILD
      break;
      }
    // -----------------------------------------------------
    }
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSettingProvider::ProvidePendingSettingChangesL(
    RArray<NCamCameraController::TCamCameraSettingId>& aSettingIds )
  {
  PRINT( _L("Camera => CCamSettingProvider::ProvidePendingSettingChangesL") );
  aSettingIds.Reset();
  aSettingIds.ReserveL( iPendingSettingChanges.Count() );
  for( TInt i = 0; i < iPendingSettingChanges.Count(); i++ )
    {
    aSettingIds.AppendL( iPendingSettingChanges[i] );
    }
  PRINT1( _L("Camera <> %d settings changed"), aSettingIds.Count() );

  // These have been asked and delivered now.
  // Update: Reset only when asked. Enables retries.
  // iPendingSettingChanges.Reset();
  PRINT( _L("Camera <= CCamSettingProvider::ProvidePendingSettingChangesL") );
  }


// ---------------------------------------------------------------------------
// PendingSettingChangeCount
// ---------------------------------------------------------------------------
//
TInt
CCamSettingProvider::PendingSettingChangeCount() const
  {
  return iPendingSettingChanges.Count();
  }


// ===========================================================================
// New methods

// ---------------------------------------------------------------------------
// Reset
// ---------------------------------------------------------------------------
//
void
CCamSettingProvider::Reset()
  {
  PRINT( _L("Camera =><= CCamSettingProvider::Reset") );
  iPendingSettingChanges.Reset();
  }

// ---------------------------------------------------------------------------
// AddPendingSettingChangesL
// ---------------------------------------------------------------------------
//
void
CCamSettingProvider
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
// AddPendingSettingChangeL
// ---------------------------------------------------------------------------
//
void
CCamSettingProvider
::AddPendingSettingChangeL( TCamSettingItemIds aUiSettingId )
  {
  PRINT1( _L("Camera => CCamSettingProvider::AddPendingSettingChangeL [%s]"),
          KCamSettingItemNames[aUiSettingId] );

  TCamCameraSettingId cameraId =
      CCamSettingConversion::Map2CameraControllerSettingId( aUiSettingId );

  if( ECameraSettingNone != cameraId )
    {
    // Add each setting only once
    if( KErrNotFound == iPendingSettingChanges.Find( cameraId ) )
      {
      PRINT1( _L("Camera <> Add pending setting [%s]"), KCameraSettingNames[cameraId] );
      iPendingSettingChanges.AppendL( cameraId );
      }
    }
  else
    {
    User::Leave( KErrNotSupported );
    }
  PRINT( _L("Camera <= CCamSettingProvider::AddPendingSettingChangeL") );
  }

// ---------------------------------------------------------------------------
// SetSupportedISORatesL
//
// ---------------------------------------------------------------------------
//
void
CCamSettingProvider::SetSupportedISORatesL( const RArray<TInt>& aSupportedIsoRates )
  {
  iSupportedISORates = aSupportedIsoRates;
  if ( iSupportedISORates.Count() != 0 )
    {
    iValidISORates = ETrue;
    }
  }

// ===========================================================================
// private

void
CCamSettingProvider::ConstructL()
  {
  PRINT( _L("Camera => CCamSettingProvider::ConstructL") );
  // Get EV steps range.
  iEvStepAsUiValue = iController.EvRange().iStepsPerUnit;
  PRINT( _L("Camera <= CCamSettingProvider::ConstructL") );
  }


CCamSettingProvider::CCamSettingProvider( CCamAppController& aController,
                                          MCamSettingsModel* aSettingsModel )
 : iSettingsModel( aSettingsModel ),
   iController   ( aController    ),
   iPendingSettingChanges( KSettingArrayGranularity )
  {
  }


