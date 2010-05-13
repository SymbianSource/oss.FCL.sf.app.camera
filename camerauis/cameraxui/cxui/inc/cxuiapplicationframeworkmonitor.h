/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#ifndef CXUIAPPLICATIONFRAMEWORKMONITOR_H
#define CXUIAPPLICATIONFRAMEWORKMONITOR_H

#include <QObject>

class CxeSettings;
class CxuiApplication;
class CxuiApplicationFrameworkMonitorPrivate;

/*!
* Application Framework Monitor.
*/
class CxuiApplicationFrameworkMonitor : public QObject
{
    Q_OBJECT
    Q_ENUMS(ForegroundState)

public:
    /*!
    * Foreground state enumeration.
    */
    enum ForegroundState
    {
        /*! Note or dialog partially owns foreground */
        ForegroundPartiallyLost,
        /*! Other application owns foreground */
        ForegroundFullyLost,
        /*! This application owns foreground */
        ForegroundOwned
    };

public:
    CxuiApplicationFrameworkMonitor(CxuiApplication &application, CxeSettings& settings);
    virtual ~CxuiApplicationFrameworkMonitor();

public:
    ForegroundState foregroundState() const;

signals:
    /*!
    * Signal that foreground state has changed.
    * @param t New foreground state.
    */
    void foregroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState t);

    /*!
    * Signal battery being empty. Device is ecpected to power down at any moment.
    */
    void batteryEmpty();

private:
    CxuiApplicationFrameworkMonitorPrivate* p;
};

#endif // CXUIAPPLICATIONFRAMEWORKMONITOR_H
