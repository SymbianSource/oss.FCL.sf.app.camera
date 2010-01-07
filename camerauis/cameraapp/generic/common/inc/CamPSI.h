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
* Description:  A set of product specific values used by CamUtility*
*/



#ifndef CAMPSI_H
#define CAMPSI_H

#include <gdi.h>
#include <AknUtils.h> 
#include <e32keys.h>
#include "Cam.hrh"

// DATA TYPES

// This enum lists all Product Specific Information values that can be 
// retrieved by the CamUtility::GetPSI methods.
enum TCamPsiKey
  {
  ECamPsiOrientation, // application orientation landscape, portrait or unspecified
                      // values are defined in CEikAppUi
  ECamPsiPrimaryCameraOrientation,
  ECamPsiSecondaryCameraOrientation,
  ECamPsiEditingOrientation,
  
  ECamPsiSoftKeyPosition,
  
  ECamPsiSidePanePosition,
  ECamPsiZoomInKey,
  ECamPsiZoomOutKey,
  ECamPsiVideoZoomSupport,
  ECamPsiOverLaySidePane,
  ECamPsiImageSizes,
  ECamPsiViewFinderFreeze,
  
  ECamPsiSelfTimerPeriod1,    
  ECamPsiSelfTimerPeriod2,
  ECamPsiSelfTimerPeriod3,
  
  ECamPsiLongCaptureKeyPressInterval,
  ECamPsiMaxZoomSteps,
  ECamPsiMaxExtendedZoomSteps,
  ECamPsiMaxOpticalZoomSteps,
  ECamPsiSpecificZoomValues,
  ECamPsiNumSpecificZoomValues,
  ECamPsiLeftNaviKey,
  ECamPsiLeftNaviKeyCam2,
  ECamPsiRightNaviKey,
    
  ECamPsiSecondaryZoomInKey,
  ECamPsiSecondaryZoomOutKey,
  
  ECamPsiMaxBurstCapture,
  ECamPsiZoomBarLAF,
  ECamPsiSupportedStillResolutions,
  ECamPsiSupportedVideoResolutions,
  ECamPsiSupportedStillQualities,
  ECamPsiSupportedVideoQualities,
  // Supported scenes depend on user mode.
  // Each mode has now own key.
  //    ECamPsiSupportedStillScenes,
  //    ECamPsiSupportedVideoScenes,
  ECamPsiSupportedStillScenesNovice,
  ECamPsiSupportedStillScenesAdvanced,
  ECamPsiSupportedVideoScenesNovice,
  ECamPsiSupportedVideoScenesAdvanced,
  
  ECamPsiCaptureDelayTiming,
  
  ECamPsiEVCompSliderInfo,
  ECamPsiEVCompSliderBitmapId,
  ECamPsiAPEVCompLegend,
  ECamPsiEVCompSliderRangeResourceId,
  ECamPsiEVCompSliderRangeResourceAHId,
  ECamPsiUserSceneResourceIds,
  
  ECamPsiVideoViewFinderResolutions,
  ECamPsiVideoVFResSecondaryCamera,
  
  ECamPsiCourtesyUiTimeoutValue,
  
  ECamPsiNumberSupportedStillQualities,
  ECamPsiNumberSupportedVideoQualities,
  ECamPsiStillMaxQualityResId,
  
  ECamPsiStillQualitySliderLegendInfo,
  ECamPsiVideoQualitySliderLegendInfo,
  ECamPsiVideoQualityIconIds,
  ECamPsiVideoQualityFileTypeId,
  ECamPsiStillQualityIconIds,
  
  ECamPsiPrecapStillQualityIconIds,
  ECamPsiPrecapVideoQualityIconIds,
  
  ECamPsiPrecapLocationIconIds,
  
  ECamPsiModeKeyScanCode,
  ECamPsiJpegDataSizes,
  ECamPsiSequencePostProcessDataSizes,
  ECamPsiSnapshotSize,


  ECamPsiVideoUserModeNovDisabledSettings,
  ECamPsiPhotoUserModeNovDisabledSettings,
  ECamPsiEmbVideoUserModeNovDisabledSettings,
  ECamPsiEmbPhotoUserModeNovDisabledSettings,
  
  // Has own mapping function now => key not needed.
  // ECamPsiSharpnessValues,

  ECamPsiKeyNone
  };

// Whether the side pane is on the left or right hand side of the screen 
// in landscape mode.
enum TCamHandedness
	{
	ECamHandLeft,
	ECamHandRight
	};
	
// Which behaviour is implemented for the left and right arrow keys
enum TCamLeftRightNaviKeyBehaviour
    {
    ECamNaviKeyNone,
    ECamNaviKeyIncreaseFlashSetting,
    ECamNaviKeyDecreaseFlashSetting,
    ECamNaviKeyHorizontalSelector,
    ECamNaviKeyCaptureSetup
    };
    
// Which keys initiate a capture event
enum TCamCaptureKeys
    {
    ECamCaptureKeyNaviSelect = EStdKeyDevice3
    };

// This enum lists the options available for zooming in relation to video
// capture.
enum TCamVideoZoom          // NOTE: Use as a bitmask
    {       
    ECamZoomWhenRecord = 0x1 // Can zoom when recording video.
    };


// ---------------------------------------------------------------------------
class TImageSizeArray
    {
public:
    TInt iNoOfImageQuality;
    TInt iNoOfImageRes;
    TInt iArray[3][4];
    };

// ---------------------------------------------------------------------------
const TInt KNoOfSelfTimerPeriods = 3;

class TSelfTimerSetting
  {
  public:

    // Default constructor to init values to KErrNotSupported.
    TSelfTimerSetting()
      {
      for( TInt i = 0; i < KNoOfSelfTimerPeriods; i++ )
        {
        iThreshold  [i] = KErrNotSupported;
        iFlashPeriod[i] = KErrNotSupported;
        iBeepPeriod [i] = KErrNotSupported;
        }
      iAutoFocus = KErrNotSupported;
      };

    TInt iThreshold[KNoOfSelfTimerPeriods];     // 
    TInt iFlashPeriod[KNoOfSelfTimerPeriods];   // 
    TInt iBeepPeriod[KNoOfSelfTimerPeriods];   // 
    TInt iAutoFocus;
  };

// ---------------------------------------------------------------------------
// This class represents the maximum supported zoom step 
// for photo and video resolutions
class TCamMaxZoomSteps
  {   
  public:

    // Default constructor to init values to KErrNotSupported.
    TCamMaxZoomSteps(): 
      iMaxPhotoStepW9MP (KErrNotSupported), 
      iMaxPhotoStepW6MP (KErrNotSupported), 
      iMaxPhotoStep12MP (KErrNotSupported), 
      iMaxPhotoStep8MP  (KErrNotSupported), 
      iMaxPhotoStep5MP  (KErrNotSupported), 
      iMaxPhotoStep3MP  (KErrNotSupported),
      iMaxPhotoStep2MP  (KErrNotSupported), 
      iMaxPhotoStep1_3MP(KErrNotSupported),
      iMaxPhotoStep1MP  (KErrNotSupported), 
      iMaxPhotoStep0_8MP(KErrNotSupported),
      iMaxPhotoStepSVGA (KErrNotSupported), 
      iMaxPhotoStepVGA  (KErrNotSupported),

      iMaxVideoStepHD   (KErrNotSupported), 
      iMaxVideoStepWVGA (KErrNotSupported), 
      iMaxVideoStepNHD  (KErrNotSupported), 
      iMaxVideoStepVGA  (KErrNotSupported), 
      iMaxVideoStepQVGA (KErrNotSupported),
      iMaxVideoStepCIF  (KErrNotSupported), 
      iMaxVideoStepQCIF (KErrNotSupported),
      iMaxVideoStepSQCIF(KErrNotSupported),

      iMax2ndCamPhotoStep     (KErrNotSupported),

      iMax2ndCamVideoStepCIF  (KErrNotSupported),
      iMax2ndCamVideoStepQCIF (KErrNotSupported),
      iMax2ndCamVideoStepSQCIF(KErrNotSupported)
      {};

    // Primary camera Photo steps
    TInt iMaxPhotoStepW9MP;
    TInt iMaxPhotoStepW6MP;
    TInt iMaxPhotoStep12MP;
    TInt iMaxPhotoStep8MP;
    TInt iMaxPhotoStep5MP;
    TInt iMaxPhotoStep3MP;  
    TInt iMaxPhotoStep2MP;
    TInt iMaxPhotoStep1_3MP;
    TInt iMaxPhotoStep1MP;
    TInt iMaxPhotoStep0_8MP;
    TInt iMaxPhotoStepSVGA;
    TInt iMaxPhotoStepVGA;

    // Primary camera Video steps
    TInt iMaxVideoStepHD;
    TInt iMaxVideoStepWVGA;
    TInt iMaxVideoStepNHD;
    TInt iMaxVideoStepVGA;
    TInt iMaxVideoStepQVGA;
    TInt iMaxVideoStepCIF;
    TInt iMaxVideoStepQCIF;
    TInt iMaxVideoStepSQCIF;

    TInt iMax2ndCamPhotoStep;

    TInt iMax2ndCamVideoStepCIF;
    TInt iMax2ndCamVideoStepQCIF;
    TInt iMax2ndCamVideoStepSQCIF;
  };

// ---------------------------------------------------------------------------    
// Contains details of the time interval between zoom steps,  and the number
// of zoom steps to jump in each interval.  
// Also has the colour of each segment of the zoom pane.
class TCamZoomLAF
  {
  public:
    // Default constructor to init values.
    TCamZoomLAF() :
      iZoomSpeedOpt(1), iZoomStepsOpt(1), iZoomColourOpt(),
      iZoomSpeedDig(1), iZoomStepsDig(1), iZoomColourDig(),
      iZoomSpeedExt(1), iZoomStepsExt(1), iZoomColourExt(),
      iSecondCameraZoomSteps(1)
      {};


    TInt iZoomSpeedOpt;  // Time per zoom tick in milliseconds
    TInt iZoomStepsOpt;  // No of zoom steps per tick
    TRgb iZoomColourOpt; // Colour of optical zoom in zoom bar
    
    TInt iZoomSpeedDig;  // Time per zoom tick in milliseconds
    TInt iZoomStepsDig;  // No of zoom steps per tick
    TRgb iZoomColourDig; // Colour of optical zoom in zoom bar
    
    TInt iZoomSpeedExt;  // Time per zoom tick in milliseconds
    TInt iZoomStepsExt;  // No of zoom steps per tick
    TRgb iZoomColourExt; // Colour of optical zoom in zoom bar
    
    TInt iSecondCameraZoomSteps;
  };

// ---------------------------------------------------------------------------    
// EV comp slider range
class TCamEvCompRange
 	{
  public:
    // Default constructor to init values.
    TCamEvCompRange(): 
      iMinValue(0), iMaxValue(0), iStepsPerUnit(-1)
      {};
  
 	  TInt iMinValue;		// Minimum EV value displayed on slider
 	  TInt iMaxValue;		// Maximum EV value displayed on slider
 	  TInt iStepsPerUnit;	// Number of divisions per singe EV unit
 	};

#endif // CAMPSI_H
