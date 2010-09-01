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
* Description:  Utility class to rotate bitmaps*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include "CamBmpRotatorAo.h"
#include "camlogging.h"

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::CCamBmpRotatorAo
// C++ constructor
// -----------------------------------------------------------------------------
//
CCamBmpRotatorAo::CCamBmpRotatorAo() : CActive( EPriorityHigh )
    {    
    }

// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::ConstructL
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CCamBmpRotatorAo::ConstructL()
    {    
    iRotator = CBitmapRotator::NewL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamBmpRotatorAo* CCamBmpRotatorAo::NewL()
    {
    CCamBmpRotatorAo* self = new( ELeave ) CCamBmpRotatorAo();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::~CCamBmpRotatorAo()
// Destructor
// -----------------------------------------------------------------------------
//
CCamBmpRotatorAo::~CCamBmpRotatorAo()
  {
  PRINT( _L("Camera => ~CCamBmpRotatorAo") );
  if ( IsActive() )
    {
    Cancel();
    }
  
  delete iRotator;
  iQueue.ResetAndDestroy();
  PRINT( _L("Camera <= ~CCamBmpRotatorAo") );
  }


    
// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::AddToQueueL
// Adds the specified bitmap/rotator to the queue of items to be rotated
// -----------------------------------------------------------------------------
//
void CCamBmpRotatorAo::AddToQueueL( CFbsBitmap* aBitmap, CBitmapRotator::TRotationAngle aAngle )
    {
    CRotateTask* task = new ( ELeave ) CRotateTask;
    CleanupStack::PushL( task );
    task->iBitmap = aBitmap;
    task->iAngle = aAngle;
    User::LeaveIfError( iQueue.Append( task ) );        
    CleanupStack::Pop( task );
    }
    
// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::RunL
// Called when a rotation operation is completed (or cancelled)
// -----------------------------------------------------------------------------
//
void CCamBmpRotatorAo::RunL()
    {
    // Pop the completed image from the queue.
    CRotateTask* task = iQueue[0];    
    iQueue.Remove( 0 );    
    delete task;  // NOTE: no need to delete bitmap (as not owned)
        
    // If rotate completed successfully...
    // ... and there are more left on the queue...
    // ... then start the next rotate
    if ( iStatus.Int() == KErrNone )
        {
        if ( iQueue.Count() > 0 )
            {
            StartNextRotate();
            }    
        }
    else 
        {
        // Rotation failed; cancel further rotation attempts in 
        // the queue
        iQueue.ResetAndDestroy();
        }
    }
    
// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::DoCancel
// Called to cancel an outstanding rotation operation
// -----------------------------------------------------------------------------
//
void CCamBmpRotatorAo::DoCancel()
    {
    iRotator->Cancel();
    }
    
// -----------------------------------------------------------------------------
// CCamBmpRotatorAo::StartNextRotate
// Called to start the next queued rotation task.  If no tasks are queued, does nothing
// -----------------------------------------------------------------------------
//
void CCamBmpRotatorAo::StartNextRotate()
    {
    if ( iQueue.Count() == 0 )
        return;
    
    CRotateTask* task = iQueue[0];
       
    iRotator->Rotate( &iStatus, *task->iBitmap, task->iAngle );
    SetActive();
    }

//  End of File  
