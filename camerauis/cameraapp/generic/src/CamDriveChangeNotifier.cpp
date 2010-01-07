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

#include    "CamDriveChangeNotifier.h"
#include    "camlogging.h"
#include    "CamUtility.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamDriveChangeNotifier::CCamDiskChangeListener::NewL
// -----------------------------------------------------------------------------
//
CCamDriveChangeNotifier::CCamDiskChangeListener* 
    CCamDriveChangeNotifier::CCamDiskChangeListener::NewLC(
                  RFs& aFs,
                  TDriveNumber aDrive,                                         
                  MCamDriveChangeNotifierObserver::TCamDriveChangeType aType,
                  CCamDriveChangeNotifier& aObserver )
  {
  PRINT( _L( "Camera => CCamDiskChangeListener::NewLC" ) );

  CCamDiskChangeListener* self = 
        new( ELeave ) CCamDiskChangeListener( aFs, aDrive, aType, aObserver );
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
                  MCamDriveChangeNotifierObserver::TCamDriveChangeType aType,
                  CCamDriveChangeNotifier& aObserver )
    : CActive( CActive::EPriorityIdle ),
      iFs( aFs ),
      iDrive( aDrive ),
      iType( aType ),
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
    iFs.NotifyChange( ENotifyDisk, iStatus );
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
  PRINT2( _L( "Camera => CCamDiskChangeListener::RunL iType: %d, iStatus: %d" ), iType, iStatus.Int() );
  TInt ret = KErrNone;
  
  if ( iStatus == KErrNone )
    {
    ret = iObserver.NotifyChangeL( iType );
        
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
  
  self->iListeners.AppendL( CCamDiskChangeListener::NewLC( self->iFs, EDriveE, 
                      MCamDriveChangeNotifierObserver::EDriveMount, *self ) );
    
  self->iListeners.AppendL( CCamDiskChangeListener::NewLC( self->iFs, EDriveD, 
                      MCamDriveChangeNotifierObserver::EDriveMount, *self ) );
  
  CleanupStack::Pop(); // listener
  CleanupStack::Pop(); // listener 2
    
  self->StartMonitoring();

  PRINT( _L( "Camera <= CCamDriveChangeNotifier::NewL" ) );
  return self;
  }


// Destructor
CCamDriveChangeNotifier::~CCamDriveChangeNotifier()
  {
  PRINT( _L( "Camera => CCamDriveChangeNotifier::~CCamDriveChangeNotifier" ) );
  CancelMonitoring();
  iListeners.ResetAndDestroy();
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
    iFs.AllowDismount( EDriveD );
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
  iFs.AllowDismount( EDriveD );
  StartMonitoring();
  PRINT( _L( "Camera <= CCamDriveChangeNotifier::SendAllowDismount" ) );
  }    
//  End of File
