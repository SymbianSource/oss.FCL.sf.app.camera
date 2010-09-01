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
* Description:  Active object that checks if files have been deleted outside camera
*
*/


// INCLUDE FILES
#include "CamFileCheckAo.h"
#include "camlogging.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamFileCheckAo::CCamFileCheckAo
// C++ constructor
// -----------------------------------------------------------------------------
//
CCamFileCheckAo::CCamFileCheckAo( CCamAppController& aController, MCamFileCheckObserver& aObserver )
    :CActive( EPriorityStandard ), iController( aController ), iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CCamFileCheckAo::~CCamFileCheckAo
// Destructor
// -----------------------------------------------------------------------------
//
CCamFileCheckAo::~CCamFileCheckAo()
    {
    PRINT( _L("Camera => CCamFileCheckAo::~CCamFileCheckAo()") );
    Cancel();
    PRINT( _L("Camera <= CCamFileCheckAo::~CCamFileCheckAo()") );
    }


// -----------------------------------------------------------------------------
// CCamFileCheckAo::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamFileCheckAo* CCamFileCheckAo::NewL( CCamAppController& aController, MCamFileCheckObserver& aObserver )
    {
    CCamFileCheckAo* self = new( ELeave ) CCamFileCheckAo( aController, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CCamFileCheckAo::ConstructL
// 2nd phase construction
// -----------------------------------------------------------------------------
//
void CCamFileCheckAo::ConstructL()
   {
   CActiveScheduler::Add( this );
   }

// -----------------------------------------------------------------------------
// CCamFileCheckAo::DoCancel
// Cancels the active object
// -----------------------------------------------------------------------------
//
void CCamFileCheckAo::DoCancel()
    {
    PRINT( _L("Camera => CCamFileCheckAo::DoCancel()") );
    }

// -----------------------------------------------------------------------------
// CCamFileCheckAo::RunL
// Check existance of next file in burstcapturearray.
// -----------------------------------------------------------------------------
//
void CCamFileCheckAo::RunL()
    {
    PRINT1( _L("Camera => CCamFileCheckAo::RunL()  findex=%d"), iFileIndex )

    //Check to prevent crash. Some items from array might have been deleted
    //before this object have been activated...Or by different thread, active object and so on...
    if ( iFileIndex < iArray->Count() && iFileIndex >= 0 )
        {
        if ( !BaflUtils::FileExists( iFs, iArray->FileName( iFileIndex ) ) 
             && iArray->FileName( iFileIndex ) != KNullDesC )
            {
            PRINT1( _L("Camera <> CCamFileCheckAo::RunL: File %s didn't exist"),iArray->FileName( iFileIndex ).Ptr() )
            iArray->SetDeleted( iFileIndex, ETrue );             
            iFileCount--;
            
            // Avoid instant view switch from postcapture
            // to precapture when the file was deleted
            // in Photos or File Manager while camera
            // postcapture view was in background.
            User::After( 150000 ); 
            }  	
        }
    else
        {
        PRINT( _L("Camera <> CCamFileCheckAo::RunL emptyrun") )
        iFileIndex = iArray->Count();
        iFileCount = iFileIndex;
        iEmptyRun=ETrue;
        }

    if( iFileIndex > 0 && iFileCount > 0 )
        {
        if( iEmptyRun )
            {
            if( iFileIndex > iFileCount)
                {
                iFileIndex = iFileCount;	
                }
            else
                {
                iFileIndex--;	
                }
            }
        else
            {
            iFileIndex--;          	
            }
        PRINT( _L("Camera <> CCamFileCheckAo::RunL: Set active again.") )
        StartRequest();
        }
    else
        {
        //File check is complete. Don't set active anymore.
        //Inform observer about result.
        TRAPD( err, iObserver.FileCheckingCompleteL( iFileCount, KErrNone ) );
        if( err != KErrNone )
            {
            User::Leave( err );
            }
        }
    }



// -----------------------------------------------------------------------------
// CCamFileCheckAo::Start
// Initializes starting index and set active..
// -----------------------------------------------------------------------------
//
void CCamFileCheckAo::Start()
    {
    PRINT( _L("Camera => CCamFileCheckAo::Start()") );
    iArray = iController.BurstCaptureArray();
    iFileCount = iArray->Count();
    iFileIndex = iFileCount;
    iFs = CEikonEnv::Static()->FsSession();

    StartRequest();
    PRINT( _L("Camera <= CCamFileCheckAo::Start()") );
    }

// -----------------------------------------------------------------------------
// CCamFileCheckAo::StartRequest
// Set active and starts new request immediately
// -----------------------------------------------------------------------------
//
void CCamFileCheckAo::StartRequest()
    {
    iEmptyRun = EFalse;
    SetActive();
    TRequestStatus* statusPtr = &iStatus;
    User::RequestComplete( statusPtr, KErrNone );
    }

// -----------------------------------------------------------------------------
// CCamFileCheckAo::RunError
// Called when an error has occurred.
// -----------------------------------------------------------------------------
//
TInt CCamFileCheckAo::RunError( TInt aError )
    {
    PRINT( _L("Camera => CCamFileCheckAo::RunError()") );
    
    TRAP_IGNORE( iObserver.FileCheckingCompleteL( 0, aError ) )
    PRINT( _L("Camera <= CCamFileCheckAo::RunError()") );
    return KErrNone;
    }

//  End of File  
