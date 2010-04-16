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

#include <QList>
#include "cxefakesettings.h"
#include "unittest_cxequalitypresetssymbian.h"
#include "cxequalitypresetssymbian.h"
#include "cxequalitydetails.h"
#include "cxutils.h"

UnitTestCxeQualityPresetsSymbian::UnitTestCxeQualityPresetsSymbian()
{
}


UnitTestCxeQualityPresetsSymbian::~UnitTestCxeQualityPresetsSymbian()
{
    cleanup();
}


void UnitTestCxeQualityPresetsSymbian::init()
{
    mSettings = new CxeFakeSettings();
    mQualityPresets = new CxeQualityPresetsSymbian(*mSettings);
}

void UnitTestCxeQualityPresetsSymbian::cleanup()
{
    delete mQualityPresets;
    mQualityPresets = NULL;

    delete mSettings;
    mSettings = NULL;
}

void UnitTestCxeQualityPresetsSymbian::testImageQualityPresets()
{
    CX_DEBUG_ENTER_FUNCTION();

    QList<CxeImageDetails> imagePresets =
        mQualityPresets->imageQualityPresets(Cxe::PrimaryCameraIndex);

    QVERIFY(imagePresets.count() > 0);

    QCOMPARE(imagePresets[0].mWidth, 0);
    QCOMPARE(imagePresets[0].mHeight, 0);

    imagePresets.clear();
    imagePresets = mQualityPresets->imageQualityPresets(Cxe::SecondaryCameraIndex);
    QVERIFY(imagePresets.count() > 0);

    QCOMPARE(imagePresets[0].mWidth, 0);
    QCOMPARE(imagePresets[0].mHeight, 0);

    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeQualityPresetsSymbian::testVideoQualityPresets()
{
    CX_DEBUG_ENTER_FUNCTION();

    QList<CxeVideoDetails> videoPresets =
        mQualityPresets->videoQualityPresets(Cxe::PrimaryCameraIndex);

    QVERIFY(videoPresets.count() > 0);

    QCOMPARE(videoPresets[0].mWidth, 640);
    QCOMPARE(videoPresets[0].mHeight, 360);

    videoPresets.clear();
    videoPresets = mQualityPresets->videoQualityPresets(Cxe::SecondaryCameraIndex);
    QVERIFY(videoPresets.count() > 0);

    QCOMPARE(videoPresets[0].mWidth, 640);
    QCOMPARE(videoPresets[0].mHeight, 360);

    CX_DEBUG_EXIT_FUNCTION();
}

// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeQualityPresetsSymbian);

// End of file
