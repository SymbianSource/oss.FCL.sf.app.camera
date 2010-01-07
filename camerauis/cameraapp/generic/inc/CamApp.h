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
* Description:  Main application class*
*/




#ifndef CAMAPP_H__
#define CAMAPP_H__

//  INCLUDES
#include <aknapp.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Application class.
* Provides factory to create concrete document object.
*
*  @since 2.8
*/

class CCamApp : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CAknApplication Return the application UID value.
        * @since 2.8
        * @return UID of the application
        */
        TUid AppDllUid() const;
        
    protected:  // Functions from base classes
        
        /**
        * From CAknApplication Create an instance of CApaDocument
		* and return a pointer to it.
        * @since 2.8
        * @return Newly created CCamDocument instance
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CEikApplication
		* Creates first-stage constructed file server application
		* @since 3.0
		* @param aAppServer pointer to AppServer object
		*/
		void NewAppServerL( CApaAppServer*& aAppServer );		

    };

#endif      // __CAMAPP_H__   
            
// End of File
