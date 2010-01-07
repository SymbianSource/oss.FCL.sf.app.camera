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
* Description:  Active Palette 2 Fading Tooltip implementation
*
*/



// ===========================================================================
// Includes
#include <activepalette2observer.h>

#include "ActivePalette2BasicUI.h"
#include "ActivePalette2Model.h"
#include "ActivePalette2Styler.h"
#include "activepalette2tooltipfading.h"

// ===========================================================================
// Constants


// ===========================================================================
// Class implementation

// static 
CActivePalette2TooltipFading* 
CActivePalette2TooltipFading::NewL( const CActivePalette2BasicUI*  aParent,
                                          CActivePalette2Model*  aModel,
                                          CActivePalette2Styler* aStyler )
  {
  CActivePalette2TooltipFading* self = 
      new (ELeave) CActivePalette2TooltipFading( aModel, aStyler );
        
  CleanupStack::PushL( self );
  self->ConstructL( aParent );
  CleanupStack::Pop( self );

  return self;
  }


// virtual 
CActivePalette2TooltipFading::~CActivePalette2TooltipFading()
  {
  // No new resources to free.
  }

 
// virtual 
void 
CActivePalette2TooltipFading
::ProduceTooltipToScreen( TBool aActivateGc ) const
  {
  TRect redrawRect;
  TRect tooltipRect( TooltipRect() );

  // -------------------------------------------------------
  // Figure out redraw area needed  
  if ( iModel->ShowTooltip() )
    {
    redrawRect = tooltipRect;

    if ( iRenderedRect.Width() > 0 )
      {
      redrawRect.BoundingRect( iRenderedRect );
      }
    }
  else
    {
    redrawRect = iRenderedRect;
    }
        
  // -------------------------------------------------------
  // Activate system gc if needed
  if ( !iRenderGc && aActivateGc )
    {
    Window().Invalidate(redrawRect);
    ActivateGc();
    Window().BeginRedraw(redrawRect);
    }
    
  // -------------------------------------------------------
  // Draw in the background
  if ( iParent && iParent->APObserver() )
    {
    if ( redrawRect.Size().iHeight > 0 && redrawRect.Size().iWidth > 0 )
      {
      iParent->APObserver()->Redraw(redrawRect);
      }
    }

  // -------------------------------------------------------
  // Blit tooltip to buffer and use current opaqueness
  // value to blend the tooltip to gc.
  if ( iModel->ShowTooltip() )
    {
    CBitmapContext* context = ( iRenderGc ? iRenderGc : &SystemGc() );
    iStyler->BlitTooltip( context, 
                          tooltipRect.iTl, 
                          iModel->TooltipCurrentFrame(), 
                          iModel->TooltipTotalFrames() );
    }
              
  // -------------------------------------------------------
  // Deactivate gc if needed
  if ( !iRenderGc && aActivateGc )
    {
    Window().EndRedraw();
    DeactivateGc(); 
    }

  // -------------------------------------------------------
  // Determine redraw area for next round.
  if ( iModel->ShowTooltip() )
    {
    iRenderedRect = tooltipRect;
    }
  else
    {
    iRenderedRect = TRect(0,0,0,0);
    }
  // -------------------------------------------------------
  }



// virtual 
TRect 
CActivePalette2TooltipFading::TooltipRect() const
  {
  TInt  tlX  = iAnchor.iX - iStyler->TooltipSize().iWidth;
  TInt  tlY  = iAnchor.iY + iStyler->TootipYOffset( iModel->TooltipScreenPosition() );
  TInt  brX  = iAnchor.iX;
  TInt  brY  = tlY + iStyler->TooltipSize().iHeight;

  return TRect( tlX, tlY, brX, brY );
  }



// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::SetAnchor()
// -----------------------------------------------------------------------------
//
void 
CActivePalette2TooltipFading
::ConstructL( const CActivePalette2BasicUI* aParent )
  {
  CActivePalette2Tooltip::ConstructL( aParent );
  }

CActivePalette2TooltipFading
::CActivePalette2TooltipFading( CActivePalette2Model*  aModel,
                                CActivePalette2Styler* aStyler )
  : CActivePalette2Tooltip( aModel, aStyler )
  {
  }
