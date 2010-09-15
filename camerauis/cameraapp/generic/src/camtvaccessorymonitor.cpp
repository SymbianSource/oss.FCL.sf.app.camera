/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class to handle TV accessory events*
*/

#include <eikenv.h>
#include <AccPolGenericIdDefinitions.h>
#include <AccPolSubblockNameArray.h>

#include "camtvaccessorymonitor.h"
#include "camlogging.h"

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::CCamTvAccessoryMonitor
// -----------------------------------------------------------------------------
//
CCamTvAccessoryMonitor::CCamTvAccessoryMonitor( MCamTvAccessoryObserver* aObserver ) :
    CActive(EPriorityHigh),
    iObserver( aObserver )
    {
    PRINT(_L("Camera <> CCamTvAccessoryMonitor::CCamTvAccessoryMonitor"));
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::NewL
// -----------------------------------------------------------------------------
//
CCamTvAccessoryMonitor* CCamTvAccessoryMonitor::NewL( MCamTvAccessoryObserver* aObserver )
    {
    PRINT(_L("Camera => CCamTvAccessoryMonitor::NewL"));
    CCamTvAccessoryMonitor* self = new (ELeave) CCamTvAccessoryMonitor( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    PRINT(_L("Camera <= CCamTvAccessoryMonitor::NewL"));
    return self;
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::ConstructL
// -----------------------------------------------------------------------------
//
void CCamTvAccessoryMonitor::ConstructL()
    {
    PRINT(_L("Camera => CCamTvAccessoryMonitor::ConstructL"));
    CActiveScheduler::Add(this); // Add to scheduler
    User::LeaveIfError(iTvAccServer.Connect());
    iTvAccMode.CreateSubSession(iTvAccServer);
    TInt err = iTvAccMode.GetAccessoryMode( iPolAccMode );
   
    if ( err != KErrNone )
       {
       iPolAccMode.iAccessoryMode = EAccModeHandPortable;
       iPreviousMode = EAccModeHandPortable;
       }
    else
       {
       iPreviousMode = iPolAccMode.iAccessoryMode;
       }          
    PRINT(_L("Camera <= CCamTvAccessoryMonitor::ConstructL"));
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::~CCamTvAccessoryMonitor
// -----------------------------------------------------------------------------
//
CCamTvAccessoryMonitor::~CCamTvAccessoryMonitor()
    {
    PRINT(_L("Camera => CCamTvAccessoryMonitor::~CCamTvAccessoryMonitor"));
    Cancel(); // Cancel any request, if outstanding
    iTvAccMode.CloseSubSession();
    iTvAccServer.Disconnect();
    PRINT(_L("Camera <= CCamTvAccessoryMonitor::~CCamTvAccessoryMonitor"));
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::DoCancel
// -----------------------------------------------------------------------------
//
void CCamTvAccessoryMonitor::DoCancel()
    {
    PRINT(_L("Camera => CCamTvAccessoryMonitor::DoCancel"));
    iTvAccMode.CancelNotifyAccessoryModeChanged();
    PRINT(_L("Camera <= CCamTvAccessoryMonitor::DoCancel"));
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::StartListeningL
// -----------------------------------------------------------------------------
//
void CCamTvAccessoryMonitor::StartListeningL()
    {
    PRINT(_L("Camera => CCamTvAccessoryMonitor::StartListeningL"));
    if (!IsActive()) // required for testing
        {
        iTvAccMode.NotifyAccessoryModeChanged( iStatus, iPolAccMode );
        SetActive(); 
        }
    PRINT(_L("Camera <= CCamTvAccessoryMonitor::StartListeningL"));
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::RunL
// -----------------------------------------------------------------------------
//
void CCamTvAccessoryMonitor::RunL()
    {
    PRINT(_L("Camera => CCamTvAccessoryMonitor::RunL"));
    iPreviousMode = iPolAccMode.iAccessoryMode;
    if ( iPolAccMode.iAccessoryMode == EAccModeHandPortable )
        {
        iObserver->HandleTvAccessoryDisconnectedL();
        }
    else if( IsHdmiCableConnected() 
                || IsTvOutCableConnected() )
        {
        iObserver->HandleTvAccessoryConnectedL();
        }
    StartListeningL();
    PRINT(_L("Camera <= CCamTvAccessoryMonitor::RunL"));
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::RunError
// -----------------------------------------------------------------------------
//
TInt CCamTvAccessoryMonitor::RunError(TInt aError)
    {
    return aError;
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::IsHdmiCableConnected
// -----------------------------------------------------------------------------
//
TBool CCamTvAccessoryMonitor::IsHdmiCableConnected()
    {
    TBool ret = ( iPolAccMode.iAccessoryMode == EAccModeHDMI );
    PRINT1(_L("Camera <> CCamTvAccessoryMonitor::IsHdmiCableConnected=%d"),ret);
    return ret;
    }

// -----------------------------------------------------------------------------
// CCamTvAccessoryMonitor::IsTvOutCableConnected
// -----------------------------------------------------------------------------
//
TBool CCamTvAccessoryMonitor::IsTvOutCableConnected()
    {
    TBool ret = ( iPolAccMode.iAccessoryMode == EAccModeTVOut );
    PRINT1(_L("Camera <> CCamTvAccessoryMonitor::IsTvOutCableConnected=%d"),ret);
    return ret;
    }

