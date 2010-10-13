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
* Description:  Implemantation for CCamLineVfGridDrawer class.
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/


#include <gdi.h>
#include "camlinevfgriddrawer.h"
#include "camlogging.h"

// ======== LOCAL CONSTANTS ========

namespace
  {
  const TInt KArrayGranularity =   8;
  const TInt KMaxLineCount     =  64;

  const TInt KPenColorRed   = 255;
  const TInt KPenColorGreen =   0;//255;
  const TInt KPenColorBlue  =   0;//255;
  const TInt KPenColorAlpha = 255;//128;
  const TInt KPenWidth      =   2;
  const TInt KPenHeight     =   2;
  const CGraphicsContext::TPenStyle KPenStyle = CGraphicsContext::ESolidPen;  
  }
  
// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ===========================================================================
// Public part

// ---------------------------------------------------------------------------
// static NewL
//
// 2-phased constructor
// ---------------------------------------------------------------------------
//
CCamLineVfGridDrawer* 
CCamLineVfGridDrawer::NewL()
  {
  CCamLineVfGridDrawer* self = new (ELeave) CCamLineVfGridDrawer;
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );
  return self;
  }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamLineVfGridDrawer::~CCamLineVfGridDrawer()
  {
  PRINT( _L("Camera => ~CCamLineVfGridDrawer") );
  iLineArray.Reset();
  iLineArray.Close();
  PRINT( _L("Camera <= ~CCamLineVfGridDrawer") );
  }
    


// ---------------------------------------------------------------------------
// virtual InitL
// * from MCamVfGridDrawer
// 
// Initialize the drawer.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::InitL( TAny* /*aParam*/ )
  {
  }

// ---------------------------------------------------------------------------
// virtual Draw( CBitmapContext& )
//   * from MCamVfGridDrawer
// 
// Draw the grid. If not set visible nothing is drawn.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::Draw( CBitmapContext& aGc ) const
  {
  if( iVisible &&   
      iLineArray.Count() > 0 )
    {
    // Set drawing properties.
    aGc.SetPenStyle( iPenStyle );
    aGc.SetPenSize ( iPenSize  );
    aGc.SetPenColor( iPenColor );

    // Draw the lines
    for( TInt i = 0; i < iLineArray.Count(); i++ )
      {
      // Top-left and bottom-right corners of the rect 
      // represent the line ends.
      aGc.DrawLine( iLineArray[i].iTl, iLineArray[i].iBr );
      }

    // Clear any settings made to gc
    aGc.Reset();
    }
  else
    {
    // Not visible => draw nothing.
    }
  }


// ---------------------------------------------------------------------------
// virtual Draw( const TRect&, CBitmapContext& )
//   * from MCamVfGridDrawer
// 
// Draw part of the grid. If not set visible nothing is drawn.
// Note:
//   Only pure vertical and pure horizontal lines
//   get drawn to right place if lines expand outside
//   the aRect rectangle given here as parameter.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::Draw( const TRect&          aRect,
                                  CBitmapContext& aGc   ) const
  {
  if( iVisible &&   
      iLineArray.Count() > 0 )
    {
    // Limit the drawing to given rectangle.
    // Drawing outside this rectangle has no visible effect.
    aGc.SetClippingRect( aRect );
    // Make the same drawing operations as in full screen draw.
    // The drawing is so simple that any optimization is
    // probably more time consuming than the drawing itself.
    // Draw resets the gc in the end, so clipping rect is cleared.
    Draw( aGc );
    }
  else
    {
    // Not visible => draw nothing.
    }
  }

// ---------------------------------------------------------------------------
// virtual SetVisible
// * from MCamVfGridDrawer
// 
// Set visibility of the grid.
// 
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::SetVisible( TBool aVisible )
  {
  iVisible = aVisible;
  }



// ---------------------------------------------------------------------------
// virtual IsVisible
// * from MCamVfGridDrawer
// 
// Get visibility of the grid.
// 
// ---------------------------------------------------------------------------
//
TBool
CCamLineVfGridDrawer::IsVisible() const
  {
  return iVisible;
  }


// ---------------------------------------------------------------------------
// SetLinesL
// * from MCamVfGridDrawer
// 
// Set the line end pair array.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::SetLinesL( const RLineArray& aLineArray )
  {
  TInt newLineCount = aLineArray.Count();
  if( newLineCount > KMaxLineCount )
    {
    User::Leave( KErrArgument );
    }
  
  iLineArray.Reset();
  for( TInt i = 0; i < newLineCount; i++ )
    {
    iLineArray.Append( aLineArray[i] );
    }
  }


// ---------------------------------------------------------------------------
// SetLinesL
// 
// Set the line end pair array by defining the count and bordered property
// of lines per horizontal and vertical axis.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::SetLinesL( const TRect& aDrawingRect, 
                                       TInt   aHorizontalLines, 
                                       TInt   aVerticalLines,
                                       TBool  aBorderedGrid /* = EFalse */)
  {
  // -------------------------------------------------------
  // Check the max limit for lines does not exceed.
  if( aHorizontalLines < 0
   || aVerticalLines   < 0
   || aHorizontalLines + aVerticalLines > KMaxLineCount
    )
    {
    User::Leave( KErrArgument );
    }

  // -------------------------------------------------------
  // Fill the line end array
  //
  // The lines are evenly spaced over the draw area axis in question.  
  // The aBorderedGrid parameter determines whether the grid 
  // draws lines to the borders of draw area:
  // E.g. 3 vertical lines: '[' and ']' are the draw area borders.
  //  aBorderedGrid: [|    |    |]
  // !aBorderedGrid: [  |  |  |  ]  
  //
  // The formula for the location of i:th line when N lines
  // are used along dimension D:
  // -  aBorderedGrid: (  i   * D) / (N - 1) 
  // - !aBorderedGrid: ((i+1) * D) / (N + 1) 
  // * Divide by (N-1) needs special handling with N=1.
  // * Half of pen width is subtracted from the above formula result
  //   to center the line to the calculated location.
  // * odd area or pen width/height may cause error of 1 pixel.
  TInt indexOffset = (aBorderedGrid ?  0 : 1);
  TInt countOffset = (aBorderedGrid ? -1 : 1);

  // -----------------------------------
  // First horizontal lines
  TInt height  = aDrawingRect.Height();
  TInt leftX   = aDrawingRect.iTl.iX;
  TInt rightX  = aDrawingRect.iBr.iX;

  TInt i = 0;
  while( i < aHorizontalLines )
    {
    // Max used to make sure no div-by-zero
    TInt currentY = ((i+indexOffset)*height) / Max(1,aHorizontalLines+countOffset) - KPenHeight/2;
    iLineArray.Append( TRect( leftX, currentY, rightX, currentY ) );
    i++;
    }

  // -----------------------------------
  // Then vertical lines
  TInt width   = aDrawingRect.Width();
  TInt topY    = aDrawingRect.iTl.iY;
  TInt bottomY = aDrawingRect.iBr.iY;

  TInt j = 0;
  while( j < aVerticalLines )
    {
    // Max used to make sure no div-by-zero
    TInt currentX = leftX + ((j+indexOffset)*width) / Max(1,aVerticalLines+countOffset) - KPenWidth/2;
    iLineArray.Append( TRect( currentX, topY, currentX, bottomY ) );
    j++;
    }
  // -------------------------------------------------------
  }


// ---------------------------------------------------------------------------
// SetPenStyle
// 
// Set the style of drawing pen.
// See CGraphicsContext.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::SetPenStyle( const CGraphicsContext::TPenStyle& aPenStyle )
  {
  iPenStyle = aPenStyle;
  }


// ---------------------------------------------------------------------------
// SetPenSize
// 
// Set the size of drawing pen.
// See CGraphicsContext.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::SetPenSize( const TSize& aPenSize )
  {
  iPenSize = aPenSize;
  }

// ---------------------------------------------------------------------------
// SetPenColor
// 
// Set the color of drawing pen.
// See CGraphicsContext.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::SetPenColor( const TRgb aPenColor )
  {
  iPenColor = aPenColor;
  }


// ===========================================================================
// Protected part

// ---------------------------------------------------------------------------
// ConstructL
// 
// 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void 
CCamLineVfGridDrawer::ConstructL()
  {
  // empty
  }



// ===========================================================================
// Private part

// ---------------------------------------------------------------------------
// C++ constructor.
// Called in 1st phase of construction.
// ---------------------------------------------------------------------------
//
CCamLineVfGridDrawer::CCamLineVfGridDrawer()
  : iLineArray( KArrayGranularity ),
    iVisible( EFalse ),
    iPenStyle( KPenStyle ),
    iPenSize( KPenWidth, KPenHeight ),
    iPenColor( KPenColorRed, KPenColorGreen, KPenColorBlue, KPenColorAlpha )
  {
  // empty
  }


// ============================== end of file ================================
