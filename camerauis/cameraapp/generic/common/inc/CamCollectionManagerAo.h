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
* Description:  Implementation of Asynchronous Operation of Add to Album
*/

#ifndef __CAMCOLLECTIONMANAGERAO_H__
#define __CAMCOLLECTIONMANAGERAO_H__

#include <e32base.h>

class CGlxCollectionInfo;
class CGlxCollectionManager;
class MCamAddToAlbumObserver;

class CCamCollectionManagerAO : public CActive
    {
    public:  // Constructors and destructor
        
       
        /**
        * Destructor.
        */
        ~CCamCollectionManagerAO();
        
        /**
        * Constructor
        * @aObserver Observer for compeleted/failed operations.
        */
        CCamCollectionManagerAO(  MCamAddToAlbumObserver& aObserver );

    public: // New Method

        /**
        * Initiates Add To Album Operation
        * @param aSelectedAlbumIds Array of ids for albums where to add.
        * @param aUri              Filename of the image / video to add.
        */
        void StartCallBackAddToAlbumL( const RArray<TUint32>& aSelectedAlbumIds, 
                                       const TDesC& aUri );
        
        /**
        * Initiate Check for Album Operation
        * @param aDefaultAlbumId Album id.
        */
        void CheckForAlbumExistenceL( const TUint32 aDefaultAlbumId );

    public: // CActive

        /**
        * Perform the next scheduled task
        * @since 2.8
        */
        void RunL();
        
        /**
        * Cancels the active object
        * @since 2.8
        */
        void DoCancel();
        
        /**
        * Handles a leave occuring in the request completion event handler
        * @since 2.8
        * @param aError Error (leave) code.
        */
        TInt RunError( TInt aError );

    private:

        /**
        * Performs the necessary add to album request
        * Using the collection manager.
        */
        void DoAddToAlbumOperationL();
        
        /**
        * We Perform the needed operation after we complete
        * Checking for album existence
        */
        void DoCheckAlbumOperationCompleteL();
        
        /**
        * Returns the Default Album Id from the CenRep key
        */
        TUint32 DefaultAlbumIdFromCenRepL();
        
    private:

        CGlxCollectionInfo* iCollectionInfo;
        CGlxCollectionManager* iCollectionManager;
        MCamAddToAlbumObserver& iObserver;
        TInt iCurrentAlbumIndex;
        RArray<TUint32> iSelectedAlbumIds;
        TFileName iUri;
        TBool iAddToAlbum;
        TBool iCheckForAlbum;
    };


#endif // __CAMCOLLECTIONMANAGERAO_H__
