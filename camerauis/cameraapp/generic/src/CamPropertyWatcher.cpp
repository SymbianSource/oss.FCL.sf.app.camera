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
* Description:  Callback wrapper for Publish and Subscribe*
*/


// INCLUDE FILES

#include <e32svr.h>
#include <f32file.h>
#include "CamPropertyWatcher.h"
#include "CamPropertyObserver.h"
#include "CamUtility.h"

// ========================= MEMBER FUNCTIONS ================================

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::NewL
// Two phase construction
// -----------------------------------------------------------------------------
//
CCamPropertyWatcher* CCamPropertyWatcher::NewL( MPropertyObserver& aPropertyObserver, 
                                                const TUid& aCategory, 
                                                const TUint aKey )
    {
    CCamPropertyWatcher* self = new(ELeave) CCamPropertyWatcher( aPropertyObserver, 
                                                                 aCategory, 
                                                                 aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::Destructor
// -----------------------------------------------------------------------------
//
CCamPropertyWatcher::~CCamPropertyWatcher()
  {
  PRINT( _L("Camera => ~CCamPropertyWatcher") );
  Cancel();
  iProperty.Close();
  PRINT( _L("Camera <= ~CCamPropertyWatcher") );
  }

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::Subscribe
// Request notification of change events
// -----------------------------------------------------------------------------
//
void CCamPropertyWatcher::Subscribe()
    {
    PRINT( _L( "Camera => CCamPropertyWatcher::Subscribe()" ) )
    if(!IsActive())
        {
        iProperty.Subscribe( iStatus );
        SetActive();
        }
    PRINT( _L( "Camera <= CCamPropertyWatcher::Subscribe()" ) )
    }

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::Get
// Return the current value
// -----------------------------------------------------------------------------
//
TInt CCamPropertyWatcher::Get( TInt& aValue )
	{
	return iProperty.Get( aValue );
	}

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::CCamPropertyWatcher
// Constructor
// -----------------------------------------------------------------------------
//
CCamPropertyWatcher::CCamPropertyWatcher( MPropertyObserver& aPropertyObserver, 
                                          const TUid& aCategory, 
                                          const TUint aKey ) :
    CActive( EPriorityStandard ),
    iPropertyObserver( aPropertyObserver ),
    iCategory( aCategory),
    iKey( aKey )
    {
    }

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::ConstructL
// Second phase construction
// -----------------------------------------------------------------------------
//
void CCamPropertyWatcher::ConstructL()
    {
    CActiveScheduler::Add( this );
	User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    }

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::RunL
// Handle notification of value change
// -----------------------------------------------------------------------------
//
void CCamPropertyWatcher::RunL()
    {
    PRINT( _L( "Camera => CCamPropertyWatcher::RunL()" ) )
    const TInt error ( iStatus.Int() );
    PRINT1( _L( "Camera CCamPropertyWatcher::RunL() status = %d" ), error )
    Subscribe();
    if ( error == KErrNone )
        {
        PRINT( _L( "Camera CCamPropertyWatcher::RunL() calling HandlePropertyChangedL" ) )
		iPropertyObserver.HandlePropertyChangedL( iCategory, iKey );
		}
    }

// -----------------------------------------------------------------------------
// CCamPropertyWatcher::DoCancel
// Handle cancellation
// -----------------------------------------------------------------------------
//
void CCamPropertyWatcher::DoCancel()
    {
    iProperty.Cancel();
    }

//  End of File  
