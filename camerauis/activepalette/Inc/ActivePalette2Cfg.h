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
* Description:  Active Palette internal configuration and constants*
*/


/**
 * @file ActivePalette2Cfg.h
 * Active Palette internal configuration and constants
 */

/**
 * @mainpage Active Palette 2
 *
 * @section sec1 Introduction
 *
 * Active Palette 2 is a library that can be used to provide an icon-based menu UI component.
 * 
 * <IMG src="../../SemiTrans.png">
 * 
 * This is the documentation for all the code. For just the public API, see
 * @ref sec_info.
 *
 * @section sec2 Typical Usage
 *
 * Initialization:
 *  - Create the AP2 control using a ActivePalette2Factory function
 *    - If a CCoeControl is created, set the container window and Mop parent
 *    - Or, if a CHuiControl is created, add it to a control group
 *  - Register an instance of MActivePalette2Observer to receive callbacks
 *  - Install the required plugins
 *
 * Normal runtime behaviour:
 *  - Manipulate the control using the MActivePalette2UI interface
 *  - Respond to callbacks to the registered MActivePalette2Observer
 *
 * Cleanup:
 *  - Delete the control
 *
 * @section sec_groups Classes
 *
 * The following classes comprise the public API of the Active Palette 2.
 *
 * - ActivePalette2Factory
 * - MActivePalette2UI
 * - MActivePalette2Observer
 * - TActivePalette2NavigationKeys
 * - TActivePalette2ItemVisible
 * - TActivePalette2EventData
 * 
 * A generic, general-purpose plugin is provided.
 * 
 * - TAP2GenericPluginParams
 * - TAP2GenericPluginIcon
 * 
 * Plugin developers will use the following APIs:
 *
 * - CActivePalettePluginBase
 * - MActivePalettePluginObserver
 * - NActivePalettePluginFactory::TPluginCreationParams
 *
 * @section sec_info More Information
 *
 * More information is available in the docs/ directory of this release.
 *
 *
 */


#ifndef _ACTIVE_PALETTE_2_CFG_H
#define _ACTIVE_PALETTE_2_CFG_H

#include "ActivePalette2Const.h"
#include <aknenv.h>

/// For converting between milliseconds and microseconds
const TInt KMicrosecondsPerMillisecond = 1000;

/// Used in event data if Item ID is unknown
const TInt KInvalidId = -1;
/// Used internally for invalid Item indices
const TInt KInvalidItemIndex = -1;
/// Used internally for invalid screen position
const TInt KInvalidScreenPos = -2;

/// Return value from timer callbacks signalling that no future callbacks are necessary
const TInt KCallbackFinished  = 0;
/// Return value from timer callbacks signalling that future callbacks are required
const TInt KCallbackCallAgain = 1;

/// Opacity value for "fully opaque"
const TReal KOpacityFull = 1.0;
/// Opacity value for "fully transparent"
const TReal KOpacityNone = 0.0;

namespace NAP2Cfg
    {
    // General AP dimensions
    /// Item icon width, in pixels
	const TInt KItemWidth = 23;
    /// Item icon height, in pixels
	const TInt KItemHeight = 23;
    /// Width of the AP
    const TInt KPaletteWidth = 33;
    /// Height of the top section
    const TInt KTopSectionHeight = 11;
    /// Height of the bottom section
    const TInt KBottomSectionHeight = 11;
	/// The height of the gap between items
    const TInt KGapBetweenItems = 4;
    /// X offset for the items
    const TInt KItemXOffset = 5;
    /// Width of the focus ring
    const TInt KFocusRingWidth = 29;
    /// Height of the focus ring
    const TInt KFocusRingHeight = 29;
    /// X offset from the focused item for the focus ring
    const TInt KFocusRingXOffset = -3;
    /// Y offset from the focused item for the focus ring
    const TInt KFocusRingYOffset = -3;
    /// Y offset from the focused item for the tooltip
    const TInt KTooltipYOffset = 2;
    /// Height of 2D tooltips
	const TInt KTooltipHeight = 17;
    /// Width of 2D tooltips
	const TInt KTooltipWidth = 16;
    /// Factor for tooltip size
	const TReal KTooltipFactor = 1.3;
	/// 2D tooltips: baseline for font
	const TInt KTooltipBaseline = 13;
	/// 2D tooltips: width of right, blank area
	const TInt KTooltipTail = 4;
	/// 2D tooltips: with of left, blank area
	const TInt KTooltipNose = 4;

    // 2D-specific constants
    /// 2D Tooltip colour
	const TInt KColourTooltipText	      = 215; 		// black

    // 3D-specific constants
	/// 3D tooltips: YOffset for text
	const TInt   K3dTooltipTextYOffset = 2;
	/// 3D tooltips: max width of tooltip
	const TInt   K3dTooltipMaxWidth = 320;

    /// Default timer tick length
    const TInt KDefaultTimerTickLength     = 40 * KMicrosecondsPerMillisecond;

    /// Focus change tick length
    const TInt KFocusChangeTimerTickLength = KDefaultTimerTickLength;
    /// Total time for a focus change to complete, in microseconds
    const TInt KFocusChangeTotalTime       = 200 * KMicrosecondsPerMillisecond; 
    /// Number of frames needed for a complete focus change
    const TInt KFocusChangeFrames          = KFocusChangeTotalTime / KFocusChangeTimerTickLength;

    /// Item scroll tick length
    const TInt KItemScrollTimerTickLength  = KFocusChangeTimerTickLength;
    /// Total time for an item scroll to complete, in microseconds
    const TInt KItemScrollTotalTime        = KFocusChangeTotalTime;
    /// Number of frames needed for a complete item scroll
    const TInt KItemScrollFrames           = KItemScrollTotalTime / KItemScrollTimerTickLength;

    /// Palette appearance tick length
    const TInt KPaletteAppearTimerTickLength  = 100 * KMicrosecondsPerMillisecond;
    /// Total time for a palette appearance to complete, in microseconds
    const TInt KPaletteAppearTotalTime        = 400 * KMicrosecondsPerMillisecond;
    /// Number of frames needed for a complete palette appearance
    const TInt KPaletteAppearFrames           = KPaletteAppearTotalTime / KPaletteAppearTimerTickLength;

    /// Tooltip change tick length
    const TInt KTooltipTimerTickLength       = KDefaultTimerTickLength;
    const TInt KTooltipTimerTickLengthFading =  40 * KMicrosecondsPerMillisecond;
    /// Delay between an item being focused, and the tooltip being shown (in microseconds)
    const TInt KTooltipPreshowDelay          = 800 * KMicrosecondsPerMillisecond;
    const TInt KTooltipPreshowDelayFading    =  50 * KMicrosecondsPerMillisecond;
    /// Delay between the tooltip being fully shown, and beginning to disappear (in microseconds)
    const TInt KTooltipFullshowPeriod        = 3000 * KMicrosecondsPerMillisecond;
    const TInt KTooltipFullshowPeriodFading  =  800 * KMicrosecondsPerMillisecond;
    /// The speed the scrolling tooltips should move, in pixels per timer tick
    const TInt KTooltipShowSpeed             =  40; 
    /// Fading tooltip, shade steps in full in or out animation.
    const TInt KTooltipFadingSteps           = 32; // 1..255

    /// The number of frames in an item animation
    const TInt KItemAnimNoFrames = 8;
    /// The frame duration for the item animation
    const TInt KItemAnimFrameDuration = (800 * KMicrosecondsPerMillisecond) / KItemAnimNoFrames;

    /// Time for arrow indicators to fade in/out: 7/10 of the change animation. In milliseconds    
	const TInt   K3dScrollIndicatorFadeTime = (7 * KFocusChangeTotalTime) / (10 * KMicrosecondsPerMillisecond);

    /// Max number of items that can be shown
	const TInt KMaxNofItemsInView = 6;

    /// The virtual screen position above the topmost item
	const TInt KScreenPosAboveTop = -1;

    /// SVG background icon file
	_LIT(KUIGraphics, "z:\\resource\\apps\\activepalette2graphics.mif");
    }

/// The item's icon size
#define KItemSize                       TSize(NAP2Cfg::KItemWidth, NAP2Cfg::KItemHeight)
/// The colour of the 2D focus ring
#define KColourFocus                    AKN_LAF_COLOR_STATIC(198)
/// Colour of the whole-palette background
#define KColourArea  		            AKN_LAF_COLOR_STATIC(0)
/// Colour of the palette's border
#define KColourAreaBorder 	            AKN_LAF_COLOR_STATIC(0)
/// Default 2D offset
#define KDefault2dPaletteOffset         TPoint(-50, 6)
/// Default 3D offset
#define KDefault3dPaletteOffset         TPoint(-50, 20)
/// Default texture size for 3D palette/tooltip background
#define K3dPaletteTextureSize           TSize(32, 32)
/// Background colour for 3D AP
#define K3dPaletteBackgroundColour      TRgb(0xFFFFFF)
/// 3D scroll arrow bitmap size
#define K3dScrollIconSize               TSize(7, 4)
/// 3D scroll arrow offset
#define K3dScrollIconOffset             TPoint(13, 3)
/// 3D focus ring bitmap size
#define K3dFocusRingSize                TSize(29, 29)
/// 3D focus ring offset
#define K3dFocusRingOffset              TPoint(-2, -2)
/// 3D tooltip offset 
#define K3dTooltipOffset                TPoint(-NAP2Cfg::KItemXOffset, 3)
/// Colour of 2D tooltip background
#define KColourTooltipBackground        AKN_LAF_COLOR_STATIC(0)
/// Colour of 2D tooltip border
#define KColourTooltipBorder            AKN_LAF_COLOR_STATIC(215)
/// Colour of 2D tooltip mask transparency
#define KColourTooltipTransparency      TRgb::Gray256(215)


#endif // _ACTIVE_PALETTE_2_CFG_H

// End of File
