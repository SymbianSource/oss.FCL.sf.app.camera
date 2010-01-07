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
* Description:  Defines abstract interface for CCamSelfTimer observers*
*/



#ifndef CAMSELFTIMEROBSERVER_H
#define CAMSELFTIMEROBSERVER_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES
enum TCamSelfTimerEvent
    {
    ECamSelfTimerEventTimerOn,
    ECamSelfTimerEventTimerOff
    };


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Abstract API for self timer observer. 
* Derived classes may register as observers to get related
* notifications about events from CCamSelfTimer.
*
*  @since 2.8
*/
class MCamSelfTimerObserver
    {
    public:
        /**
        * Handle an event from CCamSelfTimer.
        * @since 2.8
        * @param aEvent the self timer event
        * @param aCountdown self timer countdown duration
        */
        virtual void HandleSelfTimerEvent( TCamSelfTimerEvent aEvent, TInt aCountDown ) = 0;
    };

#endif      // CAMSELFTIMEROBSERVER_H
            
// End of File
