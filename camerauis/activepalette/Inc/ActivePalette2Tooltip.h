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
* Description:  Active Palette Tooltip interface*
*/


/**
 * @file ActivePalette2Tooltip.h
 * Active Palette Tooltip interface
 */

#ifndef _ACTIVE_PALETTE_2_TOOLTIP_H
#define _ACTIVE_PALETTE_2_TOOLTIP_H

//  INCLUDES
#include <e32std.h>
#include <coecntrl.h>
#include <activepalette2factory.h>

#include "ActivePalette2Const.h"

// FORWARD DECLARATIONS
class CActivePalette2Model;
class CActivePalette2BasicUI;
class CActivePalette2Styler;

// CLASS DECLARATIONS

/**
 * 2D tooltip renderer for Active Palette 2.
 */
class CActivePalette2Tooltip : public CCoeControl
  {
  public:

    /** 
    * 2-stage construction
    * @param aParent Parent renderer
    * @param aModel Model driving the animation
    * @param aStyler Rendering styler to use
    */
    static CActivePalette2Tooltip * NewL( const CActivePalette2BasicUI*  aParent,
                                                CActivePalette2Model*  aModel,
                                                CActivePalette2Styler* aStyler );
                     
    /** 
    * Destructor
    */
    virtual ~CActivePalette2Tooltip();

  public: // From CCoeControl

    /** 
    * Draw's the tooltip within the specified rectangle
    * @param aRect The rectangle to draw within
    */
    virtual void Draw( const TRect& aRect ) const;

  public: // New methods

    /**
    * Sets the on-screen postion of the AP. The tooltip is drawn relative to this point.
    * @param aAnchor The AP position
    */
    virtual void SetAnchor( TPoint aAnchor );

    /**
    * Called when a new tooltip is first required
    * @return Total length, in pixels, of tooltip
    */
    virtual TInt TooltipUpdated();
    
    /**
    * Called when tooltip animation requires an update
    */
    virtual void TooltipAnimated();
    
    /**
    * Sets the graphics context to draw to.
    * @param aGc The graphics context to draw to
    */
    virtual void SetGc( CBitmapContext* aGc = NULL );
    
    /** 
    * Draw the pre-rendered tooltip to the screen
    */
    virtual void ProduceTooltipToScreen( TBool aActivateGc ) const;

  protected:

    /**
    * Returns where the tooltip should currently be on-screen
    * @return Where the tooltip should currently be on-screen
    */
    virtual TRect TooltipRect() const;

  protected: // Methods 

    /** 
    * Constructor
    * @param aModel Model driving the animation
    * @param aStyler Rendering styler to use
    */
    CActivePalette2Tooltip( CActivePalette2Model*  aModel,
                            CActivePalette2Styler* aStyler );

    /** 
    * Leaving constructor
    */
    void ConstructL( const CActivePalette2BasicUI* aParent );

  protected: // Data

    /// The position of the parent AP
    TPoint              iAnchor;
    
    /// The controlling model. Not owned.     
    CActivePalette2Model*           iModel;
    
    /// The parent renderer. Not owned.
    const CActivePalette2BasicUI*           iParent;
    
    /// The rendering styler to use.        
    CActivePalette2Styler* iStyler;
    
    /// The context to render to. If NULL (default), the screen is used. Not owned
    CBitmapContext* iRenderGc;
    
    /// Where the palette was last rendered
    mutable TRect iRenderedRect;
  };

#endif // _ACTIVE_PALETTE_2_TOOLTIP_H

// End of File
