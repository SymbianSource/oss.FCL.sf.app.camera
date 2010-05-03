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

#include <e32keys.h>

#include <QApplication>

#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbtoolbarextension.h>
#include <hbtransparentwindow.h>
#include <hblabel.h>
#include <hbslider.h>
#include <hbmenu.h>
#include <hbdialog.h>
#include <hbnotificationdialog.h>
#include <hbfeedbackplayer.h>
#include <hbfeedbacksettings.h>
#include <hbfeedbacknamespace.h>

#include "cxuivideoprecaptureview.h"
#include "cxeengine.h"
#include "cxeviewfindercontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxenamespace.h"
#include "cxesettings.h"
#include "cxuienums.h"
#include "cxutils.h"
#include "cxuizoomslider.h"
#include "cxuicapturekeyhandler.h"
#include "cxuidocumentloader.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuivideoprecaptureviewTraces.h"
#endif
#include "cxuiserviceprovider.h"


using namespace Cxe;
using namespace CxUiLayout;
using namespace CxUiInternal;

namespace
{
    static const int CXUI_ELAPSED_TIME_TIMEOUT = 1000; // 1 second
    static const int CXUI_RECORD_ANIMATION_DURATION = 3000; // milliseconds

    //!@todo Localization?
    static const char* VIDEO_TIME_FORMAT = "%02d:%02d";
}


CxuiVideoPrecaptureView::CxuiVideoPrecaptureView(QGraphicsItem *parent) :
    CxuiPrecaptureView(parent),
    mElapsedTimer(this),
    mTimeElapsed(0),
    mTimeRemaining(0),
    mElapsedTimeText(NULL),
    mRemainingTimeText(NULL),
    mRecordingIcon(NULL),
    mGoToStillAction(NULL),
    mToolBarIdle(NULL),
    mToolBarRec(NULL),
    mToolBarPaused(NULL),
    mVideoScenePopup(NULL),
    mVideoCaptureControl(NULL),
    mMenu(NULL),
    mCapturePending(false)
{
    CX_DEBUG_IN_FUNCTION();
}

CxuiVideoPrecaptureView::~CxuiVideoPrecaptureView()
{
    CX_DEBUG_IN_FUNCTION();
    mElapsedTimer.stop();
    delete mMenu;
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::construct(HbMainWindow *mainwindow, CxeEngine *engine,
                                        CxuiDocumentLoader *documentLoader,
                                        CxuiCaptureKeyHandler *keyHandler)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxuiPrecaptureView::construct(mainwindow, engine, documentLoader, keyHandler);
    mKeyHandler = keyHandler;

    mVideoCaptureControl = &(engine->videoCaptureControl());

    connect(&mElapsedTimer, SIGNAL(timeout()), this, SLOT(updateTimeLabels()));
    connect(mVideoCaptureControl, SIGNAL(snapshotReady(CxeError::Id, const QPixmap&, const QString&)),
            this, SLOT(handleSnapshot(CxeError::Id)));
    connect(mVideoCaptureControl, SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)),
            this, SLOT(handleVideoStateChanged(CxeVideoCaptureControl::State,CxeError::Id)));
    connect(mVideoCaptureControl, SIGNAL(remainingTimeChanged()),
            this, SLOT(updateTimeLabels()));

    HbAction *quitAction = new HbAction(Hb::QuitNaviAction, this);
    setNavigationAction(quitAction);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(handleQuitClicked()));

    loadDefaultWidgets();
    loadWidgets();
    hideControls();

    CX_DEBUG_EXIT_FUNCTION();

}

void CxuiVideoPrecaptureView::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    // get pointers to ui components from the layout data
    QGraphicsWidget *widget = NULL;
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_VIEWFINDER);
    mViewfinder = qobject_cast<HbTransparentWindow *> (widget);
    CX_DEBUG_ASSERT(mViewfinder);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_QUALITY_ICON);
    mQualityIcon = qobject_cast<HbLabel *> (widget);
    CX_DEBUG_ASSERT(mQualityIcon);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_INDICATOR_CONTAINER_TOP);
    mIndicators = qobject_cast<HbWidget *>(widget);
    CX_DEBUG_ASSERT(mIndicators);
    // Create background graphics for indicator container
    createWidgetBackgroundGraphic(mIndicators, TRANSPARENT_BACKGROUND_GRAPHIC);

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::loadWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    if (mWidgetsLoaded) {
        CX_DEBUG(("Widgets already loaded"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    // get pointers to ui components from the layout data
    QGraphicsWidget *widget = NULL;

    // load the "secondary" widgets
    bool ok = false;

    // load widgets section (creates the widgets)
    mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_WIDGETS_SECTION, &ok);
    Q_ASSERT_X(ok, "camerax ui", "error in xml file parsing");
    if (CxuiServiceProvider::isCameraEmbedded()) {
        mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_EMBEDDED_SECTION, &ok);
    } else {
        mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_STANDALONE_SECTION, &ok);
    }
    Q_ASSERT_X(ok, "camerax ui", "error in xml file parsing");
    // get needed pointers to some of the widgets
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_ZOOM_SLIDER);
    mSlider = qobject_cast<CxuiZoomSlider *> (widget);
    CX_DEBUG_ASSERT(mSlider);

    //Let's add a plus and minus buttons to the slider
    addIncreaseDecreaseButtons(mSlider);
    createWidgetBackgroundGraphic(mSlider, TRANSPARENT_BACKGROUND_GRAPHIC);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_TOOLBAR);
    mToolBarIdle = qobject_cast<HbToolBar *> (widget);
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_TOOLBAR_REC);
    mToolBarRec = qobject_cast<HbToolBar *> (widget);
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_TOOLBAR_PAUSED);
    mToolBarPaused = qobject_cast<HbToolBar *> (widget);

    mToolBar = mToolBarIdle;

    CX_DEBUG_ASSERT(mToolBarIdle);
    CX_DEBUG_ASSERT(mToolBarRec);
    CX_DEBUG_ASSERT(mToolBarPaused);

    hideControls();

    if (CxuiServiceProvider::isCameraEmbedded()) {
        CX_DEBUG(("EMBEDDED: camera in embedded mode"));

        if (!CxuiServiceProvider::instance()->allowQualityChange()) {

            CX_DEBUG(("EMBEDDED: don't allow quality change"));
            HbAction* vq = qobject_cast<HbAction*> (mDocumentLoader->findObject(VIDEO_PRE_CAPTURE_VIDEO_QUALITY_ACTION));
            if (vq) {
                CX_DEBUG(("EMBEDDED: setting image quality setting to disabled"));
                vq->setEnabled(false);
            }
        }
        if (!CxuiServiceProvider::instance()->allowModeSwitching()) {
            CX_DEBUG(("EMBEDDED: don't allow mode switching"));

            HbAction *still = qobject_cast<HbAction*> (mDocumentLoader->findObject(VIDEO_PRE_CAPTURE_GOTO_STILL_ACTION));
            if (still) {
                CX_DEBUG(("EMBEDDED: setting mode switch to disabled"));
                still->setEnabled(false);
            }
        }
        if (!CxuiServiceProvider::instance()->allowCameraSwitching()) {
            CX_DEBUG(("EMBEDDED: don't allow camera switching"));

        }

    }


    // Create background graphic for indicator container
    HbWidget *indicatorContainer;
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_INDICATOR_CONTAINER_BOTTOM);
    indicatorContainer = qobject_cast<HbWidget *>(widget);
    CX_DEBUG_ASSERT(indicatorContainer);
    createWidgetBackgroundGraphic(indicatorContainer, TRANSPARENT_BACKGROUND_GRAPHIC);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_ELAPSED_TIME_LABEL);
    mElapsedTimeText = qobject_cast<HbLabel *> (widget);
    CX_DEBUG_ASSERT(mElapsedTimeText);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_REMAINING_TIME_LABEL);
    mRemainingTimeText = qobject_cast<HbLabel *> (widget);
    CX_DEBUG_ASSERT(mRemainingTimeText);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_RECORDING_ICON);
    mRecordingIcon = qobject_cast<HbLabel *> (widget);
    CX_DEBUG_ASSERT(mRecordingIcon);

    mWidgetsLoaded = true;

    // Initializing recording indicator animation
    mRecordingAnimation = new QPropertyAnimation(mRecordingIcon, "opacity");
    mRecordingAnimation->setStartValue(0.0);
    mRecordingAnimation->setKeyValueAt(0.5, 1.0);
    mRecordingAnimation->setEndValue(0.0);
    mRecordingAnimation->setDuration(CXUI_RECORD_ANIMATION_DURATION);
    mRecordingAnimation->setLoopCount(-1);
    mRecordingAnimation->setEasingCurve(QEasingCurve::InCubic);

    // Initialize the video time counters.
    updateTimeLabels();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Initialize settings grid.
*/
void CxuiVideoPrecaptureView::initializeSettingsGrid()
{
    if(!mSettingsGrid) {
        HbAction* action(NULL);

        mSettingsGrid = new HbToolBarExtension;

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_exposure.svg"), hbTrId("txt_cam_button_exposure_compensation"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::EV_COMPENSATION_VALUE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_small_rgb.svg"), hbTrId("txt_cam_button_color_tone"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::COLOR_TONE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_white_balance.svg"), hbTrId("txt_cam_button_white_balance"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::WHITE_BALANCE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        connect(mKeyHandler, SIGNAL(autofocusKeyPressed()), mSettingsGrid, SLOT(close()));
    }
}

void CxuiVideoPrecaptureView::handleSnapshot(CxeError::Id /*error*/)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::record()
{
    CX_DEBUG_ENTER_FUNCTION();

    int time(0);
    mVideoCaptureControl->remainingTime(time);

    if (time) {
        mMenu = takeMenu();
        hideControls();
        mVideoCaptureControl->record();
    } else {
        launchDiskFullNotification();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::pause()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording) {
        if (mRecordingAnimation && mRecordingIcon) {
            mVideoCaptureControl->pause();
            mRecordingAnimation->stop();
            }

        // force update of toolbar
        showToolbar();
    } else if (state == CxeVideoCaptureControl::Paused) {
        mVideoCaptureControl->record();
        //mRecordingAnimation->start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::stop()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (isPostcaptureOn()) {
        CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
        if (state == CxeVideoCaptureControl::Recording ||
            state == CxeVideoCaptureControl::Paused) {
            mVideoCaptureControl->stop();
            // Continue in handleVideoStateChanged().
        }
    } else {
        // no postcapture
        stopAndPrepareNewVideo();
    }


    CX_DEBUG_EXIT_FUNCTION();
}

// CxuiVideoPrecaptureView::stopAndPrepareNewVideo()
// A version of stop that doesn't go to post-capture. When
// not going to post-capture, we need to prepare new video
void CxuiVideoPrecaptureView::stopAndPrepareNewVideo()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoCaptureControl->stop();
    mElapsedTimer.stop();
    hideControls();

    if (mMenu) {
        setMenu(mMenu);
        mMenu = NULL;
    }

    // prepare new video when old one is ready
    connect(mVideoCaptureControl, SIGNAL(videoComposed(CxeError::Id, const QString&)),
            this, SLOT(prepareNewVideo(CxeError::Id)));

    CX_DEBUG_EXIT_FUNCTION();
}


// CxuiPrecaptureView::showToolbar()
// Shows toolbar. Calls the base class implementation if not recording
// since toolbar is not shown during recording
void CxuiVideoPrecaptureView::showToolbar()
{
    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording) {
        if (mToolBar != mToolBarRec) {
            mToolBar->hide();
            mToolBar = mToolBarRec;
        }
    } else if (state ==CxeVideoCaptureControl::Ready) {
        if (mToolBar != mToolBarIdle) {
            mToolBar->hide();
            mToolBar = mToolBarIdle;
        }
    } else if (state == CxeVideoCaptureControl::Paused) {
        if (mToolBar != mToolBarPaused) {
            mToolBar->hide();
            mToolBar = mToolBarPaused;
        }
    }

    CxuiPrecaptureView::showToolbar();
}

void CxuiVideoPrecaptureView::disableFeedback()
{
    CX_DEBUG_ENTER_FUNCTION();

    HbFeedbackPlayer* feedback = HbFeedbackPlayer::instance();
    if (feedback) {
        feedback->settings().disableFeedback();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::enableFeedback()
{
    CX_DEBUG_ENTER_FUNCTION();

    HbFeedbackPlayer* feedback = HbFeedbackPlayer::instance();
    if (feedback) {
        feedback->settings().disableFeedback();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiVideoPrecaptureView::goToStill()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, DUP1_CXUIVIDEOPRECAPTUREVIEW_GOTOSTILL, "msg: e_CX_GO_TO_STILL_MODE 1" );

    hideControls();
    mEngine->initMode(ImageMode);
    emit changeToPrecaptureView();

    OstTrace0( camerax_performance, DUP2_CXUIVIDEOPRECAPTUREVIEW_GOTOSTILL, "msg: e_CX_GO_TO_STILL_MODE 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update the remaining and elapsed time labels.
*/
void CxuiVideoPrecaptureView::updateTimeLabels()
{
    CX_DEBUG_IN_FUNCTION();

    if (!mRemainingTimeText || !mElapsedTimeText) {
        // Section not loaded yet. Skip update until created.
        CX_DEBUG(("CxuiVideoPrecaptureView: video time labels not loaded yet!"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    switch (state) {
        case CxeVideoCaptureControl::Ready:
            getRemainingTime();
            // Not recording => elapsed time is zero
            mTimeElapsed = 0;
            break;

        case CxeVideoCaptureControl::Recording:
        case CxeVideoCaptureControl::Paused:
            getRemainingTime();
            getElapsedTime();
            break;

        case CxeVideoCaptureControl::Idle:
        case CxeVideoCaptureControl::Initialized:
        case CxeVideoCaptureControl::Preparing:
        default:
            // Minimize processing during initialization phase.
            // Calculating remaining time involves checking disk space, avoiding that.
            // which
            mTimeRemaining = 0;
            mTimeElapsed = 0;
            break;
    }

    setVideoTime(mRemainingTimeText, mTimeRemaining);
    setVideoTime(mElapsedTimeText, mTimeElapsed);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method for formatting video time to requested label.
* @param label Text label to show the time.
* @param time Time in seconds to be formatted to the label text.
*/
void CxuiVideoPrecaptureView::setVideoTime(HbLabel* label, int time)
{
    // Convert time (seconds) into mm:ss
    // HbExtendedLocale wraps minutes at 60 so we can't use that.
    // We need to show times over 1 hour, e.g. "90:00".
    QString timeString;
    timeString.sprintf(VIDEO_TIME_FORMAT, time/60, time%60);
    label->setPlainText(timeString);
}

bool CxuiVideoPrecaptureView::getElapsedTime()
{
    CX_DEBUG_ENTER_FUNCTION();

    TBool status = mVideoCaptureControl->elapsedTime(mTimeElapsed);
    CX_DEBUG(("Elapsed time: %d", mTimeElapsed));
    CX_DEBUG(("status: %d", status));

    CX_DEBUG_EXIT_FUNCTION();
    return status;
}

void CxuiVideoPrecaptureView::getRemainingTime()
{
    CX_DEBUG_ENTER_FUNCTION();

    mVideoCaptureControl->remainingTime(mTimeRemaining);
    CX_DEBUG(("getRemainingTime time: %d", mTimeRemaining));

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::showEvent(QShowEvent *event)
{
    CxuiPrecaptureView::showEvent(event);

    updateQualityIcon();

    if (event->type() == QEvent::Show) {
        event->accept();
    }
}

/*!
* Slot to handle video capture control state change.
* Update visible items and stop / start timers.
*/
void CxuiVideoPrecaptureView::handleVideoStateChanged(CxeVideoCaptureControl::State newState,
        CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(error);

    updateTimeLabels();

    switch (newState) {
    case CxeVideoCaptureControl::Ready:
        if (mDocumentLoader){
            mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_IDLE);
        }
        if (mCapturePending) {
            mCapturePending = false;
            record();
        }
        break;
    case CxeVideoCaptureControl::Recording:
        if (mDocumentLoader){
            mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_RECORDING);
        }
        mElapsedTimer.start(CXUI_ELAPSED_TIME_TIMEOUT);
        disableFeedback();
        break;
    case CxeVideoCaptureControl::Paused:
        mElapsedTimer.stop();

        if (mDocumentLoader){
            mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_PAUSED);
        }

        enableFeedback();
        break;
    case CxeVideoCaptureControl::Stopping:
        if (mDocumentLoader){
            mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_PAUSED);
        }

        enableFeedback();

        if (isPostcaptureOn()) {
            mElapsedTimer.stop();
            hideControls();

            if (mRecordingAnimation && mRecordingIcon) {
                mRecordingAnimation->stop();
                mRecordingIcon->setOpacity(0);
            }
            if (mMenu) {
                setMenu(mMenu);
                mMenu = NULL;
            }

            emit changeToPostcaptureView();
        }
        break;
    default:
        // in any other state, just hide the controls
        setRecordingItemsVisibility(false);
        break;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::updateOrientation(Qt::Orientation orientation)
{
    CX_DEBUG_ENTER_FUNCTION();

    hideControls();
    mMainWindow->setOrientation(orientation);

    if (mToolBar) {
        if (orientation == Qt::Horizontal) {
            mToolBar->setOrientation(Qt::Vertical);
        } else {
            mToolBar->setOrientation(Qt::Horizontal);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();

    switch (state) {
        case CxeVideoCaptureControl::Ready:
        case CxeVideoCaptureControl::Paused:
            record();
            break;
        case CxeVideoCaptureControl::Recording:
            stop();
            break;
        case CxeVideoCaptureControl::Idle:
        case CxeVideoCaptureControl::Initialized:
        case CxeVideoCaptureControl::Preparing:
            mCapturePending = true;
            break;

    }
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::prepareNewVideo(CxeError::Id error)
{
    if (!error) {
        mEngine->initMode(Cxe::VideoMode);
        disconnect(mVideoCaptureControl, SIGNAL(videoComposed(CxeError::Id, const QString&)),
                   this, SLOT(prepareNewVideo(CxeError::Id)));
    } else {
        emit reportError(error);
    }

}

/**
* Toggle video light on / off.
*/
void CxuiVideoPrecaptureView::toggleLight()
{
    launchNotSupportedNotification();
}

void CxuiVideoPrecaptureView::handleQuitClicked()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording){
        // Disable going to post-capture when video capture control goes to stopping state.
        disconnect(mVideoCaptureControl, SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)),
                   this, SLOT(handleVideoStateChanged(CxeVideoCaptureControl::State,CxeError::Id)));
        mVideoCaptureControl->stop();
        mElapsedTimer.stop();
    }

    QCoreApplication::exit();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::handleFocusLost()
{
    CX_DEBUG_IN_FUNCTION();
    // Release camera. Stopping possibly ongoing recording is handled by engine.
    releaseCamera();
}

void CxuiVideoPrecaptureView::handleBatteryEmpty()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording){
        stop(); // delete recording icon
    }
    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiVideoPrecaptureView::launchVideoScenePopup()
{
    CX_DEBUG_ENTER_FUNCTION();
    hideControls();
    emit showScenesView();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Launches show-postcapture setting pop-up
*/
void CxuiVideoPrecaptureView::launchSetting()
{
    CX_DEBUG_ENTER_FUNCTION();

    QObject *action = sender();

    if (action) {
        QString settingsKey = action->property(PROPERTY_KEY_SETTING_ID).toString();
        CX_DEBUG(("settingsKey=%s", settingsKey.toAscii().constData()));
        launchSettingsDialog(action);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle change in setting value. Update UI to reflect new value.
*/
void CxuiVideoPrecaptureView::handleSettingValueChanged(const QString& key, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(newValue);
    // Ignored if not in video mode.
    if (mEngine->mode() == Cxe::VideoMode) {
        if (key == CxeSettingIds::VIDEO_QUALITY) {
            // update the quality indicator on screen
            updateQualityIcon();

            // update video remaining time counter when video quality is changed
            updateTimeLabels();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Sets the visibility of recording icon and elapsed time text.
    \param visible True if widgets are to be shown, false if not.
*/
void CxuiVideoPrecaptureView::setRecordingItemsVisibility(bool visible) {

    if (mRecordingIcon) {
        mRecordingIcon->setVisible(visible);
        mRecordingIcon->setOpacity(1.0f);
    }

    if (mElapsedTimeText) {
        mElapsedTimeText->setVisible(visible);
    }
}



//end of file
