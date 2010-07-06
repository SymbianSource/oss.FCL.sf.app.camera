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
                   CxuiCaptureKeyHandler *keyHandler,
                   HbActivityManager *activityManager);

    /**
     * Loads widgets that are needed right from the start.
     */
    virtual void loadDefaultWidgets();

    /**
     * Loads widgets that are not part of the default section in layouts xml.
     * Widgets are created at the time they are first loaded.
     */
    virtual void loadWidgets();

    void restoreActivity(const QString &activityId, const QVariant &data);
    void saveActivity();
    void clearActivity();

public slots:

    // from CxuiPrecaptureView
    virtual void handleSettingValueChanged(const QString& key, QVariant newValue);

    void handleSceneChanged(CxeScene &scene);
    void handleQuitClicked();

protected slots:

    void handleSnapshot(CxeError::Id error);
    void record();
    void pause();
    void stop();
    void goToStill();
    void showEvent(QShowEvent *event);
    void handleVideoStateChanged(CxeVideoCaptureControl::State newState, CxeError::Id error);
    void handleCaptureKeyPressed();
    void prepareNewVideo(CxeError::Id error = CxeError::None);
    void launchVideoScenePopup();
    void launchSetting();
    void updateTimeLabels();

    // from CxuiPrecaptureView
    void hideControls();

protected:

    void setVideoTime(HbLabel* label, int time);
    bool getElapsedTime();
    void getRemainingTime();
    bool allowShowControls() const;
    bool isFeedbackEnabled() const;
    void showToolbar();
    void disableFeedback();
    void enableFeedback();

    void initializeSettingsGrid();
    bool isPostcaptureOn() const;
    void updateSceneIcon(const QString& sceneId);
    void updateQualityIcon();
    void setRecordingItemsVisibility(bool visible);

    // from QObject
    bool eventFilter(QObject *object, QEvent *event);

protected:
    QTimer mElapsedTimer;
    int mTimeElapsed;
    int mTimeRemaining;

    HbLabel *mElapsedTimeText; // not own
    HbLabel *mRemainingTimeText; // not own
    HbLabel *mRecordingIcon; // not own
    HbAction *mGoToStillAction; // not own
    HbToolBar *mToolbarIdle; // not own
    HbToolBar *mToolbarRec; // not own
    HbToolBar *mToolbarPaused; // not own
    HbDialog *mVideoScenePopup;
    CxeVideoCaptureControl *mVideoCaptureControl;
    HbMenu *mMenu;    // own
    QPropertyAnimation* mRecordingAnimation; //For recording indicator

    bool mCapturePending;
    QTimer mPauseTimer;
    };

#endif // CXUIVIDEOPRECAPTUREVIEW_H
