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
#include <QSignalSpy>

#include "unittest_cxethumbnailmanagersymbian.h"
#include "cxethumbnailmanagersymbian.h"
#include "cxutils.h"

UnitTestCxeThumbnailManagerSymbian::UnitTestCxeThumbnailManagerSymbian()
{
}


UnitTestCxeThumbnailManagerSymbian::~UnitTestCxeThumbnailManagerSymbian()
{
    cleanup();
}


void UnitTestCxeThumbnailManagerSymbian::init()
{
    mThumbnailManager = new CxeThumbnailManagerSymbian();
}

void UnitTestCxeThumbnailManagerSymbian::cleanup()
{
    delete mThumbnailManager;
    mThumbnailManager = NULL;
}

void UnitTestCxeThumbnailManagerSymbian::testCreateThumbnail()
{
    CX_DEBUG_ENTER_FUNCTION();

    QSignalSpy thumbnailReadySpy(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, int)));
    QString filename;

    QVERIFY(thumbnailReadySpy.isValid()); 

    // case 1: testing with wrong dummy file, we should get an error code with thumbnailready
    // since filename is invalid
    mThumbnailManager->createThumbnail(filename, QImage());

    // we should not get any call back when we have invalid file name.
    QCOMPARE(thumbnailReadySpy.count(), 0);


    // case 1: testing with proper file name, we shouldnt get an error code with thumbnailready
    // since filename is valid
    filename = QString("c:\\test.jpg");    
    mThumbnailManager->createThumbnail(filename, QImage());

    QTest::qWait(1500);
 
    QCOMPARE( thumbnailReadySpy.count(), 1 );
    if (thumbnailReadySpy.count() > 0) {
        QList<QVariant> initModeArguments = thumbnailReadySpy.takeFirst();
        // we are only interested in error code in this case 1
        QCOMPARE(initModeArguments.at(1).toInt(), KErrNone);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeThumbnailManagerSymbian::testCancelThumbnail()
{
    CX_DEBUG_ENTER_FUNCTION();

    mThumbnailManager->cancelThumbnail(QString("filename"));

    CX_DEBUG_EXIT_FUNCTION();
}

// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeThumbnailManagerSymbian);

// End of file
