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
* Description:  AP implementation using HUITK*
*/


/**
 * @file ActivePalette2HUI.h
 * AP implementation using HUITK
 */
 

#ifndef _ACTIVE_PALETTE_2_HUI_H
#define _ACTIVE_PALETTE_2_HUI_H

#include <e32std.h>
#include <uiacceltk/HuiControl.h>
#include <activepalette2ui.h>
#include <activepalette2itemvisible.h>
#include <activepalette2navigationkeys.h>

#include "ActivePalette2ModelObserver.h"

class CHuiEnv;
class CHuiLayout;
class CHuiImageVisual;
class CHuiMeshVisual;
class CHuiTextVisual;
class CHuiTexture;
class TActivePalette2EventData;
class MActivePalette2Observer;
class CActivePalette2Item;
class CActivePalette2Model;

/**
 * 3D renderer for Active Palette 2.
 */
class CActivePalette2HUI : public CHuiControl, 
                              public MActivePalette2UI,
                              public MActivePalette2ModelObserver,
                              public MActivePalette2TooltipDescriptiveObserver,
                              public MActivePalette2FocusDescriptiveObserver,
                              public MActivePalette2ItemScrollDescriptiveObserver,
                              public MActivePalette2AppearanceDescriptiveObserver
    {
public:  
/// @name Object creation & deletion
//@{

    /**
     * 2-stage construction
     * @param aEnv The Hui environment the control should be created in
     * @return The newly-created object
     */
    static CActivePalette2HUI* NewL(CHuiEnv& aEnv);
    
    /**
     * Destructor
     */
    ~CActivePalette2HUI();
//@}

private:
/// @name Object creation helpers
//@{
    /**
     * Constructor
     * @param aEnv The Hui environment the control should be created in
     */
    CActivePalette2HUI(CHuiEnv& aEnv);

    /**
     * Second-stage construction
     */
    void ConstructL();

//@}

public:  
/// @name From CHuiControl
//@{
    virtual TBool OfferEventL(const THuiEvent& aEvent);
    virtual void VisualLayoutUpdated(CHuiVisual &aVisual);
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
	virtual void LocateTo(const TPoint& aTopLeft);
	virtual TPoint Location() const;
    virtual CCoeControl* CoeControl();
    virtual CHuiControl* HuiControl();
    virtual void SetGc(CBitmapContext* aGc = NULL);
    virtual void RenderActivePalette(const TRect& aRect) const;
    virtual void SetObserver(MActivePalette2Observer* aObserver);
    virtual TInt GetAvailablePlugins(RArray<TUid>& aPluginList) const;
    virtual void SetNavigationKeys(const TActivePalette2NavigationKeys& aNavigationKeys);
//@}    

public: 
/// @name From MActivePalette2ModelObserver
//@{
    virtual TInt TooltipUpdated();
    virtual void ItemsUpdated();
    virtual void ItemAnimated(TInt aScreenPos, CActivePalette2Item* aItem, TInt aAnimFrame);
    virtual void ItemAnimationComplete(TInt aScreenPos, CActivePalette2Item* aItem);
    virtual void PaletteAppearingUpdated();
	virtual MActivePalette2Observer* APObserver() const;
    virtual void ShowTooltipUpdated();
//@}    

public: 
/// @name From MActivePalette2TooltipDescriptiveObserver
//@{
    virtual void AnimateTooltip(TBool aToShow, TInt aTimeInMilliseconds);
//@}

public: 
/// @name From MActivePalette2FocusDescriptiveObserver
//@{
    virtual void AnimateFocus(TInt aCurrentPos, TInt aTargetPos, TInt aTimeInMilliseconds);
//@}

public: 
/// @name From MActivePalette2ItemScrollDescriptiveObserver
//@{
    virtual void AnimateItemScroll(TInt aDirection, TInt aTimeInMilliseconds);
//@}
    
public: 
/// @name From MActivePalette2AppearanceDescriptiveObserver
//@{
    virtual void AnimateAppearance(TBool aVisible, TInt aTimeInMilliseconds);
    virtual TInt GuessCurrentAppearanceFrame();
//@}

private: 
/// @name New functions
//@{
    /**
     * Renders the main section of the palette. It repositions all objects within it's layout,
     * and updates all textures.
     */
    void Render();

    /**
     * Returns the current screen position of the AP.
     * @return The screen position of the AP
     */                      
    TPoint PalettePosition() const;

    /**
     * Updates state when a focus-change animation is completed
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt AnimateFocusTimerComplete();

    /**
     * Updates state when a palette-appearance animation is completed
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt AnimateAppearanceTimerComplete();

    /**
     * Updates state when a item-scroll animation is completed
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt AnimateItemScrollTimerComplete();

    /**
     * Updates state when a tooltip animation is completed
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    TInt AnimateTooltipTimerComplete();

    /**
     * Returns the palette offset for an item at the given screen position
     * @param aScreenPos The screen postion
     * @return The item offset
     */
    TPoint ItemPosition(TInt aScreenPos) const;
    
    /**
     * The offset for the area of the palette that contains the top scroll arrow
     * @return The offset
     */
    TPoint TopScrollPanePosition() const;
    
    /**
     * The offset for the area of the palette that contains the bottom scroll arrow
     * @return The offset
     */
    TPoint BottomScrollPanePosition() const;
    
    /**
     * The overall size of the palette
     * @return The size
     */
    TSize PaletteSize() const;
    
    /**
     * Returns the current offset to be applied to all items due to item-scrolling
     * @return The item scroll offset
     */
    TPoint CurrentScrollOffset() const;

    /**
     * Set the correct opacities for the top & bottom items during an item-scroll animation
     */
    void ItemsScrolledSetItemOpacities();
    
    /**
     * Updates the position and opacity of the focus ring, with immediate effect
     */
    void UpdateFocus();

    /**
     * Updates iScreenSize, iTopLeftCorner based on current screen size 
     */
    void UpdateTopLeftCornerWithScreenSize();
//@}

/// @name New functions
//@{
    /**
     * Timer callback for focus-change animation
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    static TInt AnimateFocusTimerCallback(TAny* aPtr);

    /**
     * Timer callback for palette-appearance animation
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    static TInt AnimateAppearanceTimerCallback(TAny* aPtr);

    /**
     * Timer callback for item-scroll animation
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    static TInt AnimateItemScrollTimerCallback(TAny* aPtr);
    
    /**
     * Timer callback for tooltip animation
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    static TInt AnimateTooltipTimerCallback(TAny* aPtr);

//@}

private: // data
    /// The model used
    CActivePalette2Model* iModel;
    
    /// The observer to use for callbacks
    MActivePalette2Observer* iObserver;

    /// The keys to respond to in OfferEventL
    TActivePalette2NavigationKeys iNavigationKeys;

    /// Where the AP should be when fully shown
    TPoint iTopLeftCorner;    
    

    /// The full-screen layout - to get VisualLayoutUpdated on 
    /// screen orientation change
    CHuiLayout*      iFullScreenLayout;
    
    /// The palette layout
    CHuiLayout*      iPaletteLayout;

    /// The top section background layout
    CHuiLayout*      iBackgroundTopSectionLayout;
    /// The top section background visual
    CHuiImageVisual* iBackgroundTopSectionVisual;
    /// The top section background texture
    CHuiTexture*     iBackgroundTopSectionTexture;

    /// The mid section background layout
    CHuiLayout*      iBackgroundMidSectionLayout;
    /// The mid section background visual
    CHuiImageVisual* iBackgroundMidSectionVisual;
    /// The mid section background texture
    CHuiTexture*     iBackgroundMidSectionTexture;

    /// The bottom section background layout
    CHuiLayout*      iBackgroundBottomSectionLayout;
    /// The bottom section background visual
    CHuiImageVisual* iBackgroundBottomSectionVisual;
    /// The bottom section background texture
    CHuiTexture*     iBackgroundBottomSectionTexture;

    /// The top-arrow layout
    CHuiLayout*      iTopScrollLayout;
    /// The top-arrow visual
    CHuiImageVisual* iTopScrollVisual;
    /// The top-arrow texture
    CHuiTexture*     iTopScrollTexture;

    /// The bottom-arrow layout    
    CHuiLayout*      iBottomScrollLayout;
    /// The bottom-arrow visual
    CHuiImageVisual* iBottomScrollVisual;
    /// The bottom-arrow texture
    CHuiTexture*     iBottomScrollTexture;

    /// The focus ring layout
    CHuiLayout*      iFocusRingLayout;
    /// The focus ring texture
    CHuiTexture*     iFocusRingTexture;
    /// The focus ring visual
    CHuiImageVisual* iFocusRingVisual;

    /// The layouts for the items
    RPointerArray<CHuiLayout>      iItemLayout;
    /// The visuals for the items
    RPointerArray<CHuiImageVisual> iItemVisual;
    /// The textures for the items
    RPointerArray<CHuiTexture>     iItemTexture;

    /// The tooltip layout
    CHuiLayout*      iTooltipLayout;
    
    /// The tooltip text layout
    CHuiLayout*      iTooltipTextLayout;
    /// The tooltip text visual
    CHuiTextVisual*  iTooltipTextVisual;

    /// The nose section tooltip background layout
    CHuiLayout*      iTooltipBackgroundNoseLayout;
    /// The nose section tooltip background visual
    CHuiImageVisual* iTooltipBackgroundNoseVisual;
    /// The nose section tooltip background texture
    CHuiTexture*     iTooltipBackgroundNoseTexture;

    /// The body section tooltip background layout
    CHuiLayout*      iTooltipBackgroundBodyLayout;
    /// The body section tooltip background visual
    CHuiImageVisual* iTooltipBackgroundBodyVisual;
    /// The body section tooltip background texture
    CHuiTexture*     iTooltipBackgroundBodyTexture;


    /// A general purpose timer. Used for synchronizing with animations.
    CPeriodic* iGeneralTimer;
    
    /// Stores the current screen size.
	TSize iScreenSize;
    };


#endif // _ACTIVE_PALETTE_2_HUI_H

