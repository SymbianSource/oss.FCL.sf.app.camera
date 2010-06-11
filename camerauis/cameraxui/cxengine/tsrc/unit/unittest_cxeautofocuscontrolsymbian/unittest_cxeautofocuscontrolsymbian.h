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
#ifndef UNITTEST_CXEAUTOFOCUSCONTROLSYMBIAN_H
#define UNITTEST_CXEAUTOFOCUSCONTROLSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeAutoFocusControlSymbian;
class CxeFakeCameraDeviceControl;
class CxeFakeCameraDevice;

class UnitTestCxeAutoFocusControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeAutoFocusControlSymbian();
    ~UnitTestCxeAutoFocusControlSymbian();

private slots:
    void init();
    void cleanup();

    void testStart();
    void testCancel();
    void testMode();
    void testIsFixedFocusMode();

    void testSupported();

private:
    // Helper methods for test cases

private:

    // Need to test with derived class to hide missing ECAM.
    CxeAutoFocusControlSymbian *mAutoFocusControl;
    CxeFakeCameraDeviceControl *mCameraDeviceControl;
    CxeFakeCameraDevice *mCameraDevice;
};

#endif // UNITTEST_CXEAUTOFOCUSCONTROLSYMBIAN_H
