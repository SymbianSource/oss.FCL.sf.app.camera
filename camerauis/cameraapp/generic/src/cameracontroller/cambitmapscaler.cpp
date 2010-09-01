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
* Description:  Implementation of Camera Bitmap Scaler
*
*/




#include <bitmaptransforms.h>
#include "camlogging.h"
#include "cambitmapscaler.h"

// ===========================================================================
// Local constants

namespace NCamCameraController
  {
  static const TInt KPriority = CActive::EPriorityHigh;
  }

using namespace NCamCameraController;

// ===========================================================================
// public constructor and destructor

// ---------------------------------------------------------------------------
// 2 phase constructor
// ---------------------------------------------------------------------------
//
CCamBitmapScaler* 
CCamBitmapScaler::NewL( MCamBitmapScalerObserver& aObserver )
  {
  CCamBitmapScaler* self = 
      new (ELeave) CCamBitmapScaler( aObserver );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;  
  }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamBitmapScaler::~CCamBitmapScaler()
  {
  PRINT( _L("Camera => ~CCamBitmapScaler") );
  Cancel();

  delete iScaledBitmap;
  delete iScaler;

  PRINT( _L("Camera <= ~CCamBitmapScaler") );
  }

// ===========================================================================
// public methods


// ---------------------------------------------------------------------------
// InitScalingL
// ---------------------------------------------------------------------------
//
void 
CCamBitmapScaler::InitScalingL( const TSize&        aTargetSize,
                                const TDisplayMode& aTargetDisplayMode,
                                      TBool         aMaintainAspect )
  {
  PRINT( _L("Camera => CCamBitmapScaler::InitScalingL") );
  Cancel();

  if( !iScaledBitmap ) iScaledBitmap = new (ELeave) CFbsBitmap;
  else                 iScaledBitmap->Reset();
  
  PRINT3( _L("Camera <> Create bitmap, size(%d,%d), mode(%d)"), 
          aTargetSize.iWidth, 
          aTargetSize.iHeight, 
          aTargetDisplayMode );

  User::LeaveIfError( iScaledBitmap->Create( aTargetSize, 
                                             aTargetDisplayMode ) );

  iMaintainAspect = aMaintainAspect;
  PRINT( _L("Camera <= CCamBitmapScaler::InitScalingL") );
  }


// ---------------------------------------------------------------------------
// StartScaling
// ---------------------------------------------------------------------------
//
void CCamBitmapScaler::StartScaling( CFbsBitmap& aSourceBitmap )
  {
  PRINT( _L("Camera => CCamBitmapScaler::StartScaling") );

  Cancel();

  iScaler->Scale( &iStatus, aSourceBitmap, *iScaledBitmap, iMaintainAspect );
  SetActive();

  PRINT( _L("Camera <= CCamBitmapScaler::StartScaling") );
  }


// ===========================================================================
// from CActive

// ---------------------------------------------------------------------------
// virtual 
// ---------------------------------------------------------------------------
//
void 
CCamBitmapScaler::DoCancel()
  {
  PRINT( _L("Camera => CCamBitmapScaler::DoCancel") );

  if( iScaler )
    {
    iScaler->Cancel();
    }

  PRINT( _L("Camera <= CCamBitmapScaler::DoCancel") );
  }


// ---------------------------------------------------------------------------
// virtual 
// ---------------------------------------------------------------------------
//
void 
CCamBitmapScaler::RunL()
  {
  PRINT1( _L("Camera => CCamBitmapScaler::RunL, iStatus:%d"), iStatus.Int() );

  switch( iStatus.Int() )
    {
    case KErrNone :
      {
      iObserver.BitmapScaled( iStatus.Int(), iScaledBitmap );
      break;
      }
    default :
      {
      User::Leave( iStatus.Int() );
      break;
      }
    }

  PRINT( _L("Camera <= CCamBitmapScaler::RunL") );
  }


// ---------------------------------------------------------------------------
// virtual 
// ---------------------------------------------------------------------------
//
TInt 
CCamBitmapScaler::RunError( TInt aError )
  {
  PRINT1( _L("Camera => CCamBitmapScaler::RunError(%d)"), aError );

  // Leave has occurred in RunL.
  // Notify observer with error.
  iObserver.BitmapScaled( aError, NULL );

  PRINT( _L("Camera <= CCamBitmapScaler::RunError") );
  return KErrNone;
  }


// ===========================================================================
// private constructors

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void
CCamBitmapScaler::ConstructL()
  {
  CActiveScheduler::Add( this );

  iScaler = CBitmapScaler::NewL();
  }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCamBitmapScaler::CCamBitmapScaler( MCamBitmapScalerObserver& aObserver )
  : CActive( KPriority  ),
    iObserver( aObserver )
  {
  }

// end of file
