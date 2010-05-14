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
#ifndef CXUIPOSTCAPTUREVIEW_H
#define CXUIPOSTCAPTUREVIEW_H

#include <QObject>
#include <QList>
#include <QTimer>
#include <hbview.h>
#include <xqappmgr.h>

class QGraphicsRectItem;
class HbMainWindow;
class HbAction;
class HbLabel;

class CxeEngine;
class CxuiDocumentLoader;


/**
 * Post-capture view
 */
class CxuiPostcaptureView : public HbView
{
    Q_OBJECT

public:
    CxuiPostcaptureView(QGraphicsItem *parent = 0);
    virtual ~CxuiPostcaptureView();
    void construct(HbMainWindow *mainwindow, CxeEngine *engine, CxuiDocumentLoader *documentLoader);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
    void changeToPrecaptureView();

public slots:
    void handleCaptureKeyPressed();
    void handleAutofocusKeyPressed();

protected:
    bool eventFilter(QObject *object, QEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void showToolbar();
    void createBackground();
    void updateSnapshotImage();
    void stopTimers();

protected slots:
    void goToPrecaptureView();
    void stopViewfinder();
    void releaseCamera();

    // Control visibility
    void hideControls();
    void showControls();
    void toggleControls();

    // toolbar actions
    void launchPhotosApp();
    void select();
    void launchShare();
    void playVideo();
    void showDeleteNote();
    void launchVideosApp();

    void handleDeleteDialogClosed(HbAction *action);

private:
    void launchNotSupportedNotification();
    QString getCurrentFilename();

private slots:
    void startTimers();
    void handleFocusLost();

private: // data
    HbMainWindow *mMainWindow; // not own
    CxeEngine *mEngine; // not own
    CxuiDocumentLoader *mDocumentLoader; // not own
    HbToolBar *mStillToolbar;
    HbToolBar *mVideoToolbar;
    HbToolBar *mEmbeddedToolbar;
    QGraphicsRectItem *mBackgroundItem;
    HbLabel *mImageLabel;

    QTimer mHideControlsTimeout;

    /**
     * Timer used to stop viewfinder after a delay if the user remains in
     * post-capture view. Viewfinder is not stopped immediately when
     * capturing an image to improve the latency it takes to return to
     * pre-capture view.
     */
    QTimer mStopViewfinderTimer;

    /**
     * Timer used to release the camera HW after a delay if the user
     * remains in post-capture view. Releasing the camera improves
     * power efficiency, but creates additional latency when returning
     * to pre-capture view.
     */
    QTimer mReleaseCameraTimer;

    /*
    * Timer used to return back to precapture view after a time-out
    * Timer is disabled if show postcapture view setting is "continuous" i.e. -1
    */
    QTimer mPostcaptureTimer;

    bool mControlsVisible;

    bool mTimersStarted;
    
    XQApplicationManager mAppManager;

};

#endif // CXUIPOSTCAPTUREVIEW_H
