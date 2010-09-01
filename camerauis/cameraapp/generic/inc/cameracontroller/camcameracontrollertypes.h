/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Data type definitions for Camera Controller.
*
*/


#ifndef CAM_CONTROLLERTYPES_H
#define CAM_CONTROLLERTYPES_H

#include "camcameracontrollerflags.hrh"
#include "camcamerarequests.h"
#ifdef CAMERAAPP_CAPI_V2_ADV
  #include <ecamadvsettings.h>
#endif

// Max video recording time is 1:29:59
// To be moved to a separate constants header.
const TInt64 KMaxRecordingLength = MAKE_TINT64( 1, 0x41CE33C0 );

typedef RArray<TCamCameraRequestId> RCamRequestArray;


enum TCamCameraStateFlags
  {
  ECamIdle      =    0,
  ECamReserved  = 1<<0,
  ECamPowerOn   = 1<<1,
  ECamVFOn      = 1<<2,
  ECamImageOn   = 1<<3,
  ECamVideoOn   = 1<<4
//  ECamCaptureOn = 1<<5
  };

enum TCamCameraCaptureState
  {
  ECamCaptureOff,
  ECamCaptureOn,
  ECamCapturePaused
  };

enum TCamCameraTriState
  {
  ECamTriIdle,
  ECamTriInactive,
  ECamTriActive
  };

enum TCamCameraReadyState
  {
  ECamUnknown,
  ECamNotReady,
  ECamReady
  };

enum TCamViewfinderMode
  {
  ECamViewfinderNone,
  ECamViewfinderBitmap,
  ECamViewfinderDirect
  };

enum TCamCameraControllerBusyFlags
  {
  ECamBusyOff      = 0,
  ECamBusySequence = 1,
  ECamBusySingle   = 2,
  ECamBusySetting  = 4
  };

class TCamVideoRecordingTimes
  {
  public:
    TTimeIntervalMicroSeconds iTimeElapsed;
    TTimeIntervalMicroSeconds iTimeRemaining;
  };


/**
* Camera controller information
*/
class TCamControllerInfo
  {
  // ===================================
  public:

    TCamControllerInfo();

    void Reset( TBool aPreserveBusyFlag    = EFalse,
                TBool aPreserveCameraIndex = EFalse );

    void PrintInfo() const;

  // ===================================
  // Data
  public:

    /** 
    * Current camera index 
    */
    TInt  iCurrentCamera;

    /**
    * Controller overall state.
    * TCamCameraStateFlags ORed.
    */
    TUint                  iState;

    /**
    * Current state of capturing image or video.
    */
    TCamCameraCaptureState iCaptureState;  //< Is capture on, off or paused
    TInt                   iCaptureLimit;  //< Amount of captures  (images) requested
    TInt                   iCaptureCount;  //< Amount of captures  (images) received
    TInt                   iSnapshotCount; //< Amount of snapshots (images) received
    
    /** 
    * Viewfinder details
    */
    TCamViewfinderMode iVfMode;
    TCamCameraTriState iVfState;
    TSize              iViewfinderSize;
    CCamera::TFormat   iViewfinderFormat;

    /** 
    * Snapshot details
    */
    TCamCameraTriState iSsState;
    TSize              iSnapshotSize;
    CCamera::TFormat   iSnapshotFormat;
    TBool              iSnapshotAspectMaintain;

#ifdef CAMERAAPP_CAPI_V2_ADV
    /**
    * Autofocus range
    */
    CCamera::CCameraAdvancedSettings::TFocusRange iCurrentFocusRange;    
#endif // CAMERAAPP_CAPI_V2_ADV


    /** 
    * Is controller busy handling requests?
    * New requests can not be issued if this is not EBusyOff.
    * CancelSequence and CancelRequest can still be used.
    */
    TUint iBusy;

  // ===================================
  };


#ifdef CAMERAAPP_CAPI_V2_ADV
/**
* Information on advanced settings.
* (These use CCameraAdvancedSettings.)
*
*/
class TCamAdvancedSettingInfo
  {
  // ===================================
  public:

    TCamAdvancedSettingInfo();
    ~TCamAdvancedSettingInfo();

    void Reset();
    void PrintInfo() const;

  // ===================================
  // Data
  //
  // NOTE: Remember to modify Reset() if members are added.
  public:

    // Camera index which this info matches
    TInt         iForCameraIndex;

    // Exposure compenstion support
    TInt         iEvModesSupport;
    RArray<TInt> iEvStepsSupport;
    TValueInfo   iEvStepsValueInfo;

    // ISO rate support
    RArray<TInt> iIsoRatesSupport;

    // Digital zoom steps support
    RArray<TInt> iDigitalZoomSupport;
    TValueInfo   iDigitalZoomValueInfo;

#ifdef CAMERAAPP_CAPI_V2_IP
    // Colour fx support
    RArray<TInt> iColourEffectSupport;
    TValueInfo   iColourEffectValueInfo;

    // Sharpness support
    RArray<TInt> iSharpnessSupport;
    TValueInfo   iSharpnessValueInfo;
#endif

    // Stabilization support
    TInt         iStabilizationModeSupport;
    TInt         iStabilizationEffectSupport;
    TInt         iStabilizationComplexitySupport;

  // ===================================
  };
#endif // CAMERAAPP_CAPI_V2_ADV

/**
* Container for CCamera supported resolutions
* for certain camera index and format.
*/
class TCamCameraResolutionSupport
  {
  // ===================================
  public:

    TCamCameraResolutionSupport();
    ~TCamCameraResolutionSupport();

    void Reset();    

  // ===================================
  // Data
  public:

    TInt             iForCameraIndex;
    TInt             iForCameraFormat;
    RArray<TSize>    iResolutions;

  // ===================================
  };



// ===========================================================================
// Debug strings
#ifdef _DEBUG
const TUint16* const KCamCameraCaptureStateNames[] =
  {
  (const TUint16* const)_S16("ECamCaptureOff"),
  (const TUint16* const)_S16("ECamCaptureOn"),
  (const TUint16* const)_S16("ECamCapturePaused")
  };

const TUint16* const KCamCameraTriStateNames[] =
  {
  (const TUint16* const)_S16("ECamTriIdle"),
  (const TUint16* const)_S16("ECamTriInactive"),
  (const TUint16* const)_S16("ECamTriActive")
  };

const TUint16* const KCamCameraReadyStateNames[] =
  {
  (const TUint16* const)_S16("ECamUnknown"),
  (const TUint16* const)_S16("ECamNotReady"),
  (const TUint16* const)_S16("ECamReady")
  };

const TUint16* const KCamViewfinderModeNames[] =
  {
  (const TUint16* const)_S16("ECamViewfinderNone"),
  (const TUint16* const)_S16("ECamViewfinderBitmap"),
  (const TUint16* const)_S16("ECamViewfinderDirect")
  };
#endif // _DEBUG

#endif // CAM_CONTROLLERTYPES_H

// end of file
