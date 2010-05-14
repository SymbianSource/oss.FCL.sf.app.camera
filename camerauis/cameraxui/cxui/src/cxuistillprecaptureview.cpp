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
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbtransparentwindow.h>
#include <QVariant>
#include <QMetaType>
#include <hbslider.h>
#include <hblistwidget.h>
#include <hbdialog.h>
#include <hblistwidgetitem.h>
#include <hbtoolbarextension.h>
#include <hbtoolbutton.h>
#include <hbdeviceprofile.h> // HbDeviceProfile
#include <hbmenu.h>
#include <hbicon.h>

#include "cxuiselftimer.h"
#include "cxeengine.h"
#include "cxeviewfindercontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxeautofocuscontrol.h"
#include "cxuicapturekeyhandler.h"
#include "cxutils.h"
#include "cxenamespace.h" // CxeSettingIds
#include "cxuienums.h"
#include "cxesettings.h"
#include "cxefeaturemanager.h" // mEngine->featureManager()
#include "cxuidocumentloader.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuistillprecaptureviewTraces.h"
#endif
#include "cxuistillprecaptureview.h"
#include "cxuiserviceprovider.h"
#include "cxuisettingdialog.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuizoomslider.h"

using namespace Cxe;
using namespace CxUiLayout;
using namespace CxUiInternal;

const int CXUI_CAPTURE_PENDING_TIMEOUT = 5000; // 5 seconds
const int CXUI_IMAGES_LEFT_LIMIT = 99; // images left is shown only if below this limit

CxuiStillPrecaptureView::CxuiStillPrecaptureView( QGraphicsItem *parent) :
    CxuiPrecaptureView(parent),
    mSelfTimer(NULL),
    mFlashSetting(NULL),
    mStillSettingsPopup(NULL),
    mSceneModePopup(NULL),
    mCapturePending(false),
    mImagesLeft(NULL),
    mImagesLeftContainer(NULL),
    mPendingAfCanceling(false)
{
    CX_DEBUG_IN_FUNCTION();
}

CxuiStillPrecaptureView::~CxuiStillPrecaptureView()
{
    CX_DEBUG_IN_FUNCTION();
    delete mSelfTimer;
}

void CxuiStillPrecaptureView::construct(HbMainWindow *mainwindow, CxeEngine *engine,
                                        CxuiDocumentLoader *documentLoader,
                                        CxuiCaptureKeyHandler *keyHandler)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUISTILLPRECAPTUREVIEW_CONSTRUCT, "msg: e_CX_STILLPRECAPVIEW_CONSTRUCT 1" );

    // constuct base class
    CxuiPrecaptureView::construct(mainwindow, engine, documentLoader, keyHandler);

    connect(&mEngine->autoFocusControl(), SIGNAL(stateChanged(CxeAutoFocusControl::State,CxeError::Id)),
            this, SLOT(handleAutoFocusStateChanged(CxeAutoFocusControl::State,CxeError::Id)));
    connect(&mEngine->stillCaptureControl(), SIGNAL(snapshotReady(CxeError::Id, const QPixmap&, int)),
            this, SLOT(handleSnapshot(CxeError::Id)));
    connect(&mEngine->stillCaptureControl(), SIGNAL(stateChanged(CxeStillCaptureControl::State, CxeError::Id)),
            this, SLOT(handleStillCaptureStateChanged(CxeStillCaptureControl::State, CxeError::Id)));
    connect(&mEngine->viewfinderControl(), SIGNAL(stateChanged(CxeViewfinderControl::State, CxeError::Id)),
            this, SLOT(handleViewfinderStateChanged(CxeViewfinderControl::State, CxeError::Id)));
    connect(&(mEngine->settings()), SIGNAL(sceneChanged(CxeScene&)),
            this, SLOT(handleSceneChanged(CxeScene&)));
    connect(&mEngine->stillCaptureControl(), SIGNAL(availableImagesChanged()),
            this, SLOT(updateImagesLeftLabel()));

    loadDefaultWidgets();
    hideControls();

    mSelfTimer = new CxuiSelfTimer(mEngine->settings());
    connect(mSelfTimer, SIGNAL(timerFinished()), this, SLOT(focusAndCapture()));

    OstTrace0( camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_CONSTRUCT, "msg: e_CX_STILLPRECAPVIEW_CONSTRUCT 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    // get pointer to the viewfinder
    QGraphicsWidget *widget = NULL;
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_VIEWFINDER);
    mViewfinder = qobject_cast<HbTransparentWindow *>(widget);
    CX_DEBUG_ASSERT(mViewfinder);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_QUALITY_ICON);
    mQualityIcon = qobject_cast<HbLabel *>(widget);
    CX_DEBUG_ASSERT(mQualityIcon);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_INDICATOR_CONTAINER);
    mIndicators = qobject_cast<HbWidget *>(widget);
    CX_DEBUG_ASSERT(mIndicators);
    // create background for indicator container
    createWidgetBackgroundGraphic(mIndicators, TRANSPARENT_BACKGROUND_GRAPHIC);

    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiStillPrecaptureView::loadWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    if( mWidgetsLoaded ) {
        CX_DEBUG(("Widgets already loaded"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    OstTrace0( camerax_performance, CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_STILLPRECAPTUREVIEW_LOADWIDGETS 1" );

    bool ok = false;

    OstTrace0( camerax_performance, DUP4_CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_DOCUMENTLOADER_LOAD 1" );
    mDocumentLoader->load(STILL_1ST_XML, STILL_PRE_CAPTURE_WIDGETS_SECTION, &ok);
    Q_ASSERT_X(ok, "camerax ui", "error in xml file parsing");
    if (CxuiServiceProvider::isCameraEmbedded()) {
        mDocumentLoader->load(STILL_1ST_XML, STILL_PRE_CAPTURE_EMBEDDED_SECTION, &ok);
    } else {
        mDocumentLoader->load(STILL_1ST_XML, STILL_PRE_CAPTURE_STANDALONE_SECTION, &ok);
    }
    OstTrace0( camerax_performance, DUP5_CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_DOCUMENTLOADER_LOAD 0" );
    Q_ASSERT_X(ok, "camerax ui", "error in xml file parsing");

    // get pointers to ui components from the layout data
    QGraphicsWidget *widget = NULL;
    QObject *object = NULL;

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_ZOOM_SLIDER);
    mSlider = qobject_cast<CxuiZoomSlider *>(widget);
    CX_DEBUG_ASSERT(mSlider);
    addIncreaseDecreaseButtons(mSlider);
    createWidgetBackgroundGraphic(mSlider, TRANSPARENT_BACKGROUND_GRAPHIC);

    if (mSelfTimer) {
        // let selftimer class get needed selftimer related widgets
        // from the documentloader
        mSelfTimer->loadSelftimerWidgets(mDocumentLoader);
    }

    // create background for selftimer containers
    HbWidget *container = NULL;
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_CONTAINER);
    container = qobject_cast<HbWidget *>(widget);
    CX_DEBUG_ASSERT(container);
    createWidgetBackgroundGraphic(container, TRANSPARENT_BACKGROUND_GRAPHIC);

    // connect selftimer start button to hide controls
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_START_BUTTON);
    HbPushButton *startButton = qobject_cast<HbPushButton *>(widget);
    CX_DEBUG_ASSERT(startButton);
    connect(startButton, SIGNAL(released()), this, SLOT(hideControls()));

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_TOOLBAR);
    mToolBar = qobject_cast<HbToolBar *>(widget);
    CX_DEBUG_ASSERT(mToolBar);

    object = mDocumentLoader->findObject(STILL_PRE_CAPTURE_FLASH_ACTION);
    mFlashSetting = qobject_cast<HbAction *>(object);
    CX_DEBUG_ASSERT(mFlashSetting);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_IMAGES_LEFT_LABEL);
    mImagesLeft = qobject_cast<HbLabel *>(widget);
    CX_DEBUG_ASSERT(mImagesLeft);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_IMAGES_LEFT_CONTAINER);
    mImagesLeftContainer = qobject_cast<HbWidget *>(widget);
    CX_DEBUG_ASSERT(mImagesLeftContainer);
    createWidgetBackgroundGraphic(mImagesLeftContainer, TRANSPARENT_BACKGROUND_GRAPHIC);
    updateImagesLeftLabel();

    if (CxuiServiceProvider::isCameraEmbedded()) {
        CX_DEBUG(("EMBEDDED: camera in embedded mode"));

        if (!CxuiServiceProvider::instance()->allowQualityChange()) {

            CX_DEBUG(("EMBEDDED: don't allow quality change"));
            HbAction* iq = qobject_cast<HbAction*>(mDocumentLoader->findObject(STILL_PRE_CAPTURE_IMAGE_QUALITY_ACTION));
            if (iq) {
                CX_DEBUG(("EMBEDDED: setting image quality setting to disabled"));
                iq->setEnabled(false);
            }
        }
        if (!CxuiServiceProvider::instance()->allowModeSwitching()) {
            CX_DEBUG(("EMBEDDED: don't allow mode switching"));

            HbAction *video = qobject_cast<HbAction*>(mDocumentLoader->findObject(STILL_PRE_CAPTURE_GOTO_VIDEO_ACTION));
            if (video) {
                CX_DEBUG(("EMBEDDED: setting mode switch to disabled"));
                video->setEnabled(false);
            }
        }
        if (!CxuiServiceProvider::instance()->allowCameraSwitching()) {
            CX_DEBUG(("EMBEDDED: don't allow camera switching"));
        }

    }

    // update toolbar flash icon
    int flash;
    if (mEngine->settings().get(CxeSettingIds::FLASH_MODE, flash) == CxeError::None) {
        handleSettingValueChanged(CxeSettingIds::FLASH_MODE, flash);
    }

    // update toolbar scene mode
    QString scene;
    if (mEngine->settings().get(CxeSettingIds::IMAGE_SCENE, scene) == CxeError::None) {
        handleSettingValueChanged(CxeSettingIds::IMAGE_SCENE, scene);
    }

    hideControls();

    mWidgetsLoaded = true;

    OstTrace0( camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_STILLPRECAPTUREVIEW_LOADWIDGETS 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Initializing settings grid
*/
void CxuiStillPrecaptureView::initializeSettingsGrid()
{
    if(!mSettingsGrid) {
        HbAction* action(NULL);

        // Initialize settings grid
        mSettingsGrid = new HbToolBarExtension;

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_exposure.svg"), hbTrId("txt_cam_button_exposure_compensation"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::EV_COMPENSATION_VALUE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_iso.svg"), hbTrId("txt_cam_button_iso"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::LIGHT_SENSITIVITY);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_small_rgb.svg"), hbTrId("txt_cam_button_color_tone"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::COLOR_TONE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_white_balance.svg"), hbTrId("txt_cam_button_white_balance"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::WHITE_BALANCE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_sharpness.svg"), hbTrId("txt_cam_grid_sharpness"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::SHARPNESS);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_contrast.svg"), hbTrId("txt_cam_button_contrast"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::CONTRAST);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        mSettingsGrid->addAction(HbIcon("qtg_mono_face_tracking.svg"), hbTrId("txt_cam_button_face_tracking"), this, SLOT(launchNotSupportedNotification()));
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        connect(mKeyHandler, SIGNAL(autofocusKeyPressed()), mSettingsGrid, SLOT(close()));
    }
}

void CxuiStillPrecaptureView::launchSceneModesPopup()
{
    CX_DEBUG_ENTER_FUNCTION();
    hideControls();
    emit showScenesView();
    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiStillPrecaptureView::handleSnapshot(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mPendingAfCanceling) {
        mPendingAfCanceling = false;
        mEngine->autoFocusControl().cancel();
    }

    if (error == CxeError::None) {
        // Enging may emit snapshot events even if we don't need the snapshot.
        // Check if postcapture is actually wanted and change view now if so.
        if(isPostcaptureOn()) {
            emit changeToPostcaptureView();
            if (mSlider) {
                mSlider->hide();
            }
        }
    } else {
        emit reportError(error);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
     Method for starting focusing which will be followed by capture.
     Meant for selftimer purposes. If autofocusing is not supported, capture
     will start without focusing.

     \sa capture()
     \sa handleAutofocusKeyPressed()
     \sa handleCaptureKeyPressed()
     \sa setCapturePending()
 */
void CxuiStillPrecaptureView::focusAndCapture()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mEngine->autoFocusControl().supported()) {
        // autofocus is not supported, so start capturing straight away
        capture();
    } else {
        // start focusing
        // Auto-focus can only work if viewfinder is running
        if (mEngine->viewfinderControl().state() == CxeViewfinderControl::Running) {
            mEngine->autoFocusControl().start(false);
        }
        setCapturePending();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::capture()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCapturePending = false;

    if (mEngine->mode() == Cxe::ImageMode) {
        // do not start capturing, if it is already ongoing
        // the user might be repeatedly triggering capture key
        if (mEngine->stillCaptureControl().state() == CxeStillCaptureControl::Ready) {
            // If focusing in progress, cancel it now.
            // Set capture pending and continue once focusing is cancelled.
            if(mEngine->autoFocusControl().state() == CxeAutoFocusControl::InProgress) {
                mEngine->autoFocusControl().cancel();
                setCapturePending();
            } else {
                // Engine uses disk space *estimate* for imagesLeft() so it
                // should not cause significant delay / additional shutter lag.
                if (mEngine->stillCaptureControl().imagesLeft()) {
                    mEngine->stillCaptureControl().reset();  //! @todo: Do not delete snapshots before images are really saved
                    mEngine->stillCaptureControl().capture();
                } else {
                    launchDiskFullNotification();
                }
            }
        } else {
            setCapturePending();
        }
    }

    // after capturing check what is the new amount for images left
    updateImagesLeftLabel();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::setCapturePending()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCapturePending = true;
    QTimer::singleShot(CXUI_CAPTURE_PENDING_TIMEOUT, this, SLOT(resetCapturePendingFlag()));
    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiStillPrecaptureView::goToVideo()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_GOTOVIDEO, "msg: e_CX_GO_TO_VIDEO_MODE 1" );

    hideControls();
    mEngine->initMode(VideoMode);
    emit changeToPrecaptureView();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    CxuiStillPrecaptureView::handleAutoFocusStateChanged

    \sa focusAndCapture()
    \sa capture()
    \sa handleAutofocusKeyPressed()
*/
void CxuiStillPrecaptureView::handleAutoFocusStateChanged(CxeAutoFocusControl::State newstate, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeAutoFocusControl::Mode mode = mEngine->autoFocusControl().mode();
    CX_DEBUG( ("CxuiStillPrecaptureView::handleAutoFocusStateChanged <> error: %d mode: %d state %d", error, mode,
            newstate ) );


    switch (newstate) {
    case CxeAutoFocusControl::Failed:
    case CxeAutoFocusControl::Ready:
        if (mCapturePending) {
            // focus and capture were grouped together, capture straight away
            capture();
        }
        break;
    case CxeAutoFocusControl::Unknown:
        // Capture key was pressed, focus has been cancelled,
        // continue with capturing.
        if(mCapturePending) {
            CX_DEBUG(("mCapturePending is true, starting capture now"));
            capture();
        }
        break;
    default:
        break;
    }
    //}

    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiStillPrecaptureView::updateOrientation(Qt::Orientation orientation)
{
    CX_DEBUG_ENTER_FUNCTION();

    hideControls();
    mMainWindow->setOrientation(orientation);

    if (orientation == Qt::Horizontal) {
        CX_DEBUG(("New screen orientation is horizontal"));
        mToolBar->setOrientation(Qt::Vertical);
    } else {
        CX_DEBUG(("New screen orientation is vertical"));
        mToolBar->setOrientation(Qt::Horizontal);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle capture key full press.
*/
void CxuiStillPrecaptureView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mEngine->mode() == Cxe::ImageMode) {
        OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SHOT, "msg: e_CX_SHOT_TO_SHOT 1");
        OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SNAP, "msg: e_CX_SHOT_TO_SNAPSHOT 1");
        OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SAVE, "msg: e_CX_SHOT_TO_SAVE 1");

        menu()->close();

        // close open dialogs
        closeDialogs();

        if (mSelfTimer && mSelfTimer->isEnabled() ) {
            if (mSelfTimer->isOngoing()) {
                // dismiss selftimer and capture image
                mSelfTimer->cancel();
                capture();
            } else {
                // start selftimer if it is enabled and not yet started
                mSelfTimer->startTimer();
            }
        } else {
            // normal capture
            capture();
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle capture key half pressed (aka "autofocus key press").
*/
void CxuiStillPrecaptureView::handleAutofocusKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUISTILLPRECAPTUREVIEW_AUTOFOCUS, "msg: e_CX_AUTOFOCUS_LOCK 1" );

    // Base class handling needs to happen first to keep e.g. settings dialogs handling sane.
    CxuiPrecaptureView::handleAutofocusKeyPressed();

    if (!mEngine->autoFocusControl().supported()) {
        CX_DEBUG(("Autofocus not supported, ignoring key press"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    // in case of selftimer being enabled, autofocuskey does not start focusing
    if (mSelfTimer && mSelfTimer->isEnabled()){
        CX_DEBUG(("Selftimer enabled, ignoring autofocus key press"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    // Auto-focus can only work if viewfinder is running
    if (mEngine->viewfinderControl().state() == CxeViewfinderControl::Running) {

        mEngine->autoFocusControl().start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::handleAutofocusKeyReleased()
{
    CX_DEBUG_ENTER_FUNCTION();

    // in case of selftimer being enabled, autofocuskey should not do anything
    if (mSelfTimer && mSelfTimer->isEnabled()){
        CX_DEBUG(("Selftimer enabled, ignoring autofocus key release"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    if (mEngine->autoFocusControl().supported()) {
        // Check that capture is not in progress, or pending.
        // Otherwise wouldn't want to cancel the focus.
        CxeStillCaptureControl::State state(mEngine->stillCaptureControl().state());
        if (state != CxeStillCaptureControl::Capturing && !mCapturePending) {
            mEngine->autoFocusControl().cancel();
        } else {
            // set pending AF release
            mPendingAfCanceling = true;
        }

    } else {
        CX_DEBUG(("Autofocus not supported, ignoring key release"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::showEvent(QShowEvent *event)
{
    CxuiPrecaptureView::showEvent(event);

    updateImagesLeftLabel();
    updateQualityIcon();

    // cancel selftimer when returning to precapture
    // since selftimer needs to be turned off after capturing an image
    // (with selftimer postcapture is always shown)
    if (mSelfTimer && mSelfTimer->isEnabled()) {
        mSelfTimer->cancel();
    }

    // In case the user presses and holds the auto-focus key (ie. half-press)
    // in post-capture view, we need to start auto-focusing when entering
    // the still precapture view.
    if (event->type() == QEvent::Show &&
            mKeyHandler && mKeyHandler->isAutofocusKeyPressed()) {

        CX_DEBUG(("Still pre-capture coming visible and auto-focus key pressed -> starting AF"));
        handleAutofocusKeyPressed();
    }
}

void CxuiStillPrecaptureView::showToolbar()
{
    if (mEngine && mEngine->isEngineReady()) {
        CxuiPrecaptureView::showToolbar();
    }
}


void CxuiStillPrecaptureView::handleViewfinderStateChanged(
    CxeViewfinderControl::State newState, CxeError::Id /*error*/)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (newState == CxeViewfinderControl::Running) {

        // the toolbar is created when the viewfinder is started the first time
        // in order to hide the delay caused by the creation
        if(!mWidgetsLoaded) {
            loadWidgets();
        }

        if(mMainWindow->currentView() == this &&
            mKeyHandler->isAutofocusKeyPressed()) {
            // Viewfinder just started and the user is pressing the auto-focus key.
            // Start focusing.

            CX_DEBUG(("Viewfinder starting and auto-focus key pressed -> starting AF"));
            handleAutofocusKeyPressed();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::handleStillCaptureStateChanged(
    CxeStillCaptureControl::State newState, CxeError::Id /*error*/)
{
    if (newState == CxeStillCaptureControl::Ready) {
        OstTrace0(camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_SHOT_TO_SHOT, "msg: e_CX_SHOT_TO_SHOT 0");

        if (mCapturePending) {
            CX_DEBUG(("mCapturePending is true, starting capture now"));
            capture();
        }
    }
}

void CxuiStillPrecaptureView::resetCapturePendingFlag()
{
    CX_DEBUG_IN_FUNCTION();

    mCapturePending = false;
}

void CxuiStillPrecaptureView::showControls()
{
    // show controls when selftimer counter is not active
    // and when autofocus key is not being pressed
    if ( !(mSelfTimer && mSelfTimer->isOngoing())  &&
         (!mKeyHandler || !mKeyHandler->isAutofocusKeyPressed()) ) {
        CxuiPrecaptureView::showControls();
    }

}

/*!
* Slot to handle application being sent to background.
*/
void CxuiStillPrecaptureView::handleFocusLost()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mSelfTimer && mSelfTimer->isOngoing()) {
        // If self-timer is running, stop and reset the delay now.
        mSelfTimer->reset();
    }

    // Release camera as we are going to background.
    // If taking image is just ongoing, it will be cancelled by engine.
    releaseCamera();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::handleFocusGained()
{
    CX_DEBUG_ENTER_FUNCTION();
    CxuiPrecaptureView::handleFocusGained();
    CX_DEBUG_EXIT_FUNCTION();
}

/*
    Slot for handling scene mode change
    \param scene QVariantMap containing settings related to the new scene mode
 */
void CxuiStillPrecaptureView::handleSceneChanged(CxeScene &scene)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mEngine->mode() == Cxe::ImageMode) {

        // update toolbar scene mode icon
        QString icon = getSettingItemIcon(CxeSettingIds::IMAGE_SCENE, scene[CxeSettingIds::SCENE_ID]);
        CX_DEBUG((("New scene mode icon: %s"), icon.toAscii().constData()));
        if (mDocumentLoader) {
            QObject *obj = mDocumentLoader->findObject(STILL_PRE_CAPTURE_SCENE_MODE_ACTION);
            CX_DEBUG_ASSERT(obj);
            qobject_cast<HbAction *>(obj)->setIcon(HbIcon(icon));
        }

        // for now, we are only interested in flashmode change
        if (scene.contains(CxeSettingIds::FLASH_MODE)) {
            CX_DEBUG(("updating flash to: %d", scene[CxeSettingIds::FLASH_MODE].value<int>()));
            handleSettingValueChanged(CxeSettingIds::FLASH_MODE, scene[CxeSettingIds::FLASH_MODE]);
        } else {
            // No flash mode specified within the scene.
            // Check from setting model what is it currently.
            int flashMode(Cxe::FlashAuto);
            mEngine->settings().get(CxeSettingIds::FLASH_MODE, flashMode);
            handleSettingValueChanged(CxeSettingIds::FLASH_MODE, QVariant(flashMode));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*
    Slot for handling setting value changes. Notice that changing the scene mode
    does not emit settingValueChanged signal.
    \param key      CxSettingIds key defining the changed setting
    \param newValue QVariant containing the new setting value

    \sa CxuiStillPrecaptureView::handleSceneChanged(CxeScene &scene)
 */
void CxuiStillPrecaptureView::handleSettingValueChanged(const QString& key, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mEngine->mode() == Cxe::ImageMode) {

        // update images left and image quality icons
        if (key == CxeSettingIds::IMAGE_QUALITY) {
            // update the quality indicator on screen
            updateQualityIcon();
            // update images left when quality values are changed
            updateImagesLeftLabel();
        }

        // update toolbar flash icon
        if (mFlashSetting && key == CxeSettingIds::FLASH_MODE) {
            CX_DEBUG((("flash mode: %d"), newValue.toInt()));
            QString icon = getSettingItemIcon(key, newValue);
            CX_DEBUG((("flash mode icon: %s"), icon.toAscii().constData()))
            mFlashSetting->setIcon(HbIcon(icon));
        }

    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
    Close open dialogs.
*/
void CxuiStillPrecaptureView::closeDialogs()
{

    if (mSettingsDialog && mSettingsDialog->isVisible()) {
        mSettingsDialog->close();
    }

    if (mSceneModePopup && mSceneModePopup->isVisible()) {
        mSceneModePopup->close();
    }

    if (mStillSettingsPopup && mStillSettingsPopup->isVisible()) {
        mStillSettingsPopup->close();
    }

    hideSettingsGrid();
}


/*!
*Helper method to create setting dialog
*/
void CxuiStillPrecaptureView::launchSetting()
{
    CX_DEBUG_ENTER_FUNCTION();

    QObject *action = sender();

    if (action) {
        QString settingsKey = action->property(PROPERTY_KEY_SETTING_ID).toString();
        CX_DEBUG(("settingsKey=%s", settingsKey.toAscii().constData()));
        launchSettingsDialog(action);
        // special case to get value changed event to the selftimer class
        if (settingsKey == CxeSettingIds::SELF_TIMER) {
            connect(mSettingsDialogList, SIGNAL(valueSelected(int)),
                    mSelfTimer, SLOT(changeTimeOut(int)));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Update the images left counter
*/
void CxuiStillPrecaptureView::updateImagesLeftLabel()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mImagesLeft && mImagesLeftContainer) {
        int images = mEngine->stillCaptureControl().imagesLeft();

        if (images > CXUI_IMAGES_LEFT_LIMIT) {
            mImagesLeftContainer->hide();
        } else {
            mImagesLeftContainer->show();
        }

        CX_DEBUG(("Images left %d", images));
        mImagesLeft->setPlainText(hbTrId("txt_cam_fullscreen_imagesleft").arg(images));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
