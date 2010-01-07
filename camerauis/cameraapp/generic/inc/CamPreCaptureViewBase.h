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
* Description:  Base class for pre-capture views*
*/


#ifndef CAMPRECAPTUREVIEWBASE_H
#define CAMPRECAPTUREVIEWBASE_H

#include <akntoolbarobserver.h>

#include "CamCaptureSetupViewBase.h"

#include "CamControllerObservers.h"
#include "CamAppController.h"

// FORWARD DECLARATION
class CCamCaptureSetupMenu;
class CCamStandbyContainer;
class MAknsSkinInstance; 
class TAknsItemID;
class CAknButton;

// CLASS DECLARATION

/**
* Base class for precapture views
*
*  @since 2.8
*/
class CCamPreCaptureViewBase : 	public CCamCaptureSetupViewBase,
								public MCamControllerObserver,
								public MAknToolbarObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamPreCaptureViewBase();

    public: // Functions from base classes

        /**
        * From CAknView Handle commands
        * @since 2.8
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView Handles the foreground/background event
        * @since 2.8
        * @param aForeground ETrue if this view is on foreground, else EFalse.
        */
        void HandleForegroundEventL( TBool aForeground );

        /**
        * From MCamControllerObserver
        * @since 2.8
        * @param aEvent The enumerated code for the event received
        * @param aError The error code associated with the event
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );
        
        /**
        * From CAknView.
        * @since 2.8
        * @param aPreViewId the ID for previous view we are switching from
        * @param aCustomMessageId the Uid for message passed to this view
        * @param aCustomMessage descriptor containing data content for view specific message
        */
        void DoActivateL( const TVwsViewId& aPreViewId, TUid aCustomMessageId,
                                                    const TDesC8& aCustomMessage );
        /**
        * From CAknView.
        * @since 2.8
        */
        void DoDeactivate();

        /**
        * From CCamViewBase
        * Handle change of focus to another application
        * This only handles another application coming to the foreground when
        * the camera application is already in the background behind an eikon
        * server window
        * @since 2.8
        */
        void HandleFocusLossL();

        /**
        * From CCamCaptureSetupViewBase
        * Revert to normal mode
        * @since 2.8
        */
        virtual void ExitAllModesL();
        
    public: // New functions

       /**
        * From CCamCaptureSetupViewBase
        * Return the active setting mode
        * @since 2.8
        * @return the active setting mode
        */
        TCamSettingMode ActiveSettingMode();

        /**
         * AppUi calls this to notify views of focusgained and focuslost
         * events.
         */
        void HandleAppEvent( const TCamAppEvent& aEvent );        
        
        /**
         * Sets a flag to true to indicate that access point dialog is shown and
         * viewfinder needs to be restarted.
         */        
        void SetLocationTrailConnecting();
        
        /**
         * AppUi calls this to indicate if postcapture view will be visible
         * in near future.
         */        
        void SetPostCaptureViewAsumption( TBool aAssumePostCaptureView );
        
        /**
        * Try to start the capture with MSK command. 
        * @since 9.1
        * @return ETrue if started.
        */
        virtual TBool StartMskCaptureL() = 0;

    protected:    // Functions from base classes

        /**
        * From CCamPreCaptureViewBase 
        * Set the menu bar resource.
        */
        virtual void SetMenuBar() = 0;

        /**
        * From CCamCaptureSetupViewBase
        * Switches the current mode to capture setup and activates a 
        * specific control.
        * @since 2.8
        * @param aSetupCommand that specifies which setup control type to activate.
        */
        void SwitchToCaptureSetupModeL( TInt aSetupCommand );

        /**
        * From CCamCaptureSetupViewBase
        * Switches the current mode to scene setting and activates a 
        * specific control.
        * @since 2.8
        */
        void SwitchToSceneSettingModeL();

        /**
        * From CCamCaptureSetupViewBase
        * Exits capture setup mode and activates the capture setup menu control.
        * @since 2.8
        */
        void ExitCaptureSetupModeL();

        /**
        * From CCamCaptureSetupViewBase
        * Exits scene setting mode and activates the capture setup menu control.
        * @since 2.8
        */
        void ExitSceneSettingModeL();
        
        /**
        * From CCamCaptureSetupViewBase
        * Exits scene setting mode and activates the capture setup menu control.
        * @since 2.8
        */
        void ExitInfoListBoxL();
        
        /**
        * Returns whether the view is a precapture view or not
        * @since 3.0
        * @returns ETrue
        */
        virtual TBool IsPreCapture();
        
        /**
        * Update fixed toolbar icons according to current settings
        * @since S60 5.0
        */
        virtual void UpdateToolbarIconsL() = 0;
		/**
        * Redraws the toolbar and toolbarextension if visible
        * @since S60 5.1
        */
		virtual void RedrawToolBar();
        
        /**
        * Returns the toolbar button by id from either the toolbar or toolbar extension
        * @param aCommandId CommandId correpsonding to the button 
        * @returns the button or NULL
        * @since S60 5.1
        */
        CAknButton* ToolbarButtonById(const TCamCmd aCommandId) const;
        
public:
    
        /**
        * Hides a toolbar/extension icon
        * @param aCommandId CommandId correpsonding to the button
        * @returns void
        * @since S60 5.1
        */
        virtual void HideButton(const TCamCmd aCommandId);

    protected:  // New functions

        /**
        * C++ default constructor.
        * @param aController Reference to either the application controller 
        * base class or test base class
        * @since 2.8
        */
        CCamPreCaptureViewBase( CCamAppController& aController );

        /**
        * Symbian 2nd phase constructor.
        * @since 2.8
        */
        void ConstructL();

        /**
        * Enter Viewfinder mode
        * @since 2.8
        */
        virtual void StartViewFinder() = 0;

        /**
        * Exit Viewfinder mode
        * @since 2.8
        */
        virtual void StopViewFinder() = 0;

        /**
        * Switch from current mode to standby mode
        * @since 2.8
        * @param aViewId the current view id
        * @param aError associated error code
        */
        void SwitchToStandbyModeL( TCamAppViewIds aViewId, TInt aError );

        /**
        * Exit standby mode and return to previous capture mode
        * @since 2.8
        */
        void ExitStandbyModeL();

#if 0
        /**
        * Check whether the ok options menu can be displayed
        * @since 2.8
        * @return ETrue if the menu can be displayed, else EFalse
        */
        TBool IsOkOptionsMenuAllowed();
#endif // 0

        /**
        * Stop the viewfinder and inform the controller that the engine is
        * no longer required by this view
        * @since 2.8
        */
        void ReleaseResources();
        
        /**
        * Switches the current mode to capture setup menu and activates 
        * the capture setup menu control.
        * @since 2.8
        */
        void SwitchToCaptureSetupMenuModeL();

        /**
        * Exits capture setup menu mode and activates the original precapture
        * view container that was active before entering this mode.
        * @since 2.8
        */
        void ExitCaptureSetupMenuModeL();

        /**
        * Creates a capture setup menu. Derived classes should create
        * either a video or photo menu, depending on the class type.
        * @since 2.8
        */
		virtual void CreateCaptureSetupMenuL() = 0;

        /**
        * Removes the capture setup menu and original container from
        * the container stack.
        * @since 2.8
        */
        void RemoveCaptureSetupMenuContainers();		

        /**
        * Set the icon of a toolbar button
        * @since S60 5.0
        * @param aButton       The button whose icon is to be changed.
        * @param aIconFilename Full path to the MBM/MIF file
        * @param aIconId       Icon ID in MBM/MIF file
        * @param aMaskId       Mask ID in MBM/MIF file
        * @param aSkinInstance Current skin instance
        * @param aSkinItemId   Skin ID for the button graphic
        * 
        */
        void SetIconL(
            CAknButton* aButton,
            const TDesC& aIconFilename,
            TInt aIconId,
            TInt aMaskId,
            MAknsSkinInstance* aSkinInstance, 
            const TAknsItemID& aSkinItemId );

        /**
        * Initialise the switch camera menu item, used from
        * DynInitMenuPaneL()
        * @since 3.0
        * @param aMenuPane The menu pane to initialise
        */
        void DynInitSwitchCameraMenuItemL( CEikMenuPane* aMenuPane );

// from base class MAknToolbarObserver

        void OfferToolbarEventL( TInt aCommand );

    private: // new functions
  
        /**
        * Cleans up the capture setup menu if a leave occurs
        * while switching to capture setup menu mode
        * @param aAny pointer to the object to cleanup
        * @since 2.8
        */
        static void CleanupCaptureSetupMenu( TAny* aAny );
        
        /**
        * Helper method to setup the Active Palette.
        * If needed, the AP is created.
        * For existing AP, the items are first cleared.
        * New items are set with resource from GetAPResourceId.
        */        
        void SetupActivePaletteL();

    protected: // data
        
        // An extra container that the view uses when in capture setup menu mode.
        CCamCaptureSetupMenu* iCaptureSetupMenuContainer;
        
        // Used to determine when the view is in capture setup mode       
        TBool iCaptureSetupMenuModeActive;
        
        // Used to determine which capture setup menu item to select
        // when a capture setup control is exited.
        TInt iCaptureSetupMenuLastItemIndex;


        // Indicates that the application has lost the foreground but has not
        // released the resources
        TBool iContinueInBackground;

        // Indicates if the view was told to initiate standby mode from
        // activation
        TBool iActivateToStandby;
        
        // Indicates if the view was told to exit to standby mode from
        // the current mode
        TBool iDeactivateToStandby;
        
        // Indicates if toolbar extension should be invisible when returning
        // precapture view
        TBool iToolbarExtensionInvisible;
        
        // Asumption that next view is postcaptureview. If true don't start
        // viewfinder if coming to foreground from background.
        TBool iAssumePostCaptureView;
        
        TBool iLocationTrailConnecting;
		
    };

#endif      // CAMPRECAPTUREVIEWBASE_H 
            
// End of File
