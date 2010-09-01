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
* Description:  Contains (item id, visible) pairs used by AP2*
*/


/**
 * @file ActivePalette2ItemVisible.cpp
 * Contains (item id, visible) pairs used by AP2
 */


#include <activepalette2itemvisible.h>


// -----------------------------------------------------------------------------
// TActivePalette2ItemVisible::TActivePalette2ItemVisible()
// -----------------------------------------------------------------------------
//
EXPORT_C TActivePalette2ItemVisible::TActivePalette2ItemVisible(TInt aItemId, TBool aVisible)
:	iItemId(aItemId),
	iVisible(aVisible)
	{
    // No implementation required
	}


// -----------------------------------------------------------------------------
// TActivePalette2ItemVisible::ItemId()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt TActivePalette2ItemVisible::ItemId() const
	{
	return iItemId;
	}

// -----------------------------------------------------------------------------
// TActivePalette2ItemVisible::Visible()
// -----------------------------------------------------------------------------
//
EXPORT_C TBool TActivePalette2ItemVisible::Visible() const
	{
	return iVisible;
	}

// -----------------------------------------------------------------------------
// TActivePalette2ItemVisible::TActivePalette2ItemVisible()
// -----------------------------------------------------------------------------
//
TActivePalette2ItemVisible::TActivePalette2ItemVisible()
:   iItemId(0),
	iVisible(EFalse)
	{
    // No implementation required
	}
