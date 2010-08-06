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

#include "cxefakesettings.h"


CxeFakeSettings::CxeFakeSettings()
{
}

CxeFakeSettings::~CxeFakeSettings()
{
}


void CxeFakeSettings::get(long int uid,
         unsigned long int key,
         Cxe::SettingKeyType type,
         QVariant &value) const
{
    Q_UNUSED(uid);
    Q_UNUSED(key);
    Q_UNUSED(type);
    Q_UNUSED(value);
}

bool CxeFakeSettings::listenForSetting(const QString &settingKey, QObject *target, const char *slot)
{
    Q_UNUSED(settingKey);
    Q_UNUSED(target);
    Q_UNUSED(slot);
    return true;
}

CxeError::Id CxeFakeSettings::getVariationValue(const QString &key, QVariant &value)
{
    int variation = mVariationKeyHash[key];
    value = QVariant(variation);
    return CxeError::None;
}

void CxeFakeSettings::getValue(const QString &key, QVariant &value) const
{
    value = mSettingKeyHash[key];
}

void CxeFakeSettings::setValue(const QString &key, const QVariant &newValue)
{
    mSettingKeyHash[key] = newValue;
    emit settingValueChanged(key, newValue);
}



void CxeFakeSettings::reset()
{
}


void CxeFakeSettings::emulate(long int uid, unsigned long int key, QVariant value)
{
    emit settingValueChanged(uid, key, value);
}
