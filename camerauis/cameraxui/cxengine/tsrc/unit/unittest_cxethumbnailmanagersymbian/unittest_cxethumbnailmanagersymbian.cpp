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

    QString filename;

    // case 1: testing with wrong dummy file, we should get an error code with thumbnailready
    // since filename is invalid
    mThumbnailManager->createThumbnail(filename, QImage());


    // case 1: testing with proper file name, we shouldnt get an error code with thumbnailready
    // since filename is valid
    filename = QString("c:\\test.jpg");    
    mThumbnailManager->createThumbnail(filename, QImage());

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
