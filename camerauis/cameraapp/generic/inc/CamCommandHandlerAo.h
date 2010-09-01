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
* Description:  Active object for handling toolbar commands
*
*/


#ifndef CAMCOMMANDHANDLERAO_H
#define CAMCOMMANDHANDLERAO_H

//  INCLUDES
 
#include <e32base.h>

// FORWARD DECLARATIONS
class CAknView;

/**
*  Class to manage the asynchronous command handling
*  @since S60 v5.0
*/
class CCamCommandHandlerAo : public CActive
    {
	public: // Constructors and destructor
		
		~CCamCommandHandlerAo();
		
        /**
        * Two-phased constructor.
        * @since S60 v5.0	
        */	
		static CCamCommandHandlerAo* NewL( CAknView* aView );
		
	private:
	
        /**
        * Constructor
        * @since S60 v5.0
        */	
		CCamCommandHandlerAo( CAknView* aView );
		
		/**
        * 2nd phase construction
        * @since S60 v5.0
        */
        void ConstructL();

        /**
        * Cancels the active object
        * @since S60 v5.0
        */
        void DoCancel();

        /**
        * Perform the next scheduled task
        * @since S60 v5.0
        */
        void RunL();

        /**
        * Perform error reporting and cleanup
        * @since S60 v5.0
        */
		TInt RunError( TInt aError );
		
	public:
		
		/**
		* Adds the command to queue and sets active
		* @since S60 v5.0
		*/		
		void HandleCommandL( TInt aCommandId );
		
	private:
		
		// Not own
		CAknView* iView;
		
   		RArray< TInt > iCommandQueue;
   
    };

#endif      // CAMCOMMANDHANDLERAO_H    
    
// End of File    
    
	
				
		