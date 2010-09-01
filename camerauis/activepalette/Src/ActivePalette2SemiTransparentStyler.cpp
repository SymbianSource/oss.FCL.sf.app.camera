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
 * @file ActivePalette2SemiTransparentStyler.cpp
 * Active Palette Semi-transparent Styler
 */

#include "ActivePalette2SemiTransparentStyler.h"
#include "ActivePalette2Cfg.h"
#include "ActivePalette2Utils.h"
#include <activepalette2graphics.mbg>

#include <AknIconUtils.h>
#include <bitstd.h>
#include <gdi.h>

//#define USE_OPAQUE_TOOLTIPS


// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::NewL()
// -----------------------------------------------------------------------------
//
CActivePalette2SemiTransparentStyler* CActivePalette2SemiTransparentStyler::NewL()
    {
	CActivePalette2SemiTransparentStyler* self = new (ELeave) CActivePalette2SemiTransparentStyler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // this
	return self;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::ConstructL()
// -----------------------------------------------------------------------------
//
void CActivePalette2SemiTransparentStyler::ConstructL()
    {
    CActivePalette2Styler::ConstructL();
	
    iAPTopSectionMask = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_top_mask);
    AknIconUtils::SetSize(iAPTopSectionMask, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) ) );

    iAPItemMask = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body_mask);
    AknIconUtils::SetSize(iAPItemMask, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) ) );

    iAPGapMask = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_gap_mask);
    AknIconUtils::SetSize(iAPGapMask, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems) ) );

    iAPBottomSectionMask = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom_mask);
    AknIconUtils::SetSize(iAPBottomSectionMask, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EBottomSectionHeight) ) );

    }

// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::~CActivePalette2SemiTransparentStyler()
// -----------------------------------------------------------------------------
//
CActivePalette2SemiTransparentStyler::~CActivePalette2SemiTransparentStyler()
    {
	delete iAPTopSectionMask;
	delete iAPItemMask;
	delete iAPGapMask;
	delete iAPBottomSectionMask;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::DrawItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2SemiTransparentStyler::DrawItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TRect area(ItemRect(aScreenPos, aOffsetFrame, aOffsetTotal));
    
    iDrawBufContext->SetClippingRect(iItemsRect);
    iDrawBufMaskContext->SetClippingRect(iItemsRect);
    
    BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPItem, iAPItemMask, EFalse, EFalse);
	BlitGraphic(area.iTl + TPoint(ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset),0), iDrawBufContext, iDrawBufMaskContext, aIcon, aMask, ETrue, ETrue);

    iDrawBufMaskContext->CancelClippingRect();
    iDrawBufContext->CancelClippingRect();

    area.Intersection(iItemsRect);
    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::DrawGap()
// -----------------------------------------------------------------------------
//
void CActivePalette2SemiTransparentStyler::DrawGap(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TRect area(GapRect(aScreenPos, aOffsetFrame, aOffsetTotal));
    
    iDrawBufContext->SetClippingRect(iItemsRect);
    iDrawBufMaskContext->SetClippingRect(iItemsRect);

    BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPGap, iAPGapMask, EFalse, EFalse);

    iDrawBufMaskContext->CancelClippingRect();
    iDrawBufContext->CancelClippingRect();

    area.Intersection(iItemsRect);
    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::AnimItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2SemiTransparentStyler::AnimItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask)
    {
    TRect area(ItemRect(aScreenPos));
    area.iTl.iX = ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset);
    area.SetWidth(ActivePalette2Utils::APDimension( ActivePalette2Utils::EItemWidth ));

    TRect clippingRect(TPoint(ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset), 0), ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::EItemSize ));

    iDrawBufContext->BitBlt(area.iTl, iAPItem, clippingRect);
    iDrawBufMaskContext->BitBlt(area.iTl, iAPItemMask, clippingRect);
    
	BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, aIcon, aMask, ETrue, ETrue);

    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::DrawTopScrollArrowSection()
// -----------------------------------------------------------------------------
//
void CActivePalette2SemiTransparentStyler::DrawTopScrollArrowSection(TBool aShowArrow)
    {
    TRect area(TopSectionRect());
    
    BlitGraphic(TPoint(0,0), iDrawBufContext, iDrawBufMaskContext, iAPTopSection, iAPTopSectionMask, EFalse, EFalse);

    if ( aShowArrow )
        {
        CentreGraphic(area, iDrawBufContext, iDrawBufMaskContext, iScrollUpIcon, iScrollUpIconMask, ETrue, ETrue);
        }

    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::DrawBottomScrollArrowSection()
// -----------------------------------------------------------------------------
//
void CActivePalette2SemiTransparentStyler::DrawBottomScrollArrowSection(TBool aShowArrow)
    {
    TRect area(BottomSectionRect());
    
    BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPBottomSection, iAPBottomSectionMask, EFalse, EFalse);

    if ( aShowArrow )
        {
        CentreGraphic(area, iDrawBufContext, iDrawBufMaskContext, iScrollDownIcon, iScrollDownIconMask, ETrue, ETrue);
        }

    AddToDirtyRect(	area );
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::TooltipNoseMaskId()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2SemiTransparentStyler::TooltipNoseMaskId()
    {
#ifdef USE_OPAQUE_TOOLTIPS
    return CActivePalette2Styler::TooltipNoseMaskId();
#else    
    return EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_end_mask;
#endif    
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2SemiTransparentStyler::TooltipBodyMaskId()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2SemiTransparentStyler::TooltipBodyMaskId()
    {
#ifdef USE_OPAQUE_TOOLTIPS
    return CActivePalette2Styler::TooltipBodyMaskId();
#else
    return EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_body_mask;
#endif    
    }





