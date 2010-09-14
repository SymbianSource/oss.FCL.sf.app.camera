/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Setup pane handler/controller class.
*/


#ifndef CAMMODESETUPPANEHANDLER_H
#define CAMMODESETUPPANEHANDLER_H

// INCLUDES
#include <E32Base.h>
#include <w32std.h>
#include "CamSetupPaneModel.h"
#include "CamLastCaptureHelper.h"

// FORWARD DECLARATIONS
class CCamAppController;
class CCoeControl;
class CCamSetupPaneItemArray;
class CCamSetupPane;


// CLASS DECLARATION

/**
* Mode setup pane controller class.
*/
class CCamModeSetupPaneHandler : public CBase,
                                 public MCamSetupPaneModel
	{
public:
    enum TEvent
        {
        EEventLeft,
        EEventRight,
        EEventSelect,
        EEventCancel,
        EEventEditUser
        };
public:
	/**
    * Two-phase class constructor.
    * @since 3.0
    * @param aController - A ref to a base app controller.
    * @return The new object.
    */
	static CCamModeSetupPaneHandler* NewLC( CCamAppController& aController );

	/**
    * Two-phase class constructor.
    * @since 3.0
    * @param aController - A ref to a base app controller.
    * @return The new object.
    */
	static CCamModeSetupPaneHandler* NewL( CCamAppController& aController );

	/**
    * C++ class destructor.   
    * @since 3.0
    */
	~CCamModeSetupPaneHandler();

public:
	/**
    * Creates and returns a new pane control.
    * @since 
    * @param aParent - The parent (container) control.
    * @param aRect - the rectangle this control is to occupy.
    * @return The new pane control.
    */
	CCoeControl* CreatePaneControlL( CCoeControl* aParent, const TRect& aRect );

	/**
    * Handles events forwarded from the pane control.
    * @since 3.0
    * @param aEvent - The event to handle
    */		
	void HandleEvent( TEvent aEvent );

	/**
	* Moves the current highlight item.
    * @since 3.0
    */
	void MoveToPreviousItem();

	/**
	* Moves the current highlight item.
    * @since 3.0
    */
	void MoveToNextItem();
	
	/**
	* Sets the context of the Setup Pane, based on the mode it is in
    * @since 3.0
    * @param aFirstCamera ETrue if first camera in use, else EFalse
    * @param aPhoto ETrue if in photo mode, else EFalse if in video
    * @param aUserSceneSetup ETrue if in user scene setup mode.
    */
	void SetMenuContextL( TBool aFirstCamera, TBool aPhoto, TBool aUserSceneSetup = EFalse );

	/**
	* Activates a particular sub-menu.
    * @since 3.0
    * @param aMenuItem The id of the submenu to activate.
    */
	void ActivateMenuItem( TInt aMenuItem );
	
	/**
	* Set whether SetupPane control is in foreground or not
    * @since 3.0
    * @param aForeground ETrue if in foreground, EFalse if in background
    */
	void SetForeground( TBool aForeground );
	
public:
	/**
    * From   MCamSetupPaneModel
    * @since 3.0 
    * @return The number of items in the model.
    */
	TInt NumberOfItems() const;

	/**
    * From   MCamSetupPaneModel
    * @since 3.0
    * @return The title text in the model.
    */
	const TDesC& TitleText() const;

	/**
    * From   MCamSetupPaneModel
    * @since 3.0
    * @return The descriptor text in the model.
    */
	const TDesC& DescriptorText() const;

	/**
    * From   MCamSetupPaneModel
    * @since 3.0
    * @param aItemIndex - The model item index to get an icon for.
    * @return The corresponding icon.
    */
	CGulIcon& Icon( TInt aItemIndex );

	/**
    * From   MCamSetupPaneModel
    * @since 3.0
    * @return The index of the highlighted model item.
    */
	TInt HighlightedItemIndex() const;

	/**
    * From   MCamSetupPaneModel
    * @since 3.0
    * @return The id of the highlighted model item.
    */
	TInt HighlightedItemId() const;
	
	/**
    * From   MCamSetupPaneModel
    * @since 3.0 
    * @param aItemIndex Index of the SetupPane menu item
    * @return ETrue if this item is the LastCaptured thumbnail, else EFalse
    */
	TBool IconIsThumbnail( TInt aItemIndex ) const;

	/**
    * From   MCamSetupPaneModel
    * @since 3.0 
    * @param aSmall Whether requesting the small (ETrue) or large (EFalse) thumbnail icon
    * @return The number of items in the model.
    */
    CGulIcon& Thumbnail( TBool aSmall );
    
    /**
    * From   MCamSetupPaneModel
    * @since 3.0
    */
    void Refresh();

private:
	/**
    * C++ constructor.
    * @since 3.0
    * @param aController - A ref to a base app controller object.
    */
	CCamModeSetupPaneHandler( CCamAppController& aController );

	/**
	* Carriers out two-phase class construction.
    * @since 3.0
    */
	void ConstructL();

	/**
    * Handles a pane highlighted item change.
    * @since 3.0
    */
	void HandlePaneItemChanged();

	/**
    * Handles a pane highlighted item change.
    * @since 3.0
    */
	void HandlePaneItemLevelItemChanged();

	/**
    * Handles a pane highlighted menu item selection.
    * @since 3.0 
    * @param aValueToBeHighlighted The value of the selected pane to be highlighted.
    */
	void HandlePaneMenuLevelSelection( TInt aValueToBeHighlighted );
	
	/**
    * Handles user selection of the "LastCapture" thumbnail where available
    * @since 3.0 
    */	
	void HandleLastCaptureSelection();	

	/**
    * Handles a pane highlighted item-level selection.
    * @since 3.0
    */
	void HandlePaneItemLevelSelection();

	/**
    * Handles a pane highlighted item-level selection.
    * @since 3.0
    */
	void HandlePaneCancel();

	/**
	* Updates the setup pane based on the specified resource
    * @since 3.0
    * @param aResource The resource specifying the new pane contents
    */    
	void UpdateFromResourceL( TInt aResource );

	/**
	* Deletes the contents of the array, and resets internal state
    * @since 3.0
    */        
	void ResetArray();
	
	/**
	* Switches to the User Scene Setup view.
    * @since 3.0
    */        
	void SwitchToUserSceneView();


private:
	// Setup pane modes - top ('menu') level, or bottom ('item') level.
	enum TSetupPaneLevel
		{
		EMenuLevel,
		EItemLevel
		};

	// Various definitions:
	enum
		{
		KSettingsArrayCount = 6,	// Number of settings arrays.
		KItemArrayGranularity = 4,	// Granularity of item arrays.
		KMaxTitleCharLength = 32	// Max character length of title text.
		};

private:
	// The app controller object (gives access to settings).
	CCamAppController& iController;

	// The current level that the pane is at (menu or item level).
	TSetupPaneLevel iPaneLevel;

	// The zero-based index of the currently highlighted item.
	TInt iHighlightedItemIndex;

	// Zero-based index of the current settings array (=the selected menu item).
	TInt iCurrentSettingsArrayIndex;

	// IDs of the pane menu items (+ one dynamic item).
	TInt iMenuItemIds[KSettingsArrayCount + 1];

	// Title text for each menu item (+ one dynamic item)
	TBuf<KMaxTitleCharLength> iMenuTitles[KSettingsArrayCount + 1];

	// 'Item-level' settings item arrays.
	CCamSetupPaneItemArray* iSettingItems[KSettingsArrayCount];

	// Pointer to a pane control object (not owned).
	CCamSetupPane* iPaneControl;
	
	// Pointer to the owning object (not owned)
	CCoeControl* iParent;
    
    // Number of settings in the iSettingItems array
	TInt iNoOfSettingsItems;
	
	// Specifies whether the user scene setup is currently being edited.
	TBool iUserSceneSetupMode;

	// Helper class to get the last-captured thumbnails where available.
	CCamLastCaptureHelper* iLastCaptureHelper;
	
    // The current mode.  ETrue if Photo mode, EFalse if video mode
	TBool iPhotoMode;
	
    // Whether the SetupPane is currently in foreground or not.
	TBool iForeground;		
	
	// Title to use for the LastCapture selection
	HBufC16* iLastCapTitle;
	
	// Filename of last capture
	TFileName iLastCapName;
	};

#endif
