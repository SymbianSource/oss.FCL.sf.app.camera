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
* Description:  AP implementation using standard Symbian controls*
*/


/**
 * @file ActivePalette2BasicUI.h
 * AP implementation using standard Symbian controls
 */


#ifndef _ACTIVE_PALETTE_2_BASIC_UI_H
#define _ACTIVE_PALETTE_2_BASIC_UI_H

#include <e32std.h>
#include <coecntrl.h>
#include <activepalette2ui.h>
#include <activepalette2genericpluginint.h>
#include <activepalette2navigationkeys.h>
#include <activepalette2factory.h>
#include <activepalette2itemvisible.h>

#include "CActivePalettePluginBase.h"
#include "ActivePalette2ModelObserver.h"
#include "ActivePalette2Const.h"

// FORWARD DECLARATIONS
class MActivePalette2Observer;

class CFbsBitmap;
class CFbsBitmapDevice;
class CFbsBitGc;
class CPeriodic;

class CAknsBasicBackgroundControlContext;

class TActivePalette2NavigationKeys;
class CActivePalette2Item;
class TActivePalette2EventData;
class CActivePalette2Model;
class CActivePalette2Styler;
class CActivePalette2Model;
class CActivePalette2Tooltip;
class CActivePalette2Configuration;

/**
 * 2D renderer for Active Palette 2.
 *
 * @warning   application SHOULD NOT use directly SetSize(),
 *            SetRect() and SetLocation() of Active Palette object.
 *            Active palette object will adjust its size automatically
 *            when showing / hiding items. Position should be set using
 *            LocateTo(). 
 */
class CActivePalette2BasicUI :    public CCoeControl, 
                                public MActivePalette2UI, 
                                public MActivePalette2ModelObserver,
                                public MActivePalette2TooltipTimerObserver,
                                public MActivePalette2FocusTimerObserver,
                                public MActivePalette2ItemScrollTimerObserver,
                                public MActivePalette2AppearanceTimerObserver
    {

public:
/// @name Object creation and deletion
//@{
    /**
     * 2-stage construction
     * @param aDrawMode The drawing mode required
     * @return The newly-created object
     */
    static CActivePalette2BasicUI* NewL(TActivePalette2DrawMode aDrawMode);
    
    /**
     * Destructor
     */
	virtual ~CActivePalette2BasicUI();
//@}
    
public: 
/// @name From CoeControl
//@{
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void SetContainerWindowL(const CCoeControl& aContainer);    
//@}

private: 
/// @name From CoeControl
//@{
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
	void Draw(const TRect& aRect) const;
	void SizeChanged(void);
	
	/**
	* Allow AP to respond to layout/orientation changes
	* @param aType event type
	*/
	void HandleResourceChange( TInt aType );
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
    virtual void FocusUpdated();
    virtual void PaletteAppearingUpdated();
	virtual MActivePalette2Observer* APObserver() const;
    virtual void ShowTooltipUpdated();
//@}

public: 
/// @name From MActivePalette2TooltipTimerObserver
//@{
    virtual void TooltipAnimated();
//@}

public: 
/// @name From MActivePalette2FocusTimerObserver
//@{
    virtual void FocusAnimated();
//@}

public: 
/// @name From MActivePalette2ItemScrollTimerObserver
//@{
    virtual void ItemsScrolled();
//@}

public: 
/// @name From MActivePalette2AppearanceTimerObserver
//@{
    virtual void PaletteAppearingAnimated();
//@}
		
private:
/// @name Object creation helpers
//@{
	/**
	 * Constructor
	 */
	CActivePalette2BasicUI();
	
	/**
	 * Leaving constructor
	 * @param aDrawMode The rendering style for the AP
	 */
	void ConstructL(TActivePalette2DrawMode aDrawMode);
//@}

/// @name New functions
//@{	
	/**
	 * Flush the draw buffer in the specified area
	 * @param aRectToFlush The area to flush
	 */
	void FlushBufNow(const TRect& aRectToFlush);
	
	/**
	 * Recalculate the size of the AP
	 */
	void ReCalcAutoSizeL();
	
	/** 
	 * Draw palette
	 * @param aForceDisplayUpdate Indicates whether to force display update
	 */
	void RedrawIdle(TBool aForceDisplayUpdate = ETrue) const;

    /**
     * Produce the palette on the screen
     * @param aRect Rectangle in which to display the palette
     */
	void ProducePaletteToScreen(const TRect& aRect) const;

    /**
     * Translates a draw buffer rect to it's on-screen position
     * @param aDrawBufRect The buffer rect
     * @return The on-screen rect
     */
    TRect DrawBufRectToScreenRect(TRect aDrawBufRect) const;
    
    /**
     * Translates a screen rect to it's in-buffer position
     * @param aScreenRect The on-screen rect
     * @return The buffer rect
     */
    TRect ScreenRectToDrawBufRect(TRect aScreenRect) const;
    
    /**
     * The palette's current screen position
     * @return The palette's current screen position
     */
    TPoint PalettePosition() const;

    /**
     * Draws a range of items, including the gaps between them.
     * @param aStartIndex The item index to begin drawing from
     * @param aFinishIndex The item index to finish with
     */
    void DrawItemRange(TInt aStartIndex, TInt aFinishIndex) const;

    /**
     * Draws a particular item
     * @param aIndex The item index to draw
     * @param aAnimOnly Whether to only draw the item's icon area, for use when animating
     */
    void DrawItem(TInt aIndex, TBool aAnimOnly = EFalse) const;
    
    /**
     * Draws the separator below the given item index.
     * @param aIndex The item index. If -1, the top scroll-arrow portion is drawn. If the same as the
     *               last shown item index, the bottom scroll-arrow portion is drawn.
     */
    void DrawSeparator(TInt aIndex) const;

    /**
     * Redraw to screen the portion of the AP most recently updated.
     */
    void FlushDirtyRect();
//@}

	


private:	// data

    /// The model. Owned.
	CActivePalette2Model* 	iModel;

    /// The observer; may be NULL. Not owned.
	MActivePalette2Observer*	iAPObserver;

    /// The position of a fully-displayed palette.
	TPoint                  iTopLeftCorner;
	/// The current size of the palette
	TSize                   iOverallSize;
	
    /// The tooltip note. Owned
	CActivePalette2Tooltip*	iTooltipNote;

    /// Current navigation keys
	TActivePalette2NavigationKeys iNavigationKeys;

    /// Focused item's animation info
	CActivePalettePluginBase::TPluginAnimationInfo iFocusPluginAniInfo;

    /// Stores the current screen size.
	TSize	                iScreenSize;

    /// The rendering style
    TActivePalette2DrawMode iDrawMode;

    /// The context to render to. If NULL (default), the screen is used. Not owned
    CBitmapContext* iRenderGc;
    
    /// The style to render with
    CActivePalette2Styler* iStyler;
  	
  	// Configuration provider
  	CActivePalette2Configuration* iConfiguration;
    
    /// Remeber which items are (partially) covered by focus
    mutable TInt iFocusedItem1;
    /// Remeber which items are (partially) covered by focus
    mutable TInt iFocusedItem2;
    
    /// Where the palette was last rendered
    mutable TRect iRenderedRect;
    };


#endif // _ACTIVE_PALETTE_2_BASIC_UI_H
