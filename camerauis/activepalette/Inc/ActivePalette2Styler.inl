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
* Description:  Active Palette Styler inline methods*
*/


/**
 * @file ActivePalette2Styler.inl
 * Active Palette Styler inline methods
 */

#include "ActivePalette2Cfg.h"
#include "ActivePalette2Utils.h"
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::TopSectionRect()
// -----------------------------------------------------------------------------
//
inline TRect CActivePalette2Styler::TopSectionRect()
    {
    return TRect(TPoint(0,0), TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight)));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::BottomSectionRect()
// -----------------------------------------------------------------------------
//
inline TRect CActivePalette2Styler::BottomSectionRect()
    {
    return TRect(TPoint(0, ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) + iNumItems * ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + iNumGaps * ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)), 
                 TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EBottomSectionHeight)));    
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::ItemRect()
// -----------------------------------------------------------------------------
//
inline TRect CActivePalette2Styler::ItemRect(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TInt offset = ((ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) * aOffsetFrame) / aOffsetTotal;
    return TRect(TPoint(0, ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) + aScreenPos * (ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) - offset), 
                 TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight)));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::GapRect()
// -----------------------------------------------------------------------------
//
inline TRect CActivePalette2Styler::GapRect(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TInt offset = ((ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) * aOffsetFrame) / aOffsetTotal;
    return TRect(TPoint(0, ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) + (aScreenPos+1) * (ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) - ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems) - offset), 
                 TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)));
    }
    
