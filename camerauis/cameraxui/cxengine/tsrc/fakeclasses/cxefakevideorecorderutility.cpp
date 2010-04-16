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
#include <QTest>
#include "cxetestutils.h"
#include "cxefakevideorecorderutility.h"

CxeFakeVideoRecorderUtility::CxeFakeVideoRecorderUtility(MVideoRecorderUtilityObserver& aObserver)
    : mObserver( &aObserver )
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeVideoRecorderUtility::~CxeFakeVideoRecorderUtility()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeVideoRecorderUtility::CxeFakeVideoRecorderUtility(MVideoRecorderUtilityObserver& aObserver,
                              TInt aPriority,
                              TMdaPriorityPreference aPref)
                              : mObserver( &aObserver )
{
    CX_DEBUG_IN_FUNCTION();
    //return new (ELeave) CxeFakeVideoRecorderUtility(aObserver);
}

/*CxeVideoRecorderUtility* CxeFakeVideoRecorderUtility::NewL(MVideoRecorderUtilityObserver& aObserver,
                              TInt aPriority,
                              TMdaPriorityPreference aPref)
{
    CX_DEBUG_IN_FUNCTION();
    return new (ELeave) CxeFakeVideoRecorderUtility(aObserver);
}*/

TInt CxeFakeVideoRecorderUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination,
                       TInt aFunction,
                       const TDesC8& aDataTo1,
                       const TDesC8& aDataTo2)
{
    CX_DEBUG_IN_FUNCTION();
    return KErrNone;
}

void CxeFakeVideoRecorderUtility::OpenFileL(const TDesC& aFileName,
				TInt aCameraHandle,
				TUid aControllerUid,
				TUid aVideoFormat,
				const TDesC8& aVideoType,
				TFourCC aAudioType)
{
    CX_DEBUG_ENTER_FUNCTION();
    mObserver->MvruoOpenComplete(KErrNone);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeVideoRecorderUtility::SetVideoFrameSizeL(TSize aSize)
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::SetVideoFrameRateL(TInt aRate)
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::SetVideoBitRateL(TInt aRate)
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::SetAudioEnabledL(TBool aEnable)
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::SetMaxClipSizeL(TInt aClipSizeInBytes)
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::Close()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::Prepare()
{
    CX_DEBUG_ENTER_FUNCTION();
    mObserver->MvruoPrepareComplete(KErrNone);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeVideoRecorderUtility::Record()
{
    CX_DEBUG_IN_FUNCTION();
}

int CxeFakeVideoRecorderUtility::Stop()
{
    CX_DEBUG_IN_FUNCTION();
    mObserver->MvruoRecordComplete(KErrNone);
    return KErrNone;
}

void CxeFakeVideoRecorderUtility::PauseL()
{
    CX_DEBUG_IN_FUNCTION();
}

TTimeIntervalMicroSeconds CxeFakeVideoRecorderUtility::RecordTimeAvailable()
{
    CX_DEBUG_IN_FUNCTION();
    // 100 s
    return TTimeIntervalMicroSeconds(100000000);
}

TTimeIntervalMicroSeconds CxeFakeVideoRecorderUtility::DurationL()
{
    CX_DEBUG_IN_FUNCTION();
    // 10 s
    return TTimeIntervalMicroSeconds(10000000);
}



// end of file
