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
* Description:  Implemantation of CCamSnapshotProvider class.
*                Handles different setups of CCamSnapshot and CCameraSnapshot.
*                For example secondary camera does not always support 
*                CCameraSnapshot.
*
*/


// ===========================================================================
// Includes
#include <ecam.h>           // CCamera, MCameraObserver2
#include <ecam/camerasnapshot.h> // CCameraSnapshot

#include "camcameracontroller.pan"
#include "camlogging.h"
#include "camsnapshot.h"
#include "camsnapshotprovider.h"



// ===========================================================================
// Constants etc
using namespace NCamCameraController;


// ===========================================================================
// Methods (see also inline methods in .inl)

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamSnapshotProvider* 
CCamSnapshotProvider::NewL( CCamera&              aCamera,
                            MCameraObserver2&     aObserver, 
                            MCamCameraObservable& aObservable )
  {
  CCamSnapshotProvider* self = new (ELeave) CCamSnapshotProvider;

  CleanupStack::PushL( self );
  self->ConstructL( aCamera, aObserver, aObservable );
  CleanupStack::Pop( self );

  return self;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamSnapshotProvider::~CCamSnapshotProvider()
  {
  delete iSs1;
  delete iSs2;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamSnapshotProvider::ConstructL( CCamera&              aCamera, 
                                  MCameraObserver2&     aObserver, 
                                  MCamCameraObservable& aObservable )
  {
  PRINT( _L("Camera => CCamSnapshotProvider::ConstructL") );
#ifdef CAMERAAPP_CAPI_V2_SS 
  TRAPD( error, iSs1 = CCamera::CCameraSnapshot::NewL( aCamera ) );

  PRINT1( _L("Camera <> status from CCameraSnapshot::NewL: %d"), error );
  if( KErrNone != error )
    {
    if( KErrNotSupported == error )
      {
      PRINT( _L("Camera <> CCameraSnapshot not supported, use CCamSnapshot..") );
      iSs2 = CCamSnapshot::NewL( aCamera, aObserver, aObservable );
      }
    else
      {
      User::Leave( error );
      }
    }
#else
  iSs1 = NULL;
  iSs2 = CCamSnapshot::NewL( aCamera, aObserver, aObservable );
#endif

  __ASSERT_ALWAYS( iSs1 || iSs2, Panic( ECamNullPointer ) );
  PRINT( _L("Camera <= CCamSnapshotProvider::ConstructL") );
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCamSnapshotProvider::CCamSnapshotProvider()
  {
  }



// ===========================================================================
// end of file
