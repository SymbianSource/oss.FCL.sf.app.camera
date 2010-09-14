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
* Description:  Class for file server service.
*/


#ifndef __CAMNEWFILESERVICE_H__
#define __CAMNEWFILESERVICE_H__

// INCLUDES
#include <CNewFileServiceBase.h>

// CONSTANTS

// CLASS DECLARATION

/**
* An observer class used to notify objects that embedded recording
* was completed
* @since 3.0
*/
class MCamEmbeddedObserver 
    {

    public: // new methods

        /**
        * This function is called when application wants to notify
        * the observer that embedded image or clip was recorded
        * @param aName The name of the file that was recorded
        */
        virtual void FileCompleteL( const TDesC& aName ) = 0;
        
        /**
        * This function may be used to inform server to abort transfer.
        * If operation already has completed, nothing is done.
        */
        virtual void AbortL() = 0;
    };

/**
* Class for file server service. (embedding) 
* @since 3.0
*/
class CCamNewFileService: public CNewFileServiceBase, 
                          public MCamEmbeddedObserver
    {

    public:
        /**
        * Constructor.
        * @since 3.0
        */
        static CCamNewFileService* NewL();
        
        /**
        * Destructor.
        */
        ~CCamNewFileService();
        
        
        /*
        * Receive message from service to record new files
        * @since 3.0
        * @param aObserver service observer
        * @param aFileNameArray array for filename
        * @param aType service type (image or video)
        * @param aMultipleFiles if False only one file possible
        */
        void HandleNewFileL( MNewFileServiceObserver* aObserver,
                                      CDesCArray& aFilenameArray,
                                      TNewServiceFileType aType,
                                      TBool aMultipleFiles );
        /*
        * Receive message from service to record new files
        * @since 3.0
        * @param aObserver service observer
        * @param aFile file to record clip
        * @param aType service type (image or video)
        */
        void HandleNewFileL( MNewFileServiceObserver* aObserver,
                                      RFile& aFile,
                                      TNewServiceFileType aType );
                                      
            /*
        * Receive message from service that file service has completed
        * and can be closed
        * @since 3.0
        */
        void ServiceCompleteL();
        
    public: // MCamEmbeddedObserver     
        
        /*
        * Receive comand from application that file has been recorded
        * @since 3.0
        * @param aName file name and location
        */
        void FileCompleteL( const TDesC& aName );
        
        /*
        * Receive command from application that file service must be aborted
        * @since 3.0
        */
        void AbortL();                  

    private: // data
        MNewFileServiceObserver* iObserver;
        CDesCArray* iCamFilenameArray;	// renamed from iFilenameArray to avoid Lint warning about
                                        // the same private member name used in the base class
        TBool iCompleted;   
        CActiveSchedulerWait iActiveWait;
        
    };
    

#endif // __CAMNEWFILESERVICE_H__

