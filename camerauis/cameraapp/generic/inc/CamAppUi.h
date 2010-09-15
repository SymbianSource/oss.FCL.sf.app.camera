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
* Description:  App UI class for cameraapp
*
*/


#ifndef CAMAPPUI_H
#define CAMAPPUI_H

// ===========================================================================
//  INCLUDES
#include "CamAppUiBase.h"
#include "CamNewFileService.h"
#include "CamSelfTimerObserver.h"  
#include "CamVolumeKeyObserver.h"
#include "mcamcameraobserver.h"
#include "CamFileCheckAo.h"

#include "MCamAddToAlbumObserver.h"

class CFbsBitGc;
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CCamWaitDialog;
class CCamTimerBase;
class CCamNaviCounterControl;
class CCamNaviCounterModel;
class CCamNaviProgressBarControl;
class CCamNaviProgressBarModel;
class CCamSelfTimer;
class MCamBurstModeObserver;
class CDocumentHandler;
class CTextResolver;
class CCamSidePane;
class CCamZoomPane;
class MCamResourceLoader;
class CCamTimer;
class CIdle;
class CAknStylusPopUpMenu; 

class CGSPluginInterface;
class CGSTabHelper;
class CCamGSInterface;

class CAknGlobalNote;

//class CASpaAppLauncherActionData;
//class CSFIUtilsAppInterface;    // InCall Send interface
class CCamPropertyWatcher;
class CRepository;
class MActivePalette2UI;
class CCamActivePaletteHandler;
class CAknLocalScreenClearer;
class CAknStaticNoteDialog;
class CCamCollectionManagerAO;
class CCamStillPreCaptureView;
class CCamVideoPreCaptureView;
class CCamMemoryMonitor;
//FTU message display class
class CCamFtuDisplay;
class CCamStartupLogoController;


// ===========================================================================
// CLASS DECLARATION

/**
*  AppUi class for Camera
*
*  @since 2.8
*/
class CCamAppUi : public CCamAppUiBase,
                  public MCamSelfTimerObserver,
                  public MCamControllerObserver,
                  public MCamVolumeKeyObserver,
                  public MCamFileCheckObserver,
                  public MCamCameraObserver
                  ,public MCamAddToAlbumObserver
                  ,public MAknServerAppExitObserver
  {
  // =======================================================
  // Data Types
  private: 
        
    // enumeration to describe the type of view switch required when 
    // the capture mode or view state changes
    enum TCamViewSwitch
        {
        ECamViewSwitchNone,    // no switch required
        ECamViewSwitchModeOnly,// only capture mode or view state need updating
        ECamViewSwitch         // full view switch required including state 
                               // and mode update
        };

    /**
     * Enumeration used for the state of delayed postacapture views
     * construction. Only used if CAMERAAPP_DELAYED_POSTCAPTURE_CREATION is
     * enabled.
     */
    enum TCamPostCapConstruction
        {    
        ECamConstructStillPostCap,
        ECamConstructBurstPostCap,
        ECamConstructVideoPostCap,
        ECamPostCapsDone
        };
    
    enum TSoftkeyIndex
        {
        ESoftkeyIndex1 = 0,
        ESoftkeyIndex2 = 2
        };
      
    enum TPendingViewSwitchType 
        {
        EPendingViewSwitchNone = 0,
        EPendingViewSwitchNormal,
        EPendingViewSwitchDeactivateFirst
        };

    /**
     * Memory card status
     */
    enum TCamMemoryStatus
        {
        ECamMemoryStatusOK,
        ECamMemoryStatusFull,
        ECamMemoryStatusInternalFull,
        ECamMemoryStatusCardNotInserted,
        ECamMemoryStatusCardReadOnly,
        ECamMemoryStatusCardLocked,
        ECamMemoryStatusMassStorageFull
        };

  // =======================================================
  // Methods 

  // -------------------------------------------------------
  // Constructors and destructor
  public:  

    /**
    * C++ constructor.
    * @since 2.8
    * @param aController Reference to CCamAppController
    * @return Pointer to CCamAppUi
    */
    CCamAppUi( CCamAppController& aController );

    /**
    * Perform the second phase construction of a CCamAppUi object.
    * @since 2.8
    */
    void ConstructL();

    /**
    * Destructor.
    */
    virtual ~CCamAppUi();
  
  // -------------------------------------------------------
  // From MCamCameraObserver
  public:

    /**
    * Handle camera event.
    *
    * @param aStatus    Status code describing the event status.
    * @param aEventId   Identifier for the event.
    * @param aEventData Event associated data. If such data is provided with
    *                   the event, it is mentioned in the documentation of
    *                   particular event id.
    *                   Ownership not transferred to observer.
    */
    void HandleCameraEventL(
        TInt aStatus, 
        TCamCameraEventId aEventId, 
        TAny* aEventData = NULL );   

  // -------------------------------------------------------
  // From MCamFileCheckObserver
  public:

    /**
    * Callback to notify that file checking operation has completed.
    * @param aStatus Number of files that were missing
    * @param aError  Error code
    */
    void FileCheckingCompleteL( TInt aStatus, TInt aError );   

    // -------------------------------------------------------
    // New functions
public: 
    /*
     * HyperlinkCallback
     * static methode  to be called from TCallback
     */

    static TInt HyperlinkCallback( TAny* aAny );
    
    /**
     * CamFtuDisplayL
     * Check if camera is launching for first time ,
     * accordingly FTU Message will be displayed.
     */
    void CamFtuDisplayL();
  
     /**
	  * OpenSettingView
	  * Non static public method , to launch the settings view
	  */
    void OpenSettingView();

    /**
     * Returns whether or the toolbar has been activated/visible
     * Will return ETrue when when the toolbar is active/visible
     * @since 3.2
     * @return ETrue if toolbar is active/visible, else returns EFalse
     */
    virtual TBool IsToolBarVisible() const;

        /**
        * Check if given view state indicates that "internal",
        * CCamViewBase inherited view is in question.
        */
        TBool IsInternalView( TCamViewState aViewState ) const;

        /**
        * Set status of standby mode.
        */
        void SetStandbyStatus( TInt aStatus );

        /**
        * Get status of standby mode.
        */
        TInt StandbyStatus() const;

        /**
         * returns ETrue if the stsnby status is recoverable
         */
        TBool IsRecoverableStatus();
        
        /**
         * Change the standby status
         */
        TBool ChangeStandbyStatusL( TInt aError );
        
        /**
        * returns Getter for iPreCaptureMode
        */
        TCamPreCaptureMode PreCaptureMode();
        
        /*
        * Check for file name deletion/renaming outside camera
        */
        void StartFileNameCheck();
             
        /**
        * Determines whether or not all construction has been completed
        * Used on view activation to determine if the view should
        * open in standby mode. This will happen if the app is waiting for
        * an orientation switch to complete or for the initial view to be set
        * when launched in embedded
        * @since 3.0
        * @return ETrue if all construction has completed, else EFalse
        */   
        TBool IsConstructionComplete() const;

        /**
        * Set title pane text from a resource.
        * @since 2.8
        * @param aResourceId resource to read from
        */
        void SetTitleL( TInt aResourceId );

        /**
        * Set title pane text from a descriptor.
        * @since 2.8
        * @param aText title pane text
        */
        void SetTitleL( const TDesC& aText );

        /**
        * In embedded mode set title to name of embedding application
        * @since 2.8
        */
        void SetTitleEmbeddedL();

        /**
        * Push navi pane indicators on to navi pane stack.
        * @since 2.8
        */
        void PushNaviPaneCounterL();

        /**
        * Push navi pane progress bar on to navi pane stack.
        * @since 2.8
        */
        void PushNaviPaneProgressBarL();

        /**
        * Push default navi pane on to navi pane stack.
        * @since 2.8
        */
        void PushDefaultNaviPaneL();

        /**
        * Call DisplayDeleteNoteL on the current view
        * @return whether the delete was performed
        * @since 2.8
        */
        TBool DisplayDeleteNoteL();

        /**
        * Enable or disable self-timer functionality
        * @since 2.8
        * @param aEnable The selftimer functionality to use
        */
        void SelfTimerEnableL( TCamSelfTimerFunctions aEnable );

        /**
        * Returns current self-timer state
        * @since 2.8
        * @return Returns whether or not Self Timer is currently enabled.
        */
        TBool SelfTimerEnabled() const;

        /**
        * Returns pointer to self-timer
        * @since 2.8
        * @return pointer to CCamSelfTimer object.
        */
        CCamSelfTimer* SelfTimer();

        /**
        * Registers an observer for changes in burst mode activation
        * @since 2.8
        * @param aBurstModeObserver the object observing burst mode changes
        */
        void AddBurstModeObserverL( MCamBurstModeObserver* aBurstModeObserver );

        /**
        * Removes an observer for changes in burst mode activation
        * @since 2.8
        * @param aBurstModeObserver the observer to remove
        */
        void RemoveBurstModeObserver( const MCamBurstModeObserver* aBurstModeObserver );

        /**
        * Returns whether or not burst capture mode is enabled.
        * Will return ETrue when burst mode is enabled for still capture
        * even if the application is currently in video capture mode
        * @since 2.8
        * @return ETrue if burst mode is enabled, else returns EFalse
        */
        TBool IsBurstEnabled() const;

        /*
        * Gets the handle to the controller, which is used inturn by the
        * plugin to call the interfaces for performing needed actions.
        */
        MCamAppController& AppController();
        
        /*
        * Gets the handle to the settings model, which is used inturn by the
        * plugin to call the interfaces for performing needed actions.
        */
        MCamStaticSettings& StaticSettingsModel();
        
        /**
        * Returns the currently enabled burst mode.
        * @since 3.0
        * @return the currently enable burst mode or ECamImageCaptureNone if 
        * burst is not enabled
        */
        TCamImageCaptureMode CurrentBurstMode() const;

        /**
        * Sets the current capture mode ( and target capture mode )
        * @since 2.8
        */
        void SetCaptureMode(
            TCamCameraMode aMode, 
            TCamImageCaptureMode aImageMode = ECamImageCaptureNone );

        /**
        * Changes the state of sequence capture
        * @since 3.0
        * @param aImageMode Single/Multi/Timelapse
        * @param aReprepare Whether or not the engine should be reprepared with sequence
        * settings (Do not reprepare if the engine is about to be deleted/released)
        * @param aUpdateUi  Whether or not to do Ui update related things. Those are not
        * needed/allowed if this method is called just to return sequence mode setting
        * to default value (single capture).
        */ 
        void SwitchStillCaptureModeL( TCamImageCaptureMode aImageMode, 
                                      TBool                aReprepare, 
                                      TBool                aUpdateUi = ETrue );


        /**
        * Returns whether or the secondary camera has been activated
        * Will return ETrue when when the second camera is active
        * @since 2.8
        * @return ETrue if second camera is active, else returns EFalse
        */
        TBool IsSecondCameraEnabled() const;

        /**
        * Secondary camera orientation is constructed according to this flag.
        * Flag can be forced to portrait/landscape or it can follow qwerty 
        * state. Variation is handled via KCamCr2ndCamOrientation CenRep key.
        * @param aLock, Lock orientation state flag. 
        *               ETrue, ask state and lock it.
        *               EFalse, ask current state.
        * @return ETrue if secondary camera orientation must be landscape
        */
        TBool IsQwerty2ndCamera( TBool aLock = EFalse );  

        /**
        * 
        * @return ETrue if wait dialog exist
        */
        TBool CCamAppUi::IsWaitDialog() const;

        /**
        * Returns the active camera
        * @since 2.8
        * @return either primary, secondary or none ( if in process of switching )
        */
        TCamActiveCamera ActiveCamera() const;

        /**
        * Called just prior to capture. Checks if there is sufficient memory
        * available for capture and handles out of memory/error conditions. 
        * @since 2.8
        * @return ETrue if there is memory available for capture
        */
        TBool CheckMemoryL();

        /**
        * Display error note - with OK softkey only
        * @since 2.8
        * @param aStatus error status
        */
        void ShowErrorNoteL( TCamMemoryStatus aStatus );

        /**
        * Display error note for OOM condition - OK and Cancel softkeys
        * @since 2.8
        * @param aLabel text to be shown
        * @param aWithCancel Enables cancel softkey; default=ETrue
        * @return Response code returned from Out of memory dialog
        */
        TInt ShowOOMNoteL( TDesC& aLabel, TBool aWithCancel = ETrue);

        /**
        * Set the requested resolution for a new file
        * @since 3.2
        * @param aRequestedResolution the requested resolution from the new file request.
        */
        void SetRequestedNewFileResolution( const TSize& aRequestedResolution );

        /**
        * Get the requested resolution for a new file
        * @since 3.2
        * @return the requested resolution from the new file request.
        *  Default is TSize(0,0) which interpreted as mms quality resolution
        */
        TSize RequestedNewFileResolution() const;

        /**
        * Shows the mmc removal note if it is required
        * @since 2.8
        */
        void ShowMMCRemovalNoteIfRequiredL();

        /**
        * Check memory card for available space and card status
        * @since 2.8
        * @param aFull on return contains ETrue if card is full
        * @param aStatus on return contains the card status
        * @param aCurrentLocation current storage location
        */
        void CheckMemoryCard( TBool& aFull,
                TCamMemoryStatus& aStatus, TCamMediaStorage aCurrentLocation );

        /**
        * Checks if aStorage location is full
        * @since 2.8
        * @param aStorage target storage location
        * @param aCurrentLocation current storage location
        * @return ETrue if aStorage location is full
        */
        TBool MemoryFull( TCamMediaStorage aStorage, TCamMediaStorage aCurrentLocation );

        /**
        * Starts the video/photo capture
        * @since 2.8
        * @param aKeyEvent Key used to start the capture
        * @return whether the key was consumed by the function
        */
        TKeyResponse StartCaptureL( const TKeyEvent& aKeyEvent );

        /**
        * Stops the video capture
        * @since 2.8
        */
        void EndCapture();
        
        /**
        * Changes the orientation to aNewOrientation
        * If aNewOrientation is EAppUiOrientationUnspecified, switches
        * to the normal orientation (this may depend on the currently
        * activated camera)
        * @since 3.0
        */
        void SwitchOrientationIfRequiredL( TInt aNewOrientation );

        /**
        * Returns pointer to side-pane
        * @since 2.8
        * @return pointer to CCamSidePane object constructed by appUi
        */
        CCamSidePane* SidePane();

        /**
        * Returns pointer to zoom-pane
        * @since 2.8
        * @return pointer to CCamZoomPane object constructed by appUi
        */
        CCamZoomPane* ZoomPane();

        /**
        * Returns current camera orientation
        * @since 3.0
        * @return current camera orientation
        */
        TCamOrientation CamOrientation();

        /**
        * set current camera orientation as landscape
        */
        void SetCamOrientationToLandscape();
	
        /**
        * Returns current viewstate
        * @since 3.0
        * @return current viewstate
        */
        TCamViewState CurrentViewState() const;
        
        /**
        * Returns target view state
        * @since 4.0
        * @return target view state
        */
        TCamViewState TargetViewState();

        /**
        * Returns the target mode of current mode switch
        * (Image/Video/Idle/Shutdown). The returned value
        * should equal to current state, if no switch is
        * in progress
        */        
        TCamCameraMode TargetMode() const;

        /**
        * Handles the shutter key press
        * @param aPressed ETrue if the shutter is pressed, EFalse if released
        * @since 2.8
        */
        void HandleShutterKeyL( TBool aPressed );    
        
        /**
        * Set embedding flag 
        * @param aEmbedding ETrue if we are embedding (ie Messaging, Realplayer), 
        * EFalse if not
        * @since 2.8
        */
        void SetEmbedding( TBool aEmbedding );
        
        /**
        * Return embedding flag 
        * @return ETrue if we are embedding (ie Messaging, Realplayer), 
        * EFalse if not
        * @since 2.8
        */
        TBool Embedding() const;
        
        /**
        * Set SendAs flag 
        * @param aSendAsInProgress ETrue if SendAs in progress, EFalse if not
        * @since 2.8
        */
        void SetSendAsInProgressL( TBool aSendAsInProgress );

        /**
        * Get SendAs flag value 
        * @return ETrue if SendAs in progress, EFalse if not
        * @since 4.0
        */        
        virtual TBool SendAsInProgress() const;
        
        /**
        * Offers a pointer to appui's TextResolver instance
        * @return pointer to appui's TextResolver instance   
        */
        CTextResolver* TextResolver();
            
#ifndef __WINSCW__
        /**
        * Get pointer to SFI utility class 
        * @since 3.0
        * @return pointer to CSFIUtilsAppInterface
        */
        //CSFIUtilsAppInterface* SFIUtility() const;

        /**
        * Return whether we are currently in-call send
        * @since 3.0
        * @return TBool true if we are in in-call send mode
        */
        //TBool IsInCallSend() const;

        /**
        * Send media file using SFI utility
        * @since 3.0
        */
        //void DoInCallSendL() const;
#endif //WINSCW

        /**
        * Whether or not the application is in the background
        * @since 2.8
        * @param aIncludeNotifier If EFalse then the application is not
        * classed as being in the background if the foreground application is
        * a notifier, e.g. charging enabled.
        * @return ETrue if the application is in the background
        */   
        TBool AppInBackground( TBool aIncludeNotifier ) const;
        
        /**
        * Indicates whether or not the application is in a simulated exit situation
        * @return ETrue if the application has pretended to exit, else EFalse 
        */
        TBool IsInPretendExit() const;
        
        /**
        * Indicates whether or not the application was in a simulated exit situation
        * @return ETrue if the application had pretended to exit, else EFalse 
        */
        TBool ReturningFromPretendExit() const; 
        
        /**
        * Indicates whether or not the application started the first time
        * @return ETrue if the application booted first time, else EFalse 
        */
        TBool IsFirstBoot() const; 
                
        /**
        * Returns whether an MMC removed note is pending
        * @since 3.0
        * @return TBool ETrue if the UI needs to show an MMC 
        */
        TBool IsMMCRemovedNotePending() const;                  

        /**
        * Returns the pointer to the NaviCounter pane
        * @since 3.0
        * @return The Navi Counter pointer
        */
        CCamNaviCounterControl* NaviCounterControl();
        
        /**
        * Returns the pointer to the NaviProgress pane
        * @since 3.0
        * @return The Navi Progress pointer
        */
        CCamNaviProgressBarControl* NaviProgressBarControl();

        /**
        * Returns the pointer to the Navi Counter ModelProgress pane
        * @since 3.0
        * @return The Navi Counter Model pointer
        */
        CCamNaviCounterModel* NaviCounterModel();

        /**
        * Return handle to active palette if supported
        * @since 3.0
        * @return Handle to Active palette
        */             
        MActivePalette2UI* ActivePalette() const;
        
        /**
        * Get pointer to Active Palette handler instance.
        *
        * @return Pointer to Active Palette handler instance. Ownership not
        *         transferred.
        */
        CCamActivePaletteHandler* APHandler() const;

        /**
        * Tries to show or hide the Active Toolbar
        * @since 3.0
        * @param aShown True to show it, False to hide it
        */
        void SetActivePaletteVisibility( TBool aShown );

        /**
        * Returns whether the user has set the icons to always display in pre-capture
        * @since 3.0
        * @return ETrue if the optional icons should always be displayed
        */
        TBool AlwaysDrawPreCaptureCourtesyUI() const;

        /**
        * Returns whether the user has set the icons to always display in post-capture
        * @since 3.0
        * @return ETrue if the optional icons should always be displayed
        */
        TBool AlwaysDrawPostCaptureCourtesyUI() const;

        /**
        * Set whether the icons should always display in pre-capture
        * @since 3.0
        * @param aNewSetting True if they should always be drawn; False if they should only be drawn when the UI is raised
        */
        void  SetAlwaysDrawPreCaptureCourtesyUI( TBool aNewSetting );

        /**
        * Set whether the icons should always display in post-capture
        * @since 3.0
        * @param aNewSetting True if they should always be drawn; False if they should only be drawn when the UI is raised
        */
        void SetAlwaysDrawPostCaptureCourtesyUI( TBool aNewSetting );

        /**
        * Check whether a key event should cause the courtesy UI to be raised
        * @since 3.0
        * @param aKeyEvent The key event
        * @param aType The type of key event
        * @param aInPreCapture Whether the key was pressed in precapture mode
        * @return Whether the key was consumed or not
        */
        TKeyResponse CheckCourtesyKeyEventL( const TKeyEvent aKeyEvent, TEventCode aType, TBool aInPreCapture );

        /**
        * Whether to draw the complete UI in precapture mode
        * @since 3.0
        * @return True if it should all be drawn
        */
        TBool DrawPreCaptureCourtesyUI() const;

        /**
        * Whether to draw the complete UI in postcapture mode
        * @since 3.0
        * @return True if it should all be drawn
        */
        TBool DrawPostCaptureCourtesyUI() const;

        /**
        * Bring the complete UI to the fore in precapture
        * @param aShowZoomBar Briefly shows the zoombar first, if necessary
        * @since 3.0
        */
        void RaisePreCaptureCourtesyUI(TBool aShowZoomBar);

        /**
        * Bring the complete UI to the fore in postcapture
        * @since 3.0
        */
        void RaisePostCaptureCourtesyUI();

        /**
        * Hide the optional UI in precapture
        * @since 3.0
        */
        void SubmergePreCaptureCourtesyUI();

        /**
        * Hide the optional UI in postcapture
        * @since 3.0
        */
        void SubmergePostCaptureCourtesyUI();

        /**
        * Ensures the zoom pane gets shown and updated, specifically in direct viewfinder/active toolbar modes. Calling this method
        * is not sufficient to show the zoom pane however, other methods in CZoomPane must be called to achieve this.
        * @param aRedraw Whether to force a redraw event
        * @since 3.0
        */
        void ShowZoomPane( TBool aRedraw );
        
        /**
        * Ensures the zoom pane gets hidden, specifically in direct viewfinder/active toolbar modes. As above, not sufficient to 
        * hide the zoom pane.
        * @param aRedraw Whether to force a redraw event
        * @since 3.0
        */
        void HideZoomPane( TBool aRedraw );

        /**
        * Refreshes the CBA based on current view and app state
        * @since 2.8
        */       
        void UpdateCba();

        /**
        * Sets the softkeys to the specified Resource ID
        * @since 3.0
        * @param aResource ID of the softkey resource to use
        */
        void SetSoftKeysL( TInt aResource );
        
        /**
        * Returns whether the time lapse slider is shown
        * @since 3.0
        * @return ETrue if time lapse slider is shown
        */
        TBool TimeLapseSliderShown() const;

        /**
        * Detects whether the direct viewfinder is in use or not
        * @since 3.0
        * @return True if it is active, otherwise false
        */
        TBool IsDirectViewfinderActive();

        /**
        * Let the AppUi know what is being shown in pre-capture
        * @param aMode What's being shown
        */
        void SetPreCaptureModeL(TCamPreCaptureMode aMode);  

        /**
         * Let the AppUi know what is being shown in pre-capture
         * @param aMode What's being shown
         */
        void SetPreCaptureMode(TCamPreCaptureMode aMode);
        
        /**
        * Whether or not the telephone application is in the foreground
        * @since 3.0
        * @return ETrue if the telephone application is in the foreground
        */    
        TBool ForegroundAppIsPhoneApp() const;

        /**
        * Whether to show the post capture view in current mode
        * @return ETrue iff we should show the post capture view in current mode
        */    
        TBool ShowPostCaptureView() const;

        /**
         * Whether the automatic UI-rise should be suppressed on next viewfinder start event
         * e.g. when postcapture is set to off, the UI is off and a photo is taken
         * @return Whether the automatic UI-rise should be suppressed
         */
        TBool SuppressUIRiseOnViewfinderStart() const;

        /**
         * Set whether the automatic UI-rise should be suppressed on next viewfinder start event
         * @see SuppressUIRiseOnViewfinderStart
         * @param aSuppress Whether the automatic UI-rise should be suppressed
         */
        void SetSuppressUIRiseOnViewfinderStart(TBool aSuppress);
        
        /**
        * Whether the latest capture was selftimer initiated
        * @return ETrue if the latest capture was started via selftimer
        */
        TBool IsSelfTimedCapture() const;


        /**
         * Set the 'freeing memory' flag.
         *
         * @param aFreeingMemory New value for the 'freeing memory' flag.
         */
        void SetFreeingMemory( TBool aFreeingMemory );
        
        /**
         * Get the current state of the 'freeing memory' flag.
         *
         * @return Current value for the 'freeing memory' flag.
         */
        TBool IsFreeingMemory() const;

        
        /**
        * Checks whether its safe to draw controls
        * @return ETrue if its safe to draw the controls 
        */
        TBool ReadyToDraw() const;
        
        /**
        * Set toolbar visibility in according to whether current view and mode are fit.
        */
        void SetToolbarVisibility();

        /**
        * Hide the toolbar.
        */
        void SubmergeToolbar(); 
        
        /**
        * Returns ETrue if the toolbar extension is visible,
        * otherwise EFalse.
        */
        TBool IsToolBarExtensionVisible() const;
        
        /*
         * Handles Hdmi events
         */
        void HandleHdmiEventL( TCamHdmiEvent aEvent );
 
    public: // Functions from base classes
        /**
        * Draw the counter to the bitmap used in the navi pane
        * @since 3.0
        * @param aBmpGc The graphics context for the bitmap
        * @param aBmpMaskGc The graphics context for the bitmap mask
        */
        void CreateNaviBitmapsL( TBool aDrawStorageIconAndCounter );
	    
        /**
        * From CAknAppUi Handle user menu selections
        * @since 2.8
        * @param aCommand The enumerated code for the option selected
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From MCamControllerObserver
        * @since 2.8
        * @param aEvent The enumerated code for the event received
        * @param aError The error code associated with the event
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );

        /**
        * Return whether we are in embedded mode (if we have a pointer to a
        * MApaEmbeddedDocObserver).
        * @since 2.8
        * @return TBool if tthere is a handle to MApaEmbeddedDocObserver
        */
        TBool IsEmbedded() const;

        /**
        * Handle window server event.
        * @since 2.8
        * @param aEvent the window server event
        * @param aDestination the control associated with the event. 
        * Only valid for pointer events.
        */
        void HandleWsEventL( const TWsEvent& aEvent,
                             CCoeControl* aDestination );
                             
                             
        /**
        * Start application as file server application
        * @since 3.0
        * @param aEmbeddedObserver service observer
        * @param aMode Service type (image or video)
        */
        void StartAsServerAppL( MCamEmbeddedObserver *aEmbeddedObserver, 
        	               TInt aMode );
        
        /**
        * Set application file server observer  
        * @since 9.2
        * @param aEmbeddedObserver service observer
        */
        void SetEmbeddedObserver( MCamEmbeddedObserver* aEmbeddedObserver );

        /**
        * Closes the application
        * @since 2.8
        */
        void CloseAppL(); 
        
        /**
        * Closes the application
        * @since 2.8
        * Exits the application according to current state, if the app always
        * running flag is set, the application goes to the background but if 
        * embedded, or app always running flag is not set or in an error situation 
        * the application truly exits
        */
        void InternalExitL();

        /**
        * Inform the controller of the count of images required at the next
        * capture. Called before every still capture request.
        * @since 2.8
        */
        void InitCaptureCountL();

        /**
		* From MCamVolumeKeyObserver - pick up volume key events
		* @since 3.1
		* @param aOperationId - operation id (i.e. volume key up/down)
		* @param aButtonAct bitton action press/click/release
		*/
		void HandleVolumeKeyEvent( TRemConCoreApiOperationId aOperationId,
                            	   TRemConCoreApiButtonAction aButtonAct);
                          			
        /**
        * Handle an event from CCamSelfTimer.   
        * @since S60 v5.0
        * @param aEvent the self timer event
        * @param aCountDown self timer countdown duration
        */
        void HandleSelfTimerEvent( TCamSelfTimerEvent aEvent, 
                                   TInt aCountDown );

  
    public: // new fuctions

        /**
        * We handle the case when settings plugin exists
        **/
        void SettingsPluginExitedL( TCamSettingsPluginExitMessage aType ); 

        /**
        * We keep track of lauching settings plugin within camera
        * to control the behaviour of the camera when the plugin exists and
        * returns to camera
        **/
        TBool SettingsLaunchedFromCamera() const;
        
        /**
        * Checks if the memory type is full or unavailable
        * @param aStorage TCamMediaStorage enum specifying the memory type; 
        *	default=ECamMediaStorageCurrent
        * @return ETrue if the specified memory is full or unavailable
        **/
        TBool 
        IsMemoryFullOrUnavailable(const TCamMediaStorage 
        						  aStorage = ECamMediaStorageCurrent) const;
        
        /**
        * Checks if all memories are full and unavailable
        * @return ETrue if all memories are full or unavailable
        **/
        TBool AllMemoriesFullOrUnavailable() const;
        
        /**
        * Checks the availability of memory on the currently selected 
        * mediastorage for capturing images or video, result saved
        * in iMemoryAvailableForCapturing
        **/
        void CheckMemoryAvailableForCapturing();
        
        /**
        * Returns the previously saved availability of memory
        * @return ETrue if memory available for capturing
        **/
        TBool IsMemoryAvailableForCapturing() const;

        /**
        * Get a pointer to the startup logo controller.
        * AppUi keeps ownership.
        * @return Pointer to CCamStartupLogoController or NULL.
        */
        CCamStartupLogoController* StartupLogoController();
        
        /**
        * Get a pointer to the NaviProgressBarModel.
        */
        CCamNaviProgressBarModel* NaviProgressBarModel();

    private: // New functions

      /**
      * Helper method to handle controller event for
      * going to standby mode.
      */
      void HandleStandbyEventL( TInt aStatus );
            
      /**
      * Notify our views of application level events, like focus gained/lost.
      */
      void NotifyViews( const TCamAppEvent& aEvent );


        /**
        * Construct views
        * Construct pre capture views        
        * @since 2.8
        */
        void ConstructPreCaptureViewsL();

        /**
        * Construct post capture views
        * @since 2.8
        */
        void ConstructPostCaptureViewsL();

        /**
        * Construct settings, user scene setup, and postcapture views 
        * @since 2.8
        */
        void ConstructViewIfRequiredL();

        /**
        * Construct navi pane
        * @since 2.8
        */
        void ConstructNaviPaneL();

        /**
        * Switch to pre-capture view
        * @since 2.8
        * @return TInt to indicate if function should be called again
        */
        TInt DoPostCaptureTimeoutL();

        /**
        * Try to switch active view.
        * @since 2.8
        */
        void TrySwitchViewL( TBool aDeactivateFirst = EFalse );

        /**
        * Determines if a switch of the active view required.
        * @param aDeactivateFirst Deactivate before check
        * @since 2.8
        */
        TCamViewSwitch ViewSwitchRequired( TBool aDeactivateFirst = EFalse ) const;

        /**
        * Get uid of view with requested view and mode.
        * @since 2.8
        * @param aState      View state
        * @param aMode       Main camera mode (image/video)
        * @param aImageMode  In image mode, image capture type (single/burst/timelapse)
        * @param aUid        Uid of view passed by reference and populated in function
        * @param aMessageUid A message Uid to pass to the view
        * @return            Whether or not a valid view uid can be obtained
        */
        TBool ViewId( TCamViewState        aState, 
                             TCamCameraMode       aMode, 
                             TCamImageCaptureMode aImageMode, 
                             TUid&                aUid, 
                             TUid&                aMessageUid,
                             TBool                aDeactivateFirst );

        /**
        * Return a pointer to the navi pane.
        * @since 2.8
        * @return pointer to the status pane's navigation control container
        */
        CAknNavigationControlContainer* NaviPaneL();

        /**
        * Display the wait dialog when saving video
        * @since 2.8
        */
        void DisplayWaitDialogL();

        /**
        * Get the default startup mode from settings
        * If this is not defined the factory default will be used
        * @since 2.8
        * @return The default startup capture mode
        */
        TCamStartupMode StartupMode() const;
    
        /**
        * Save the current capture mode to settings.
        * This will be used as the startup mode
        * @since 2.8
        */
        void SaveCaptureMode();
        
        /**
        * Save the current state of the UI to settings.
        * 
        */
        
        void SaveCourtesyUiState();

        /**
        * Override HandleScreenDeviceChangedL() and trigger a re-layout
		* of the main controls from this - addition for 2.8
        * @since 2.8
        */
        void HandleScreenDeviceChangedL();

        /**
        * Called when the video is ready to be stopped
        * @since 2.8
        * @param aPtr pointer to the object that called the timer
        * @return Error code
        */
        static TInt StopVideoRecording( TAny* aPtr );

        /**
        * Called when the application is ready to exit
        * @since 2.8
        * @param aAny pointer to the object that called the timer
        * @return Error code
        */
        static TInt CallExit( TAny* aAny );

        /**
        * Callback used when the application needs to switch view
        * @since 2.8
        * @param aAny pointer to the object that called the timer
        * @return Error code
        */
        static TInt CallSwitch( TAny* aAny );

        /**
        * Launch the context sensitive help application
        * @since 2.8
        */  
        void LaunchHelpL();

        /**
        * Returns the AppUi orientation (e.g. Portrait or Landscape) for the given product-specific orientation
        * @since 3.0
        * @param aCamOrientation The product-specific orientation
        * @return The AppUi orientation (e.g. Portrait or Landscape)
        */
        TAppUiOrientation AppUiOrientation( TCamOrientation aCamOrientation );

        /**
        * Returns the camera orientation based on the layout state set by the phone
        * @since 3.0
        * @param aLayoutState The layout state set by the phone
        * @return The camera orientation
        */
        TCamOrientation LayoutStateToOrientation( TInt aLayoutState );

        /**
        * Returns the camera orientation based on the layout state set by the phone
        * @since 3.0
        * @return The camera orientation based on the layout state set by the phone
        */
        TCamOrientation ReadCurrentDeviceOrientation();

        /**
         * Returns whether or not the full screen viewfinder is being shown
         * @return Whether or not the full screen viewfinder is being shown 
         */
        TBool FullScreenViewfinderEnabled() const;
        
        /**
        * Construct a postcapture view if not already constructed
        *
        * The following parameter combinations can be used:
        * Still postcapture view: sViewState = ECamViewStatePostCapture, aMode = ECamControllerImage
        * Burst thumbnail view:   aViewState = ECamViewStateBurstThumbnail
        * Video Postcapture view: aViewState = ECamViewStatePostCapture, aMode = ECamControllerVideo
        *
        * @param aViewState
        * @param aCaptureMode
        * @since 2.8
        */
        void ConstructPostCaptureViewIfRequiredL( TCamViewState aViewState,
                                                  TCamCameraMode aMode );
        
        /*
        * Initiate the check operation using collection manager
        */
        /**
         * We start check operation to see if "Default Album exists"
         * Usecase: Whenever the camera returns from background/startup we
         * first start checking the default album which is set, if any, to make
         * sure that album really exists. If not, then we set back the value
         * of setting item "Add to album" to "No" in Image/Video setting
         * image settings
         */
        void StartCheckingDefaultAlbumIdL();

                                                  
#ifdef CAMERAAPP_DELAYED_POSTCAPTURE_CREATION                                                  
                                                
        /**
        * Starts calls to PostCapIdleCallBack during processor idle time. The calls
        * are continued until PostCapIdleCallBack is done.
        * @since 3.1
        */
        void StartPostCapIdleL();
        
        /**
        * Handles postcapture view construction so that each call to this function
        * constructs one postcapture view. Returns ETrue if there are still more views
        * to construct, EFalse if all are finished and no further calls are required.
        *
        * @param aSelf pointer to current CCamAppUi object
        * @return construction status
        * @since 3.1
        */
        static TInt PostCapIdleCallBack( TAny* aSelf );        

#endif // CAMERAAPP_DELAYED_POSTCAPTURE_CREATION        

        
        /**
        * Initiates a call to SensorIdleCallback during processor idle time
        * @since 3.1
        */        
        void StartSensorIdleL();
        
        /**
        * Cancels the (possibly) pending call to SensorIdleCallBack
        * @since 3.1
        */        
        void CancelSensorIdle();                
                                
        /**
        * Initializes the sensor API by calling iController.UpdateSensorApiL()
        *        
        * @param aSelf pointer to current CCamAppUi object
        * @return construction status
        * @since 3.1
        */
        static TInt SensorIdleCallBack( TAny* aSelf );        
        

    private: // Functions from base classes

#ifndef __WINSCW__
        /**
        * From CEikAppUi.
        * Processes shell commands.
        * @since 3.0
        * @param aCommand The command line command
        * @param aDocumentName The name of the document to open
        * @param aTail Not used.
        * @return TBool
        */
        TBool ProcessCommandParametersL( TApaCommand aCommand,
                                         TFileName& aDocumentName,
                                         const TDesC8& aTail );
        /**
        * From CEikAppUi.
        * Handle the external message.
        * @since 3.0
        * @param aClientHandleOfTargetWindowGroup Handle.
        * @param aMessageUid Message id.
        * @param aMessageParameters Message parameters.
        * @return Response to the message.
        */
        MCoeMessageObserver::TMessageResponse HandleMessageL(
            TUint32 aClientHandleOfTargetWindowGroup,
            TUid aMessageUid,
            const TDesC8& aMessageParameters );
            
#endif //WINSCW            
            
    private: // new functions

#ifndef __WINSCW__        
        /**
        * Bring the Phone application back to foreground after in-call send completion
        * @since 3.0
        */
        void BringPhoneAppToForeGroundL();
#endif //WINSCW

        /**
        * Sends the camera application to the background, to pretend we're closing
        * @since 2.8
        */
        void SendCameraAppToBackgroundL();

        /**
        * Hides the application from task list, to pretend that camera app is closed
        * @since 3.0 (added back since its useful in certain situation)
        * @param aHide whether to hide or make visible
        */
        void HideTaskL(TBool aHide);
        
        /**
        * returns ETrue if headset is connected
        */ 
        TBool IsHeadsetConnected() const;
        
    public:
        /**
        * Display error note
        * @since 2.8
        * @param aError The error code
        */ 
        void HandleCameraErrorL( TInt aError );
        
        
        /**
        * Set iAssumePostCaptureView flag
        * @param aValue ETrue sets flag on, EFalse off
        */ 
        void SetAssumePostCaptureView( TBool aValue );
        
        /**
         * Set iLensCoverExit flag
         * @param aValue ETrue sets flag on, EFalse off
         */
        void SetLensCoverExit( TBool aValue );
		
		/** 
        * Set iDrawPreCaptureCourtesyUI flag 
        * @param aValue ETrue sets flag on, EFalse off 
        */        
        void SetDrawPreCaptureCourtesyUI(TBool iDraw); 
		
         /**
        * Finishes the AppUI construction by calling 
        * CompleteAppUIConstructionL
        */ 
        static TInt AppUIConstructCallbackL( TAny* aAny );
        
        /**
        * returns ETrue if appUi construction is complete
        */ 
        TBool IsUiConstructionComplete();
        
        /**
		* set whether camera viewfinder stop or not
		* @param aViewFinderStopped ETrue viewfinder is stoped,EFalse viewfinder is runing
		* 
		*/ 
		void SetViewFinderStoppedStatus(TBool aViewFinderStopped);
		
        /**
        * Getter for iViewFinderInTransit 
        * @return ETrue if viewfinder handle is in transit between different windows
        */ 
		TBool IsViewFinderInTransit();
		
		/**
        * Setter for iViewFinderInTransit
        * @param aInTransit Set to ETrue if viewfinder window handle is in transit 
        * @return void
        */
		void SetViewFinderInTransit(TBool aInTransit);

        /**
        * Turns screen back visible if leave has occured.
        * @param aAny Pointer to CamAppUi instance 
        * @return void
        */
		static void CleanupBlankScreen( TAny* aAny );
		
     public: //MCamAddToAlbumObserver


        /*
        * From MCamAddToAlbumObserver
        * Notification once the aysnchronous operations using
        * CollectionManager interface are done completly
        * Status of the operation are returned using aStatus
        * e.g. KErrNone
        */
        void AddToAlbumIdOperationCompleteL() {};
        
        /**
        * From MCamAddToAlbumObserver
        * Notification once the asynchronous operations using
        * CollectionManager interface have completed.
        *
        * @param aAlbumExists ETrue if album exists
        * @param aAlbumTitle  Title of the album
        */
        void CheckAlbumIdOperationCompleteL(
                TBool aAlbumExists, const TDesC& aAlbumTitle );

    public: // MAknServerAppExitObserver
        /**
        * Handle the exit of a connected server app.
        * This implementation provides Series 60 default behavior
        * for handling of the EAknCmdExit exit code. Derived classes
        * should base-call this implementation if they override this
        * function.
        * @param aReason The reason that the server application exited.
        * This will either be an error code, or the command id that caused
        * the server app to exit.
        */
        virtual void HandleServerAppExit(TInt aReason);

    private:

        /**
        * Called when the courtesy timer ends
        * @since 3.0
        * @param aPtr The CCamAppUI object the timer has completed on
        */ 
        static TInt CourtesyTimeout( TAny* aPtr );

        /**
        * Called when the courtesy timer ends
        * @since 3.0
        */ 
        TInt DoCourtesyTimeout();

        /**
        * Called to set the correct navicounter / naviprogress controls to "active"
        * @since 3.0
        */ 
        void UpdateNaviModelsL( TBool aUpdateNavi = ETrue );
                

        /**
        /**
        * Dismisses out of memory / memory card removed note, if active.
        */
        void DismissMemoryNoteL();  
        
		/**
		* Register to capture half and full shutter press key events
		* @since 3.0
		*/
		void RegisterCaptureKeysL();
		
		/**
		* Release capture of half and full shutter press key events
		* @since 3.0
		*/
		void UnRegisterCaptureKeys();  

    /**
    * Select the setting for viewfinder grid.
    */
    void SelectViewFinderGridSettingL();

    /**
    * Starts the Photos application command line
    **/
    void StartAppCmdLineL( const TDes8& aData );

    /**
    * Check if it's applicable to use the courtesy UI
    * @since S60 v5.0
    * @return ETrue if courtesy UI can be used
    */
    TBool CourtesyUiApplies() const;
    
    /**
    * Finish the AppUI construction
    **/
    void CompleteAppUIConstructionL();
    
  // =======================================================
  // Data
  private:    
    
        CCamAppController& iController;

        TCamSelfTimerFunctions iInSelfTimerMode;
        
        // Own
        CCamSelfTimer* iSelfTimer;

        // Not own.
        CCamNaviCounterControl* iNaviPaneCounterControl;

        // Not own.
        CCamNaviProgressBarControl* iNaviPaneProgressBarControl;

        // Own.
        CAknNavigationDecorator* iDecoratedNaviPaneCounter;

        // Own.
        CAknNavigationDecorator* iDecoratedNaviPaneProgressBar;


        // NOT owned; this pointer is just for checking 
        // if the wait dialog object still exists, or
        // whether it has deleted itself
        CCamWaitDialog* iWaitDialog;

        // Not own.
        CCamStillPreCaptureView* iStillCaptureView;

        // Not own.
        CCamVideoPreCaptureView* iVideoCaptureView;

        // Not own.
        CAknView*   iViaPlayerView;

        TCamViewState iViewState;
        TCamViewState iTargetViewState;
        TCamCameraMode iMode;
        TCamCameraMode iTargetMode;

        TCamImageCaptureMode iImageMode;
        TCamImageCaptureMode iTargetImageMode;
        TInt                  iStandbyStatus;

        TBool iKeyShutter;

        // Used to determine whether or not the startup view has been
        // set when app is launched embedded  
        TBool iEmbeddedViewSet;

        // Name of parent app if we're embedded
        // Own.
        HBufC*  iParentAppName;

        // Whether or not the cba has been blanked
        TBool iCbaIsDisabled;

        // Whether or not the burst mode has been activated
        TBool iBurstModeEnabled;

        // Observer that needs to know when burst mode is switched on/off
        // Observer objects are not owned.
        RPointerArray<MCamBurstModeObserver> iBurstModeObservers;

        // handle of the camera key captured
        TInt iCameraKeyHandle;

        // timer to assist with displaying wait dialog
        // Own.
        CPeriodic* iWaitTimer;

        // Whether or not the wait timer should be used
        TBool iApplyTimer;

        // Used for opening (and playing) video clips
        // Own.
        CDocumentHandler* iDocHandler;

        // the id of the application's window group
        TInt iMyWgId;

        // the id of the eikon server window group
        TInt iEikonServerWindowGroupId;

        // Whether or not a view switch is pending, used to delay switch to
        // post capture view when the application is in the background
        TPendingViewSwitchType iPendingViewSwitch;

        // Keeps track of the state before focus gained events
        TBool iReturnFromBackground;

        // Side-pane handler
        // Own.
        CCamSidePane* iCamSidePane;
        
        // Zoom-pane handler
        // Own.
        CCamZoomPane* iCamZoomPane;

        // Used to access settings
        // Own.
        CRepository* iRepository;        

        // Flag to indicate when video recording has stopped due to incoming call
        // Post capture timer is not started when this flag is set, delays the
        // timeout of the video post capture view
        TBool iDelayVideoPostCaptureTimeout;
                
        // Indicates if the mmc was detected as removed at startup, requiring
        // the user to be informed
        TBool iMMCRemoveNoteRequired;
        
        // Indicates if the mmc was detected as removed whilst we were in focus,
        // requiring us to inform the user when capture is attempted.
        TBool iMMCRemoveNoteRequiredOnCapture;     

        // Only the precapture views are created on construction
        // These flags indicate whether or not the other views have been created
        TBool iPostCaptureViewsConstructed;
        TBool iUserSceneSetupViewConstructed;
        TBool iCustomiseToolbarViewConstructed;

        // Used to resolve error message texts
        // Own.
        CTextResolver* iTextResolver;    

//#ifndef __WINS__
        // in-call send utility class
        // Own.
        //CSFIUtilsAppInterface* iSFIUtils;

        // Whether in in-call send mode
        //TBool iSendFileInCall;
//#endif              

        // embedded observer
        // Not own.
        MCamEmbeddedObserver* iEmbeddedObserver;

        // embedded start up capture mode
        TInt iApplicationMode;

        // Pointer to the only navicounter model
        // Own.
        CCamNaviCounterModel* iNaviCounterModel;

        //Pointer to the only naviprogressbar model
        // Own.
        CCamNaviProgressBarModel* iNaviProgressBarModel;

        // Pointer to the NaviCounter control
        // Own.
        CCamNaviCounterControl* iNaviCounterControl;

        // Pointer to the NaviProgress control
        // Own.
        CCamNaviProgressBarControl* iNaviProgressBarControl;

        TCamOrientation iCamOrientation;

        // Keeps a list of MCamResourceLoaders that this class owns
        RPointerArray<MCamResourceLoader> iResourceLoaders;

        // Own.
        CCamActivePaletteHandler* iActivePaletteHandler;

        // Maximum no if images in burst capture
        TInt iMaxBurstCaptureNum;

        // Whether the Coutesy UI should currently be displayed (precapture)
        TBool iDrawPreCaptureCourtesyUI;
        // Whether the user has set the UI to always be on (precapture)
        TBool iAlwaysDrawPreCaptureCourtesyUI;

        // Whether the Coutesy UI should currently be displayed (postcapture)
        TBool iDrawPostCaptureCourtesyUI;
        // Whether the user has set the UI to always be on (postcapture)
        TBool iAlwaysDrawPostCaptureCourtesyUI;

        // The courtesy timer: when this ends, the UI should be hidden if applicable
        // Own.
        CCamTimer* iCourtesyTimer;

        // Whether the timer was started in precapture mode or not
        TBool iCourtesyTimerInPreCapture;

        // True if the zoom pane is currently being shown, False if not
        TBool iZoomPaneShown;

        // True when application is started  
        TBool iFirstBoot;
        
        // Flag to show if a screen redraw event is being processed
        TBool iProcessingScreenRedraw;
        
        // Whether the active palette should be prevented from being shown:
        // used during saving of an image/video
        TBool iPreventActivePaletteDisplay;
        
        // Used to delay engine creation on app startup or camera switch
        // if the orientation has not completed initialising.
        TBool iEngineConstructionDelayed;
        TBool iCameraSwitchDelayed;  

        // Set to true in the case where we pretend to exit the app, but are still really
        // running in the background. The app should behave slightly differently when we 
        // are next brought to the foreground.
        TBool iPretendExit;
        TBool iReturningFromPretendExit; 

        // flag to indicate whether we are currently embedding another application
        TBool iEmbedding;

        // flag to indicate if "Send As" is in progress
        TBool iSendAsInProgress;

        // 
        TInt iFullShutterPressHandle;
        TInt iHalfShutterPressHandle;
        TInt iFullShutterUpDownHandle;

        // Window group ID for Bluetooth server
        TInt iBTServerWindowGroupId;
        
        // Window group ID for Telephone application
        TInt iPhoneAppWindowGroupId;
        
        // position of softkeys in landscape mode (left or right)
        TInt iSoftkeyPosition;
        
        /// The current pre capture mode shown
        TCamPreCaptureMode iPreCaptureMode;

        // flag to indicate viewfinder stopped
        TBool iViewFinderStopped;  

        // flag used to say whether we want to suppress auto-UI rise on next viewfinder start event
        TBool iSuppressUIRise;

        // Flag used to indicate that focus was lost to an incoming call during sequence capture
        TBool iFocusLostToCallDuringSequenceCapture;

        // observer to receive volume key presses
        // Own.
        CCamRemConObserver* iVolumeKeyObserver;

        TBool iZoomUsingVolumeKeys;

        // Own
        CAknLocalScreenClearer* iScreenClearer;

        // Status of postcapture views construction
        TBool iStillPostCaptureViewConstructed;
        TBool iBurstThumbnailViewConstructed;
        TBool iVideoPostCaptureViewConstructed;

#ifdef CAMERAAPP_DELAYED_POSTCAPTURE_CREATION        
        // CIdle object used for late construction of the postcapture views
        // Own.
        CIdle* iPostCapIdle;

        // Status of the views creation
        TCamPostCapConstruction iPostCapProgress;
#endif // CAMERAAPP_DELAYED_POSTCAPTURE_CREATION

        // Own.
        CIdle* iSensorIdle;       
        TBool iSensorIdleRunning;

        // Own.
        HBufC* iLowRamNoteText;

        // Used to allow postcapture view to show for selftimed captures
        TBool iSelfTimedCapture;

        TBool iDSASupported;

        TInt iNoteId;

        // View UID if the ViaPlayerApplication view
        TUid iViaPlayerUid;
        // LeftSoftKey is Pressed
        TBool iLeftSoftKeyPressed;
        TBool iEndKeyPressed;

        TBool iCheckOperationInProgress;
        CCamCollectionManagerAO* iCollectionManager;

        // Own.
        CCamGSInterface* iPlugin;

        TBool iSettingsPluginLaunched;

        TBool iReturnedFromPlugin;

        // Own
        CCamFileCheckAo *iFileCheckAo;

        // Own
        CAknStylusPopUpMenu* iTouchPopupMenu;

        // Own
        CAknStaticNoteDialog* iMemoryNote;

        TSize iRequestedNewFileResolution;

        TBool iLostFocusToNewWindow;
        TBool        iToolbarVisibility;
        TBool        iRotatedKeyEvent;
        
        // flag that tells us that we are exiting app because lens cover is closed
        // used to skip post-capture view in these cases
        TBool iLensCoverExit; 
        // flag that indicates whether view finder start is the
        // first one after camera startup
        TBool iFirstVFStart;

        TUid iLastHiddenViewId;
        
        TCamMediaStorage iInternalStorage; //Points to either Phone or Internal Mass storage based on availability
        T2ndCamOrientation iLockedQwertyState; 
        TBool iUiConstructionComplete;
        CCamMemoryMonitor* iMemoryMonitor;
		        
        //Flag to monitor viewfinder window handle switch
        TBool iViewFinderInTransit;

		TInt iLandscapeScreenMode;
        TInt iPortraitScreenMode;
        
        TBool iMemoryAvailableForCapturing;

        CCamStartupLogoController* iStartupLogoController;

        TBool iVideoClipPlayInProgress;

        /**
         * iCamFtuDisplay
         * An instance of the CCamFtuDisplay class
         */
        CCamFtuDisplay* iCamFtuDisplay;
        };

// ===========================================================================
// Debug strings

#ifdef _DEBUG
const TUint16* const KCamViewStateNames[] =
  {
  (const TUint16* const)_S16("ECamViewStateRangeInternalMin"),

  (const TUint16* const)_S16("ECamViewStatePreCapture"),
  (const TUint16* const)_S16("ECamViewStatePostCapture"),
  (const TUint16* const)_S16("ECamViewStateBurstThumbnail"),
  (const TUint16* const)_S16("ECamViewStateSettings"),
  (const TUint16* const)_S16("ECamViewStateStandby"),
  (const TUint16* const)_S16("ECamViewStateUserSceneSetup"),
  (const TUint16* const)_S16("ECamViewStateWhiteBalanceUser"),
  (const TUint16* const)_S16("ECamViewStateColourFilterUser"),
  (const TUint16* const)_S16("ECamViewStateFlashUser"),
  (const TUint16* const)_S16("ECamViewStateSceneUser"),
  (const TUint16* const)_S16("ECamViewStateSceneStill"),
  (const TUint16* const)_S16("ECamViewStateCaptureSetupMenu"),
  (const TUint16* const)_S16("ECamViewStateSceneSettingList"),
  (const TUint16* const)_S16("ECamViewStateExposureUser"),
  (const TUint16* const)_S16("ECamViewStateCustomiseToolbar"),
  (const TUint16* const)_S16("ECamViewStatePrePortraitCapture"),
  
  (const TUint16* const)_S16("ECamViewStateRangeInternalMax"),
  (const TUint16* const)_S16("ECamViewStateRangeExternalMin"),

  (const TUint16* const)_S16("ECamViewStateViaPlayer"),
  (const TUint16* const)_S16("ECamViewStateGsCamcorderPlugin"),

  (const TUint16* const)_S16("ECamViewStateRangeExternalMax")
  };
__ASSERT_COMPILE( (sizeof(KCamViewStateNames)/sizeof(TUint16*)) == ECamViewStateCount );
#endif // _DEBUG

#endif      // CAMAPPUI_H   
            
// End of File
