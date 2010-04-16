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
#ifndef CXUISELFTIMER_H
#define CXUISELFTIMER_H

#include <QObject>
#include <QTimer>

class CxeSettings;
class CxuiDocumentLoader;
class HbLabel;
class HbPushButton;
class HbWidget;

class CxuiSelfTimer : public QObject
{
    Q_OBJECT

public:
    explicit CxuiSelfTimer(CxeSettings &mSettings);
    ~CxuiSelfTimer();

    void loadSelftimerWidgets(CxuiDocumentLoader *documentLoader);
    bool isEnabled();
    bool isOngoing();

signals:
    void timerFinished();
    void cancelled();

public slots:
    void changeTimeOut(int seconds);
    void startTimer();
    void reset();
    void cancel();

protected slots:
    void timeout();

private:
    void reset(bool update);
    void updateWidgets();
    void showWidgets();
    void hideWidgets();

private:
    Q_DISABLE_COPY(CxuiSelfTimer)

    int mDelay;
    int mCounter;
    QTimer mTimer;
    int mOldPostCaptureTimeOut;

    // UI widgets, not own
    HbWidget *mIndicatorContainer;
    HbWidget *mButtonContainer;
    HbLabel *mTimerLabel;
    HbPushButton *mCancelButton;
    HbPushButton *mStartButton;

    // settings, not own
    CxeSettings &mSettings;

};

#endif // CXUISELFTIMER_H
