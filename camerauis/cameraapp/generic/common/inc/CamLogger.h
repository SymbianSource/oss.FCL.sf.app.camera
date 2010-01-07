/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Responsible for logging tests results to one or more files.
*
*  CCopyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
*
*
*/


#ifndef CAMLOGGER_H
#define CAMLOGGER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
#ifdef CAMERAAPP_LOGGING
enum TTestFileType { ETestFileFailed, ETestFilePassed, ETestFileLog };
#endif

// FORWARD DECLARATIONS

// CLASS DECLARATION

#ifndef CAMERAAPP_LOGGING

// Add dummy file Identifiers here, for non test mode. One for each unit test.
#define CONTAINERUNITTESTLOGID
#define TIMERUNITTESTLOGID 
#define UTILITY_UNIT_TESTS_LOG_ID
#define NAVICOUNTERUNITTESTLOGID
#define NAVIPROGRESSUNITTESTLOGID
#define FRAMEWORKUNITTESTLOGID
#define CONTROLLERUNITTESTLOGID
#define VIEWSUNITTESTLOGID
#define CAPTURESETUPUNITTESTLOGID
#define SIDEPANEUNITTESTLOGID
#define CAPTURESETUPMENUUNITTESTLOGID 
#define BURSTARRAYUNITTESTLOGID
#define BURSTCAPTUREUNITTESTLOGID
#define SETTINGSLISTSUNITTESTLOGID
#define UNITTESTUSERSCENESETUPLOGID
#define UNITTESTUTILITYLOGID
#define GENERALUNITTESTLOGID

#define CAM_LOG_DUMMYINITIALISELOGS( a, b )
#define CAM_LOG_INITIALISELOGS( a )
#define CAM_LOG_LOGHEADER( a, b )
#define CAM_LOG_LOGTESTRESULT( a, b, c )
#define CAM_LOG_LOGDETAIL( a, b, c, d )


/**
* Class that does nothing. To be used in non-test mode.
*/

#else

enum TTestResult
{
    UTStarted,
    UTFinished,
    UTFailed,
    UTNumberOfTests,
    UTAllTestsFinished
};

// Add file Identifiers here, for test code. One for each unit test.
#define TIMERUNITTESTLOGID _L("CamTimer Unit Tests ")
#define UTILITY_UNIT_TESTS_LOG_ID _L("CamUtility Unit Tests ")
#define NAVICOUNTERUNITTESTLOGID _L("CamNaviCounter Unit Tests ")
#define NAVIPROGRESSUNITTESTLOGID _L("CamNaviProgressBar Unit Tests ")
#define FRAMEWORKUNITTESTLOGID _L("Framework Unit Tests ")

#define CONTROLLERUNITTESTLOGID _L("Controller Unit Tests ")
#define CONTAINERUNITTESTLOGID _L("Container Unit Tests ")

#define SELFTIMERUNITTESTLOGID _L("Self Timer Unit Tests ")
#define BURSTARRAYUNITTESTLOGID _L("Burst Array Unit Tests ")
#define BURSTGRIDUNITTESTLOGID _L("Burst Grid Unit Tests ")
#define ZOOMPANEUNITTESTLOGID _L("Zoom Pane Unit Tests ")
#define SETTINGSLISTSUNITTESTLOGID _L("Settings Lists Units Tests ")

#define VIEWSUNITTESTLOGID _L("Views Unit Tests ")
#define SETTINGSMODELUNITTESTLOGID _L("Settings Model Unit Tests")
#define CAPTURESETUPUNITTESTLOGID _L("Capture Setup Unit Tests")
#define CAPTURESETUPMENUUNITTESTLOGID _L("Capture Setup Menu Unit Tests")
#define SIDEPANEUNITTESTLOGID _L("Side Pane Unit Tests")
#define BURSTCAPTUREUNITTESTLOGID _L("Burst Capture Unit Tests ")
#define UNITTESTUSERSCENESETUPLOGID _L("Unit Test User Scene Setup Unit Tests ")
#define UNITTESTUTILITYLOGID _L("Unit Test Utility Unit Tests ")


#define GENERALUNITTESTLOGID _L("General Unit Tests ")


#define CAM_LOG_DUMMYINITIALISELOGS( a, b ) CamLogger::InitialiseLogsL( a, b )
#define CAM_LOG_INITIALISELOGS( a ) CamLogger::InitialiseLogsL( a )
#define CAM_LOG_LOGHEADER( a, b ) CamLogger::LogHeader( a, b )
#define CAM_LOG_LOGTESTRESULT( a, b, c ) CamLogger::LogTestResult( a, b, c )
#define CAM_LOG_LOGDETAIL( a, b, c, d ) CamLogger::LogDetail( GENERALUNITTESTLOGID, a, b, c, d )

#endif // __CAM_TEST_MODE__


#endif // CAMLOGGER_H

// End of File
