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
* Description:  Listens for changes to phone call state*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <eikenv.h>

#include "CamUtility.h"
#include "CamCallStateAo.h"
#include "CamPanic.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================
#ifdef _DEBUG
void ExpandStateDebug( TPSCTsyCallState aVal )
    {
    switch ( aVal )
        {   
    case EPSCTsyCallStateUninitialized:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => Uninit") );
        break;
    case EPSCTsyCallStateNone:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => NONE") );
        break;
    case EPSCTsyCallStateAlerting:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => ALERTING") );
        break;
    case EPSCTsyCallStateRinging:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => RINGING") );
        break;
    case EPSCTsyCallStateDialling:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => DIALLING") );
        break;
    case EPSCTsyCallStateAnswering:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => ANSWERING") );
        break;
    case EPSCTsyCallStateDisconnecting:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => DISCONNECTING") );
        break;
    case EPSCTsyCallStateConnected:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => CONNECTED") );
        break;
    case EPSCTsyCallStateHold:
        PRINT(_L("Camera    CCamCallStateAo::RunL NEW STATUS => HOLD") );
        break;
        }
    }
#endif // _DEBUG


// -----------------------------------------------------------------------------
// CCamCallStateAo::CCamCallStateAo
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCamCallStateAo::CCamCallStateAo( MCamCallStateObserver* aObserver ) : CActive( EPriorityStandard )
    {            
    PRINT( _L("Camera => CCamCallStateAo::CCamCallStateAo") );
    __ASSERT_DEBUG( aObserver != NULL, CamPanic(ECamPanicNullImage) );
    CActiveScheduler::Add( this );
    
    iObserver = aObserver;
    }
    
    
// -----------------------------------------------------------------------------
// CCamCallStateAo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamCallStateAo* CCamCallStateAo::NewL( MCamCallStateObserver* aObserver )
    {
    CCamCallStateAo* self = new( ELeave ) CCamCallStateAo( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCamSelfTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamCallStateAo::ConstructL()
    {
    PRINT( _L("Camera => CCamCallStateAo::ConstructL") );
    
    TInt ret = iProperty.Attach( KPSUidCtsyCallInformation, KCTsyCallState );
    PRINT1( _L("CCamCallStateAo::ConstructL attaching property ret %d"), ret );
           
    if ( ret == KErrNone )
        {   
        // Will subscribe to the attribute required 
        RunL();
        }
    else
        {   
        // If an error occurred, we can't get call state info which we really need
        // so leave
        User::Leave( ret );
        }
    
    PRINT( _L("Camera <= CCamCallStateAo::ConstructL") );
    }

// -----------------------------------------------------------------------------
// CCamCallStateAo::~CCamCallStateAo
// Destructor
// -----------------------------------------------------------------------------
//
CCamCallStateAo::~CCamCallStateAo()
  {
  PRINT( _L("Camera => ~CCamCallStateAo") );
  Cancel();
  PRINT( _L("Camera <= ~CCamCallStateAo") );
  }

    
// -----------------------------------------------------------------------------
// CCamCallStateAo::DoCancel
// Cancels an outstanding request
// -----------------------------------------------------------------------------
//
void CCamCallStateAo::DoCancel()
    {
    iProperty.Close();
    }
    

// -----------------------------------------------------------------------------
// CCamCallStateAo::RunL
// Called when a request has completed.  
// -----------------------------------------------------------------------------
//
void CCamCallStateAo::RunL()
    {
    PRINT(_L("Camera => CCamCallStateAo::RunL") );
    iProperty.Subscribe( iStatus );
    PRINT(_L("Camera    CCamCallStateAo::RunL subscribed") );

    SetActive();
    PRINT(_L("Camera    CCamCallStateAo::RunL set active") );

    TInt val = KErrNotFound;
    TInt ret = iProperty.Get( val );

    if ( val != KErrNotFound && ret != KErrNotFound )
        {
        TPSCTsyCallState callState = static_cast<TPSCTsyCallState>( val );
#ifdef _DEBUG
        ExpandStateDebug( callState );
#endif
        PRINT(_L("Camera    CCamCallStateAo::RunL calling callback") );
        iObserver->CallStateChanged( callState, ret );
        PRINT(_L("Camera    CCamCallStateAo::RunL returned from callback") );
        }
    else
        {
        iObserver->CallStateChanged( EPSCTsyCallStateUninitialized, ret );
        }
    PRINT(_L("Camera <= CCamCallStateAo::RunL") );
    }
    

// -----------------------------------------------------------------------------
// CCamCallStateAo::RunError
// Called when an error has occurred.
// -----------------------------------------------------------------------------
//
TInt CCamCallStateAo::RunError( TInt aError )
    {
    iObserver->CallStateChanged( EPSCTsyCallStateUninitialized, aError );    
    return KErrNone;
    }

//  End of File  
