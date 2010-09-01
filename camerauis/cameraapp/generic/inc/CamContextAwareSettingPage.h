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
* Description:  Setting page for CAknEnumeratedItems.*
*/




#ifndef CAMCONTEXTAWARESETTINGPAGE_H
#define CAMCONTEXTAWARESETTINGPAGE_H

//  INCLUDES
#include <AknPopupSettingPage.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknEnumeratedTextPopupSettingItem;
class CCamAppController;

// CLASS DECLARATION

/**
*  Setting page class similar to standard avkon CAknPopupSettingPage.
*  Allows the Cba to be updated dynamically according to any limitations 
*  on available items imposed by other setting values or available
*  memory
*
*  @since 2.8
*/

class CCamContextAwareSettingPage : public CAknPopupSettingPage
	{
public: // Constructors, etc.
	/**
	* Construct a list, storing in it a resource (to be read at 2nd stage construction)
	* and a reference to an externally owned integer - the value to be manipulated
    * @since 2.8
    * @param aSettingText	Text at top of setting pane
    * @param aSettingNumber		Number at top left (if present)
    * @param aControlType			Determines the type constructed and how its resource is read
    * @param aEditorResourceId	Editor resource to use in the setting page (if present)
    * @param aSettingPageResourceId		Setting Page to use (if present)
    * @param aQueryValue			reference to a query value object
    * @param aParent reference to the list that created this page.
    * @param aController reference the the application controller
	*/
	CCamContextAwareSettingPage( 
					const TDesC* aSettingText, 
					TInt aSettingNumber, 
					TInt aControlType,
					TInt aEditorResourceId, 
					TInt aSettingPageResourceId, 
					MAknQueryValue& aQueryValue,
                    CAknEnumeratedTextPopupSettingItem& aParent,
                    CCamAppController& aController );

    /**
	* Destructor
    * @since 2.8
	*/
    ~CCamContextAwareSettingPage();

    /**
    * Symbian 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();

public: // From base class

    /**
    * From CAknSettingPage
    * Called when the state of the list box changes.
    * Used to update the Cba when the list box is navigated.
    * @since 2.8
    */
    void UpdateSettingL();

    /**
    * From CAknSettingPage
    * Processes events from the softkeys. Responds to EAknSoftkeyOk and EAknSoftkeyBack
    * to accept or cancel the pop-up.
    * @since 2.8
    * @param aCommandId	Event Id from the soft-key
    */
    void ProcessCommandL( TInt aCommandId );
    
    /**
    * From CAknSettingPage
    * Puts up a constructed dialog. Part of the non-waiting API calls.    
    * Activity to perform as soon as the setting page is displayed
    * @since 2.8
    * @return Whether check succeeded
    */
    TBool PostDisplayCheckL();

private:
    /**
    * Called when the setting value is to be previewed
    * @since 2.8
    * @param aPtr pointer to the object that called the timer
    * @return Error code
    */
    static TInt PreviewSettingChange( TAny* aPtr );

private: // data
    // Used to retrieve the settings value for the currently
    // highlighted list item.
    CAknEnumeratedTextPopupSettingItem& iParent;
    // Used to determine whether a particular setting value is
    // enabled or disabled for selection.
    CCamAppController& iController;
    // Used to perform the preview setting change
    CPeriodic* iTimer;
    // the current value of the setting item
    TInt iCurrentVal;
	};

#endif // CAMCONTEXTAWARESETTINGITEM_H

// End of File
