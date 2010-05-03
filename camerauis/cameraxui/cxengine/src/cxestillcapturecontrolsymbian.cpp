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


#include <algorithm>
#include <exception>
#include <fbs.h>
#include <QPixmap>
#include <coemain.h>
#include <ECamOrientationCustomInterface2.h>
#include <ecam/camerasnapshot.h>
#include <ecamfacetrackingcustomapi.h>

#include "cxestillcapturecontrolsymbian.h"
#include "cxeimagedataqueuesymbian.h"
#include "cxefilenamegenerator.h"
#include "cxefilesavethread.h"
#include "cxutils.h"
#include "cxecameradevicecontrol.h"
#include "cxecameradevice.h"
#include "cxesoundplayersymbian.h"
#include "cxestillimagesymbian.h"
#include "cxeviewfindercontrol.h"
#include "cxeviewfindercontrolsymbian.h"
#include "cxesettingsmappersymbian.h"
#include "cxestate.h"
#include "cxesettings.h"
#include "cxeerrormappingsymbian.h"
#include "cxeautofocuscontrol.h"
#include "cxesensoreventhandler.h"
#include "cxesensoreventhandler.h"
#include "cxequalitypresetssymbian.h"
#include "cxediskmonitor.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxestillcapturecontrolsymbianTraces.h"
#endif


// constants
const int KMaintainAspectRatio = false;
const TInt64 KMinRequiredSpaceImage = 2000000;


/**
 * Constructor.
 */
CxeStillCaptureControlSymbian::CxeStillCaptureControlSymbian(
    CxeCameraDevice &cameraDevice,
    CxeViewfinderControl &viewfinderControl,
    CxeCameraDeviceControl &cameraDeviceControl,
    CxeFilenameGenerator &nameGenerator,
    CxeSensorEventHandler &sensorEventHandler,
    CxeAutoFocusControl &autoFocusControl,
    CxeSettings &settings,
    CxeQualityPresets &qualityPresets,
    CxeFileSaveThread &fileSaveThread,
    CxeDiskMonitor &diskMonitor)
    : CxeStateMachine("CxeStillCaptureControlSymbian"),
      mCameraDevice(cameraDevice),
      mViewfinderControl(viewfinderControl),
      mCameraDeviceControl(cameraDeviceControl),
      mFilenameGenerator(nameGenerator),
      mSensorEventHandler(sensorEventHandler),
      mAutoFocusControl(autoFocusControl),
      mSettings(settings),
      mQualityPresets(qualityPresets),
      mFileSaveThread(fileSaveThread),
      mDiskMonitor(diskMonitor),
      mMode(SingleImageCapture),
      mAfState(CxeAutoFocusControl::Unknown)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_IN, "msg: e_CX_STILLCAPTURECONTROL_NEW 1");

    qRegisterMetaType<CxeStillCaptureControl::State>();
    initializeStates();
    reset();

    // If camera is already allocated, call the slot ourselves.
    if (mCameraDevice.camera()) {
        handleCameraAllocated(CxeError::None);
    }

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_MID1, "msg: e_CX_ENGINE_CONNECT_SIGNALS 1");
    // connect signals from cameraDevice to recieve events when camera reference changes
    connect(&cameraDevice, SIGNAL(prepareForCameraDelete()),
            this, SLOT(prepareForCameraDelete()));
    connect(&cameraDevice, SIGNAL(prepareForRelease()),
            this, SLOT(prepareForRelease()));
    connect(&cameraDevice, SIGNAL(cameraAllocated(CxeError::Id)),
            this, SLOT(handleCameraAllocated(CxeError::Id)));
    connect(&mSensorEventHandler,
            SIGNAL(sensorEvent(CxeSensorEventHandler::SensorType,QVariant)),
            this, SLOT(handleSensorEvent(CxeSensorEventHandler::SensorType,QVariant)));

    // enabling setting change callbacks to stillcapturecontrol
    connect(&mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&,QVariant)));

    // Connect ECam image buffer ready event
    connect(&mCameraDeviceControl, SIGNAL(imageBufferReady(MCameraBuffer*,int)),
            this, SLOT(handleImageData(MCameraBuffer*,int)));
    // Connect signals for ECam events
    connect(&mCameraDeviceControl, SIGNAL(cameraEvent(int,int)),
            this, SLOT(handleCameraEvent(int,int)));

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_MID2, "msg: e_CX_ENGINE_CONNECT_SIGNALS 0");

    mImageDataQueue = new CxeImageDataQueueSymbian();
    mAutoFocusSoundPlayer = new CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::AutoFocus);
    mCaptureSoundPlayer = new CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::StillCapture);

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_OUT, "msg: e_CX_STILLCAPTURECONTROL_NEW 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Destructor.
 */
CxeStillCaptureControlSymbian::~CxeStillCaptureControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    deinit();
    reset();
    mIcmSupportedImageResolutions.clear();
    mECamSupportedImageResolutions.clear();
    delete mImageDataQueue;
    delete mCaptureSoundPlayer;
    delete mAutoFocusSoundPlayer;

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Return the current state.
 */
CxeStillCaptureControl::State CxeStillCaptureControlSymbian::state() const
{
    return static_cast<CxeStillCaptureControl::State>( stateId() );
}

/**
 * Handle state changed event. Normally just emits the signal
 * for observers to react appropriately.
 */
void CxeStillCaptureControlSymbian::handleStateChanged( int newStateId, CxeError::Id error )
{
    emit stateChanged( static_cast<State>( newStateId ), error );
}

/**
 * Initialize the control states.
 */
void CxeStillCaptureControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState( Uninitialized , "Uninitialized", Ready));
    addState(new CxeState( Ready , "Ready", Uninitialized | Capturing));
    addState(new CxeState( Capturing , "Capturing", Uninitialized | Ready));

    setInitialState(Uninitialized);
}

/**
 * Initialize the still image capture control.
 */
void CxeStillCaptureControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_INIT_IN, "msg: e_CX_STILL_CAPCONT_INIT 1");

    if (state() == Uninitialized) {
        prepare();
        // Initialize orientation sensor and other sensors
        mSensorEventHandler.init();
    }

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_INIT_OUT, "msg: e_CX_STILL_CAPCONT_INIT 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Un-initialize the image mode.
 */
void CxeStillCaptureControlSymbian::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Uninitialized) {
        // nothing to do
        return;
    }

    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_DEINIT_IN, "msg: e_CX_STILL_CAPCONT_DEINIT 1" );

    // Stop monitoring disk space.
    mDiskMonitor.stop();
    disconnect(&mDiskMonitor, SIGNAL(diskSpaceChanged()), this, SLOT(handleDiskSpaceChanged()));

    //stop viewfinder
    mViewfinderControl.stop();

    // disable sensor event handler.
    mSensorEventHandler.deinit();

    if (mCameraDevice.cameraSnapshot()) {
        mCameraDevice.cameraSnapshot()->StopSnapshot();
    }

    if (state() == Capturing) {
        mCameraDevice.camera()->CancelCaptureImage();
    }
    setState(Uninitialized);

    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_DEINIT_OUT, "msg: e_CX_STILL_CAPCONT_DEINIT 0" );
    CX_DEBUG_EXIT_FUNCTION();
}


/**!
 * Prepare still capture mode.
 */
void CxeStillCaptureControlSymbian::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Uninitialized) {
        // wrong state and we return
        return;
    }

    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_PREPARE_IN, "msg: e_CX_STILLCAPCONT_PREPARE 1" );

    int err = KErrNone;
    CxeError::Id cxErr = getImageQualityDetails(mCurrentImageDetails);
    int ecamStillResolutionIndex = 0;

    if (cxErr == CxeError::None) {
        int imageWidth =  mCurrentImageDetails.mWidth;
        int imageHeight = mCurrentImageDetails.mHeight;
        CX_DEBUG(("CxeStillCaptureControlSymbian::prepare <> resolution = (%d, %d)", imageWidth, imageHeight));

        TSize imageSize;
        imageSize.SetSize(imageWidth, imageHeight);

        if (mECamSupportedImageResolutions.count() > 0) {
            ecamStillResolutionIndex = mECamSupportedImageResolutions.indexOf(imageSize);
        }

        if (ecamStillResolutionIndex < 0) {
            CX_DEBUG(("CxeStillCaptureControlSymbian::prepare - WARNING! resolution not supported, falling back to index 0"));
            ecamStillResolutionIndex = 0;
        }

        // Prepare Image capture
        CCamera::TFormat imgFormat = supportedStillFormat(mCameraDeviceControl.cameraIndex());
        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_PREPARE_MID1, "msg: e_CX_PREPARE_IMAGE_CAPTURE 1");
        TRAP(err, mCameraDevice.camera()->PrepareImageCaptureL(imgFormat, ecamStillResolutionIndex));
        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_PREPARE_MID2, "msg: e_CX_PREPARE_IMAGE_CAPTURE 0");

        CX_DEBUG(("PrepareImageCaptureL done, err=%d, resolution index = %d", err, ecamStillResolutionIndex));

        if (!err) {
            // still capture prepare went fine, try preparing snapshot
            err = prepareStillSnapshot();
        }
    } else {
        err = KErrNotFound;
    }

    if (!err) {
        // Start viewfinder before claiming to be ready,
        // as e.g. pending capture might be started by state change,
        // and viewfinder start might have problems with just started capturing.
        // If viewfinder is already running, this call does nothing.
        mViewfinderControl.start();

        // Start monitoring disk space.
        mDiskMonitor.start();
        connect(&mDiskMonitor, SIGNAL(diskSpaceChanged()), this, SLOT(handleDiskSpaceChanged()));


        // Enable AF reticule drawing by adaptation
        MCameraFaceTracking *faceTracking = mCameraDevice.faceTracking();
        if (faceTracking) {
            TRAP_IGNORE(faceTracking->EnableFaceIndicatorsL(ETrue));
        }

        // Still capture and still snapshot are OK.
        // We can safely set state to READY.
        setState(Ready);

        // inform zoom control to prepare zoom
        emit prepareZoomForStill(ecamStillResolutionIndex);
    } else {
        CX_DEBUG(("Image Prepare FAILED! symbian error = %d", err));
        // release resources
        deinit();
    }

    // Inform interested parties that image mode has been prepared for capture
    emit imagePrepareComplete(CxeErrorHandlingSymbian::map(err));

    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_PREPARE_OUT, "msg: e_CX_STILLCAPCONT_PREPARE 0" );

    CX_DEBUG_EXIT_FUNCTION();
}



/**!
 Prepare still snapshot
 Returns symbian error code.
 */
int CxeStillCaptureControlSymbian::prepareStillSnapshot()
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
        OstTrace0( camerax_performance, DUP4_CXESTILLCAPTURECONTROLSYMBIAN_PREPARE, "msg: e_CX_PREPARE_SNAPSHOT 1" );
        TRAP(err, cameraSnapshot->PrepareSnapshotL(snapFormat,
                                                   getSnapshotSize(),
                                                   KMaintainAspectRatio));
        OstTrace0( camerax_performance, DUP5_CXESTILLCAPTURECONTROLSYMBIAN_PREPARE, "msg: e_CX_PREPARE_SNAPSHOT 0" );
        CX_DEBUG(("PrepareSnapshotL done, err=%d", err));

        // Start snapshot if no errors encountered.
        if (err == KErrNone) {
            CX_DEBUG(("Start still snapshot"));
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


/**!
 imageInfo contains image qualities details
 Returns CxeError error code.
 */
CxeError::Id CxeStillCaptureControlSymbian::getImageQualityDetails(CxeImageDetails &imageInfo)
{
    CX_DEBUG_ENTER_FUNCTION();

    int imageQuality = 0;
    CxeError::Id err = CxeError::None;
    if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex) {
        err = mSettings.get(CxeSettingIds::IMAGE_QUALITY, imageQuality);

        bool validQuality = (imageQuality >= 0 && imageQuality < mIcmSupportedImageResolutions.count());

        if (err == CxeError::None && validQuality ) {
            // get image quality details
            imageInfo = mIcmSupportedImageResolutions.at(imageQuality);
        } else {
            err = CxeError::NotFound;
            CX_DEBUG(("Invalid ImageQuality = %d", imageQuality));
        }
    } else {
        // we are in secondary camera
        // get secondary camera image quality details
       imageInfo = mIcmSupportedImageResolutions.at(imageQuality);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}


/*!
* Returns snapshot size. Snapshot size is calculated based on the
* display resolution and current image aspect ratio.
*/
TSize CxeStillCaptureControlSymbian::getSnapshotSize() const
{
    CX_DEBUG_ENTER_FUNCTION();

    TSize snapshotSize;

    QSize deviceResolution = mViewfinderControl.deviceDisplayResolution();
    QSize size = QSize(mCurrentImageDetails.mWidth, mCurrentImageDetails.mHeight);

    // scale according to aspect ratio.
    size.scale(deviceResolution.width(), deviceResolution.height(), Qt::KeepAspectRatio);
    CX_DEBUG(("Still Snapshot size, (%d,%d)", size.width(), size.height()));
    snapshotSize.SetSize(size.width(), deviceResolution.height());

    CX_DEBUG_EXIT_FUNCTION();

    return snapshotSize;
}


/**
 * Command to start image capture now.
 */
void CxeStillCaptureControlSymbian::capture()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.camera());

    // Start the image capture as fast as possible to minimize lag.
    // Check e.g. space available *after* this.
    mCameraDevice.camera()->CaptureImage();

    if (imagesLeft() > 0) {
        mCaptureSoundPlayer->play();
        setState(Capturing);

        //! @todo: NOTE: This call may not stay here. It can move depending on the implementation for burst capture.
        if (mMode == BurstCapture) {
            // Start a new filename sequence
            mFilenameGenerator.startNewImageFilenameSequence();
        }
    } else {
        // There's no space for the image.
        // Cancel started capturing.
        mCameraDevice.camera()->CancelCaptureImage();

        // Report error.
        // Ui notification has anyway some delays, hence handling VF after this.
        emit imageCaptured(CxeError::DiskFull, CxeStillImage::INVALID_ID);

        // Capturing stops viewfinder, so restart it now.
        mViewfinderControl.stop();
        mViewfinderControl.start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/* !
@Param cameraIndex indicates which camera we are in use, primary/secondary
Returns the format we use for specific camera index.
*/
CCamera::TFormat CxeStillCaptureControlSymbian::supportedStillFormat(Cxe::CameraIndex cameraIndex)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(cameraIndex);

    // The same image format used for both primary and secodary cameras
    CCamera::TFormat imgFormat = CCamera::EFormatExif;

    CX_DEBUG_EXIT_FUNCTION();

    return imgFormat;
}


/**
 * Camera events. Only relevant one(s) are handled.
 */
void CxeStillCaptureControlSymbian::handleCameraEvent(int eventUid, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (eventUid == KUidECamEventSnapshotUidValue &&
        mCameraDeviceControl.mode() == Cxe::ImageMode) {
        handleSnapshotEvent(CxeErrorHandlingSymbian::map(error));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Snapshot ready notification. Ask the snapshot from snapshot interface.
 * NB: Typically snapshot arrives before image data but can be in reverse
 * order as well.
 */
void CxeStillCaptureControlSymbian::handleSnapshotEvent(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == CxeStillCaptureControl::Uninitialized) {
        // we ignore this event, when we are not active
        return;
    }

    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_HANDLESNAPSHOTEVENT, "msg: e_CX_HANDLE_SNAPSHOT 1" );

    // Get image container for current snapshot index.
    // Remember to increment counter.
    CxeStillImageSymbian* stillImage = getImageForIndex(mNextSnapshotIndex++);

    if (error == CxeError::None) {
        try {
            stillImage->setSnapshot(extractSnapshot());
        } catch (const std::exception& ex) {
            error = CxeError::General;
        }
    }

    // Emit snapshotReady signal in all cases (error or not)
    emit snapshotReady(error, stillImage->snapshot(), stillImage->id());

    // When the snapshot ready event is handled, prepare new filename.
    if (stillImage->filename().isEmpty()) {
        // Error ignored at this point, try again when image data arrives.
        prepareFilename(stillImage);
    }

    OstTrace0( camerax_performance, DUP1_CXESTILLCAPTURECONTROLSYMBIAN_HANDLESNAPSHOTEVENT, "msg: e_CX_HANDLE_SNAPSHOT 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Gets QPixmap snapshot from ECAM buffer, if available.
* @param buffer ECAM buffer containing the snapshot data. Will be released when this
* method returns, even on exception.
*/
QPixmap CxeStillCaptureControlSymbian::extractSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();
    QPixmap pixmap;

    if (mCameraDevice.cameraSnapshot()) {

        QT_TRAP_THROWING({
            RArray<TInt> frameIndex;
            CleanupClosePushL(frameIndex);

            MCameraBuffer &buffer(mCameraDevice.cameraSnapshot()->SnapshotDataL(frameIndex));

            // Make sure buffer is released on leave / exception
            CxeCameraBufferCleanup cleaner(&buffer);
            TInt firstImageIndex(frameIndex.Find(0));
            CFbsBitmap &snapshot(buffer.BitmapL(firstImageIndex));

            CleanupStack::PopAndDestroy(); // frameIndex

            TSize size = snapshot.SizeInPixels();
            TInt sizeInWords = size.iHeight * CFbsBitmap::ScanLineLength(size.iWidth, EColor16MU) / sizeof(TUint32);
            CX_DEBUG(("size %d x %d, sizeInWords = %d", size.iWidth, size.iHeight, sizeInWords ));

            TUint32* pixelData = new (ELeave) TUint32[ sizeInWords ];
            // Convert to QImage
            snapshot.LockHeap();
            TUint32* dataPtr = snapshot.DataAddress();
            memcpy(pixelData, dataPtr, sizeof(TUint32)*sizeInWords);
            snapshot.UnlockHeap();

            CX_DEBUG(("Creating QImage"));
            QImage *snapImage = new QImage((uchar*)pixelData, size.iWidth, size.iHeight,
                                           CFbsBitmap::ScanLineLength(size.iWidth, EColor16MU),
                                           QImage::Format_RGB32);

            pixmap = QPixmap::fromImage(*snapImage);
            delete [] pixelData;
            delete snapImage;
        });
    }

    CX_DEBUG_EXIT_FUNCTION();
    return pixmap;
}

/**
 * handleImageData: Image data received from ECam
 */
void CxeStillCaptureControlSymbian::handleImageData(MCameraBuffer* cameraBuffer, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == CxeStillCaptureControl::Uninitialized) {
        // we ignore this event, when we are not active
        cameraBuffer->Release();
        return;
    }

    // Get image container for current image data index.
    // Remember to increment counter.
    CxeStillImageSymbian* stillImage = getImageForIndex(mNextImageDataIndex++);

    if (error) {
        // Indicate error in image capture to the UI.
        cameraBuffer->Release();
        emit imageCaptured(CxeErrorHandlingSymbian::map(error), stillImage->id());
        return;
    }

    // If filename is not available at this stage, then generate one now
    if (stillImage->filename().isEmpty()) {
        CxeError::Id cxErr = prepareFilename(stillImage);
        if (cxErr) {
            cameraBuffer->Release();
            emit imageCaptured(cxErr, stillImage->id());
            return;
        }
    }

    // Get the image data from the buffer
    TDesC8* data = NULL;
    TRAPD( symbErr, data = cameraBuffer->DataL(0) );
    CX_DEBUG(("dataError: %d, data: 0x%08x", symbErr, data));

    if (!data && !symbErr) {
        symbErr = KErrNoMemory;
    }

    // If data is available, initiate saving of image
    if (!symbErr) {
        //! @todo: this does a deep copy... we want to avoid this for performance/memory consumption reasons
        QByteArray byteArray( reinterpret_cast<const char*>( data->Ptr() ), data->Size() );
        data = NULL;

        // Save the image data
        CxeImageDataItemSymbian* dataItem = mImageDataQueue->startSave(byteArray, stillImage->filename(), stillImage->id());
        stillImage->setDataItem(dataItem);
        mFileSaveThread.save(dataItem); // Saving thread takes ownership of dataItem.
    }

    // ECam Camera buffer should always be released
    cameraBuffer->Release();

    // Inform interested parties about image capture
    emit imageCaptured(CxeErrorHandlingSymbian::map(symbErr), stillImage->id());

    // set state to ready, since capturing is complete
    setState(Ready);

    // image capture ready, before that we check if the orientation of the device changed during capture and if so, we set the new orientation
    setOrientation(mSensorEventHandler.sensorData(CxeSensorEventHandler::OrientationSensor));

    CX_DEBUG_EXIT_FUNCTION();
}


/**
 * Settings changed, needs updated
 */
void CxeStillCaptureControlSymbian::handleSettingValueChanged(const QString& settingId, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(newValue);

    if (settingId == CxeSettingIds::IMAGE_QUALITY) {
        // re-prepare for still
        if (state() == Ready) {
            deinit();
            init();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Disk space changed.
* Emit remaining images changed signal, if space change affects it.
*/
void CxeStillCaptureControlSymbian::handleDiskSpaceChanged()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ignore updates on other states.
    if (state() == CxeStillCaptureControl::Ready) {

        int images = calculateRemainingImages(mCurrentImageDetails.mEstimatedSize);

        if (images != mCurrentImageDetails.mPossibleImages) {
            CX_DEBUG(("CxeStillCaptureControlSymbian - available images changed %d -> %d",
                      mCurrentImageDetails.mPossibleImages, images));

            mCurrentImageDetails.mPossibleImages = images;
            emit availableImagesChanged();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * ECam reference changing, release resources
 */
void CxeStillCaptureControlSymbian::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Camera being released. Cancel ongoing capture, if any.
 */
void CxeStillCaptureControlSymbian::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 *  New camera available,
 */
void CxeStillCaptureControlSymbian::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (error == CxeError::None) {
        // load all still resoultions supported by ecam
        mECamSupportedImageResolutions.clear();
        TCameraInfo cameraInfo;
        Cxe::CameraIndex cameraIndex = mCameraDeviceControl.cameraIndex();
        CCamera::TFormat imgFormat = supportedStillFormat(cameraIndex);
        mCameraDevice.camera()->CameraInfo(cameraInfo);

        for(int i = 0; i < cameraInfo.iNumImageSizesSupported; i++) {
            TSize size;
            mCameraDevice.camera()->EnumerateCaptureSizes(size, i, imgFormat);
            CX_DEBUG(("ECam supported resolution <> Size (%d): (%d,%d)", i, size.iWidth, size.iHeight));
            mECamSupportedImageResolutions.insert(i, size);
        }

        // load all still qualities supported by icm
        mIcmSupportedImageResolutions.clear();
        // get list of supported image qualities based on camera index
        mIcmSupportedImageResolutions = mQualityPresets.imageQualityPresets(cameraIndex);

        CX_DEBUG(("ECAM Supported Qualities count = %d", mECamSupportedImageResolutions.count()));
        CX_DEBUG(("ICM Supported Qualities count = %d", mIcmSupportedImageResolutions.count()));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Return number of images captured (during current capture operation only).
 */
int CxeStillCaptureControlSymbian::imageCount() const
{
    return mImages.count();
}

/**
 * Reset the image array.
 */
void CxeStillCaptureControlSymbian::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    qDeleteAll( mImages );
    mImages.clear();

    mNextSnapshotIndex = 0;
    mNextImageDataIndex = 0;

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * This should cancel any ongoing image captures.
 */
void CxeStillCaptureControlSymbian::cancelAll()
{
    mImageDataQueue->clear();
    reset();
}

/**
 * Sets the current capture mode: SingleImageCapture / BurstCapture.
 */
void CxeStillCaptureControlSymbian::setMode( CaptureMode mode )
{
    mMode = mode;
}

/**
 * Returns the current capture mode.
 */
CxeStillCaptureControl::CaptureMode CxeStillCaptureControlSymbian::mode() const
{
    return mMode;
}

/**
 * Operator [] - returns the indexed image from capture array.
 */
CxeStillImage &CxeStillCaptureControlSymbian::operator[]( int index )
{
    return *mImages[ index ];
}

/**
 * Getter for image data queue.
 */
CxeImageDataQueue &CxeStillCaptureControlSymbian::imageDataQueue()
{
    return *mImageDataQueue;
}

/**
 * Generates a filename and sets it in the still image object.
 * Skips the process if filename already copied exists in the object. This
 * behaviour is required in rare cases where image data arrives before snapshot.
 */
CxeError::Id
CxeStillCaptureControlSymbian::prepareFilename(CxeStillImageSymbian *stillImage)
{
    CxeError::Id err = CxeError::None;
    if (stillImage->filename().isEmpty()) {
        QString path;

        QString fileExt = mCurrentImageDetails.mImageFileExtension;

        if (mMode == SingleImageCapture) {
            err = mFilenameGenerator.generateFilename(path, fileExt);
        }
        else {
            err = mFilenameGenerator.nextImageFilenameInSequence(path, fileExt);
        }

        if (!err) {
            CX_DEBUG(( "Next image file path: %s", path.toAscii().constData() ));
            stillImage->setFilename(path);
        }
        else {
            //! @todo: Error ID can be returned by this function.
            // Also error can be detected from empty filename string.
            CX_DEBUG(("ERROR in filename generation. err:%d", err));
        }
    }
    return err;
}

/*!
* Helper method to set orientation data from the orientation sensor
*/
void CxeStillCaptureControlSymbian::setOrientation(QVariant sensorData)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCameraDevice.cameraOrientation() && state() == Ready) {
        Cxe::DeviceOrientation uiOrientation = qVariantValue<Cxe::DeviceOrientation >(sensorData);
        MCameraOrientation::TOrientation currentCameraOrientation = mCameraDevice.cameraOrientation()->Orientation();
        MCameraOrientation::TOrientation newCameraOrientation = CxeSettingsMapperSymbian::Map2CameraOrientation(uiOrientation);

        CX_DEBUG((("cameraindex: %d mMode: %d state(): %d mAfState: %d uiOrientation: %d "),mCameraDeviceControl.cameraIndex(),
                mMode, state(), mAfState, uiOrientation ));
        if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex &&
            mMode                              == SingleImageCapture &&
            uiOrientation                      != Cxe::OrientationNone &&
            currentCameraOrientation           != newCameraOrientation &&
           (mAfState                           == CxeAutoFocusControl::Unknown ||
            mAfState                           == CxeAutoFocusControl::Canceling)
        ) {
            CX_DEBUG(("Setting Orientation to adaptation"));
            TRAP_IGNORE(mCameraDevice.cameraOrientation()->SetOrientationL(newCameraOrientation));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Get the image container at given index or create a new one if needed.
 */
CxeStillImageSymbian* CxeStillCaptureControlSymbian::getImageForIndex(int index)
{
    CxeStillImageSymbian* image(NULL);

    if (mImages.count() <= index) {
        image = new CxeStillImageSymbian();
        mImages.append(image);
    } else {
        CX_DEBUG_ASSERT( mNextImageDataIndex >= 0 && index < mImages.count() );
        image = mImages[index];
    }
    return image;
}


/*!
* Slot to handle Autofocus events.
*/
void CxeStillCaptureControlSymbian::handleAutofocusStateChanged(
                                         CxeAutoFocusControl::State newState,
                                         CxeError::Id /*error*/ )
{
    CX_DEBUG_ENTER_FUNCTION();
    mAfState = newState;
    CxeAutoFocusControl::Mode mode = mAutoFocusControl.mode();

    // if focused and in correct mode, play sound
    if  (newState == CxeAutoFocusControl::Ready &&
         mode != CxeAutoFocusControl::Hyperfocal &&
         mode != CxeAutoFocusControl::Infinity) {
        mAutoFocusSoundPlayer->play();
    }
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Slot that sets orientation data emited from orientation sensor
*/
void CxeStillCaptureControlSymbian::handleSensorEvent(
                                CxeSensorEventHandler::SensorType type,
                                QVariant data)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (type == CxeSensorEventHandler::OrientationSensor) {
        setOrientation(data);
    } else {
        // nothing to do
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Returns supported image qualities based on the camera index
* (primary/secondary).
*/
QList<CxeImageDetails> CxeStillCaptureControlSymbian::supportedImageQualities()
{
    // update remaining images counter
    updateRemainingImagesCounter();

    return mIcmSupportedImageResolutions;
}


/*!
* Updates remaining images counter to all the image qualities supported by ICM
* this should be done whenever storage location setting changes and when values are
* read from ICM for the first time
*/
void CxeStillCaptureControlSymbian::updateRemainingImagesCounter()
{
    CX_DEBUG_ENTER_FUNCTION();

    for( int index = 0; index < mIcmSupportedImageResolutions.count(); index++) {
        CxeImageDetails &qualityDetails = mIcmSupportedImageResolutions[index];
        qualityDetails.mPossibleImages = calculateRemainingImages(qualityDetails.mEstimatedSize);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Returns the number of images left for the current image quality setting
*/
int CxeStillCaptureControlSymbian::imagesLeft()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCurrentImageDetails.mPossibleImages == CxeImageDetails::UNKNOWN) {
        mCurrentImageDetails.mPossibleImages = calculateRemainingImages(mCurrentImageDetails.mEstimatedSize);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return mCurrentImageDetails.mPossibleImages;
}



/*!
* CxeStillCaptureControlSymbian::calculateRemainingImages
@ param estimatedImagesize, the estimated size for image resolution
@ returns number of images remaining
*/
int
CxeStillCaptureControlSymbian::calculateRemainingImages(int estimatedImagesize)
{
    CX_DEBUG_ENTER_FUNCTION();

    qint64 space = mDiskMonitor.free() - KMinRequiredSpaceImage;
    int images = std::max(qint64(0), space / estimatedImagesize);

    CX_DEBUG_EXIT_FUNCTION();

    return images;
}

// end of file
