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
* Description:  Active Palette logger*
*/


/**
 * @file ActivePalette2Logger.h
 * Active Palette logger
 */

#ifndef _ACTIVE_PALETTE_2_LOGGER_H
#define _ACTIVE_PALETTE_2_LOGGER_H

#ifdef _DEBUG

#include <e32debug.h>

/// Debug logger
#define LOGTEXT(a)   		{ RDebug::Print(a); }
/// Debug logger, takes 1 extra param
#define LOGTEXT1(a,b)   	{ RDebug::Print(a,b); }
/// Debug logger, takes 2 extra params
#define LOGTEXT2(a,b,c)   	{ RDebug::Print(a,b,c); }
/// Debug logger, takes 3 extra params
#define LOGTEXT3(a,b,c,d)   { RDebug::Print(a,b,c,d); }

#else 

#define LOGTEXT(a)
#define LOGTEXT1(a,b)   
#define LOGTEXT2(a,b,c) 
#define LOGTEXT3(a,b,c,d)

#endif // _DEBUG

#endif //_ACTIVE_PALETTE_2_LOGGER_H

// End of File
