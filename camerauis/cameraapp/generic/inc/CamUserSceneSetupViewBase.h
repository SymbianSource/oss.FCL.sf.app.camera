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
* Description:  Controls the switching between different control modes*
*/


#ifndef CAMUSERSCENESETUPVIEWBASE_H
#define CAMUSERSCENESETUPVIEWBASE_H


//  INCLUDES
 
#include "CamCaptureSetupViewBase.h"
#include "CamControllerObservers.h"  // MCamControllerObserver

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Controls the switching between different control modes
* for the user scene setup.
*
*  @since 2.8
*/

class CCamUserSceneSetupViewBase :  public CCamCaptureSetupViewBase,
                                    public MCamControllerObserver                                    
    {
    protected: // data types


    public:  // Constructors and destructor
      
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamUserSceneSetupViewBase() = 0;

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
        * From CAknView activate the view
        * @param aPrevViewId the id of the previously active view.
        * @param aCustomMessageId the id of the custom message
        * @param aCustomMessage the custom message
        * @return void
        * @since 2.8
        */
        void DoActivateL( const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
            const TDesC8& /*aCustomMessage*/ );

        /**
        * From CAknView deactivate the view (free resources)
        * @return void
        * @since 2.8
        */
        void DoDeactivate();

        /**
        * From MCamControllerObserver
        * @since 2.8
        * @param aEvent The enumerated code for the event received
        * @param aError The error code associated with the event
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );

        /**
        * From CCamCaptureSetupViewBase
        * Revert to normal mode
        * @since 2.8
        */
        virtual void ExitAllModesL();

    protected:  // Constructors etc.

        /**
        * C++ default constructor.
        * @param aController Reference to either the application controller 
        * base class or test base class
        * @since 2.8
        */
        CCamUserSceneSetupViewBase( CCamAppController& aController );

    protected:  // New functions to be implemented by derived classes.

        /**
        * Exits the user scene setup mode
        * @since 2.8
        */
        virtual void ExitUserSceneSetupModeL();

        /**
        * Switches to the user scene setup mode.
        * @since 2.8
        */
        virtual void SwitchToUserSceneSetupModeL();

        /**
        * Starts the viewfinder.
        * @since 2.8
        */
        virtual void StartViewFinder() = 0;
        /**
        * Stops the viewfinder.
        * @since 2.8
        */
        virtual void StopViewFinder() = 0;
        
    protected:  // From base class

        /**
        * From CCamViewBase
        * Update the softkeys for the current situation.
        * @since 2.8
        */
        void UpdateCbaL();  

        /**
        * From CCamViewBase
        * Set the view's title text
        * @since 2.8
        */
        void SetTitlePaneTextL();

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
        * Exit scene setting mode.
        * Replaces the normal container on the control stack. 
        * Reverts the Cba and title pane.
        * @since 2.8
        */
        void ExitSceneSettingModeL();
        
        /**
        * From CCamCaptureSetupViewBase
        * Exit InfoListBox
        * Updates the Cba, title pane and navi pane.
        * @since 3.0
        */        
        virtual void ExitInfoListBoxL();

    private:    // New functions
        /**
        * Switches to the capture setup mode.
        * @param command that specifies which capture setup control to use
        * in the new mode.
        * @since 2.8
        */
        void SwitchToCaptureSetupModeL( TInt aSetupCommand );

        /**
        * Returns true if only the user scene setup mode is active.
        * @since 2.8
        */
        TBool IsOnlyUserSceneSetupModeActive();
            
        
        /**
        * Displays reset user scene settings cofirmation note
        * @since 3.0
        */
        void DisplayResetUserSceneDlgL();
        
    protected:

        // Specifies whether the user scene setup mode is active
        TBool iUserSceneSetupModeActive;

        // Specifies whether the user scene page mode is active.
        TBool iUserScenePageModeActive;
        
        // Indicates that user has pressed cancel to close this view.
        TBool iCancelRequest;

        // Indicates that user has pressed OK to close this view.
        TBool iOkRequest;
        
        // Indicates that Viewfinder start has been requested. View switch
        // should not occur until viewfinder start has completed.
        TBool iVFRequested;
    };

#endif      // CAMUSERSCENESETUPVIEWBASE_H 
            
// End of File
