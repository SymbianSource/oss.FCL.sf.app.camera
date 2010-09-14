/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ????????????????
*
*/



//INCLUDES
#include "CamZoomUpdateManager.h"
#include "CamUtility.h"
#include "CameraUiConfigManager.h"



//MACROS

// Cooldown period inbetween camera driver updates
const TInt KCamZoomCooldown    = 200000;  // .2  seconds
const TInt KCamMaxZoomCooldown = 250000;  // .25 seconds


// ====================== MEMBER FUNCTIONS =====================

// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamZoomUpdateManager* CCamZoomUpdateManager::NewL( CCamAppController& aController )
    {
    PRINT((_L("CCamZoomUpdateManager::NewL in")));
    CCamZoomUpdateManager* self = new(ELeave) CCamZoomUpdateManager( aController );
    CleanupStack::PushL(self);
    self->ConstructL ();
    CleanupStack::Pop();
    PRINT((_L("CCamZoomUpdateManager::NewL out")));
    return self;
    }


// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::CCamZoomUpdateManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamZoomUpdateManager::CCamZoomUpdateManager( CCamAppController& aController ) :
        CActive( /*EPriorityStandard*/ EPriorityUserInput ),
        iController( aController )
    {
    iDelayedUpdate = EFalse;
    }

// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::~CCamZoomUpdateManager
// Destructor.
// -----------------------------------------------------------------------------
//

CCamZoomUpdateManager::~CCamZoomUpdateManager()
{
    PRINT((_L("CCamZoomUpdateManager::~CCamZoomUpdateManager() in")));
    Cancel();
    iTimer.Close();
    PRINT((_L("CCamZoomUpdateManager::~CCamZoomUpdateManager() out")));
}

// -----------------------------------------------------------------------------
// CCamZoomUpdateTimer::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CCamZoomUpdateManager::ConstructL()
    {
    PRINT((_L("CCamZoomUpdateManager::ConstructL() in")));
    TRAPD( err, ReadDelayValuesL() );
    if( err )
        {
        PRINT((_L("CCamZoomUpdateManager::ConstructL() error in reading delay values")));
        }
    User::LeaveIfError(iTimer.CreateLocal());
    CActiveScheduler::Add(this);
    PRINT((_L("CCamZoomUpdateManager::ConstructL() out")));
    }
    
// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::SetZoomValue
// 
// -----------------------------------------------------------------------------
//
void CCamZoomUpdateManager::SetZoomValue( TInt aValue )
    {
    PRINT((_L("CCamZoomUpdateManager::SetZoomValue() => in")));
    
    // PUT IN MUTEX????
    
    // update value
    iValue = aValue;
    
    if ( IsActive() )
        {
        // check for starvation
        TTime current;
        current.UniversalTime();
        TInt64 delay = current.MicroSecondsFrom(iCooldownStart).Int64();
        
        if ( delay > iCamMaxZoomCooldown )
            {
            PRINT((_L("CCamZoomUpdateManager::SetZoomValue() <> IsActive, starved")));
            
            // force update if active object is starved
            Cancel();
            UpdateAndStartWait();
            }
        else
            {
            PRINT((_L("CCamZoomUpdateManager::SetZoomValue() <> IsActive, updating iValue")));
            
            // set the delayed update flag and wait
            iDelayedUpdate = ETrue;    
            }
        }
    else
        {
        PRINT((_L("CCamZoomUpdateManager::SetZoomValue() <> IsNotActive, setting zoom")));
        
        // notify controller and set active
        UpdateAndStartWait();
        }
    
    PRINT((_L("CCamZoomUpdateManager::SetZoomValue() <= out")));
    }

// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::UpdateAndStartWait
// 
// -----------------------------------------------------------------------------
//
void CCamZoomUpdateManager::UpdateAndStartWait()
    {
    // update cooldown start time
    iCooldownStart.UniversalTime();
    
    // reset delayed update flag
    iDelayedUpdate = EFalse;
    
    // notify iController
    iController.SetZoomValue( iValue );
        
    // activate cooldown period
    SetActive();
    iTimer.After( iStatus, iCamZoomCooldown );
    }

// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::RunL
// 
// -----------------------------------------------------------------------------
//
void CCamZoomUpdateManager::RunL()
    {
    PRINT((_L("CCamZoomUpdateManager::RunL() in")));
    
    //PUT IN MUTEX??
    
    if ( iDelayedUpdate )
        {
        // notify controller and set active
        UpdateAndStartWait();
        }
     
    PRINT((_L("CCamZoomUpdateManager::RunL() out")));
    }

// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::DoCancel
// Cancels the timer
// -----------------------------------------------------------------------------
//
void CCamZoomUpdateManager::DoCancel()
    {
    PRINT((_L("CCamZoomUpdateManager::DoCancel() in")));
    // cancel the timer
    iTimer.Cancel();
    PRINT((_L("CCamZoomUpdateManager::DoCancel() out")));
    }
    
// -----------------------------------------------------------------------------
// CCamZoomUpdateManager::ReadDelayValuesL
// Gets delay values from cenrep
// -----------------------------------------------------------------------------
//
void CCamZoomUpdateManager::ReadDelayValuesL()
    {
    // default values are used in case of error
    iCamZoomCooldown = KCamZoomCooldown;
    iCamMaxZoomCooldown = KCamMaxZoomCooldown;
    
    if( iController.UiConfigManagerPtr() )
        {
        RArray<TInt> delayValues;
        CleanupClosePushL( delayValues );
        iController.UiConfigManagerPtr()->SupportedZoomDelaysL( delayValues );
        iCamZoomCooldown = delayValues[0]*1000;
        iCamMaxZoomCooldown = delayValues[1]*1000;
        CleanupStack::PopAndDestroy( &delayValues );
        }
    }
// End of File
