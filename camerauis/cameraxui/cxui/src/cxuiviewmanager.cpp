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
#include <QDebug>
#include <QTimer>
#include <hbmainwindow.h>
#include <coemain.h>
#include <avkondomainpskeys.h>  // keyguard
#include <hwrmpowerstatesdkpskeys.h> // battery
#include <QGraphicsSceneEvent>


#include "cxuiviewmanager.h"
#include "cxuivideoprecaptureview.h"
#include "cxuivideoprecaptureview2.h"
#include "cxuistillprecaptureview.h"
#include "cxuistillprecaptureview2.h"
#include "cxuipostcaptureview.h"
#include "cxuicapturekeyhandler.h"
#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxefeaturemanager.h" //  mEngine->featureManager()
#include "cxeviewfindercontrol.h"
#include "cxenamespace.h"
#include "cxuienums.h"
#include "cxutils.h"
#include "xqsettingsmanager.h" // Settings Manager API
#include "cxuidocumentloader.h"
#include "cxuistandby.h"
#include "cxuierrormanager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuiviewmanagerTraces.h"
#endif
#include "cxuiserviceprovider.h"



using namespace Cxe;
using namespace CxUiLayout;


// ---------------------------------------------------------------------------
// CxuiViewManager::CxuiViewManager
//
// ---------------------------------------------------------------------------
//
CxuiViewManager::CxuiViewManager(HbMainWindow &mainWindow, CxeEngine &engine, CxuiCaptureKeyHandler &keyHandler) :
    mMainWindow(mainWindow),
    mStillPrecaptureView(NULL),
    mVideoPrecaptureView(NULL),
    mPostcaptureView(NULL),
    mStillPrecaptureView2(NULL),
    mVideoPrecaptureView2(NULL),
    mEngine(engine),
    mKeyHandler(keyHandler),
    mSettingsManager(NULL),
    mFocused(true),
    mCameraDocumentLoader(NULL)
{
    CX_DEBUG_ENTER_FUNCTION();

    mMainWindow.installEventFilter(this); // in order to filter capture and AF keys

    OstTrace0( camerax_performance, CXUIVIEWMANAGER_CXUIVIEWMANAGER, "msg: e_CX_VIEWMANAGER_CREATE_DOCLOADER 1" );
    mCameraDocumentLoader = new CxuiDocumentLoader;
    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CXUIVIEWMANAGER, "msg: e_CX_VIEWMANAGER_CREATE_DOCLOADER 0" );

    // standby functionality and necessary signal connections
    mStandbyHandler = new CxuiStandby(keyHandler, mCameraDocumentLoader, &engine);

    connect(mStandbyHandler, SIGNAL(aboutToEnterStandby()),this, SLOT(aboutToLooseFocus()));
    connect(mStandbyHandler, SIGNAL(aboutToExitStandby()),this, SLOT(aboutToGainFocus()));

    // error manager, handling errors and notifying users based on their severity
    mErrorManager = new CxuiErrorManager(keyHandler, mCameraDocumentLoader);

    // connecting necessary signals from error manager to release and init camera.
    connect(mErrorManager, SIGNAL(aboutToRecoverError()), this, SLOT(aboutToLooseFocus()));
    connect(mErrorManager, SIGNAL(errorRecovered()), this, SLOT(aboutToGainFocus()));

	if (!CxuiServiceProvider::isCameraEmbedded()) {
		// for embedded mode: don't create view yet, create
		// when engine inits to correct mode
#ifdef FORCE_SECONDARY_CAMERA
	    // 2nd camera hack: always use 1st camera view
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

#else
	    if (mEngine.cameraDeviceControl().cameraIndex() == Cxe::PrimaryCameraIndex) {
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
	    } else {
	        if (mEngine.mode() == Cxe::VideoMode) {
	            createVideoPrecaptureView2();
	            mMainWindow.blockSignals(true);
	            mMainWindow.setCurrentView(mVideoPrecaptureView2, false);
	            mMainWindow.blockSignals(false);
	        } else {
	            createStillPrecaptureView2();
	            mMainWindow.blockSignals(true);
	            mMainWindow.setCurrentView(mStillPrecaptureView2, false);
	            mMainWindow.blockSignals(false);
	        }
	    }
#endif
		connectPreCaptureSignals();
	}
    
    startEventMonitors();

    //connecting initmode signals
    connect(&mEngine.cameraDeviceControl(), SIGNAL(initModeComplete(CxeError::Id)),
            this, SLOT(createPostcaptureView()));
    connect(&mEngine.cameraDeviceControl(), SIGNAL(initModeComplete(CxeError::Id)),
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
    delete mSettingsManager;

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
    bool ok = false;
    CX_DEBUG_ASSERT(mCameraDocumentLoader);

    OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_ADDPRECAPTUREVIEWS, "msg: e_CX_DOCLOADER_LOAD 1" );

    // Use document loader to create widgets and layouts
    // (non-sectioned parts are parsed and loaded)
    mCameraDocumentLoader->load(STILL_1ST_XML, &ok);
    Q_ASSERT_X(ok, "createStillPrecaptureView", "error in xml file parsing");

    QGraphicsWidget *widget = NULL;

    OstTrace0( camerax_performance, DUP2_CXUIVIEWMANAGER_CREATESTILLPRECAPTUREVIEW, "msg: e_CX_DOCLOADER_LOAD 0" );


    OstTrace0( camerax_performance, DUP4_CXUIVIEWMANAGER_ADDPRECAPTUREVIEWS, "msg: e_CX_DOCLOADER_FINDWIDGET 1" );
    // ask for the still precapture view widget pointer
    widget = mCameraDocumentLoader->findWidget(STILL_PRE_CAPTURE_VIEW);
    mStillPrecaptureView = qobject_cast<CxuiStillPrecaptureView *> (widget);
    CX_DEBUG_ASSERT(mStillPrecaptureView);
    OstTrace0( camerax_performance, DUP5_CXUIVIEWMANAGER_ADDPRECAPTUREVIEWS, "msg: e_CX_DOCLOADER_FINDWIDGET 0" );

    // call for needed consturction methods
    mStillPrecaptureView->construct(&mMainWindow, &mEngine, mCameraDocumentLoader, &mKeyHandler);
    // .. and add to main window (which also takes ownership)
    mMainWindow.addView(widget);

    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CREATESTILLPRECAPTUREVIEW, "msg: e_CX_CREATE_STILLPRECAPTUREVIEW 0" );

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::createStillPrecaptureView2
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::createStillPrecaptureView2()
{
    CX_DEBUG_ENTER_FUNCTION();

    bool ok = false;
    CX_DEBUG_ASSERT(mCameraDocumentLoader);

    // Use document loader to create widgets and layouts
    // (non-sectioned parts are parsed and loaded)
    mCameraDocumentLoader->load(STILL_2ND_XML, &ok);
    Q_ASSERT_X(ok, "createStillPrecaptureView2", "error in xml file parsing");

    QGraphicsWidget *widget = NULL;

    // ask for the still precapture view widget pointer
    widget = mCameraDocumentLoader->findWidget(STILL_PRE_CAPTURE_VIEW2);
    mStillPrecaptureView2 = qobject_cast<CxuiStillPrecaptureView2 *> (widget);
    CX_DEBUG_ASSERT(mStillPrecaptureView2);

    // call for needed consturction methods
    mStillPrecaptureView2->construct(&mMainWindow, &mEngine, mCameraDocumentLoader, &mKeyHandler);
    // .. and add to main window (which also takes ownership)
    mMainWindow.addView(widget);

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
    mVideoPrecaptureView->construct(&mMainWindow, &mEngine, mCameraDocumentLoader, &mKeyHandler);

    // add view to main window
    mMainWindow.addView(mVideoPrecaptureView);
    OstTrace0( camerax_performance, DUP1_CXUIVIEWMANAGER_CREATEVIDEOPRECAPTUREVIEW, "msg: e_CX_CREATE_VIDEOPRECAPTUREVIEW 0" );

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiViewManager::createVideoPrecaptureView2
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::createVideoPrecaptureView2()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mCameraDocumentLoader);

    bool ok = false;
    // load and create the default widgets in video xml
    mCameraDocumentLoader->load(VIDEO_2ND_XML, &ok);
    CX_DEBUG_ASSERT(ok);

    // get pointer to videoprecaptureview and do some initialisation
    QGraphicsWidget *widget = NULL;
    widget = mCameraDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_VIEW2);
    mVideoPrecaptureView2 = qobject_cast<CxuiVideoPrecaptureView2 *> (widget);
    CX_DEBUG_ASSERT(mVideoPrecaptureView2);
    mVideoPrecaptureView2->construct(&mMainWindow, &mEngine, mCameraDocumentLoader);

    // add view to main window
    mMainWindow.addView(mVideoPrecaptureView2);

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

#ifdef FORCE_SECONDARY_CAMERA
        // 2nd camera forced: always use 1st camera view
        CX_DEBUG(("CxuiViewManager::getPrecaptureView() forcing primary camera view"));
        camera = Cxe::PrimaryCameraIndex;
#endif // FORCE_SECONDARY_CAMERA

    if (mode == ImageMode) {

        CX_DEBUG(("CxuiViewManager::getPrecaptureView() image mode"));
        if (camera == Cxe::PrimaryCameraIndex) {
            CX_DEBUG(("CxuiViewManager::getPrecaptureView() primary camera"));
            if(!mStillPrecaptureView) {
                createStillPrecaptureView();
            }
            return mStillPrecaptureView;
        } else {
            CX_DEBUG(("CxuiViewManager::getPrecaptureView() secondary camera"));
            if(!mStillPrecaptureView2) {
                createStillPrecaptureView2();
            }
            return mStillPrecaptureView2;
        }
    } else {

        CX_DEBUG(("CxuiViewManager::getPrecaptureView() video mode"));
        if (camera == Cxe::PrimaryCameraIndex) {
            CX_DEBUG(("CxuiViewManager::getPrecaptureView() primary camera"));
            if(!mVideoPrecaptureView) {
                createVideoPrecaptureView();
            }
            return mVideoPrecaptureView;
        } else {
            CX_DEBUG(("CxuiViewManager::getPrecaptureView() secondary camera"));
            if(!mVideoPrecaptureView2) {
                createVideoPrecaptureView2();
            }
            return mVideoPrecaptureView2;
        }
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

    //@todo: refactor postcapture view to new focus and state handling as agreed with package owner
    connect(this, SIGNAL(focusGained()), mPostcaptureView, SLOT(startTimers()), Qt::UniqueConnection);

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

    // connecting necessary pre-capture view signals
    connectPreCaptureSignals();

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
    bool eventWasConsumed = false;

    switch (event->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        eventWasConsumed = mKeyHandler.handleKeyEvent(event);
        break;
    case QEvent::WindowActivate:
        if (object == &mMainWindow) {
            CX_DEBUG(("CxuiViewManager - window activated"));
            // If we gained focus and keyguard is off.
            if (!mFocused) {
                mFocused = true;
                if (mKeyLockState == EKeyguardNotActive) {
                    aboutToGainFocus();
                }
            }
        }
        break;
    case QEvent::WindowDeactivate:
        if (object == &mMainWindow)
        {
            CX_DEBUG(("CxuiViewManager - window de-activated"));
            if (mFocused) {
                mFocused = false;
                aboutToLooseFocus();
            }
        }
        break;
    }

    // No need to call base class implementation, because we derive from QObject directly.
    // QObject::eventFilter() implementation always returns false.
    return eventWasConsumed;
}

// ---------------------------------------------------------------------------
// CxuiViewManager::startEventMonitors
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::startEventMonitors()
{
    CX_DEBUG_ENTER_FUNCTION();
    bool ok = false;

    mSettingsManager = new XQSettingsManager(this);
    ok = connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
                 this,               SLOT(eventMonitor(XQSettingsKey, QVariant)));
    CX_DEBUG_ASSERT(ok);

    // Keyguard status (locked / unlocked)
    XQSettingsKey keyguard(XQSettingsKey::TargetPublishAndSubscribe,
                           KPSUidAvkonDomain.iUid,
                           KAknKeyguardStatus);

    QVariant value = mSettingsManager->readItemValue(keyguard);
    if (value == QVariant::Int) {
        mKeyLockState = value.toInt();
    }
    ok = mSettingsManager->startMonitoring(keyguard);
    CX_DEBUG_ASSERT(ok);

    // Battery status (ok / low / empty)
    XQSettingsKey battery(XQSettingsKey::TargetPublishAndSubscribe,
                          KPSUidHWRMPowerState.iUid,
                          KHWRMBatteryStatus);
    value = mSettingsManager->readItemValue(battery);
    if (value == QVariant::Int) {
        mBatteryStatus = value.toInt();
    }
    ok = mSettingsManager->startMonitoring(battery);
    CX_DEBUG_ASSERT(ok);

    CX_DEBUG_EXIT_FUNCTION();
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
    mKeyHandler.disconnect();

    QObject *currentView = mMainWindow.currentView();

    if (currentView) {
        // If the view class does not implement the named slot, the connect will fail
        // and output some warnings as debug prints. This is by design.
        connect(&mKeyHandler, SIGNAL(autofocusKeyPressed()),  currentView, SLOT(handleAutofocusKeyPressed()));
        connect(&mKeyHandler, SIGNAL(autofocusKeyReleased()), currentView, SLOT(handleAutofocusKeyReleased()));
        connect(&mKeyHandler, SIGNAL(captureKeyPressed()),    currentView, SLOT(handleCaptureKeyPressed()));
        connect(&mKeyHandler, SIGNAL(captureKeyReleased()),   currentView, SLOT(handleCaptureKeyReleased()));
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
        connect(&mKeyHandler, SIGNAL(autofocusKeyPressed()),  mStandbyHandler, SLOT(stopTimer()), Qt::UniqueConnection);
        connect(&mKeyHandler, SIGNAL(autofocusKeyReleased()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(&mKeyHandler, SIGNAL(captureKeyPressed()),    mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(&mKeyHandler, SIGNAL(captureKeyReleased()),   mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);

        // connecting pre-capture view signals to standby.
        connect(currentView, SIGNAL(startStandbyTimer()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPrecaptureView()), mStandbyHandler, SLOT(startTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(stopStandbyTimer()), mStandbyHandler, SLOT(stopTimer()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPostcaptureView()), mStandbyHandler, SLOT(stopTimer()), Qt::UniqueConnection);

        // connecting precapture view signals to viewmanager slots
        connect(currentView, SIGNAL(changeToPostcaptureView()), this, SLOT(changeToPostcaptureView()), Qt::UniqueConnection);
        connect(currentView, SIGNAL(changeToPrecaptureView()),  this, SLOT(changeToPrecaptureView()), Qt::UniqueConnection);
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
    mKeyHandler.disconnect();

    disconnect(SIGNAL(focusGained()));
    disconnect(SIGNAL(focusLost()));
    disconnect(SIGNAL(batteryEmpty()));

    CX_DEBUG_EXIT_FUNCTION();
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

    // we are getting the focus.
    if (mMainWindow.currentView() != mPostcaptureView) {
        connectPreCaptureSignals();
    } else {
        connectPostCaptureSignals();
    }

    emit focusGained();

    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiViewManager::eventMonitor
//
// ---------------------------------------------------------------------------
//
void CxuiViewManager::eventMonitor( const XQSettingsKey& key, const QVariant& value )
{
    CX_DEBUG_ENTER_FUNCTION();

    if (key.uid() == KPSUidAvkonDomain.iUid && key.key() == KAknKeyguardStatus) {
        CX_DEBUG(("new Keyguard value = %d, mKeyLockState = %d", value.toInt(), mKeyLockState));
        if (mSettingsManager->error() == XQSettingsManager::NoError) {
            // New key guard state
            int newKeyLockState = value.toInt();

            // Check if the keylock value has actually changed
            bool keylockValueChanged = (newKeyLockState != mKeyLockState);
            mKeyLockState = newKeyLockState;

            // Ignore unlock events when on background
            bool unlockedOnBackground = (!mFocused && newKeyLockState == EKeyguardNotActive);

            if (keylockValueChanged &&
                !unlockedOnBackground) {
                // Key lock value changed and we need to handle it
                if (mKeyLockState == EKeyguardNotActive) {
                    // All other keylock states are handled as a active in S60 code
                    aboutToGainFocus();
                } else {
                    mFocused = false;
                    aboutToLooseFocus();
                }
            }
        } else {
            // Error from settings manager - ignore
            CX_DEBUG(("mSettingsManager indicated error %d for key guard status", mSettingsManager->error()));
        }
    } else if (key.uid() == KPSUidHWRMPowerState.iUid && key.key() == KHWRMBatteryStatus ) {
        CX_DEBUG(("new battery status = %d, mBatteryStatus = %d", value.toInt(), mBatteryStatus));

        if (mSettingsManager->error() == XQSettingsManager::NoError) {

            // If status changed, check if battery is going empty.
            const int newState = value.toInt();
            if( newState != mBatteryStatus ) {
                mBatteryStatus = newState;

                // Notify that battery is almost empty,
                // need to stop any recordings etc.
                if( mBatteryStatus == EBatteryStatusEmpty ) {
                    emit batteryEmpty();
                }
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
