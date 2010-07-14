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

#ifndef CXEGEOTAGGINGTRAILPRIVATE_H
#define CXEGEOTAGGINGTRAILPRIVATE_H

#include <QTimer>
#include <QObject>
#include <QVariant>
#include <QString>

#include "cxeerror.h"
#include "rlocationtrail.h"
#include "cxestatemachine.h"
#include "cxegeotaggingtrail.h"




// Forward declarations
class CxeSettings;
class RLocationTrail;
class CxeStillCaptureControl;
class CxeVideoCaptureControl;




class CxeGeoTaggingTrailPrivate : public QObject,
                                  public CxeStateMachine
{
    Q_OBJECT

private:

    CxeGeoTaggingTrailPrivate(CxeStillCaptureControl &stillControl,
                              CxeVideoCaptureControl &videoControl,
                              CxeSettings &settings);
    ~CxeGeoTaggingTrailPrivate();

signals:
    void stateChanged(CxeGeoTaggingTrail::State newState, CxeError::Id error);

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

private slots:
    void handleSettingValueChanged(const QString&,QVariant);
    void handleGeoTaggingPropertyEvent(long int uid, unsigned long int key, QVariant value);
    void timeout();
    void stop(bool closeSession = false);

private:
    void start();
    void initializeStates();
    bool canStopTrail() const;
    CxeGeoTaggingTrail::State state() const;

private:
    CxeStillCaptureControl &mStillCaptureControl;
    CxeVideoCaptureControl &mVideoCaptureControl;
    CxeSettings &mSettings;
    QTimer mStopLocationTrailTimer;
    bool mPendingStopTrailSession;
#if defined(Q_OS_SYMBIAN)
    RLocationTrail mLocationTrail;
#endif
    
    friend class CxeGeoTaggingTrail;
};

#endif // CXEGEOTAGGINGTRAILPRIVATE_H
