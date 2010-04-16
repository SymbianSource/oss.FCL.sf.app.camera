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
#include <QDebug>
#include <QSignalSpy>

#include "cxetestutils.h"
#include "cxefakesettingsmodel.h"
#include "unittest_cxesettingsimp.h"

const char* FAIL_TEST_SETTING = "Fail test";

UnitTestCxeSettingsImp::UnitTestCxeSettingsImp() :
        mSettingsModel(NULL),
        mSettingsImp(NULL)
{
    qRegisterMetaType<CxeScene>("CxeScene");
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
    qRegisterMetaType<Cxe::CameraMode>("Cxe::CameraMode");
}

UnitTestCxeSettingsImp::~UnitTestCxeSettingsImp()
{
    cleanup();
}

void UnitTestCxeSettingsImp::init()
{
    mSettingsModel = new CxeFakeSettingsModel;
    mSettingsModel->initDefaultCameraSettings();
    mSettingsImp = new CxeSettingsImp(*mSettingsModel);
}

void UnitTestCxeSettingsImp::cleanup()
{
    delete mSettingsModel;
    mSettingsModel = NULL;

    delete mSettingsImp;
    mSettingsImp = NULL;
}

void UnitTestCxeSettingsImp::testLoadSettings()
{
    Cxe::CameraMode mode;

    /* Observe sceneChanged signal from mSettingsImp
     */
    CxeScene sigSce;
    QSignalSpy spyStateChange(mSettingsImp, SIGNAL(sceneChanged(CxeScene &)));
    QVERIFY( spyStateChange.isValid() );

    /* Test load settings in camera/video mode. Helpper method is used to get the
     * new camera mode to test loadSettings method has set right camera mode
     */
    mSettingsImp->loadSettings(Cxe::ImageMode);
    mode = mSettingsModel->getDummyCameraMode();
    QCOMPARE(mode, Cxe::ImageMode);
    QVERIFY(CxeTestUtils::waitForSignal(spyStateChange, 1000)); // verify statechanges

    mSettingsImp->loadSettings(Cxe::VideoMode);
    mode = mSettingsModel->getDummyCameraMode();
    QCOMPARE(mode, Cxe::VideoMode);
    QVERIFY(CxeTestUtils::waitForSignal(spyStateChange, 1000)); // verify statechanges
}


void UnitTestCxeSettingsImp::testGet()
{
    CxeError::Id err = CxeError::None;

    QString stringValue;
    QString expectedValue = "1";
    QVariant testValue;

    /* Test that get -method accepts key and returns correct value. Fakeclass method
     * is used to check that returned value matches with fakeclass' test values
     */
    err = mSettingsImp->get(CxeSettingIds::BRIGHTNESS, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::BRIGHTNESS, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    /* Test that returned value can be viewed as string and int, since get -method
     * may be used for both strings and integers
     */
    int num = 1;
    QCOMPARE(stringValue, expectedValue);
    QVERIFY(stringValue.toInt() == num);

    err = mSettingsImp->get(CxeSettingIds::COLOR_TONE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::CONTRAST, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::CONTRAST, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::EV_COMPENSATION_VALUE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::EV_COMPENSATION_VALUE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::EXPOSURE_MODE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::EXPOSURE_MODE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::FLASH_MODE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FLASH_MODE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::FNAME_FOLDER_SUFFIX, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_FOLDER_SUFFIX, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::FNAME_IMAGE_COUNTER, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_IMAGE_COUNTER, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::FNAME_MONTH_FOLDER, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_MONTH_FOLDER, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::FNAME_VIDEO_COUNTER, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_VIDEO_COUNTER, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::FOCAL_RANGE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FOCAL_RANGE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::FRAME_RATE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FRAME_RATE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_QUALITY, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_QUALITY, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_AUTO, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_AUTO, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_CLOSEUP, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_CLOSEUP, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_MACRO, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_MACRO, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_NIGHT, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_NIGHT, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_PORTRAIT, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_PORTRAIT, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_SCENERY, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_SCENERY, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::IMAGE_SCENE_SPORTS, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_SPORTS, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::LIGHT_SENSITIVITY, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::LIGHT_SENSITIVITY, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::SCENE_ID, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SCENE_ID, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::SECONDARY_CAMERA, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SECONDARY_CAMERA, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::SELF_TIMER, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SELF_TIMER, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::SHARPNESS, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SHARPNESS, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::VIDEO_SCENE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::VIDEO_SCENE_AUTO, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_AUTO, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::VIDEO_SCENE_LOWLIGHT, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_LOWLIGHT, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::VIDEO_SCENE_NIGHT, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_NIGHT, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::VIDEO_SCENE_NIGHTPORTRAIT, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_NIGHTPORTRAIT, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    err = mSettingsImp->get(CxeSettingIds::WHITE_BALANCE, stringValue);
    QCOMPARE(err, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::WHITE_BALANCE, testValue);
    QCOMPARE(stringValue, QString(testValue.toString()));

    /* Test error values
     */
    err = mSettingsImp->get(FAIL_TEST_SETTING , stringValue);
    QCOMPARE(err, CxeError::NotFound);
}

void UnitTestCxeSettingsImp::testGet2()
{
    QVariant value;
    QVariant reference;

    mSettingsImp->get(0, 0, Cxe::Repository, value);
    mSettingsModel->getSettingValue(0, 0, Cxe::Repository, reference);
    QCOMPARE(value, reference);

    mSettingsImp->get(0, 0, Cxe::PublishAndSubscribe, value);
    mSettingsModel->getSettingValue(0, 0, Cxe::PublishAndSubscribe, reference);
    QCOMPARE(value, reference);

}

void UnitTestCxeSettingsImp::testSet()
{
    CxeError::Id error;
    int range = 0;
    QString string = "";
    QVariant variant(1);
    QVariant checkValue;

    /* Test that signal is corresponding correctly when setting value is changed
     */
    QList<QVariant> spyArguments;
    QSignalSpy signalSpy(mSettingsImp, SIGNAL(settingValueChanged(QString, QVariant)));
    QVERIFY( signalSpy.isValid() );

    error = mSettingsImp->set(CxeSettingIds::BRIGHTNESS, range);
    QCOMPARE(error, CxeError::None);
    QVERIFY(CxeTestUtils::waitForSignal(signalSpy, 1000)); // verify statechanges

    spyArguments = signalSpy.takeFirst();
    QCOMPARE(spyArguments.at(0).toString(), QString(CxeSettingIds::BRIGHTNESS));
    QCOMPARE(spyArguments.at(1).toInt(), range);

    /* Test parameters
     */
    error = mSettingsImp->set(CxeSettingIds::BRIGHTNESS, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::BRIGHTNESS, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneNormal);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ColortoneNormal));

    error = mSettingsImp->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneSepia);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ColortoneSepia));

    error = mSettingsImp->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneNegative);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ColortoneNegative));

    error = mSettingsImp->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneBlackAndWhite);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ColortoneBlackAndWhite));

    error = mSettingsImp->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneVivid);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ColortoneVivid));

    error = mSettingsImp->set(CxeSettingIds::CONTRAST, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::CONTRAST, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::EV_COMPENSATION_VALUE, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::EV_COMPENSATION_VALUE, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::EXPOSURE_MODE, Cxe::ExposureAuto);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::EXPOSURE_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ExposureAuto));

    error = mSettingsImp->set(CxeSettingIds::EXPOSURE_MODE, Cxe::ExposureBacklight);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::EXPOSURE_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ExposureBacklight));

    error = mSettingsImp->set(CxeSettingIds::EXPOSURE_MODE, Cxe::ExposureNight);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::EXPOSURE_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ExposureNight));

    error = mSettingsImp->set(CxeSettingIds::EXPOSURE_MODE, Cxe::ExposureSport);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::EXPOSURE_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::ExposureSport));

    error = mSettingsImp->set(CxeSettingIds::FLASH_MODE, Cxe::FlashAntiRedEye);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FLASH_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::FlashAntiRedEye));

    error = mSettingsImp->set(CxeSettingIds::FLASH_MODE, Cxe::FlashAuto);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FLASH_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::FlashAuto));

    error = mSettingsImp->set(CxeSettingIds::FLASH_MODE, Cxe::FlashOff);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FLASH_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::FlashOff));

    error = mSettingsImp->set(CxeSettingIds::FLASH_MODE, Cxe::FlashOn);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FLASH_MODE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::FlashOn));

    error = mSettingsImp->set(CxeSettingIds::FNAME_FOLDER_SUFFIX, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_FOLDER_SUFFIX, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::FNAME_IMAGE_COUNTER, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_IMAGE_COUNTER, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::FNAME_MONTH_FOLDER, string);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_MONTH_FOLDER, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::FNAME_VIDEO_COUNTER, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FNAME_VIDEO_COUNTER, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::FOCAL_RANGE, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::FOCAL_RANGE, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_QUALITY,  range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_QUALITY, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE,  range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_AUTO,  range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_AUTO, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_CLOSEUP,  range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_CLOSEUP, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_MACRO,  range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_MACRO, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_NIGHT, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_NIGHT, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_PORTRAIT, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_PORTRAIT, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_SCENERY, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_SCENERY, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_SPORTS, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::IMAGE_SCENE_SPORTS, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::LIGHT_SENSITIVITY, Cxe::LightSensitivityAutomatic);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::LIGHT_SENSITIVITY, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::LightSensitivityAutomatic));

    error = mSettingsImp->set(CxeSettingIds::LIGHT_SENSITIVITY, Cxe::LightSensitivityHigh);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::LIGHT_SENSITIVITY, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::LightSensitivityHigh));

    error = mSettingsImp->set(CxeSettingIds::LIGHT_SENSITIVITY, Cxe::LightSensitivityLow);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::LIGHT_SENSITIVITY, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::LightSensitivityLow));

    error = mSettingsImp->set(CxeSettingIds::LIGHT_SENSITIVITY, Cxe::LightSensitivityMedium);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::LIGHT_SENSITIVITY, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::LightSensitivityMedium));

    error = mSettingsImp->set(CxeSettingIds::SCENE_ID, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SCENE_ID, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::SECONDARY_CAMERA, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SECONDARY_CAMERA, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::SELF_TIMER, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SELF_TIMER, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::SHARPNESS, Cxe::SharpnessHard);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SHARPNESS, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::SharpnessHard));

    error = mSettingsImp->set(CxeSettingIds::SHARPNESS, Cxe::SharpnessNormal);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SHARPNESS, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::SharpnessNormal));

    error = mSettingsImp->set(CxeSettingIds::SHARPNESS, Cxe::SharpnessSoft);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::SHARPNESS, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::SharpnessSoft));

    error = mSettingsImp->set(CxeSettingIds::VIDEO_SCENE, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::VIDEO_SCENE_AUTO, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_AUTO, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::VIDEO_SCENE_LOWLIGHT, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_LOWLIGHT, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::VIDEO_SCENE_NIGHT, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_NIGHT, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::VIDEO_SCENE_NIGHTPORTRAIT, range);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::VIDEO_SCENE_NIGHTPORTRAIT, checkValue);
    QCOMPARE(int(checkValue.toInt()), range);

    error = mSettingsImp->set(CxeSettingIds::WHITE_BALANCE, Cxe::WhitebalanceAutomatic);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::WHITE_BALANCE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::WhitebalanceAutomatic));

    error = mSettingsImp->set(CxeSettingIds::WHITE_BALANCE, Cxe::WhitebalanceCloudy);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::WHITE_BALANCE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::WhitebalanceCloudy));

    error = mSettingsImp->set(CxeSettingIds::WHITE_BALANCE, Cxe::WhitebalanceFluorescent);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::WHITE_BALANCE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::WhitebalanceFluorescent));

    error = mSettingsImp->set(CxeSettingIds::WHITE_BALANCE, Cxe::WhitebalanceIncandescent);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::WHITE_BALANCE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::WhitebalanceIncandescent));

    error = mSettingsImp->set(CxeSettingIds::WHITE_BALANCE, Cxe::WhitebalanceSunny);
    QCOMPARE(error, CxeError::None);
    mSettingsModel->getSettingValue(CxeSettingIds::WHITE_BALANCE, checkValue);
    QCOMPARE(int(checkValue.toInt()), ((int)Cxe::WhitebalanceSunny));

    /* Test fail parameter
     */
    error = mSettingsImp->set(FAIL_TEST_SETTING, Cxe::ColortoneNormal);
    QCOMPARE(error, CxeError::NotFound);
}

// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeSettingsImp)

