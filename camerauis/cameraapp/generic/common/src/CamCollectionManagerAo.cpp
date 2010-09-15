/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of Collection Manager asynchronous operation
*  
*/


#include "CamCollectionManagerAo.h"
#include <glxcollectionmanager.h>
#include <glxcollectioninfo.h>
#include "MCamAddToAlbumObserver.h"
#include "CamLogger.h"
#include "Cam.hrh"
#include "CamUtility.h"


// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::CCamCollectionManagerAO()
// ---------------------------------------------------------------------------
//
CCamCollectionManagerAO::CCamCollectionManagerAO( MCamAddToAlbumObserver& aObserver )
:CActive(EPriorityStandard),iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::~CCamCollectionManagerAO()
// ---------------------------------------------------------------------------
//
CCamCollectionManagerAO::~CCamCollectionManagerAO()
    {
    Cancel();
    if ( iCollectionManager )
        {
        delete iCollectionManager;
        iCollectionManager = NULL;
        }
    if ( iCollectionInfo )
        {
        delete iCollectionInfo;
        iCollectionInfo = NULL;
        }
    iSelectedAlbumIds.Close();
    }

// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::StartAddtoAlbumOperationL
//
// Initiates Callback Mechanism for adding Images/Videos to albums
// ---------------------------------------------------------------------------
//
void CCamCollectionManagerAO::StartCallBackAddToAlbumL(
                              const RArray<TUint32>& aSelectedAlbumIds, 
                              const TDesC& aUri 
                              )
    {
    PRINT( _L("Camera => CCamCollectionManagerAO::StartCallBackAddToAlbumL") );
    PRINT( _L("Camera <> CCamCollectionManagerAO::StartCallBackAddToAlbumL ... Calling CGlxCollectionManager::NewL()") );
    if (!iCollectionManager)
        {
        iCollectionManager = CGlxCollectionManager::NewL();
        }
    PRINT( _L("Camera <> CCamCollectionManagerAO::StartCallBackAddToAlbumL ... CGlxCollectionManager::NewL() Success ") );
    iUri = aUri;
    iSelectedAlbumIds.Reset();

    for ( TInt i = 0; i < aSelectedAlbumIds.Count(); i++ )
        {
        iSelectedAlbumIds.AppendL( aSelectedAlbumIds[i] );
        }
    iCurrentAlbumIndex = 0;
    PRINT( _L("Camera <> CCamCollectionManagerAO::StartCallBackAddToAlbumL ... Calling iCollectionManager->AddToCollectionL()") );
    iCollectionManager->AddToCollectionL(
                        aUri, 
    			        aSelectedAlbumIds[iCurrentAlbumIndex], 
    			        iStatus );
    PRINT( _L("Camera <> CCamCollectionManagerAO::StartCallBackAddToAlbumL ... iCollectionManager->AddToCollectionL() : Success") );
    iCurrentAlbumIndex++;
    iAddToAlbum = ETrue;
    SetActive();
    PRINT( _L("Camera <= CCamCollectionManagerAO::StartCallBackAddToAlbumL") );
    }



// ---------------------------------------------------------------------------
//  CCamCollectionManagerAO::CheckForAlbumExistenceL
//
//  Initiates Callback Mechanism for checking if the album really exists
// ---------------------------------------------------------------------------
//
void CCamCollectionManagerAO::CheckForAlbumExistenceL( 
                                       const TUint32 aDefaultAlbumId )
    {
    PRINT( _L("Camera => CCamCollectionManagerAO::CheckForAlbumExistenceL") );
    if (!iCollectionManager)
        {
        iCollectionManager = CGlxCollectionManager::NewL();
        }
    if ( iCollectionInfo )
        {
        delete iCollectionInfo;
        iCollectionInfo = NULL;
        }
    iCollectionInfo = CGlxCollectionInfo::NewL( aDefaultAlbumId );
    iCollectionManager->CollectionInfoL( *iCollectionInfo, iStatus );
    iCheckForAlbum = ETrue;
    SetActive();
    PRINT( _L("Camera <= CCamCollectionManagerAO::CheckForAlbumExistenceL") );
    }
   
// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::RunL()
// ---------------------------------------------------------------------------
//
void CCamCollectionManagerAO::RunL()
    {
    PRINT( _L("Camera => CCamCollectionManagerAO::RunL") );
    if ( iAddToAlbum )
        {
        iAddToAlbum = EFalse;
        if ( iStatus.Int() == KErrNone)
    	    {
    	    PRINT( _L("Camera <> CCamCollectionManagerAO::RunL() Finalizing") );
            DoAddToAlbumOperationL();
    	    }
    	else
    	    {
    	    // if there is any error, we pass and handle there
    	    PRINT( _L("Camera <> CCamCollectionManagerAO::RunL() add to album has problems") );
    	    RunError( iStatus.Int() );
    	    }
        }
     else if ( iCheckForAlbum )
        {
        iCheckForAlbum = EFalse;
        if ( iStatus.Int() == KErrNone)
    	    {
            DoCheckAlbumOperationCompleteL();
    	    }
    	else
    	    {
    	    // if there is any error, we pass and handle there
    	    RunError( iStatus.Int() );
    	    }
        }
     else
        {
        // Handling special cases if any
        }
    PRINT( _L("Camera <= CCamCollectionManagerAO::RunL") );
    }
 

// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::DoAddToAlbumOperationL()
// ---------------------------------------------------------------------------
//
void CCamCollectionManagerAO::DoAddToAlbumOperationL()
    {
    PRINT( _L("Camera => CCamCollectionManagerAO::DoAddToAlbumOperationL") );
    PRINT1( _L("Camera <> CCamCollectionManagerAO::DoAddToAlbumOperationL ... iCurrentAlbumIndex (%d)"), iCurrentAlbumIndex )
    PRINT1( _L("Camera <> CCamCollectionManagerAO::DoAddToAlbumOperationL ... iSelectedAlbumIds.Count() (%d)"), iSelectedAlbumIds.Count() )
	if ( iCurrentAlbumIndex < iSelectedAlbumIds.Count() )
        {
        PRINT( _L("Camera <> CCamCollectionManagerAO::DoAddToAlbumOperationL ... Enters If Loop") );
        iCollectionManager->AddToCollectionL(
                            iUri, 
    			            iSelectedAlbumIds[iCurrentAlbumIndex], 
    			            iStatus );
        iCurrentAlbumIndex++;
        iAddToAlbum = ETrue;
    	SetActive();
        }
    else
        {
        PRINT( _L("Camera <> CCamCollectionManagerAO::DoAddToAlbumOperationL ... Enters Else Loop") );
        iAddToAlbum = EFalse;
        iSelectedAlbumIds.Reset();
        iObserver.AddToAlbumIdOperationCompleteL();
        }
    PRINT1( _L("Camera <= CCamCollectionManagerAO::DoAddToAlbumOperationL ... Before leaving iCurrentAlbumIndex (%d)"), iCurrentAlbumIndex )
    }



// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::CheckAlbumOperationComplete
// ---------------------------------------------------------------------------
//
void CCamCollectionManagerAO::DoCheckAlbumOperationCompleteL()
    {
    PRINT( _L("Camera => CCamCollectionManagerAO::DoCheckAlbumOperationCompleteL") );
    if ( iCollectionInfo )
        {
   	    iObserver.CheckAlbumIdOperationCompleteL( iCollectionInfo->Exists(),
    	                                          iCollectionInfo->Title() );
        }
    PRINT( _L("Camera <= CCamCollectionManagerAO::DoCheckAlbumOperationCompleteL") );
    }


// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::DoCancel()
//
// Cancels the active object
// ---------------------------------------------------------------------------
//
void CCamCollectionManagerAO::DoCancel()
    {
    PRINT( _L("Camera => CCamCollectionManagerAO::DoCancel") );
    if ( iCollectionManager )
        {
        iCollectionManager->Cancel();
        }
    iSelectedAlbumIds.Reset();
    PRINT( _L("Camera <= CCamCollectionManagerAO::DoCancel") );
    }


// ---------------------------------------------------------------------------
// CCamCollectionManagerAO::RunError()
//
// Handles all leaves and its safe to handle them in this method
// ---------------------------------------------------------------------------
//
TInt CCamCollectionManagerAO::RunError( TInt /*aError*/ )
    {
    PRINT( _L("Camera => CCamCollectionManagerAO::RunError") );
    // We make sure that if any error occurs, we cancel,
    // the ongoing request and delete all the pointers
    // so that no memory leaks occurs
    if ( iCollectionManager )
        {
        delete iCollectionManager;
        iCollectionManager = NULL;
        }
    if ( iCollectionInfo )
        {
        delete iCollectionInfo;
        iCollectionManager = NULL;
        }
    iSelectedAlbumIds.Reset();

    PRINT( _L("Camera <= CCamCollectionManagerAO::RunError") );
    return KErrNone;
    }

//End of file
