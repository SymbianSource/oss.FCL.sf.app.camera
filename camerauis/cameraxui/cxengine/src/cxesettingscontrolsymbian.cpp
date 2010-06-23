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

#include <ecam.h>
#include <ecamadvsettings.h>
#include <ecamimageprocessing.h>
#include <ecam/ecamconstants.h>
#include <ecamfacetrackingcustomapi.h>

#include <QString>
#include <QVariant>

#include "cxutils.h"
#include "cxenamespace.h"
#include "cxecameradevice.h"
#include "cxesettings.h"
#include "cxesettingsmappersymbian.h"
#include "cxesettingscontrolsymbian.h"

/*!
* Constructor
*/
CxeSettingsControlSymbian::CxeSettingsControlSymbian(CxeCameraDevice &cameraDevice, CxeSettings &settings)
    : mCameraDevice(cameraDevice),
      mSettings(settings)
{
    // connect scene / setting change callbacks to settings control
    connect(&mSettings,
            SIGNAL(settingValueChanged(const QString&,QVariant)),
            this,
            SLOT(handleSettingValueChanged(const QString&,QVariant)));

    connect(&mSettings,
            SIGNAL(sceneChanged(CxeScene&)),
            this,
            SLOT(handleSceneChanged(CxeScene&)));
}

/*!
* Destructor
*/
CxeSettingsControlSymbian::~CxeSettingsControlSymbian()
{
}

/*!
* Handle new setting value.
* New value is set to camera.
* \param settingId The id of the updated setting
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::handleSettingValueChanged(const QString& settingId, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (settingId == CxeSettingIds::COLOR_TONE) {
        updateColorToneSetting(newValue);
    } else if (settingId == CxeSettingIds::WHITE_BALANCE) {
        updateWhiteBalanceSetting(newValue);
    } else if (settingId == CxeSettingIds::LIGHT_SENSITIVITY) {
            updateLightSensitivitySetting(newValue);
    } else if (settingId == CxeSettingIds::SHARPNESS) {
        updateSharpnessSetting(newValue);
    } else if (settingId == CxeSettingIds::CONTRAST) {
        updateContrastSetting(newValue);
    } else if (settingId == CxeSettingIds::BRIGHTNESS) {
        updateBrightnessSetting(newValue);
    } else if (settingId == CxeSettingIds::EXPOSURE_MODE) {
        updateExposureModeSetting(newValue);
    } else if (settingId == CxeSettingIds::EV_COMPENSATION_VALUE) {
            updateExposureCompensationSetting(newValue);
    } else if (settingId == CxeSettingIds::FACE_TRACKING) {
            updateFaceTrackingSetting(newValue);
    } else {
        // do nothing
    }

    // Image mode specific
    if (settingId == CxeSettingIds::FLASH_MODE) {
        updateFlashSetting(newValue);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle new scene being set.
* Scene settings are checked and new values are set to camera.
* \param scene New scene containing scene specific settings.
*/
void CxeSettingsControlSymbian::handleSceneChanged(CxeScene& scene)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeSettingsControlSymbian <> new scene [%s]",
              scene[CxeSettingIds::SCENE_ID].value<QString>().toAscii().constData()));

    foreach (const QString& settingId, scene.keys()) {
        handleSettingValueChanged(settingId, scene[settingId]);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update color tone setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateColorToneSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

//! @todo: Should be fixed in Dummy engine
#if !defined(__WINSCW__) && !defined(CXE_USE_DUMMY_CAMERA)

    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    CCamera::CCameraImageProcessing::TEffect effect =
        CxeSettingsMapperSymbian::Map2CameraEffect(newValue.toInt());

    if (effect != mCameraDevice.imageProcessor()->TransformationValue(KUidECamEventImageProcessingEffect)) {
        mCameraDevice.imageProcessor()->SetTransformationValue(KUidECamEventImageProcessingEffect, effect);
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }
#else
    Q_UNUSED(newValue)
#endif

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update white balance setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateWhiteBalanceSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mCameraDevice.camera());

    CCamera::TWhiteBalance wb = CxeSettingsMapperSymbian::Map2CameraWb(newValue.toInt());

    if (wb != mCameraDevice.camera()->WhiteBalance()) {
        TRAP_IGNORE( mCameraDevice.camera()->SetWhiteBalanceL(wb) );
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update light sensitivity (ISO) setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateLightSensitivitySetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.advancedSettings());

    int iso = newValue.toInt();

    if (iso != mCameraDevice.advancedSettings()->IsoRate()) {
        CCamera::CCameraAdvancedSettings::TISORateType type;
        if(iso == 0) {
            // Automatic ISO rate
            type = CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised;
        } else {
            // Manual ISO rate
            type = CCamera::CCameraAdvancedSettings::EISOManual;
        }
        TRAP_IGNORE(mCameraDevice.advancedSettings()->SetISORateL(type, iso));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update sharpness setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateSharpnessSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

//! @todo: Should be fixed in Dummy engine
#if !defined(__WINSCW__) && !defined(CXE_USE_DUMMY_CAMERA)

    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    // Scale UI values of -2..2 to ECAM range -100..100.
    int currentSharpness = mCameraDevice.imageProcessor()->TransformationValue(KUidECamEventImageProcessingAdjustSharpness);
    int newSharpness     = newValue.toReal()*50;

    CX_DEBUG(("Current sharpness [uid:0x%08x] value is [%d]", KUidECamEventImageProcessingAdjustSharpness, currentSharpness));
    CX_DEBUG(("Setting sharpness [uid:0x%08x] to value [%d]", KUidECamEventImageProcessingAdjustSharpness, newSharpness));
    if (newSharpness != currentSharpness) {
        mCameraDevice.imageProcessor()->SetTransformationValue(KUidECamEventImageProcessingAdjustSharpness, newSharpness);
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }
#else
    Q_UNUSED(newValue)
#endif

   CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update contrast setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateContrastSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    // Scale UI values of -2..2 to ECAM range -100..100.
    int currentContrast = mCameraDevice.imageProcessor()->TransformationValue(KUidECamEventImageProcessingAdjustContrast);
    int newContrast     = newValue.toReal()*50;

    CX_DEBUG(("Current contrast [uid:0x%08x] value is [%d]", KUidECamEventImageProcessingAdjustContrast, currentContrast));
    CX_DEBUG(("Setting contrast [uid:0x%08x] to value [%d]", KUidECamEventImageProcessingAdjustContrast, newContrast));
    if (newContrast != currentContrast) {
        mCameraDevice.imageProcessor()->SetTransformationValue(KUidECamEventImageProcessingAdjustContrast, newContrast);
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update brightness setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateBrightnessSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    // Scale UI values of -10..10 to ECAM range -100..100.
    int currentBrightness = mCameraDevice.imageProcessor()->TransformationValue(KUidECamEventImageProcessingAdjustBrightness);
    int newBrightness     = newValue.toInt()*10;

    CX_DEBUG(("Current brightness [uid:0x%08x] value is [%d]", KUidECamEventImageProcessingAdjustBrightness, currentBrightness));
    CX_DEBUG(("Setting brightness [uid:0x%08x] to value [%d]", KUidECamEventImageProcessingAdjustBrightness, newBrightness));
    if (newBrightness != currentBrightness) {
        mCameraDevice.imageProcessor()->SetTransformationValue(KUidECamEventImageProcessingAdjustBrightness, newBrightness);
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update Exposure mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateExposureModeSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mCameraDevice.advancedSettings());

    CCamera::TExposure exposure = CxeSettingsMapperSymbian::Map2CameraExposureMode(newValue.toInt());

    if(exposure != mCameraDevice.advancedSettings()->ExposureMode()) {
        mCameraDevice.advancedSettings()->SetExposureMode(exposure);
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update Exposure Compensation mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateExposureCompensationSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.advancedSettings());

    // Exposure compensation is a float value, e.g. "-1.5".
    // ECAM interface takes integer values, so KECamFineResolutionFactor from
    // ecamconstants.h needs to be used as scaler.
    int ev = newValue.toReal()*KECamFineResolutionFactor;

    if (ev != mCameraDevice.advancedSettings()->ExposureCompensationStep()) {
        mCameraDevice.advancedSettings()->SetExposureCompensationStep(ev);
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update flash mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateFlashSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.camera());

    CCamera::TFlash flash = CxeSettingsMapperSymbian::Map2CameraFlash(newValue.toInt());

    if (flash != mCameraDevice.camera()->Flash()) {
        TRAP_IGNORE(mCameraDevice.camera()->SetFlashL(flash));
    } else {
        CX_DEBUG(("CxeSettingsControlSymbian: value up-to-date"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update face tracking mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateFaceTrackingSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    MCameraFaceTracking *faceTracking = mCameraDevice.faceTracking();
    if(faceTracking) {
        TRAP_IGNORE(faceTracking->SetFaceTrackingL(newValue.toInt()));
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file