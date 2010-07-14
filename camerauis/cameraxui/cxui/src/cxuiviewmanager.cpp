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
#include <QDebug>
#include <QTimer>
#include <hbmainwindow.h>
#include <coemain.h>
#include <QGraphicsSceneEvent>
#include <hbstyleloader.h>

#include "cxuiapplication.h"
#include "cxuiapplicationframeworkmonitor.h"
#include "cxuiviewmanager.h"
#include "cxuivideoprecaptureview.h"
#include "cxuistillprecaptureview.h"
#include "cxuipostcaptureview.h"
#include "cxuicapturekeyhandler.h"
#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxefeaturemanager.h" //  mEngine->featureManager()
#include "cxeviewfindercontrol.h"
#include "cxenamespace.h"
#include "cxuienums.h"
#include "cxutils.h"
#include "cxuidocumentloader.h"
#include "cxuistandby.h"
#include "cxuierrormanager.h"
#include "cxesettings.h"
#include "cxememorymonitor.h"
#include "cxuiserviceprovider.h"
#include "cxuiscenemodeview.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuiviewmanagerTraces.h"
#endif


using namespace Cxe;
using namespace CxUiLayout;


// ---------------------------------------------------------------------------
// CxuiViewManager::CxuiViewManager
//
// ---------------------------------------------------------------------------
//
CxuiViewManager::CxuiViewManager(CxuiApplication &application, HbMainWindow &mainWindow, CxeEngine &engine) :
    mApplication(application),
    mMainWindow(mainWindow),
    mStillPrecaptureView(NULL),
    mVideoPrecaptureView(NULL),
    mPostcaptureView(NULL),
    mEngine(engine),
    mKeyHandler(NULL),
    mApplicationMonitor(NULL),
    mCameraDocumentLoader(NULL),
    mSceneModeView(NULL)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Application monitor
    mApplicationMonitor = new CxuiApplicationFrameworkMonitor(mApplication, mEngine.settings());
    connect(mApplicationMonitor, SIGNAL(foregroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState)),
            this, SLOT(handleForegroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState)));
    connect(mApplicationMonitor, SIGNAL(batteryEmpty()),
            this, SLOT(handleBatteryEmpty()));

    // Connect memory monitor start / stop to focused status
    connect(this, SIGNAL(focusGained()), &mEngine.memoryMonitor(), SLOT(startMonitoring()));
    connect(this, SIGNAL(focusLost()),   &mEngine.memoryMonitor(), SLOT(stopMonitoring()));

    // Key handler
    mKeyHandler = new CxuiCaptureKeyHandler(mEngine);

    mMainWindow.installEventFilter(this); // in order to filter capture and AF keys

    OstTrace0( camerax_performance, CXUIVIEWMANAGER_CXUIVIEWMANAGER, "msg: e_CX_VIEWMANAGER_CREATE_DOCLOADER 1" );
    mCameraDocumentLoader = new CxuiDocumentLoader(&engine);
    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CXUIVIEWMANAGER, "msg: e_CX_VIEWMANAGER_CREATE_DOCLOADER 0" );

    // standby functionality and necessary signal connections
    mStandbyHandler = new CxuiStandby(*mKeyHandler, mCameraDocumentLoader, &mEngine);

    connect(mStandbyHandler, SIGNAL(aboutToEnterStandby()),this, SLOT(aboutToLooseFocus()));
    connect(mStandbyHandler, SIGNAL(aboutToExitStandby()),this, SLOT(aboutToGainFocus()));

    // error manager, handling errors and notifying users based on their severity
    mErrorManager = new CxuiErrorManager(*mKeyHandler, mCameraDocumentLoader);

    // connecting necessary signals from error manager to release and init camera.
    connect(mErrorManager, SIGNAL(aboutToRecoverError()), this, SLOT(aboutToLooseFocus()));
    connect(mErrorManager, SIGNAL(errorRecovered()), this, SLOT(aboutToGainFocus()));

    if (!CxuiServiceProvider::isCameraEmbedded()) {
        // For embedded mode: don't create view yet, create
        // when engine inits to correct mode
        CX_DEBUG_ASSERT(mEngine.cameraDeviceControl().cameraIndex() == Cxe::PrimaryCameraIndex);
        if (mEngine.mode() == Cxe::VideoMode) {
            createVideoPrecaptureView();
            mMainWindow.blockSignals(true);
            mMainWindow.setCurrentView(mVideoPrecaptureView, false);
            mMainWindow.blockSignals(false);
        } else {
            createStillPrecaptureView();
            mMainWindow.blockSignals(true);
            mMainWindow.setCurrentView(mStillPrecaptureView, false);
            mMainWindow.blockSignals(false);
        }
        connectPreCaptureSignals();
    }

    //connecting initmode signals
    connect(&mEngine.cameraDeviceControl(), SIGNAL(initModeComplete(CxeError::Id)),
            this, SLOT(createPostcaptureView()));

    connect(&mEngine.cameraDeviceControl(), SIGNAL(initModeComplete(CxeError::Id)),
            mErrorManager, SLOT(analyze(CxeError::Id)));

    connect(&mEngine.stillCaptureControl(), SIGNAL(imageCaptured(CxeError::Id, int)),
            mErrorManager, SLOT(analyze(CxeError::Id)));

    if (CxuiServiceProvider::isCameraEmbedded()) {
        // connect signals to set up the view after image/video prepare
        connect(&mEngine.stillCaptureControl(), SIGNAL(imagePrepareComplete(CxeError::Id)),
                this, SLOT(changeToPrecaptureView()));
        connect(&mEngine.videoCaptureControl(), SIGNAL(videoPrepareComplete(CxeError::Id)),
                this, SLOT(changeToPrecaptureView()));

        // start standby timer now because view will not be ready when viewfinder is started
        mStandbyHandler->startTimer();
    }

    // Register stylesheet. It will be automatically destroyed on application
    // exit.
    HbStyleLoader::registerFilePath(":/camerax/cxui.css");

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::~CxuiViewManager
//
// ---------------------------------------------------------------------------
//
CxuiViewManager::~CxuiViewManager()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mCameraDocumentLoader;
    delete mKeyHandler;
    delete mApplicationMonitor;

    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiViewManager::prepareWindow
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::prepareWindow()
{
    getPrecaptureView(mEngine.mode(), mEngine.cameraDeviceControl().cameraIndex())->prepareWindow();
}


// ---------------------------------------------------------------------------
// CxuiViewManager::createStillPrecaptureView
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::createStillPrecaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUIVIEWMANAGER_CREATESTILLPRECAPTUREVIEW, "msg: e_CX_CREATE_STILLPRECAPTUREVIEW 1" );

    // Use document loader to create widgets and layouts
    // (non-sectioned parts are parsed and loaded)
    OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_ADDPRECAPTUREVIEWS, "msg: e_CX_DOCLOADER_LOAD 1" );

    bool ok = false;
    CX_DEBUG_ASSERT(mCameraDocumentLoader);
    mCameraDocumentLoader->load(STILL_1ST_XML, &ok);
    Q_ASSERT_X(ok, "createStillPrecaptureView", "error in xml file parsing");

    OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_CREATESTILLPRECAPTUREVIEW, "msg: e_CX_DOCLOADER_LOAD 0" );

    OstTrace0( camerax_performance, DUP4_CXUIVIEWMANAGER_ADDPRECAPTUREVIEWS, "msg: e_CX_DOCLOADER_FINDWIDGET 1" );
    QGraphicsWidget *widget = NULL;
    // ask for the still precapture view widget pointer
    widget = mCameraDocumentLoader->findWidget(STILL_PRE_CAPTURE_VIEW);
    mStillPrecaptureView = qobject_cast<CxuiStillPrecaptureView *> (widget);
    CX_DEBUG_ASSERT(mStillPrecaptureView);
    OstTrace0( camerax_performance, DUP5_CXUIVIEWMANAGER_ADDPRECAPTUREVIEWS, "msg: e_CX_DOCLOADER_FINDWIDGET 0" );

    // call for needed consturction methods
    mStillPrecaptureView->construct(&mMainWindow, &mEngine, mCameraDocumentLoader, mKeyHandler);
    // .. and add to main window (which also takes ownership)
    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_MAINWINDOW_ADDVIEW, "msg: e_CX_MAINWINDOW_ADDVIEW 1" );
    mMainWindow.addView(widget);
    OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_MAINWINDOW_ADDVIEW, "msg: e_CX_MAINWINDOW_ADDVIEW 0" );

    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CREATESTILLPRECAPTUREVIEW, "msg: e_CX_CREATE_STILLPRECAPTUREVIEW 0" );

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::createStillScenesView
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::createSceneModesView()
{
    CX_DEBUG_ENTER_FUNCTION();

    bool ok = false;
    CX_DEBUG_ASSERT(mCameraDocumentLoader);
    CxuiDocumentLoader *documentLoader = new CxuiDocumentLoader(&mEngine);
    // Use document loader to create widgets and layouts
    // (non-sectioned parts are parsed and loaded)
    documentLoader->load(SCENEMODE_SETTING_XML, &ok);

    QGraphicsWidget *widget = NULL;

    // ask for the scenes mode view widget pointer
    widget = documentLoader->findWidget(STILL_SCENES_VIEW);
    Q_ASSERT_X(ok && (widget != 0), "camerax ui", "invalid xml file");
    mSceneModeView = qobject_cast<CxuiSceneModeView *> (widget);

    // call for needed construction methods
    mSceneModeView->construct(&mMainWindow, &mEngine, documentLoader, mKeyHandler);
    // .. and add to main window (which also takes ownership)
    mMainWindow.addView(widget);
    mSceneModeView->loadBackgroundImages();

    connect(mSceneModeView, SIGNAL(viewCloseEvent()), this, SLOT(changeToPrecaptureView()));
    delete documentLoader;
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* CxuiViewManager::showScenesView
* A private method for displaying Scene Modes selection view
*/
void CxuiViewManager::showScenesView()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (!mSceneModeView) {
        createSceneModesView();
    }
    else {
        mSceneModeView->loadBackgroundImages();
    }
    CX_DEBUG_ASSERT(mSceneModeView);
    mMainWindow.blockSignals(true);
    mMainWindow.setCurrentView(mSceneModeView, false);
    emit disableStandbyTimer();
    connectCaptureKeySignals();
    mMainWindow.blockSignals(false);
    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiViewManager::createVideoPrecaptureView
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::createVideoPrecaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUIVIEWMANAGER_CREATEVIDEOPRECAPTUREVIEW, "msg: e_CX_CREATE_VIDEOPRECAPTUREVIEW 1" );

    CX_DEBUG_ASSERT(mCameraDocumentLoader);

    bool ok = false;
    // load and create the default widgets in video xml
    mCameraDocumentLoader->load(VIDEO_1ST_XML, &ok);
    CX_DEBUG_ASSERT(ok);

    // get pointer to videoprecaptureview and do some initialisation
    QGraphicsWidget *widget = NULL;
    widget = mCameraDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_VIEW);
    mVideoPrecaptureView = qobject_cast<CxuiVideoPrecaptureView *> (widget);
    CX_DEBUG_ASSERT(mVideoPrecaptureView);
    mVideoPrecaptureView->construct(&mMainWindow, &mEngine, mCameraDocumentLoader, mKeyHandler);

    // add view to main window
    OstTrace0( camerax_performance, DUP3_CXUIVIEWMANAGER_MAINWINDOW_ADDVIEW, "msg: e_CX_MAINWINDOW_ADDVIEW 1" );
    mMainWindow.addView(mVideoPrecaptureView);
    OstTrace0( camerax_performance, DUP4_CXUIVIEWMANAGER_MAINWINDOW_ADDVIEW, "msg: e_CX_MAINWINDOW_ADDVIEW 0" );
    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CREATEVIDEOPRECAPTUREVIEW, "msg: e_CX_CREATE_VIDEOPRECAPTUREVIEW 0" );

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::createStillPostcaptureView
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::createPostcaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mCameraDocumentLoader);
    OstTrace0( camerax_performance, CXUIVIEWMANAGER_CREATESTILLPOSTCAPTUREVIEW, "msg: e_CX_CREATE_STILLPOSTCAPTUREVIEW 1" );

    if (!mPostcaptureView) {
        bool ok = false;
        mCameraDocumentLoader->load(POSTCAPTURE_XML, &ok);
        CX_DEBUG_ASSERT(ok);

        // get pointer to videoprecaptureview and do some initialisation
        QGraphicsWidget *widget = NULL;
        widget = mCameraDocumentLoader->findWidget(POSTCAPTURE_VIEW);
        mPostcaptureView = qobject_cast<CxuiPostcaptureView *> (widget);
        CX_DEBUG_ASSERT(mPostcaptureView);
        mPostcaptureView->construct(&mMainWindow, &mEngine, mCameraDocumentLoader);

        mMainWindow.addView(mPostcaptureView);
        connect(mPostcaptureView, SIGNAL(changeToPrecaptureView()), this, SLOT(changeToPrecaptureView()));

    }

    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CREATESTILLPOSTCAPTUREVIEW, "msg: e_CX_CREATE_STILLPOSTCAPTUREVIEW 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::getPrecaptureView
//
// ---------------------------------------------------------------------------
//
CxuiPrecaptureView*
CxuiViewManager::getPrecaptureView(Cxe::CameraMode mode, Cxe::CameraIndex camera)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxuiViewManager::getPrecaptureView() mode=%d, camera index=%d", mode, camera));

    CX_DEBUG_ASSERT(camera == Cxe::PrimaryCameraIndex);

    if (mode == ImageMode) {
        CX_DEBUG(("CxuiViewManager::getPrecaptureView() image mode"));
        if(!mStillPrecaptureView) {
            createStillPrecaptureView();
        }
        return mStillPrecaptureView;
    } else {
        CX_DEBUG(("CxuiViewManager::getPrecaptureView() video mode"));
        if(!mVideoPrecaptureView) {
            createVideoPrecaptureView();
        }
        return mVideoPrecaptureView;
    }
}

CxuiDocumentLoader* CxuiViewManager::documentLoader()
{
    return mCameraDocumentLoader;
}

// ---------------------------------------------------------------------------
// CxuiViewManager::changeToPostcaptureView
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::changeToPostcaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mPostcaptureView) {
        createPostcaptureView();
    }

    mMainWindow.blockSignals(true);
    mMainWindow.setCurrentView(mPostcaptureView, false);
    mMainWindow.blockSignals(false);

    // connecting all necessary signals for postcapture view
    connectPostCaptureSignals();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::changeToPrecaptureView
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::changeToPrecaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (CxuiServiceProvider::isCameraEmbedded()) {
        // disconnect signals, we only want to call this once after the initial prepare
        disconnect(&mEngine.stillCaptureControl(), SIGNAL(imagePrepareComplete(CxeError::Id)),
                this, SLOT(changeToPrecaptureView()));
        disconnect(&mEngine.videoCaptureControl(), SIGNAL(videoPrepareComplete(CxeError::Id)),
                this, SLOT(changeToPrecaptureView()));
    }

    HbView *view = getPrecaptureView(mEngine.mode(),
        mEngine.cameraDeviceControl().cameraIndex());

    mMainWindow.blockSignals(true);
    mMainWindow.setCurrentView(view, false);
    mMainWindow.blockSignals(false);

    if (mSceneModeView){
        delete mSceneModeView;
        mSceneModeView = NULL;
    }
    // connecting necessary pre-capture view signals
    connectPreCaptureSignals();
    emit startStandbyTimer();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::switchCamera
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::switchCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    Cxe::CameraIndex nextCamera;
    Qt::Orientation nextViewOrientation;

    if (mEngine.cameraDeviceControl().cameraIndex() == Cxe::PrimaryCameraIndex) {
        nextCamera = Cxe::SecondaryCameraIndex;
        nextViewOrientation = Qt::Vertical;
    } else {
        nextCamera = Cxe::PrimaryCameraIndex;
        nextViewOrientation = Qt::Horizontal;
    }

    CxuiPrecaptureView* view = getPrecaptureView(mEngine.mode(), nextCamera);

    mMainWindow.blockSignals(true);
    mMainWindow.setCurrentView(view, false);
    mMainWindow.blockSignals(false);
    view->updateOrientation(nextViewOrientation);
    view->prepareWindow();

    connectPreCaptureSignals();

    mEngine.cameraDeviceControl().switchCamera(nextCamera);

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::eventFilter
//
// ---------------------------------------------------------------------------
//
bool CxuiViewManager::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)

    bool eventWasConsumed = false;

    switch (event->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        eventWasConsumed = mKeyHandler->handleKeyEvent(event);
        break;
    }
    // No need to call base class implementation, because we derive from QObject directly.
    // QObject::eventFilter() implementation always returns false.
    return eventWasConsumed;
}

// ---------------------------------------------------------------------------
// CxuiViewManager::connectCaptureKeySignals
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::connectCaptureKeySignals()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Disconnect all existing capture key signals
    mKeyHandler->disconnect();

    QObject *currentView = mMainWindow.currentView();

    if (currentView) {
        // If the view class does not implement the named slot, the connect will fail
        // and output some warnings as debug prints. This is by design.
        connect(mKeyHandler, SIGNAL(autofocusKeyPressed()),  currentView, SLOT(handleAutofocusKeyPressed()));
        connect(mKeyHandler, SIGNAL(autofocusKeyReleased()), currentView, SLOT(handleAutofocusKeyReleased()));
        connect(mKeyHandler, SIGNAL(captureKeyPressed()),    currentView, SLOT(handleCaptureKeyPressed()));
        connect(mKeyHandler, SIGNAL(captureKeyReleased()),   currentView, SLOT(handleCaptureKeyReleased()));
    }

    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiViewManager::connectPreCaptureSignals
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::connectPreCaptureSignals()
{
    CX_DEBUG_ENTER_FUNCTION();

    disconnectSignals();

    QObject *currentView = mMainWindow.currentView();

    if (currentView != mPostcaptureView) {
        // connects all capture key signals.
        connectCaptureKeySignals();

        // connecting view manager focus events to pre-capture views
        connect(this, SIGNAL(focusGained()),  currentView, SLOT(handleFocusGained()), Qt::UniqueConnection);
        connect(this, SIGNAL(focusLost()),    currentView, SLOT(handleFocusLost()), Qt::UniqueConnection);
        connect(this, SIGNAL(batteryEmpty()), currentView, SLOT(handleBatteryEmpty()), Qt::UniqueConnection);

        // in standby mode, we are interested in focus gain events for dismissing standby
        connect(this, SIGNAL(focusGained()),  mStandbyHandler, SLOT(handleMouseEvent()), Qt::UniqueConnection);

        // connecting key events to standby.
        connect(mKeyHandler, SIGNAL(autofocusKeyPressed()),  mStandbyHandler, SLOT(stopTimer()), Qt::UniqueConnection);
        connect(mKeyHandler, SIGNAL(autofocusKeyReleased()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(mKeyHandler, SIGNAL(captureKeyPressed()),    mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(mKeyHandler, SIGNAL(captureKeyReleased()),   mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(this, SIGNAL(disableStandbyTimer()), mStandbyHandler, SLOT(stopTimer()), Qt::UniqueConnection);
        connect(this, SIGNAL(startStandbyTimer()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);

        // connecting pre-capture view signals to standby.
        connect(currentView, SIGNAL(startStandbyTimer()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPrecaptureView()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(stopStandbyTimer()), mStandbyHandler, SLOT(stopTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPostcaptureView()), mStandbyHandler, SLOT(stopTimer()), Qt::UniqueConnection);

        // connecting precapture view signals to viewmanager slots
        connect(currentView, SIGNAL(changeToPostcaptureView()), this, SLOT(changeToPostcaptureView()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPrecaptureView()),  this, SLOT(changeToPrecaptureView()), Qt::UniqueConnection);

        //connecting scene modes signal
        connect(currentView, SIGNAL(showScenesView()), this, SLOT(showScenesView()), Qt::UniqueConnection);

        connect(currentView, SIGNAL(switchCamera()), this, SLOT(switchCamera()), Qt::UniqueConnection);

        // connecting error signals from precapture view to errormanager.
        connect(currentView, SIGNAL(reportError(CxeError::Id)),   mErrorManager, SLOT(analyze(CxeError::Id)), Qt::UniqueConnection);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiViewManager::connectPostCaptureSignals
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::connectPostCaptureSignals()
{
    CX_DEBUG_ENTER_FUNCTION();

    disconnectSignals();
    QObject *currentView = mMainWindow.currentView();
    if (currentView == mPostcaptureView) {
        // connecting view manager focus events to pre-capture views
        connect(this, SIGNAL(focusGained()),  currentView, SLOT(startTimers()), Qt::UniqueConnection);
        connect(this, SIGNAL(focusLost()),    currentView, SLOT(handleFocusLost()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPrecaptureView()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);

        // connect necessary capturekey signals
        connectCaptureKeySignals();
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*
* CxuiViewManager::disconnectPreCaptureSignals
*/
void CxuiViewManager::disconnectSignals()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Disconnect all existing capture key signals
    mKeyHandler->disconnect();

    disconnect(SIGNAL(focusGained()));
    disconnect(SIGNAL(focusLost()));
    disconnect(SIGNAL(batteryEmpty()));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Raise application to foreground if it's currently (fully) in background.
*/
void CxuiViewManager::toForeground()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mApplicationMonitor
     && mApplicationMonitor->foregroundState() == CxuiApplicationFrameworkMonitor::ForegroundFullyLost) {
        // Bring to foreground and gain focus.
        CX_DEBUG(("CxuiViewManager - fully in background, bringing to foreground now."));
        mMainWindow.raise();
        mMainWindow.activateWindow();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle change in foreground state.
*/
void CxuiViewManager::handleForegroundStateChanged(CxuiApplicationFrameworkMonitor::ForegroundState state)
{
    switch (state) {
    case CxuiApplicationFrameworkMonitor::ForegroundPartiallyLost:
        break;
    case CxuiApplicationFrameworkMonitor::ForegroundFullyLost:
        aboutToLooseFocus();
        break;
    case CxuiApplicationFrameworkMonitor::ForegroundOwned:
        aboutToGainFocus();
        break;
    }
}

// ---------------------------------------------------------------------------
// CxuiViewManager::aboutToLooseFocus()
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::aboutToLooseFocus()
{
    CX_DEBUG_ENTER_FUNCTION();

    emit focusLost();
    disconnectSignals();

    // We do not stop listening to capture key events even if we go to background,
    // as capture key brings us back to foreground.
    connect(mKeyHandler, SIGNAL(captureKeyPressed()), this, SLOT(toForeground()));

    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiViewManager::aboutToGainFocus
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::aboutToGainFocus()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Disconnect capture key event and bringing us to foreground connection (if there is one).
    disconnect(mKeyHandler, SIGNAL(captureKeyPressed()), this, SLOT(toForeground()));

    // we are getting the focus.
    if (mMainWindow.currentView() != mPostcaptureView) {
        connectPreCaptureSignals();
    } else {
        connectPostCaptureSignals();
    }

    if (mKeyHandler) {
        mKeyHandler->listenKeys(true);
    }
    emit focusGained();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle battery emptying
*/
void CxuiViewManager::handleBatteryEmpty()
{
    CX_DEBUG_ENTER_FUNCTION();
    emit batteryEmpty();
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
