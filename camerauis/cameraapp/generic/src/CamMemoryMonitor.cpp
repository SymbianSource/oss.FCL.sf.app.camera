/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that monitors free memory and attempts to free it if necessary
*
*/

#include "CamMemoryMonitor.h"


#include <hal.h>
#include <hal_data.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <vgacamsettings.rsg>
#include "CamAppUi.h"
#include "CamAppController.h"
#include "camlogging.h"


const static TInt KMemoryCheckInterval = 10*1000*1000;
const static TInt KNeededMemoryLevel = 20*1024*1024;
const static TInt KCriticalMemoryLevel = 18*1024*1024;


// ---------------------------------------------------------------------------
// CCamMemoryMonitor::NewL
// ---------------------------------------------------------------------------
//
CCamMemoryMonitor* CCamMemoryMonitor::NewL(CCamAppUi* aAppUi, CCamAppController* aController)
    {
    CCamMemoryMonitor* self = new (ELeave) CCamMemoryMonitor( aAppUi, aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::~CCamMemoryMonitor
// ---------------------------------------------------------------------------
//
CCamMemoryMonitor::~CCamMemoryMonitor()
    {
    PRINT( _L("Camera => CCamMemoryMonitor::~CCamMemoryMonitor") )
    if ( iMemoryCheckTimer )
        {
        iMemoryCheckTimer->Cancel();
        delete iMemoryCheckTimer;
        }
    Cancel();
    PRINT( _L("Camera <= CCamMemoryMonitor::~CCamMemoryMonitor") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::StartMonitoring
// ---------------------------------------------------------------------------
//
void CCamMemoryMonitor::StartMonitoring( TInt aLimit, TInt aRequestAmount )
    {
    PRINT( _L("Camera => CCamMemoryMonitor::StartMonitoring") )
    if ( !iMemoryCheckTimer->IsActive() )
        {
        iLimit = aLimit;
        iRequestAmount = aRequestAmount;
        iMemoryCheckTimer->Start( KMemoryCheckInterval, KMemoryCheckInterval,
                              TCallBack( MemoryCheck, this ) );
        }
    PRINT( _L("Camera <= CCamMemoryMonitor::StartMonitoring") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::StartMonitoring
// ---------------------------------------------------------------------------
//
void CCamMemoryMonitor::StartMonitoring()
    {
    PRINT( _L("Camera => CCamMemoryMonitor::StartMonitoring") )
    if ( !iMemoryCheckTimer->IsActive() )
        {
        iMemoryCheckTimer->Start( KMemoryCheckInterval, KMemoryCheckInterval,
                              TCallBack( MemoryCheck, this ) );
        }
    PRINT( _L("Camera <= CCamMemoryMonitor::StartMonitoring") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::StopMonitoring
// ---------------------------------------------------------------------------
//
void CCamMemoryMonitor::StopMonitoring()
    {
    PRINT( _L("Camera => CCamMemoryMonitor::StopMonitoring") )
    iMemoryCheckTimer->Cancel();
    PRINT( _L("Camera <= CCamMemoryMonitor::StopMonitoring") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::CheckAndRequestMemoryL
// ---------------------------------------------------------------------------
//
TInt CCamMemoryMonitor::CheckAndRequestMemoryL( TInt aLimit, TInt aRequestAmount, TBool aShowNote )
    {

    PRINT( _L("Camera => CCamMemoryMonitor::CheckAndRequestMemoryL") )
    if ( IsActive() )
    {
        return KErrNone;
    }

    TInt memoryLeft = 0;
    TInt errorCode = KErrNone;
    HAL::Get( HALData::EMemoryRAMFree, memoryLeft );
    if ( memoryLeft < aLimit )
        {
        PRINT( _L("Camera <> CCamMemoryMonitor::CheckAndRequestMemoryL memory below limit, requesting") )
        HBufC* freeMemText = NULL;
        CAknGlobalNote* note = NULL;
        TInt noteId = 0;
        if ( aShowNote )
            {

            freeMemText = StringLoader::LoadL( R_NOTE_TEXT_FREE_MEMORY );
            CleanupStack::PushL( freeMemText );
            // show error note
            note = CAknGlobalNote::NewL();
            CleanupStack::PushL( note );

            note->SetTextProcessing( ETrue );
            note->SetPriority(CActive::EPriorityHigh);
            noteId = note->ShowNoteL(EAknGlobalWaitNote, *freeMemText);
            }

        User::LeaveIfError( iOomMonitor.Connect() );

        errorCode = iOomMonitor.RequestFreeMemory( aRequestAmount );
        if ( errorCode != KErrNone )
            {
            // try one more time
            PRINT( _L("Camera <> CCamMemoryMonitor::CheckAndRequestMemoryL request returned with error, let's try one more time") )
            errorCode = iOomMonitor.RequestFreeMemory( aRequestAmount );
            }
        iOomMonitor.Close();


        if ( aShowNote )
            {
            note->CancelNoteL(noteId);
            CleanupStack::PopAndDestroy( 2 ); // freeMemText & note
            }
        }

    PRINT1( _L("Camera <= CCamMemoryMonitor::CheckAndRequestMemoryL returning %d"), errorCode )
    return errorCode;
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::CheckAndRequestMemoryL
// ---------------------------------------------------------------------------
//
TInt CCamMemoryMonitor::CheckAndRequestMemoryL( TBool aShowNote )
    {
    return CheckAndRequestMemoryL( KCriticalMemoryLevel, KNeededMemoryLevel, aShowNote );
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::RunL
// ---------------------------------------------------------------------------
//
void CCamMemoryMonitor::RunL()
    {
    PRINT( _L("Camera => CCamMemoryMonitor::RunL") )
    iOomMonitor.Close();
    PRINT( _L("Camera <= CCamMemoryMonitor::RunL") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::DoCancel
// ---------------------------------------------------------------------------
//
void CCamMemoryMonitor::DoCancel()
    {
    PRINT( _L("Camera => CCamMemoryMonitor::DoCancel") )
    if ( IsActive() )
        {
        iOomMonitor.CancelRequestFreeMemory();
        iOomMonitor.Close();
        }
    PRINT( _L("Camera <= CCamMemoryMonitor::DoCancel") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::CCamMemoryMonitor
// ---------------------------------------------------------------------------
//
CCamMemoryMonitor::CCamMemoryMonitor(CCamAppUi* aAppUi, CCamAppController* aController)
: CActive( CActive::EPriorityIdle ),
  iLimit( KCriticalMemoryLevel ), iRequestAmount( KNeededMemoryLevel ),
  iAppUi( aAppUi ), iController( aController )

    {
    PRINT( _L("Camera => CCamMemoryMonitor::CCamMemoryMonitor") )
    CActiveScheduler::Add(this);
    PRINT( _L("Camera <= CCamMemoryMonitor::CCamMemoryMonitor") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::ConstructL
// ---------------------------------------------------------------------------
//
void CCamMemoryMonitor::ConstructL()
    {
    PRINT( _L("Camera => CCamMemoryMonitor::ConstructL") )
    iMemoryCheckTimer = CPeriodic::New( CActive::EPriorityIdle );
    PRINT( _L("Camera <= CCamMemoryMonitor::ConstructL") )
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::MemoryCheck
// ---------------------------------------------------------------------------
//
TInt CCamMemoryMonitor::MemoryCheck( TAny* aPtr )
    {

    PRINT( _L("Camera => CCamMemoryMonitor::MemoryCheck") )
    CCamMemoryMonitor* self = static_cast<CCamMemoryMonitor*>( aPtr );

    self->CheckMemory();

    PRINT( _L("Camera <= CCamMemoryMonitor::MemoryCheck") )
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCamMemoryMonitor::CheckMemory
// ---------------------------------------------------------------------------
//
void CCamMemoryMonitor::CheckMemory()
    {
    PRINT( _L("Camera => CCamMemoryMonitor::CheckMemory") )
    if ( IsActive() )
        {
        PRINT( _L("Camera <= CCamMemoryMonitor::CheckMemory already requesting memory") )
        return;
        }

    if ( iAppUi->CurrentViewState() == ECamViewStatePreCapture &&
         iController->CurrentOperation() == ECamNoOperation &&
         !iAppUi->AppInBackground( EFalse ) )
        {
        PRINT( _L("Camera <> CCamMemoryMonitor::CheckMemory checking memory") )
        TInt memoryLeft = 0;
        HAL::Get( HALData::EMemoryRAMFree, memoryLeft );
        if ( memoryLeft < iLimit )
            {
            TInt err =  iOomMonitor.Connect();
            if ( !err )
                {
                PRINT( _L("Camera <> CCamMemoryMonitor::CheckMemory requesting memory") )
                iOomMonitor.RequestFreeMemory( iRequestAmount, iStatus );
                SetActive();
                }
            else
                {
                PRINT1( _L("Camera <> CCamMemoryMonitor::CheckMemory failed to connect to OoMMonitor err=%d"), err )
                }
            }
        else
            {
            PRINT( _L("Camera <> CCamMemoryMonitor::CheckMemory no need for memory request") )
            }
        }
    else
        {
        PRINT( _L("Camera <> CCamMemoryMonitor::CheckMemory not in correct state, not checking memory") )
        }

    PRINT( _L("Camera <= CCamMemoryMonitor::CheckMemory") )
    }
