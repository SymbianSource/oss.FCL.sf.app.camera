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
* Description:  Ensures that the container for the user scene setup is constructed*
*/


#ifndef CAMSTILLUSERSCENESETUPVIEW_H
#define CAMSTILLUSERSCENESETUPVIEW_H

//  INCLUDES
#include "CamUserSceneSetupViewBase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CCamAppController;

/**
* Ensures that the container for the user scene setup is constructed
* using the photo user scene setup resource.
*
*  @since 2.8
*/
class CCamStillUserSceneSetupView : public CCamUserSceneSetupViewBase
    {
    protected: // data types


    public:  // Constructors and destructor
      
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param A reference to the controller.
        * @return A pointer to the new CCamStillUserSceneSetupView object.
        */
        static CCamStillUserSceneSetupView* NewLC( CCamAppController& aController );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamStillUserSceneSetupView();

    public: // Functions from base classes
        /**
        * From CAknView.
        * @since 2.8
        * @return UID of view
        */
        TUid Id() const;

    protected:  // Constructors etc.


    protected:  // From base class

        void CreateContainerL();
        /**
        * Starts the view finder.
        * @since 2.8
        */
        void StartViewFinder();

        /**
        * Stops the view finder.
        * @since 2.8
        */
        void StopViewFinder();

        /**
        * Exit capture setup mode.
        * @since 2.8
        */
        void ExitCaptureSetupModeL();

        /**
        * From CCamCaptureSetupViewBase
        * Enter InfoListBox mode
        * Creates a container to replace the normal container on the control stack. 
        * Updates the Cba, title pane and navi pane.
        * @param aMode
        * @since 3.0
        */
        void SwitchToInfoListBoxL( TCamInfoListBoxMode aMode ); 
        
        /**
        * From CCamCaptureSetupViewBase
        * Handle InfoListBox item selection
        * Updates the Cba, title pane and navi pane.
        * @since 3.0
        */        
        void HandleInfoListBoxSelectionL();

        /**
        * From MEikMenuObserver Changes MenuPane dynamically
        * @param aResourceId Resource Id
        * @param aMenuPane Handle to menu pane
        * @since 2.8
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:
        /**
        * C++ default constructor.
        * @param aController Reference to either the application controller 
        * base class or test base class
        * @since 2.8
        */
        CCamStillUserSceneSetupView( CCamAppController& aController );

        /**
        * 2nd phase constructor.
        * @since 2.8
        */
        void ConstructL();

    private: // From base class
        /**
        * Exits the user scene setup mode
        * @since 2.8
        */
        void ExitUserSceneSetupModeL();

        /**
        * Switches to the user scene setup mode.
        * @since 2.8
        */
        void SwitchToUserSceneSetupModeL();

      
        /**
        * Switches the current mode to scene setting.
        * @since 2.8
        */
        void SwitchToSceneSettingModeL();

        /**
        * Exit scene setting mode.
        * @since 2.8
        */
        void ExitSceneSettingModeL();


    private:   // data
    };

#endif      // CAMSTILLUSERSCENESETUPVIEW_H 
            
// End of File
