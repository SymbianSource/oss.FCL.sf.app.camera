/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QDate>
#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <QThread>

#include "cxetestutils.h"
#include "cxefakecameradevicecontrol.h"
#include "cxefakecameradevice.h"
#include "cxefakesettings.h"
#include "cxeautofocuscontrolsymbian.h"
#include "unittest_cxeautofocuscontrolsymbian.h"

UnitTestCxeAutoFocusControlSymbian::UnitTestCxeAutoFocusControlSymbian()
    : mAutoFocusControl(NULL),
      mCameraDeviceControl(NULL),
      mCameraDevice(NULL),
      mFakeSettings(NULL)
{
}

UnitTestCxeAutoFocusControlSymbian::~UnitTestCxeAutoFocusControlSymbian()
{
    cleanup();
}

// Run before each individual test case
void UnitTestCxeAutoFocusControlSymbian::init()
{
    qDebug() << "UnitTestCxeAutoFocusControlSymbian::init =>";

    mFakeSettings = new CxeFakeSettings();

    mCameraDeviceControl = new CxeFakeCameraDeviceControl();
    mCameraDevice = new CxeFakeCameraDevice();
    mCameraDevice->newCamera(mCameraDeviceControl->cameraIndex(), mCameraDeviceControl);

    mAutoFocusControl = new CxeAutoFocusControlSymbian(*mCameraDevice, *mFakeSettings);
    //mAutoFocusControl->initializeResources();

    connect(mCameraDeviceControl, SIGNAL(cameraEvent(int,int)),
            mAutoFocusControl, SLOT(handleCameraEvent(int,int)));

    qDebug() << "UnitTestCxeAutoFocusControlSymbian::init <=";
}

// Run after each individual test case
void UnitTestCxeAutoFocusControlSymbian::cleanup()
{
    qDebug() << "UnitTestCxeAutoFocusControlSymbian::cleanup =>";
    delete mAutoFocusControl;
    mAutoFocusControl = NULL;

    delete mCameraDeviceControl;
    mCameraDeviceControl = NULL;

    delete mCameraDevice;
    mCameraDevice = NULL;

    delete mFakeSettings;
    mFakeSettings = NULL;

    qDebug() << "UnitTestCxeAutoFocusControlSymbian::cleanup <=";
}


void UnitTestCxeAutoFocusControlSymbian::testStart()
{
    QVERIFY(mAutoFocusControl->state() == CxeAutoFocusControl::Unknown);
    mAutoFocusControl->start();
    QVERIFY(mAutoFocusControl->state() == CxeAutoFocusControl::InProgress);
    QVERIFY(CxeTestUtils::waitForState<CxeAutoFocusControl>(*mAutoFocusControl, CxeAutoFocusControl::Ready, 500));
}

void UnitTestCxeAutoFocusControlSymbian::testCancel()
{
    mAutoFocusControl->start();
    QVERIFY(mAutoFocusControl->state() == CxeAutoFocusControl::InProgress);
    mAutoFocusControl->cancel();
    QVERIFY(mAutoFocusControl->state() == CxeAutoFocusControl::Canceling);
    QVERIFY(CxeTestUtils::waitForState<CxeAutoFocusControl>(*mAutoFocusControl, CxeAutoFocusControl::Unknown, 500));
}

void UnitTestCxeAutoFocusControlSymbian::testMode()
{
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto);
    QVERIFY(mAutoFocusControl->mode() == CxeAutoFocusControl::Auto);

    mAutoFocusControl->setMode(CxeAutoFocusControl::Hyperfocal);
    QVERIFY(mAutoFocusControl->mode() == CxeAutoFocusControl::Hyperfocal);

    mAutoFocusControl->setMode(CxeAutoFocusControl::Macro);
    QVERIFY(mAutoFocusControl->mode() == CxeAutoFocusControl::Macro);

    mAutoFocusControl->setMode(CxeAutoFocusControl::Infinity);
    QVERIFY(mAutoFocusControl->mode() == CxeAutoFocusControl::Infinity);

    mAutoFocusControl->setMode(CxeAutoFocusControl::Portrait);
    QVERIFY(mAutoFocusControl->mode() == CxeAutoFocusControl::Portrait);
}

void UnitTestCxeAutoFocusControlSymbian::testIsFixedFocusMode()
{
    QVERIFY(mAutoFocusControl->isFixedFocusMode(CxeAutoFocusControl::Auto) == false);
    QVERIFY(mAutoFocusControl->isFixedFocusMode(CxeAutoFocusControl::Hyperfocal) == true);
    QVERIFY(mAutoFocusControl->isFixedFocusMode(CxeAutoFocusControl::Macro) == false);
    QVERIFY(mAutoFocusControl->isFixedFocusMode(CxeAutoFocusControl::Infinity) == true);
    QVERIFY(mAutoFocusControl->isFixedFocusMode(CxeAutoFocusControl::Portrait) == false);
}

void UnitTestCxeAutoFocusControlSymbian::testSupported()
{
    bool supported = mAutoFocusControl->supported();
    QVERIFY(supported);
}


// main() function - Need event loop for waiting signals,
// so can't use QTEST_APPLESS_MAIN.
QTEST_MAIN(UnitTestCxeAutoFocusControlSymbian);
