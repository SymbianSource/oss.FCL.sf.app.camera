/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Abstract interface for observer classes
*/


// INCLUDE FILES
#include "Cam.hrh"

#ifndef MCAMOBSERVER_H
#define MCAMOBSERVER_H

class MCamObserver
    {
public:    
    /**
    * Receives event codes from observables
    * @param aEvent The event code
    * @since 3.0
    */
    virtual void HandleObservedEvent(TCamObserverEvent aEvent) = 0;
    };
 
#endif // MCAMOBSERVER_H
