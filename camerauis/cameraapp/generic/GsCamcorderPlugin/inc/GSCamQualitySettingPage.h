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
* Description:  Setting page for Image/Video quality.*
*/



#ifndef GSCAMQUALITYSETTINGPAGE_H
#define GSCAMQUALITYSETTINGPAGE_H

//  INCLUDES
#include <aknsettingpage.h>
#include "CamSettingsInternal.hrh"

// FORWARD DECLARATIONS
class CGSCamQualitySettingContainer;
class MAknQueryValue;
class MCamAppController;

// CLASS DECLARATION

/**
*  Setting page for the quality control. This setting page is customised
*  so that it contains a custom control and not the editor control
*  supplied by CAknSettingPage. The base class doesn't allow for custom
*  controls to be used in settings pages, so this class provides this
*  functionality.
*
*  @since 2.8
*/

class CGSCamQualitySettingPage : public CAknSettingPage
	{
public: // Constructors, etc.
	/**
	* Construct the quality setting page
    * @since 2.8
    * @param aMode           The current capture mode
    * @param aSettingTitleText	    Text at top of setting pane
    * @param aSettingNumber		    Number at top left (if present)
    * @param aControlType			Determines the type constructed and how its resource is read
    * @param aEditorResourceId	    Editor resource to use in the setting page (if present)
    * @param aSettingPageResourceId	Setting Page to use (if present)
    * @param aQueryValue			reference to a query value object
	*/
	CGSCamQualitySettingPage( TCamCameraMode aMode, 
                              TDesC& aSettingTitleText,
                              TInt aSettingNumber, 
                              TInt aControlType,
                              TInt aEditorResourceId, 
                              TInt aSettingPageResourceId,
                              MAknQueryValue& aQueryValue );

    /**
	* Destructor
    * @since 2.8
	*/
    ~CGSCamQualitySettingPage();

    /**
    * Symbian 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();

public: // Functions from base classes

    /**
    * From CAknSettingPage
    * Called when the state of the quality control changes.
    * Used to preview the new setting value
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

    /**
    * From CAknSettingPage
    * Used to inform the setting item that a new value has been selected
    * @since 2.8
    */
    void SelectCurrentItemL();

    /**
    * From CCoeControl
    * Standard Constructs the control from the resource
    * @since 2.8
    * @param	aReader	The resource reader
    */
    void ConstructFromResourceL( TResourceReader& aReader );

    /**
    * From CCoeControl
    * Standard CCoeControl routine to return the number of component controls
    * @since 2.8
    * @return the number of component controls
    */
	TInt CountComponentControls() const;

    /**
    * From CCoeControl
    * Standard CCoeControl routine to return the control at a given index
    * @since 2.8
    * @param anIndex index at which to return control
    * @return the control at the given index
    */ 
    CCoeControl* ComponentControl( TInt anIndex ) const;

    /**
    * From CCoeControl
    * Used the draw the setting page
    * @since 2.8
    * @param aRect the invalid rect to draw
    */ 
    void Draw( const TRect& aRect ) const;

    /**
    * From CCoeControl
    * Called when the size of the setting page changes
    * @since 2.8
    */ 
    void SizeChanged();

    /**
    * From CCoeControl
    * Used to handle key events
    * @since 2.8
    * @param aKeyEvent the key event to handle
    * @param aType the type of key event
    * @return whether the key was handled
    */ 
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

private:
    /**
    * Called when the setting value is to be previewed
    * @since 2.8
    * @param aPtr pointer to the object that called the timer
    * @return Error code
    */
    static TInt PreviewSettingChange( TAny* aPtr );

private: // data
    // used to inform the setting item of a change in setting page
    MAknQueryValue& iQueryValue;
    // the app controller
    MCamAppController& iController;
    // the current capture mode
    TCamCameraMode iMode;
    // Used to perform the preview setting change
    CPeriodic* iTimer;
    // the custom control (quality container ) used in this setting page
    CGSCamQualitySettingContainer* iQualityContainer;
    // the setting item
    TInt iSettingItemId;
    // the current value of the setting item
    TInt iCurrentVal;
	};

#endif // GSCAMQUALITYSETTINGPAGE_H

// End of File
