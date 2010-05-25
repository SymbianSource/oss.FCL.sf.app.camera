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
* Description:  Cr keys definitions header.
 *
*/


#ifndef CAMERACONFIGURATIONCRKEYS_H
#define CAMERACONFIGURATIONCRKEYS_H


// =========================================================
// Camera dynamic configuration Settings

const TUid KCameraDynamicConfigurationCrKeys =  {0x2001B29B};


// Cr key for Settings which does not have configured setting items 
// ( checking feature supported or not )

const TUint32 KCamCrFeatureSecondaryCamera                  = 0x001;
const TUint32 KCamCrFeatureDSAViewFinder                    = 0x002;
const TUint32 KCamCrFeatureLensCover                        = 0x003;
const TUint32 KCamCrFeatureXenonFlash                       = 0x004;
const TUint32 KCamCrFeatureExtLightSensitivity              = 0x005;
const TUint32 KCamCrFeatureFaceTracking                     = 0x006;
const TUint32 KCamCrFeatureOpticalJoyStick                  = 0x007;
const TUint32 KCamCrFeatureOneClickUpLoad                   = 0x008;
const TUint32 KCamCrFeatureVideoLight                       = 0x009;
const TUint32 KCamCrFeatureAutoFocus                        = 0x010;
const TUint32 KCamCrFeatureVideoStabilization               = 0x011;
const TUint32 KCamCrFeatureOrientationSensor                = 0x012;
const TUint32 KCamCrFeatureLightSensitivity                 = 0x013;
const TUint32 KCamCrFeatureLocation                         = 0x014;
const TUint32 KCamCrFeatureKeyLockWatcher                   = 0x015;
const TUint32 KCamCrFeatureExtDigitalZoom                   = 0x016;
const TUint32 KCamCrFeatureKeyPhotos                        = 0x017;
const TUint32 KCamCrFeatureKeyPublishZoomSate               = 0x018;
const TUint32 KCamCrFeatureKeyAFSetInHyperfocalAtStartup    = 0x019;
const TUint32 KCamCrFeatureRequiredRamMemory                = 0x020;
const TUint32 KCamCrFeatureCriticalLevelRamMemory           = 0x021;
const TUint32 KCamCrFeatureRequiredRamMemoryFocusGained     = 0x022;
const TUint32 KCamCrFeatureCriticalLevelRamMemoryFocusGained= 0x023;
const TUint32 KCamCrFeatureCaptureToneDelaySupport          = 0x024;
const TUint32 KCamCrFeatureCaptureToneDelayValue            = 0x025;
const TUint32 KCamCrFeature2ndCameraOrientation             = 0x026; 
const TUint32 KCamCrFeatureUIOrientationOverride            = 0x027;
const TUint32 KCamCrFeatureThumbnailManagerAPI              = 0x028;
const TUint32 KCamCrFeatureBurstMode                        = 0x029;
const TUint32 KCamCrFeatureContinuousAF                     = 0x01E;  
const TUint32 KCamCrFeatureCustomCaptureButton              = 0x01F;
// Cr keys reserved for future use 0x020 -- 0x029

// Cr key for Settings which only have configured setting items
// ( feature always supported )

const TUint32 KCamCrFeaturePCZoomInItems                    = 0x030;
const TUint32 KCamCrFeaturePCZoomOutItems                   = 0x031;
const TUint32 KCamCrFeatureSCZoomInItems                    = 0x032;
const TUint32 KCamCrFeatureSCZoomOutItems                   = 0x033;
const TUint32 KCamCrFeaturePCCaptureKeys                    = 0x034;
const TUint32 KCamCrFeatureSCCaptureKeys                    = 0x035;
const TUint32 KCamCrFeaturePCAutoFocusKeys                  = 0x036;
const TUint32 KCamCrFeatureSCAutoFocusKeys                  = 0x037;
const TUint32 KCamCrFeatureMaxDigitalZoomLimits             = 0x038;
const TUint32 KCamCrFeatureMaxExtendedZoomLimits            = 0x039;
const TUint32 KCamCrFeatureMaxOpticalZoomLimits             = 0x040;
const TUint32 KCamCrFeatureZoomLAF                          = 0x041;
const TUint32 KCamCrFeatureImageSceneModeAuto               = 0x042;
const TUint32 KCamCrFeatureImageSceneModeUser               = 0x043;
const TUint32 KCamCrFeatureImageSceneModeMacro              = 0x044;
const TUint32 KCamCrFeatureImageSceneModePotrait            = 0x045;
const TUint32 KCamCrFeatureImageSceneModeScenery            = 0x046;
const TUint32 KCamCrFeatureImageSceneModeSports             = 0x047;
const TUint32 KCamCrFeatureImageSceneModeNight              = 0x048;
const TUint32 KCamCrFeatureImageSceneModeNightPotrait       = 0x049;
const TUint32 KCamCrFeatureBurstFileSizeEstimateFactor      = 0x04A;
const TUint32 KCamCrFeatureZoomDelays                       = 0x04B;
const TUint32 KCamCrFeatureScreenModes                      = 0x04C;

// Cr key for Settings where we check both for feature support 
// And configured setting items

const TUint32 KCamCrFeatureColorTone                        = 0x050;
const TUint32 KCamCrFeatureColorToneItems                   = 0x051;

const TUint32 KCamCrFeatureWhiteBalance                     = 0x052;
const TUint32 KCamCrFeatureWhiteBalanceItems                = 0x053;

const TUint32 KCamCrFeatureEV                               = 0x054;
const TUint32 KCamCrFeatureEVItems                          = 0x055;

const TUint32 KCamCrFeatureFlashMode                        = 0x056;
const TUint32 KCamCrFeatureFlashModeItems                   = 0x057;

const TUint32 KCamCrFeatureSharpness                        = 0x058;
const TUint32 KCamCrFeatureSharpnessItems                   = 0x059;

const TUint32 KCamCrFeatureContrast                         = 0x060;
const TUint32 KCamCrFeatureContrastItems                    = 0x061;

const TUint32 KCamCrFeatureBrightness                       = 0x062;
const TUint32 KCamCrFeatureBrightnessItems                  = 0x063;

const TUint32 KCamCrFeatureVideoSceneNormal                 = 0x064;
const TUint32 KCamCrFeatureVideoSceneModeNight              = 0x065;
const TUint32 KCamCrFeatureVideoSceneModeLowLight           = 0x066;

#endif      // CAMERACONFIGURATIONCRKEYS_H
          
// End of File
