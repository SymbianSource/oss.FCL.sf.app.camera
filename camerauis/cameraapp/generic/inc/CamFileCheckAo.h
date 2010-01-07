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


#ifndef CAMFILECHECKAO_H
#define CAMFILECHECKAO_H

//  INCLUDES
#include <e32base.h>
#include <bautils.h>
#include "CamAppController.h"
#include "CamBurstCaptureArray.h"


/**
* Abstract API for file check observer. 
* Provides an asynchronous 'callback' method to inform derived classes
* that the file checking operation has been completed.
*
*  @since 2.8
*/
class MCamFileCheckObserver
    {
    public:
        
        /**
        * Callback to notify that file checking operation has completed.
        * @param aStatus Number of files that were missing
        * @param aError  Error code
        */
        virtual void FileCheckingCompleteL( TInt aStatus, TInt aError ) = 0;
    };

/**
*  Class to manage the asynchronous file deletion checking 
*  @since 2.8
*/
class CCamFileCheckAo : public CActive
    {
    public:  // Constructors and destructor
        ~CCamFileCheckAo();
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aObserver Informed when image saving has completed
        */
        static CCamFileCheckAo* NewL( CCamAppController& aController, MCamFileCheckObserver& aObserver );

	void Start();
	
	
    private:

        /**
        * Constructor
        * @since 2.8
        * @param aObserver Informed when image saving has completed
        */
        CCamFileCheckAo( CCamAppController& aController, MCamFileCheckObserver& aObserver  );

        /**
        * 2nd phase construction
        * @since 2.8
        */
        void ConstructL();

        /**
        * Cancels the active object
        * @since 2.8
        */
        void DoCancel();

        /**
        * Perform the next scheduled task
        * @since 2.8
        */
        void RunL();


        /**
        * Perform error reporting and cleanup
        * @since 2.8
        */
	TInt RunError( TInt aError );

	/**
	*
	*/
	void StartRequest();
	
    private:

        CCamAppController& iController;
        MCamFileCheckObserver& iObserver; // informed when file checking has completed
        
        TInt iFileIndex;
        TInt iFileCount;
        CCamBurstCaptureArray* iArray;
        RFs iFs;
        TBool iEmptyRun;	
    };

#endif      // CAMFILECHECKAO_H
            
// End of File
