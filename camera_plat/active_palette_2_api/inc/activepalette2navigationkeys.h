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
* Description:  Contains the key definitions used for navigation in AP
*
*/


/**
 * @file ActivePalette2NavigationKeys.h
 * Contains the key definitions used for navigation in AP
 */

#ifndef _ACTIVE_PALETTE_2_NAVIGATION_KEYS_H
#define _ACTIVE_PALETTE_2_NAVIGATION_KEYS_H

#include <e32std.h>

/**
 * Navigation key type
 */
class TActivePalette2NavigationKeys
    {
    public:
        /** 
         * Constructor
         * @param aNaviPrev The scancode of the previous key
         * @param aNaviNext The scancode of the next key
         * @param aNaviSelect The scancode of the select key
         */
        IMPORT_C TActivePalette2NavigationKeys(TInt aNaviPrev,
        		                 TInt aNaviNext,
        		                 TInt aNaviSelect);

        /** 
         * Constructor
         * Assumes default navigation keys
         */
        IMPORT_C TActivePalette2NavigationKeys(void); 
        
        /// The &quot;Previous&quot; key's scancode
        TInt    iNaviPrev;
        /// The &quot;Next&quot; key's scancode
        TInt    iNaviNext;
        /// The &quot;Select&quot; key's scancode
        TInt    iNaviSelect;
    };


#endif // _ACTIVE_PALETTE_2_NAVIGATION_KEYS_H
