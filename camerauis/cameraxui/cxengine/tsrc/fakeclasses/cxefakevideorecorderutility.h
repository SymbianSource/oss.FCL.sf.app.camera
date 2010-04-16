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
#ifndef CXEFAKEVIDEORECORDERUTILITY_H_
#define CXEFAKEVIDEORECORDERUTILITY_H_

#include <videorecorder.h>
#include "cxevideorecorderutility.h"

class CxeFakeVideoRecorderUtility : public CxeVideoRecorderUtility
{
public:

    CxeFakeVideoRecorderUtility(MVideoRecorderUtilityObserver& aObserver,
                                       TInt aPriority=EMdaPriorityNormal,
                                       TMdaPriorityPreference aPref=EMdaPriorityPreferenceTimeAndQuality);

    ~CxeFakeVideoRecorderUtility();

    //CxeVideoRecorderUtility* NewL(MVideoRecorderUtilityObserver& aObserver,
    //                                   TInt aPriority=EMdaPriorityNormal,
    //                                   TMdaPriorityPreference aPref=EMdaPriorityPreferenceTimeAndQuality);
    TInt CustomCommandSync(const TMMFMessageDestinationPckg& aDestination,
                           TInt aFunction,
                           const TDesC8& aDataTo1,
                           const TDesC8& aDataTo2);
    void OpenFileL(const TDesC& aFileName,
					TInt aCameraHandle,
					TUid aControllerUid,
					TUid aVideoFormat,
					const TDesC8& aVideoType = KNullDesC8,
					TFourCC aAudioType = KMMFFourCCCodeNULL);
    void SetVideoFrameSizeL(TSize aSize);
    void SetVideoFrameRateL(TInt aRate);
    void SetVideoBitRateL(TInt aRate);
    void SetAudioEnabledL(TBool aEnable);
    void SetMaxClipSizeL(TInt aClipSizeInBytes);
    void Close();
    void Prepare();
    void Record();
    int Stop();
    void PauseL();
    TTimeIntervalMicroSeconds RecordTimeAvailable();
    TTimeIntervalMicroSeconds DurationL();
private:
    CxeFakeVideoRecorderUtility(MVideoRecorderUtilityObserver& aObserver);

    MVideoRecorderUtilityObserver *mObserver;
};


#endif /*CXEFAKEVIDEORECORDERUTILITY_H_*/
