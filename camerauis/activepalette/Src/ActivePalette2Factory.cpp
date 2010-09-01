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
* Description:  Creates concrete implementation of AP*
*/


/**
 * @file ActivePalette2Factory.cpp
 * Creates concrete implementation of AP
 */

#include "ActivePalette2BuildConfig.h"

#ifdef AP2_USE_2D_RENDERING
#include "ActivePalette2BasicUI.h"
#endif

#ifdef AP2_USE_3D_RENDERING
#include "ActivePalette2HUI.h"
#endif

#include <activepalette2factory.h>
#include <activepalette2ui.h>


// ----------------------------------------------------------------------------
// ActivePalette2Factory::CreateActivePaletteUIL
// ----------------------------------------------------------------------------
#ifdef AP2_USE_2D_RENDERING
EXPORT_C MActivePalette2UI* ActivePalette2Factory::CreateActivePaletteUIL(TActivePalette2DrawMode aDrawMode)
    {
    return CActivePalette2BasicUI::NewL(aDrawMode);
    }
#else
EXPORT_C MActivePalette2UI* ActivePalette2Factory::CreateActivePaletteUIL(TActivePalette2DrawMode /* aDrawMode */ )
	{
    User::Leave(KErrNotSupported);
    return NULL;
	}
#endif // AP2_USE_2D_RENDERING    


// ----------------------------------------------------------------------------
// ActivePalette2Factory::CreateHUIActivePaletteUIL
// ----------------------------------------------------------------------------
#ifdef AP2_USE_3D_RENDERING
EXPORT_C MActivePalette2UI* ActivePalette2Factory::CreateHUIActivePaletteUIL(CHuiEnv& aEnv)
	{
	return CActivePalette2HUI::NewL(aEnv);
	}
#else
EXPORT_C MActivePalette2UI* ActivePalette2Factory::CreateHUIActivePaletteUIL(CHuiEnv& /* aEnv */ )
	{
    User::Leave(KErrNotSupported);
	return NULL;
	}
#endif // AP2_USE_3D_RENDERING


EXPORT_C MActivePalette2UI* ActivePalette2Factory::CreateGuiActivePaletteUIL( MGui* /*aGui*/ )
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }
