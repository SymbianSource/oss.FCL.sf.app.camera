/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
#include "CamSyncRotatorAo.h"
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
// CCamSyncRotatorAo::CCamSyncRotatorAo
// C++ constructor
// -----------------------------------------------------------------------------
//
CCamSyncRotatorAo::CCamSyncRotatorAo( ) : 
                         CActive( EPriorityHigh )
                            
    {    
    }

// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::ConstructL
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CCamSyncRotatorAo::ConstructL()
    {    
    iRotator = CBitmapRotator::NewL();
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamSyncRotatorAo* CCamSyncRotatorAo::NewL( )
    {
    CCamSyncRotatorAo* self = new( ELeave ) CCamSyncRotatorAo( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::~CCamSyncRotatorAo()
// Destructor
// -----------------------------------------------------------------------------
//
CCamSyncRotatorAo::~CCamSyncRotatorAo()
  {
  PRINT( _L("Camera => ~CCamSyncRotatorAo" ))  
  Cancel();
  delete iRotator;
  iQueue.ResetAndDestroy();  
  PRINT( _L("Camera <= ~CCamSyncRotatorAo" ))  
  }

// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::AddToQueueL
// Adds the specified bitmap/rotator to the queue of items to be rotated
// -----------------------------------------------------------------------------
//
void CCamSyncRotatorAo::AddToQueueL( MBitmapRotationObserver& aObserver, 
        CFbsBitmap* aBitmap, CBitmapRotator::TRotationAngle aAngle )
    {
    CRotateTask* task = new ( ELeave ) CRotateTask( aObserver );
    CleanupStack::PushL( task );
    task->iBitmap = aBitmap;
    task->iAngle = aAngle;
    User::LeaveIfError( iQueue.Append( task ) );        
    CleanupStack::Pop( task );
    StartNextRotate();
    }


// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::StartNextRotate
// Called to start the next queued rotation task.  If no tasks are queued, does nothing
// -----------------------------------------------------------------------------
//
void CCamSyncRotatorAo::StartNextRotate()
    {
    if ( iQueue.Count() == 0 )
        return;
    
    CRotateTask* task = iQueue[0];
    if( !IsActive() )
        {       
        iRotator->Rotate( &iStatus, *task->iBitmap, task->iAngle );
        SetActive();
        }
    }
   
// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::RunL
// Called when a rotation operation is completed (or cancelled)
// -----------------------------------------------------------------------------
//
void CCamSyncRotatorAo::RunL()
    {
    // Pop the completed image from the queue.
    CRotateTask* task = iQueue[0];    
    iQueue.Remove( 0 );    

    task->iObserver.RotationCompleteL(iStatus.Int());

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
    
    
    //iObserver.RotationCompleteL(iStatus.Int());
    }
        
// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::DoCancel
// Called to cancel an outstanding rotation operation
// -----------------------------------------------------------------------------
//
void CCamSyncRotatorAo::DoCancel()
    {
    iRotator->Cancel();
    }
    
//  End of File  
