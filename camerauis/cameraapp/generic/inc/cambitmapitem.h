/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CAMBITMAPITEM_H
#define CAMBITMAPITEM_H

#include <e32std.h>
#include <e32base.h>
#include <AknUtils.h>

class CFbsBitmap;
class TAknTextComponentLayout;
class CBitmapContext;

/**
 *  Class for rendering bitmap based indicators on top of viewfinder.
 *
 *  CCamBitmapItem can be used to draw bitmaps on the screen in a way
 *  that is compatible with S60 Posting based overlays. The mask
 *  is also written in the alpha channel of the screen buffer to allow
 *  it to be overlaid on top of the viewfinder.
 *
 *  @since S60 v5.0
 */
class CCamBitmapItem : public CBase
    {
public:
    /**
     * Two-phased constructor for non-skinned bitmaps.
     */
    static CCamBitmapItem* NewL(
            TInt aBitmapId, 
            TInt aMaskId );

    /**
     * Two-phased constructor for skinnable bitmaps.
     */
    static CCamBitmapItem* NewL(
            TInt aBitmapId,
            TInt aMaskId,
            const TAknsItemID& aID );
 
    /**
    * Destructor.
    */
    virtual ~CCamBitmapItem();

public:
    /**
     * Set the layout for the item.
     * 
     * @since S60 v5.0
     * @param aParentRect Rectangle for the parent layout item
     * @param aLayout     A TAknWindowComponentLayout containing the layout
     *                    details for this item. Usually this can be retrieved
     *                    using the AknLayout2 API.
     */
    void SetLayoutL(
            const TRect& aParentRect,
            const TAknWindowComponentLayout& aLayout );

    /**
     * Set the layout for the item.
     * 
     * @since S60 v5.0
     */
    void SetLayoutL(
            const TRect& aRect );
    
    /**
     * Render this item in a bitmap based graphics context.
     *
     * @since S60 v5.0
     * @param aBitmapContext Graphics context to draw into.
     */
    void Draw( CBitmapContext& aBitmapContext ) const;

    /**
     * Render a part of this item in a bitmap based graphics context.
     *
     * @since S60 v5.0
     * @param aBitmapContext Graphics context to draw into.
	 * @param aDestRect 	 Destination rect on the context
     * @param aCropRect      Crop rectangle specifying which part of the
     *                       bitmap should be drawn.
     */
    void DrawPartial( 
            CBitmapContext& aBitmapContext,
            const TRect& aDestRect,
            const TRect& aCropRect ) const;

    /**
     * Get the size of the bitmap in pixels.
     * 
     * @return Bitmap size in pixels (or size from layout if bitmap not
     *         available).
     */
    TSize BitmapSize() const;
    
    /**
     * Get the layout rect of the bitmap in pixels.
     * 
     * @return Layout Ret in pixels
     */    
    TRect LayoutRect() const;
    
private:
    
    CCamBitmapItem();

    void ConstructL(
            TInt aBitmapId,
            TInt aMaskId );

    void ConstructL(
            TInt aBitmapId,
            TInt aMaskId,
            const TAknsItemID& aID );

private: // data

    // own
    CFbsBitmap* iBitmap;

    // own
    CFbsBitmap* iMask;

    TRect iRect;

    TBool iScalableIcon;
    };

#endif // CAMTEXTITEM_H
