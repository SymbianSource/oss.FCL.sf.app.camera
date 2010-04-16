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

#include <QMetaType>
#include <QImage>
#include <fbs.h>
#include <ecam.h> // CCamera
#include <ecam/ecamadvsettingsintf.h> // CCamera
#include <ecamadvsettings.h>

#include "cxesettings.h"
#include "cxeautofocuscontrolsymbian.h"
#include "cxutils.h"
#include "cxecameradevice.h" // CxeCameraDevice
#include "cxesettingsmappersymbian.h"
#include "cxeerrormappingsymbian.h"
#include "cxestate.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxeautofocuscontrolsymbianTraces.h"
#endif



/*
* CxeAutoFocusControlSymbian::CxeAutoFocusControlSymbian
*/
CxeAutoFocusControlSymbian::CxeAutoFocusControlSymbian(CxeCameraDevice &cameraDevice)
    : CxeStateMachine("CxeAutoFocusControlSymbian"),
      mCameraDevice(cameraDevice),
      mAdvancedSettings(NULL),
      mCancelled(false)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeAutoFocusControl::State>();

    initializeStates();

    // connect signals from cameraDevice, so we recieve events when camera reference changes
    QObject::connect( &cameraDevice,
                      SIGNAL(prepareForCameraDelete()),
                      this,SLOT(prepareForCameraDelete()) );

    QObject::connect( &cameraDevice,
                      SIGNAL(cameraAllocated(CxeError::Id)),
                      this,SLOT(handleCameraAllocated(CxeError::Id)) );

    QObject::connect( &cameraDevice,
                      SIGNAL(prepareForRelease()),
                      this,SLOT(prepareForRelease()) );

    initializeResources();

    CX_DEBUG_EXIT_FUNCTION();
}



/*
* CxeAutoFocusControlSymbian::~CxeAutoFocusControlSymbian
*/
CxeAutoFocusControlSymbian::~CxeAutoFocusControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


/*
* Start Autofocus
*/
CxeError::Id CxeAutoFocusControlSymbian::start()
{
    CX_DEBUG( ("CxeAutoFocusControlSymbian::start() <> state: %d", state() ) );

    int err = KErrNone;

    CX_ASSERT_ALWAYS(mAdvancedSettings);

    if ( state() != CxeAutoFocusControl::InProgress && state() != CxeAutoFocusControl::Canceling  ) {
        CX_DEBUG(("CxeAutoFocusControlSymbian::start() calling SetAutoFocusType"));
        mCancelled = false;
        setState(InProgress);
        setFocusRange(mAFRange);
        setFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
    } else { // AF was started earlier, can't start until it completes
        err = KErrInUse;
    }

    CX_DEBUG( ("CxeAutoFocusControlSymbian::start() <= err : %d", err ) );

    return CxeErrorHandlingSymbian::map(err);
}



/*
* Cancel Autofocus
*/
void CxeAutoFocusControlSymbian::cancel()
{
    CX_DEBUG( ("CxeAutoFocusControlSymbian::cancel <> state: %d", state() ) );

    CX_DEBUG_ASSERT(mAdvancedSettings);

    if (!mCancelled) {
        if (state() == CxeAutoFocusControl::InProgress) {
            // Need to stop current AF first. Wait for AF event to proceed.
            setState(CxeAutoFocusControl::Canceling);
            setFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff);

        } else if (state() != CxeAutoFocusControl::Canceling) {
            // Cancel means move to hyperfocal.
            setState(CxeAutoFocusControl::Canceling);
            CX_DEBUG(("CxeAutoFocusControlSymbian::cancel() moving to hyperfocal"));
            setFocusRange(CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal);
            setFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}



/*
* Set Autofocus mode
*/
void CxeAutoFocusControlSymbian::setMode(CxeAutoFocusControl::Mode newMode)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_ASSERT(mAdvancedSettings);

    mAfMode = newMode;
    mAFRange = CxeSettingsMapperSymbian::Map2CameraAutofocus(mAfMode);

    CX_DEBUG(("CxeAutoFocusControlSymbian::setMode() mAFRange: %d", mAFRange));

    mCancelled = false;
    setFocusRange(mAFRange);

    CX_DEBUG_EXIT_FUNCTION();
}


/*
* returns Autofocus mode
*/
CxeAutoFocusControl::Mode CxeAutoFocusControlSymbian::mode() const
{
    return mAfMode;
}


/*
* To check if Autofocus is supported
*/
bool CxeAutoFocusControlSymbian::supported() const
{
    CX_DEBUG_ENTER_FUNCTION();

    bool supported =
        (supportedFocusTypes() != CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff);

    CX_DEBUG_EXIT_FUNCTION();
    return supported;
}



/*
* Slot for handling ECam events
*/
void CxeAutoFocusControlSymbian::handleCameraEvent(int eventUid, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG( ("CxeAutoFocusControlSymbian::handleCameraEvent <> state: %d error %d", state(), error ) );
    CX_DEBUG( ("CxeAutoFocusControlSymbian::handleCameraEvent <> uid: %x optimalfocusuid: %x focustype2uid %x",
              eventUid,
              KUidECamEventCameraSettingsOptimalFocusUidValue,
              KUidECamEventCameraSettingAutoFocusType2UidValue ));

    // We're only interested in autofocus events
    if ( eventUid == KUidECamEventCameraSettingsOptimalFocusUidValue ||
         eventUid == KUidECamEventCameraSettingAutoFocusType2UidValue ) {
         // Autofocus Event handle it.
         handleAfEvent(eventUid, error);
    }

    CX_DEBUG_EXIT_FUNCTION();
}




void CxeAutoFocusControlSymbian::prepareForRelease()
{

    CX_DEBUG_ENTER_FUNCTION();

    // camera is anyway released, so no need to cancel the AF anyway
    setState(CxeAutoFocusControl::Unknown);
    mCancelled = false;

    CX_DEBUG_EXIT_FUNCTION();

}



/*
* camera reference changing, release resources
*/
void CxeAutoFocusControlSymbian::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();

    prepareForRelease();
    mAdvancedSettings = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}



/*
* new camera available,
*/
void CxeAutoFocusControlSymbian::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (!error) {
        setState(CxeAutoFocusControl::Unknown);
        mCancelled = false;
        initializeResources();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*
* CxeAutoFocusControlSymbian::state
*/
CxeAutoFocusControl::State CxeAutoFocusControlSymbian::state() const
{
    return static_cast<State>( stateId() );
}

/*
* CxeAutoFocusControlSymbian::handleStateChanged
*/
void CxeAutoFocusControlSymbian::handleStateChanged( int newStateId, CxeError::Id error )
{
    emit stateChanged(static_cast<State>(newStateId), error);
}

/*
* CxeAutoFocusControlSymbian::initializeStates
*/
void CxeAutoFocusControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState( new CxeState( Unknown , "Unknown", InProgress | Canceling ) );
    addState( new CxeState( InProgress , "InProgress", Unknown | Failed | Ready | Canceling ) );
    addState( new CxeState( Failed , "Failed", InProgress | Unknown | Canceling ) );
    addState( new CxeState( Ready , "Ready", Unknown | InProgress | Canceling ) );
    addState( new CxeState( Canceling , "Canceling", Unknown ) );

    setInitialState( Unknown );
}

/*
* CxeAutoFocusControlSymbian::initializeResources
*/
void CxeAutoFocusControlSymbian::initializeResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    // No check if non-null. Not supported if zero pointer (see supported() ).
    mAdvancedSettings = mCameraDevice.advancedSettings();

    CX_DEBUG_EXIT_FUNCTION();
}


/*
* CxeAutoFocusControlSymbian::setFocusRange
*/
void CxeAutoFocusControlSymbian::setFocusRange(CCamera::CCameraAdvancedSettings::TFocusRange range)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mAdvancedSettings);

    mAdvancedSettings->SetFocusRange(range);

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* CxeAutoFocusControlSymbian::focusRange
*/
CCamera::CCameraAdvancedSettings::TFocusRange CxeAutoFocusControlSymbian::focusRange() const
{
    CX_DEBUG_ENTER_FUNCTION();
    const CCamera::CCameraAdvancedSettings::TFocusRange range(
        mAdvancedSettings
              ? mAdvancedSettings->FocusRange()
              : CCamera::CCameraAdvancedSettings::EFocusRangeAuto );

    CX_DEBUG_EXIT_FUNCTION();
    return range;
}

/*
* CxeAutoFocusControlSymbian::setFocusType
*/
void CxeAutoFocusControlSymbian::setFocusType(CCamera::CCameraAdvancedSettings::TAutoFocusType type)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mAdvancedSettings);

    mAdvancedSettings->SetAutoFocusType(type);

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* CxeAutoFocusControlSymbian::supportedFocusTypes
*/
int CxeAutoFocusControlSymbian::supportedFocusTypes() const
{
    CX_DEBUG_ENTER_FUNCTION();

    const int support( mAdvancedSettings
                     ? mAdvancedSettings->SupportedAutoFocusTypes()
                     : CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff );

    CX_DEBUG_EXIT_FUNCTION();
    return support;
}

/*
* CxeAutoFocusControlSymbian::focusType
*/
CCamera::CCameraAdvancedSettings::TAutoFocusType CxeAutoFocusControlSymbian::focusType() const
{
    CX_DEBUG_ENTER_FUNCTION();

    const CCamera::CCameraAdvancedSettings::TAutoFocusType type(
        mAdvancedSettings
              ? mAdvancedSettings->AutoFocusType()
              : CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff );

    CX_DEBUG_EXIT_FUNCTION();
    return type;
}


/*
* Image Scene mode changed, get the new autofocus value
*/
void CxeAutoFocusControlSymbian::handleSceneChanged(CxeScene& scene)
{
    CX_DEBUG_ENTER_FUNCTION();

    // whenever scene mode is changed we set the state to unknown
    setState(CxeAutoFocusControl::Unknown);

    // we are interested only in the AF range.
    if(scene.contains(CxeSettingIds::FOCAL_RANGE) && supported() ) {
        setMode(static_cast<CxeAutoFocusControl::Mode>(scene[CxeSettingIds::FOCAL_RANGE].toInt()));
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*
* CxeAutoFocusControlSymbian::handleAfEvent
*/
void CxeAutoFocusControlSymbian::handleAfEvent(int eventUid, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    switch ( state() ) {
    case CxeAutoFocusControl::InProgress: {
        if (eventUid == KUidECamEventCameraSettingsOptimalFocusUidValue ) {
            OstTrace0(camerax_performance, CXEAUTOFOCUSCONTROLSYMBIAN_AF_LOCK, "msg: e_CX_AUTOFOCUS_LOCK 0");

            CX_DEBUG(("CxeAutoFocusControlSymbian::handleAfEvent <> KUidECamEventCameraSettingsOptimalFocus"));
            if (KErrNone == error) {
                setState(CxeAutoFocusControl::Ready);
            } else {
                setState(CxeAutoFocusControl::Failed, error);
            }
         }
         break;
        }
    case CxeAutoFocusControl::Canceling: {
        CX_DEBUG(("CxeAutoFocusControlSymbian::handleAfEvent <> Canceling"));
        // Cancelling started by setting AF off to stop ongoing focus operation.
        // Finalize cancelling by setting lens to hyperfocal position.
        if (eventUid == KUidECamEventCameraSettingAutoFocusType2UidValue) {
            if (focusType() == CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff) {
                setFocusRange(CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal);
                setFocusType(CCamera::CCameraAdvancedSettings::EAutoFocusTypeSingle);
            }
        } else if (eventUid == KUidECamEventCameraSettingsOptimalFocusUidValue) {
            mCancelled = true;
            setState(CxeAutoFocusControl::Unknown);
        }

        break;
    }
    default:
        break;
    } // end switch

}

// end of file
