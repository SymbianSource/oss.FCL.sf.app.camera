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
#ifndef CXEVIDEORECORDERUTILITYSYMBIAN_H_
#define CXEVIDEORECORDERUTILITYSYMBIAN_H_

#include <videorecorder.h>
#include "cxevideorecorderutility.h"

class CxeVideoRecorderUtilitySymbian : public CxeVideoRecorderUtility
{
public:

    CxeVideoRecorderUtilitySymbian(MVideoRecorderUtilityObserver& aObserver);
    ~CxeVideoRecorderUtilitySymbian();

    virtual void open(int cameraHandle,
                      const QString &filename,
                      const QString &fileMimeType,
                      const QString &supplier,
                      const QString &videoType,
                      const QString &audioType);
    virtual void setVideoFrameSize(const QSize& size);
    virtual void setVideoFrameRate(int rate);
    virtual void setVideoBitRate(int rate);
    virtual void setAudioEnabled(bool enabled);
    virtual void setVideoMaxSize(int sizeInBytes);
    virtual void close();
    virtual void prepare();
    virtual void record();
    virtual void stop(bool asynchronous = false);
    virtual void pause();
    virtual int availableRecordingTime();
    virtual int duration();

private:
    void findControllerL(const QString& fileMimeType,
                         const QString& supplier,
                         TUid& controllerId,
                         TUid& formatId);

    TFourCC audioFourCC(const QString& str);

private:
    CVideoRecorderUtility *mVideoRecorder;
};


#endif /*CXEVIDEORECORDERUTILITYSYMBIAN_H_*/
