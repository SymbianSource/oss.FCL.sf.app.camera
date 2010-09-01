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
* Description:  Handles system agent events*
*/



#ifndef CAMSAEVENTACTIVE_H
#define CAMSAEVENTACTIVE_H

//  INCLUDES
#include <saclient.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CCamAppController;

// CLASS DECLARATION

/**
* Active Object listening to events coming from System Agent
* @since 2.8
*/

class CCamSAEventActive : public CActive 
    {
    public: // Constructors and destructor
        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to CCamAppController instance
        * @param aSystemAgent System agent handle
        */
        CCamSAEventActive( CCamAppController& aController,
                            const RSystemAgent& aSystemAgent );

        /**
        * Destructor.
        */
        virtual ~CCamSAEventActive();

    public:
        /**
        * Issue a request to receive events from System Agent
        * @since 2.8
        */
        void IssueRequest();

    private: // Functions from base classes
        /**
        * From CActive, called when RunL() is trapped by Active Scheduler
        * @since 2.8
        * @param aError the error code trapped from RunL()
        * @return TInt KErrNone
        */
        TInt RunError( TInt aError );   

        /**
        * From CActive called when an event comes from System Agent
        * Calls HandleSAEvent in Controller to handle the event
        * @since 2.8
        */
        void RunL();

        /**
        * From CActive Cancel request
        * @since 2.8
        */
        void DoCancel();

    private:
        CCamAppController& iController;
        RSystemAgent iSystemAgent;
        TSysAgentEvent iSAEvent;
        TBool iRequestIssued;
    };

#endif  // CAMSAEVENTACTIVE_H   
            
// End of File
