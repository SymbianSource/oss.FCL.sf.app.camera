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
#include "cxeenginesymbianunit.h"

#include "cxefakecameradevicecontrol.h"
#include "cxefakesettings.h"
#include "cxefakefeaturemanager.h"
#include "cxefakeautofocuscontrol.h"
#include "cxefakestillcapturecontrol.h"
#include "cxefakevideocapturecontrol.h"
#include "cxefakezoomcontrol.h"
#include "cxefakeviewfindercontrol.h"
#include "cxefakesensoreventhandler.h"
#include "cxefakefilenamegenerator.h"
#include "cxutils.h"

//  Member Functions

CxeEngineSymbianUnit::CxeEngineSymbianUnit()
{
}

CxeEngineSymbianUnit::~CxeEngineSymbianUnit()
{
}


/*!
    Create all control classes
*/
void CxeEngineSymbianUnit::createControls()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mVideoCaptureControl) {
        mCameraDeviceControl = new CxeFakeCameraDeviceControl();

        mSettings = new CxeFakeSettings();

        mFeatureManager = new CxeFakeFeatureManager();

        mSensorEventHandler = new CxeFakeSensorEventHandler();

        mFilenameGenerator = new CxeFakeFilenameGenerator();

        mViewfinderControl = new CxeFakeViewfinderControl();

        mStillCaptureControl = new CxeFakeStillCaptureControl();

        mAutoFocusControl = new CxeFakeAutoFocusControl();

        mZoomControl = new CxeFakeZoomControl();

        mVideoCaptureControl = new CxeFakeVideoCaptureControl();

    }

    CX_DEBUG_EXIT_FUNCTION();
}

// End of file
