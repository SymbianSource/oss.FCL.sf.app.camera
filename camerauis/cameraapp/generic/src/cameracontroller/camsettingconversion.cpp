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
* Description:  Setting conversion utility class implementation
*
*/


// ===========================================================================
// Includes
#include "camcameracontrollerflags.hrh" // Keep first

#include <ecam.h>
#ifdef CAMERAAPP_CAPI_V2_ADV
  #include <ecamadvsettingsuids.hrh>
#endif
#include "camcameracontroller.pan"
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "camcameracontrollersettings.h"
#include "camsettingconversion.h"


using namespace NCamCameraController;

// ===========================================================================
// Class methods

// ---------------------------------------------------------------------------
// Map2CameraWb
// ---------------------------------------------------------------------------
//
CCamera::TWhiteBalance
CCamSettingConversion::Map2CameraWb( const TCamWhiteBalanceId& aWbId )
  {
  CCamera::TWhiteBalance wb( CCamera::EWBAuto );
  switch( aWbId )
    {
    case ECamWhiteBalanceAWB:         wb = CCamera::EWBAuto;        break;
    case ECamWhiteBalanceDaylight:    wb = CCamera::EWBDaylight;    break;
    case ECamWhiteBalanceCloudy:      wb = CCamera::EWBCloudy;      break;
    case ECamWhiteBalanceTungsten:    wb = CCamera::EWBTungsten;    break;
    case ECamWhiteBalanceFlourescent: wb = CCamera::EWBFluorescent; break;
/*
    case ECamWhiteBalanceFlash:       wb = CCamera::EWBFlash;       break;
    case ECamWhiteBalanceSnow:        wb = CCamera::EWBSnow;        break;
    case ECamWhiteBalanceBeach:       wb = CCamera::EWBBeach;       break;
    case ECamWhiteBalanceManual:      wb = CCamera::EWBManual;      break;
    case ECamWhiteBalanceShade:       wb = CCamera::EWBShade;       break;
*/
    default:                  
      break;
    }
  return wb;
  }

// ---------------------------------------------------------------------------
// Map2CameraFlash
// ---------------------------------------------------------------------------
//
CCamera::TFlash
CCamSettingConversion::Map2CameraFlash( const TCamFlashId& aFlashId )
  {
  CCamera::TFlash flash( CCamera::EFlashAuto );
  switch ( aFlashId )
    {
    case ECamFlashAuto:       flash = CCamera::EFlashAuto;          break;
    case ECamFlashAntiRedEye: flash = CCamera::EFlashRedEyeReduce;  break;
    case ECamFlashForced:     flash = CCamera::EFlashForced;        break;
    case ECamFlashOff:        flash = CCamera::EFlashNone;          break;
/*
    case ECamFlashFill:       flash = CCamera::EFlashFillIn;        break;
    case ECamFlashSlowFront:  flash = CCamera::EFlashSlowFrontSync; break;
    case ECamFlashSlowRear:   flash = CCamera::EFlashSlowRearSync;  break;
    case ECamFlashManual:     flash = CCamera::EFlashManual;        break;
*/
    default:                  
      break;
    }
  return flash;
  }


// ---------------------------------------------------------------------------
// Map2FlashId
// ---------------------------------------------------------------------------
//
TCamFlashId
CCamSettingConversion::Map2FlashId( const CCamera::TFlash& aCameraFlash )
  {
  TCamFlashId flashId( ECamFlashAuto );

  switch( aCameraFlash )
    {
    case CCamera::EFlashAuto:         flashId = ECamFlashAuto;       break;
    case CCamera::EFlashRedEyeReduce: flashId = ECamFlashAntiRedEye; break;
    case CCamera::EFlashForced:       flashId = ECamFlashForced;     break;
    case CCamera::EFlashNone:         flashId = ECamFlashOff;        break;
    default: 
      break;
    }

  return flashId;
  }




// ---------------------------------------------------------------------------
// Map2CameraFormat
// ---------------------------------------------------------------------------
//
CCamera::TFormat
CCamSettingConversion::Map2CameraFormat( const TDisplayMode& aDisplayMode )
  {
  CCamera::TFormat format( CCamera::EFormatFbsBitmapColor16M );
  switch ( aDisplayMode )
    {
    case EColor16MA:       format = CCamera::EFormatFbsBitmapColor16MU;  break;
    case EColor16MU:       format = CCamera::EFormatFbsBitmapColor16MU;  break;
    case EColor4K:         format = CCamera::EFormatFbsBitmapColor4K;    break;
    case EColor16M:        format = CCamera::EFormatFbsBitmapColor16M;   break;
    case EColor64K:        format = CCamera::EFormatFbsBitmapColor64K;   break;
  //case EColor256:        format = CCamera::                            break;
  //case EColor16:         format = CCamera::                            break;
  //case EGray256:         format = CCamera::                            break;
  //case EGray16:          format = CCamera::                            break;
  //case EGray4:           format = CCamera::                            break;
  //case EGray2:           format = CCamera::                            break;
    default:                                                             break;
    }
  return format;
  }


// ---------------------------------------------------------------------------
// Map2CameraExposureMode
// ---------------------------------------------------------------------------
//
CCamera::TExposure
CCamSettingConversion::Map2CameraExposureMode( const TCamSceneAEMode& aExpModeId )
  {
  CCamera::TExposure expo( CCamera::EExposureAuto );

  switch ( aExpModeId )
    {
    case ECamSceneAEModeAuto:       expo = CCamera::EExposureAuto; break;
    case ECamSceneAEModeNight:      expo = CCamera::EExposureNight; break;
    case ECamSceneAEModeBacklight:  expo = CCamera::EExposureBacklight; break;
    case ECamSceneAEModeCenter:     expo = CCamera::EExposureCenter; break;
    case ECamSceneAEModeSports:     expo = CCamera::EExposureSport; break;
    default:
      break;
    }

  return expo;
  }




#ifdef CAMERAAPP_CAPI_V2_IP
// ---------------------------------------------------------------------------
// Map2CameraEffect
// ---------------------------------------------------------------------------
//
CCamera::CCameraImageProcessing::TEffect
CCamSettingConversion::Map2CameraEffect( const TCamColourFilterId& aColourFilterId )
  {
  CCamera::CCameraImageProcessing::TEffect effect(
            CCamera::CCameraImageProcessing::EEffectNone );

  switch( aColourFilterId )
    {
    case ECamColourFilterColour:        effect = CCamera::CCameraImageProcessing::EEffectNone;       break;
    case ECamColourFilterBlackAndWhite: effect = CCamera::CCameraImageProcessing::EEffectMonochrome; break;
    case ECamColourFilterSepia:         effect = CCamera::CCameraImageProcessing::EEffectSepia;      break;
    case ECamColourFilterNegative:      effect = CCamera::CCameraImageProcessing::EEffectNegative;   break;
    case ECamColourFilterVivid:         effect = CCamera::CCameraImageProcessing::EEffectVivid;      break;
    default:
      break;
    }
  return effect;
  }
#endif


// ---------------------------------------------------------------------------
// Map2CameraControllerSettingId
// ---------------------------------------------------------------------------
//
TCamCameraSettingId
CCamSettingConversion
::Map2CameraControllerSettingId( const TCamSettingItemIds& aUiSettingId )
  {
  switch( aUiSettingId )
    {
    // -------------------------------
    case ECamSettingItemPhotoDigitalZoom: 
    case ECamSettingItemVideoDigitalZoom:               return ECameraSettingDigitalZoom;
    // -------------------------------
    case ECamSettingItemPhotoCompression:               return ECameraSettingQualityFactor;
    // -------------------------------
    case ECamSettingItemDynamicPhotoExposure:
    case ECamSettingItemDynamicVideoExposure:           return ECameraSettingExposure;
    // -------------------------------
    case ECamSettingItemUserSceneExposure:              return ECameraUserSceneSettingExposure;
    // -------------------------------
    case ECamSettingItemDynamicPhotoWhiteBalance:
    case ECamSettingItemDynamicVideoWhiteBalance:       return ECameraSettingWhiteBalance;
    // -------------------------------
    case ECamSettingItemUserSceneWhitebalance:          return ECameraUserSceneSettingWhiteBalance;
    // -------------------------------
    case ECamSettingItemDynamicPhotoLightSensitivity:
    case ECamSettingItemDynamicVideoLightSensitivity:   return ECameraSettingLightSensitivity;
    // -------------------------------
    case ECamSettingItemUserSceneLightSensitivity:      return ECameraUserSceneSettingLightSensitivity;
    // -------------------------------
    case ECamSettingItemFaceTracking:                   return ECameraSettingFacetracking;
    // -------------------------------
    case ECamSettingItemDynamicPhotoFlash:
    case ECamSettingItemDynamicVideoFlash:              return ECameraSettingFlash;
    // -------------------------------
    case ECamSettingItemUserSceneFlash:                 return ECameraUserSceneSettingFlash;
    // -------------------------------
    case ECamSettingItemDynamicPhotoBrightness:
    case ECamSettingItemDynamicVideoBrightness:         return ECameraSettingBrightness;
    // -------------------------------
    case ECamSettingItemUserSceneBrightness:            return ECameraUserSceneSettingBrightness;
    // -------------------------------
    case ECamSettingItemDynamicPhotoContrast:
    case ECamSettingItemDynamicVideoContrast:           return ECameraSettingContrast;
    // -------------------------------
    case ECamSettingItemUserSceneContrast:              return ECameraUserSceneSettingContrast;
    // -------------------------------
    case ECamSettingItemDynamicPhotoImageSharpness:     return ECameraSettingSharpness;
    /* no video sharpness setting item available */
    case ECamSettingItemUserSceneImageSharpness:        return ECameraUserSceneSettingSharpness;
    // -------------------------------
    case ECamSettingItemDynamicPhotoColourFilter:
    case ECamSettingItemDynamicVideoColourFilter:       return ECameraSettingColourEffect;
    // -------------------------------
    case ECamSettingItemUserSceneColourFilter:          return ECameraUserSceneSettingColourEffect;
    // -------------------------------
    case ECamSettingItemVideoAudioRec:                  return ECameraSettingAudioMute;
    // -------------------------------
    case ECamSettingItemContinuousAutofocus:            return ECameraSettingContAF;  
    // -------------------------------
    case ECamSettingItemVideoClipLength:                return ECameraSettingFileMaxSize;
    // -------------------------------
    case ECamSettingItemVideoNameBase:
    case ECamSettingItemVideoNameBaseType:
    case ECamSettingItemVideoNumber:                    return ECameraSettingFileName;
    // -------------------------------
    case ECamSettingItemVideoStab:                      return ECameraSettingStabilization;
    case ECamSettingItemFlickerCancel:                  return ECameraSettingFlickerCancel;
    // -------------------------------
    // Not supported
    case ECamSettingItemDynamicSelfTimer:
    case ECamSettingItemDynamicPhotoColourSaturation:
    case ECamSettingItemDynamicVideoColourSaturation:
    case ECamSettingItemUserSceneColourSaturation:
    default:                                            return ECameraSettingNone;
    // -------------------------------
    }
  }


#if defined( CAMERAAPP_CAPI_V2_ADV ) || defined( CAMERAAPP_CAPI_V2_IP )
// ---------------------------------------------------------------------------
// MapCameraSetting2EventUidValue
// ---------------------------------------------------------------------------
//
TInt
CCamSettingConversion::Map2EventUidValue( const TCamCameraSettingId& aSettingId )
  {
  TInt uid( 0 );

  switch( aSettingId )
    {
    case ECameraSettingFlash:            uid = KUidECamEventCameraSettingFlashModeUidValue;                   break;
    case ECameraSettingExposure:         uid = KUidECamEventCameraSettingExposureCompensationStepUidValue;    break;
    case ECameraSettingLightSensitivity: uid = KUidECamEventCameraSettingIsoRateTypeUidValue;                 break;
    case ECameraSettingWhiteBalance:     uid = KUidECamEventCameraSettingWhiteBalanceModeUidValue;            break;
    case ECameraSettingColourEffect:     uid = KUidECamEventImageProcessingEffectUidValue;                    break;
    case ECameraSettingSharpness:        uid = KUidECamEventImageProcessingAdjustSharpnessUidValue;           break;
 
    case ECameraSettingDigitalZoom:      uid = KUidECamEventCameraSettingDigitalZoomUidValue;                 break;
    case ECameraSettingOpticalZoom:      uid = KUidECamEventCameraSettingOpticalZoomUidValue;                 break;

    case ECameraSettingFocusRange:       uid = KUidECamEventCameraSettingFocusRange2UidValue;                 break;
    case ECameraSettingStabilization:    uid = KUidECamEventSettingsStabilizationAlgorithmComplexityUidValue; break;


    // case ECameraSettingAudioMute:
    // case ECameraSettingFileMaxSize:
    // case ECameraSettingFileName:    
    // case ECameraSettingContrast:   
    // case ECameraSettingBrightness: 
    // case ECameraSettingQualityFactor
    // case ECameraSettingNone
    default:
      break;
    }

  return uid;
  }


// ---------------------------------------------------------------------------
// Map2CameraSetting
// ---------------------------------------------------------------------------
//
TCamCameraSettingId
CCamSettingConversion::Map2CameraSetting( TInt aUidValue )
  {
  TCamCameraSettingId id( ECameraSettingNone );

  switch( aUidValue )
    {
    case KUidECamEventCameraSettingFlashModeUidValue:                   id = ECameraSettingFlash;            break;
    case KUidECamEventCameraSettingExposureCompensationStepUidValue:    id = ECameraSettingExposure;         break;
    case KUidECamEventCameraSettingIsoRateTypeUidValue:                 id = ECameraSettingLightSensitivity; break;
    case KUidECamEventCameraSettingWhiteBalanceModeUidValue:            id = ECameraSettingWhiteBalance;     break;
    case KUidECamEventImageProcessingEffectUidValue:                    id = ECameraSettingColourEffect;     break;
    case KUidECamEventImageProcessingAdjustSharpnessUidValue:           id = ECameraSettingSharpness;        break;
    case KUidECamEventCameraSettingDigitalZoomUidValue:                 id = ECameraSettingDigitalZoom;      break;
    case KUidECamEventCameraSettingOpticalZoomUidValue:                 id = ECameraSettingOpticalZoom;      break;
    case KUidECamEventCameraSettingFocusRange2UidValue:                 id = ECameraSettingFocusRange;       break;
    case KUidECamEventSettingsStabilizationAlgorithmComplexityUidValue: id = ECameraSettingStabilization;    break;
    default:
      break;
    }

  return id;
  }

// ---------------------------------------------------------------------------
// CCamSettingConversion::Map2CameraAutofocus
// ---------------------------------------------------------------------------
//
CCamera::CCameraAdvancedSettings::TFocusRange
CCamSettingConversion::Map2CameraAutofocus( const TCamSceneAFMode& aAfMode )
  {
  switch( aAfMode )
    {
    case ECamSceneAFModeMacro:
      return CCamera::CCameraAdvancedSettings::EFocusRangeMacro;
    case ECamSceneAFPortrait:
      return CCamera::CCameraAdvancedSettings::EFocusRangePortrait;
    case ECamSceneAFModeInfinity:
      return CCamera::CCameraAdvancedSettings::EFocusRangeInfinite;
    case ECamSceneAFModeHyperfocal:
      return CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal;
      
    case ECamSceneAFModeContinuous: // Fall through              
      // Currently no support for continuous auto focus.
      // This case anyway needs to be handled differently as 
      // continuous auto focus is not a setting, but autofocus
      // type given when starting focusing.
    case ECamSceneAFModeNormal:     // Fall through
    default:
      return CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
    }
  
  }

#endif // defined( CAMERAAPP_CAPI_V2_ADV ) || defined( CAMERAAPP_CAPI_V2_IP )

// ---------------------------------------------------------------------------
// CCamSettingConversion::Map2CameraOrientation
// ---------------------------------------------------------------------------
//
MCameraOrientation::TOrientation 
CCamSettingConversion::Map2CameraOrientation( 
    const TCamImageOrientation& aSettingOrientation )
  {
  switch( aSettingOrientation )
    {
    case ECamOrientation90:   return MCameraOrientation::EOrientation90;
    case ECamOrientation180:  return MCameraOrientation::EOrientation180;
    case ECamOrientation270:  return MCameraOrientation::EOrientation270;
    case ECamOrientation0:    // <<fall through>>
    default:                  return MCameraOrientation::EOrientation0;
    }
  }



// ===========================================================================
// end of file
