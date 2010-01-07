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
* Description:  Active Palette 2 Tooltip Factory class
*
*/


#ifndef ACTIVE_PALETTE_2_TOOLTIP_FACTORY_H
#define ACTIVE_PALETTE_2_TOOLTIP_FACTORY_H


// ===========================================================================
// Includes
#include <activepalette2factory.h>


// ===========================================================================
// Forward declarations
class CActivePalette2Tooltip;
class CActivePalette2BasicUI;
class CActivePalette2Model;
class CActivePalette2Styler;


// ===========================================================================
// Class declarations
/**
* Factory class to generate Tooltip instances.
*/
class ActivePalette2TooltipFactory
  {
  public:

    /**
    * Creates a tooltip based on the supplied draw mode
    * @param aDrawMode The draw mode to implement
    * @return The instantiated tooltip
    */
    static CActivePalette2Tooltip* CreateTooltipL( TActivePalette2DrawMode     aDrawMode,
                                                   const CActivePalette2BasicUI* aParent,
                                                   CActivePalette2Model*       aModel,
                                                   CActivePalette2Styler*      aStyler   );
  };

#endif // ACTIVE_PALETTE_2_TOOLTIP_FACTORY_H

// ===========================================================================
// end of file
