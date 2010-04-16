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
#include "cxevideorecorderutilitysymbian.h"
#include "cxenamespace.h"
#include "cxutils.h"


CxeVideoRecorderUtilitySymbian::~CxeVideoRecorderUtilitySymbian()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mVideoRecorder;
    mVideoRecorder = NULL;
    CX_DEBUG_EXIT_FUNCTION();
}

CxeVideoRecorderUtilitySymbian::CxeVideoRecorderUtilitySymbian(MVideoRecorderUtilityObserver& aObserver,
                              TInt aPriority,
                              TMdaPriorityPreference aPref)
{
    CX_DEBUG_ENTER_FUNCTION();
    TRAPD(initErr, mVideoRecorder = CVideoRecorderUtility::NewL(aObserver, aPriority, aPref));
    mStartuperror = initErr;
    CX_DEBUG_EXIT_FUNCTION();
}

TInt CxeVideoRecorderUtilitySymbian::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination,
                       TInt aFunction,
                       const TDesC8& aDataTo1,
                       const TDesC8& aDataTo2)
{
    CX_DEBUG_IN_FUNCTION();
    return mVideoRecorder->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
}

void CxeVideoRecorderUtilitySymbian::OpenFileL(const TDesC& aFileName,
				TInt aCameraHandle,
				TUid aControllerUid,
				TUid aVideoFormat,
				const TDesC8& aVideoType,
				TFourCC aAudioType)
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->OpenFileL(aFileName, aCameraHandle, aControllerUid, aVideoFormat, aVideoType, aAudioType);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::SetVideoFrameSizeL(TSize aSize)
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->SetVideoFrameSizeL(aSize);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::SetVideoFrameRateL(TInt aRate)
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->SetVideoFrameRateL(aRate);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::SetVideoBitRateL(TInt aRate)
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->SetVideoBitRateL(aRate);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::SetAudioEnabledL(TBool aEnable)
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->SetAudioEnabledL(aEnable);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::SetMaxClipSizeL(TInt aClipSizeInBytes)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (aClipSizeInBytes <= 0) {
        aClipSizeInBytes = KMMFNoMaxClipSize;
    }
    mVideoRecorder->SetMaxClipSizeL(aClipSizeInBytes);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::Close()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->Close();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::Prepare()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->Prepare();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoRecorderUtilitySymbian::Record()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->Record();
    CX_DEBUG_EXIT_FUNCTION();
}

int CxeVideoRecorderUtilitySymbian::Stop()
{
    CX_DEBUG_IN_FUNCTION();
    return mVideoRecorder->Stop();
}

void CxeVideoRecorderUtilitySymbian::PauseL()
{
    CX_DEBUG_ENTER_FUNCTION();
    mVideoRecorder->PauseL();
    CX_DEBUG_EXIT_FUNCTION();
}

TTimeIntervalMicroSeconds CxeVideoRecorderUtilitySymbian::RecordTimeAvailable()
{
    CX_DEBUG_IN_FUNCTION();
    return mVideoRecorder->RecordTimeAvailable();
}

TTimeIntervalMicroSeconds CxeVideoRecorderUtilitySymbian::DurationL()
{
    CX_DEBUG_IN_FUNCTION();
    return mVideoRecorder->DurationL();
}



// end of file
