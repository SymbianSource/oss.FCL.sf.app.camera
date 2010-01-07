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
* Description:  Listens for changes to the call state*
*/


#ifndef CAMCALLSTATEAO_H
#define CAMCALLSTATEAO_H

//  INCLUDES
#include <e32property.h>
#include <ctsydomainpskeys.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  
*  Mix-in class specifying the callback interface to be used with CCamCallStateAo
*
*  @since 3.0
*/
class MCamCallStateObserver
    {
    public:
        /**
        * Callback function, called when the call state changes
        * @since 3.0
        * @param aState The new call state
        * @param aError The error associated with request completion
        */    
        virtual void CallStateChanged( TPSCTsyCallState aState, TInt aError ) = 0;
    };
    

/**
*  Active Object class to watch the call state
*
*  @since 3.0
*/
class CCamCallStateAo : public CActive

    {
    public:  // Constructors and destructor        
        /**
        * Two-phased constructor.
        * @since 3.0
        * @param aObserver The observer requesting callback on call events
        * @return Constructed instance of the class
        */
        static CCamCallStateAo* NewL( MCamCallStateObserver* aObserver );
            
        /**
        * Destructor.
        */
        ~CCamCallStateAo();
        
    protected:  // Functions from base classes        
        /**
        * From CActive
        * Cancels an outstanding request
        * @since 3.0
        */
    	void DoCancel();		

        /**
        * From CActive
        * Called on completion of a request
        * @since 3.0
        */
    	void RunL();

        /**
        * From CActive
        * Called when an error occurs during a request
        * @since 3.0
        * @param aError The Epoc-error code for the error.
        * @return KErrNone
        */
    	TInt RunError( TInt aError );    	        
    
    private:
        /**
        * C++ default constructor.
        * @since 3.0
        * @param aObserver The observer requesting callback on call events
        */
        CCamCallStateAo( MCamCallStateObserver* aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    	

    public:     // Data
        // The observer to be notified on an event
        MCamCallStateObserver* iObserver;

        // Publish and Subscribe property handle
        RProperty iProperty;
    };

#endif      // CAMCALLSTATEAO_H
            
// End of File
