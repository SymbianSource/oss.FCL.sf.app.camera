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
#ifndef CXEFAKESETTINGS_H
#define CXEFAKESETTINGS_H

#include "cxesettings.h"
#include "cxeerror.h"

class CxeFakeSettings : public CxeSettings
{

    Q_OBJECT

    public:
        CxeFakeSettings();
        ~CxeFakeSettings();

    public: // from CxeSettings

        CxeError::Id get(const QString &key, int &value) const;
        CxeError::Id get(const QString &key, QString &stringValue) const;
        void get(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value) const;
        CxeError::Id set(const QString &key, int newValue);
        CxeError::Id set(const QString &key,const QString &newValue);
        CxeError::Id get(const QString &key, qreal &value) const;
        CxeError::Id set(const QString &key, qreal newValue);
        void reset();

    public: // methods for unit testing
        // ??

    private: // data
        QHash<QString, QVariant> mSettingKeyHash;
        QHash<QString, unsigned long int> mRuntimeKeyHash;

    private:
        Q_DISABLE_COPY(CxeFakeSettings)
};

#endif /*CXEFAKESETTINGS_H*/

// end  of file
