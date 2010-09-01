/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active object used to emulate the auto focus custom extension*
*/


#ifndef TESTAFEXTENSION_H
#define TESTAFEXTENSION_H

// INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class CCamAppControllerBase;

#include "cam.hrh"

// CLASS DECLARATION

/**
* Stub implementation for engine side autofocus notification
*/
class CTestAFExtension : public CTimer
    {
    public:  // Constructors and destructor
        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aController reference to CamController
        * @param aFailAutoFocus if ETrue autofocus operations will fail
        * @return pointer to a newly instantiated CTestAFExtension object
        */
        static CTestAFExtension* NewL( CCamAppControllerBase& aController, TBool aFailAutoFocus );

        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aController reference to CamController
        * @param aFailAutoFocus if ETrue autofocus operations will fail
        * @return pointer to a newly instantiated CTestAFExtension object
        */
        static CTestAFExtension* NewLC( CCamAppControllerBase& aController, TBool aFailAutoFocus );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CTestAFExtension();

    public: // New functions
        /**        
        * Start timer.
        * @since 2.8
        */
        void Start();

#ifdef CAMERAAPP_UNIT_TEST
        /**        
        * Set whether or not the autofocus operation will fail
        * @since 2.8
        * @param aFailAutoFocus autofocus fails if ETrue
        */
        void SetFailAutoFocus( TBool aFailAutoFocus );
#endif

    public: // Functions from base classes
        /**
        * From CActive, handle timeout expiration
        * @since 2.8
        */
        void RunL();

    private: // New functions
        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to CamController
        * @param aFailAutoFocus if ETrue autofocus operations will fail
        */
        CTestAFExtension( CCamAppControllerBase& aController, TBool aFailAutoFocus );

        /**
        * Symbian 2nd phase constructor
        * @since 2.8
        */
        void ConstructL();

    private:
        // Reference to the application controller
        CCamAppControllerBase& iController; 
        // Whether or not autofocus operations should fail
        TBool iFailAutoFocus;
    };

#endif

// End of File
