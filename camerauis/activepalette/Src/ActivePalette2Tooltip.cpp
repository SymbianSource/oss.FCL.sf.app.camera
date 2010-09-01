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
* Description:  Active Palette Tooltip*
*/


/**
 * @file ActivePalette2Tooltip.cpp
 * Active Palette Tooltip
 */

// INCLUDE FILES
#include <activepalette2observer.h>
#include <fbs.h>
#include <bitstd.h>
#include <AknsDrawUtils.h>
#include <AknsControlContext.h>
#include <AknsBasicBackgroundControlContext.h> 
#include <AknUtils.h>

#include "ActivePalette2Tooltip.h"
#include "ActivePalette2Logger.h"
#include "ActivePalette2Utils.h"
#include "ActivePalette2Model.h"
#include "ActivePalette2Cfg.h"
#include "ActivePalette2BasicUI.h"
#include "ActivePalette2Styler.h"


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::NewL()
// -----------------------------------------------------------------------------
//
 CActivePalette2Tooltip* CActivePalette2Tooltip::NewL(const CActivePalette2BasicUI* aParent,
													CActivePalette2Model* aModel,
													CActivePalette2Styler* aStyler)
    {
	CActivePalette2Tooltip * self = new (ELeave) 
		CActivePalette2Tooltip(aModel, aStyler);
				
	CleanupStack::PushL(self);
	self->ConstructL(aParent);
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::CActivePalette2Tooltip()
// C++ constructor
// -----------------------------------------------------------------------------
//
CActivePalette2Tooltip::CActivePalette2Tooltip(
    CActivePalette2Model* aModel,
    CActivePalette2Styler* aStyler)
:   CCoeControl(),
    iAnchor(TPoint(0,0)),
    iModel(aModel),
    iStyler(aStyler)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::ConstructL()
// Symbian 2nd phase construction
// -----------------------------------------------------------------------------
//
void CActivePalette2Tooltip::ConstructL(const CActivePalette2BasicUI* aParent)
    {

    iParent = aParent;

	if(aParent)
	    {
	    SetContainerWindowL(*aParent);
	    }

	// finally
	MakeVisible(EFalse);	
	ActivateL();
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::~CActivePalette2Tooltip()
// Destructor
// -----------------------------------------------------------------------------
//
CActivePalette2Tooltip::~CActivePalette2Tooltip()
    {
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::Draw()
// -----------------------------------------------------------------------------
//
void CActivePalette2Tooltip::Draw(const TRect& /*aRect*/) const
    {
    if ( !iRenderGc && iModel->ShowTooltip() )
        {
    	ProduceTooltipToScreen(EFalse);
    	}
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::ProduceTooltipToScreen()
// -----------------------------------------------------------------------------
//
void CActivePalette2Tooltip::ProduceTooltipToScreen(TBool aActivateGc) const
    {
    // Redraw background
    TRect redrawRect;
    TRect tooltipRect(TooltipRect());
    
    if ( iModel->ShowTooltip() )
        {
        redrawRect = tooltipRect;

        if ( iRenderedRect.Width() > 0 )
            {
            redrawRect.BoundingRect(iRenderedRect);
            }
        }
    else
        {
        redrawRect = iRenderedRect;
        }
        
    if ( !iRenderGc && aActivateGc )
        {
        Window().Invalidate(redrawRect);
    	ActivateGc();
    	Window().BeginRedraw(redrawRect);
        }
    
    // Draw in the background
    if ( iParent && iParent->APObserver() )
        {
        if ( redrawRect.Size().iHeight > 0 && redrawRect.Size().iWidth > 0 )
            {
            iParent->APObserver()->Redraw(redrawRect);
            }
        }

    if ( iModel->ShowTooltip() )
        {
        if ( iRenderGc )
            {
        	iStyler->BlitTooltip(iRenderGc, tooltipRect.iTl,
        	                     TRect(TPoint(0,0),TPoint(tooltipRect.Width(), tooltipRect.Height())));
            }
        else
            {    
            iStyler->BlitTooltip(&SystemGc(), tooltipRect.iTl,
                                 TRect(TPoint(0,0),TPoint(tooltipRect.Width(), tooltipRect.Height())));
        	}
        }
            	
    if ( !iRenderGc && aActivateGc )
        {
    	Window().EndRedraw();
    	DeactivateGc();	
        }

    if ( iModel->ShowTooltip() )
        {
        iRenderedRect = tooltipRect;
        }
    else
        {
        iRenderedRect = TRect(0,0,0,0);
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipUpdated()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Tooltip::TooltipUpdated()
    {
    return iStyler->DrawTooltip(iModel->TooltipText());
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipAnimated()
// -----------------------------------------------------------------------------
//
void CActivePalette2Tooltip::TooltipAnimated()
    {
    ProduceTooltipToScreen(ETrue);

    if ( !iRenderGc )
        {                	
    	SetRect(TooltipRect());
    	}
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipRect()
// -----------------------------------------------------------------------------
//
TRect CActivePalette2Tooltip::TooltipRect() const
    {
    TInt yPos = iAnchor.iY + iStyler->TootipYOffset(iModel->TooltipScreenPosition());
    return TRect(TPoint(iAnchor.iX - ((iStyler->TooltipSize().iWidth * iModel->TooltipCurrentFrame()) / iModel->TooltipTotalFrames()),
                            yPos),
                   TPoint(iAnchor.iX,
                            yPos + iStyler->TooltipSize().iHeight));
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::SetAnchor()
// -----------------------------------------------------------------------------
//
void CActivePalette2Tooltip::SetAnchor(TPoint aAnchor)
    {
    iAnchor = aAnchor;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::SetGc()
// -----------------------------------------------------------------------------
//
void CActivePalette2Tooltip::SetGc(CBitmapContext* aGc)
    {
    iRenderGc = aGc;
    MakeVisible(!aGc);
    }
    

// End of File  
