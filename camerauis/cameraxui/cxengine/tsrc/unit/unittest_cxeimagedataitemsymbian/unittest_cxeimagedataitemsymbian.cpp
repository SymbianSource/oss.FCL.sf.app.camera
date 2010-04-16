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
#include <QDate>
#include <QTest>
#include <QDebug>
#include <QSignalSpy>

#include <bitdev.h>

#include "unittest_cxeimagedataitemsymbian.h"
#include "cxeimagedataitemunit.h"
#include "cxestillimagesymbian.h"
#include "cxutils.h"


UnitTestCxeImageDataItemSymbian::UnitTestCxeImageDataItemSymbian()
: mImageDataItem(NULL)
{
}

UnitTestCxeImageDataItemSymbian::~UnitTestCxeImageDataItemSymbian()
{
    delete mImageDataItem;
}

void UnitTestCxeImageDataItemSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    TInt index = 0;
    mPath = generateImageFileName(index);
    QByteArray data = "1234";

    mImageDataItem = new CxeImageDataItemUnit(index, data, mPath);

    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SavePending);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeImageDataItemSymbian::cleanup()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mImageDataItem;
    mImageDataItem = NULL;
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeImageDataItemSymbian::testSave()
{
    CX_DEBUG_ENTER_FUNCTION();
    const int id = mImageDataItem->id();

    QSignalSpy deviceStateSpy(mImageDataItem, SIGNAL(imageSaved(CxeError::Id, const QString&, int)));
    QVERIFY(deviceStateSpy.isValid());

    int returnValue = mImageDataItem->save();

    QVERIFY(mImageDataItem->state() == CxeImageDataItem::Saved);
    QVERIFY(returnValue == KErrNone);

    QCOMPARE( deviceStateSpy.count(), 1 );
    if (deviceStateSpy.count() > 0) {
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).toInt() == CxeError::None );
        QVERIFY( initModeArguments.at(1).toString().compare(mPath) == 0);
        QVERIFY( initModeArguments.at(2).toInt() == id);
    }
    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeImageDataItemSymbian::testSaveFail()
{
    CX_DEBUG_ENTER_FUNCTION();
    int returnValue;
    int index = 1;
    QString path = NULL;
    QByteArray data = "";

    returnValue = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::Saved);
    QVERIFY(returnValue == KErrArgument);

    delete mImageDataItem;
    mImageDataItem = NULL;
    mImageDataItem = new CxeImageDataItemUnit( index, data, path );
    returnValue = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(returnValue == KErrArgument);

    delete mImageDataItem;
    mImageDataItem = NULL;
    QString filename = generateImageFileNameWithLetter(++index, "C");
    mImageDataItem = new CxeImageDataItemUnit( index, data, filename );
    returnValue = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(returnValue != KErrNone);

    qDebug() << "UnitTestCxeImageDataItemSymbian::testSaveFail3 =>";
    delete mImageDataItem;
    mImageDataItem = NULL;
    filename = generateImageFileNameWithLetter(++index, "");
    mImageDataItem = new CxeImageDataItemUnit( index, data, filename );
    returnValue = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(returnValue != KErrNone);

    qDebug() << "UnitTestCxeImageDataItemSymbian::testSaveFail4 =>";
    delete mImageDataItem;
    mImageDataItem = NULL;
    filename = generateImageFileNameWithLetter(++index, "12");
    mImageDataItem = new CxeImageDataItemUnit( index, data, filename );
    returnValue = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(returnValue != KErrNone);

    qDebug() << "UnitTestCxeImageDataItemSymbian::testSaveFail5 =>";
    delete mImageDataItem;
    mImageDataItem = NULL;
    filename = generateImageFileNameWithLetter(++index, "Edata");
    mImageDataItem = new CxeImageDataItemUnit( index, data, filename );
    returnValue = mImageDataItem->save();
    QVERIFY(mImageDataItem->state() == CxeImageDataItem::SaveFailed);
    QVERIFY(returnValue != KErrNone);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeImageDataItemSymbian::testPath()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mImageDataItem->path().compare(mPath) == 0);
}


QString UnitTestCxeImageDataItemSymbian::generateImageFileName(int counter)
{
    CX_DEBUG_ENTER_FUNCTION();
    QString monthName = QDate::currentDate().toString("yyyyMM");
    QString filename;
    filename.sprintf("E:\\Images\\Camera\\%s\\%sA0\\%04d_Nokia5800.jpg",
            monthName.toAscii().data(),
            monthName.toAscii().data(),
            counter);
    CX_DEBUG_EXIT_FUNCTION();
    return filename;
}

QString UnitTestCxeImageDataItemSymbian::generateImageFileNameWithLetter(int counter, QString letter)
{
    CX_DEBUG_ENTER_FUNCTION();
    QString monthName = QDate::currentDate().toString("yyyyMM");
    QString filename;
    filename.sprintf("%s:\\Images\\Camera\\%s\\%sA0\\%04d_Nokia5800.jpg",
            letter.toAscii().data(),
            monthName.toAscii().data(),
            monthName.toAscii().data(),
            counter);
    CX_DEBUG_EXIT_FUNCTION();
    return filename;
}

// main() function non-GUI testing
QTEST_APPLESS_MAIN(UnitTestCxeImageDataItemSymbian);
