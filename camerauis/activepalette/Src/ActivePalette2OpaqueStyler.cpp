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
 * @file ActivePalette2OpaqueStyler.cpp
 * Active Palette Opaque Styler
 */

#include "ActivePalette2OpaqueStyler.h"
#include "ActivePalette2Cfg.h"
#include "ActivePalette2Utils.h"

#include <bitstd.h>
#include <gdi.h>


// -----------------------------------------------------------------------------
// CActivePalette2OpaqueStyler::NewL()
// -----------------------------------------------------------------------------
//
CActivePalette2OpaqueStyler* CActivePalette2OpaqueStyler::NewL()
    {
	CActivePalette2OpaqueStyler* self = new (ELeave) CActivePalette2OpaqueStyler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // this
	return self;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2OpaqueStyler::ConstructL()
// -----------------------------------------------------------------------------
//
void CActivePalette2OpaqueStyler::ConstructL()
    {
    CActivePalette2Styler::ConstructL();
    }

// -----------------------------------------------------------------------------
// CActivePalette2OpaqueStyler::DrawItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2OpaqueStyler::DrawItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TRect area(ItemRect(aScreenPos, aOffsetFrame, aOffsetTotal));
    
    iDrawBufContext->SetClippingRect(iItemsRect);
    
    iDrawBufContext->SetBrushColor(TRgb(0xDDDDDD));
    iDrawBufContext->Clear(area);

	iDrawBufContext->BitBltMasked(area.iTl + TPoint(ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset),0),
								 aIcon,
								 TRect(TPoint(0,0), ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::EItemSize )),
								 aMask,
								 ETrue);

    iDrawBufContext->CancelClippingRect();
    
    area.Intersection(iItemsRect);
    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2OpaqueStyler::DrawGap()
// -----------------------------------------------------------------------------
//
void CActivePalette2OpaqueStyler::DrawGap(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TRect area(GapRect(aScreenPos, aOffsetFrame, aOffsetTotal));
    
    iDrawBufContext->SetClippingRect(iItemsRect);

    iDrawBufContext->SetBrushColor(TRgb(0xCCCCCC));
    iDrawBufContext->Clear(area);

    iDrawBufContext->CancelClippingRect();

    area.Intersection(iItemsRect);
    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2OpaqueStyler::AnimItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2OpaqueStyler::AnimItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask)
    {
    TRect area(ItemRect(aScreenPos));
    area.iTl.iX = ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset);
    area.SetWidth(ActivePalette2Utils::APDimension( ActivePalette2Utils::EItemWidth ));
    
    iDrawBufContext->SetBrushColor(TRgb(0xDDDDDD));
    iDrawBufContext->Clear(area);

	iDrawBufContext->BitBltMasked(area.iTl,
								 aIcon,
								 TRect(TPoint(0,0), area.Size()),
								 aMask,
								 ETrue);

    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2OpaqueStyler::DrawTopScrollArrowSection()
// -----------------------------------------------------------------------------
//
void CActivePalette2OpaqueStyler::DrawTopScrollArrowSection(TBool aShowArrow)
    {
    TRect area(TopSectionRect());
    
    iDrawBufContext->SetBrushColor(TRgb(0x8888FF));
    iDrawBufContext->Clear(area);

    if ( aShowArrow )
        {
        CentreGraphic(area, iDrawBufContext, iDrawBufMaskContext, iScrollUpIcon, iScrollUpIconMask, ETrue, ETrue);
        }

    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2OpaqueStyler::DrawBottomScrollArrowSection()
// -----------------------------------------------------------------------------
//
void CActivePalette2OpaqueStyler::DrawBottomScrollArrowSection(TBool aShowArrow)
    {
    TRect area(BottomSectionRect());
    
    iDrawBufContext->SetBrushColor(TRgb(0xFF8888));
    iDrawBufContext->Clear(area);

    if ( aShowArrow )
        {
        CentreGraphic(area, iDrawBufContext, iDrawBufMaskContext, iScrollDownIcon, iScrollDownIconMask, ETrue, ETrue);
        }

    AddToDirtyRect(	area );
    }
    
