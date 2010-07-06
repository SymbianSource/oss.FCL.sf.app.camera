/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <xqserviceutil.h>

#include "cxeenginesymbian.h"
#include "cxecameradevicecontrolsymbian.h"
#include "cxestillcapturecontrolsymbian.h"
#include "cxevideocapturecontrolsymbian.h"
#include "cxesettingscontrolsymbian.h"
#include "cxeviewfindercontrolsymbian.h"
#include "cxesnapshotcontrol.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxeautofocuscontrolsymbian.h"
#include "cxezoomcontrolsymbian.h"
#include "cxequalitypresetssymbian.h"
#include "cxutils.h"
#include "cxesettingsimp.h"
#include "cxefeaturemanagerimp.h"
#include "cxesettingsmodelimp.h"
#include "cxesettingscenrepstore.h"
#include "cxesoundplayersymbian.h"
#include "cxesensoreventhandlersymbian.h"
#include "cxefilesavethread.h"
#include "cxecameradevice.h"
#include "cxememorymonitor.h"
#include "cxediskmonitor.h"
#include "cxegeotaggingtrail.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxeenginesymbianTraces.h"
#endif


//  Member Functions

CxeEngineSymbian::CxeEngineSymbian()
    : mCameraDeviceControl(NULL),
      mCameraDevice(NULL),
      mViewfinderControl(NULL),
      mSnapshotControl(NULL),
      mStillCaptureControl(NULL),
      mVideoCaptureControl(NULL),
      mSettingsControl(NULL),
      mAutoFocusControl(NULL),
      mZoomControl(NULL),
      mSettings(NULL),
      mFeatureManager(NULL),
      mSettingsModel(NULL),
      mFilenameGenerator(NULL),
      mSensorEventHandler(NULL),
      mQualityPresets(NULL),
      mFileSaveThread(NULL),
      mDiskMonitor(NULL),
      mMemoryMonitor(NULL),
      mGeoTaggingTrail(NULL)
{
    CX_DEBUG_IN_FUNCTION();
}


/*!
    Create all control classes and connect relevant signals
*/
void CxeEngineSymbian::construct()
{
    CX_DEBUG_ENTER_FUNCTION();

    createControls();
    connectSignals();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
    Create all control classes.
    Note: Throws an error, if free memory request by CxeMemoryMonitor constructor fails!
*/
void CxeEngineSymbian::createControls()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Check we do this only once.
    if (!mSettingsModel) {
        OstTrace0(camerax_performance, CXEENGINESYMBIAN_CREATECONTROLS_IN, "e_CX_ENGINE_CREATE_CONTROLS 1");

        CxeCameraDeviceControlSymbian *deviceControl = new CxeCameraDeviceControlSymbian();
        mCameraDeviceControl = deviceControl;
        mCameraDevice = deviceControl->cameraDevice();

        CX_ASSERT_ALWAYS(mCameraDeviceControl);
        CX_ASSERT_ALWAYS(mCameraDevice);

        mCameraDeviceControl->init();

        CxeSettingsCenRepStore *settingsStore(NULL);
        if (XQServiceUtil::isService()) {
            settingsStore = new CxeSettingsLocalStore();
        } else {
            settingsStore = new CxeSettingsCenRepStore();
        }
        //ownership of settings store transferred to the settings model.
        mSettingsModel = new CxeSettingsModelImp(settingsStore);
        // assert if settings model fails to intialize
        CX_DEBUG_ASSERT(mSettingsModel);

        mSettings = new CxeSettingsImp(*mSettingsModel);

        // Loading current camera mode value from settings store and updating 
		// devicecontrol        
        Cxe::CameraMode cameraMode = Cxe::ImageMode;
        int value;
        CxeError::Id err = mSettings->get(CxeSettingIds::CAMERA_MODE, value);
        if (!err) {
            cameraMode = static_cast<Cxe::CameraMode>(value);
        }
        // set current camera mode to devicecontrol.
        mCameraDeviceControl->setMode(cameraMode);
        
        //! @todo a temporary hack to change the startup sequence to avoid GOOM problems
        static_cast<CxeSettingsImp*>(mSettings)->loadSettings(mode());


        // Connect P&S key updates to settings signal.
        connect(settingsStore, SIGNAL(settingValueChanged(long int, unsigned long int, QVariant)),
                mSettings, SIGNAL(settingValueChanged(long int, unsigned long int, QVariant)));

        mFeatureManager = new CxeFeatureManagerImp(*mSettingsModel);

        // Memory monitor needed as early as possible to request free memory.
        // Note: Throws error if enough memory cannot be freed!
        mMemoryMonitor = new CxeMemoryMonitor(*mFeatureManager);

        mDiskMonitor = new CxeDiskMonitor(*mSettings);

        mQualityPresets = new CxeQualityPresetsSymbian(*mSettings);
        CX_DEBUG_ASSERT(mQualityPresets);

        // sensor event handler initialization
        mSensorEventHandler = new CxeSensorEventHandlerSymbian();

        mFilenameGenerator = new CxeFilenameGeneratorSymbian(*mSettings,
            mode());

        mViewfinderControl = new CxeViewfinderControlSymbian(*mCameraDevice,
            *mCameraDeviceControl);

        mSnapshotControl = new CxeSnapshotControl(*mCameraDevice);

        mAutoFocusControl = new CxeAutoFocusControlSymbian(*mCameraDevice, 
		                            *mSettings);

        mFileSaveThread = CxeFileSaveThreadFactory::createFileSaveThread();

        mStillCaptureControl = new CxeStillCaptureControlSymbian(
            *mCameraDevice, *mViewfinderControl, *mSnapshotControl, *mCameraDeviceControl,
            *mFilenameGenerator, *mSensorEventHandler, *mAutoFocusControl,
            *mSettings, *mQualityPresets, *mFileSaveThread, *mDiskMonitor);

        mZoomControl = new CxeZoomControlSymbian(
            *mCameraDevice, *mCameraDeviceControl, *mSettings, *mFeatureManager);

        mVideoCaptureControl = new CxeVideoCaptureControlSymbian(
            *mCameraDevice, *mViewfinderControl, *mSnapshotControl, *mCameraDeviceControl,
            *mFilenameGenerator, *mSettings, *mQualityPresets, *mDiskMonitor);

        mSettingsControl = new CxeSettingsControlSymbian(*mCameraDevice, *mSettings);

        mGeoTaggingTrail = new CxeGeoTaggingTrail(*mStillCaptureControl,
                                                  *mVideoCaptureControl,
                                                  *mSettings);

        OstTrace0(camerax_performance, CXEENGINESYMBIAN_CREATECONTROLS_OUT, "e_CX_ENGINE_CREATE_CONTROLS 0");
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Connect internal signals for control classes
*/
void CxeEngineSymbian::connectSignals()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEENGINESYMBIAN_CONNECTSIGNALS_IN, "e_CX_ENGINE_CONNECT_SIGNALS 1");

    // enabling scene setting change callbacks to Autofocus control
    connect(mSettings,
            SIGNAL(sceneChanged(CxeScene&)),
            mAutoFocusControl,
            SLOT(handleSceneChanged(CxeScene&)));

    // connecting Autofocus state change callbacks to stillcapturecontrol
    connect(mAutoFocusControl,
            SIGNAL(stateChanged(CxeAutoFocusControl::State, CxeError::Id)),
            mStillCaptureControl,
            SLOT(handleAutofocusStateChanged(CxeAutoFocusControl::State,CxeError::Id)));

    // Connect signals for ECam events
    connect(mCameraDeviceControl,
            SIGNAL(cameraEvent(int,int)),
            mSnapshotControl,
            SLOT(handleCameraEvent(int,int)));

    connect(mCameraDeviceControl,
            SIGNAL(cameraEvent(int,int)),
            mAutoFocusControl,
            SLOT(handleCameraEvent(int,int)));

    connect(mCameraDeviceControl,
            SIGNAL(cameraEvent(int,int)),
            mStillCaptureControl,
            SLOT(handleCameraEvent(int,int)));

    // Connect signal for device ready events
    connect(mCameraDeviceControl,
            SIGNAL(deviceReady()),
            this,
            SLOT(doInit()));

    // Connect image and video init complete signals to
    // CameraDeviceControl initModeComplete
    connect(mStillCaptureControl,
            SIGNAL(imagePrepareComplete(CxeError::Id)),
            mCameraDeviceControl,
            SIGNAL(initModeComplete(CxeError::Id)));

    connect(mVideoCaptureControl,
            SIGNAL(videoPrepareComplete(CxeError::Id)),
            mCameraDeviceControl,
            SIGNAL(initModeComplete(CxeError::Id)));

    // Zoom is prepared once the image/video emits prepare zoom signals
    connect(mStillCaptureControl,
            SIGNAL(prepareZoomForStill(int)),
            mZoomControl,
            SLOT(prepareZoomForStill(int)));

    connect(mVideoCaptureControl,
            SIGNAL(prepareZoomForVideo()),
            mZoomControl,
            SLOT(prepareZoomForVideo()));

    // Connect file saving thread to snapshot signals and video saved signal.
    // Image saving it handles internally.
    connect(mStillCaptureControl,
            SIGNAL(snapshotReady(CxeError::Id, const QImage&, int)),
            mFileSaveThread,
            SLOT(handleSnapshotReady(CxeError::Id, const QImage&, int)));
    connect(mVideoCaptureControl,
            SIGNAL(videoComposed(CxeError::Id, const QString&)),
            mFileSaveThread,
            SLOT(handleVideoSaved(CxeError::Id, const QString&)));
    connect(mVideoCaptureControl,
            SIGNAL(snapshotReady(CxeError::Id, const QImage&, const QString&)),
            mFileSaveThread,
            SLOT(handleSnapshotReady(CxeError::Id, const QImage&, const QString&)));


    // stop location trail when releasing camera.
    connect(mCameraDevice,
            SIGNAL(prepareForRelease()),
            mGeoTaggingTrail,
            SLOT(stop()),
            Qt::UniqueConnection);

    OstTrace0(camerax_performance, CXEENGINESYMBIAN_CONNECTSIGNALS_OUT, "e_CX_ENGINE_CONNECT_SIGNALS 0");

    CX_DEBUG_EXIT_FUNCTION();
}

CxeEngineSymbian::~CxeEngineSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    // Saving current camera mode to cenrep
    saveMode();
    
    delete mGeoTaggingTrail;
    delete mAutoFocusControl;
    delete mZoomControl;
    delete mSettingsControl;
    delete mStillCaptureControl;
    delete mVideoCaptureControl;
    delete mSnapshotControl;
    delete mViewfinderControl;
    delete mFilenameGenerator;
    delete mDiskMonitor;
    delete mMemoryMonitor;
    delete mFeatureManager;
    delete mSettings;
    delete mSettingsModel;
    delete mCameraDeviceControl;
    delete mQualityPresets;
    delete mFileSaveThread;

    CX_DEBUG_EXIT_FUNCTION();
}

CxeCameraDeviceControl &CxeEngineSymbian::cameraDeviceControl()
{
    return *mCameraDeviceControl;
}

CxeViewfinderControl &CxeEngineSymbian::viewfinderControl()
{
    return *mViewfinderControl;
}

CxeSnapshotControl &CxeEngineSymbian::snapshotControl()
{
    return *mSnapshotControl;
}

CxeStillCaptureControl &CxeEngineSymbian::stillCaptureControl()
{
    return *mStillCaptureControl;
}

CxeVideoCaptureControl &CxeEngineSymbian::videoCaptureControl()
{
    return *mVideoCaptureControl;
}

CxeAutoFocusControl &CxeEngineSymbian::autoFocusControl()
{
    return *mAutoFocusControl;
}

CxeZoomControl &CxeEngineSymbian::zoomControl()
{
    return *mZoomControl;
}

/*!
Returns the settings handle
*/
CxeSettings &CxeEngineSymbian::settings()
{
    return *mSettings;
}

/*! 
Returns the sensor event  handle
*/
CxeSensorEventHandler &CxeEngineSymbian::sensorEventHandler()
{
    return *mSensorEventHandler;
}


CxeFeatureManager &CxeEngineSymbian::featureManager()
{
    return *mFeatureManager;
}

/*!
* Returns memory monitor utility handle.
*/
CxeMemoryMonitor &CxeEngineSymbian::memoryMonitor()
{
    return *mMemoryMonitor;
}


/*!
 Returns geotaggingtrail handle
 */
CxeGeoTaggingTrail &CxeEngineSymbian::geoTaggingTrail()
{
    return *mGeoTaggingTrail;
}

/*!
* Returns true, if the engine is ready or else false.
*/
bool CxeEngineSymbian::isEngineReady()
{
    bool ready = false;
    if (mCameraDeviceControl->state() == CxeCameraDeviceControl::Ready &&
        mViewfinderControl->state() == CxeViewfinderControl::Running) {
        if (mode() == Cxe::ImageMode) {
            if (mStillCaptureControl->state() == CxeStillCaptureControl::Ready) {
                ready = true;
            }
        } else if (mode() == Cxe::VideoMode) {
            if (mVideoCaptureControl->state() == CxeVideoCaptureControl::Ready) {
                ready = true;
            }
        }
    }
    return ready;
}

/*!
    Prepare current capture control (CxeVideoCaptureControl in video mode,
    CxeStillCaptureControl in still image mode).
*/
void CxeEngineSymbian::doInit()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEENGINESYMBIAN_DOINIT_IN, "e_CX_ENGINE_DO_INIT 1");

    mFilenameGenerator->init(mode());
    // load settings whenever we change mode or start camera or switch camera
    CxeSettingsImp *settingsImp = qobject_cast<CxeSettingsImp*>(mSettings);
    if (settingsImp) {
        settingsImp->loadSettings(mode());
    }

    if (mode() == Cxe::ImageMode) {
        // start geotagging trail in image mode.
        startGeotaggingTrail();
        mVideoCaptureControl->deinit();
        mStillCaptureControl->init();
    } else if (mode() == Cxe::VideoMode) {
        mStillCaptureControl->deinit();
        if (mGeoTaggingTrail) {
            // in video mode, Geotagging is not supported for now.
            mGeoTaggingTrail->stop();
        }
        mVideoCaptureControl->init();
    }

    OstTrace0(camerax_performance, CXEENGINESYMBIAN_DOINIT_OUT, "e_CX_ENGINE_DO_INIT 0");

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Returns camera mode.
 */
Cxe::CameraMode CxeEngineSymbian::mode() const
{
    return mCameraDeviceControl->mode();
}

/*!
 * Sets the camera mode.
 * \parama mode New camera mode
 */
void CxeEngineSymbian::setMode(Cxe::CameraMode mode)
{
    CX_DEBUG_ENTER_FUNCTION();
    mCameraDeviceControl->setMode(mode);
    CX_DEBUG_EXIT_FUNCTION();
}
/*!
    Check if we need to reserve camera.
*/
bool CxeEngineSymbian::reserveNeeded()
{
    // If camera device control is in Idle or pending release state,
    // we need to call reserve()
    return (mCameraDeviceControl->state() == CxeCameraDeviceControl::Idle ||
            mCameraDeviceControl->state() == CxeCameraDeviceControl::PendingRelease);
}

/*!
    Check if we need to prepare image/video capture.
*/
bool CxeEngineSymbian::initNeeded()
{
    bool result = false;

    if (mCameraDeviceControl->state() == CxeCameraDeviceControl::Ready) {
        if (mode() == Cxe::ImageMode) {
            if(mStillCaptureControl->state() == CxeStillCaptureControl::Uninitialized) {
                // We're in still image mode, camera is reserved and ready, but
                // we need to prepare still image control
                result = true;
            }
        } else if (mode() == Cxe::VideoMode) {
            if (mVideoCaptureControl->state() == CxeVideoCaptureControl::Idle ||
                mVideoCaptureControl->state() == CxeVideoCaptureControl::Initialized) {
                // We're in video mode, camera is reserved and ready, but
                // we need to prepare for video recording.
                result = true;
            }
        }
    }

    return result;
}

/*!
    Check if we need to start viewfinder.
*/
bool CxeEngineSymbian::startViewfinderNeeded()
{
    bool result = false;

    if (mViewfinderControl->state() != CxeViewfinderControl::Running) {
        if (mode() == Cxe::ImageMode &&
            mStillCaptureControl->state() == CxeStillCaptureControl::Ready) {
            // We're in still image mode and ready for capturing... we just need to
            // start viewfinder.
            result = true;
        } else if (mode() == Cxe::VideoMode &&
                   mVideoCaptureControl->state() == CxeVideoCaptureControl::Ready) {
            // We're in video mode and ready for capturing... we just need to
            // start viewfinder.
            result = true;
        }
    }

    return result;
}

void CxeEngineSymbian::initMode(Cxe::CameraMode cameraMode)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mode() == cameraMode) {
        CX_DEBUG(("initMode() called for current mode"));

        // We're already in the requested mode. However, it is possible
        // that we need to reserve and power on camera and/or prepare
        // the capture control.
        if (reserveNeeded()) {
            // Camera needs to be reserved. Initialization will continue
            // when we get the deviceReady() signal.
            CX_DEBUG(("initMode() - calling reserve()"));
            reserve();
        } else if (initNeeded()) {
            // Camera is reserved and ready, but we need to prepare still image control or
            // video capture control
            CX_DEBUG(("initMode() - calling doInit()"));
            doInit();
        } else if (startViewfinderNeeded()) {
            // Everything is ready and prepared, but we viewfinder is not yet running
            CX_DEBUG(("initMode() - calling viewfinder start()"));
            mViewfinderControl->start();
        } else {
            CX_DEBUG(("initMode() - no actions needed"));
            // Do nothing.
            // We are either already ready to capture or the we're pending
            // on prepare complete callback.
        }
    } else {
        CX_DEBUG(("initMode() called for mode switch"));

        // Mode switch
        mCameraDeviceControl->setMode(cameraMode);
        // stop the vf
        mViewfinderControl->stop();

        if (reserveNeeded()) {
            CX_DEBUG(("initMode() - calling reserve()"));
            reserve();
        } else if (mCameraDeviceControl->state() == CxeCameraDeviceControl::Ready) {
            CX_DEBUG(("initMode() - calling doInit()"));
            // Camera device is ready... we only need to prepare video or still
            // capture control, depending on current mode.
            doInit();
        } else {
            CX_DEBUG(("initMode() - no actions needed"));
            // Do nothing.
            // Camera device control is in Initializing state. Initialization will continue
            // when we get the deviceReady() callback.
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeEngineSymbian::reserve()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCameraDeviceControl->reserve();
    emit reserveStarted();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Saves current mode to the cenrep
*/
void CxeEngineSymbian::saveMode()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCameraDeviceControl && mSettings) {
        int value = mCameraDeviceControl->mode();
        mSettings->set(CxeSettingIds::CAMERA_MODE, value);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Start geotagging trail.
*/
void CxeEngineSymbian::startGeotaggingTrail()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    if (mGeoTaggingTrail && mSettings) {
        // location trail is limited to image mode only for now.
        int value = Cxe::GeoTaggingDisclaimerDisabled;
        mSettings->get(CxeSettingIds::GEOTAGGING_DISCLAIMER, value);

        // we start location trail only when Geotagging First-time-use note is accepted by user.
        if (value == Cxe::GeoTaggingDisclaimerDisabled) {
            mGeoTaggingTrail->start();
        }
    }
    
    CX_DEBUG_EXIT_FUNCTION();
}

// End of file
