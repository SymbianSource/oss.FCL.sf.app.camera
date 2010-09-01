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
 * @file ActivePalette2CheckerboardStyler.cpp
 * Active Palette Checkerboard Styler
 */

#include "ActivePalette2Logger.h"
#include "ActivePalette2CheckerboardStyler.h"
#include "ActivePalette2Cfg.h"
#include "ActivePalette2Utils.h"
#include <activepalette2graphics.mbg>

#include <bitstd.h>
#include <gdi.h>
#include <AknIconUtils.h>

/// Returns whether supplied number is odd
#define IS_ODD(num) ((num) & 1)

// Lightness threshold for checking whether a pixel
// should be considered 'white'. Lightness is defined as the sum of the
// 8-bit red, green, and blue values.
const TInt KLightnessThreshold = 0x80 * 3;

// Threshold for determining if a pixel in a soft mask is considered
// opaque or transparent.
const TUint8 KMaskThreshold = 0x7f;

// Color value for transparent pixel (ARGB format).
const TUint32 KColorKey = 0xFF000000;

// Color value for opaque pixel (ARGB format).
const TUint32 KOpaqueColor = 0x00FFFFFF;


// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::NewL()
// -----------------------------------------------------------------------------
//
CActivePalette2CheckerboardStyler* CActivePalette2CheckerboardStyler::NewL()
    {
	CActivePalette2CheckerboardStyler* self = new (ELeave) CActivePalette2CheckerboardStyler();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // this
	return self;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::ConstructL()
// -----------------------------------------------------------------------------
//
void CActivePalette2CheckerboardStyler::ConstructL()
    {
	LOGTEXT( _L( "CActivePalette2CheckerboardStyler::ConstructL entered"));
    CActivePalette2Styler::ConstructL();

	iAPTopSectionMask	    = new (ELeave) CFbsBitmap;
	iAPItemMaskA            = new (ELeave) CFbsBitmap;
	iAPItemMaskB            = new (ELeave) CFbsBitmap;
	iAPGapMaskA	            = new (ELeave) CFbsBitmap;
	iAPGapMaskB	            = new (ELeave) CFbsBitmap;
	iAPBottomSectionMaskA   = new (ELeave) CFbsBitmap;
	iAPBottomSectionMaskB   = new (ELeave) CFbsBitmap;

    // The order of calling these has an importance, to create the checkers correctly

    LoadAndScaleCheckerboardL( iAPTopSectionMask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_top, EMbmActivepalette2graphicsQgn_graf_cam4_tb_top_mask, 
            TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) ) );
    LoadAndScaleCheckerboardL( iAPItemMaskA, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body_mask, 
            TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) ) );
    LoadAndScaleCheckerboardL( iAPGapMaskA, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_gap, EMbmActivepalette2graphicsQgn_graf_cam4_tb_gap_mask, 
            TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems) ) );
    LoadAndScaleCheckerboardL( iAPItemMaskB, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body_mask,
            TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) ) );
    LoadAndScaleCheckerboardL( iAPGapMaskB, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_gap, EMbmActivepalette2graphicsQgn_graf_cam4_tb_gap_mask, 
            TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems) ) );
    LoadAndScaleCheckerboardL( iAPBottomSectionMaskB, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom_mask,
            TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EBottomSectionHeight) ) );
    LoadAndScaleCheckerboardL( iAPBottomSectionMaskA, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom_mask,
            TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EBottomSectionHeight) ) );

	LOGTEXT( _L( "CActivePalette2CheckerboardStyler::ConstructL Left"));
    }

// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::~CActivePalette2CheckerboardStyler()
// -----------------------------------------------------------------------------
//
CActivePalette2CheckerboardStyler::~CActivePalette2CheckerboardStyler()
    {
	delete iAPTopSectionMask;
	delete iAPItemMaskA;
	delete iAPItemMaskB;
	delete iAPGapMaskA;
	delete iAPGapMaskB;
	delete iAPBottomSectionMaskA;
	delete iAPBottomSectionMaskB;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::DrawItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2CheckerboardStyler::DrawItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TRect area(ItemRect(aScreenPos, aOffsetFrame, aOffsetTotal));
    
    iDrawBufContext->SetClippingRect(iItemsRect);
    iDrawBufMaskContext->SetClippingRect(iItemsRect);

    if ( IS_ODD(aScreenPos) )
        {  
        BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPItem, iAPItemMaskB, EFalse, EFalse);
        }
    else
        {
        BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPItem, iAPItemMaskA, EFalse, EFalse);
        }
        
	BlitGraphic(area.iTl + TPoint(ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset),0), iDrawBufContext, iDrawBufMaskContext, aIcon, aMask, ETrue, ETrue);

    iDrawBufMaskContext->CancelClippingRect();
    iDrawBufContext->CancelClippingRect();

    area.Intersection(iItemsRect);
    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::DrawGap()
// -----------------------------------------------------------------------------
//
void CActivePalette2CheckerboardStyler::DrawGap(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TRect area(GapRect(aScreenPos, aOffsetFrame, aOffsetTotal));
    
    iDrawBufContext->SetClippingRect(iItemsRect);
    iDrawBufMaskContext->SetClippingRect(iItemsRect);

    if ( IS_ODD(aScreenPos) )
        {  
        BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPGap, iAPGapMaskB, EFalse, EFalse);
        }
    else
        {  
        BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPGap, iAPGapMaskA, EFalse, EFalse);
        }

    iDrawBufMaskContext->CancelClippingRect();
    iDrawBufContext->CancelClippingRect();

    area.Intersection(iItemsRect);
    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::AnimItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2CheckerboardStyler::AnimItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask)
    {
    TRect area(ItemRect(aScreenPos));
    area.iTl.iX = ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset);
    area.SetWidth(ActivePalette2Utils::APDimension( ActivePalette2Utils::EItemWidth ));

    TRect clippingRect(TPoint(ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset), 0), ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::EItemSize ) );
    
    iDrawBufContext->BitBlt(area.iTl, iAPItem, clippingRect);
    
    if ( IS_ODD(aScreenPos) )
        {  
        iDrawBufMaskContext->BitBlt(area.iTl, iAPItemMaskB, clippingRect);
        }
    else
        {
        iDrawBufMaskContext->BitBlt(area.iTl, iAPItemMaskA, clippingRect);
        }
        
	BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, aIcon, aMask, ETrue, ETrue);

    AddToDirtyRect(	area );
    }

// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::DrawTopScrollArrowSection()
// -----------------------------------------------------------------------------
//
void CActivePalette2CheckerboardStyler::DrawTopScrollArrowSection(TBool aShowArrow)
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
// CActivePalette2CheckerboardStyler::DrawBottomScrollArrowSection()
// -----------------------------------------------------------------------------
//
void CActivePalette2CheckerboardStyler::DrawBottomScrollArrowSection(TBool aShowArrow)
    {
    TRect area(BottomSectionRect());
    
    if ( IS_ODD(iNumItems) )
        {  
        BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPBottomSection, iAPBottomSectionMaskA, EFalse, EFalse);
        }
    else
        {
        BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iAPBottomSection, iAPBottomSectionMaskB, EFalse, EFalse);
        }
        
    if ( aShowArrow )
        {
        CentreGraphic(area, iDrawBufContext, iDrawBufMaskContext, iScrollDownIcon, iScrollDownIconMask, ETrue, ETrue);
        }

    AddToDirtyRect(	area );
    }
    

// -----------------------------------------------------------------------------
// CActivePalette2CheckerboardStyler::LoadAndScaleCheckerboard()
// -----------------------------------------------------------------------------
//
void CActivePalette2CheckerboardStyler::LoadAndScaleCheckerboardL(
                                                CFbsBitmap* aTargetBitmap,
                                                const TDesC& aFileName,
                                                TInt32 aId,
                                                TInt32 aMaskId,
                                                TSize aSize )
    {
    LOGTEXT( _L( "CActivePalette2CheckerboardStyler::LoadAndScaleCheckerboard entered"));
    CFbsBitmap* sourceIcon = NULL;
    CFbsBitmap* sourceIconMask = NULL;
    AknIconUtils::CreateIconLC(sourceIcon, sourceIconMask, aFileName, aId, aMaskId);
    AknIconUtils::SetSize(sourceIcon, aSize);
    AknIconUtils::SetSize(sourceIconMask, aSize);
    LOGTEXT( _L( "CActivePalette2CheckerboardStyler::LoadAndScaleCheckerboard svg icons loaded"));
    User::LeaveIfError( aTargetBitmap->Create( aSize, EColor16MA ) );

    TSize bmpSize = aTargetBitmap->SizeInPixels();
    TRgb rgbColor = KRgbBlack;
    TRgb rgbAlphaColor = KRgbBlack;

    HBufC8* lineBuf = HBufC8::NewLC( CFbsBitmap::ScanLineLength( bmpSize.iWidth, EColor16MA ) );
    TPtr8 lineDes = lineBuf->Des();
    HBufC8* maskBuf = HBufC8::NewLC( CFbsBitmap::ScanLineLength( bmpSize.iWidth, EGray256 ) );
    TPtr8 maskDes = maskBuf->Des();

    for (TInt y = 0; y < bmpSize.iHeight; y++)
        {
        sourceIcon->GetScanLine(
                lineDes, TPoint(0, y), bmpSize.iWidth, EColor16MA);
        sourceIconMask->GetScanLine(
                maskDes, TPoint(0, y), bmpSize.iWidth, EGray256);

        // EColor16MA is 4 bytes (32-bits) per pixel
        TUint32* linePtr = reinterpret_cast<TUint32*>( &lineDes[0] );

        // EGray256 is 1 byte per pixel
        TUint8* maskPtr = reinterpret_cast<TUint8*>( &maskDes[0] );

        for( TInt x = 0; x < bmpSize.iWidth; x++ )
            {
            TUint32 color = linePtr[x];
            TUint8 mask = maskPtr[x];

            // Get the RGB values in order to check the lightness of the pixel
            // to determine whether to mask with the checkerboard pattern.
            TInt lightness =
                (color & 0xFF) +         // blue
                ((color >> 8) & 0xFF) +  // green
                ((color >> 16) & 0xFF);  // red

            if ( mask < KMaskThreshold )
                {
                // Transparent area.
                linePtr[x] = KColorKey;
                }
            else if ( lightness > KLightnessThreshold )
                {
                // Semitransparent (dithered) area.
                // Make every second pixel transparent.
                if ( ( x + iLineCheckerCounter ) & 1 )
                    {
                    linePtr[x] = KColorKey;
                    }
                else
                    {
                    linePtr[x] = KOpaqueColor;
                    }
                }
            else
                {
                // Fully opaque area
                linePtr[x] = KOpaqueColor;
                }
            }

        aTargetBitmap->SetScanLine(lineDes, y);
        iLineCheckerCounter++;
        }

    // end draw checkers
    LOGTEXT( _L( "CActivePalette2CheckerboardStyler::LoadAndScaleCheckerboard svg4"));

    CleanupStack::PopAndDestroy(maskBuf);
    CleanupStack::PopAndDestroy(lineBuf);
    CleanupStack::PopAndDestroy(2); // sourceIcon, sourceIconMask
    LOGTEXT( _L( "CActivePalette2CheckerboardStyler::LoadAndScaleCheckerboard left"));
    }


