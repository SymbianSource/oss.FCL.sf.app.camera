/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QGraphicsItem>
#include <QProcess>
#include <QGraphicsSceneEvent>
#include <hbtoolbar.h>
#include <hbnotificationdialog.h>
#include <hbframeitem.h>
#include "cxuiview.h"
#include "cxutils.h"
#include "cxeengine.h"
#include "cxuidocumentloader.h"
#include "cxuicapturekeyhandler.h"
#include "cxecameradevicecontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxuizoomslider.h"
#include "cxezoomcontrol.h"

// CONSTANTS
const QString PhotosAppExe = "photos.exe";
const QString VideosAppExe = "videoplayer.exe";

/*!
* Constructor
*/
CxuiView::CxuiView(QGraphicsItem *parent) :
        HbView(parent),
        mMainWindow(NULL),
        mEngine(NULL),
        mDocumentLoader(NULL),
        mSlider(NULL),
        mToolbar(NULL),
        mIndicators(NULL),
        mHideControlsTimeout(this),
        mControlsFeedback(HbFeedback::BasicItem)
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Destructor
*/
CxuiView::~CxuiView()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* CxuiView::construct
*/
void CxuiView::construct(HbMainWindow *mainWindow, CxeEngine *engine,
                         CxuiDocumentLoader *documentLoader,
                         CxuiCaptureKeyHandler * keyHandler,
                         HbActivityManager *activityManager)
{
    CX_DEBUG_ENTER_FUNCTION();

    //Minimum requirements to construct the view
    CX_ASSERT_ALWAYS(mainWindow);
    CX_ASSERT_ALWAYS(engine);
    CX_ASSERT_ALWAYS(documentLoader);
    CX_ASSERT_ALWAYS(activityManager);

    mMainWindow = mainWindow;
    mDocumentLoader = documentLoader;
    mCaptureKeyHandler = keyHandler;
    mEngine = engine;
    mActivityManager = activityManager;

    // adjust the timer, and connect it to correct slot
    connect(&mHideControlsTimeout, SIGNAL(timeout()), this, SLOT(hideControls()));
    mHideControlsTimeout.setSingleShot(true);
    mHideControlsTimeout.setInterval(CXUI_HIDE_CONTROLS_TIMEOUT);
    
    mControlsFeedback.setOwningWindow(mMainWindow);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Is standby mode supported / needed by this view.
* Default implementation returns false.
* Inherited classes need to re-implement this if standby mode is needed.
* @return True if standby mode is supported, false otherwise.
*/
bool CxuiView::isStandbyModeSupported() const
{
    return false;
}


/*!
* CxuiView::updateOrientation
*/
void CxuiView::updateOrientation(Qt::Orientation orientation)
{
    CX_DEBUG_ENTER_FUNCTION();

    hideControls();
    mMainWindow->setOrientation(orientation);

    if (mToolbar) {
        if (orientation == Qt::Horizontal) {
            mToolbar->setOrientation(Qt::Vertical);
        } else {
            mToolbar->setOrientation(Qt::Horizontal);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
 * Restore view state from activity. Default implementation does nothing.
 */
void CxuiView::restoreActivity(const QString &activityId, const QVariant &data)
{
    Q_UNUSED(activityId);
    Q_UNUSED(data);
}

/*!
 * Save view state to activity. Default implementation does nothing.
 */
void CxuiView::saveActivity()
{

}

/*!
 * Clear activity from activity manager. Default implementation does nothing.
 */
void CxuiView::clearActivity()
{

}

/*!
* Allow showing UI controls?
* Default behaviour is that controls can be shown at any time.
*/
bool CxuiView::allowShowControls() const
{
    return true;
}

/*!
 * Play feedback when touching view outside of any widget?
 * Default behaviour is that feedback is always played.
 */
bool CxuiView::isFeedbackEnabled() const
{
    return true;
}

/*!
* Toggle visibility of UI controls.
*/
void CxuiView::toggleControls()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mControlsVisible) {
        hideControls();
    } else {
        showControls();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for entering standby mode.
* By default, release camera.
*/
void CxuiView::enterStandby()
{
    CX_DEBUG_IN_FUNCTION();
    releaseCamera();
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Slot for exiting standby mode.
* By default, no action needed.
*/
void CxuiView::exitStandby()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* CxuiView::launchNotSupportedNotification
* Show "not supported" notification.
*/
void CxuiView::launchNotSupportedNotification()
{
    CX_DEBUG_ENTER_FUNCTION();
    HbNotificationDialog::launchDialog("Notification", "Not supported yet");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * CxuiView::launchScenesView
 */
void CxuiView::launchScenesView()
{
    CX_DEBUG_ENTER_FUNCTION();
    hideControls();
    emit showScenesView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * CxuiView::launchPhotosApp
 */
void CxuiView::launchPhotosApp()
{
    // Release camera device in order to free resources for Photos application
    releaseCamera();
    QProcess::startDetached(PhotosAppExe);
}

/*!
 * CxuiView::launchVideosApp
 * Launching Videos application as a separate process
 */
void CxuiView::launchVideosApp()
{
    // Release camera device in order to free resources for Videos application
    releaseCamera();
    QProcess::startDetached(VideosAppExe);
}

/*!
 * Releasing camera hw
 */
void CxuiView::releaseCamera()
{
    CX_DEBUG_ENTER_FUNCTION();
    mEngine->cameraDeviceControl().release();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * CxuiView::hideControls
 */
void CxuiView::hideControls()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.stop();
    }

    // Hide title bar and status bar.
    //!@todo: View flags property is missing from HbView, so can't set these in DocML.
    HbView::HbViewFlags flags(HbView::ViewTitleBarTransparent
                            | HbView::ViewTitleBarFloating
                            | HbView::ViewTitleBarHidden
                            | HbView::ViewStatusBarTransparent
                            | HbView::ViewStatusBarFloating
                            | HbView::ViewStatusBarHidden);
    setViewFlags(flags);
    //!@todo: Once the flags are defined in DocML, we can just use these convenience functions
    //        to hide the title bar and status bar here.
    //setTitleBarVisible(false);
    //setStatusBarVisible(false);

    hideZoom();

    hideToolbar();

    // show indicators when controls are hidden
    showIndicators();

    mControlsVisible = false;

    // give the keyboard focus back to the view
    // for the view to receive key events
    setFocus();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * CxuiView::showControls
 */
void CxuiView::showControls()
{
    if (allowShowControls()) {
        // show toolbar
        showToolbar();

        // show zoom
        showZoom();

        // show title bar and status bar
        setTitleBarVisible(true);
        setStatusBarVisible(true);

        // hide indicators when controls are shown
        hideIndicators();

        mHideControlsTimeout.start();
        mControlsVisible = true;
    }
}

/*!
 * CxuiView::hideToolbar
 */
void CxuiView::hideToolbar()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mToolbar) {
        mToolbar->hide();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * CxuiView::showToolbar
 */
void CxuiView::showToolbar()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mToolbar) {
        mToolbar->show();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * CxuiView::hideIndicators
 */
void CxuiView::hideIndicators()
{
    if (mIndicators) {
        mIndicators->hide();
    }
}

/*!
 * CxuiView::showIndicators
 */
void CxuiView::showIndicators()
{
    if (mIndicators) {
        mIndicators->show();
    }
}

/*!
 * CxuiView::hideZoom
 */
void CxuiView::hideZoom()
{
    if (mSlider) {
        mSlider->hide();
    }
    mZoomVisible = false;
}

/*!
 * CxuiView::showZoom
 */
void CxuiView::showZoom()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mSlider) {

        // if maxVal has not been set yet, ask for new parameters from engine
        if (mSlider->maximum() <= 0) {

            // get the zoom range
            int min = mEngine->zoomControl().min();
            int max = mEngine->zoomControl().max();

            // only change values if they are acceptable and have changed
            if ((max - min > 0) && ((mSlider->maximum() != max) || (mSlider->minimum() != min))) {
                mSlider->setRange(min, max);
            }
        }

        // show zoom only if the slider has acceptable value
        if (mSlider->maximum() > 0) {
            mSlider->show();
        }
    }

    mZoomVisible = true;
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Function can be used to create a graphics item and setting it as a background
* item for HbWidget. graphicName refers to system wide graphic name. Given graphic
* can consist of one, three or nine pieces. Nine piece graphics are used by default.
* See HbFrameDrawer documentation for graphic naming.
*/
void CxuiView::createWidgetBackgroundGraphic(HbWidget *widget,
                                                       const QString &graphicName,
                                                       HbFrameDrawer::FrameType frameType)
{
    if (widget) {
        HbFrameDrawer *drawer = new HbFrameDrawer(graphicName, frameType);

        if (drawer) {
            HbFrameItem *backgroundItem = new HbFrameItem(drawer, widget);
            if (backgroundItem) {
                // set item to fill the whole widget
                backgroundItem->setGeometry(QRectF(QPointF(0, 0), widget->size()));
                backgroundItem->setZValue(0);
                widget->setBackgroundItem(backgroundItem);
            }
        }
    }
}

/*!
 * Handle mouse press events on this view. Needed to implement toggling of
 * controls and playing feedback.
 * \param event event to be handled
 */
void CxuiView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //! @todo temporary workaround for title bar mouse event handling bug
    if (event->type() == QEvent::GraphicsSceneMousePress && event->scenePos().y() > 70 &&
        isFeedbackEnabled()) {
        mControlsFeedback.setModalities(HbFeedback::All);
        mControlsFeedback.play();
        event->accept();
    }
}

/*!
 * Handle mouse release events on this view. Needed to implement toggling of
 * controls and playing feedback.
 * \param event to be handled
 */
void CxuiView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //! @todo temporary workaround for title bar mouse event handling bug
    if (event->type() == QEvent::GraphicsSceneMouseRelease && event->scenePos().y() > 70 &&
        isFeedbackEnabled()) {        
        mControlsFeedback.setModalities(HbFeedback::Tactile);
        mControlsFeedback.play();
        toggleControls();
        event->accept();
    }
}

// End of file
