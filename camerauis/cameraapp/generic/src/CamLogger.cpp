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
* Description:  Implements logging of test data.*
*/



// INCLUDE FILES
#include "CamLogger.h"
#include <f32file.h>
#include <Flogger.h>

// CONSTANTS
const TInt KMaxFileNameLength = 50;

// FORWARD DECLARATIONS
//#ifdef CAMERAAPP_TEST

#ifdef CAMERAAPP_UNIT_TEST_SETTINGS_LIST_TESTER
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // CAMERAAPP_UNIT_TEST_SETTINGS_LIST_TESTER

#ifdef CAMERAAPP_UNIT_TEST
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // CAMERAAPP_UNIT_TEST

#ifdef CAPTURESETUP_UNIT_TEST
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // CAMERAAPP_UNIT_TEST

#ifdef CAMERAAPP_INTEGRATION_TEST
_LIT( KDirectory, "Integration Tests\\Output" );
#endif // CAMERAAPP_INTEGRATION_TEST

#ifdef CAMERAAPP_FRAMEWORK_UNIT_TEST
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // CAMERAAPP_FRAMEWORK_UNIT_TEST

#ifdef CAPTURE_SETUP_MENU_UNIT_TEST
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // CAPTURE_SETUP_MENU_UNIT_TEST

#ifdef SETUP_PANE_UNIT_TEST
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // SETUP_PANE_UNIT_TEST

#ifdef CAMERAAPP_BURST_UNIT_TEST
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // CAMERAAPP_BURST_UNIT_TEST

#ifdef CAMERAAPP_UNIT_TEST_USER_SCENE_SETUP
_LIT( KDirectory, "Unit Tests\\Output" );
#endif // CAMERAAPP_UNIT_TEST_USER_SCENE_SETUP

// ============================ STATIC FUNCTIONS ===============================

// ---------------------------------------------------------
// CamLogger::InitialiseLogs
// Initialises the files for writing to.
// Necessary, otherwise previous output will remain in logs.
// ---------------------------------------------------------
//
void CamLogger::InitialiseLogsL( const TDesC& aIdentifier, TBool aIgnore )
    {
	if ( !aIgnore )
		{
		// Create the directory for log files, if not already created.
		_LIT( KUnitTestsLogPath, "C:\\Logs\\Unit Tests\\Output\\" );
		RFs rfs;
		User::LeaveIfError( rfs.Connect() );
		CleanupClosePushL( rfs );
		TInt err = rfs.MkDirAll( KUnitTestsLogPath );
		// If there was an error and it wasn't that the directory already exists
		// then leave.
		if ( ( err != KErrNone ) && ( err != KErrAlreadyExists ) )
			{
			User::Leave( err );
			}
		CleanupStack::PopAndDestroy( &rfs );

		// Overwrite contents of passed file.
		TBuf<KMaxFileNameLength> passedFileName;
		GetPassedFileName( aIdentifier, passedFileName );
		RFileLogger::Write( KDirectory, passedFileName, EFileLoggingModeOverwrite, KNullDesC );

		// Overwrite contents of failed file.
		TBuf<KMaxFileNameLength> failedFileName;
		GetFailedFileName( aIdentifier, failedFileName );
		RFileLogger::Write( KDirectory, failedFileName, EFileLoggingModeOverwrite, KNullDesC );

		// Overwrite contents of log file.
		TBuf<KMaxFileNameLength> logFileName;
		GetLogFileName( aIdentifier, logFileName );
		RFileLogger::Write( KDirectory, logFileName, EFileLoggingModeOverwrite, KNullDesC );
		}
    }

// ---------------------------------------------------------
// CamLogger::LogHeader
// Writes header info to all files identified by aIdentifier.
// ---------------------------------------------------------
//
void CamLogger::LogHeader( const TDesC& aIdentifier, const TDesC& aHeader )
    {
    // Header divider to be written under the header content.
    _LIT( KHeaderDivider, "=====================================================" );

    // Write header to passed file.
    TBuf<KMaxFileNameLength> passedFileName;
    GetPassedFileName( aIdentifier, passedFileName );
    RFileLogger::Write( KDirectory, passedFileName, EFileLoggingModeAppend, aHeader );
    RFileLogger::Write( KDirectory, passedFileName, EFileLoggingModeAppend, KHeaderDivider );

    // Write header to failed file.
    TBuf<KMaxFileNameLength> failedFileName;
    GetFailedFileName( aIdentifier, failedFileName );
    RFileLogger::Write( KDirectory, failedFileName, EFileLoggingModeAppend, aHeader ); 
    RFileLogger::Write( KDirectory, failedFileName, EFileLoggingModeAppend, KHeaderDivider ); 

    // Write header to log file.
    TBuf<KMaxFileNameLength> logFileName;
    GetLogFileName( aIdentifier, logFileName );
    RFileLogger::Write( KDirectory, logFileName, EFileLoggingModeAppend, aHeader ); 
    RFileLogger::Write( KDirectory, logFileName, EFileLoggingModeAppend, KHeaderDivider ); 
    }

// ---------------------------------------------------------
// CamLogger::LogTestResult
// Writes results of a test to either the passed, failed or log file.
// ---------------------------------------------------------
//
void CamLogger::LogTestResult( const TDesC& aIdentifier, 
    TTestFileType aExpr, const TDesC& aTestDesc )
    {
    const TInt KMaxTestDescLength = 256;
    // Evaluate expression (aExpr), and write the test description to either the
    // passed.txt or failed.txt or log.txt file, depending on result.   
    if ( aExpr == ETestFilePassed )
        {
        // Create text
        _LIT( KPassedTest, "Test passed: " );
        TBuf<KMaxTestDescLength> desc( KPassedTest );
        desc.Append( aTestDesc );

        // Write text to passed file.
        TBuf<KMaxFileNameLength> passedFileName;
        GetPassedFileName( aIdentifier, passedFileName );
        RFileLogger::Write( KDirectory, passedFileName, EFileLoggingModeAppend, desc );
        }
    else if ( aExpr == ETestFileFailed )
        {
        // Create text
        _LIT( KFailedTest, "Test failed: " );
        TBuf<KMaxTestDescLength> desc( KFailedTest );
        desc.Append( aTestDesc );

        // Write text to failed file.
        TBuf<KMaxFileNameLength> failedFileName;
        GetFailedFileName( aIdentifier, failedFileName );
        RFileLogger::Write( KDirectory, failedFileName, EFileLoggingModeAppend, desc ); 
        }
    else
        {
        // Write text to log file.
        TBuf<KMaxFileNameLength> logFileName;
        GetLogFileName( aIdentifier, logFileName );
        RFileLogger::Write( KDirectory, logFileName, EFileLoggingModeAppend, aTestDesc ); 
        }
    }

void CamLogger::LogDetail( const TDesC& aIdentifier,
            const TDesC&    aUnitTestId,
            TInt            aTestId,
            TTestResult     aResult,
            const TDesC&    aDetail )
	{
    // Write detail to log file.
    const TInt KMaxTestDescLength = 256;
    TBuf<KMaxFileNameLength> logFileName;
    GetLogFileName( aIdentifier, logFileName );

    TBuf<KMaxTestDescLength> LogString;

    TBuf<KMaxTestDescLength> Result;
    switch (aResult)
    {
    case UTStarted:
        {
        _LIT(KUTStarted, "Started");
        Result = KUTStarted;
        LogString.Format(_L("UT:%S,T%03d,%S,%S"),
            &aUnitTestId,
            static_cast<TInt>(aTestId),
            &Result,
            &aDetail);
        }
        break;
    case UTFinished:
        {
        _LIT(KUTFinished, "Finished");
        Result = KUTFinished;
        LogString.Format(_L("UT:%S,T%03d,%S,%S"),
            &aUnitTestId,
            static_cast<TInt>(aTestId),
            &Result,
            &aDetail);
        }
        break;
    case UTFailed:
        {
        _LIT(KUTFailed, "Failed");
        Result = KUTFailed;
        LogString.Format(_L("UT:%S,T%03d,%S,%S"),
            &aUnitTestId,
            static_cast<TInt>(aTestId),
            &Result,
            &aDetail);
        }
        break;
    case UTNumberOfTests:
        {
        _LIT(KUTNumberOfTests, "NoOfTests");
        Result = KUTNumberOfTests;
        LogString.Format(_L("UT:%S,%S,%d"),
            &aUnitTestId,
            &Result,
            static_cast<TInt>(aTestId));
        }
        break;
    case UTAllTestsFinished:
        {
        _LIT(KUTAllTestsFinished, "AllTestsFinished");
        Result = KUTAllTestsFinished;
        LogString.Format(_L("UT:%S,%S"),
            &aUnitTestId,
            &Result);
        }
        break;
    default:
        {
        LogString.Format(_L("UT:Invalid Log"));
        }
        break;
    }

    RFileLogger::Write( KDirectory, logFileName, EFileLoggingModeAppend, LogString ); 
	}

// ---------------------------------------------------------
// CamLogger::GetFailedFileName
// Returns the file name of failed test results, for a specific identifer.
// ---------------------------------------------------------
//
void CamLogger::GetFailedFileName( const TDesC& aIdentifier, TDes& aFileName )
    {
    _LIT( KFailedPostfix, "Failed.txt" );
    aFileName = aIdentifier;
    aFileName.Append( KFailedPostfix );
    }

// ---------------------------------------------------------
// CamLogger::GetPassedFileName
// Returns the file name of passed test results, for a specific identifer.
// ---------------------------------------------------------
//
void CamLogger::GetPassedFileName( const TDesC& aIdentifier, TDes& aFileName )
    {
    _LIT( KPassedPostfix, "Passed.txt" );
    aFileName = aIdentifier;
    aFileName.Append( KPassedPostfix );
    }

// ---------------------------------------------------------
// CamLogger::GetLogFileName
// Returns the file name of log test results, for a specific identifer.
// ---------------------------------------------------------
//
void CamLogger::GetLogFileName( const TDesC& aIdentifier, TDes& aFileName )
    {
    _LIT( KLogPostfix, "Log.txt" );
    aFileName = aIdentifier;
    aFileName.Append( KLogPostfix );
    }


//#endif // __CAM_TEST_MODE__
//  End of File  
