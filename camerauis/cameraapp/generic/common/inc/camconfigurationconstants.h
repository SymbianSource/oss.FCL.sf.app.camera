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
* Description:  Constants used by camera dynamic configuration.
*                Mappings between enums, resolutions, icons, description
*                and CamcorderVisible values.
*
*
*/

#ifndef CAMCONFIGURATIONCONSTANTS_H
#define CAMCONFIGURATIONCONSTANTS_H

#include "CamSettingsInternal.hrh"
#include "camconfigurationtypes.h"
#include <cameraapp.mbg>

// Display IDs for cameras, used when requesting data from ICM
const TInt KCamPrimaryCameraDisplayId   = 2;
const TInt KCamSecondaryCameraDisplayId = 3;

// Photo resolutions corresponding each photo size and image quality
// enum value
const TCamPhotoSize KCamPhotoSizes[] =
  {
    { ECamPhotoSizeVGA,   EImageResolutionMMS,   ECamImageQualityMMS,       640,  480   },
    { ECamPhotoSizeSVGA,  EImageResolutionMMS,   ECamImageQualityEmail05m,  800,  600   },
    { ECamPhotoSizeXGA,   EImageResolution0_8MP, ECamImageQualityEmail08m,  1024, 768   },
    { ECamPhotoSize1MP,   EImageResolution1MP,   ECamImageQualityEmail1m,   1152, 864   },
    { ECamPhotoSize1_3MP, EImageResolution1_3MP, ECamImageQualityPrint1_3m, 1280, 960   },
    { ECamPhotoSize2MP,   EImageResolution2MP,   ECamImageQualityPrint2m,   1600, 1200  },
    { ECamPhotoSize3MP,   EImageResolution3MP,   ECamImageQualityPrint3m,   2048, 1536  },
    { ECamPhotoSize5MP,   EImageResolution5MP,   ECamImageQualityPrint5m,   2592, 1944  },
    { ECamPhotoSize8MP,   EImageResolution8MP,   ECamImageQualityPrint8m,   3264, 2448  },
    { ECamPhotoSize12MP,  EImageResolution12MP,  ECamImageQualityPrint12m,  4000, 3000  },
    { ECamPhotoSizeW6MP,   EImageResolutionW6MP,   ECamImageQualityPrintW6m,  3264, 1832  },
    { ECamPhotoSizeW9MP,   EImageResolutionW9MP,   ECamImageQualityPrintW9m,  4000, 2248  },
    { ECamPhotoSizeCIF,   EImageResolutionMMS,   ECamImageQualityMMS,        354, 288   },
    { ECamPhotoSizeQVGA,  EImageResolutionQVGA,  ECamImageQualityMMS,        320, 240   },
    { ECamPhotoSizeWVGA,  EImageResolutionWVGA,  ECamImageQualityPrint8m,   4000, 2672   } // @todo: fake values
  };
__ASSERT_COMPILE( (sizeof(KCamPhotoSizes) / sizeof(TCamPhotoSize)) == ECamPhotoSizeLast );


// Mappings for image qualities and small/medium/large types (type
// is calculated based on ICM's CamcorderVisible value)
// In case a type is not supported for a quality, closest description
// and quality are used.
const TCamImageQualityMapping
   KCamImageQualityMappings[][ECamImageTypeLast] =
  {
  // ECamPhotoSizeVGA
    {
      { ECamPhotoQualityDescMms, EImageQualityMMS }, // Small
      { ECamPhotoQualityDescMms, EImageQualityMMS }, // Medium
      { ECamPhotoQualityDescMms, EImageQualityMMS }  // Large
    },
  // ECamPhotoSizeSVGA - Not supported for any type!
    {
      { ECamPhotoQualityDescLast, EImageQualityEmail }, // Small
      { ECamPhotoQualityDescLast, EImageQualityEmail }, // Medium
      { ECamPhotoQualityDescLast, EImageQualityEmail }  // Large
    },
  // ECamPhotoSizeXGA
    {
      { ECamPhotoQualityDesc0_8mMed, EImageQualityEmail }, // Small
      { ECamPhotoQualityDesc0_8mMed, EImageQualityEmail }, // Medium
      { ECamPhotoQualityDesc0_8mMed, EImageQualityEmail }  // Large
    },
  // ECamPhotoSize1MP
    {
      { ECamPhotoQualityDesc1mSmall, EImageQualityEmail }, // Small
      { ECamPhotoQualityDesc1mMed,   EImageQualityEmail }, // Medium
      { ECamPhotoQualityDesc1mMed,   EImageQualityEmail }  // Large
    },
  // ECamPhotoSize1_3MP
    {
      { ECamPhotoQualityDesc1_3mSmall, EImageQualityEmail }, // Small
      { ECamPhotoQualityDesc1_3mSmall, EImageQualityEmail }, // Medium
      { ECamPhotoQualityDesc1_3mSmall, EImageQualityEmail }  // Large
    },
  // ECamPhotoSize2MP
    {
      { ECamPhotoQualityDesc2mMed,   EImageQualityPrint }, // Small
      { ECamPhotoQualityDesc2mMed,   EImageQualityPrint }, // Medium
      { ECamPhotoQualityDesc2mLarge, EImageQualityPrint }  // Large
    },
  // ECamPhotoSize3MP
    {
      { ECamPhotoQualityDesc3mMed,   EImageQualityPrint }, // Small
      { ECamPhotoQualityDesc3mMed,   EImageQualityPrint }, // Medium
      { ECamPhotoQualityDesc3mLarge, EImageQualityPrint }  // Large
    },
  // ECamPhotoSize5MP
    {
      { ECamPhotoQualityDesc5mLarge, EImageQualityPrint }, // Small
      { ECamPhotoQualityDesc5mLarge, EImageQualityPrint }, // Medium
      { ECamPhotoQualityDesc5mLarge, EImageQualityPrint }  // Large
    },
  // ECamPhotoSize8MP
    {
      { ECamPhotoQualityDesc8mLarge, EImageQualityPrint }, // Small
      { ECamPhotoQualityDesc8mLarge, EImageQualityPrint }, // Medium
      { ECamPhotoQualityDesc8mLarge, EImageQualityPrint }  // Large
    },
  // ECamPhotoSize12MP
    {
      { ECamPhotoQualityDesc12mLarge, EImageQualityPrint }, // Small
      { ECamPhotoQualityDesc12mLarge, EImageQualityPrint }, // Medium
      { ECamPhotoQualityDesc12mLarge, EImageQualityPrint }  // Large
    },
  // ECamPhotoSizeW6MP
    {
      { ECamPhotoQualityDescW6mLarge, EImageQualityPrint }, // Small
      { ECamPhotoQualityDescW6mLarge, EImageQualityPrint }, // Medium
      { ECamPhotoQualityDescW6mLarge, EImageQualityPrint }  // Large
    },
  // ECamPhotoSizeW9MP
    {
      { ECamPhotoQualityDescW9mLarge, EImageQualityPrint }, // Small
      { ECamPhotoQualityDescW9mLarge, EImageQualityPrint }, // Medium
      { ECamPhotoQualityDescW9mLarge, EImageQualityPrint }  // Large
    },
  // ECamPhotoSizeCIF
    {
      { ECamPhotoQualityDescMms, EImageQualityMMS }, // Small
      { ECamPhotoQualityDescMms, EImageQualityMMS }, // Medium
      { ECamPhotoQualityDescMms, EImageQualityMMS }  // Large
    },
  // ECamPhotoSizeQVGA
    {
      { ECamPhotoQualityDescMms, EImageQualityMMS }, // Small
      { ECamPhotoQualityDescMms, EImageQualityMMS }, // Medium
      { ECamPhotoQualityDescMms, EImageQualityMMS }  // Large
    },
  // ECamPhotoSizeWVGA    // @todo: fake values
    {
      { ECamPhotoQualityDesc8mLarge, EImageQualityPrint }, // Small
      { ECamPhotoQualityDesc8mLarge, EImageQualityPrint }, // Medium
      { ECamPhotoQualityDesc8mLarge, EImageQualityPrint }  // Large
    },
  };
__ASSERT_COMPILE( (sizeof(KCamImageQualityMappings) / (sizeof(TCamImageQualityMapping)*ECamImageTypeLast) ) == ECamPhotoSizeLast );

// Video resolutions corresponding enum values
const TCamVideoSize KCamVideoSizes[] =
  {
    { ECamVideoResolutionSubQCIF, EVideoResolutionQCIF,   128,   96 },
    { ECamVideoResolutionQCIF,    EVideoResolutionQCIF,   176,  144 },
    { ECamVideoResolutionCIF,     EVideoResolutionQCIF,   352,  288 },
    { ECamVideoResolutionVGA,     EVideoResolutionVGA,    640,  480 },
    { ECamVideoResolutionQVGA,    EVideoResolutionQVGA,   320,  240 },
    { ECamVideoResolutionNHD,     EVideoResolutionNHD,    640,  352 },
    { ECamVideoResolutionWVGA,    EVideoResolutionWVGA,   864,  480 },
    { ECamVideoResolutionHD,      EVideoResolutionHD,    1280,  720 }
  };
__ASSERT_COMPILE( (sizeof(KCamVideoSizes) / sizeof(TCamVideoSize)) == ECamVideoResolutionLast );

// Image quality icons, settings view and precapture
// Array indexed quality description ID
const TCamQualityIcons KCamImageIcons[] =
  {


  // ECamPhotoQualityDesc12mLarge
    {
    EMbmCameraappQgn_indi_cam4_quality_12mp_high,
    EMbmCameraappQgn_indi_cam4_quality_12mp_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_12mp_high,
    EMbmCameraappQgn_indi_cam4_quality_12mp_high_mask
    },
  // ECamPhotoQualityDescW9mLarge
    {
    EMbmCameraappQgn_indi_cam4_quality_9mp_high,
    EMbmCameraappQgn_indi_cam4_quality_9mp_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_9mp_high,
    EMbmCameraappQgn_indi_cam4_quality_9mp_high_mask
    },
  // ECamPhotoQualityDesc8mLarge
    {
    EMbmCameraappQgn_indi_cam4_quality_8mp_high,
    EMbmCameraappQgn_indi_cam4_quality_8mp_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_8mp_high,
    EMbmCameraappQgn_indi_cam4_quality_8mp_high_mask
    },
  // ECamPhotoQualityDescW6mLarge
    {
    EMbmCameraappQgn_indi_cam4_quality_6mp_high,
    EMbmCameraappQgn_indi_cam4_quality_6mp_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_6mp_high,
    EMbmCameraappQgn_indi_cam4_quality_6mp_high_mask
    },
  // ECamPhotoQualityDesc5mLarge
    {
    EMbmCameraappQgn_indi_cam4_quality_5mp_high,
    EMbmCameraappQgn_indi_cam4_quality_5mp_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_5mp_high,
    EMbmCameraappQgn_indi_cam4_quality_5mp_high_mask
    },
  // ECamPhotoQualityDesc3mLarge
    {
    EMbmCameraappQgn_indi_cam4_quality_3mp_high,
    EMbmCameraappQgn_indi_cam4_quality_3mp_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_3mp_high,
    EMbmCameraappQgn_indi_cam4_quality_3mp_high_mask
    },
  // ECamPhotoQualityDesc2mLarge
    {
    EMbmCameraappQgn_indi_cam4_quality_2mp_high,
    EMbmCameraappQgn_indi_cam4_quality_2mp_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_2mp_high,
    EMbmCameraappQgn_indi_cam4_quality_2mp_high_mask
    },
  // ECamPhotoQualityDesc3mMed
    {
    EMbmCameraappQgn_indi_cam4_quality_3mp_medium,
    EMbmCameraappQgn_indi_cam4_quality_3mp_medium_mask,
    EMbmCameraappQgn_indi_cam4_quality_3mp_medium,
    EMbmCameraappQgn_indi_cam4_quality_3mp_medium_mask
    },
  // ECamPhotoQualityDesc2mMed
    {
    EMbmCameraappQgn_indi_cam4_quality_2mp_medium,
    EMbmCameraappQgn_indi_cam4_quality_2mp_medium_mask,
    EMbmCameraappQgn_indi_cam4_quality_2mp_medium,
    EMbmCameraappQgn_indi_cam4_quality_2mp_medium_mask
    },
  // ECamPhotoQualityDesc1mMed
    {
    EMbmCameraappQgn_indi_cam4_quality_1mp_low, //Medium icon missing!!
    EMbmCameraappQgn_indi_cam4_quality_1mp_low_mask,
    EMbmCameraappQgn_indi_cam4_quality_1mp_low,
    EMbmCameraappQgn_indi_cam4_quality_1mp_low_mask
    },
  // ECamPhotoQualityDesc1_3mSmall
    {
    EMbmCameraappQgn_indi_cam4_quality_13mp_low,
    EMbmCameraappQgn_indi_cam4_quality_13mp_low_mask,
    EMbmCameraappQgn_indi_cam4_quality_13mp_low,
    EMbmCameraappQgn_indi_cam4_quality_13mp_low_mask
    },
  // ECamPhotoQualityDesc1mSmall
    {
    EMbmCameraappQgn_indi_cam4_quality_1mp_low,
    EMbmCameraappQgn_indi_cam4_quality_1mp_low_mask,
    EMbmCameraappQgn_indi_cam4_quality_1mp_low,
    EMbmCameraappQgn_indi_cam4_quality_1mp_low_mask
    },
  // ECamPhotoQualityDesc0_8mMed
    {
    EMbmCameraappQgn_indi_cam4_quality_08mp_low,
    EMbmCameraappQgn_indi_cam4_quality_08mp_low_mask,
    EMbmCameraappQgn_indi_cam4_quality_08mp_low,
    EMbmCameraappQgn_indi_cam4_quality_08mp_low_mask
    },
  // ECamPhotoQualityDescMms
    {
    EMbmCameraappQgn_indi_cam4_quality_03mp_low,
    EMbmCameraappQgn_indi_cam4_quality_03mp_low_mask,
    EMbmCameraappQgn_indi_cam4_quality_03mp_low,
    EMbmCameraappQgn_indi_cam4_quality_03mp_low_mask
    }
  };
__ASSERT_COMPILE( (sizeof(KCamImageIcons) / sizeof(TCamQualityIcons)) == ECamPhotoQualityDescLast );

// Video quality icons, settings view and precapture
// Array indexed quality description ID
const TCamQualityIcons KCamVideoIcons[] =
  {
  // ECamVideoQualityDescHDHighest
    {
    EMbmCameraappQgn_indi_cam4_quality_hd,
    EMbmCameraappQgn_indi_cam4_quality_hd_mask,
    EMbmCameraappQgn_indi_cam4_quality_hd,
    EMbmCameraappQgn_indi_cam4_quality_hd_mask
    },
  // ECamVideoQualityDescWidescreenHighest
    {
    EMbmCameraappQgn_indi_cam4_quality_wide_high,
    EMbmCameraappQgn_indi_cam4_quality_wide_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_wide_high,
    EMbmCameraappQgn_indi_cam4_quality_wide_high_mask
    },
  // ECamVideoQualityDescTVHigh
    {
    EMbmCameraappQgn_indi_vid4_quality_tv_high,
    EMbmCameraappQgn_indi_vid4_quality_tv_high_mask,
    EMbmCameraappQgn_indi_vid4_quality_tv_high,
    EMbmCameraappQgn_indi_vid4_quality_tv_high_mask
    },
  // ECamVideoQualityDescHigh
    {
    EMbmCameraappQgn_indi_vid4_quality_high,
    EMbmCameraappQgn_indi_vid4_quality_high_mask,
    EMbmCameraappQgn_indi_vid4_quality_high,
    EMbmCameraappQgn_indi_vid4_quality_high_mask
    },
  // ECamVideoQualityDescWidescreenHigh
    {
    EMbmCameraappQgn_indi_cam4_quality_wide_high,
    EMbmCameraappQgn_indi_cam4_quality_wide_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_wide_high,
    EMbmCameraappQgn_indi_cam4_quality_wide_high_mask
    },
  // ECamVideoQualityDescWidescreenNorm
    {
    // use high icons for now, until the correct ones can be found from the build
    EMbmCameraappQgn_indi_cam4_quality_wide_high,
    EMbmCameraappQgn_indi_cam4_quality_wide_high_mask,
    EMbmCameraappQgn_indi_cam4_quality_wide_high,
    EMbmCameraappQgn_indi_cam4_quality_wide_high_mask
    //EMbmCameraappQgn_indi_cam4_quality_wide_normal,
    //EMbmCameraappQgn_indi_cam4_quality_wide_normal_mask,
    //EMbmCameraappQgn_indi_cam4_quality_wide_normal,
    //EMbmCameraappQgn_indi_cam4_quality_wide_normal_mask
    },
  // ECamVideoQualityDescTVNorm
    {
    EMbmCameraappQgn_indi_vid4_quality_tv_normal,
    EMbmCameraappQgn_indi_vid4_quality_tv_normal_mask,
    EMbmCameraappQgn_indi_vid4_quality_tv_normal,
    EMbmCameraappQgn_indi_vid4_quality_tv_normal_mask
    },
  // ECamVideoQualityDescNorm
    {
    EMbmCameraappQgn_indi_vid4_quality_normal,
    EMbmCameraappQgn_indi_vid4_quality_normal_mask,
    EMbmCameraappQgn_indi_vid4_quality_normal,
    EMbmCameraappQgn_indi_vid4_quality_normal_mask
    },
  // ECamVideoQualityDescWebHigh
    {
    EMbmCameraappQgn_indi_vid4_quality_email_high,
    EMbmCameraappQgn_indi_vid4_quality_email_high_mask,
    EMbmCameraappQgn_indi_vid4_quality_email_high,
    EMbmCameraappQgn_indi_vid4_quality_email_high_mask
    },
  // ECamVideoQualityDescWebNorm
    {
    EMbmCameraappQgn_indi_vid4_quality_email_normal,
    EMbmCameraappQgn_indi_vid4_quality_email_normal_mask,
    EMbmCameraappQgn_indi_vid4_quality_email_normal,
    EMbmCameraappQgn_indi_vid4_quality_email_normal_mask
    },
  // ECamVideoQualityDescMMS
    {
    EMbmCameraappQgn_indi_cam4_quality_sharing,
    EMbmCameraappQgn_indi_cam4_quality_sharing_mask,
    EMbmCameraappQgn_indi_cam4_quality_sharing,
    EMbmCameraappQgn_indi_cam4_quality_sharing_mask
    },
  // ECamVideoQualityDescSharing
    {
    EMbmCameraappQgn_indi_cam4_quality_sharing,
    EMbmCameraappQgn_indi_cam4_quality_sharing_mask,
    EMbmCameraappQgn_indi_cam4_quality_sharing,
    EMbmCameraappQgn_indi_cam4_quality_sharing_mask
    }
  };
__ASSERT_COMPILE( (sizeof(KCamVideoIcons) / sizeof(TCamQualityIcons)) == ECamVideoQualityDescLast );

// Icons and mask for each video type (mp4, 3gp etc.)
const TCamFileTypeIcons KCamVideoTypeIcons[] =
  {
    {
    EMbmCameraappQgn_prop_cam4_codec_mp4,
    EMbmCameraappQgn_prop_cam4_codec_mp4_mask
    },
    {
    EMbmCameraappQgn_prop_cam4_codec_3gp,
    EMbmCameraappQgn_prop_cam4_codec_3gp_mask
    }
  };
__ASSERT_COMPILE( (sizeof(KCamVideoTypeIcons) / sizeof(TCamFileTypeIcons)) == ECamVideoTypeLast );

// Mime types for mapping from strings to our enum
_LIT8( KCamMimeMp4, "video/mp4" );
_LIT8( KCamMime3gpp, "video/3gpp" );

// Camera configuration panic id
_LIT( KCamConfigPanicId, "CAM-CONF" );

// Constants for mapping CamcorderVisible to small/medium/large image
const TInt KCamImageLargeTypeMin  = 300; // >= 300 = large
const TInt KCamImageMediumTypeMin = 200; // 200-299 = medium, otherwise small

// Constants for mapping CamcorderVisible value to video quality description
const TCamVideoQualityMapping
  KCamVideoQualityMappings[] =
  {
    { ECamVideoQualityDescHDHighest,         500 },
    { ECamVideoQualityDescWidescreenHighest, 490 },
    { ECamVideoQualityDescTVHigh,            450 },
    { ECamVideoQualityDescHigh,              400 },
    { ECamVideoQualityDescWidescreenHigh,    375 },
    { ECamVideoQualityDescWidescreenNorm,    365 },
    { ECamVideoQualityDescTVNorm,            350 },
    { ECamVideoQualityDescNorm,              300 },
    { ECamVideoQualityDescWebHigh,           250 },
    { ECamVideoQualityDescWebNorm,           200 },
    { ECamVideoQualityDescMMS,                90 },
    { ECamVideoQualityDescSharing,             1 }
  };
__ASSERT_COMPILE( (sizeof(KCamVideoQualityMappings) / sizeof(TCamVideoQualityMapping)) == ECamVideoQualityDescLast );


#endif // CAMCONFIGURATIONCONSTANTS_H

// End of file


