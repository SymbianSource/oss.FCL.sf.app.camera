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
* Description:  Class for new file service application*
*/



#ifndef CAMSERVER_H
#define CAMSERVER_H

// INCLUDES

#include <CNewFileServiceBase.h>
#include <AknServerApp.h>

// CLASS DECLARATION

/**
* Application server class.
* Provides factory to create concrete file server application object.
* @since 3.0
*/
class CCamAppServer : public CAknAppServer
    {
public:  // from AknAppServer

    IMPORT_C CApaAppServiceBase* CreateServiceL( TUid aServiceType) const;   
           			
public:
    /*
	* Returns pointer to new file server application
	* @since 3.0
	*/
	MNewFileServiceObserver* NewFileServiceObserver() const;    
    
private:
    mutable MNewFileServiceObserver* iNewFileServiceObserver;
    };

#endif // CAMSERVER_H
// End of File

