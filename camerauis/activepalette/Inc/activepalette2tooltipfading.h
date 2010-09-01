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
* Description:  Active Palette 2 Tooltip that fades in and out
*
*/


#ifndef ACTIVE_PALETTE_2_TOOLTIP_FADING_H
#define ACTIVE_PALETTE_2_TOOLTIP_FADING_H

// ===========================================================================
// Includes
#include "ActivePalette2Tooltip.h"


// ===========================================================================
// Forward declarations
class CActivePalette2Model;
class CActivePalette2BasicUI;
class CActivePalette2Styler;


// ===========================================================================
// Class declarations
class CActivePalette2TooltipFading : public CActivePalette2Tooltip
  {    
  // =======================================================
  // Methods

  // -------------------------------------------------------
  // Constructors and destructor
  public:
  
    static CActivePalette2TooltipFading* NewL( const CActivePalette2BasicUI*  aParent,
                                                     CActivePalette2Model*  aModel,
                                                     CActivePalette2Styler* aStyler );
    virtual ~CActivePalette2TooltipFading();

  protected: 
 
    void ConstructL( const CActivePalette2BasicUI* aParent );

    CActivePalette2TooltipFading( CActivePalette2Model*  aModel,
                                  CActivePalette2Styler* aStyler );

  // -------------------------------------------------------
  // From CActivePalette2Tooltip
  public: 
  
    /**
    * Draw the tooltip.
    */
    virtual void ProduceTooltipToScreen( TBool aActivateGc ) const;

  protected: 

    /**
    * Current tooltip rectangle.
    */
    virtual TRect TooltipRect() const;

  // =======================================================
  // Data
  protected:

  // =======================================================
  };

#endif // ACTIVE_PALETTE_2_TOOLTIP_FADING_H

// ===========================================================================
// end of file
