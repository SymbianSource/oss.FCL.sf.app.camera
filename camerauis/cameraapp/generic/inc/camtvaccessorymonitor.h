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
* Description:  Utility class to handle TV accessory events
*/

#ifndef CAMTVACCESSORYMONITOR_H
#define CAMTVACCESSORYMONITOR_H


#include <AknWsEventObserver.h>     // for MAknWsEventObserver
#include <AccessoryServer.h>        // for RAccessoryServer
#include <AccessoryMode.h>          // for RAccessoryMode
#include <AccessoryConnection.h>    // for RAccessoryConnection
#include <accpolgenericidarray.h> 


class MCamTvAccessoryObserver
    {
public:
    virtual void HandleTvAccessoryConnectedL() = 0;
    virtual void HandleTvAccessoryDisconnectedL() = 0;
    };

class CCamTvAccessoryMonitor : public CActive
    {
public:
    // Cancel and destroy
    ~CCamTvAccessoryMonitor();

    // Two-phased constructor.
    static CCamTvAccessoryMonitor* NewL( MCamTvAccessoryObserver* aObserver );

public:

    // Starts to listen to tv accessory events
    void StartListeningL();
    
    // Returns ETrue if TVOut cable is connected
    TBool IsTvOutCableConnected();
    
    // Returns ETrue if HDMI cable is connected
    TBool IsHdmiCableConnected();
    
private:
    // C++ constructor
    CCamTvAccessoryMonitor( MCamTvAccessoryObserver* aObserver );

    // Second-phase constructor
    void ConstructL();

private:
    // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError(TInt aError);


private:

    MCamTvAccessoryObserver* iObserver;
 
    TBool iTvOutConnectionState;
    TBool iHDMIConnectionState;

    
    RAccessoryServer iTvAccServer;
    RAccessoryMode iTvAccMode;
    RAccessoryConnection iTvAccCon;
    TAccPolGenericIDArray iCurrentAccArray;
    TAccPolAccessoryMode iPolAccMode;
    TAccMode iPreviousMode;
    };

#endif // CAMTVACCESSORYMONITOR_H
