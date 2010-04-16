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
#ifndef CXEVIDEOCAPTURECONTROLSYMBIAN_H_
#define CXEVIDEOCAPTURECONTROLSYMBIAN_H_

#include <QTime>
#include <QPixmap>
#include <QString>
#include <driveinfo.h>
#include <videorecorder.h>
#include <ecam/camerasnapshot.h>

#include "cxesettings.h"
#include "cxequalitypresets.h"
#include "cxequalitypresetssymbian.h"
#include "cxestillimagesymbian.h"
#include "cxevideocapturecontrol.h"
#include "cxestatemachine.h"
#include "cxenamespace.h"


// constants
const TInt KOneSecond = 1000000;

//CamCControllerCustomCommands.h.

// Controller UId, can be used by the client to identify the controller, e.g. if the custom command can be used
const TUid KCamCControllerImplementationUid = {0x101F8503};
const int KMaintainAspectRatio = false;
// TMMFEvent UIDs for Async stop
const TUid KCamCControllerCCVideoRecordStopped = {0x2000E546};
const TUid KCamCControllerCCVideoFileComposed = {0x2000E547};

// Custom command for setting a new filename without closing & reopening the controller
enum TCamCControllerCustomCommands
    {
    ECamCControllerCCNewFilename = 0,
    ECamCControllerCCVideoStopAsync
    };


// forward declarations
class CxeSettings;
class CxeCameraDevice;
class CxeQualityPresets;
class CxeFilenameGenerator;
class CxeViewfinderControl;
class CxeStillImageSymbian;
class CxeSoundPlayerSymbian;
class CxeCameraDeviceControl;
class CxeVideoRecorderUtility;


class CxeVideoCaptureControlSymbian : public CxeVideoCaptureControl,
                                      public CxeStateMachine,
                                      public MVideoRecorderUtilityObserver
{
    Q_OBJECT

public:  // constructors

    CxeVideoCaptureControlSymbian( CxeCameraDevice &cameraDevice,
                                   CxeViewfinderControl &viewfinderControl,
                                   CxeCameraDeviceControl &cameraDeviceControl,
                                   CxeFilenameGenerator &nameGenerator,
                                   CxeSettings &settings,
                                   CxeQualityPresets &qualityPresets);

    virtual ~CxeVideoCaptureControlSymbian();

public:  // functions from CxeVideoCaptureControl
    CxeVideoCaptureControl::State state() const;
    void record();
    void pause();
    void stop();
    void remainingTime(int &time);
    bool elapsedTime(int &time);
    void reset();
    QString filename() const;
    QPixmap snapshot() const;
    QList<CxeVideoDetails> supportedVideoQualities();

public:  // public member functions, not in client API
    void handleSnapshotEvent(CxeError::Id error);

public: // functions from MVideoRecorderUtilityObserver
    void MvruoOpenComplete(TInt aError);
    void MvruoPrepareComplete(TInt aError);
    void MvruoRecordComplete(TInt aError);
    void MvruoEvent(const TMMFEvent& aEvent);

signals:
    void videoPrepareComplete(CxeError::Id error);
    void prepareZoomForVideo();

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

public slots:
    //! Notification for when camera device is ready and we're entering video mode
    void init();
    //! Notification for when we're exiting video mode
    void deinit();
    //! Notification that videocapture sound has been played
    void handleSoundPlayed();

protected slots:
    // notifications when ECam reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);
    void prepareForRelease();
    // ECam events
    void handleCameraEvent(int eventUid, int error);
    // settings call back
    void handleSettingValueChanged(const QString& settingId,QVariant newValue);

private: // helper methods
    CxeError::Id findVideoController(const TDesC8& aMimeType, const TDesC&  aPreferredSupplier);
    void releaseResources();
    void initializeStates();
    CxeError::Id getVideoQualityDetails(CxeVideoDetails &videoInfo);
    int prepareVideoSnapshot();
    void initVideoRecorder();
    void open();
	void prepare();
    TSize getSnapshotSize() const;
    virtual void createVideoRecorder();
    void calculateRemainingTime(CxeVideoDetails videoDetails, int &time);
    TFourCC audioType(const QString& str);
    void updateRemainingRecordingTimeCounter();

protected: // protected data
    //! Video Recorder
    CxeVideoRecorderUtility *mVideoRecorder;

private: // private data

    //! Video Recorder controller UID.
    TUid mVideoControllerUid;
    //! Video Recorder format UID.
    TUid mVideoFormatUid;
    CxeCameraDevice &mCameraDevice;
    CxeCameraDeviceControl &mCameraDeviceControl;
    CxeViewfinderControl &mViewfinderControl;
    CxeFilenameGenerator &mFilenameGenerator;
    CxeSettings &mSettings;
    CxeQualityPresets &mQualityPresets;
    //! Snapshot image.
    QPixmap mSnapshot;
    //! Soundplayers, own
    CxeSoundPlayerSymbian *mVideoStartSoundPlayer;
    CxeSoundPlayerSymbian *mVideoStopSoundPlayer;
    //! New file name generated for the video prepare.
    QString mNewFileName;
    //! Current video file name
    QString mCurrentFilename;
    //video resolutions supported by ICM
    QList<CxeVideoDetails> mIcmSupportedVideoResolutions;
    //current video quality details in use
    CxeVideoDetails mCurrentVideoDetails;
};

#endif /* CXEVIDEOCAPTURECONTROLSYMBIAN_H_ */
