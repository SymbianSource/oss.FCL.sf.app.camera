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
* Description:  Contains (item id, visible) pairs used by AP2
*
*/


/**
 * @file ActivePalette2ItemVisible.h
 * Contains (item id, visible) pairs used by AP2
 */

#ifndef _ACTIVE_PALETTE_2_ITEM_VISIBLE_H
#define _ACTIVE_PALETTE_2_ITEM_VISIBLE_H

#include <e32std.h>

/**
 * Container class to describe (Item ID, visibility) pairs.
 */
class TActivePalette2ItemVisible
    {
    public:
        /** 
         * Constructor
         * @param aItemId The Item ID
         * @param aVisible Whether the item is / should be visible
         */
        IMPORT_C TActivePalette2ItemVisible(TInt aItemId, TBool aVisible);

        /**
         * Returns the item ID
         * @return The item ID
         */
		IMPORT_C TInt ItemId() const;

        /**
         * Returns the visibility
         * @return The visibility of the item
         */
		IMPORT_C TBool Visible() const;

	private:
	    /**
	     * Constructor. Not for external use.
	     */
		TActivePalette2ItemVisible(); 
		
		/// The item ID
        TInt    iItemId;
        
        /// The visibility
        TBool   iVisible;
    };


#endif // _ACTIVE_PALETTE_2_ITEM_VISIBLE_H
