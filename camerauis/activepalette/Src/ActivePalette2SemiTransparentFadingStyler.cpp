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
* Description:  
*
*/




#include <gdi.h>
#include <fbs.h>

#include <activepalette2graphics.mbg>

#include "ActivePalette2Logger.h"
#include "ActivePalette2Cfg.h"
#include "ActivePalette2Utils.h"
#include "ActivePalette2SemiTransparentFadingStyler.h"


static const TInt KAlphaMask = 0xFF;

// -----------------------------------------------------------------------------
// 2-phase constructor <<static>>
// -----------------------------------------------------------------------------
//
CActivePalette2SemiTransparentFadingStyler* 
CActivePalette2SemiTransparentFadingStyler::NewL()
  {
  CActivePalette2SemiTransparentFadingStyler* self = 
      new (ELeave) CActivePalette2SemiTransparentFadingStyler();

  CleanupStack::PushL(self);
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;
  }

// -----------------------------------------------------------------------------
// Destructor <<virtual>>
// -----------------------------------------------------------------------------
//
CActivePalette2SemiTransparentFadingStyler
::~CActivePalette2SemiTransparentFadingStyler()
  {
  ReleaseTooltipBuf();
  }


// -----------------------------------------------------------------------------
// BlitTooltip <<virtual>>
// -----------------------------------------------------------------------------
//
void 
CActivePalette2SemiTransparentFadingStyler
::BlitTooltip( CBitmapContext* aGc, 
               const TPoint&   aPos, 
               TInt            aCurrentPhase, 
               TInt            aTotalPhases )
  {
  TInt alpha_orig  = ( KAlphaMask * aCurrentPhase ) / aTotalPhases;
  TInt alpha_value = alpha_orig & KAlphaMask; // Limit to 0..255
  TRgb alpha       = TRgb::Gray256( alpha_value );

  LOGTEXT1( _L("CActivePalette2SemiTransparentFadingStyler::BlitTooltip, alpha=%d"), alpha_value );

  iTooltipAlphaContext->SetBrushColor( alpha );
  iTooltipAlphaContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
  iTooltipAlphaContext->Clear();

  iTooltipBlendedMaskContext->SetBrushColor( KRgbBlack );
  iTooltipBlendedMaskContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
  iTooltipBlendedMaskContext->Clear();
  iTooltipBlendedMaskContext->BitBltMasked( TPoint( 0,0 ), 
                                            iTooltipAlphaBitmap, 
                                            TooltipSize(),
                                            iTooltipBufMaskBitmap, 
                                            EFalse );

  aGc->BitBltMasked( aPos, 
                     iTooltipBufBitmap, 
                     TooltipSize(),
                     iTooltipBlendedMaskBitmap, 
                     EFalse );
  }



    
// -----------------------------------------------------------------------------
// TooltipNoseMaskId <<virtual>>
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2SemiTransparentFadingStyler::TooltipNoseMaskId()
  {
  return EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_end_mask;
  }
    
// -----------------------------------------------------------------------------
// TooltipBodyMaskId <<virtual>>
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2SemiTransparentFadingStyler::TooltipBodyMaskId()
  {
  return EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_body_mask;
  }


// -----------------------------------------------------------------------------
// AllocateTooltipBufL <<virtual>>
// -----------------------------------------------------------------------------
//
void 
CActivePalette2SemiTransparentFadingStyler::AllocateTooltipBufL()
  {
  ReleaseTooltipBuf();

  ActivePalette2Utils::CreateDrawingObjectsL( TooltipSize(),
                                              &iTooltipBufBitmap,
                                              &iTooltipBufDevice,
                                              &iTooltipBufContext );

  ActivePalette2Utils::CreateDrawingObjectsL( EGray2,
                                              TooltipSize(),
                                              &iTooltipBufMaskBitmap,
                                              &iTooltipBufMaskDevice,
                                              &iTooltipBufMaskContext );

  ActivePalette2Utils::CreateDrawingObjectsL( EGray256, 
                                              TooltipSize(),
                                              &iTooltipAlphaBitmap,
                                              &iTooltipAlphaDevice,
                                              &iTooltipAlphaContext );

  ActivePalette2Utils::CreateDrawingObjectsL( EGray256, 
                                              TooltipSize(),
                                              &iTooltipBlendedMaskBitmap,
                                              &iTooltipBlendedMaskDevice,
                                              &iTooltipBlendedMaskContext );
  }

// -----------------------------------------------------------------------------
// ReleaseTooltipBuf <<virtual>>
// -----------------------------------------------------------------------------
//
void 
CActivePalette2SemiTransparentFadingStyler::ReleaseTooltipBuf()
  {
  ActivePalette2Utils::DeleteDrawingObjects( &iTooltipBufBitmap,
                                             &iTooltipBufDevice,
                                             &iTooltipBufContext );

  ActivePalette2Utils::DeleteDrawingObjects( &iTooltipBufMaskBitmap,
                                             &iTooltipBufMaskDevice,
                                             &iTooltipBufMaskContext );

  ActivePalette2Utils::DeleteDrawingObjects( &iTooltipAlphaBitmap,
                                             &iTooltipAlphaDevice,
                                             &iTooltipAlphaContext );

  ActivePalette2Utils::DeleteDrawingObjects( &iTooltipBlendedMaskBitmap,
                                             &iTooltipBlendedMaskDevice,
                                             &iTooltipBlendedMaskContext );
  }

