/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cxuiapplicationframeworkmonitor.h"


class QGraphicsSceneMouseEvent;
class HbMainWindow;
class CxuiApplication;
class CxuiCaptureKeyHandler;
class CxuiPrecaptureView;
class CxuiStillPrecaptureView;
class CxuiVideoPrecaptureView;
class CxuiPostcaptureView;
class CxeEngine;
class CxuiDocumentLoader;
class CxuiErrorManager; // class that handles all errors in ui.
class CxuiStandby;
class CxuiSceneModeView;

class CxuiViewManager : public QObject
{
    Q_OBJECT

public:
    CxuiViewManager(CxuiApplication &application, HbMainWindow &mainWindow, CxeEngine &engine);
    ~CxuiViewManager();

    void prepareWindow();

    /**
    * Get a pointer to the document loader instance.
    */
    CxuiDocumentLoader* documentLoader();

    //@todo: Temporarily needed in main().
    bool proceedStartup();

public slots:
    void changeToPostcaptureView();
    void changeToPrecaptureView();
    void switchCamera();
    void createPostcaptureView();
    void showScenesView();

private slots:
    void startupCheck();
    void toForeground();
    void handleForegroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState state);
    void showUsbErrorPopup(bool show);
    void handleBatteryEmpty();
    void aboutToLooseFocus();
    void aboutToGainFocus();

signals:
    void focusGained();
    void focusLost();
    void batteryEmpty();
    void disableStandbyTimer();
    void startStandbyTimer();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    void initStartupView();
    void createStillPrecaptureView();
    void createVideoPrecaptureView();
    CxuiPrecaptureView* getPrecaptureView(Cxe::CameraMode mode, Cxe::CameraIndex camera);

    void createSceneModesView();

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
    CxuiApplication &mApplication;
    HbMainWindow &mMainWindow;
    CxuiStillPrecaptureView *mStillPrecaptureView;
    CxuiVideoPrecaptureView *mVideoPrecaptureView;
    CxuiPostcaptureView *mPostcaptureView;

    CxeEngine &mEngine;
    CxuiCaptureKeyHandler *mKeyHandler;
    CxuiApplicationFrameworkMonitor *mApplicationMonitor;

private:
    CxuiDocumentLoader *mCameraDocumentLoader;
    CxuiStandby *mStandbyHandler;
    CxuiErrorManager *mErrorManager;
    CxuiSceneModeView *mSceneModeView;
};

#endif // CXUIVIEWMANAGER_H
