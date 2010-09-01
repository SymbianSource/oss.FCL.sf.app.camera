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
* Description:  Active Palette 2 DLL entry point*
*/


/**
 * @file ActivePalette2DllMain.cpp
 * Active Palette 2 DLL entry point
 */

// INCLUDES
#include <e32base.h>

/**
 * Standard Symbian DLL entry point function.
 */
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /* aReason*/)
    {
	return KErrNone;
    }    
#endif
