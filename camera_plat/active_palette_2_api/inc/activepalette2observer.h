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
* Description:  Interface used by AP to report events and request services
*                from client application
*
*/



/**
 * @file ActivePalette2Observer.h
 * Interface used by AP to report events and request services from 
 * client application
 */


#ifndef _ACTIVE_PALETTE_2_OBSERVER_H
#define _ACTIVE_PALETTE_2_OBSERVER_H

#include <e32std.h>

class TActivePalette2EventData;

/**
 * Interface for an Active Palette 2 observer. The AP will use this interface to inform the client app
 * about certain events, and to request services.
 */
class MActivePalette2Observer
    {
    public:
        /**
         * Called when a new item is focused
         * @param aPrevItem Item ID of previously focused item
         * @param aFocusedItem Item ID of newly focused item
         */
    	virtual void NotifyItemFocused(TInt aPrevItem, TInt aFocusedItem) = 0;

        /**
         * Called when the user has selected an item from the AP, before the plugin is informed
         * @param aEvent The event data
         */
    	virtual void NotifyItemSelected(const TActivePalette2EventData& aEvent) = 0;
    	
        /**
         * Called when the user has selected an item from the AP, after the relevant plugin
         * has completed processing the event
         * @param aEvent The event data
         * @param aDataDes Data descriptor
         * @param aDataInt Data integer
         */
    	virtual void NotifyItemComplete(const TActivePalette2EventData& aEvent,
    	                                const TDesC8& aDataDes,
    	                                TInt aDataInt) = 0;

        /**
         * Called when a plugin generates a message for the client app.
         * @param aEvent The event data
         * @param aMessageID The ID of the message
         * @param aDataDes Data descriptor
         * @param aDataInt Data integer
         */
    	virtual void NotifyMessage(const TActivePalette2EventData& aEvent,
    	                           TInt aMessageID,
    	                           const TDesC8& aDataDes,
    	                           TInt aDataInt) = 0;
    	                           
        /**
         * Request for the client app to redraw the given area underneath the AP. Used to support semi-transparency.
         * It is important that this completes quickly. This will only be called if a CCoeControl-derived AP is
         * in use.
         * @param aArea The area to redraw.
         */    	                           
        virtual void Redraw(const TRect& aArea) = 0;
        
        /**
         * Informs the observer that the AP has been updated in some way. Includes animations. 
         * Only called if SetGc() has previously been called on the AP with a non-NULL parameter, and if the AP
         * in use is a CCoeControl.
         */    	                           
        virtual void ActivePaletteUpdated(void) = 0;
    };

#endif // _ACTIVE_PALETTE_2_OBSERVER_H
