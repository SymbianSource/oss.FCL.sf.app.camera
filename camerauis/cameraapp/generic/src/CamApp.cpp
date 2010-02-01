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
* Description:  Application class for CameraApp
*
*/



// INCLUDE FILES
#include "CamDocument.h"
#include "CamAppUid.h"
#include "CamApp.h"
#include "CamLogger.h"
#include "CamServer.h"

#include "CamUtility.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamAppTraces.h"
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamApp::AppDllUid
// Return the UID for the application
// -----------------------------------------------------------------------------
//
TUid CCamApp::AppDllUid() const
    {
    return TUid::Uid( KCameraappUID );
    }

// -----------------------------------------------------------------------------
// CCamApp::CreateDocumentL
// Create a CCamApp document, and return a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CCamApp::CreateDocumentL()
    { 
    OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPP_CREATEDOCUMENTL, "e_CAM_APP_INIT 1" );    //CCORAPP_APP_INIT_START
    return CCamDocument::NewL( *this ); 
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================


#ifdef RD_APPS_TO_EXES

#include <eikstart.h>


// -----------------------------------------------------------------------------
// NewApplication() implements...
// Create an application, and return a pointer to it
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CCamApp;
    }

// -----------------------------------------------------------------------------
// E32Main()
// Start the application
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE, DUP2__E32MAIN, "e_CAM_PRI_OFF_TO_ON 1" ); // Should be defined in DeviceModeHanlder/SysAp
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, _E32MAIN, "e_E32Main 1" );
    TInt ret = EikStart::RunApplication(NewApplication);
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1__E32MAIN, "e_E32Main 0" );
    return ret;
    }


// -----------------------------------------------------------------------------
// CCamApp::NewAppServerL
// Creates new file service application server.
// -----------------------------------------------------------------------------
//
void CCamApp::NewAppServerL( CApaAppServer*& aAppServer )
    {
#ifdef RD_PF_SEC_APPARC
	PRINT( _L( "Camcorder: NewAppServerL()" ) );
    aAppServer = new (ELeave) CCamAppServer;
#else   
    aAppServer = 0;
#endif
    }


#else  // !RD_APPS_TO_EXES


// -----------------------------------------------------------------------------
// NewApplication() implements...
// Create an application, and return a pointer to it
// -----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication() 
    {
    return new CCamApp;
    }

#endif // RD_APPS_TO_EXES

//  End of File  
