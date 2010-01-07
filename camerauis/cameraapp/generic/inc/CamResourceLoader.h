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
* Description:  Defines abstract interface for classes that load resource data*
*/



#ifndef CAMRESOURCELOADER_H
#define CAMRESOURCELOADER_H

//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Abstract API for MCamResourceLoader
*
*  @since 3.0
*/
class MCamResourceLoader
    {
    public:
    
        /**
        * Read in all resource-based data that the class needs
        * @since 3.0
        */
        virtual void LoadResourceDataL() = 0;
        
        /**
        * Free all resources created by LoadResourceData() 
        * @since 3.0
        */
        virtual void UnloadResourceData() = 0;
        
        /**
        * Forces class to reread all resource information
        * Should be equivalent of {UnloadResourceData(); LoadResourceData()}
        * @since 3.0
        */
        virtual void ReloadResourceDataL() = 0;
    };

#endif      // CAMRESOURCELOADER_H
            
// End of File
