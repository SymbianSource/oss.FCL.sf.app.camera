/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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



// INCLUDE FILES
#include    <pathinfo.h>
#include    <driveinfo.h>
#include    <UsbWatcherInternalPSKeys.h>
#include    <usbpersonalityids.h>

#include    "CamDriveChangeNotifier.h"
#include    "camlogging.h"
#include    "CamUtility.h"


_LIT(KDriveE, "E:\\");
_LIT(KDriveF, "F:\\");

const TInt KUSBTimeout = 1000000;  //1 sec

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDiskChangeListener::NewL
// -----------------------------------------------------------------------------
//
CCamDriveChangeNotifier::CCamDiskChangeListener* 
    CCamDriveChangeNotifier::CCamDiskChangeListener::NewLC(
                  RFs& aFs,
                  TDriveNumber aDrive,                                         
                  CCamDriveChangeNotifier& aObserver )
  {
  PRINT( _L( "Camera => CCamDiskChangeListener::NewLC" ) );

  CCamDiskChangeListener* self = 
        new( ELeave ) CCamDiskChangeListener( aFs, aDrive, aObserver );
  CleanupStack::PushL( self );

  PRINT( _L( "Camera <= CCamDiskChangeListener::NewLC" ) );
  return self;
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDiskChangeListener::~CCamDiskChangeListener
// -----------------------------------------------------------------------------
//
CCamDriveChangeNotifier::CCamDiskChangeListener::~CCamDiskChangeListener()
  {
  PRINT( _L( "Camera => CCamDiskChangeListener::~CCamDiskChangeListener" ) );
  Cancel();
  PRINT( _L( "Camera <= CCamDiskChangeListener::~CCamDiskChangeListener" ) );
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDiskChangeListener::CCamDiskChangeListener
// -----------------------------------------------------------------------------
//
CCamDriveChangeNotifier::CCamDiskChangeListener::CCamDiskChangeListener(
                  RFs& aFs,
                  TDriveNumber aDrive,                                         
                  CCamDriveChangeNotifier& aObserver )
    : CActive( CActive::EPriorityIdle ),
      iFs( aFs ),
      iDrive( aDrive ),
      iObserver( aObserver )                                          
  {
  CActiveScheduler::Add( this );
  }
    
// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDiskChangeListener::Start
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::CCamDiskChangeListener::Start()
  {
  PRINT( _L( "Camera => CCamDiskChangeListener::Start" ) );
  if ( !IsActive() )
    {
    PRINT( _L( "Camera <> CCamDiskChangeListener::Start SetActive()" ) );
        
    // Start listening for change events
    if(iDrive == EDriveE)
        {
        iFs.NotifyChange( ENotifyDisk, iStatus, KDriveE );
        }
    else if( iDrive == EDriveF )
        {
        iFs.NotifyChange( ENotifyDisk, iStatus, KDriveF );
        }
    else
        {
        iFs.NotifyChange( ENotifyDisk, iStatus );
        }
    
    SetActive();
    }
  PRINT( _L( "Camera <= CCamDiskChangeListener::Start" ) );
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDiskChangeListener::Stop
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::CCamDiskChangeListener::Stop()
  {
  PRINT( _L( "Camera => CCamDiskChangeListener::Stop" ) );
  Cancel();
  PRINT( _L( "Camera <= CCamDiskChangeListener::Stop" ) );
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier:::CCamDiskChangeListener::DoCancel
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::CCamDiskChangeListener::DoCancel()
  {
  PRINT( _L( "Camera => CCamDiskChangeListener::DoCancel" ) );
  iFs.NotifyChangeCancel();
  PRINT( _L( "Camera <= CCamDiskChangeListener::DoCancel" ) );
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDiskChangeListener::RunL
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::CCamDiskChangeListener::RunL()
  {
  PRINT1( _L( "Camera => CCamDiskChangeListener::RunL, iStatus: %d" ), iStatus.Int() );
  TInt ret = KErrNone;
  
  if ( iStatus == KErrNone )
    {
    TUint driveStatus;  
    ret = DriveInfo::GetDriveStatus(iFs, iDrive, driveStatus );
    
    if( ret == KErrNone)
        {
        if( !( driveStatus & DriveInfo::EDrivePresent ) ||
                ( ( driveStatus & DriveInfo::EDrivePresent ==  DriveInfo::EDrivePresent ) &&
                  ( driveStatus & DriveInfo::EDriveInUse ==  DriveInfo::EDriveInUse ) ) )
            {
            RDebug::Print(_L("CCamDiskChangeListener::RunL Dismount:%d"),iDrive);
            ret = iObserver.NotifyChangeL( MCamDriveChangeNotifierObserver::EDriveDismount );
            }
        else if( ( driveStatus & DriveInfo::EDrivePresent ) == DriveInfo::EDrivePresent )
            {
            RDebug::Print(_L("CCamDiskChangeListener::RunL Mount:%d"),iDrive);
            ret = iObserver.NotifyChangeL( MCamDriveChangeNotifierObserver::EDriveMount );
            }
        }

    if( ret == KErrNone )
      {
      Start();
      }
    }
  else if ( iStatus == KErrNotReady )
    {
    // Give other threads chance to run	
    User::After( 0 );	
    Start();
    }	

  PRINT( _L( "Camera <= CCamDiskChangeListener::RunL" ) );
  }

                
// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDriveChangeNotifier
// -----------------------------------------------------------------------------
//
CCamDriveChangeNotifier::CCamDriveChangeNotifier(
                                    RFs& aFs,
                                    MCamDriveChangeNotifierObserver& aObserver )
    : iObserver( aObserver ),
      iFs( aFs )
  {
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::NewL
// -----------------------------------------------------------------------------
//
CCamDriveChangeNotifier* CCamDriveChangeNotifier::NewL(
                                  RFs& aFs,
                                  MCamDriveChangeNotifierObserver& aObserver )
  {
  PRINT( _L( "Camera => CCamDriveChangeNotifier::NewL" ) );

  CCamDriveChangeNotifier* self =
                      new( ELeave ) CCamDriveChangeNotifier( aFs, aObserver );
  
  self->ConstructL();
  
  PRINT( _L( "Camera <= CCamDriveChangeNotifier::NewL" ) );
  return self;
  }


// Destructor
CCamDriveChangeNotifier::~CCamDriveChangeNotifier()
  {
  PRINT( _L( "Camera => CCamDriveChangeNotifier::~CCamDriveChangeNotifier" ) );
  CancelMonitoring();
  iListeners.ResetAndDestroy();
  if( iUsbMSWatcher )
      {
      if( iUsbMSWatcher->IsActive() )
          {
          iUsbMSWatcher->Cancel();
          }
      delete iUsbMSWatcher;
      iUsbMSWatcher = NULL;
      }
  if ( iUSBTimer )
      {
      if( iUSBTimer->IsActive() )
          {
          iUSBTimer->Cancel();
          }
      delete iUSBTimer;
      iUSBTimer = NULL;
      }
  PRINT( _L( "Camera <= CCamDriveChangeNotifier::~CCamDriveChangeNotifier" ) );
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::StartMonitoring
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::StartMonitoring()
  {
  PRINT( _L( "Camera => CCamDriveChangeNotifier::StartMonitoring" ) );

  for ( TInt i = iListeners.Count(); --i >= 0; )
    {
    iListeners[i]->Start();
    }
  
  iUsbMSWatcher->Subscribe();
  
  PRINT( _L( "Camera <= CCamDriveChangeNotifier::StartMonitoring" ) );
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CancelMonitoring
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::CancelMonitoring()
  {
  PRINT( _L( "Camera => CCamDriveChangeNotifier::CancelMonitoring" ) );

  for ( TInt i = iListeners.Count(); --i >= 0; )
    {
    iListeners[i]->Stop();
    }
  //iUsbMSWatcher->Cancel();
  PRINT( _L( "Camera <= CCamDriveChangeNotifier::CancelMonitoring" ) );
  }
    
// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::NotifyChangeL
// -----------------------------------------------------------------------------
//
TInt CCamDriveChangeNotifier::NotifyChangeL( 
                  MCamDriveChangeNotifierObserver::TCamDriveChangeType aType )
  {
  PRINT( _L( "Camera => CCamDriveChangeNotifier::NotifyChangeL" ) );
  TInt ret = iObserver.DriveChangeL( aType );
  if ( ret == KErrNone && aType == 
                            MCamDriveChangeNotifierObserver::EDriveDismount )
    {
    PRINT( _L("Camera <> iFs.AllowDismount( EDriveE )") )
    iFs.AllowDismount( EDriveE );
    PRINT( _L("Camera <> iFs.AllowDismount( EDriveD )") )
    iFs.AllowDismount( EDriveF );
    }
        
  PRINT( _L( "Camera <= CCamDriveChangeNotifier::NotifyChangeL" ) );        
  return ret;
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::SendAllowDismount
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::SendAllowDismount()
  {
  PRINT( _L( "Camera => CCamDriveChangeNotifier::SendAllowDismount" ) );
  iFs.AllowDismount( EDriveE );
  iFs.AllowDismount( EDriveF );
  StartMonitoring();
  PRINT( _L( "Camera <= CCamDriveChangeNotifier::SendAllowDismount" ) );
  }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::HandlePropertyChangedL
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::HandlePropertyChangedL( const TUid& aCategory, const TUint aKey )
    {
    PRINT( _L( "Camera => CCamDriveChangeNotifier::HandlePropertyChangedL" ) );
    if(KPSUidUsbWatcher == aCategory && 
       KUsbWatcherSelectedPersonality == aKey)
        {
        TInt value = 0;
        iUsbMSWatcher->Get( value );
        if( KUsbPersonalityIdMS == value )
            {
            PRINT( _L( "Camera => CCamDriveChangeNotifier::HandlePropertyChangedL StartTimer" ) );
            iUSBTimer->Cancel();
            iUSBTimer->StartTimer();
            }
        else
            {
            if( iMassStorageModeOn )
                {
                PRINT( _L("CCamDriveChangeNotifier::HandlePropertyChangedL KUsbPersonalityIdMS Off") );
                iMassStorageModeOn = EFalse;
                iObserver.DriveChangeL( MCamDriveChangeNotifierObserver::EDriveUSBMassStorageModeOff );
                }
            }
        }
    
    PRINT( _L( "Camera <= CCamDriveChangeNotifier::HandlePropertyChangedL" ) );
    }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::ConstructL
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::ConstructL()
    {
    
    iListeners.AppendL( CCamDiskChangeListener::NewLC( iFs, EDriveE, 
                          *this ) );
    
    iListeners.AppendL( CCamDiskChangeListener::NewLC( iFs, EDriveF, 
                         *this ) );  
    
    iUsbMSWatcher = CCamPropertyWatcher::NewL(*this, KPSUidUsbWatcher, 
                        KUsbWatcherSelectedPersonality );
    
    iUSBTimer = CCamTimer::NewL( KUSBTimeout, TCallBack(USBTimerCallBack, this));
    CleanupStack::Pop(); // listener
    CleanupStack::Pop(); // listener 2
    
    StartMonitoring();

    }

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::USBTimerCallBackL
// -----------------------------------------------------------------------------
//
void CCamDriveChangeNotifier::USBTimerCallBackL()
    {
    TInt value = 0;
    iUsbMSWatcher->Get( value );
    if( KUsbPersonalityIdMS == value )
        {
        PRINT( _L("CCamDriveChangeNotifier::USBTimerCallBackL KUsbPersonalityIdMS On") );
        iMassStorageModeOn = ETrue;
        iObserver.DriveChangeL( MCamDriveChangeNotifierObserver::EDriveUSBMassStorageModeOn );
        }
    else
        {
        if( iMassStorageModeOn )
            {
            PRINT( _L("CCamDriveChangeNotifier::USBTimerCallBackL KUsbPersonalityIdMS Off") );
            iMassStorageModeOn = EFalse;
            iObserver.DriveChangeL( MCamDriveChangeNotifierObserver::EDriveUSBMassStorageModeOff );
            }
        }
    }
        
// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::USBTimerCallBack
// -----------------------------------------------------------------------------
//
TInt CCamDriveChangeNotifier::USBTimerCallBack( TAny* aPtr )
    {
    CCamDriveChangeNotifier* self = static_cast<CCamDriveChangeNotifier*>(aPtr);
    if( self )
        {
        TRAP_IGNORE( self->USBTimerCallBackL() );
        }
    return KErrNone;
    }

//  End of File
