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
* Description:  Definition of constants equired by external applications*
*/



#ifndef CAMUICONSTANTS_H
#define CAMUICONSTANTS_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// Initial value for direct viewfinder color key. 
// const TUint KCamDirectColorVal = 0xFF8C8C8C;
const TUint KCamDirectColorVal = 0x00030303;

// Precapture view background color. This color is used to
// paint the background during application startup and mode switches,
// when viewfinder is not yet running.
// In case of direct viewfinding, same value as for color key
// should be used, to avoid backround with different color showing
// when viewfinder is stopped before mode switch, but screen is 
// not redrawn.
const TUint KCamPrecaptureBgColor = 0x00030303;

// The last two decimals, ie. 8C, is the amount of
// transparency. 00 would be totally transparent
// and FF totally visible.
#define KRgbShadedWhite TRgb( 0xFF, 0xFF, 0xFF, 0x8C )

#endif      // CAMUICONSTANTS_H   
            
// End of File
