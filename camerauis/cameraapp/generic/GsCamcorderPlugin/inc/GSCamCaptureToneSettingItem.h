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
* Description:  Setting item for capture tone
*/


#ifndef GSCAMCAPTURETONESETTINGITEM_H_
#define GSCAMCAPTURETONESETTINGITEM_H_

//  INCLUDES
#include <aknsettingitemlist.h>

/**
* Setting item for capture tone selection
* @since 5.0
 */
class CGSCamCaptureToneSettingItem : public CAknEnumeratedTextPopupSettingItem
    {
public:
    // Constructors and destructor
    /**
     * C++ constructor
     * @since 5.0
     * @param aIdentifier setting page resource id
     * @param aValue      current name base type
     */
    CGSCamCaptureToneSettingItem(TInt aIdentifier,
            TInt& aValue);
    
    /**
     * Destructor
     */
    virtual ~CGSCamCaptureToneSettingItem();
    
protected:// Functions from base classes
    /**
     *
     * Virtual method called by framework to complete the construction.  
     * In this fuction, if the "off" option is disabled in the cenrep,
     * it is removed from the setting item list (Enumerated List)
     */
    void CompleteConstructionL();
    };

#endif /* CGSCAMCAPTURETONESETTINGITEM_H_ */
