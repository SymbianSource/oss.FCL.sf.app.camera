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
#include <hbactivitymanager.h>

#include <shareui.h>
#include <thumbnailmanager_qt.h>

#include "cxeviewfindercontrol.h"
#include "cxuienums.h"
#include "cxuipostcaptureview.h"
#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxestillimage.h"
#include "cxutils.h"
#include "cxefeaturemanager.h"
#include "cxuidocumentloader.h"
#include "cxesettings.h"
#include "cxenamespace.h"
#include "cxuiserviceprovider.h"

#ifdef Q_OS_SYMBIAN
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuipostcaptureviewTraces.h"
#endif
#endif //Q_OS_SYMBIAN


using namespace CxUiLayout;
using namespace Cxe;


namespace {
    const QString FILENAME_KEY = "filename";
    const int CXUI_STOP_VIEWFINDER_TIMEOUT = 5000; //  5 seconds
    const int CXUI_RELEASE_CAMERA_TIMEOUT = 60000; // 60 seconds
}


/*!
* Constructor.
*/
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
    mDeleteNoteOpen(false),
    mFilename(QString::null),
    mThumbnailManager(NULL)
{
    CX_DEBUG_IN_FUNCTION();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor.
*/
CxuiPostcaptureView::~CxuiPostcaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::instance()->removeEventFilter(this);
    stopTimers();
    delete mThumbnailManager;
    delete mShareUi;
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Second phase construction.
*/
void CxuiPostcaptureView::construct(HbMainWindow *mainwindow, CxeEngine *engine,
                                    CxuiDocumentLoader *documentLoader,
                                    CxuiCaptureKeyHandler *keyHandler,
                                    HbActivityManager *activityManager)
{
    Q_UNUSED(keyHandler);
    CX_DEBUG_ENTER_FUNCTION();

    CxuiView::construct(mainwindow, engine, documentLoader, NULL, activityManager);

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

/*!
* Handle pressing capture key.
*/
void CxuiPostcaptureView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mDeleteNoteOpen) {
        goToPrecaptureView();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle pressing auto focus key.
*/
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
    CX_DEBUG_ENTER_FUNCTION();

    stopTimers();
    releaseCamera();

    QString videoFile(getCurrentFilename());

    XQAiwRequest *videoRequest = mAppManager.create(
        "com.nokia.symbian.IVideoView","playMedia(QString)", true);

    if (videoRequest) {
        QVariantList fileList;
        fileList.append(QVariant(videoFile));
        videoRequest->setArguments(fileList);

        CX_DEBUG(("CxuiPostcaptureView: sending request"));
        QVariant result;
        bool res = videoRequest->send(result);
        if (res) {
            CX_DEBUG(("CxuiPostcaptureView: request sent, received \"%s\"",
                      result.toString().toAscii().constData()));
        } else {
            CX_DEBUG(("CxuiPostcaptureView: request sending failed, error=%d",
                      videoRequest->lastError()));
        }
        delete videoRequest;
        videoRequest = NULL;
    }

    CX_DEBUG_EXIT_FUNCTION();

}

/*!
* Show delete query.
*/
void CxuiPostcaptureView::showDeleteNote()
{
    CX_DEBUG_ENTER_FUNCTION();

    hideControls();

    QString text(mEngine->mode() == Cxe::VideoMode
               ? hbTrId("txt_cam_other_delete_video_clip")
               : hbTrId("txt_cam_other_delete_image"));

    HbMessageBox::question(text,
                           this,
                           SLOT(handleDeleteDialogClosed(int)),
                           HbMessageBox::Yes | HbMessageBox::No);

    mDeleteNoteOpen = true;
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle closing delete query dialog.
* @param action HbMessageBox::Yes if user accepted the delete query, HbMessageBox::No if not.
*/
void CxuiPostcaptureView::handleDeleteDialogClosed(int action)
{
    CX_DEBUG_ENTER_FUNCTION();

    hideControls();
    mDeleteNoteOpen = false;

    // Check that user confirmed delete
    if (action == HbMessageBox::Yes) {
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
            CX_DEBUG(("Delete file [%s], status %d", qPrintable(fileInfo.fileName()), ok));

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

/*!
* Go to pre-capture view.
*/
void CxuiPostcaptureView::goToPrecaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Cannot return to pre-capture while stopping in video mode
    if (mEngine->mode() != Cxe::VideoMode ||
        mEngine->videoCaptureControl().state() != CxeVideoCaptureControl::Stopping) {
        stopTimers();
        // Re-enabling starting timers the next time we enter post capture view.
        mTimersStarted = false;

        // reset saved filename
        mFilename = QString::null;

        // Switch to pre-capture view
        emit changeToPrecaptureView();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Stop viewfinder.
*/
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

/*!
* Handle events.
* Needed for restarting timers.
*/
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

/*!
 * Restore view state from activity.
 * @param activityId Activity id
 * @param data Activity data
 */
void CxuiPostcaptureView::restoreActivity(const QString &activityId, const QVariant &data)
{
    Q_UNUSED(activityId);
    CX_DEBUG_ENTER_FUNCTION();

    // get filename. if filename is not found (toString() returns empty string)
    // we will go back to pre-capture in updateSnapshotImage()
    mFilename = data.toMap()[FILENAME_KEY].toString();
    CX_DEBUG(("Got filename %s from activity", mFilename.toAscii().data()));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Save view state to activity.
 */
void CxuiPostcaptureView::saveActivity()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVariantMap data;
    QVariantHash params;

    QString filename = getCurrentFilename();
    CX_DEBUG(("Saving filename %s", filename.toAscii().data()));
    data.insert(FILENAME_KEY, filename);

    QImage img(mMainWindow->rect().size(), QImage::Format_ARGB32_Premultiplied);
    QPainter p(&img);
    mMainWindow->render(&p, mMainWindow->rect(), mMainWindow->rect());

    QPixmap screenshot = QPixmap::fromImage(img);

    params.insert("screenshot", screenshot);
    if (mEngine->mode() == Cxe::ImageMode) {
        mActivityManager->removeActivity(CxuiActivityIds::STILL_POSTCAPTURE_ACTIVITY);
        mActivityManager->addActivity(CxuiActivityIds::STILL_POSTCAPTURE_ACTIVITY, data, params);
    } else {
        mActivityManager->removeActivity(CxuiActivityIds::VIDEO_POSTCAPTURE_ACTIVITY);
        mActivityManager->addActivity(CxuiActivityIds::VIDEO_POSTCAPTURE_ACTIVITY, data, params);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Clear activity from activity manager.
 */
void CxuiPostcaptureView::clearActivity()
{
    CX_DEBUG_ENTER_FUNCTION();
    mActivityManager->removeActivity(CxuiActivityIds::STILL_POSTCAPTURE_ACTIVITY);
    mActivityManager->removeActivity(CxuiActivityIds::VIDEO_POSTCAPTURE_ACTIVITY);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Show event for this view.
* Update snapshot and start timers.
*/
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

/*!
* Hide event.
* Release snapshot and stop timers.
*/
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

/*!
 * Updates snapshot image. In normal case snapshot is retrieved from engine
 * but if we are restoring camera to post-capture through activity, then
 * we get snapshot from thumbnail manager.
 */
void CxuiPostcaptureView::updateSnapshotImage()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mFilename.isNull()) {
        CX_DEBUG(("CxuiPostcaptureView::updateSnapshot restoring activity"));
        // filename set, we are restoring activity
        if (QFile::exists(mFilename)) {
            CX_DEBUG(("Filename ok, requesting thumbnail from TNM"));
            if (!mThumbnailManager) {
                mThumbnailManager = new ThumbnailManager();
                connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
                                                this, SLOT(handleThumbnailReady(QPixmap, void*, int, int)));
                mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailLarge);
            }
            mThumbnailManager->getThumbnail(mFilename);
            CX_DEBUG(("Thumbnail requested"));

        } else {
            // file deleted
            CX_DEBUG(("File %s has been deleted, going back to pre-capture", mFilename.toAscii().data()));
            goToPrecaptureView();
        }
    } else {
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
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/* !
 * gets the filename of the current file
 */
QString CxuiPostcaptureView::getCurrentFilename()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mFilename.isNull()) {
        // post-capture started by activity, engine doesn't contain correct
        // filename anymore so use the stored one
        CX_DEBUG(("Using filename saved in activity"));
        CX_DEBUG_EXIT_FUNCTION();
        return mFilename;
    }

    CX_DEBUG(("Getting filename from engine"));
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
* Handle exiting standby.
*/
void CxuiPostcaptureView::exitStandby()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Common functionality first.
    CxuiView::exitStandby();

    //!@note We should not start timers until we receive the ShowEvent
    showControls();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle entering standby.
*/
void CxuiPostcaptureView::enterStandby()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Common functionality (release camera).
    CxuiView::enterStandby();

    stopTimers();
    hideControls();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Handle thumbnail received from ThumbnailManager.
 *
 * @param thumbnail Thumbnail as QPixmap
 * @param clientData Not used
 * @param id Thumbnail manager request id
 * @param errorCode Error code
 */
void CxuiPostcaptureView::handleThumbnailReady(QPixmap thumbnail, void *clientData, int id, int errorCode)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(clientData);
    Q_UNUSED(id);

    if (thumbnail.isNull()) {
        CX_DEBUG(("Received null thumbnail from TNM, going to pre-capture. Error=%d", errorCode));
        // null thumbnail, go to precapture
        goToPrecaptureView();
    } else if (mImageLabel) {
        mImageLabel->setIcon(HbIcon(QIcon(thumbnail)));
    }

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

    if (!mFilename.isNull()) {
        // restored from activity, don't do post-capture timeout
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

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

    // Release camera and stop viewfinder if user stays in postcapture long enough.
    // Battery could otherwise drain fast.
    mReleaseCameraTimer.start(CXUI_RELEASE_CAMERA_TIMEOUT);
    mStopViewfinderTimer.start(CXUI_STOP_VIEWFINDER_TIMEOUT);

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

    //!@note mTimersStarted is intentionally not reset here.
    // Once the timers are stopped, they are not to be started again until
    // we come from precapture view again.
    // E.g. returning from background could otherwise restart the timers and
    // if post-capture timer would be on, user could be confused: camera
    // shows up with  post-capture view, after couple  seconds it disappears
    // and we return to pre-capture view. That's not what we want.

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
