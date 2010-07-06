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
/*
* Symbian specific settings handling
*/

#include <QVariant>
#include <QFile>
#include <QList>
#include <QMultiMap>
#include <QMetaType>

#include "cxesettingsmodelimp.h"
#include "cxesettingscenrepstore.h"
#include "cxesettings.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeerror.h"
#include "cxeautofocuscontrol.h"


using namespace Cxe;


/*!
* CxeSettingsModel::CxeSettingsModel
*/
CxeSettingsModelImp::CxeSettingsModelImp(CxeSettingsStore *settingsStore)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(settingsStore);
    // we take ownership of the settings store.
    mSettingStore = settingsStore;
    init();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* CxeSettingsModelImp::init
*/
void CxeSettingsModelImp::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCurrentImgScene.clear();
    mCurrentVidScene.clear();

    loadRuntimeSettings();
    loadImageScenes();
    loadVideoScenes();

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* CxeSettingsModelImp::~CxeSettingsModelImp()
*/
CxeSettingsModelImp::~CxeSettingsModelImp()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mSettingStore;

    mCurrentImgScene.clear();
    mCurrentVidScene.clear();

    mImageSceneModes.clear();
    mVideoSceneModes.clear();

    mRuntimeSettings.clear();

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Loads all run-time settings
*/
void CxeSettingsModelImp::loadRuntimeSettings()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT( mSettingStore );

    QList<QString> runtimeKeys;
    // all supported runtime keys are fetched from here.
    supportedKeys(runtimeKeys);
    // load all run-time setting values from cenrep.
    mRuntimeSettings = mSettingStore->loadRuntimeSettings(runtimeKeys);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Get setting value associated with the key.
* @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
* @Param value - contains the value associated with the key.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
CxeError::Id CxeSettingsModelImp::getSettingValue(const QString &key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingStore);

    // Try first to find the item from cenrep store.

    CxeError::Id err = mSettingStore->get(key, value);

    // If setting is not in cenrep store, try fetching it from scene settings.
    if (err) {
        // setting not found in setting store, try finding if its scene specific setting.
        CX_DEBUG(( "fetching value from scene settings" ));
        err = sceneSettingValue(key, value);
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}




/*!
* Get setting value associated with the key.
* @param uid - UID of component that owns the setting key
* @Param key - key id of the setting
* @param type - the type of key
* @Param value - contains the value associated with the key.
*/
void CxeSettingsModelImp::getSettingValue(long int uid,
                                          unsigned long int key,
                                          Cxe::SettingKeyType type,
                                          QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingStore);

    mSettingStore->startMonitoring(uid, key, type, value);

    CX_DEBUG_EXIT_FUNCTION();
}




/*!
* Set a value to the key.
* @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
* @Param value - contains the value associated with the key.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
CxeError::Id CxeSettingsModelImp::set(const QString &key, const QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingStore);

    // Try storing new value to cenrep
    CxeError::Id err = mSettingStore->set(key, newValue);

    if (err) {
        CX_DEBUG(( "Key not found in cenrepstore, writing value to scene settings" ));
        err = setSceneSettingValue(key, newValue);
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
* Reset all settings
*/
void CxeSettingsModelImp::reset()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingStore);
    mSettingStore->reset();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Get the configured run-time value associated with the key.
* @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
* @Param value - contains the value associated with the key.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
CxeError::Id CxeSettingsModelImp::getRuntimeValue(const QString &key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;

    // read run-time configuration value
    if ( mRuntimeSettings.contains(key) ) {
        value = qVariantFromValue<QVariantList > (mRuntimeSettings.value(key));
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
* Set new Image scene mode.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
CxeError::Id CxeSettingsModelImp::setImageScene(const QString &newScene)
{
    CX_DEBUG_ENTER_FUNCTION();

    // load scene specific settings
    CxeError::Id err = loadSceneData(newScene, mCurrentImgScene);

    if (!err) {
        // saving current image scene to cenrep
        err = set(CxeSettingIds::IMAGE_SCENE, newScene);

        // saving flash value from scene to cenrep
        QString key(CxeSettingIds::FLASH_MODE);
        err = set(key, mCurrentImgScene[key].toInt());

        // saving face tracking value from scene to cenrep
        key = CxeSettingIds::FACE_TRACKING;
        err = set(key, mCurrentImgScene[key].toInt());
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
* Set new video scene mode.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
CxeError::Id CxeSettingsModelImp::setVideoScene(const QString &newScene)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = loadSceneData(newScene, mCurrentVidScene);

    if (!err) {
        // video scene loaded successfully, store the scene value to cenrep
        err = set(CxeSettingIds::VIDEO_SCENE, newScene);
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*!
* Loads Image scene settings for the given Scene ID
*/
CxeError::Id CxeSettingsModelImp::imageScene(const QString &sceneId, CxeScene &sceneSettings)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;

    if(mImageSceneModes.contains(sceneId)) {
        sceneSettings = mImageSceneModes[sceneId];
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
* Loads Video scene settings for the given Scene ID
* \param sceneId
* \param sceneSettings
*/
CxeError::Id CxeSettingsModelImp::videoScene(const QString &sceneId, CxeScene &sceneSettings)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;

    if(mVideoSceneModes.contains(sceneId)) {
        sceneSettings = mVideoSceneModes[sceneId];
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();

   return err;
}


/*!
* Creates a copy of the selected scene that we use for accessing specific scene settings.
* \param newScene
* \param currentSceneSettings
*/
CxeError::Id CxeSettingsModelImp::loadSceneData(const QString &newScene, CxeScene &currentSceneSettings)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeScene sceneDefaultSettings;
    CxeError::Id err = imageScene(newScene, sceneDefaultSettings);

    if (err == CxeError::NotFound) {
        // not still scene, try in video scene.
        err = videoScene(newScene, sceneDefaultSettings);
    }

    if (!err) {
        // We have a new scene available, so we can clear the old values.
        currentSceneSettings.clear();

        // creating a deep copy of the scene mode selected.
        CxeScene::const_iterator scene = sceneDefaultSettings.constBegin();
         while (scene != sceneDefaultSettings.constEnd()) {
             currentSceneSettings.insert(scene.key(), scene.value());
             ++scene;
         }
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
* Returns scene setting value
* \param key Settings key
* \param[out] value Value associated with the key
* \return Error id. CxeError::None if no errors.
*/
CxeError::Id CxeSettingsModelImp::sceneSettingValue(const QString &key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeScene scene;
    CxeError::Id err = CxeError::None;

    if(mCameraMode == Cxe::ImageMode) {
        CX_DEBUG(( "CxeSettingsModelImp::sceneSettingValue - Image mode Setting"));
        scene = mCurrentImgScene;
    } else {
        CX_DEBUG(( "CxeSettingsModelImp::sceneSettingValue - Video mode Setting"));
        scene = mCurrentVidScene;
    }

    if (scene.contains(key)) {
        value = scene[key];
    } else {
        err = CxeError::NotFound;
    }


    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
* Sets new value to settings specific to the scene.
* @param key - setting id.
* @param newValue - new setting value
* @param error Error code. CxeError::None if operation has been successful.
* @return Error id. CxeError::None if no errors.
*/
CxeError::Id CxeSettingsModelImp::setSceneSettingValue(const QString &key, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;
    CxeScene *scene(0);

    if (mCameraMode == Cxe::ImageMode) {
        CX_DEBUG(( "CxeSettingsModelImp::setSceneSettingValue - Image mode Setting"));
        scene = &mCurrentImgScene;
    } else {
        CX_DEBUG(( "CxeSettingsModelImp::setSceneSettingValue - Video mode Setting"));
        scene = &mCurrentVidScene;
    }

    if (scene && scene->contains(key)) {
        CX_DEBUG(( "CxeSettingsModelImp::setSceneSettingValue KEY found, writing value"));
        scene->insert(key, newValue);
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*! 
* Appending the run-time keys to an array
* \param[in,out] runtimeKeys QList where the supported runtimekeys will be added to
*/
void CxeSettingsModelImp::supportedKeys(QList<QString>& runtimeKeys)
{
    CX_DEBUG_ENTER_FUNCTION();

    runtimeKeys.append(CxeRuntimeKeys::PRIMARY_CAMERA_CAPTURE_KEYS);
    runtimeKeys.append(CxeRuntimeKeys::PRIMARY_CAMERA_AUTOFOCUS_KEYS);
    runtimeKeys.append(CxeRuntimeKeys::SECONDARY_CAMERA_CAPTURE_KEYS);
    runtimeKeys.append(CxeRuntimeKeys::FREE_MEMORY_LEVELS);
    runtimeKeys.append(CxeRuntimeKeys::STILL_MAX_ZOOM_LIMITS);
    runtimeKeys.append(CxeRuntimeKeys::VIDEO_MAX_ZOOM_LIMITS);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Loads all video scene modes
*/
void CxeSettingsModelImp::loadVideoScenes()
{
    CX_DEBUG_ENTER_FUNCTION();

    mVideoSceneModes.clear();

    CxeScene vidSceneAuto;

    vidSceneAuto.insert(CxeSettingIds::SCENE_ID, Cxe::VIDEO_SCENE_AUTO);
    vidSceneAuto.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    vidSceneAuto.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    vidSceneAuto.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    vidSceneAuto.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    vidSceneAuto.insert(CxeSettingIds::CONTRAST, 0);
    vidSceneAuto.insert(CxeSettingIds::FRAME_RATE, 0);
    vidSceneAuto.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);

    mVideoSceneModes.insert(Cxe::VIDEO_SCENE_AUTO,vidSceneAuto);


    CxeScene vidSceneNight;

    vidSceneNight.insert(CxeSettingIds::SCENE_ID, Cxe::VIDEO_SCENE_NIGHT);
    vidSceneNight.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    vidSceneNight.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    vidSceneNight.insert(CxeSettingIds::EXPOSURE_MODE, ExposureNight);
    vidSceneNight.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    vidSceneNight.insert(CxeSettingIds::CONTRAST, 0);
    vidSceneNight.insert(CxeSettingIds::FRAME_RATE, 0);
    vidSceneNight.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);

    mVideoSceneModes.insert(Cxe::VIDEO_SCENE_NIGHT, vidSceneNight);


    CxeScene vidSceneLowLight;

    vidSceneLowLight.insert(CxeSettingIds::SCENE_ID, Cxe::VIDEO_SCENE_LOWLIGHT);
    vidSceneLowLight.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    vidSceneLowLight.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    vidSceneLowLight.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    vidSceneLowLight.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    vidSceneLowLight.insert(CxeSettingIds::CONTRAST, 0);
    vidSceneLowLight.insert(CxeSettingIds::FRAME_RATE, 15); //fps
    vidSceneLowLight.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);

    mVideoSceneModes.insert(Cxe::VIDEO_SCENE_LOWLIGHT, vidSceneLowLight);


    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Loads all Image Scene Modes
*/
void CxeSettingsModelImp::loadImageScenes()
{
    CX_DEBUG_ENTER_FUNCTION();

    mImageSceneModes.clear();

    CxeScene imgSceneAuto;

    imgSceneAuto.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_AUTO);
    imgSceneAuto.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Auto);
    imgSceneAuto.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgSceneAuto.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    imgSceneAuto.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgSceneAuto.insert(CxeSettingIds::CONTRAST, 0);
    imgSceneAuto.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgSceneAuto.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgSceneAuto.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgSceneAuto.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgSceneAuto.insert(CxeSettingIds::FLASH_MODE, FlashAuto);
    imgSceneAuto.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_AUTO, imgSceneAuto);


    CxeScene imgSceneSports;

    imgSceneSports.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_SPORTS);
    imgSceneSports.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    imgSceneSports.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgSceneSports.insert(CxeSettingIds::EXPOSURE_MODE, ExposureSport);
    imgSceneSports.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgSceneSports.insert(CxeSettingIds::CONTRAST, 0);
    imgSceneSports.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgSceneSports.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgSceneSports.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgSceneSports.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgSceneSports.insert(CxeSettingIds::FLASH_MODE, FlashOff);
    imgSceneSports.insert(CxeSettingIds::FACE_TRACKING, 0);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_SPORTS, imgSceneSports);


    CxeScene imgSceneCloseUp;

    imgSceneCloseUp.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_MACRO);
    imgSceneCloseUp.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Macro);
    imgSceneCloseUp.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgSceneCloseUp.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    imgSceneCloseUp.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgSceneCloseUp.insert(CxeSettingIds::CONTRAST, 0);
    imgSceneCloseUp.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgSceneCloseUp.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgSceneCloseUp.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgSceneCloseUp.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgSceneCloseUp.insert(CxeSettingIds::FLASH_MODE, FlashAuto);
    imgSceneCloseUp.insert(CxeSettingIds::FACE_TRACKING, 0);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_MACRO, imgSceneCloseUp);

    CxeScene imgPortraitscene;

    imgPortraitscene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_PORTRAIT);
    imgPortraitscene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Portrait);
    imgPortraitscene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgPortraitscene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureBacklight);
    imgPortraitscene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgPortraitscene.insert(CxeSettingIds::CONTRAST, 0);
    imgPortraitscene.insert(CxeSettingIds::SHARPNESS, SharpnessSoft);
    imgPortraitscene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgPortraitscene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgPortraitscene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgPortraitscene.insert(CxeSettingIds::FLASH_MODE, FlashAntiRedEye);
    imgPortraitscene.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_PORTRAIT, imgPortraitscene);

    CxeScene imglandscapescene;

    imglandscapescene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_SCENERY);
    imglandscapescene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Infinity);
    imglandscapescene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceSunny);
    imglandscapescene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    imglandscapescene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imglandscapescene.insert(CxeSettingIds::CONTRAST, 0);
    imglandscapescene.insert(CxeSettingIds::SHARPNESS, SharpnessHard);
    imglandscapescene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imglandscapescene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imglandscapescene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imglandscapescene.insert(CxeSettingIds::FLASH_MODE, FlashOff);
    imglandscapescene.insert(CxeSettingIds::FACE_TRACKING, 0);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_SCENERY, imglandscapescene);


    CxeScene imgNightscene;

    imgNightscene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_NIGHT);
    imgNightscene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Auto);
    imgNightscene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgNightscene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureNight);
    imgNightscene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgNightscene.insert(CxeSettingIds::CONTRAST, 0);
    imgNightscene.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgNightscene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgNightscene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgNightscene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgNightscene.insert(CxeSettingIds::FLASH_MODE, FlashOff);
    imgNightscene.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_NIGHT, imgNightscene);

    CxeScene imgNightpotraitscene;

    imgNightpotraitscene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_NIGHTPORTRAIT);
    imgNightpotraitscene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Portrait);
    imgNightpotraitscene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgNightpotraitscene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureNight);
    imgNightpotraitscene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgNightpotraitscene.insert(CxeSettingIds::CONTRAST, 0);
    imgNightpotraitscene.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgNightpotraitscene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgNightpotraitscene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgNightpotraitscene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgNightpotraitscene.insert(CxeSettingIds::FLASH_MODE, FlashAntiRedEye);
    imgNightpotraitscene.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_NIGHTPORTRAIT, imgNightpotraitscene);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Returns the current image scene mode.
*/
CxeScene& CxeSettingsModelImp::currentImageScene()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();

    return mCurrentImgScene;
}


/*!
* Returns the current video scene mode.
*/
CxeScene& CxeSettingsModelImp::currentVideoScene()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();

    return mCurrentVidScene;
}


/*!
* Restores settings whenever we switch between Image/Video modes or
* during startup.
*/
void CxeSettingsModelImp::cameraModeChanged(Cxe::CameraMode newMode)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (newMode == Cxe::ImageMode) {
        restoreImageSettings();
    } else {
        restoreVideoSettings();
    }

    mCameraMode = newMode;

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Restores image settings, during mode change or during startup.
*/
void CxeSettingsModelImp::restoreImageSettings()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;
    QVariant currentSceneInUse = mCurrentImgScene[CxeSettingIds::SCENE_ID];

    // get the image scene value from cenrep and load the scene settings
    QVariant cenrepSceneValue;
    QString key(CxeSettingIds::IMAGE_SCENE);
    err = getSettingValue(key, cenrepSceneValue);

    bool ok2LoadSceneSettings = (cenrepSceneValue != currentSceneInUse);

    if (!err && ok2LoadSceneSettings) {
        // loading scene settings
        err = loadSceneData(cenrepSceneValue.toString(), mCurrentImgScene);
    }

    // Updating Flash setting from cenrep
    QVariant value;
    key = CxeSettingIds::FLASH_MODE;
    err = getSettingValue(key, value);

    if (!err && mCurrentImgScene.contains(key)) {
        // update local datastructure with flash setting value from cenrep.
        CX_DEBUG(( "flash setting value %d", value.toInt()));
        mCurrentImgScene[key] = value;
    }

    // Updating Face Tracking setting from cenrep
    key = CxeSettingIds::FACE_TRACKING;
    err = getSettingValue(key, value);

    if (!err && mCurrentImgScene.contains(key)) {
        // update local datastructure with flash setting value from cenrep.
        CX_DEBUG(( "Face Tracking setting value %d", value.toInt()));
        mCurrentImgScene[key] = value;
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Restores video settings, during mode change or during startup.
*/
void CxeSettingsModelImp::restoreVideoSettings()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;
    QVariant currentSceneInUse = mCurrentVidScene[CxeSettingIds::SCENE_ID];

    // get the video scene value from cenrep and load the scene settings
    QVariant cenrepSceneValue;
    err = getSettingValue(CxeSettingIds::VIDEO_SCENE, cenrepSceneValue);

    bool ok2LoadSceneSettings = (cenrepSceneValue != currentSceneInUse);

    if (!err && ok2LoadSceneSettings) {
        // loading video scene settings
        loadSceneData(cenrepSceneValue.toString(), mCurrentVidScene);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
