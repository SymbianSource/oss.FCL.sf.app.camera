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
#include <QTime>
#include <fbs.h>
#include <QPixmap>
#include <coemain.h>
#include <QStringList>
#include <AudioPreference.h>

#include "cxevideocapturecontrolsymbian.h"
#include "cxevideorecorderutilitysymbian.h"
#include "cxecameradevicecontrolsymbian.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxeerrormappingsymbian.h"
#include "cxeviewfindercontrol.h"
#include "cxestillimagesymbian.h"
#include "cxecameradevice.h"
#include "cxutils.h"
#include "cxesysutil.h"
#include "cxestate.h"
#include "cxesettings.h"
#include "cxenamespace.h"
#include "OstTraceDefinitions.h"
#include "cxesoundplayersymbian.h"
#include "cxequalitypresetssymbian.h"
#include "cxeviewfindercontrolsymbian.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxevideocapturecontrolsymbianTraces.h"
#endif


// constants
const TInt64  KMinRequiredSpaceVideo       = 4000000;
const uint    KOneMillion                 = 1000000;
const qreal   KMetaDataCoeff              = 1.03;      // Coefficient to estimate metadata amount
const uint    KCamCMaxClipDurationInSecs  = 5400;      // Maximun video clip duration in seconds
const qreal   KCMRAvgVideoBitRateScaler   = 0.9;       // avg video bit rate scaler


/*!
* CxeVideoCaptureControlSymbian::CxeVideoCaptureControlSymbian
*/
CxeVideoCaptureControlSymbian::CxeVideoCaptureControlSymbian(CxeCameraDevice &cameraDevice,
                                                             CxeViewfinderControl &viewfinderControl,
                                                             CxeCameraDeviceControl &cameraDeviceControl,
                                                             CxeFilenameGenerator &nameGenerator,
                                                             CxeSettings &settings,
                                                             CxeQualityPresets &qualityPresets) :
    CxeStateMachine("CxeVideoCaptureControlSymbian"),
    mVideoRecorder(NULL),
    mCameraDevice(cameraDevice),
    mCameraDeviceControl(cameraDeviceControl),
    mViewfinderControl(viewfinderControl),
    mFilenameGenerator(nameGenerator),
    mSettings(settings),
    mQualityPresets(qualityPresets),
    mSnapshot(),
    mNewFileName(""),
    mCurrentFilename("")
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeVideoCaptureControl::State> ();
    initializeStates();
    mVideoStopSoundPlayer = new
             CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::VideoCaptureStop);
    mVideoStartSoundPlayer = new
             CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::VideoCaptureStart);

    // connect signals from cameraDevice, so we recieve events when camera reference changes
    connect(&cameraDevice, SIGNAL(prepareForCameraDelete()),
            this, SLOT(prepareForCameraDelete()));
    connect(&cameraDevice, SIGNAL(prepareForRelease()),
            this, SLOT(prepareForRelease()) );
    connect(&cameraDevice, SIGNAL(cameraAllocated(CxeError::Id)),
            this, SLOT(handleCameraAllocated(CxeError::Id)));
    // connect playing sound signals
    connect(mVideoStartSoundPlayer, SIGNAL(playComplete(int)),
            this, SLOT(handleSoundPlayed()));

    // enabling setting change callbacks to videocapturecontrol
    connect(&mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&,QVariant)));

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* CxeVideoCaptureControlSymbian::~CxeVideoCaptureControlSymbian()
*/
CxeVideoCaptureControlSymbian::~CxeVideoCaptureControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    releaseResources();
    mIcmSupportedVideoResolutions.clear();
    delete mVideoStartSoundPlayer;
    delete mVideoStopSoundPlayer;

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Initializes resources for video recording.
*/
void CxeVideoCaptureControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_INIT, "msg: e_CX_VIDEO_CAPCONT_INIT 1" );

    if (state() == Idle) {
        // start initializing resources for video capture
        initVideoRecorder();
    } else if (state() == Initialized) {
        // video recorder already initalized. Continue to prepare video reocording.
        open();
    }

    OstTrace0( camerax_performance, DUP1_CXEVIDEOCAPTURECONTROLSYMBIAN_INIT, "msg: e_CX_VIDEO_CAPCONT_INIT 0" );

    CX_DEBUG_EXIT_FUNCTION();
}


/*
* Releases all resources
*/
void CxeVideoCaptureControlSymbian::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_DEINIT, "msg: e_CX_VIDEO_CAPCONT_DEINIT 1" );

    if(state() == Idle) {
        // nothing to do
        return;
    }

    // first stop viewfinder
    mViewfinderControl.stop();

    // stop video-recording in-case if its ongoing.
    stop();

    if (mCameraDevice.cameraSnapshot()) {
        mCameraDevice.cameraSnapshot()->StopSnapshot();
    }

    if (mVideoRecorder) {
        mVideoRecorder->Close();
    }

    // revert back the new filename to empty string so that we generate a new file name
    // when we init again
    mNewFileName = QString("");

    setState(Idle);

    OstTrace0( camerax_performance, DUP1_CXEVIDEOCAPTURECONTROLSYMBIAN_DEINIT, "msg: e_CX_VIDEO_CAPCONT_DEINIT 0" );

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Intializes VideoRecorder for recording.
*/
void CxeVideoCaptureControlSymbian::initVideoRecorder()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Idle) {
        // not valid state to start "open" operation
        return;
    }

    if(!mVideoRecorder) {
        // if video recorder is not created, do it now.
        createVideoRecorder();
    }

    // update current video quality details from icm
    CxeError::Id err = getVideoQualityDetails(mCurrentVideoDetails);

    if (!err) {
        // read videofile mime type
        QByteArray videoFileData =
                    mCurrentVideoDetails.mVideoFileMimeType.toLatin1();
        TPtrC8 videoFileMimeType(reinterpret_cast<const TUint8*>
                           (videoFileData.constData()), videoFileData.size());

        // read preferred supplier
        TPtrC16 supplier(reinterpret_cast<const TUint16*>
                           (mCurrentVideoDetails.mPreferredSupplier.utf16()));

        err = findVideoController(videoFileMimeType, supplier);

        if (!err) {
            // video recorder is ready to open video file for recording.
            setState(Initialized);
            open();
        }
    } else {
        err = CxeErrorHandlingSymbian::map(KErrNotReady);
    }

    if (err) {
        // In case of error
        emit videoPrepareComplete(err);
        deinit();
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Opens file for video recording.
*/
void CxeVideoCaptureControlSymbian::open()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Initialized) {
        // not valid state to start "open" operation
        return;
    }

    CxeError::Id err = CxeError::None;

    // generate video file name, if necessary
    if (mNewFileName.isEmpty()) {
        QStringList list = mCurrentVideoDetails.mVideoFileMimeType.split("/");
        QString fileExt(".");
        if (list.count() == 2) {
            fileExt = fileExt + list[1];
        }
        // Generate new filename and open the file for writing video data
        err = mFilenameGenerator.generateFilename(mNewFileName, fileExt);
        if (err == CxeError::None) {
            mCurrentFilename = mNewFileName;
        } else {
            // file name is not valid, re-initialize the value of current string
            // back to empty string
            mCurrentFilename = QString("");
        }
    }

    if (!err &&
        mVideoRecorder &&
        !mCurrentFilename.isEmpty()) {

        TPtrC16 fName(reinterpret_cast<const TUint16*>(mCurrentFilename.utf16()));
        CX_DEBUG(( "Next video file path: %s", mCurrentFilename.toAscii().constData() ));

        // read video codec mime type
        QByteArray videoCodecData =
                    mCurrentVideoDetails.mVideoCodecMimeType.toLatin1();
        TPtrC8 videoCodecMimeType(reinterpret_cast<const TUint8*>
                           (videoCodecData.constData()), videoCodecData.size());

        setState(CxeVideoCaptureControl::Preparing);

        TRAPD(openErr, mVideoRecorder->OpenFileL(fName,
                       mCameraDevice.camera()->Handle(),
                       mVideoControllerUid,
                       mVideoFormatUid,
                       videoCodecMimeType,
                       audioType(mCurrentVideoDetails.mAudioType)));

        err = CxeErrorHandlingSymbian::map(openErr);
    }
    if (err) {
        // error occured.
        deinit();
        emit videoPrepareComplete(err);
    }
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Prepare Video Recorder with necessary settings for video capture.
*/
TFourCC CxeVideoCaptureControlSymbian::audioType(const QString& str)
{
    CX_DEBUG_ENTER_FUNCTION();

    QByteArray audioType = str.toAscii();

    quint8 char1(' ');
    quint8 char2(' ');
    quint8 char3(' ');
    quint8 char4(' ');

    if (audioType.count() > 3) {
        char1 = audioType[0];
        char2 = audioType[1];
        char3 = audioType[2];

        if (audioType.count() == 4) {
            char4 = audioType[3];
        }
    }

    return TFourCC(char1, char2, char3, char4);
}


/*!
* Prepare Video Recorder with necessary settings for video capture.
*/
void CxeVideoCaptureControlSymbian::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Preparing) {
        // not valid state to continue prepare.
        return;
    }

    CX_DEBUG(("Video resoulution (%d,%d)", mCurrentVideoDetails.mWidth,
                                           mCurrentVideoDetails.mHeight));
    CX_DEBUG(("Video bitrate = %d)", mCurrentVideoDetails.mVideoBitRate));
    CX_DEBUG(("Video frame rate = %f)", mCurrentVideoDetails.mVideoFrameRate));

    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_PREPARE, "msg: e_CX_VIDCAPCONT_PREPARE 1");
    TSize frameSize;
    frameSize.SetSize(mCurrentVideoDetails.mWidth, mCurrentVideoDetails.mHeight);

    int muteSetting = 0; // audio enabled
    mSettings.get(CxeSettingIds::VIDEO_MUTE_SETTING, muteSetting);

    TRAPD(err,
              {
              mVideoRecorder->SetVideoFrameSizeL(frameSize);
              mVideoRecorder->SetVideoFrameRateL(mCurrentVideoDetails.mVideoFrameRate);
              mVideoRecorder->SetVideoBitRateL(mCurrentVideoDetails.mVideoBitRate);
              mVideoRecorder->SetAudioEnabledL(muteSetting == 0);
              // "No limit" value is handled in video recorder wrapper.
              mVideoRecorder->SetMaxClipSizeL(mCurrentVideoDetails.mMaximumSizeInBytes);
              }
         );

    if (!err) {
        // settings have been applied successfully, start to prepare
        mVideoRecorder->Prepare();
        // prepare snapshot
        err = prepareVideoSnapshot();
    }

    if (!err) {
        // prepare zoom only when there are no errors during prepare.
        emit prepareZoomForVideo();
    }
    // emit video prepare status
    emit videoPrepareComplete(CxeErrorHandlingSymbian::map(err));

    OstTrace0(camerax_performance, DUP1_CXEVIDEOCAPTURECONTROLSYMBIAN_PREPARE, "msg: e_CX_VIDCAPCONT_PREPARE 0");

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
Fetches video qualites details based on video quality setting.
Returns CxeError codes if any.
*/
CxeError::Id
CxeVideoCaptureControlSymbian::getVideoQualityDetails(CxeVideoDetails &videoInfo)
{
    CX_DEBUG_ENTER_FUNCTION();

    int videoQuality = 0;
    CxeError::Id err = CxeError::None;

    if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex) {
        err = mSettings.get(CxeSettingIds::VIDEO_QUALITY, videoQuality);

        bool validQuality = (videoQuality >= 0 &&
                             videoQuality < mIcmSupportedVideoResolutions.count());
        if (err == CxeError::None && validQuality) {
            // get video quality details
            videoInfo = mIcmSupportedVideoResolutions.at(videoQuality);
        } else {
            // not valid video quality
            err = CxeError::NotFound;
        }
    } else {
        // get secondary camera video quality index
        if (mIcmSupportedVideoResolutions.count() > 0) {
            videoInfo = mIcmSupportedVideoResolutions.at(videoQuality);
        } else {
            // not valid video quality
            err = CxeError::NotFound;
        }
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/**!
 Prepare snapshot
 Returns symbian error code.
 */
int CxeVideoCaptureControlSymbian::prepareVideoSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();

    CCamera::CCameraSnapshot *cameraSnapshot = mCameraDevice.cameraSnapshot();
    CX_ASSERT_ALWAYS(cameraSnapshot);

    int err = KErrNone;
    // Whether or not we have postcapture on, we need the snapshot for Thumbnail Manager.
    if (cameraSnapshot) {
        // Cancel active snapshot
        cameraSnapshot->StopSnapshot();

        // Prepare snapshot
        CCamera::TFormat snapFormat = CCamera::EFormatFbsBitmapColor16MU;
        TRAP(err, cameraSnapshot->PrepareSnapshotL(snapFormat,
                                                   getSnapshotSize(),
                                                   KMaintainAspectRatio));
        CX_DEBUG(("PrepareSnapshotL done, err=%d", err));
        // Start snapshot if no errors encountered.
        if (err == KErrNone) {
            CX_DEBUG(("Start video snapshot"));
            cameraSnapshot->StartSnapshot();
        }
    } else {
        // No snapshot interface available. Report error.
        // Assert above takes care of this, but keeping this as an option.
        err = KErrNotReady;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*!
* Returns snapshot size. Snapshot size is calculated based on the
* display resolution and current video aspect ratio.
*/
TSize CxeVideoCaptureControlSymbian::getSnapshotSize() const
{
    CX_DEBUG_ENTER_FUNCTION();

    TSize snapshotSize;

    QSize deviceResolution = mViewfinderControl.deviceDisplayResolution();
    QSize size = QSize(mCurrentVideoDetails.mWidth, mCurrentVideoDetails.mHeight);

    // scale according to aspect ratio.
    size.scale(deviceResolution.width(),
               deviceResolution.height(),
               Qt::KeepAspectRatio);
    CX_DEBUG(("Video Snapshot size, (%d,%d)", size.width(), size.height()));
    snapshotSize.SetSize(size.width(), deviceResolution.height());

    CX_DEBUG_EXIT_FUNCTION();

    return snapshotSize;
}



/**!
* Camera events coming from ecam.
*/
void CxeVideoCaptureControlSymbian::handleCameraEvent(int eventUid, int error)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (eventUid == KUidECamEventSnapshotUidValue) {
        handleSnapshotEvent(CxeErrorHandlingSymbian::map(error));
    }
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Handle Snapshot event from ecam
*/
void CxeVideoCaptureControlSymbian::handleSnapshotEvent(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Idle) {
        // we ignore this event, when we are not in active state(s)
        CX_DEBUG(( "wrong state, ignoring snapshot" ));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    if (error) {
        mSnapshot = QPixmap();
        emit snapshotReady(error, mSnapshot, filename());
        return;
    }

    RArray<TInt> snapList;
    MCameraBuffer* buffer(NULL);
    // Note: Cleanup not required in this function
    CFbsBitmap *snapshot = NULL;
    TRAPD(snapErr,
          buffer = &mCameraDevice.cameraSnapshot()->SnapshotDataL(snapList));
    if (!snapErr) {
        TInt firstImageIndex = snapList.Find(0);
        snapList.Close();
        TRAP(snapErr, snapshot = &(buffer->BitmapL(firstImageIndex)));
    }
    if (!snapErr) {
        TSize size = snapshot->SizeInPixels();
        TInt sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, EColor16MU)
                / sizeof(TUint32);

        TUint32 *snapshotData = NULL;
        snapshotData = new TUint32[sizeInWords];

        if (snapshotData) {
            // Convert to QImage
            snapshot->LockHeap();
            TUint32* dataPtr = snapshot->DataAddress();
            memcpy(snapshotData, dataPtr, sizeof(TUint32) * sizeInWords);
            snapshot->UnlockHeap();


            QImage *img = new QImage((uchar*) snapshotData, size.iWidth, size.iHeight,
                                   CFbsBitmap::ScanLineLength(size.iWidth, EColor16MU),
                                   QImage::Format_RGB32);

            mSnapshot = QPixmap::fromImage(*img);
            delete [] snapshotData;
            delete img;
        }
    }
    if (buffer) {
        buffer->Release();
    }
    // Snapshot ready
    emit snapshotReady(CxeErrorHandlingSymbian::map(snapErr), mSnapshot, filename());

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Resets the video snapshot and current video filename
*/
void CxeVideoCaptureControlSymbian::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Snapshot will consume considerably memory.
    // Replace it with null pixmap to have it freed.
    mSnapshot = QPixmap();
    // reset the current file name.
    mCurrentFilename = QString("");

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Returns current video filename
*/
QString CxeVideoCaptureControlSymbian::filename() const
{
    // Simply return the current contents of mCurrentFilename.
    // If video recording was started then it returns proper filename
    // otherwise an empty string is returned.
    return mCurrentFilename;
}


/*!
* Returns current video snapshot
*/
QPixmap CxeVideoCaptureControlSymbian::snapshot() const
{
    return mSnapshot;
}


/*!
* Starts video recording if we are in appropriate state.
*/
void CxeVideoCaptureControlSymbian::record()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Ready || state() == Paused) {
        // ask the player to play the sound
        // recording will start once start sound is played
        setState(CxeVideoCaptureControl::PlayingStartSound);
        mVideoStartSoundPlayer->play();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Pauses video recording.
*/
void CxeVideoCaptureControlSymbian::pause()
{
    CX_DEBUG_ENTER_FUNCTION();

    setState(CxeVideoCaptureControl::Paused);
    TRAPD(pauseErr, mVideoRecorder->PauseL());
    if (pauseErr) {
        CX_DEBUG(("[WARNING] Error %d pausing!", pauseErr));
        //pause operation failed, report it
        emit videoComposed(CxeErrorHandlingSymbian::map(pauseErr), filename());
        // release resources.
        deinit();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Stops video recording.
*/
void CxeVideoCaptureControlSymbian::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Recording || state() == Paused) {
        // first stop viewfinder
        mViewfinderControl.stop();

        TInt asyncStopErr = mVideoRecorder->CustomCommandSync(
            TMMFMessageDestination(KCamCControllerImplementationUid,
                                   KMMFObjectHandleController),
            ECamCControllerCCVideoStopAsync,
            KNullDesC8,
            KNullDesC8 );
        CX_DEBUG(("ECamCControllerCCVideoStopAsync sent, err=%d", asyncStopErr));
        if (asyncStopErr) { // fall back to synchronous stop
            TInt syncStopErr = mVideoRecorder->Stop();
            if (syncStopErr) {
                // error during stop operation, release resources
                emit videoComposed(CxeErrorHandlingSymbian::map(asyncStopErr), filename());
                deinit();
            } else {
                // stop operation went fine, set back the state to Initialized.
                setState(Initialized);
                mFilenameGenerator.raiseCounterValue();
            }
        } else {
            // No error from asynch stop custom command... wait for stop event
            setState(Stopping);
            mFilenameGenerator.raiseCounterValue();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Callback when "Open" operation is complete.
*/
void CxeVideoCaptureControlSymbian::MvruoOpenComplete(TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeVideoCaptureControlSymbian::MvruoOpenComplete, err=%d", aError));

    if (state() == Preparing) {
        if (aError != KErrNone) {
            deinit();
            CxeError::Id err = CxeErrorHandlingSymbian::map(KErrNotReady);
            // report error to interested parties
            emit videoPrepareComplete(CxeErrorHandlingSymbian::map(err));
        } else {
            prepare();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Callback when "Prepare" request is complete.
*/
void CxeVideoCaptureControlSymbian::MvruoPrepareComplete(TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeVideoCaptureControlSymbian::MvruoPrepareComplete, err=%d", aError));

    if (state() == Preparing) {
        if (!aError) {
            setState(CxeVideoCaptureControl::Ready);
            mViewfinderControl.start();
        } else {
            deinit();
            // report error to interested parties
            CxeError::Id err = CxeErrorHandlingSymbian::map(KErrNotReady);
            emit videoPrepareComplete(CxeErrorHandlingSymbian::map(err));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Callback when "Record" operation is complete.
*/
void CxeVideoCaptureControlSymbian::MvruoRecordComplete(TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeVideoCaptureControlSymbian::MvruoRecordComplete, err=%d", aError));

    //! async stop customcommand stuff
    if (aError == KErrNone) {
        setState(CxeVideoCaptureControl::Ready);
    } else if (aError == KErrCompletion) {
        // KErrCompletion is received when video recording stops
        // because of maximum clip size is reached. Emulate
        // normal stopping.
        setState(Stopping);
        MvruoEvent(TMMFEvent(KCamCControllerCCVideoRecordStopped, KErrNone));
        MvruoEvent(TMMFEvent(KCamCControllerCCVideoFileComposed, KErrNone));
    }
    else {
        // error during recording, report to client
        deinit();
        emit videoComposed(CxeErrorHandlingSymbian::map(aError), filename());
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Callback from MVideoRecorderUtilityObserver
*/
void CxeVideoCaptureControlSymbian::MvruoEvent(const TMMFEvent& aEvent)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (aEvent.iEventType.iUid == KCamCControllerCCVideoRecordStopped.iUid) {
        CX_DEBUG(("KCamCControllerCCVideoRecordStopped"));
        // play the sound, but not changing the state
        mVideoStopSoundPlayer->play();
    } else if (aEvent.iEventType.iUid == KCamCControllerCCVideoFileComposed.iUid) {
        CX_DEBUG(("KCamCControllerCCVideoFileComposed"));
        if (state() == Stopping) {
            // stop operation went fine, set back the state to intialized.
            setState(Initialized);
        }
        // video file has composed, everything went well, inform the client
        emit videoComposed(CxeError::None, filename());
        // revert back the new filename to empty string, since recording
        // is complete and we need to generate a new file name
        mNewFileName = QString("");
    } else {
        CX_DEBUG(("ignoring unknown MvruoEvent 0x%08x", aEvent.iEventType.iUid ));
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
Get corresponding controller for video capture.
@param aMimeType denotes videofile mimetype,
@param aPreferredSupplier denotes supplier.
Returns CxeError::Id if any.
*/
CxeError::Id
CxeVideoCaptureControlSymbian::findVideoController(const TDesC8& aMimeType,
                                                   const TDesC& aSupplier)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("video file mime type : %s", &aMimeType));
    CX_DEBUG(("supplier name: %s", &aSupplier));

    mVideoControllerUid.iUid = 0;
    mVideoFormatUid.iUid = 0;

    // Retrieve a list of possible controllers from ECOM.
    // Controller must support recording the requested mime type.
    // Controller must be provided by preferred supplier.

    CMMFControllerPluginSelectionParameters* cSelect(NULL);
    CMMFFormatSelectionParameters* fSelect(NULL);
    RMMFControllerImplInfoArray controllers;

    TRAPD(err, cSelect = CMMFControllerPluginSelectionParameters::NewL());
    if (err) {
        return CxeErrorHandlingSymbian::map(err);
    }

    TRAP(err, fSelect = CMMFFormatSelectionParameters::NewL());
    if (err) {
        if(cSelect) {
            delete cSelect;
        }
        return CxeErrorHandlingSymbian::map(err);
    }

    TRAP( err, {
        fSelect->SetMatchToMimeTypeL(aMimeType);
        cSelect->SetRequiredRecordFormatSupportL(*fSelect);
        cSelect->SetPreferredSupplierL(aSupplier,
                                       CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);
        cSelect->ListImplementationsL(controllers);
    } );

    if (!err && controllers.Count() >= 1) {
        // KErrNotFound is returned unless a controller is found
        err = KErrNotFound;
        // Get the controller UID.
        mVideoControllerUid = controllers[0]->Uid();

        // Inquires the controller about supported formats.
        // We use the first controller found having index 0.
        RMMFFormatImplInfoArray formats;
        formats = controllers[0]->RecordFormats();

        // Get the first format that supports our mime type.
        int count = formats.Count();
        for (int i=0; i<count; i++) {
            if (formats[i]->SupportsMimeType(aMimeType)) {
                mVideoFormatUid = formats[i]->Uid(); // set the UID
                err = KErrNone;
                break;
            }
        }
    }
    delete cSelect;
    delete fSelect;
    controllers.ResetAndDestroy();

    CX_DEBUG_EXIT_FUNCTION();

    return CxeErrorHandlingSymbian::map(err);
}


/*!
* camera reference changing, release resources
*/
void CxeVideoCaptureControlSymbian::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();
    releaseResources();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* prepare for camera release.
*/
void CxeVideoCaptureControlSymbian::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* new camera available,
*/
void CxeVideoCaptureControlSymbian::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!error) {
        // initialize the video recorder utility
        createVideoRecorder();
        // new camera available, read supported video qualities from icm
        // load all still qualities supported by icm
        mIcmSupportedVideoResolutions.clear();
        Cxe::CameraIndex cameraIndex = mCameraDeviceControl.cameraIndex();
        // get list of supported image qualities based on camera index
        mIcmSupportedVideoResolutions =
                mQualityPresets.videoQualityPresets(cameraIndex);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Initializes video recorder.
*/
void CxeVideoCaptureControlSymbian::createVideoRecorder()
{
    CX_DEBUG_ENTER_FUNCTION();

    // init video recoder
    if (state() == CxeVideoCaptureControl::Idle) {
        if (mVideoRecorder == NULL) {
            TRAPD(initErr, mVideoRecorder =
                new CxeVideoRecorderUtilitySymbian( *this ,
                            KAudioPriorityVideoRecording,
                            TMdaPriorityPreference( KAudioPrefVideoRecording )));
            if (initErr) {
                CX_DEBUG(("WARNING - VideoRecorderUtility could not be reserved. Failed with err:%d", initErr));
                mVideoRecorder = NULL;
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* releases resources used by videocapture
*/
void CxeVideoCaptureControlSymbian::releaseResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Save the state and filename before releasing.
    QString filenameCopy(filename());
    CxeVideoCaptureControl::State stateCopy(state());

    // first de-init videocapture control
    deinit();
    reset();

    // Check if state is stopping, in which case we have to inform the
    // file harvester that a file is to be completed. We would not
    // call harvestFile otherwise in this case.
    // Otherwise the video will not be found from videos app.
    if (stateCopy == CxeVideoCaptureControl::Stopping) {
        emit videoComposed(CxeError::None, filenameCopy);
    }

    delete mVideoRecorder;
    mVideoRecorder = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
Returns current state of videocapture
*/
CxeVideoCaptureControl::State CxeVideoCaptureControlSymbian::state() const
{
    return static_cast<CxeVideoCaptureControl::State> (stateId());
}


/*!
* slot called when state is changed.
*/
void CxeVideoCaptureControlSymbian::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State> (newStateId), error);
}


/*!
* Initialize states for videocapturecontrol
*/
void CxeVideoCaptureControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState(Idle, "Idle", Initialized));
    addState(new CxeState(Initialized, "Initialized", Preparing | Idle));
    addState(new CxeState(Preparing, "Preparing", Ready | Idle));
    addState(new CxeState(Ready, "Ready", Recording | PlayingStartSound | Preparing | Idle));
    addState(new CxeState(Recording, "Recording", Recording | Paused | Stopping | Idle));
    addState(new CxeState(Paused, "Paused", Recording | Stopping | PlayingStartSound | Idle));
    addState(new CxeState(Stopping, "Stopping", Initialized | Idle));
    addState(new CxeState(PlayingStartSound, "PlayingStartSound", Recording | Idle));

    setInitialState(Idle);
}


/*!
* Updates remaining video recordng time counter to all the video qualities supported by ICM
* this should be done whenever storage location setting changes and when values are
* read from ICM for the first time
*/
void CxeVideoCaptureControlSymbian::updateRemainingRecordingTimeCounter()
{
    CX_DEBUG_ENTER_FUNCTION();

    for( int index = 0; index < mIcmSupportedVideoResolutions.count(); index++) {
        CxeVideoDetails &qualityDetails = mIcmSupportedVideoResolutions[index];
        calculateRemainingTime(qualityDetails, qualityDetails.mRemainingTime);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* calculates remaining video recording time.
*/
void CxeVideoCaptureControlSymbian::remainingTime(int &time)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == CxeVideoCaptureControl::Recording) {
        TTimeIntervalMicroSeconds remaining = 0;
        remaining = mVideoRecorder->RecordTimeAvailable();
        time = remaining.Int64() * 1.0 / KOneSecond;
        CX_DEBUG(( "timeRemaining2: %d", time ));
    } else {
        calculateRemainingTime(mCurrentVideoDetails, time);
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* algorithm to calculate remaining recording time
@ param videoDetails contains the current video resolution that is in use.
@ time contains the remaining recording time
*/
void CxeVideoCaptureControlSymbian::calculateRemainingTime(CxeVideoDetails videoDetails, int &time)
{
    CX_DEBUG_ENTER_FUNCTION();

    TTimeIntervalMicroSeconds remaining = 0;

    // get available space in the drive selected in the settings
    // for storing videos
    qint64 availableSpace = CxeSysUtil::spaceAvailable(CCoeEnv::Static()->FsSession(), mSettings);

    availableSpace = availableSpace - KMinRequiredSpaceVideo;

    // Maximum clip size may be limited for mms quality.
    // If mMaximumSizeInBytes == 0, no limit is specified.
    if (videoDetails.mMaximumSizeInBytes > 0 && videoDetails.mMaximumSizeInBytes < availableSpace) {
        availableSpace = videoDetails.mMaximumSizeInBytes;
    }

    // Use average audio/video bitrates to estimate remaining time
    quint32  averageBitRate = 0;
    quint32  averageByteRate = 0;
    qreal    scaler = mQualityPresets.avgVideoBitRateScaler();

    if (scaler == 0) {
        // video bit rate scaler is 0, use the constant value
        scaler = KCMRAvgVideoBitRateScaler;
    }

    int avgVideoBitRate = (videoDetails.mVideoBitRate * scaler);
    int avgAudioBitRate =  videoDetails.mAudioBitRate;

    int muteSetting = 0; // audio enabled
    mSettings.get(CxeSettingIds::VIDEO_MUTE_SETTING, muteSetting);

    if (muteSetting == 1) {
        // audio disabled from setting. hence no audio
        avgAudioBitRate = 0;
    }

    averageBitRate = (quint32)(
                     (avgVideoBitRate + avgAudioBitRate) * KMetaDataCoeff);

    averageByteRate = averageBitRate / 8;

    if (availableSpace <= 0) {
        remaining = 0;
    } else {
        // converting microseconds to seconds
        remaining = availableSpace * KOneMillion / averageByteRate;
        if ( (remaining.Int64()) > (quint64(KCamCMaxClipDurationInSecs) * KOneMillion) ) {
            remaining = (quint64(KCamCMaxClipDurationInSecs) * KOneMillion);
        }
    }
    if ( remaining <= quint64(0) ) {
        remaining = 0;
    }

    time = remaining.Int64() * 1.0 / KOneSecond;

    CX_DEBUG(( "remaining time from algorithm: %d", time ));

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Calculates remaining recording time during video recording
*/
bool CxeVideoCaptureControlSymbian::elapsedTime(int &time)
{
    CX_DEBUG_ENTER_FUNCTION();

    TTimeIntervalMicroSeconds timeElapsed = 0;
    bool ok = false;
    if (state() == CxeVideoCaptureControl::Recording) {
        TRAPD( err, timeElapsed = mVideoRecorder->DurationL() );
        if (!err) {
            time = timeElapsed.Int64() * 1.0 / KOneSecond;
            CX_DEBUG(("timeElapsed2: %d", time));
            ok = true;
        }
    }

    CX_DEBUG_EXIT_FUNCTION();

    return ok;
}

/*!
* slot called when playing a sound has finished.
*/
void CxeVideoCaptureControlSymbian::handleSoundPlayed()
{
    CX_DEBUG_ENTER_FUNCTION();

    // start recording, if we were playing capture sound
    if (state() == CxeVideoCaptureControl::PlayingStartSound) {
        setState(CxeVideoCaptureControl::Recording);
        mVideoRecorder->Record();
    }

    // in case of video capture stop sound playing, nothing needs to be done
    // meaning the state set elsewhere, and the video capture has been stopped already

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* setting has changed, check if we are interested.
*/
void CxeVideoCaptureControlSymbian::handleSettingValueChanged(const QString& settingId,
                                                              QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(newValue);

    if (mCameraDeviceControl.mode() == Cxe::VideoMode) {
        if (settingId == CxeSettingIds::VIDEO_QUALITY) {
            // re-prepare for video
            if (state() == Ready) {
                // release resources
         	    deinit();
                // initialize video recording again
           	    init();
           	}
        } else if (settingId == CxeSettingIds::VIDEO_MUTE_SETTING) {
            // mute setting changed, apply the new setting and re-prepare.
            setState(Preparing);
            prepare();
        } else {
            // Setting not relevant to video mode
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Returns QList of all supported video quality details based on the camera index
* (primary/secondary).
*/
QList<CxeVideoDetails> CxeVideoCaptureControlSymbian::supportedVideoQualities()
{
    // update the remaining time counters for each quality setting
    updateRemainingRecordingTimeCounter();
    return mIcmSupportedVideoResolutions;
}

// End of file
