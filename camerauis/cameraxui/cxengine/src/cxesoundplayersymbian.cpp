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
#include <cxesoundplayersymbian.h>
#include <AudioPreference.h>
#include "cxutils.h"
#include "cxecameradevicecontrolsymbian.h"
#include "cxestate.h"

const TUint KCxeAudioPriority = KAudioPriorityCameraTone;
//const TUint KCxeAudioPriority = KAudioPriorityVideoRecording;

_LIT(KCxeCaptureSound,    "z:\\system\\sounds\\digital\\capture.wav");
_LIT(KCxeVideoStartSound, "z:\\system\\sounds\\digital\\videoStart.wav");
_LIT(KCxeVideoStopSound,  "z:\\system\\sounds\\digital\\videoStop.wav");
_LIT(KCxeAutoFocusSound,  "z:\\system\\sounds\\digital\\autoFocus.wav");


CxeSoundPlayerSymbian::CxeSoundPlayerSymbian(CaptureSound soundId) :
    CxeStateMachine("CxeSoundPlayerSymbian"),
    mAudioPlayer(NULL),
    mSoundId(soundId)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeSoundPlayerSymbian::State>();
    initializeStates();

    doOpen();
    CX_DEBUG_EXIT_FUNCTION();
}

CxeSoundPlayerSymbian::~CxeSoundPlayerSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mAudioPlayer;

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeSoundPlayerSymbian::play()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeSoundPlayerSymbian::play mSoundId: %d, state: %d", mSoundId, state()));

    // Only play the capture sound if CMdaAudioPlayerUtility is initialised
    if (state() == Ready) {
        //! @todo: Define & implement what to do, when sound is already playing.
        // Important for sequence mode.
        setState(Playing);
        mAudioPlayer->Play();
    } else if( state() == NotReady ) {
        // Here sound loading has failed.
        // Emit signal with error code.
        emit playComplete(KErrNotReady);
    } 

    CX_DEBUG_EXIT_FUNCTION();
}


void CxeSoundPlayerSymbian::MapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds &/*aDuration*/)
{
    CX_DEBUG_IN_FUNCTION();
    CX_DEBUG(("MapcInitComplete aStatus: %d", aStatus));

    if (aStatus) {
        setState(NotReady);
    } else {
        setState(Ready);
    }
}

void CxeSoundPlayerSymbian::MapcPlayComplete(TInt aStatus)
{
    CX_DEBUG_IN_FUNCTION();
    CX_DEBUG(("MapcPlayComplete aStatus: %d", aStatus));

    if (aStatus != KErrNone && aStatus != KErrInUse) {
        // An error occurred. Close and reopen sound player to be sure.
        mAudioPlayer->Close();
        setState(NotReady);
        doOpen();
    } else {
        setState(Ready);
    }

    emit playComplete(aStatus);
}

void CxeSoundPlayerSymbian::doOpen()
{
    CX_DEBUG(("Calling OpenFileL for sound %d", mSoundId));
    TInt error = KErrNone;
    const TDesC* filename = 0;
    switch (mSoundId) {
    case StillCapture:
        filename = &KCxeCaptureSound;
        break;
    case VideoCaptureStart:
        filename = &KCxeVideoStartSound;
        break;
    case VideoCaptureStop:
        filename = &KCxeVideoStopSound;
        break;
    case  AutoFocus:
        filename = &KCxeAutoFocusSound;
        break;
    default:
        // sound is not known
        mSoundId = Unknown;
        break;
    }

    if (filename) {
        if (mAudioPlayer) {
            delete mAudioPlayer;
            mAudioPlayer = 0;
        }
        TRAP( error, mAudioPlayer =
                      CMdaAudioPlayerUtility::NewFilePlayerL(*filename, *this, KCxeAudioPriority,
                                                        TMdaPriorityPreference(KAudioPrefCamera)) );
        if (!error) {
            setState(Opening);
        } else {
            setState(NotReady);
        }
    } else {
        setState(NotReady);
    }
}


void CxeSoundPlayerSymbian::handleStateChanged(int /*newStateId*/, CxeError::Id /*error*/)
{
    // No implementation needed, because state is not visible outside of this class
}

CxeSoundPlayerSymbian::State CxeSoundPlayerSymbian::state() const
{
    return static_cast<State>(stateId());
}

void CxeSoundPlayerSymbian::initializeStates()
{
    // addState(id, name, allowed next states)
    addState(new CxeState(NotReady, "NotReady", Opening));
    addState(new CxeState(Opening, "Opening", NotReady | Ready));
    addState(new CxeState(Ready, "Ready", Playing | Opening | NotReady));
    addState(new CxeState(Playing, "Playing", Ready | Opening | NotReady));

    setInitialState(NotReady);
}
