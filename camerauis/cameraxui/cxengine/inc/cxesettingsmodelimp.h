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

#ifndef CXESETTINGSMODELIMP_H
#define CXESETTINGSMODELIMP_H

//  Include Files
#include <QHash>
#include <QVariant>
#include <QMetaType>
#include "cxesettingsmodel.h"
#include "cxenamespace.h"
#include "cxeerror.h"

// forward declarations
class QVariant;
class CxeSettingsStore;

/*
* Settings model class  handles all settings data i.e. loading, saving, getting settings data based on the settings key.
*/
class CxeSettingsModelImp : public CxeSettingsModel
{

public:

    CxeSettingsModelImp(CxeSettingsStore *settingsStore);
    ~CxeSettingsModelImp();

public:
  
    void reset();
    CxeError::Id getSettingValue(const QString &key, QVariant &value);
    void getSettingValue(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value);
    CxeError::Id getRuntimeValue(const QString &key, QVariant &value);
    CxeError::Id set(const QString &key, const QVariant newValue);
    CxeScene& currentImageScene();
    CxeScene& currentVideoScene();
    void cameraModeChanged(Cxe::CameraMode newMode);
    CxeError::Id setImageScene(const QString& newScene);
    CxeError::Id setVideoScene(const QString& newScene);

private:
    
    void init();
    void loadRuntimeSettings();

    void loadImageScenes();
    void loadVideoScenes();

    void loadSceneData(CxeScene &currentScene, CxeScene &sceneDefaultSettings);
    CxeError::Id sceneSettingValue(const QString &key, QVariant &value);
    CxeError::Id setSceneSettingValue(const QString &key, QVariant newValue);

    void supportedKeys(QList<QString> &runtimeKeys);

    CxeError::Id videoScene(const QString &sceneId, CxeScene &sceneSettings);
    CxeError::Id imageScene(const QString &sceneId, CxeScene &sceneSettings);

private:

    CxeSettingsStore *mSettingStore;
    QHash<QString, QVariantList> mRuntimeSettings;

    QHash<QString, CxeScene> mImageSceneModes;
    QHash<QString, CxeScene> mVideoSceneModes;

    CxeScene mCurrentImgScene;
    CxeScene mCurrentVidScene;

    Cxe::CameraMode mCameraMode;
};

Q_DECLARE_METATYPE(QVariantList)
Q_DECLARE_METATYPE(CxeScene)

#endif /* CXESETTINGSMODELIMP_H */
