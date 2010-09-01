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
* Description:  Declaration of CCamFlashSimulator class.
*                Simulates flash charging when real implementation
*                cannot be used.
*
*/

#ifndef CAMFLASHSIMULATOR_H
#define CAMFLASHSIMULATOR_H

// ===========================================================================
// Includes
#include <e32base.h>
#include "mcamcameraobserver.h"


// ===========================================================================
// Constants

// Duration of simulated flash charging delay 
// Value: 10 seconds
const TInt KSimulatedChargingTime = 10*1000*1000; // [us]


// ===========================================================================
// Forward declarations
class CCamCameraController;

// ===========================================================================
// Class declarations

/**
* Flash simulator class
*/
class CCamFlashSimulator: public CBase,
                          public MCamCameraObserver
  {
  public:
    
    /**
    * Two-phased constructor
    */
    static CCamFlashSimulator* NewL( CCamCameraController& aController );
    
    /**
    * Destructor
    */
    ~CCamFlashSimulator();

  public: // From MCamCameraObserver
  
    virtual void HandleCameraEventL( TInt              aStatus, 
                                     TCamCameraEventId aEventId, 
                                     TAny*             aEventData = NULL );

  public: // Other

    /**
    * Set flash requirement status
    */
    void SetFlashRequired( TBool aFlashRequired );   
    
    /**
    * Set flash error state
    */
    void SetFlashError( TBool aFlashError );     
    
    /**
    * Start recharging of the flash. In case aError is true,
    * error flag shall be 
    * @param aError
    */       
    void StartRecharging( TBool aError = EFalse );
    
    /**
    * Is the flash currently being recharged
    */
    TBool Recharging();       
                
  private:

    /**
    * Notify Camera Controller of flash status
    */
    void NotifyController();
    
    /**
    * Callback function for recharging timer
    * @param aSelf
    */
    static TInt RechargeTimerCallback( TAny* aSelf );

  private: // Constructors

    /**
    * C++ default constructor
    */
    CCamFlashSimulator( CCamCameraController& aController );
    
    /**
    * Second phase constructor
    */
    void ConstructL();
       
  private:

    CCamCameraController& iController;
    CPeriodic*            iRechargeTimer;
    TBool                 iError;
    TBool                 iFlashRequired;
    TBool                 iFlashReady;
  };

#endif // CAMFLASHSIMULATOR_H

 