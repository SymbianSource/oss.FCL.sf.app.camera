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
#ifndef CXEVIDEORECORDERUTILITY_H_
#define CXEVIDEORECORDERUTILITY_H_

#include <QObject>

// interface class for usage of CVideoRecorderUtility
class CxeVideoRecorderUtility
{
public:

    virtual ~CxeVideoRecorderUtility() {}

    virtual TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination,
                           TInt aFunction,
                           const TDesC8& aDataTo1,
                           const TDesC8& aDataTo2) = 0;
    virtual void OpenFileL(const TDesC& aFileName,
					TInt aCameraHandle,
					TUid aControllerUid,
					TUid aVideoFormat,
					const TDesC8& aVideoType = KNullDesC8,
					TFourCC aAudioType = KMMFFourCCCodeNULL) = 0;
    virtual void SetVideoFrameSizeL(TSize aSize) = 0;
    virtual void SetVideoFrameRateL(TInt aRate) = 0;
    virtual void SetVideoBitRateL(TInt aRate) = 0;
    virtual void SetAudioEnabledL(TBool aEnable) = 0;
    virtual void SetMaxClipSizeL(TInt aClipSizeInBytes) = 0;
    virtual void Close() = 0;
    virtual void Prepare() = 0;
    virtual void Record() = 0;
    virtual int Stop() = 0;
    virtual void PauseL() = 0;
    virtual TTimeIntervalMicroSeconds RecordTimeAvailable() = 0;
    virtual TTimeIntervalMicroSeconds DurationL() = 0;

protected:
    CxeVideoRecorderUtility() {}

private:
    Q_DISABLE_COPY( CxeVideoRecorderUtility )
};


#endif /*CXEVIDEORECORDERUTILITY_H_*/
