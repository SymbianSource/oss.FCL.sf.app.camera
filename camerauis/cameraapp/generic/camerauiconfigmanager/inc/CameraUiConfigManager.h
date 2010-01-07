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
* Description:  CameraUiConfigManager API header.
 *
*/


#ifndef CAMERAUICONFIGMANAGER_H
#define CAMERAUICONFIGMANAGER_H

// INCLUDES

#include <eikenv.h>


// Photo/Video scene information
struct TSceneSettings
  {
  // Used for both image and video scene configuration.
  TInt iSupported;
  TInt iIdentifier;
  TInt iExposureMode;
  TInt iWhiteBalance;
  TInt iFlash;
  TInt iContrastMode;
  TInt iFocalDistance;
  /*
  * The following 3 only used for image mode. In Video mode, we ignore this
  */
  TInt iSharpnessMode; 
  TInt iLightSensitivityMode;
  TInt iExposureCompensationValue;
  /*
  * 0 -> Read from ICM or else.. take the frame rate defined here into use
  * only used in video. Ingored in image mode.
  */
  TInt iFrameRate;  
  };


class CCameraUiConfigManagerImp;

// CCameraUiConfigManager API 

class CCameraUiConfigManager : public CBase
  {
  // =========================================================================
  public: // Constructors and destructor
  
        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aConfiguration    
        * engine changes.
        */
        IMPORT_C static CCameraUiConfigManager* NewLC();
      
        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aConfiguration    
        * engine changes.
        */
        IMPORT_C static CCameraUiConfigManager* NewL();
        
        /*
        * Constructor
        */
        CCameraUiConfigManager();
        
        /**
        * Destructor.
        */
        ~CCameraUiConfigManager();
    
 public:
 
        /*
        * Checking for Color feature support
        */
        IMPORT_C TBool IsColorToneFeatureSupported() const;
        
        /*
        * To check for sharpness feature support
        */
        IMPORT_C TBool IsSharpnessFeatureSupported() const;
        
        /*
        * To check for white balance feature support
        */
        IMPORT_C TBool IsWhiteBalanceFeatureSupported() const;
        
        /*
        * Secondary camera support
        */
        IMPORT_C TBool IsSecondaryCameraSupported() const;
        
        /*
        * Direct Screen View Finder feature support
        * aPrimaryCamera = ETrue ( Direct screen access support for primary camera )
        * aPrimaryCamera = EFalse ( Direct screen access support for secondary camera )
        */
        IMPORT_C TBool IsDSAViewFinderSupported( TBool aPrimaryCamera = ETrue ) const;
        
        /*
        * Lens cover feature support
        */
        IMPORT_C TBool IsLensCoverSupported() const;
        
        /*
        * Xenon flash feature support
        */
        IMPORT_C TBool IsXenonFlashSupported() const;
        
        /*
         * Light sensitivity feature support
        */
        IMPORT_C TBool IsISOSupported() const;
            
        /*
        * EV feature support
        */
        IMPORT_C TBool IsEVSupported() const;
        
        /*
        * Flash mode feature support
        */
        IMPORT_C TBool IsFlashModeSupported() const;
    
        
        /*
        * Contrast feature support
        */
        IMPORT_C TBool IsContrastSupported() const;
        
        /*
        * Brightness feature support
        */
        IMPORT_C TBool IsBrightnessSupported() const;
        
        
        /*
        * Flash mode feature support
        */
        IMPORT_C TBool IsExtendedLightSensitivitySupported() const;
        
        
        /*
        * One click upload feature support
        */
        IMPORT_C TBool IsOneClickUploadSupported() const;
        
        
        /*
        * Video Light feature support
        */
        IMPORT_C TBool IsVideoLightSupported() const;
        
        
        /*
        * Auto focus feature support
        */
        IMPORT_C TBool IsAutoFocusSupported() const;
    
        /*
        * Video Stabilization feature support
        */
        IMPORT_C TBool IsVideoStabilizationSupported() const;
        
        /*
        * Orientation sensor feature support
        */
        IMPORT_C TBool IsOrientationSensorSupported() const;
        
        /*
        * Face-tracking feature support
        */
        IMPORT_C TBool IsFaceTrackingSupported() const;
        
        /*
        * Optical joy stick feature support
        */
        IMPORT_C TBool IsOpticalJoyStickSupported() const;

        /*
        * location feature support
        */
        IMPORT_C TBool IsLocationSupported() const;
        
        /*
        * keywatcher feature support
        */
        IMPORT_C TBool IsKeyLockWatcherSupported() const;
        
        /*
        * Extended Digital Zoom feature support
        */
        IMPORT_C TBool IsExtendedDigitalZoomSupported() const;

        /*
        * MC photos support available
        */
        IMPORT_C TBool IsPhotosSupported() const;

        /*
        * Publish Zoom State support available
        */
        IMPORT_C TBool IsPublishZoomStateSupported() const;
        
        /*
        * EFalse = HW/Adapation sets AF to Hyperfocal during startup
        * ETrue  = Should be handled in UI
        */
        IMPORT_C TBool IsAutofocusSetInHyperfocalAtStartup() const;

        /*
        * Capture tone delay support available
        */
        IMPORT_C TBool IsCaptureToneDelaySupported() const;

        /*
        * UI Orientation Override feature support
        */
        IMPORT_C TBool IsUIOrientationOverrideSupported() const;
        
        /*
        * ThumbnailManager API feature support
        */
        IMPORT_C TBool IsThumbnailManagerAPISupported() const;
        
        /*
        * Capture tone delay in milliseconds.
        */
        IMPORT_C TInt CaptureToneDelay() const;

        /*
        * Required RAM memory for CameraApp
        */
        IMPORT_C TInt RequiredRamMemory() const;
        
        /*
        * Critical Level RAM memory for CameraApp
        */
        IMPORT_C TInt CriticalLevelRamMemory() const;

        /*
        * Required RAM memory for CameraApp when focus gained
        */
        IMPORT_C TInt RequiredRamMemoryFocusGained() const;
        
        /*
        * Critical Level RAM memory for CameraApp when focus gained
        */
        IMPORT_C TInt CriticalLevelRamMemoryFocusGained() const;

        /*
        *  To get all supported color tone setting items
        * aParam: aSupportedValues contain colortone setting enums supported by camera ui
        * check camsettings.hrh for supported enums in camera ui
        */
        IMPORT_C void SupportedColorToneSettingsL( RArray<TInt>& aSupportedValues );
        
        /*
        * To get all supported white balance setting items
        * aParam: aSupportedValues contain white balance setting enums supported by camera ui
        * check camsettings.hrh for supported enums in camera ui
        */
        IMPORT_C void SupportedWhiteBalanceSettingsL( 
                                    RArray<TInt>& aSupportedValues );
        
        /*
        * To get all supported Sharpness setting items
        * @aSupportedValues contains values ranging from MIN to MAX
        */
        IMPORT_C void SupportedSharpnessSettingsL( RArray<TInt>& aSupportedValues );

        /*
        * Primary Camera Zoom-In Keys
        * aParam: aSupportedValues will have key scan codes configured via cenrep
        */
        IMPORT_C void SupportedPrimaryCameraZoomInKeysL( RArray<TInt>& aSupportedValues );
    
        /*
        * Primary Camera Zoom-Out Keys
        * aParam: aSupportedValues will have key scan codes configured via cenrep
        */
        IMPORT_C void SupportedPrimaryCameraZoomOutKeysL( RArray<TInt>& aSupportedValues );
    
        /*
        * Secondary Camera Zoom-In Keys
        * aParam: aSupportedValues will have key scan codes configured via cenrep
        */
        IMPORT_C void SupportedSecondaryCameraZoomInKeysL( RArray<TInt>& aSupportedValues );
    
        /*
        * Secondary Camera Zoom-Out Keys
        * aParam: aSupportedValues will have key scan codes configured via cenrep
        */
        IMPORT_C void SupportedSecondaryCameraZoomOutKeysL( RArray<TInt>& aSupportedValues );
        
        /*
        * EV compensation supported settings
        * aSupportedValues contains the complete range of EV settings from MIN to MAX
        * @e.g. -2,2,10 ( aSupportedValues[0] = MIN,
        *                 aSupportedValues[1] = MAX,
        *                 aSupportedValues[2] = Steps )
        */
        IMPORT_C void SupportedEVSettingsL( RArray<TInt>& aSupportedValues );
    
    
        /*
        * Flash mode setting items
        * aParam: aSupportedValues contain flash mode enums supported by camera ui
        * check camsettings.hrh for supported enums in camera ui
        */
        IMPORT_C void SupportedFlashModeSettingsL( RArray<TInt>& aSupportedValues );

        
        /*
        * Primary camera capture key
        * aParam: aSupportedValues will have key mappings for capture key
        */
        IMPORT_C void SupportedPrimaryCameraCaptureKeyL( RArray<TInt>& aSupportedValues );
    
        /*
        * Secondary camera capture key
        * aParam: aSupportedValues will have key mappings for capture key
        */
        IMPORT_C void SupportedSecondaryCameraCaptureKeyL( RArray<TInt>& aSupportedValues );
        
        /*
        * Primary camera AF key
        * aParam: aSupportedValues will have key mappings for Autofocus key
        */
        IMPORT_C void SupportedPrimaryCameraAutoFocusKeyL( RArray<TInt>& aSupportedValues );
    
        /*
        * Secondary camera AutoFocus key
        * aParam: aSupportedValues will have key mappings for Autofocus key
        */
        IMPORT_C void SupportedSecondaryCameraAutoFocusKeyL( RArray<TInt>& aSupportedValues );
        
        /*
        * Supported contrast setting range values
        * aParam: aSupportedValues will have contrast values ranging 
        * from MIN - MAX and step size
        * aSupportedValues[0] -> MIN value
        * aSupportedValues[1] -> MAX value
        * aSupportedValues[2] -> Steps
        */
        IMPORT_C void SupportedContrastRangeL( RArray<TInt>& aSupportedValues );
        
        /*
        * Supported brightness setting range values
        * aParam: aSupportedValues will have brightness values ranging 
        * from MIN - MAX and step size
        * aSupportedValues[0] -> MIN value
        * aSupportedValues[1] -> MAX value
        * aSupportedValues[2] -> Steps
        */
        IMPORT_C void SupportedBrightnessRangeL( RArray<TInt>& aSupportedValues );
        
        /*
        *  To get all supported Max Zoom steps for all Image, Video resoultions for
        *  primary and second camera
        *  @ aSupportedValues[ 0] -> KMaxPhoto8MPZoomStep
        *  @ aSupportedValues[ 1] -> KMaxPhoto5MPZoomStep
        *  @ aSupportedValues[ 2] -> KMaxPhoto3MPZoomStep
        *  @ aSupportedValues[ 3] -> KMaxPhoto2MPZoomStep
        *  @ aSupportedValues[ 4] -> KMaxPhoto1_3MPZoomStep
        *  @ aSupportedValues[ 5] -> KMaxPhoto1MPZoomStep
        *  @ aSupportedValues[ 6] -> KMaxPhoto08MPZoomStep
        *  @ aSupportedValues[ 7] -> ?? no constant in UI
        *  @ aSupportedValues[ 8] -> KMaxPhotoVGAZoomStep
        *  @ aSupportedValues[ 9] -> KMaxVideoZoomStepHD
        *  @ aSupportedValues[10] -> KMaxVideoZoomStepWVGA
        *  @ aSupportedValues[11] -> KMaxVideoZoomStepNHD
        *  @ aSupportedValues[12] -> KMaxVideoZoomStepVGA
        *  @ aSupportedValues[13] -> KMaxVideoZoomStepQVGA
        *  @ aSupportedValues[14] -> KMaxVideoZoomStepCIF
        *  @ aSupportedValues[15] -> KMaxVideoZoomStepQCIF
        *  @ aSupportedValues[16] -> iMaxVideoStepSQCIF
        *  @ aSupportedValues[17] -> KMax2ndCamPhotoStep
        *  @ aSupportedValues[18] -> KMax2ndCamVideoStepCIF
        *  @ aSupportedValues[19] -> KMax2ndCamVideoStepQCIF
        *  @ aSupportedValues[20] -> iMax2ndCamVideoStepSQCIF
        */
        IMPORT_C void SupportedMaxZoomStepsL( RArray<TInt>& aSupportedValues );
        
        /*
        * To get all supported photo specific MAX extended zoom steps w.r.t all photo
        * resoultions ( only for main camera for now )
        * aSupportedValues[0] -> 5MP max ext-zoom step
        * aSupportedValues[n] -> lowest resolution for main camera MAX ext-zoom step
        */
        IMPORT_C void SupportedMaxExtendedZoomStepsL( 
                                    RArray<TInt>& aSupportedValues );
        /*
        * Supported zoom bar Look and Feel ( LAF ) values
        * @aSupportedValues[0] -> Time per zoom tick in milliseconds
        * @aSupportedValues[1] -> Time per extended zoom tick in milliseconds 
        * @aSupportedValues[2] -> No of zoom steps per tick
        * @aSupportedValues[3] -> No of extended zoom steps per tick 
        * @aSupportedValues[4] -> Secondary camera zoom steps per tick
        */
        IMPORT_C void SupportedLAFZoomBarL( 
                                    RArray<TInt>& aSupportedValues );
        
        /*
        * Supported Image Scene modes and Scene specific settings
        */
        IMPORT_C void SupportedSceneModesAndSettingsL( 
                               RArray<TSceneSettings>& aSupportedValues,
                               TBool aPhotoScene = ETrue );

        /*
        * Determines device qwerty-mode state.
        * This method is meant to be used with secondary camera.
        * @return ETrue if qwerty keyboard is open
        */
        IMPORT_C TInt IsQwerty2ndCamera() const;  

        /*
        * Burst file size estimate correction factor
        * in percent (100 = 100%).
        * @return correction factor in percent
        */        
        IMPORT_C TInt BurstFileSizeEstimateFactor() const;       
        
        /*
        * @aSupportedValues[0] -> Value for iCamZoomCooldown
        * @aSupportedValues[1] -> Value for iCamMaxZoomCooldown
        */
        IMPORT_C void SupportedZoomDelaysL( 
                                    RArray<TInt>& aSupportedValues );
        /*
         * BurstMode Support
         */
        IMPORT_C TBool IsBurstModeSupported() const; 
		
		 /*
        * Continuous autofocus. 
        * @return ETrue if continuous autofocus feature is enabled
        */
        IMPORT_C TBool IsContinuosAutofocusSupported() const; 
        
 private:
        void ConstructL();
  
 private:
 
        CCameraUiConfigManagerImp* iConfigManagerImp;
};

#endif // CAMERAUICONFIGMANAGER_H

// End of File
