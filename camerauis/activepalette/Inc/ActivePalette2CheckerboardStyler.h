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
* Description:  Active Palette Checkerboard Styler*
*/


/**
 * @file ActivePalette2CheckerboardStyler.h
 * Active Palette Checkerboard Styler
 */

#ifndef _ACTIVE_PALETTE_2_CHECKERBOARD_STYLER_H
#define _ACTIVE_PALETTE_2_CHECKERBOARD_STYLER_H

#include "ActivePalette2Styler.h"

/**
 * Renders the checkerboard drawing style 2D AP
 */
class CActivePalette2CheckerboardStyler : public CActivePalette2Styler
    {
public:    
    /**
     * 2-stage construction
     * @return The newly-created object
     */
    static CActivePalette2CheckerboardStyler* NewL();
    
    /**
     * Destructor
     */
    ~CActivePalette2CheckerboardStyler();

/// @name From CActivePalette2Styler
//@{
    void ConstructL();    
    virtual void DrawItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask, TInt aOffsetFrame, TInt aOffsetTotal);
    virtual void DrawGap(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal);
    virtual void AnimItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask);
    virtual void DrawTopScrollArrowSection(TBool aShowArrow);
    virtual void DrawBottomScrollArrowSection(TBool aShowArrow);
//@}
    void LoadAndScaleCheckerboardL( CFbsBitmap* aTargetBitmap, const TDesC& aFileName, TInt32 aId, TInt32 aMaskId, TSize aSize );
    
private:
    /// The mask for the top section
	CFbsBitmap* iAPTopSectionMask;

/**
 * @name Alternate masks
 * A note about different masks: as the checkerboard pattern repeats over an even number of pixels,
 * and the difference in y-positions of two consecutive items is odd, two masks are required for some elements.
 * For example, to keep the pattern consistent, you should use ItemMaskA, GapMaskA, ItemMaskB, GapMaskB, BottomSectionMaskB
 */
//@{
    /// Item mask for even items. Owned
	CFbsBitmap* iAPItemMaskA;
    /// Item mask for odd items. Owned
	CFbsBitmap* iAPItemMaskB;
    /// Gap mask for even items. Owned
	CFbsBitmap* iAPGapMaskA;
    /// Gap mask for odd items. Owned
	CFbsBitmap* iAPGapMaskB;
	/// Bottom section mask for when the total number of items is odd. Owned
	CFbsBitmap* iAPBottomSectionMaskA;
	/// Bottom section mask for when the total number of items is even. Owned
	CFbsBitmap* iAPBottomSectionMaskB;
//@}	
    TBool iLineCheckerCounter;
    };

#endif // _ACTIVE_PALETTE_2_CHECKERBOARD_STYLER_H
