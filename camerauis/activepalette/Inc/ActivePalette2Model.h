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
* Description:  Active Palette model*
*/


/**
 * @file ActivePalette2Model.h
 * Active Palette model
 */

#ifndef _ACTIVE_PALETTE_2_MODEL_H
#define _ACTIVE_PALETTE_2_MODEL_H

//  INCLUDES
#include <e32std.h>

#include <activepalette2ui.h>
#include "CActivePalettePluginBase.h"
#include "ActivePalette2Cfg.h"
#include "activepalette2configuration.h"

// FORWARD DECLARATIONS
class CActivePalette2Item;
class CActivePalette2IDHolder;
class TActivePalette2EventData;
class MActivePalette2Observer;
class TActivePalette2ItemVisible;
class MActivePalette2ModelObserver;
class MActivePalette2TooltipTimerObserver;
class MActivePalette2FocusTimerObserver;
class MActivePalette2ItemScrollTimerObserver;
class MActivePalette2AppearanceTimerObserver;
class MActivePalette2TooltipDescriptiveObserver;
class MActivePalette2FocusDescriptiveObserver;
class MActivePalette2ItemScrollDescriptiveObserver;
class MActivePalette2AppearanceDescriptiveObserver;

/**
 * The type of user input detected.
 * @see CActivePalette2Model::ProcessInputEvent()
 */
enum TActivePalette2InputEvent {
    EAP2InputUp,     ///< User pressed up
    EAP2InputDown,   ///< User pressed down
    EAP2InputSelect  ///< User selected
};

// CLASS DECLARATIONS


/**
 * Describes the type of callback the renderer would like for all animations
 * @see CActivePalette2Model::NewL()
 */
class TRendererCallBacks
    {
public:
    /**
     * Constructor
     * By default, all categories are set to TRendererCallBackApiType::ERendererCallBackApiTypeTimer
     */
    TRendererCallBacks();

    MActivePalette2TooltipTimerObserver*            iTooltipTimer;
    MActivePalette2FocusTimerObserver*              iFocusTimer;
    MActivePalette2ItemScrollTimerObserver*         iItemScrollTimer;
    MActivePalette2AppearanceTimerObserver*         iAppearanceTimer;
    MActivePalette2TooltipDescriptiveObserver*      iTooltipDescriptive;
    MActivePalette2FocusDescriptiveObserver*        iFocusDescriptive;
    MActivePalette2ItemScrollDescriptiveObserver*   iItemScrollDescriptive;
    MActivePalette2AppearanceDescriptiveObserver*   iAppearanceDescriptive;
    };


/**
 * Active Palette model. Contains all internal logic of the AP.
 */
class CActivePalette2Model : public CBase, public MActivePalette2UI
    {
public:
/// @name Object construction & deletion
//@{
    /**
     * Destructor
     */
	virtual ~CActivePalette2Model();
	
	
    /**
     * 2 phase construction
     * @param aModelObserver A reference to the observer
     * @param aItemSize      The size of the item icons
     * @param aCallbacks     The callback styles to use
     * @param aConfiguration The runtime configuration provider. Can be NULL.
     * @return The newly-constructed object
     */
	static CActivePalette2Model* NewL( 
	  MActivePalette2ModelObserver& aModelObserver,
		TSize                         aItemSize, 
		const TRendererCallBacks&     aCallbacks,
		CActivePalette2Configuration* aConfiguration = NULL );

//@}

public:
/// @name From MActivePalette2UI
//@{
	virtual TInt InstallItemL(const TActivePalette2ItemVisible& aItemVisible, 
						const TUid& aPluginUid, 
						const TDesC8& aCustomDataDes);
	virtual TInt InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
						const TUid& aPluginUid, 
						TInt aCustomDataInt = 0);
	virtual TInt InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
						const TUid& aPluginUid, 
						TInt aCustomDataInt, 
						const TDesC8& aCustomDataDes);
	virtual TInt RemoveItem(TInt aItemId);
	virtual TInt SetItemVisibility(TInt aItemId, TBool aIsVisible);
	virtual TInt GetItemVisibility(TInt aItemId, TBool &aIsVisible) const;
    virtual TInt GetItemList(RArray<TActivePalette2ItemVisible>& aItemVisibleList) const;
    virtual TInt SetItemList(const RArray<TActivePalette2ItemVisible>& aItemVisibleList);
	virtual TInt SendMessage(TInt aItemId, TInt aMessageId, const TDesC8& aDataDes);
	virtual TInt SendMessage(TInt aItemId, TInt aMessageId, TInt aDataInt);
	virtual TInt GetCurrentItem(TInt& aItemId) const;
	virtual TInt SetCurrentItem(TInt aItemId);
    virtual TInt SetPaletteVisibility(TBool aVisible, TBool aAnimated, TInt aDelayedStartMilliseconds = 0);
    virtual TInt SetPaletteVisibilityAnimationDuration(TInt aTimeInMilliseconds);
    virtual TInt GetPaletteVisibilityAnimationDuration(TInt& aTimeInMilliseconds) const;
    virtual TInt GetAvailablePlugins(RArray<TUid>& aPluginList) const;
	virtual void LocateTo(const TPoint& aTopLeft);
	virtual TPoint Location() const;
    virtual CCoeControl* CoeControl();
    virtual CHuiControl* HuiControl();
    virtual void SetGc(CBitmapContext* aGc = NULL);
    virtual void RenderActivePalette(const TRect& aRect) const;
    virtual void SetObserver(MActivePalette2Observer* aObserver);
    virtual void SetNavigationKeys(const TActivePalette2NavigationKeys& aNavigationKeys);
//@}


public:
/// @name Renderer interrogation
//@{
    /**
     * Returns the number of items on-screen
     * @return The number of items on-screen
     */
    TInt CountItemsOnScreen(void);
    
    /**
     * Whether to show the top scroll indicator
     * @return Whether to show the top scroll indicator
     */
    TBool ShowTopScrollIndicator(void);
    
    /**
     * Whether to show the bottom scroll indicator
     * @return Whether to show the bottom scroll indicator
     */
    TBool ShowBottomScrollIndicator(void);
    
    /**
     * Gets the item at the given screen position
     * @return The item at that position
     */
    CActivePalette2Item* ItemFromScreenPos(TInt aScreenPos);

    /**
     * The current tooltip text
     * @return The current tooltip text
     */
    TDesC* TooltipText();
    
    /**
     * Whether any tooltip should be currently shown
     * @return Whether any tooltip should be currently shown
     */
    TBool ShowTooltip();
    
    /**
     * The current tooltip frame.
     * The entire animation will consist of frames 0 to TooltipTotalFrames(), and may be played backwards
     * as well as forwards.
     * @return Current frame of tooltip animation to show
     */
    TInt TooltipCurrentFrame();
    
    /**
     * Total number of frames in a complete tooltip animation
     * @return Total frames
     */
    TInt TooltipTotalFrames();
    
    /**
     * The screen position of the item showing a tooltip
     * @return The Screen pos
     */
    TInt TooltipScreenPosition();

    /**
     * The currently focused item
     * @return The focused item
     */
    TInt FocusedItem();

    /**
     * The current focus offset.
     * The entire animation will consist of frames 0 to FocusCurrentTotalOffsetFrames(), and may be played backwards
     * as well as forwards.
     * The focus ring's position should be the position of FocusedItem() + this offset, correctly scaled
     * @return Current offset of focus ring animation to show
     */
    TInt FocusCurrentOffset();

    /**
     * Total number of frames in a complete focus-change animation
     * @return Total frames
     */
    TInt FocusCurrentTotalOffsetFrames();

    /**
     * The current item scroll offset.
     * The entire animation will consist of frames 0 to ItemScrollTotalFrames(), and may be played backwards
     * as well as forwards.
     * The top item's offset should be moved upwards an amount calculated from this offset.
     * @return Current offset of item scroll animation to show
     */
    TInt ItemScrollOffset();

    /**
     * Total number of frames in a complete item-scroll animation
     * @return Total frames
     */
    TInt ItemScrollTotalFrames();

    /**
     * Whether the palette is open for user input or not
     * @return Whether user input is accepted
     */
    TBool PaletteActive();

    /**
     * The current appearance frame.
     * The entire animation will consist of frames 0 to PaletteAppearingTotalFrames(), and may be played backwards
     * as well as forwards.
     * @return Current frame of appearance animation to show
     */
    TInt PaletteAppearingCurrentFrame();        

    /**
     * Total number of frames in a complete appearance animation
     * @return Total frames
     */
    TInt PaletteAppearingTotalFrames();        

    /**
     * The current item's animation info
     * @return The current item's animation info
     */
    CActivePalettePluginBase::TPluginAnimationInfo ItemAnimInfo();
//@}

public:
/// @name User input
//@{
    /**
     * Process a user input event
     * @param aEvent The event to process
     * @return Whether the input has been consumed
     */
    TBool ProcessInputEvent(TActivePalette2InputEvent aEvent);
//@}

public:
/// @name Descriptive callbacks
//@{
    /**
     * Signal that the focus-change animation has completed
     */
    void AnimateFocusRendererComplete();

    /**
     * Signal that the palette-appearance animation has completed
     */
    void AnimateAppearanceRendererComplete();

    /**
     * Signal that the item-scroll animation has completed
     */
    void AnimateItemScrollRendererComplete();

    /**
     * Signal that the tooltip animation has completed
     */
    void AnimateTooltipRendererComplete();
//@}

public:
/// @name Item collaboration
//@{    
    /**
     * Returns the model observer
     * @return A reference to the model observer
     */
	MActivePalette2ModelObserver& Observer(void) const;

    /**
     * Returns the item icon dimensions
     * @return The item size
     */
    TSize ItemSize();

    /**
     * Item selection
     * @param aItem A reference to the item
     * @param aResult The result value
     * @param aDataDes Data descriptor
     * @param aDataInt Data integer
     */ 
	void NotifyItemComplete(const CActivePalette2Item& aItem,
	                                TInt aResult,
	                                const TDesC8& aDataDes,
	                                TInt aDataInt);

    /**
     * Message arrived
     * @param aItem A reference to the item
     * @param aResult The result value
     * @param aMessageID The message ID
     * @param aDataDes Data descriptor
     * @param aDataInt Data integer
     */ 
	void NotifyMessage(const CActivePalette2Item& aItem,
	                           const TInt aResult,
	                           const TInt aMessageID,
	                           const TDesC8& aDataDes,
	                           TInt aDataInt);

    /**
     * Graphics changes
     * @param aItem A reference to the item
     * @param aIsIconChanged Indicates if the icon has changed
     * @param aIsTooltipChanged Indicates if the toolip text has chaned
     */ 
	void NotifyGraphicsChanged(const CActivePalette2Item& aItem,
	                                   const TBool aIsIconChanged,
	                                   const TBool aIsTooltipChanged);
//@}
    	
    
private:
/// @name Object construction helpers
//@{
    /**
     * Constructor
     */
	CActivePalette2Model( MActivePalette2ModelObserver& aModelObserver,
						            TSize                         aItemSize, 
						            const TRendererCallBacks&     aCallbacks,
						            CActivePalette2Configuration* aConfiguration );
						
    /**
     * Leaving constructor
     */
	void ConstructL(void);
//@}


private:
/// @name Item manipulation
//@{
    /**
     * Returns an item from a given ID
     * @param aItemId The item ID to be found
     * @return A pointer to the item
     */
	CActivePalette2Item* FindItem(TInt aItemId) const;

    /**
     * Handles the selection of an item
     * @param aItemIndex The item ID
     */
	void HandleItemSelected(TInt aItemIndex);
	
    /**
     * Installs an item
     * @param aItemId The item's ID
     * @param aVisible Whether the item is installed as visible
     * @param aPluginUid The UID of the plugin to handle this item
     * @param aCustomDataDes Custom data descriptor for the plugin
     * @param aCustomDataInt Custom data integer for the plugin
     * @return System-wide error code
     */
	TInt DoInstallItemL(TInt aItemId,
                        TBool aVisible,
                        const TUid& aPluginUid,
                        const TDesC8& aCustomDataDes,
                        TInt aCustomDataInt);
	                                 
    /**
     * Sends a message to the plugin handling the specified item
     * @param aItemId The item's ID
     * @param aMessageID The ID of the message to send
     * @param aDataDes Custom data descriptor for the plugin
     * @param aDataInt Custom data integer for the plugin
     * @return System-wide error code
     */
	TInt DispatchMessage(TInt aItemId,
	                     TInt aMessageID,
	                     const TDesC8& aDataDes,
	                     TInt aDataInt);

    /**
     * Initialize the model's state
     */
    void InitialiseConstraints(void);
    
    /**
     * Ensure that the model's state is self-consistent and valid.
     * Call this after the item or visibility array is changed. May resize the palette, scroll it,
     * move the focus to a valid item.
     */
    void EnforceConstraints(void);

    /**
     * Finds the next visible item index, including aStartingItem.
     * @return The next visible item, or KInvalidItemIndex if none found
     */
    TInt FindVisibleItem(TInt aStartingItem);
    
    /**
     * Finds the previous visible item index, including aStartingItem.
     * @return The previous visible item, or KInvalidItemIndex if none found
     */
    TInt PreviousVisibleItem(TInt aStartingItem);
    
    /**
     * Returns the total number of visible items
     * @return The total number of visible items
     */
    TInt CountVisibleItems(TInt aStartingItem);
    
    /**
     * Returns the item index of the given screen position
     * @param aScreenPos The screen position
     * @return The item index, or KInvalidItemIndex if none
     */
    TInt ItemIndexFromScreenPos(TInt aScreenPos);
    
    /**
     * Given an item index, work out where it is on screen
     * @param aItem The item index
     * @return The screen pos
     */
    TInt ScreenPosFromItemIndex(TInt aItem);
    
    /**
     * Given an item ID, work out the item index
     * @param aItemId The item ID
     * @return The item index, or KInvalidItemIndex
     */
    TInt ItemIndexFromId(const TInt aItemId) const;
    
    /**
     * The item index of the lowermost item on-screen.
     * @return The item index
     */
    TInt BottomItemOnScreen(void);
    
    /**
     * Call whenever the items or visibility arrays are updated
     */
    void ItemsUpdated();

    /**
     * Returns whether a given item index is valid or not
     * @return Item index validity
     */
    TBool ValidItemIndex(TInt aItemIndex) const;
//@}
	

private:
/// @name Tooltips
//@{
    /**
     * Tooltip animation state
     */
	enum TTooltipState
	    {
		ETooltipInvalid,  ///< Invalid state
		ETooltipNone = 2, ///< Tooltip hidden
		ETooltipSteady,   ///< Tooltip fully shown
		ETooltipRise,     ///< Tooltip appearing
		ETooltipSet       ///< Tooltip disappearing
	    };

    /** 
     * Removes the tooltip
     */
	void RemoveTooltip();
	
	/**
	 * Start a new tooltip animation
	 */
    void CompelTooltip();

    /** 
     * Begin the steady animation phase (where the tooltip is fully visible)
     * @return Error code
     */
	TInt TooltipSteadyAniAction();

    /** 
     * Begin the rising animation phase (where the tooltip slides out)
     * @return Error code
     */
	TInt TooltipRiseAniAction();

    /** 
     * Begin the rising animation phase (where the tooltip slides in)
     * @return Error code
     */
	TInt TooltipDisappearAniAction();

    /** 
     * Sets the tooltip's state
     * @param aState New state
     */
	void TooltipSetState(TTooltipState aState);

    /** 
     * Called on tooltip timer tick
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
	static TInt TooltipTimerCallback(TAny * aPtr);

private:
    /// Current tooltip animation state
	TTooltipState   				iTooltipState;
	
	/// Tooltip animation timer. Owned
	CPeriodic*						iTooltipTimer;

    /// Total number of frames in animation
    TInt iTooltipTotalFrames;
    /// Current animation frame
    TInt iTooltipCurrentFrame;
    /// Whether the tooltip is currently being shown or not
    TBool iShowTooltip;
    /// Screen position of the tooltip
    TInt iTooltipPos;
//@}	

    
private:
/// @name Animation functions
//@{
    /**
     * The current animation state of the palette
     */
	enum TPaletteAnimState
	    {
		EPaletteAnimIdle,           ///< No animation
		EPaletteAnimItemScroll,     ///< Item scroll
		EPaletteAnimFocusChange,    ///< Focus change
		EPaletteAnimItemAnim,       ///< Item animation
		EPaletteAnimAppearing       ///< Whole-palette appearance
	    };

    /**
     * Set current animation state. Stops previous animation (if necessary) and starts new animation (if necessary)
     * @param aNewState The new animation state
     */
    void SetAnimState(TPaletteAnimState aNewState);

    /**
     * If any animations are taking place that involves individual items (item-scroll, item-anim, focus-change),
     * this completes them immediately.
     */
    void CompleteAnyItemBasedAnimations();

    /**
     * Start a change focus item.
     * Uses iFocusedItem and iFocusChangeTargetItem to work out what the animation should be
     */
    void ChangeFocus();

    /**
     * Begin the scroll-items animation
     * @param aDirection +1 to move the items downwards, -1 to move them upwards
     */
    void ScrollItems(TInt aDirection);

    /** 
     * Called on focus-change timer tick
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
	static TInt FocusChangeTimerCallback(TAny * aPtr);
	
	/**
	 * Perform action for focus-change animation timer tick.
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt FocusChangeAction();
    
    /**
     * Called when focus-change animation has completed
     */
    void FocusChangeComplete();
    
    /** 
     * Called on item-scroll timer tick
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
	static TInt ItemScrollTimerCallback(TAny * aPtr);

	/**
	 * Perform action for item-scroll animation timer tick.
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt ItemScrollAction();

    /**
     * Called when item-scroll animation has completed
     */
    void ItemScrollComplete();
    
    /**
     * Called when item-animation is starting
     */
    void StartItemAnimation();
    
    /** 
     * Called on item-animation timer tick
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    static TInt ItemAnimTimerCallback(TAny* aPtr);

	/**
	 * Perform action for item-animation animation timer tick.
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt ItemAnimAction();

    /**
     * Called when item-animation animation has completed
     */
    void ItemAnimationComplete();
    
    /** 
     * Called on palette-appearance timer tick
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    static TInt PaletteAnimTimerCallback(TAny* aPtr);

	/**
	 * Perform action for palette-appearance animation timer tick.
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt PaletteAnimAction();

    /**
     * Called when palette-appearance animation has completed
     */
    void PaletteAnimComplete();

    /**
     * Called whenever a new item gains focus
     */
    void FocusedItemUpdated();
//@}

    /**
     * Prepares a TActivePalette2EventData object
     * @param aItem The item the event concerns
     * @param aResult The result of the event
     * @return The initialized TActivePalette2EventData object
     */
    TActivePalette2EventData PrepareEventData(const CActivePalette2Item& aItem, TInt aResult);

    /**
     * Appends a plugin's UID to an array, if the plugin can be found on the device.
     * @param aPluginList The array to append to
     * @param aDllUid The UID to add if found
     * @param aDllFileName The filename of the DLL to look for
     */
    void AppendPluginIdIfAvailable(RArray<TUid>& aPluginList, const TUid aDllUid, const TFileName aDllFileName) const;

private:
/// @name Item information
//@{
    /// The currently installed items
    RPointerArray<CActivePalette2Item> iItemsArray;
    /// The visibility of each installed item.
    RArray<TBool> iVisibilityArray;
    /// The index of the top item on the screen
    TInt iTopItemOnScreen;
    /// How many items are currently on the screen
    TInt iNoItemsOnScreen;
    /// The total number of visible items
    TInt iNoVisibleItems;
    /// Currently focused item index
    TInt iFocusedItem;
    /// Previously focused item ID
    TInt iPreviousItemID;
    /// Currently focused item ID
    TInt iFocusedItemID;
//@}

private:
/// @name Animation information
//@{
    /// Timer for focus-change animation. Owned
	CPeriodic* iFocusChangeTimer;
	/// The item index being moved to
    TInt iFocusChangeTargetItem;
    /// The velocity of the focus-change animation
    TInt iFocusChangeDirection;
    /// The current focus-change frame
    TInt iFocusedCurrentOffset;
    
    /// Timer for item-scroll animation. Owned
	CPeriodic* iItemScrollTimer;
    /// The velocity of the item-scroll animation
    TInt iItemScrollDirection;              
    /// The current item-scroll frame
    TInt iItemScrollCurrentOffset;

    /// The item being animated    
    CActivePalette2Item* iAniItem;
    /// The screen position of the item being animated
    TInt iAniItemScreenPos;
    /// The animation info for the animated item
	CActivePalettePluginBase::TPluginAnimationInfo iAniItemInfo;
    /// The item animation timer. Owned.    
	CPeriodic* iItemAnimTimer;			
	/// The current item animation frame number
    TInt iAnimationFrame;

    /// The current appearance animation frame
    TInt iPaletteMoveCurrentOffset;
    /// The total number of appearance animation frames
    TInt iPaletteMoveOffsetTotalFrames;
    /// The velocity of the appearance animation 
    TInt iPaletteMoveAnimationDirection;
    /// The timer for the appearance animation. Owned
	CPeriodic* iPaletteMoveTimer;
	/// The duration of the appearance animation, in milliseconds
    TInt iPaletteMoveAnimationDuration; 

    /// The current animation state
    TPaletteAnimState iAnimState;
//@}

private:
/// @name Cached information, for speed
//@{
    /// The indices of the items shown on-screen
    TInt iOnScreenItemIndex[NAP2Cfg::KMaxNofItemsInView + 1];
    
    /// Whether the top scroll arrow should be shown
    TBool iShowTopScrollArrow;
    
    /// Whether the bottom scroll arrow should be shown
    TBool iShowBottomScrollArrow;
//@}

private:
    /// The callback styles requested
    TRendererCallBacks iCallbacks;
    
    /// The model observer (renderer)
	MActivePalette2ModelObserver& iModelObserver;

  //  Configuration provider	
	//CActivePalette2Configuration& iConfiguration;
	
	/// The dimensions of each item's icon
	TSize 		 iItemSize;

	// Tooltip details
	TActivePalette2TooltipConfig iTooltipConfig;
};

#endif // _ACTIVE_PALETTE_2_MODEL_H

// End of File
