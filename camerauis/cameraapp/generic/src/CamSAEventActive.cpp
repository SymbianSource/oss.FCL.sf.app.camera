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
* Description:  Handler for System Agent events*
*/


// INCLUDE FILES
#include <saclient.h>
#include "CamSAEventActive.h"
#include "CamAppController.h"

// We need to use a large enough priority, so that
// we get system agent events even when incoming call
// occurs during recording.
const TInt EPrioritySuperHigh = 2000;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------
// CCamSAEventActive::CCamSAEventActive
// C++ constructor
// ---------------------------------------------------------
//
CCamSAEventActive::CCamSAEventActive( CCamAppController& aController,
                                      const RSystemAgent& aSystemAgent )
    : CActive( EPrioritySuperHigh ), iController( aController ),
      iSystemAgent( aSystemAgent )
    {
    iRequestIssued = EFalse;
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CCamSAEventActive::~CCamSAEventActive() 
  {
  PRINT( _L("Camera => ~CCamSAEventActive") );
  Cancel();
  PRINT( _L("Camera <= ~CCamSAEventActive") );
  }

// ---------------------------------------------------------
// CCamSAEventActive::IssueRequest
// Issue a request to receive events from System Agent
// ---------------------------------------------------------
//
void CCamSAEventActive::IssueRequest()
    {
    iSAEvent.SetRequestStatus( iStatus );
    if ( !iRequestIssued ) 
        {
        iRequestIssued = ETrue;
        iSystemAgent.NotifyOnAnyEvent( iSAEvent );
        SetActive() ;       
        }
    }

// ---------------------------------------------------------
// CCamSAEventActive::DoCancel   
// Cancel the request to receive events from System Agent 
// ---------------------------------------------------------
//
void CCamSAEventActive::DoCancel() 
    {
    iSystemAgent.NotifyEventCancel();
    iRequestIssued = EFalse;
    }

// ---------------------------------------------------------
// CCamSAEventActive::RunError
// Called when RunL() is trapped by Active Scheduler
// ---------------------------------------------------------
//
TInt CCamSAEventActive::RunError( TInt /*aError*/ )
    {
    Cancel();
    IssueRequest();
    return KErrNone;
    }

// ---------------------------------------------------------
// Called when an event comes from System Agent
// ---------------------------------------------------------
//
void CCamSAEventActive::RunL()
    {   
    iRequestIssued = EFalse;
    iController.HandleSysAgentEventL( iSAEvent );
    IssueRequest();
    }

// End of File
