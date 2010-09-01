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
* Description:  Draws a list item.*
*/


// INCLUDE FILES
#include	<AknsDrawUtils.h>
#include  <AknUtils.h>
#include  <barsread.h>
#include  <AknsFrameBackgroundControlContext.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include	"CamCaptureSetupMenuListItemDrawer.h"
#include	"CamCaptureSetupMenuListBoxModel.h"

#include "camlogging.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// NewL
// Two-phased constructor.
// Returns: CCamCaptureSetupMenuListItemDrawer*: Pointer to the created list item drawer.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupMenuListItemDrawer* CCamCaptureSetupMenuListItemDrawer::NewL(
    CCamCaptureSetupMenuListBoxModel* aListBoxModel, // used to get content information about list item being drawn.
    TResourceReader& aReader ) // used to read layout information about list items to be drawn.
    {
    CCamCaptureSetupMenuListItemDrawer* self = 
        new (ELeave) CCamCaptureSetupMenuListItemDrawer( aListBoxModel );
    CleanupStack::PushL( self );
    self->ConstructFromResourceL( aReader );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CCamCaptureSetupMenuListItemDrawer::DrawActualItem
// Draws item and highlights if needed
// ---------------------------------------------------------
//
void CCamCaptureSetupMenuListItemDrawer::DrawActualItem( 
    TInt aItemIndex,                // index of item
    const TRect& aActualItemRect,   // rectangular area of item
    TBool aItemIsCurrent,           // ETrue if current item
    TBool /*aViewIsEmphasized*/,        // ETrue if emphasized
	TBool /*aViewIsDimmed*/,        // ETrue if dimmed
    TBool /*aItemIsSelected*/ ) const   // ETrue if selected
	{
    // Draw unhighlighted rectangle that encapsulates the item text and bitmap.
	DrawItemRect( aActualItemRect );       

    // ...if the item is the current item highlight it.
	if ( aItemIsCurrent )
		{
		DrawHighlightedItemRect( aActualItemRect );
		}

    // Draw the text.
    // ...Create a text layout object for drawing the text
    // ...inside of the list item's rectangle.
    TAknLayoutText layoutText;
    layoutText.LayoutText( aActualItemRect, iLayoutDataForText );
    // get the text color from the skin
    TRgb textColor;
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
    // default is list item text colour
    TAknsQsnTextColorsIndex index = EAknsCIQsnTextColorsCG7;
    if( Flags() & CListItemDrawer::ESingleClickDisabledHighlight )
        {
        aItemIsCurrent = EFalse;
        }
    if ( aItemIsCurrent )
		{
        index = EAknsCIQsnTextColorsCG10; // list highlight text
        }
    AknsUtils::GetCachedColor( skin, textColor, KAknsIIDQsnTextColors, index );
    // ...Pass the text to be drawn, into the text layout object
    // ...and draw it.
    const TDesC& itemText = iModel->TextForItem( aItemIndex );
    layoutText.DrawText( *iGc, itemText, ETrue, textColor );

	if ( !iModel->ItemHasIconText( aItemIndex ) )
		{
		// Draw the bitmap.
		// Create a rect layout object for drawing the bitmap
		//  inside of the list item's rectangle.
		TAknLayoutRect layoutRect;
		layoutRect.LayoutRect( aActualItemRect, iLayoutDataForBitmap );
		// Pass the bitmap to be drawn, into the rect layout object and draw it.
		CFbsBitmap* bitmap = iModel->BitmapForItem( aItemIndex, EFalse );
		CFbsBitmap* mask = iModel->BitmapForItem( aItemIndex, ETrue );
    
		AknIconUtils::SetSize( bitmap, TSize( iLayoutDataForBitmap.iW, 
											  iLayoutDataForBitmap.iH ),
											  EAspectRatioPreserved );

		layoutRect.DrawImage( *iGc, bitmap, mask );
		}
	else
		{
		const TDesC& iconText = iModel->IconTextForItem( aItemIndex );
		layoutText.LayoutText( aActualItemRect, iLayoutDataForIconText );
	    layoutText.DrawText( *iGc, iconText, ETrue, textColor );
		}
	}

// ---------------------------------------------------------
// CCamCaptureSetupMenuListItemDrawer::CCamCaptureSetupMenuListItemDrawer
// Constructor
// ---------------------------------------------------------
//
CCamCaptureSetupMenuListItemDrawer::CCamCaptureSetupMenuListItemDrawer( 
    CCamCaptureSetupMenuListBoxModel* aListBoxModel ) // used to get information about item that drawing.
	:iModel( aListBoxModel )
	{
	}

// ---------------------------------------------------------
// CCamCaptureSetupMenuListItemDrawer::~CCamCaptureSetupMenuListItemDrawer
// Destructor
// ---------------------------------------------------------
//
CCamCaptureSetupMenuListItemDrawer::~CCamCaptureSetupMenuListItemDrawer()
    {
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListItemDrawer::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------
//
void CCamCaptureSetupMenuListItemDrawer::ConstructFromResourceL
( TResourceReader& aReader )
    {
    // Read the text layout from resource.
    iLayoutDataForText.iFont = aReader.ReadInt16();
    iLayoutDataForText.iC = aReader.ReadInt16();
    iLayoutDataForText.iL = aReader.ReadInt16();
    iLayoutDataForText.iR = aReader.ReadInt16();
    iLayoutDataForText.iB = aReader.ReadInt16();
    iLayoutDataForText.iW = aReader.ReadInt16();
    iLayoutDataForText.iJ = aReader.ReadInt16();

    // Read the text layout from resource.
    iLayoutDataForBitmap.iC = aReader.ReadInt16();
    iLayoutDataForBitmap.iL = aReader.ReadInt16();
    iLayoutDataForBitmap.iT = aReader.ReadInt16();
    iLayoutDataForBitmap.iR = aReader.ReadInt16();
    iLayoutDataForBitmap.iB = aReader.ReadInt16();
    iLayoutDataForBitmap.iW = aReader.ReadInt16();
    iLayoutDataForBitmap.iH = aReader.ReadInt16();

    // Read the icon text layout from resource.
    iLayoutDataForIconText.iFont = aReader.ReadInt16();
    iLayoutDataForIconText.iC = aReader.ReadInt16();
    iLayoutDataForIconText.iL = aReader.ReadInt16();
    iLayoutDataForIconText.iR = aReader.ReadInt16();
    iLayoutDataForIconText.iB = aReader.ReadInt16();
    iLayoutDataForIconText.iW = aReader.ReadInt16();
    iLayoutDataForIconText.iJ = aReader.ReadInt16();

    // Read the highlight offset amount from resource.
    iHighlightOffset = aReader.ReadInt16();
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListItemDrawer::DrawHighlightedRect
// Draws a highlighted rectangle
// ---------------------------------------------------------
//
void CCamCaptureSetupMenuListItemDrawer::DrawHighlightedItemRect( 
    const TRect& aActualItemRect ) // the rectangular area to be highlighted
    const
	{
    if( Flags() & CListItemDrawer::ESingleClickDisabledHighlight )
        {
        return;
        }

	MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TRect innerRect( aActualItemRect );
    innerRect.Shrink( iHighlightOffset, iHighlightOffset );
    AknsDrawUtils::DrawFrame( skin, *iGc, aActualItemRect, innerRect, 
                              KAknsIIDQsnFrList, KAknsIIDDefault );
	}


// ---------------------------------------------------------
// CCamCaptureSetupMenuListItemDrawer::DrawItemRect
// Draws a rectangle for an item.
// ---------------------------------------------------------
//
void CCamCaptureSetupMenuListItemDrawer::DrawItemRect( 
    const TRect& aActualItemRect ) // the rectangular area to be drawn
    const
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext *cc = AknsDrawUtils::ControlContext( iParentControl ); 
    AknsDrawUtils::Background( skin, cc, iParentControl, *iGc, aActualItemRect );
#if 0
	iGc->SetBrushColor( KRgbRed );
	iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
	iGc->SetPenStyle( CGraphicsContext::ENullPen );	// shadow drawn later
    iGc->DrawRect( aActualItemRect );
#endif
    }


// ---------------------------------------------------------
// CCamCaptureSetupMenuListItemDrawer::SetControl
// Sets the parent control of the listbox
// ---------------------------------------------------------
//
void CCamCaptureSetupMenuListItemDrawer::SetParentControl( 
    const CCoeControl* aControl )
    {
    iParentControl = aControl;
    }

//End of file


