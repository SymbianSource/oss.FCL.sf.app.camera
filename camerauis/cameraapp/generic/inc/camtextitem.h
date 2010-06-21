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
* Description:  Class for rendering text based indicators on top of viewfinder
*
*/


#ifndef CAMTEXTITEM_H
#define CAMTEXTITEM_H

#include <e32std.h>
#include <e32base.h>
#include <AknUtils.h>

class CFbsBitmap;
class TAknTextComponentLayout;
class CBitmapContext;

/**
 *  Class for rendering text based indicators on top of viewfinder.
 *
 *  CCamTextItem can be used to render text on the screen in a way
 *  that is compatible with S60 Posting based overlays. The text mask
 *  is also written in the alpha channel of the screen buffer to allow
 *  it to be overlaid on top of the viewfinder.
 *
 *  @since S60 v5.0
 */
class CCamTextItem : public CBase
    {
public:
    /**
     * Two-phased constructor.
     */
    static CCamTextItem* NewL();

    /**
    * Destructor.
    */
    virtual ~CCamTextItem();

public:
    /**
     * Set the layout for the text item.
     * 
     * @since S60 v5.0
     * @param aParentRect Rectangle for the parent layout item
     * @param aLayout     A TAknTextComponentLayout containing the layout
     *                    details for this item. Usually this can be retrieved
     *                    using the AknLayout2 API.
     */
    void SetLayoutL( const TRect& aParentRect, const TAknTextComponentLayout& aLayout );

    /**
     * Set the text content for the text item.
     *
     * @since S60 v5.0
     * @param aText New text for this item. There are no size limitations. A
     *              local copy if the text is created
     */
    void SetTextL( const TDesC& aText );

    /**
     * Render this text in a bitmap based graphics context.
     *
     * @since S60 v5.0
     * @param aBitmapContext Graphics context to draw into.
     */
    void Draw( CBitmapContext& aBitmapContext ) const;
    
    /**
     * Return the rect of the textitem
     *
     */
    TRect Rect();

private:
    
    CCamTextItem();

    void ConstructL();

private: // data

    TAknLayoutText iLayout;

    // own
    HBufC* iText;

    };

#endif // CAMTEXTITEM_H
