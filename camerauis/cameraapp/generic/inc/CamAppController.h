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
* Description:  Class for managing intercommunication between Camcera UI*
*/


#ifndef CAMAPPCONTROLLER_H
#define CAMAPPCONTROLLER_H


// ===========================================================================
//  INCLUDES
#include <e32base.h>

#include <cflistener.h> // publish zoom state related
#include <cfclient.h>

#include <sensrvdatalistener.h>
#include <sensrvchannel.h>
#include <sensrvorientationsensor.h>

#include "MCamAppController.h"
#include "CamControllerObservers.h"
#include "mcamsettingsmodelobserver.h"
#include "CamSettings.hrh"
#include "Cam.hrh"          // TCamSoundId
#include "CamSettingsInternal.hrh"

#include "mcamplayerobserver.h"

#include "CamImageSaveActive.h"
#include "CamCallStateAo.h"
#include "CamObserver.h"
#include "CamBurstModeObserver.h"
#include "CamPerformance.h"

#ifndef __WINSCW__
#include <rlocationtrail.h>
#endif

#include "CamPropertyObserver.h"

// <CAMERAAPP_CAPI_V2_MIGRATION>
#include "mcamcameraobserver.h" 
#include "camcamerarequests.h"
#include "camfolderutility.h"
#include "camcameracontroller.h"
#include "CamDriveChangeNotifier.h"
#include "CamSyncRotatorAo.h"


// ===========================================================================
// CONSTANTS

// !Do not change the order of these without careful consideration!
// 
enum TCamCameraState
  {
  ECamCameraIdle          = 0,
  ECamCameraReserved      = 1 << 0,
  ECamCameraPowerOn       = 1 << 1,
  ECamCameraPreparedImage = 1 << 2,
  ECamCameraPreparedVideo = 1 << 3,
  ECamCameraStateLast // Not valid state, only marker
  };

/**
* Flags for ongoing operations
*/
enum TCamBusyFlags
  {
  EBusyNone     = 0,
  EBusyRequest  = 1<<0,
  EBusySequence = 1<<1,
  EBusySetting  = 1<<2,
  EBusyLast // Marker
  };


// ===========================================================================
// FORWARD DECLARATIONS
class CRepository;
//class CCamSettingsModelBase;
class MCamAppController;
class MCamSettingsModel;
class MCamSettingPreviewHandler;   // <CAMERAAPP_CAPI_V2_MIGRATION/>
//class CCamConstantSettingProvider; // <CAMERAAPP_CAPI_V2_MIGRATION/>
class CCamSettingProvider; // <CAMERAAPP_CAPI_V2_MIGRATION/>
class CCamTimer;
class CCamObserverHandler;
class CCamBurstCaptureArray;
class CCamAudioPlayerController;
class MCamStaticSettings;
class CCamGSInterface;
class CCameraUiConfigManager;
class CCamPropertyWatcher;
class CCamSnapShotRotator;


class CCamFlashStatus;
class CCamConfiguration;
class CCFClient;

// ===========================================================================
// CLASS DECLARATION

class TCamAppControllerInfo
  {
  public:
    TCamAppControllerInfo();

  public:
    TCamActiveCamera      iActiveCamera;

    TCamCameraMode        iMode;
    TCamCameraMode        iTargetMode;

    TCamImageCaptureMode  iImageMode; // ECamImageCaptureNone in video controller mode
    TCamImageCaptureMode  iTargetImageMode; 

    TCamCaptureOperation iOperation; // Current operation 

    // Target viewfinder state and mode
    TCamCameraTriState iTargetVfState;
    TBool              iViewfinderMirror; // set when viewfinder initialized

    // Target image parameters        
    TInt               iTargetImageResolution;
    TInt               iTargetImageQuality;
    
    // Target video parameters
    /** not supported yet */

    // Request being handled        
    TCamCameraRequestId iBusyRequestId;
  };


/**
* Handle intercommunication between camcorder UI and engine.
*
*  @since 2.8
*/
class CCamAppController : public CBase, 
                          public MCamAppController,
                          public MCamSettingsModelObserver,
                          public MCamCameraObserver,
                          public MCamImageSaveObserver,
                          public MCamCallStateObserver,
                          public MPropertyObserver,
                          public MSensrvDataListener,
                          public MCamPlayerObserver,
                          public MCamObserver,
                          public MCamBurstModeObserver,
                          public MCFListener
                          ,public MCamDriveChangeNotifierObserver
                          ,public MBitmapRotationObserver 
                          
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 2.8
        * @return pointer to the created CCamAppController object
        */
        static CCamAppController* NewL();
        
               
        /**
        * Complete construction of the engine and issue notification requests.
        * The camera engine cannot be instantiated until the application
        * orientation has been set by the CCamAppUi. This does not
        * exist when the CCamAppController is constructed. CompleteConstructionL
        * must be called in the CCamAppUi::ConstructL()
        * @since 3.0
        */ 
        void CompleteConstructionL();

        
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamAppController();

   public:		
     /** 
      * Calls CamSettingModel to save user defined FT setting
      * @since S60 5.0
      * @param None
      * @return None
      */
      void StoreFaceTrackingValue();		

  // -------------------------------------------------------
  // From MCamCameraObserver
  public:

    virtual void HandleCameraEventL( TInt              aStatus, 
                                     TCamCameraEventId aEventId, 
                                     TAny*             aEventData /*= NULL*/ );



  // -------------------------------------------------------
  // from MCamSettingsModelObserver 
  public:

    virtual void IntSettingChangedL( TCamSettingItemIds aSettingItem, 
                                     TInt               aSettingValue );
    virtual void TextSettingChangedL( TCamSettingItemIds aSettingItem, 
                                      const TDesC&       aSettingValue );


  // -------------------------------------------------------
  // from MCamBurstModeObserver
  
    /**
    * Burst mode activation has changed
    * @since 2.8
    * @param aActive whether or not burst mode is active
    * @param aStillModeActive whether or not still capture is active
    */
    virtual void BurstModeActiveL( TBool aActive, TBool aStillModeActive );

  // -------------------------------------------------------
  // from MCamAudioPlayerObserver
    virtual void PlayComplete( TInt aStatus, TInt aSoundId );


  // -------------------------------------------------------
  public:
    // From MCFListener ( Context framework for publishing zoom state.
    /**
    * Indicates a change in context for subscribed clients.
    * If client has subscribed to partial context class path,
    * only the changed context will be indicated.
    * 
    * @since S60 4.0
    * @param aIndication: Context indication.
    * @return None
    */
    inline void ContextIndicationL(
            const CCFContextIndication& /*aChangedContext*/ ) {};
    
    /**
    * Indicates that specified action is needed to be executed.
    * When action indication is received certain rule has been triggered
    * and action is needed to be performed.
    *
    * @since S60 4.0
    * @param aActionToExecute Action indication.
    * @return None
    */
    inline void ActionIndicationL(
        const CCFActionIndication& /*aActionToExecute*/ ) {};
            
    /**
    * Indicates that error has occured.
    * @since S60 4.0
    * @param aError Error code.
    * @return None
    */
    inline void HandleContextFrameworkError( TCFError /*aError*/,
            const TDesC& /*aSource*/,
            const TDesC& /*aType*/ ){};
    
    /**
    * Returns an extension interface.
    * The extension interface is mapped with the extension UID.
    *
    * If the client does not support the requested interface,
    * client must return NULL. Otherwise client needs to return
    * the correct interface combined with the UID.
    *
    * @since S60 5.0
    * @param aExtensionUid: The identifier of the extension.
    * @return Pointer to the extension.
    */
   inline TAny* Extension( const TUid& /*aExtensionUid*/ ) const { return NULL; };
    
    // New Function

    /*
    * Method to publish the zoom key and setting the
    * value or the state to enabled or disabled.
    * @Paran aEnabled used to set the the state of zoom key
    * to be enabled or disabled.
    */
    void PublishZoomStateL( const TBool aEnabled );

  public: 
    
      /**
      * Returns whether or not the current profile is silent
      * @since 3.0
      * @return ETrue if current profile is silent
      */
      TBool IsProfileSilent();    
    
      /**
      * Returns whether or not we have touch screen support in the device
      * @return ETrue if device supports touch screen
      */
      TBool IsTouchScreenSupported();
    
      /**
      * Returns whether or not we have direct screen viewfinder support in the device
      * @return ETrue if device supports direct screen viewfinder
      */
      TBool IsDirectScreenVFSupported( TBool aPrimaryCamera = ETrue );
    
	// ---------------------------------------------------------------------------
	//Helper function for making notification about deleted files
	//Added this method to do notification if files have been
	//deleted outside camera application and then camera is
	//put back to foreground in postcapture view.
	//Security note. Contents of the files might have been
	//changed when camera was in background.
	// ---------------------------------------------------------------------------
	//
	void  MediaFileChangedNotification();
    
	TBool EngineRequestsPending() const; 	
    /**
    * Set settings restore to be done at earliest
    * convenient time.
    */
    void  SetSettingsRestoreNeeded();

    /**
    * Set the flag to indicate that the original values 
    * of remaining images is stored
    */
     void SetRemainingImageStored();
    
    /** 
    * Returns the flag that indicates if the original value 
    * of remaing images is stored or not.
    */    
     TBool IsRemainingImageStored() const;
    
    /** 
    * Return whether saving is still in progress.
    */    
    TBool IsSavingInProgress() const;
     
    /**
    * Return if settings restore is set pending.
    */
    TBool IsSettingsRestoreNeeded() const;
  
	
    void RestartTimeLapseTimerL();
	
  
    static TInt CamerasAvailable();

    /**
    * Is time lapse supported.
    * Value read from Central Repository.
    */
    TBool TimeLapseSupported();
    
    /**
    * Is time lapse supported.
    * Value read from Central Repository.
    */
    TBool AlwaysOnSupported();

    /**
    * Release version number.
    * Value read from Central Repository.
    */
    const TVersion& Version();

    /***
    * Can a new request be made to controller or not.
    * Shutdown may be requested at any moment.
    */
    TBool Busy() const;

    /**
    * Returns true if the last captured image was saved
    * @since 2.8
    * @return ETrue if the last captured image has been saved.
    */
    TBool SavedCurrentImage() const;

    /**
    * Rename image/video.
    * @since 2.8
    * @param aNewName the new name for the current capture array file.
    * @param aMode the current capture mode.
    * @return ETrue if the file has been renamed successfully. 
    */
    TBool RenameCurrentFileL( const TDesC&          aNewName, 
                              const TCamCameraMode& aMode    );

    /**
    * Add a controller observer.
    * @since 2.8
    * @param aObserver Pointer to an object implementing 
    *                   MCamControllerObserver
    * @return ?description
    */
    void AddControllerObserverL( const MCamControllerObserver* aObserver );

    /**
    * Remove a controller observer
    * @since 2.8
    * @param aObserver Pointer to the observer to remove. The object
    *                  does not have to be added as an observer.
    */
    void RemoveControllerObserver
        ( const MCamControllerObserver* aObserver );

    /**
    * Add camera observer.
    * Forwards call to CCamCameraController.
    * @see CCamCameraController for more details.
    */
    void AddCameraObserverL( const MCamCameraObserver* aObserver, 
                             const TUint&              aEventInterest );
    
    /**
    * Removes camera observer.
    * Forwards call to CCamCameraController.
    * @see CCamCameraController for more details.
    */
    void RemoveCameraObserver( const MCamCameraObserver* aObserver );

    void AddSettingsObserverL( const MCamSettingsModelObserver* aObserver );
    void RemoveSettingsObserver( const MCamSettingsModelObserver* aObserver );

    TBool CameraControllerBusy() const;
    TUint CameraControllerState() const;

    TCamCameraState       CameraState() const;
    TCamCameraMode        CurrentMode() const;
    TCamCameraMode        TargetMode() const;
    TCamImageCaptureMode  CurrentImageModeSetup() const;
    TCamCaptureOperation  CurrentOperation() const;
    TCamCaptureOperation  CurrentImageOperation() const;
    TCamCaptureOperation  CurrentVideoOperation() const;
    TCamImageCaptureMode  CurrentImageMode() const;
    

    TInt CaptureLimit() const;
    TInt CaptureLimitSetting() const;
    void SetCaptureLimitL( TInt aLimit );
    
    /**
    * From MBitmapRotationObserver
    * Notification of bitmap rotation complete events
    * @param aErr KErrNone if successful
    */
    void RotationCompleteL( TInt aErr );

private:
    void ReadVariantFlagsL();


    void SetMode( const TCamCameraMode& aNewMode );
    void SetTargetMode( const TCamCameraMode& aNewMode );

    void SetImageMode( const TCamImageCaptureMode& aNewMode );
    void SetTargetImageMode( const TCamImageCaptureMode& aNewMode );
    
    void SetOperation( TCamCaptureOperation aNewOperation, 
                       TInt                 aError = KErrNone );
                                     
    TCamCameraTriState ViewfinderTargetState() const;


public:
    /*
     *  Handle to Camera Ui Config Manager
     */
    CCameraUiConfigManager* UiConfigManagerPtr();
    
    /*
    * De-register harvester client events.
    */
    void DeRegisterHarverterClientEvents();
        
    /**
    * Return resource id for video viewfinder layout
    * @since 3.0
    * @param aResolution for current resolution
    * @return Resource id for video viewfinder layout
    */
    TInt VideoViewFinderResourceId( TCamVideoResolution aResolution );

    /**
    * Start the viewfinder for the specified mode
    * as soon as the engine is intialised
    * @since 2.8
    * @param aMode The viewfinder mode being entered
    */
    void EnterViewfinderMode( const TCamCameraMode& aMode );

    /**
    * ExitViewinderMode    
    * @since 2.8
    * @param aMode The viewfinder mode being exited
    */
    void ExitViewfinderMode( const TCamCameraMode& aMode );

    /**
    * FreezeViewFinder    
    * @since 2.8
    * @param aFreeze Whether or not the viewfinder is
    * being frozen after this frame
    */
    void FreezeViewFinder( TBool aFreeze );

    /**
    * Return number of images that can still be captured
    * @since 2.8
    * @param aStorage storage location - defaults to current loation
    * @param aBurstActive - set to ETrue if burst mode is active
    * @return the number of images
    */
    TInt ImagesRemaining( TCamMediaStorage aStorage,
                          TBool            aBurstActive );

    /**
    * Return number of images that can still be captured for a particular
    * image quality
    * @since 2.8
    * @param aStorage storage location - defaults to current loation
    * @param aBurstActive - set to ETrue if burst mode is active
    * @param aQualityIndex - index of the image quality
    * @return the number of images
    */
    TInt ImagesRemaining( TCamMediaStorage aStorage,
                          TBool            aBurstActive, 
                          TInt             aQualityIndex );

    /**
    * Return amount of video that has been captured
    * @since 2.8
    * @return the amount of video in micro secs
    */
    TTimeIntervalMicroSeconds RecordTimeElapsed() const;


    /**
    * Set amount of video that has been captured
	* @param aElapsed - recorded video time.
    */
    void RecordTimeElapsed(TTimeIntervalMicroSeconds aElapsed );


    /*
    * Return amount of video that can still be captured
    * @since 2.8
    * @return the amount of video in secs
    */
    TTimeIntervalMicroSeconds RecordTimeRemaining();

    /**
    * Updates the interval used in next TimeLapse capture
    * @since 3.0
    * @param aInterval The new interval to use
    */        
    void SetTimeLapseInterval( TTimeIntervalMicroSeconds aInterval );   

    /**
    * Returns the interval used in next TimeLapse capture
    * @since 3.0
    * @return The current time lapse interval in microseconds
    */
    TTimeIntervalMicroSeconds TimeLapseInterval();
    
    /**
    * Returns the remaining time lapse interval until the next capture
    * @since 3.0
    * @return The remaining time to the next timelapse capture
    */
    TTimeIntervalMicroSeconds TimeLapseCountdown();

    /** 
    * Start the autofocus procedure
    * @since 2.8
    */
    void StartAutoFocus();

    /**
    * Begin the capture procedure
    * @since 2.8
    */
    void Capture();               

    /**
    * Stop the burst capture operation
    * @since 2.8
    */
    void StopSequenceCaptureL();

    /**
    * Whether or not a still capture operation is pending
    * i.e. a request to start still capture has been made and is 
    * waiting for the focus operation to complete.
    * @since 2.8
    * @return ETrue if a still capture operation is pending
    */
    TBool CapturePending() const;

	/**
    * Whether or not a capture key pressed during image saving
    * @since 2.8
    * @return ETrue if capture key is pressed during image saving
    */
    TBool CaptureKeyPressedWhileImageSaving() const;

	/**
    * Stores shutter key press during saving image
    * @since 2.8
    */
    void SetCaptureKeyPressedWhileImageSaving(TBool aCaptureKeyPressed);
    
    /**
    * Whether or not a video recording operation is pending
    * i.e. a request to start video recording has been made and is
    * waiting for the engine to complete a video name update operation.
    * @since 2.8
    * @return ETrue if a record operation is pending
    */
    TBool VideoRecordPending() const;

    /**
    * Whether or not a sequence capture is in progress
    * @since 2.8
    * @return ETrue if a sequence capture is in progress
    */
    TBool SequenceCaptureInProgress() const;

    /**
    * Begin the recording procedure
    * @since 2.8
    */
    void StartVideoRecordingL();

    /**
    * End the recording procedure
    * @since 2.8
    */
    void StopVideoRecording();

    /**
    * End the recording procedure
    * @since 5.0
    */
    void StopVideoRecordingAsync();
    
    /**
    * Store the cause of the video stop
    * @since 3.0
    */
    void SetVideoStoppedForPhoneApp( TBool aIsPhoneApp );

    /**
    * Pause the recording procedure
    * @since 2.8
    */
    void PauseVideoRecording();

    /**
    * Continue the recording procedure
    * @since 2.8
    */
    void ContinueVideoRecording();

    /**
    * Issue AF request.
    */
    TBool TryAFRequest( TInt aAFRequest );
    
    /**
    * Cancel ongoing AutoFocus.
    */
    void CancelAFNow();
	
    /**
    * Cancel a requested capture
    * @since 2.8
    */
    void CancelFocusAndCapture();

    /**
    * Return the current snapshot image
    * @since 2.8
    * @return the current snapshot image
    */
    const CFbsBitmap* SnapshotImage();

    /**
    * Sets the specified burst mode item as the current image, in 
    * preparation for a Post-capture view of the image.
    * @since 2.8        
    * @param aIndex The index of the burst item in "CCamBurstCaptureArray"
    */
    void SetAsCurrentImage( TInt aIndex );

    /**
    * Get the user-visible name for next image.
    * @since 2.8
    * @return file name for the current image or video without path or
    *         extension
    */
    const TDesC& CurrentImageName() const;

    /**
    * Delete the current image/video file
    * @since 2.8
        * @param aNofity Should notification be sent to observers about
        *                change in media files. When this call is made
        *                several times in a row, it's worth to do the
        *                notification only once.
    * @return KErrNone if delete successful, otherwise system-wide 
    * error code
    */
    TInt DeleteCurrentFile( TBool aNotify = ETrue );
    
    /**
    * Delete the timelapse sequence files
    * @since 3.0
    */
    void DeleteTimeLapseFiles();
    
    /**
    * Indicate whether or not the controller is actively engaged in still
    * or video capture - this includes paused video, saving photos/videos,
    * periods in between timelapse captures and pending captures 
    * (captures that have been requested but not yet started)
    * @since 3.0
    * @return EFalse if not currently capturing, otherwise ETrue
    */
    TBool IsProcessingCapture();
    
    /**
    * Complete current capture asap, then report completion to the observer
    * @since 3.0
    * @param aObserver To be notified when the current capture process completes
    * @param aSafeCompletion ETrue if all captures should be saved. EFalse to complete 
    * sooner and lose any images that have not yet begun to save
    */
    void CompleteCaptureAndNotifyL( MCamObserver* aObserver, TBool aSafeCompletion );

    /**
    * The application is closing. Complete any active operations then
    * exit the application.
    * @param aImmediateShutdown ETrue: shutdown immediately or 
    * EFalse: wait for pending operations to complete.
    * @since 2.8
    */
    void EnterShutdownMode( TBool aImmediateShutdown );

    /**
    * Whether or not the controller is in shutdown mode
    * @since 2.8
    * @return ETrue if the controller is in shutdown mode else EFalse
    */
    TBool IsInShutdownMode() const;
      
    /**
    * Returns true if AppUi has been constructed, and not yet
    * destructed.               
    */
    TBool IsAppUiAvailable() const;
    
    /**
    * Sets AppUi availablility (see IsAppUiAvailable())
    */
    void SetAppUiAvailable( TBool aAvailable );        


    /**
    * Called to return the range of supported zoom values
    * @since 2.8
    * @param aMinZoom on return contains the minimum zoom value
    * @param aMaxZoom on return contains the maximum zoom value
    */
    void GetZoomRange( TInt& aMinZoom, TInt& aMaxZoom ) const;

    /**
    * Called to return current zoom value
    * @since 2.8
    * @return the current zoom value
    */
    TInt ZoomValue() const;

    /**
    * Sets the current zoom value
    * @since 2.8
    * @param aValue the new zoom value to be used
    */
    void SetZoomValue( TInt aValue );

    /**
    * Resets all user scene settings to their defaults.
    * @since 2.8
    */
        void ResetUserSceneL();
 
        /**
    * Previews a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want 
    * to preview.
    * @param aSettingValue the new integer value for the specified 
    * setting item to be previewed.
    * @since 2.8
    */
    void PreviewSettingChangeL( TInt aSettingItem, TInt aSettingValue );

    /**
    * Cancels all preview changes, since last commit/cancel.
    * @since 2.8
    */
    void CancelPreviewChangesL();

    /**
    * Commits last preview change.
    * @since 2.8
    */
    void CommitPreviewChanges();

    /**
    * Re-prepare video after e.g. a setting change requires this.
    */
    void RePrepareVideoL();

    /**
    * Returns the current integer value for the specified setting
    * @return the current integer setting value
    * @param aSettingItem specifies which setting item that want 
    * the value of.
    * @since 2.8
    */
    TInt IntegerSettingValue( TInt aSettingItem ) const;

    /**
    * Returns the current integer value for the specified setting
    * without the filtering usually performed on storage location.
    * This is neccesary to allow the video/photo settings list to show
    * the selected storage location rather than the forced storage location.
    * @return the current integer setting value
    * @param aSettingItem specifies which setting item that want the value of.
    * @since 2.8
    */
    TInt IntegerSettingValueUnfiltered( TInt aSettingItem ) const;

    /**
    * Sets a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want 
    * to set the value of.
    * @param aSettingValue the new integer value for the specified 
    * setting item.
    * @since 2.8
    */
    void SetIntegerSettingValueL( TInt aSettingItem, TInt aSettingValue );

    /**
    * Sets pathnames for video and image files to point to the specified storage
    * @param aMediaStorage specifies phone memory or MMC
    * @since 2.8
    */
    void SetPathnamesToNewStorageL( TCamMediaStorage aMediaStorage );

   /**
    * Sets a new text value for the specified setting
    * @param aSettingItem specifies which setting item that want to set
    * the text value of.
    * @param aSettingValue the new text value for the specified setting 
    * item.
    * @since 2.8
    */
    void SetTextSettingValueL( TInt aSettingItem, 
        const TDesC& aSettingValue );


    /**
    * Returns the current text value for the specified setting
    * @return the current text setting value
    * @param aSettingItem specifies which setting item that want 
    * the text value of.
    * @since 2.8
    */
    TPtrC TextSettingValue( TInt aSettingItem ) const;

    /**
    * Returns whether a particular setting value can be set or not.
    * @return ETrue if can be set. Otherwise, EFalse.
    * @param aSettingItem the setting item in question. 
    * @param aSettingValue the value in question. 
    * @since 2.8
    */
    TBool SettingValueEnabled( TInt aSettingItem, TInt aSettingValue ) const;

    /**
    * Loads the settings from shared data, or if unable to from the 
    * resource file. 
    * @param aIsEmbedded whether or not the application is running in
    * embedded mode.
    * @since 2.8
    */       
    void LoadStaticSettingsL( TBool aIsEmbedded );   

    /**
    * Get the default value for a setting item.
    * @param  aSettingId The id of the setting.
    * @return The default value for setting item or KErrNotFound if not found.
    */
    TInt DynamicSettingDefaultValue( TInt aSettingId ) const;

    /**
    * Sets all dynamic settings to their defaults.
    * @since 2.8
    */       
    void SetDynamicSettingsToDefaults();

    /**
    * Sets a single dynamic setting to its default.
    * @since 2.8
    */       
    void SetDynamicSettingToDefault(TInt aSettingItem);
    
    /**
    * Set scene default value to a dynamic setting.
    * @param aSceneId    Id of the scene to use.
    * @param aSceneField Id of scene field.
    * @param aSettingId  If of the dynamic setting to set.
    * @param aVideoScene Does aSceneId refer to a video scene.
    * @return KErrNotFound, if setting could not be set.
    */
    TInt SetDynamicSettingToSceneDefaultL( TInt  aSceneId,
                                           TInt  aSceneField,
                                           TInt  aSettingId,
                                           TBool aVideoScene );
    

#ifdef PRODUCT_USES_GENERIC_SETUP_INDICATOR
    /**
    * Returns whether or not the video setup settings are set to the
    * defaults for its current scene.
    * @since 2.8
    * @return ETrue if set to defaults. Otherwise, EFalse
    */       
    TBool VideoSceneDefaultsAreSet();

    /**
    * Returns whether or not the photo setup settings are set to the
    * defaults for its current scene.
    * @since 2.8
    * @return ETrue if set to defaults. Otherwise, EFalse
    */       
    TBool PhotoSceneDefaultsAreSet();
#endif // PRODUCT_USES_GENERIC_SETUP_INDICATOR

    /**
    * Returns the file name of the most recent saved image/video clip
    * @return Filename of most recent captured image/video 
    * @since 2.8
    */
    const TDesC& CurrentFullFileName() const;

    /**
    * Get the current video filename.
    */
    const TDesC& CurrentVideoFileName() const;

    /**
    * Returns a pointer to an array representing the images in a burst
    * capture
    * @return reference to the burst capture array
    * @since 2.8
    */
    CCamBurstCaptureArray* BurstCaptureArray() const;

    /**
    * Returns the number of non-deleted items in the array
    * @return count of non-deleted items
    * @since 2.8
    */
    TInt CurrentItemCount();
    
    /**
    * Returns the number of burst capture moments that have taken
    * place in a current burst. If no burst is active this returns
    * 0
    * @return count of burst capture moments
    * @since 3.0
    */
    TInt CurrentCapturedCount();
    
    /**
    * Returns the number of items in a timelapse sequence
    * @return count of timelapse sequence items
    * @since 3.0
    */
    TInt TimeLapseImageCount();
 
    /**
    * Resets the array of filenames used in timelapse capture
    * @since 3.0
    */   
    void ResetTimelapseArray();

    /**
    * Initiates the playing of the specified sound.
    * @since 2.8
    * @param aSoundId The enum value of the sound to play
    * @param aEnableCallback Whether we want a callback when play complete
    */
    void PlaySound( TCamSoundId aSoundId, TBool aEnableCallback );


    /** 
    * Releases the camera hardware for other apps to use
    * @since 2.8
    */
    void ReleaseCamera();

    /**
    * Increments the engine usage count
    * @since 2.8
    */
    void IncCameraUsers();

    /**
    * Decrements the engine usage count If the engine is no longer required
    * then it is released. If it is still processing, the engine count will 
    * be checked when the current process completes and the engine will be 
    * released if it is no longer needed.
    * @since 2.8
    */
    void DecCameraUsers();
    
    /*
    * Gets the handle to the static settings model, which is used inturn by the
    * plugin to call the interfaces for performing needed actions.
    */
    MCamStaticSettings& StaticSettingsModel();
    
    /*
    * Check if image/video scene mode is supported
    */
    TBool IsSceneSupported( const TInt aSceneId, TBool aPhotoScene = ETrue  ) const;

    /** 
    * Whether a call is active, including the ringing state
    * @since 2.8
    * @return ETrue if in call or ringing, else EFalse
    */
    TBool InCallOrRinging() const;

    /** 
    * Whether a video call is active, including the ringing state
    * @return ETrue if in videocall or ringing, else EFalse
    */
    TBool InVideocallOrRinging();
    
    /** 
    * Whether or not a video clip is currently being saved
    * @since 2.8
    * @return ETrue if saving, else EFalse
    */
    TBool CurrentlySavingVideo() const;

    /**
    * Returns whether all the settings list options for a particular setting
    * item are visible or not
    * @since 2.8
    * @param aSettingItem The setting item to be checked.
    * @param aRevisedResourceId Receives the id of a resource if the return
    * value is EFalse. The resource contains the list of  visible
    * options.
    * @return ETrue if all options are visible. Otherwise, EFalse.
    */
    TBool AllOptionsVisibleForSettingItem( TInt aSettingItem, 
                                                   TInt& aRevisedResourceId );

    /**
    * Switches the camera being used
    * @since 2.8
    */
    void SwitchCameraL();

    /**
    * Returns the active camera
    * @since 2.8
    * @return @return either primary, secondary or none ( if in process of switching )
    */
    TCamActiveCamera ActiveCamera() const;
    
    /**
    * Whether we currently have a camera switch required
    * @since 2.8
    * @return TCameraSwitchRequired a camera switch is required or not
    */
    TCameraSwitchRequired CameraSwitchQueued() const;

    /**
    * Check if exit is required
    * @since 2.8
    * @return TBool True if exit is required
    */
    TBool CheckExitStatus();
        
    /**
    * Informs the controller that an end key event is waiting
    * to close the app, or has been acted upon
    * This is used when the end key is pressed during sequence capture
    * @since 2.8
    */
    void SetEndKeyExitWaiting( TBool aEndKeyWaiting );

    /**
    * Returns the status of the camera slide
    * @since 2.8
    * @return the status of the camera slide
    */
    TInt CameraSlideState() const;

    /**
    * Informs the controller that a slider close event has
    * been reacted to by 'pretending' to exit
    * @since 2.8
    */
    void SliderCloseEventActioned();
    
    /**
    * Set camera switch to queue
    */
    void SetCameraSwitchRequired( TCameraSwitchRequired aSwitchRequired ); 

    /**
    * Check slide state and schedule camera switch if required
    * @since 2.8
    */
    void CheckSlideStatus();    
            

    /**
     * Return range of EV value supported for current product
     * @since 3.0
     * @return TEvRange struct
     */
    TCamEvCompRange EvRange() const;

    /**
     * Switches camera to standby
     * @since 5.1
     * @param aSratus Error code for Standby mode
     * @return
     */
    void SwitchToStandbyL( TInt aStatus = KErrNone );
    
  public: // Derived from MCamCallStateObserver base class     
    /**
    * Callback for when the current phone call state changes
    * @since 3.0
    * @param aState The new call state
    * @param aError An error code associated with the event
    */
    void CallStateChanged( TPSCTsyCallState aState, TInt aError );

    /**
    * Returns the file size of the specified file
    * @since 3.0
    * @param aFilename The file to retrieve the size of.
    * @return The size of the file or KErrNotFound
    */        
    TInt FileSize( TDesC& aFilename ) const;

    /**
    * Returns whether the current scene uses forced
    * autofocus (so no AF required at capture time)
    * @since 3.0       
    * @return ETrue if forced, EFalse if not
    */ 
    TBool CurrentSceneHasForcedFocus() const;        

public:

    /**
    * Get the actual resolution (WxH) of current image quality.
    */
    TSize GetCurrentImageDimensions() const;

    /**
    * Retrieves the current image resolution, taking into account
    * the user settings and and modifications due to zoom state.
    * @since 2.8
    * @returns The current image resolution.
    */
    TCamPhotoSizeId GetCurrentImageResolution() const;
    
    /**
    * Retrieves the current image compression
    * @since 2.8
    * @returns The current image compression.
    */
    TInt GetCurrentImageCompression() const;

    /**
    * Retrieves the current video resolution.
    * @since 2.8
    * @returns The current video resolution.
    */
    TCamVideoResolution GetCurrentVideoResolution() const;
    
    /**
    * Force use of phone memory storage
    * @since 2.8
    */
    void UsePhoneMemoryL() const; 

    /**
    * Sets the correct memory to be used based on settings 
    * and availability of MMC.
    * @since 4.0
    * @return ETrue if memory location changed
    */
    TBool CheckMemoryToUseL();
    
    /**
    * Indicate whether the phone memory is being force-used 
    * because of un-availability or errors in MMC.
    * @since 4.0
    */        		
	TBool IsPhoneMemoryForced();        

    /**
    * Retrieves the current image resolution from Settings.
    * @since 2.8
    * @returns The current image from Settings
    */
    TCamPhotoSizeId SettingsPhotoResolution() const;


    /**
    * Indication that a view has finished using the capture array
    * @since 2.8
        * @param aForcedRelease Should the array be released even if 
        *                       the apparent usage count is not null.
    */
        void ReleaseArray( TBool aForcedRelease = EFalse );
    
    /**
    * Restore Camera settings to default
    * @since 3.0
    * @param aIsEmbedded if app is embedded
    */
    void RestoreFactorySettingsL( TBool aIsEmbedded );

private:
    /**
    * Called when playback of a sound has completed
    * @since 2.8
    */
    void PlaySoundComplete();
public:

    /**
    * Non-leaving version of StartViewFinderL()
    * @since 2.8
    */
    void StartViewFinder();

    /**
    * Stop viewfinder     
    * @since 2.8
    */
    void StopViewFinder(); 

    /**
    * Stop viewfinder Ecam only
    * Needed with async video recording stopping     
    * @since 5.0
    */
    void StopViewFinderEcam(); 

    /**
    * Sets the zoom mode of the engine
    * @param aZoomMode The zoom mode to set
    * @since 3.0
    */
    // Only digital mode supported currently - no need for this
    //void SetZoomMode( const TCamZoomMode& aZoomMode );

    /**
    * Report whether a capture mode transition is currently in progress
    * @since 2.8
    * @return TBool ETrue if transition (i.e. still/video capture prepare) is 
    *  currently in progress, EFalse otherwise;
    */
    TBool CaptureModeTransitionInProgress();

    /**
    * Check if viewfinder is active
    * @since 2.8
    * @returns ETrue if viewfinding
    */
    TBool IsViewFinding() const;

    /**
    * Check if capture key needs to be released before starting 
    * new capture process. Used in post capture state to prevent
    * image capture before viewfinder is completely shown.
    * @since S60 v5.0
    * @return ETrue if waiting key release before next capture
    */
    TBool IsDemandKeyRelease();

    /**
    * Set iDemandKeyRelease value
    * @param aDemand Demand to release capture key
    * @since S60 v5.0
    */
    void SetDemandKeyRelease( TBool aDemand );

    /**
    * Stop the idle timer
    * @since 2.8
    */
    void StopIdleTimer();

    /**
    * Restart the idle timer
    * @since 2.8
    */
    void StartIdleTimer();


    /**
    * Flash modes supported by engine
    * @since 3.0
    * @return supported flash modes   
    */
    TUint32 SupportedFlashModes();
    
    
    /**
    * White balance modes supported by engine
    * @since 3.0
    * @return supported white balance modes   
    */
    TUint32 SupportedWBModes();
    
    /**
    * EV modes supported by engine
    * @since 3.0
    * @return supported EV modes    
    */
    TUint32 SupportedEVModes();

    /**
    * Capture state
    * @return TInt capture state    
    */
    TInt CaptureState(); 

    /**
    * Returns the current capture-tone id
    * @since 3.0
    * @return the current capture-tone id
    */
    TCamSoundId CaptureToneId( );

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT 
  public:
    void SetPerformanceState( TCamPerformanceState aState );
#endif        

  /**
   * Set pointer to settings plug-in. Takes ownership!
   * @param aPlugin Pointer to settings plug-in. This object
   *                takes ownership.
   * @since S60 v5.0
   */
void SetSettingsPlugin( CCamGSInterface* aPlugin );

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

protected:  // New functions

    /**
    * C++ default constructor.
    * @since 2.8
    */
    CCamAppController();

    /**
    * Symbian OS 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();

// -----------------------------------------------------
// Burst related
public:   

    /**
        * Stop burst in a soft way:
        * - if have not moved yet to long burst mode,
        *   wait until the short burst ends and do not 
        *   enter long burst mode (if aFastStop is true,
        *   stop asap when atleast 2 images captured).
        * - if long burst already entered, stop capturing
        *   after the next capture.
        * @param aFastStop Stop also short burst asap, but so that
        *                  atleast 2 images are captured.
    */
        void SoftStopBurstL( TBool aFastStop = EFalse );

    /**
     * Set iNoBurstCancel flag which is used to prevent stopping burst
     * when actual burst capture has started after focusing.
     */
     void SetNoBurstCancel( TBool aValue );
        
        /**
        * Set iKeyUPl flag which is used to prevent late 
        * activated burst timer when capturekey is up already
        * before burstcapture has started.
        */
        void SetKeyUp( TBool aValue = EFalse );
        
        /**
        * Returns amount of images captured in burst mode
        * @return Amount of captured images
        * 
        */
        TInt CapturedImages() const;

private:   

    /**
    * Start the burst timer for changing to long burst mode.
    */
    void StartBurstTimerL();

    /*
    * Stop the burst timer.
    */
    void StopBurstTimer();

    /**
    * Callback for short burst timer.
    * @param aController Pointer to app controller instance.
    * @return Always zero, implies no repeated callbacks wanted.
    */
    static TInt ShortBurstTimeout( TAny* aController );

    /**
    * Helper method for short burst timer callback.
    */
    void DoShortBurstTimeoutL();


    // -----------------------------------------------------
    private:   

    /**
    * Return number of images that can still be captured for a particular
    * image photo resolution
    * @since 2.8
    * @param aStorage storage location - defaults to current loation
    * @param aBurstActive - set to ETrue if burst mode is active
    * @param aSize - the id of the photo resolution
    * @return the number of images
    */
    TInt ImagesRemaining( TCamMediaStorage aStorage,
                          TBool            aBurstActive, 
                          TCamPhotoSizeId  aSize );

    /**
    * Change the controller's current state
    * Notifies controller observers of change
    * @since 2.8
    * @param aNewState new state code
    */
    void ChangeState( const TCamCameraState& aNewState );

    /**
    * Change the controller's target state. The state will not be
    * altered if the controller is already in shutdown mode.
    * @since 2.8
    * @param aNewState new state code
    */
    void ChangeTargetState( const TCamCameraState& aNewState );

    /**
    * Change the controller's current operation
    * Notifies controller observers of change
    * @since 2.8
    * @param aNewOperation new operation code
    * @param aError error code associated with new state (if any)
    */
    void ChangeOperation( TCamCaptureOperation aNewOperation, 
                          TInt          aError = KErrNone );

    /**
    * Notify controller observers of an event.
    * @since 2.8
    * @param aEvent event code
    * @param aError error code
    */
    void NotifyControllerObservers( TCamControllerEvent aEvent,
                                    TInt                aError = KErrNone ) const;

    /**
    * Starts the viewfinder if the engine has been prepared for the 
    * current mode.  
    * @since 2.8
    */
    void StartViewFinderL();

    /**        
    * Initiate image capture
    * @since 2.8
    * @return ETrue if the capture was initiated otherwise EFalse
    */
    TBool DoCaptureL();

    /**
    * Initiate video recording
    * @since 2.8
    */
    void DoRecord();

    /**
    * Make a local copy of the viewfinder frame to use when
    * the viewfinder is frozen
    * @since 2.8
    */
    void CopyFrameL( const CFbsBitmap& aBitmap );

    /**
    * Reserve a set of file names for the current capture
    * Store the reserved name(s) in iCaptureArray
    * @since 2.8
    * @param aCaptureMode still, burst or video mode
    * @param aForcePhoneMem force path to use Phone memory (C:)
    */
    void ReserveFileNamesL( 
           const TCamCameraMode&       aCaptureMode,
           const TCamImageCaptureMode& aImageMode,
           TBool                       aForcePhoneMem = EFalse );

    /**
    * Create a file ready for saving captured image into
    * @since 2.8
    * @param aFs: Handle to RFs
    * @param aFullPath: Full path including drive and extension
    * @return error code from file creation
    */
    TInt CreateDummyFileL( RFs& aFs, const TDesC& aFullPath );


    /**
    * Store file name and full file name (incl. path)
    * to burst capture array
    * @since 2.8
    * @param aFullPath: Full path including drive and extension
    */
    void StoreNextFileNameL( const TDesC& aFullPath );

    /**
    * Generate a new file name and full path for saving the next video
    * according to the specified mode (unless specified otherwise)
    * @since 2.8
    * @param aForcePhoneMem force path to use Phone memory (C:)
    */
    void GenerateNextVideoFilePathL( TBool aForcePhoneMem = EFalse );
    
    /**
    * Generate a new path for saving the next image
    * @since 3.0
    */
    void GenerateNextValidImagesPathL();

    /**
    * Generate a new file name and full path for saving the next video
    * according to the specified mode.  File will always be 
    * available.
    * @since 2.8      
    */
    void GenerateNextValidVideoPathL();

    /**
    * Handle a notification from the autofocus custom extension
    * @since 2.8
    */
    void HandleAutoFocusNotification( TCamExtAutoFocus aNotification );

    /**
    * Static callback function for idle timer expiry
    * @since 2.8
    * @param aPtr Pointer to an instance of the CCamAppController
    * @return TInt to indicate if function should be called again
    */
        static TInt IdleTimeoutL( TAny* aPtr );
   
        /**
    * Static callback function for time lapse timer expiry
    * @since 3.0
    * @param aPtr Pointer to an instance of the CCamAppController
    * @return TInt to indicate if function should be called again
    */     
    static TInt TimeLapseTimeoutL( TAny* aPtr );

private: // Deep sleep timer
    /**
    * Static callback function for camera deep sleep
    * @since S60 5.0
    * @param aPtr Pointer to an instance of the CCamAppController
    * @return TInt dummy
    */
    static TInt DeepSleepTimeout( TAny* aPtr );

    /**
    * Deep sleep timeout handler
    */
    void DoDeepSleepTimeout();

public: // Deep sleep timer
    /**
    * Start deep sleep timer
    */
    void DeepSleepTimerStart();
    
    /**
    * Cancel deep sleep timer
    */
    void DeepSleepTimerCancel();

private:
    /**
    * Set video length (max or short)
    * @param aLength Length of video
    * @since 2.8
    */
    void SetVideoLengthL( TInt aLength );

    /**
    * Returns the current photo storage location.
    * @return the photo storage location
    * @since 2.8
    */
    TInt CurrentPhotoStorageLocation() const;

    /**
    * Returns the current video storage location.
    * @return the video storage location
    * @since 2.8
    */
    TInt CurrentVideoStorageLocation() const;    
    
        /**
        * Returns ETrue if the supplied toneId should be silent if the profile is 
        * set to silent. Does *not* take into account network variant forcing 
        * sounds on.
        * @since 3.0
        * @param aSoundId The tone to check
        * @return ETrue if sound should always be played, else EFalse           
        */
    TBool ToneShouldBeSilent( TCamSoundId aSoundId );

        /**
        * Report to any observers that capture processing has completed
        * @since 3.0      
        */    
    void HandleCaptureCompletion();

    /**
    * Tries to autofocus, will do nothing if camera in use is not
    * primary camera, or focus range has not been set first
    * @since 3.0
    * @return Whether or not auto focus has been initiated
    */
    TBool TryAutoFocus();

    /**
    * Checks the current scene, and where required, updates the autofocus
    * mode in use.  Called when the photo scene or based-on-scene change
    * @since 3.0      
    * @param aForceAFReset Whether to force a ResetToIdle independant of scene mode
    */    
    void CheckAfModeForScene( TBool aForceAFReset = EFalse );

    /**
    * Initalises the AutoFocus interface, if needed
    * @since 3.0
    */
    void InitAutoFocusL();
    
    /**
    * Timeout to play capture tone after delay (to match
    * capture tone to actual capture event)
    * @since 3.0
    * @param aPtr TAny* ptr (cast to CCamAppController)
    * @return Error code retruned by function
    */
    static TInt CaptureToneDelayTimeout( TAny* aPtr );
    
    /**
    * Play the capture tone (called from capture delay timeout)
    * @since 3.0
    */
    void PlayDelayedCaptureTone();

    /**
    * Copy Snapshot if needed. Helper function for McaeoSnapImageReady
    * @since 3.0
    * @param aSnapshot the snapshot to copy
    * @param aError Error code KErrNone to indicate success or a
    *               standard Symbian OS error code.        
    */
    void CopySnapshotIfNeeded(const CFbsBitmap& aSnapshot, TInt aError);
    
   
    /**
    * Checks whether flash is required for taking a picture.
    * @return true if flash is required current mode
    */
    TBool FlashRequired() const;              


private:  // Functions from base classes

    /**
    * From MCamImageSaveObserver, image save has completed
    * @since 2.8
    */
    void HandleSaveEvent( TCamSaveEvent aEvent );
    
    /**
    * From MCamImageSaveObserver, image save has completed
    * @since 2.8
    */
    void HandleFileHarvestingComplete();

    /**
    * Idle timeout expiry - invoke switch to standby mode
    * @since 2.8
    * @return TInt to indicate if function should be called again
    */
    TInt DoIdleTimeoutL();

    /**
    * Time lapse timeout expiry - update the remaining time countdown
    * or initiate the next capture if the full period has expired
    * @return TInt to indicate if function should be called again
    */
    TInt DoTimeLapseTimeoutL();

    /**
    * Force the application to use phone memory for Video and Image saving.
    * Typically called when attempting to access MMC fails.
    * @since 2.8
    */        
    void ForceUsePhoneMemoryL( TBool aEnable = ETrue );

    /**
    * Complete the burst operation
    * @since 2.8
    */      
    void CompleteBurstOperation();
    
    /**
    * Complete the time lapse operation
    * @since 3.0
    */  
    void CompleteTimeLapseOperation();


    /**
    * Whether or not the engine is currently processing a capture
    * still, video or sequence.
    * @since 2.8
    * @return ETrue if the engine is performing a capture operation
    */
    TBool EngineProcessingCapture() const;

    /**
    * Handle an arriving call. Stops video and cancels sequence capture
    * in early stages.
    * @since 2.8
    */
    void HandleIncomingCall();

    /**
* Cleans up the controller active camera state
    * @param aAny Pointer to the object which is the target 
    * of the cleanup operation
    * @since 2.8
    */
    static void Cleanup( TAny* aAny );
    
    /**
    * Immediately cancels a still capture at any stage.
    * @since 2.8
    */
    void CancelStillCaptureNow();
    
    /**
    * Attempt to cleanup capture array if burst capture fails
    * @since 2.8
    * @param aImageCountDelivered number of images delivered
    * @param aError error returned from burst capture operation
    */
    void TidyCaptureArray( TInt aImageCountDelivered, 
                                    TInt aError );
   
    /**
    * Load 2ndary caera specifc settings (on construction or camera switch)
    * @since 3.0
    */
    void LoadSecondaryCameraSettingsL();

  // -------------------------------------------------------
  // Orientation sensor / rotation related
public:

    /**
    * Get current orientation to be used on image rotation.
    * Always ECamOrientation0 if rotation setting is OFF.
    * @return Current image orientation value.
    */
    TCamImageOrientation ImageOrientation() const;
    
    /**
      *  Checks if mass storage exist in current phone.
      *  @ return True if Mass storage exist
      */
     TBool ExistMassStorage() const;

    /**
      *  Checks if a memory type is available on the phone.
      *  @param aStorage TCamMediaStorage enum specifiying the type of storage;
      *  	default=ECamMediaStorageCurrent
      *  @return ETrue if the Memory type is available
      */
     TBool IsMemoryAvailable( const TCamMediaStorage aStorage = 
        ECamMediaStorageCurrent, TBool aIgnoreUsbPersonality = EFalse ) const;
    
  public:

    /**
    * Update the sensor api object
    * @since 3.0
    * @param aStartupApi eTrue if we need to recreate the Api
    */
    void UpdateSensorApiL( TBool aStartupApi );
    
    /**
    * Forces a refresh of the local copy of the back slide status
    * @since 2.8
    */
    void RefreshSlideStatus();

  private:

    /**
    * From MSensrvDataListener receive Sensor Data
    *
    * Indicates data is available in the receiving buffer. A client can read 
    * the data through GetData()-function in the related channel object. Data 
    * is valid until the data received notification occurs again.
    *
    * @since 5.0
    * @param[in] aChannel Reference to the related channel object
    * @param[in] aCount Data object count in receiving buffer.
    * @param[in] aDataLost Number of lost data items. Data loss can occur if
    *     the client does not request new data from server fast enough. This 
    *     can happen
    *     when system is under heavy load and the client process has lower
    *     priority than sensor server process. If data loss is a problem, 
    *     consider using higher object count in data listening, which will reduce 
    *     number of expensive IPC calls used in data transmission.
    */
    void DataReceived( CSensrvChannel& aChannel, 
                       TInt aCount, 
                       TInt aDataLost );
    /**
    * From MSensrvDataListener receive data error notice
    *
    * Data listening failed. 
    * If error was fatal, channel has also been closed and sensor server session 
    * terminated. If error was minor, some data has potentially been lost.
    *
    * @since 5.0
    * @param[in] aChannel Reference to the related channel object
    * @param[in] aError Error code.
    */
    void DataError( CSensrvChannel& aChannel, 
                    TSensrvErrorSeverity aError );

    /** 
    * From MSensrvDataListener
    *
    * Returns a pointer to a specified interface - to allow future extension
    * of this class without breaking binary compatibility
    *
    * @since S60 5.0
    * @param aInterfaceUid Identifier of the interface to be retrieved
    * @param aInterface A reference to a pointer that retrieves the specified interface.
    */
    void GetDataListenerInterfaceL( TUid aInterfaceUid, 
                                    TAny*& aInterface );
                                                    
    /**
    * Set image rotation parameters
    * @since 3.0
    */
    void SetImageOrientationL();

    /**
    * Mapping from Sensor orientation data type to our own
    * image orientation type.
    * @param aSensorOrientation Orientation Sensor device orientation value.
    * @return Camera orientation value.
    */
    static TCamImageOrientation MapSensorOrientatio2CamOrientation( 
        const TSensrvOrientationData::TSensrvDeviceOrientation& aSensorOrientation, 
        TCamImageOrientation aLastImageOrientation, 
        TCamActiveCamera aActiveCamera );
        
    /**
    * Mapping from camera orientation data type to bitmap orientation type.
    * @param camera orientation value.
    * @return CBitmapRotator::TRotationAngle.
    */
    static CBitmapRotator::TRotationAngle MapCamOrientation2RotationAngle( 
        const TCamImageOrientation aOrientation );        
    
  // -------------------------------------------------------

  private:        
   
        /**
        * From MPropertyObserver Handle change in a watched property
        * @since 2.8
        * @param aCategory The type of property
        * @param aKey The property that has changed
        */
        void HandlePropertyChangedL( const TUid& aCategory, const TUint aKey );
          
  private:        

        /**
         * Callback function that is called when lens cover state changes.
         * Used to implement delayed handling of lens cover events.
         * @param aPtr Pointer to CCamAppController
         * @return 
         */
        static TInt LensCoverUpdateL( TAny* aPtr );
  
        /**
        * Handles the slide closed event
        * @since 2.8
        */
        void HandleSlideClosedL();
        
        /**
        * Handles the slide open event
        * @since 2.8
        */
        void HandleSlideOpenedL();        

 
    private: // Functions from base classes

        /**
        * From MCamObserver
        * Receives event codes from observables
        * @since 3.0
        * @param aEvent The event code
        */
        void HandleObservedEvent( TCamObserverEvent aEvent );

        /**
        * Returns whether or not the current profile is silent
        * @since 3.0
        * @return ETrue if current profile is silent
        */
        TBool IsProfileSilentL();      

         /**
        * Static callback function for backlight timer reset
        * since 3.0
        * @param aPtr Pointer to an instance of the CCamAppController
        * @return TInt to indicate if function should be called again
        */
        static TInt ResetInactivityTimer( TAny* aPtr );
        
        /**
    * Enable or disable the system screensaver
    * @since 3.0
    * @param aEnable ETrue if the screensaver is enabled
    *           else EFalse
    */  
    void EnableScreenSaver( TBool aEnable );

    /**
     * Static callback function for timer callback for closing 
     * location trail.
     * @since S60 v5.0
     * @param aPtr Pointer to an instance of the CCamAppController
     * @return TInt to indicate if function should be called again
     */             
     static TInt RetryStopLocationTrail( TAny* aPtr );   
     
    public:
        void StartLocationTrailL();
        void StopLocationTrail( TBool aCloseSession = EFalse );

    public:
        /**
        * Returns a pointer to flash status object. Never NULL if
        * ConstructL has been executed successfully.
        * @return flashStatusPointer
        */
        CCamFlashStatus* FlashStatus() const;        
        
        /**
        * Returns false, if a picture cannot be taken, due to flash being
        * required and recharged, true otherwise.
        * @return flashstatus
        */
        TBool CheckFlash() const;
        
        /**
        * Called when about to switch to burst or timelapse mode. Stores
        * the current flash mode and scene.
        */
        void SaveFlashMode();
        
        /**
        * Called when switching to single image capture mode. Restores
        * flash mode, if using same scene mode as was used when the
        * flash mode was stored. Otherwise clears the stored info.
        */
        void RestoreFlashMode();
        
     public:
        /**
        * Returns a pointer to dynamic Configuration Manager.
        *
        * Never NULL if ConstructL has been executed successfully.
        * @return Pointer to Configuration Manager
        */
        CCamConfiguration* Configuration() const;


        /**
        * Returns whether or not the keylock is on
        * @since 5.0
        * @return ETrue if keylock is on, otherwise EFalse
        */
        TBool IsKeyLockOn();
        
        /**
        * Used to get whether user is sliding in the self timer preview.
        * @return ETrue if user is sliding in the self timer preview.
        */
        inline TBool IsSlideInSelfTimerPreview() const { return iSlideInSelfTimerPreview; };

        /**
        * Used to set whether user is sliding in the self timer preview.
        * @param aValue the value to indicate whether user is sliding in the self timer preview.
        */
        inline void SetSlideInSelfTimerPreview( TBool aValue ) { iSlideInSelfTimerPreview = aValue; };

        /**
        * Used to get the current storage.
        * @return the current storage.
        */
        TCamMediaStorage CurrentStorage();
        
        /**
        * Returns whether autofocus is still needed before capturing.
        * @return ETrue if it's necessary to try autofocus one more time before the capture.
        */
        TBool IsAfNeeded();
        
        /**
        * Sets an autofocus needed flag, so that autofocus is done after cancel and range set are finished. 
        */
        void SetAfNeeded( TBool aAfNeeded );
        
        /**
        * Returns whether recording was stopped because USB was connected.
        * @return ETrue if video recording was stopped because USB was connected.
        */
        TBool IsCaptureStoppedForUsb();

        /**
        * set the flag of iSetCaptureStoppedForUsb.
        */
        void SetCaptureStoppedForUsb( TBool aCaptureStoppedForUsb );
		
        /**
        * Cancels the memory card dismount monitoring
        */
        void CancelDismountMonitoring();
        
        /**
        * Start the memory card dismount monitoring
        */
        void StartDismountMonitoring();
        
        /**
        * Returns whether all snapshots have been received in burst mode
        * @return ETrue if all snapshots have been received in burst mode.
        */
        TBool AllSnapshotsReceived();
        
        /**
        * Returns the last made autofocusrequest type or 0, if there are
        * no pending autofocus requests.  
        */
        TInt PendingAFRequest();
        
        /**
		* Calculates the remaining video recording time based on video
		* quality settings (instead of getting the value from camera
		* controller)
		* @param aStorage TCamMediaStorage enum specifying the type of storage;
		*	 default=ECamMediaStorageCurrent
		* @return remaining time
		*/       
		TTimeIntervalMicroSeconds 
			CalculateVideoTimeRemainingL( TCamMediaStorage 
										aStorage = ECamMediaStorageCurrent);
        /**
        * Sets orientation of the application to CCamera
        * 
        */
        void SetCameraOrientationModeL( TInt aOrientation );
        
        /**
        * Final part in constructing cameracontroller
        * 
        */
        void CompleteCameraConstructionL();
        
        /**
         * Returns index of current image.
         */
        TInt CurrentImageIndex();
        
        /**
         * Sets the value defining the need for CAE video init and prepare
         */
        void SetVideoInitNeeded( TBool aVideoInitNeeded );

        /**
         * Returns the value defining the need for CAE video init and prepare
         */
        TBool VideoInitNeeded();
        
        /**
         * Stores the UserScene settings
         */
        void StoreUserSceneSettingsL();
 
        /**
         * Toggles between highest 16:9 and 4:3 quality levels.
         */
        TBool ToggleWideScreenQuality( TBool aWide );             

        /**
         * Returns the value of iSaveStarted
         */
        TBool IsSaveStarted();
        
        /**
         * Getter for iIssueModeChangeSequenceSucceeded
         */
        TBool IssueModeChangeSequenceSucceeded();
		
        /**
         * Initiates startup sequence in embedded mode 
         */
        void EmbeddedStartupSequence();
        
        /**
         * Called by the snapshotrotator to indicate when snapshot
         * is rotated and ready for drawing
         */
        void SnapshotRotationComplete();

  private:

    // -----------------------------------------------------
    // Construction parts

    void ConstructCameraControllerL( TInt aCameraIndex );

    // -----------------------------------------------------
    // Helper utility methods  
    TInt GetCriticalMemoryLevelL( const TCamMediaStorage& aStorage );

    // -----------------------------------------------------
    // Events and requests handling  
    void EventHandlingErrorRecovery( TInt aError );

    void HandleSequenceEventL( TInt              aStatus, 
                               TCamCameraEventId aEventId, 
                               TAny*             aEventData );

    void HandleRequestEventL( TInt              aStatus, 
                              TCamCameraEventId aEventId, 
                              TAny*             aEventData );

    void HandleSettingEventL( TInt              aStatus, 
                              TCamCameraEventId aEventId, 
                              TAny*             aEventData );

    void HandleImageCaptureEventL( TInt             aStatus, 
                                   CCamBufferShare* aShare  );
    void HandleImageStopEventL( TInt aStatus, 
                                TInt aFullCaptureCount  );

    void HandleViewfinderFrame( TInt aStatus, CFbsBitmap* aFrame );
    void HandleSnapshotEvent( TInt aStatus, CFbsBitmap* aBitmap );
    void HandleVideoAsyncStopEvent( TInt aStatus );
    void HandleVideoStopEvent( TInt aStatus );

    void IssueModeChangeSequenceL( TBool aStartup = EFalse );
    void GenerateModeChangeSequenceL( RCamRequestArray& aSequence );
    void GenerateStartupSequenceL( RCamRequestArray& aSequence );
    
    void IssueRequestL( const TCamCameraRequestId& aId );
    void IssueDirectRequestL( const TCamCameraRequestId& aId );
    void IssueSettingsChangeRequestL();

    void ProceedPendingOrNotifyReadyL();

    void DoVideoNameRetryL( TInt aStatus );
    
    /**
    * Sets the current image/video settings from UI to camera, by calling
    * RestoreSettingIfNecessaryL for each supported setting, and starting
    * the setting process if any of them needs setting.
    * This function needs to be called after camera release and switches
    * between image/video mode.
    */    
    void RestoreSettingsToCameraL();
    
    /**
    * Checks if the setting needs restoring (UI setting value doesn't match
    * the actual setting value in camera).  In that case, function adds
    * this setting to the list of pending settings.
    * @param aSettingItem Setting item id
    */    
    void RestoreSettingIfNecessaryL( TCamSettingItemIds aSettingItem );

    /**
    * Checks if the setting needs restoring (UI setting value doesn't match
    * the actual setting value in camera).  In that case, function 
    * directly set settting to camera
    * @param aSettingItem Setting item id
    */ 
    void DirectRestoreSettingIfNecessaryL( TCamSettingItemIds aSettingItem );

    /**
    * Set internal state based on camera event.
    */
    void SetStateFromEvent( TCamCameraEventId aEventId );

    void InitCaptureLimitL();
    
    static TCamCameraRequestId 
           NextRequestL( const TCamCameraState& aFrom, 
                         const TCamCameraState& aTo   );
    
    static TCamCameraEventId 
           ResponseForRequest( const TCamCameraRequestId& aRequestId );
           
    
        /**
    * Called when mmc is about to be dismounted
    */       
    TInt DriveChangeL( TCamDriveChangeType aType );
    
    /**
    * Rotate the snapshot copy before thumbnail creation
    */
    void RotateSnapshotL();
    
    /**
    * Handle change in image quality setting.
    * Handle stopping viewfinder, re-preparing quality and
    * restarting viewfinder if viewfinder is running.
    * Notify observers about quality change.
    */
    void HandleImageQualitySettingChangeL();
    
    /**
    * Handle change in video quality setting.
    * Handle stopping viewfinder, re-preparing quality and
    * restarting viewfinder if viewfinder is running.
    * Notify observers about quality change.
     */
    void HandleVideoQualitySettingChangeL();

  // =======================================================
  private:    // Data
    TBool  iValueIsStored;
    CCamCameraController*        iCameraController;
    CCamSettingProvider* iSettingProvider;

    TUint                        iBusyFlags; // TCamBusyFlags values    
    TCamAppControllerInfo        iInfo;

    // Temporarily stop notifications to observers
    TBool                        iMuteNotifications; 

    // Observers of controller events
    RPointerArray<MCamControllerObserver> iControllerObservers;
    TInt iCameraReferences;

    // The settings model for dynamic and static data.
    // CCamSettingsModelBase* iSettingsModel;
    MCamSettingsModel*         iSettingsModel;
    MCamSettingPreviewHandler* iSettingsPreviewHandler; // Currently same object as iSettingsModel.
    RArray<TCamSettingItemIds> iPreviewRollbacks;       // Used when cancelling previews.

    // Used to indicate whether user is sliding in the self timer preview.
    TBool iSlideInSelfTimerPreview;

    TBool iVideoPrepareNeeded;       // Video re-prepare needed due to setting change

    // Boolean to record if view finder should be frozen at next frame
    TBool iFreezeViewFinder;
    // Boolean to indicate that the frozen frame should be updated
    TBool iUpdateFrozenFrame;

    // name for next file, including path and extension
    TFileName iSuggestedVideoPath;
    // Used to indicate that the media storage has been changed tbe
    // pathnames need to be checked
    TCamMediaStorage iSetNewPathnamesPending;
    // Counters used for creating multiple month folders 
    TInt iMonthFolderCounters[CCamFolderUtility::ECamFolderTypeLast];
    // Counter for image number. 
    // Stored locally to avoid performance problems during burst.
    TInt iImageNumberCache;

    // Whether or not the engine is initialized for video recording
    TBool iVideoRecorderInitialized;

    // estimate of record time elapsed
    TTimeIntervalMicroSeconds iVideoTimeElapsed;
    // estimate of record time remaining
    TTimeIntervalMicroSeconds iVideoTimeRemaining;

    // Whether or not a capture mode transition is in progress
    TBool iCaptureModeTransitionInProgress;

    // Active object for saving images to file asynchronously
    CCamImageSaveActive* iImageSaveActive;
    // Array containing data that represents the images in a burst capture
    CCamBurstCaptureArray* iCaptureArray;
    // Index into burst capture array used to determine current image for
    // SnapshotImage(), CurrentImageName(), CurrentFullFileName() and
    // DeleteCurrentFile(). Set by call to SetAsCurrentImage().
    // Single capture and video always use index 0.
    TInt iCurrentImageIndex;                  
    // Filenames for a timelapse sequence used for the delete sequence option
    CDesCArray* iSequenceFilenameArray; 
    // Tracks usage of the capture array, for timely release of memory
    TInt iArrayUsageCount;
    // Capture count requested.
    // Used in burst and self-timer+timelapse modes.
    TInt iRequestedCaptureCount;
    // Indicates wheter autofocuscancel requests should be handled or not..
    // Used in CancelFocusAndCapture.
    TBool iNoBurstCancel;
        
        // Indicates if capturekey has been raised or not. This is used to
        // determine if bursttimer should be activated or not.
        TBool iKeyUP;

    // Timer to control when short burst is changed to  press and hold burst.
    CCamTimer* iBurstTimer;   
    // Timer for switch to standby mode
    CCamTimer* iIdleTimer;
    // Timer for initiating Time Lapse captures
    CCamTimer* iTimeLapseTimer;
        
    // Status variable for location trail
    TBool iLocationTrailStarted;
        
    // Timer for stopping location trail 
    CCamTimer* iLocationTrailTimer;
        
	// Whether or not a capture key pressed during image saving
    TBool iCaptureKeyPressedWhileImageSaving;
    // flag to indicate current VF status
    TBool iViewFinding;
    // timer to keep backlight active while VF is running
	CPeriodic* iBacklightTimer;
    // Class for playing sounds
    CCamAudioPlayerController* iSoundPlayer;
    CRepository* iRepository;
    TInt iDiskCriticalLevel;        // critical threshold for card storage
    TInt iRamDiskCriticalLevel;     // critical threshold for phone memory storage
    // Whether we are currently in a call or ringing
    TBool iInCallOrRinging;
    // Keep track of whether we are currently saving a video clip or image
    TBool iSaving;
    TBool iSaveStarted;
    // Keep track of whether a ReleaseEngine call has been made while an Init call has been made
    TBool iPendingRelease;
    TBool iEndKeyWaiting;
    TBool iCompleteTimeLapsePending;
    TBool iAutoFocusRequested;
    TBool iCaptureRequested;
    TBool iVideoRequested;
    TBool iAfNeeded;

    TBool iDemandKeyRelease; // Demand to release capture key

    // Keep track of the camera that the next switch will activate
    TInt iNextSwitchIsToPrimaryCamera;

    // Flag for queued Camera switch operation
    TCameraSwitchRequired iCameraSwitchRequired;  
    
    TBool iForceUseOfPhoneMemory;
    
    // Keeps track of the number of times we have attempted to set the video name
    TInt iVideoNameRetries;

    // Specified whether an image save request is pending. That is,
    // we are waiting for the engine to return with an image that can be saved.
    // Required to let post-capture know whether an image has been saved yet.
    TBool iImageSaveRequestPending;

    // Keeps track of the camera slider state so that
    // we don't have to keep accessing the shared data file
    TInt iSlideState;
    TInt iNewSlideState;
    
    // callback used for delayed slider update
    CPeriodic* iSliderCallBack;
    
    // Slider event flag
    TBool iSliderCloseEvent;        

    // The period between each timelapse capture 
    TTimeIntervalMicroSeconds iTimeLapseInterval;
    // The time at which the previous timelapse capture was initiated
    TTime iTimeLapseStartTime;



  	CSensrvChannel*       iAccSensorChannel;
  	TBool                 iAccSensorListening;
  	TBool                 iAccSensorInfoInitialized;
    // Flag to allow delayed re-preparation of engine
    TBool                 iOrientationChangeOccured;

    // Outside flags, as available for Camera Controller at all times.
    TCamImageOrientation  iImageOrientation;
    TCamImageOrientation  iLastImageOrientation;
    
    // Property watcher to report changes in the slide state
    CCamPropertyWatcher* iSlideStateWatcher;
    // Property watcher to report changes in the keylock state
    CCamPropertyWatcher* iKeyLockStatusWatcher;
    // Set to ETrue if shutter sound always played
    // otherwise playing shutter sound depends on
    // current profile
    TBool iShutterSndAlwaysOn;
    // Active object to listen to the call state.
    CCamCallStateAo* iCallStateAo;   
    // Organises notifications to observers of capture completion    
    CCamObserverHandler* iCaptureCompletionObserverHandler;
    TInt iActualBurst;
    CCamTimer* iCaptureToneDelayTimer;
    // Whether or not the video was stopped due to the phone app
    // having the foreground
    TBool iVideoStoppedForPhoneApp;        
    // used to simulate the receipt of a viewfinder frame when 
    // timelapse capturing.
    CFbsBitmap* iSnapShotCopy;
    // estimated snapshot size in bytes - used when monitoring free
    // RAM during burst capture
    TInt iSnapshotDataSize;
    // array of estimated jpeg sizes in bytes for different image
    // resolutions - used when monitoring free RAM during burst capture
    RArray<TInt> iJpegDataSizes;
    // array of estimated post capture sizes required in bytes for
    // different image resolutions - used when monitoring free RAM
    // during burst capture
    RArray<TInt> iSequencePostProcessDataSizes;        
    // Array of CameraEventInterested observers which wanted to be camera
    // observers when camera wasn't yet created. When camera is created
    // Observers from this array should be attached first and array should
    // be cleared.
    RArray<const MCamCameraObserver*> iPendingObserversArray;
    RArray<TUint> iCameraEventInterested;

     // Used to wake the camera hardware approximately 2-3 seconds before 
    // a timelapse capture
    TBool iCameraWoken;
    TBool iZoomWaitingForCamera;
    TBool iNewZoomPending;
    TInt iDesiredZoomVal;
    
    TBool iSequenceCaptureInProgress;
    TInt  iLongSequenceLimit;
    
    // Boolean to determine whether filename has already been
    // reserved for the image being currently captured. Reserving 
    // is done when either the image's snapshot snapshot or
    // image data arrives. (But not again when the other one comes)
    TBool iFilenameReserved;
    TCamEvCompRange iEvRange;
        
#if defined(RD_MDS_2_5) && !defined(__WINSCW__)
    RLocationTrail iLocationUtility;
    TBool iLocationUtilityConnected;
#endif // __WINSCW__

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT 
    TCamPerformanceState iPerformanceState; 
#endif        

    TBool iAppUiAvailable;
    // If this boolean is set to true, the next focus successful
    // or focus failed event should be ignored as it's resulted by
    // moving lens to hyperfocal position when cancelling AF.
    TBool iAFCancelInProgress;
    // Holds last made autofocusrequest type or 0, if there are no
    // pending autofocus requests.
    TInt iPendingAFRequest;
    // AFrequest currently in processing.    
    TInt iCurrentAFRequest;
    CCamFlashStatus* iFlashStatus;
    TBool iFlashModeSaved;
    TCamSceneId iSavedFlashScene;
    TCamFlashId iSavedFlashMode;
    TBool iSettingsRestoreNeeded;
    CCamConfiguration*  iConfiguration; //< Configuration Manager. *owned*    
    CCFClient* iContextFwClient;
    TBool iDefineContextSuccessful;
    TBool iStorageChangeProcessOngoing;
    // own.
    CCamGSInterface* iPlugin;
    TBool iSilentProfile;
    TInt iTimeLapseCaptureCount;
    RFs iFs;
    CCamDriveChangeNotifier* iDriveChangeNotifier;
    TCamMediaStorage iCurrentStorage;
    TCamMediaStorage iPreferredStorageLocation;
    TCamMediaStorage iForcedStorageLocation;
    TCamMediaStorage iInitialVideoStorageLocation;
    TBool iCaptureStoppedForUsb;
    TBool iDismountPending;
    TBool iAllSnapshotsReceived;
    CCamTimer* iDeepSleepTimer; // own
    TBool iDeepSleepTimerExpired;
    // not owned by camappcontroller
    CCameraUiConfigManager* iConfigManager;
    TBool iVideoInitNeeded; // Used to decide if the need to call CAE video init
    CCamSyncRotatorAo* iRotatorAo;
    CFbsBitmap* iRotatedSnapshot;
    CCamBurstCaptureArray* iRotationArray;
    CCamSnapShotRotator* iSnapShotRotator;
    // orientation at capture time
    TCamImageOrientation  iCaptureOrientation;
    
    TBool iIssueModeChangeSequenceSucceeded;
    TBool iSnapshotRedrawNeeded;
    
    };

#endif      // CAMAPPCONTROLLER_H
            
// End of File
