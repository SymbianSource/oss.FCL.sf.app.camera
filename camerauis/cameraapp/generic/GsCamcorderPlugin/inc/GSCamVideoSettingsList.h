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
* Description:  Video settings list class
*
*/



#ifndef GSCAMVIDEOSETTINGSLIST_H
#define GSCAMVIDEOSETTINGSLIST_H

// INCLUDES
 
#include <bldvariant.hrh> // for feature definitions

#include <aknsettingitemlist.h> // CAknSettingItemList
#include <CAknMemorySelectionDialog.h>
#include <driveinfo.h>

#include "CamStaticSettingsModel.h"
#include "GSCamcorderPlugin.hrh"

// FORWARD DECLARATIONS
class CCamStaticSettingsModel;
class CGSCamcorderPlugin;

class CCamWaitDialog;


/**
* Image settings list class
*/
class CGSCamVideoSettingsList : public CAknSettingItemList

    {
    public: // Constructors and destructor
        /** 
        * C++ constructor.
        * @since 2.1
        * @param aController reference to CCcorController
        */
        CGSCamVideoSettingsList( CCamStaticSettingsModel& aModel );

        /**
        * Symbian 2nd phase constructor
        * @since 2.1
        */
        void ConstructL( TBool aLaunchedFromGS,
                         TBool aSecondaryCameraSettings );

        /**
        * Destructor.
        */
        virtual ~CGSCamVideoSettingsList();


    public: // Functions of base classes
        /**
        * From CAknSettingItemList
        * Launch the setting page for the current item by calling
        * EditItemL on it.
        */
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );
        
        /**
        * From CCoeControl set the size and position of its component controls.
        */
        void SizeChanged();
    
 
    private: // Functions from base classes
        /**
        * From CAknSettingItemList Handles creating setting items
        * @param aIdentifier Identifier of the setting item to create
        */
        CAknSettingItem* CreateSettingItemL( TInt aIdentifier );

        /**
        * Returns the current value of a setting for this list.
        * @since 2.8
        * @param aItemId The id of the setting in question
        * @return the current value.
        */
        TInt ValueOfSetting( TInt aItemId ) const;

        /**
        * Returns the array index of a setting for this list.
        * @since 2.8
        * @param aItemId The id of the setting in question
        * @return the array index.
        */
        TInt IndexOfSetting( TInt aItemId ) const;
        
  
    private: // Data
        // Store values for the different settings.
        RPointerArray<TIntSetting> iSettings;
        // Array of supported quality levels
        RArray<TInt> iSupportedVideoQualities;

        TInt iQualityValue;
        TBool iLaunchedFromGS;
        TBool iSecondaryCameraSettings;
        TBuf<KMaxNameBaseLength> iDefaultNameBase;

        // Used to store memory value. Required by the CAknMemorySelectionSettingItemMultiDrive.
        DriveInfo::TDefaultDrives iMemVal;
        TDriveNumber iDrive;

        CCamStaticSettingsModel& iModel;
        CCamWaitDialog* iWaitDialog;
        // Index of the setting item which is active 
        TInt iIndex;
        // Flag to check if the Wait note is active
        TBool iWaitNoteActive;
    };

#endif

// End of File
