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
* Description:  Contains the key definitions used for navigation in AP*
*/


/**
 * @file ActivePalette2NavigationKeys.cpp
 * Contains the key definitions used for navigation in AP
 */


#include <e32keys.h>
#include <activepalette2navigationkeys.h>


// -----------------------------------------------------------------------------
// CActivePaletteUI::TNavigationKeys::TNavigationKeys()
// -----------------------------------------------------------------------------
//
EXPORT_C TActivePalette2NavigationKeys::TActivePalette2NavigationKeys(void)
:	iNaviPrev(EStdKeyUpArrow),
	iNaviNext(EStdKeyDownArrow),
	iNaviSelect(EStdKeyDevice3)
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CActivePaletteUI::TNavigationKeys::TNavigationKeys()
// -----------------------------------------------------------------------------
//
EXPORT_C TActivePalette2NavigationKeys::TActivePalette2NavigationKeys(
    TInt aNaviPrev,
    TInt aNaviNext,
    TInt aNaviSelect )
:	iNaviPrev( aNaviPrev ),
	iNaviNext( aNaviNext ),
	iNaviSelect( aNaviSelect )
    {
    // No implementation required
    }


