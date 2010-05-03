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
#ifndef CXUIVIDEOPRECAPTUREVIEW_H
#define CXUIVIDEOPRECAPTUREVIEW_H

#include <QTimer>
#include <QPropertyAnimation>

#include "cxuiprecaptureview.h"
#include "cxevideocapturecontrol.h"

class HbLabel;
class HbAction;
class CxeVideoCaptureControl;
class CxuiDocumentLoader;
class CxuiCaptureKeyHandler;
class HbDialog;


/**
 * Pre-capture view for video recording mode
 */
class CxuiVideoPrecaptureView : public CxuiPrecaptureView
    {
    Q_OBJECT

public:
    CxuiVideoPrecaptureView(QGraphicsItem *parent = 0);

    virtual ~CxuiVideoPrecaptureView();

public:

    /**
     * Construct-method handles initialisation tasks for this class.
     * @param mainwindow
     * @param engine
     * @param documentLoader
     * @param keyHandler
     */
    void construct(HbMainWindow *mainwindow, CxeEngine *engine,
                   CxuiDocumentLoader *documentLoader,
                   CxuiCaptureKeyHandler *keyHandler = NULL);

    /**
     * Loads widgets that are needed right from the start.
     */
    virtual void loadDefaultWidgets();

    /**
     * Loads widgets that are not part of the default section in layouts xml.
     * Widgets are created at the time they are first loaded.
     */
    virtual void loadWidgets();

    void updateOrientation(Qt::Orientation orientation);

public slots:

    // from CxuiPrecaptureView
    virtual void handleSettingValueChanged(const QString& key, QVariant newValue);

    void handleQuitClicked();

protected slots:

    void handleSnapshot(CxeError::Id error);
    void record();
    void pause();
    void stop();
    void stopAndPrepareNewVideo();
    void goToStill();
    void showEvent(QShowEvent *event);
    void handleVideoStateChanged(CxeVideoCaptureControl::State newState, CxeError::Id error);
    void handleCaptureKeyPressed();
    void prepareNewVideo(CxeError::Id error = CxeError::None);
    void toggleLight();
    void launchVideoScenePopup();
    void launchSetting();

    /**
     * Application focus slots are called if focus state is changed.
     * Recording is stopped and camera released or reserved according to new state.
     */
    void handleFocusLost();

    /**
     * Battery almost empty warning signal.
     * Need to stop ongoing recording.
     */
    void handleBatteryEmpty();

    void updateTimeLabels();

protected:

    void setVideoTime(HbLabel* label, int time);
    bool getElapsedTime();
    void getRemainingTime();
    virtual void showToolbar();
    void disableFeedback();
    void enableFeedback();

    void initializeSettingsGrid();
    void setRecordingItemsVisibility(bool visible);

protected:
    QTimer mElapsedTimer;
    int mTimeElapsed;
    int mTimeRemaining;

    HbLabel *mElapsedTimeText; // not own
    HbLabel *mRemainingTimeText; // not own
    HbLabel *mRecordingIcon; // not own
    HbAction *mGoToStillAction; // not own
    HbToolBar *mToolBarIdle; // not own
    HbToolBar *mToolBarRec; // not own
    HbToolBar *mToolBarPaused; // not own
    HbDialog *mVideoScenePopup;
    CxeVideoCaptureControl *mVideoCaptureControl;
    HbMenu *mMenu;    // own
    QPropertyAnimation* mRecordingAnimation; //For recording indicator

    bool mCapturePending;
    };

#endif // CXUIVIDEOPRECAPTUREVIEW_H
