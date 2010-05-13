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
/*
* Symbian specific settings handling
*/

#include <QCoreApplication>
#include <QVariant>
#include <QList>
#include <QMetaType>
#include <ecam.h>
#include <ecamadvsettingsuids.hrh>
#include <ecamadvsettings.h>
#include <QObject>

#include "cxesettingsmodel.h"
#include "cxesettingsimp.h"
#include "cxesettings.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeerror.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxesettingsimpTraces.h"
#endif




/*!
    Load image/video specific settings during mode change or startup
*/
void CxeSettingsImp::loadSettings(Cxe::CameraMode mode)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESETTINGSIMP_LOADSETTINGS_IN, "msg: e_CX_SETTINGS_LOADSETTINGS 1");


    // inform the settings model for the change in mode.
    mSettingsModel.cameraModeChanged(mode);

    if (mode == Cxe::ImageMode) {
        emit sceneChanged(mSettingsModel.currentImageScene());
    } else {
        emit sceneChanged(mSettingsModel.currentVideoScene());
    }

    OstTrace0(camerax_performance, CXESETTINGSIMP_LOADSETTINGS_OUT, "msg: e_CX_SETTINGS_LOADSETTINGS 0");
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
    Return the current integer setting value for the given key
*/
CxeError::Id CxeSettingsImp::get(const QString& key, int& value) const
{
    CX_DEBUG_ENTER_FUNCTION();

    QVariant v;
    CxeError::Id err = mSettingsModel.getSettingValue(key, v);

    bool isInt;
    value = v.toInt(&isInt); // 0 denotes base, check the API

    if (isInt) {
        CX_DEBUG(("CxeSettingsImp::get - key: %s value: %d",
                  key.toAscii().data(), value ));
    } else {
        err = CxeError::NotSupported;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}




/*!
    Return the current real setting value for the given key
*/
CxeError::Id CxeSettingsImp::get(const QString &key, qreal &value) const
{
    CX_DEBUG_ENTER_FUNCTION();

    QVariant v;
    CxeError::Id err = mSettingsModel.getSettingValue(key, v);

    bool isReal;
    value = v.toReal(&isReal);

    if (isReal) {
        CX_DEBUG(("CxeSettingsImp::get - key: %s value: %f",
                  key.toAscii().data(), value ));
    } else {
        err = CxeError::NotSupported;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
    Return the current string setting value for the given key
*/
CxeError::Id CxeSettingsImp::get(
        const QString& key, QString &stringValue) const
{
    CX_DEBUG_ENTER_FUNCTION();

    QVariant value;
    CxeError::Id err = getSceneMode(key, stringValue);

    if (err == CxeError::NotFound) {
        // read from settings store
        err = mSettingsModel.getSettingValue(key, value);
        stringValue = value.toString();
    }

    CX_DEBUG(("CxeSettingsImp::get - key: %s value: %s",
              key.toAscii().data(), stringValue.toAscii().data()));
    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*
* Reads a value from cenrep
* @param key   - setting key
* @param uid   - setting UID of the component that owns the setting key
* @param type  - the type of key cr key or P&S key (constantly monitoring value)
* @param value - setting value read from cenrep
*/

void CxeSettingsImp::get(long int uid,
                         unsigned long int key,
                         Cxe::SettingKeyType type,
                         QVariant &value) const
{
    CX_DEBUG_ENTER_FUNCTION();

    mSettingsModel.getSettingValue(uid, key, type, value);

    CX_DEBUG_EXIT_FUNCTION();

}



/*!
    Get the current scene mode setting value for the given key
*/
CxeError::Id CxeSettingsImp::getSceneMode(
        const QString& key, QString &stringValue) const
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;
    CxeScene scene;

    if(CxeSettingIds::IMAGE_SCENE == key) {
         scene = mSettingsModel.currentImageScene();
    } else if(CxeSettingIds::VIDEO_SCENE == key) {
        scene = mSettingsModel.currentVideoScene();
    } else {
        err = CxeError::NotFound;
    }

    if (err == CxeError::None) {
        stringValue = scene["sceneId"].toString();
    }

    CX_DEBUG(("CxeSettingsImp::get - key: %s value: %s",
              key.toAscii().data(), stringValue.toAscii().data()));

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
    Set new int value for the given key
*/
CxeError::Id CxeSettingsImp::set(const QString& key,int newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("CxeSettingsImp::set - key: %s value: %d",
              key.toAscii().data(), newValue));

    CxeError::Id err = mSettingsModel.set(key, newValue);
    emit settingValueChanged(key, newValue);

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*!
    Set new int value for the given key
*/
CxeError::Id CxeSettingsImp::set(const QString& key,qreal newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("CxeSettingsImp::set - key: %s value: %f",
              key.toAscii().data(), newValue));

    CxeError::Id err = mSettingsModel.set(key, newValue);
    emit settingValueChanged(key, newValue);

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
    Set new string value for the given key
*/
CxeError::Id CxeSettingsImp::set(const QString& key,const QString& newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("CxeSettingsImp::set - key: %s value: %s",
              key.toAscii().data(), newValue.toAscii().data()));

    CxeError::Id err = setSceneMode(key, newValue);

    if (err == CxeError::NotFound) {
        // not scene mode setting, try setting value to settings store
        mSettingsModel.set(key, newValue);
        emit settingValueChanged(key, newValue);
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*!
    Set the current scene mode setting value for the given key
*/
CxeError::Id CxeSettingsImp::setSceneMode(
        const QString& key,const QString& newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("CxeSettingsImp::set - key: %s value: %s",
              key.toAscii().data(), newValue.toAscii().data()));

    CxeError::Id err = CxeError::None;
    CxeScene scene;

    if(CxeSettingIds::IMAGE_SCENE == key) {
        err = mSettingsModel.setImageScene(newValue);
        scene = mSettingsModel.currentImageScene();
    } else if(CxeSettingIds::VIDEO_SCENE == key) {
        err = mSettingsModel.setVideoScene(newValue);
        scene = mSettingsModel.currentVideoScene();
    } else {
        err = CxeError::NotFound;
    }

    if (err == CxeError::None) {
        // scene mode set, inform clients about scene mode change
        emit sceneChanged(scene);
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}

/*!
    Reset static settings (persistent settings)
*/
void CxeSettingsImp::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    mSettingsModel.reset();

    CX_DEBUG_EXIT_FUNCTION();
}



/*
* CxeSettingsImp::CxeSettingsImp
*/
CxeSettingsImp::CxeSettingsImp(CxeSettingsModel& settingsModel)
: mSettingsModel(settingsModel)
{
    CX_DEBUG_IN_FUNCTION();
}



/*
* CxeSettingsImp::close
*/
CxeSettingsImp::~CxeSettingsImp()
{
    CX_DEBUG_IN_FUNCTION();
}

// end of file
