/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active object for handling toolbar commands
*
*/


// INCLUDE FILES
#include "CamCommandHandlerAo.h"
#include "camlogging.h"
#include <aknview.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::CCamCommandHandlerAo
// C++ constructor
// -----------------------------------------------------------------------------
//
CCamCommandHandlerAo::CCamCommandHandlerAo( CAknView* aView ) 
	:CActive( CActive::EPriorityStandard ), iView( aView )
    {
    }

// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::~CCamCommandHandlerAo
// Destructor
// -----------------------------------------------------------------------------
//
CCamCommandHandlerAo::~CCamCommandHandlerAo()
	{
	Cancel();
    iCommandQueue.Close();
	}

// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamCommandHandlerAo* CCamCommandHandlerAo::NewL( CAknView* aView )
	{
	CCamCommandHandlerAo* self = new( ELeave ) CCamCommandHandlerAo( aView );
  
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;	
	}

// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::ConstructL
// 2nd phase construction
// -----------------------------------------------------------------------------
//
void CCamCommandHandlerAo::ConstructL()
   {
   CActiveScheduler::Add( this );
   }
   
// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::DoCancel
// Cancels the active object
// -----------------------------------------------------------------------------
//
void CCamCommandHandlerAo::DoCancel()
    {
    iCommandQueue.Close();    
    PRINT( _L("Camera => CCamCommandHandlerAo::DoCancel()") );
    }

// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::RunL
// Checks the command from command queue and sends it to view
// -----------------------------------------------------------------------------
//
void CCamCommandHandlerAo::RunL()
	{
	if ( iCommandQueue.Count() )
    	{
        TInt command = iCommandQueue[ 0 ];
        iCommandQueue.Remove( 0 );
        iView->HandleCommandL( command );
        }
	}

// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::RunError
// Called when an error has occurred.
// -----------------------------------------------------------------------------
//
TInt CCamCommandHandlerAo::RunError( TInt aError )
	{
	PRINT1( _L("Camera => CCamCommandHandlerAo::RunError( %d )"), aError );
	(void)aError; // remove compiler warning
	
	iCommandQueue.Close();
	
	PRINT( _L("Camera <= CCamCommandHandlerAo::RunError()"));	
	
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CCamCommandHandlerAo::HandleCommandL
// Adds the command to queue and sets active
// -----------------------------------------------------------------------------
//	
void CCamCommandHandlerAo::HandleCommandL( TInt aCommandId )
	{
    PRINT( _L("Camera => CCamCommandHandlerAo::HandleCommandL()"));

    TInt count = iCommandQueue.Count();

    if( count < 1 || iCommandQueue[ count - 1 ] != aCommandId )
        {
        iCommandQueue.AppendL( aCommandId );
        }     
  	
    if ( !IsActive() )
   	    {
        TRequestStatus* statusPtr = &iStatus;
        User::RequestComplete( statusPtr, KErrNone ); 
    	SetActive();
  	    }
  	  
    PRINT( _L("Camera <= CCamCommandHandlerAo::HandleCommandL()"));
    }























