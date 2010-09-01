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
* Description:  Base class for pre capture views*
*/


#ifndef CAMCAPTURESETUPVIEWBASE_H
#define CAMCAPTURESETUPVIEWBASE_H

//  INCLUDES
#include "CamViewBase.h"

// FORWARD DECLARATIONS
class CCamCaptureSetupControlHandler;
class CCamCaptureSetupContainer;
class CCamShootingModeContainer;
class CCamInfoListBoxContainer;

// CLASS DECLARATION

/**
* Base class for views that need to run capture setup
*
*  @since 2.8
*/
class CCamCaptureSetupViewBase : public CCamViewBase
    {
    public: // data types

    /**
    * Setting mode
    */
    enum TCamSettingMode
        {
        ECamSettingModeNone,
        ECamSettingModeCaptureMenu,
        ECamSettingModeCapture,
        ECamSettingModeScene,
        ECamSettingModeInfoListBox
        };

		// Info listbox mode 
	enum TCamInfoListBoxMode
		{
		EInfoListBoxModeNone,
		EInfoListBoxModeTimeLapse,
		EInfoListBoxModeISO
        };

    public:  // Constructors and destructor
        
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamCaptureSetupViewBase();

    public: // Functions from base classes

     /**
        * Handle events sending app to foreground or background
        * @param aForeground whether the event brings the app to fore or background
     */       
        void HandleForegroundEventL( TBool aForeground );
        
        /**
        * From CAknView Handle commands
        * @since 2.8
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * Handle change of focus to another application
        * This only handles another application coming to the foreground when
        * the camera application is already in the background behind an eikon
        * server window
        * @since 2.8
        */
        void HandleFocusLossL();
        
        /**
        * Revert to normal mode
        * @since 2.8
        */
        virtual void ExitAllModesL();

    public: // New functions

        /**
        * From CAknView deactivate the view (free resources)
        * @return void
        */
        void DoDeactivate();
        
         /**
          * Sets the iSceneSettingModeActive flag
          * @param aActive What to set iSceneSettingModeActive to
          */
        void SetSceneSettingMode(TBool aActive);
        
         /**
          * Sets the iInfoListBoxActive flag
          * @param aActive What to set iInfoListBoxActive to
          */
        void SetInfoListBoxMode(TBool aActive, TBool aFullySkinned=EFalse);        
        
         /**
          * Sets the iCaptureSetupModeActive flag
          * @param aActive What to set iCaptureSetupModeActive to
          */
        void SetCaptureSetupModeActive(TBool aActive);
            
    protected:  // New functions

        /**
        * C++ default constructor.
        * @param aController Reference to either the application controller 
        * base class or test base class
        * @since 2.8
        */
        CCamCaptureSetupViewBase( CCamAppController& aController );

        /**
        * Creates a control handler for the appropriate control type
        * @since 2.8
        * @param aSetupCommand The command that initiated capture setup mode, used to 
        *                        determine the required control type
        */
        void CreateCaptureSetupControlHandlerL(  TInt aSetupCommand );

        /**
        * Enter capture setup mode.
        * Creates a capture setup control handler and a second container to replace 
        * the normal container on the control stack. Updates the Cba, title pane 
        * and navi pane.
        * Derived classes should override this method to add any additional 
        * functionality they require e.g. starting the viewfinder in the
        * appropriate capture mode if it is not already active.
        * @since 2.8
        * @param aSetupCommand The command that initiated capture setup mode, used to 
        *                        determine the required control type
        */
        virtual void SwitchToCaptureSetupModeL( TInt aSetupCommand, TBool aFullScreenVF=ETrue );

        /**
        * Exit capture setup mode.
        * Replaces the normal container on the control stack. 
        * Reverts the Cba and title pane.
        * Derived classes should override this method to add any additional 
        * functionality they require e.g exiting the viewfinder and
        * reverting the navi pane if necessary when switching back to the view's 
        * normal mode.
        * @since 2.8
        */
        virtual void ExitCaptureSetupModeL();


        /**
        * Enter scene setting mode.
        * Creates a container to replace the normal container on the control stack. 
        * Updates the Cba, title pane and navi pane.
        * @since 2.8
        */
        virtual void SwitchToSceneSettingModeL();

        /**
        * Exit capture setup mode.
        * Replaces the normal container on the control stack. 
        * Reverts the Cba and title pane.
        * @since 2.8
        */
        virtual void ExitSceneSettingModeL();

        /**
        * Enter InfoListBox mode
        * Creates a container to replace the normal container on the control stack. 
        * Updates the Cba, title pane and navi pane.
        * @param aMode
        * @since 3.0
        */
        virtual void SwitchToInfoListBoxL( TCamInfoListBoxMode aMode, TBool aFullySkinned=EFalse );        

        /**
        * Handle InfoListBox item selection
        * Updates the Cba, title pane and navi pane.
        * @since 3.0
        */
   		virtual void HandleInfoListBoxSelectionL();
        
        /**
        * Exit InfoListBox
        * Updates the Cba, title pane and navi pane.
        * @since 3.0
        */        
        virtual void ExitInfoListBoxL();

        /**
        * Determine the title pane resource for the current setup mode
        * @since 2.8
        * @return Id of the title pane text for current setup mode
        */
        TInt SetupModeTitlePaneResourceId();

        /**
        * From MEikMenuObserver Changes MenuPane dynamically
        * @param aResourceId Resource Id
        * @param aMenuPane Handle to menu pane
        * @since 2.8
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        void CaptureSetupModeSelection();
    private:

        /**
        * Cleans up the capture setup container if a leave occurs
        * while switching to capture setup mode
        * @param aAny pointer to the object to cleanup
        * @since 2.8
        */
        static void CleanupCaptureSetupContainer( TAny* aAny );

        /**
        * Cleans up the scene setting container if a leave occurs
        * while switching to scene setting mode
        * @param aAny pointer to the object to cleanup
        * @since 2.8
        */
        static void CleanupSceneSettingContainer( TAny* aAny );

        /**
        * Cleans up InfoListBox if a leave occurs
        * while switching to scene setting mode
        * @param aAny pointer to the object to cleanup
        * @since 2.8
        */
        static void CleanupInfoListBox( TAny* aAny );

        /**
        * Cleans up the containers if a leave occurs
        * while exiting scene setting/capture setup mode
        * @param aAny pointer to the object to cleanup
        * @since 2.8
        */
        static void CleanupExit( TAny* aAny );

    protected: // data
        // Used to determine when the view is in capture setup mode
        TBool iCaptureSetupModeActive;

        // Used to determine when the view is in scene settings mode
        TBool iSceneSettingModeActive;
        
        // Used to determine InfoListBox is visible
        TBool iInfoListBoxActive;

        // Resource Id for title view when in capture setup or scene setting mode
        TInt iSettingModeTitleResourceId;

        // Used to determine when user has pressed change button        
        TBool iChangeButtonPressed;

    protected:
        // A container that the view uses when in scene settings mode
        CCamShootingModeContainer* iSceneSettingContainer;        
        
        // Container for other settings using info listbox
        CCamInfoListBoxContainer* iInfoListBoxContainer;
        // Purpose of current instance of iInfoListBoxContainer
        TCamInfoListBoxMode iInfoListBoxMode;                
        
        TBool iForceAvkonCBA;
    private:   // data

        // Handler for the capture setup control
        CCamCaptureSetupControlHandler* iCaptureSetupControlHandler;
        
        // A second container that the view uses when in capture setup mode
        CCamCaptureSetupContainer* iCaptureSetupContainer;
        
        // Stores the previous capture mode of camera before entering Infolistbox mode 
        TCamPreCaptureMode iPrevCaptureMode;

    };

#endif      // CAMCAPTURESETUPVIEWBASE_H 
            
// End of File
