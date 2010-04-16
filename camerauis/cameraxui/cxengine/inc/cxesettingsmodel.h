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
#ifndef  CXESETTINGSMODEL_H_
#define  CXESETTINGSMODEL_H_

//  Include Files
#include <QVariant>
#include "cxeerror.h"
#include "cxenamespace.h"

/*
* Settings model class  handles all settings data i.e. loading, saving, getting settings data based on the settings key.
*/
class CxeSettingsModel
{

public:
    
    virtual ~CxeSettingsModel() {};
    
    /*
    * resets and reload settings that qualify reseting
    * return CxeEngine specific error code
    */
    virtual void reset() = 0;

    /*
    * Get setting value associated with the key.
    * @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
    * @Param value - contains the value associated with the key.
    * @returns CxeError::None if successful or any CxeError specific error code.
    */
    virtual CxeError::Id getSettingValue(const QString &key, QVariant &value) = 0;

    /*
    * Get setting value associated with the key.
    * @param uid - UID of component that owns the setting key
    * @Param key - key id of the setting
    * @param type - the type of key
    * @Param value - contains the value associated with the key.
    */
    virtual void getSettingValue(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value) = 0;

    /*
    * Get the configured run-time value associated with the key.
    * @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
    * @Param value - contains the value associated with the key.
    * @returns CxeError::None if successful or any CxeError specific error code.
    */
    virtual CxeError::Id getRuntimeValue(const QString &key, QVariant &value) = 0;

    /*
    * Set a value to the key.
    * @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
    * @Param value - contains the value associated with the key.
    * @returns CxeError::None if successful or any CxeError specific error code.
    */
    virtual CxeError::Id set(const QString &key,const QVariant newValue) = 0;


    /*
    * Returns the current image scene mode.
    */
    virtual CxeScene& currentImageScene() = 0;

    /*
    * Returns the current video scene mode.
    */
    virtual CxeScene& currentVideoScene() = 0;

    /*
    * Updating settings model whenever mode is changed from image to video and vice-versa.
    */
    virtual void cameraModeChanged(Cxe::CameraMode newMode) = 0;

    /*
    * Set a new image scene mode
    * @returns CxeError::None if successful or any CxeError specific error code.
    */
    virtual CxeError::Id setImageScene(const QString &newScene) = 0;

    /*
    * Set a new video scene mode
    * @returns CxeError::None if successful or any CxeError specific error code.
    */
    virtual CxeError::Id setVideoScene(const QString &newScene) = 0;

};

#endif  // CXESETTINGSMODEL_H_

