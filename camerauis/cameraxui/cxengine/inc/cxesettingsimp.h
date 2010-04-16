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
#ifndef CXESETTINGSIMP_H
#define CXESETTINGSIMP_H

#include "cxesettings.h"
#include "cxeerror.h"


// forward declaration
class CxeSettingsModel;


/*
* Class to access all kind of Settings
*/
class CxeSettingsImp : public CxeSettings
{
    Q_OBJECT

public:

    CxeSettingsImp(CxeSettingsModel &settingsModel);
    virtual ~CxeSettingsImp();

    CxeError::Id get(const QString &key, int &value) const;
    CxeError::Id get(const QString &key, QString &stringValue) const;
    CxeError::Id get(const QString &key, qreal &value) const;
    void get(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value) const;
    CxeError::Id set(const QString &key, int newValue);
    CxeError::Id set(const QString &key,const QString &newValue);
    CxeError::Id set(const QString &key, qreal newValue);
    void reset();

public slots:
    void loadSettings(Cxe::CameraMode mode);

private: // helper methods
    CxeError::Id getSceneMode(const QString &key, QString &stringValue) const;
    CxeError::Id setSceneMode(const QString &key, const QString &stringValue);

private:
    CxeSettingsModel &mSettingsModel; // not owned

};

#endif // CXESETTINGSIMP_H

// end  of file
