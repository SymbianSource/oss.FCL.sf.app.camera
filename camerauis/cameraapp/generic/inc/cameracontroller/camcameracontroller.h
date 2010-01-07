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
* Description:  
*
*/


#ifndef CAM_CAMERACONTROLLER_H
#define CAM_CAMERACONTROLLER_H


// ===========================================================================
// Included headers
#include "camcameracontrollerflags.hrh"

#include <e32base.h>
#include <ecam.h>
#ifdef CAMERAAPP_CAE_FOR_VIDEO
  #include <CaeEngine.h>
#endif

#include "camimageencoder.h"
#include "camcameraevents.h"
#include "camcamerarequests.h"
#include "camcameracontrollertypes.h"
#include "camcameracontrollersettings.h"
#include "mcamcameraobserver.h"          // MCamCameraObservable, MCamCameraObserver

// ===========================================================================
// Forward declarations

class MCameraBuffer;
class MCamSettingProvider;
class CIdle;
class CFbsBitmap;

class CCamSnapshotProvider;
class CCamCameraControllerActive;
class CCamAppController; 

#ifdef CAMERAAPP_CAPI_EMULATOR
  class CCamCamera;
  #define CAMERA CCamCamera
#else
  #define CAMERA CCamera 
#endif // CAMERAAPP_CAPI_EMULATOR

#ifdef FORCE_DUMMY_ENGINE
  class CCameraappDummyEngine;
#endif // FORCE_DUMMY_ENGINE

class MCameraOrientation;
class MCameraFaceTracking;
  class MCameraUIOrientationOverride;

#ifdef CAMERAAPP_PERFORMANCE_CONTROLLER
  class CCamPerformanceLogger;
#endif
#ifdef CAMERAAPP_FLASH_SIMULATOR
  class CCamFlashSimulator;
#endif

#ifdef CAMERAAPP_CAE_ERR_SIMULATION
  class CCamTimer;
#endif // CAMERAAPP_CAE_ERR_SIMULATION

// ===========================================================================
// Class declarations


/**
 * Camera controller class.
 *
 * If using MCameraObserver to receive callbacks from (old) Camera API,
 * still use (new) MCameraObserver2 to receive thumbnails events.
 */
class CCamCameraController : public CBase,
                             public MCamCameraObservable,
#ifdef CAMERAAPP_CAE_FOR_VIDEO
                             public MCamAppEngineObserver,
#endif
#if !defined( CAMERAAPP_CAPI_V2 )
                             public MCameraObserver,
#endif                             
                             public MCameraObserver2,
                             public MCamImageEncoderObserver
  {
  // =======================================================
  // Friends
  private:

    friend class CCamCameraControllerActive;

  // =======================================================
  // Methods

  // -------------------------------------------------------
  // Public constructors and destructor
  public:

    static CCamCameraController* NewL( MCamSettingProvider& aProvider, 
                                       CCamAppController& aAppController, 
                                       TInt aCameraIndex = 0 );
    virtual ~CCamCameraController();

  // -------------------------------------------------------
  // From MCamCameraObservable
  public:
    virtual void AttachObserverL( const MCamCameraObserver* aObserver,
                                  const TUint&              aInterest );
    virtual void DetachObserver ( const MCamCameraObserver* aObserver );

  // -------------------------------------------------------
  // From MCameraObserver2
  public:
    virtual void HandleEvent( const TECAMEvent& aEvent );
    virtual void ViewFinderReady( MCameraBuffer& aCameraBuffer, TInt aError );
    virtual void ImageBufferReady( MCameraBuffer& aCameraBuffer, TInt aError );
    virtual void VideoBufferReady( MCameraBuffer& aCameraBuffer, TInt aError );

#ifndef CAMERAAPP_CAPI_V2
  // -------------------------------------------------------
  // From MCameraObserver
  public:
    virtual void ReserveComplete( TInt aError );
    virtual void PowerOnComplete( TInt aError );
    virtual void ViewFinderFrameReady( CFbsBitmap& aFrame );
    virtual void ImageReady( CFbsBitmap* aBitmap, HBufC8* aData, TInt aError );
    virtual void FrameBufferReady( MFrameBuffer* aFrameBuffer, TInt aError );
#endif // CAMERAAPP_CAPI_V2

#ifdef CAMERAAPP_CAE_FOR_VIDEO
  // -------------------------------------------------------
  // From MCamAppEngineObserver
  public:
    virtual void McaeoInitComplete( TInt aError );
    virtual void McaeoStillPrepareComplete( TInt aError );
    virtual void McaeoVideoPrepareComplete( TInt aError );
    virtual void McaeoViewFinderFrameReady( CFbsBitmap& aFrame, TInt aError );
    virtual void McaeoSnapImageReady( const CFbsBitmap& aBitmap, TInt aError );
    virtual void McaeoStillImageReady( CFbsBitmap* aBitmap, HBufC8* aData, TInt aError );
    virtual void McaeoVideoRecordingOn( TInt aError );
    virtual void McaeoVideoRecordingPaused( TInt aError );
    virtual void McaeoVideoRecordingComplete( TInt aError );
    virtual void McaeoVideoRecordingTimes( 
        TTimeIntervalMicroSeconds aTimeElapsed, 
        TTimeIntervalMicroSeconds aTimeRemaining, 
        TInt aError );
    virtual void McaeoVideoRecordingStopped();
#endif // CAMERAAPP_CAE_FOR_VIDEO

  // -------------------------------------------------------
  // From MCamImageEncoderObserver
  public:

    virtual void ImageEncoded( TInt aStatus, HBufC8* aData );
  
  // -------------------------------------------------------
  // New methods
  public:
    
    /**
    * Issue a request for one operation.
    * Operation will be started synchronously.
    * Observers will receive notification when the operation is ready.
    */
    void RequestL( const TCamCameraRequestId& aRequestId );

    /**
    * 
    */
    void DirectRequestL( const TCamCameraRequestId& aRequestId );

    /**
    * Issue a sequence of operation requests.
    * This method will return immediately and the requests will be
    * processed in the background by callbacks.
    */
    void RequestSequenceL( const RCamRequestArray& aSequence );

    /**
    * Request one or more settings to be set to camera.
    * The settings to be processed are queried from Setting Provider.
    * Also the values are queried from there.
    * Settings are processed asynchronously and ECamCameraEventSettingsDone
    * event is sent when the process is complete.
    */
    void RequestSettingsChangeL();

    /**
    * Request one setting to be set synchronously to camera.
    * @param aSettingId  Id of the setting to change. Value to set
    *                    is asked from Setting Provider.
    * @see TCamCameraSettingId    
    */
    void DirectSettingsChangeL( 
            const NCamCameraController::TCamCameraSettingId& aSettingId );

    /**
    * Cancel outstanding request sequence.
    * Operations that have been already performed will not be rolled back.
    * Observers have received notifications of such operations.
    * If the sequence is not completed already (observers have not received
    * ECamEngineEventSequenceEnd notification), once the sequence is cancelled,
    * observers will receive ECamEngineEventSequenceEnd notification with 
    * status KErrCancel.
    */
    void CancelSequence();
    
    void SwitchCameraL( TInt aCameraIndex );

    void CompleteSwitchCameraL();
    
    void SetOrientationModeL( TInt aOrientation );
    
    /**
    * Set viewfinder window for direct screen viewfinder. The handle must remain
    * valid as long as viewfinder is running. 
    * @since S60 v5.0
    * @param aWindow Handle to viewfinder window. Must remain valid while
    *                viewfinder is running. Ownership not transferred.
    */
    void SetViewfinderWindowHandle( RWindowBase* aWindow );

    /**
    * Notify that the viewfinder window is about to be deleted. Viewfinder
    * will be stopped if this window handle is currently used for the
    * viewfinder.
    * @since S60 v5.0
    * @param aWindow Handle to viewfinder window. Ownership not transferred.
    */
    void ViewfinderWindowDeleted( RWindowBase* aWindow );

  // -------------------------------------------------------
  // Info getters
  public:

    static TInt CamerasAvailable();

    TInt CameraHandle(); // Calls CCamera::Handle which is not const

    const TCameraInfo& CameraInfo() const;
    const TCamControllerInfo& ControllerInfo() const;

    TUint                State()           const;
    TCamViewfinderMode   ViewfinderMode()  const;    
    TCamCameraTriState   ViewfinderState() const;
    TCamCameraTriState   SnapshotState()   const;
    TCamCameraReadyState FlashState()      const;

    /**
    * Method to retrieve the Remaining recording time from
    * CCaeEngine, this information is used in AppUi and
    * AppController to handle or continue with Video Rec operation
    */
    TTimeIntervalMicroSeconds RemainingVideoRecordingTime();

    void GetCameraSettingValueL( 
            const NCamCameraController::TCamCameraSettingId& aSettingId,
                  TAny*                                      aSettingData );

    /**
    * Checks whether setting value given by setting provider matches
    * the value currently set for iCamera.
    * @param aSettingId Setting id
    * @return upToDate
    */
    TBool SettingValueUpToDateL( 
            const NCamCameraController::TCamCameraSettingId& aSettingId );

  // -------------------------------------------------------
  // Private methods
  private:

    TInt ProcessNextRequestL();

    TInt ProcessOneRequestL( const TCamCameraRequestId& aRequestId );

    TInt ProcessSettingL( 
            const NCamCameraController::TCamCameraSettingId& aSettingId );

    // Helper methods used to divide ProcessOneRequestL to smaller portions.
    TInt ProcessControlStartupRequestL( const TCamCameraRequestId& aRequestId );
    TInt ProcessControlShutdownRequest( const TCamCameraRequestId& aRequestId ); // non-leaving
    
    TInt ProcessVfRequestL( const TCamCameraRequestId& aRequestId );
    void InitViewfinderL( const TCamViewfinderMode& aMode );
    void ProcessVfStartRequestL();
    void ProcessVfStopRequestL();
    void ProcessVfStopEcamRequest();
    void ProcessVfRelaseRequest(); // non-leaving    

    TInt ProcessSnapshotRequestL( const TCamCameraRequestId& aRequestId );
    void ProcessSsStopRequest();    // non-leaving
    void ProcessSsReleaseRequest(); // non-leaving
    void InitSnapshotL();
    
    TInt ProcessImageRequestL( const TCamCameraRequestId& aRequestId );
    TInt ProcessImageShutdownRequest( const TCamCameraRequestId& aRequestId );
    
    TInt ProcessVideoRequestL( const TCamCameraRequestId& aRequestId );

    void ProcessAutofocusRequestL( const TCamCameraRequestId& aRequestId );

    void ProcessCaptureLimitSettingL();
    
    void EndSequence( TInt aStatus );
    void ClearRequestQueue();
    void ClearSettingQueue();

  // -------------------------------------------------------
  // Event handlers

    void HandlePowerOnEvent( TInt aStatus );
    void HandleReserveGainEvent( TInt aStatus );
    void HandleReserveLostEvent( TInt aStatus );
    void HandleViewfinderEvent( MCameraBuffer* aBitmap, TInt aStatus );
    void HandleImageCaptureEvent( MCameraBuffer* aBitmap, TInt aStatus );
    void HandleImageCaptureEventL( MCameraBuffer*& aCameraBuffer );
    void HandleSnapshotEvent( TInt aStatus );
    void HandleSnapshotEventL( TBool aIgnore = EFalse  );

    void HandleVideoEvent( const TCamCameraEventId& aEventId,
                                 TInt               aStatus  );

    void HandleVideoTimeEvent( TInt aStatus,
                               TTimeIntervalMicroSeconds aTimeElapsed, 
                               TTimeIntervalMicroSeconds aTimeRemaining );

    void HandleAutoFocusEvent(       TInt  aStatus, 
                               const TUid& aEventUid );

    void HandleFlashStatusEvent(       TInt                aStatus,
                                 const TCamCameraEventId& aEventId );

    void HandleCallbackEvent( 
                        TInt                    aStatus, 
                  const TCamCameraEventId&      aEventId, 
                  const TCamCameraEventClassId& aEventClass,
                        TAny*                   aEventData = NULL );

  // -------------------------------------------------------
  // Helper methods
    
    void ReleaseCurrentCamera();
    void ReleaseCustomInterfaces();

    TBool HasCallback( const TCamCameraRequestId& aType );
 
    TBool CurrentSettingHasCallback();

    /**
    * Sets viewfinder window ordinal position so that the window is visible.
    * Window needs to visible when viewfinder is started. In addition,
    * ordinal position can be set as a parameter, if original position needs to
    * be returned back.
    * @since S60 5.0
    * @param aOrdinalPosition,viewfinder window ordinal position to set 
    * @return TInt, DSA viewfinder window ordinal position or KErrUnknown
    */   
    TInt SetVfWindowOrdinal( TInt aOrdinalPosition = KErrUnknown ); 


  // -------------------------------------------------------
  // Notification of observers

    void NotifyObservers( TInt                   aStatus, 
                          TCamCameraEventId      aEventId,
                          TCamCameraEventClassId aEventClass,
                          TAny*                  aEventData = NULL );

 

  // -------------------------------------------------------
  // Getters for info on current camera
  
    TInt GetResolutionIndexL(       CCamera::TFormat& aFormat,
                              const TSize&            aSize   );
#ifdef CAMERAAPP_CAPI_V2_ADV
    /**
    * Get info related to advanced settings.
    */
    void GetAdvancedSettingsInfoL();
    
    /**
    * Get closest supported EV compensation step to the given value.
    * @aEvProposedStep The step for which the closest supported one
    *                  is asked.
    * @return Closest supported step to the given proposed step.
    *         Always 0 if no supported EV steps available.
    */
    TInt ResolveEvStep( TInt aEvProposedStep ) const;

    /**
    * Helper method to check whether the given value is supported,
    * based on the value array and value info.
    * Depending on aValueInfo content, the aValueList may represent
    * the supported values in completely different ways. This 
    * method is used to help determining if single value is supported
    * or not.
    * @param aValue     The value to be checked if it's supported.
    * @param aValueList Array describing the supported values. Actual meaning
    *                   depends on aValueInfo value.
    * @param aValueInfo Enumeration value describing the meaning of values in
    *                   aValueList array.
    * @see   TValueInfo
    */
    static TBool IsSupportedValue( const TInt&         aValue, 
                                   const RArray<TInt>& aValueList, 
                                   const TValueInfo&   aValueInfo );

#endif // CAMERAAPP_CAPI_V2_ADV

    /**
    * Resolve the snapshot format to be used based on supported formats
    * and the given preferred format.
    *
    * @aPreferredFormat Format to be preferably used, if available.
    * @leave  KErrNotSupported No acceptable formats are supported.
    * @leave  KErrNotReady     Snapshot provider not available.
    * @return Best supported snapshot format.
    */
    CCamera::TFormat ResolveSnapshotFormatL( CCamera::TFormat aPreferredFormat ) const;


  // -------------------------------------------------------
  // Conversion methods between requests, events and event classes 

    static TCamCameraEventId Request2Event( const TCamCameraRequestId& aType );

    static TCamCameraEventClassId EventClass( const TCamCameraEventId& aEventId );

    // Comparison function for searching for a matching resolution in
    // an RArray<TSize>
    static TBool CompareSize( const TSize& aA, const TSize& aB );

  // -------------------------------------------------------
  // Camera / engine construction
  
    CCaeEngine* NewCaeEngineL( TInt aCameraIndex );

    CAMERA* NewCameraL( TInt aCameraIndex );
  
    CAMERA* NewDuplicateCameraL( TInt aCameraHandle );    

  // -------------------------------------------------------
  // Private constructors
  private:

    void ConstructL( TInt aCameraIndex );

    CCamCameraController( MCamSettingProvider& aProvider, 
                          CCamAppController& aAppController );

  // -------------------------------------------------------
  // Prints for debug builds
#ifdef _DEBUG
  private:

    void PrintCameraInfo() const;

  #ifdef CAMERAAPP_CAPI_V2_ADV
    void PrintAdvancedSettingInfo() const;
  #endif // CAMERAAPP_CAPI_V2_ADV

    void PrintSnapshotInfo() const;

#endif // _DEBUG


    void SetFaceTrackingL();


  // =======================================================
  // Data
  private:

  // -------------------------------------------------------
  // Request queue and related
    TInt                               iSequenceIndex;
    RCamRequestArray                   iSequenceArray;
    
    TInt                               iReserveTryAgainCount;

  // -------------------------------------------------------
  // Settings queue and related
    TInt                                              iSettingIndex;
    RArray<NCamCameraController::TCamCameraSettingId> iSettingArray;

    MCamSettingProvider&               iSettingProvider;   /** Entity providing settings.             */

  // -------------------------------------------------------
  // Active object thriving our asynchronous processes.

    CCamCameraControllerActive*        iActive;            /** Own.                                   */

  // -------------------------------------------------------
  // Observers and their interests

    RPointerArray<MCamCameraObserver>  iObservers;         /** Array of observers. Observers not own. */
    RArray       <TUint>               iObserverInterests; /** Array of observer interests            */


  // -------------------------------------------------------
  // Info containers
    TCameraInfo                        iCameraInfo;        /** Camera details                         */
    TCamControllerInfo                 iInfo;              /** Our own details                        */

    TCamCameraResolutionSupport        iResolutionSupport; /** Supported resolutions info             */
#ifdef CAMERAAPP_CAPI_V2_ADV
    TCamAdvancedSettingInfo            iAdvancedSettingInfo;  /** Info related to advanced settings   */
#endif

    TBool                              iAlternateFormatInUse; /** Encode jpeg from bitmap internally  */                                       

  // -------------------------------------------------------
  // ECam
    CAMERA*                            iCamera;            /** Onboard camera instance. Own.          */
    TBool                              iReleasedByUi;      /** Flag to distinguish own and external   */
                                                           /** camera release.                        */

#ifdef CAMERAAPP_CAPI_V2
  #ifdef CAMERAAPP_CAPI_V2_DVF
    CCamera::CCameraDirectViewFinder*  iDirectViewfinder;  /** Direct viewfinder. Own.                */
  #endif
  #ifdef CAMERAAPP_CAPI_V2_OL
    CCamera::CCameraOverlay*           iOverlayProvider;   /** Overlay provider. Own.                 */
  #endif
  #ifdef CAMERAAPP_CAPI_V2_ADV
    CCamera::CCameraAdvancedSettings*  iAdvancedSettings;  /** Advanced settings instance. Own.       */
  #endif
  #ifdef CAMERAAPP_CAPI_V2_HG
    CCamera::CCameraHistogram*         iHistogramProvider; /** Histogram provider. Own.               */
  #endif
  #ifdef CAMERAAPP_CAPI_V2_IP
    CCamera::CCameraImageProcessing*   iImageProcessor;    /** Image processor. Own.                  */
  #endif
#endif

    CCamSnapshotProvider*              iSnapshotProvider;

    CCamImageEncoder*                  iEncoder;

    MCameraOrientation*                iCustomInterfaceOrientation;   /** Not own. */

    MCameraFaceTracking*               iCustomInterfaceFaceTracking;   /** Not own. */
    
    MCameraUIOrientationOverride*      iCustomInterfaceUIOrientationOverride; /** Not own. */

  // -------------------------------------------------------
  // CAE 
#ifdef CAMERAAPP_CAE_FOR_VIDEO

    CCaeEngine*                        iCaeEngine;         /** Camera app engine for video. Own.      */
    TCamVideoRecordingTimes            iVideoTimes;
    TBool                              iAsyncVideoStopModeSupported;

  #ifdef CAMERAAPP_CAE_FIX
    /** 
    * Workaround for changing between video (CCaeEngine) and image (CCamera) modes
    * Without the support for camera handle sharing in CAE.
    */

    TBool iCaeInUse;
    
    TInt ProceedModeSwitch();
    TInt DoProceedModeSwitchL();

    enum TCamModeChange
      {
      ECamModeChangeInactive,
      ECamModeChangeVideo2Image,
      ECamModeChangeImage2Video
      };
    enum TCamModeChangePhase
      {
      ECamModeChangePhaseIdle,
      ECamModeChangePhase0,
      ECamModeChangePhase1,
      ECamModeChangePhase2,
      ECamModeChangePhase3,
      ECamModeChangePhase4
      };

    TCamModeChange      iModeChange;      
    TInt                iModeChangePhase; // TCamModeChangePhase
    TInt                iModeChangeStatus;
  #endif // CAMERAAPP_CAE_FIX
#endif

    // This bool is used to determine whether current event is the first or second one.
    TBool               iFirstAfEventReceived;
	 
    // This bool is used to determine whether AF operation is started 
    TBool               iAfInProgress;
	
    // This bool is used to determine whether HyperFocal needs to be set 	
    TBool               iAfHyperfocalPending;

  // -------------------------------------------------------
  // Flash simulation
#ifdef CAMERAAPP_FLASH_SIMULATOR
    CCamFlashSimulator*                iFlashSimulator;
#endif 

    // -------------------------------------------------------
    // CAE error simulation
#ifdef CAMERAAPP_CAE_ERR_SIMULATION
    // Timer for delaying callbacks to simulate difficult/erroneous situations
    CCamTimer* iCallbackTimer;
    
    TCamCameraEventId  iSimEventId;
    TInt               iSimStatus;

    static TInt DelayedCaeCallback( TAny* aController );
    
    void CallAppropriateCallback( const TCamCameraEventId aEventId = ECamCameraEventNone,
                                        TInt              aStatus  = 0 );
    
#endif // CAMERAAPP_CAE_ERR_SIMULATION
    
  // -------------------------------------------------------    

  // -------------------------------------------------------
  // Performance logging related
#ifdef CAMERAAPP_PERFORMANCE_CONTROLLER
  public:
    const CCamPerformanceLogger* PerformanceLogger() const;

  private:
    CCamPerformanceLogger*             iPerformanceLogger;
    TBool                              iFirstVfFrameReceived;    
#endif // CAMERAAPP_PERFORMANCE_CONTROLLER
  // -------------------------------------------------------

  public:
     void SetBurstImagesRemaining( TInt aBurstImagesRemaining );
    TBool AsyncVideoStopModeSupported();

        
    /**
    * Used to recover lost resources. 
    *
    * @param aSelf pointer to CCamCameraController object
    * @return N/A
    */
    static TInt IdleCallback( TAny* aSelf );   

    /**
    * Helper method for IdleCallback 
    */
    void DoIveRecovery();

    /**
    * Indicates resource recovery state.
    * @return TBool, ETrue if resource recovering is initated
    */
    TBool IsWaitingIveResources();  
    
  private:
    TInt iBurstImagesRemaining;

    // not own
    RWindowBase* iViewfinderWindow;
    CIdle* iIdle;  

    // Counts how many times lost resources are tried to recover 
    // (KIveRecoveryCountMax).
    TInt iIveRecoveryCount;  
    TBool iIveRecoveryOngoing;
    
    CCamAppController& iAppController;
    TInt iCurrentCameraIndex;  

  };

#endif // CAM_CAMERACONTROLLER_H

// end of file
