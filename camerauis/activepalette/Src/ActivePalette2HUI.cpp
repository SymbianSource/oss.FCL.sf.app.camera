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
* Description:  AP implementation using HUITK*
*/


/**
 * @file ActivePalette2HUI.cpp
 * AP implementation using HUITK
 */

#include <uiacceltk/HuiEvent.h>
#include <uiacceltk/HuiImageVisual.h>
#include <uiacceltk/HuiUtil.h>
#include <uiacceltk/HuiFlowLayout.h>
#include <uiacceltk/HuiTextVisual.h>
#include <uiacceltk/HuiMeshVisual.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <activepalette2factory.h>
#include <activepalette2observer.h>
#include <activepalette2ui.h>
#include <activepalette2graphics.mbg>

#include "ActivePalette2HUI.h"
#include "ActivePalette2Cfg.h"
#include "ActivePalette2Item.h"
#include "ActivePalette2Utils.h"
#include "ActivePalette2Model.h"



#define PREPROCESS_BITMAPS

#define KPaletteHiddenXPos (iScreenSize.iWidth)

// -----------------------------------------------------------------------------
// CActivePalette2HUI::CActivePalette2HUI()
// -----------------------------------------------------------------------------
//
CActivePalette2HUI::CActivePalette2HUI(CHuiEnv& aEnv)
: CHuiControl(aEnv) 
    {
    // No implementation required 
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::NewL()
// -----------------------------------------------------------------------------
//
CActivePalette2HUI* CActivePalette2HUI::NewL(CHuiEnv& aEnv) 
    { 
	CActivePalette2HUI* self = new(ELeave) CActivePalette2HUI(aEnv); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::ConstructL()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::ConstructL()
    {
    TRendererCallBacks callBacks;
    callBacks.iTooltipDescriptive = this;
    callBacks.iItemScrollDescriptive = this;
    callBacks.iFocusDescriptive = this;
    callBacks.iAppearanceDescriptive = this;
    
	iModel = CActivePalette2Model::NewL(*this, ActivePalette2Utils::APDimensionSize( EItemSize ), callBacks);

    UpdateTopLeftCornerWithScreenSize();
	                        
	// Full screen layout, used to get screen-orientation notifications
	iFullScreenLayout = AppendLayoutL(EHuiLayoutTypeAnchor);
	iFullScreenLayout->SetFlags(EHuiVisualFlagLayoutUpdateNotification);
	
    iPaletteLayout = AppendLayoutL(EHuiLayoutTypeAnchor);
    iPaletteLayout->SetFlags(EHuiVisualFlagManualLayout);
    iPaletteLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(EPaletteWidth), ActivePalette2Utils::APDimension(ETopSectionHeight) + ActivePalette2Utils::APDimension(EBottomSectionHeight) )));
    iPaletteLayout->SetPos(THuiRealPoint(iTopLeftCorner));
    iPaletteLayout->SetClipping(ETrue);

	CFbsBitmap* picture;
	CFbsBitmap* mask;

    // Background top-section
    iBackgroundTopSectionLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iPaletteLayout);
    iBackgroundTopSectionLayout->SetFlags(EHuiVisualFlagManualLayout);
    iBackgroundTopSectionLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(EPaletteWidth), ActivePalette2Utils::APDimension(ETopSectionHeight))));
    iBackgroundTopSectionLayout->SetPos(THuiRealPoint(TopScrollPanePosition()));

    iBackgroundTopSectionVisual = CHuiImageVisual::AddNewL(*this, iBackgroundTopSectionLayout);

	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_top, EMbmActivepalette2graphicsQgn_graf_cam4_tb_top_mask);

    iBackgroundTopSectionTexture = CHuiTexture::NewL();
    iBackgroundTopSectionTexture->UploadL(*picture, mask);
    
    iBackgroundTopSectionVisual->SetImage( THuiImage( *iBackgroundTopSectionTexture ) );
    
    CleanupStack::PopAndDestroy(); // picture
    CleanupStack::PopAndDestroy(); // mask


    // Background mid-section
    iBackgroundMidSectionLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iPaletteLayout);
    iBackgroundMidSectionLayout->SetFlags(EHuiVisualFlagManualLayout);
    iBackgroundMidSectionLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(EPaletteWidth), 0)));
    iBackgroundMidSectionLayout->SetPos(THuiRealPoint(TopScrollPanePosition() + TPoint(0, ActivePalette2Utils::APDimension(ETopSectionHeight))));

    iBackgroundMidSectionVisual = CHuiImageVisual::AddNewL(*this, iBackgroundMidSectionLayout);

	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body, EMbmActivepalette2graphicsQgn_graf_cam4_tb_body_mask);

    iBackgroundMidSectionTexture = CHuiTexture::NewL();
    iBackgroundMidSectionTexture->UploadL(*picture, mask);
    
    iBackgroundMidSectionVisual->SetImage( THuiImage( *iBackgroundMidSectionTexture ) );
    
    CleanupStack::PopAndDestroy(); // picture
    CleanupStack::PopAndDestroy(); // mask


    // Background bottom-section
    iBackgroundBottomSectionLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iPaletteLayout);
    iBackgroundBottomSectionLayout->SetFlags(EHuiVisualFlagManualLayout);
    iBackgroundBottomSectionLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(EPaletteWidth), ActivePalette2Utils::APDimension(EBottomSectionHeight))));
    iBackgroundBottomSectionLayout->SetPos(THuiRealPoint(BottomScrollPanePosition()));

    iBackgroundBottomSectionVisual = CHuiImageVisual::AddNewL(*this, iBackgroundBottomSectionLayout);

	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom, EMbmActivepalette2graphicsQgn_graf_cam4_tb_bottom_mask);

    iBackgroundBottomSectionTexture = CHuiTexture::NewL();
    iBackgroundBottomSectionTexture->UploadL(*picture, mask);
    
    iBackgroundBottomSectionVisual->SetImage( THuiImage( *iBackgroundBottomSectionTexture ) );
    
    CleanupStack::PopAndDestroy(); // picture
    CleanupStack::PopAndDestroy(); // mask

    
    // Item layouts
    for( TInt i = 0; i < NAP2Cfg::KMaxNofItemsInView + 1; i++)
        {
        CHuiLayout* layout = AppendLayoutL(EHuiLayoutTypeAnchor, iPaletteLayout);
        layout->SetFlags(EHuiVisualFlagManualLayout);
        layout->SetSize(THuiRealSize(ActivePalette2Utils::APDimensionSize( EItemSize )));
        layout->SetPos(THuiRealPoint(ItemPosition(i)));
        iItemLayout.Append(layout);
        
        CHuiImageVisual* image = CHuiImageVisual::AddNewL(*this, layout);
        iItemVisual.Append(image);
        
        CHuiTexture* texture = CHuiTexture::NewL();
        iItemTexture.Append(texture);
        
        image->SetImage( THuiImage( *texture ) );
        }
    
    
    // Top scroll icon
    iTopScrollLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iPaletteLayout);
    iTopScrollLayout->SetFlags(EHuiVisualFlagManualLayout);
    iTopScrollLayout->SetSize(THuiRealSize(ActivePalette2Utils::APDimensionSize(E3dScrollIconSize)));
    iTopScrollLayout->SetPos(THuiRealPoint(TopScrollPanePosition() + ActivePalette2Utils::APDimensionPoint(E3dScrollIconOffset)));
    
    iTopScrollVisual = CHuiImageVisual::AddNewL(*this, iTopScrollLayout);

	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_up, EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_up_mask);

    iTopScrollTexture = CHuiTexture::NewL();
    iTopScrollTexture->UploadL(*picture, mask);
    
    iTopScrollVisual->SetImage( THuiImage( *iTopScrollTexture ) );
    
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    
	
	// Bottom scroll icon
    iBottomScrollLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iPaletteLayout);
    iBottomScrollLayout->SetFlags(EHuiVisualFlagManualLayout);
    iBottomScrollLayout->SetSize(THuiRealSize(ActivePalette2Utils::APDimensionSize(E3dScrollIconSize)));
    iBottomScrollLayout->SetPos(THuiRealPoint(BottomScrollPanePosition() + ActivePalette2Utils::APDimensionPoint(E3dScrollIconOffset)));

    iBottomScrollVisual = CHuiImageVisual::AddNewL(*this, iBottomScrollLayout);

	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_down, EMbmActivepalette2graphicsQgn_graf_cam4_tb_arrow_down_mask);

    iBottomScrollTexture = CHuiTexture::NewL();
    iBottomScrollTexture->UploadL(*picture, mask);
    
    iBottomScrollVisual->SetImage( THuiImage( *iBottomScrollTexture ) );
    
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    
    
    // Focus ring
    iFocusRingLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iPaletteLayout);
    iFocusRingLayout->SetFlags(EHuiVisualFlagManualLayout);
    iFocusRingLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(EFocusRingWidth), ActivePalette2Utils::APDimension(EFocusRingHeight))));
    iFocusRingLayout->SetPos(THuiRealPoint(ItemPosition(0) + TPoint(ActivePalette2Utils::APDimension(EFocusRingXOffset), ActivePalette2Utils::APDimension(EFocusRingYOffset))));
    
    iFocusRingVisual = CHuiImageVisual::AddNewL(*this, iFocusRingLayout);
    
	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_focus, EMbmActivepalette2graphicsQgn_graf_cam4_tb_focus_mask);

    iFocusRingTexture = CHuiTexture::NewL();
    iFocusRingTexture->UploadL(*picture, mask);
    
    iFocusRingVisual->SetImage( THuiImage( *iFocusRingTexture ) );
    
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    
    
    // Tooltip
	iTooltipLayout = AppendLayoutL(EHuiLayoutTypeAnchor);
    iTooltipLayout->SetFlags(EHuiVisualFlagManualLayout);
    iTooltipLayout->SetSize(THuiRealSize(TSize(0, 0)));
    iTooltipLayout->SetPos(THuiRealPoint(iTopLeftCorner));
    iTooltipLayout->SetClipping(ETrue);


    // Tooltip background for nose section
    iTooltipBackgroundNoseLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iTooltipLayout);
    iTooltipBackgroundNoseLayout->SetFlags(EHuiVisualFlagManualLayout);
    iTooltipBackgroundNoseLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(ETooltipNose), ActivePalette2Utils::APDimension(ETooltipHeight))));
    iTooltipBackgroundNoseLayout->SetPos(THuiRealPoint(TPoint(0,0)));

    iTooltipBackgroundNoseVisual = CHuiImageVisual::AddNewL(*this, iTooltipBackgroundNoseLayout);

	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_end, EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_end_mask);

    iTooltipBackgroundNoseTexture = CHuiTexture::NewL();
    iTooltipBackgroundNoseTexture->UploadL(*picture, mask);
    
    iTooltipBackgroundNoseVisual->SetImage( THuiImage( *iTooltipBackgroundNoseTexture ) );
    
    CleanupStack::PopAndDestroy(); // picture
    CleanupStack::PopAndDestroy(); // mask

    
    // Tooltip background for nose section
    iTooltipBackgroundBodyLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iTooltipLayout);
    iTooltipBackgroundBodyLayout->SetFlags(EHuiVisualFlagManualLayout);
    iTooltipBackgroundBodyLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(E3dTooltipMaxWidth), ActivePalette2Utils::APDimension(ETooltipHeight))));
    iTooltipBackgroundBodyLayout->SetPos(THuiRealPoint(TPoint(ActivePalette2Utils::APDimension(ETooltipNose),0)));

    iTooltipBackgroundBodyVisual = CHuiImageVisual::AddNewL(*this, iTooltipBackgroundBodyLayout);

	AknIconUtils::CreateIconLC(picture, mask, NAP2Cfg::KUIGraphics, EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_body, EMbmActivepalette2graphicsQgn_graf_cam4_tb_tooltip_body_mask);
    
    iTooltipBackgroundBodyTexture = CHuiTexture::NewL();
    iTooltipBackgroundBodyTexture->UploadL(*picture, mask);
    
    iTooltipBackgroundBodyVisual->SetImage( THuiImage( *iTooltipBackgroundBodyTexture ) );
    
    CleanupStack::PopAndDestroy(); // picture
    CleanupStack::PopAndDestroy(); // mask


    // Tooltip text layouts    
    iTooltipTextLayout = AppendLayoutL(EHuiLayoutTypeAnchor, iTooltipLayout);
    iTooltipTextLayout->SetFlags(EHuiVisualFlagManualLayout);
    iTooltipTextLayout->SetSize(THuiRealSize(TSize(ActivePalette2Utils::APDimension(E3dTooltipMaxWidth), ActivePalette2Utils::APDimension(ETooltipHeight))));
    iTooltipTextLayout->SetPos(THuiRealPoint(TPoint(ActivePalette2Utils::APDimension(ETooltipNose), ActivePalette2Utils::APDimension(E3dTooltipTextYOffset))));

    iTooltipTextVisual = CHuiTextVisual::AddNewL(*this, iTooltipTextLayout);
    iTooltipTextVisual->SetStyle(EHuiTextStyleMenuSmall, EHuiBackgroundTypeLight);
    iTooltipTextVisual->SetMaxLineCount(1);
    iTooltipTextVisual->SetAlign(EHuiAlignHLeft, EHuiAlignVCenter);
    iTooltipTextVisual->SetTextL(_L(""));


	iGeneralTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::~CActivePalette2HUI()
// -----------------------------------------------------------------------------
//
CActivePalette2HUI::~CActivePalette2HUI()
    {
    if (iGeneralTimer)
        {
        iGeneralTimer->Cancel();
        delete iGeneralTimer;
        }
        
    iItemLayout.Reset();
    iItemVisual.Reset();
    iItemTexture.ResetAndDestroy();
    
    delete iBackgroundTopSectionTexture;
    delete iBackgroundMidSectionTexture;
    delete iBackgroundBottomSectionTexture;
    
    delete iFocusRingTexture;
    
    delete iBottomScrollTexture;
    delete iTopScrollTexture;

    delete iTooltipBackgroundBodyTexture;
    delete iTooltipBackgroundNoseTexture;
    
    delete iModel;
	iModel = NULL;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::InstallItemL(const TActivePalette2ItemVisible& aItemVisible, 
    						const TUid& aPluginUid, 
    						const TDesC8& aCustomDataDes) 
    { 
	return iModel->InstallItemL(aItemVisible, aPluginUid, aCustomDataDes);
    }
				
// -----------------------------------------------------------------------------
// CActivePalette2HUI::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
    						const TUid& aPluginUid, 
    						TInt aCustomDataInt ) 
    { 
	return iModel->InstallItemL(aItemVisible, aPluginUid, aCustomDataInt);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
    						const TUid& aPluginUid, 
    						TInt aCustomDataInt, 
    						const TDesC8& aCustomDataDes) 
	{ 
	return iModel->InstallItemL(aItemVisible, aPluginUid, aCustomDataInt, aCustomDataDes);
	}

// -----------------------------------------------------------------------------
// CActivePalette2HUI::RemoveItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::RemoveItem(const TInt aItemId) 
    { 
	return iModel->RemoveItem(aItemId);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetItemVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::SetItemVisibility(const TInt aItemId, const TBool aIsVisible) 
    { 
    return iModel->SetItemVisibility(aItemId, aIsVisible);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::GetItemVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::GetItemVisibility(const TInt aItemId, TBool &aIsVisible) const
    { 
    return iModel->GetItemVisibility(aItemId, aIsVisible);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::GetItemList()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::GetItemList(RArray<TActivePalette2ItemVisible>& aItemVisibleList) const
    { 
    return iModel->GetItemList(aItemVisibleList);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetItemList()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::SetItemList(const RArray<TActivePalette2ItemVisible>& aItemVisibleList) 
    { 
    return iModel->SetItemList(aItemVisibleList);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SendMessage()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::SendMessage(TInt aItemId, TInt aMessageId, const TDesC8& aDataDes) 
    { 
    return iModel->SendMessage(aItemId, aMessageId, aDataDes);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SendMessage()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::SendMessage(TInt aItemId, TInt aMessageId, TInt aDataInt) 
    { 
    return iModel->SendMessage(aItemId, aMessageId, aDataInt);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::GetCurrentItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::GetCurrentItem(TInt& aItemId) const 
    { 
	return iModel->GetCurrentItem(aItemId);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetCurrentItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::SetCurrentItem(const TInt aItemId) 
    { 
    return iModel->SetCurrentItem(aItemId);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetPaletteVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::SetPaletteVisibility(TBool aVisible, TBool aAnimated, TInt aDelayedStartMilliseconds) 
    { 
    return iModel->SetPaletteVisibility(aVisible, aAnimated, aDelayedStartMilliseconds);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetPaletteVisibilityAnimationDuration()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::SetPaletteVisibilityAnimationDuration(TInt aTimeInMilliseconds) 
    { 
    return iModel->SetPaletteVisibilityAnimationDuration(aTimeInMilliseconds);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::GetPaletteVisibilityAnimationDuration()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::GetPaletteVisibilityAnimationDuration(TInt& aTimeInMilliseconds) const 
    { 
    return iModel->GetPaletteVisibilityAnimationDuration(aTimeInMilliseconds);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::CoeControl()
// -----------------------------------------------------------------------------
//
CCoeControl* CActivePalette2HUI::CoeControl() 
    { 
    return NULL; 
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::HuiControl()
// -----------------------------------------------------------------------------
//
CHuiControl* CActivePalette2HUI::HuiControl() 
    { 
    return this; 
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetGc()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::SetGc(CBitmapContext* /*aGc*/) 
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::RenderActivePalette()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::RenderActivePalette(const TRect& /*aRect*/) const 
    { 
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetObserver()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::SetObserver(MActivePalette2Observer* aObserver) 
    { 
    iObserver = aObserver;
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::GetAvailablePlugins()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::GetAvailablePlugins(RArray<TUid>& aPluginList) const 
    { 
    return iModel->GetAvailablePlugins(aPluginList);
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::LocateTo()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::LocateTo(const TPoint& aTopLeft) 
    {  
    iTopLeftCorner = aTopLeft;
    iPaletteLayout->SetPos(THuiRealPoint(PalettePosition()));
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::Location()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2HUI::Location() const 
    { 
    return iTopLeftCorner; 
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::SetNavigationKeys()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::SetNavigationKeys(const TActivePalette2NavigationKeys& aNavigationKeys) 
    {  
    iNavigationKeys = aNavigationKeys;
    }





// -----------------------------------------------------------------------------
// CActivePalette2HUI::ShowTooltipUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::ShowTooltipUpdated()
    {
    if ( iModel->ShowTooltip() )
        {
        iTooltipLayout->iOpacity.Set(KOpacityFull);
    	}
    else
        {
        iTooltipLayout->iOpacity.Set(KOpacityNone);
        }
    }
    

// -----------------------------------------------------------------------------
// CActivePalette2HUI::ItemsUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::ItemsUpdated()
    {
    // Update size
    TSize overallSize(PaletteSize());
    iPaletteLayout->SetSize(THuiRealSize(overallSize));
    TSize itemsSize(overallSize);
    itemsSize.iHeight -= (ActivePalette2Utils::APDimension(ETopSectionHeight) + ActivePalette2Utils::APDimension(EBottomSectionHeight));
    iBackgroundMidSectionLayout->SetSize(THuiRealSize(itemsSize));
    iBackgroundBottomSectionLayout->SetPos(THuiRealPoint(BottomScrollPanePosition()));
    
    Render();
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::ItemAnimated()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::ItemAnimated(TInt aScreenPos, CActivePalette2Item* aItem, TInt aAnimFrame)
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

    TRAP_IGNORE(iItemTexture[aScreenPos]->UploadL(*frame, frameMask));
    iItemVisual[aScreenPos]->SetChanged();

	// release resources
	if (ownershipTransferForFrame)
	    {
		delete frame;
	    }
	if (ownershipTransferForMask)
	    {
		delete frameMask;
	    }
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::ItemAnimationComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::ItemAnimationComplete(TInt /*aScreenPos*/, CActivePalette2Item* aItem)
    {
	aItem->Plugin().ReleaseAniFrames(CActivePalettePluginBase::EAniFocused);
	Render();
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::PaletteAppearingUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::PaletteAppearingUpdated()
    {
    iGeneralTimer->Cancel();
    iPaletteLayout->SetPos(THuiRealPoint(PalettePosition()));
    UpdateFocus();
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::UpdateFocus()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::UpdateFocus()
    {
    if ( iModel->PaletteActive()
         && iModel->CountItemsOnScreen() > 0 )
        {
        TPoint startPos = ItemPosition(iModel->FocusedItem()) + TPoint(ActivePalette2Utils::APDimension(EFocusRingXOffset), ActivePalette2Utils::APDimension(EFocusRingYOffset));
        iFocusRingLayout->SetPos(THuiRealPoint(startPos));
        iFocusRingVisual->iOpacity.Set(1.0);
        }
    else
        {
        iFocusRingVisual->iOpacity.Set(0.0);
        }
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::APObserver()
// -----------------------------------------------------------------------------
//
MActivePalette2Observer* CActivePalette2HUI::APObserver() const
    {
    return iObserver;
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::Render()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::Render()
    {
#ifdef PREPROCESS_BITMAPS    
    CFbsBitmap*       tempBitmap = NULL;
    CFbsBitmapDevice* tempDevice = NULL;
    CBitmapContext*   tempGc     = NULL;

    CFbsBitmap*       tempMaskBitmap = NULL;
    CFbsBitmapDevice* tempMaskDevice = NULL;
    CBitmapContext*   tempMaskGc     = NULL;
    
    TRAPD(err,
	        ActivePalette2Utils::CreateDrawingObjectsL(EColor16MU, ActivePalette2Utils::APDimensionSize( EItemSize ),
											   &tempBitmap,
											   &tempDevice,
											   &tempGc));
											   
    if ( err != KErrNone )
        {
        return;
        }											   

	TRAPD(err2, ActivePalette2Utils::CreateDrawingObjectsL(EGray256,
		ActivePalette2Utils::APDimensionSize( EItemSize ),
		&tempMaskBitmap,
		&tempMaskDevice,
		&tempMaskGc));		

    if ( err2 != KErrNone )
        {
        return;
        }											   

#endif // PREPROCESS_BITMAPS
    
    if ( iModel->ShowTopScrollIndicator() )
        {
        iTopScrollVisual->iOpacity.Set(KOpacityFull, NAP2Cfg::K3dScrollIndicatorFadeTime);
        }
    else
        {
        iTopScrollVisual->iOpacity.Set(KOpacityNone, NAP2Cfg::K3dScrollIndicatorFadeTime);
        }
    
    
    TInt numItemsOnScreen = iModel->CountItemsOnScreen();

    numItemsOnScreen++;

    TPoint scrollOffset = CurrentScrollOffset();
    
    TInt i;
    for ( i = 0; i < numItemsOnScreen; i++ )
        {
		CActivePalette2Item* item = iModel->ItemFromScreenPos(i);
		
		if ( item )
		    {
            iItemLayout[i]->SetPos(THuiRealPoint(ItemPosition(i) + scrollOffset));
            
#ifdef PREPROCESS_BITMAPS
            tempGc->BitBlt(TPoint(0,0), &(item->Icon()));
            tempMaskGc->BitBlt(TPoint(0,0), item->Mask());
            TRAP_IGNORE(iItemTexture[i]->UploadL(*tempBitmap, tempMaskBitmap));
#else
            TRAP_IGNORE(iItemTexture[i]->UploadL(item->Icon(), item->Mask()));
#endif 
            iItemVisual[i]->iOpacity.Set(KOpacityFull);
            }
        }

    if ( iModel->ItemScrollOffset() == 0 )
        {
        iItemVisual[numItemsOnScreen-1]->iOpacity.Set(KOpacityNone);
        }
    else
        {
        ItemsScrolledSetItemOpacities();
        }


    for ( i = numItemsOnScreen; i < iItemVisual.Count(); i++ )
        {
        iItemVisual[i]->iOpacity.Set(KOpacityNone);
        }

    if ( iModel->ShowBottomScrollIndicator() )
        {
        iBottomScrollLayout->SetPos(THuiRealPoint(BottomScrollPanePosition() + ActivePalette2Utils::APDimensionPoint(E3dScrollIconOffset)));
        iBottomScrollVisual->iOpacity.Set(KOpacityFull, NAP2Cfg::K3dScrollIndicatorFadeTime);
        }
    else
        {
        iBottomScrollVisual->iOpacity.Set(KOpacityNone, NAP2Cfg::K3dScrollIndicatorFadeTime);
        }
    
    UpdateFocus();
    
#ifdef PREPROCESS_BITMAPS
	ActivePalette2Utils::DeleteDrawingObjects(&tempMaskBitmap,
											  &tempMaskDevice,
											  &tempMaskGc);
	ActivePalette2Utils::DeleteDrawingObjects(&tempBitmap,
											  &tempDevice,
											  &tempGc);
#endif											  
    }


// -----------------------------------------------------------------------------
// CActivePalette2HUI::OfferEventL()
// -----------------------------------------------------------------------------
//
TBool CActivePalette2HUI::OfferEventL(const THuiEvent& aEvent) 
    {
    TBool res = EFalse;
    
    if(aEvent.IsKeyEvent() && aEvent.iCode == EEventKey)
        {
        if (aEvent.iKeyEvent.iScanCode == iNavigationKeys.iNaviPrev)
	    	{ 
	    	// go to previous
            res = iModel->ProcessInputEvent(EAP2InputUp);
		    }
		else if (aEvent.iKeyEvent.iScanCode == iNavigationKeys.iNaviNext)
		    { 
		    // go to next
            res = iModel->ProcessInputEvent(EAP2InputDown);
		    }
    	else if (aEvent.iKeyEvent.iScanCode == iNavigationKeys.iNaviSelect)
		    { 
		    // select item
            res = iModel->ProcessInputEvent(EAP2InputSelect);
		    }
		else
		    {
		    // lint
		    }
        }

    return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::PalettePosition()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2HUI::PalettePosition() const
    {
    TInt totalFrames = Max(1, iModel->PaletteAppearingTotalFrames());
    TInt xOffset = ((KPaletteHiddenXPos - iTopLeftCorner.iX) * (totalFrames - iModel->PaletteAppearingCurrentFrame())) / totalFrames;
    return TPoint(iTopLeftCorner.iX + xOffset, iTopLeftCorner.iY);
    }


// -----------------------------------------------------------------------------
// CActivePalette2HUI::ItemsScrolledSetItemOpacities()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::ItemsScrolledSetItemOpacities()
    {
    TInt currentOffset = iModel->ItemScrollOffset();
    TInt totalFrames = Max(1, iModel->ItemScrollTotalFrames());
    
    iItemVisual[0]->iOpacity.Set(TReal(totalFrames - currentOffset) / totalFrames);
    iItemVisual[iModel->CountItemsOnScreen()]->iOpacity.Set(TReal(currentOffset) / totalFrames);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateFocus()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::AnimateFocus(TInt aCurrentPos, TInt aTargetPos, TInt aTimeInMilliseconds)
    {
    if ( iModel->PaletteActive()
         && iModel->CountItemsOnScreen() > 0 )
        {
        iFocusRingLayout->SetPos(THuiRealPoint(ItemPosition(aCurrentPos) + TPoint(ActivePalette2Utils::APDimension(EFocusRingXOffset), ActivePalette2Utils::APDimension(EFocusRingYOffset))));
        
        iFocusRingLayout->SetPos(THuiRealPoint(ItemPosition(aTargetPos) + TPoint(ActivePalette2Utils::APDimension(EFocusRingXOffset), ActivePalette2Utils::APDimension(EFocusRingYOffset))), aTimeInMilliseconds);

        iFocusRingVisual->iOpacity.Set(KOpacityFull);
        
        iGeneralTimer->Cancel();
    	iGeneralTimer->Start(
    		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
    		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
    		TCallBack(&CActivePalette2HUI::AnimateFocusTimerCallback,(TAny *)this));
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateFocusTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateFocusTimerCallback(TAny* aPtr)
    {
	CActivePalette2HUI* self = (CActivePalette2HUI*) aPtr;
	
	if ( self )
	    {
    	return self->AnimateFocusTimerComplete();
    	}
    else
        {
        return KCallbackFinished;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateFocusTimerComplete()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateFocusTimerComplete()
    {
    iGeneralTimer->Cancel();
    iModel->AnimateFocusRendererComplete();
    return KCallbackFinished;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::ItemPosition()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2HUI::ItemPosition(TInt aScreenPos) const
    {
    return TPoint(ActivePalette2Utils::APDimension(EItemXOffset), ActivePalette2Utils::APDimension(ETopSectionHeight) + aScreenPos * (ActivePalette2Utils::APDimension(EItemHeight) + ActivePalette2Utils::APDimension(EGapBetweenItems)));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::TopScrollPanePosition()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2HUI::TopScrollPanePosition() const
    {
    return TPoint(0, 0);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::BottomScrollPanePosition()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2HUI::BottomScrollPanePosition() const
    {
    TInt gaps = Max(0, iModel->CountItemsOnScreen() - 1);
    return TPoint(0, ActivePalette2Utils::APDimension(ETopSectionHeight) + iModel->CountItemsOnScreen() * ActivePalette2Utils::APDimension(EItemHeight) + gaps * ActivePalette2Utils::APDimension(EGapBetweenItems));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::PaletteSize()
// -----------------------------------------------------------------------------
//
TSize CActivePalette2HUI::PaletteSize() const
    {
    return TSize(ActivePalette2Utils::APDimension(EPaletteWidth), BottomScrollPanePosition().iY + ActivePalette2Utils::APDimension(EBottomSectionHeight));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::CurrentScrollOffset()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2HUI::CurrentScrollOffset() const
    {
    return TPoint(0, -((ActivePalette2Utils::APDimension(EItemHeight) + ActivePalette2Utils::APDimension(EGapBetweenItems)) * iModel->ItemScrollOffset())/Max(1, iModel->ItemScrollTotalFrames()));
    }

// -----------------------------------------------------------------------------
// CActivePalette2BasicUI::AnimateAppearance()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::AnimateAppearance(TBool aVisible, TInt aTimeInMilliseconds)
    {
    if ( aVisible )
        {
        iPaletteLayout->SetPos(THuiRealPoint(TPoint(iTopLeftCorner.iX, iTopLeftCorner.iY)), aTimeInMilliseconds);
        }
    else
        {
        iPaletteLayout->SetPos(THuiRealPoint(TPoint(KPaletteHiddenXPos, iTopLeftCorner.iY)), aTimeInMilliseconds);
        }
        
    iGeneralTimer->Cancel();
	iGeneralTimer->Start(
		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
		TCallBack(&CActivePalette2HUI::AnimateAppearanceTimerCallback,(TAny *)this));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateFocusTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateAppearanceTimerCallback(TAny* aPtr)
    {
	CActivePalette2HUI* self = (CActivePalette2HUI*) aPtr;
	
	if ( self )
	    {
    	return self->AnimateAppearanceTimerComplete();
    	}
    else
        {
        return KCallbackFinished;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateFocusTimerComplete()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateAppearanceTimerComplete()
    {
    iGeneralTimer->Cancel();
    iModel->AnimateAppearanceRendererComplete();
    return KCallbackFinished;
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::GuessCurrentAppearanceFrame()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::GuessCurrentAppearanceFrame()
    {
    if ( KPaletteHiddenXPos - iTopLeftCorner.iX == 0 )
        {
        return 0;
        }
    else
        {
        TInt currentPos = iPaletteLayout->Pos().Now().iX;
        return ((KPaletteHiddenXPos - currentPos) * iModel->PaletteAppearingTotalFrames()) / (KPaletteHiddenXPos - iTopLeftCorner.iX);
        }
    }


// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateItemScroll()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::AnimateItemScroll(TInt aDirection, TInt aTimeInMilliseconds)
    {

    TInt numItems = iModel->CountItemsOnScreen();
        
    TInt itemOffset = 0;
    if ( aDirection == 1 )
        {
        itemOffset = -1;
        }
                
    for( TInt i = 0; i <= numItems; i++)
        {
        iItemLayout[i]->SetPos(THuiRealPoint(ItemPosition(i + itemOffset)), aTimeInMilliseconds);
        }
        
    if ( aDirection > 0 )
        {
        iItemVisual[0]->iOpacity.Set(KOpacityNone, aTimeInMilliseconds);
        iItemVisual[numItems]->iOpacity.Set(KOpacityFull, aTimeInMilliseconds);
        }
    else
        {
        iItemVisual[0]->iOpacity.Set(KOpacityFull, aTimeInMilliseconds);
        iItemVisual[numItems]->iOpacity.Set(KOpacityNone, aTimeInMilliseconds);
        }   
    
    iGeneralTimer->Cancel();
	iGeneralTimer->Start(
		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
		TCallBack(&CActivePalette2HUI::AnimateItemScrollTimerCallback,(TAny *)this));
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateFocusTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateItemScrollTimerCallback(TAny* aPtr)
    {
	CActivePalette2HUI* self = (CActivePalette2HUI*) aPtr;
	
	if ( self )
	    {
    	return self->AnimateItemScrollTimerComplete();
    	}
    else
        {
        return KCallbackFinished;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateFocusTimerComplete()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateItemScrollTimerComplete()
    {
    iGeneralTimer->Cancel();
    iModel->AnimateItemScrollRendererComplete();
    return KCallbackFinished;
    }


// -----------------------------------------------------------------------------
// CActivePalette2HUI::TooltipUpdated()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::TooltipUpdated()
    {
    TRAP_IGNORE(iTooltipTextVisual->SetTextL(*(iModel->TooltipText())));
    TSize textSize = iTooltipTextVisual->TextExtents();
    return textSize.iWidth + ActivePalette2Utils::APDimension(ETooltipTail) + ActivePalette2Utils::APDimension(ETooltipNose); 
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateTooltip()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::AnimateTooltip(TBool aToShow, TInt aTimeInMilliseconds)
    {
    TInt startMultiplier = aToShow ? 0 : 1;
    TInt endMultiplier = aToShow ? 1 : 0;
    
    TPoint startPos(iTopLeftCorner + ItemPosition(iModel->TooltipScreenPosition()) + ActivePalette2Utils::APDimensionPoint(E3dTooltipOffset));
    TSize startSize(((iTooltipTextVisual->TextExtents().iWidth + ActivePalette2Utils::APDimension(ETooltipNose) + ActivePalette2Utils::APDimension(ETooltipTail)) * startMultiplier), ActivePalette2Utils::APDimension(ETooltipHeight));
    startPos.iX -= startSize.iWidth;
    
    iTooltipLayout->SetSize(THuiRealSize(startSize));
    iTooltipLayout->SetPos(THuiRealPoint(startPos));
    
    TPoint endPos(iTopLeftCorner + ItemPosition(iModel->TooltipScreenPosition()) + ActivePalette2Utils::APDimensionPoint(E3dTooltipOffset));
    TSize endSize(((iTooltipTextVisual->TextExtents().iWidth + ActivePalette2Utils::APDimension(ETooltipNose) + ActivePalette2Utils::APDimension(ETooltipTail)) * endMultiplier), ActivePalette2Utils::APDimension(ETooltipHeight));
    endPos.iX -= endSize.iWidth;

    iTooltipLayout->SetSize(THuiRealSize(endSize), aTimeInMilliseconds);
    iTooltipLayout->SetPos(THuiRealPoint(endPos), aTimeInMilliseconds);
    
    ShowTooltipUpdated();
    
    iGeneralTimer->Cancel();
	iGeneralTimer->Start(
		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
		aTimeInMilliseconds * KMicrosecondsPerMillisecond,
		TCallBack(&CActivePalette2HUI::AnimateTooltipTimerCallback,(TAny *)this));
   }
    
// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateTooltipTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateTooltipTimerCallback(TAny* aPtr)
    {
	CActivePalette2HUI* self = (CActivePalette2HUI*) aPtr;
	
	if ( self )
	    {
    	return self->AnimateTooltipTimerComplete();
    	}
    else
        {
        return KCallbackFinished;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::AnimateTooltipTimerComplete()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2HUI::AnimateTooltipTimerComplete()
    {
    iGeneralTimer->Cancel();
    iModel->AnimateTooltipRendererComplete();
    return KCallbackFinished;
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::VisualLayoutUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::VisualLayoutUpdated(CHuiVisual& /*aVisual*/)
    {
    UpdateTopLeftCornerWithScreenSize();
    iPaletteLayout->SetPos(THuiRealPoint(PalettePosition()));
    }

// -----------------------------------------------------------------------------
// CActivePalette2HUI::UpdateTopLeftCornerWithScreenSize()
// -----------------------------------------------------------------------------
//
void CActivePalette2HUI::UpdateTopLeftCornerWithScreenSize()
    {
	AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, iScreenSize );
	                        
    // This uses the same logic that MC Photos used. If in landscape mode or no
    // location specified in portrait mode then position the palette relative to
    // the top RH corner of the screen using the default settings
	if ( ( iScreenSize.iWidth > iScreenSize.iHeight ) ||
	     ( ! iLocated ) )
	  {	                        
	  iTopLeftCorner.iX = iScreenSize.iWidth + ActivePalette2Utils::APDimensionPoint(EDefault3dPaletteOffset).iX;
	  iTopLeftCorner.iY = ActivePalette2Utils::APDimensionPoint(EDefault3dPaletteOffset).iY;
      }
	}
