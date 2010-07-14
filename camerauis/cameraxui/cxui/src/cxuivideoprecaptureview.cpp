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
#include <QGraphicsLayout>

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
#include <hbfeedbacksettings.h>
#include <hbfeedbacknamespace.h>
#include <hbactivitymanager.h>

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
#include "cxuiserviceprovider.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuivideoprecaptureviewTraces.h"
#endif


using namespace Cxe;
using namespace CxUiLayout;
using namespace CxUiInternal;

namespace
{
    static const int CXUI_ELAPSED_TIME_TIMEOUT = 1000; // 1 second
    static const int CXUI_RECORD_ANIMATION_DURATION = 3000; // milliseconds
    static const int CXUI_PAUSE_TIMEOUT = 60*1000;   // 60 seconds

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
    mToolbarIdle(NULL),
    mToolbarRec(NULL),
    mToolbarPaused(NULL),
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

/*!
 * Construct-method handles initialisation tasks for this class.
 * @param mainwindow
 * @param engine
 * @param documentLoader
 * @param keyHandler
 */
void CxuiVideoPrecaptureView::construct(HbMainWindow *mainwindow, CxeEngine *engine,
                                        CxuiDocumentLoader *documentLoader,
                                        CxuiCaptureKeyHandler *keyHandler,
                                        HbActivityManager *activityManager)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxuiPrecaptureView::construct(mainwindow, engine, documentLoader, keyHandler, activityManager);
    mCaptureKeyHandler = keyHandler;

    mVideoCaptureControl = &(engine->videoCaptureControl());

    connect(&mElapsedTimer, SIGNAL(timeout()), this, SLOT(updateTimeLabels()));
    connect(mVideoCaptureControl, SIGNAL(snapshotReady(CxeError::Id, const QImage&, const QString&)),
            this, SLOT(handleSnapshot(CxeError::Id)));
    connect(mVideoCaptureControl, SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)),
            this, SLOT(handleVideoStateChanged(CxeVideoCaptureControl::State,CxeError::Id)));
    connect(&(mEngine->settings()), SIGNAL(sceneChanged(CxeScene&)),
            this, SLOT(handleSceneChanged(CxeScene&)));
    connect(mVideoCaptureControl, SIGNAL(remainingTimeChanged()),
            this, SLOT(updateTimeLabels()));

    mPauseTimer.setSingleShot(true);
    connect(&mPauseTimer, SIGNAL(timeout()), this, SLOT(stop()));
    mPauseTimer.setInterval(CXUI_PAUSE_TIMEOUT);

    HbAction *quitAction = new HbAction(Hb::QuitNaviAction, this);
    setNavigationAction(quitAction);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(handleQuitClicked()));

    loadDefaultWidgets();
    loadWidgets();
    hideControls();

    CX_DEBUG_EXIT_FUNCTION();

}

/*!
 * Loads widgets that are needed right from the start.
 */
void CxuiVideoPrecaptureView::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mDocumentLoader);

    // get pointers to ui components from the layout data
    QGraphicsWidget *widget = NULL;
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_VIEWFINDER);
    mViewfinder = qobject_cast<HbTransparentWindow *> (widget);
    CX_ASSERT_ALWAYS(mViewfinder);

    reloadIndicatorWidgets();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Loads default indicators from docml and modifies the visibility 
 * according to current settings.
 */
void CxuiVideoPrecaptureView::reloadIndicatorWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mDocumentLoader);

    bool ok = false;
    mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_INDICATORS_SECTION, &ok);
    CX_ASSERT_ALWAYS(ok);

    QGraphicsWidget *widget = NULL;
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_QUALITY_ICON);
    mQualityIcon = qobject_cast<HbLabel *> (widget);
    CX_ASSERT_ALWAYS(mQualityIcon);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_VIDEOAUDIOMUTE_INDICATOR_ICON);
    HbLabel *videoaudiomuteIndicatorIcon = qobject_cast<HbLabel *>(widget);
    CX_ASSERT_ALWAYS(videoaudiomuteIndicatorIcon);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_STABILITY_INDICATOR_ICON);
    HbLabel *videoStabilityIndicatorIcon = qobject_cast<HbLabel *>(widget);
    CX_ASSERT_ALWAYS(videoStabilityIndicatorIcon);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_INDICATOR_CONTAINER_TOP);
    mIndicators = qobject_cast<HbWidget *>(widget);
    CX_ASSERT_ALWAYS(mIndicators);

    QGraphicsLayout *layout = mIndicators->layout();
    QGraphicsLayoutItem *graphicsLayoutItem = NULL;
    QGraphicsItem *graphicsItem = NULL;
    QString key = "";
    int currentSettingValue = -1;
    bool isSettingOff = false;
    // Go through the items in the layout to check whether they should be
    // shown or not in the indicator pane. Start from the last towards
    // the first, so that removing items from between works correctly.
    for (int i = layout->count() - 1; i >= 0; i--) {
        graphicsLayoutItem = layout->itemAt(i);
        isSettingOff = false;
        if (graphicsLayoutItem) {
            graphicsItem = graphicsLayoutItem->graphicsItem();
            currentSettingValue = -1;
            if (graphicsItem == videoaudiomuteIndicatorIcon) {
                key = CxeSettingIds::VIDEO_MUTE_SETTING;
                mEngine->settings().get(key, currentSettingValue);
                // video mute implementation does not use 
                // enum for on/off values but instead 
                // 0 for off and 1 for on.
                if (currentSettingValue == 0) {
                    isSettingOff = true;
                }
            } else if (graphicsItem == videoStabilityIndicatorIcon) {
                // remove video stability indicator.
                isSettingOff = true;
            }
            if (isSettingOff) {
                layout->removeAt(i);
            }
        }
    }

    // Create background graphics for indicator container
    createWidgetBackgroundGraphic(mIndicators, TRANSPARENT_BACKGROUND_GRAPHIC);

    mIndicators->setVisible(true);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Loads widgets that are not part of the default section in layouts xml.
 * Widgets are created at the time they are first loaded.
 */
void CxuiVideoPrecaptureView::loadWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mDocumentLoader);

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
    CX_ASSERT_ALWAYS(ok);
    if (CxuiServiceProvider::isCameraEmbedded()) {
        mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_EMBEDDED_SECTION, &ok);
    } else {
        mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_STANDALONE_SECTION, &ok);
    }
    CX_ASSERT_ALWAYS(ok);
    // get needed pointers to some of the widgets
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_ZOOM_SLIDER);
    mSlider = qobject_cast<CxuiZoomSlider *> (widget);
    CX_ASSERT_ALWAYS(mSlider);

    //Let's add a plus and minus buttons to the slider
    mSlider->addZoomButtons();
    createWidgetBackgroundGraphic(mSlider, TRANSPARENT_BACKGROUND_GRAPHIC);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_TOOLBAR);
    mToolbarIdle = qobject_cast<HbToolBar *> (widget);
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_TOOLBAR_REC);
    mToolbarRec = qobject_cast<HbToolBar *> (widget);
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_TOOLBAR_PAUSED);
    mToolbarPaused = qobject_cast<HbToolBar *> (widget);

    mToolbar = mToolbarIdle;

    CX_ASSERT_ALWAYS(mToolbarIdle);
    CX_ASSERT_ALWAYS(mToolbarRec);
    CX_ASSERT_ALWAYS(mToolbarPaused);

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
    CX_ASSERT_ALWAYS(indicatorContainer);
    createWidgetBackgroundGraphic(indicatorContainer, TRANSPARENT_BACKGROUND_GRAPHIC);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_ELAPSED_TIME_LABEL);
    mElapsedTimeText = qobject_cast<HbLabel *> (widget);
    CX_ASSERT_ALWAYS(mElapsedTimeText);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_REMAINING_TIME_LABEL);
    mRemainingTimeText = qobject_cast<HbLabel *> (widget);
    CX_ASSERT_ALWAYS(mRemainingTimeText);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_RECORDING_ICON);
    mRecordingIcon = qobject_cast<HbLabel *> (widget);
    CX_ASSERT_ALWAYS(mRecordingIcon);

    mWidgetsLoaded = true;

    // Initializing recording indicator animation
    mRecordingAnimation = new QPropertyAnimation(mRecordingIcon, "opacity");
    mRecordingAnimation->setStartValue(0.2);
    mRecordingAnimation->setKeyValueAt(0.5, 1.0);
    mRecordingAnimation->setEndValue(0.2);
    mRecordingAnimation->setDuration(CXUI_RECORD_ANIMATION_DURATION);
    mRecordingAnimation->setLoopCount(-1);
    mRecordingAnimation->setEasingCurve(QEasingCurve::OutInQuad);


    // Update toolbar scene mode icon.
    QString sceneId;
    if (mEngine->settings().get(CxeSettingIds::SCENE_ID, sceneId) == CxeError::None) {
        updateSceneIcon(sceneId);
    }

    // Initialize the video time counters.
    updateTimeLabels();

    // View is ready. Needed for startup performance automated testing.
    emit viewReady();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Restore view state from activity.
 */
void CxuiVideoPrecaptureView::restoreActivity(const QString &activityId, const QVariant &data)
{
    Q_UNUSED(activityId);
    Q_UNUSED(data);

    CX_DEBUG_ENTER_FUNCTION();
    // no need to restore any state
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Save view state to activity.
 */
void CxuiVideoPrecaptureView::saveActivity()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVariantMap data;
    QVariantHash params;
    //@todo: add pre-capture icon as screenshot
    mActivityManager->removeActivity(CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY);
    mActivityManager->addActivity(CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY, data, params);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Clear activity from activity manager.
 */
void CxuiVideoPrecaptureView::clearActivity()
{
    CX_DEBUG_ENTER_FUNCTION();
    mActivityManager->removeActivity(CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY);
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

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_exposure"), hbTrId("txt_cam_button_exposure_compensation"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::EV_COMPENSATION_VALUE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_small_rgb"), hbTrId("txt_cam_button_color_tone"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::COLOR_TONE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_white_balance"), hbTrId("txt_cam_button_white_balance"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::WHITE_BALANCE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        connect(mCaptureKeyHandler, SIGNAL(autofocusKeyPressed()), mSettingsGrid, SLOT(close()));
    }
}

/**
* Get if postcapture view should be shown or not.
* Postcapture view may be shown for a predefined time or
* until user dismisses it, or it may be completely disabled.
*/
bool CxuiVideoPrecaptureView::isPostcaptureOn() const
{
    CX_DEBUG_ENTER_FUNCTION();
    if (CxuiServiceProvider::isCameraEmbedded()) {
        // always show post capture in embedded mode
        CX_DEBUG_EXIT_FUNCTION();
        return true;
    }

    // Read the value from settings. Ignoring reading error.
    // On error (missing settings) default to "postcapture on".
    int showPostcapture(-1);
    if(mEngine) {
        mEngine->settings().get(CxeSettingIds::VIDEO_SHOWCAPTURED, showPostcapture);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return showPostcapture != 0; // 0 == no postcapture
}

/*!
* Update the scene mode icon.
* @param sceneId The new scene id.
*/
void CxuiVideoPrecaptureView::updateSceneIcon(const QString& sceneId)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mEngine->mode() == Cxe::VideoMode) {
        CX_DEBUG(("CxuiPrecaptureView - scene: %s", sceneId.toAscii().constData()));

        // No need to update icon, if widgets are not even loaded yet.
        // We'll update the icon once the widgets are loaded.
        if (mWidgetsLoaded) {
            QString iconObjectName = VIDEO_PRE_CAPTURE_SCENE_MODE_ACTION;
            QString icon = getSettingItemIcon(CxeSettingIds::VIDEO_SCENE, sceneId);

            CX_DEBUG(("CxuiVideoPrecaptureView - icon: %s", icon.toAscii().constData()));

            if (mDocumentLoader) {
                QObject *obj = mDocumentLoader->findObject(iconObjectName);
                CX_ASSERT_ALWAYS(obj);
                qobject_cast<HbAction *>(obj)->setIcon(HbIcon(icon));
            }
        } else {
            CX_DEBUG(("CxuiVideoPrecaptureView - widgets not loaded yet, ignored!"));
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Update the quality indicator
*/
void CxuiVideoPrecaptureView::updateQualityIcon()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mQualityIcon && mEngine) {
        QString icon = "";
        int currentValue = -1;

        mEngine->settings().get(CxeSettingIds::VIDEO_QUALITY, currentValue);
        icon = getSettingItemIcon(CxeSettingIds::VIDEO_QUALITY, currentValue);

        mQualityIcon->setIcon(HbIcon(icon));
    }

    CX_DEBUG_EXIT_FUNCTION();
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
        mVideoCaptureControl->record();
    } else {
        emit errorEncountered(CxeError::DiskFull);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::pause()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording) {
        mVideoCaptureControl->pause();
    } else if (state == CxeVideoCaptureControl::Paused) {
        mVideoCaptureControl->record();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording ||
        state == CxeVideoCaptureControl::Paused) {
        mVideoCaptureControl->stop();
        // Continue in handleVideoStateChanged().
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Allow showing UI controls?
*/
bool CxuiVideoPrecaptureView::allowShowControls() const
{
    bool show(false);
    if (mEngine) {
        CxeVideoCaptureControl::State state(mEngine->videoCaptureControl().state());

        show = (mEngine->isEngineReady()
             || state == CxeVideoCaptureControl::Recording
             || state == CxeVideoCaptureControl::Paused);
    }
    return show;
}

/*!
 * Play feedback when touching view outside of any widget?
 * If video is paused feedback is off.  Otherwise on.
 */
bool CxuiVideoPrecaptureView::isFeedbackEnabled() const
{
    CxeVideoCaptureControl::State state(mEngine->videoCaptureControl().state());
    if (state == CxeVideoCaptureControl::Paused) {
        return false;
    } else {
        return true;
    }
}

// CxuiPrecaptureView::showToolbar()
// Shows toolbar. Calls the base class implementation if not recording
// since toolbar is not shown during recording
void CxuiVideoPrecaptureView::showToolbar()
{
    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording) {
        if (mToolbar != mToolbarRec) {
            mToolbar->hide();
            mToolbar = mToolbarRec;
        }
    } else if (state ==CxeVideoCaptureControl::Ready) {
        if (mToolbar != mToolbarIdle) {
            mToolbar->hide();
            mToolbar = mToolbarIdle;
        }
    } else if (state == CxeVideoCaptureControl::Paused) {
        if (mToolbar != mToolbarPaused) {
            mToolbar->hide();
            mToolbar = mToolbarPaused;
        }
    }

    CxuiPrecaptureView::showToolbar();
}

void CxuiVideoPrecaptureView::disableFeedback()
{
    CX_DEBUG_ENTER_FUNCTION();

    HbFeedbackSettings settings;
    settings.disableFeedback();


    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::enableFeedback()
{
    CX_DEBUG_ENTER_FUNCTION();

    HbFeedbackSettings settings;
    settings.enableFeedback();


    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiVideoPrecaptureView::goToStill()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUIVIDEOPRECAPTUREVIEW_GOTOSTILL, "msg: e_CX_GO_TO_STILL_MODE 1" );

    hideControls();
    mEngine->initMode(ImageMode);
    emit changeToPrecaptureView();

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
  Overridded version of hideControls() that doesn't hide the controls when video recording
  is paused.
 */
void CxuiVideoPrecaptureView::hideControls()
{
    if (mVideoCaptureControl && mVideoCaptureControl->state() == CxeVideoCaptureControl::Paused) {
        // never hide controls in paused state
        return;
    }

    CxuiPrecaptureView::hideControls();

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

    bool status = mVideoCaptureControl->elapsedTime(mTimeElapsed);
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

    mPauseTimer.stop();

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
        hideControls();
        emit stopStandbyTimer();
        if (mDocumentLoader){
            mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_RECORDING);
        }
        mElapsedTimer.start(CXUI_ELAPSED_TIME_TIMEOUT);
        disableFeedback();

        // commented out the activation of the recording animation because
        // it is causing issues when using SW rendering
        //if (mRecordingAnimation && mRecordingIcon) {
        //  mRecordingAnimation->start();
        //}

        break;
    case CxeVideoCaptureControl::Paused:
        mElapsedTimer.stop();
        if (mDocumentLoader){
            mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_PAUSED);
        }
        showControls();
        enableFeedback();
        mPauseTimer.start();
        break;
    case CxeVideoCaptureControl::Stopping:
        if (mDocumentLoader){
            mDocumentLoader->load(VIDEO_1ST_XML, VIDEO_PRE_CAPTURE_PAUSED);
        }

        // commented out the stopping of the recording animation since the activation
        // is commented out also couple lines earlier
        //if (mRecordingAnimation && mRecordingIcon) {
        //    mRecordingAnimation->stop();
        //}

        enableFeedback();
        emit startStandbyTimer();
        mElapsedTimer.stop();
        hideControls();

        if (mMenu) {
            setMenu(mMenu);
            mMenu = NULL;
        }

        if (isPostcaptureOn()) {
            emit changeToPostcaptureView();
        } else {
            // post capture off, we need prepare new video
            // do the prepare when the previous video is ready
            connect(mVideoCaptureControl, SIGNAL(videoComposed(CxeError::Id, const QString&)),
                    this, SLOT(prepareNewVideo(CxeError::Id)));
        }
        break;
    case CxeVideoCaptureControl::PlayingStartSound:
        // don't change anything
        break;
    default:
        // in any other state, just hide the controls
        setRecordingItemsVisibility(false);
        break;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiVideoPrecaptureView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();

    switch (state) {
        case CxeVideoCaptureControl::Ready:
            record();
            break;
        case CxeVideoCaptureControl::Recording:
        case CxeVideoCaptureControl::Paused:
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
        emit errorEncountered(error);
    }

}

void CxuiVideoPrecaptureView::handleQuitClicked()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeVideoCaptureControl::State state = mVideoCaptureControl->state();
    if (state == CxeVideoCaptureControl::Recording ||
        state == CxeVideoCaptureControl::Paused) {
        // Disable going to post-capture when video capture control goes to stopping state.
        disconnect(mVideoCaptureControl, SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)),
                   this, SLOT(handleVideoStateChanged(CxeVideoCaptureControl::State,CxeError::Id)));
        mVideoCaptureControl->stop();
        mElapsedTimer.stop();
    }

    QCoreApplication::exit();

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
        } else if (key == CxeSettingIds::GEOTAGGING) {
            reloadIndicatorWidgets();
        } else if (key == CxeSettingIds::VIDEO_MUTE_SETTING) {
            reloadIndicatorWidgets();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle scene mode change.
* @param scene The new active scene mode.
*/
void CxuiVideoPrecaptureView::handleSceneChanged(CxeScene &scene)
{
    CX_DEBUG_ENTER_FUNCTION();
    // Ignore if not in video mode.
    if (mEngine->mode() == Cxe::VideoMode) {
        // Update toolbar scene mode icon.
        updateSceneIcon(scene[CxeSettingIds::SCENE_ID].toString());
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

/*!
 * Overridden eventFilter() to restart the pause timer.
 */
bool CxuiVideoPrecaptureView::eventFilter(QObject *object, QEvent *event)
{

    if (mVideoCaptureControl && mVideoCaptureControl->state() == CxeVideoCaptureControl::Paused) {
        // restart the timer if the screen is touched and we are in paused state
        switch (event->type())
        {
        case QEvent::GraphicsSceneMouseRelease:
            mPauseTimer.start();
            break;
        case QEvent::GraphicsSceneMousePress:
            mPauseTimer.stop();
            break;
        default:
            break;
        }
    }
    return CxuiPrecaptureView::eventFilter(object, event);
}

//end of file

