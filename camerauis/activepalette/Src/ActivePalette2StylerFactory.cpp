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
* Description:  Active Palette Styler Factory*
*/


/**
 * @file ActivePalette2StylerFactory.cpp
 * Active Palette Styler Factory
 */

#include "ActivePalette2StylerFactory.h"
#include "ActivePalette2Styler.h"
#include "ActivePalette2SemiTransparentFadingStyler.h"
#include "ActivePalette2SemiTransparentStyler.h"
#include "ActivePalette2CheckerboardStyler.h"
#include "ActivePalette2OpaqueStyler.h"

// -----------------------------------------------------------------------------
// ActivePalette2StylerFactory::CreateStylerL()
// -----------------------------------------------------------------------------
//
CActivePalette2Styler* ActivePalette2StylerFactory::CreateStylerL(TActivePalette2DrawMode aDrawMode)
    {
    CActivePalette2Styler* res = NULL;
    
    switch ( aDrawMode )
        {
        case EAP2DrawModeSemiTransparentTooltipModeFading:
            res = CActivePalette2SemiTransparentFadingStyler::NewL();
            break;

        case EAP2DrawModeSemiTransparent:
            res = CActivePalette2SemiTransparentStyler::NewL();
            break;
            
        case EAP2DrawModeCheckerboard:
            res = CActivePalette2CheckerboardStyler::NewL();
            break;
            
        case EAP2DrawModeOpaque:
        default:
            res = CActivePalette2OpaqueStyler::NewL();
            break;
        }
    
    return res;
    }
