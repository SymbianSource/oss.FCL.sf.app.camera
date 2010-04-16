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
#include <ECamOrientationCustomInterface2.h>
#include <ecamfacetrackingcustomapi.h>

#include "cxecameradevice.h"
#include "cxeerrormappingsymbian.h"
#include "cxutils.h"

#include "cxedummycamera.h"

CxeCameraDevice::CxeCameraDevice() :
    mCamera(NULL),
    mAdvancedSettings(NULL),
    mImageProcessor(NULL),
    mCameraSnapshot(NULL),
    mCameraOrientation(NULL),
    mFaceTracking(NULL)
{

}

CxeCameraDevice::~CxeCameraDevice()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Exiting, release all resources
    releaseResources();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDevice::deleteCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    emit prepareForCameraDelete();

    // preparing for new camera, release old resources
    releaseResources();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDevice::releaseCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCamera) {
        emit prepareForRelease();
        mCamera->Release();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDevice::setCamera( CCamera *camera )
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCamera != camera) {

        // new camera, delete old resources
        releaseResources();

        mCamera = camera;

        // initialize resources
        CxeError::Id err = initResources();

        emit cameraAllocated(err);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

CCamera* CxeCameraDevice::camera()
{
    return mCamera;
}

CCamera::CCameraAdvancedSettings* CxeCameraDevice::advancedSettings()
{
    return mAdvancedSettings;
}

CCamera::CCameraImageProcessing* CxeCameraDevice::imageProcessor()
{
    return mImageProcessor;
}


CCamera::CCameraSnapshot* CxeCameraDevice::cameraSnapshot()
{
    return mCameraSnapshot;
}


MCameraOrientation* CxeCameraDevice::cameraOrientation()
{
    return mCameraOrientation;
}

MCameraFaceTracking* CxeCameraDevice::faceTracking()
{
    return mFaceTracking;
}

CxeError::Id CxeCameraDevice::initResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    TInt status = KErrNone;

    if (mCamera) {
        CX_DEBUG(("Get CCamera extensions.."));

        mCameraOrientation = static_cast<MCameraOrientation*>(
            mCamera->CustomInterface(KCameraOrientationUid));
        CX_DEBUG(("MCameraOrientation interface 0x%08x", mCameraOrientation));

        mFaceTracking = static_cast<MCameraFaceTracking*>(
            mCamera->CustomInterface(KCameraFaceTrackingUid));
        CX_DEBUG(("MCameraFaceTracking interface 0x%08x", mFaceTracking));

        TRAPD(errorAdvSet, mAdvancedSettings =
              CCamera::CCameraAdvancedSettings::NewL(*mCamera));
        CX_DEBUG(("CCameraAdvancedSettings status: %d", errorAdvSet));

        TRAPD(errorImgPr, mImageProcessor =
              CCamera::CCameraImageProcessing::NewL(*mCamera));
        CX_DEBUG(("CCameraImageProcessing status: %d", errorImgPr));

        TRAPD(errorSnap, mCameraSnapshot =
              CCamera::CCameraSnapshot::NewL(*mCamera));
        CX_DEBUG(("CCameraSnapshot status: %d", errorSnap));

        // Check all statuses and set first error code encountered.
        // Imageprocessing is not supported by 2nd camera so we just ignore the error.
        status = errorAdvSet != KErrNone
                 ? errorAdvSet : errorSnap;
        CX_DEBUG(("Total status: %d", status));
    }

    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(status);
}

void CxeCameraDevice::releaseResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    // not owned.
    mCameraOrientation = NULL;
    mFaceTracking = NULL;

    delete mCameraSnapshot;
    mCameraSnapshot = NULL;

    delete mAdvancedSettings;
    mAdvancedSettings = NULL;

    delete mImageProcessor;
    mImageProcessor = NULL;

    delete mCamera;
    mCamera = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

CxeError::Id CxeCameraDevice::newCamera( Cxe::CameraIndex cameraIndex, MCameraObserver2 *observer )
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("Cxe: using camera index %d", cameraIndex));

    CX_DEBUG_ASSERT(cameraIndex == Cxe::PrimaryCameraIndex || cameraIndex == Cxe::SecondaryCameraIndex);

    CCamera* camera = NULL;

#if defined(CXE_USE_DUMMY_CAMERA) || defined(__WINSCW__)
    TRAPD(err, camera = CxeDummyCamera::NewL(*observer, cameraIndex, 100, 2));
    CX_DEBUG(("CxeCameraDevice::newCamera <> new CxeDummyCamera"));
#else
    TRAPD(err, camera = CCamera::New2L(*observer, cameraIndex, 100 /*KCameraClientPriority*/));
    CX_DEBUG(("CxeCameraDevice::newCamera <> new CCamera"));
#endif

    if (!err) {
        setCamera(camera);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}

// end of file
