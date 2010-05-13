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

#include "cxefakesettingsmodel.h"
#include "cxutils.h"

CxeFakeSettingsModel::CxeFakeSettingsModel()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeSettingsModel::~CxeFakeSettingsModel()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeError::Id CxeFakeSettingsModel::getRuntimeValue( const QString &key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeError::Id err = CxeError::None;

    // read run-time configuration value
    if (mStore.contains(key))  {
        value = qVariantFromValue<QList<QVariant> > (mStore[key]);
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}

CxeError::Id CxeFakeSettingsModel::getSettingValue( const QString &key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeError::Id err = CxeError::None;

    // read run-time configuration value
    if (mSettingStore.contains(key))  {
        value = mSettingStore[key];
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}

void CxeFakeSettingsModel::getSettingValue(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value)
{
    Q_UNUSED(uid);
    Q_UNUSED(key);
    Q_UNUSED(type);
    Q_UNUSED(value);

    // no support yet
    CX_DEBUG_IN_FUNCTION();
}


/* This is a helper method for this fake class that can be used to set key-value pairs
 * to local mStore database. Key-value pairs don't have to refer any real values used
 * in camera application as the fake class is used only for testing purposes
 */
CxeError::Id CxeFakeSettingsModel::set(const QString &key, QVariant value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeError::Id error = CxeError::None;

    if(mSettingStore.contains(key)) {
        mSettingStore.insert(key, value);
    } else {
        error = CxeError::NotFound;
    }



    CX_DEBUG_EXIT_FUNCTION();
    return error;
}

CxeError::Id CxeFakeSettingsModel::setImageScene(const QString &newScene)
{
    mDummyScene.clear();

    // image scene mode key values
    if(newScene == CxeSettingIds::IMAGE_SCENE_AUTO) {
        mDummyScene.insert(CxeSettingIds::IMAGE_SCENE, CxeSettingIds::IMAGE_SCENE_AUTO);
    }
    else if(newScene == CxeSettingIds::IMAGE_SCENE_PORTRAIT) {
        mDummyScene.insert(CxeSettingIds::IMAGE_SCENE, CxeSettingIds::IMAGE_SCENE_PORTRAIT);
    }
    else if(newScene == CxeSettingIds::IMAGE_SCENE_SCENERY) {
        mDummyScene.insert(CxeSettingIds::IMAGE_SCENE, CxeSettingIds::IMAGE_SCENE_SCENERY);
    }
    else if(newScene == CxeSettingIds::IMAGE_SCENE_MACRO) {
        mDummyScene.insert(CxeSettingIds::IMAGE_SCENE, CxeSettingIds::IMAGE_SCENE_MACRO);
    }
    else if(newScene == CxeSettingIds::IMAGE_SCENE_SPORTS) {
        mDummyScene.insert(CxeSettingIds::IMAGE_SCENE, CxeSettingIds::IMAGE_SCENE_SPORTS);
    }
    else if(newScene == CxeSettingIds::IMAGE_SCENE_NIGHT) {
        mDummyScene.insert(CxeSettingIds::IMAGE_SCENE, CxeSettingIds::IMAGE_SCENE_NIGHT);
    }
    else if(newScene == CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT) {
        mDummyScene.insert(CxeSettingIds::IMAGE_SCENE, CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT);
    }
    else {
        return CxeError::NotFound;
    }

    return CxeError::None;
}

CxeError::Id CxeFakeSettingsModel::setVideoScene(const QString &newScene)
{
    mDummyScene.clear();

    // image scene mode key values
    if(newScene == CxeSettingIds::VIDEO_SCENE_AUTO) {
        mDummyScene.insert(CxeSettingIds::VIDEO_SCENE, CxeSettingIds::VIDEO_SCENE_AUTO);
    }
    else if(newScene == CxeSettingIds::VIDEO_SCENE_NIGHTPORTRAIT) {
        mDummyScene.insert(CxeSettingIds::VIDEO_SCENE, CxeSettingIds::VIDEO_SCENE_NIGHTPORTRAIT);
    }
    else if(newScene == CxeSettingIds::VIDEO_SCENE_LOWLIGHT) {
        mDummyScene.insert(CxeSettingIds::VIDEO_SCENE, CxeSettingIds::VIDEO_SCENE_LOWLIGHT);
    }
    else if(newScene == CxeSettingIds::VIDEO_SCENE_NIGHT) {
        mDummyScene.insert(CxeSettingIds::VIDEO_SCENE, CxeSettingIds::VIDEO_SCENE_NIGHT);
    }
    else {
        return CxeError::NotFound;
    }

    return CxeError::None;
}


/* This helper method initializes fake store for default camera settings
 */
void CxeFakeSettingsModel::initDefaultCameraSettings()
{
    CX_DEBUG_ENTER_FUNCTION();
    mSettingStore.insert(CxeSettingIds::FNAME_FOLDER_SUFFIX, QVariant(1));
    mSettingStore.insert(CxeSettingIds::FNAME_MONTH_FOLDER,  QVariant(1));
    mSettingStore.insert(CxeSettingIds::FNAME_IMAGE_COUNTER, QVariant(1));
    mSettingStore.insert(CxeSettingIds::FNAME_VIDEO_COUNTER, QVariant(1));
    mSettingStore.insert(CxeSettingIds::COLOR_TONE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::SHARPNESS, QVariant(1));
    mSettingStore.insert(CxeSettingIds::CONTRAST, QVariant(1));
    mSettingStore.insert(CxeSettingIds::BRIGHTNESS,   QVariant(1));
    mSettingStore.insert(CxeSettingIds::EV_COMPENSATION_VALUE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::EXPOSURE_MODE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::FLASH_MODE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::FOCAL_RANGE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::FRAME_RATE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_QUALITY, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE_AUTO, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE_MACRO, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE_NIGHT, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE_PORTRAIT, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE_SCENERY, QVariant(1));
    mSettingStore.insert(CxeSettingIds::IMAGE_SCENE_SPORTS, QVariant(1));
    mSettingStore.insert(CxeSettingIds::LIGHT_SENSITIVITY, QVariant(1));
    mSettingStore.insert(CxeSettingIds::SCENE_ID, QVariant(1));
    mSettingStore.insert(CxeSettingIds::SECONDARY_CAMERA, QVariant(1));
    mSettingStore.insert(CxeSettingIds::SELF_TIMER, QVariant(1));
    mSettingStore.insert(CxeSettingIds::VIDEO_SCENE, QVariant(1));
    mSettingStore.insert(CxeSettingIds::VIDEO_SCENE_AUTO, QVariant(1));
    mSettingStore.insert(CxeSettingIds::VIDEO_SCENE_LOWLIGHT, QVariant(1));
    mSettingStore.insert(CxeSettingIds::VIDEO_SCENE_NIGHT, QVariant(1));
    mSettingStore.insert(CxeSettingIds::VIDEO_SCENE_NIGHTPORTRAIT, QVariant(1));
    mSettingStore.insert(CxeSettingIds::WHITE_BALANCE, QVariant(1));
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeSettingsModel::testSetCurrenImageScene()
{
    mDummyScene.insert(CxeSettingIds::SCENE_ID, CxeSettingIds::IMAGE_SCENE);
}

void CxeFakeSettingsModel::testSetCurrenVideoScene()
{
    mDummyScene.insert(CxeSettingIds::SCENE_ID, CxeSettingIds::VIDEO_SCENE);
}


// end of file
