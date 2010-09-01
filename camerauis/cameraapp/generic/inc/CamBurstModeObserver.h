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
* Description:  Defines abstract API for burst mode observers*
*/


#ifndef CAMBURSTMODEOBSERVER_H
#define CAMBURSTMODEOBSERVER_H


// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Abstract API for burst mode observer. 
* Derived classes may register as a burst mode observer to get
* related notifications from CCamAppUiBase
*
*  @since 2.8
*/
class MCamBurstModeObserver
    {
    public:
        /**
        * Burst mode activation has changed
        * @since 2.8
        * @param aActive whether or not burst mode is active
        * @param aStillModeActive whether or not still capture is active
        */
        virtual void BurstModeActiveL( TBool aActive, TBool aStillModeActive ) = 0;
    };

#endif      // CAMBURSTMODEOBSERVER_H
            
// End of File
