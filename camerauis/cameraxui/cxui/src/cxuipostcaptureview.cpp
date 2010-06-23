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
#include <QPixmap>
#include <QTimer>
#include <QFileInfo>
#include <QApplication>
#include <QGraphicsRectItem>
#include <QBrush>
#include <QDir>
#include <QProcess>

#include <hblabel.h>
#include <hbmainwindow.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmessagebox.h>
#include <hbnotificationdialog.h>

#include <shareui.h>

#include "cxeviewfindercontrol.h"
#include "cxuienums.h"
#include "cxuipostcaptureview.h"
#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxeviewfindercontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxestillimage.h"
#include "cxutils.h"
#include "cxefeaturemanager.h"
#include "cxuidocumentloader.h"
#include "cxesettings.h"
#include "cxenamespace.h"
#include "cxuiserviceprovider.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuipostcaptureviewTraces.h"
#endif


using namespace CxUiLayout;
using namespace Cxe;


//!@todo Temporarily disabled.
//const int CXUI_STOP_VIEWFINDER_TIMEOUT = 5000; // 5 seconds
//const int CXUI_RELEASE_CAMERA_TIMEOUT = 10000; // 10 seconds

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::CxuiPostcaptureView
//
// ---------------------------------------------------------------------------
//
CxuiPostcaptureView::CxuiPostcaptureView(QGraphicsItem *parent) :
    CxuiView(parent),
    mStillToolbar(NULL),
    mVideoToolbar(NULL),
    mEmbeddedToolbar(NULL),
    mBackgroundItem(NULL),
    mImageLabel(NULL),
    mShareUi(NULL),
    mStopViewfinderTimer(this),
    mReleaseCameraTimer(this),
    mPostcaptureTimer(this),
    mTimersStarted(false),
    mDeleteNoteOpen(false)
{
    CX_DEBUG_IN_FUNCTION();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::~CxuiPostcaptureView
//
// ---------------------------------------------------------------------------
//
CxuiPostcaptureView::~CxuiPostcaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::instance()->removeEventFilter(this);
    stopTimers();
    delete mShareUi;
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::construct
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::construct(HbMainWindow *mainwindow, CxeEngine *engine,
                                    CxuiDocumentLoader *documentLoader)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxuiView::construct(mainwindow, engine, documentLoader, NULL);

    // set back action to go back to pre-capture
    HbAction *backAction = new HbAction(Hb::BackNaviAction, this);
    connect(backAction, SIGNAL(triggered()), this, SLOT(goToPrecaptureView()));
    setNavigationAction(backAction);

    // creates black background item
    createBackground();

    CX_DEBUG_ASSERT(mDocumentLoader);
    QGraphicsWidget *widget = 0;
    widget = mDocumentLoader->findWidget(POST_CAPTURE_SNAPSHOT_LABEL);
    mImageLabel = qobject_cast<HbLabel *>(widget);
    CX_DEBUG_ASSERT(mImageLabel);

    mShareUi = new ShareUi();

    // get toolbar pointers from the documentloader
    widget = mDocumentLoader->findWidget(STILL_POST_CAPTURE_TOOLBAR);
    // This resize is a workaround to get toolbar shown correctly.
    widget->resize(60, 300);
    mStillToolbar = qobject_cast<HbToolBar *> (widget);
    CX_DEBUG_ASSERT(mStillToolbar);

    widget = mDocumentLoader->findWidget(VIDEO_POST_CAPTURE_TOOLBAR);
    // This resize is a workaround to get toolbar shown correctly.
    widget->resize(60, 300);
    mVideoToolbar = qobject_cast<HbToolBar *> (widget);
    CX_DEBUG_ASSERT(mVideoToolbar);

    widget = mDocumentLoader->findWidget(EMBEDDED_POST_CAPTURE_TOOLBAR);
    // This resize is a workaround to get toolbar shown correctly.
    widget->resize(60, 300);
    mEmbeddedToolbar = qobject_cast<HbToolBar *> (widget);
    CX_DEBUG_ASSERT(mEmbeddedToolbar);

    mStopViewfinderTimer.setSingleShot(true);
    connect(&mStopViewfinderTimer, SIGNAL(timeout()),
            this, SLOT(stopViewfinder()));

    mReleaseCameraTimer.setSingleShot(true);
    connect(&mReleaseCameraTimer, SIGNAL(timeout()),
            this, SLOT(releaseCamera()));

    mHideControlsTimeout.setSingleShot(true);
    connect(&mHideControlsTimeout, SIGNAL(timeout()),
            this, SLOT(hideControls()));

    mPostcaptureTimer.setSingleShot(true);
    connect(&mPostcaptureTimer, SIGNAL(timeout()),
            this, SLOT(goToPrecaptureView()));

    // set focus flags
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocusPolicy(Qt::StrongFocus);

    QCoreApplication::instance()->installEventFilter(this);
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::handleCaptureKeyPressed
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mDeleteNoteOpen) {
        goToPrecaptureView();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::handleAutofocusKeyPressed
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::handleAutofocusKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mDeleteNoteOpen) {
        goToPrecaptureView();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Slot for starting video playing.
 */
void CxuiPostcaptureView::playVideo()
{
    
    launchNotSupportedNotification();
    //! @todo needs an implementation
    CX_DEBUG_IN_FUNCTION();

}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::showDeleteNote
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::showDeleteNote()
{
    CX_DEBUG_ENTER_FUNCTION();

    hideControls();

    if (mEngine->mode() == Cxe::VideoMode) {
        HbMessageBox::question(hbTrId("txt_cam_other_delete_video_clip"),
                               this,
                               SLOT(handleDeleteDialogClosed(HbAction*)));
    } else {
        HbMessageBox::question(hbTrId("txt_cam_other_delete_image"),
                               this,
                               SLOT(handleDeleteDialogClosed(HbAction*)));
    }

    mDeleteNoteOpen = true;
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::handleDeleteDialogClosed
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::handleDeleteDialogClosed(HbAction *action)
{
    CX_DEBUG_ENTER_FUNCTION();

    hideControls();
    mDeleteNoteOpen = false;

    HbMessageBox *dlg = qobject_cast<HbMessageBox*>(sender());

    // check that it was "primary action" that closed the dialog
    if (dlg && dlg->actions().at(0) == action) {
        // User confirmed delete
        QString filename = getCurrentFilename();
        QFileInfo fileInfo(filename);
        if (fileInfo.exists()) {
            //! @todo
            // We can retry deletion if file deletion does'nt succeed,
            // but this is left out for the time being since the user
            // can't delete it so early that it's not saved yet or
            // is being harvested by MdS etc.
            QDir dir = fileInfo.absolutePath();
            bool ok = dir.remove(fileInfo.fileName());
            CX_DEBUG(("Delete file [%s], status %d", fileInfo.fileName().toAscii().constData(), ok));

            // Go back to precapture view
            goToPrecaptureView();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
    Slot for handling image/video sharing.
 */
void CxuiPostcaptureView::launchShare()
{
    CX_DEBUG_ENTER_FUNCTION();

    stopTimers();
    releaseCamera();
    hideControls();
    QString filename = getCurrentFilename();
    QStringList filelist;
    filelist.append(filename);

    mShareUi->send(filelist, true);

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::goToPrecaptureView
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::goToPrecaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Cannot return to pre-capture while stopping in video mode
    if (mEngine->mode() != Cxe::VideoMode ||
        mEngine->videoCaptureControl().state() != CxeVideoCaptureControl::Stopping) {
        stopTimers();
        // Re-enabling starting timers the next time we enter post capture view.
        mTimersStarted = false;

        // Make sure engine prepares for new image/video if necessary
        mEngine->initMode(mEngine->mode());

        // Switch to pre-capture view
        emit changeToPrecaptureView();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::stopViewfinder
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::stopViewfinder()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mMainWindow->currentView() == this) {
        mEngine->viewfinderControl().stop();
    }
    mStopViewfinderTimer.stop();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Hides toolbar.
*/
void CxuiPostcaptureView::hideToolbar()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mStillToolbar) {
        mStillToolbar->hide();
    }
    if (mVideoToolbar) {
        mVideoToolbar->hide();
    }
    if (mEmbeddedToolbar) {
        mEmbeddedToolbar->hide();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::eventFilter
//
// ---------------------------------------------------------------------------
//
bool CxuiPostcaptureView::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)
    bool eventWasConsumed = false;

    switch (event->type())
    {
    case QEvent::GraphicsSceneMouseRelease:
        mHideControlsTimeout.start();
        break;
    case QEvent::GraphicsSceneMousePress:
        mHideControlsTimeout.stop();
        // stop the postcapture timer
        mPostcaptureTimer.stop();
        break;
    default:
        break;
    }

    return eventWasConsumed;
}

/*!
* Paint method.
* Used for performance tracing purposes.
*/
void CxuiPostcaptureView::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
        OstTrace0(camerax_performance, CXUIPOSTCAPTUREVIEW_SNAPSHOT_DRAW, "msg: e_CX_SHOT_TO_SNAPSHOT 0");
        QGraphicsWidget::paint(painter, option, widget);
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::showEvent
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::showEvent(QShowEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();

    QGraphicsWidget::showEvent(event);

    if (event->type() == QEvent::Show) {
        QCoreApplication::instance()->installEventFilter(this);

        updateSnapshotImage();
        showControls();
        startTimers();
        event->accept();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::hideEvent
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::hideEvent(QHideEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();

    QGraphicsWidget::hideEvent(event);

    if (event->type() == QEvent::Hide) {
        // remove event filter to disable unnecessary actions
        QCoreApplication::instance()->removeEventFilter(this);

        // Clear the snapshot.
        mImageLabel->setIcon(HbIcon());

        stopTimers();
        // Hide controls to make sure title bar reacts to show()
        // when this view is reactivated.
        hideControls();
        event->accept();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*! Shows toolbar.
    Toolbar that is shown is selected based on current mode.
 */
void CxuiPostcaptureView::showToolbar(){

    CX_DEBUG_ENTER_FUNCTION();

    if (CxuiServiceProvider::isCameraEmbedded()) {
        mEmbeddedToolbar->setVisible(true);
        mStillToolbar->setVisible(false);
        mVideoToolbar->setVisible(false);
    } else {
        mEmbeddedToolbar->setVisible(false);
        if (mEngine->mode() == ImageMode) {
            mVideoToolbar->setVisible(false);
            mStillToolbar->setVisible(true);
            mStillToolbar->show();
        } else {
            mStillToolbar->setVisible(false);
            mVideoToolbar->setVisible(true);
            mVideoToolbar->show();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Creates black background item. If snapshot smaller than the screen size,
    background is visible.
 */
void CxuiPostcaptureView::createBackground()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mBackgroundItem) {
        mBackgroundItem = new QGraphicsRectItem(this);
        QBrush blackBrush = QBrush(Qt::black);
        mBackgroundItem->setBrush(blackBrush);
        mBackgroundItem->setRect(mMainWindow->sceneRect());
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPostcaptureView::setImage
//
// ---------------------------------------------------------------------------
//
void CxuiPostcaptureView::updateSnapshotImage()
{
    CX_DEBUG_ENTER_FUNCTION();

    QPixmap snapshot;

    if (mEngine->mode() == ImageMode) {
        if( mEngine->stillCaptureControl().imageCount() > 0 ) {
            snapshot = mEngine->stillCaptureControl()[0].snapshot();
        }
    } else {
        snapshot = mEngine->videoCaptureControl().snapshot();
    }

    if (mImageLabel) {
        mImageLabel->setIcon(HbIcon(QIcon(snapshot)));
    } else {
        // do nothing
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/* !
 * gets the filename of the current file
 */
QString CxuiPostcaptureView::getCurrentFilename()
{
    CX_DEBUG_ENTER_FUNCTION();

    QString filename;

    if (mEngine->mode() == Cxe::VideoMode) {
        filename = mEngine->videoCaptureControl().filename();
    } else {
        //!@todo Currently only gets index 0 from the still capture control.
        CxeStillCaptureControl& stillCaptureControl = mEngine->stillCaptureControl();
        if (stillCaptureControl.imageCount()) {
            filename = stillCaptureControl[0].filename();
        }
    }
    CX_DEBUG((filename.toAscii()));

    CX_DEBUG_EXIT_FUNCTION();

    return filename;
}

/*!
    Sends current capture to client app and closes camera
*/
void CxuiPostcaptureView::select()
{
    CxuiServiceProvider *serviceProvider = CxuiServiceProvider::instance();
    if (!serviceProvider) {
        return;
    }

    QString filename = getCurrentFilename();
    serviceProvider->sendFilenameToClientAndExit(filename);
}

/*!
    Handle cases when we gain focus
*/
void CxuiPostcaptureView::handleFocusGained()
{
    CX_DEBUG_ENTER_FUNCTION();

    //Note: We should not start timers until we receive the ShowEvent
    showControls();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Handle cases when we loose focus
*/
void CxuiPostcaptureView::handleFocusLost()
{
    CX_DEBUG_ENTER_FUNCTION();

    // we have lost focus
    releaseCamera();
    stopTimers();
    hideControls();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Start the timers
*/
void CxuiPostcaptureView::startTimers()
{
    CX_DEBUG_ENTER_FUNCTION();

    // we start timers only once in a given postcapture view session
    if(!mTimersStarted) {
        startPostcaptureTimer();
        startReleaseTimers();
        mTimersStarted = true;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Start the timer to return to pre-capture view
*/
void CxuiPostcaptureView::startPostcaptureTimer()
{
    CX_DEBUG_ENTER_FUNCTION();

    int postCaptureTimeout = 0;
    QString settingId;

    if (mEngine->mode() == ImageMode) {
        settingId = CxeSettingIds::STILL_SHOWCAPTURED;
    } else {
        settingId = CxeSettingIds::VIDEO_SHOWCAPTURED;
    }

    if (!CxuiServiceProvider::isCameraEmbedded()) {
        CxeError::Id err = mEngine->settings().get(settingId, postCaptureTimeout);

        if (postCaptureTimeout > 0 && err == CxeError::None) {
            mPostcaptureTimer.start(postCaptureTimeout);
        } else {
            // do nothing
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Start the timers to stop viewfinder and release the camera
*/
void CxuiPostcaptureView::startReleaseTimers()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Todo Note: Temporarily disabling release timer because of
    // graphics memory problems related to releasing and reserving again.
    // mReleaseCameraTimer.start(CXUI_RELEASE_CAMERA_TIMEOUT);
    // mStopViewfinderTimer.start(CXUI_STOP_VIEWFINDER_TIMEOUT);

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiPostcaptureView::stopTimers()
{
    CX_DEBUG_ENTER_FUNCTION();

    // stop all the timers
    mHideControlsTimeout.stop();
    mReleaseCameraTimer.stop();
    mPostcaptureTimer.stop();
    mStopViewfinderTimer.stop();

    // Note: mTimersStarted is intentionally not reset here.
    // Once the timers are stopped, they are not to be started again until
    // we come from precapture view again.
    // E.g. returning from background could otherwise restart the timers and
    // if post-capture timer would be on, user could be confused: camera
    // shows up with  post-capture view, after couple  seconds it disappears
    // and we return to pre-capture view. That's not what we want.

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
