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
* Description:  AP implementation using standard Symbian controls*
*/


/**
 * @file ActivePalette2BasicUI.cpp
 * AP implementation using standard Symbian controls
 */


// INCLUDE FILES
#include <eikenv.h>
#include <aknenv.h>
#include <barsread.h>
#include <AknsDrawUtils.h>
#include <AknsControlContext.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknUtils.h>
#include <gdi.h>
#include <activepalette2factory.h>
#include <activepalette2observer.h>
#include <activepalette2ui.h>
#include <activepalette2eventdata.h>

#include "ActivePalette2BasicUI.h"
#include "ActivePalette2Styler.h"
#include "ActivePalette2StylerFactory.h"
#include "ActivePalette2Item.h"
#include "ActivePalette2Cfg.h"
#include "activepalette2configuration.h"
#include "ActivePalette2Logger.h"
#include "ActivePalette2Utils.h"
#include "ActivePalette2Model.h"
#include "ActivePalette2Const.h"
#include "ActivePalette2Tooltip.h"
#include "activepalette2tooltipfactory.h"


// ================= MEMBER FUNCTIONS =======================


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::NewL()
// Symbian 2 phase constructor
// -----------------------------------------------------------------------------
//
CActivePalette2BasicUI* CActivePalette2BasicUI::NewL(TActivePalette2DrawMode aDrawMode)
	{
	CActivePalette2BasicUI* self = new (ELeave) CActivePalette2BasicUI();
	CleanupStack::PushL(self);
	self->ConstructL(aDrawMode);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::CActivePalette2BasicUI()
// C++ constructor
// -----------------------------------------------------------------------------
//

CActivePalette2BasicUI::CActivePalette2BasicUI():
    iNavigationKeys()                  // default navigation keys
  {
	LOGTEXT( _L( "CActivePalette2BasicUI::CActivePalette2BasicUI"));
  }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ConstructL
// Leaving constructor
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ConstructL(TActivePalette2DrawMode aDrawMode )
	{
	LOGTEXT( _L( "CActivePalette2BasicUI::ConstructL entered"));

	// UI sizes and dimensions
	AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, iScreenSize );

	iTopLeftCorner.iX = iScreenSize.iWidth + ActivePalette2Utils::APDimensionPoint(ActivePalette2Utils::EDefault2dPaletteOffset).iX;
	iTopLeftCorner.iY = ActivePalette2Utils::APDimensionPoint(ActivePalette2Utils::EDefault2dPaletteOffset).iY;
	
	iDrawMode = aDrawMode;

  iStyler = ActivePalette2StylerFactory::CreateStylerL(aDrawMode);

  TRendererCallBacks callBacks;
  callBacks.iTooltipTimer = this;
  callBacks.iFocusTimer = this;
  callBacks.iItemScrollTimer = this;
  callBacks.iAppearanceTimer = this;
  
  // Configuration provider
  iConfiguration = CActivePalette2Configuration::NewL( aDrawMode );  
  
	// model
	iModel = CActivePalette2Model::NewL( *this, 
	                                     ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::EItemSize ), 
	                                     callBacks, 
	                                     iConfiguration );
  // valid model required from this point

  iTooltipNote = ActivePalette2TooltipFactory::CreateTooltipL( aDrawMode, this, iModel, iStyler );
  iTooltipNote->SetAnchor(iTopLeftCorner);

	ReCalcAutoSizeL();

	// enter idle
	RedrawIdle(EFalse);

	ActivateL();

	LOGTEXT( _L( "CActivePalette2BasicUI::ConstructL left"));
	}

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::~CActivePalette2BasicUI()
// Destructor
// -----------------------------------------------------------------------------
//
CActivePalette2BasicUI::~CActivePalette2BasicUI()
    {
	LOGTEXT( _L( "CActivePalette2BasicUI::~CActivePalette2BasicUI entered"));

	delete iTooltipNote;
	iTooltipNote = NULL;

  delete iStyler;
  iStyler = NULL;
  
  delete iConfiguration;
  iConfiguration = NULL;

	delete iModel;
	iModel = NULL;

	LOGTEXT( _L( "CActivePalette2BasicUI::~CActivePalette2BasicUI left"));
    }




// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::Draw()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::Draw(const TRect& aRect) const
	{
	LOGTEXT( _L( "CActivePalette2BasicUI::Draw entered" ));

    if ( !iRenderGc )
        {    
    	// Flush buf to the screen
    	ProducePaletteToScreen(aRect);
    	
    	if ( iTooltipNote )
        	{
        	iTooltipNote->Draw(aRect);
        	}
    	}

	LOGTEXT( _L( "CActivePalette2BasicUI::Draw left" ));
	}


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::RedrawIdle()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::RedrawIdle(TBool aForceDisplayUpdate) const
    {
    const TRect rect(TPoint(0,0),TPoint(Rect().Width(), Rect().Height()));

    // Invalidating and redrawing the toolbar background at this point causes
    // blinking and is unnecessary. Background will be redrawn with the toolbar.
    /*
    if ( iAPObserver )
        {
        TRect rectToFlush(DrawBufRectToScreenRect(rect));
        
        if ( rectToFlush.Size().iWidth > 0 && rectToFlush.Size().iHeight > 0 )
            {
            if ( !iRenderGc )
                {
            	Window().Invalidate(rectToFlush);
            	ActivateGc();
            	Window().BeginRedraw(rectToFlush);
                }            
                
            iAPObserver->Redraw(rectToFlush);

            if ( !iRenderGc )
                {
            	Window().EndRedraw();
            	DeactivateGc();
            	}
        	}
        }
    */
    
    iStyler->DrawTopScrollArrowSection(iModel->ShowTopScrollIndicator());

    TInt num_items = iModel->CountItemsOnScreen();
    if ( iModel->ItemScrollOffset() != 0 )
        {
        num_items++;
        }

	TInt index;
	for (index = 0; index < num_items; index++)
	    {
		CActivePalette2Item* item = iModel->ItemFromScreenPos(index);
		
		if ( item )
		    {
            iStyler->DrawItem(index, &item->Icon(), item->Mask(), iModel->ItemScrollOffset(), iModel->ItemScrollTotalFrames());
            if (index+1 < num_items)
                {
                iStyler->DrawGap(index, iModel->ItemScrollOffset(), iModel->ItemScrollTotalFrames());
                }
			}
	    }

    iStyler->DrawBottomScrollArrowSection(iModel->ShowBottomScrollIndicator());
    
    if ( iModel->PaletteActive() 
         && iModel->CountItemsOnScreen() > 0 )
        {
        TInt focusedItem = iModel->FocusedItem();
        iStyler->DrawFocusRing(focusedItem, iModel->FocusCurrentOffset(), iModel->FocusCurrentTotalOffsetFrames());
        iFocusedItem1 = focusedItem;
        iFocusedItem2 = focusedItem + 1;
	    }

	if (aForceDisplayUpdate)
	    {
		DrawDeferred();
		
        if ( iRenderGc && iAPObserver )
            {
            iAPObserver->ActivePaletteUpdated();
            }
    	}
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ProducePaletteToScreen()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ProducePaletteToScreen(const TRect& aRect) const
	{
	if ( iAPObserver )
    	{
    	TRect redrawRect(aRect);
    	redrawRect.Intersection(iRenderedRect);
        if ( redrawRect.Size().iHeight > 0 && redrawRect.Size().iWidth > 0 )
            {
            iAPObserver->Redraw(redrawRect);
            }
        } 
   
    iRenderedRect = TRect(PalettePosition(), iStyler->Size());

	if ( iRenderGc )
    	{
        iStyler->BlitPalette(iRenderGc, PalettePosition(), ScreenRectToDrawBufRect(aRect));
    	}
	else
    	{
        iStyler->BlitPalette(&SystemGc(), PalettePosition(), ScreenRectToDrawBufRect(aRect));
        }            

	}

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::FlushBufNow()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::FlushBufNow(const TRect& aRectToFlush)
    {
    if ( !iRenderGc )
        {
    	Window().Invalidate(aRectToFlush);
    	ActivateGc();
    	Window().BeginRedraw(aRectToFlush);
        }            
            
	ProducePaletteToScreen(aRectToFlush);

    if ( !iRenderGc )
        {
    	Window().EndRedraw();
    	DeactivateGc();
    	}
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetCurrentItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::SetCurrentItem(const TInt aItemId)
    {
    return iModel->SetCurrentItem(aItemId);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::GetCurrentItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::GetCurrentItem(TInt & aItemId) const
    {
    return iModel->GetCurrentItem(aItemId);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SizeChanged()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::SizeChanged(void)
    {
	LOGTEXT( _L( "CActivePalette2BasicUI::SizeChanged") );
	CCoeControl::SizeChanged();
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetNavigationKeys()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::SetNavigationKeys(const TActivePalette2NavigationKeys & aNavigationKeys)
    {
	iNavigationKeys = aNavigationKeys;
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetObserver()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::SetObserver(MActivePalette2Observer * aObserver)
    {
	iAPObserver = aObserver;
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SendMessage()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::SendMessage(TInt aItemId, TInt aMessageId, const TDesC8& aDataDes)
    {
    return iModel->SendMessage(aItemId, aMessageId, aDataDes);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SendMessage()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::SendMessage(const TInt aItemId, const TInt aMessageId, const TInt aDataInt)
    {
    return iModel->SendMessage(aItemId, aMessageId, aDataInt);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ReCalcAutoSizeL()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ReCalcAutoSizeL(void)
    {
    iStyler->SetNumberOfItemsL(iModel->CountItemsOnScreen());

    TRect newRect(TPoint(0,0),iStyler->Size());
    newRect.Move(PalettePosition());
    SetRect(newRect);
    }


// -----------------------------------------------------------------------------
// TInt CActivePalette2BasicUI::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
                                             const TUid& aPluginUid,
                                             const TDesC8& aCustomDataDes)
    {
	return iModel->InstallItemL(aItemVisible, aPluginUid, aCustomDataDes);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
                                             const TUid& aPluginUid,
                                             TInt aCustomDataInt)
    {
	return iModel->InstallItemL(aItemVisible, aPluginUid, aCustomDataInt);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
                                             const TUid& aPluginUid,
                                             TInt aCustomDataInt,
                                             const TDesC8& aCustomDataDes)
    {
	return iModel->InstallItemL(aItemVisible, aPluginUid, aCustomDataInt, aCustomDataDes);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::RemoveItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::RemoveItem(const TInt aItemId)
    {
    return iModel->RemoveItem(aItemId);
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::LocateTo()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::LocateTo(const TPoint& aTopLeft)
	{
	iTopLeftCorner = aTopLeft;
    iTooltipNote->SetAnchor(aTopLeft);
    
	TRect newRect(PalettePosition(),Rect().Size());
	SetRect(newRect);
	}


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::Location()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2BasicUI::Location(void) const
	{
	return iTopLeftCorner;
	}


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::OfferKeyEventL(
// -----------------------------------------------------------------------------
//
TKeyResponse CActivePalette2BasicUI::OfferKeyEventL(const TKeyEvent& aKeyEvent,
                                              TEventCode aType)
    {
	LOGTEXT2( _L( "CActivePalette2BasicUI::OfferKeyEventL entered, type : %d, scancode : %d"),
			aType, aKeyEvent.iScanCode);

	TKeyResponse res = EKeyWasNotConsumed;
	if (EEventKey == aType && iModel)
    	{
		if (aKeyEvent.iScanCode == iNavigationKeys.iNaviPrev)
	    	{ 
	    	// go to previous
            if ( iModel->ProcessInputEvent(EAP2InputUp) )
                {
    			res = EKeyWasConsumed;
                }
		    }
		else if (aKeyEvent.iScanCode == iNavigationKeys.iNaviNext)
		    { 
		    // go to next
            if ( iModel->ProcessInputEvent(EAP2InputDown) )
                {
    			res = EKeyWasConsumed;
                }
		    }
		else if ( aKeyEvent.iScanCode == iNavigationKeys.iNaviSelect &&
		          aKeyEvent.iRepeats == 0 )
		    { 
		    // select item
            if ( iModel->ProcessInputEvent(EAP2InputSelect) )
                {
    			res = EKeyWasConsumed;
                }
		    }
		else
		    {
		    // Lint!
    		}
	    }

	LOGTEXT1( _L( "CActivePalette2BasicUI::OfferKeyEventL left, res : %d"),res);

	return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::DrawBufRectToScreenRect()
// -----------------------------------------------------------------------------
//
TRect CActivePalette2BasicUI::DrawBufRectToScreenRect(TRect aDrawBufRect) const
    {
    TRect screenRect(aDrawBufRect);
    screenRect.Move(PalettePosition());
    return screenRect;
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ScreenRectToDrawBufRect()
// -----------------------------------------------------------------------------
//
TRect CActivePalette2BasicUI::ScreenRectToDrawBufRect(TRect aScreenRect) const
    {
    TRect bufRect(aScreenRect);
    bufRect.Move(-PalettePosition());
    return bufRect;
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetContainerWindowL()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::SetContainerWindowL(const CCoeControl& aContainer)
    {
    CCoeControl::SetContainerWindowL(aContainer);
    
    if(iTooltipNote)
        {
        iTooltipNote->SetContainerWindowL(aContainer);
        }

    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::CountComponentControls()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::CountComponentControls() const
    {
    return 1;
    }


// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ComponentControl()
// -----------------------------------------------------------------------------
//
CCoeControl* CActivePalette2BasicUI::ComponentControl(TInt aIndex) const
    {
    switch ( aIndex )
        {
        case 0:
            return iTooltipNote;
        default:
            return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::TooltipUpdated()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::TooltipUpdated()
    {
    return iTooltipNote->TooltipUpdated();
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::TooltipAnimated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::TooltipAnimated()
    {
    iTooltipNote->TooltipAnimated();

    if ( iRenderGc && iAPObserver )
        {
        iAPObserver->ActivePaletteUpdated();
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ShowTooltipUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ShowTooltipUpdated()
    {
    TooltipAnimated();
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ItemsUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ItemsUpdated()
    {
	TRAP_IGNORE(ReCalcAutoSizeL());
	
    RedrawIdle(ETrue);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ItemAnimated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ItemAnimated(TInt aScreenPos, CActivePalette2Item* aItem, TInt aAnimFrame)
    {
	// animation frame
	TBool ownershipTransferForFrame;
	TBool ownershipTransferForMask;
	CFbsBitmap* frame = NULL;
	CFbsBitmap* frameMask = NULL;

	aItem->Plugin().ProduceAniFrame(
		CActivePalettePluginBase::EAniFocused,
		aAnimFrame,
		iModel->ItemAnimInfo(),
		aItem->Icon(),
		aItem->Mask(),
		&frame,
		&frameMask,
		ownershipTransferForFrame,
		ownershipTransferForMask);

    iStyler->AnimItem(aScreenPos, frame, frameMask);

	// release resources
	if (ownershipTransferForFrame)
	    {
		delete frame;
	    }
	if (ownershipTransferForMask)
	    {
		delete frameMask;
	    }

	// flush
	FlushDirtyRect();
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ItemAnimationComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ItemAnimationComplete(TInt aScreenPos, CActivePalette2Item* aItem)
    {
	aItem->Plugin().ReleaseAniFrames(CActivePalettePluginBase::EAniFocused);
	DrawItem(aScreenPos, ETrue);
	FlushDirtyRect();
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::FocusUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::FocusUpdated()
    {
    TInt focusedItem = iModel->FocusedItem();

    DrawItemRange(iFocusedItem1, iFocusedItem2);
    iFocusedItem1 = focusedItem;
    iFocusedItem2 = focusedItem;

    DrawItemRange(focusedItem, focusedItem);
    iStyler->DrawFocusRing(focusedItem, iModel->FocusCurrentOffset(), iModel->FocusCurrentTotalOffsetFrames());
    FlushDirtyRect();
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::FocusAnimated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::FocusAnimated()
    {
    TInt focusedItem = iModel->FocusedItem();

    if ( focusedItem != iFocusedItem1 || (focusedItem + 1) != iFocusedItem2)
        {
        DrawItemRange(iFocusedItem1, iFocusedItem2);
        iFocusedItem1 = focusedItem;
        iFocusedItem2 = focusedItem + 1;
        }

    DrawItemRange(iFocusedItem1, iFocusedItem2);
        
    iStyler->DrawFocusRing(iFocusedItem1, iModel->FocusCurrentOffset(), iModel->FocusCurrentTotalOffsetFrames());
    FlushDirtyRect();
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::PaletteAppearingAnimated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::PaletteAppearingAnimated()
    {
    PaletteAppearingUpdated();
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::PaletteAppearingUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::PaletteAppearingUpdated()
    {
    TRect newRect(PalettePosition(), Rect().Size());
    
    if ( !iRenderGc )
        {
        Window().Invalidate(Rect());
        Window().Invalidate(newRect);  
        }
    SetRect(newRect);            
    RedrawIdle(ETrue);
    
    if ( iRenderGc )
        {
        TRect totalRect(Rect());
        totalRect.BoundingRect(newRect);
        ProducePaletteToScreen(totalRect);
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetPaletteVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::SetPaletteVisibility(TBool aVisible, TBool aAnimated, TInt aDelayedStartMilliseconds)
    {
    return iModel->SetPaletteVisibility(aVisible, aAnimated, aDelayedStartMilliseconds);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::PalettePosition()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2BasicUI::PalettePosition() const
    {
    TInt totalFrames = iModel->PaletteAppearingTotalFrames();
    TInt xOffset = ((iScreenSize.iWidth - iTopLeftCorner.iX) * (totalFrames - iModel->PaletteAppearingCurrentFrame())) / totalFrames;
    
    return TPoint(iTopLeftCorner.iX + xOffset, iTopLeftCorner.iY);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::APObserver()
// -----------------------------------------------------------------------------
//
MActivePalette2Observer* CActivePalette2BasicUI::APObserver() const
    {
    return iAPObserver;
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetItemVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::SetItemVisibility(const TInt aItemId, const TBool aIsVisible)
    {
    return iModel->SetItemVisibility(aItemId, aIsVisible);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::GetItemVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::GetItemVisibility(const TInt aItemId, TBool &aIsVisible) const
    {
    return iModel->GetItemVisibility(aItemId, aIsVisible);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetGc()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::SetGc(CBitmapContext* aGc)
    {
    iRenderGc = aGc;
    iTooltipNote->SetGc(aGc);
    iRenderedRect = TRect(0,0,0,0);
    MakeVisible(!aGc);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::GetAvailablePlugins()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::GetAvailablePlugins(RArray<TUid>& aPluginList) const
    {
    return iModel->GetAvailablePlugins(aPluginList);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::RenderActivePalette()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::RenderActivePalette(const TRect& aRect) const
    {
        ProducePaletteToScreen(aRect);

    	if ( iModel->ShowTooltip() )
        	{
        	iTooltipNote->ProduceTooltipToScreen(EFalse);
        	}
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetPaletteVisibilityAnimationDuration()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::SetPaletteVisibilityAnimationDuration(TInt aTimeInMilliSeconds)
    {
    return iModel->SetPaletteVisibilityAnimationDuration(aTimeInMilliSeconds);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::GetItemList()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::GetItemList(RArray<TActivePalette2ItemVisible>& aItemVisibleList) const
    {
    return iModel->GetItemList(aItemVisibleList);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::SetItemList()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::SetItemList(const RArray<TActivePalette2ItemVisible>& aItemVisibleList)
    {
    return iModel->SetItemList(aItemVisibleList);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::CoeControl()
// -----------------------------------------------------------------------------
//
CCoeControl* CActivePalette2BasicUI::CoeControl() 
    { 
    return this; 
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::HuiControl()
// -----------------------------------------------------------------------------
//
CHuiControl* CActivePalette2BasicUI::HuiControl() 
    { 
    return NULL; 
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::GetPaletteVisibilityAnimationDuration()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2BasicUI::GetPaletteVisibilityAnimationDuration(TInt& aTimeInMilliseconds) const
    {
    return iModel->GetPaletteVisibilityAnimationDuration(aTimeInMilliseconds);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::ItemsScrolled()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::ItemsScrolled()
    {
    RedrawIdle(ETrue);
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::DrawItemRange()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::DrawItemRange(TInt aStartIndex, TInt aFinishIndex) const
    {
    aStartIndex = Max(aStartIndex, 0);
    aFinishIndex = Min(aFinishIndex, iModel->CountItemsOnScreen());

    if ( aFinishIndex >= aStartIndex )
        {
        TInt index;
        for ( index = aStartIndex - 1; index <= aFinishIndex; index++)
            {
            DrawSeparator(index);
            }

        for ( index = aStartIndex; index <= aFinishIndex; index++)
            {
            DrawItem(index);
            }
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::DrawItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::DrawItem(TInt aIndex, TBool aAnimOnly) const
    {
	CActivePalette2Item* item = iModel->ItemFromScreenPos(aIndex);
	
	if ( item )
	    {
	    if ( aAnimOnly )
	        {
            iStyler->AnimItem(aIndex, &item->Icon(), item->Mask());
	        }
	    else
	        {
            iStyler->DrawItem(aIndex, &item->Icon(), item->Mask(), iModel->ItemScrollOffset(), iModel->ItemScrollTotalFrames());
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::DrawSeparator()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::DrawSeparator(TInt aIndex) const
    {
    if ( aIndex < 0 )
        {
        iStyler->DrawTopScrollArrowSection(iModel->ShowTopScrollIndicator());
        }
    else if (aIndex >= (iModel->CountItemsOnScreen()-1))
        {
        iStyler->DrawBottomScrollArrowSection(iModel->ShowBottomScrollIndicator());
        }
    else
        {
        iStyler->DrawGap(aIndex, iModel->ItemScrollOffset(), iModel->ItemScrollTotalFrames());
        }
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::FlushDirtyRect()
// -----------------------------------------------------------------------------
//
void CActivePalette2BasicUI::FlushDirtyRect()
    {
	FlushBufNow(DrawBufRectToScreenRect(iStyler->DirtyRect()));
	iStyler->ClearDirtyRect();

    if ( iRenderGc && iAPObserver)
        {
        iAPObserver->ActivePaletteUpdated();
        }
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::HandleResourceChange()
// -----------------------------------------------------------------------------
//   
void CActivePalette2BasicUI::HandleResourceChange( TInt aType )
	{
	if ( KEikDynamicLayoutVariantSwitch == aType )
        {
        // UI sizes and dimensions
		AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, iScreenSize );
		
		TRAP_IGNORE( ReCalcAutoSizeL() );
		
		// enter idle
		RedrawIdle(EFalse);
        }
        
    CCoeControl::HandleResourceChange( aType );

	}
// End of File
