/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class for AP2
*
*/


/**
 * @file ActivePalette2UI.h
 * Interface class for AP2
 */

#ifndef _ACTIVE_PALETTE_2_UI_H
#define _ACTIVE_PALETTE_2_UI_H


#include <e32std.h>

class CCoeControl;
class CHuiControl;
class CBitmapContext;
class MActivePalette2Observer;
class TActivePalette2NavigationKeys;
class TActivePalette2ItemVisible;

/**
* The interface for the Active Palette. The ActivePalette2Factory is used to instantiate
* objects that provide this interface.
*/
class MActivePalette2UI
    {
public: 
    /**
     * Installs an item to the AP. 
     * @param aItemVisible The ID and visibility of the new item
     * @param aPluginUid The UID of the plugin to handle this item
     * @param aCustomDataDes Descriptor data to pass to the plugin
     * @return Standard error code
     */
	virtual TInt InstallItemL(const TActivePalette2ItemVisible& aItemVisible, 
						const TUid& aPluginUid, 
						const TDesC8& aCustomDataDes) = 0;
						
    /**
     * Installs an item to the AP. 
     * @param aItemVisible The ID and visibility of the new item
     * @param aPluginUid The UID of the plugin to handle this item
     * @param aCustomDataInt Integer data to pass to the plugin
     * @return Standard error code
     */
	virtual TInt InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
						const TUid& aPluginUid, 
						TInt aCustomDataInt = 0) = 0;

    /**
     * Installs an item to the AP. 
     * @param aItemVisible The ID and visibility of the new item
     * @param aPluginUid The UID of the plugin to handle this item
     * @param aCustomDataInt Integer data to pass to the plugin
     * @param aCustomDataDes Descriptor data to pass to the plugin
     * @return Standard error code
     */
	virtual TInt InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
						const TUid& aPluginUid, 
						TInt aCustomDataInt, 
						const TDesC8& aCustomDataDes) = 0;
	
    /**
     * Removes an item
     * @param aItemId The resource ID of the item to install
     * @return Standard error code
     */
	virtual TInt RemoveItem(TInt aItemId) = 0;

    /**
     * Sets the visibility of an item
     * @param aItemId The item's ID
     * @param aIsVisible The visibility state
     * @return Standard error code
     */
	virtual TInt SetItemVisibility(TInt aItemId, TBool aIsVisible) = 0;

    /**
     * Find out the visibility of a specified item
     * @param aItemId The item's ID
     * @param aIsVisible On return, indicates the visibility of the item
     * @return Standard error code
     */
	virtual TInt GetItemVisibility(TInt aItemId, TBool& aIsVisible) const = 0;

    /**
     * Gets the list of currently installed items. 
     * The list will include all items, both visible and invisible.
     * @see SetItemList()
     * 
     * @param aItemVisibleList On return, will contain the item list.
     * @return Standard error code
     */
    virtual TInt GetItemList(RArray<TActivePalette2ItemVisible>& aItemVisibleList) const = 0;

    /**
     * Sets the order and visibility of installed items.
     * aItemVisibleList may be incomplete (i.e. not all installed item IDs must be present). 
     * In this case, the non-specified items will be shuffled down to the end of the AP, 
     * whilst retaining their relative order.
     * If the item that was in focus before the call is still visible, it retains the focus, 
     * else the first visible item gains focus.
     *
     * @see GetItemList()
     *
     * @param aItemVisibleList The list describing the order and visibility of the items
     * @return Standard error code
     */
    virtual TInt SetItemList(const RArray<TActivePalette2ItemVisible>& aItemVisibleList) = 0;

    /**
     * Sends a message to the specified item's handling plugin
     * @param aItemId The item's ID
     * @param aMessageId Message ID
     * @param aDataDes Custom data passed as a descriptor
     * @return Error code
     */
	virtual TInt SendMessage(TInt aItemId, TInt aMessageId, const TDesC8& aDataDes) = 0;

    /**
     * Sends a message to the specified item's handling plugin
     * @param aItemId The item's ID
     * @param aMessageId Message ID
     * @param aDataInt Custom data passed as an integer
     * @return Standard error code
     */
	virtual TInt SendMessage(TInt aItemId, TInt aMessageId, TInt aDataInt) = 0;

    /**
     * Gets the currently focussed item
     * @param aItemId On return, will contain the ID of the item in focus
     * @return Standard error code
     */
	virtual TInt GetCurrentItem(TInt& aItemId) const = 0;
	
    /**
     * Sets the currently focussed item
     * @param aItemId The ID of the item to focus
     * @return Standard error code
     */
	virtual TInt SetCurrentItem(TInt aItemId) = 0;

    /**
     * Hides or shows the palette.
     * After calling this function, and until the transition (if any) is complete, the AP will
     * not accept any user input.
     *
     * @see SetPaletteVisibilityAnimationDuration()
     * @see GetPaletteVisibilityAnimationDuration()
     *
     * @param aVisible If ETrue, shows the AP, otherwise hides it
     * @param aAnimated Whether the transition should be animated. If not, the change is made as soon 
     *        as aDelayedStartMilliseconds has passed
     * @param aDelayedStartMilliseconds An optional pause before the transition begins.
     * @return Standard error code
     */
    virtual TInt SetPaletteVisibility(TBool aVisible, TBool aAnimated, TInt aDelayedStartMilliseconds = 0) = 0;
    
    /**
     * Sets the duration of the SetPaletteVisibility animation.
     *
     * @see SetPaletteVisibility()
     * @see GetPaletteVisibilityAnimationDuration()
     *
     * @param aTimeInMilliseconds How long the animation should take, in milliseconds
     * @return Standard error code
     */
    virtual TInt SetPaletteVisibilityAnimationDuration(TInt aTimeInMilliseconds) = 0;
        
    /**
     * Gets the duration of the SetPaletteVisibility animation.
     *
     * @see SetPaletteVisibility()
     * @see SetPaletteVisibilityAnimationDuration()
     *
     * @param aTimeInMilliseconds On return, will contain how long the animation will take, in milliseconds
     * @return Standard error code
     */
    virtual TInt GetPaletteVisibilityAnimationDuration(TInt& aTimeInMilliseconds) const = 0;
    
    /**
     * Move the AP.
     * @param aTopLeft Point specifying where to move the AP to
     */
	virtual void LocateTo(const TPoint& aTopLeft) = 0;
	
    /**
     * Returns the top left point of the AP
     * @return A TPoint indicating the top left point of the AP
     */
	virtual TPoint Location() const = 0;

    /**
     * Returns the underlying CCoeControl, if any.
     * @return A valid CCoeControl pointer if the underlying implementation is a CCoeControl; NULL otherwise.
     */
    virtual CCoeControl* CoeControl() = 0;
    
    /**
     * Returns the underlying CHuiControl, if any.
     * @return A valid CHuiControl pointer if the underlying implementation is a CHuiControl; NULL otherwise.
     */
    virtual CHuiControl* HuiControl() = 0;

    /**
     * Sets the graphics context to draw to.
     * If called with aGc being non-NULL, the AP will be rendered to the supplied context
     * in future drawing operations. If called with NULL, the screen's GC will be used instead.
     * Calls to this will only have an effect if a CCoeControl-based AP has been created.
     * 
     * @see RenderActivePalette()
     *
     * @param aGc The graphics context to draw to
     */
    virtual void SetGc(CBitmapContext* aGc = NULL) = 0;

    /**
     * Forces the AP to draw itself to the supplied aRect. 
     * Uses the context previously set by SetGc, if any; uses the screen's context otherwise.
     * Calls to this will only have an effect if a CCoeControl-based AP has been created.
     * 
     * @see SetGc()
     *
     * @param aRect The rect to render to
     */
    virtual void RenderActivePalette(const TRect& aRect) const = 0;

    /**
     * Sets the active palette observer used for callbacks. Calls to this function replace the previously 
     * set observer. SetObserver(NULL) may be called to remove the previously set observer without specifying 
     * a new observer
     *
     * @param aObserver The new observer to use
     */
    virtual void SetObserver(MActivePalette2Observer* aObserver) = 0;

    /**
     * Gets a list of available external plugins. 
     *
     * @param aPluginList On return, will contain a list a valid plugin UIDs
     * @return Standard error code
     */
    virtual TInt GetAvailablePlugins(RArray<TUid>& aPluginList) const = 0;

    /**
     * Sets the keys the user will press to navigate the AP. This sets the scancodes responded to when 
     * CCoeControl::OfferKeyEvent, CHuiControl::OfferEvent or similar are called.
     *
     * @param aNavigationKeys The new navigation keys
     */
    virtual void SetNavigationKeys(const TActivePalette2NavigationKeys& aNavigationKeys) = 0;

    /**
     * Destructor. Will clean up all installed plugins.
     */        
    virtual ~MActivePalette2UI() {};
};

#endif // _ACTIVE_PALETTE_2_UI_H
