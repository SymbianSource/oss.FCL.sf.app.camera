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
#include <hbactivitymanager.h>
#include <hbaction.h>

#include "cxuiapplication.h"
#include "cxuiapplicationstate.h"
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

namespace
{
    static const int CXUI_STANDBY_CAMERA_TIMEOUT = 60000; // 60 seconds
}


/*!
* Constructor.
*/
CxuiViewManager::CxuiViewManager(CxuiApplication &application, HbMainWindow &mainWindow, CxeEngine &engine) :
    mApplication(application),
    mMainWindow(mainWindow),
    mEngine(engine),
    mKeyHandler(NULL),
    mCameraDocumentLoader(NULL),
    mApplicationState(NULL),
    mErrorManager(NULL),
    mSceneModeView(NULL)
{
    CX_DEBUG_ENTER_FUNCTION();

    // create mappings to map view name to correct docml file
    mDocmlFilesByView.insert(STILL_PRE_CAPTURE_VIEW, STILL_1ST_XML);
    mDocmlFilesByView.insert(VIDEO_PRE_CAPTURE_VIEW, VIDEO_1ST_XML);
    mDocmlFilesByView.insert(POSTCAPTURE_VIEW, POSTCAPTURE_XML);
    mDocmlFilesByView.insert(STILL_SCENES_VIEW, SCENEMODE_SETTING_XML);

    // Key handler
    mKeyHandler = new CxuiCaptureKeyHandler(mEngine);

    // Document loader
    OstTrace0( camerax_performance, CXUIVIEWMANAGER_CXUIVIEWMANAGER, "msg: e_CX_VIEWMANAGER_CREATE_DOCLOADER 1" );
    mCameraDocumentLoader = new CxuiDocumentLoader(&engine);
    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CXUIVIEWMANAGER, "msg: e_CX_VIEWMANAGER_CREATE_DOCLOADER 0" );

    // Application state
    mApplicationState = new CxuiApplicationState(mApplication, mEngine.settings(), *mKeyHandler, mCameraDocumentLoader);
    CX_ASSERT_ALWAYS(mApplicationState);
    bool ok = connect(mApplicationState, SIGNAL(stateChanged(CxuiApplicationState::State, CxuiApplicationState::State)),
                      this, SLOT(handleApplicationStateChanged(CxuiApplicationState::State, CxuiApplicationState::State)));
    Q_ASSERT_X(ok, "CxuiViewManager", "Application state change signal connect failed.");
    connect(this, SIGNAL(standbyExitRequested()), mApplicationState, SLOT(exitStandby()), Qt::QueuedConnection);

    // Standby timer
    mStandbyTimer.setInterval(CXUI_STANDBY_CAMERA_TIMEOUT);
    mStandbyTimer.setSingleShot(true);
    connect(&mStandbyTimer, SIGNAL(timeout()), mApplicationState, SLOT(enterStandby()));

    // Filter capture and AF keys and follow user activity from mouse events.
    QCoreApplication::instance()->installEventFilter(this);

    // Monitor memory on normal state, not on standby, error or background.
    connect(this, SIGNAL(normalStateEntered()), &mEngine.memoryMonitor(), SLOT(startMonitoring()));
    connect(this, SIGNAL(normalStateExited()), &mEngine.memoryMonitor(), SLOT(stopMonitoring()));

    //connecting initmode signals
    connect(&mEngine.cameraDeviceControl(), SIGNAL(initModeComplete(CxeError::Id)),
            mApplicationState, SLOT(handleApplicationError(CxeError::Id)));

    connect(&mEngine.stillCaptureControl(), SIGNAL(imageCaptured(CxeError::Id, int)),
            mApplicationState, SLOT(handleApplicationError(CxeError::Id)));

    // Register stylesheet. It will be automatically destroyed on application
    // exit.
    HbStyleLoader::registerFilePath(":/camerax/cxui.css");

    // Create the view we are starting in, or connect signals so it
    // will be created once we know the mode we are starting to.
    initStartupView();

    // For UI startup testing, we need to emit applicationReady signal once UI is ready to be used.
    connect(mMainWindow.currentView(), SIGNAL(viewReady()), &mApplication, SIGNAL(applicationReady()));

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Destructor.
*/
CxuiViewManager::~CxuiViewManager()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::instance()->removeEventFilter(this);

    if (!CxuiServiceProvider::isCameraEmbedded()) {
        currentView()->saveActivity();
    }

    delete mApplicationState;
    delete mCameraDocumentLoader;
    delete mKeyHandler;

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Handle change in overall state of application.
* @param newState The new application state.
*/
void CxuiViewManager::handleApplicationStateChanged(CxuiApplicationState::State newState,
                                                    CxuiApplicationState::State oldState)
{
    Q_UNUSED(oldState);
    CX_DEBUG_ENTER_FUNCTION();

    CxuiView *view = qobject_cast<CxuiView *>(mMainWindow.currentView());
    CX_DEBUG(("CxuiViewManager - current view %d", view));

    switch (newState) {
    case CxuiApplicationState::Normal:
        // Disable raising to foreground with capture key.
        disconnect(mKeyHandler, SIGNAL(captureKeyPressed()), this, SLOT(toForeground()));

        connectSignals(view);

        CX_DEBUG(("CxuiViewManager - emitting normalStateEntered"));
        emit normalStateEntered();
        break;
    case CxuiApplicationState::Standby:
         if (view && !view->isStandbyModeSupported()) {
            // If we move to Stanby, and current view does not
            // support standby mode, move to Normal mode right away.
            CX_DEBUG(("CxuiViewManager - view does not support stanbdy, exiting standby right away.."));
            mApplicationState->exitStandby();
        } else {
            handleExitingNormalState();
        }
        break;
    case CxuiApplicationState::Error:
    case CxuiApplicationState::Background:
        handleExitingNormalState();

        if (newState == CxuiApplicationState::Background) {
            // Moved to background.
            // Bring application back to foreground by capture key press
            connect(mKeyHandler, SIGNAL(captureKeyPressed()), this, SLOT(toForeground()));
        }
        break;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method to handle (possible) exiting from normal state.
* @param oldState The previous state.
*/
void CxuiViewManager::handleExitingNormalState()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Store view that is active now.
    CxuiView *view = qobject_cast<CxuiView *>(mMainWindow.currentView());
    CX_DEBUG(("CxuiViewManager - current view %d", view));

    // Emit signal so current view can enter standby.
    emit normalStateExited();
    // Disconnect signals so we don't send key event's etc.
    // Using the view before normalStateExited() signal was emitted,
    // just in case it caused view switch.
    disconnectSignals(view);
    // Make sure standby timer is not running.
    stopStandbyTimer();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for starting standby timer.
*/
void CxuiViewManager::startStandbyTimer()
{
    CX_DEBUG_ENTER_FUNCTION();
    mStandbyTimer.start();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for stopping standby timer.
*/
void CxuiViewManager::stopStandbyTimer()
{
    CX_DEBUG_ENTER_FUNCTION();
    mStandbyTimer.stop();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Init engine to correct mode based on activity being restored.
 */
void CxuiViewManager::initEngine()
{
    Cxe::CameraMode mode = Cxe::ImageMode;
    QString activityId = mApplication.activateId();
    if (activityId == CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY ||
        activityId == CxuiActivityIds::VIDEO_POSTCAPTURE_ACTIVITY) {
        mode = Cxe::VideoMode;
    }
    if (activityId == CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY ||
        activityId == CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY) {
        // init engine only if going to pre-capture
        mEngine.initMode(mode);
    } else {
        // in post-capture don't init but set the correct mode to engine
        // so init can be done later
        mEngine.setMode(mode);
    }
}

/*!
 * Helper function to return current view cast to CxuiView.
 */
CxuiView* CxuiViewManager::currentView() const
{
    CxuiView *view = qobject_cast<CxuiView*> (mMainWindow.currentView());
    CX_ASSERT_ALWAYS(view);
    return view;
}

/*!
* Select and initialize the view we need to start into.
*/
void CxuiViewManager::initStartupView()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mApplication.activateReason() == Hb::ActivationReasonService) {
        // For embedded mode: don't create view yet, create when engine inits to correct mode.
        // Connect signals to set up the view after image/video prepare
        connect(&mEngine.stillCaptureControl(), SIGNAL(imagePrepareComplete(CxeError::Id)),
                this, SLOT(changeToPrecaptureView()));
        connect(&mEngine.videoCaptureControl(), SIGNAL(videoPrepareComplete(CxeError::Id)),
                this, SLOT(changeToPrecaptureView()));

    } else if (mApplication.activateReason() == Hb::ActivationReasonActivity ) {
        // restoring activity, read startup view from stored activity

        // view to start in
        QString viewName = STILL_PRE_CAPTURE_VIEW;

        bool preCapture = true;
        QString activityId = mApplication.activateId();
        if (activityId == CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY) {
            viewName = STILL_PRE_CAPTURE_VIEW;
        } else if (activityId == CxuiActivityIds::STILL_POSTCAPTURE_ACTIVITY) {
            viewName = POSTCAPTURE_VIEW;
            preCapture = false;
        } else if (activityId == CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY) {
            viewName = VIDEO_PRE_CAPTURE_VIEW;
        } else if (activityId == CxuiActivityIds::VIDEO_POSTCAPTURE_ACTIVITY) {
            viewName = POSTCAPTURE_VIEW;
            preCapture = false;
        }

        CxuiView *view = createView(viewName);
        mMainWindow.setCurrentView(view, false);

        if (preCapture) {
            connectPreCaptureSignals();
        } else {
            connectPostCaptureSignals();
        }

        // Check the current application state, signalled to handleApplicationStateChanged.
        mApplicationState->startMonitoring();

        // restore view from activity
        bool ok = mApplication.activityManager()->waitActivity();

        view->restoreActivity(activityId,
                              mApplication.activityManager()->activityData(mApplication.activateId()));

        clearAllActivities();
    } else {
        // normal start
        // no activity id, get default view from engine state
        CxuiView *view = NULL;
        if (mEngine.mode() == Cxe::VideoMode) {
            view = createView(VIDEO_PRE_CAPTURE_VIEW);
        } else {
            view = createView(STILL_PRE_CAPTURE_VIEW);
        }

        mMainWindow.setCurrentView(view, false);
        connectPreCaptureSignals();

        // Check the current application state, signalled to handleApplicationStateChanged.
        mApplicationState->startMonitoring();

        clearAllActivities();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Create view and add it to main window.
*/
CxuiView* CxuiViewManager::createView(const QString &viewName)
{
    CX_DEBUG_ENTER_FUNCTION();

    OstTrace0( camerax_performance, CXUIVIEWMANAGER_CREATEVIEW, "msg: e_CX_CREATE_VIEW 1" );

    CX_DEBUG(("View name: %s", viewName.toAscii().data()));

    CxuiView* view = mViews[viewName];
    if (view == NULL) {
        CX_DEBUG(("View not loaded yet, loading now"));
        bool ok = false;
        // Use document loader to create widgets and layouts
        // (non-sectioned parts are parsed and loaded)
        QString docmlFile = mDocmlFilesByView[viewName];
        CX_DEBUG_ASSERT(mCameraDocumentLoader);
        CX_ASSERT_ALWAYS(!docmlFile.isNull());

        OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_ADDPRECAPTUREVIEWS, "msg: e_CX_DOCLOADER_LOAD 1" );

        mCameraDocumentLoader->load(docmlFile, &ok);

        Q_ASSERT_X(ok, "createView", "error in xml file parsing");

        OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_CREATEVIEW, "msg: e_CX_DOCLOADER_LOAD 0" );

        OstTrace0( camerax_performance, DUP4_CXUIVIEWMANAGER_ADDVIEWS, "msg: e_CX_DOCLOADER_FINDWIDGET 1" );
        QGraphicsWidget *widget = NULL;
        // ask for the view widget pointer
        widget = mCameraDocumentLoader->findWidget(viewName);
        view = qobject_cast<CxuiView *> (widget);
        CX_DEBUG_ASSERT(view);
        OstTrace0( camerax_performance, DUP5_CXUIVIEWMANAGER_ADDVIEWS, "msg: e_CX_DOCLOADER_FINDWIDGET 0" );

        // call for needed consturction methods
        view->construct(&mMainWindow, &mEngine, mCameraDocumentLoader, mKeyHandler, mApplication.activityManager());
        // .. and add to main window (which also takes ownership)
        OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_MAINWINDOW_ADDVIEW, "msg: e_CX_MAINWINDOW_ADDVIEW 1" );
        mMainWindow.addView(view);
        mViews.insert(viewName, view);

        OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_MAINWINDOW_ADDVIEW, "msg: e_CX_MAINWINDOW_ADDVIEW 0" );
        OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CREATEVIEW, "msg: e_CX_CREATE_STILLPRECAPTUREVIEW 0" );

    }

    CX_DEBUG_EXIT_FUNCTION();
    return view;
}

/*!
* Create scene mode view.
*/
CxuiView* CxuiViewManager::createSceneModesView()
{
    CX_DEBUG_ENTER_FUNCTION();

    mSceneModeView = qobject_cast<CxuiSceneModeView*>(createView(STILL_SCENES_VIEW));
    mSceneModeView->loadBackgroundImages();

    CX_DEBUG_EXIT_FUNCTION();
    return mSceneModeView;
}

/*!
* CxuiViewManager::showScenesView
* A private method for displaying Scene Modes selection view
*/
void CxuiViewManager::showScenesView()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Disconnect signals from old view.
    disconnectSignals();

    if (!mSceneModeView) {
        createSceneModesView();
    }
    else {
        mSceneModeView->loadBackgroundImages();
    }
    CX_DEBUG_ASSERT(mSceneModeView);
    mMainWindow.setCurrentView(mSceneModeView, false);
    stopStandbyTimer();
    connectSceneModeSignals();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get the precapture view appropriate for given camera mode and index.
* If the view does not exist, it is created.
* @param mode Which camera mode the view should match.
* @param index Which camera index the view should match.
*/
CxuiPrecaptureView*
CxuiViewManager::getPrecaptureView(Cxe::CameraMode mode, Cxe::CameraIndex camera)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxuiViewManager::getPrecaptureView - mode=%d, camera index=%d", mode, camera));

    CX_DEBUG_ASSERT(camera == Cxe::PrimaryCameraIndex);

    if (mode == ImageMode) {
        CX_DEBUG(("CxuiViewManager::getPrecaptureView - image mode"));
        return qobject_cast<CxuiPrecaptureView*>(createView(STILL_PRE_CAPTURE_VIEW));

    } else {
        CX_DEBUG(("CxuiViewManager::getPrecaptureView - video mode"));
        return qobject_cast<CxuiPrecaptureView*>(createView(VIDEO_PRE_CAPTURE_VIEW));
    }
}

/*!
* Get a pointer to the document loader instance.
*/
CxuiDocumentLoader *CxuiViewManager::documentLoader()
{
    return mCameraDocumentLoader;
}

/*!
* Get reference to application state instance.
*/
CxuiApplicationState &CxuiViewManager::applicationState()
{
    return *mApplicationState;
}


/*!
* Move to post-capture view.
*/
void CxuiViewManager::changeToPostcaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Disconnect signals from old view.
    disconnectSignals();

    CxuiView *postCaptureView = createView(POSTCAPTURE_VIEW);

    mMainWindow.setCurrentView(postCaptureView, false);

    // Connecting all necessary signals for postcapture view.
    // Not connected yet if not in normal state. We connect the signals
    // once we enter normal state again.
    if (mApplicationState->currentState() == CxuiApplicationState::Normal) {
        connectPostCaptureSignals();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Move to pre-capture view.
*/
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

    // If normal mode is not active, don't switch to precapture view and reserve camera now.
    if (mApplicationState->currentState() != CxuiApplicationState::Normal) {
        CX_DEBUG(("CxuiViewManager - Change to precapture blocked as not normal mode (error, standby, background)."));
    } else {

        // Disconnect signals from old view.
        disconnectSignals();

        HbView *view = getPrecaptureView(mEngine.mode(),
            mEngine.cameraDeviceControl().cameraIndex());
        mMainWindow.setCurrentView(view, false);

        if (mSceneModeView){
            mViews.remove(STILL_SCENES_VIEW);
            delete mSceneModeView;
            mSceneModeView = NULL;
        }
        // connecting necessary pre-capture view signals
        connectPreCaptureSignals();

        // Make sure engine prepares for new image/video if necessary
        mEngine.initMode(mEngine.mode());

        startStandbyTimer();

    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Switch between cameras.
*/
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
    mMainWindow.setCurrentView(view, false);
    view->updateOrientation(nextViewOrientation);

    connectPreCaptureSignals();

    mEngine.cameraDeviceControl().switchCamera(nextCamera);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Event filter function.
* Used to get mouse and key events for standby and capture key handling.
* @param object Target object.
* @param event Event to be checked.
*/
bool CxuiViewManager::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)

    bool eventWasConsumed = false;
    bool userActivity = false;

    switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        userActivity = true;
        eventWasConsumed = mKeyHandler->handleKeyEvent(event);
        if (eventWasConsumed && mApplicationState->currentState() == CxuiApplicationState::Standby) {
            // Queued exit from standby.
            emit standbyExitRequested();
        }
        break;

    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
        userActivity = true;
        break;
    case QEvent::GraphicsSceneMouseRelease:
        userActivity = true;
        if (mApplicationState->currentState() == CxuiApplicationState::Standby) {
            // Queued exit from standby.
            // Standby popup in view can receive mouse event before it is dismissed.
            emit standbyExitRequested();
        }
        break;
    }

    // Restart standby timer if there is user activity.
    // Only restart the timer if it is running, do not set it running here.
    if (userActivity && mStandbyTimer.isActive()) {
        startStandbyTimer();
    }

    // No need to call base class implementation, because we derive from QObject directly.
    // QObject::eventFilter() implementation always returns false.
    return eventWasConsumed;
}

/*!
* Connect signals specific to given view.
*/
void CxuiViewManager::connectSignals(QObject *view)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (view) {
        if (view == mViews[POSTCAPTURE_VIEW]) {
            connectPostCaptureSignals();
        } else if (view == mSceneModeView) {
            connectSceneModeSignals();
        } else {
            connectPreCaptureSignals();
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Disconnect signals.
* We don't want to send or receive signals with inactive views, so this is done every time changing a view.
* @param view View object from which signals are disconnected. If NULL is given, current view is used.
*/
void CxuiViewManager::disconnectSignals(QObject *view)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Disconnect all existing capture key signals
    mKeyHandler->disconnect();

    if (!view) {
        // If view is not given, take current view.
        view = mMainWindow.currentView();
    }

    CX_DEBUG(("CxuiViewManager - disconnecting from view %d", view));
    if (view) {
        // Disconnect all signals from current view to us.
        disconnect(view, 0, this, 0);
        // Disconnect all signals from us to current view.
        disconnect(this, 0, view, 0);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Connect signals to pre-capture view.
*/
void CxuiViewManager::connectPreCaptureSignals()
{
    CX_DEBUG_ENTER_FUNCTION();

    disconnectSignals();

    HbView *currentView = mMainWindow.currentView();

    if (currentView != mViews[POSTCAPTURE_VIEW]) {
        // connects all capture key signals.
        connectCaptureKeySignals();

        // connecting pre-capture view signals to standby timer.
        connect(currentView, SIGNAL(startStandbyTimer()),       this, SLOT(startStandbyTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(stopStandbyTimer()),        this, SLOT(stopStandbyTimer()),  Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPrecaptureView()),  this, SLOT(startStandbyTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPostcaptureView()), this, SLOT(stopStandbyTimer()),  Qt::UniqueConnection);

        // connecting pre-capture view signals to viewmanager slots
        connect(currentView, SIGNAL(changeToPostcaptureView()), this, SLOT(changeToPostcaptureView()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPrecaptureView()),  this, SLOT(changeToPrecaptureView()),  Qt::UniqueConnection);

        //connecting scene modes signal
        connect(currentView, SIGNAL(showScenesView()), this, SLOT(showScenesView()), Qt::UniqueConnection);

        connect(currentView, SIGNAL(switchCamera()), this, SLOT(switchCamera()), Qt::UniqueConnection);

        // connecting error signals from precapture view to application state.
        connect(currentView, SIGNAL(errorEncountered(CxeError::Id)),
                mApplicationState, SLOT(handleApplicationError(CxeError::Id)),
                Qt::UniqueConnection);

        // Standby signals
        connect(this, SIGNAL(normalStateEntered()), currentView, SLOT(exitStandby()), Qt::UniqueConnection);
        connect(this, SIGNAL(normalStateExited()), currentView, SLOT(enterStandby()), Qt::UniqueConnection);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Connect signals to post-capture view.
*/
void CxuiViewManager::connectPostCaptureSignals()
{
    CX_DEBUG_ENTER_FUNCTION();

    disconnectSignals();
    QObject *currentView = mMainWindow.currentView();
    if (currentView == mViews[POSTCAPTURE_VIEW]) {
        connect(currentView, SIGNAL(changeToPrecaptureView()), this, SLOT(changeToPrecaptureView()), Qt::UniqueConnection);

        // Standby signals
        connect(this, SIGNAL(normalStateEntered()), currentView, SLOT(exitStandby()), Qt::UniqueConnection);
        connect(this, SIGNAL(normalStateExited()), currentView, SLOT(enterStandby()), Qt::UniqueConnection);

        // connect necessary capturekey signals
        connectCaptureKeySignals();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Connect signals to scene mode view.
*/
void CxuiViewManager::connectSceneModeSignals()
{
    CX_DEBUG_ENTER_FUNCTION();
    disconnectSignals();

    connectCaptureKeySignals();

    // Standby signals for releasing camera
    connect(this, SIGNAL(normalStateEntered()), mSceneModeView, SLOT(exitStandby()));
    connect(this, SIGNAL(normalStateExited()), mSceneModeView, SLOT(enterStandby()));

    // Moving back to pre-capture view
    connect(mSceneModeView, SIGNAL(viewCloseEvent()), this, SLOT(changeToPrecaptureView()));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Connect key handler capture key signals.
*/
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

/*!
 * Clears all camera activities from activity manager.
 */
void CxuiViewManager::clearAllActivities()
{
    HbActivityManager *activityManager = mApplication.activityManager();
    activityManager->removeActivity(CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY);
    activityManager->removeActivity(CxuiActivityIds::STILL_POSTCAPTURE_ACTIVITY);
    activityManager->removeActivity(CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY);
    activityManager->removeActivity(CxuiActivityIds::VIDEO_POSTCAPTURE_ACTIVITY);
}

/*!
* Raise application to foreground if it's currently (fully) in background.
*/
void CxuiViewManager::toForeground()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mApplicationState->currentState() == CxuiApplicationState::Background) {
        // Bring to foreground and gain focus.
        CX_DEBUG(("CxuiViewManager - in background, bringing to foreground now."));
        mMainWindow.raise();
        mMainWindow.activateWindow();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
