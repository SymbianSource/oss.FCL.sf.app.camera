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
 * @file ActivePalette2Styler.cpp
 * Active Palette Styler
 */

#include "ActivePalette2Logger.h"
#include "ActivePalette2Styler.h"
#include "ActivePalette2Utils.h"
#include "ActivePalette2Cfg.h"
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <activepalette2graphics.mbg>

static const TInt KDefaultFontId = EAknLogicalFontSecondaryFont;

// -----------------------------------------------------------------------------
// CActivePalette2Styler::CActivePalette2Styler()
// -----------------------------------------------------------------------------
//
CActivePalette2Styler::CActivePalette2Styler()
    {
    }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::ConstructL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::ConstructL()
  {
  LOGTEXT( _L( "CActivePalette2Styler::ConstructL entered"));
  AllocateDrawBufL();
  AllocateTooltipBufL();    
  iFont = AknLayoutUtils::FontFromId(KDefaultFontId);

  AknIconUtils::CreateIconL(iScrollUpIcon, iScrollUpIconMask, NAP2Cfg::KUIGraphics, 
                            EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_up, EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_up_mask);
  AknIconUtils::SetSize(iScrollUpIcon, ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::E3dScrollIconSize ), EAspectRatioNotPreserved );
  AknIconUtils::SetSize(iScrollUpIconMask, ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::E3dScrollIconSize ), EAspectRatioNotPreserved );

  AknIconUtils::CreateIconL(iScrollDownIcon, iScrollDownIconMask, NAP2Cfg::KUIGraphics, 
                            EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_down, EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_down_mask);
  AknIconUtils::SetSize(iScrollDownIcon, ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::E3dScrollIconSize ), EAspectRatioNotPreserved );
  AknIconUtils::SetSize(iScrollDownIconMask, ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::E3dScrollIconSize ), EAspectRatioNotPreserved );

  AknIconUtils::CreateIconL(iFocusRing, iFocusRingMask, NAP2Cfg::KUIGraphics, 
                            EMbmActivepalette2graphicsQgn_graf_cam4_tb_focus, EMbmActivepalette2graphicsQgn_graf_cam4_tb_focus_mask);
  AknIconUtils::SetSize(iFocusRing, ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::E3dFocusRingSize ), EAspectRatioNotPreserved );
  AknIconUtils::SetSize(iFocusRingMask, ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::E3dFocusRingSize ), EAspectRatioNotPreserved );

  AknIconUtils::CreateIconL(iTooltipNose, iTooltipNoseMask, NAP2Cfg::KUIGraphics, 
                            EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_end, EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_end_mask);
  AknIconUtils::SetSize(iTooltipNose, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipNose), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipHeight) ), EAspectRatioNotPreserved );
  AknIconUtils::SetSize(iTooltipNoseMask, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipNose), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipHeight) ), EAspectRatioNotPreserved );

  AknIconUtils::CreateIconL(iTooltipBody, iTooltipBodyMask, NAP2Cfg::KUIGraphics, 
                            EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_body, EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_body_mask);
  AknIconUtils::SetSize(iTooltipBody, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipHeight) ), EAspectRatioNotPreserved );
  AknIconUtils::SetSize(iTooltipBodyMask, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipHeight) ), EAspectRatioNotPreserved );

  iAPTopSection = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_top);
  AknIconUtils::SetSize(iAPTopSection, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) ), EAspectRatioNotPreserved );

  iAPItem = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body);
  AknIconUtils::SetSize(iAPItem, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) ), EAspectRatioNotPreserved );

  iAPGap = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_gap);
  AknIconUtils::SetSize(iAPGap, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems) ), EAspectRatioNotPreserved );

  iAPBottomSection = AknIconUtils::CreateIconL( NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom);
  AknIconUtils::SetSize(iAPBottomSection, TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EBottomSectionHeight) ), EAspectRatioNotPreserved );

  LOGTEXT( _L( "CActivePalette2Styler::ConstructL left"));
  }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::~CActivePalette2Styler()
// -----------------------------------------------------------------------------
//
CActivePalette2Styler::~CActivePalette2Styler()
  {
  ReleaseDrawBuf(); // Utility function checks pointers to be deleted.
  ReleaseTooltipBuf(); // Utility function checks pointers to be deleted.

  delete iScrollUpIcon;
  delete iScrollUpIconMask;
  delete iScrollDownIcon;
  delete iScrollDownIconMask;
  delete iFocusRing;
  delete iFocusRingMask;
  delete iTooltipNose;
  delete iTooltipNoseMask;
  delete iTooltipBody;
  delete iTooltipBodyMask;
  delete iAPTopSection;
  delete iAPItem;
  delete iAPGap;
  delete iAPBottomSection;
  }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::SetNumberOfItemsL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::SetNumberOfItemsL(TInt aItems)
    {
    AddToDirtyRect(TRect(TPoint(0,0), Size()));
    iNumItems = aItems;
    
    if ( iNumItems > 0 )
        {
        iNumGaps = iNumItems - 1;
        }
    else
        {
        iNumGaps = 0;
        }
        
    AllocateDrawBufL();
    AddToDirtyRect(TRect(TPoint(0,0), Size()));

    iItemsRect = TRect(TPoint(0, ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight)), TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), iNumItems * ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + iNumGaps * ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) );
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::Size()
// -----------------------------------------------------------------------------
//
TSize CActivePalette2Styler::Size() const
    {
    return (iNumItems > 0)
           ? TSize( ActivePalette2Utils::APDimension(ActivePalette2Utils::EPaletteWidth), 
                    ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) 
                    + iNumItems * ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) 
                    + iNumGaps  * ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems) 
                    + ActivePalette2Utils::APDimension(ActivePalette2Utils::EBottomSectionHeight) )
           : TSize( 0, 0 );
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::DirtyRect()
// -----------------------------------------------------------------------------
//
TRect CActivePalette2Styler::DirtyRect()
    {
    if ( iDirtyRectExists )
        {
        return iDirtyRect;
        }
    else
        {
        return TRect(TPoint(0,0), TSize(0,0));
        }
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::ClearDirtyRect()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::ClearDirtyRect()
    {
    iDirtyRectExists = EFalse;
    }
    

// -----------------------------------------------------------------------------
// CActivePalette2Styler::AllocateDrawBufL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::AllocateDrawBufL(void)
    {    
  ReleaseDrawBuf();

  ActivePalette2Utils::CreateDrawingObjectsL(Size(),
                         &iDrawBufBitmap,
                         &iDrawBufDevice,
                         &iDrawBufContext);

  ActivePalette2Utils::CreateDrawingObjectsL(EGray256, Size(),
                         &iDrawBufMaskBitmap,
                         &iDrawBufMaskDevice,
                         &iDrawBufMaskContext);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::ReleaseDrawBuf()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::ReleaseDrawBuf(void)
  {
  // DeleteDrawingObjects checks passed pointers to ensure they are not null
  ActivePalette2Utils::DeleteDrawingObjects(&iDrawBufBitmap,
                        &iDrawBufDevice,
                        &iDrawBufContext);

  ActivePalette2Utils::DeleteDrawingObjects(&iDrawBufMaskBitmap,
                        &iDrawBufMaskDevice,
                        &iDrawBufMaskContext);
    }


// -----------------------------------------------------------------------------
// CActivePalette2Styler::AllocateTooltipBufL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::AllocateTooltipBufL(void)
  {
  ReleaseTooltipBuf();
    
  ActivePalette2Utils::CreateDrawingObjectsL(TooltipSize(),
                         &iTooltipBufBitmap,
                         &iTooltipBufDevice,
                         &iTooltipBufContext);

  ActivePalette2Utils::CreateDrawingObjectsL(EGray256, TooltipSize(),
                         &iTooltipBufMaskBitmap,
                         &iTooltipBufMaskDevice,
                         &iTooltipBufMaskContext);
  }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::ReleaseTooltipBuf()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::ReleaseTooltipBuf(void)
  {
  // DeleteDrawingObjects checks passed pointers to ensure they are not null  
  ActivePalette2Utils::DeleteDrawingObjects(&iTooltipBufBitmap,
                        &iTooltipBufDevice,
                        &iTooltipBufContext);

  ActivePalette2Utils::DeleteDrawingObjects(&iTooltipBufMaskBitmap,
                        &iTooltipBufMaskDevice,
                        &iTooltipBufMaskContext);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::AddToDirtyRect()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::AddToDirtyRect(TRect aNewRect)
    {
    if ( iDirtyRectExists )
        {
        iDirtyRect.BoundingRect(aNewRect);
        }
    else
        {
        iDirtyRectExists = ETrue;
        iDirtyRect = aNewRect;
        }
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::BlitPalette()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::BlitPalette(CBitmapContext* aGc, TPoint aPos, TRect aClippingRect) const
    {
  aGc->BitBltMasked(
        aPos + aClippingRect.iTl,
    iDrawBufBitmap,
        aClippingRect,
        iDrawBufMaskBitmap, EFalse);    
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::DrawTooltip()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Styler::DrawTooltip(TDesC* aText)
    {
    iTextWidth = iFont->TextWidthInPixels( *aText );
  TRAP_IGNORE(AllocateTooltipBufL());
  RenderTooltip(aText);
  return TooltipSize().iWidth;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::BlitTooltip()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::BlitTooltip(CBitmapContext* aGc, const TPoint& aPos, TRect aClippingRect)
  {
  aGc->BitBltMasked( aPos,
                     iTooltipBufBitmap,
                     aClippingRect,
                     iTooltipBufMaskBitmap, 
                     EFalse );    
  }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::BlitTooltip()
// -----------------------------------------------------------------------------
//
void 
CActivePalette2Styler::BlitTooltip( CBitmapContext* aGc, 
                                    const TPoint&   aPos, 
                                    TInt            aCurrentPhase, 
                                    TInt            aTotalPhases  )
  {
  TSize tooltipSize( TooltipSize() );

  TInt shownHeight = tooltipSize.iHeight;
  TInt shownWidth  = (tooltipSize.iWidth * aCurrentPhase) / aTotalPhases;

  TRect clippingRect( TPoint(0,0), TSize( shownWidth, shownHeight ) );
  TPoint topLeft( aPos.iX + tooltipSize.iWidth - shownWidth, aPos.iY );

  aGc->BitBltMasked( topLeft,
                     iTooltipBufBitmap,
                     clippingRect,
                     iTooltipBufMaskBitmap, 
                     EFalse );    
  }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::TooltipSize()
// -----------------------------------------------------------------------------
//
TSize CActivePalette2Styler::TooltipSize()
    {
    return TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipNose) + iTextWidth + ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipTail), ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipHeight));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::RenderTooltip()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::RenderTooltip(TDesC* aText)
    {
  const TInt height = ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipHeight);
  const TInt nose = ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipNose);

    TPoint currentPos(0, 0);
    
    BlitGraphic(currentPos, iTooltipBufContext, iTooltipBufMaskContext, iTooltipNose, iTooltipNoseMask, EFalse, EFalse);
    currentPos.iX += iTooltipNose->SizeInPixels().iWidth;
    
    TInt width = TooltipSize().iWidth;
    while ( currentPos.iX <= width )
        {
        BlitGraphic(currentPos, iTooltipBufContext, iTooltipBufMaskContext, iTooltipBody, iTooltipBodyMask, EFalse, EFalse);
        currentPos.iX += iTooltipBody->SizeInPixels().iWidth;
        }
    
    TAknLayoutText tooltipLayout;
    TRect layoutRect(TPoint(nose, 0), TSize(iTextWidth, height));
    
    TInt baseline_offset = ( ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipHeight) + iFont->FontMaxAscent() - iFont->FontMaxDescent() ) / 2;
    tooltipLayout.LayoutText(layoutRect, KDefaultFontId, NAP2Cfg::KColourTooltipText, 0, 0, /*NAP2Cfg::KTooltipBaseline - 1*/ baseline_offset/*EYZG-796AF7*/, iTextWidth, ELayoutAlignRight);
    tooltipLayout.DrawText(*iTooltipBufContext, *aText, ETrue);
    tooltipLayout.DrawText(*iTooltipBufMaskContext, *aText, ETrue, TRgb(KRgbWhite));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::TootipYOffset()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Styler::TootipYOffset(TInt aScreenPos)
    {
    return ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) + aScreenPos * (ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) + ActivePalette2Utils::APDimension(ActivePalette2Utils::ETooltipYOffset);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::TooltipNoseMaskId()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Styler::TooltipNoseMaskId()
    {
    return EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_end_mask;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::TooltipBodyMaskId()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Styler::TooltipBodyMaskId()
    {
    return EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_body_mask;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::BlitGraphic()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::BlitGraphic(TPoint aPos, CBitmapContext* aGraphicGc, CBitmapContext* aMaskGc, CFbsBitmap* aGraphic, CFbsBitmap* aGraphicMask, TBool aMaskGraphic, TBool aMaskMask)
    {
    if ( aGraphicGc )
        {
        if ( aMaskGraphic )
            {
            aGraphicGc->BitBltMasked(aPos, aGraphic, TRect(TPoint(0,0), aGraphic->SizeInPixels()), aGraphicMask, EFalse);
            }
        else
            {
            aGraphicGc->BitBlt(aPos, aGraphic);
            }
        }
    
        
    if ( aMaskGc )
        {
        if ( aMaskMask )
            {
            aMaskGc->BitBltMasked(aPos, aGraphicMask, TRect(TPoint(0,0), aGraphic->SizeInPixels()), aGraphicMask, EFalse);
            }
        else
            {
            aMaskGc->BitBlt(aPos, aGraphicMask);
            }
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Styler::CentreGraphic()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::CentreGraphic(TRect aArea, CBitmapContext* aGraphicGc, CBitmapContext* aMaskGc, CFbsBitmap* aGraphic, CFbsBitmap* aGraphicMask, TBool aMaskGraphic, TBool aMaskMask)
    {
    TPoint pos(aArea.iTl);
    pos.iX += (aArea.iBr.iX - aArea.iTl.iX)/2;
    pos.iY += (aArea.iBr.iY - aArea.iTl.iY)/2;
    pos.iX -= aGraphic->SizeInPixels().iWidth/2;
    pos.iY -= aGraphic->SizeInPixels().iHeight/2;
    
    BlitGraphic(pos, aGraphicGc, aMaskGc, aGraphic, aGraphicMask, aMaskGraphic, aMaskMask);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Styler::DrawFocusRing()
// -----------------------------------------------------------------------------
//
void CActivePalette2Styler::DrawFocusRing(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal)
    {
    TInt offset = ((ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) * aOffsetFrame) / aOffsetTotal;
    TRect area(TPoint(ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemXOffset) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EFocusRingXOffset), 
                      ActivePalette2Utils::APDimension(ActivePalette2Utils::ETopSectionHeight) + aScreenPos * (ActivePalette2Utils::APDimension(ActivePalette2Utils::EItemHeight) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EGapBetweenItems)) + ActivePalette2Utils::APDimension(ActivePalette2Utils::EFocusRingYOffset) + offset), 
               TSize(ActivePalette2Utils::APDimension(ActivePalette2Utils::EFocusRingWidth), ActivePalette2Utils::APDimension(ActivePalette2Utils::EFocusRingHeight)));

    BlitGraphic(area.iTl, iDrawBufContext, iDrawBufMaskContext, iFocusRing, iFocusRingMask, ETrue, ETrue);

    area.Intersection(TRect(TPoint(0,0), Size()));
    AddToDirtyRect( area );
    }
