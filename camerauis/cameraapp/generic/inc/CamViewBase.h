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
* Description:  Base class for all camera views*
*/


#ifndef CAMVIEWBASE_H
#define CAMVIEWBASE_H

// ===========================================================================
//  INCLUDES

#include "CamAppUiBase.h"
#include "CamCommandHandlerAo.h"
#include <aknview.h>

// ===========================================================================
// FORWARD DECLARATIONS
class CCamAppController;
class CCamContainerBase;
class CCamStandbyContainer;
class CCamWaitDialog;


// ===========================================================================
// CLASS DECLARATION

/**
*  Base class for all camera views
*
*  @since 2.8
*/
class CCamViewBase : public CAknView
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamViewBase();

    public: // New functions
        
        virtual void HandleAppEvent( const TCamAppEvent& aEvent );

     	/**
        * Update the softkeys for the current situation.
        * @since 2.8
        */
        virtual void UpdateCbaL();  
        
        /**
        * Blank the softkeys on the cba
        * @since 3.0
        */
        virtual void BlankSoftkeysL();  

        /**
        * Switch from current mode to standby mode
        * @since 2.8
        * @param aViewId the current view id
        * @param aError associated error code
        */
        virtual void SwitchToStandbyModeL( TCamAppViewIds aViewId, TInt aError );

        /**
        * Exit standby mode
        * @since 2.8
        */
        void ExitStandbyModeL();
        
        /**
        * Exit all modes.
        * Has empty default implementation which can be overriden
        * if needed in inherited classes. Atleast user scene setup view does this.
        * Called e.g. when entering standby mode to be able to enter normal
        * VF mode again when exiting standby.
        */
        virtual void ExitAllModesL();

        /**
        * Handle change of focus to another application
        * This only handles another application coming to the foreground when
        * the camera application is already in the background behind an eikon
        * server window
        * @since 2.8
        */
        virtual void HandleFocusLossL();
        
        /**
        * Handle change of mode
        * @since 3.0
        */
        virtual void PrepareForModeChange();

        /**
        * Called to identify the help context for this view
        * @since 2.8
        * @param aContext Help context object to populate with control context
        * information
        */
        virtual void GetHelpContext(TCoeHelpContext& aContext) const;

        /**
        * Returns the currently used container for this view
        * @since 2.8
        * @return   The currently used container
        * information
        */
        CCamContainerBase* Container();

        /**
        * Sets the softkeys to the specified Resource ID
        * @since 3.0
        * @param aResource ID of the softkey resource to use
        */
        virtual void SetSoftKeysL(TInt aResource);
        
        /**
        * Blanks the softkeys for the Courtesy UI Off state. 
        * @since 3.0
        */
        void SetCourtesySoftKeysL();
        
        /**
        * Returns the softkeys to their previous state after UI has been turned back on
        * @since 3.0
        */
        void UnsetCourtesySoftKeysL();
    
        /**
         * Returns whether the view is in standby mode
         * @return Whether the view is in standby mode
         */
        TBool IsInStandbyMode();

        /**
        * Requests that the camera application redraws the supplied active palette area
        * @since 3.0
        * @param aArea the area to redraw
        */
        void RedrawAPArea(const TRect& aArea);


        /**
        * Return a pointer to view specific CBA (or applications CBA if
        * there is no view specific CBA defined).
        * @since S60 v5.0
        * @return Pointer to CEikButtonGroupContainer. Ownership not
        *         transferred
        */
        CEikButtonGroupContainer* ViewCba();
        
        /**
         * Updates toolbar icons.
         * 
         */
        virtual void UpdateToolbarIconsL();
        
        /**
        * Sets the standby to the current error if the current state is in standby; else does nothing
        * @since 5.1
        * @aError System wide error code
        */
        virtual void SetStandbyStatusL( TInt aError );

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
        * Active object for handling toolbar commands
        * @since S60 v.5.0
        */
        void HandleCommandAoL( TInt aCommand );  

    protected:  // Functions from base classes

        /**
        * From CAknView.
        * @since 2.8
        * @param aPostvViewId the ID for previous view we are switching from
        * @param aCustomMessageId the Uid for message passed to this view
        * @param aCustomMessage descriptor containing data content for view specific message
        */
        void DoActivateL( const TVwsViewId& aPostvViewId, TUid aCustomMessageId,
                                                    const TDesC8& aCustomMessage );

        /**
        * From CAknView.
        * @since 2.8
        */
        void DoDeactivate();
        
    protected:  // New functions

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aController Reference to either the application controller base class or test base class
        */
        CCamViewBase( CCamAppController& aController );

        /**
        * Create the container associated with this view.
        * @since 2.8
        */
        virtual void CreateContainerL() = 0;

        /**
        * Set the view's title text
        * @since 2.8
        */
        virtual void SetTitlePaneTextL() = 0;       

        /**
        * Show rename query and rename file.
        * Switch orientation if required. Calls DoQueryRenameL()
        * @since 2.8
        * @param aMode The current mode.
        */
        void QueryRenameL( TCamCameraMode aMode );
        
        /**
        * Show rename query and rename file.
        * @since 2.8
        * @param aMode The current mode.
        */
        void DoQueryRenameL( TCamCameraMode aMode );

        /**
        * Show an overwrite confirmation query. This is used
        * when trying to overwrite a file using rename.
        * @since 2.8
        * @param aFilename The name of the file that trying to overwrite.
        * @return ETrue if the user has confirmed the overwrite.
        */
        TBool ShowOverwriteConfirmationQueryL( 
            const TDesC& aFilename );

        /**
        * Show rename query.
        * @since 2.8
        * @param aFilename The current name of the file that renaming.
        * @param aMode The current capture mode.
        * @return ETrue if the user has entered a name. Or EFalse, if 
        *         user has cancelled the rename.
        */
        TBool ShowRenameQueryL( TDes& aFilename, TCamCameraMode aMode );

        /**
        * Check if renaming to the given name is valid.
        * @since 2.8
        * @param aFilename The new name to check.
        * @return ETrue if the name is valid.
        */
        virtual TBool IsNewNameValidL( const TDesC& aFilename );

        /**
        * Renames the current capture array file.
        * @since 2.8
        * @param aNewName The new name for the current file.
        * @param aMode The current capture mode.
        * @return ETrue if the name has been renamed succesfully.
        */
        TBool RenameCurrentFileL( const TDesC& aNewName, TCamCameraMode aMode );

        /**
        * Returns whether the view is a precapture view or not
        * @since 3.0
        * @returns ETrue if view is precapture
        */
        virtual TBool IsPreCapture();

        /**
        * Increment the camera user count.
        * This method ensures that view only increments the counter by one.
        */        
        void IncrementCameraUsers();
        
        /**
        * Decrement the camera user count.
        * If count goes to zero, camera is released.
        * This method ensures, that decrement call is only forwarded to
        * controller, if this view has earlier incremented the count.
        */
        void DecrementCameraUsers();
        
        /**
        * Is this view using camera, i.e. has IncrementCameraUsers been called,
        * but DecrementCameraUsers not.
        */
        TBool IsCameraUser() const;
        
public:
        /**
        * Returns whether the view is a postcapture view or not
        * @since 3.0
        * @returns ETrue if view is postcapture
        */
        virtual TBool IsPostCapture();
private:

        /**
        * Returns whether the view is a burst capture postcapture view or not
        * @since 3.0
        * @returns ETrue if view is burst post capture postcapture
        */
        virtual TBool IsBurstPostCapture();

        /**
         * Sets the iStandbyModeActive flag
         * @param aActive What to set iStandbyModeActive to
         */
        void SetStandbyModeActive(TBool aActive);

    protected:  // Data
        
        // Base class for Camera application controller
        CCamAppController& iController; 
 
        // The container for the view's controls
        CCamContainerBase* iContainer; // owned

        // Specifies whether the application is embedded or not.
        TBool iEmbedded;

        // Resource Id for the view menu bar 
        TInt iPreviousMenuResourceId;

        // set to ETrue when in standby mode
        TBool iStandbyModeActive;

        // error code associated with standby mode
        //TInt iStandbyError;

        // standby mode container
        CCamStandbyContainer* iStandbyContainer;
        
    private: // data

        // whether or not the view registered an interest in resources
        // when it came to the foreground
        TBool iIncEngineCount;


        // Stores the current and previous resources used for setting the CBA
        // Allows one level of "undo", currently used by the courtesy UI.
        TInt iCurrentCbaResource;
        TInt iPreviousCbaResource;
        TBool iCourtesySoftkeys;
        
        // own.
        CCamCommandHandlerAo* iCommandHandlerAo;        
        
    };

#endif      // CAMVIEWBASE_H   
            
// ===========================================================================
// End of File
