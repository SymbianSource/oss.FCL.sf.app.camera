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
* Description:  Active Palette item*
*/


/**
 * @file ActivePalette2Item.h
 * Active Palette item
 */

#ifndef _ACTIVE_PALETTE_2_ITEM_H
#define _ACTIVE_PALETTE_2_ITEM_H

//  INCLUDES
#include <e32std.h>

#include "CActivePalettePluginBase.h"


// FORWARD DECLARATIONS
class CActivePalettePluginBase;
class CActivePalette2Model;
class CGulIcon;
class CFbsBitmap;

// CONSTANTS
/// The print plugin UID
const TInt KPrintPluginUid = 0x200009E2;
/// The print plugin DLL filename
_LIT(KPrintPluginDLLName,"PrintActivePalette.dll");

/// The print intent plugin UID
const TInt KPrintIntentPluginUid = 0x200009CE;
/// The print intent plugin DLL filename
_LIT(KPrintIntentPluginDLLName,"PrintIntentActivePalette.dll");

/// The online sharing  plugin UID
const TInt KOnlineSharingPluginUid = 0x20007189;
/// The print intent plugin DLL filename
_LIT(KOnlineSharingPluginDLLName,"OnlineSharingActivePalette.dll");


#ifdef ACTIVEPALETTE_SUPPORT_PLUGIN_CAM
/// The cam plugin UID
const TInt KCamAPPluginUid = static_cast<TInt>(0x8000FFFF);
/// The cam plugin DLL filename
_LIT(KCamAPluginDLLName,"CamAPPlugin.dll");
#endif // ACTIVEPALETTE_SUPPORT_PLUGIN_CAM


// CLASS DECLARATIONS

/**
 * Stores the Item ID
 */
class CActivePalette2IDHolder : public CBase
    {
public:

	/**
    * Constructor
    * @param aItemId item ID
    */
	CActivePalette2IDHolder(TInt aItemId);

	/**
    * Constructor
    */
	CActivePalette2IDHolder();

	/**
    * Returns the item ID
    * @return The item ID
    */
	inline TInt ItemId(void) const;
	
    /** 
    * Sets the item ID
    * @param aItemId The item ID
    */
	void SetItemId(TInt aItemId);

private: // Data
    /// The item ID
	TInt	iItemId;
    };

/**
 * Item for use in the AP
 */
class CActivePalette2Item : public CActivePalette2IDHolder, 
                            private MActivePalettePluginObserver
    {
public:

    /** 
    * Public constructor
    * @param aItemId The item ID
    * @param aPluginUid The plugin UID
    * @param aModel The model being used
    * @return Newly constructed object
    */
	static CActivePalette2Item * NewLC(TInt aItemId, 
									  const TUid& aPluginUid,
									  CActivePalette2Model& aModel);

    /** 
    * Destructor
    */
	virtual ~CActivePalette2Item();
	
    /** 
    * Returns the UID of the plugin handling this item
    * @return The UID of the plugin handling this item
    */
	TUid PluginUid(void) const;
	
    /** 
    * Returns a reference to the plugin handling this item
    * @return A reference to the plugin handling this item
    */
	CActivePalettePluginBase& Plugin(void) const;
	
    /** 
    * Returns a reference to the item's icon as a bitmap
    * @return A reference to the item's icon as a bitmap
    */
	CFbsBitmap& Icon(void) const;

    /** 
    * Returns a pointer to the item's mask as a bitmap
    * @return A pointer to the item's mask as a bitmap
    */
	CFbsBitmap* Mask(void) const;

    /** 
    * Returns a pointer to the item's tooltip text
    * @return A pointer to the item's tooltip text
    */
	HBufC*	Tooltip(void) const;
	
    /** 
    * Sets the item position
    * @param aItemPosition The new position
    */
	void SetItemPosition(TInt aItemPosition);

    /** 
    * Sets the visibility of the item
    * @param aIsVisible The new visibility
    */
	void SetItemVisible(TBool aIsVisible);

    /** 
    * Initialise the plugin handling this item
    * @param aCustomDataDes Custom data descriptor
    * @param aCustomDataInt Custom data integer
    */
	void InitPluginL(const TDesC8& aCustomDataDes, TInt aCustomDataInt);

private:

    /** 
    * Constructor
    * @param aItemId The item's ID
    * @param aPluginUid The UID of the plugin to handle this item
    * @param aModel A reference to the active palette model
    */
	CActivePalette2Item(TInt aItemId,
	                   const TUid& aPluginUid,
	                   CActivePalette2Model& aModel);
	
    /** 
    * 2 phase construction
    */
	void ConstructL(void);
	
    /** 
    * Returns a plugin factory from a given UID
    * @param aUid The UID of the plugin
    * @return A pointer to the plugin instantiation method
    */
	NActivePalettePluginFactory::TInstantiatePluginLFunc FindPluginFactoryL(const TUid aUid);

    /** 
    * Returns the plugin creation parameters
    * @return The plugin creation parameters
    */
	NActivePalettePluginFactory::TPluginCreationParams GetPluginCreationParams(void);

    /** 
    * Performs the setting of an icon to this item
    * @param aIcon A pointer to the icon
    * @param aOnwershipTransfer Whether to transfer ownership of the icon
    */
	void DoSetItemIcon(CGulIcon* aIcon, TBool aOnwershipTransfer);
	
private:
/// @name From MActivePalettePluginObserver
//@{
	void SetItemIconAndTooltipL(CGulIcon* aIcon, 
								TBool aOnwershipTransfer,
								const TDesC& aTooltipText);
	void SetItemIcon(CGulIcon* aIcon, TBool aOnwershipTransfer);
	void SetItemTooltipL(const TDesC& aTooltipText);
	void NotifyItemComplete(const TDesC8& aDataDes);
	void NotifyItemComplete(TInt aDataInt);
	void NotifyPluginDestroyed(CActivePalettePluginBase* aPlugin);
	void NotifyPluginCreated(CActivePalettePluginBase* aPlugin);
	void SendMessage(TInt aMessageID, const TDesC8& aDataDes);
	void SendMessage(TInt aMessageID, TInt aDataInt);
//@}

private:
	/// The plugin UID used for the item
	TUid		    iPluginUid;
    /// The model
	CActivePalette2Model& iModel;

    /// Plug-in instance. Owned.
	CActivePalettePluginBase*	iPlugin;
	/// The plugin library
	RLibrary		iDLL;

    /// Current icon information. @see iOwnsIcon
	CGulIcon* 		iIcon;
	/// Whether this instance owns the icon data
	TBool			iOwnsIcon;

    /// The current tooltip. Owned.
	HBufC* 			iTooltip;				// owned
    };

#include "ActivePalette2Item.inl"

#endif // _ACTIVE_PALETTE_2_ITEM_H

// End of File
