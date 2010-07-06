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

#include <QVariant>
#include <QList>
#include <QHash>

#include "cxenamespace.h"
#include "cxefakesettingsstore.h"

CxeFakeSettingsStore::CxeFakeSettingsStore()
{
    // all supported settings are initialized here.
    mSettingKeyHash.clear();
    mSettingKeyHash.insert(CxeSettingIds::FNAME_FOLDER_SUFFIX, QVariant("_Nokia"));
    mSettingKeyHash.insert(CxeSettingIds::FNAME_MONTH_FOLDER,  QVariant("08042009"));
    mSettingKeyHash.insert(CxeSettingIds::FNAME_IMAGE_COUNTER, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::FNAME_VIDEO_COUNTER, QVariant(0));
    
    mSettingKeyHash.insert(CxeSettingIds::CAMERA_MODE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::FLASH_MODE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::IMAGE_SCENE,  QVariant("image_scene_auto"));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_SCENE, QVariant("video_scene_auto"));
    mSettingKeyHash.insert(CxeSettingIds::STILL_SHOWCAPTURED, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_SHOWCAPTURED, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_MUTE_SETTING,  QVariant(0));
    
    mSettingKeyHash.insert(CxeSettingIds::GEOTAGGING, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::GEOTAGGING_DISCLAIMER, QVariant(1));
    mSettingKeyHash.insert(CxeSettingIds::FACE_TRACKING, QVariant(1));
    mSettingKeyHash.insert(CxeSettingIds::IMAGE_QUALITY,  QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_QUALITY, QVariant(0));

}

CxeFakeSettingsStore::~CxeFakeSettingsStore()
{
}

/*!
* Reads a value from cenrep
* @param "key"   - setting key
* @param "value" - setting value read from cenrep
*/
CxeError::Id CxeFakeSettingsStore::get(const QString& key, QVariant &value)
{
    CxeError::Id error = CxeError::None;
    if(mSettingKeyHash.contains(key)) {
        value = mSettingKeyHash[key];
    } else if(mRuntimeKeyHash.contains(key)) {
        value = mRuntimeKeyHash[key];
    } else {
        error = CxeError::NotFound;
    }

    return error;
}

/*!
* Reads a value from cenrep
* @param "uid"   - UID of the component that own setting key
* @param "key"   - setting key id
* @param "type"  - type of setting key
* @param "value" - setting value read from cenrep
*/
void CxeFakeSettingsStore::startMonitoring(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value)
{
    Q_UNUSED(uid);
    Q_UNUSED(key);
    Q_UNUSED(type);
    Q_UNUSED(value);

    // no support yet
}

/*!
* Reads/loads all run-time settings values from cenrep
* @param QList<QString> contains list of all runtime key ids which we use to load values from cenrep.
* returns: QHash container, "contains" values associated with each key that are read from cenrep
* NOTE: loading runtime settings should be done only ONCE at start-up.
*/
QHash<QString, QVariantList> CxeFakeSettingsStore::loadRuntimeSettings(QList<QString>& keylist)
{
    QVariant data;
    CxeError::Id err = CxeError::None;
    QVariantList list;
    QHash<QString, QVariantList> settings;
    mRuntimeKeyHash.clear();

    foreach (QString key, keylist) {

        // before we read from get function we set values to the key
        mRuntimeKeyHash.insert(key, QVariant(1));

        // read the data from cenrep
        err = get(key, data);
        // clear the list
        list.clear();
        if (CxeError::None == err) {
           list.append(data);
        }
        // append the values associated with the key to the list.
        settings.insert(key, list);
    } // end for

    return settings;
}


/*!
* Sets a new value to cenrep
* @param "key"   - setting key
* @param "newValue" - new value set to the key in cenrep
*/
CxeError::Id CxeFakeSettingsStore::set(const QString& key, const QVariant newValue)
{
    CxeError::Id error = CxeError::NotFound;
    if(mSettingKeyHash.contains(key)) {
       mSettingKeyHash[key] = newValue;
       error =  CxeError::None;
    }
    return error;
}

/*!
* resets the cenrep store
*/
void CxeFakeSettingsStore::reset()
{
}

