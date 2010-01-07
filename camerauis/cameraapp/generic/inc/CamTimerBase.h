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
* Description:  Mixin class for implementing viewfinder timeout and*
*/


#ifndef CAMTIMERBASE_H
#define CAMTIMERBASE_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Active object for implementing viewfinder and post capture timeout
*/
class CCamTimerBase : public CTimer
    {
    public: // New functions
        /**        
        *	Start timer.
        */
        virtual void StartTimer() = 0;
        
        /**        
        * Set the timeout period
        * @since 3.0
        * @param aTimeout new timer period
        */    
        virtual void SetTimeout( TInt aTimeout ) = 0;

    protected:

        inline CCamTimerBase( TInt aPriority );
    };

#include "CamTimerBase.inl"

#endif // CAMTIMERBASE_H

// End of File
