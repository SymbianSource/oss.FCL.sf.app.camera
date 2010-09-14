/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Controller for managing the camera startup logo state
*
*/

#include "camstartuplogocontroller.h"
#include "camlogging.h"

// Maximum time the logo can be shown
const TInt KCamFailSafeTimeout = 5000000; // 5 seconds

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamStartupLogoController::CCamStartupLogoController(TStartupLogoState aInitialState)
    : iState(aInitialState)
    {
    // No implementation required
    PRINT( _L("Camera <> CCamStartupLogoController::CCamStartupLogoController") );
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamStartupLogoController::~CCamStartupLogoController()
    {
    PRINT( _L("Camera <> CCamStartupLogoController::~CCamStartupLogoController") );
    delete iFailSafeTimer;
    iObservers.Reset();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStartupLogoController* CCamStartupLogoController::NewLC(TStartupLogoState aInitialState)
    {
    CCamStartupLogoController* self =
            new (ELeave) CCamStartupLogoController(aInitialState);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStartupLogoController* CCamStartupLogoController::NewL(
    TStartupLogoState aInitialState)
    {
    CCamStartupLogoController* self = CCamStartupLogoController::NewLC(aInitialState);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamStartupLogoController::ConstructL()
    {
    iFailSafeTimer = CPeriodic::NewL(0);
    }

// ---------------------------------------------------------------------------
// CCamStartupLogoController::AddObserver
// ---------------------------------------------------------------------------
//
TInt CCamStartupLogoController::AddObserver(
    MCamStartupLogoControllerObserver* aObserver)
    {
    return iObservers.Append(aObserver);
    }

// ---------------------------------------------------------------------------
// CCamStartupLogoController::RemoveObserver
// ---------------------------------------------------------------------------
//
void CCamStartupLogoController::RemoveObserver(
    MCamStartupLogoControllerObserver* aObserver)
    {
    TInt index = iObservers.Find(aObserver);
    if (index >= 0)
        {
        iObservers.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// CCamStartupLogoController::State
// ---------------------------------------------------------------------------
//
TStartupLogoState CCamStartupLogoController::State() const
    {
    return iState;
    }

// ---------------------------------------------------------------------------
// CCamStartupLogoController::ShowLogo
// ---------------------------------------------------------------------------
//
void CCamStartupLogoController::ShowLogo()
    {
    PRINT(_L("Camera <> CCamStartupLogoController::ShowLogo"));

    if (iFailSafeTimer)
        {
        iFailSafeTimer->Cancel();
        iFailSafeTimer->Start(TTimeIntervalMicroSeconds32(KCamFailSafeTimeout),
                TTimeIntervalMicroSeconds32(KCamFailSafeTimeout),
                TCallBack(FailSafeCallback, this));
        }

    SetState(EStartupLogoVisible);
    }

// ---------------------------------------------------------------------------
// CCamStartupLogoController::HideLogo
// ---------------------------------------------------------------------------
//
void CCamStartupLogoController::HideLogo()
    {
    PRINT(_L("Camera <> CCamStartupLogoController::HideLogo"));

    // Hide immediately
    SetState(EStartupLogoNotVisible);
    if (iFailSafeTimer)
        {
        iFailSafeTimer->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CCamStartupLogoController::SetState
// ---------------------------------------------------------------------------
//
void CCamStartupLogoController::SetState(TStartupLogoState aNewState)
    {
    PRINT1(_L("Camera => CCamStartupLogoController::SetState %d"), aNewState);
    
    if (iState != aNewState)
        {
        iState = aNewState;
        for (TInt i=0; i<iObservers.Count(); i++)
            {
            iObservers[i]->StartupLogoControllerStateChanged(aNewState);
            }
        }
    PRINT1(_L("Camera <= CCamStartupLogoController::SetState %d"), aNewState);
    }

// ---------------------------------------------------------------------------
// CCamStartupLogoController::FailSafeCallback
// ---------------------------------------------------------------------------
//
TInt CCamStartupLogoController::FailSafeCallback(TAny* ptr)
    {
    PRINT(_L("Camera <> CCamStartupLogoController::FailSafeCallback"));

    CCamStartupLogoController* self = static_cast<CCamStartupLogoController*>(ptr);
    if (self)
        {
        self->HideLogo();
        if (self->iFailSafeTimer)
            {
            self->iFailSafeTimer->Cancel();
            }
        }
    return 0;
    }
