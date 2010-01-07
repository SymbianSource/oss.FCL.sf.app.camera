/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ???????????????????
*
*/

#ifndef CCAMZOOMUPDATEMANAGER_H
#define CCAMZOOMUPDATEMANAGER_H

// INCLUDES
#include <e32base.h>
#include "CamAppController.h" // for controller definition

// FORWARD DECLARATIONS

//CLASS DECLARATIONS

/**
*  Camera Zoom Update Class
*
*  Implements a cooldown period to avoid overtaxing the camera driver with touchscreen zoom updates.
*
*  @since 5.0
*/
class CCamZoomUpdateManager : public CActive
    {
    public: // constructors and destructor
                
        /**
        * Two-phased constructor.
        */
        static CCamZoomUpdateManager* NewL( CCamAppController& aController );
                
        /**
        * Destructor.
        */
        ~CCamZoomUpdateManager();
        
        /**
        * From CActive Does the required action (calls the observer)
        */
        void SetZoomValue( TInt aValue );

    protected: // from base classes

        /**
        * From CActive Does the required action (calls the observer)
        */
        void RunL();

        /**
        * From CActive Cancels pending actions
        */
        void DoCancel();

    private: // constructor

        /**
        * C++ default constructor.
        */
        CCamZoomUpdateManager( CCamAppController& aController );

        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();
        
        /**
        * Notify the controller and SetActive
        */
        void UpdateAndStartWait();
        
        /**
        * Get delay values from cenrep
        */
        void ReadDelayValuesL();

    private: // data
        
        // timer used to facilitate cooldown period
        RTimer  iTimer;
        
        // time used to force update if active object gets starved
        TTime iCooldownStart;
        
        // the value to give to the controller
        TInt iValue; 
        
        // handle outstanding update requests
        TBool iDelayedUpdate; 
        
        // reference to the controller used for setting the zoom
        CCamAppController& iController;
        
        // Variated values for cooldown period inbetween 
        // camera driver updates
        TInt iCamZoomCooldown;
        TInt iCamMaxZoomCooldown;

    };

#endif      
            
// End of File


