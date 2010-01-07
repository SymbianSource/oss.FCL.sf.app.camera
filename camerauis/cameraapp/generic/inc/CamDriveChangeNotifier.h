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
            EDriveMount
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
class CCamDriveChangeNotifier : public CBase

    {
    private:
        class CCamDiskChangeListener : public CActive
            {            
            public:
                static CCamDiskChangeListener* NewLC( 
                     RFs& aFs,
                     TDriveNumber aDrive,
                     MCamDriveChangeNotifierObserver::TCamDriveChangeType aType,
                     CCamDriveChangeNotifier& aObserver );
                ~CCamDiskChangeListener();
                
                void Start();
                void Stop();
            
            protected:
                CCamDiskChangeListener( 
                     RFs& aFs,
                     TDriveNumber aDrive,
                     MCamDriveChangeNotifierObserver::TCamDriveChangeType aType,
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
                MCamDriveChangeNotifierObserver::TCamDriveChangeType iType;
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

    private:

        /**
        * C++ default constructor.
        */
        CCamDriveChangeNotifier( 
                                RFs& aFs,
                                MCamDriveChangeNotifierObserver& aObserver );

    private:    // Data
        // Ref.
        MCamDriveChangeNotifierObserver& iObserver;

        // Ref. File server session
        RFs& iFs;

        RPointerArray<CCamDiskChangeListener> iListeners;

    };

#endif      // CAMDISKCHANGELISTENER_H

// End of File
