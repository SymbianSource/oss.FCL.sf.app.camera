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
#ifndef CXESOUNDPLAYERSYMBIAN_H
#define CXESOUNDPLAYERSYMBIAN_H

#include <QObject>
#include <QMetaType>
#include <mdaaudiosampleplayer.h>
#include "cxestatemachine.h"

class CMdaAudioPlayerUtility;
class CxeCameraDeviceControlSymbian;

class CxeSoundPlayerSymbian : public QObject,
                              public CxeStateMachine,
                              public MMdaAudioPlayerCallback
{
    Q_OBJECT
public:
    /**
     * CaptureSound enum defines different types of capture sounds
     */
    enum CaptureSound {
        //! Unknown is used when the capture sound is not set
        Unknown = 0,
        //! StillCapture sound is used in image capturing
        StillCapture,
        //! VideoCaptureStart is used as video start sound
        VideoCaptureStart,
        //! VideoCaptureStop is used as video stop sound
        VideoCaptureStop,
        //! AutoFocus is used when auto focus completes succesfully
        AutoFocus
    };

    /*
     * Sound player states.
     */
    enum State {
        //! No sound is open
        NotReady = 0x01,

        //! OpenFileL is in progress
        Opening  = 0x02,

        //! Sound file is open and we're ready to play
        Ready = 0x04,

        //! Playback in progress
        Playing = 0x08
    };

    CxeSoundPlayerSymbian(CaptureSound soundId);
    virtual ~CxeSoundPlayerSymbian();

    /**
     * Plays the currently open capture sound.
     */
    void play();

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

signals:
    /**
     * playComlete signal is emitted when sound has been played.
     * @param error Contains status information whether there was a problem with playing or not
     */
    void playComplete(int error);

public slots:

protected: // from MMdaAudioPlayerCallback
    void MapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds &aDuration);
    void MapcPlayComplete(TInt aStatus);

private:
    /**
     * Get current sound player state.
     */
    State state() const;

    /**
     *
     */
    void doOpen();

    /**
     * Initialize CxeStateMachine.
     */
    void initializeStates();

    //! Own.
    CMdaAudioPlayerUtility *mAudioPlayer;

    //! Currently opened sound file
    CxeSoundPlayerSymbian::CaptureSound mSoundId;
};

Q_DECLARE_METATYPE(CxeSoundPlayerSymbian::State)

#endif  // CXESOUNDPLAYERSYMBIAN_H

