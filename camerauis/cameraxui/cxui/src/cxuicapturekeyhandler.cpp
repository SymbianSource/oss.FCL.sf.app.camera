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

    listenKeys(true);
    CX_DEBUG_EXIT_FUNCTION();
}

CxuiCaptureKeyHandler::~CxuiCaptureKeyHandler()
{
    CX_DEBUG_ENTER_FUNCTION();
    listenKeys(false);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Start or stop listening key events.
* @param listen Should we start (true) or stop (false) listening key events.
*/
void CxuiCaptureKeyHandler::listenKeys(bool listen)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (listen) {
        // Protect from multiple calls
        if (mCapturedKeyUpDownHandles.empty() && mCapturedKeyHandles.empty()) {

            int key(0);
            foreach (key, mPrimaryCameraAutofocusKeys) {
                CX_DEBUG(("CxuiCaptureKeyHandler - hooking autofocus key with scan / key code: %d", key));
                listenKey(key);
            }
            foreach (key, mPrimaryCameraCaptureKeys) {
                CX_DEBUG(("CxuiCaptureKeyHandler - hooking capture key with scan / key code: %d", key));
                listenKey(key);
            }
        }
    } else {

        int handle(0);
        foreach (handle, mCapturedKeyUpDownHandles) {
            mWindowGroup.CancelCaptureKeyUpAndDowns(handle);
        }
        mCapturedKeyUpDownHandles.clear();

        foreach (handle, mCapturedKeyHandles) {
            mWindowGroup.CancelCaptureKey(handle);
        }
        mCapturedKeyHandles.clear();
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
        CX_DEBUG(("CxuiCaptureKeyHandler - key press with nativeVirtualKey(): %d", keyEvent->nativeVirtualKey()));
        CX_DEBUG(("CxuiCaptureKeyHandler - key press with nativeScanCode(): %d", keyEvent->nativeScanCode()));

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
        CX_DEBUG(("CxuiCaptureKeyHandler - key press with nativeVirtualKey(): %d", keyEvent->nativeVirtualKey()));
        CX_DEBUG(("CxuiCaptureKeyHandler - key press with nativeScanCode(): %d", keyEvent->nativeScanCode()));

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

/*!
* Helper method to listen to given key (key code or scan code).
* We need to listen to both "key up", "key down" and "key pressed" events to
* get all the necessary events to handleKeyEvent(). If we e.g. just listen
* to up/down events, the way native events are translated to QKeyEvents,
* we only get QEvent::KeyRelease event when partially in background.
* @param key Keycode or scancode for the key to listen. Both should be listened.
*/
void CxuiCaptureKeyHandler::listenKey(int key)
{
    // Capture key down and up events
    int handle = mWindowGroup.CaptureKeyUpAndDowns(key, 0, 0, CXUI_KEY_PRIORITY);

    // Handle < 0 means error.
    if (handle >= 0) {
        mCapturedKeyUpDownHandles.append(handle);
    } else {
        CX_DEBUG(("[WARNING] CxuiCaptureKeyHandler - Problem hooking to key-up/key-down with code: %d", key));
    }

    // Capture key press events
    handle = mWindowGroup.CaptureKey(key, 0, 0, CXUI_KEY_PRIORITY);

    if (handle >= 0) {
        mCapturedKeyHandles.append(handle);
    } else {
        CX_DEBUG(("[WARNING] CxuiCaptureKeyHandler - Problem hooking to key-press with code: %d", key));
    }
}


// end of file
