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
#ifndef CXUIAPPLICATIONFRAMEWORKMONITORPRIVATE_H
#define CXUIAPPLICATIONFRAMEWORKMONITORPRIVATE_H

#include <QObject>
#include <QVariant>
#include "cxuiapplicationframeworkmonitor.h"

class CxeSettings;
class CxuiApplication;
#ifdef Q_OS_SYMBIAN
class RWsSession;
class RWindowGroup;
class QString;
class QSymbianEvent;
#endif // Q_OS_SYMBIAN


class CxuiApplicationFrameworkMonitorPrivate : public QObject
{
    Q_OBJECT

public:
    CxuiApplicationFrameworkMonitorPrivate(CxuiApplication &application, CxeSettings& settings);
    virtual ~CxuiApplicationFrameworkMonitorPrivate();

public:
    CxuiApplicationFrameworkMonitor::ForegroundState foregroundState() const;

signals:
    void foregroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState t);
    void batteryEmpty();

#ifdef Q_OS_SYMBIAN
private slots:
    void handleEvent(const QSymbianEvent *event);
    void handlePropertyEvent(long int uid, unsigned long int key, QVariant value);

private:
    void init();
    bool handleWindowServerEvent(const QSymbianEvent *event);
    void setState(CxuiApplicationFrameworkMonitor::ForegroundState state);
    CxuiApplicationFrameworkMonitor::ForegroundState getCurrentState();
    unsigned int focusedApplicationUid();
#endif // Q_OS_SYMBIAN


private:
    CxuiApplication &mApplication;
    CxeSettings& mSettings;
#ifdef Q_OS_SYMBIAN
    RWsSession &mWsSession;
    RWindowGroup &mWindowGroup;
    int mWindowGroupId;
    QString mWindowGroupName;
    int mKeyLockState;
    int mBatteryStatus;
#endif // Q_OS_SYMBIAN
    CxuiApplicationFrameworkMonitor::ForegroundState mState;
};

#endif // CXUIAPPLICATIONFRAMEWORKMONITORPRIVATE_H
