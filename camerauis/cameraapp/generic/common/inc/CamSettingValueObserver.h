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


#ifndef CAMSETTINGVALUEOBSERVER_H
#define CAMSETTINGVALUEOBSERVER_H


// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Mixin class used to inform an observer of a change 
* in a setting value
*
*  @since 2.8
*/
class MCamSettingValueObserver
    {
    public:
        /*
        * Used when a change in a setting value requires interaction
        * by an observer
        * @since 2.8
        * @param aNewValue The new value of the setting observed
        */
        virtual void HandleSettingValueUpdateL( TInt aNewValue ) = 0;
    };

#endif      // CAMSETTINGVALUEOBSERVER_H
            
// End of File
