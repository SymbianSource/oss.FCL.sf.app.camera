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
#ifndef CXEQUALITYPRESETSSYMBIAN_H
#define CXEQUALITYPRESETSSYMBIAN_H

//  Include Files
#include <QSize>
#include "cxequalitypresets.h"

// forward declarations
class CImagingConfigManager;
class CxeSettings;

/**!
* Class CxeQualityPresetsSymbian
* Queries supported image and video qualities from ICM and creates the related
* data and wraps up the quality settings for the client.
*/
class CxeQualityPresetsSymbian : public CxeQualityPresets
{
public:
    CxeQualityPresetsSymbian(CxeSettings &settings);
    ~CxeQualityPresetsSymbian();
    QList<CxeImageDetails> imageQualityPresets(Cxe::CameraIndex cameraId);
    QList<CxeVideoDetails> videoQualityPresets(Cxe::CameraIndex cameraId);
    qreal avgVideoBitRateScaler();

private:
    Cxe::AspectRatio calculateAspectRatio(int width, int height) const;
    QString toString(const TUint8* aData);
    CxeVideoDetails createVideoPreset(TVideoQualitySet set);
    CxeImageDetails createImagePreset(TImageQualitySet set);
    QString calculateMegaPixelCount(int imageWidth, int imageHeight);
    void debugPrints(CxeVideoDetails preset);
    void debugPrints(CxeImageDetails newPreset);

private:
    CImagingConfigManager *mIcm;
    CxeSettings &mSettings;
    qreal mCMRAvgVideoBitRateScaler;
};


#endif  // CXEQUALITYPRESETSSYMBIAN_H
