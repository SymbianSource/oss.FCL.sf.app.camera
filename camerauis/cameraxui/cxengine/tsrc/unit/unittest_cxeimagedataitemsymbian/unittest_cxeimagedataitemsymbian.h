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
#ifndef UNITTEST_CXEIMAGEDATAITEMSYMBIAN_H
#define UNITTEST_CXEIMAGEDATAITEMSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeImageDataItemSymbian;
class CxeStillImageSymbian;

class UnitTestCxeImageDataItemSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeImageDataItemSymbian();
    ~UnitTestCxeImageDataItemSymbian();

private slots:
    void init();
    void cleanup();
    void testSave();
    void testSaveFail();
    void testPath();

private:
    QString generateImageFileName(int counter);
    QString generateImageFileNameWithLetter(int counter, QString letter);

private:
    CxeImageDataItemSymbian *mImageDataItem; // This will be recreated for each test
    QString mPath;
};

#endif // UNITTEST_CXEIMAGEDATAITEMSYMBIAN_H
