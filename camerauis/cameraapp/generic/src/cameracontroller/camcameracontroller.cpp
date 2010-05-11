/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Camera Controller implementation.
*              : Class handles CCamera related operations.
*
*/



// ===========================================================================
// Included headers

#include "camcameracontrollerflags.hrh"

// -------------------------------------
// General
#include <e32base.h>
#include <e32const.h> // KNullHandle
#include <fbs.h>
#include <gdi.h>
#include <aknlayoutscalable_apps.cdl.h>

// -------------------------------------
// ECAM
#include <ecam.h>
#ifdef CAMERAAPP_CAPI_V2
  #include <ecam/ecamdirectviewfinder.h>
  #include <ecam/camerahistogram.h>
  #include <ecam/cameraoverlay.h>
  #include <ecamadvsettings.h>
  #include <ecamadvsettingsuids.hrh>
#endif
#include <ecam/camerasnapshot.h> // KUidECamEventCameraSnapshot

// -------------------------------------
// Camera Application Engine (CAE)
#ifdef CAMERAAPP_CAE_FOR_VIDEO
  #include <CaeEngine.h>
#endif

// -------------------------------------
// Custom interfaces
#include <ECamOrientationCustomInterface2.h>
#include <ECamUIOrientationOverrideCustomAPI.h>
#include <ecamusecasehintcustomapi.h>
#include <ecamfacetrackingcustomapi.h>
#include <akntoolbar.h>
// -------------------------------------
// Own
#include "camcameracontroller.pan"
#include "camlogging.h"
#include "cambuffershare.h"
#include "cambuffersharecleanup.h"
#include "cambuffercleanup.h"
#include "cambuffer.h"

#include "camsnapshotprovider.h"
#include "camimageencoder.h"

#include "mcamsettingprovider.h"
#include "mcamcameraobserver.h"
#include "camsettingconversion.h"
#include "camcameraevents.h"
#include "camcamerarequests.h"
#include "camcameracontrolleractive.h"
#include "camcameracontroller.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "camcameracontrollerTraces.h"
#endif

#ifdef FORCE_DUMMY_ENGINE
  #include "CameraappDummyEngine.h"
#endif // FORCE_DUMMY_ENGINE
#ifdef CAMERAAPP_CAPI_EMULATOR
  #include "CamCamera.h"
#endif // CAMERAAPP_CAPI_EMULATOR
#ifdef CAMERAAPP_FLASH_SIMULATOR
  #include "camflashsimulator.h"
#endif // CAMERAAPP_FLASH_SIMULATOR
#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  #include "CamTimer.h"
#endif // CAMERAAPP_CAE_ERR_SIMULATION

#include "CamAppUi.h"
#include "CameraUiConfigManager.h"
#include "CamViewBase.h"
#include "CamPanic.h"
// ===========================================================================
// Local constants
const TInt KIveRecoveryCountMax = 2;
typedef CCamera::CCameraAdvancedSettings CAS;
typedef CCamera::CCameraImageProcessing  CIP;

//for camusecasehintcustomapi
_LIT8(KCMRMimeTypeH263, "video/H263-2000");
_LIT8(KCMRMimeTypeMPEG4VSPL2, "video/mp4v-es; profile-level-id=2");
_LIT8(KCMRMimeTypeMPEG4VSPL3, "video/mp4v-es; profile-level-id=3");
_LIT8(KCMRMimeTypeMPEG4VSPL4A, "video/mp4v-es; profile-level-id=4");
_LIT8(KCMRMimeTypeH264AVCBPL30, "video/H264; profile-level-id=42801E");
_LIT8(KCMRMimeTypeH264AVCBPL31, "video/H264; profile-level-id=42801F");


namespace NCamCameraController
  {
  static const TInt               KPrimaryCameraIndex         =  0;
  //static const TInt               KSecondaryCameraIndex       =  1;
  static const TInt               KInvalidCameraIndex         = -1;

  #ifdef CAMERAAPP_CAE_FOR_VIDEO
  //  static const TInt             KCameraDisplayIndex         = 0;
  #endif
  #ifdef CAMERAAPP_CAPI_V2
    static const TInt             KCameraClientPriority       = 100; // -100...100
  #endif

  static const TInt               KCamCallBackPriority        = EPriorityHigh;

  static const TInt               KCamSequenceGranularity     =  2;
  static const TInt               KResolutionArrayGranularity =  8;
  #ifdef CAMERAAPP_CAPI_V2_ADV
    static const TInt             KAdvArrayGranularity        =  5;
  #endif

  static const CCamera::TFormat   KCamJpegAlternativeFormat1  = CCamera::EFormatFbsBitmapColor16M;
  static const CCamera::TFormat   KCamJpegAlternativeFormat2  = CCamera::EFormatFbsBitmapColor64K;

  static const TInt             KCamReserveTryAgainMaxCount     = 50;
  static const TInt             KCamReserveTryAgainWaitInterval = 50000; // 50 milliseconds
  #ifdef _DEBUG
  _LIT( KCamDriveModeNameBurst,   "EDriveModeBurst"      );
  _LIT( KCamDriveModeNameSingle,  "EDriveModeSingleShot" );
  _LIT( KCamDriveModeNameUnknown, "Unknown drive mode!"  );
  #endif // _DEBUG
  }

// ===========================================================================
// Local methods

#ifdef CAMERAAPP_PERFORMANCE_CONTROLLER
  #include "camperformancelogger.h"

  #define CAMERAAPP_PERF_CONTROLLER_START( AAA )          iPerformanceLogger->Requested( AAA );
  #define CAMERAAPP_PERF_CONTROLLER_STARTNEW( AAA, BBB )  BBB = EFalse; iPerformanceLogger->Requested( AAA );
  #define CAMERAAPP_PERF_CONTROLLER_STOP( AAA )           iPerformanceLogger->Completed( AAA );
  #define CAMERAAPP_PERF_CONTROLLER_STOP_ONCE( AAA, BBB ) if( !BBB ) { iPerformanceLogger->Completed( AAA ); BBB = ETrue; }
#else
  #define CAMERAAPP_PERF_CONTROLLER_START( AAA )
  #define CAMERAAPP_PERF_CONTROLLER_STARTNEW( AAA, BBB )
  #define CAMERAAPP_PERF_CONTROLLER_STOP( AAA )
  #define CAMERAAPP_PERF_CONTROLLER_STOP_ONCE( AAA, BBB )
#endif // CAMERAAPP_PERFORMANCE_CONTROLLER

// Helper methods
#include "camflagutility.inl"
#include "campointerutility.inl"


namespace NCamCameraController
  {
  /*
  inline void SetFalse( TAny* aTBool )
    {
    TBool* boolean = static_cast<TBool*>( aTBool );

    if( boolean )
      *boolean = EFalse;
    }
  */

  inline void SetNotBusy( TAny* aTUint )
    {
    TUint* flags = static_cast<TUint*>( aTUint );

    if( flags )
      *flags = ECamBusyOff;
    };

  inline void ReleaseAndNull( MCameraBuffer*& aBuffer )
    {
    if( aBuffer )
      {
      aBuffer->Release();
      aBuffer = NULL;
      }
    };
#ifdef CAMERAAPP_CAPI_V2_ADV
  inline void ResetInfo( TAny* aInfo )
    {
    TCamAdvancedSettingInfo* info =
      static_cast<TCamAdvancedSettingInfo*>( aInfo );

    if( info )
      {
      info->Reset();
      }
    };


  inline const TDesC& DriveModeName( TInt aMode )
    {
#ifdef _DEBUG
    switch( aMode )
      {
      case CAS::EDriveModeBurst:      return KCamDriveModeNameBurst;
      case CAS::EDriveModeSingleShot: return KCamDriveModeNameSingle;
      default:                        return KCamDriveModeNameUnknown;
      }
#else
    return KNullDesC;
#endif // _DEBUG
    };
#endif
  }
using namespace NCamCameraController;

// ===========================================================================
// TCamControllerInfo class methods
TCamControllerInfo::TCamControllerInfo()
  {
  Reset();
  }

void
TCamControllerInfo::Reset( TBool aPreserveBusyFlag    /*= EFalse*/,
                           TBool aPreserveCameraIndex /*= EFalse*/ )
  {
  // When we reset the info e.g. after reserve lost event
  // we need to keep the current camera index stored.
  if( !aPreserveCameraIndex )
    {
    iCurrentCamera          = KErrNotFound;
    }

  iState                  = ECamIdle;
  iCaptureState           = ECamCaptureOff;
  iCaptureLimit           = 1;
  iCaptureCount           = 0;
  iSnapshotCount          = 0;

  iVfMode                 = ECamViewfinderNone;
  iVfState                = ECamTriIdle;
  iViewfinderSize         = TSize();
  iViewfinderFormat       = CCamera::EFormatFbsBitmapColor16M;

  iSsState                = ECamTriIdle;
  iSnapshotSize           = TSize();
  iSnapshotFormat         = CCamera::EFormatFbsBitmapColor16M;
  iSnapshotAspectMaintain = EFalse;

  if( !aPreserveBusyFlag )
    {
    iBusy = ECamBusyOff;
    }
  }

// ---------------------------------------------------------------------------
// PrintInfo
// ---------------------------------------------------------------------------
//
void
TCamControllerInfo::PrintInfo() const
  {
  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT ( _L("Camera <> TCamControllerInfo")                                 );
  PRINT ( _L("Camera <> ")                                                   );

  PRINT1( _L("Camera <>   iCurrentCamera          :  %d"    ), iCurrentCamera );
  PRINT1( _L("Camera <>   iState                  :  %08b"  ), iState );

  PRINT1( _L("Camera <>   iCaptureState           : [%s]"   ), KCamCameraCaptureStateNames[iCaptureState] );
  PRINT1( _L("Camera <>   iCaptureLimit           :  %d"    ), iCaptureLimit );
  PRINT1( _L("Camera <>   iCaptureCount           :  %d"    ), iCaptureCount );
  PRINT1( _L("Camera <>   iSnapshotCount          :  %d"    ), iSnapshotCount );

  PRINT1( _L("Camera <>   iVfMode                 : [%s]"   ), KCamViewfinderModeNames    [iVfMode] );
  PRINT1( _L("Camera <>   iVfState                : [%s]"   ), KCamCameraTriStateNames    [iVfState] );
  PRINT2( _L("Camera <>   iViewfinderSize         : (%dx%d)"), iViewfinderSize.iWidth, iViewfinderSize.iHeight );
  PRINT1( _L("Camera <>   iViewfinderFormat       :  %d"    ), iViewfinderFormat );

  PRINT1( _L("Camera <>   iSsState                : [%s]"   ), KCamCameraTriStateNames    [iSsState] );
  PRINT2( _L("Camera <>   iSnapshotSize           : (%dx%d)"), iSnapshotSize.iWidth, iSnapshotSize.iHeight );
  PRINT1( _L("Camera <>   iSnapshotFormat         : %d"     ), iSnapshotFormat );
  PRINT1( _L("Camera <>   iSnapshotAspectMaintain : %d"     ), iSnapshotAspectMaintain );

  PRINT1( _L("Camera <>   iBusy                   : %08b"     ), iBusy );

  PRINT ( _L("Camera <> --------------------------------------------------") );
  }


// ===========================================================================
// TCamCameraResolutionSupport class methods

TCamCameraResolutionSupport::TCamCameraResolutionSupport()
  : iResolutions( KResolutionArrayGranularity )
  {
  Reset();
  }

TCamCameraResolutionSupport::~TCamCameraResolutionSupport()
  {
  iResolutions.Close();
  }

void
TCamCameraResolutionSupport::Reset()
  {
  iForCameraIndex  = KInvalidCameraIndex;
  iForCameraFormat = -1;
  iResolutions.Reset();
  }

#ifdef CAMERAAPP_CAPI_V2_ADV
// ===========================================================================
// TCamAdvancedSettingInfo class methods

TCamAdvancedSettingInfo::TCamAdvancedSettingInfo()
  : iEvStepsSupport ( KAdvArrayGranularity ),
    iIsoRatesSupport( KAdvArrayGranularity )
  {
  Reset();
  }

TCamAdvancedSettingInfo::~TCamAdvancedSettingInfo()
  {
  iEvStepsSupport.Close();
  iIsoRatesSupport.Close();
  }

void
TCamAdvancedSettingInfo::Reset()
  {
  iForCameraIndex = KInvalidCameraIndex;

  iEvModesSupport = 0;
  iEvStepsSupport.Reset();
  iEvStepsValueInfo = ENotActive;

  iIsoRatesSupport.Reset();

  iDigitalZoomSupport.Reset();
  iDigitalZoomValueInfo = ENotActive;

#ifdef CAMERAAPP_CAPI_V2_IP
  iColourEffectSupport.Reset();
  iColourEffectValueInfo = ENotActive;

  iSharpnessSupport.Reset();
  iSharpnessValueInfo = ENotActive;
#endif

  iStabilizationModeSupport       = 0;
  iStabilizationEffectSupport     = 0;
  iStabilizationComplexitySupport = 0;
  }

// ---------------------------------------------------------------------------
// PrintInfo
// ---------------------------------------------------------------------------
//
void
TCamAdvancedSettingInfo::PrintInfo() const
  {
#ifdef _DEBUG
  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT ( _L("Camera <> Value infos:")                                       );
  PRINT1( _L("Camera <> EV value info            : %04b"), iEvStepsValueInfo      );
  PRINT1( _L("Camera <> Dig zoom value info      : %04b"), iDigitalZoomValueInfo  );
#ifdef CAMERAAPP_CAPI_V2_IP
  PRINT1( _L("Camera <> Colour effct value info  : %04b"), iColourEffectValueInfo );
  PRINT1( _L("Camera <> Sharpness value info     : %04b"), iSharpnessValueInfo    );
#endif
  PRINT1( _L("Camera <> ENotActive               : %04b"), ENotActive               );
  PRINT1( _L("Camera <> EBitField                : %04b"), EBitField                );
  PRINT1( _L("Camera <> EDiscreteSteps           : %04b"), EDiscreteSteps           );
  PRINT1( _L("Camera <> EDiscreteRangeMinMaxStep : %04b"), EDiscreteRangeMinMaxStep );
  PRINT1( _L("Camera <> EContinuousRangeMinMax   : %04b"), EContinuousRangeMinMax   );

  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT ( _L("Camera <> Dig zoom ratios:") );
  for( TInt i = 0; i < iDigitalZoomSupport.Count(); i++ )
    {
    PRINT2( _L("Camera <> step[%4d]: ratio[%4f]"), i, (TReal)iDigitalZoomSupport[i]/(TReal)KECamFineResolutionFactor );
    }
  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT ( _L("Camera <> ISO rates supported:") );
  for( TInt i = 0; i < iIsoRatesSupport.Count(); i++ )
    {
    PRINT2( _L("Camera <> rate[%3d]: %4d"), i, iIsoRatesSupport[i] );
    }
  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT ( _L("Camera <> EV steps supported:") );
  for( TInt i = 0; i < iEvStepsSupport.Count(); i++ )
    {
    PRINT2( _L("Camera <> step[%3d]: %4d"), i, iEvStepsSupport[i] );
    }

  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT1( _L("Camera <> Advanced EV modes support : %016b"), iEvModesSupport                    );
  PRINT1( _L("Camera <> EExposureAuto             : %016b"), CCamera::EExposureAuto             );
  PRINT1( _L("Camera <> EExposureNight            : %016b"), CCamera::EExposureNight            );
  PRINT1( _L("Camera <> EExposureBacklight        : %016b"), CCamera::EExposureBacklight        );
  PRINT1( _L("Camera <> EExposureCenter           : %016b"), CCamera::EExposureCenter           );
  PRINT1( _L("Camera <> EExposureSport            : %016b"), CCamera::EExposureSport            );
  PRINT1( _L("Camera <> EExposureVeryLong         : %016b"), CCamera::EExposureVeryLong         );
  PRINT1( _L("Camera <> EExposureSnow             : %016b"), CCamera::EExposureSnow             );
  PRINT1( _L("Camera <> EExposureBeach            : %016b"), CCamera::EExposureBeach            );
  PRINT1( _L("Camera <> EExposureProgram          : %016b"), CCamera::EExposureProgram          );
  PRINT1( _L("Camera <> EExposureAperturePriority : %016b"), CCamera::EExposureAperturePriority );
  PRINT1( _L("Camera <> EExposureShutterPriority  : %016b"), CCamera::EExposureShutterPriority  );
  PRINT1( _L("Camera <> EExposureManual           : %016b"), CCamera::EExposureManual           );
  PRINT1( _L("Camera <> EExposureSuperNight       : %016b"), CCamera::EExposureSuperNight       );
  PRINT1( _L("Camera <> EExposureInfra            : %016b"), CCamera::EExposureInfra            );
 PRINT ( _L("Camera <> --------------------------------------------------") );

#ifdef CAMERAAPP_CAPI_V2_IP
  PRINT ( _L("Camera <> Sharpness support:") );
  for( TInt i = 0; i < iSharpnessSupport.Count(); i++ )
    {
    PRINT2( _L("Camera <> step[%3d]: %4d"), i, iSharpnessSupport[i] );
    }
  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT ( _L("Camera <> Colour effect support:") );
  for( TInt i = 0; i < iColourEffectSupport.Count(); i++ )
    {
    PRINT2( _L("Camera <> effect[%3d]           : %016b"), i, iColourEffectSupport[i] );
    }
  PRINT1( _L("Camera <> EEffectNone           : %016b"), CIP::EEffectNone            );
  PRINT1( _L("Camera <> EEffectNegative       : %016b"), CIP::EEffectNegative        );
  PRINT1( _L("Camera <> EEffectMonochrome     : %016b"), CIP::EEffectMonochrome      );
  PRINT1( _L("Camera <> EEffectSolarize       : %016b"), CIP::EEffectSolarize        );
  PRINT1( _L("Camera <> EEffectSepia          : %016b"), CIP::EEffectSepia           );
  PRINT1( _L("Camera <> EEffectEmboss         : %016b"), CIP::EEffectEmboss          );
  PRINT1( _L("Camera <> EEffectRedEyeReduction: %016b"), CIP::EEffectRedEyeReduction );
  PRINT1( _L("Camera <> EEffectVivid          : %016b"), CIP::EEffectVivid           );
  PRINT1( _L("Camera <> EEffectLowSharpening  : %016b"), CIP::EEffectLowSharpening   );
  PRINT ( _L("Camera <> --------------------------------------------------") );
#endif // CAMERAAPP_CAPI_V2_IP

  PRINT ( _L("Camera <> --------------------------------------------------") );
  PRINT1( _L("Camera <> Stabilization modes support      : %016b"), iStabilizationModeSupport           );
  PRINT1( _L("Camera <>   EStabilizationModeOff          : %016b"), CAS::EStabilizationModeOff          );
  PRINT1( _L("Camera <>   EStabilizationModeHorizontal   : %016b"), CAS::EStabilizationModeHorizontal   );
  PRINT1( _L("Camera <>   EStabilizationModeVertical     : %016b"), CAS::EStabilizationModeVertical     );
  PRINT1( _L("Camera <>   EStabilizationModeRotation     : %016b"), CAS::EStabilizationModeRotation     );
  PRINT1( _L("Camera <>   EStabilizationModeAuto         : %016b"), CAS::EStabilizationModeAuto         );
  PRINT1( _L("Camera <>   EStabilizationModeManual       : %016b"), CAS::EStabilizationModeManual       );
  PRINT1( _L("Camera <> Stabilization fx support         : %016b"), iStabilizationEffectSupport         );
  PRINT1( _L("Camera <>   EStabilizationOff              : %016b"), CAS::EStabilizationOff              );
  PRINT1( _L("Camera <>   EStabilizationAuto             : %016b"), CAS::EStabilizationAuto             );
  PRINT1( _L("Camera <>   EStabilizationFine             : %016b"), CAS::EStabilizationFine             );
  PRINT1( _L("Camera <>   EStabilizationMedium           : %016b"), CAS::EStabilizationMedium           );
  PRINT1( _L("Camera <>   EStabilizationStrong           : %016b"), CAS::EStabilizationStrong           );
  PRINT1( _L("Camera <> Stabilization complexity support : %016b"), iStabilizationComplexitySupport     );
  PRINT1( _L("Camera <>   EStabilizationComplexityAuto   : %016b"), CAS::EStabilizationComplexityAuto   );
  PRINT1( _L("Camera <>   EStabilizationComplexityLow    : %016b"), CAS::EStabilizationComplexityLow    );
  PRINT1( _L("Camera <>   EStabilizationComplexityMedium : %016b"), CAS::EStabilizationComplexityMedium );
  PRINT1( _L("Camera <>   EStabilizationComplexityHigh   : %016b"), CAS::EStabilizationComplexityHigh   );
  PRINT ( _L("Camera <> --------------------------------------------------") );
#endif // _DEBUG
  }

#endif // CAMERAAPP_CAPI_V2_ADV


// ===========================================================================
// CCamCameraController class methods

// ---------------------------------------------------------------------------
// static 2-phase constructor
// ---------------------------------------------------------------------------
//
CCamCameraController*
CCamCameraController::NewL( MCamSettingProvider& aProvider,
                            CCamAppController& aAppController,
                            TInt aCameraIndex /*= 0*/ )
  {
  CCamCameraController* self
      = new (ELeave) CCamCameraController( aProvider, aAppController );

  CleanupStack::PushL( self );
  self->ConstructL( aCameraIndex );
  CleanupStack::Pop( self );

  return self;
  }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamCameraController::~CCamCameraController()
  {
  PRINT( _L("Camera => ~CCamCameraController") );

#ifdef CAMERAAPP_FLASH_SIMULATOR
  delete iFlashSimulator;
#endif

#ifdef CAMERAAPP_PERFORMANCE_CONTROLLER
  delete iPerformanceLogger;
#endif // CAMERAAPP_PERFORMANCE_CONTROLLER

  delete iIdle;
  iSettingArray.Close();

  // Remove all observers.
  // We do not want to broadcast events anymore in this phase.
  iObservers.Reset();
  iObserverInterests.Reset();

  ClearRequestQueue();
  ClearSettingQueue();


  PRINT( _L("Camera <> CCamCameraController: release current camera..") );
  // Release and null CCamera related objects.
  ReleaseCurrentCamera();
  PRINT( _L("Camera <> CCamCameraController: ..done") );

  delete iActive;

  iObservers.Close();
  iObserverInterests.Close();

#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  if( iCallbackTimer )
    {
    iCallbackTimer->Cancel();
    delete iCallbackTimer;
    }
#endif // CAMERAAPP_CAE_ERR_SIMULATION

  PRINT( _L("Camera <= ~CCamCameraController") );
  }


// ===========================================================================
// MCameraObserver2

// ---------------------------------------------------------------------------
// HandleEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleEvent( const TECAMEvent& aEvent )
  {
  // TUid iEventType;
  // TInt iErrorCode;
  PRINT2( _L("Camera => CCamCameraController::HandleEvent, event uid(0x%08x) error(%d)"),
          aEvent.iEventType.iUid,
          aEvent.iErrorCode );

  // If we are getting zoom event while saving video,
  // we must not handle the event
  TInt uidValue( aEvent.iEventType.iUid );
  if ( uidValue == KUidECamEventCameraSettingDigitalZoomUidValue
    && IsFlagOn( iInfo.iBusy, ECamBusySingle ) )
      {
       return;
      }

#ifdef CAMERAAPP_CAPI_V2

  // -------------------------------------------------------
  if( aEvent.iEventType == KUidECamEventReserveComplete )
    {
    HandleReserveGainEvent( aEvent.iErrorCode );
    }
  // -------------------------------------------------------
  else if( aEvent.iEventType == KUidECamEventPowerOnComplete )
    {
    HandlePowerOnEvent( aEvent.iErrorCode );
    }
  // -------------------------------------------------------
  else if( aEvent.iEventType == KUidECamEventCameraNoLongerReserved )
    {
    HandleReserveLostEvent( aEvent.iErrorCode );
    }
  // -------------------------------------------------------
  else if( !(IsFlagOn( iInfo.iState , ECamPowerOn )))
    {
    //If camera is in background then all the rest events will be ignored. Should return without leaving
    return;
    }
  // -------------------------------------------------------
  else if( aEvent.iEventType == KUidECamEventCameraSnapshot )
    {
    HandleSnapshotEvent( aEvent.iErrorCode );
    }
  // -------------------------------------------------------
  else if( aEvent.iEventType == KUidECamEventCameraSettingsOptimalFocus
        || aEvent.iEventType == KUidECamEventCameraSettingAutoFocusType2 )
    {
    HandleAutoFocusEvent( aEvent.iErrorCode, aEvent.iEventType );
    }
  // -------------------------------------------------------
  else
    {
#ifdef CAMERAAPP_CAPI_V2_ADV
    TInt uidValue( aEvent.iEventType.iUid );
    switch( uidValue )
      {
      // -------------------------------
      // Settings
      //
      // case KUidECamEventCameraSettingTimerIntervalUidValue:
      // case KUidECamEventCameraSettingTimeLapseUidValue:
      //
      case KUidECamEventCameraSettingFlashModeUidValue:
      case KUidECamEventCameraSettingExposureCompensationStepUidValue:
      case KUidECamEventCameraSettingIsoRateTypeUidValue:
      case KUidECamEventCameraSettingWhiteBalanceModeUidValue:
      case KUidECamEventCameraSettingDigitalZoomUidValue:
      case KUidECamEventCameraSettingOpticalZoomUidValue:
      case KUidECamEventImageProcessingAdjustSharpnessUidValue:
      case KUidECamEventImageProcessingEffectUidValue:
      case KUidECamEventSettingsStabilizationAlgorithmComplexityUidValue:
        {
        TCamCameraSettingId id =
            CCamSettingConversion::Map2CameraSetting( uidValue );

        HandleCallbackEvent( aEvent.iErrorCode,
                             ECamCameraEventSettingsSingle,
                             ECamCameraEventClassSettings,
                             &id );
        break;
        }
      // -------------------------------
      // Flash status info
      case KUidECamEventFlashReadyUidValue:
        HandleFlashStatusEvent( aEvent.iErrorCode, ECamCameraEventFlashReady );
        break;
      case KUidECamEventFlashNotReadyUidValue:
        HandleFlashStatusEvent( aEvent.iErrorCode, ECamCameraEventFlashNotReady );
        break;
      // -------------------------------
      default:
        break;
      // -------------------------------
      }
#endif // CAMERAAPP_CAPI_V2_ADV
    }

#else // CAMERAAPP_CAPI_V2

  // Only snapshot event recognized if old observer interface in
  // use towards CCamera. This one is then generated by CCamSnapshot.
  if( aEvent.iEventType == KUidECamEventCameraSnapshot )
    {
    HandleSnapshotEvent( aEvent.iErrorCode );
    }
  else
    {
    // ignore other events
    PRINT( _L("Camera <> CCamCameraController: Unknown event") );
    }
#endif
  // -------------------------------------------------------
  PRINT( _L("Camera <= CCamCameraController::HandleEvent") );
  }



// ---------------------------------------------------------------------------
// ViewFinderReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ViewFinderReady( MCameraBuffer& aCameraBuffer, TInt aError )
  {
  HandleViewfinderEvent( &aCameraBuffer, aError );
  }


// ---------------------------------------------------------------------------
// ImageBufferReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ImageBufferReady( MCameraBuffer& aCameraBuffer, TInt aError )
  {
  HandleImageCaptureEvent( &aCameraBuffer, aError );
  }


// ---------------------------------------------------------------------------
// VideoBufferReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::VideoBufferReady( MCameraBuffer& aCameraBuffer, TInt aError )
  {
  if( KErrNone == aError )
    {
    aCameraBuffer.Release();
    }
  }



#if !defined( CAMERAAPP_CAPI_V2 )
// ===========================================================================
// MCameraObserver


// ---------------------------------------------------------------------------
// ReserveComplete
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ReserveComplete( TInt aError )
  {
  HandleReserveGainEvent( aError );
  }


// ---------------------------------------------------------------------------
// PowerOnComplete
// ---------------------------------------------------------------------------
//
void
CCamCameraController::PowerOnComplete( TInt aError )
  {
  HandlePowerOnEvent( aError );
  }


// ---------------------------------------------------------------------------
// ViewFinderFrameReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ViewFinderFrameReady( CFbsBitmap& aFrame )
  {
  PRINT( _L("CamTest => CCamCameraController::ViewFinderFrameReady") );

  CCamBuffer* buffer = NULL;

  TRAPD( error, buffer = CCamBuffer::NewL( aFrame, NULL ) );

  // Releases buffer
  HandleViewfinderEvent( buffer, error );

  PRINT( _L("CamTest <= CCamCameraController::ViewFinderFrameReady") );
  }


// ---------------------------------------------------------------------------
// ImageReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ImageReady( CFbsBitmap* aBitmap,
                                  HBufC8*    aData,
                                  TInt       aError )
  {
  PRINT( _L("Camera => CCamCameraController::ImageReady") );

  CCamBuffer* buffer = NULL;

  // If no error so far, wrap the data to MCameraBuffer compatible wrapper.
  // New API provides these buffers already from CCamera callbacks.
  if( KErrNone == aError )
    {
    TRAP( aError,
      {
      CleanupStack::PushL( aData   );
      CleanupStack::PushL( aBitmap );
      buffer = CCamBuffer::NewL( aBitmap, aData );
      CleanupStack::Pop( 2 );
      });
    }

  // Ownership taken by buffer, or destroyed by cleanup stack.
  aData   = NULL;
  aBitmap = NULL;

  PRINT1( _L("Camera <> status after buffer create(%d)"), aError );

  // Releases buffer
  HandleImageCaptureEvent( buffer, aError );

  PRINT( _L("Camera <= CCamCameraController::ImageReady") );
  }


// ---------------------------------------------------------------------------
// FrameBufferReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::FrameBufferReady( MFrameBuffer* aFrameBuffer,
                                        TInt          /*aError*/ )
  {
  // Release the buffer if one is provided to make sure
  // CCamera can reuse it.
  if( aFrameBuffer )
    {
    aFrameBuffer->Release();
    }
  }
#endif // CAMERAAPP_CAPI_V2


#ifdef CAMERAAPP_CAE_FOR_VIDEO
// ===========================================================================
// From MCamAppEngineObserver

// ---------------------------------------------------------------------------
// McaeoInitComplete
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoInitComplete( TInt aError )
  {
  PRINT( _L("Camera => CCamCameraController::McaeoInitComplete") );
  // Only interested on this callback, if CCamera duplicate
  // cannot be used with CAE. Otherwise we follow this event through
  // MCameraObserver(2) interface.
#ifdef CAMERAAPP_CAE_FIX
  HandlePowerOnEvent( aError );
#endif
  PRINT( _L("Camera <= CCamCameraController::McaeoInitComplete") );
  }

// ---------------------------------------------------------------------------
// McaeoStillPrepareComplete
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoStillPrepareComplete( TInt /*aError*/ )
  {
  PRINT( _L("Camera =><= CCamCameraController::McaeoStillPrepareComplete, PANIC!") );
  // Still capture not done through CCaeEngine.
  __ASSERT_DEBUG( EFalse, Panic( ECamCameraControllerCaeUnsupported ) );
  }

// ---------------------------------------------------------------------------
// McaeoVideoPrepareComplete
//
// This method is called asynchronously after a call has been made to
// CCaeEngine::PrepareVideoRecordingL.
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoVideoPrepareComplete( TInt aError )
  {
  PRINT( _L("Camera => CCamCameraController::McaeoVideoPrepareComplete") );
#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  HandleVideoEvent( ECamCameraEventVideoInit, aError );
  // DelayCallback( ECamCameraEventVideoInit, aError, 500000 );
#else
  if( aError == KErrWrite )
      {
      aError = KErrNone;
      }
  if( aError &&
      ( iIveCancel || ( !iIveSequenceActive && iIveRecoveryOngoing ) ) )
      // Return if error and recovering process has been started,
      // but this video prepare complete is not part of recovery
      // i.e. iIveSequenceActive is not active yet.
    {
    PRINT1( _L("Camera => CCamCameraController::McaeoVideoPrepareComplete - Ignore err %d"), aError );
    return;
    }
  HandleVideoEvent( ECamCameraEventVideoInit, aError );
#endif // CAMERAAPP_CAE_ERR_SIMULATION

  PRINT( _L("Camera <= CCamCameraController::McaeoVideoPrepareComplete") );
  }

// ---------------------------------------------------------------------------
// McaeoViewFinderFrameReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoViewFinderFrameReady( CFbsBitmap& aFrame,
                                                 TInt        aError )
  {
  PRINT( _L("Camera => CCamCameraController::McaeoViewFinderFrameReady") );
  CCamBuffer* buffer = NULL;
  if( KErrNone == aError )
    {
    TRAP( aError, buffer = CCamBuffer::NewL( aFrame, NULL ) );
    }

  // Releases buffer if not NULL
  HandleViewfinderEvent( buffer, aError );

  PRINT( _L("Camera <= CCamCameraController::McaeoViewFinderFrameReady") );
  }

// ---------------------------------------------------------------------------
// McaeoSnapImageReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoSnapImageReady( const CFbsBitmap& aBitmap,
                                                 TInt        aError  )
  {
  PRINT1( _L("Camera => CCamCameraController::McaeoSnapImageReady, status: %d"), aError );
  //__ASSERT_DEBUG( EFalse, Panic( ECamCameraControllerCaeUnsupported ) );
  if(
#ifdef CAMERAAPP_CAE_FIX
      ECamModeChangeInactive == iModeChange &&
#endif
      IsFlagOn( iInfo.iState, ECamVideoOn )
    )
    {
    CAMERAAPP_PERF_CONTROLLER_STOP( ECamRequestSsStart );

    CFbsBitmap* copy( NULL );
    if( KErrNone == aError )
      {
      TRAP( aError,
        {
        copy = new (ELeave) CFbsBitmap;
        User::LeaveIfError( copy->Duplicate( aBitmap.Handle() ) );
        });
      }

    NotifyObservers( aError,
                     ECamCameraEventSsReady,
                     ECamCameraEventClassSsData,
                     copy );
    delete copy;
    copy = NULL;
    }
  PRINT( _L("Camera <= CCamCameraController::McaeoSnapImageReady") );
  }

// ---------------------------------------------------------------------------
// McaeoStillImageReady
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoStillImageReady( CFbsBitmap* /*aBitmap*/,
                                            HBufC8*     /*aData  */,
                                            TInt        /*aError */ )
  {
  // Still images are not captured through CCaeEngine.
  __ASSERT_DEBUG( EFalse, Panic( ECamCameraControllerCaeUnsupported ) );
  }

// ---------------------------------------------------------------------------
// McaeoVideoRecordingOn
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoVideoRecordingOn( TInt aError )
  {
  PRINT( _L("Camera => CCamCameraController::McaeoVideoRecordingOn") );

#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  HandleVideoEvent( ECamCameraEventVideoStart, aError );
#else
  HandleVideoEvent( ECamCameraEventVideoStart, aError );
#endif // CAMERAAPP_CAE_ERR_SIMULATION

  PRINT( _L("Camera <= CCamCameraController::McaeoVideoRecordingOn") );

  }

// ---------------------------------------------------------------------------
// McaeoVideoRecordingPaused
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoVideoRecordingPaused( TInt aError )
  {
  PRINT( _L("Camera => CCamCameraController::McaeoVideoRecordingPaused") );

#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  HandleVideoEvent( ECamCameraEventVideoPause, KErrUnknown );
#else
  HandleVideoEvent( ECamCameraEventVideoPause, aError );
#endif // CAMERAAPP_CAE_ERR_SIMULATION

  PRINT( _L("Camera <= CCamCameraController::McaeoVideoRecordingPaused") );

  }

// ---------------------------------------------------------------------------
// McaeoVideoRecordingComplete
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoVideoRecordingComplete( TInt aError )
  {
  PRINT( _L("Camera => CCamCameraController::McaeoVideoRecordingComplete") );

#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  HandleVideoEvent( ECamCameraEventVideoStop, KErrUnknown );
#else
  if( aError == KErrWrite )
      {
      aError = KErrNone;
      }
  HandleVideoEvent( ECamCameraEventVideoStop, aError );
#endif // CAMERAAPP_CAE_ERR_SIMULATION
  // Change stopping mode back to default sync mode
  if ( iAsyncVideoStopModeSupported )
    {
    if ( iCaeEngine )
        {
        iCaeEngine->SetAsyncVideoStopMode( EFalse );
        }
    iAsyncVideoStopModeSupported = EFalse;
    }

  PRINT( _L("Camera <= CCamCameraController::McaeoVideoRecordingComplete") );

  }

// ---------------------------------------------------------------------------
// McaeoVideoRecordingTimes
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoVideoRecordingTimes(
    TTimeIntervalMicroSeconds aTimeElapsed,
    TTimeIntervalMicroSeconds aTimeRemaining,
    TInt aError )
  {
  HandleVideoTimeEvent( aError, aTimeElapsed, aTimeRemaining );
  }

// ---------------------------------------------------------------------------
// McaeoVideoRecordingStopped
// ---------------------------------------------------------------------------
//
void
CCamCameraController::McaeoVideoRecordingStopped()
  {
  PRINT( _L("Camera => CCamCameraController::McaeoVideoRecordingStopped") );

#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  HandleVideoEvent( ECamCameraEventVideoAsyncStop, KErrUnknown );
#else
  HandleVideoEvent( ECamCameraEventVideoAsyncStop, KErrNone );
#endif // CAMERAAPP_CAE_ERR_SIMULATION

  PRINT( _L("Camera <= CCamCameraController::McaeoVideoRecordingStopped") );
  }

#endif // CAMERAAPP_CAE_FOR_VIDEO
// ===========================================================================
// From MCamImageEncoderObserver

// ---------------------------------------------------------------------------
// ImageEncoded
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ImageEncoded( TInt aStatus, HBufC8* aData )
  {
  PRINT( _L("Camera => CCamCameraController::ImageEncoded") );
  MCameraBuffer* buffer = NULL;

  if( KErrNone == aStatus )
    {
    if( aData )
      {
      TRAP( aStatus,
        {
        CleanupStack::PushL( aData );
        buffer = CCamBuffer::NewL( NULL, aData );
        CleanupStack::Pop( aData );
        aData  = NULL; // ownership to buffer
        });
      }
    else
      {
      aStatus = KErrNotFound;
      }
    }
  else
    {
    // Just delete on errors
    delete aData;
    aData = NULL;
    }

  // Takes ownership of buffer
  HandleImageCaptureEvent( buffer, aStatus );
  PRINT( _L("Camera <= CCamCameraController::ImageEncoded") );
  }


// ===========================================================================


// ---------------------------------------------------------------------------
// AttachObserverL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::AttachObserverL( const MCamCameraObserver* aObserver,
                                       const TUint&              aInterest )
  {
  // Only add the observer once. NULL pointer only checked here.
  // Observer not added, if no notifications would be sent due to
  // event interest being ECamCameraEventClassNone.
  if( aObserver
   && ECamCameraEventClassNone != aInterest
   && KErrNotFound             == iObservers.Find( aObserver )
    )
    {
    // Try to add the observer to our list.
    TInt error = iObservers.Append( aObserver );

    // If we manage to add the observer, make sure the interest info gets set.
    if( KErrNone == error )
      {
      error = iObserverInterests.Append( aInterest );
      // If we are unable to add the interest info,
      // remove also the observer.
      if( KErrNone != error )
        {
        iObservers.Remove( iObservers.Count() - 1 );
        }
      }

    User::LeaveIfError( error );
    }
  }

// ---------------------------------------------------------------------------
// DetachObserver
// ---------------------------------------------------------------------------
//
void
CCamCameraController::DetachObserver( const MCamCameraObserver* aObserver )
  {
  if( aObserver )
    {
    // Ignore error if not found among observers
    const TInt KObserverIndex( iObservers.Find( aObserver ) );
    if( KErrNotFound != KObserverIndex )
      {
      iObservers.Remove( KObserverIndex );
      iObserverInterests.Remove( KObserverIndex );
      }
    }
  }


// ---------------------------------------------------------------------------
// RequestL
//
// Issue request for one operation.
// ---------------------------------------------------------------------------
//
void
CCamCameraController::RequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT1( _L("Camera => CCamCameraController::RequestL, request:[%s]"), KCamRequestNames[aRequestId] );
  CheckEqualsL( iInfo.iBusy, ECamBusyOff,  KErrInUse );
  SetFlags    ( iInfo.iBusy, ECamBusySingle );

  TInt proceed( ETrue );

  // If the request leaves, need to reset busy flag.
  CleanupStack::PushL( TCleanupItem( SetNotBusy, &iInfo.iBusy ) );
  PRINT( _L("Camera <> process request..") );
  proceed = ProcessOneRequestL( aRequestId );
  CleanupStack::Pop();

  // If this request will be responded through MCameraObserver(2) callback,
  // new requests cannot be accepted until that call arrives and
  // notification to our observers will be sent there.
  // Otherwise the request is fully processed from our point of view,
  // and new request can be accepted.
  if( proceed )
    {
    PRINT( _L("Camera <> ..Request has no callback from CCamera, ready for new request.") );
    // Need to clear the busy flag before notification,
    // as observer might want to issue a new request during
    // the notification callback.
    ClearFlags( iInfo.iBusy, ECamBusySingle );

    if( ECamRequestVideoStop == aRequestId
     || ECamRequestSetAsyncVideoStopMode == aRequestId
     || ECamRequestImageCancel == aRequestId )
      {
      // For video stop request, callback has already happened and notification
      // has been given. No need to do anything here.
      }
    else
      {
      // Give notification to observers
      TCamCameraEventId event( Request2Event( aRequestId ) );
      NotifyObservers( KErrNone, event, EventClass( event ) );
      }
    }
  else
    {
    PRINT( _L("Camera <> ..Request has callback, waiting for it.") );
    // Request is finalized in the MCameraObserver(2) callback.
    }
  PRINT( _L("Camera <= CCamCameraController::RequestL") );
  }

// ---------------------------------------------------------------------------
// DirectRequestL
//
// Issue request even when busy.
// ---------------------------------------------------------------------------
//
void
CCamCameraController::DirectRequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::DirectRequestL") );

  TBool notify( ETrue );
  switch( aRequestId )
    {
    // -----------------------------------------------------
    case ECamRequestPowerOff:
    case ECamRequestRelease:
      ProcessControlShutdownRequest( aRequestId );
      break;
    // -----------------------------------------------------
    // Viewfinder
    // This does not depend on prepare of video / image
    // so even starting can be done during sequence.
    case ECamRequestVfStart:
      ProcessVfStartRequestL();
      break;
    case ECamRequestVfStop:
      ProcessVfStopRequestL();
      break;
    case ECamRequestVfStopEcam:
      ProcessVfStopEcamRequest();
      notify = EFalse;
      break;
    case ECamRequestVfRelease:
      ProcessVfRelaseRequest();
      break;
    // -----------------------------------------------------
    // Snapshot
    //   Just as viewfinder, this can be even started
    //   during a sequence.
    case ECamRequestSsStart:
      ProcessSnapshotRequestL( ECamRequestSsStart );
      break;
    case ECamRequestSsStop:
      ProcessSsStopRequest();
      break;
    case ECamRequestSsRelease:
      ProcessSsReleaseRequest();
      break;
    // -----------------------------------------------------
    // Image capture (cancel)
    case ECamRequestImageCancel:
      ProcessImageShutdownRequest( ECamRequestImageCancel );
      notify = EFalse;
      break;
    // -----------------------------------------------------
    // Start/cancel autofocus or set range
    case ECamRequestStartAutofocus:
    case ECamRequestCancelAutofocus:
    case ECamRequestSetAfRange:
      ProcessAutofocusRequestL( aRequestId );
      break;
    // -----------------------------------------------------
    // Initialize image quality
    case ECamRequestImageInit:
      ProcessImageRequestL( ECamRequestImageInit );
      break;
    // -----------------------------------------------------
    default:
      Panic( ECamCameraControllerUnsupported );
      break;
    }

  if( notify )
    {
    TCamCameraEventId event( Request2Event( aRequestId ) );
    NotifyObservers( KErrNone, event, EventClass( event ) );
    }

  PRINT( _L("Camera <= CCamCameraController::DirectRequestL") );
  }

// ---------------------------------------------------------------------------
// RequestSequenceL
//
// Issue request for a sequence of operations.
//
// Operations done during the sequence will be notified with
// associated event codes to observers. If any errors happen during the
// sequence, the notification will reflect this with the status code.
// When sequence ends, observers will be notified with event
// ECamCameraEventSequenceEnd. No new requests are accepted before
// ECamCameraEventSequenceEnd notification is sent.
// ---------------------------------------------------------------------------
//
void
CCamCameraController
::RequestSequenceL( const RCamRequestArray& aSequence )
  {
  PRINT( _L("Camera => CCamCameraController::RequestSequence") );
  CheckEqualsL( iInfo.iBusy, ECamBusyOff,  KErrInUse );

  CAMERAAPP_PERF_CONTROLLER_START( ECamRequestNone );

  ClearRequestQueue();
  iReserveTryAgainCount = KCamReserveTryAgainMaxCount;

  iSequenceArray.ReserveL( aSequence.Count() );
  for( TInt i = 0; i < aSequence.Count(); i++ )
    iSequenceArray.AppendL( aSequence[i] );
//  iSequenceArray.Compress();

  SetFlags( iInfo.iBusy, ECamBusySequence ); // Cleared in EndSequence
  iActive->IssueRequest();

  PRINT( _L("Camera <= CCamCameraController::RequestSequence") );
  }


// ---------------------------------------------------------------------------
// RequestSettingsChangeL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::RequestSettingsChangeL()
  {
  PRINT ( _L("Camera => CCamCameraController::RequestSettingsChangeL") );
  CheckEqualsL( iInfo.iBusy, ECamBusyOff,  KErrInUse    );
  CheckFlagOnL( iInfo.iState, ECamReserved, KErrNotReady );

  PRINT ( _L("Camera <> Get changed settings..") );
  ClearSettingQueue();
  iSettingProvider.ProvidePendingSettingChangesL( iSettingArray );

  PRINT ( _L("Camera <> Start processing..") );
  SetFlags( iInfo.iBusy, ECamBusySetting ); // Cleared in EndSequence
  iActive->IssueRequest();

  PRINT ( _L("Camera <= CCamCameraController::RequestSettingsChangeL") );
  }


// ---------------------------------------------------------------------------
// DirectRequestSettingsChangeL
// ---------------------------------------------------------------------------
//
void
CCamCameraController
::DirectSettingsChangeL( const TCamCameraSettingId& aSettingId )
  {
  PRINT ( _L("Camera => CCamCameraController::DirectSettingsChangeL") );

  CheckFlagOnL( iInfo.iState, ECamReserved, KErrNotReady );

  TBool callback = ProcessSettingL( aSettingId );
  // If there is a callback, do notification there.
  if( !callback )
    {
    TCamCameraSettingId setting = aSettingId;
    NotifyObservers( KErrNone,
                     ECamCameraEventSettingsSingle,
                     ECamCameraEventClassSettings,
                     &setting );
    }

  PRINT ( _L("Camera <= CCamCameraController::DirectSettingsChangeL") );
  }


// ---------------------------------------------------------------------------
// CancelSequence
// ---------------------------------------------------------------------------
//
void
CCamCameraController::CancelSequence()
  {
  PRINT( _L("Camera => CCamCameraController::CancelSequence") );
  if( iActive &&
      iActive->IsActive() )
    {
    PRINT( _L("Camera <> iActive is active, call Cancel()..") );
    // Results to call to iActive::DoCancel
    // which in turn calls our EndSequence.
    iActive->Cancel();
    }

  // If busy flag is not set, we either had no sequence going on or
  // iActive::DoCancel already handled calling EndSequence().
  if( IsFlagOn( iInfo.iBusy, ECamBusySequence|ECamBusySetting ) )
    {
    PRINT( _L("Camera <> Busy, call EndSequence()..") );
    // Clears busy flag, if we were processing a sequence of requests / settings.
    // If iBusy is set because one request is on the way,
    // we must not clear it here - when that request is processed,
    // the flag will be cleared.
    EndSequence( KErrCancel );
    }
  PRINT( _L("Camera <= CCamCameraController::CancelSequence") );
  }

// ---------------------------------------------------------------------------
// SwitchCameraL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::SwitchCameraL( TInt aCameraIndex )
  {
  PRINT1( _L("Camera => CCamCameraController::SwitchCameraL%d"),aCameraIndex );
  #ifdef _DEBUG
  if( aCameraIndex == iInfo.iCurrentCamera )
    {
    PRINT( _L("Camera <> CCamCameraController::SwitchCameraL - Changing Secondary camera orientation") );
    }
  #endif // _DEBUG

  if( aCameraIndex < 0 || aCameraIndex >= CamerasAvailable() )
    {
    User::Leave( KErrNotSupported );
    }
  else
    {
    // -----------------------------------------------------
    // First free old camera resources
    if( iCamera )
      {
      ReleaseCurrentCamera();
      }

    // used in CompleteSwitchCamera
    iCurrentCameraIndex = aCameraIndex;

    // -----------------------------------------------------
    // Then create new camera:
    PRINT ( _L("############################################################") );
#ifdef CAMERAAPP_CAE_FOR_VIDEO

    PRINT1( _L("Camera <> Creating CCaeEngine, camera index: %d .."), aCameraIndex );
    iCaeEngine = NewCaeEngineL( aCameraIndex );

#else

    PRINT ( _L("Camera <> Note: CCamera only in use, no CCaeEngine") );
    PRINT1( _L("Camera <> Creating CCamera, camera index: %d .."), aCameraIndex );
    iCamera = NewCameraL( aCameraIndex );

#endif // CAMERAAPP_CAE_FOR_VIDEO


  PRINT( _L("Camera <= CCamCameraController::SwitchCameraL") );
    }
  }
// ---------------------------------------------------------------------------
// CompleteSwitchCameraL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::CompleteSwitchCameraL()
  {
  PRINT( _L("Camera => CCamCameraController::CompleteSwitchCameraL") );
  if( iCurrentCameraIndex < 0 || iCurrentCameraIndex >= CamerasAvailable() )
    {
    User::Leave( KErrNotSupported );
    }
  else if( iCurrentCameraIndex == iInfo.iCurrentCamera )
    {
    return;
    }

#ifdef CAMERAAPP_CAE_FOR_VIDEO
  #ifdef CAMERAAPP_CAE_FIX
    PRINT ( _L("Camera <> Creating new CCamera..") );
    iCamera    = NewCameraL( iCurrentCameraIndex );
  #else
    PRINT1( _L("Camera <> Creating CCamera duplicate, camera handle: %d .."), iCaeEngine->CCameraHandle() );
    iCamera    = NewDuplicateCameraL( iCaeEngine->CCameraHandle() );
  #endif
#endif // CAMERAAPP_CAE_FOR_VIDEO
    // Get the camera info and store current camera index.
    // Camera index needed by GetAdvancedSettingsInfoL.
    iCamera->CameraInfo( iCameraInfo );
    iInfo.iCurrentCamera = iCurrentCameraIndex;

#ifdef CAMERAAPP_CAPI_V2_ADV
    PRINT ( _L("Camera <> Creating CCameraAdvancedSettings..") );
    iAdvancedSettings = CCamera::CCameraAdvancedSettings::NewL( *iCamera );
#ifdef CAMERAAPP_CAPI_V2_IP
    if( KPrimaryCameraIndex == iInfo.iCurrentCamera )
      {
      // Support only for primary camera.
      // Ignore error in instantiation: If NewL leaves, there's no
      // support for Image Processing available. Report error to client
      // if settings requiring it are used.
      PRINT ( _L("Camera <> Create CCameraImageProcessing..") );
      TRAP_IGNORE( iImageProcessor = CCamera::CCameraImageProcessing::NewL( *iCamera ) );
      PRINT1( _L("Camera <> Image Processor pointer: %d"), iImageProcessor );
      }
#endif // CAMERAAPP_CAPI_V2_IP

    // Get info on CCameraAdvancedSettings and CCameraImageProcessing
    // support for several setting. Not supported settings will not
    // cause this method to leave, only more serious errors.
    GetAdvancedSettingsInfoL();

    PRINT ( _L("Camera <> Get i/f MCameraOrientation..") )
    iCustomInterfaceOrientation =
      static_cast <MCameraOrientation*>(
  	    iCamera->CustomInterface( KCameraOrientationUid ) );
    PRINT1( _L("Camera <> Orientation custom i/f pointer:%d"), iCustomInterfaceOrientation );
    
    iCustomInterfaceUseCaseHint =
      static_cast <MCameraUseCaseHint*>(
  	    iCamera->CustomInterface( KCameraUseCaseHintUid ) );
    PRINT1( _L("Camera <> UseCaseHint custom i/f pointer:%d"), iCustomInterfaceUseCaseHint );
    
    
    

#endif // CAMERAAPP_CAPI_V2_ADV

    PRINT ( _L("Camera <> ..done") );
    PRINT ( _L("############################################################") );

    // -----------------------------------------------------
#ifdef _DEBUG
    PrintCameraInfo();
#endif
    // -----------------------------------------------------
  PRINT( _L("Camera <= CCamCameraController::CompleteSwitchCameraL") );
  }


// ---------------------------------------------------------------------------
// SetOrientationModeL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::SetOrientationModeL( TInt aOrientation )
  {
  PRINT1( _L("Camera => CCamCameraController::SetOrientationModeL %d"), aOrientation );
#ifdef CAMERAAPP_CAE_FOR_VIDEO
    PRINT( _L("Camera <> CCameraController: Get i/f MUIOrientationOverride from iCaeEngine..") )
    iCustomInterfaceUIOrientationOverride =
      static_cast <MCameraUIOrientationOverride*>(
  	    iCaeEngine->CustomInterface( KCameraUIOrientationOverrideUid ) );
#else
    PRINT( _L("Camera <> CCameraController: Get i/f MUIOrientationOverride from iCamera..") )
    iCustomInterfaceUIOrientationOverride =
      static_cast <MCameraUIOrientationOverride*>(
  	    iCamera->CustomInterface( KCameraUIOrientationOverrideUid ) );
#endif // CAMERAAPP_CAE_FOR_VIDEO

    PRINT1( _L("Camera <> OrientationOverride custom i/f pointer:%d"), iCustomInterfaceUIOrientationOverride );
    TRAP_IGNORE(iCustomInterfaceUIOrientationOverride->SetOrientationModeL( aOrientation ));

  PRINT( _L("Camera <= CCamCameraController::SetOrientationModeL") );
  }

// ---------------------------------------------------------------------------
// CamerasAvailable <<static>>
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::CamerasAvailable()
  {
#ifndef CAMERAAPP_CAPI_EMULATOR
  return CCamera::CamerasAvailable();
#else
  // Temporary
  return 2;
#endif
  }

// ---------------------------------------------------------------------------
// CameraHandle
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::CameraHandle()
  {
  if( iCamera )
    return iCamera->Handle();
  else
    return KNullHandle;
  }


// ---------------------------------------------------------------------------
// CameraInfo
// ---------------------------------------------------------------------------
//
const TCameraInfo&
CCamCameraController::CameraInfo() const
  {
  return iCameraInfo;
  }

// ---------------------------------------------------------------------------
// ControllerInfo
// ---------------------------------------------------------------------------
//
const TCamControllerInfo&
CCamCameraController::ControllerInfo() const
  {
  return iInfo;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint
CCamCameraController::State() const
  {
  return iInfo.iState;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamViewfinderMode
CCamCameraController::ViewfinderMode()  const
  {
  return iInfo.iVfMode;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCameraTriState
CCamCameraController::ViewfinderState() const
  {
  return iInfo.iVfState;
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCameraTriState
CCamCameraController::SnapshotState() const
  {
  return iInfo.iSsState;
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCameraReadyState
CCamCameraController::FlashState() const
  {
  TCamCameraReadyState state = ECamUnknown;

#ifdef CAMERAAPP_CAPI_V2_ADV
  if( iAdvancedSettings )
    {
    TBool ready       = EFalse;
    TInt  queryStatus = iAdvancedSettings->IsFlashReady( ready );

    if( KErrNone == queryStatus )
      {
      state = ready ? ECamReady : ECamNotReady;
      }
    }
#endif // CAMERAAPP_CAPI_V2_ADV
  return state;
  }



// ---------------------------------------------------------------------------
// GetCameraSettingValueL
// ---------------------------------------------------------------------------
//
void
CCamCameraController
::GetCameraSettingValueL( const TCamCameraSettingId& aSettingId,
                          TAny* aSettingData )
  {
  PRINT( _L("Camera => CCamCameraController::GetCameraSettingValueL") );
  CheckNonNullL( aSettingData, KErrArgument );

  switch( aSettingId )
    {
    // -----------------------------------------------------
    case ECameraSettingFlash:
    case ECameraUserSceneSettingFlash:
      {
      CCamera::TFlash* flash = static_cast<CCamera::TFlash*>( aSettingData );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        *flash = iCaeEngine->FlashMode();
      else
#endif
        {
        *flash = iCamera->Flash();
        }
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingExposure:
    case ECameraUserSceneSettingExposure:
      {
      TPckgBuf<TCamSettingDataExposure>* exposure =
          static_cast<TPckgBuf<TCamSettingDataExposure>*>( aSettingData );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        (*exposure)().iExposureMode = iCaeEngine->ExposureMode();
      else
#endif // CAMERAAPP_CAE_FIX
        {
  #ifdef CAMERAAPP_CAPI_V2_ADV
        (*exposure)().iExposureMode = iAdvancedSettings->ExposureMode();
        (*exposure)().iExposureStep = iAdvancedSettings->ExposureCompensationStep();
  #else
        (*exposure)().iExposureMode = iCamera->Exposure();
  #endif
        }
      break;
      }
    // -----------------------------------------------------
#ifdef CAMERAAPP_CAPI_V2_ADV
    case ECameraSettingLightSensitivity:
    case ECameraUserSceneSettingLightSensitivity:
      {
      TInt* iso = static_cast<TInt*>( aSettingData );
      *iso = iAdvancedSettings->IsoRate();
      break;
      }
    case ECameraSettingContAF:
      {
      TInt* contAF = static_cast<TInt*>( aSettingData );
      *contAF = iAdvancedSettings->AutoFocusType() &  //bitwise
                CAS::EAutoFocusTypeContinuous;
      break;
      }
#endif
    // -----------------------------------------------------
    case ECameraSettingWhiteBalance:
    case ECameraUserSceneSettingWhiteBalance:
      {
      TPckgBuf<TCamSettingDataWhiteBalance>* wb =
          static_cast<TPckgBuf<TCamSettingDataWhiteBalance>*>( aSettingData );

#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        (*wb)().iWhiteBalanceMode = iCaeEngine->WhiteBalanceMode();
      else
#endif // CAMERAAPP_CAE_FIX
        {
        (*wb)().iWhiteBalanceMode = iCamera->WhiteBalance();
        }
      break;
      }
    // -----------------------------------------------------
#ifdef CAMERAAPP_CAPI_V2_IP
    case ECameraSettingColourEffect:
    case ECameraUserSceneSettingColourEffect:
      {
      CheckNonNullL( iImageProcessor, KErrNotSupported );

      CIP::TEffect* effect = static_cast<CIP::TEffect*>( aSettingData );
      *effect =
        (CIP::TEffect)
          iImageProcessor->TransformationValue( KUidECamEventImageProcessingEffect );
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingSharpness:
    case ECameraUserSceneSettingSharpness:
      {
      CheckNonNullL( iImageProcessor, KErrNotSupported );

      TInt* sharpness = static_cast<TInt*>( aSettingData );
      *sharpness = iImageProcessor->TransformationValue( KUidECamEventImageProcessingAdjustSharpness );
      break;
      }
#endif
    // -----------------------------------------------------
    case ECameraSettingDigitalZoom:
      {
      TInt* zoom = static_cast<TInt*>( aSettingData );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        *zoom = iCaeEngine->ZoomValue();
      else
#endif
        {
        // Zoom step despite the misleading method name.
        *zoom = iCamera->ZoomFactor();
        }
      break;
      }
    // -----------------------------------------------------
#ifdef CAMERAAPP_CAPI_V2_ADV
    case ECameraSettingOpticalZoom:
      {
      TInt* zoom = static_cast<TInt*>( aSettingData );
      *zoom = iAdvancedSettings->OpticalZoom();
      break;
      }
#endif
    // -----------------------------------------------------
#ifdef CAMERAAPP_CAE_FOR_VIDEO
    case ECameraSettingFileMaxSize:
      {
      if( iCaeEngine )
        {
        TInt* bytes = static_cast<TInt*>( aSettingData );
        *bytes = iCaeEngine->VideoClipMaxSize();
        }
      else
        {
        User::Leave( KErrNotReady );
        }
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingFileName:
      {
      User::Leave( KErrNotSupported );
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingAudioMute:
      {
      if( iCaeEngine )
        {
        TBool* mute = static_cast<TBool*>( aSettingData );
        *mute = iCaeEngine->VideoAudio();
        }
      else
        {
        User::Leave( KErrNotReady );
        }
      break;
      }
#endif
    // -----------------------------------------------------
    case ECameraSettingFacetracking:
      {
      if( iCustomInterfaceFaceTracking )
        {
        TBool* ftOn = static_cast<TBool*>( aSettingData );
        *ftOn = iCustomInterfaceFaceTracking->FaceTrackingOn();
        PRINT1( _L("Camera <> facetracking setting: %d"), *ftOn )
        }
      else
        {
        User::Leave( KErrNotReady );
        }
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingBrightness:
    case ECameraUserSceneSettingBrightness:
      {
      CCamera::TBrightness* brightness =
          static_cast<CCamera::TBrightness*>( aSettingData );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        *brightness = (CCamera::TBrightness)iCaeEngine->Brightness();
      else
#endif
        *brightness = (CCamera::TBrightness)iCamera->Brightness();
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingContrast:
    case ECameraUserSceneSettingContrast:
      {
      CCamera::TContrast* contrast =
          static_cast<CCamera::TContrast*>( aSettingData );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        *contrast = (CCamera::TContrast)iCaeEngine->Contrast();
      else
#endif
        *contrast = (CCamera::TContrast)iCamera->Contrast();
      break;
      }
#ifdef CAMERAAPP_CAPI_V2_ADV
    // -----------------------------------------------------
    case ECameraSettingStabilization:
      {
      TPckgBuf<TCamSettingDataStabilization>* pckg =
          static_cast<TPckgBuf<TCamSettingDataStabilization>*>( aSettingData );
      TCamSettingDataStabilization& stabilization = (*pckg)();

      stabilization.iMode       = iAdvancedSettings->StabilizationMode();
      stabilization.iEffect     = iAdvancedSettings->StabilizationEffect();
      stabilization.iComplexity = iAdvancedSettings->StabilizationComplexity();
      break;
      }
#endif // CAMERAAPP_CAPI_V2_ADV
    // -----------------------------------------------------
    // Image orientation
    case ECameraSettingOrientation:
      {
      CheckNonNullL( iCustomInterfaceOrientation, KErrNotSupported );

      MCameraOrientation::TOrientation* orientation =
          static_cast<MCameraOrientation::TOrientation*>( aSettingData );

      *orientation = iCustomInterfaceOrientation->Orientation();
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingFocusRange:
    default:
      {
#ifndef CAMERAAPP_EMULATOR_BUILD
      PRINT ( _L("Camera <> Not supported, LEAVE!") );
      User::Leave( KErrNotSupported );
#endif // not CAMERAAPP_EMULATOR_BUILD
      break;
      }
    // -------------------------------
    }
  }



// ---------------------------------------------------------------------------
// ProcessNextRequestL
//
// Called from iActive::RunL.
// If the request made to CCamera has a callback, more calls from iActive
// will not be requested here, but in those callbacks.
// Leave here will cause iActive to call EndSequence with the error code.
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::ProcessNextRequestL()
  {
  PRINT( _L("Camera => CCamCameraController::ProcessNextRequestL") );
  TInt readyForNextStep( EFalse );


#ifdef CAMERAAPP_CAE_FIX
  // -------------------------------------------------------
  if( ECamModeChangeInactive != iModeChange )
    {
    readyForNextStep = ProceedModeSwitch();
    }
  else
#endif // CAMERAAPP_CAE_FIX

  // -------------------------------------------------------
  if( IsFlagOn( iInfo.iBusy, ECamBusySetting ) )
    {
    PRINT( _L("Camera <> Settings ongoing") );

    iSettingIndex++;
    if( iSettingIndex < iSettingArray.Count() )
      {
      const TCamCameraSettingId& setting( iSettingArray[iSettingIndex] );

      readyForNextStep = ProcessSettingL( setting );
      // No notification sent at the moment per setting.
      }
    else
      {
      EndSequence( KErrNone );
      }
    }
  // -------------------------------------------------------
  else if( IsFlagOn( iInfo.iBusy, ECamBusySequence ) )
    {
    PRINT( _L("Camera <> Sequence ongoing") );

    iSequenceIndex++;
    if( iSequenceIndex < iSequenceArray.Count() )
      {
      const TCamCameraRequestId& requestId( iSequenceArray[iSequenceIndex] );

      PRINT( _L("Camera <> process request..") );
      // If this request will be responded through MCameraObserver(2) callback,
      // iActive will be requested a new callback there and also
      // the notification to our observers will be sent there.
      // Otherwise we do both of those right away.
      readyForNextStep = ProcessOneRequestL( requestId );

      if( readyForNextStep )
        {
        if( ECamRequestVideoStop == requestId
         || ECamRequestImageCancel == requestId )
          {
          // For video stop request, callback has already happened and notification
          // has been given. No need to do anything here.
          PRINT( _L("Camera <> CCamCameraController::RequestL .. Skipping commmon notification, is done already.") );
          }
        else
          {
          TCamCameraEventId event( Request2Event( requestId ) );
          NotifyObservers( KErrNone, event, EventClass( event ) );
          }
        }
      }
    else
      {
      PRINT( _L("Camera <> Sequence end detected") );
      EndSequence( KErrNone );
      }
    }
  // -------------------------------------------------------
  else
    {
    PRINT( _L("Camera <> Nothing active!! PANIC") );
    __ASSERT_DEBUG( EFalse, Panic( ECamCameraControllerCorrupt ) );
    }
  // -------------------------------------------------------

  PRINT1( _L("Camera <= CCamCameraController::ProcessNextRequestL, more calls now:%d"), readyForNextStep );
  return readyForNextStep;
  }



// ---------------------------------------------------------------------------
// ProcessSettingL
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::ProcessSettingL( const TCamCameraSettingId& aSettingId )
  {
  PRINT1( _L("Camera => CCamCameraController::ProcessSettingL [%s]"),
          KCameraSettingNames[aSettingId] );

  TInt callback( EFalse );
  switch( aSettingId )
    {
    // -------------------------------
    case ECameraSettingCaptureLimit:
      {
      ProcessCaptureLimitSettingL();
      break;
      }
    // -------------------------------
    case ECameraSettingFlash:
    case ECameraUserSceneSettingFlash:
      {
      if ( IsFlagOn( iInfo.iState, ECamVideoOn ) )
        {
        CCamera::TFlash flash( CCamera::EFlashNone );
        TCamFlashId currentFlashSetting( ECamFlashOff );
        // Video light setting has values ECamFlashOff/ECamFlashForced
        iSettingProvider.ProvideCameraSettingL( aSettingId,
                                                &currentFlashSetting );
        PRINT2( _L("Camera => Video Flash now = %d, new = %d"),
                                       iCamera->Flash(), currentFlashSetting );
        // Camera uses values EFlashNone/EFlashVideoLight
        flash = (currentFlashSetting == ECamFlashOff)?
                CCamera::EFlashNone:CCamera::EFlashVideoLight;
        PRINT1( _L("Camera => iCamera->SetFlashL( %d )"), flash );
        iCamera->SetFlashL( flash );
        }
      else
        {
        // Still image flash
        CCamera::TFlash flash( CCamera::EFlashAuto );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &flash );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
            {
            iCaeEngine->SetFlashModeL( flash );
            }
      else
#endif
        {
        if( iCamera->Flash() != flash )
          {
          iCamera->SetFlashL( flash );
#ifdef CAMERAAPP_CAPI_V2
          callback = ETrue;
#endif
          }
        }
         }
      break;
      }
    // -------------------------------
    case ECameraSettingExposure:
    case ECameraUserSceneSettingExposure:
      {
      TPckgBuf<TCamSettingDataExposure> params;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &params );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        iCaeEngine->SetExposureModeL( params().iExposureMode );
      else
#endif // CAMERAAPP_CAE_FIX
        {
#ifdef CAMERAAPP_CAPI_V2_ADV
      PRINT( _L("Calling iAdvancedSettings->SetExposureMode..") );
      const CCamera::TExposure& mode = params().iExposureMode;
      // Zero value is default and always supported
      // (CCamera::EExposureAuto)
      if( mode == CCamera::EExposureAuto || mode & iAdvancedSettingInfo.iEvModesSupport )
        {
        iAdvancedSettings->SetExposureMode( mode );
        // iCamera->SetExposureL( mode );

        TInt step = ResolveEvStep( params().iExposureStep );
        // Value needs to be multiplied by KECamFineResolutionFactor.
        // Setting provider does this for us.
        PRINT( _L("Calling iAdvancedSettings->SetExposureCompensationStep..") );
        iAdvancedSettings->SetExposureCompensationStep( step );

        callback = ETrue;
        }
      else
        {
        User::Leave( KErrNotSupported );
        }
#else
      iCamera->SetExposureL( params().iExposureMode );
#endif // CAMERAAPP_CAPI_V2_ADV
        }
      break;
      }
    // -------------------------------
#ifdef CAMERAAPP_CAPI_V2_ADV
    case ECameraSettingLightSensitivity:
    case ECameraUserSceneSettingLightSensitivity:
      {
      TInt iso( 0 );
      RArray<TInt> ISOarray;
      CleanupClosePushL( ISOarray );
      iAdvancedSettings->GetSupportedIsoRatesL( ISOarray );
      iSettingProvider.SetSupportedISORatesL( ISOarray );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &iso );
      callback = ETrue;
      CleanupStack::PopAndDestroy( &ISOarray );

      PRINT1( _L("Camera <> Setting ISO rate to: %d"), iso );
      if( !iso )
        {
        // ISO Auto
        iAdvancedSettings->SetISORateL(
                CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised, iso );
        }
      else if( IsSupportedValue( iso,
                                 iAdvancedSettingInfo.iIsoRatesSupport,
                                 EDiscreteSteps ) )
        {
        // Selected ISO rate
        iAdvancedSettings->SetISORateL(
                CCamera::CCameraAdvancedSettings::EISOManual, iso );
        }
      else
        {
        User::Leave( KErrNotSupported );
        }
      break;
      }
#endif
    // -------------------------------
    case ECameraSettingWhiteBalance:
    case ECameraUserSceneSettingWhiteBalance:
      {
      TPckgBuf<TCamSettingDataWhiteBalance> params;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &params );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        iCaeEngine->SetWhiteBalanceModeL( params().iWhiteBalanceMode );
      else
#endif // CAMERAAPP_CAE_FIX
        {
        iCamera->SetWhiteBalanceL( params().iWhiteBalanceMode );
#ifdef CAMERAAPP_CAPI_V2
        callback = ETrue;
#endif
        }
      break;
      }
    // -------------------------------
#ifdef CAMERAAPP_CAPI_V2_IP
    case ECameraSettingColourEffect:
    case ECameraUserSceneSettingColourEffect:
      {
      CheckNonNullL( iImageProcessor, KErrNotSupported );
      CIP::TEffect effect( CIP::EEffectNone );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &effect );

      if( IsSupportedValue( effect,
                            iAdvancedSettingInfo.iColourEffectSupport,
                            iAdvancedSettingInfo.iColourEffectValueInfo ) )
        {
        iImageProcessor->SetTransformationValue( KUidECamEventImageProcessingEffect, effect );
        callback = ETrue;
        }
      else
        User::Leave( KErrNotSupported );
      break;
      }
    // -------------------------------
    case ECameraSettingSharpness:
    case ECameraUserSceneSettingSharpness:
      {
      CheckNonNullL( iImageProcessor, KErrNotSupported );
      TInt sharpness( 0 );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &sharpness );

      if( IsSupportedValue( sharpness,
                            iAdvancedSettingInfo.iSharpnessSupport,
                            iAdvancedSettingInfo.iSharpnessValueInfo ) )
        {
        iImageProcessor->SetTransformationValue( KUidECamEventImageProcessingAdjustSharpness, sharpness );
        callback = ETrue;
        }
      else
        User::Leave( KErrNotSupported );
      break;
      }
#endif // CAMERAAPP_CAPI_V2_IP
    // -------------------------------
    case ECameraSettingBrightness:
    case ECameraUserSceneSettingBrightness:
      {
      CCamera::TBrightness brightness( CCamera::EBrightnessAuto );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &brightness );
      iCamera->SetBrightnessL( brightness );
      break;
      }
    // -------------------------------
    case ECameraSettingContrast:
    case ECameraUserSceneSettingContrast:
      {
      CCamera::TContrast contrast( CCamera::EContrastAuto );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &contrast );
      iCamera->SetContrastL( contrast );
      break;
      }
    // -------------------------------
    case ECameraSettingDigitalZoom:
      {
      TInt zoom( 0 );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &zoom );
      PRINT1( _L("Camera <> Setting digital zoom step to: %d"), zoom );
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        iCaeEngine->SetZoomValueL( zoom );
      else
#endif
        {
#ifdef CAMERAAPP_CAPI_V2_ADV
        if( iAdvancedSettingInfo.iDigitalZoomSupport.Count() > zoom )
            iAdvancedSettings->SetDigitalZoom( iAdvancedSettingInfo.iDigitalZoomSupport[zoom] );
          else
            User::Leave( KErrNotSupported );
        callback = ETrue;

#else
        // Note: Even if the method is misleadingly named
        //       CCamera::SetDigitalZoomFactorL, the values are
        //       zoom steps, not zoom factors.
        iCamera->SetDigitalZoomFactorL( zoom );
#endif

        }
      break;
      }
    // -------------------------------
#ifdef CAMERAAPP_CAPI_V2_ADV
    case ECameraSettingOpticalZoom:
      {
      TInt zoom( 0 );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &zoom );
      PRINT1( _L("Camera <> Setting optical zoom to: %d"), zoom );
      iAdvancedSettings->SetOpticalZoom( zoom );
      callback = ETrue;
      break;
      }
    // -----------------------------------------------------
    case ECameraSettingStabilization:
      {
      PRINT( _L("Camera <> CCamCameraController::ProcessSettingL ECameraSettingStabilization") );
      TPckgBuf<TCamSettingDataStabilization> stabilization;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &stabilization );

      // Check that the values are supported..
      TBool modeOk    = ( CAS::EStabilizationModeOff == stabilization().iMode
                       || iAdvancedSettingInfo.iStabilizationModeSupport & stabilization().iMode );
      TBool effectOk  = ( CAS::EStabilizationOff     == stabilization().iEffect
                       || iAdvancedSettingInfo.iStabilizationEffectSupport & stabilization().iEffect );
      TBool complexOk = ( CAS::EStabilizationComplexityAuto == stabilization().iComplexity
                       || iAdvancedSettingInfo.iStabilizationComplexitySupport & stabilization().iComplexity );

      if( modeOk && effectOk && complexOk )
        {
        PRINT1( _L("Camera <> CCamCameraController::ProcessSettingL set stabilization mode=%d"), stabilization().iMode );
        iAdvancedSettings->SetStabilizationMode      ( stabilization().iMode       );
        iAdvancedSettings->SetStabilizationEffect    ( stabilization().iEffect     );
        iAdvancedSettings->SetStabilizationComplexity( stabilization().iComplexity );
        // Events:
        //   KUidECamEventCameraSettingStabilizationMode
        //   KUidECamEventCameraSettingsStabilizationEffect
        //   KUidECamEventSettingsStabilizationAlgorithmComplexity
        // We use the latest one to determine when we can continue.
        // Do not change above order unless CCamSettingConversion::Map2EventUidValue
        // is edited..
        }
      else
        {
        User::Leave( KErrNotSupported );
        }

      callback = ETrue;
      break;
      }
    // -------------------------------
    case ECameraSettingContAF:
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if ( appUi->AppController().UiConfigManagerPtr()->IsContinuosAutofocusSupported() )
         {
         TBool isContAFon( iAdvancedSettings->AutoFocusType() &
                           CAS::EAutoFocusTypeContinuous );
         if( IsFlagOn( iInfo.iState, ECamVideoOn ) )
            {
            TBool contAF( ETrue );
            iSettingProvider.ProvideCameraSettingL( aSettingId, &contAF );
            if( contAF != isContAFon )
              {
              CAS::TAutoFocusType type = contAF? CAS::EAutoFocusTypeContinuous:
                                                 CAS::EAutoFocusTypeOff;
              iAdvancedSettings->SetAutoFocusType( type );
			  }
            }
         else
			{
			// Do nothing
			}
		 }
      break;
      }
#endif // CAMERAAPP_CAPI_V2_ADV
    // -------------------------------
#ifdef CAMERAAPP_CAE_FOR_VIDEO
    case ECameraSettingFileMaxSize:
      {
      if( iCaeEngine )
        {
        TInt bytes( 0 );
        iSettingProvider.ProvideCameraSettingL( aSettingId, &bytes );
        if( iCaeEngine->VideoClipMaxSize() != bytes )
          {
          iCaeEngine->SetVideoClipMaxSizeL( bytes );
          callback = IsFlagOn( iInfo.iState, ECamVideoOn );
          }
        }
      else
        {
        //
        PRINT( _L("Camera <> Video file size too early, NOT SET!!") );
        }
      break;
      }
    // -------------------------------
    case ECameraSettingFileName:
      {
      if( iCaeEngine )
        {
        PRINT ( _L("Camera <> processing filename setting") );

        HBufC* filename = HBufC::NewLC( KMaxFileName );
        TPtr   ptr      = filename->Des();
        PRINT ( _L("Camera <> get filename from setting provider..") );
        iSettingProvider.ProvideCameraSettingL( aSettingId, &ptr );

        PRINT1( _L("Camera <> set filename [%S]"), filename );
        iCaeEngine->SetVideoRecordingFileNameL( ptr );

        CleanupStack::PopAndDestroy( filename );
        // If video has been prepared, MCaeoVideoPrepareComplete will be repeated,
        // and needs to be waited.
        callback = IsFlagOn( iInfo.iState, ECamVideoOn );
        }
      else
        {
        PRINT( _L("Camera <> Video name too early, NOT SET!!") );
        }
      break;
      }
    // -------------------------------
    case ECameraSettingAudioMute:
      {
      if( iCaeEngine )
        {
         TBool isAudioOn( EFalse );
        iSettingProvider.ProvideCameraSettingL( aSettingId, &isAudioOn );
        TBool audioOn( iCaeEngine->VideoAudio() );
        // If we need to mute audio and audio is on
        // or we need to enable audio and audio is off
        if( isAudioOn != audioOn )
          {
          iCaeEngine->SetVideoAudioL( isAudioOn );
          callback = IsFlagOn( iInfo.iState, ECamVideoOn );
          }
        }
      else
        {
        PRINT( _L("Camera <> Video mute too early, NOT SET!!") );
        }
      break;
      }
    // -------------------------------
    case ECameraSettingFacetracking:
      {
      if( iCustomInterfaceFaceTracking )
        {
        SetFaceTrackingL();
        }
      else
        {
        PRINT( _L("Camera <> Facetracking too early, NOT SET!!") );
        }
      break;
      }
#endif
    // -------------------------------
    // Image orientation
    case ECameraSettingOrientation:
      {
      CheckNonNullL( iCustomInterfaceOrientation, KErrNotSupported );
      MCameraOrientation::TOrientation
        orientation( MCameraOrientation::EOrientation0 );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &orientation );

      if ( iInfo.iCurrentCamera != KPrimaryCameraIndex &&
           orientation == MCameraOrientation::EOrientation90 )
        {
        PRINT( _L("Camera <> Rotate portrait secondary camera image 270 degrees") );
        orientation = MCameraOrientation::EOrientation270;
        }

      PRINT1( _L("Camera <> setting camera orientation to [0x%02x]"), orientation );
      iCustomInterfaceOrientation->SetOrientationL( orientation );
      break;
      }
    // -------------------------------
    case ECameraSettingFocusRange:
    default:
      {
#ifndef CAMERAAPP_EMULATOR_BUILD
      PRINT ( _L("Camera <> Not supported, LEAVE!") );
      User::Leave( KErrNotSupported );
#endif // not CAMERAAPP_EMULATOR_BUILD
      break;
      }
    // -------------------------------
    }

  PRINT1( _L("Camera <= CCamCameraController::ProcessSettingL, continue now:%d"), !callback );
  return !callback;
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessOneRequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::ProcessOneRequestL") );
  PRINT2( _L("Camera <> processing request [%s] id:%d "),
          KCamRequestNames[aRequestId],
          aRequestId );

  TInt readyForNext( ETrue );

  // Order from most time critical / frequent
  // to less time critical / seldom.
  // Handled in submethods to preserve readability.
  // -----------------------------------------------------
  switch( aRequestId )
    {
    // -------------------------------
    case ECamRequestReserve:
    case ECamRequestPowerOn:
      readyForNext = ProcessControlStartupRequestL( aRequestId );
      break;
    case ECamRequestPowerOff:
    case ECamRequestRelease:
      readyForNext = ProcessControlShutdownRequest( aRequestId );
      break;
    // -------------------------------
    case ECamRequestVfStart:
    case ECamRequestVfStop:
    case ECamRequestVfRelease:
      readyForNext = ProcessVfRequestL( aRequestId );
      break;
    // -------------------------------
    case ECamRequestSsStart:
    case ECamRequestSsStop:
    case ECamRequestSsRelease:
      readyForNext = ProcessSnapshotRequestL( aRequestId );
      break;
    // -------------------------------
    case ECamRequestImageInit:
    case ECamRequestImageCapture:
      readyForNext = ProcessImageRequestL( aRequestId );
      break;
    case ECamRequestImageCancel:
    case ECamRequestImageRelease:
      readyForNext = ProcessImageShutdownRequest( aRequestId );
      break;
    // -------------------------------
    case ECamRequestVideoInit:
    case ECamRequestVideoStart:
    case ECamRequestVideoPause:
    case ECamRequestVideoStop:
    case ECamRequestVideoRelease:
    case ECamRequestSetAsyncVideoStopMode:
      readyForNext = ProcessVideoRequestL( aRequestId );
      break;
    // -------------------------------
    default:
      Panic( ECamCameraControllerUnknownRequest );
      break;
    // -------------------------------
    }
  // -----------------------------------------------------
  PRINT( _L("Camera <= CCamCameraController::ProcessOneRequestL") );
  return readyForNext;
  }


// ---------------------------------------------------------------------------
// ProcessControlStartupRequestL
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessControlStartupRequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT1( _L("Camera => CCamCameraController::ProcessControlStartupRequestL [%s]"),
          KCamRequestNames[aRequestId] );
  // -------------------------------------------------------
  // Reserve
  if( ECamRequestReserve == aRequestId )
    {
    CheckFlagOffL( iInfo.iState, ECamReserved, KErrInUse );

    // tell CAPI the usecase (still or video), not fatal if fails
    TRAPD( err, HintUseCaseL() );
    
    if( err ) 
      {
      PRINT1( _L("Camera <> CCamCameraController::ProcessControlStartupRequestL HintUseCaseL failed:%d"), err)
      }

    CAMERAAPP_PERF_CONTROLLER_START( ECamRequestReserve );
    iCamera->Reserve();
    // If UIOrientationOverrideAPI is used, ui construction is completed while
    // waiting for Reserve to complete, event sent here to continue ui construction
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi->AppController().UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
      {
      NotifyObservers( KErrNone,
                       ECamCameraEventReserveRequested,
                       ECamCameraEventClassBasicControl );
      }
    }
  // -------------------------------------------------------
  // Power on
  else if( ECamRequestPowerOn == aRequestId )
    {
    CheckFlagOnL ( iInfo.iState, ECamReserved, KErrNotReady );
    CheckFlagOffL( iInfo.iState, ECamPowerOn,  KErrInUse    );

    CAMERAAPP_PERF_CONTROLLER_START( ECamRequestPowerOn );

    iCamera->PowerOn();
    }
  // -------------------------------------------------------
  // Unknown
  else
    {
    Panic( ECamCameraControllerCorrupt );
    }
  // -------------------------------------------------------
  iReleasedByUi = EFalse;
  PRINT( _L("Camera <= CCamCameraController::ProcessControlStartupRequestL") );
  // Callback needs to be received before we can continue.
  return EFalse;
  }

// ---------------------------------------------------------------------------
// ProcessControlShutdownRequest
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessControlShutdownRequest( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::ProcessControlShutdownRequest") );
  // -------------------------------------------------------
  // Power off
  if( ECamRequestPowerOff == aRequestId )
    {
    if( IsFlagOn( iInfo.iState, ECamPowerOn ) )
      {
      // -------------------------------
      // Release image or video capture
      if( IsFlagOn( iInfo.iState, ECamImageOn ) )
        {
        // Leaves only if state is wrong (already checked here).
        ProcessImageShutdownRequest( ECamRequestImageRelease );
        NotifyObservers( KErrNone,
                         ECamCameraEventImageRelease,
                         ECamCameraEventClassImage );
        }
      else if( IsFlagOn( iInfo.iState, ECamVideoOn ) )
        {
        TRAP_IGNORE( ProcessVideoRequestL( ECamRequestVideoRelease ) );
        NotifyObservers( KErrNone,
                         ECamCameraEventVideoRelease,
                         ECamCameraEventClassVideo );
        }
      else
        {
        }
      // -------------------------------
      // If viewfinder is not released yet, release it now.
      if( ECamTriIdle != iInfo.iVfState )
        {
        ProcessVfRelaseRequest();
        NotifyObservers( KErrNone,
                         ECamCameraEventVfRelease,
                         ECamCameraEventClassVfControl );
        }

      // -------------------------------
      // If snapshot is not released yet, release it now.
      if( ECamTriIdle != iInfo.iSsState )
        {
        ProcessSsReleaseRequest();
        NotifyObservers( KErrNone,
                         ECamCameraEventSsRelease,
                         ECamCameraEventClassSsControl );
        }
      // -------------------------------
      // Do the actual powering off.
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        {
        iCaeEngine->PowerOff();
        }
      else
#endif // CAMERAAPP_CAE_FIX
        {
        iCamera->PowerOff();
        }
      // -------------------------------
      }
    ClearFlags( iInfo.iState, ECamPowerOn );
    }
  // -------------------------------------------------------
  // Release
  else if( ECamRequestRelease == aRequestId )
    {
    if( IsFlagOn( iInfo.iState, ECamReserved ) )
      {
      // If power off not called yet, do it now.
      if( IsFlagOn( iInfo.iState, ECamPowerOn ) )
        {
        ProcessControlShutdownRequest( ECamRequestPowerOff );
        // Separate notification needed here, because operation done
        // as part of handling other request.
        NotifyObservers( KErrNone,
                         ECamCameraEventPowerOff,
                         ECamCameraEventClassBasicControl );
        }
#ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        {
        iCaeEngine->Release();
        iCaeInUse = EFalse;
        }
      else
#endif // CAMERAAPP_CAE_FIX
        {
        iCamera->Release();
        iReleasedByUi = ETrue;
        if( iIveRecoveryCount > 0 )
            {
            HandleReserveLostEvent( KErrNone );
            }
        }
      }
    PRINT( _L("Camera <> released, reset info") );

    // Reset our info, but preserve busy flag and camera index.
    // Sets iInfo.iState to ECamIdle.
    iInfo.Reset( ETrue, ETrue );
    }
  // -------------------------------------------------------
  // Unknown
  else
    {
    Panic( ECamCameraControllerCorrupt );
    }
  // -------------------------------------------------------
  PRINT( _L("Camera <= CCamCameraController::ProcessControlShutdownRequest") );

  // No callback needs to be waited.
  return ETrue;
  }

// ---------------------------------------------------------------------------
// ProcessViewfinderRequestL
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessVfRequestL( const TCamCameraRequestId& aRequestId )
  {
  CheckFlagOnL( iInfo.iState, ECamPowerOn, KErrNotReady );

  switch( aRequestId )
    {
    // -----------------------------------------------------
    case ECamRequestVfStart:
      {
      CAMERAAPP_PERF_CONTROLLER_STARTNEW( ECamRequestVfStart, iFirstVfFrameReceived );
      ProcessVfStartRequestL();
      break;
      }
    // -----------------------------------------------------
    case ECamRequestVfStop:
      {
      ProcessVfStopRequestL();
      break;
      }
    // -----------------------------------------------------
    case ECamRequestVfRelease:
      {
      ProcessVfRelaseRequest();
      break;
      }
    // -----------------------------------------------------
    default:
      {
      Panic( ECamCameraControllerCorrupt );
      break;
      }
    // -----------------------------------------------------
    }

  // No callback to be waited
  return ETrue;
  }


// ---------------------------------------------------------------------------
// InitViewfinderL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::InitViewfinderL( const TCamViewfinderMode& aMode )
  {
  PRINT( _L("Camera => CCamCameraController::InitViewfinderL") );

  // Check the old viewfinder is released now.
  // No further checks made here.
  __ASSERT_DEBUG( ECamTriIdle == iInfo.iVfState, Panic( ECamCameraControllerCorrupt ) );

  // Then init the new mode
  switch( aMode )
    {
    // ---------------------------------
    case ECamViewfinderDirect:
      {
      // Don't proceed if not supported by camera.
      CheckFlagOnL( iCameraInfo.iOptionsSupported,
                    TCameraInfo::EViewFinderDirectSupported,
                    KErrNotSupported );

  #ifdef CAMERAAPP_CAPI_V2_DVF
      PRINT( _L("Camera <> Creating CCameraDirectViewFinder instance") );
      if( iDirectViewfinder )
        {
        	delete iDirectViewfinder;
        	iDirectViewfinder=NULL;
        }
      iDirectViewfinder = CCamera::CCameraDirectViewFinder::NewL( *iCamera );
  #else
      // not implemented
      Panic( ECamCameraControllerUnsupported );
  #endif
      iInfo.iVfState  = ECamTriInactive;
      iInfo.iVfMode   = ECamViewfinderDirect;
      break;
      }
    // ---------------------------------
    case ECamViewfinderBitmap:
      {
      // Don't proceed if not supported by camera.
      CheckFlagOnL( iCameraInfo.iOptionsSupported,
                    TCameraInfo::EViewFinderBitmapsSupported,
                    KErrNotSupported );

      iInfo.iVfState  = ECamTriInactive;
      iInfo.iVfMode   = ECamViewfinderBitmap;
      break;
      }
    // ---------------------------------
    case ECamViewfinderNone:
      // Mode is "none", so no init needed.
      break;
    // ---------------------------------
    default:
      // Unknown mode
      Panic( ECamCameraControllerUnsupported );
      break;
    // ---------------------------------
    }
  PRINT( _L("Camera <= CCamCameraController::InitViewfinderL") );
  }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void
CCamCameraController
::ProcessVfStartRequestL()
  {
  PRINT( _L("Camera => CCamCameraController::ProcessVfStartRequestL") );

#ifdef __WINSCW__
  const TCamViewfinderMode KTargetMode( ECamViewfinderBitmap );
#else

  TPckgBuf<TCamViewfinderMode> mode;
  iSettingProvider.ProvideCameraParamL( ECameraParamVfMode, &mode );

  // If the viewfinder mode needs to be changed,
  // first stop and release resources related to the old viewfinder type.
  const TCamViewfinderMode KTargetMode( mode() );
#endif

  if( ECamViewfinderNone != iInfo.iVfMode
   && KTargetMode        != iInfo.iVfMode )
    {
    ProcessVfRelaseRequest();
    }

  // Check that application is still in foreground, if not, then vf not started.
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  if ( appUi && appUi->AppInBackground( ETrue ) )
    {
    // Notify appUi that we did not start viewfinder although asked to do so.
    appUi->SetViewFinderStoppedStatus( iInfo.iVfState != ECamTriActive );
    }
  else
    {
    switch( iInfo.iVfState )
      {
      // -----------------------------------------------------
      case ECamTriIdle:
       PRINT( _L("Camera <> case ECamTriIdle") );

        InitViewfinderL( KTargetMode );
        // << fall through >>

      case ECamTriInactive:
        {
        // -------------------------------
        PRINT( _L("Camera <> case ECamTriInactive") );

        if( ECamViewfinderDirect == iInfo.iVfMode )
#ifdef CAMERAAPP_CAPI_V2_DVF
          {
          switch( iDirectViewfinder->ViewFinderState() )
            {
            case CCamera::CCameraDirectViewFinder::EViewFinderInActive:
              {
              if ( !iViewfinderWindow )
                  {
                  PRINT( _L("Camera <> CCamCameraController::iViewfinderWindow is NULL - cannot start VF!") );
                  User::Leave( KErrNotReady );
                  }

              // Use the same viewfinder position and size as for bitmap viewfinder
              TPckgBuf<TCamParamsVfBitmap> params;
              iSettingProvider.ProvideCameraParamL( ECameraParamVfBitmap, &params );

              CEikonEnv* env = CEikonEnv::Static();

              OstTrace0( CAMERAAPP_PERFORMANCE, CCAMCAMERACONTROLLER_PROCESSVFSTARTREQUESTL, "e_CAM_APP_VF_INIT 0" ); //CCORAPP_APP_VF_INIT_END
              OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMCAMERACONTROLLER_PROCESSVFSTARTREQUESTL, "e_CAM_APP_OVERLAY_INIT 0" ); //CCORAPP_APP_OVERLAY_INIT_END

              TInt orgPos = SetVfWindowOrdinal(); // Set visible
              iCamera->StartViewFinderDirectL(
                  env->WsSession(),
                  *env->ScreenDevice(),
                  *iViewfinderWindow,
                  params().iRect );
              (void) SetVfWindowOrdinal( orgPos ); // back to original

              if ( appUi && ECamActiveCameraSecondary == appUi->ActiveCamera() )
                  {
                  iCamera->SetViewFinderMirrorL(ETrue);
                  }
              // VF started succesfully, reset recovery counter
              delete iIdle;
              iIdle = NULL;
              iIveRecoveryCount = KIveRecoveryCountMax;
              break;
              }
            case CCamera::CCameraDirectViewFinder::EViewFinderPause:
              {
              iDirectViewfinder->ResumeViewFinderDirectL();
              break;
              }
            case CCamera::CCameraDirectViewFinder::EViewFinderActive:
              {
              // Already running. Not considered as error.
              break;
              }
            default:
              {
              Panic( ECamCameraControllerUnsupported );
              break;
              }
            }
          }
#else // CAMERAAPP_CAPI_V2_DVF
          {
          // No controller support for direct vf.
          Panic( ECamCameraControllerUnsupported );
          }
#endif // CAMERAAPP_CAPI_V2_DVF
        // -------------------------------
        else
          {
          PRINT( _L("Camera <> Get bitmap vf details..") );

          TPckgBuf<TCamParamsVfBitmap> params;
          iSettingProvider.ProvideCameraParamL( ECameraParamVfBitmap, &params );
          iInfo.iViewfinderFormat = params().iFormat;
          iInfo.iViewfinderSize   = params().iRect.Size();
#ifdef CAMERAAPP_CAE_FIX
          if( iCaeInUse )
            {
            PRINT( _L("Camera <> Call CCaeEngine::StartViewFinderBitmapsL..") );

            OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMCAMERACONTROLLER_PROCESSVFSTARTREQUESTL, "e_CAM_APP_VF_INIT 0" ); //CCORAPP_APP_VF_INIT_END
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMCAMERACONTROLLER_PROCESSVFSTARTREQUESTL, "e_CAM_APP_OVERLAY_INIT 0" ); //CCORAPP_APP_OVERLAY_INIT_END
            iCaeEngine->StartViewFinderBitmapsL( iInfo.iViewfinderSize );
            }
          else
#endif // CAMERAAPP_CAE_FIX
            {
            PRINT( _L("Camera <> Call CCamera::StartViewFinderBitmapsL..") );
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMCAMERACONTROLLER_PROCESSVFSTARTREQUESTL, "e_CAM_APP_VF_INIT 0" );  //CCORAPP_APP_VF_INIT_END
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMCAMERACONTROLLER_PROCESSVFSTARTREQUESTL, "e_CAM_APP_OVERLAY_INIT 0" ); //CCORAPP_APP_OVERLAY_INIT_END
            iCamera->StartViewFinderBitmapsL( iInfo.iViewfinderSize );

  		  if ( params().iMirrorImage )
  		  	{
  		   	iCamera->SetViewFinderMirrorL( params().iMirrorImage );
  		   	}
            }
          }
        // -------------------------------

        iInfo.iVfState = ECamTriActive;
        //view finder started now(set stop status as false)
        if ( appUi )
            {
            appUi->SetViewFinderStoppedStatus( EFalse );
            }
        break;
        }
      // -----------------------------------------------------
      case ECamTriActive:
        {
        PRINT( _L("Camera <> case ECamTriActive") );
        // Viewfinder started *and* the right mode
        // no action needed.
        break;
        }
      // -----------------------------------------------------
      default:
        Panic( ECamCameraControllerCorrupt );
        break;
      // -----------------------------------------------------
      }
    }
  PRINT( _L("Camera <= CCamCameraController::ProcessVfStartRequestL") );
  }



// ---------------------------------------------------------------------------
// ProcessVfStopRequestL
//
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ProcessVfStopRequestL()
  {
  PRINT( _L("Camera => CCamCameraController::ProcessVfStopRequestL") );

  switch( iInfo.iVfState )
    {
    case ECamTriActive:
      {
      ProcessVfStopEcamRequest();
      iInfo.iVfState = ECamTriInactive;
      break;
      }
    case ECamTriIdle:
      {
      // Viewfinder not initialized.
      // Let this pass as our "start viewfinder" handles all states.
      break;
      }
    case ECamTriInactive:
      {
      // Already stopped. No action needed.
      break;
      }
    default:
      {
      Panic( ECamCameraControllerCorrupt );
      break;
      }
    }
  PRINT( _L("Camera <= CCamCameraController::ProcessVfStopRequestL") );
  }


// ---------------------------------------------------------------------------
// ProcessVfStopEcamRequest
//
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ProcessVfStopEcamRequest()
  {
  PRINT( _L("Camera => CCamCameraController::ProcessVfStopEcamRequest") );

  if( ECamViewfinderDirect == iInfo.iVfMode )
    {
#ifdef CAMERAAPP_CAPI_V2_DVF
    if ( iCamera )
      {
      PRINT( _L("Camera <> VF stop by CCamera") );
      iCamera->StopViewFinder();
      }
#else // CAMERAAPP_CAPI_V2_DVF
    Panic( ECamCameraControllerUnsupported );
#endif // CAMERAAPP_CAPI_V2_DVF
    }
  else
    {
#ifdef CAMERAAPP_CAE_FIX
    if( iCaeInUse && iCaeEngine )
      {
      PRINT( _L("Camera <> stop by CAE") );
      iCaeEngine->StopViewFinder();
      }
    else
#endif // CAMERAAPP_CAE_FIX
      {
      if ( iCamera )
        {
        PRINT( _L("Camera <> stop by CCamera") );
        iCamera->StopViewFinder();
        }
      }
    }
  PRINT( _L("Camera <= CCamCameraController::ProcessVfStopEcamRequest") );
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamCameraController
::ProcessVfRelaseRequest()
  {
  switch( iInfo.iVfState )
    {
    // -----------------------------------------------------
    case ECamTriActive:
      {
      // Do the stopping first and continue then with release.
      // Leaves only if iVfState is ECamVfIdle, which is not the case here.
      TRAP_IGNORE( ProcessVfStopRequestL() );

      // Need to notify here, because done as a part of other request.
      NotifyObservers( KErrNone,
                       ECamCameraEventVfStop,
                       ECamCameraEventClassVfControl );
      // << fall through >>
      }
    // -----------------------------------------------------
    case ECamTriInactive:
      {
      if( ECamViewfinderDirect == iInfo.iVfMode )
        {
#ifdef CAMERAAPP_CAPI_V2_DVF
        delete iDirectViewfinder;
        iDirectViewfinder = NULL;
#endif
        }
      else
        {
        // No further actions needed for bitmap viewfinding.
        }
      // These may very well remain as they are.
      // Atleast for the format there is no "zero" value available.
      //    iInfo.iViewfinderSize
      //    iInfo.iViewfinderFormat
      iInfo.iVfState          = ECamTriIdle;
      iInfo.iVfMode           = ECamViewfinderNone;
      break;
      }
    // -----------------------------------------------------
    case ECamTriIdle:
      // Already released.
      break;
    // -----------------------------------------------------
    default:
      Panic( ECamCameraControllerCorrupt );
      break;
    // -----------------------------------------------------
    }
  }


// ---------------------------------------------------------------------------
// ProcessImageRequestL
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessImageRequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::ProcessImageRequestL") );
  CheckFlagOnL( iInfo.iState, ECamPowerOn, KErrNotReady );

  TInt callback( ETrue );
  switch( aRequestId )
    {
    // -----------------------------------------------------
    case ECamRequestImageInit:
      {
      // When camera engine still capture don't Init a new capture

      if ( ECamCaptureOn == iInfo.iCaptureState )
      	{
      	return EFalse;
      	}
      CAMERAAPP_PERF_CONTROLLER_START( ECamRequestImageInit );

#ifdef CAMERAAPP_CAE_FOR_VIDEO
  #ifdef CAMERAAPP_CAE_FIX
      if( iCaeInUse )
        {
        iModeChange       = ECamModeChangeVideo2Image;
        iModeChangePhase  = ECamModeChangePhaseIdle;   // incremented before first step
        iModeChangeStatus = KErrNone;
        iActive->IssueRequest();
        }
      else
  #else
      if( IsFlagOn( iInfo.iState, ECamVideoOn ) )
        {
        // Release any video recording resources
        iCaeEngine->CloseVideoRecording();
        ClearFlags( iInfo.iState, ECamVideoOn );
        }
  #endif // CAMERAAPP_CAE_FIX
#endif
        {
      PRINT( _L("Camera <> Checking that image capture not ongoing..") );
      CheckEqualsL( iInfo.iCaptureState, ECamCaptureOff, KErrInUse    );

      PRINT( _L("Camera <> Checking that image capture supported..") );
      CheckFlagOnL( iCameraInfo.iOptionsSupported,
                    TCameraInfo::EImageCaptureSupported,
                    KErrNotSupported );

      PRINT( _L("Camera <> Ask image parameters..") );
      TPckgBuf<TCamParamsImage> params;
      iSettingProvider.ProvideCameraParamL( ECameraParamImage, &params );

      PRINT( _L("Camera <> Enumerating capture sizes..") );
      // Query the supported resolutions for the selected format.
      // GetResolutionIndexL may change the format to some alternative,
      // if we are able to convert the incoming image later to the
      // format requested by client.
      CCamera::TFormat format( params().iFormat );
      TInt index = GetResolutionIndexL( format, params().iSize );
      User::LeaveIfError( index );

      PRINT( _L("Camera <> Call CCamera::PrepareImageCaptureL..") );
      PRINT2( _L("Camera <> Image size: (%d, %d)"),
                  params().iSize.iWidth,
                  params().iSize.iHeight );

      OstTrace0( CAMERAAPP_PERFORMANCE, CCAMCAMERACONTROLLER_PROCESSIMAGEREQUESTL, "e_CAM_APP_CONFIGURATIONS 0" );  //CCORAPP_APP_CONFIGS_END
      iCamera->PrepareImageCaptureL( format, index );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMCAMERACONTROLLER_PROCESSIMAGEREQUESTL, "e_CAM_APP_STILL_INIT 0" ); //CCORAPP_APP_STILL_INIT_END
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMCAMERACONTROLLER_PROCESSIMAGEREQUESTL, "e_CAM_APP_OVERLAY_INIT 1" );   //CCORAPP_APP_OVERLAY_INIT_START

      iCamera->SetJpegQuality( params().iQualityFactor );

      SetFlags( iInfo.iState, ECamImageOn );

#ifdef CAMERAAPP_CAPI_V2_ADV
      // Set current autofocus range to invalid value to force focusing
      iInfo.iCurrentFocusRange = static_cast<CAS::TFocusRange>( -1 );
      GetAdvancedSettingsInfoL();
#endif // CAMERAAPP_CAPI_V2_ADV

      iInfo.iCaptureCount  = 0;
      iInfo.iSnapshotCount = 0;
      callback = EFalse; // No callback to wait for.
      CAMERAAPP_PERF_CONTROLLER_STOP( ECamRequestImageInit );
        }
      break;
      }
    // -----------------------------------------------------
    case ECamRequestImageCapture:
      {
      CheckFlagOnL( iInfo.iState,        ECamImageOn,    KErrNotReady );
      CheckEqualsL( iInfo.iCaptureState, ECamCaptureOff, KErrInUse    );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMCAMERACONTROLLER_PROCESSIMAGEREQUESTL, "e_CAM_APP_CAPTURE_START 0" );  //CCORAPP_CAPTURE_START_END

      // New capture starts, reset capture and snapshot counters.
      iInfo.iCaptureCount  = 0;
      iInfo.iSnapshotCount = 0;
      iInfo.iCaptureState  = ECamCaptureOn;

      PRINT1( _L("Camera <> CCamCameraController .. About to start capture, total shared buffers in use: %d"), CCamBufferShare::TotalBufferShareCount() );
      iCamera->CaptureImage();

      // When image data is received from CCamera,
      // an event is generated for that. We need to notify
      // here as other places check if the request has associated
      // callback, and send no notification yet if callback exist.
      NotifyObservers( KErrNone,
                       ECamCameraEventImageStart,
                       ECamCameraEventClassImage );
      break;
      }
    // -----------------------------------------------------
    default:
      {
      Panic( ECamCameraControllerCorrupt );
      break;
      }
    // -----------------------------------------------------
    }
  PRINT1( _L("Camera <= CCamCameraController::ProcessImageRequestL, continue now:%d"), !callback );
  return !callback;
  }


// ---------------------------------------------------------------------------
// ProcessImageShutdownRequest
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessImageShutdownRequest( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::ProcessImageShutdownRequest") );

  switch( aRequestId )
    {
    // -----------------------------------------------------
    case ECamRequestImageCancel:
      {
      if( IsFlagOn( iInfo.iState, ECamImageOn )
//      && ECamCaptureOn == iInfo.iCaptureState
        )
        {
        const TCamCameraCaptureState previousState( iInfo.iCaptureState );

        if( iEncoder )
          {
          iEncoder->Cancel();

          delete iEncoder;
          iEncoder = NULL;
          }

        PRINT( _L("Camera <> Calling iCamera->CancelCaptureImage()") );
        iCamera->CancelCaptureImage();

        iInfo.iCaptureState = ECamCaptureOff;

        // Notify if we actually stopped the capturing here.
        if( ECamCaptureOn == previousState )
          {

          // Capturing image was stopped here.
          // If single request was ongoing, it must be "capture image".
          ClearFlags( iInfo.iBusy, ECamBusySingle );

          TInt fullCaptures( Min( iInfo.iCaptureCount, iInfo.iSnapshotCount ) );
          NotifyObservers( KErrNone,
                           ECamCameraEventImageStop,
                           ECamCameraEventClassImage,
                           &fullCaptures );
          }
        }
      break;
      }
    // -----------------------------------------------------
    case ECamRequestImageRelease:
      {
      // Cancel any pending capture.
      ProcessImageShutdownRequest( ECamRequestImageCancel );

      // Nothing else really needed for image, just set flags.
      ClearFlags( iInfo.iState, ECamImageOn );
      iInfo.iCaptureCount  = 0;
      iInfo.iSnapshotCount = 0;
      iInfo.iCaptureState  = ECamCaptureOff;

      break;
      }
    // -----------------------------------------------------
    default:
      {
      Panic( ECamCameraControllerCorrupt );
      break;
      }
    // -----------------------------------------------------
    }
  PRINT( _L("Camera <= CCamCameraController::ProcessImageShutdownRequest") );
  return ETrue; // can continue sequence, if needed
  }



// ---------------------------------------------------------------------------
// ProcessVideoRequestL
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessVideoRequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::ProcessVideoRequestL") );
  CheckFlagOnL( iInfo.iState, ECamPowerOn, KErrNotReady );

  TInt callback( ETrue );

#ifdef CAMERAAPP_CAE_FOR_VIDEO
  switch( aRequestId )
    {
    // -----------------------------------------------------
    case ECamRequestVideoInit:
      {
      PRINT( _L("Camera <> case ECamRequestVideoInit") );
  #ifdef CAMERAAPP_CAE_FIX
      PRINT( _L("Camera <> old CAE") );
      // First init for video after image mode or startup
      if( !IsFlagOn( iInfo.iState, ECamVideoOn ) )
        {
        iModeChange       = ECamModeChangeImage2Video;
        iModeChangePhase  = ECamModeChangePhaseIdle; // incremented before first step
        iModeChangeStatus = KErrNone;
        iActive->IssueRequest();
        }
      // Just repeat prepare
      else
  #else
      PRINT( _L("Camera <> new CAE, CCameraHandle + SetCameraReserved supported") );
      if( !IsFlagOn( iInfo.iState, ECamVideoOn ) )
        {
        PRINT( _L("Camera <> CCaeEngine::EnableVideoRecording..") );
        iCaeEngine->EnableVideoRecording();
        }
  #endif
        {
       if ( iAppController.VideoInitNeeded() )
            {
            PRINT( _L("Camera <> CCaeEngine::InitVideoRecorderL..") );
            iCaeEngine->InitVideoRecorderL();
            }

        PRINT( _L("Camera <> Ask filename..") );
        HBufC* filename( HBufC::NewLC( KMaxFileName ) );
        TPtr   ptr     ( filename->Des()              );
        iSettingProvider.ProvideCameraSettingL( ECameraSettingFileName, &ptr );
        // _LIT( KTempFilename, "C:\\video.3gp" );
        // TPtrC ptr;
        // ptr.Set( KTempFilename() );
        PRINT1( _L("Camera <> Set filename [%S]"), &ptr );
        iCaeEngine->SetVideoRecordingFileNameL( ptr );
        CleanupStack::PopAndDestroy( filename );

        // Set max video clip size
        ProcessSettingL( ECameraSettingFileMaxSize );

        TPckgBuf<TCamParamsVideoCae> params;
        PRINT( _L("Camera <> Getting params from setting provider..") );
        iSettingProvider.ProvideCameraParamL( ECameraParamVideoCae, &params );
        // The audioOn value is defined On==0 and Off==1, but the engine expects
        // ETrue if audio recording is On
        params().iAudioOn = ( ECamSettOn == params().iAudioOn )
        											? ETrue
        											: EFalse;
        PRINT( _L("Camera <> Calling PrepareVideoRecordingL..") );
        PRINT2( _L("Camera <> FrameSize: (%d x %d)"), params().iFrameSize.iWidth, params().iFrameSize.iHeight );
        if ( iAppController.VideoInitNeeded() )
            {
            // McaeoVideoPrepareComplete will be called when prepare is ready.
            // The callback is allowed to come also *during* this call.
            iCaeEngine->PrepareVideoRecordingL( params().iFrameSize,
                    params().iFrameRate,
                    params().iVideoBitRate,
                    params().iAudioOn,
                    params().iAudioBitRate,
                    params().iMimeType,
                    params().iSupplier,
                    params().iVideoType,
                    params().iAudioType );
            }

#ifdef CAMERAAPP_CAPI_V2_ADV
      // Set current autofocus range to invalid value to force focusing
      iInfo.iCurrentFocusRange = static_cast<CAS::TFocusRange>( -1 );
#endif // CAMERAAPP_CAPI_V2_ADV

        iAppController.SetVideoInitNeeded( EFalse );
        // iState is updated in the callback.
        PRINT( _L("Camera <> ..waiting for callback") );
        }
      break;
      }
    // -----------------------------------------------------
    case ECamRequestVideoStart:
      {
      PRINT( _L("Camera <> case ECamRequestVideoStart") );
      CheckFlagOnL( iInfo.iState, ECamVideoOn, KErrNotReady );
      switch( iInfo.iCaptureState )
        {
        case ECamCaptureOn:     User::Leave( KErrInUse );           break;
        case ECamCaptureOff:    iCaeEngine->StartVideoRecording();  break;
        //case ECamCaptureOff:    iCaeEngine->ResumeVideoRecording();  break;
        case ECamCapturePaused: iCaeEngine->ResumeVideoRecording(); break;
        default:
          Panic( ECamCameraControllerCorrupt );
          break;
        }
      // iCaptureState is updated in McaeoVideoRecordingOn.
      break;
      }
    // -----------------------------------------------------
    case ECamRequestVideoPause:
      {
      PRINT( _L("Camera <> case ECamRequestVideoPause") );
      CheckEqualsL( iInfo.iCaptureState, ECamCaptureOn, KErrNotReady );
      iCaeEngine->PauseVideoRecording();
      // iCaptureState is updated in McaeoVideoRecordingPaused.
      break;
      }
    // -----------------------------------------------------
    case ECamRequestVideoStop:
      {
      PRINT( _L("Camera <> case ECamRequestVideoStop") );
      switch( iInfo.iCaptureState )
        {
        case ECamCaptureOn:     // << fall through >>
        case ECamCapturePaused:
          PRINT( _L("Camera <> call CCaeEngine::StopVideoRecording..") );
          iCaeEngine->StopVideoRecording();
          // If we got the callback during above call,
          // we should not wait for it anymore.
          // Check the capture state to be sure.
          callback = (ECamCaptureOff != iInfo.iCaptureState);
          break;
        case ECamCaptureOff:
          // no action, already stopped
          break;
        default:
          Panic( ECamCameraControllerCorrupt );
          break;
        }
      // iCaptureState is updated in McaeoVideoRecordingComplete.
      break;
      }
    // -----------------------------------------------------
    case ECamRequestVideoRelease:
      {
      // Need to be stopped.
      CheckEqualsL( iInfo.iCaptureState, ECamCaptureOff, KErrInUse );

      if( IsFlagOn( iInfo.iState, ECamVideoOn ) )
        iCaeEngine->CloseVideoRecording();

      ClearFlags( iInfo.iState, ECamVideoOn );
      callback = EFalse;
#ifdef CAMERAAPP_CAPI_V2_ADV
      GetAdvancedSettingsInfoL();
#endif
      break;
      }
    // -----------------------------------------------------
    case ECamRequestSetAsyncVideoStopMode:
      {
      TInt err = iCaeEngine->SetAsyncVideoStopMode( ETrue );
      PRINT1( _L("Camera <> CCamCameraController::ProcessVideoRequestL, engine SetAsyncVideoStopMode return code:%d"), err );
      iAsyncVideoStopModeSupported = !err; // Use async stop if KErrNone
      callback = EFalse;
      break;
      }
    // -----------------------------------------------------
    default:
      Panic( ECamCameraControllerCorrupt );
      break;
    // -----------------------------------------------------
    }
#else
  #pragma message("CCamCameraController::ProcessVideoRequestL not implemented!")
  PRINT( _L("Camera <> Not supported, PANIC!") );
  Panic( ECamCameraControllerUnsupported );
#endif // CAMERAAPP_CAE_FOR_VIDEO

  PRINT1( _L("Camera <= CCamCameraController::ProcessVideoRequestL, continue now:%d"), !callback );
  return !callback;
  }



// ---------------------------------------------------------------------------
// ProcessSnapshotRequestL
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::ProcessSnapshotRequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::ProcessSnapshotRequestL") );
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMCAMERACONTROLLER_PROCESSSNAPSHOTREQUESTL, "e_CCamCameraController_ProcessSnapshotRequestL 1" );
  CheckFlagOnL( iInfo.iState, ECamReserved, KErrNotReady );

  switch( aRequestId )
    {
    // -----------------------------------------------------
    case ECamRequestSsStart:
      {
      PRINT( _L("Camera <> case ECamRequestSsStart") );
      switch( iInfo.iSsState )
        {
        case ECamTriIdle:
          PRINT( _L("Camera <> ECamTriIdle") );
          InitSnapshotL();
          // << fall through >>
        case ECamTriInactive:
          {
          PRINT( _L("Camera <> ECamTriInactive") );
          iSnapshotProvider->StartSnapshot();
          break;
          }
        case ECamTriActive: // Already active, no action
          PRINT( _L("Camera <> ECamTriActive") );
          break;
        default:
          Panic( ECamCameraControllerCorrupt );
          break;
        }
      iInfo.iSsState = ECamTriActive;
      break;
      }
    // -----------------------------------------------------
    case ECamRequestSsStop:
      {
      PRINT( _L("Camera <> case ECamRequestSsStop") );
      ProcessSsStopRequest();
      break;
      }
    // -----------------------------------------------------
    case ECamRequestSsRelease:
      {
      PRINT( _L("Camera <> case ECamRequestSsRelease") );
      ProcessSsReleaseRequest();
      break;
      }
    // -----------------------------------------------------
    default:
      Panic( ECamCameraControllerCorrupt );
      break;
    // -----------------------------------------------------
    }
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMCAMERACONTROLLER_PROCESSSNAPSHOTREQUESTL, "e_CCamCameraController_ProcessSnapshotRequestL 0" );

  PRINT( _L("Camera <= CCamCameraController::ProcessSnapshotRequestL") );

  // No callback to wait for any of the requests => can continue now.
  return ETrue;
  }

// ---------------------------------------------------------------------------
// ProcessSsStopRequest
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ProcessSsStopRequest()
  {
  PRINT( _L("Camera <= CCamCameraController::ProcessSsStopRequest") );
  switch( iInfo.iSsState )
    {
    case ECamTriIdle:     // Not even initialized yet. No action needed.
    case ECamTriInactive: // Already inactive, no action.
      PRINT( _L("Camera <> Snapshot idle/inactive, no need to stop") );
      break;
    case ECamTriActive:
      PRINT( _L("Camera <> Snapshot ECamTriActive -> need to stop") );
      iSnapshotProvider->StopSnapshot();
      break;
    default:
      Panic( ECamCameraControllerCorrupt );
      break;
    }
  iInfo.iSsState = ECamTriInactive;
  PRINT( _L("Camera <= CCamCameraController::ProcessSsStopRequest") );
  }

// ---------------------------------------------------------------------------
// ProcessSsReleaseRequest
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ProcessSsReleaseRequest()
  {
  PRINT( _L("Camera => CCamCameraController::ProcessSsReleaseRequest") );
  switch( iInfo.iSsState )
    {
    case ECamTriIdle:    // No action needed
      break;
    case ECamTriActive:
      iSnapshotProvider->StopSnapshot();
      NotifyObservers( KErrNone,
                       ECamCameraEventSsStop,
                       ECamCameraEventClassSsControl );
      // << fall through >>
    case ECamTriInactive:
      delete iSnapshotProvider;
      iSnapshotProvider = NULL;
      break;
    default:
      Panic( ECamCameraControllerCorrupt );
      break;
    }

  iInfo.iSsState = ECamTriIdle;
  PRINT( _L("Camera <= CCamCameraController::ProcessSsReleaseRequest") );
  }

// ---------------------------------------------------------------------------
// InitSnapshotL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::InitSnapshotL()
  {
  PRINT( _L("Camera => CCamCameraController::InitSnapshotL") );

  // Check the snapshot state here. No further checks made.
  __ASSERT_DEBUG( ECamTriIdle == iInfo.iSsState, Panic( ECamCameraControllerCorrupt ) );

  // -------------------------------------------------------
  // Create snapshot provider if not available yet.
  if( !iSnapshotProvider )
    {
    iSnapshotProvider = CCamSnapshotProvider::NewL( *iCamera, *this, *this );
    }

#ifdef _DEBUG
  PrintSnapshotInfo();
#endif // _DEBUG


  // -------------------------------------------------------
  // Initialize the snapshot parameters
  TPckgBuf<TCamParamsSnapshot> params;
  iSettingProvider.ProvideCameraParamL( ECameraParamSnapshot, &params );

  // Try to get the best, still supported, snapshot format.
  iInfo.iSnapshotFormat         = ResolveSnapshotFormatL( params().iFormat );
  iInfo.iSnapshotSize           = params().iSize;
  iInfo.iSnapshotAspectMaintain = params().iMaintainAspect;

  PRINT( _L("Camera <> Prepare snapshot..") );
  iSnapshotProvider->PrepareSnapshotL( iInfo.iSnapshotFormat,
                                       iInfo.iSnapshotSize,
                                       iInfo.iSnapshotAspectMaintain );

  iInfo.iSsState = ECamTriInactive;
  // -------------------------------------------------------
  PRINT( _L("Camera <= CCamCameraController::InitSnapshotL") );
  }


// ---------------------------------------------------------------------------
// CCamCameraController::ProcessAutofocusRequestL
// ---------------------------------------------------------------------------
//
void CCamCameraController::ProcessAutofocusRequestL( const TCamCameraRequestId& aRequestId )
  {
  PRINT( _L("Camera => CCamCameraController::ProcessAutofocusRequestL") );
#ifdef CAMERAAPP_CAPI_V2_ADV
 CheckNonNullL( iAdvancedSettings, KErrNotReady );
  if( ECamRequestStartAutofocus == aRequestId )
    {
    PRINT( _L("Camera <> SetAutoFocusType( EAutoFocusTypeSingle )") );
    iAdvancedSettings->SetAutoFocusType( CAS::EAutoFocusTypeSingle );
    iAfInProgress = ETrue;
    iFirstAfEventReceived = EFalse;
    }
  else if( ECamRequestCancelAutofocus == aRequestId )
    {
    if( iAfInProgress && iInfo.iCurrentCamera == KPrimaryCameraIndex )
      {
      // Autofocus in progress, need to cancel it before setting range to hyperfocal
      PRINT( _L("Camera <> Cancel ongoing autofocus request") );
      iAdvancedSettings->SetAutoFocusType( CAS::EAutoFocusTypeOff );
      iAfHyperfocalPending = ETrue;
      }
    else
      {
      // If focustype is set to continuous, need to change it to off
	  // before continuing

      PRINT( _L("Camera <> SetAutoFocusType( EAutoFocusTypeOff )") );
      if ( iAdvancedSettings->AutoFocusType() & CAS::EAutoFocusTypeContinuous )
        {
        iAdvancedSettings->SetAutoFocusType( CAS::EAutoFocusTypeOff );
        }

      PRINT( _L("Camera <> Cancel autofocus - set focus range to hyperfocal") );
      iInfo.iCurrentFocusRange = CAS::EFocusRangeHyperfocal;
      iAdvancedSettings->SetFocusRange( CAS::EFocusRangeHyperfocal );

      // Then start the focus. The callback of this cancel request sets
      // a boolean in CamAppController, so the resulting optimal
      // focus event does not change reticule/focus state.
      iAdvancedSettings->SetAutoFocusType( CAS::EAutoFocusTypeSingle );
      iAfInProgress = ETrue;
      }
    }
  else if( ECamRequestSetAfRange == aRequestId )
    {
    PRINT( _L("Camera <> Set autofocus range") );
    // Get autofocus mode from settings provider

    CAS::TFocusRange afRange;
    iSettingProvider.ProvideCameraSettingL( ECameraSettingFocusRange, &afRange );

    if( iInfo.iCurrentFocusRange != afRange )
      {
      iAdvancedSettings->SetFocusRange( afRange );

      // Should this be done in the callback?:
      iInfo.iCurrentFocusRange = afRange;
      }
    else
      {
      // Correct range already set. No need to do anything.
      }
    }
  else
    {
    // Other request must not end up here
    __ASSERT_DEBUG( EFalse, Panic( ECamCameraControllerCorrupt ) );
    }
#endif // CAMERAAPP_CAPI_V2_ADV

  (void)aRequestId; // removes compiler warning
  PRINT( _L("Camera <= CCamCameraController::ProcessAutofocusRequestL") );
  }



// ---------------------------------------------------------------------------
// ProcessCaptureLimitSettingL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ProcessCaptureLimitSettingL()
  {
  PRINT ( _L("Camera => CCamCameraController::ProcessCaptureLimitSettingL") );
#ifdef CAMERAAPP_CAPI_V2_ADV
  // Check that we are prepared for image mode.
  CheckFlagOnL( iInfo.iState, ECamImageOn, KErrNotReady );

  // Get requested capture count and determine current and target drive modes.
  TInt requestedLimit( 1 );
  iSettingProvider.ProvideCameraSettingL( ECameraSettingCaptureLimit, &requestedLimit );
  if( requestedLimit < 1 ) User::Leave( KErrArgument );

  const CAS::TDriveMode& currentMode( iAdvancedSettings->DriveMode() );
  const CAS::TDriveMode  targetMode ( requestedLimit <= 1
                                    ? CAS::EDriveModeSingleShot
                                    : CAS::EDriveModeBurst      );
  const TInt cameraLimit( iAdvancedSettings->BurstImages() );

  PRINT ( _L("Camera <> CCamCameraController .. --------------------------------------") );
  PRINT1( _L("Camera <> CCamCameraController .. requested capture  limit   %04d"), requestedLimit                  );
  PRINT1( _L("Camera <> CCamCameraController .. current   capture  limit : %04d"), iInfo.iCaptureLimit           );
  PRINT1( _L("Camera <> CCamCameraController .. current   camera's limit : %04d"), cameraLimit                   );
  PRINT1( _L("Camera <> CCamCameraController .. current   capture  count : %04d"), iInfo.iCaptureCount           );
  PRINT1( _L("Camera <> CCamCameraController .. current   snapshot count : %04d"), iInfo.iSnapshotCount          );
  PRINT1( _L("Camera <> CCamCameraController .. current drive mode       : [%S]"), &DriveModeName( currentMode ) );
  PRINT1( _L("Camera <> CCamCameraController .. target  drive mode       : [%S]"), &DriveModeName( targetMode  ) );
  PRINT ( _L("Camera <> CCamCameraController .. --------------------------------------") );

  // -------------------------------------------------------
  // Determine needed changes and when to issue them
  //
  // During burst capture, we may receive snapshots and
  // image data on mixed order, e.g:
  //
  //   [ ss#1 | img#1 | ss#2 | ss#3 | img2 | img3 ]
  //   --------------^^--------------------------^^
  //
  // C-API starts new burst capture when we adjust the
  // capture limit. To avoid problems and to get equal
  // amount of snapshots and images, capture count is
  // only changed when we image data is received,
  // and as many images as snapshots have arrived.
  // In the chart above ^ marks a place where capture limit
  // can be updated.
  TBool callback( EFalse );

  // -----------------------------------
  // Capture ongoing..
  if( ECamCaptureOn == iInfo.iCaptureState )
    {
    PRINT( _L("Camera <> CCamCameraController .. Capture ongoing..") );
    if( targetMode != currentMode )
      {
      // Cannot change drive mode during capturing.
      PRINT( _L("Camera <> CCamCameraController .. Drive mode update not possible now, LEAVE!") );
      User::Leave( KErrInUse );
      }
    else if ( CAS::EDriveModeBurst == currentMode )
      {
      if( cameraLimit != requestedLimit )
        {
        PRINT( _L("Camera <> CCamCameraController .. Adjusting capture limit during burst..") );
        // Set limit locally and react on next image data event.
        iInfo.iCaptureLimit = Min( cameraLimit, Max( requestedLimit, iInfo.iCaptureCount+1 ) );
        PRINT1( _L("Camera <> CCamCameraController .. Set local capture limit to %d.."), iInfo.iCaptureLimit );
        }
      else
        {
        PRINT( _L("Camera <> CCamCameraController .. Capture limit during burst already has right value.") );
        }
      }
    else
      {
      // No action needed. Capture limit of 1 image kept.
      PRINT( _L("Camera <> CCamCameraController .. Single shot mode kept, no action") );
      }
    }
  // -----------------------------------
  // Not capturing
  //
  else
    {
    PRINT( _L("Camera <> CCamCameraController .. No capture ongoing..") );

    iInfo.iCaptureLimit = requestedLimit;
    // Capture limit has changed,
    // check if drive mode also needs to be changed..
    if( targetMode != currentMode )
      {
      PRINT1( _L("Camera <> CCamCameraController .. Calling SetDriveMode([%S])"), &DriveModeName( targetMode  ) );
      iAdvancedSettings->SetDriveMode( targetMode );
      }
    if( CAS::EDriveModeBurst == targetMode )
      {
      PRINT1( _L("Camera <> CCamCameraController .. Calling SetBurstImages(%d).."), iInfo.iCaptureLimit );
      iAdvancedSettings->SetBurstImages( iInfo.iCaptureLimit );
      }
    }

  // -------------------------------------------------------
  // Notify user that the change is done
  // or wait for camera callbacks to finish.
  PRINT1( _L("Camera <> CCamCameraController .. Should wait callback = %d"), callback );
  if( !callback )
    {
    TInt setting( ECameraSettingCaptureLimit );
    NotifyObservers( KErrNone,
                     ECamCameraEventSettingsSingle,
                     ECamCameraEventClassSettings,
                     &setting );
    }
#else
  User::Leave( KErrNotSupported );
#endif // CAMERAAPP_CAPI_V2_ADV
  PRINT ( _L("Camera <= CCamCameraController::ProcessCaptureLimitSettingL") );
  }



// ---------------------------------------------------------------------------
// EndSequence
// ---------------------------------------------------------------------------
//
void
CCamCameraController::EndSequence( TInt aStatus )
  {
  PRINT1( _L("Camera => CCamCameraController::EndSequence, status:%d"), aStatus );

  iIveRecoveryOngoing = EFalse;
  iIveSequenceActive = EFalse;
#ifdef CAMERAAPP_CAE_FIX
  if( ECamModeChangeInactive != iModeChange )
    {
    PRINT( _L("Camera <> mode change not cleared, PANIC!!") );
    Panic( ECamCameraControllerUnrecovableError );
    }
#endif // CAMERAAPP_CAE_FIX
  // -------------------------------------------------------
  if( IsFlagOn( iInfo.iBusy, ECamBusySequence ) )
    {
    CAMERAAPP_PERF_CONTROLLER_STOP( ECamRequestNone );

    PRINT( _L("Camera <> ending sequence..") );
    ClearRequestQueue();
    iReserveTryAgainCount = KCamReserveTryAgainMaxCount;


    // Need to first clear busy flag as observer might issue
    // new requests in notification callback.
    ClearFlags( iInfo.iBusy, ECamBusySequence );

    PRINT( _L("Camera <> send notification..") );
    NotifyObservers( aStatus,
                     ECamCameraEventSequenceEnd,
                     ECamCameraEventClassBasicControl );
    }
  // -------------------------------------------------------
  else if( IsFlagOn( iInfo.iBusy, ECamBusySetting ) )
    {
    PRINT( _L("Camera <> ending settings handling..") );


    TCamCameraSettingId last( ECameraSettingNone );
    if( KErrNone != aStatus )
      {
      if( Rng( 0, iSettingIndex, iSettingArray.Count()-1 ) )
         {
         last = iSettingArray[iSettingIndex];
         }
      }
    ClearSettingQueue();

    ClearFlags( iInfo.iBusy, ECamBusySetting );

    NotifyObservers( aStatus,
                     ECamCameraEventSettingsDone,
                     ECamCameraEventClassSettings,
                     &last );
    }
  // -------------------------------------------------------
  else
    {
    PRINT( _L("Camera <> No active operation, no action needed") );
    }
  // -------------------------------------------------------
  PRINT( _L("Camera <= CCamCameraController::EndSequence") );
  }



// ---------------------------------------------------------------------------
// ClearRequestQueue
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ClearRequestQueue()
  {
  iSequenceArray.Reset();
  iSequenceIndex = -1;
  }

// ---------------------------------------------------------------------------
// ClearSettingQueue
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ClearSettingQueue()
  {
  iSettingArray.Reset();
  iSettingIndex = -1;
  }




// ---------------------------------------------------------------------------
// HandlePowerOnEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandlePowerOnEvent( TInt aStatus )
  {
  PRINT1( _L("Camera => CCamCameraController::HandlePowerOnEvent, status:%d"), aStatus );

  // We should be blocking new request at the moment.
  __ASSERT_DEBUG( iInfo.iBusy, Panic( ECamCameraControllerCorrupt ) );
  CAMERAAPP_PERF_CONTROLLER_STOP( ECamRequestPowerOn );

  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMCAMERACONTROLLER_HANDLEPOWERONEVENT, "e_CAM_APP_INIT 0" );  //CCORAPP_APP_INIT_END
  OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMCAMERACONTROLLER_HANDLEPOWERONEVENT, "e_CAM_APP_VF_INIT 1" );  //CCORAPP_APP_VF_INIT_START
  OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMCAMERACONTROLLER_HANDLEPOWERONEVENT, "e_CAM_APP_CONFIGURATIONS 1" );   //CCORAPP_APP_CONFIGS_START
  OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMCAMERACONTROLLER_HANDLEPOWERONEVENT, "e_CAM_APP_STILL_INIT 1" );   //CCORAPP_APP_STILL_INIT_START

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

  if ( appUi->AppController().UiConfigManagerPtr()->IsFaceTrackingSupported() )
      {
      PRINT( _L("Camera <> CCamAppController: Get i/f MCameraFaceTracking..") )
      iCustomInterfaceFaceTracking =
          static_cast <MCameraFaceTracking*>(
            iCamera->CustomInterface( KCameraFaceTrackingUid ) );
      PRINT1( _L("Camera <> Face Tracking custom i/f pointer:%d"), iCustomInterfaceFaceTracking );
      }

  if( KErrNone == aStatus )
    {
    SetFlags( iInfo.iState, ECamPowerOn|ECamReserved );

#if defined( CAMERAAPP_CAE_FOR_VIDEO ) && !defined( CAMERAAPP_CAE_FIX )
    // We need to tell to CCaeEngine that the CCamera has been reserved
    // and powered on "behind its back".
    if( iCaeEngine )
      {
      iCaeEngine->EnableVideoRecording();
      }
#endif
    }
  else
    {
    ClearFlags( iInfo.iState, ECamPowerOn );
    }

#ifdef CAMERAAPP_CAE_FIX
  if( ECamModeChangeVideo2Image == iModeChange )
    {
    if( ECamModeChangePhase2 == iModeChangePhase )
      {
      // We have called CCamera::PowerOn in phase2
      PRINT( _L("Camera <> mode change image->video ongoing, continue") );
      iModeChangeStatus = aStatus;
      // Handle repeative events before iActive gets to run.
      if( !iActive->IsActive() )
        iActive->IssueRequest();
      }
    }
  else if( ECamModeChangeImage2Video == iModeChange )
    {
    if( ECamModeChangePhase1 == iModeChangePhase )
      {
      // We have called CCaeEngine::InitL in phase1
      PRINT( _L("Camera <> mode change image->video ongoing, continue") );
      iModeChangeStatus = aStatus;
      // Handle repeative events before iActive gets to run.
      if( !iActive->IsActive() )
        iActive->IssueRequest();
      }
    }
  else
    {
    PRINT( _L("Camera <> no mode change ongoing") );
    HandleCallbackEvent( aStatus,
                         ECamCameraEventPowerOn,
                         ECamCameraEventClassBasicControl );
    }
#else

  HandleCallbackEvent( aStatus,
                       ECamCameraEventPowerOn,
                       ECamCameraEventClassBasicControl );

#endif // CAMERAAPP_CAE_FIX

  if ( appUi->AppController().UiConfigManagerPtr()->IsOrientationSensorSupported() )
      {
      PRINT( _L("Camera <> CCamCameraController: Get i/f MCameraOrientation..") )

      if(!iCustomInterfaceOrientation )
          {
          iCustomInterfaceOrientation =
          static_cast <MCameraOrientation*>(
          iCamera->CustomInterface( KCameraOrientationUid ) );
          }

      PRINT1( _L("Camera <> Orientation custom i/f pointer:%d"), iCustomInterfaceOrientation );
      }

  PRINT( _L("Camera <= CCamCameraController::HandlePowerOnEvent") );
  }

// ---------------------------------------------------------------------------
// HandleReserveGainEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleReserveGainEvent( TInt aStatus )
  {
  PRINT1( _L("Camera => CCamCameraController::HandleReserveGainEvent, status:%d"), aStatus );
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  if( appUi->StandbyStatus() && !appUi->IsRecoverableStatus() )
      {
      PRINT( _L("Camera <= CCamCameraController::HandleReserveGainEvent - return, in non recoverable standby state"));
      return;
      }
  if( iIdle && iIdle->IsActive() )
    {
    PRINT( _L("Camera => CCamCameraController::HandleReserveGainEvent - return, recovery in progress"));
    return;
    }
  if( iIveRecoveryOngoing && !iIveSequenceActive )
    {
    PRINT( _L("Camera => CCamCameraController::HandleReserveGainEvent - first recovering sequence command executed"));
    iIveSequenceActive = ETrue;
    }
  // We should be blocking new request at the moment.
  __ASSERT_DEBUG( iInfo.iBusy, Panic( ECamCameraControllerCorrupt ) );
  CAMERAAPP_PERF_CONTROLLER_STOP( ECamRequestReserve );

  if( KErrNone == aStatus ) SetFlags  ( iInfo.iState, ECamReserved );
  else                      ClearFlags( iInfo.iState, ECamReserved );

#ifdef CAMERAAPP_CAE_FIX
  if( ECamModeChangeVideo2Image == iModeChange )
    {
    if( ECamModeChangePhase1 == iModeChangePhase )
      {
      // We have called CCamera::Reserve in phase1
      PRINT( _L("Camera <> mode change video->image ongoing, continue") );
      iModeChangeStatus = aStatus;
      // Handle repeative events before iActive gets to run.
      if( !iActive->IsActive() )
        iActive->IssueRequest();
      }
    }
  else if( ECamModeChangeImage2Video == iModeChange )
    {
    // No action yet, wait for power on event
    PRINT( _L("Camera <> mode change image->video ongoing, waiting for power on event") );
    }
  else
    {
    PRINT( _L("Camera <> no mode change ongoing") );
    HandleCallbackEvent( aStatus,
                         ECamCameraEventReserveGain,
                         ECamCameraEventClassBasicControl );
    }
#else
  HandleCallbackEvent( aStatus,
                       ECamCameraEventReserveGain,
                       ECamCameraEventClassBasicControl );
#endif // CAMERAAPP_CAE_FIX
  PRINT( _L("Camera <= CCamCameraController::HandleReserveGainEvent") );
  }



// ---------------------------------------------------------------------------
// HandleReserveLostEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleReserveLostEvent( TInt aStatus )
  {
  PRINT1( _L("Camera => CCamCameraController::HandleReserveLostEvent, status:%d"), aStatus );
  iInfo.iState   = ECamIdle;
  iInfo.iVfState = ECamTriIdle;
  iInfo.iSsState = ECamTriIdle;

#pragma message("CCamCameraController: Reserve lost event does not stop sequence")

  // These are not valid anymore.
  ReleaseCustomInterfaces();

#ifdef CAMERAAPP_CAE_FOR_VIDEO
  // -------------------------------------------------------
#ifdef CAMERAAPP_CAE_FIX
#ifdef CAMERAAPP_CAPI_V2
  if( ECamModeChangeVideo2Image == iModeChange )
    {
    // ignore
    }
  else if( ECamModeChangeImage2Video == iModeChange )
    {
    // ignore
    }
  else
#endif // CAMERAAPP_CAPI_V2
    {
    PRINT( _L("Camera <> no mode change ongoing") );
    NotifyObservers( aStatus,
                     ECamCameraEventReserveLose,
                     ECamCameraEventClassBasicControl );
    }

#else // CAMERAAPP_CAE_FIX
/*
  // We need to tell to CCaeEngine that the CCamera
  // has been released "behind its back".
  if( iCaeEngine )
    {
    iCaeEngine->DisableVideoRecording();
    }
*/
#endif // CAMERAAPP_CAE_FIX
  // -------------------------------------------------------
#else

  NotifyObservers( aStatus,
                   ECamCameraEventReserveLose,
                   ECamCameraEventClassBasicControl );
  // -------------------------------------------------------
#endif // CAMERAAPP_CAE_FOR_VIDEO

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  TInt err = KErrNone;
  if( !iIdle )
    {
    TRAP( err, iIdle = CIdle::NewL( CActive::EPriorityIdle ) );
    }

  PRINT1( _L("Camera <> CCamCameraController::HandleReserveLostEvent iIveRecoveryOngoing = %d"), iIveRecoveryOngoing );
  if ( !err && iIdle && !iIdle->IsActive() && // Recovering already
       iAppController.IsAppUiAvailable()  && // Exit mode
       iIveRecoveryCount &&                   // Give up eventually
       !appUi->AppInBackground( EFalse ) &&   // Only if on the foreground
       ( !iReleasedByUi ||             // Try recover if unknown reason
         ( appUi->StandbyStatus() && appUi->IsRecoverableStatus() ) ) &&    // or known error 
         !iAppController.InVideocallOrRinging() && // Video telephony parallel use case
         !iIveRecoveryOngoing        //  processing recovery sequence
         )
    {
    PRINT( _L("Camera <> CCamCameraController::HandleReserveLostEvent - Start recovery") );
    NotifyObservers( aStatus,
                     ECamCameraEventReserveLose,
                     ECamCameraEventClassBasicControl );
    iIdle->Start( TCallBack( IdleCallback, this ) );
    }
  PRINT( _L("Camera <= CCamCameraController::HandleReserveLostEvent") );
  }

// ---------------------------------------------------------------------------
// HandleViewfinderEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleViewfinderEvent( MCameraBuffer* aCameraBuffer,
                                             TInt           aStatus )
  {
  PRINT_FRQ1( _L("Camera => CCamCameraController::HandleViewfinderEvent, status in:%d"), aStatus );

  CAMERAAPP_PERF_CONTROLLER_STOP_ONCE( ECamRequestVfStart, iFirstVfFrameReceived );

  CFbsBitmap* vfFrame = NULL;

  if( KErrNone == aStatus )
    {
    __ASSERT_ALWAYS( aCameraBuffer, Panic( ECamCameraControllerCorrupt ) );

    PRINT_FRQ( _L("Camera <> get new one..") );
    TRAP( aStatus,
      {
      vfFrame = &(aCameraBuffer->BitmapL( 0 ));
      });
    PRINT_FRQ1( _L("Camera <> ..status after getting bitmap data: %d"), aStatus );
    }

  NotifyObservers( aStatus,
                   ECamCameraEventVfFrameReady,
                   ECamCameraEventClassVfData,
                   vfFrame );

  ReleaseAndNull( aCameraBuffer );

  PRINT_FRQ( _L("Camera <= CCamCameraController::HandleViewfinderEvent") );
  }


// ---------------------------------------------------------------------------
// HandleImageCaptureEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleImageCaptureEvent( MCameraBuffer* aCameraBuffer,
                                               TInt           aStatus )
  {
  PRINT1( _L("Camera => CCamCameraController::HandleImageCaptureEvent, status in: %d"), aStatus );


  // Check that we are expecting images..
  if( ECamCaptureOn == iInfo.iCaptureState )
    {
    // Store flags
    TUint busyFlags( iInfo.iBusy );

    // -----------------------------------------------------
    // Try to get the image data.
    if( KErrNone == aStatus )
      {
      // Takes ownership of aCameraBuffer and NULLs the pointer.
      TRAP( aStatus, HandleImageCaptureEventL( aCameraBuffer ) );
      PRINT1( _L("Camera <> status after handling data: %d"), aStatus );
      }

    // Release if not NULLed in HandleImageCaptureEventL.
    ReleaseAndNull( aCameraBuffer );

    // -----------------------------------------------------
    // Errors in or in handling
    if( KErrNone != aStatus )
      {
      PRINT( _L("Camera <> CCamCameraController ... [WARNING] error in image data event!") );
      iInfo.PrintInfo();

      iInfo.iCaptureState = ECamCaptureOff;

      // See HandleCallbackEvent for comments.
      // ClearFlags( iInfo.iBusy, ECamBusySingle );

      NotifyObservers( aStatus,
                       ECamCameraEventImageData,
                       ECamCameraEventClassImage );
      }
    // -----------------------------------------------------

    // If capture state was just changed to OFF,
    // send notification that capture has fully completed.
    if( ECamCaptureOff == iInfo.iCaptureState )
      {
      // See HandleCallbackEvent for comments.
      ClearFlags( iInfo.iBusy, ECamBusySingle );

      TInt fullCaptures( Min( iInfo.iCaptureCount, iInfo.iSnapshotCount ) );
      NotifyObservers( aStatus,
                       ECamCameraEventImageStop,
                       ECamCameraEventClassImage,
                       &fullCaptures );

      // Now we can actually continue, if necessary.
      if( IsFlagOn( busyFlags, ECamBusySequence ) )
        {
        __ASSERT_DEBUG( iActive, Panic( ECamCameraControllerCorrupt ) );
        iActive->IssueRequest();
        }
      }
    }
  else
    {
    PRINT( _L("Camera <> CCamCameraController ... [WARNING] Unexpected image data event!") );
    iInfo.PrintInfo();

    // Stop capture to be safe.
    PRINT( _L("Camera <> CCamCameraController ... Calling CancelCaptureImage..") );
    iCamera->CancelCaptureImage();

    // Not expecting image data at this time.
    // Release the buffer and ignore event.
    ReleaseAndNull( aCameraBuffer );
    }
  PRINT( _L("Camera <= CCamCameraController::HandleImageCaptureEvent") );
  }

// ---------------------------------------------------------------------------
// HandleImageCaptureEventL
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleImageCaptureEventL( MCameraBuffer*& aCameraBuffer )
  {
  PRINT( _L("Camera => CCamCameraController::HandleImageCaptureEventL") );
  CheckNonNullL( aCameraBuffer, KErrNotFound );

  MCameraBuffer* temp = aCameraBuffer;
  aCameraBuffer = NULL; // ownership taken

  PRINT( _L("Camera <> Wrap MCameraBuffer to sharable..") );
  CleanupStack::PushL( TCleanupItem( CameraBufferCleanup, temp ) );
  CCamBufferShare* share = new (ELeave) CCamBufferShare( temp );
  CleanupStack::Pop(); // cleanup item
  temp = NULL;         // ownership to share

  share->Reserve();
  CleanupStack::PushL( TCleanupItem( CamBufferShareCleanup, share ) );

  PRINT( _L("Camera <> CCamCameraController .. Checking encoded data availability..") );
  TRAPD( dataStatus,
    {
    TDesC8* data = share->SharedBuffer()->DataL( 0 );
    if( !data )
      User::Leave( KErrNotFound );
    });

  // -------------------------------------------------------
  // Normal capture format case
  if( KErrNone == dataStatus )
    {
    PRINT( _L("Camera <> CCamCameraController .. Normal, encoded capture format found..") );

    // Increase received images count. Needed in burst drive mode.
    iInfo.iCaptureCount++;
    PRINT1( _L("Camera <> CCamCameraController ... Incremented capture counter to: %d"), iInfo.iCaptureCount );

    // If we have needed amount of snapshots and images, end capture.
    if( iInfo.iCaptureCount  >= iInfo.iCaptureLimit
     && ( (iInfo.iSnapshotCount >= iInfo.iCaptureLimit) || iInfo.iSsState != ECamTriActive ) )
      {
      PRINT( _L("Camera <> CCamCameraController ... Capture limit met, calling CancelCaptureImage..") );
      iCamera->CancelCaptureImage();

      PRINT( _L("Camera <> CCamCameraController ... Setting capture state to OFF..") );
      iInfo.iCaptureState = ECamCaptureOff;
      }
#ifdef CAMERAAPP_CAPI_V2_ADV
    // If we are in burst mode and need to increase capture limit,
    // do it now. If we have just decreased the capture limit,
    // we just cancel once the limit is met.
    else if( iAdvancedSettings
          && iAdvancedSettings->DriveMode() == CAS::EDriveModeBurst )
      {
      const TInt cameraLimit( iAdvancedSettings->BurstImages() );

      PRINT1( _L("Camera <> CCamCameraController ... Camera capture limit: %d"), cameraLimit         );
      PRINT1( _L("Camera <> CCamCameraController ... Local  capture limit: %d"), iInfo.iCaptureLimit );

      if( cameraLimit < iInfo.iCaptureLimit )
        {
        TInt startedCaptures = Max( iInfo.iSnapshotCount, iInfo.iCaptureCount );
        iInfo.iCaptureLimit  = Max( iInfo.iCaptureLimit, startedCaptures+1 );

        PRINT1( _L("Camera <> CCamCameraController ... Pending capture limit increase, calling SetBurstImages(%d).."), iInfo.iCaptureLimit );
        iAdvancedSettings->SetBurstImages( iInfo.iCaptureLimit );
        }
      }
#endif // CAMERAAPP_CAPI_V2_ADV
    // Normal, single capture.
    else
      {
      }

    // See HandleCallbackEvent for comments.
    // ClearFlags( iInfo.iBusy, ECamBusySingle );

    NotifyObservers( KErrNone,
                     ECamCameraEventImageData,
                     ECamCameraEventClassImage,
                     share );
    }
  // -------------------------------------------------------
  // Bitmap format case. Need to encode first.
  else if( iAlternateFormatInUse )
    {
    PRINT( _L("Camera <> Bitmap alternative capture format in use..") );
    // NOTE: We get another callback with encoded image data
    //       once encoder has done the conversion.
    PRINT( _L("Camera <> CCamCameraController ... Starting jpeg encoding..") );
    if( !iEncoder ) iEncoder = CCamImageEncoder::NewL( *this );

    // Encoder needs to call Reserve
    iEncoder->StartConversionL( share );
    }
  // -------------------------------------------------------
  // Encoded data missing although expected.
  else
    {
    User::Leave( dataStatus );
    }
  // -----------------------------------------------------
  CleanupStack::PopAndDestroy(); // share->Release();
  share = NULL;

  PRINT( _L("Camera <= CCamCameraController::HandleImageCaptureEventL") );
  }


// ---------------------------------------------------------------------------
// HandleSnapshotEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleSnapshotEvent( TInt aStatus )
  {
  PRINT( _L("Camera => CCamCameraController::HandleSnapshotEvent") );

  // Check that we are in right state to handle snapshot.
  // CCameraSnapshot sends atleast event with KErrCancel status
  // when releasing the camera.
  TBool videoSsExpected( IsFlagOn( iInfo.iState, ECamVideoOn ) );
  TBool imageSsExpected( IsFlagOn( iInfo.iState, ECamImageOn )
                      && ECamCaptureOn        == iInfo.iCaptureState
                      && iInfo.iSnapshotCount <  iInfo.iCaptureLimit );

  if(
#ifdef CAMERAAPP_CAE_FIX
      ECamModeChangeInactive == iModeChange &&
#endif
      (videoSsExpected||imageSsExpected)
    )
    {
    CAMERAAPP_PERF_CONTROLLER_STOP( ECamRequestSsStart );

    // -----------------------------------------------------
    // Try to get snapshot data
    if( KErrNone == aStatus )
      {
      TRAP( aStatus, HandleSnapshotEventL() );
      PRINT1( _L("Camera <> CCamCameraController ... status after handling snapshot data: %d"), aStatus );
      }

    // -----------------------------------------------------
    // If snapshot was received and notified ok,
    // check if capture ended.
    if( KErrNone == aStatus )
      {
      // If we have needed amount of snapshots and images, end capture.
      if( iInfo.iCaptureCount  >= iInfo.iCaptureLimit
       && iInfo.iSnapshotCount >= iInfo.iCaptureLimit )
        {
        PRINT( _L("Camera <> CCamCameraController ... Setting capture state to OFF..") );
        iInfo.iCaptureState = ECamCaptureOff;
        }
      }
    // -----------------------------------------------------
    // If any error, stop capture.
    else
      {
      PRINT( _L("Camera <> CCamCameraController ... error encountered, notify and set capture state off..") );
      NotifyObservers( aStatus,
                       ECamCameraEventSsReady,
                       ECamCameraEventClassSsData );
      iInfo.iCaptureState = ECamCaptureOff;
      }

    // -----------------------------------------------------
    // Check if we have just completed the capture.
    // Notify observers if so.
    if( imageSsExpected
     && ECamCaptureOff == iInfo.iCaptureState )
      {
      // See HandleCallbackEvent for comments.
      ClearFlags( iInfo.iBusy, ECamBusySingle );

      TInt fullCaptures( Min( iInfo.iCaptureCount, iInfo.iSnapshotCount ) );
      NotifyObservers( aStatus,
                       ECamCameraEventImageStop,
                       ECamCameraEventClassImage,
                       &fullCaptures );
      }
    // -----------------------------------------------------
    }
  else
    {
    // Just retrieve and release camera buffer if snapshot is not expected
    TRAP_IGNORE( HandleSnapshotEventL( ETrue ) );

    if( iInfo.iSnapshotCount > iInfo.iCaptureLimit )
      {
      // For burst stopping:
      //   We have more snapshots already than requested..
      //   All needed captures have to have started,
      //   so we can call cancel here.
      // Note:
      //   Cannot use ">=" as last image might not be provided then.
      //
      PRINT( _L("Camera <> CCamCameraController ... Snapshot limit passed, calling CancelCaptureImage..") );
      iCamera->CancelCaptureImage();
      }

    PRINT( _L("Camera <> CCamCameraController ... [WARNING] Snapshot ignored!") );
    iInfo.PrintInfo();
    }

  PRINT( _L("Camera <= CCamCameraController::HandleSnapshotEvent") );
  }


// ---------------------------------------------------------------------------
// HandleSnapshotEventL
//
// Helper method for leaving part of HandleSnapshotEvent.
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleSnapshotEventL( TBool aIgnore )
  {
   PRINT1( _L("Camera => CCamCameraController::HandleSnapshotEventL ignore %d"),aIgnore);
  __ASSERT_DEBUG( iSnapshotProvider, Panic( ECamCameraControllerCorrupt ) );
  RArray<TInt> temp;
  CleanupClosePushL( temp );
  MCameraBuffer* buffer( NULL );

    buffer = &iSnapshotProvider->SnapshotDataL( temp );

  TInt firstImageIndex = temp.Find( 0 );
  CleanupStack::PopAndDestroy(); // temp.Close()

  if( !aIgnore )
      {

      CleanupStack::PushL( TCleanupItem( CameraBufferCleanup, buffer ) );
      CFbsBitmap& snapshot = buffer->BitmapL( firstImageIndex );
      PRINT2( _L("Camera <> CCamCameraController ... snapshot size: (%dx%d)"),
              snapshot.SizeInPixels().iWidth, snapshot.SizeInPixels().iHeight );

      // Increase received snapshots count.
      // Needed in burst drive mode.
      iInfo.iSnapshotCount++;
      PRINT1( _L("Camera <> CCamCameraController ... Incremented snapshot counter to: %d"), iInfo.iSnapshotCount );

      // No leaving code after notification as otherwise
      // HandleSnapshotEvent calling this method will notify clients again.
      NotifyObservers( KErrNone,
              ECamCameraEventSsReady,
              ECamCameraEventClassSsData,
              &snapshot );

      CleanupStack::PopAndDestroy(); // buffer->Release()
      }
  else
      {
      ReleaseAndNull(buffer);
      }
  PRINT( _L("Camera <= CCamCameraController::HandleSnapshotEventL"));
  }




// ---------------------------------------------------------------------------
// HandleVideoInitEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleVideoEvent( const TCamCameraEventId& aEventId,
                                              TInt               aStatus   )
  {
  PRINT2( _L("Camera => CCamCameraController::HandleVideoEvent, status:%d, event[%s]"),
          aStatus,
          KCamCameraEventNames[aEventId] );

  // Only video stop of these events may come without our explicit request.
/*
  if( ECamCameraEventVideoStop != aEventId )
    {
    if( ECamBusyOff != iInfo.iBusy )
      Panic( ECamCameraControllerCorrupt );
    }
*/

  switch( aEventId )
    {
    // -----------------------------------------------------
    case ECamCameraEventVideoInit:
      // Some settings repeat this event if CCaeEngine in use.
      if( !IsFlagOn( iInfo.iBusy, ECamBusySetting ) )
        {
        CAMERAAPP_PERF_CONTROLLER_STOP( ECamRequestVideoInit );
        if( KErrNone == aStatus ) SetFlags  ( iInfo.iState, ECamVideoOn );
        else                      ClearFlags( iInfo.iState, ECamVideoOn );
        }
#ifdef CAMERAAPP_CAPI_V2_ADV
        TRAP_IGNORE( GetAdvancedSettingsInfoL() );
#endif
#ifdef CAMERAAPP_CAE_FIX
      if( ECamModeChangeImage2Video == iModeChange )
        {
        iModeChangeStatus = aStatus;
        iActive->IssueRequest();
        return;
        }
#endif // CAMERAAPP_CAE_FIX
      break;
    // -----------------------------------------------------
    case ECamCameraEventVideoStart:
      if( KErrNone == aStatus )
        {
        iInfo.iCaptureState = ECamCaptureOn;
        }
      break;
    // -----------------------------------------------------
    case ECamCameraEventVideoPause:
      if( KErrNone == aStatus )
        {
        iInfo.iCaptureState = ECamCapturePaused;
        }
      else
        {
        // Problem with pausing.
        // Keep our capture state unchanged.
        }
      break;
    // -----------------------------------------------------
    case ECamCameraEventVideoStop:
      // Even if error we have done what we can for stopping.
      // Update internal capture state anyway.
      iInfo.iCaptureState = ECamCaptureOff;
      // We may receive this event from CCaeEngine, even though we have not
      // issued a request to stop the video recording. If e.g. there is not
      // enough space in the disk to continue recording, this event is
      // generated without explicit request.
      //
      // We must not continue any pending operations if this event is not
      // a response to our stop request.
      // HandleCallbackEvent takes care of that.
      break;
    // -----------------------------------------------------
    case ECamCameraEventVideoAsyncStop:
      PRINT( _L("Camera <> CCamCameraController::HandleVideoEvent case ECamCameraEventVideoAsyncStop") );
      // Do not change state, just play sound later
      break;
    // -----------------------------------------------------
    default:
      {
      Panic( ECamCameraControllerCorrupt );
      break;
      }
    // -----------------------------------------------------
    }

  HandleCallbackEvent( aStatus,
                       aEventId,
                       ECamCameraEventClassVideo );
  PRINT( _L("Camera <= CCamCameraController::HandleVideoEvent") );
  }


#ifdef CAMERAAPP_CAE_FOR_VIDEO
// ---------------------------------------------------------------------------
// HandleVideoTimeEvent
// ---------------------------------------------------------------------------
//
void
CCamCameraController
::HandleVideoTimeEvent( TInt aStatus,
                        TTimeIntervalMicroSeconds aTimeElapsed,
                        TTimeIntervalMicroSeconds aTimeRemaining )
  {
  // If capture has already stopped, we don't echo this to our observers.
  if( ECamCaptureOff != iInfo.iCaptureState )
    {
    iVideoTimes.iTimeElapsed   = aTimeElapsed.Int64();
    iVideoTimes.iTimeRemaining = aTimeRemaining.Int64();

    NotifyObservers( aStatus,
                     ECamCameraEventVideoTimes,
                     ECamCameraEventClassVideoTimes,
                     &iVideoTimes );
    }
  }
#endif

// ---------------------------------------------------------------------------
// HandleAutoFocusEvent
//
// ---------------------------------------------------------------------------
//
void
CCamCameraController::HandleAutoFocusEvent( TInt aStatus, const TUid& aEventUid )
  {
  PRINT1( _L("Camera => CCamCameraController::HandleAutoFocusEvent, status: %d"), aStatus );

 if( IsFlagOn( iInfo.iState, ECamVideoOn ) && ECamCaptureOn == iInfo.iCaptureState )
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi && !appUi->AppController().UiConfigManagerPtr()->IsContinuosAutofocusSupported() )
        {
        // Autofocus events are not need anymore, if video recording is started already.
        PRINT( _L("Camera <= CCamCameraController::HandleAutoFocusEvent - ignore") );
        return;
        }
    }

  TBool proceed = EFalse;

  // -------------------------------------------------------
  // Check the event
  if( KUidECamEventCameraSettingsOptimalFocus == aEventUid )
    {
    PRINT( _L("Camera <> CCamCameraController: event KUidECamEventCameraSettingsOptimalFocus") );
    if( !iAfHyperfocalPending )
      {
      proceed = ETrue;
      iAfInProgress = EFalse;
      iFirstAfEventReceived = EFalse;
      }
    }
  else if( KUidECamEventCameraSettingAutoFocusType2 == aEventUid )
    {
    PRINT( _L("Camera <> CCamCameraController: event KUidECamEventCameraSettingsAutoFocusType2") );
    // If AF started and canceled before finishing
    if( iAfInProgress && iAfHyperfocalPending && iFirstAfEventReceived )
      {
      // continue to set focus to hyperfocal
      proceed = ETrue;
      }
    if( !iFirstAfEventReceived )
      {
      // For the first time, this event means that the autofocusing has been started
      iFirstAfEventReceived = ETrue;
      PRINT( _L("Camera <> CCamCameraController: event KUidECamEventCameraSettingAutoFocusType2 - first time, set iFirstAfEventReceived ") );
      }
    else
      {
      iFirstAfEventReceived = EFalse;
      PRINT( _L("Camera <> CCamCameraController: event KUidECamEventCameraSettingAutoFocusType2 - second time") );
      }
    }
  else
    {
    // No other event acceptable here..
    __ASSERT_DEBUG( EFalse, Panic( ECamCameraControllerCorrupt ) );
    }
  // -------------------------------------------------------
  // Proceed if right event received
  if( proceed )
    {
    TCamCameraEventId  event = ( KErrNone == aStatus )
                             ? ECamCameraEventAutofocusSuccessful
                             : ECamCameraEventAutofocusFailed;
    if( iAfHyperfocalPending )
      {
      PRINT( _L("Camera <> Cancelled active autofocus request.") );
      PRINT( _L("Camera <> Setting focus range to hyperfocal, no event to observers yet") );
      iAfHyperfocalPending = EFalse;
      iAfInProgress = EFalse;
      TRAP_IGNORE( ProcessAutofocusRequestL( ECamRequestCancelAutofocus ) );
      }
    else
      {
      HandleCallbackEvent( KErrNone,
                           event,
                           ECamCameraEventClassAutofocus );
      }

    }
  else if(iFirstAfEventReceived && (aStatus != KErrNone) )
    {
    //There might have been error when first event was received
    //and no further autofocus events would occur.
    iFirstAfEventReceived = EFalse;
    iAfInProgress = EFalse;

    if( iAfHyperfocalPending )
      {
      PRINT( _L("Camera <> Cancelled active autofocus request.") );
      PRINT( _L("Camera <> Set focus range to hyperfocal, no event to observers yet") );
      iAfHyperfocalPending = EFalse;
      TRAP_IGNORE( ProcessAutofocusRequestL( ECamRequestCancelAutofocus ) );
      }
    else
      {
      HandleCallbackEvent( KErrNone,
                           ECamCameraEventAutofocusFailed,
                           ECamCameraEventClassAutofocus );
      }
    }
  // -------------------------------------------------------
  PRINT( _L("Camera <= CCamCameraController::HandleAutoFocusEvent") );
  }



// ---------------------------------------------------------------------------
//
// HandleFlashStatusEvent
//
// Helper method to handle flash status updates.
//
// ---------------------------------------------------------------------------
void
CCamCameraController::HandleFlashStatusEvent( TInt                     aStatus,
                                              const TCamCameraEventId& aEventId )
  {
  PRINT2( _L("Camera => CCamCameraController::HandleFlashStatusEvent, event[%s] status:%d"), KCamCameraEventNames[aEventId], aStatus );
  NotifyObservers( aStatus,
                   aEventId,
                   EventClass( aEventId ) );
  PRINT ( _L("Camera <= CCamCameraController::HandleFlashStatusEvent" ) );
  }



// ---------------------------------------------------------------------------
// HandleCallbackEvent
//
// Helper method to handle CCamera callback events.
// Notifies observers and checks if we should proceed with the sequence.
// ---------------------------------------------------------------------------
//
void
CCamCameraController
::HandleCallbackEvent(       TInt                    aStatus,
                       const TCamCameraEventId&      aEventId,
                       const TCamCameraEventClassId& aEventClass,
                             TAny*                   aEventData /*= NULL*/ )
  {
  PRINT1( _L("Camera => CCamCameraController::HandleCallbackEvent, event[%s]"),
          KCamCameraEventNames[aEventId] );
  // -------------------------------------------------------
  // Clear single request busy flag before notification to observers,
  // in case client wishes to issue new request during the notification.
  // Sequence busy flag can only be cleared in EndSequence
  ClearFlags( iInfo.iBusy, ECamBusySingle );

  // Observers might issue a new sequence during notification.
  // We need to decide if sequence needs to continue here
  // based on the status *before* notification.
  TBool proceedSequence = EFalse;
  TBool notify          = ETrue;

  // -------------------------------------------------------
  // Continue if:
  // 1) Processing sequence is ongoing.
  // 2) The received event is exact response to our request.
  //    There are events like "video stopped" or "reserve lost"
  //    that may be received without our associated request.
  //    In that case we must not proceed sequence processing now.
  // 3) In case that aStatus is KErrInUse and the current event is
  //    Reserve, it means reserve failed for some reason
  if( IsFlagOn( iInfo.iBusy, ECamBusySequence ) ) // 1
    {
    PRINT( _L("Camera <> Sequence is active") );
    PRINT2( _L("Camera <> seq index: %d, seq array len: %d"), iSequenceIndex, iSequenceArray.Count() );
    if( KErrNone != aStatus )
        {

        if ( KErrInUse == aStatus               // 3
            && aEventId == ECamCameraEventReserveGain
            && iSequenceIndex >= 0
            && Request2Event( iSequenceArray[iSequenceIndex] ) == ECamCameraEventReserveGain
            && iReserveTryAgainCount > 0 )
            {
            // Handle here only if reserve gain with error is part of a sequence and try again count is not zero

            iReserveTryAgainCount--;
            User::After( KCamReserveTryAgainWaitInterval );

            // decrease index, to get the same request handled again
            iSequenceIndex--;
            notify          = EFalse;
            proceedSequence = ETrue;
            PRINT1( _L( "Camera <> CCamCameraController::HandleCallbackEvent - %d reserve try again left" ), iReserveTryAgainCount );
            }
        else if(KErrBadName == aStatus &&
                iSequenceIndex >= 0 &&   
                Request2Event( iSequenceArray[iSequenceIndex] ) == aEventId)  
            {
            aStatus         = KErrNone;
            notify          = ETrue;
            proceedSequence = ETrue;
            //EndSequence( aStatus );      
            }
        else 
            {

            notify          = EFalse;
            proceedSequence = EFalse;
            EndSequence( aStatus );

            }

        }
    else if( iSequenceIndex >= 0 &&  // Sequence has started
             Request2Event( iSequenceArray[iSequenceIndex] ) == aEventId ) // 2
        {
        notify          = ETrue;
        proceedSequence = ETrue;
        }
    else
        {
         // Not the event we need yet.
        }

    }
  // -------------------------------------------------------
  // Setting sequence ongoing
  else if( IsFlagOn( iInfo.iBusy, ECamBusySetting )
           && iSettingIndex >= 0 )
    {
    PRINT( _L("Camera <> Settings change ongoing..") );

    // Default is no notification
    if( aEventClass != ECamCameraEventClassAutofocus )
      {
      notify = EFalse;
	  }
    if( KErrNone != aStatus )
      {
      PRINT( _L("Camera <> error in, end settings sequence!") );
      proceedSequence = EFalse;
      EndSequence( aStatus ); // does one notification
      }
    else if( ECamCameraEventSettingsSingle == aEventId
          && aEventData
          && iSettingArray[iSettingIndex] == *( static_cast<TInt*>( aEventData ) )
           )
      {
      // We have checked already in HandleEvent that this is
      // the right event for the setting we wait to finish.
      // Notification can be sent with the given data.
      PRINT( _L("Camera <> One setting finished, continue sequence") );
      proceedSequence = ETrue;
      }
    // Video init event repeated when filename or max file size set
    // after init already done
    else if( IsFlagOn( iInfo.iState, ECamVideoOn )
          && ECamCameraEventVideoInit == aEventId
          && ( ECameraSettingFileName    == iSettingArray[iSettingIndex]
            || ECameraSettingFileMaxSize == iSettingArray[iSettingIndex]
            || ECameraSettingAudioMute   == iSettingArray[iSettingIndex] ) )
      {
      PRINT( _L("Camera <> One setting finished, continue sequence") );
      proceedSequence = ETrue;
      }
    else if( ECamCameraEventVideoStop == aEventId || ECamCameraEventVideoAsyncStop == aEventId  )
      {
      // Video stop might come whenever.
      // Need to notify client.
      notify = ETrue;
      }
    else
      {
      // Not event for us yet
      }
    }
  // -------------------------------------------------------
  else
    {
    // Single request or no request sent
    }
  // -------------------------------------------------------


  // -------------------------------------------------------
  // Notify the observers with data from the event.
  if( notify )
    {
    NotifyObservers( aStatus, aEventId, aEventClass, aEventData );
    }

  // -------------------------------------------------------
  // If this is part of sequence, continue processing.
  if( proceedSequence )
    {
    PRINT ( _L("Camera <> proceeding with the sequence") );
    __ASSERT_DEBUG( iActive, Panic( ECamCameraControllerCorrupt ) );
    iActive->IssueRequest();
    }
  // -------------------------------------------------------
  PRINT ( _L("Camera <= CCamCameraController::HandleCallbackEvent") );
  }


// ===========================================================================

// ---------------------------------------------------------------------------
// Notify all our observers of an event.
// ---------------------------------------------------------------------------
//
void
CCamCameraController
::NotifyObservers( TInt                   aStatus,
                   TCamCameraEventId      aEventId,
                   TCamCameraEventClassId aEventClass,
                   TAny*                  aEventData /*=NULL*/ )
  {
//  PRINT ( _L("Camera => CCamCameraController::NotifyObservers") );
//  PRINT3( _L("Camera <> status:% 3d event:% 3d class:%032b"), aStatus, aEventId, aEventClass );

  // NOTE:
  //   We might skip an observer, if during the notification some observer
  //   is removed from the array (from smaller index than current one).
  //   This is a commmon problem for all observable classes in this application.
  for( TInt i = 0; i < iObservers.Count(); i++ )
    {
    // Observer defines which events it is interested in.
//    PRINT1( _L("Camera <>          observer interest:%032b"), iObserverInterests[i] );

    if( iObserverInterests[i] & aEventClass )
      {
      TRAP_IGNORE( iObservers[i]->HandleCameraEventL( aStatus, aEventId, aEventData ) );
      }
    }
//  PRINT ( _L("Camera <= CCamCameraController::NotifyObservers") );
  }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ReleaseCurrentCamera()
  {
  PRINT( _L("Camera => CCamCameraController::ReleaseCurrentCamera") );
  // Cancel any outstanding sequence.
  if( IsFlagOn( iInfo.iBusy, ECamBusySequence )
#ifdef CAMERAAPP_CAE_FIX
   // Mode change calls this method possibly in the middle
   // of request sequence. Must not try to cancel then.
   && ECamModeChangeInactive == iModeChange
#endif
    )
    {
    CancelSequence();
    }

  if( IsFlagOn( iInfo.iState, ECamReserved ) )
    {
    // Releases viewfinder and snapshot, cancels capture,
    // powers off and releases CCamera if needed.
    ProcessControlShutdownRequest( ECamRequestRelease );
    }

  // Custom interfaces (not owned)
  ReleaseCustomInterfaces();

  delete iEncoder;            iEncoder           = NULL;
#ifdef CAMERAAPP_CAE_FOR_VIDEO
  delete iCaeEngine;          iCaeEngine         = NULL;
#endif // CAMERAAPP_CAE_FOR_VIDEO

#ifdef CAMERAAPP_CAPI_V2_ADV
  delete iAdvancedSettings;   iAdvancedSettings  = NULL;
  iAdvancedSettingInfo.Reset();
#endif
#ifdef CAMERAAPP_CAPI_V2_DVF
  delete iDirectViewfinder;   iDirectViewfinder  = NULL;
#endif
#ifdef CAMERAAPP_CAPI_V2_OL
  delete iOverlayProvider;    iOverlayProvider   = NULL;
#endif
#ifdef CAMERAAPP_CAPI_V2_HG
  delete iHistogramProvider;  iHistogramProvider = NULL;
#endif
#ifdef CAMERAAPP_CAPI_V2_IP
  delete iImageProcessor;     iImageProcessor    = NULL;
#endif
  delete iSnapshotProvider;   iSnapshotProvider  = NULL;

  delete iCamera;             iCamera            = NULL;

#ifdef CAMERAAPP_CAE_FIX
  // Must not reset busy flags if this method is called
  // because of mode switch.
  TBool saveBusyFlags = (ECamModeChangeInactive != iModeChange);
  iInfo.Reset( saveBusyFlags );
#else
  iInfo.Reset();
#endif
  iResolutionSupport.Reset();
  iInfo.iState = ECamIdle;

  PRINT( _L("Camera <= CCamCameraController::ReleaseCurrentCamera") );
  }


// ---------------------------------------------------------------------------
// ReleaseCustomInterfaces
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ReleaseCustomInterfaces()
  {
  iCustomInterfaceOrientation   = NULL;
  }

// ---------------------------------------------------------------------------
// CompareSize
// Compare two TSize instances
// ---------------------------------------------------------------------------
//
TBool CCamCameraController::CompareSize( const TSize& aA, const TSize& aB )
    {
    return ( aA.iWidth == aB.iWidth && aA.iHeight == aB.iHeight );
    }


// ---------------------------------------------------------------------------
// GetResolutionIndexL
//
// Get quality index for prepare with given format and size.
// If current info is not for the given format, update the
// supported resolutions info. After the support info is up to date,
// find the index. Return KErrNotFound if not supported size.
//
// For JPEG format we try two alternate bitmap formats, if
// the given resolution is not supported in JPEG format.
// If support is achieved by using one of the alternate formats,
// the format in use is returned in aFormat. When image is then
// later captured, we encode the bitmap to JPEG before handing
// it over to clients.
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController
::GetResolutionIndexL(       CCamera::TFormat& aFormat,
                       const TSize&            aSize   )
  {
  PRINT ( _L("Camera => CCamCameraController::GetResolutionIndexL") );

  if( iResolutionSupport.iForCameraFormat != aFormat
   || iResolutionSupport.iForCameraIndex  != iInfo.iCurrentCamera )
    {
    iAlternateFormatInUse = EFalse;

    // Reset all old resolution info
    iResolutionSupport.Reset();

    // Get the resolution info for current camera with given format.
    const TInt KResolutionCount( iCameraInfo.iNumImageSizesSupported );
    RArray<TSize>& resolutions( iResolutionSupport.iResolutions );

    resolutions.ReserveL( KResolutionCount );
    for( TInt i = 0; i < KResolutionCount; i++ )
      {
      // Reserve called, so should not leave.
      resolutions.AppendL( TSize() );
      iCamera->EnumerateCaptureSizes( resolutions[i], i, aFormat );
      }

    // Set these after successful filling of the array
    iResolutionSupport.iForCameraIndex  = iInfo.iCurrentCamera;
    iResolutionSupport.iForCameraFormat = aFormat;
    }

#ifdef _DEBUG
  PRINT ( _L("Camera <> ==================================================") );
  PRINT2( _L("Camera <> Size requested           : (%d,%d)"), aSize.iWidth, aSize.iHeight        );
  PRINT1( _L("Camera <> Formats supported        : %032b"  ), iCameraInfo.iImageFormatsSupported );
  PRINT1( _L("Camera <> Sizes listed for         : %032b"  ), aFormat                            );
  PRINT1( _L("Camera <> EFormatJpeg              : %032b"  ), CCamera::EFormatJpeg               );
  PRINT1( _L("Camera <> EFormatExif              : %032b"  ), CCamera::EFormatExif               );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor4K  : %032b"  ), CCamera::EFormatFbsBitmapColor4K   );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor64K : %032b"  ), CCamera::EFormatFbsBitmapColor64K  );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor16M : %032b"  ), CCamera::EFormatFbsBitmapColor16M  );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor16MU: %032b"  ), CCamera::EFormatFbsBitmapColor16MU );
  for( TInt i = 0; i < iResolutionSupport.iResolutions.Count(); i++ )
    {
    const TSize& size( iResolutionSupport.iResolutions[i] );
    PRINT3( _L("Camera <> Size (%d): (%d,%d)"), i, size.iWidth, size.iHeight );
    }
  PRINT ( _L("Camera <> ==================================================") );
#endif

  TIdentityRelation<TSize> compareFunc( CCamCameraController::CompareSize );
  TInt index = iResolutionSupport.iResolutions.Find( aSize, compareFunc );

  // Exception for JPEG format:
  //   If JPEG format is not supported, but bitmap format is,
  //   pretend to support the format anyway.
  //   When capturing, we encode the JPEG from the bitmap.
  if( CCamera::EFormatJpeg == aFormat
   && KErrNotFound         == index )
    {
    PRINT ( _L("Camera <> Jpeg not supported, trying bitmap format 1..") );
    aFormat = KCamJpegAlternativeFormat1;
    index   = GetResolutionIndexL( aFormat, aSize );

    if( KErrNotFound == index )
      {
      PRINT ( _L("Camera <> Jpeg not supported, trying bitmap format 2..") );
      aFormat = KCamJpegAlternativeFormat2;
      index   = GetResolutionIndexL( aFormat, aSize );
      }

    if( KErrNotFound != index )
      {
      iAlternateFormatInUse = ETrue;
      }
    else
      {
      // Could not find alternative.
      aFormat = CCamera::EFormatJpeg;
      iAlternateFormatInUse = EFalse;
      }
   }

  PRINT1( _L("Camera <= CCamCameraController::GetResolutionIndexL, return: %d"), index );
  return index;
  }


#ifdef CAMERAAPP_CAPI_V2_ADV
// ---------------------------------------------------------------------------
// GetAdvancedSettingsInfoL
//
// ---------------------------------------------------------------------------
//
void
CCamCameraController::GetAdvancedSettingsInfoL()
  {
  PRINT ( _L("Camera => CCamCameraController::GetAdvancedSettingsInfoL") );
  PRINT2( _L("Camera <> current camera:%d, latest info for:%d"),
          iInfo.iCurrentCamera,
          iAdvancedSettingInfo.iForCameraIndex );


    CheckNonNullL( iAdvancedSettings, KErrNotReady );
    iAdvancedSettingInfo.Reset();

    TInt error( KErrNone );
    CleanupStack::PushL( TCleanupItem( ResetInfo, &iAdvancedSettingInfo ) );

    // -----------------------------------------------------
    // Digital zoom steps support
    PRINT( _L("Camera <> Get zoom steps..") );
    if( IsFlagOn( iInfo.iState, ECamImageOn ) )
      {
      TPckgBuf<TCamParamsImage> params;
      iSettingProvider.ProvideCameraParamL( ECameraParamImage, &params );

      PRINT( _L("Camera <> Enumerating capture sizes..") );
      CCamera::TFormat format( params().iFormat );
      TInt             index ( GetResolutionIndexL( format, params().iSize ) );
      User::LeaveIfError( index );
      TBool isInfluencePossible; // Not used

      PRINT( _L("Camera <> Get zoom steps for image") );
      TRAP( error, iAdvancedSettings->GetDigitalZoomStepsForStillL (
                      iAdvancedSettingInfo.iDigitalZoomSupport,
                      iAdvancedSettingInfo.iDigitalZoomValueInfo,
                      index,
                      format,
                      isInfluencePossible  ) );

      if( KErrNotSupported != error ) User::LeaveIfError( error );
      }
    else if( IsFlagOn( iInfo.iState, ECamVideoOn ) )
      {
      PRINT( _L("Camera <> Get zoom steps for video") );
      TRAP( error, iAdvancedSettings->GetDigitalZoomStepsL(
                      iAdvancedSettingInfo.iDigitalZoomSupport,
                      iAdvancedSettingInfo.iDigitalZoomValueInfo ) );
      if( KErrNotSupported != error ) User::LeaveIfError( error );
      }

    // -----------------------------------------------------
    // EV steps multiplied by KECamFineResolutionFactor.
    PRINT( _L("Camera <> Get EV support..") );
    iAdvancedSettingInfo.iEvModesSupport =
        iAdvancedSettings->SupportedExposureModes();

    TRAP( error, iAdvancedSettings->GetExposureCompensationStepsL(
                    iAdvancedSettingInfo.iEvStepsSupport,
                    iAdvancedSettingInfo.iEvStepsValueInfo ) );
    // Ignore error if just not supported.
    // We check the support when setting is requested.
    if( KErrNotSupported != error ) User::LeaveIfError( error );

    // -----------------------------------------------------
    // ISO rates (.. 50, 100, 200, ..)
    PRINT( _L("Camera <> Get ISO rates..") );
    TRAP( error, iAdvancedSettings->GetSupportedIsoRatesL(
                    iAdvancedSettingInfo.iIsoRatesSupport ) );
    if( KErrNotSupported != error ) User::LeaveIfError( error );

    // -----------------------------------------------------
    // Stabilization
    PRINT( _L("Camera <> Get Stabilization info..") );
    iAdvancedSettingInfo.iStabilizationModeSupport =
        iAdvancedSettings->SupportedStabilizationModes();

    iAdvancedSettingInfo.iStabilizationEffectSupport =
        iAdvancedSettings->SupportedStabilizationEffects();

    iAdvancedSettingInfo.iStabilizationComplexitySupport =
        iAdvancedSettings->SupportedStabilizationComplexityValues();
    // -----------------------------------------------------

#ifdef CAMERAAPP_CAPI_V2_IP
    // Image Processor only for primary camera
    if( KPrimaryCameraIndex == iInfo.iCurrentCamera )
      {
      PRINT( _L("Camera <> Get image processing info..") );

      if( iImageProcessor != NULL )
        {
        // -----------------------------------------------------
        PRINT( _L("Camera <> Get sharpness support..") );
        TRAP( error, iImageProcessor->GetTransformationSupportedValuesL(
                        KUidECamEventImageProcessingAdjustSharpness,
                        iAdvancedSettingInfo.iSharpnessSupport,
                        iAdvancedSettingInfo.iSharpnessValueInfo ) );
        if( KErrNotSupported != error ) User::LeaveIfError( error );

        // -----------------------------------------------------
        PRINT( _L("Camera <> Get colour effect support..") );
        TRAP( error, iImageProcessor->GetTransformationSupportedValuesL(
                          KUidECamEventImageProcessingEffect,
                          iAdvancedSettingInfo.iColourEffectSupport,
                          iAdvancedSettingInfo.iColourEffectValueInfo ) );
        if( KErrNotSupported != error ) User::LeaveIfError( error );
        }
      else
        {
        PRINT( _L("Camera <> [WARNING] Image processing not supported") );
        }
      }
#endif // CAMERAAPP_CAPI_V2_IP
    // -----------------------------------------------------
    PRINT( _L("Camera <> ..done") );
    CleanupStack::Pop(); // Cleanup item

    // Set only after successfully gathering the data
    iAdvancedSettingInfo.iForCameraIndex = iInfo.iCurrentCamera;

#ifdef _DEBUG
    iAdvancedSettingInfo.PrintInfo();
#endif

  PRINT ( _L("Camera <= CCamCameraController::GetAdvancedSettingsInfoL") );
  }



// ---------------------------------------------------------------------------
// ResolveEvStep
//
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::ResolveEvStep( TInt aEvProposedStep ) const
  {
  PRINT1( _L("Camera => CCamCameraController::ResolveEvStep( %d )"), aEvProposedStep );

  TInt delta ( KMaxTInt );
  TInt delta1( 0        );
  TInt step  ( 0        );

  for( TInt i = 0; i < iAdvancedSettingInfo.iEvStepsSupport.Count() && delta != 0; i++ )
    {
    delta1 = Max( iAdvancedSettingInfo.iEvStepsSupport[i], aEvProposedStep )
           - Min( iAdvancedSettingInfo.iEvStepsSupport[i], aEvProposedStep );

    if( delta1 < delta )
      {
      delta = delta1;
      step  = iAdvancedSettingInfo.iEvStepsSupport[i];
      }
    }

  PRINT1( _L("Camera <= CCamCameraController::ResolveEvStep, return %d"), step );
  return step;
  }


// ---------------------------------------------------------------------------
// IsSupportedValue <<static>>
// ---------------------------------------------------------------------------
//
TBool
CCamCameraController::IsSupportedValue( const TInt&         aValue,
                                        const RArray<TInt>& aValueList,
                                        const TValueInfo&   aValueInfo )
  {
  TBool support( EFalse );

  switch( aValueInfo )
    {
    // -----------------------------------------------------
    case EBitField:
      {
      // Supported values are described by a bitfield.
      // Compare the support bitfield and the setting value.
      // Default value for these settings is zero (0) and
      // that is always supported.
      // Array: [0] bitfield of supported bits
      support = ( 1 <= aValueList.Count()
               && ( !aValue
                 || (aValue & aValueList[0]) // 0 always supported
                  )
                );
      break;
      }
    // -----------------------------------------------------
    case EDiscreteSteps:
      {
      // All supported steps are listed, look for this one.
      support = ( KErrNotFound != aValueList.Find( aValue ) );
      break;
      }
    // -----------------------------------------------------
    case EDiscreteRangeMinMaxStep:
      {
      // Support is described with evenly spaced steps between
      // min and max value. Step is given.
      // Array: [0] min supported value
      //        [1] max supported value
      //        [2] step between supported values
      support =
        ( 3      <= aValueList.Count()                 // We need 3 items.
       && 1      <=  aValueList[2]                     // 1      <= step
       && aValue >=  aValueList[0]                     // aValue >= min
       && aValue <=  aValueList[1]                     // aValue <= max
       && (aValue - aValueList[0])%aValueList[2] == 0  // aValue  = min + N*step
        );
      break;
      }
    // -----------------------------------------------------
    case EContinuousRangeMinMax:
      {
      // Support is described by min and max value,
      // continuous range of values within those two is supported.
      // Array: [0] min supported value
      //        [1] max supported value
      support =
        ( 2      <= aValueList.Count() // We need 2 items.
       && aValue >= aValueList[0]      // aValue >= min
       && aValue <= aValueList[1]      // aValue <= max
        );
      break;
      }
    // -----------------------------------------------------
    case ENotActive:
    default:
      // Not supported setting
      support = EFalse;
      break;
    // -----------------------------------------------------
    }

  return support;
  }
#endif // CAMERAAPP_CAPI_V2_ADV


// ---------------------------------------------------------------------------
// ResolveSnapshotFormat
//
// ---------------------------------------------------------------------------
//
CCamera::TFormat
CCamCameraController
::ResolveSnapshotFormatL( CCamera::TFormat aPreferredFormat ) const
  {
  PRINT ( _L("Camera => CCamCameraController::ResolveSnapshotFormatL") );

  CheckNonNullL( iSnapshotProvider, KErrNotReady );

  const TUint32    support = iSnapshotProvider->SupportedFormats();
  CCamera::TFormat final   = aPreferredFormat;

  // Return the preferred format if it is supported.
  // Otherwise return the "best" supported format.
  if      ( support & aPreferredFormat                   ) final = aPreferredFormat;
  else if ( support & CCamera::EFormatFbsBitmapColor16MU ) final = CCamera::EFormatFbsBitmapColor16MU;
  else if ( support & CCamera::EFormatFbsBitmapColor16M  ) final = CCamera::EFormatFbsBitmapColor16M;
  else if ( support & CCamera::EFormatFbsBitmapColor64K  ) final = CCamera::EFormatFbsBitmapColor64K;
  else if ( support & CCamera::EFormatFbsBitmapColor4K   ) final = CCamera::EFormatFbsBitmapColor4K;
  else
    {
    // No known acceptable format supported
    PRINT( _L("Camera <> CCamCameraController: No acceptable format available, LEAVE!") );
    User::Leave( KErrNotSupported );
    }

  PRINT1( _L("Camera <> Preferred format : %032b"), aPreferredFormat  );
  PRINT1( _L("Camera <> Supported formats: %032b"), support           );
  PRINT1( _L("Camera <> Selected  format : %032b"), final             );
  PRINT ( _L("Camera <= CCamCameraController::ResolveSnapshotFormatL") );
  return final;
  }



// ---------------------------------------------------------------------------
// Request2Event <<static>>
//
// Convert request id to event id
// ---------------------------------------------------------------------------
//
TCamCameraEventId
CCamCameraController::Request2Event( const TCamCameraRequestId& aType )
  {
  switch( aType )
    {
    // -----------------------------------------------------
    // Camera control
    case ECamRequestReserve:       return ECamCameraEventReserveGain;
    case ECamRequestRelease:       return ECamCameraEventReserveLose;
    case ECamRequestPowerOn:       return ECamCameraEventPowerOn;
    case ECamRequestPowerOff:      return ECamCameraEventPowerOff;
    // -----------------------------------------------------
    // Viewfinder
    case ECamRequestVfStart:       return ECamCameraEventVfStart;
    case ECamRequestVfStop:        return ECamCameraEventVfStop;
    case ECamRequestVfRelease:     return ECamCameraEventVfRelease;
    // -----------------------------------------------------
    // Snapshot
    case ECamRequestSsStart:        return ECamCameraEventSsStart;
    case ECamRequestSsStop:         return ECamCameraEventSsStop;
    case ECamRequestSsRelease:      return ECamCameraEventSsRelease;
    // -----------------------------------------------------
    // Still capture
    case ECamRequestImageInit:     return ECamCameraEventImageInit;
    case ECamRequestImageCapture:  return ECamCameraEventImageStart;
    case ECamRequestImageCancel:   return ECamCameraEventImageStop;
    case ECamRequestImageRelease:  return ECamCameraEventImageRelease;
    // -----------------------------------------------------
    // Video recording
    case ECamRequestVideoInit:     return ECamCameraEventVideoInit;
    case ECamRequestVideoStart:    return ECamCameraEventVideoStart;
    case ECamRequestVideoPause:    return ECamCameraEventVideoPause;
    case ECamRequestVideoStop:     return ECamCameraEventVideoStop;
    case ECamRequestVideoRelease:  return ECamCameraEventVideoRelease;
    // -----------------------------------------------------
    // Autofocus
    case ECamRequestStartAutofocus:   return ECamCameraEventStartAutofocus;
    case ECamRequestCancelAutofocus:  return ECamCameraEventCancelAutofocus;
    case ECamRequestSetAfRange:       return ECamCameraEventSetAfRange;
    // -----------------------------------------------------

    // Unrecognised
    default:
      {
      Panic( ECamCameraControllerUnknownRequest );
      return ECamCameraEventNone;
      }
    // -----------------------------------------------------
    }
  }


// ---------------------------------------------------------------------------
// EventClass <<static>>
//
// Get the event class for an event
// ---------------------------------------------------------------------------
//
TCamCameraEventClassId
CCamCameraController::EventClass( const TCamCameraEventId& aEventId )
  {
  switch( aEventId )
    {
    // -------------------------------------------------------
    case ECamCameraEventSequenceEnd:
    case ECamCameraEventReserveGain:
    case ECamCameraEventReserveLose:
    case ECamCameraEventPowerOn:
    case ECamCameraEventPowerOff:
      return ECamCameraEventClassBasicControl;
    // -------------------------------------------------------
    case ECamCameraEventVfStart:
    case ECamCameraEventVfStop:
    case ECamCameraEventVfRelease:
      return ECamCameraEventClassVfControl;
    // -------------------------------------------------------
    case ECamCameraEventVfFrameReady:
      return ECamCameraEventClassVfData;
    // -------------------------------------------------------
    case ECamCameraEventSsStart:
    case ECamCameraEventSsStop:
    case ECamCameraEventSsRelease:
      return ECamCameraEventClassSsControl;
    // -------------------------------------------------------
    case ECamCameraEventSsReady:
      return ECamCameraEventClassSsData;
    // -------------------------------------------------------
    case ECamCameraEventImageInit:
    case ECamCameraEventImageStart:
    case ECamCameraEventImageStop:
    case ECamCameraEventImageRelease:
    case ECamCameraEventImageData:
      return ECamCameraEventClassImage;
    // -------------------------------------------------------
    case ECamCameraEventVideoInit:
    case ECamCameraEventVideoStart:
    case ECamCameraEventVideoPause:
    case ECamCameraEventVideoStop:
    case ECamCameraEventVideoRelease:
      return ECamCameraEventClassVideo;
    // -------------------------------------------------------
    case ECamCameraEventVideoTimes:
      return ECamCameraEventClassVideoTimes;
    // -------------------------------------------------------
    case ECamCameraEventSettingsSingle:
    case ECamCameraEventSettingsDone:
      return ECamCameraEventClassSettings;
    // -------------------------------------------------------
    case ECamCameraEventStartAutofocus:
    case ECamCameraEventCancelAutofocus:
    case ECamCameraEventAutofocusSuccessful:
    case ECamCameraEventAutofocusFailed:
    case ECamCameraEventSetAfRange:
      return ECamCameraEventClassAutofocus;
    // -------------------------------------------------------
    case ECamCameraEventFlashReady:
    case ECamCameraEventFlashNotReady:
      return ECamCameraEventClassFlashStatus;
    // -------------------------------------------------------
    default:
      {
    #ifdef _DEBUG
      Panic( ECamCameraControllerCorrupt );
    #endif
      return ECamCameraEventClassAll;
      }
    }
  }



// ---------------------------------------------------------------------------
// HasCallback
// ---------------------------------------------------------------------------
//
TBool
CCamCameraController::HasCallback( const TCamCameraRequestId& aType )
  {
  TBool callback = EFalse;
  switch( aType )
    {
    // -----------------------------------------------------
    case ECamRequestReserve:      // ReserveComplete / KUidECamEventReserveComplete
    case ECamRequestPowerOn:      // PowerOnComplete / KUidECamEventPowerOnComplete

    case ECamRequestImageCapture: // ImageReady      / ImageBufferReady

#ifdef CAMERAAPP_CAE_FOR_VIDEO
    case ECamRequestVideoInit:    // McaeoVideoPrepareComplete
    case ECamRequestVideoStart:   // McaeoVideoRecordingOn
    case ECamRequestVideoPause:   // McaeoVideoRecordingPaused
    case ECamRequestVideoStop:    // McaeoVideoRecordingComplete
#endif
      callback = ETrue;
      break;
    // -----------------------------------------------------
    default:
      callback = EFalse;
      break;
    // -----------------------------------------------------
    }
  return callback;
  }

// ---------------------------------------------------------------------------
// CurrentSettingHasCallback
// ---------------------------------------------------------------------------
//
TBool
CCamCameraController::CurrentSettingHasCallback()
  {
  TBool callback( EFalse );
#ifdef CAMERAAPP_CAE_FOR_VIDEO
  if( IsFlagOn( iInfo.iState, ECamVideoOn ) )
    {
    switch( iSettingArray[iSettingIndex] )
      {
      case ECameraSettingFileName:
      case ECameraSettingFileMaxSize:
        callback = ETrue;
        break;
      default:
        break;
      }
    }
  else
    {
    // no callback if not prepared for video yet.
    }
#endif
  return callback;
  }

#ifdef CAMERAAPP_CAE_FOR_VIDEO
// ---------------------------------------------------------------------------
// NewCaeEngineL
// ---------------------------------------------------------------------------
//
CCaeEngine*
CCamCameraController::NewCaeEngineL( TInt aCameraIndex )
  {
  PRINT( _L("Camera => CCamCameraController::NewCaeEngineL") );

#ifndef FORCE_DUMMY_ENGINE
  CCaeEngine* engine = CCaeEngine::NewL( aCameraIndex );
#else
  (void)aCameraIndex; // remove compiler warning
  CCaeEngine* engine = CCameraappDummyEngine::NewL();
#endif

  engine->SetCamAppEngineObserver( *this );

  PRINT( _L("Camera <= CCamCameraController::NewCaeEngineL") );
  return engine;
  }
#endif

// ---------------------------------------------------------------------------
// NewCameraL
// ---------------------------------------------------------------------------
//
CAMERA*
CCamCameraController::NewCameraL( TInt aCameraIndex )
  {
  PRINT( _L("Camera => CCamCameraController::NewCameraL") );
  CAMERA* camera( NULL );
#ifdef CAMERAAPP_CAPI_V2
  camera = CAMERA::New2L( *this, aCameraIndex, KCameraClientPriority );
#else

  // Both v1 and v2 observer interface implemented
  // Need to cast to avoid ambiguous call.
  MCameraObserver* self( this );
  PRINT1( _L("Camera <> Give observer pointer: %d"), self );

  camera = CAMERA::NewL ( *self, aCameraIndex );

#endif // CAMERAAPP_CAPI_V2

  PRINT( _L("Camera <= CCamCameraController::NewCameraL") );
  return camera;
  }

// ---------------------------------------------------------------------------
// NewDuplicateCameraL
// ---------------------------------------------------------------------------
//
CAMERA*
CCamCameraController::NewDuplicateCameraL( TInt aCameraHandle )
  {
  PRINT1( _L("Camera => CCamCameraController::NewDuplicateCameraL( %d )"), aCameraHandle );
  CAMERA* camera( NULL );

#ifdef CAMERAAPP_CAPI_V2
  PRINT( _L("Camera <> call NewDuplicate2L..") );
  camera = CAMERA::NewDuplicate2L( *this, aCameraHandle );
#else
  MCameraObserver* self( this );
  PRINT( _L("Camera <> call NewDuplicateL..") );
  camera = CAMERA::NewDuplicateL ( *self, aCameraHandle );
#endif

  PRINT( _L("Camera <= CCamCameraController::NewCameraL") );
  return camera;
  }


// ===========================================================================
// Constructors


// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void
CCamCameraController::ConstructL( TInt aCameraIndex )
  {
  iActive = CCamCameraControllerActive::NewL( *this, KCamCallBackPriority );
  iIveRecoveryCount = KIveRecoveryCountMax;

#ifdef CAMERAAPP_PERFORMANCE_CONTROLLER
  iPerformanceLogger    = new (ELeave) CCamPerformanceLogger;
  iFirstVfFrameReceived = EFalse;
#endif

#ifdef CAMERAAPP_FLASH_SIMULATOR
  iFlashSimulator = CCamFlashSimulator::NewL( *this );
#endif

  SwitchCameraL( aCameraIndex );
  iAsyncVideoStopModeSupported = EFalse;

  }



// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
CCamCameraController
::CCamCameraController( MCamSettingProvider& aSettingProvider,
                        CCamAppController& aAppController )
  : iSequenceArray       ( KCamSequenceGranularity     ),
    iReserveTryAgainCount( KCamReserveTryAgainMaxCount ),
    iSettingProvider     ( aSettingProvider            ),
    iAppController       ( aAppController              )
  {
  }


// ===========================================================================
// Workaround for CCaeEngine with no support for CCamera sharing

#ifdef CAMERAAPP_CAE_FIX
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::ProceedModeSwitch()
  {
  PRINT1( _L("Camera => CCamCameraController::ProceedModeSwitch, status in:%d"), iModeChangeStatus );
  TInt proceed( EFalse );

  // Update only after here, so any events arriving
  // in the middle get ignored if wanted.
  ++iModeChangePhase;

  if( KErrNone == iModeChangeStatus )
    {
    TRAP( iModeChangeStatus, proceed = DoProceedModeSwitchL() );
    }

  if( KErrNone != iModeChangeStatus )
    {
    PRINT1( _L("Camera <> Mode switch error: %d"), iModeChangeStatus );
    iModeChange       = ECamModeChangeInactive;
    iModeChangePhase  = ECamModeChangePhase0;
    iModeChangeStatus = KErrNone;

    PRINT( _L("Camera <> Deleting engine..") );
    delete iCaeEngine;
    iCaeEngine = NULL;

    PRINT( _L("Camera <> Deleting camera..") );
    delete iCamera;
    iCamera = NULL;

    iInfo.Reset();

    PRINT( _L("Camera <> PANIC!!") );

    Panic( ECamCameraControllerUnrecovableError );
    }

  PRINT1( _L("Camera <= CCamCameraController::ProceedModeSwitch, more calls now:%d"), proceed );
  return proceed;
  }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::DoProceedModeSwitchL()
  {
  PRINT( _L("Camera => CCamCameraController::DoProceedModeSwitchL") );
  User::LeaveIfError( iModeChangeStatus );

  TBool callback = ETrue;

  // -------------------------------------------------------
  if( ECamModeChangeImage2Video == iModeChange )
    {
    PRINT( _L("Camera <> Changing mode image => video") );
    switch( iModeChangePhase )
      {
      // First two phases only usable if CAE v1 in use.
      // Must start from phase2 otherwise.
      // -------------------------------
      case ECamModeChangePhase0:
        {
        PRINT( _L("Camera <> Phase0: Release camera..") );
        TInt cameraIndex = iInfo.iCurrentCamera;
        ReleaseCurrentCamera();
        SwitchCameraL( cameraIndex );
        CompleteSwitchCameraL();
        callback = EFalse;
        // iModeChangePhase is incremented when iActive->RunL calls
        // ProcessNextRequestL for the next time.
        break;
        }
      // -------------------------------
      case ECamModeChangePhase1:
        {
        PRINT( _L("Camera <> Phase1: calling CCaeEngine::InitL..") );
        iCaeEngine->InitL();
        iCaeInUse = ETrue;
        // Continue mode change in callback
        PRINT( _L("Camera <> ..waiting for to complete") );
        break;
        }
      // -------------------------------
      case ECamModeChangePhase2:
        {
        PRINT( _L("Camera <> Phase2: Prepare video..") );
        PRINT( _L("Camera <> Call InitVideoRecorderL..") );
        iCaeEngine->InitVideoRecorderL();

        PRINT( _L("Camera <> Ask filename..") );
//        HBufC* filename( HBufC::NewLC( KMaxFileName ) );
//        TPtr   ptr     ( filename->Des()              );
//        iSettingProvider.ProvideCameraSettingL( ECameraSettingFileName, &ptr );
        _LIT( KTempFilename, "C:\\video.3gp" );
        TPtrC ptr;
        ptr.Set( KTempFilename() );
#pragma message( "Camera Controller: video filename hardcoded" )
        PRINT1( _L("Camera <> Set filename [%S]"), &ptr );
        iCaeEngine->SetVideoRecordingFileNameL( ptr );
//        CleanupStack::PopAndDestroy( filename );


        // Set max video clip size
        ProcessSettingL( ECameraSettingFileMaxSize );


        TPckgBuf<TCamParamsVideoCae> params;
        iSettingProvider.ProvideCameraParamL( ECameraParamVideoCae, &params );
        // The audioOn value is defined On==0 and Off==1, but the engine expects
        // ETrue if audio recording is On
        params().iAudioOn = ( ECamSettOn == params().iAudioOn )
        											? ETrue
        											: EFalse;
        PRINT( _L("Camera <> Call prepare..") );
        // McaeoVideoPrepareComplete will be called when prepare is ready.
        // The callback is allowed to come also *during* this call.
        iCaeEngine->PrepareVideoRecordingL( params().iFrameSize,
                                            params().iFrameRate,
                                            params().iVideoBitRate,
                                            params().iAudioOn,
                                            params().iAudioBitRate,
                                            params().iMimeType,
                                            params().iSupplier,
                                            params().iVideoType,
                                            params().iAudioType );

//        iCaeEngine->PrepareVideoRecordingL( 0 );

        break;
        }
      // -------------------------------
      case ECamModeChangePhase3:
        {
        PRINT( _L("Camera <> Phase3: Notifying..") );
        iModeChange      = ECamModeChangeInactive;
        iModeChangePhase = ECamModeChangePhaseIdle;
        callback         = EFalse;

        NotifyObservers( iModeChangeStatus,
                         ECamCameraEventVideoInit,
                         ECamCameraEventClassVideo );
        break;
        }
      // -------------------------------
      default:
        {
        Panic( ECamCameraControllerCorrupt );
        break;
        }
      // -------------------------------
      }
    }
  // -------------------------------------------------------
  else if( ECamModeChangeVideo2Image == iModeChange )
    {
    PRINT( _L("Camera <> Changing mode video => image") );
    switch( iModeChangePhase )
      {
      // -------------------------------
      case ECamModeChangePhase0:
        {
        PRINT( _L("Camera <> Phase0: Release camera..") );
        TInt cameraIndex = iInfo.iCurrentCamera;
        ReleaseCurrentCamera();
        SwitchCameraL( cameraIndex );
        CompleteSwitchCameraL();
        callback = EFalse;
        break;
        }
      // -------------------------------
      case ECamModeChangePhase1:
        {
        PRINT( _L("Camera <> Phase1: Reserve camera..") );
        iCaeInUse = EFalse;
        iCamera->Reserve();
        break;
        }
      // -------------------------------
      case ECamModeChangePhase2:
        {
        PRINT( _L("Camera <> Phase2: Power on..") );
        iCamera->PowerOn();
        break;
        }
      // -------------------------------
      case ECamModeChangePhase3:
        {
        PRINT( _L("Camera <> Phase3: Prepare image..") );
        TPckgBuf<TCamParamsImage> params;
        iSettingProvider.ProvideCameraParamL( ECameraParamImage, &params );

        // Query the supported resolutions for the selected format.
        TInt index = GetResolutionIndexL( params().iFormat, params().iSize );
        User::LeaveIfError( index );

        PRINT2( _L("Camera <> Image size: (%d, %d)"),
                  params().iSize.iWidth,
                  params().iSize.iHeight );
        iCamera->PrepareImageCaptureL( params().iFormat, index );
        iCamera->SetJpegQuality( params().iQualityFactor );

        SetFlags( iInfo.iState, ECamImageOn );

        callback  = EFalse;
        break;
        }
      // -------------------------------
      case ECamModeChangePhase4:
        {
        PRINT( _L("Camera <> Phase4: Notifying..") );
        iModeChange      = ECamModeChangeInactive;
        iModeChangePhase = ECamModeChangePhaseIdle;
        callback         = EFalse;

        NotifyObservers( iModeChangeStatus,
                         ECamCameraEventImageInit,
                         ECamCameraEventClassImage );

        break;
        }
      // -------------------------------
      default:
        {
        Panic( ECamCameraControllerCorrupt );
        break;
        }
      // -------------------------------
      }
    }
  // -------------------------------------------------------
  else
    {
    Panic( ECamCameraControllerCorrupt );
    }
  // -------------------------------------------------------

  PRINT1( _L("Camera <= CCamCameraController::DoProceedModeSwitchL, more calls now:%d"), !callback );
  return !callback;
  }
#endif // CAMERAAPP_CAE_FIX


/**
* CCamCameraController::RemainingVideoRecordingTime()
* Method to retrieve Remaining Recording time from CCaeEngine
* which is used in AppController and AppUi during Video Rec Operation
*/
TTimeIntervalMicroSeconds
CCamCameraController::RemainingVideoRecordingTime()
	{
	if( iCaeEngine )
  	return iCaeEngine->RemainingVideoRecordingTime();
  else
    return 0;
	}


// ===========================================================================
// Performance measurement related

#ifdef CAMERAAPP_PERFORMANCE_CONTROLLER

// ---------------------------------------------------------------------------
// PerformanceLogger()
//
// Return the performance logger instance.
// Used to record performance measurement data.
// ---------------------------------------------------------------------------
//
const CCamPerformanceLogger*
CCamCameraController::PerformanceLogger() const
  {
  return iPerformanceLogger;
  }

#endif // CAMERAAPP_PERFORMANCE_CONTROLLER



// ===========================================================================
// Debug prints

#ifdef _DEBUG
// ---------------------------------------------------------------------------
// PrintCameraInfo
// ---------------------------------------------------------------------------
//
void
CCamCameraController::PrintCameraInfo() const
  {
  PRINT ( _L("Camera <> ==================================================") );
  PRINT ( _L("Camera <> Camera info:") );
  PRINT3( _L("Camera <> Camera hw version[%d.%d.%d]"),
          iCameraInfo.iHardwareVersion.iMajor,
          iCameraInfo.iHardwareVersion.iMinor,
          iCameraInfo.iHardwareVersion.iBuild );
  PRINT3( _L("Camera <> Camera sw version[%d.%d.%d]"),
          iCameraInfo.iSoftwareVersion.iMajor,
          iCameraInfo.iSoftwareVersion.iMinor,
          iCameraInfo.iSoftwareVersion.iBuild );
  PRINT1( _L("Camera <> TCameraInfo.iMinZoom             : %d"), iCameraInfo.iMinZoom );
  PRINT1( _L("Camera <> TCameraInfo.iMaxZoom             : %d"), iCameraInfo.iMaxZoom );
  PRINT1( _L("Camera <> TCameraInfo.iMaxDigitalZoom      : %d"), iCameraInfo.iMaxDigitalZoom );
  PRINT1( _L("Camera <> TCameraInfo.iMinZoomFactor       : %f"), iCameraInfo.iMinZoomFactor );
  PRINT1( _L("Camera <> TCameraInfo.iMaxZoomFactor       : %f"), iCameraInfo.iMaxZoomFactor );
  PRINT1( _L("Camera <> TCameraInfo.iMaxDigitalZoomFactor: %f"), iCameraInfo.iMaxDigitalZoomFactor );
  PRINT ( _L("Camera <> ==================================================") );
  }


// ---------------------------------------------------------------------------
// PrintSnapshotInfo
// ---------------------------------------------------------------------------
//
void
CCamCameraController::PrintSnapshotInfo() const
  {
  PRINT ( _L("Camera <> ===========================================================") );
  PRINT ( _L("Camera <> Snapshot info"  ) );
  PRINT1( _L("Camera <> Formats supported (1st)  : %032b"  ), iSnapshotProvider->SupportedFormats() );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor4K  : %032b"  ), CCamera::EFormatFbsBitmapColor4K      );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor64K : %032b"  ), CCamera::EFormatFbsBitmapColor64K     );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor16M : %032b"  ), CCamera::EFormatFbsBitmapColor16M     );
  PRINT1( _L("Camera <> EFormatFbsBitmapColor16MU: %032b"  ), CCamera::EFormatFbsBitmapColor16MU    );
  PRINT ( _L("Camera <> ===========================================================")               );
  }

// ===========================================================================
#endif // _DEBUG

// ---------------------------------------------------------------------------
// CCamCameraController::SettingValueUpToDateL
// ---------------------------------------------------------------------------
//
TBool
CCamCameraController::SettingValueUpToDateL( const NCamCameraController::TCamCameraSettingId& aSettingId )
  {
  PRINT( _L("Camera => CCamCameraController::SettingValueUpToDate") );
  TBool upToDate = ETrue;

  switch( aSettingId )
    {
    case ECameraSettingFlash:
    case ECameraUserSceneSettingFlash:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingFlash") );

      // Supposed setting value from settings provider
      CCamera::TFlash flashSetting( CCamera::EFlashNone );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &flashSetting );

      // Real settings value from camera
      CCamera::TFlash flashValue( CCamera::EFlashNone );
      GetCameraSettingValueL( aSettingId, &flashValue );

      PRINT2( _L("Camera <> flashValue = %d, flashSetting = %d"), flashValue, flashSetting );
      upToDate = ( flashValue == flashSetting );
      break;
      }
    case ECameraSettingExposure:
    case ECameraUserSceneSettingExposure:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingExposure") );
      TPckgBuf<TCamSettingDataExposure> exposureSetting;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &exposureSetting );

      TPckgBuf<TCamSettingDataExposure> exposureValue;
      GetCameraSettingValueL( aSettingId, &exposureValue );

      if ( exposureSetting().iExposureMode != exposureValue().iExposureMode ||
           exposureSetting().iExposureStep != exposureValue().iExposureStep )
        {
        upToDate = EFalse;
        }

      break;
      }
    case ECameraSettingLightSensitivity:
    case ECameraUserSceneSettingLightSensitivity:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingLightSensitivity") );

      RArray<TInt> ISOarray;
      CleanupClosePushL( ISOarray );
#ifdef CAMERAAPP_CAPI_V2
      iAdvancedSettings->GetSupportedIsoRatesL( ISOarray );
#endif
      iSettingProvider.SetSupportedISORatesL( ISOarray );

      TInt* isoSetting = 0;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &isoSetting );
      CleanupStack::PopAndDestroy( &ISOarray );


      TInt* isoValue = 0;
      GetCameraSettingValueL( aSettingId, &isoValue );

      upToDate = ( isoSetting == isoValue );
      break;
      }
    case ECameraSettingWhiteBalance:
    case ECameraUserSceneSettingWhiteBalance:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingWhiteBalance") );

      TPckgBuf<TCamSettingDataWhiteBalance> wbSetting;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &wbSetting );

      TPckgBuf<TCamSettingDataWhiteBalance> wbValue;
      GetCameraSettingValueL( aSettingId, &wbValue );

      upToDate = ( wbSetting().iWhiteBalanceMode == wbValue().iWhiteBalanceMode );
      break;
      }
    case ECameraSettingColourEffect:
    case ECameraUserSceneSettingColourEffect:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingColourEffect") );

      CIP::TEffect effectSetting( CIP::EEffectNone );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &effectSetting );

      CIP::TEffect effectValue( CIP::EEffectNone );
      // Can't leave here or other settings won't be restored
      TRAP_IGNORE( GetCameraSettingValueL( aSettingId, &effectValue ) );

      upToDate = ( effectSetting == effectValue );
      break;
      }
    case ECameraSettingBrightness:
    case ECameraUserSceneSettingBrightness:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingBrightness") );
      CCamera::TBrightness brightnessSetting;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &brightnessSetting );

      CCamera::TBrightness brightnessValue;
      GetCameraSettingValueL( aSettingId, &brightnessValue );

      upToDate = ( brightnessSetting == brightnessValue );
      break;
      }
    case ECameraSettingContrast:
    case ECameraUserSceneSettingContrast:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingContrast") );
      CCamera::TContrast contrastSetting( CCamera::EContrastAuto );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &contrastSetting );

      CCamera::TContrast contrastValue( CCamera::EContrastAuto );
      GetCameraSettingValueL( aSettingId, &contrastValue );

      upToDate = ( contrastSetting == contrastValue );
      break;
      }
    case ECameraSettingSharpness:
    case ECameraUserSceneSettingSharpness:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingSharpness") );
      TInt sharpnessSetting( 0 );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &sharpnessSetting );

      TInt sharpnessValue( 0 );
      // Can't leave here or other settings won't be restored
      TRAP_IGNORE( GetCameraSettingValueL( aSettingId, &sharpnessValue ) );

      upToDate = ( sharpnessSetting == sharpnessValue );
      break;
      }
    case ECameraSettingDigitalZoom:
      {
      TInt zoomSetting( 0 );
      iSettingProvider.ProvideCameraSettingL( aSettingId, &zoomSetting );

      TInt zoomValue( 0 );
      GetCameraSettingValueL( aSettingId, &zoomValue );

      upToDate = ( zoomSetting == zoomValue );
      break;
      }
#ifdef CAMERAAPP_CAPI_V2
    case ECameraSettingStabilization:
      {
      PRINT( _L("Camera <> CCamCameraController::SettingValueUpToDate ECameraSettingStabilization") );

      TPckgBuf<TCamSettingDataStabilization> stabilizationSetting;
      iSettingProvider.ProvideCameraSettingL( aSettingId, &stabilizationSetting );

      TPckgBuf<TCamSettingDataStabilization> stabilizationValue;
      GetCameraSettingValueL( aSettingId, &stabilizationValue );
      if ( stabilizationSetting().iMode != stabilizationValue().iMode ||
              stabilizationSetting().iEffect != stabilizationValue().iEffect ||
              stabilizationSetting().iComplexity != stabilizationValue().iComplexity )
        {
        upToDate = EFalse;
        }
      break;
      }
    case ECameraSettingContAF:
      {
      TBool isContAFon( iAdvancedSettings->AutoFocusType() &
                        CAS::EAutoFocusTypeContinuous );
      if( IsFlagOn( iInfo.iState, ECamVideoOn ) )
        {
        TBool contAF( ETrue );
        iSettingProvider.ProvideCameraSettingL( aSettingId, &contAF );
        if( contAF != isContAFon )
          {
          upToDate = EFalse;
          }
        }
      else
        {
        //Do nothing

        }
      break;
      }
#endif
    default:
      {
      PRINT1( _L("CCamCameraController::SettingValueUpToDate - unknown setting id: %d"), aSettingId );
      }

    }

  PRINT1( _L("Camera <= CCamCameraController::SettingValueUpToDate returning %d"), upToDate );
  return upToDate;
  }



// ---------------------------------------------------------------------------
// CCamCameraController::SetViewfinderWindowHandle
// ---------------------------------------------------------------------------
//
void CCamCameraController::SetViewfinderWindowHandle( RWindowBase* aWindow )
    {
    PRINT( _L("Camera => CCamCameraController::SetViewfinderWindowHandle") );
    PRINT2( _L("Camera <> aWindow=0x%08x iViewfinderWindow=0x%08x "), aWindow, iViewfinderWindow );

    if ( aWindow != iViewfinderWindow &&
        iCamera &&
        iInfo.iVfState == ECamTriActive &&
        iInfo.iVfMode == ECamViewfinderDirect )
        {
        PRINT( _L("Camera <> viewfinder active and window handle changed, restarting viewfinder...") );
         iCamera->StopViewFinder();
        
        if ( iViewfinderWindow != NULL )
            {
            iViewfinderWindow = aWindow;
            // Use the same viewfinder position and size as for bitmap viewfinder
            TPckgBuf<TCamParamsVfBitmap> params;
            iSettingProvider.ProvideCameraParamL( ECameraParamVfBitmap, &params );
            
            CEikonEnv* env = CEikonEnv::Static();
    
              TInt orgPos = SetVfWindowOrdinal(); // Set visible
              iCamera->StartViewFinderDirectL(
                  env->WsSession(),
                  *env->ScreenDevice(),
                  *iViewfinderWindow,
                  params().iRect );
              (void) SetVfWindowOrdinal( orgPos ); // back to original
            }
        else
            {
            iCamera->StopViewFinder();
            iInfo.iVfState = ECamTriInactive;

            // restart viewfinder
       
		    iAppController.EnterViewfinderMode(iAppController.CurrentMode());
            }
        }
    iViewfinderWindow = aWindow;

    PRINT( _L("Camera <= CCamCameraController::SetViewfinderWindowHandle") );
    }

// ---------------------------------------------------------------------------
// CCamCameraController::ViewfinderWindowDeleted
// ---------------------------------------------------------------------------
//
void CCamCameraController::ViewfinderWindowDeleted( RWindowBase* aWindow )
    {
    PRINT( _L("Camera => CCamCameraController::ViewfinderWindowDeleted") );
    PRINT2( _L("Camera <> aWindow=0x%08x iViewfinderWindow=0x%08x "), aWindow, iViewfinderWindow );

    if ( aWindow == iViewfinderWindow )
        {
        if ( iCamera &&
            iInfo.iVfState == ECamTriActive &&
            iInfo.iVfMode == ECamViewfinderDirect )
            {
            PRINT( _L("Camera <> viewfinder active and window deleted, stopping viewfinder...") );
            iCamera->StopViewFinder();
            iInfo.iVfState = ECamTriInactive;
            }
        iViewfinderWindow = NULL;
        }

    PRINT( _L("Camera <= CCamCameraController::ViewfinderWindowDeleted") );
    }

// ---------------------------------------------------------------------------
// CCamCameraController::SetVfWindowOrdinal
// ---------------------------------------------------------------------------
//
TInt CCamCameraController::SetVfWindowOrdinal( TInt aOrdinalPosition )
    {
    TInt orgPos( KErrUnknown );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( iViewfinderWindow )
        {
        TInt cbaPos(0);
        TInt toolbarPos(0);
        CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
        orgPos = iViewfinderWindow->OrdinalPosition();
        if ( aOrdinalPosition != KErrUnknown)
            {
            // Set wanted ordinal position
            iViewfinderWindow->SetOrdinalPosition( aOrdinalPosition );
            }
        else if ( appUi )
            {
            // Find out other windows ordinal positions
            if ( cba )
                {
                cbaPos =  cba->DrawableWindow()->OrdinalPosition();
                }
            CAknToolbar* toolbar = appUi->CurrentFixedToolbar();
            if ( toolbar )
                {
                RDrawableWindow* toolbarwindow = toolbar->DrawableWindow();
                if ( toolbarwindow )
                    {
                    toolbarPos = toolbarwindow->OrdinalPosition();
                    }
                }
            // Calculate new viewfinder position,
            // just under fixed toolbar and CBA buttons
            TInt newPos = Max( toolbarPos, cbaPos ) + 1;
            // Use new position if viefinder window is not visible already
            iViewfinderWindow->SetOrdinalPosition( Min( orgPos, newPos ) );
            }
        }
    return orgPos;
    }

#ifdef CAMERAAPP_CAE_ERR_SIMULATION

// ---------------------------------------------------------------------------
// CCamCameraController::DelayedCaeCallback
// Static function called when the timer expires
// ---------------------------------------------------------------------------
//
TInt
CCamCameraController::DelayedCaeCallback( TAny* aController )
    {
    CCamCameraController* self = static_cast<CCamCameraController*>( aController );
    self->CallAppropriateCallback();

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCamCameraController::CallAppropriateCallback
// Helper function to call the appropriate callback handler.
// ---------------------------------------------------------------------------
//
void
CCamCameraController::CallAppropriateCallback( const TCamCameraEventId aEventId,
                                                     TInt              aStatus )
    {
    PRINT2( _L("Camera => CCamCameraController::CallAppropriateCallback, event:%d, status:%d"), aEventId, aStatus )

    switch ( iSimEventId )
        {
        case ECamCameraEventVideoInit:
        case ECamCameraEventVideoStart:  // fallthrough
        case ECamCameraEventVideoPause:  // fallthrough
        case ECamCameraEventVideoStop:
            if( aEventId == ECamCameraEventNone )
                {
                PRINT( _L("Camera <> CCamCameraController::CallAppropriateCallback - calling handler after delay") )
                HandleVideoEvent( iSimEventId, iSimStatus );
                }
            else
                {
                PRINT( _L("Camera <> CCamCameraController::CallAppropriateCallback - calling handler without delay") )
                HandleVideoEvent( aEventId, aStatus );
                }
            break;
        default:
            break;
        }

    // In case of being called by timer, stop and destro the timer
    if( aEventId == ECamCameraEventNone && iCallbackTimer )
        {
        iCallbackTimer->Cancel();
        delete iCallbackTimer;
        iCallbackTimer = NULL;
        }

    PRINT( _L("Camera <= CCamCameraController::CallAppropriateCallback") )
    }

#endif // CAMERAAPP_CAE_ERR_SIMULATION

// ---------------------------------------------------------------------------
// CCamCameraController::AsyncVideoStopModeSupported
// ---------------------------------------------------------------------------
//
TBool
CCamCameraController::AsyncVideoStopModeSupported()
  {
  PRINT1( _L("Camera <> CCamCameraController::AsyncVideoStopModeSupported = %d "), iAsyncVideoStopModeSupported );
  return( iAsyncVideoStopModeSupported );
  }

// ---------------------------------------------------------------------------
// IdleCallback <<static>>
// ---------------------------------------------------------------------------
//
// static
TInt CCamCameraController::IdleCallback( TAny* aSelf )
    {
    CCamCameraController* self( static_cast<CCamCameraController*>( aSelf ) );
    self->DoIveRecovery();

    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCamCameraController::DoIveRecovery
// ---------------------------------------------------------------------------
//
void CCamCameraController::DoIveRecovery()
    {
    PRINT( _L("Camera => CCamCameraController::DoIveRecovery") )
    if( iAppController.IsAppUiAvailable()
            && !( iAppController.IsInShutdownMode() || iAppController.CheckExitStatus() ) )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        __ASSERT_DEBUG(appUi, CamPanic(ECamPanicNullPointer));
		TVwsViewId activeView;
        TInt viewErr = appUi->GetActiveViewId( activeView );
        CCamViewBase* view = NULL;
        if( !viewErr )
            {
            view = static_cast<CCamViewBase*>( appUi->View( activeView.iViewUid ));
            }
        else
            {
            //There was an error when getting active view ID. Propably camera
            //application went to background. In that case just return because
            //camera resource should be released when going to background.
            return;
            }
		__ASSERT_DEBUG(view, CamPanic(ECamPanicNullPointer));
        if ( appUi->StandbyStatus() && appUi->IsRecoverableStatus() && view->IsInStandbyMode() ) 
            {
            PRINT( _L("Camera <> CCamCameraController::DoIveRecovery - Standby mode active, try to exit") )
            TRAP_IGNORE( appUi->HandleControllerEventL(  ECamEventCameraChanged,
                                                         KErrNone ) );
            }
        else
            {
            PRINT( _L("Camera <> CCamCameraController::DoIveRecovery - Start recovering from beginning") )
            iIveSequenceActive = EFalse;
            if( IsFlagOn( iInfo.iBusy, ECamBusySequence|ECamBusySetting ) )
                {
                if( iActive &&
                    iActive->IsActive() )
                    {
                    iActive->Cancel();
                    }
                iIveCancel = ETrue;
                EndSequence( KErrNone ); // Clears iIveRecoveryOngoing and iIveSequenceActive
                }
            NotifyObservers( KErrNone, ECamCameraEventIveRecover,
                             ECamCameraEventClassBasicControl );
            }
        iIveRecoveryCount--;
        iIveRecoveryOngoing = ETrue;
        iIveCancel = EFalse;
        }
    PRINT1( _L("Camera <= CCamCameraController::DoIveRecovery iIveRecoveryCount%d"),iIveRecoveryCount )
    }

// ---------------------------------------------------------------------------
// CCamCameraController::IsWaitingIveResources
// ---------------------------------------------------------------------------
//
TBool CCamCameraController::IsWaitingIveResources()
    {
    // ETrue if recovery is started, but not completed.
    // iIveRecoveryOngoing is set to false when last recovery command is executed
    return (iIdle && iIdle->IsActive()) ||
            iIveCancel || iIveRecoveryOngoing || iIveSequenceActive;
    }


// ---------------------------------------------------------------------------
// CCamCameraController::SetFaceTrackingL
// ---------------------------------------------------------------------------
//
void CCamCameraController::SetFaceTrackingL()
    {
    if( iCustomInterfaceFaceTracking &&
        KPrimaryCameraIndex == iInfo.iCurrentCamera )
      {
      // always enable FaceIndicators regardless of FT setting
      // to get reticule visible  
      iCustomInterfaceFaceTracking->EnableFaceIndicatorsL( ETrue );
      TBool ftOn( EFalse );
      iSettingProvider.ProvideCameraSettingL( ECameraSettingFacetracking, &ftOn );
      PRINT1( _L("Camera <> Set facetracking: %d"), ftOn )
      if( ( ftOn && !iCustomInterfaceFaceTracking->FaceTrackingOn() ) ||
          ( !ftOn && iCustomInterfaceFaceTracking->FaceTrackingOn() ) )
          {
          iCustomInterfaceFaceTracking->SetFaceTrackingL( ftOn );
          DirectRequestL( ECamRequestSetAfRange );
          }
      }
    }

// ---------------------------------------------------------------------------
// CCamCameraController::HintUseCase
// ---------------------------------------------------------------------------
//
void CCamCameraController::HintUseCaseL()
    {
    PRINT( _L("Camera => CCamCameraController::HintUseCaseL"))
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );    
    //videomode    
    if ( iCustomInterfaceUseCaseHint && appUi && 
       ( ( appUi->IsEmbedded() && appUi->TargetMode() == ECamControllerVideo ) || 
       ( !appUi->IsEmbedded() && iAppController.TargetMode() == ECamControllerVideo ) ) )
        {
        PRINT( _L("Camera <> HintUseCaseL VideoMode") );
        MCameraUseCaseHint::TVideoCodec codec = MCameraUseCaseHint::ECodecUnknown;
        MCameraUseCaseHint::TVideoProfile profile = MCameraUseCaseHint::EProfileUnknown;
        TPckgBuf<TCamParamsVideoCae> params;
        iSettingProvider.ProvideCameraParamL( ECameraParamVideoCae, &params );
        if( params().iVideoType == KCMRMimeTypeH264AVCBPL31 )
            {
            PRINT( _L("Camera <> HintUseCaseL VideoMode KCMRMimeTypeH264AVCBPL31") );    
            codec = MCameraUseCaseHint::ECodecH264;
            profile = MCameraUseCaseHint::EProfileH264BpL3_1;
            }
        else if( params().iVideoType == KCMRMimeTypeH263 )
            {
            PRINT( _L("Camera <> HintUseCaseL VideoMode KCMRMimeTypeH263") );    
            codec = MCameraUseCaseHint::ECodecH263;
            profile = MCameraUseCaseHint::EProfileH263P0L10;
            }
        else if( params().iVideoType == KCMRMimeTypeH264AVCBPL30 )
            {
            PRINT( _L("Camera <> HintUseCaseL VideoMode KCMRMimeTypeH264AVCBPL30") );    
            codec = MCameraUseCaseHint::ECodecH264;
            profile = MCameraUseCaseHint::EProfileH264BpL3;
            }            
        else if( params().iVideoType == KCMRMimeTypeMPEG4VSPL4A )
            {
            PRINT( _L("Camera <> HintUseCaseL VideoMode KCMRMimeTypeMPEG4VSPL4A") );    
            codec = MCameraUseCaseHint::ECodecMpeg4;
            profile = MCameraUseCaseHint::EProfileMPEG4SpL4a;     
            }
        else if( params().iVideoType == KCMRMimeTypeMPEG4VSPL3 )
            {
            PRINT( _L("Camera <> HintUseCaseL VideoMode KCMRMimeTypeMPEG4VSPL3") );    
            codec = MCameraUseCaseHint::ECodecMpeg4;
            profile = MCameraUseCaseHint::EProfileMPEG4SpL3;                        
            }
        else if( params().iVideoType == KCMRMimeTypeMPEG4VSPL2 )
            {
            PRINT( _L("Camera <> HintUseCaseL VideoMode KCMRMimeTypeMPEG4VSPL2") );    
            codec = MCameraUseCaseHint::ECodecMpeg4;
            profile = MCameraUseCaseHint::EProfileMPEG4SpL2;                        
            }                        
        else
            {
            PRINT( _L("Camera <> HintUseCaseL VideoMode not supported") );        
            //leave if type not supported
            User::Leave( KErrNotSupported );
            }    
        iCustomInterfaceUseCaseHint->HintDirectVideoCaptureL( codec, 
                                                              profile, 
                                                              params().iFrameSize );
            
        }
    //stillmode    
    else if ( iCustomInterfaceUseCaseHint && appUi && 
            ( ( appUi->IsEmbedded() && appUi->TargetMode() == ECamControllerImage ) || 
              ( !appUi->IsEmbedded() && iAppController.TargetMode() == ECamControllerImage ) ) )
        {
        PRINT( _L("Camera <> HintUseCaseL ImageMode") );    
        TPckgBuf<TCamParamsImage> params;
        iSettingProvider.ProvideCameraParamL( ECameraParamImage, &params );
        CCamera::TFormat format( params().iFormat );
        TInt index ( GetResolutionIndexL( format, params().iSize ) );    
        iCustomInterfaceUseCaseHint->HintStillCaptureL( format, index );
        }

    PRINT( _L("Camera <= CCamCameraController::HintUseCaseL"))    
    }
// End of file
