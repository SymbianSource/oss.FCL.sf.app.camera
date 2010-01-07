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
* Description:  Generic handler to be used by Observables
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/



// INCLUDE FILES
#include "CamObservable.h"
#include "CamObserver.h"
#include "CamObserverHandler.h"
#include "camlogging.h"

// ---------------------------------------------------------
// CCamObserverHandler::CCamObserverHandler
// C++ constructor
// ---------------------------------------------------------
//
CCamObserverHandler::CCamObserverHandler()
    {
    }
    
// ---------------------------------------------------------
// CCamObserverHandler::~CCamObserverHandler
// C++ Destructor
// ---------------------------------------------------------
//
CCamObserverHandler::~CCamObserverHandler()
  {
  PRINT( _L("Camera => ~CCamObserverHandler") );
  iObservers.Reset();
  PRINT( _L("Camera <= ~CCamObserverHandler") );
  }
    
// ---------------------------------------------------------
// CCamObserverHandler::NewL
// Symbian 2-phase constructor
// ---------------------------------------------------------
//
CCamObserverHandler* CCamObserverHandler::NewL()
    {
    CCamObserverHandler* self = new( ELeave ) CCamObserverHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// CCamObserverHandler::ConstructL
// Second phase constructor
// ---------------------------------------------------------
//
void CCamObserverHandler::ConstructL()
    {
    }

// ---------------------------------------------------------
// CCamObserverHandler::RegisterObserverL
// Registers observer if not previously registered
// ---------------------------------------------------------
//
void CCamObserverHandler::RegisterObserverL(MCamObserver* aObserver)
    {
    if ( aObserver )
        {
        if ( KErrNotFound == iObservers.Find( aObserver ))
            {
            User::LeaveIfError(iObservers.Append(aObserver));
            }
        }
    }
    
// ---------------------------------------------------------
// CCamObserverHandler::DeregisterObserver
// Removes observer
// ---------------------------------------------------------
//
void CCamObserverHandler::DeregisterObserver(MCamObserver* aObserver)
    {
    if ( aObserver )
        {
        TInt index = iObservers.Find( aObserver );
        if ( KErrNotFound != index )
            {
            iObservers.Remove(index);
            }
        }
    }
    
// ---------------------------------------------------------
// CCamObserverHandler::BroadcastEvent
// Broadcasts an event code to all observers
// ---------------------------------------------------------
//
void CCamObserverHandler::BroadcastEvent( TCamObserverEvent aEvent )
    {
    TInt count = iObservers.Count();
    TInt i;
    for ( i = 0; i < count; i++ )
        {
        // Checked to be non-NULL when added to array.
        iObservers[i]->HandleObservedEvent( aEvent );
        }
    }
    
// End of File
