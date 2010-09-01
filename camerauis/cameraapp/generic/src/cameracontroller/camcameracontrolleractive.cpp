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
* Description:  CCamCameraControllerActive class implementation.
*                
*
*/


#include <e32base.h>
#include "camcameracontroller.pan"
#include "camlogging.h"
#include "camcameracontroller.h"
#include "camcameracontrolleractive.h"

using namespace NCamCameraController;


// ===========================================================================
// Costructors and destructor

// ---------------------------------------------------------------------------
// 2 phase constructor <<static>>
// ---------------------------------------------------------------------------
//
CCamCameraControllerActive* 
CCamCameraControllerActive::NewL( CCamCameraController& aController, 
                                  TInt                  aPriority   )
  {
  CCamCameraControllerActive* self = 
      new (ELeave) CCamCameraControllerActive( aController, aPriority );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;
  }


// ---------------------------------------------------------------------------
// Destructor <<virtual>>
// ---------------------------------------------------------------------------
//
CCamCameraControllerActive::~CCamCameraControllerActive()
  {
  PRINT( _L("Camera => ~CCamCameraControllerActive") );
  Cancel();
  PRINT( _L("Camera <= ~CCamCameraControllerActive") );
  }

  
// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void 
CCamCameraControllerActive::ConstructL()
  {
  CActiveScheduler::Add( this );
  }


// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
CCamCameraControllerActive
::CCamCameraControllerActive( CCamCameraController& aController, 
                              TInt                  aPriority   )
  : CActive( aPriority ),
    iController( aController )
  {
  }
    
// ===========================================================================
// From CActive

// ---------------------------------------------------------------------------
// DoCancel <<virtual>> 
// ---------------------------------------------------------------------------
//
void 
CCamCameraControllerActive::DoCancel()
  {
  HandleError( KErrCancel );
  }

// ---------------------------------------------------------------------------
// RunL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamCameraControllerActive::RunL()
  {
  PRINT( _L("Camera => CCamCameraControllerActive::RunL") );
  User::LeaveIfError( iStatus.Int() );

  TInt more = iController.ProcessNextRequestL();
  
  if( more )
    {
    IssueRequest();
    }
  else
    {
    // No more callbacks needed atleast for now.    
    }
  PRINT( _L("Camera <= CCamCameraControllerActive::RunL") );
  }

// ---------------------------------------------------------------------------
// RunError <<virtual>>
//
// Called when leave occurs in RunL
// ---------------------------------------------------------------------------
//
TInt 
CCamCameraControllerActive::RunError( TInt aError )
  {
  PRINT1( _L("Camera => CCamCameraControllerActive::RunError, error:%d"), aError );

  HandleError( aError );
  
  PRINT( _L("Camera <= CCamCameraControllerActive::RunError") );
  return KErrNone;
  }

// ===========================================================================
// New methods

// ---------------------------------------------------------------------------
// IssueRequest
// ---------------------------------------------------------------------------
//
void
CCamCameraControllerActive::IssueRequest()
  {
  PRINT( _L("Camera => CCamCameraControllerActive::IssueRequest") );

  __ASSERT_ALWAYS( !IsActive(), Panic( ECamCameraControllerBusy ) );

  // Set RunL to be called as soon as possible  
  TRequestStatus* status = &iStatus;
  User::RequestComplete( status, KErrNone );
  SetActive();

  PRINT( _L("Camera <= CCamCameraControllerActive::IssueRequest") );
  }


// ---------------------------------------------------------------------------
// HandleError
// ---------------------------------------------------------------------------
//
void 
CCamCameraControllerActive::HandleError( TInt aStatus )
  {
  iController.EndSequence( aStatus );
  }

// ===========================================================================
// end of file
