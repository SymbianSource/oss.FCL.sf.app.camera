/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Creates concrete implementation of AP
*
*/


/**
 * @file ActivePalette2Factory.h
 * Creates concrete implementation of AP
 */

#ifndef _ACTIVE_PALETTE_2_FACTORY_H
#define _ACTIVE_PALETTE_2_FACTORY_H

#include <e32std.h>
#include <e32base.h>

class MActivePalette2UI;
class CHuiEnv;
class MGui;

/**
 * Describes how the standard CCoeControl-based Active Palette will be rendered
 */
enum TActivePalette2DrawMode
	{
	EAP2DrawModeSemiTransparent = 0, ///< Semi-transparent, alpha-blended background
	EAP2DrawModeCheckerboard,        ///< With a checkerboard background. Alpha values will either be fully opaque or fully transparent.
	EAP2DrawModeOpaque,              ///< No tranparency. Not officially supported

	EAP2DrawModeSemiTransparentTooltipModeFading ///< Semi-transparent, alpha-blended background, fading tooltip
  };

/**
 * Factory class for Active Palette 2. Use these static functions to create AP2 objects.
 * Class should not be instantiated.
 */
class ActivePalette2Factory
    {
public:
    /**
    * Creates a legacy-style, CCoeControl-based Active Palette.
    * The control created is NOT a window-owning control. Set the container window and Mop parent using
    * the CoeControl() function.
    * @param aDrawMode The drawing style used to render the AP
    * @return The constructed Active Palette
    */
    IMPORT_C static MActivePalette2UI* CreateActivePaletteUIL(TActivePalette2DrawMode aDrawMode);
    
    /**
    * Creates a new-style, CHuiControl-based Active Palette.
    * The control should be added to a control group using
    * the CoeControl() function.
    * @param aHuiEnv The client app's Hui environment
    * @return The constructed Active Palette
    */
    IMPORT_C static MActivePalette2UI* CreateHUIActivePaletteUIL(CHuiEnv& aHuiEnv);

    /**
    * Creates a new-style, GuiLib-based Active Palette.
    * @param aGui The client app's Gui environment
    * @return The constructed Active Palette
    */
    IMPORT_C static MActivePalette2UI* CreateGuiActivePaletteUIL(MGui* aGui);

protected:
    /**
     * Constructor - not for external use
     */
    ActivePalette2Factory() {}
    };


#endif //  _ACTIVE_PALETTE_2_FACTORY_H
