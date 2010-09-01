/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CamAppPerfTest test cases
*
*/

// [INCLUDE FILES] - do not remove
#include <e32math.h>
#include <e32base.h>
#include <f32file.h>
#include <apgtask.h>
#include <apgcli.h>
#include <EIKENV.H> // for UI components

#include "CamAppPerfTest.h"

#define EProductKeyCapture EStdKeyDevice7


// CONSTANTS
_LIT( KCamAppName, "Z:\\sys\\bin\\cameraapp.exe" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamAppPerfTest::Case
// Returns a test case by number.
//
// This function contains an array of all available test cases 
// i.e pair of case name and test function. If case specified by parameter
// aCaseNumber is found from array, then that item is returned.
// 
// The reason for this rather complicated function is to specify all the
// test cases only in one place. It is not necessary to understand how
// function pointers to class member functions works when adding new test
// cases. See function body for instructions how to add new test case.
// -----------------------------------------------------------------------------
//
const TCaseInfo CCamAppPerfTest::Case ( 
    const TInt aCaseNumber ) const 
     {

    /**
    * To add new test cases, implement new test case function and add new 
    * line to KCases array specify the name of the case and the function 
    * doing the test case
    * In practice, do following
    * 1) Make copy of existing test case function and change its name
    *    and functionality. Note that the function must be added to 
    *    CamAppPerfTest.cpp file and to CamAppPerfTest.h 
    *    header file.
    *
    * 2) Add entry to following KCases array either by using:
    *
    * 2.1: FUNCENTRY or ENTRY macro
    * ENTRY macro takes two parameters: test case name and test case 
    * function name.
    *
    * FUNCENTRY macro takes only test case function name as a parameter and
    * uses that as a test case name and test case function name.
    *
    * Or
    *
    * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
    * only with OOM (Out-Of-Memory) testing!
    *
    * OOM_ENTRY macro takes five parameters: test case name, test case 
    * function name, TBool which specifies is method supposed to be run using
    * OOM conditions, TInt value for first heap memory allocation failure and 
    * TInt value for last heap memory allocation failure.
    * 
    * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
    * that as a test case name, TBool which specifies is method supposed to be
    * run using OOM conditions, TInt value for first heap memory allocation 
    * failure and TInt value for last heap memory allocation failure. 
    */ 

    static TCaseInfoInternal const KCases[] =
        {
        // [test cases entries] - do not remove
        
        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g. 
        // CCamAppPerfTest::PrintTest. Otherwise the compiler
        // gives errors.
        
        FUNCENTRY( CCamAppPerfTest::PerfTest )
        
        //FUNCENTRY( CCamAppPerfTest::PrintTest )
        //ENTRY( "Loop testzzz", CCamAppPerfTest::LoopTest ),
        // Example how to use OOM functionality
        //OOM_ENTRY( "Loop test with OOM", CCamAppPerfTest::LoopTest, ETrue, 2, 3),
        //OOM_FUNCENTRY( CCamAppPerfTest::PrintTest, ETrue, 1, 3 ),
        };

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) / 
                               sizeof( TCaseInfoInternal ) )
        {
        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        } 

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;

    }


////////////////////////////WORKSPACE START/////////////////////////////////////


// -----------------------------------------------------------------------------
// CCamAppPerfTest::PerfTest
// CameraApp Performance Test.
// -----------------------------------------------------------------------------
//
TInt CCamAppPerfTest::PerfTest( 
    TTestResult& aResult )
    {
    // The purpose of this performance test is just to launch the cameraapp and get
    // the related RD_SYMBIAN_TRACES.  This test should never fail.
        
     /* Simple print test */
    _LIT( KPerfTest, "PerfTest" );
    _LIT( KEnter, "Enter" );
    _LIT( KOnGoing, "On-going" );
    _LIT( KExit, "Exit" );
    
    TestModuleIf().Printf( 0, KPerfTest, KEnter );
    TestModuleIf().Printf( 1, KPerfTest, KOnGoing );
    
    ////////////////////////////////////////////////////////////////////////
    
    // Start test
    RDebug::Print( _L( "[cam launcher] in" ) );


    RDebug::Print( _L( "[cam launcher] ECPCloseCheck" ));
    // make sure camera app is closed
    KillIfAlive();
    // announce that we are sure camera is closed,
    // and now we can start testing
    StartWaiting( 5000000 ); // 5 seconds
    
    
    RDebug::Print( _L( "[cam launcher] ECPTestStart" ));
    // launch first instance of camera app
    RProcess process;
    process.Create(_L("cameraapp.exe"), _L("PerformanceTest"));
    RDebug::Print( _L( "[cam launcher] resuming" ) );
    process.Resume();
    RDebug::Print( _L( "[cam launcher] resumed" ) );
    // announce that we are waiting for the first startup to finish
    StartWaiting( 15000000 ); // 15 seconds
    
    
    RDebug::Print( _L( "[cam launcher] ECPTestWaitFirstStartupFinished" ));
    // fake the capture key event
    SendCaptureKeyEventToCamApp();
    // announce that we are waiting for the first capture to finish
    StartWaiting( 10000000 ); // 10 seconds
    
    
    RDebug::Print( _L( "[cam launcher] ECPTestWaitFirstCaptureFinished" ));
    // fake the red end key event
    SendExitKeyEventToCamApp();
    // announce that we are waiting for the first close to finish
    StartWaiting( 15000000 ); // 15 seconds
    
    
    RDebug::Print( _L( "[cam launcher] ECPTestSecondStartup" ));
    // launch second instance of camera app
    RProcess process2;
    process2.Create(_L("cameraapp.exe"), _L("CameraPerformanceTest"));
    RDebug::Print( _L( "[cam launcher] resuming" ) );
    process2.Resume();
    RDebug::Print( _L( "[cam launcher] resumed" ) );
    // announce that we are waiting for the second startup to finish
    StartWaiting( 15000000 ); // 15 seconds
    
    
    RDebug::Print( _L( "[cam launcher] ECPTestWaitSecondStartupFinished" ));
    // fake the capture key event
    SendCaptureKeyEventToCamApp();
    // announce that we are waiting for the second capture to finish
    StartWaiting( 10000000 ); // 10 seconds
    
    
    RDebug::Print( _L( "[cam launcher] ECPTestWaitFirstCaptureFinished" ));
    // fake the red end key event
    SendExitKeyEventToCamApp();
    // announce that we are waiting for the second close to finish
    StartWaiting( 15000000 ); // 15 seconds 
   
    
    // Test done
    RDebug::Print( _L( "[cam launcher] close process" ) );
    process.Close();
    RDebug::Print( _L( "[cam launcher] close process2" ) );
    process2.Close();
    RDebug::Print( _L( "[cam launcher] out" ) );
    
        
    ////////////////////////////////////////////////////////////////////////
    
    TestModuleIf().Printf( 0, KPerfTest, KExit );
    
    RDebug::Print( _L( "[cam launcher] final" ) );

    // Test case passed
    // This test should never fail.

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "PerfTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CCamAppPerfTest::StartWaiting( TInt period )
    {
    RDebug::Print( _L( "[cam launcher]::StartWaiting in" ) );
    User::After( period ); 
    RDebug::Print( _L( "[cam launcher]::StartWaiting out" ) );
    }


// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CCamAppPerfTest::KillIfAlive()
    {
    RDebug::Print( _L( "[cam launcher]::KillIfAlive in" ) );
    
    RApaLsSession RSession;
    TInt AAppCount = 0;//get the number of applications
    RSession.Connect();       
    RSession.AppCount(AAppCount);//number of tasks
    RSession.GetAllApps();//get all the tasks in the RApaLsSession obj
    
    RDebug::Print( _L( "[cam launcher]::KillIfAlive test-> KCamAppName=%S" ), 
                   &KCamAppName );
            
    
    if(AAppCount > 0)
        {
        RDebug::Print( _L( "[cam launcher]::KillIfAlive there are apps, count=%d" ),AAppCount );
        //if the task is present get info about it in AppInfo
        TApaAppInfo AppInfo;
        
        RDebug::Print( _L( "[cam launcher]::KillIfAlive making CCoeEnv" ) );
        CCoeEnv* coeEnv = new (ELeave) CCoeEnv();
        RDebug::Print( _L( "[cam launcher]::KillIfAlive made CCoeEnv" ) );
        
        TRAPD(coeEnvErr,
            {
            RDebug::Print( _L( "[cam launcher]::KillIfAlive constructing CCoeEnv" ) );
            coeEnv->ConstructL();
            RDebug::Print( _L( "[cam launcher]::KillIfAlive getting WsSession from CCoeEnv" ) );
            TApaTaskList aList(coeEnv->WsSession()); 
            RDebug::Print( _L( "[cam launcher]::KillIfAlive got WsSession from CCoeEnv" ) );
        
            for(TInt i=0;i<AAppCount;i++)
                {
                RSession.GetNextApp(AppInfo);
                TApaTask ATask3 = aList.FindApp(AppInfo.iUid);
                
                if ( AppInfo.iFullName.Compare( KCamAppName ) == 0 )
                    {
                    RDebug::Print( _L( "[cam launcher]::KillIfAlive KCamAppName MATCH" ) );
                
                    if(ATask3.Exists())
                        {
                        // if the camera is in standby mode, then...
                        // the task exists, so now we try to close it
                        RDebug::Print( _L( "[cam launcher]::KillIfAlive task exists.. uid=%d name=%S" ), 
                                       AppInfo.iUid,
                                       &(AppInfo.iFullName) );
                    
                        // kill the task
                        ATask3.KillTask();
                        RDebug::Print( _L( "[cam launcher]::KillIfAlive task killed" ) );
                        }
                    }
                } // for
            }); // TRAPD
            RDebug::Print( _L( "[cam launcher]::KillIfAlive coeEnvErr=%d" ),coeEnvErr );
            coeEnv->DestroyEnvironment();
            RDebug::Print( _L( "[cam launcher]::KillIfAlive coeEnv destroyed" ) );
            
        } // if
        
    RSession.Close();
    
    RDebug::Print( _L( "[cam launcher]::KillIfAlive out" ) );
    }
    
    
// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CCamAppPerfTest::SendCaptureKeyEventToCamApp()
    {
    RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp in" ) );
        
    TKeyEvent keyEvent;
    keyEvent.iCode = 0;
    keyEvent.iScanCode = EProductKeyCapture;
    keyEvent.iModifiers = 0;
    keyEvent.iRepeats = 0;
    TWsEvent wsEvent;
    *(wsEvent.Key()) = keyEvent;
    wsEvent.SetType(EEventKeyDown); 
    wsEvent.SetTimeNow();
    
    RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp making CCoeEnv" ) );
    CCoeEnv* coeEnv = new (ELeave) CCoeEnv();
    RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp made CCoeEnv" ) );
    
    TRAPD(coeEnvErr,
        {
        RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp constructing CCoeEnv" ) );
        coeEnv->ConstructL();
        RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp getting WsSession from CCoeEnv" ) );
        coeEnv->WsSession().SendEventToWindowGroup(coeEnv->WsSession().GetFocusWindowGroup(), wsEvent);    
        RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp got WsSession from CCoeEnv" ) );
        }); // TRAPD
    
    RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp coeEnvErr=%d" ),coeEnvErr );
    coeEnv->DestroyEnvironment();
    RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp coeEnv destroyed" ) );
        
    RDebug::Print( _L( "[cam launcher]::SendCaptureKeyEventToCamApp out" ) );
    }


// -----------------------------------------------------------------------------
//  
// -----------------------------------------------------------------------------
//
void CCamAppPerfTest::SendExitKeyEventToCamApp()
    {
    RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp in" ) );
        
    TKeyEvent keyEvent;
    keyEvent.iCode = 0;
    keyEvent.iScanCode = EStdKeyNo; 
    keyEvent.iModifiers = 0;
    keyEvent.iRepeats = 0;
    TWsEvent wsEvent;
    *(wsEvent.Key()) = keyEvent;
    wsEvent.SetType(EEventKeyDown); 
    wsEvent.SetTimeNow();
    
    RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp making CCoeEnv" ) );
    CCoeEnv* coeEnv = new (ELeave) CCoeEnv();
    RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp made CCoeEnv" ) );
    
    TRAPD(coeEnvErr,
        {
        RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp constructing CCoeEnv" ) );
        coeEnv->ConstructL();
        RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp getting WsSession from CCoeEnv" ) );
        coeEnv->WsSession().SendEventToWindowGroup(coeEnv->WsSession().GetFocusWindowGroup(), wsEvent);    
        RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp got WsSession from CCoeEnv" ) );
        }); // TRAPD
    
    RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp coeEnvErr=%d" ),coeEnvErr ); 
    coeEnv->DestroyEnvironment();
    RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp coeEnv destroyed" ) );  
    
    RDebug::Print( _L( "[cam launcher]::SendExitKeyEventToCamApp out" ) );
    }    
    

////////////////////////////WORKSPACE END///////////////////////////////////////


// -----------------------------------------------------------------------------
// CCamAppPerfTest::PrintTest
// Simple printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CCamAppPerfTest::PrintTest( 
    TTestResult& aResult )
    {
     /* Simple print test */
    _LIT( KPrintTest, "PrintTest" );
    _LIT( KEnter, "Enter" );
    _LIT( KOnGoing, "On-going" );
    _LIT( KExit, "Exit" );

    TestModuleIf().Printf( 0, KPrintTest, KEnter );
           
    TestModuleIf().Printf( 1, KPrintTest, KOnGoing );
    
    TestModuleIf().Printf( 0, KPrintTest, KExit );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "PrintTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CCamAppPerfTest::LoopTest
// Another printing to UI test.
// -----------------------------------------------------------------------------
//
TInt CCamAppPerfTest::LoopTest( TTestResult& aResult )
    {

    /* Simple print and wait loop */
    _LIT( KState, "State" );
    _LIT( KLooping, "Looping" );

    TestModuleIf().Printf( 0, KState, KLooping );

    _LIT( KRunning, "Running" );
    _LIT( KLoop, "%d" );
    for( TInt i=0; i<10; i++)
        {
        TestModuleIf().Printf( 1, KRunning, KLoop, i);
        User::After( 1000000 );
        }

    _LIT( KFinished, "Finished" );
    TestModuleIf().Printf( 0, KState, KFinished );

    // Test case passed

    // Sets test case result and description(Maximum size is KStifMaxResultDes)
    _LIT( KDescription, "LoopTest passed" );
    aResult.SetResult( KErrNone, KDescription );

    // Case was executed
    return KErrNone;

    }

//  [End of File] - do not remove
