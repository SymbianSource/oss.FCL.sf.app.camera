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
* Description:  Active Palette Opaque Styler*
*/


/**
 * @file ActivePalette2OpaqueStyler.h
 * Active Palette Opaque Styler
 */

#ifndef _ACTIVE_PALETTE_2_OPAQUE_STYLER_H
#define _ACTIVE_PALETTE_2_OPAQUE_STYLER_H

#include "ActivePalette2Styler.h"

/**
 * Renders the opaque drawing style 2D AP
 */
class CActivePalette2OpaqueStyler : public CActivePalette2Styler
    {
public:    
    /**
     * 2-stage construction
     * @return The newly-created object
     */
    static CActivePalette2OpaqueStyler* NewL();

/// @name From CActivePalette2Styler
//@{
    virtual void ConstructL();    
    virtual void DrawItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask, TInt aOffsetFrame, TInt aOffsetTotal);
    virtual void DrawGap(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal);
    virtual void AnimItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask);
    virtual void DrawTopScrollArrowSection(TBool aShowArrow);
    virtual void DrawBottomScrollArrowSection(TBool aShowArrow);
//@}    
    };

#endif // _ACTIVE_PALETTE_2_OPAQUE_STYLER_H
