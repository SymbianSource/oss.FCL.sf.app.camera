/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#include "camsnapshotrotator.h"
#include "CamSyncRotatorAo.h"
#include "CamBurstCaptureArray.h"
#include "camlogging.h"
#include "CamAppController.h"
          
// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamSnapShotRotator::CCamSnapShotRotator
// C++ constructor
// -----------------------------------------------------------------------------
//
CCamSnapShotRotator::CCamSnapShotRotator( CCamAppController& aController ) 
    : iController(aController), iRotatedSnapshot(NULL), iRotatorAo(NULL)
    {    
    }

// -----------------------------------------------------------------------------
// CCamSnapShotRotator::ConstructL
// Second phase constructor.
// -----------------------------------------------------------------------------
//
void CCamSnapShotRotator::ConstructL()
    {   

    }

// -----------------------------------------------------------------------------
// CCamSnapShotRotator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamSnapShotRotator* CCamSnapShotRotator::NewL( CCamAppController& aController )
    {
    CCamSnapShotRotator* self = new( ELeave ) CCamSnapShotRotator( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CCamSnapShotRotator::~CCamSnapShotRotator()
// Destructor
// -----------------------------------------------------------------------------
//
CCamSnapShotRotator::~CCamSnapShotRotator()
    {
    PRINT( _L("Camera => ~CCamSnapShotRotator") );
    if ( iRotatorAo && iRotatorAo->IsActive() )
        {
        iRotatorAo->Cancel();
        }
    delete iRotatorAo;
    delete iRotatedSnapshot;
    PRINT( _L("Camera <= ~CCamSnapShotRotator") );
    }

// ---------------------------------------------------------------------------
// CCamSnapShotRotator::RotationCompleteL
// Notification of bitmap rotation complete events
// ---------------------------------------------------------------------------
//  
void CCamSnapShotRotator::RotationCompleteL( TInt aErr )
    {
    PRINT1( _L( "Camera => CCamSnapShotRotator::RotationCompleteL aErr:%d" ), aErr );
     // Use this snapshot, even if rotation fails. 
    if ( iRotatedSnapshot )
        {
        PRINT( _L( "Camera => CCamSnapShotRotator::RotationCompleteL - ReplaceSnapshot" ) );
        iController.BurstCaptureArray()->ReplaceSnapshot( iRotatedSnapshot, 0 );
        PRINT( _L( "Camera => CCamSnapShotRotator::RotationCompleteL - delete iRotatedSnapshot" ) );
        delete iRotatedSnapshot;
        iRotatedSnapshot = NULL;
        }
            
    PRINT( _L( "Camera <= CCamSnapShotRotator::RotationCompleteL" ) );         
    }    

// ---------------------------------------------------------------------------
// CCamSnapShotRotator::Rotate
// ---------------------------------------------------------------------------
//  
void CCamSnapShotRotator::RotateL( CFbsBitmap* aBitmap )
    {
    PRINT( _L( "Camera => CCamSnapShotRotator::Rotate" ) ); 
    if ( !iRotatedSnapshot )
        {
        iRotatedSnapshot = new (ELeave)CFbsBitmap();
        }
    iRotatedSnapshot->Create( aBitmap->SizeInPixels(), aBitmap->DisplayMode() );
    TRect tmpRect( TPoint( 0, 0 ), aBitmap->SizeInPixels() );
    TInt tmpLen = tmpRect.Width() * tmpRect.Height() * 4;
    //copy the snapshot data
    iRotatedSnapshot->BeginDataAccess();
    aBitmap->BeginDataAccess();
    Mem::Copy( iRotatedSnapshot->DataAddress(), aBitmap->DataAddress(), tmpLen );
    iRotatedSnapshot->EndDataAccess();
    aBitmap->EndDataAccess();
    if ( !iRotatorAo )
        {
        iRotatorAo = CCamSyncRotatorAo::NewL( *this );
        }    
    iRotatorAo->RotateL( iRotatedSnapshot, CBitmapRotator::ERotation180DegreesClockwise );
        
    PRINT( _L( "Camera <= CCamSnapShotRotator::Rotate" ) );  
    }

// ---------------------------------------------------------------------------
// CCamSnapShotRotator::IsActive
// ---------------------------------------------------------------------------
//  
TBool CCamSnapShotRotator::IsActive()
    {
    TBool ret(EFalse);
    if ( iRotatorAo )
        {
        ret = iRotatorAo->IsActive();
        }
    return ret;
    }

//  End of File  
