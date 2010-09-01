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
* Description:  Contains an event's data*
*/


/**
 * @file ActivePalette2EventData.cpp
 * Contains an event's data
 */


#include <activepalette2eventdata.h>

// -----------------------------------------------------------------------------
// TActivePaletteEventData::TActivePaletteEventData()
// -----------------------------------------------------------------------------
//
TActivePalette2EventData::TActivePalette2EventData(TInt aResult, TInt aCurItem, const TInt aPrevItem, const TInt aServingItem, const TUid& aServingPlugin):
    iResult(aResult),
    iCurItem(aCurItem),
    iPrevItem(aPrevItem),
    iServingItem(aServingItem),
    iServingPlugin(aServingPlugin)
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// TActivePaletteEventData::Result()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TActivePalette2EventData::Result(void) const
    {
	return iResult;
    }


// -----------------------------------------------------------------------------
// TActivePaletteEventData::CurItem()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TActivePalette2EventData::CurItem(void) const
    {
	return iCurItem;
    }


// -----------------------------------------------------------------------------
// TActivePaletteEventData::PrevItem()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TActivePalette2EventData::PrevItem(void) const
    {
	return iPrevItem;
    }


// -----------------------------------------------------------------------------
// TActivePaletteEventData::ServingItem()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TActivePalette2EventData::ServingItem(void) const
    {
	return iServingItem;
    }


// -----------------------------------------------------------------------------
// TActivePaletteEventData::ServingPlugin()
// -----------------------------------------------------------------------------
//
EXPORT_C TUid TActivePalette2EventData::ServingPlugin(void) const
    {
	return iServingPlugin;
    }

