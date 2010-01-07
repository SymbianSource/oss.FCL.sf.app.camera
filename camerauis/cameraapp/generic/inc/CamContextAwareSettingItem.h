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
* Description:  Setting item type for CAknEnumeratedItems.*
*/




#ifndef CAMCONTEXTAWARESETTINGITEM_H
#define CAMCONTEXTAWARESETTINGITEM_H

//  INCLUDES
 
#include <AknSettingItemList.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCamAppController;    

// CLASS DECLARATION

/**
*  Setting item class similar to standard avkon CAknEnumeratedTextPopupSettingItem.
*  Allows the text array to be updated dynamically on construction according
*  to any limitations on available items imposed by other setting values or available
*  memory
*
*  @since 2.8
*/

class CCamContextAwareSettingItem : public CAknEnumeratedTextPopupSettingItem
	{
public:
	/**
	* Construct a list, storing in it a resource (to be read at 2nd stage construction)
	* and a reference to an externally owned integer - the value to be manipulated
    * @since 2.8
    * @param aController reference the the application controller
    *        provides access to query for a revised resource id
    * @param aSettingItemId the setting item being constructed
    * @param aValue the current value of the item
	*/
	CCamContextAwareSettingItem( CCamAppController& aController,
                                 TInt aSettingItemId, 
                                 TInt& aValue );



private: // Functions from base classes

	/**
    * From CAknEnumeratedTextPopupSettingItem
	* Virtual Method called by framework to create the setting page.  
	* Creates setting page that switches cba depending on setting value
    * that user navigates to.
    * @since 2.8
	*/
    CAknSettingPage* CreateSettingPageL();

	/**
    * From CAknEnumeratedTextPopupSettingItem
	* Virtual Method called by framework to complete construction of
    * the setting page.  
    * Determines whether or not the value list is to be limited (visually) 
    * and replaces the popup text array with a new array created from the 
    * revised resource id.
    * @since 2.8
	*/
    void CompleteConstructionL();

private: // data

    // Used to determine whether a particular setting value can be
    // set by the user.
    CCamAppController& iController; 

	};

#endif // CAMCONTEXTAWARESETTINGITEM_H

// End of File
