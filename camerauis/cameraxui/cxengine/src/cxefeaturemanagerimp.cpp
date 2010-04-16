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

#include "cxesettingsmodel.h"
#include "cxefeaturemanagerimp.h"
#include "cxutils.h"
#include "cxeerror.h"


/*
* CxeFeatureManagerImp::isFeatureSupported
*/
CxeError::Id CxeFeatureManagerImp::isFeatureSupported(const QString& key, bool& supported) const
{
    CX_DEBUG_ENTER_FUNCTION();

    QVariant value;
    CxeError::Id err = mSettingsModel.getRuntimeValue(key, value);
    if (err == CxeError::None) {
        QList<QVariant> values = qVariantValue<QList<QVariant> >(value);
        supported = values[0].toInt();
    } else {
        supported = false;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*
* CxeFeatureManagerImp::configuredValues
*/
CxeError::Id CxeFeatureManagerImp::configuredValues(const QString& key,QList<int>& values)
{
    CX_DEBUG_ENTER_FUNCTION();

    values.clear();

    QVariant variant;
    CxeError::Id err = mSettingsModel.getRuntimeValue(key, variant);

    if(CxeError::None == err) {
        QVariantList list;
        list = qVariantValue<QVariantList>(variant);

        CX_DEBUG( ("CxeFeatureManagerImp::configuredValues <> count: %d",list.count() ) );

        foreach (QVariant value, list ) {
            // go through the settings list.
            QString dataString = value.toString();
            bool isInt;
            int intValue = dataString.toInt(&isInt,0); // 0 denotes base, check the API

            if (isInt) {
                CX_DEBUG( ("CxeFeatureManagerImp::configuredValues <> value = %d",intValue) );
                values.append(intValue);
            } else {
                err = CxeError::NotSupported;
            }
        }
        list.clear();
    }

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}


/*
*CxeFeatureManagerImp::CxeFeatureManagerImp
*/
CxeFeatureManagerImp::CxeFeatureManagerImp(CxeSettingsModel& settingsModel)
: mSettingsModel(settingsModel)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


/*
*CxeFeatureManagerImp::~CxeFeatureManagerImp
*/
CxeFeatureManagerImp::~CxeFeatureManagerImp()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


// end of file
