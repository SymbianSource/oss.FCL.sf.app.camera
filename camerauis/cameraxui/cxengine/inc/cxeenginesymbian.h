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
#ifndef CXEENGINESYMBIAN_H
#define CXEENGINESYMBIAN_H

#include <QObject>
#include "cxeengine.h"
#include "cxefilenamegenerator.h"

class CxeCameraDeviceControl;
class CxeViewfinderControl;
class CxeStillCaptureControl;
class CxeVideoCaptureControl;
class CxeSettingsControlSymbian;
class CxeAutoFocusControl;
class CxeFeatureManager;
class CxeAutoFocusControl;
class CxeSettings;
class CxeFeatureManager;
class CxeSettingsModel;
class CxeSensorEventHandler;
class CxeFilenameGenerator;
class CxeQualityPresets;
class CxeFileSaveThread;



class CxeEngineSymbian : public CxeEngine
{
    Q_OBJECT
public:
    CxeEngineSymbian();
    virtual ~CxeEngineSymbian();

    void construct();

    CxeCameraDeviceControl &cameraDeviceControl();
    CxeViewfinderControl &viewfinderControl();
    CxeStillCaptureControl &stillCaptureControl();
    CxeVideoCaptureControl &videoCaptureControl();
    CxeAutoFocusControl &autoFocusControl();
    CxeZoomControl &zoomControl();
    CxeSettings &settings();
    CxeSensorEventHandler &sensorEventHandler();
    CxeFeatureManager &featureManager();
    Cxe::CameraMode mode() const;
    void initMode(Cxe::CameraMode cameraMode);
    bool isEngineReady();

protected:
    virtual void createControls();

private slots:
    void doInit();

private:
    void connectSignals();
    bool reserveNeeded();
    bool initNeeded();
    bool startViewfinderNeeded();

protected:
    CxeCameraDeviceControl *mCameraDeviceControl;
    CxeViewfinderControl *mViewfinderControl;
    CxeStillCaptureControl *mStillCaptureControl;
    CxeVideoCaptureControl *mVideoCaptureControl;
    CxeSettingsControlSymbian *mSettingsControl;
    CxeAutoFocusControl *mAutoFocusControl;
    CxeZoomControl *mZoomControl;
    CxeSettings *mSettings;
    CxeFeatureManager *mFeatureManager;
    CxeSettingsModel *mSettingsModel;
    CxeFilenameGenerator *mFilenameGenerator;
    CxeSensorEventHandler* mSensorEventHandler;
    CxeQualityPresets *mQualityPresets;
    CxeFileSaveThread *mFileSaveThread;
};


#endif  // CXEENGINESYMBIAN_H

