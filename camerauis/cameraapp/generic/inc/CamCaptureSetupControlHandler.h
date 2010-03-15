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
* Description:  Class for creating, initializing and observing*
*/



#ifndef CAMCAPTURESETUPCONTROLHANDLER_H
#define CAMCAPTURESETUPCONTROLHANDLER_H

//  INCLUDES
#include <e32base.h>
 
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "CamSettingValueObserver.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CCamAppController;
class CCoeControl;

// CLASS DECLARATION

/**
* Class to create, initialize and observe a capture set-up control
*
*  @since 2.8
*/
class CCamCaptureSetupControlHandler : public CBase,
                                       public MCamSettingValueObserver

    {
    public:  // Constructors and destructor
        
        /**
        * Constructor.
        * @since 2.8
        * @param aController The application controller providing access to the 
        *                   camera settings
        * @param aSettingType An enumeration value used to indicate the setting
        *                   to be edited
        */
        CCamCaptureSetupControlHandler( CCamAppController& aController,
                                        TCamSettingItemIds aSettingType );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamCaptureSetupControlHandler();

    public: // New functions

        /**
        * Create and initialize a new capture setup control
        * ownership of the control is transferred to the calling class.
        * @since 2.8
        * @return A pointer to the new control
        */
        CCoeControl* CreateCaptureSetupControlL( const CCoeControl* aParent,
               TBool aSkinnedBackGround = EFalse  );

        /**
        * From MCamSettingValueObserver
        * Handle an update to the setting that is created by the capture setup controller
        * @since 2.8
        */
        void HandleSettingValueUpdateL( TInt aNewValue );
        
        /**
        * Return the id of the title pane resource for the current setting type
        * @since 2.8
        * @return A title pane resource id
        */
        TInt ControlTitleResourceId() const;

        /**
        * Whether or not the control requires a viewfinder preview pane
        * @since 2.8
        * @return ETrue if the control needs a viewfinder pane, else EFalse
        */
        TBool ControlUsesViewFinder() const;

        /**
        * Whether or not the control is of listbox type
        * @return ETrue if listbox, else EFalse
        */
        TBool ControlIsListBox() const;

        /**
        * Whether or not the control is of slider type
        * @return ETrue if slider, else EFalse
        */
        TBool ControlIsSlider() const;

        /**
        * Return the id of the position resource for the current setting type
        * @since 2.8
        * @return A position resource id
        */
        TInt ControlPositionResourceId() const;

		/**
        * Return the id of the viewfinder resource for the current setting type
        * @since 2.8
        * @return A viewfinder resource id
        */
        TInt ViewfinderResourceId() const;

        /**
        * Return Identifies which setting is being edited
        * @since 2.8
        * @return Identifies which setting is being edited
        */
        TCamSettingItemIds SettingType() const;

    private:   

        /**
        * Return the id of the resource to construct the control
        * @since 2.8
        * @return the control's resource id
        */
        TInt ResourceForControl() const;

    private:    // Data
        // Provides access to the settings model
        CCamAppController& iController;
        // Identifies which setting is being edited
        TCamSettingItemIds iSettingType;
    };

#endif      // CAMCAPTURESETUPCONTROLHANDLER_H
            
// End of File
