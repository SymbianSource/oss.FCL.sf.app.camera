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
* Description:  Setting item type for "Add to album".*
*/




#ifndef CAMALBUMSETTINGITEM_H
#define CAMALBUMSETTINGITEM_H

//  INCLUDES
#include <AknSettingItemList.h>


class CAknSingleGraphicPopupMenuStyleListBox;
class CCamStaticSettingsModel;

/**
*  Setting item class similar to standard avkon CAknEnumeratedTextPopupSettingItem.
*  Allows the setting list array to be updated dynamically to show a currently selected album
*  name. The setting page displays "Yes" and "No options and displays an album selection list 
*  popup if the user selects the "Yes" option.
*
*/

class CCamAlbumSettingItem : public CAknEnumeratedTextPopupSettingItem
	{
public:
	/**
	* Construct a list, storing in it a resource (to be read at 2nd stage construction)
	* and a reference to an externally owned integer - the value to be manipulated
    * @since 2.8
    * @param aSettingItemId the setting item being constructed
    * @param aValue the current value of the item
	*/
	CCamAlbumSettingItem( TInt aSettingItemId, TInt& aValue );

	/**
	* Destructor
    * @since 2.8
	*/
	~CCamAlbumSettingItem();

public:  // Functions from base classes

    /**
    * From MAknSettingPageObserver
	* Virtual Method called by framework to handle events in the
    * setting page. Used to launch an album selection list.
    * @since 2.8
    * @param aSettingPage the setting page reporting the event
    * @param aEventType the type of event reported
	*/
	void HandleSettingPageEventL( CAknSettingPage* aSettingPage,
                                  TAknSettingPageEvent aEventType );

public:  // New methods

	/**
	* Replace the "Yes" text in the setting list with the currently 
    * selected capture album. Text in the setting page is unchanged.
    * @since 2.8
	*/
    void UpdateListTextL();

private: // Functions from base classes

	/**
    * From CAknEnumeratedTextPopupSettingItem
	* Virtual Method called by framework to complete the construction.  
	* Calls UpdateListTextL to set the list text when the item is first 
    * constructed
    * @since 2.8
	*/
	void CompleteConstructionL();

private: // New methods

    /**
	* Launches an album selection list
    * @since 2.8
    * @return Whether or not an album was selected from the list
	*/
    void DisplayAlbumSelectionListL();

    /**
	* Handle situations where the capture album does not exist or a request for
    * information on the capture album fails
    * @since 2.8
	*/
    void HandleAlbumError();

private: // data

    // List of Album IDs - One for each unique album name
    TBool iAlbumExists;
    HBufC* iAlbumTitle;
	};

#endif // CAMALBUMSETTINGITEM_H

// End of File
