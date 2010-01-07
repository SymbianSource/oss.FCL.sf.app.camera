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
* Description:  Active object used to emulate the auto focus custom extension*
*/


// INCLUDE FILES
#include <e32std.h>

#include "CamAppControllerBase.h"
#include "TestAFExtension.h"

const TInt KAutoFocusTimeout = 1500000;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CTestAFExtension::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CTestAFExtension* CTestAFExtension::NewL( CCamAppControllerBase& aController, TBool aFailAutoFocus )
    {
    CTestAFExtension* self = NewLC( aController, aFailAutoFocus );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CTestAFExtension::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CTestAFExtension* CTestAFExtension::NewLC( CCamAppControllerBase& aController, TBool aFailAutoFocus )
    {
    CTestAFExtension* self = new ( ELeave ) CTestAFExtension( aController, aFailAutoFocus );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CTestAFExtension::~CTestAFExtension()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CTestAFExtension::Start
// Start 'auto focus' procedure.
// ---------------------------------------------------------------------------
//
void CTestAFExtension::Start()
    {
    // Notify observers that AF procedure has started
    TUid uid = KUidCamExtAutoFocus;
    TAny* p1 = static_cast< TAny* > ( &uid );
    TCamExtAutoFocus focusState = ECamExtAutoFocusFocusing;
    TAny* p2 = static_cast< TAny* > ( &focusState );
    iController.Notify( p1, p2 );
    After( KAutoFocusTimeout );
    }

#ifdef CAMERAAPP_UNIT_TEST
// ---------------------------------------------------------------------------
// CTestAFExtension::SetFailAutoFocus
// Set whether or not the autofocus operation returns a failure
// ---------------------------------------------------------------------------
//
void CTestAFExtension::SetFailAutoFocus( TBool aFailAutoFocus )
    {
    iFailAutoFocus = aFailAutoFocus;
    }
#endif

// ---------------------------------------------------------------------------
// CTestAFExtension::RunL
// From CActive, handle timeout expiration
// ---------------------------------------------------------------------------
//
void CTestAFExtension::RunL()
    {
    TUid uid = KUidCamExtAutoFocus;
    TAny* p1 = static_cast< TAny* > ( &uid );
    TCamExtAutoFocus focusState;
    if ( iFailAutoFocus )
        {
        // Notify the controller that focus procedure has failed
        focusState = ECamExtAutoFocusFailed;
        }
    else 
        {
        // Notify the controller that focus has been achieved
        focusState = ECamExtAutoFocusFocused;
        }
    TAny* p2 = static_cast< TAny* > ( &focusState );
    iController.Notify( p1, p2 );
    }

// ---------------------------------------------------------------------------
// CTestAFExtension::CTestAFExtension
// C++ constructor
// ---------------------------------------------------------------------------
//
CTestAFExtension::CTestAFExtension( CCamAppControllerBase& aController, TBool aFailAutoFocus )
: CTimer( EPriorityStandard ), iController( aController ), iFailAutoFocus( aFailAutoFocus )
    {
    }

// ---------------------------------------------------------------------------
// CTestAFExtension::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CTestAFExtension::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// End of File  
