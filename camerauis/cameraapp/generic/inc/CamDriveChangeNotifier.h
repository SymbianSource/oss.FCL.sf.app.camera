/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*
*/



#ifndef CAMDISKCHANGELISTENER_H
#define CAMDISKCHANGELISTENER_H

//  INCLUDES
#include <f32file.h>
#include "campropertywatcher.h"
#include "campropertyobserver.h"

// FORWARD DECLARATIONS
class MCamDriveChangeNotifierObserver;

// CLASS DECLARATION

/**
*  Mmc Change Notifier.
*
*  @since 5.0
*/
class MCamDriveChangeNotifierObserver
    {
    public:
        enum TCamDriveChangeType
            {
            EDriveDismount,
            EDriveMount,
            EDriveUSBMassStorageModeOn,
            EDriveUSBMassStorageModeOff
            };
            
    public: // New functions

        /**
        * This method is used to notify changes.
        */
        virtual TInt DriveChangeL( TCamDriveChangeType aType ) = 0;
        

    protected:

        /**
        * Destructor.
        */
        virtual ~MCamDriveChangeNotifierObserver() {}

    };

/**
*  Mmc Change Notifier.
*
*  @since 5.0
*/
class CCamDriveChangeNotifier : public CBase, 
                                public MPropertyObserver

    {
    private:
        class CCamDiskChangeListener : public CActive
            {            
            public:
                static CCamDiskChangeListener* NewLC( 
                     RFs& aFs,
                     TDriveNumber aDrive,
                     CCamDriveChangeNotifier& aObserver );
                ~CCamDiskChangeListener();
                
                void Start();
                void Stop();
            
            protected:
                CCamDiskChangeListener( 
                     RFs& aFs,
                     TDriveNumber aDrive,
                     CCamDriveChangeNotifier& aObserver );
                
            private:
                /**
                * From CActive
                */
                void DoCancel();

                /**
                * From CActive
                */
                void RunL();
                
            private:
                RFs& iFs;
                TDriveNumber iDrive;
                CCamDriveChangeNotifier& iObserver;
            };
        
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CCamDriveChangeNotifier* NewL(
                                RFs& aFs,
                                MCamDriveChangeNotifierObserver& aObserver  );

        /**
        * Destructor.
        */
        ~CCamDriveChangeNotifier();

    public:
        /**
        * Called from listeners
        */
        TInt NotifyChangeL( 
                MCamDriveChangeNotifierObserver::TCamDriveChangeType aType );

        /**
        * Called by the observer when pending dismount can be done.
        */     
        void SendAllowDismount();        

        // New functions

        void StartMonitoring();
        void CancelMonitoring();

    public: // From MPropertyObserver
        /**
        * The value of a watched property has changed
        * @param aCategory The category of the property
        * @param aKey the Identifier of the property
        */    
        void HandlePropertyChangedL( const TUid& aCategory, const TUint aKey );
        
    private:

        /**
        * C++ default constructor.
        */
        CCamDriveChangeNotifier( 
                                RFs& aFs,
                                MCamDriveChangeNotifierObserver& aObserver );
        
        void ConstructL();

    private:    // Data
        // Ref.
        MCamDriveChangeNotifierObserver& iObserver;

        // Ref. File server session
        RFs& iFs;

        RPointerArray<CCamDiskChangeListener> iListeners;
        
        CCamPropertyWatcher* iUsbMSWatcher;
        
        TBool iMassStorageModeOn;

    };

#endif      // CAMDISKCHANGELISTENER_H

// End of File
