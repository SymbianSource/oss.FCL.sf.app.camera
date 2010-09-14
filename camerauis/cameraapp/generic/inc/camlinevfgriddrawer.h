/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for CCamLineVfGridDrawer class.
*
*/



#ifndef CAMLINEDRAWVFGRID_H
#define CAMLINEDRAWVFGRID_H

#include <gdi.h> // TRgb
#include "camvfgridinterface.h"

const TInt KCamLineDrawVfGridUid = 0x07700770;


class CBitmapContext;
/**
*
* CCamLineVfGridDrawer class.
*
*/
class CCamLineVfGridDrawer : public CBase,
                             public MCamVfGridDrawer
  {
  // ------------------------------------------------------------------------
  // Public part
  public:

    typedef RArray<TRect> RLineArray;

  public: // Construction / destruction --------------------

    static CCamLineVfGridDrawer* NewL();

    virtual ~CCamLineVfGridDrawer();
    
  public: // From MCamVFGridDrawer -------------------------

    /**
    * Init the drawer.
    * @param aParam Not used.
    */
    virtual void  InitL( TAny* aParam );

    /**
    * Draw the full grid.
    */
    virtual void  Draw( CBitmapContext& aGc ) const;
   
    /**
    * Draw part of the grid.
    * Only pure horizontal and pure vertical lines are
    * supported if partial redraw is used.
    */
    virtual void  Draw( const TRect&          aRect,
                              CBitmapContext& aGc   ) const;

    /**
    * Set the grid visibility. 
    * If visibility is set to false, drawing is not done.
    * @param aVisible Is the grid visible.
    */
    virtual void  SetVisible( TBool aVisible );
    /**
    * Get the grid visibility setting.
    * @return Is grid visible.
    */
    virtual TBool IsVisible() const;

  public: // New methods

    /**
    * SetLines
    * @param aLineArray     Pointer to array containing the new lines
    *                       to be used when drawing VF grid.
    *                       TRect::iTL and TRect::iTL define the end
    *                       points of a line.
    * @param aLineCount     Line count is array. Ignored if aLineArray is NULL.
    * @param aTakeOwnerShip Is ownership of aLineArray transferred.
    *     
    */
    void SetLinesL( const RLineArray& aLineArray );
                   
    void SetLinesL( const TRect& aDrawingRect, 
                          TInt   aHorizontalLines, 
                          TInt   aVerticalLines,
                          TBool  aBorderedGrid = EFalse );
    
    void SetPenStyle( const CGraphicsContext::TPenStyle& aPenStyle ); 
    void SetPenSize ( const TSize& aPenSize );
    void SetPenColor( const TRgb aPenColor );

  // ------------------------------------------------------------------------
  // Protected part
  protected:

    void ConstructL();

  // ------------------------------------------------------------------------
  // Private part
  private:

    CCamLineVfGridDrawer();

  private: // Data

    // Line properties
    RLineArray iLineArray;      // Line end locations
    TBool      iVisible;        // Visibility of lines

    // Drawing parameters
    CGraphicsContext::TPenStyle iPenStyle;
    TSize                       iPenSize;
    TRgb                        iPenColor;

  };

#endif