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
CCamSyncRotatorAo::CCamSyncRotatorAo(MBitmapRotationObserver& aObserver) : 
                         CActive( EPriorityHigh ),
                         iObserver( aObserver )
                            
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
CCamSyncRotatorAo* CCamSyncRotatorAo::NewL(MBitmapRotationObserver& aObserver)
    {
    CCamSyncRotatorAo* self = new( ELeave ) CCamSyncRotatorAo(aObserver);
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
  PRINT( _L("Camera <= ~CCamSyncRotatorAo" ))  
  }

// -----------------------------------------------------------------------------
// CCamSyncRotatorAo::RotateL()
// Rotate the provided bitmap
// -----------------------------------------------------------------------------
//
void CCamSyncRotatorAo::RotateL( CFbsBitmap* aBitmap, CBitmapRotator::TRotationAngle aRotation )
    {
    if( !IsActive() )
        {
        iRotator->Rotate( &iStatus, *aBitmap, aRotation );
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
    iObserver.RotationCompleteL(iStatus.Int());
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
