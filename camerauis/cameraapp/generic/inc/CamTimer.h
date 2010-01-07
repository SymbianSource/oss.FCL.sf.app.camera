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
* Description:  Active object for implementing viewfinder timeout and*
*/


#ifndef CAMTIMER_H
#define CAMTIMER_H

// INCLUDES
#include "CamTimerBase.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Active object for implementing viewfinder and post capture timeout
*/
class CCamTimer : public CCamTimerBase
    {
    public:  // Constructors and destructor
        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aTimeout timer period
        * @param aCallBack Method to call into when timer completes
        * @return pointer to the created CCamTimer
        */
        static CCamTimer* NewL( TInt aTimeout, TCallBack aCallBack );

        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aTimeout timer period
        * @param aCallBack Method to call into when timer completes
        */
        static CCamTimer* NewLC( TInt aTimeout, TCallBack aCallBack );

        /**
        * Destructor.
        */
        virtual ~CCamTimer();

    public: // From CCamTimerBase
        /**        
        * Start timer.
        */
        void StartTimer();
     
        /**        
        * Set the timeout period
        * @since 3.0
        * @param aTimeout new timer period
        */   
        void SetTimeout( TInt aTimeout );

    private: // Functions from base classes
        /**
        * From CActive, handle timeout expiration
        */
        void RunL();

    private: // New functions
        /**
        * C++ constructor
        * @since 2.8
        * @param aTimeout timer period
        * @param aCallBack Method to call into when timer completes
        */
        CCamTimer( TInt aTimeout, TCallBack aCallBack  );

        /**
        * Symbian 2nd phase constructor
        * @since 2.8
        */
        void ConstructL();

    private:
        TUint iTimeout;
        TCallBack iCallBack;
    };

#endif

// End of File
