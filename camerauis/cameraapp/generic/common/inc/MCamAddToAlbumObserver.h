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
* Description:  notifier of mc photos operation.
 *
*/



#ifndef MCAMADDTOALBUMOBSERVER_H
#define MCAMADDTOALBUMOBSERVER_H


/**
* Abstract API for notifying the obersvers for completion event
* Once the Add to Album operation is complete via Collection 
* Manager
*/
class MCamAddToAlbumObserver
  {
  public:

    /**
    * Notification once the asynchronous operations using
    * CollectionManager interface have completed.
    *
    * @param aAlbumExists ETrue if album exists
    * @param aAlbumTitle  Title of the album
    */
    virtual void CheckAlbumIdOperationCompleteL(
            TBool aAlbumExists, const TDesC& aAlbumTitle ) = 0;

    /**
    * Notification once the asynchronous operations using
    * CollectionManager interface have completed.
    */
    virtual void AddToAlbumIdOperationComplete() = 0;
                                        
    };

#endif // MCAMADDTOALBUMOBSERVER_H

            
// End of File
