/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for rendering bitmap based indicators
*
*/


#include "cambitmapitem.h"
#include "AknLayout2ScalableDef.h"
#include "AknLayoutFont.h"
#include "CamUtility.h"
#include "CamPanic.h"  

#include <AknIconUtils.h>
#include <AknsUtils.h>
#include <AknsConstants.h>

#include <cameraapp.mbg> 

_LIT( KCamBitmapFile, "z:\\resource\\apps\\cameraapp.mif" );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamBitmapItem::CCamBitmapItem()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamBitmapItem::~CCamBitmapItem()
    {
    delete iBitmap;
    delete iMask;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamBitmapItem* CCamBitmapItem::NewL(
        TInt aBitmapId, 
        TInt aMaskId )
    {
    CCamBitmapItem* self = new ( ELeave ) CCamBitmapItem();
    CleanupStack::PushL( self );
    self->ConstructL( aBitmapId, aMaskId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamBitmapItem* CCamBitmapItem::NewL(
        TInt aBitmapId,
        TInt aMaskId,
        const TAknsItemID& aID )
    {
    CCamBitmapItem* self = new ( ELeave ) CCamBitmapItem();
    CleanupStack::PushL( self );
    self->ConstructL( aBitmapId, aMaskId, aID );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamBitmapItem::ConstructL( TInt aBitmapId, TInt aMaskId )
    {
    AknIconUtils::CreateIconL(
        iBitmap,
        iMask,
        KCamBitmapFile,
        aBitmapId,
        aMaskId );

    // If SizeInPixels() return zero size, this is a scalable icon
    iScalableIcon = iBitmap->SizeInPixels() == TSize( 0, 0 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamBitmapItem::ConstructL(
    TInt aBitmapId, TInt aMaskId, const TAknsItemID& aID )
    {
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    
    AknsUtils::CreateIconL(
        skinInstance, 
        aID,
        iBitmap,
        iMask,
        KCamBitmapFile,
        aBitmapId,
        aMaskId );

    // If SizeInPixels() return zero size, this is a scalable icon
    iScalableIcon = iBitmap->SizeInPixels() == TSize( 0, 0 );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamBitmapItem::SetLayoutL(
        const TRect& aParentRect,
        const TAknWindowComponentLayout& aLayout )
    {
    TAknLayoutRect layout;
    layout.LayoutRect( aParentRect, aLayout.LayoutLine() );
    SetLayoutL( layout.Rect() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamBitmapItem::SetLayoutL(
        const TRect& aRect )
    {
    if ( iRect != aRect )
        {
        iRect = aRect;
		if ( iBitmap && iScalableIcon )
            {
            AknIconUtils::SetSize( iBitmap, iRect.Size() );
            }
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamBitmapItem::Draw( CBitmapContext& aBitmapContext ) const
    {
    TRect cropRect(TPoint(0,0),iRect.Size());
    DrawPartial( aBitmapContext, iRect, cropRect );
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamBitmapItem::DrawPartial(
    CBitmapContext& aBitmapContext,
    const TRect& aDestRect,
    const TRect& aCropRect ) const
    {
    if ( iBitmap && iMask )
        {
        // Alpha channel based rendering
        aBitmapContext.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        aBitmapContext.BitBltMasked(
            aDestRect.iTl + aCropRect.iTl,
            iBitmap,
            aCropRect,
            iMask,
            EFalse);
        }
    else
        {
        // Do nothing
        }
    }

// ---------------------------------------------------------------------------
// CCamBitmapItem::BitmapSize
// ---------------------------------------------------------------------------
//
TSize CCamBitmapItem::BitmapSize() const
    {
    if ( iBitmap )
        {
        return iBitmap->SizeInPixels();
        }
    else
        {
        return iRect.Size();
        }
    }

// ---------------------------------------------------------------------------
// CCamBitmapItem::LayoutRect
// ---------------------------------------------------------------------------
//
TRect CCamBitmapItem::LayoutRect() const
    {
    return iRect;
    }

// end of file
