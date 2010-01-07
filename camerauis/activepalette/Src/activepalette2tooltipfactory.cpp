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
* Description:  Active Palette 2 Tooltip Factory implementation
*
*/


// ===========================================================================
// Includes
#include "ActivePalette2Tooltip.h"
#include "activepalette2tooltipfading.h"
#include "activepalette2tooltipfactory.h"

// ===========================================================================
// Implementation

// ---------------------------------------------------------------------------
// CreateTooltipL
// ---------------------------------------------------------------------------
//
CActivePalette2Tooltip* 
ActivePalette2TooltipFactory::CreateTooltipL( 
    TActivePalette2DrawMode     aDrawMode,
    const CActivePalette2BasicUI* aParent,
    CActivePalette2Model*       aModel,
    CActivePalette2Styler*      aStyler  )
  {
  CActivePalette2Tooltip* tt = NULL;
  
  switch ( aDrawMode )
    {
    case EAP2DrawModeSemiTransparentTooltipModeFading:
      tt = CActivePalette2TooltipFading::NewL( aParent, aModel, aStyler );
      break;  
    case EAP2DrawModeSemiTransparent:
    case EAP2DrawModeCheckerboard:
    case EAP2DrawModeOpaque:
    default:
      tt = CActivePalette2Tooltip::NewL( aParent, aModel, aStyler );
      break;
    }
  
  return tt;
  }

// ===========================================================================
// end of file
