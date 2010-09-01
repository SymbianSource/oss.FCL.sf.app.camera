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
* Description:  Camera controller setting ids and associated data types.
*
*/


#ifndef CAM_CAMERACONTROLLERSETTINGS_H
#define CAM_CAMERACONTROLLERSETTINGS_H

#include <ecam.h>            // Data types (CCamera::TFormat etc)
#include <ecamadvsettings.h> // Data types

// ===========================================================================
// Definitions

namespace NCamCameraController
  {
  // =======================================================
  // Settings and parameters needed by Camera Controller from Setting Provider.
  // These are the keys by which Camera Controller requests the value.
  //
  // The comments list 
  //   [meaning of setting][expected setting data type]
  //

  // -------------------------------------------------------
  // Camera parameter ids and related data types
  enum TCamCameraParamId
    {
    ECameraParamNone,

    ECameraParamImage,    // [ Image init parameters ] [ TPckgBuf<TCamImageParams>    ]

    ECameraParamVfMode,   // [ Viewfinder mode       ] [ TPckgBuf<TCamViewfinderMode> ]
    ECameraParamVfBitmap, // [ Bitmap vf parameters  ] [ TPckgBuf<TCamVfBitmapParams> ]
    ECameraParamVfDirect, // [ Direct vf parameters  ] [ TPckgBuf<TCamVfDirectParams> ]

    ECameraParamSnapshot, // [ Snapshot parameters   ] [ TPckgBuf<TCamSnapshotParams> ]

    ECameraParamVideoCae, // [ CCaeEngine video parameters ] [ TPckgBuf<TCamVideoCaeParams> ]

    // NOTE: As the name of the enum below suggests, 
    //       ADD ALL NEW PARAMETER IDS ABOVE THIS COMMENT!
    //       Remember also to add the name of the item to
    //       the KCameraParamNames array of debug strings.
    ECameraParamCount    
    };


  /**
  * Camera image parameters
  */
  class TCamParamsImage
    {
    public:
      TCamParamsImage() 
        : iFormat( CCamera::EFormatExif ),
          iSize  (   ),
          iQualityFactor( 100 ) {};
  
    public:

      CCamera::TFormat iFormat;
      TSize            iSize;
      TInt             iQualityFactor;
    };

  /**
  * Camera bitmap viewfinder setting data
  */
  class TCamParamsVfBitmap
    {
    public:
      TCamParamsVfBitmap()
        : iFormat( CCamera::EFormatFbsBitmapColor16M ), 
          iRect(),
          iMirrorImage (EFalse)
          {
          }

    public:
      CCamera::TFormat iFormat;
      TRect            iRect;

      TBool iMirrorImage;
    };

  /**
  * Camera direct viewfinder setting data
  */
  class TCamParamsVfDirect
    {
    public:
      TCamParamsVfDirect()
        : iWindowServer  ( NULL ),
          iScreenDevice  ( NULL ),
          iWindow        ( NULL ),
          iScreenRect    ( NULL ) {};

    public:
      RWsSession*      iWindowServer;
      CWsScreenDevice* iScreenDevice;
      RWindowBase*     iWindow;
      TRect*           iScreenRect;
    };
  
  /**
  * Camera snapshot setting data
  */
  class TCamParamsSnapshot
    {
    public:
      TCamParamsSnapshot()
        : iSize(),
          iFormat( CCamera::EFormatFbsBitmapColor16M ),
          iMaintainAspect( EFalse ) {};

    public:
      TSize            iSize;
      CCamera::TFormat iFormat;
      TBool            iMaintainAspect;
    };  


  /**
  * Camera video quality setting data
  */
  class TCamParamsVideoCae
    {
    public:
      TCamParamsVideoCae() 
        : iFrameSize   ( /*empty*/  ),
          iFrameRate   ( 0          ),
          iVideoBitRate( 0          ),
          iAudioBitRate( 0          ),
          iAudioOn     ( ETrue      ),
          iMimeType    ( KNullDesC8 ),
          iSupplier    ( KNullDesC  ),
          iVideoType   ( KNullDesC8 ),
          iAudioType   ( KNullDesC8 ) {};

    public:
      TSize   iFrameSize;
      TReal32 iFrameRate;
      TInt    iVideoBitRate;
      TInt    iAudioBitRate;
      TBool   iAudioOn;
      TPtrC8  iMimeType;
      TPtrC   iSupplier;
      TPtrC8  iVideoType;
      TPtrC8  iAudioType;
    };

  // -------------------------------------------------------
  // Camera setting ids and related data types

  enum TCamCameraSettingId
    {
    ECameraSettingNone,
    
    ECameraSettingQualityFactor,    // [ (image) quality factor ] [ TInt {0..100}         ]

    ECameraSettingFlash,            // [ flash                  ] [ CCamera::TFlash                               ]
    ECameraSettingExposure,         // [ Exposure               ] [ TPckgBuf<TCamSettingDataExposure>             ]
    ECameraSettingLightSensitivity, // [ ISO                    ] [ TInt (ISO value)                              ]
    ECameraSettingWhiteBalance,     // [ WB                     ] [ TPckgBuf<TCamSettingDataWhiteBalance>         ]
    ECameraSettingColourEffect,     // [ colour effect          ] [ CCamera::CCameraImageProcessing::TEffect      ]
    ECameraSettingBrightness,       // [ brightness             ] [ CCamera::TBrightness                          ]
    ECameraSettingContrast,         // [ contrast               ] [ CCamera::TContrast                            ]
    ECameraSettingSharpness,        // [ sharpness              ] [ TInt (contrast adjustment value)              ]
    
    ECameraUserSceneSettingFlash,            // [ flash                  ] [ CCamera::TFlash                               ]
    ECameraUserSceneSettingExposure,         // [ Exposure               ] [ TPckgBuf<TCamSettingDataExposure>             ]
    ECameraUserSceneSettingLightSensitivity, // [ ISO                    ] [ TInt (ISO value)                              ]
    ECameraUserSceneSettingWhiteBalance,     // [ WB                     ] [ TPckgBuf<TCamSettingDataWhiteBalance>         ]
    ECameraUserSceneSettingColourEffect,     // [ colour effect          ] [ CCamera::CCameraImageProcessing::TEffect      ]
    ECameraUserSceneSettingBrightness,       // [ brightness             ] [ CCamera::TBrightness                          ]
    ECameraUserSceneSettingContrast,         // [ contrast               ] [ CCamera::TContrast                            ]
    ECameraUserSceneSettingSharpness,        // [ sharpness              ] [ TInt (contrast adjustment value)              ]
    
    ECameraSettingDigitalZoom,      // [ digital zoom           ] [ TInt (zoom step)       ]
    ECameraSettingOpticalZoom,      // [ optical zoom           ] [ TInt (zoom step)       ]

    ECameraSettingFocusRange,       // [ focus range            ] [ CCamera::CCameraAdvancedSettings::TFocusRange ]

    ECameraSettingStabilization,    // [ stabilization mode     ] [ TPckgBuf<TCamSettingDataStabilization>           ]
    ECameraSettingFlickerCancel,    // [ flicker cancellation   ] [ MCameraFlickerCancellation::TFlickerCancellation ]
    ECameraSettingOrientation,      // [ device orientation     ] [ MCameraOrientation::TOrientation                 ]
  
    ECameraSettingAudioMute,        // [ (video) audio muting   ] [ TBool (ETrue => muted) ]
    ECameraSettingContAF,           // [ (video) continuous AF  ] [ TBool (ETrue => on)    ]    
    ECameraSettingFileMaxSize,      // [ (video) max file size  ] [ TInt (bytes)           ]
    ECameraSettingFileName,         // [ (video) file name      ] [ TPtr                   ]
    
    ECameraSettingCaptureLimit,     // [ (image) capture limit  ] [ TInt (amount of images requested) ]

    ECameraSettingFacetracking,        // [ (still) facetracking   ] [ TBool (ETrue => facetracking on) ]

    // NOTE: As the name of the enum below suggests, 
    //       ADD ALL NEW SETTING IDS ABOVE THIS COMMENT!
    //       Remember also to add the name of the item to
    //       the KCameraSettingNames array of debug strings.
    ECameraSettingCount
    };


  /**
  * Exposure compensation setting data
  */
  class TCamSettingDataExposure
    {
    public:
      TCamSettingDataExposure()
        : iExposureMode( CCamera::EExposureAuto ),
          iExposureStep( 0 ) {};

    public:
      CCamera::TExposure iExposureMode;
      // EV compensation step times KECamFineResolutionFactor.
      // For example EV = 0.7, KECamFineResolutionFactor = 100
      // => this value should be 70
      TInt               iExposureStep; 
      };

  /**
  * White balance setting data
  */
  class TCamSettingDataWhiteBalance
    {
    public:
      TCamSettingDataWhiteBalance() 
        : iWhiteBalanceMode( CCamera::EWBAuto ) {};

    public:
      CCamera::TWhiteBalance iWhiteBalanceMode;
    };

  /**
  * Stabilization setting data
  */
  class TCamSettingDataStabilization
    {
    protected:
      typedef CCamera::CCameraAdvancedSettings CAS;

    public:

      TCamSettingDataStabilization() 
        : iMode      ( CAS::EStabilizationModeOff        ),
          iEffect    ( CAS::EStabilizationOff            ),
          iComplexity( CAS::EStabilizationComplexityAuto ) {};

    public:

      CAS::TStabilizationMode                iMode;
      CAS::TStabilizationEffect              iEffect; 
      CAS::TStabilizationAlgorithmComplexity iComplexity;
    };




  // -------------------------------------------------------  
#ifdef _DEBUG
const TUint16* const KCameraParamNames[] =
  {
  (const TUint16* const)_S16("ECameraParamNone"),
  (const TUint16* const)_S16("ECameraParamImage"),   
  (const TUint16* const)_S16("ECameraParamVfMode"),   
  (const TUint16* const)_S16("ECameraParamVfBitmap"), 
  (const TUint16* const)_S16("ECameraParamVfDirect"), 
  (const TUint16* const)_S16("ECameraParamSnapshot"), 
  (const TUint16* const)_S16("ECameraParamVideoCae")
  };
__ASSERT_COMPILE( (sizeof(KCameraParamNames)/sizeof(TUint16*)) == ECameraParamCount );

const TUint16* const KCameraSettingNames[] =
  {
  (const TUint16* const)_S16("ECameraSettingNone"),
  (const TUint16* const)_S16("ECameraSettingQualityFactor"),
  (const TUint16* const)_S16("ECameraSettingFlash"),
  (const TUint16* const)_S16("ECameraSettingExposure"),
  (const TUint16* const)_S16("ECameraSettingLightSensitivity"),
  (const TUint16* const)_S16("ECameraSettingWhiteBalance"),
  (const TUint16* const)_S16("ECameraSettingColourEffect"),
  (const TUint16* const)_S16("ECameraSettingBrightness"),
  (const TUint16* const)_S16("ECameraSettingContrast"),
  (const TUint16* const)_S16("ECameraSettingSharpness"),
  (const TUint16* const)_S16("ECameraUserSceneSettingFlash"),
  (const TUint16* const)_S16("ECameraUserSceneSettingExposure"),
  (const TUint16* const)_S16("ECameraUserSceneSettingLightSensitivity"),
  (const TUint16* const)_S16("ECameraUserSceneSettingWhiteBalance"),
  (const TUint16* const)_S16("ECameraUserSceneSettingColourEffect"),
  (const TUint16* const)_S16("ECameraUserSceneSettingBrightness"),
  (const TUint16* const)_S16("ECameraUserSceneSettingContrast"),
  (const TUint16* const)_S16("ECameraUserSceneSettingSharpness"),
  (const TUint16* const)_S16("ECameraSettingDigitalZoom"),
  (const TUint16* const)_S16("ECameraSettingOpticalZoom"),
  (const TUint16* const)_S16("ECameraSettingFocusRange"),
  (const TUint16* const)_S16("ECameraSettingStabilization"),
  (const TUint16* const)_S16("ECameraSettingFlickerCancel"),
  (const TUint16* const)_S16("ECameraSettingOrientation"),
  (const TUint16* const)_S16("ECameraSettingAudioMute"),
  (const TUint16* const)_S16("ECameraSettingContAF"),  
  (const TUint16* const)_S16("ECameraSettingFileMaxSize"),
  (const TUint16* const)_S16("ECameraSettingFileName"),
  (const TUint16* const)_S16("ECameraSettingCaptureLimit"),
  (const TUint16* const)_S16("ECameraSettingFacetracking")
  };  
__ASSERT_COMPILE( (sizeof(KCameraSettingNames)/sizeof(TUint16*)) == ECameraSettingCount );

#endif  
  // -------------------------------------------------------  
  } // namespace

// ===========================================================================
#endif // CAM_CAMERACONTROLLERSETTINGS_H

// end of file
