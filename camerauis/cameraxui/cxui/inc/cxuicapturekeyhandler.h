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
#ifndef CXUICAPTUREKEYHANDLER_H
#define CXUICAPTUREKEYHANDLER_H

#include <QObject>

class RWsSession;
class RWindowGroup;
class QEvent;
class CxeEngine;


class CxuiCaptureKeyHandler : public QObject
{
Q_OBJECT
public:
    CxuiCaptureKeyHandler(CxeEngine &aEngine);
    virtual ~CxuiCaptureKeyHandler();

public:
    bool isAutofocusKeyPressed();
    bool isCaptureKeyPressed();

    bool handleKeyEvent(QEvent *event);

signals:
    // Note that these signals should be connected to view classes only, because
    // CxuiViewManager will handle all signal connections and disconnections
    void autofocusKeyPressed();
    void autofocusKeyReleased();
    void captureKeyPressed();
    void captureKeyReleased();

private:
    bool mAutofocusKeyPressed;
    bool mCaptureKeyPressed;

    QList<int> mCapturedKeyHandles;
    QList<int> mCapturedKeyUpDownHandles;

    QList<int> mPrimaryCameraAutofocusKeys;
    QList<int> mPrimaryCameraCaptureKeys;
    QList<int> mSecondaryCameraCaptureKeys;

    CxeEngine &mEngine;

    RWsSession &mWsSession; // not own
    RWindowGroup &mWindowGroup; // not own
};

#endif // CXUICAPTUREKEYHANDLER_H
