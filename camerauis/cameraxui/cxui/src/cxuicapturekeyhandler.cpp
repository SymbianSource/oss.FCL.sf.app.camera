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
#include <w32std.h>
#include <QVariant>
#include <QKeyEvent>
#include <eikon.hrh>
#include <coemain.h>
#include <e32keys.h> // for EKeyCamera


#include "cxuicapturekeyhandler.h"
#include "cxutils.h"
#include "cxeengine.h"
#include "cxenamespace.h"
#include "cxefeaturemanager.h"

const int CXUI_KEY_PRIORITY = 100;

CxuiCaptureKeyHandler::CxuiCaptureKeyHandler(CxeEngine &aEngine) :
    mAutofocusKeyPressed(false), mCaptureKeyPressed(false), mEngine(aEngine), mWsSession(CCoeEnv::Static()->WsSession()),
    mWindowGroup(CCoeEnv::Static()->RootWin())
{
    CX_DEBUG_ENTER_FUNCTION();

    // Autofocus key
    mEngine.featureManager().configuredValues(CxeRuntimeKeys::PRIMARY_CAMERA_AUTOFOCUS_KEYS,
                                              mPrimaryCameraAutofocusKeys);

    // Capture keys for primary camera
    mEngine.featureManager().configuredValues(CxeRuntimeKeys::PRIMARY_CAMERA_CAPTURE_KEYS,
                                              mPrimaryCameraCaptureKeys);

    int scanCode = 0;
    int handle = 0;
    foreach (scanCode, mPrimaryCameraAutofocusKeys) {
        handle = mWindowGroup.CaptureKeyUpAndDowns(scanCode, 0, 0, CXUI_KEY_PRIORITY);
        mCapturedKeyUpDownHandles.append(handle);
    }
    foreach (scanCode, mPrimaryCameraCaptureKeys) {
        handle = mWindowGroup.CaptureKeyUpAndDowns(scanCode, 0, 0, CXUI_KEY_PRIORITY);
        mCapturedKeyUpDownHandles.append(handle);
    }

    // Capture key press events... this is done only to make sure other
    // applications do not react to camera key events.
    handle = mWindowGroup.CaptureKey(EKeyCamera, 0, 0, CXUI_KEY_PRIORITY);
    mCapturedKeyHandles.append(handle);

    CX_DEBUG_EXIT_FUNCTION();
}

CxuiCaptureKeyHandler::~CxuiCaptureKeyHandler()
{
    CX_DEBUG_ENTER_FUNCTION();

    int handle = 0;
    foreach (handle, mCapturedKeyUpDownHandles) {
        mWindowGroup.CancelCaptureKeyUpAndDowns(handle);
    }
    foreach (handle, mCapturedKeyHandles) {
        mWindowGroup.CancelCaptureKey(handle);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

bool CxuiCaptureKeyHandler::isAutofocusKeyPressed()
{
    return mAutofocusKeyPressed;
}

bool CxuiCaptureKeyHandler::isCaptureKeyPressed()
{
    return mCaptureKeyPressed;
}

bool CxuiCaptureKeyHandler::handleKeyEvent(QEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();
    bool eventWasConsumed = false;

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);
        if ( mPrimaryCameraAutofocusKeys.contains(keyEvent->nativeScanCode())
            && !mAutofocusKeyPressed ) {

            mAutofocusKeyPressed = true;
            eventWasConsumed = true;
            emit autofocusKeyPressed();

        } else if (mPrimaryCameraCaptureKeys.contains(keyEvent->nativeScanCode())
            && !mCaptureKeyPressed) {
            mCaptureKeyPressed = true;
            eventWasConsumed = true;
            emit captureKeyPressed();
        }
    } else if (event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*> (event);

        if ( mPrimaryCameraAutofocusKeys.contains(keyEvent->nativeScanCode())
            && mAutofocusKeyPressed ) {

            mAutofocusKeyPressed = false;
            eventWasConsumed = true;
            emit autofocusKeyReleased();

        } else if (mPrimaryCameraCaptureKeys.contains(keyEvent->nativeScanCode())
            && mCaptureKeyPressed) {

            mCaptureKeyPressed = false;
            eventWasConsumed = true;
            emit captureKeyReleased();
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
    return eventWasConsumed;
}
