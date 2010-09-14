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
* Description:  CameraUiConfigManager API.
 *
*/


// Includes

#include "CameraUiConfigManager.h"
#include "CameraUiConfigManagerImp.h"
#include "CameraDynamicSettings.hrh"


// Class methods

// ---------------------------------------------------------------------------
// CCameraUiConfigManager::NewL
// Symbian OS two-phased constructor 
// ---------------------------------------------------------------------------
//
EXPORT_C CCameraUiConfigManager* CCameraUiConfigManager::NewL()
    {
    CCameraUiConfigManager* self = CCameraUiConfigManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCameraUiConfigManager::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CCameraUiConfigManager* CCameraUiConfigManager::NewLC()
    {
    CCameraUiConfigManager* self = new( ELeave ) CCameraUiConfigManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CCameraUiConfigManager::~CCameraUiConfigManager
// Destructor
// ---------------------------------------------------------------------------
//
CCameraUiConfigManager::~CCameraUiConfigManager()
  {
  delete iConfigManagerImp;
  }
  

// ---------------------------------------------------------------------------
// CCameraUiConfigManager::CCameraUiConfigManager
// C++ constructor
// ---------------------------------------------------------------------------
//
CCameraUiConfigManager::CCameraUiConfigManager() 
  {    
  }
  

// ---------------------------------------------------------------------------
// CCameraUiConfigManager::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCameraUiConfigManager::ConstructL()
    {
    iConfigManagerImp = CCameraUiConfigManagerImp::NewL();
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsColorToneFeatureSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsColorToneFeatureSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingColorTone );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsSharpnessFeatureSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsSharpnessFeatureSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingSharpness );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsWhiteBalanceFeatureSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsWhiteBalanceFeatureSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingWhiteBalance );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsSecondaryCameraSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsSecondaryCameraSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingSecondaryCamera );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsDSAViewFinderSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsDSAViewFinderSupported( TBool aPrimaryCamera ) const
    {
    RArray<TInt> supportedItems;
    CleanupClosePushL( supportedItems );
    TBool supported ( EFalse );
    TRAPD( err,iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingDSAViewFinder,
                                                          supportedItems ) );
    if ( KErrNone == err && supportedItems.Count() > 0  )
        {
        if ( aPrimaryCamera ) supported = supportedItems[0];
        else supported = supportedItems[1];
        }
    CleanupStack::PopAndDestroy( &supportedItems );
    return supported; // not supported
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsLensCoverSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsLensCoverSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingLensCover );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsXenonFlashSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsXenonFlashSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingXenonFlash );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsISOSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsISOSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingLightSensitivity );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsEVSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsEVSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingEV );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsFlashModeSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsFlashModeSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingFlashMode );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsContrastSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsContrastSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingContrast );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsBrightnessSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsBrightnessSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingBrightness );
    }



// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsExtendedLightSensitivitySupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsExtendedLightSensitivitySupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( 
                                    ECamDynamicSettingExtLightSensitivity );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsOneClickUploadSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsOneClickUploadSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingOneClickUpload );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsVideoLightSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsVideoLightSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingVideoLight );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsAutoFocusSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsAutoFocusSupported() const
    {
#if defined(__WINS__) || defined(__WINSCW__)
    return EFalse; // return
#else
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingAutoFocus );
#endif
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsVideoStabilizationSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsVideoStabilizationSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingVideoStabilization );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsFaceTrackingSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsFaceTrackingSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingFaceTracking );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsOpticalJoyStickSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsOpticalJoyStickSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingOpticalJoyStick );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsOrientationSensorSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsOrientationSensorSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingOrientationSensor );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsLocationSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsLocationSupported() const
    {
    TBool supported = iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingLocation );
#if !defined(RD_MDS_2_5) || defined(__WINSCW__) || defined(__WINS__)
    supported = EFalse;
#endif
    return supported;
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsKeyLockWatcherSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsKeyLockWatcherSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingKeyLockWatcher );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsExtendedDigitalZoomSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsExtendedDigitalZoomSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingExtDigitalZoom );
    }


// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsAutofocusSetInHyperfocalAtStartup
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsAutofocusSetInHyperfocalAtStartup() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingExtDigitalZoom );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsCaptureToneDelaySupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsCaptureToneDelaySupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingCaptureToneDelaySupport );
    }

/*
* The capture tone delay in milliseconds
*/
EXPORT_C TInt CCameraUiConfigManager::CaptureToneDelay() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingCaptureToneDelayValue );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsPhotosSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsPhotosSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingPhoto );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsPublishZoomStateSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsPublishZoomStateSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingExtDigitalZoom );
    }



/*
* Required RAM memory for CameraApp
*/
EXPORT_C TInt CCameraUiConfigManager::RequiredRamMemory() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingRequiredRamMemory );
    }

/*
* Critical Level RAM memory for CameraApp
*/
EXPORT_C TInt CCameraUiConfigManager::CriticalLevelRamMemory() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingCriticalLevelRamMemory );
    }


/*
* Required RAM memory for CameraApp when focus gained
*/
EXPORT_C TInt CCameraUiConfigManager::RequiredRamMemoryFocusGained() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingRequiredRamMemoryFocusGained );
    }


/*
* Critical Level RAM memory for CameraApp when focus gained
*/
EXPORT_C TInt CCameraUiConfigManager::CriticalLevelRamMemoryFocusGained() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingCriticalLevelRamMemoryFocusGained );
    }


//
//CCameraUiConfigManager::SupportedPrimaryCameraCaptureKeyL
//Returns configured capture key for primary camera
//
EXPORT_C void CCameraUiConfigManager::SupportedPrimaryCameraCaptureKeyL(
                                        RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingPCCaptureKeys,
                                               aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedSecondaryCameraCaptureKeyL
//Returns configured capture key for secondary camera
EXPORT_C void CCameraUiConfigManager::SupportedSecondaryCameraCaptureKeyL(
                                        RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingSCCaptureKeys,
                                               aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedColorToneSettingsL
//
EXPORT_C void CCameraUiConfigManager::SupportedColorToneSettingsL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( 
                       ECamDynamicSettingColorTone,
                       aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedWhiteBalanceSettingsL
//
EXPORT_C void CCameraUiConfigManager::SupportedWhiteBalanceSettingsL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingWhiteBalance,
                       aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedSharpnessSettingsL
//
EXPORT_C void CCameraUiConfigManager::SupportedSharpnessSettingsL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingSharpness,
                       aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedPrimaryCameraZoomInKeysL
//
EXPORT_C void CCameraUiConfigManager::SupportedPrimaryCameraZoomInKeysL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingPCZoomIn,
                       aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedPrimaryCameraZoomOutKeysL
//
EXPORT_C void CCameraUiConfigManager::SupportedPrimaryCameraZoomOutKeysL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingPCZoomOut,
                       aSupportedValues );
    }
    

//
//CCameraUiConfigManager::SupportedSecondaryCameraZoomInKeysL
//
EXPORT_C void CCameraUiConfigManager::SupportedSecondaryCameraZoomInKeysL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingSCZoomIn,
                       aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedSecondaryCameraZoomOutKeysL
//
EXPORT_C void CCameraUiConfigManager::SupportedSecondaryCameraZoomOutKeysL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingSCZoomOut,
                       aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedEVSettingsL
//
EXPORT_C void CCameraUiConfigManager::SupportedEVSettingsL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingEV,
                       aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedFlashModeSettingsL
//
EXPORT_C void CCameraUiConfigManager::SupportedFlashModeSettingsL( 
                                    RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL(
                       ECamDynamicSettingFlashMode,
                       aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedPrimaryCameraCaptureKeyL
//Returns configured AutoFocus key for primary camera
//
EXPORT_C void CCameraUiConfigManager::SupportedPrimaryCameraAutoFocusKeyL( 
                                     RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingPCAutoFocusKeys,
                                               aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedSecondaryCameraAutoFocusKeyL
//Returns configured AutoFocus key for secondary camera
EXPORT_C void CCameraUiConfigManager::SupportedSecondaryCameraAutoFocusKeyL(
                                     RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingSCAutoFocusKeys,
                                               aSupportedValues );
    }


//
//CCameraUiConfigManager::SupportedContrastRangeL
//Returns configured range for contrast setting
EXPORT_C void CCameraUiConfigManager::SupportedContrastRangeL(
                                     RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingContrast,
                                               aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedBrightnessRangeL
//Returns configured range for brightness setting
EXPORT_C void CCameraUiConfigManager::SupportedBrightnessRangeL(
                                     RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingBrightness,
                                               aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedMaxZoomStepsL
//

EXPORT_C void CCameraUiConfigManager::SupportedMaxZoomStepsL( 
                                        RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingMaxDigitalZoomLimits,
                                               aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedMaxExtendedZoomStepsL
//

EXPORT_C void CCameraUiConfigManager::SupportedMaxExtendedZoomStepsL( 
                                        RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingMaxExtendedZoomLimits,
                                               aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedLAFZoomBarL
//
EXPORT_C void CCameraUiConfigManager::SupportedLAFZoomBarL( 
                                        RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingZoomLAF,
                                               aSupportedValues );
    }

//
//CCameraUiConfigManager::SupportedSceneModesAndSettingsL
//
EXPORT_C void CCameraUiConfigManager::SupportedSceneModesAndSettingsL( 
                                            RArray<TSceneSettings>& aSupportedValues,
                                            TBool aPhotoScene )
    {
    aSupportedValues.Reset();
    // Load all known scene mode specific settings
    RArray<TInt> sceneModeSettings;
    if ( aPhotoScene )
        {
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModeAuto,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModeUser,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModeMacro,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModePotrait,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModeScenery,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModeSports,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModeNight,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingImageSceneModeNightPotrait,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );        
        }
    else
        {
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingVideoSceneModeNormal,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );
        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingVideoSceneModeNight,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );        
        iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingVideoSceneModeLowLight,
                                                   sceneModeSettings );
        iConfigManagerImp->OrganiseSceneSettings( aSupportedValues, 
                                                   sceneModeSettings,aPhotoScene );        
        }
    sceneModeSettings.Close();
    }

//
//CCameraUiConfigManager::IsQwerty2ndCamera
//
EXPORT_C TInt CCameraUiConfigManager::IsQwerty2ndCamera() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSetting2ndCameraOrientation );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsUIOrientationOverrideSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsUIOrientationOverrideSupported() const
    {
#if defined(__WINS__) || defined(__WINSCW__)
    return EFalse;
#else    
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingUIOrientationOverride );
#endif    
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsThumbnailManagerAPISupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsThumbnailManagerAPISupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingThumbnailManagerAPI );
    }
	
// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::BurstFileSizeEstimateFactor
// ----------------------------------------------------------------------------------
//    
EXPORT_C TInt CCameraUiConfigManager::BurstFileSizeEstimateFactor() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingBurstFileSizeEstimateFactor );
    }	

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::SupportedZoomDelays
// ----------------------------------------------------------------------------------
//    
EXPORT_C void CCameraUiConfigManager::SupportedZoomDelaysL( 
                                        RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingZoomDelays,
                                               aSupportedValues );
    }

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::SupportedBurstMode
// ----------------------------------------------------------------------------------
//    
EXPORT_C TBool CCameraUiConfigManager::IsBurstModeSupported() const 
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingBurstMode );
    }   
	
// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsContinuosAutofocus  
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsContinuosAutofocusSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingContinuousAF );
    }
	
// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::SupportedScreenModesL
// ----------------------------------------------------------------------------------
//    
EXPORT_C void CCameraUiConfigManager::SupportedScreenModesL( 
                                        RArray<TInt>& aSupportedValues )
    {
    iConfigManagerImp->SupportedSettingItemsL( ECamDynamicSettingScreenModes,
                                               aSupportedValues );
    }	

// ----------------------------------------------------------------------------------
// CCameraUiConfigManager::IsCustomCaptureButtonSupported
// ----------------------------------------------------------------------------------
//
EXPORT_C TBool CCameraUiConfigManager::IsCustomCaptureButtonSupported() const
    {
    return iConfigManagerImp->IsFeatureSupported( ECamDynamicSettingCustomCaptureButton );
    }
//End of File
