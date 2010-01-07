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
* Description:  Types used by camera dynamic configuration. Previously
*                defined separately for each product in camproductconstants.h
*                
*
*/


#ifndef CAMCONFIGURATIONTYPES_H
#define CAMCONFIGURATIONTYPES_H


// Video qualities
//   Used to also index to the KVideoQualitySettings array.
//   Make sure the CamUtility returns rigth count of 
//   supported video qualities.
enum TCamVideoQuality
  {
  EVideoHigh,
  EVideoNormal,
  EVideoMMS,
  EVideoQualityLast
  };


enum TCamImageQuality
  {
  EImageQualityPrintHigh,
  EImageQualityPrint,
  EImageQualityPrintLow,
  EImageQualityEmail,
  EImageQualityMMS,
  EImageQualitySecondary,
  EImageQualityLast
  };

enum TCamImageResolution
  {
  EImageResolutionMMS,    // 0.3 MPix / MMS
  EImageResolution0_8MP,  // 0.8 MPix = XGA
  EImageResolution1MP,    // 1   MPix
  EImageResolution1_3MP,  // 1.3 MPix
  EImageResolution2MP,    // 2   MPix
  EImageResolution3MP,    // 3   MPix
  EImageResolution5MP,    // 5   MPix
  EImageResolution8MP,    // 8   MPix
  EImageResolution12MP,   // 12   MPix
  EImageResolutionW6MP,   // 6   MPix 16:9
  EImageResolutionW9MP,   // 9   MPix 16:9
  EImageResolutionCIF,    // CIF
  EImageResolutionQVGA,   // QVGA, for secondary camera
  EImageResolutionWVGA,   // WVGA    
  EImageResolutionLast    // End marker
  };

// Supported video resolutions  
// Converted to TCamVideoResolution in MapVideoResolution
enum TVideoResolution 
  {
  EVideoResolutionQCIF,
  EVideoResolutionQVGA,  
  EVideoResolutionVGA,
  EVideoResolutionNHD,
  EVideoResolutionWVGA,
  EVideoResolutionHD
  };  
  
// Image compression levels 
enum TCamImageCompression
  {
  EPhotoQualitySuperFine = 99,
  EPhotoQualityFine      = 85, // 1:10
  EPhotoQualityStandard  = 70,
  EPhotoQualityBasic     = 50, // 1:20
  EPhotoQualityMMS       = 25
  };  
  
// Image quality data
// typedef struct 
struct TCamPhotoQualitySetting 
  {
  TCamImageQuality      iPhotoQualityId;
  TCamImageResolution   iPhotoResolution;
  TInt                  iQualityLevel;    // TCamImageCompression
  }; // TCamPhotoQualitySetting;  
  
// Structure representing a video quality level 
// The text pointers must be const, so no const_casts are needed
struct TVideoQualitySettings
  {
  TInt            iQualityLevel;      // identifier
  TInt            iVideoResolution;   // resolution (ie CIF/QCIF)
  TReal           iVideoFrameRate;    // video frame rate
  TInt            iVideoBitRate;      // video bit rate
  TInt            iAudioBitRate;      // audio bit rate
  const TUint8*   iVideoMimeType;     // MIME type
  const TUint16*  iPreferredSupplier; // Supplier
  const TUint8*   iVideoCodec;        // video codec
  const TUint8*   iAudioCodec;        // audio codec
  TInt            iVideoFileType;     // video file type (i.e mpeg4, H263)
  TInt            iVideoLength;       // video length (max or short for sharing)
  };


// New dynamic configuration related types
// -----------------------------------------------------------------

// Settings slider and precapture quality icons
// Used for image and video quality data
struct TCamQualityIcons
  {
  TInt iSettingsIconId;
  TInt iSettingsIconMask;
  TInt iPrecapIconId;
  TInt iPrecapIconMask;    
  };

// File type icon
struct TCamFileTypeIcons
  {
  TInt iIconId;
  TInt iMask;  
  };

// Item types, used when appending data into integer arrays
enum TCamItemType
  {
  ECamItemSame,  
  ECamItemPhotoSize,
  ECamItemPhotoQuality,
  ECamItemSettingsIconId,
  ECamItemPrecapIconId,
  ECamItemSecondaryIconId,
  ECamItemPhotoJpegSize,
  ECamItemSeqPostProcSize
  };
    
// Photo size. Used to create image size data (resolution+enums) constant array
struct TCamPhotoSize
  {
  TCamPhotoSizeId         iSizeId; 
  TCamImageResolution     iResolution;
  TCamImageQualitySetting iQualitySettingId;  
  TInt                    iWidth;
  TInt                    iHeight;
  };

// Video size. Used to create video size data (resolution+enums) constant array
struct TCamVideoSize
  {
  TCamVideoResolution iCamVideoRes;
  TVideoResolution    iVideoRes;
  TInt                iWidth;
  TInt                iHeight;
  };

// Image quality data structure, which contains all the 
// information needed for each quality/size in the application
struct TCamImageQualityData
  {
  // This quality is for primary camera
  TBool                   iPrimaryCamera;  
   
  // Two different ways of representing the same image quality,
  // used in different parts of the application  
  TCamPhotoSize           iPhotoSize;
  TCamPhotoQualitySetting iPhotoQualitySetting;
  
  // Description Id. Combination of size (5m/3m...), usage (print/email/...)
  // and size (small/medium/large ). 
  TCamPhotoQualityDescription iDescription;
    
  // Quality icons
  TCamQualityIcons        iQualityIcons;   
  
  // Estimated file size
  TInt                    iFileSize;
  };

struct TCamVideoQualityData
  {
  // Quality settings (resolution, bitrate, file type etc), which were
  // previously in constant array KVideoQualitySettings   
  // iQualitySettings.iQuality level (0,1,...X) is used for indexing, last one is MMS
  TVideoQualitySettings iQualitySettings;  
  
  // Video resolution
  TCamVideoResolution iVideoResolution;
  
  // High/(normal high)/normal/(normal low)/sharing
  TCamVideoQualitySettings iQualitySetting;
  
  // Description ID for the quality
  TCamVideoQualityDescription iDescription;
  
  // Icon IDs
  TCamQualityIcons iQualityIcons;

  TCamFileTypeIcons iFileTypeIcons;
  };

// Image type - small/medium/large
enum TCamImageType
  {
  ECamImageTypeSmall,
  ECamImageTypeMedium,
  ECamImageTypeLarge,
  ECamImageTypeLast
  };

// Photo description and matching quality (print/email/mms etc)
struct TCamImageQualityMapping
  {  
  TCamPhotoQualityDescription iDesc;
  TCamImageQuality iQuality;  
  };
  
// Mapping between CamcorderVisible value and video description
struct TCamVideoQualityMapping
  {
  TCamVideoQualityDescription iDesc;
  TInt iCamcorderVisibleMin;    
  };  

// Panics
enum TCamConfigurationPanics
  {
  ECamConfigPanicUnknownKey,
  ECamConfigPanicUnknownType,
  ECamConfigPanicNoQuality,
  ECamConfigPanicNoDescription,
  ECamConfigPanicNoVideoRes,
  ECamConfigPanicNoVideoType,
  ECamConfigPanicInvalidNumberOfQualities,
  ECamConfigPanicInvalidItem      
  };

#endif // CAMCONFIGURATIONTYPES_H

