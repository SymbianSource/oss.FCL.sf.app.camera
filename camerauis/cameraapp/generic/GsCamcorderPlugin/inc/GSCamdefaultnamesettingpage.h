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
* Description:  Setting page for name base type (date/text)*
*/




#ifndef GSCAMDEFAULTNAMESETTINGPAGE_H
#define GSCAMDEFAULTNAMESETTINGPAGE_H

//  INCLUDES
#include <aknpopupsettingpage.h>
#include "CamSettingsInternal.hrh"
#include "Cam.hrh"

/**
* Setting page for name base type (date/text)
* @since 2.8
*/
class CGSCamDefaultNameSettingPage : public CAknPopupSettingPage
    {
    public: // Constructors and destructor
        /**
        * C++ constructor.
        * Parameters are passed directly to CAknPopupSettingPage.
        * See CAknPopupSettingPage constructor for parameter details.
        * @since 2.8
        * @param aNameBase current name base (in/out)
        * @param aMode     current mode (video or still image)
        * @param aSettingTitleText title to display in page
        * @param aSettingNumber the setting number
        * @param aControlType the type of control
        * @param aEditorResourceId the id of the editor resource
        * @param aSettingPageResourceId the id of the page resource
        * @param aQueryValue reference to the query value
        */
        CGSCamDefaultNameSettingPage(
            TDes& aNameBase,
            TCamCameraMode aMode,
            const TDesC* aSettingTitleText, 
            TInt aSettingNumber, 
            TInt aControlType,
            TInt aEditorResourceId, 
            TInt aSettingPageResourceId,
            MAknQueryValue& aQueryValue,
            TBool   aCamera );

        /**
        * Destructor
        * @since 2.8
        */
        virtual ~CGSCamDefaultNameSettingPage();

    protected: // Functions from base classes
        /*
        * From CAknSettingPage.
        * @since 2.8
        * Check if the name base value is ok, so the user can
        * exit the setting page.
        *
        * @param    aAccept ETrue if the user has indicated to accept the
        *                   setting page; EFalse otherwise
        * @return   TBool   a value indicating whether the setting page
        *                   should be dismissed
        */
        TBool OkToExitL( TBool aAccept );
        
    private: // New methods
    
       /*
        * Display the editor dialog
        * @since 3.0
        *
        * @return   TBool   a value indicating whether the setting page
        *                   should be dismissed
        */  
        TBool ShowEditorDialogL();

    private: // Data
        // Reference to the cuurent name base.
        TDes& iNameBase;
        // Used to store new name base so that it can be validated
        // before overwriting old value.
        TBuf<KMaxNameBaseLength> iTempNameBase;
        // The current mode of capture.
        TCamCameraMode iMode;
        TBool iCamera;
    };

#endif // GSCAMDEFAULTNAMESETTINGPAGE_H
