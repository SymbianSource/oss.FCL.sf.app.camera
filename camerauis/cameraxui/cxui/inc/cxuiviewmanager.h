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
#ifndef CXUIVIEWMANAGER_H
#define CXUIVIEWMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>

#include "cxeviewfindercontrol.h"
#include "cxenamespace.h"
#include "cxeerror.h"

class QGraphicsSceneMouseEvent;
class CxuiCaptureKeyHandler;
class XQSettingsManager;
class XQSettingsKey;
class HbMainWindow;

class CxuiPrecaptureView;
class CxuiStillPrecaptureView;
class CxuiStillPrecaptureView2;
class CxuiVideoPrecaptureView;
class CxuiVideoPrecaptureView2;
class CxuiPostcaptureView;
class CxeEngine;
class CxuiDocumentLoader;
class CxuiErrorManager; // class that handles all errors in ui.
class CxuiStandby;

class CxuiViewManager : public QObject
{
    Q_OBJECT

public:
    CxuiViewManager(HbMainWindow &mainWindow, CxeEngine &engine, CxuiCaptureKeyHandler &keyHandler);
    ~CxuiViewManager();

    void prepareWindow();

    /**
    * Get a pointer to the document loader instance.
    */
    CxuiDocumentLoader* documentLoader();

public slots:
    void changeToPostcaptureView();
    void changeToPrecaptureView();

    /**
     * Switch camera from primary to secondary or vise versa.
     */
    void switchCamera();

    /**
     * Event monitor can be used to monitor changes in cenrep and PS values.
     */
    void eventMonitor(const XQSettingsKey& key, const QVariant& value);

    void createPostcaptureView();

private slots:
    void aboutToLooseFocus();
    void aboutToGainFocus();

signals:
    void focusGained();
    void focusLost();
    void batteryEmpty();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    void startEventMonitors();
    void createStillPrecaptureView();
    void createStillPrecaptureView2();
    void createVideoPrecaptureView();
    void createVideoPrecaptureView2();
    CxuiPrecaptureView* getPrecaptureView(Cxe::CameraMode mode, Cxe::CameraIndex camera);

    /*
    * connects all necessary signals for precapture view
    */
    void connectPreCaptureSignals();

    /*
    * disconnects signals
    */
    void disconnectSignals();

    /*
    * connects all necessary signals for postcapture view
    */
    void connectPostCaptureSignals();


    /*
    * connects capture key handler signals to the current view.
    */
    void connectCaptureKeySignals();

private:

    //data
    HbMainWindow &mMainWindow;
    CxuiStillPrecaptureView *mStillPrecaptureView;
    CxuiVideoPrecaptureView *mVideoPrecaptureView;
    CxuiPostcaptureView *mPostcaptureView;

    CxuiStillPrecaptureView2 *mStillPrecaptureView2;
    CxuiVideoPrecaptureView2 *mVideoPrecaptureView2;

    CxeEngine &mEngine;
    CxuiCaptureKeyHandler &mKeyHandler;

private:
    XQSettingsManager *mSettingsManager;
    int mKeyLockState; //! @todo: Needed due to Settings manager error: valueChanged is emmitted with same value repeatedly
    int mBatteryStatus;
    bool mFocused;
    CxuiDocumentLoader *mCameraDocumentLoader;
    CxuiStandby *mStandbyHandler;
    CxuiErrorManager *mErrorManager;
};

#endif // CXUIVIEWMANAGER_H
