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
* Description:  Setting item for Location setting item in image settings*
*/




#ifndef GSCUSTOMQUALITYSETTINGITEM_H
#define GSCUSTOMQUALITYSETTINGITEM_H

//  INCLUDES
#include "CamStaticSettingsModel.h"
#include <aknsettingitemlist.h>

// CLASS DECLARATION

/**
* Setting item for image/video location texts
* @since 2.8
*/
class CGSCustomQualitySettingItem : public CAknEnumeratedTextPopupSettingItem
    {
    public: // Constructors and destructor
        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to the controller 
        * @param aMode       current mode (video or still image)
        * @param aIdentifier setting page resource id
        * @param aValue      current name base type
        */
        CGSCustomQualitySettingItem( 
                                    TInt aSettingItemId,
                                    TInt& aValue,
                                    CCamStaticSettingsModel& aModel,
                                    TCamCameraMode aMode
                                   );

        /**
        * Destructor
        */
        ~CGSCustomQualitySettingItem();
    
    public:  // Functions from base classes

     
    private: // Functions from base classes

	    /**
        * From CAknEnumeratedTextPopupSettingItem
	    * Virtual Method called by framework to complete the construction.  
	    * Calls UpdateListTextL to set the list text when the item is first 
        * constructed
        * @since 2.8
	    */
	    void CompleteConstructionL();
	    
	    
	    
    private: // Data
        // reference to the app controller
        CCamStaticSettingsModel& iModel; 
        // The current mode of capture.
        TCamCameraMode iMode;
        RArray<TInt> iSupportedQualityLevels;
      
    };

#endif // CAMLOCATIONSETTINGITEM_H

// End of File
