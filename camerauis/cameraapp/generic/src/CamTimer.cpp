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
* Description:  Active object for implementing viewfinder timeout and*
*/


// INCLUDE FILES
#include <e32std.h>
#include "camlogging.h"
#include "CamTimer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamTimer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamTimer* CCamTimer::NewL( TInt aTimeout, TCallBack aCallBack )
    {
    CCamTimer* self = NewLC( aTimeout, aCallBack );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamTimer::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamTimer* CCamTimer::NewLC( TInt aTimeout, TCallBack aCallBack )
    {
    CCamTimer* self = new ( ELeave ) CCamTimer( aTimeout, aCallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CCamTimer::~CCamTimer()
  {
  PRINT( _L("Camera => ~CCamTimer" ))  
  Cancel();
  PRINT( _L("Camera <= ~CCamTimer" ))  
  }

// ---------------------------------------------------------------------------
// CCamTimer::StartTimer
// Start timer.
// ---------------------------------------------------------------------------
//
void CCamTimer::StartTimer()
    {
    After( iTimeout );
    }
    
// ---------------------------------------------------------------------------
// CCamTimer::SetTimeout
// Update the timeout period
// ---------------------------------------------------------------------------
// 
void CCamTimer::SetTimeout( TInt aTimeout )
    {
    iTimeout = aTimeout;
    }   

// ---------------------------------------------------------------------------
// CCamTimer::RunL
// From CActive, handle timeout expiration
// ---------------------------------------------------------------------------
//
void CCamTimer::RunL()
    {
    iCallBack.CallBack();
    }

// ---------------------------------------------------------------------------
// CCamTimer::CCamTimer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamTimer::CCamTimer( TInt aTimeout, TCallBack aCallBack )
: CCamTimerBase( EPriorityStandard ), iTimeout( aTimeout ), iCallBack( aCallBack )
    {
    }

// ---------------------------------------------------------------------------
// CCamTimer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// End of File  



