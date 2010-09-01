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
* Description:  Contains an event's data
*
*/


/**
 * @file ActivePalette2EventData.h
 * Contains an event's data
 */


#ifndef _ACTIVE_PALETTE_2_EVENT_DATA_H
#define _ACTIVE_PALETTE_2_EVENT_DATA_H

#include <e32std.h>

/**
 * Container for the data generated when a user selects an item
 */
class TActivePalette2EventData
    {
    public:	
    
        /**
         * Constructor
         * @param aResult The result
         * @param aCurItem The current item
         * @param aPrevItem The previous item
         * @param aServingItem The serving item
         * @param aServingPlugin A reference to the serving plugin's UID
         */
    	TActivePalette2EventData(TInt aResult, 
    							TInt aCurItem, 
    							TInt aPrevItem, 
    							TInt aServingItem, 
    							const TUid& aServingPlugin);
    							
    	/**
    	 * Returns the result
    	 * @return The result
    	 */
    	IMPORT_C TInt Result(void) const;

    	/**
    	 * Returns the current item
    	 * @return The current item
    	 */
    	IMPORT_C TInt CurItem(void) const;

    	/**
    	 * Returns the previous item
    	 * @return The previous item
    	 */
    	IMPORT_C TInt PrevItem(void) const;

    	/**
    	 * Returns the serving item
    	 * @return The serving item
    	 */
    	IMPORT_C TInt ServingItem(void) const; 

    	/**
    	 * Returns the serving plugin's UID
    	 * @return The serving plugin's UID
    	 */
    	IMPORT_C TUid ServingPlugin(void) const;

    private:
        /// The result code
    	TInt	iResult;
    	
    	/// The current item ID
    	TInt	iCurItem;
    	
    	/// The previous item ID
    	TInt	iPrevItem;
    	
    	/// The serving item ID
    	TInt	iServingItem;
    	
    	/// The serving plugin's UID
    	TUid   	iServingPlugin;
    };


#endif // _ACTIVE_PALETTE_2_EVENT_DATA_H
