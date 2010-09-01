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
* Description:  Document class for Camera*
*/


#ifndef CAMDOCUMENT_H
#define CAMDOCUMENT_H

//  INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CCamAppUi;
class CEikApplication;
class CCamAppController;


// CLASS DECLARATION

/**
*  Document class for the Camera application
*
*  @since 2.8
*/
class CCamDocument : public CAknDocument
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aApp reference to the CCamApp
        * @return pointer to the created CCamDocument
        */
        static CCamDocument* NewL( CEikApplication& aApp );

        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aApp reference to the CCamApp
        * @return pointer to the created CCamDocument
        */
        static CCamDocument* NewLC( CEikApplication& aApp );
        
        /**
        * Destructor.
        * @since 2.8
        */
        ~CCamDocument();

    public: // Functions from base classes

        /**
        * From CAknDocument Create a CCamAppUi object and return a pointer to it.
        * @since 2.8
        * @return A pointer to the instance of the AppUi created
        */
        CEikAppUi* CreateAppUiL();

    private:

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aApp reference to the CCamApp
        */
        CCamDocument(CEikApplication& aApp);


        /**
        * Symbian 2nd phase constructor 
        * @since 2.8
        */
        void ConstructL();

    private:    // Data
        // For communication between the UI and engine.
        CCamAppController* iController;
    };

#endif      // CAMDOCUMENT_H  
            
// End of File
