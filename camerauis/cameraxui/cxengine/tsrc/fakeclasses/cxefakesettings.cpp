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

#include "cxefakesettings.h"

CxeFakeSettings::CxeFakeSettings()
{
}

CxeFakeSettings::~CxeFakeSettings()
{
}

CxeError::Id CxeFakeSettings::get(const QString& key, int& value) const
{
    value = mSettingKeyHash[key].toInt();
    return CxeError::None;
}

CxeError::Id CxeFakeSettings::get(const QString& key, QString &stringValue) const
{
    stringValue = mSettingKeyHash[key].toString();
    return CxeError::None;
}



CxeError::Id CxeFakeSettings::get(const QString &key, qreal &value) const
{
    value = mSettingKeyHash[key].toReal();
    return CxeError::None;
}


void CxeFakeSettings::get(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value) const
{
    Q_UNUSED(uid);
    Q_UNUSED(key);
    Q_UNUSED(type);
    Q_UNUSED(value);
}


CxeError::Id CxeFakeSettings::set(const QString& key, int newValue)
{
    mSettingKeyHash[key] = QVariant(newValue);
    emit settingValueChanged(key, newValue);
    return CxeError::None;
}

CxeError::Id CxeFakeSettings::set(const QString &key, const QString &newValue)
{
    mSettingKeyHash[key] = QVariant(newValue);
    emit settingValueChanged(key, newValue);
    return CxeError::None;
}



CxeError::Id CxeFakeSettings::set(const QString &key, qreal newValue)
{
    mSettingKeyHash[key] = QVariant(newValue);
    emit settingValueChanged(key, newValue);
    return CxeError::None;
}


void CxeFakeSettings::reset()
{
}


void CxeFakeSettings::emulate(long int uid, unsigned long int key, QVariant value)
{
    emit settingValueChanged(uid, key, value);
}
