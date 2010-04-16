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

#include "unittest_cxefeaturemanagerimp.h"
#include "cxefakesettingsmodel.h"
#include "cxefeaturemanagerimp.h"

UnitTestCxeFeatureManager::UnitTestCxeFeatureManager()
: mFakeSettingsModel(NULL), mFeatureManager(NULL)
{
}

UnitTestCxeFeatureManager::~UnitTestCxeFeatureManager()
{

}

// Run before each individual test case
void UnitTestCxeFeatureManager::init()
{
    mFakeSettingsModel = new CxeFakeSettingsModel();
    mFeatureManager = new CxeFeatureManagerImp(*mFakeSettingsModel);
}

// Run after each individual test case
void UnitTestCxeFeatureManager::cleanup()
{
    delete mFakeSettingsModel;
    mFakeSettingsModel = 0;

    delete mFeatureManager;
    mFeatureManager = 0;
}

// Testing that isFeatureSupported function returns correct values
void UnitTestCxeFeatureManager::testSupportedKeys()
{
    // Create a key that won't be added to test data
    QString falseKey = QString("testKeyFalse");
    bool isSupported;
    CxeError::Id err;

    // Test that key that is not added to test data is not found
    // also boolean value isSupported should return false as key is not found
    err = mFeatureManager->isFeatureSupported(falseKey, isSupported);
    QVERIFY(!isSupported);
    QCOMPARE(err, CxeError::NotFound);

    // create and set test data with "testKeyTrue" key
    QString trueKey = QString("testKeyTrue");
    QList<QVariant> value;
    value.append(1);
    value.append(2);
    value.append(3);
    mFakeSettingsModel->set(trueKey, value);

    // the false key should return the same values as before adding true key to test data
    err = mFeatureManager->isFeatureSupported(falseKey, isSupported);
    QVERIFY(!isSupported);
    QCOMPARE(err, CxeError::NotFound);

    // check that just added key can be found from test data and no error occurs
    err = mFeatureManager->isFeatureSupported(trueKey, isSupported);
    QVERIFY(isSupported);
    QCOMPARE(err, CxeError::None);

}

// test that configuredValues function returns a correct error code and not any values
// with any arbitrary key when there is no data set
void UnitTestCxeFeatureManager::testConfiguredValuesEmptyData()
{
    QString falseKey = QString("testKeyFalse");
    QList<int> values;
    CxeError::Id err;

    // no data
    err = mFeatureManager->configuredValues(falseKey,values);
    QCOMPARE(err, CxeError::NotFound);
    QCOMPARE(values.count(), 0);

}

// test that configuredValues function returns a correct error code and not any values
// with any arbitrary key when there is data set but not for false key
void UnitTestCxeFeatureManager::testConfiguredValuesFalseKey()
{
    QString falseKey = QString("testKeyFalse");
    QString trueKey = QString("testKeyTrue");
    QList<QVariant> value;
    value.append(1);
    value.append(2);
    value.append(3);
    mFakeSettingsModel->set(trueKey, value);

    CxeError::Id err;
    QList<int> values;

    // false key
    err = mFeatureManager->configuredValues(falseKey, values);
    QCOMPARE(err, CxeError::NotFound);
    // as key is not found values list should be empty
    QCOMPARE(values.count(), 0);
}

// test data is set with the same key that is used for fetching configured values
// Now error code should be None as the key should be found from the data
// Also value count should match with the amount of set values
void UnitTestCxeFeatureManager::testConfiguredValuesTrueKeyAndData()
{
    QString trueKey = QString("testKeyTrue");
    QList<QVariant> value;
    // values added to key
    value.append(1);
    value.append(2);
    value.append(3);
    mFakeSettingsModel->set(trueKey, value);

    CxeError::Id err;
    QList<int> values;

    // true key
    err = mFeatureManager->configuredValues(trueKey, values);
    QCOMPARE(err, CxeError::None);
    QCOMPARE(values.count(), 3);
}

// Currently only integer type values can be fetched with configuredValues-function.
// Test that proper error code is returned when data values are something else than integers
void UnitTestCxeFeatureManager::testConfiguredValuesFalseData()
{
    QString trueKey = QString("testKeyTrue");
    QList<QVariant> value;
    value.append(1);
    value.append(2);
    value.append(true); // add boolean value which is not supported
    mFakeSettingsModel->set(trueKey, value);

    CxeError::Id err;
    QList<int> values;

    // getting configured values should fail as boolean values are not supported
    err = mFeatureManager->configuredValues(trueKey, values);
    QCOMPARE(err, CxeError::NotSupported);

}

// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeFeatureManager);
