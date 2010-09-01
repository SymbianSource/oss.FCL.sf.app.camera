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
* Description:  Cameraapp CenRep keys*
*/


#ifndef CAMERAAPPPRIVATECRKEYS_H
#define CAMERAAPPPRIVATECRKEYS_H


// =========================================================
// Cameraapp Settings
const TUid KCRUidCameraappSettings = {0x101FFA86};

const TUint32 KCamCrDefaultCaptureMode      = 0x00;
const TUint32 KCamCrPhotoQuality            = 0x01;
const TUint32 KCamCrPhotoSize               = 0x02;
const TUint32 KCamCrFocusPoint              = 0x03;   
const TUint32 KCamCrPhotoStoreAlbum         = 0x04;
const TUint32 KCamCrPhotoShowCaptured       = 0x05;
const TUint32 KCamCrPhotoMemInUse           = 0x06;
const TUint32 KCamCrPhotoNameBase           = 0x07;
const TUint32 KCamCrPhotoNameType           = 0x08;
const TUint32 KCamCrPhotoImgCount           = 0x09;
const TUint32 KCamCrPhotoExtDigZoom         = 0x0A;
const TUint32 KCamCrPhotoCaptureTone        = 0x0B;
const TUint32 KCamCrPhotoFaceTracking       = 0x0C;

const TUint32 KCamCrVideoAudRec             = 0x10;
const TUint32 KCamCrVideoRes                = 0x11;
const TUint32 KCamCrVideoClipLen            = 0x12;
const TUint32 KCamCrVideoFileType           = 0x13;
const TUint32 KCamCrVideoStoreAlbum         = 0x14;
const TUint32 KCamCrVideoShowCaptured       = 0x15;
const TUint32 KCamCrVideoMemInUse           = 0x16;
const TUint32 KCamCrVideoNameBase           = 0x17;
const TUint32 KCamCrVideoQuality            = 0x18;
const TUint32 KCamCrVideoNameType           = 0x19;
const TUint32 KCamCrVideoCount              = 0x1A;
const TUint32 KCamCrVideoExtDigZoom         = 0x1B;
const TUint32 KCamCrEmbeddedVideoAudRec     = 0x1D;
const TUint32 KCamCrContinuousAutofocus     = 0x1E; 

const TUint32 KCamCrUserSceneBaseScene      = 0x20;
const TUint32 KCamCrUserSceneWhiteBalance   = 0x21;
const TUint32 KCamCrUserSceneColourFilter   = 0x22;
const TUint32 KCamCrUserSceneExposure       = 0x23; 
const TUint32 KCamCrUserSceneFlash          = 0x24;  
const TUint32 KCamCrUserSceneBrightness     = 0x25;
const TUint32 KCamCrUserSceneContrast       = 0x26;
const TUint32 KCamCrUserSceneImageSharpness = 0x27;
const TUint32 KCamCrUserSceneColourSaturation = 0x28;
const TUint32 KCamCrUserSceneLightSensitivity = 0x29;
const TUint32 KCamCrUserSceneDefault        = 0x2A; 

const TUint32 KCamCrPhotoRotation 			= 0x30;
const TUint32 KCamCrVideoStabilisation		= 0x31;
const TUint32 KCamCrFlickerCancellation		= 0x32;
const TUint32 KCamCrZoomPaneOrientation		= 0x33;
const TUint32 KCamCrZoomUsingVolumeKeys     = 0x34;
const TUint32 KCamCrZoomUsingNaviKeys       = 0x35;
const TUint32 KCamCrLastUsedMonthFolder		= 0x36;

const TUint32 KCamCrAlwaysDrawCourtesyUi    = 0x37;

const TUint32 KCamCrPhotoStoreLocation      = 0x38;
 
const TUint32 KCamCrUserMode                = 0x50;

// New CenRep key to hold the ID of the Default Album
const TUint32 KCamCrDefaultAlbumId          = 0x51;

// New CenRep key to hold the name of the Default Album
const TUint32 KCamCrDefaultAlbumTitle       = 0x57;

// Customise Toolbar keys
const TUint32 KCamCrImageToolbarItems        = 0x58;
const TUint32 KCamCrImageToolbarDefaultItems = 0x59;
//Ftu message key
const TUint32 KCamCrFtuMessageFlag           = 0x60;

//Support for Photo and Video editor
const TUint32 KCamCrPhotoEditorSupport = 0x5A;
const TUint32 KCamCrVideoEditorSupport = 0x5B;

//Remove phone memory usage
const TUint32 KCamCrRemovePhoneMemoryUsage = 0x5C;

// Stops Video recording when HDMI cable is inserted
const TUint32 KCamCrStopRecordingInHdmiMode = 0x5D;

// =========================================================
// Cameraapp variant settings file
const TUid KCRUidCameraappVariant = {0x10208A43};

const TUint32 KCamCrVariantFlags            = 0x01;  // NOTE: "CameraVariant.hrh"    
                                                     // contains the possible values
                                                     // of the flags

const TUint32 KCamCrAppAlwaysRunning        = 0x02;
const TUint32 KCamCrTimeLapseSupport        = 0x03;
const TUint32 KCamCrVersionMajor            = 0x04;
const TUint32 KCamCrVersionMinor            = 0x05;
const TUint32 KCamCrBuildNumber             = 0x06;

// =========================================================
#endif      // CAMERAAPPPRIVATECRKEYS_H
          
// End of File
