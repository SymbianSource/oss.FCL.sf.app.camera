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
* Description:  Active Palette model observers*
*/


/**
 * @file ActivePalette2ModelObserver.h
 * Active Palette model observers
 */

#ifndef _ACTIVE_PALETTE_2_MODEL_OBSERVER_H
#define _ACTIVE_PALETTE_2_MODEL_OBSERVER_H

class CActivePalette2Item;
class MActivePalette2Observer;


/**
 * Model observer interface. Used by the model to update the renderer.
 */
class MActivePalette2ModelObserver
    {
public: 
    /**
     * Called when a new tooltip is about to be shown for the first time. This allows the renderer
     * to pre-render the tooltip if necessary
     * @return The width, in pixels, of the entire tooltip
     */
    virtual TInt TooltipUpdated() = 0;

    /**
     * Called when the items have been updated.
     */
    virtual void ItemsUpdated() = 0;

    /**
     * Called when the palette-appearance animation needs to be updated
     */
    virtual void PaletteAppearingUpdated() = 0;

    /**
     * Called when an item animation needs to be updated
     * @param aScreenPos The screen position of the item
     * @param aItem pointer to the item object
     * @param aAnimFrame The frame to be displayed
     */
    virtual void ItemAnimated(TInt aScreenPos, CActivePalette2Item* aItem, TInt aAnimFrame) = 0;

    /**
     * Called when an item's animation is complete
     * @param aScreenPos The screen position of the item
     * @param aItem pointer to the item object
     */
    virtual void ItemAnimationComplete(TInt aScreenPos, CActivePalette2Item* aItem) = 0;

    /**
     * Called when a tooltip should appear or disappear
     */
    virtual void ShowTooltipUpdated() = 0;

    /**
     * Should return the currently registered observer, or NULL if none.
     * @return The AP observer
     */
	virtual MActivePalette2Observer* APObserver() const = 0;
    };
    
    
/**
 * Timer-style callbacks for tooltip animation
 */
class MActivePalette2TooltipTimerObserver
    {
public: 
    /**
     * Called when the tooltip animation needs to be updated
     */
    virtual void TooltipAnimated() = 0;
    };
    

/**
 * Timer-style callbacks for focus-change animation
 */
class MActivePalette2FocusTimerObserver
    {
public: 
    /**
     * Called when a new item has been focused
     */    
    virtual void FocusUpdated() = 0;
    
    /**
     * Called when the focus-change animation needs to be updated
     */
    virtual void FocusAnimated() = 0;
    };
    
    
/**
 * Timer-style callbacks for item-scroll animation
 */
class MActivePalette2ItemScrollTimerObserver
    {
public: 
    /**
     * Called when an item-scroll animation needs to be updated
     */
    virtual void ItemsScrolled() = 0;
    };
    
    
/**
 * Timer-style callbacks for appearance animation
 */
class MActivePalette2AppearanceTimerObserver
    {
public: 
    /**
     * Called when the palette-appearance animation needs to be updated
     */
    virtual void PaletteAppearingAnimated() = 0;
    };
    

/**
 * Descriptive-style callbacks for tooltip animation
 */
class MActivePalette2TooltipDescriptiveObserver
    {
public: 
    /**
     * Called when a tooltip animation is required
     * @param aToShow If ETrue, the tooltip should be fully shown at the end of the animation; if EFalse, it should
     *                be fully hidden
     * @param aTimeInMilliseconds The length of time the animation should take, in milliseconds
     * @see CActivePalette2Model::AnimateTooltipRendererComplete
     */
    virtual void AnimateTooltip(TBool aToShow, TInt aTimeInMilliseconds) = 0;
    };
    

/**
 * Descriptive-style callbacks for focus-change animation
 */
class MActivePalette2FocusDescriptiveObserver
    {
public: 
    /**
     * Called when a focus-change animation is required
     * @param aCurrentPos The screen position to start from
     * @param aTargetPos The destination screen position
     * @param aTimeInMilliseconds The duration of the animation, in milliseconds
     * @see CActivePalette2Model::AnimateFocusRendererComplete()
     */
    virtual void AnimateFocus(TInt aCurrentPos, TInt aTargetPos, TInt aTimeInMilliseconds) = 0;
    };

    
/**
 * Descriptive-style callbacks for item-scroll animation
 */
class MActivePalette2ItemScrollDescriptiveObserver
    {
public: 
    /**
     * Called when an item-scroll animation is required
     * @param aDirection +1 to move the items downwards, -1 to move them upwards
     * @param aTimeInMilliseconds The length of time the animation should take, in milliseconds
     * @see CActivePalette2Model::AnimateItemScrollRendererComplete
     */
    virtual void AnimateItemScroll(TInt aDirection, TInt aTimeInMilliseconds) = 0;
    };
    

/**
 * Descriptive-style callbacks for appearance animation
 */
class MActivePalette2AppearanceDescriptiveObserver
    {
public: 
    /**
     * Called when a palette-appearance animation is required
     * @param aVisible The target state
     * @param aTimeInMilliseconds The duration of the animation, in milliseconds
     * @see CActivePalette2Model::AnimateAppearanceRendererComplete()
     */
    virtual void AnimateAppearance(TBool aVisible, TInt aTimeInMilliseconds) = 0;

    /**
     * Asks the implementing renderer to estimate where the appearance animation is up to
     * @return The frame number, in relation to CActivePalette2Model::PaletteAppearingTotalFrames()
     */
    virtual TInt GuessCurrentAppearanceFrame() = 0;
    };

#endif //_ACTIVE_PALETTE_2_MODEL_OBSERVER_H
