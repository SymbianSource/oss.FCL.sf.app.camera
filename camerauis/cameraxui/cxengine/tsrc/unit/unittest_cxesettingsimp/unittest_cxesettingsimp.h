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
#ifndef UNITTEST_CXESETTINGSIMP_H
#define UNITTEST_CXESETTINGSIMP_H

#include <QObject>
#include <QMetaType>

#include "cxesettingsimp.h"

class CxeFakeSettingsModel;
class CxeSettingsImp;

class UnitTestCxeSettingsImp : public QObject
{
Q_OBJECT

public:
    UnitTestCxeSettingsImp();
    virtual ~UnitTestCxeSettingsImp();

private slots:
    void init();
    void cleanup();

    void testGet();
    void testGet2();
    void testSet();
    void testLoadSettings();

private:

    CxeFakeSettingsModel *mSettingsModel;
    CxeSettingsImp *mSettingsImp;
};

Q_DECLARE_METATYPE(CxeScene)

#endif

