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
* Description:  Callback interface for CCamPropertyWatcher
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*  
*
* ---------------------------------------------------------------------------
*
*/

#ifndef CAMPROPERTYOBSERVER_H
#define CAMPROPERTYOBSERVER_H

// CLASS DECLARATION

/**
* Abstract API for property observer. 
* Derived classes receive notification when the value of a watched property change
*
*  @since 2.8
*/
class MPropertyObserver
    {
public:
    /**
    * The value of a watched property has changed
    * @since 2.8
    * @param aCategory the category of the property
    * @param aKey the identifier of the property
    */
    virtual void HandlePropertyChangedL( const TUid& aCategory, const TUint aKey ) = 0;
    };

#endif // CAMPROPERTYOBSERVER_H

// End of File
