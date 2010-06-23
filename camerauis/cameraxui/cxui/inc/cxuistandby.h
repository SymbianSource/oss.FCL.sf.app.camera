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
#ifndef CXUISTANDBY_H
#define CXUISTANDBY_H

#include <QObject>

// constants

const int CXUI_STANDBY_CAMERA_TIMEOUT = 60000; // 60 seconds

// forward declaration
class HbDialog;
class QTimer;
class CxeEngine;
class CxuiDocumentLoader;
class CxuiCaptureKeyHandler;


/*
* class CxuiStandby continuously monitors for releasing camera when there are no user actions
* for a specific time period. Timer is used to release the camera HW and switching to stanby mode
* after a delay.Releasing the camera improves power efficiency, but creates additional latency
* when returning to pre-capture view.
*/
class CxuiStandby : public QObject
{
    Q_OBJECT

public:
    CxuiStandby(CxuiCaptureKeyHandler &keyHandler, CxuiDocumentLoader *documentLoader, CxeEngine *engine);
    ~CxuiStandby();

public:
    void allowDismiss(bool allow);
    bool isActive() const;

signals:

    /*
    * signal to prepare for standby
    */
    void aboutToEnterStandby();

    /*
    * signal to prepare UI for exiting standby
    */
    void aboutToExitStandby();

public slots:
    void startTimer();
    void stopTimer();
    bool handleMouseEvent(QEvent *event);
    void enterStandby();
    void exitStandby();

private slots:

    void dismissStandby();

private: // helper methods

    bool proceedToStandy();
    bool eventFilter(QObject *object, QEvent *event);

private:

    CxuiCaptureKeyHandler &mKeyHandler;
    CxuiDocumentLoader *mDocumentLoader; // not own
    CxeEngine *mEngine; // not own
    HbDialog *mStandbyPopup;
    QTimer *mStandbyTimer;
    bool mStandbyDialogVisible;
    bool mAllowDismiss;
};

#endif // CXUISTANDBY_H
