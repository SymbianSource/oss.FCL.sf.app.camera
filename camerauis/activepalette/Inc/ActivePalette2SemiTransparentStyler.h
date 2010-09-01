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
* Description:  Active Palette Styler*
*/


/**
 * @file ActivePalette2SemiTransparentStyler.h
 * Active Palette Semi-transparent Styler
 */


#ifndef _ACTIVE_PALETTE_2_SEMI_TRANSPARENT_STYLER_H
#define _ACTIVE_PALETTE_2_SEMI_TRANSPARENT_STYLER_H

#include "ActivePalette2Styler.h"

/**
 * Renders the semi-transparent drawing style 2D AP
 */
class CActivePalette2SemiTransparentStyler : public CActivePalette2Styler
    {
public:    
    /**
     * 2-stage construction
     * @return The newly-created object
     */
    static CActivePalette2SemiTransparentStyler* NewL();

    /**
     * Destructor
     */
    ~CActivePalette2SemiTransparentStyler();
    
/// @name From CActivePalette2Styler
//@{
    virtual void ConstructL();    
    virtual void DrawItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask, TInt aOffsetFrame, TInt aOffsetTotal);
    virtual void DrawGap(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal);
    virtual void AnimItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask);
    virtual void DrawTopScrollArrowSection(TBool aShowArrow);
    virtual void DrawBottomScrollArrowSection(TBool aShowArrow);
//@}

protected:    
/// @name From CActivePalette2Styler
//@{
    virtual TInt TooltipNoseMaskId();
    virtual TInt TooltipBodyMaskId();
//@}

private:
    /// Mask for top section. Owned
	CFbsBitmap* iAPTopSectionMask;
	/// Mask for items. Owned
	CFbsBitmap* iAPItemMask;
	/// Mask for gaps. Owned
	CFbsBitmap* iAPGapMask;
	/// Mask for bottom section. Owned
	CFbsBitmap* iAPBottomSectionMask;
    };

#endif // _ACTIVE_PALETTE_2_SEMI_TRANSPARENT_STYLER_H
