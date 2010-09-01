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
* Description:  AppUi base class for Camera AppUi and Dummy AppUi*
*/


#ifndef CAMAPPUIBASE_H
#define CAMAPPUIBASE_H

//  INCLUDES
 
#include <aknappui.h>
#include <aknViewAppUi.h>
#include <activepalette2ui.h>
#include "CamSettingsInternal.hrh"


// CONSTANTS

// MACROS

// DATA TYPES

enum  TCamAppViewIds
    {
    ECamViewIdStillPreCapture,
    ECamViewIdStillPostCapture,
    ECamViewIdVideoPreCapture,
    ECamViewIdVideoPostCapture,
    ECamViewIdBurstThumbnail,
    ECamViewIdVideoSettings,
    ECamViewIdPhotoSettings,
    ECamViewIdPhotoUserSceneSetup,
    ECamViewIdViaPlayerPlayBack,
    ECamViewIdCustomiseToolbar
    };

enum TCamSelfTimerFunctions
    {   
    ECamSelfTimerMode1,     // Eg ON 10-second timer
    ECamSelfTimerMode2,     // Eg ON 2-second timer
    ECamSelfTimerMode3,     // Eg ON additional possibility
    ECamSelfTimerDisabled   // Eg OFF 
    };

/**
* Camera orientations
*/
enum TCamOrientation
    {
    ECamOrientationDefault = 0,
    ECamOrientationCamcorder = ECamOrientationDefault,
    ECamOrientationPortrait,
    ECamOrientationViewMode,
    ECamOrientationCamcorderLeft,	// softkeys left
    ECamOrientationTotal
    };

enum TCamPreCaptureMode
    {
    ECamPreCapViewfinder,
    ECamPreCapCaptureSetup,
    ECamPreCapTimeLapseSlider,
    ECamPreCapStandby,
    ECamPreCapSceneSetting,
    ECamPreCapGenericSetting
    };

enum TCamAppEvent
  {
  ECamAppEventFocusGained,
  ECamAppEventFocusLost
  };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCamSelfTimer;
class MCamBurstModeObserver;
class CCamSidePane;
class CCamZoomPane;
class CTextResolver;
class CCamActivePaletteHandler;
class MCamAppController;
class MCamStaticSettings;
// CLASS DECLARATION

/**
*  AppUi base class for Camera AppUi and Dummy AppUi
*
*  @since 2.8
 */
class CCamAppUiBase : public CAknViewAppUi
                        //, public MCamControllerObserver
    {
    public:  // Constructors and destructor
        

    public: // New functions
    
    
        /**
        * Determines whether or not all construction has been completed
        * Used on view activation to determine if the view should
        * open in standby mode. This will happen if the app is waiting for
        * an orientation switch to complete or for the initial view to be set
        * when launched in embedded
        * @since 3.0
        * @return ETrue if all construction has completed, else EFalse
        */   
        virtual TBool IsConstructionComplete() const = 0;

        
        /**
        * Set title pane text from a resource.
        * @since 2.8
        * @param aResourceId resource to read from
        */
        virtual void SetTitleL( TInt aResourceId ) = 0;

        /**
        * Set title pane text from a descriptor.
        * @since 2.8
        * @param aText title pane text
        */
        virtual void SetTitleL( const TDesC& aText ) = 0;

        /**
        * In embedded mode set title to name of embedding application
        * @since 2.8
        */
        virtual void SetTitleEmbeddedL() = 0;

        /**
        * Draw the counter to the bitmap used in the navi pane
        * @since 3.0
        * @param aBmpGc The graphics context for the bitmap
        * @param aBmpMaskGc The graphics context for the bitmap mask
        */
	    virtual void CreateNaviBitmapsL( TBool aDrawStorageIconAndCounter )= 0;

        /**
        * Push navi pane indicators on to navi pane stack.
        * @since 2.8
        */
        virtual void PushNaviPaneCounterL() = 0;

        /**
        * Push navi pane progress bar on to navi pane stack.
        * @since 2.8
        */
        virtual void PushNaviPaneProgressBarL() = 0;

        /**
        * Push default navi pane on to navi pane stack.
        * @since 2.8
        */
        virtual void PushDefaultNaviPaneL() = 0;

        /**
        * Enable or disable SelfTimer functionality
        * @param aEnable specifies self timer mode or self timer off
        * @since 2.8
        */
        virtual void SelfTimerEnableL( TCamSelfTimerFunctions aEnable ) = 0;

        /**
        * Returns current self-timer state
        * @since 2.8
        */
        virtual TBool SelfTimerEnabled() const = 0;

        /**
        * Returns pointer to self-timer object
        * @since 2.8
        */
        virtual CCamSelfTimer* SelfTimer() = 0;

        /**
        * Registers an observer for changes in burst mode activation
        * @since 2.8
        * @param aBurstModeObserver the object observing burst mode changes
        */
        virtual void AddBurstModeObserverL(
                            MCamBurstModeObserver* aBurstModeObserver ) = 0;

        /**
        * Removes an observer for changes in burst mode activation
        * @since 2.8
        * @param aBurstModeObserver the observer to remove
        */
        virtual void RemoveBurstModeObserver(
                        const MCamBurstModeObserver* aBurstModeObserver ) = 0;

        /**
        * Returns whether or not burst capture mode is enabled.
        * Will return ETrue when burst mode is enabled for still capture
        * even if the application is currently in video capture mode
        * @since 2.8
        * @return ETrue if burst mode is enabled, else returns EFalse
        */
        virtual TBool IsBurstEnabled() const = 0;
        
        /**
        * Returns the currently enabled burst mode.
        * @since 3.0
        * @return the currently enable burst mode or ECamImageCaptureNone if 
        * burst is not enabled
        */
        virtual TCamImageCaptureMode CurrentBurstMode() const = 0;

        /**
        * Return whether we are in embedded mode (if we have a pointer to a
        * MApaEmbeddedDocObserver).
        * @since 2.8
        * @return TBool if tthere is a handle to MApaEmbeddedDocObserver
        */
        virtual TBool IsEmbedded() const = 0;

        /**
        * Sets the current capture mode ( and target capture mode )
        * @since 2.8
        * @param aMode      the desired capture mode
        * @param aImageMode In image capture, the desired mode (single/burst/timelapse)
        */
        virtual void SetCaptureMode( TCamCameraMode       aMode, 
                                     TCamImageCaptureMode aImageMode = ECamImageCaptureNone ) = 0;

        /**
        * Shows the mmc removal note if it is required
        * @since 2.8
        */
        virtual void ShowMMCRemovalNoteIfRequiredL() = 0;

        /**        
        * Returns pointer to side-pane
        * @since 2.8
        * @return pointer to CCamSidePane object constructed by appUi
        */
        virtual CCamSidePane* SidePane() = 0;

        /**
        * Returns pointer to zoom-pane
        * @since 2.8
        * @return pointer to CCamZoomPane object constructed by appUi
        */
        virtual CCamZoomPane* ZoomPane() = 0;

        /**
        * Returns current camera orientation
        * @since 3.0
        * @return current camera orientation
        */
        virtual TCamOrientation CamOrientation() = 0;

        /**
        * Returns whether or the secondary camera has been activated
        * Will return ETrue when when the second camera is active
        * @since 2.8
        * @return ETrue if second camera is active, else returns EFalse
        */
        virtual TBool IsSecondCameraEnabled() const = 0;

        /**
        * Secondary camera orientation is constructed according to this flag.
        * Flag can be forced to portrait/landscape or it can follow qwerty 
        * state. Variation is handled via configuration manager 
        * KCamCrFeature2ndCameraOrientation key.  
        * @param aLock, Lock orientation state flag. 
        *               ETrue, ask state and lock it.
        *               EFalse, ask current state.
        * @return ETrue if secondary camera orientation must be landscape
        */
        virtual TBool IsQwerty2ndCamera( TBool aLock = EFalse ) = 0;  

        /**
        * Returns the active camera
        * @since 2.8
        * @return either primary, secondary or none ( if in process of switching )
        */
        virtual TCamActiveCamera ActiveCamera() const = 0;

        /**
        * Handles the shutter key press
        * @param aPressed ETrue if the shutter is pressed, EFalse if released
        * @since 2.8
        */
        virtual void HandleShutterKeyL( TBool aPressed ) = 0;
        
        /**
        * Set embedding flag 
        * @param aEmbedding ETrue if we are embedding (ie Messaging, Realplayer), 
        * EFalse if not
        * @since 2.8
        */
        virtual void SetEmbedding( TBool aEmbedding ) = 0;
        
        /**
        * Return embedding flag 
        * @return aEmbedding ETrue if we are embedding (ie Messaging, Realplayer), 
        * EFalse if not
        * @since 2.8
        */
        virtual TBool Embedding() const = 0;
        
        /**
        * Set SendAs flag 
        * @param aSendAsInProgress ETrue if SendAs in progress, EFalse if not
        * @since 2.8
        */
        virtual void SetSendAsInProgressL( TBool aSendAsInProgress ) = 0;

        /**
        * Read SendAs flag value 
        * @return ETrue if sending file is in progress, EFalse otherwise  
        * @since 4.0
        */        
        virtual TBool SendAsInProgress() const = 0;

        /**
        * Return handle to active palette if supported
        * @since 3.0
        * @return Handle to Active palette
        */               
        virtual MActivePalette2UI* ActivePalette() const = 0;    
        
        /**
        *
        *
        */
        virtual CCamActivePaletteHandler* APHandler() const = 0;

        /**
        * Indicates whether or not the application is in a simulated exit situation
        * @return ETrue if the application has pretended to exit, else EFalse 
        */
        virtual TBool IsInPretendExit() const = 0;
        
        /**
        * Whether or not the telephone application is in the foreground
        * @since 3.0
        * @return ETrue if the telephone application is in the foreground
        */    
        virtual TBool ForegroundAppIsPhoneApp() const = 0;

        /**
         * Let the AppUi know what is being shown in pre-capture
         * @param aMode What's being shown
         */
        virtual void SetPreCaptureMode(TCamPreCaptureMode aMode) = 0;
        
        /*
        * Gets the handle to the controller, which is used inturn by the
        * plugin to call the interfaces for performing needed actions.
        */
        virtual MCamAppController& AppController() = 0;
        
        /*
        * Gets the handle to the settings model, which is used inturn by the
        * plugin to call the interfaces for performing needed actions.
        */
        virtual MCamStaticSettings& StaticSettingsModel() = 0;
        
        /**
        * Returns whether an MMC removed note is pending
        * @since 3.0
        * @return TBool ETrue if the UI needs to show an MMC 
        */
        virtual TBool IsMMCRemovedNotePending() const = 0;

        /**
         * Set the requested resolution for a new file
         * @since 3.2
         * @param aRequestedResolution the requested resolution from the new file request.
         */
         virtual void SetRequestedNewFileResolution( const TSize& aRequestedResolution ) = 0;

         /**
         * Get the requested resolution for a new file
         * @since 3.2
         * @return the requested resolution from the new file request.
         *  Default is TSize(0,0) which interpreted as mms quality resolution
         */
         virtual TSize RequestedNewFileResolution() const = 0;

         /**
         * Whether or not the application is in the background
         * @since 2.8
         * @param aIncludeNotifier If EFalse then the application is not
         * classed as being in the background if the foreground application is
         * a notifier, e.g. charging enabled.
         * @return ETrue if the application is in the background
         */   
         virtual TBool AppInBackground( TBool aIncludeNotifier ) const = 0;

    protected: 

        /**
        * Offers a pointer to appui's TextResolver instance
        * @since 3.0
        * @return pointer to appui's TextResolver instance   
        */
        virtual CTextResolver* TextResolver() = 0;

    private:

    private:    // Data
    
    };

#ifdef _DEBUG
const TUint16* const KCamPreCaptureModeNames[] =
  {
  (const TUint16* const)_S16("ECamPreCapViewfinder"),
  (const TUint16* const)_S16("ECamPreCapCaptureSetup"),
  (const TUint16* const)_S16("ECamPreCapTimeLapseSlider"),
  (const TUint16* const)_S16("ECamPreCapStandby"),
  (const TUint16* const)_S16("ECamPreCapSceneSetting"),
  (const TUint16* const)_S16("ECamPreCapGenericSetting")
  };
#endif


#endif      // CAMAPPUIBASE_H   
            
// End of File
