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

#ifndef CAMSTARTUPLOGOCONTROLLER_H
#define CAMSTARTUPLOGOCONTROLLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

enum TStartupLogoState
    {
    EStartupLogoNotVisible,
    EStartupLogoVisible
    };

// CLASS DECLARATION

/**
 *  Observer interface for listening to controller state change events
 * 
 */
class MCamStartupLogoControllerObserver
    {
public:
    virtual ~MCamStartupLogoControllerObserver() {}

    /**
     * Startup logo controller state changed.
     */
    virtual void StartupLogoControllerStateChanged(TStartupLogoState aNewState) = 0;
    };



/**
 *  Controller for managing the camera startup logo state
 * 
 */
class CCamStartupLogoController : public CBase
    {

public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CCamStartupLogoController();

    /**
     * Two-phased constructor.
     */
    static CCamStartupLogoController* NewL(TStartupLogoState aInitialState);

    /**
     * Two-phased constructor.
     */
    static CCamStartupLogoController* NewLC(TStartupLogoState aInitialState);

public:
    /**
     * Add an observer. Returns an error code.
     */
    TInt AddObserver(MCamStartupLogoControllerObserver* aObserver);

    /**
     * Removes an observer.
     */
    void RemoveObserver(MCamStartupLogoControllerObserver* aObserver);

    /**
     * Get current state.
     */
    TStartupLogoState State() const;

    /**
     * Show startup logo.
     */
    void ShowLogo();

    /**
     * Hide startup logo.
     */
    void HideLogo();

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CCamStartupLogoController(TStartupLogoState aInitialState);

    /**
     * Symbian default constructor for performing 2nd stage construction
     */
    void ConstructL();

    static TInt FailSafeCallback(TAny* ptr);

private:
    void SetState(TStartupLogoState aNewState);

private: // data
    TStartupLogoState iState;
    RPointerArray<MCamStartupLogoControllerObserver> iObservers;
    CPeriodic* iFailSafeTimer; // own
    };

#endif // CAMSTARTUPLOGOCONTROLLER_H
