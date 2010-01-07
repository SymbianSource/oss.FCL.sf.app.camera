/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that monitors free memory and attempts to free it if necessary
*
*/

#ifndef CAMMEMORYMONITOR_H
#define CAMMEMORYMONITOR_H

#include <e32base.h>
#include <oommonitorsession.h>


class CCamAppController;
class CCamAppUi;

/**
 * Class that monitors free memory and attempts to free it if necessary
 *
 */
class CCamMemoryMonitor : public CActive
{

public:

    static CCamMemoryMonitor* NewL( CCamAppUi* aAppUi, CCamAppController* aController );

    virtual ~CCamMemoryMonitor();

    /**
     * Start monitoring free memory. If available memory drop below given limit,
     * requests memory from OomManager.
     *
     * @param aLimit Monitored memory limit
     * @param aRequestAmount Amount of memory to be requested if below aLimit
     */
    void StartMonitoring( TInt aLimit, TInt aRequestAmount );

    /**
     * Start monitoring free memory. Uses default or previously given
     * values for limit and request amount
     */
    void StartMonitoring();

    /**
     * Stop monitoring free memory.
     *
     */
    void StopMonitoring();

    /**
     * Checks for free memory and requests more if below given limit.
     *
     * @param aLimit Memory limit to be checked
     * @param aRequestAmount Amount of memory to be requested if below aLimit
     * @param aShowNote If ETrue, note will be shown during memory request
     */
    TInt CheckAndRequestMemoryL( TInt aLimit, TInt aRequestAmount, TBool aShowNote );

    /**
     * Checks for free memory and requests more if below given limit. Uses
     * default values for limit and request amount.
     *
     * @param aShowNote If ETrue, note will be shown during memory request
     */
    TInt CheckAndRequestMemoryL( TBool aShowNote );



    void RunL();
    void DoCancel();

private:
    CCamMemoryMonitor( CCamAppUi* aAppUi, CCamAppController* aController );
    void ConstructL();

    static TInt MemoryCheck( TAny* aPtr );

    void CheckMemory();
private:
    // data

    TInt iLimit;
    TInt iRequestAmount;

    CCamAppUi* iAppUi;
    CCamAppController* iController;
    CPeriodic* iMemoryCheckTimer;
    ROomMonitorSession iOomMonitor;
};

#endif /* CCAMMEMORYMONITOR_H_ */
