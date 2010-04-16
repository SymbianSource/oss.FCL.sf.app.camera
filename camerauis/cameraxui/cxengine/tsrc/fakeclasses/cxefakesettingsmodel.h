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
#ifndef CXEFAKESETTINGSMODEL_H
#define CXEFAKESETTINGSMODEL_H

#include <QMap>
#include "cxesettingsmodel.h"

class CxeFakeSettingsModel  : public CxeSettingsModel
{

public:

    CxeFakeSettingsModel();
    ~CxeFakeSettingsModel();

    void reset() {};
    CxeError::Id getSettingValue(const QString &key, QVariant &value);
    void getSettingValue(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value);
    CxeError::Id getRuntimeValue(const QString &key, QVariant &value);

    CxeError::Id set(const QString &key,const QVariant newValue);
    CxeScene& currentImageScene() {return mDummyScene;}
    CxeScene& currentVideoScene() {return mDummyScene;}
    void cameraModeChanged(Cxe::CameraMode newMode) {mDummyCameraMode = newMode;}
    CxeError::Id setImageScene(const QString &newScene);
    CxeError::Id setVideoScene(const QString &newScene);

public: // own helper methods
    void setRuntimeValues(QString &key, QList<QVariant> value);
    void initDefaultCameraSettings();
    void testSetCurrenImageScene();
    void testSetCurrenVideoScene();

    Cxe::CameraMode getDummyCameraMode() {return mDummyCameraMode;}

private:

    QMap<QString, QList<QVariant> > mStore;
    QHash<QString, QVariant> mSettingStore;
    CxeScene mDummyScene;
    Cxe::CameraMode mDummyCameraMode;
};

#endif  // CXEFAKESETTINGSMODEL_H





