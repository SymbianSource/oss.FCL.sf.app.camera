/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Callback wrapper for Publish and Subscribe
*
*/


#ifndef CAMPROPERTYWATCHER_H
#define CAMPROPERTYWATCHER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class MPropertyObserver;

// CLASS DECLARATION

/**
*  Class to watch for value changes on a property
*
*  @since 2.8
*/
class CCamPropertyWatcher : public CActive
	{
public: // Constructors and destructor
    /**
    * Symbian OS two-phased constructor
    * @since 2.8
    * @param aPropertyObserver The observer, notified when the value changes
    * @param aCategory the category of the property
    * @param aKey the identifier of the property
    */      
    static CCamPropertyWatcher* NewL( MPropertyObserver& aPropertyObserver, 
                                      const TUid& aCategory, 
                                      const TUint aKey );

    /**
    * Destructor.
    * @since 2.8
    */
    ~CCamPropertyWatcher();

public: // New methods

    /**
    * Register a need for notification of value changes
    * @since 2.8
    */
    void Subscribe();

    /**
    * Return the current value of the property
    * @since 2.8
    * @param aValue Reference to an integer that will receive the current 
    *                 value of the property
    * @return KErrNone if successful otherwise another of the system-wide error 
    *                 codes 
    */
	TInt Get( TInt& aValue );

private: // new methods

    /**
    * C++ default constructor.
    * @since 2.8
    * @param aPropertyObserver Reference to observer of change events
    * @param aCategory category of the property
    * @param aKey identifier of the property
    */
    CCamPropertyWatcher( MPropertyObserver& aPropertyObserver, 
                         const TUid& aCategory, 
                         const TUint aKey );
    /**
    * Symbian 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();

public: // methods from base classes

	/**
    * From CActive
    * @since 2.8
    */
    void RunL();

private:

    /**
    * From CActive
    * @since 2.8
    */
    void DoCancel();
	
private:

    MPropertyObserver& iPropertyObserver;   // observer informed of change events
    RProperty iProperty;                    // handle to the property
    TUid iCategory;                         // category of the property
    TUint iKey;                             // identifier of the property
	};

#endif // CAMPROPERTYWATCHER_H

// End of File
