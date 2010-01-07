/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for rendering text based indicators on top of viewfinder
*
*/


#include "camtextitem.h"
#include "AknLayout2ScalableDef.h"
#include "AknLayoutFont.h"
#include "CamUtility.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamTextItem::CCamTextItem()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamTextItem::~CCamTextItem()
    {
    delete iText;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamTextItem* CCamTextItem::NewL()
    {
    CCamTextItem* self = new ( ELeave ) CCamTextItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamTextItem::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamTextItem::SetLayoutL(
        const TRect& aParentRect,
        const TAknTextComponentLayout& aLayout )
    {
    const CFont* layoutFont = AknLayoutUtils::FontFromId(aLayout.LayoutLine().FontId(), 0);
    layoutFont->FontSpecInTwips().iFontStyle.SetEffects(FontEffect::EOutline,ETrue);
    layoutFont->FontSpecInTwips().iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    iLayout.LayoutText( aParentRect, aLayout.LayoutLine(), layoutFont );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamTextItem::SetTextL( const TDesC& aText )
    {
    if ( !iText || aText.Compare( *iText ) )
        {
        delete iText;
        iText = NULL;
        iText = aText.AllocL();
        TPtr textPtr = iText->Des();
        AknTextUtils::LanguageSpecificNumberConversion( textPtr );
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamTextItem::Draw( CBitmapContext& aBitmapContext ) const
    {
    if ( iText )
        {
        aBitmapContext.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        iLayout.DrawText(
                aBitmapContext,
                *iText,
                ETrue,
                KRgbBlack);
        }
    }

