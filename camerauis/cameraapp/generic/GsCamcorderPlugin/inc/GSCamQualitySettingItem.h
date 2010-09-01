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
* Description:  Setting item for image/video quality*
*/




#ifndef GSCAMQUALITYSETTINGITEM_H
#define GSCAMQUALITYSETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>
#include "CamSettingsInternal.hrh"


class CCamStaticSettingsModel;

/**
* Setting item for image/video quality texts
* @since 2.8
*/
class CGSCamQualitySettingItem : public CAknEnumeratedTextPopupSettingItem
    {
    public: // Constructors and destructor
        /**
        * C++ constructor
        * @since 2.8
        * @param aMode       current mode (video or still image)
        * @param aIdentifier setting page resource id
        * @param aValue      current name base type
        */
        CGSCamQualitySettingItem( TCamCameraMode aMode, 
                                  TInt aIdentifier,                               
                                  TInt& aValue,
                                  CCamStaticSettingsModel& aModel );

        /**
        * Destructor
        */
        virtual ~CGSCamQualitySettingItem();

    protected: // Functions from base classes
        /**
        * From CAknEnumeratedTextPopupSettingItem
        * @since 2.8
        * Framework method for constructing the launching setting page.
        * @return   a constructed CAknSettingPage object on the heap
        */
        CAknSettingPage* CreateSettingPageL();

        /**
        *
        * Virtual method called by framework to complete the construction.  
        * In this class, the two arrays, pointers to which can be obtained by calling 
        * EnumeratedTextArray() and PoppedUpTextArray() are created.
        *
        */
        void CompleteConstructionL();

    private: // Data
        // The current mode of capture.
        TCamCameraMode iMode;
        // Array of supported quality levels
        RArray<TInt> iSupportedQualitiesArray;
        
        CCamStaticSettingsModel& iModel;
    };

#endif // GSCAMQUALITYSETTINGITEM_H

// End of File
