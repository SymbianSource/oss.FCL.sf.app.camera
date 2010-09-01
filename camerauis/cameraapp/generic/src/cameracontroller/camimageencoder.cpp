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
* Description:  Implementation of Camera image encoder
*
*/




#include <imageconversion.h>
#include <ExifRead.h>
#include <ecam.h> // MCameraBuffer

#include <eikenv.h>

#include "camlogging.h"
#include "camfilesaveutility.h"
#include "cambuffershare.h"

#include "camimageencoder.h"

// ===========================================================================
// Local constants

// Encoding flags
static const CImageEncoder::TOptions KConversionOptions =
  CImageEncoder::EOptionAlwaysThread;

// Other constants
static const TInt KImageIndex = 0;
static const TInt KPriority   = CActive::EPriorityHigh;

_LIT8( KMimeTypeDesc8, "image/jpeg" );


// ===========================================================================
// public constructor and destructor

// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CCamImageEncoder* 
CCamImageEncoder::NewL( MCamImageEncoderObserver& aObserver )
  {
  CCamImageEncoder* self = 
      new (ELeave) CCamImageEncoder( aObserver );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;  
  }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamImageEncoder::~CCamImageEncoder()
  {
  PRINT( _L("Camera => ~CCamImageEncoder") );
  Cancel();

  ReleaseEncoder();
  SetInputBuffer( NULL );

  PRINT( _L("Camera <= ~CCamImageEncoder") );
  }

// ===========================================================================
// public methods

// ---------------------------------------------------------------------------
// StartConversionL
// ---------------------------------------------------------------------------
//
void
CCamImageEncoder::StartConversionL( CCamBufferShare* aBuffer )
  {
  PRINT( _L("Camera => CCamImageEncoder::StartConversionL") );

  // Cancel any previous action
  Cancel();

  ReleaseEncoder();

  if( aBuffer
   && aBuffer->SharedBuffer() )   
    {
    // Store the bitmap data. 
    // Need to release old buffer and reserve given one.
    SetInputBuffer( aBuffer );

    PRINT( _L("Camera <> CCamImageEncoder: Creating encoder..") );
    iEncoder = CImageEncoder::DataNewL( iEncodedData, 
                                        KMimeTypeDesc8(), 
                                        KConversionOptions );

    PRINT( _L("Camera <> CCamImageEncoder: get source bitmap..") );
    MCameraBuffer* buffer( iSharedInput->SharedBuffer()   );
    CFbsBitmap&    bitmap( buffer->BitmapL( KImageIndex ) );


    PRINT( _L("Camera <> CCamImageEncoder: start conversion..") );
    iEncoder->Convert( &iStatus, bitmap );
    SetActive();
    }
  else
    {
    PRINT( _L("Camera <> CCamImageEncoder: No bitmap provided, leave..") );
    User::Leave( KErrNotFound );    
    }

  PRINT( _L("Camera <= CCamImageEncoder::StartConversionL") );
  }


// ===========================================================================
// from CActive

// ---------------------------------------------------------------------------
// virtual DoCancel
// ---------------------------------------------------------------------------
//
void 
CCamImageEncoder::DoCancel()
  {
  PRINT( _L("Camera => CCamImageEncoder::DoCancel") );

  if( iEncoder )
    {
    iEncoder->Cancel();
    }

  ReleaseEncoder();
  SetInputBuffer( NULL );

  PRINT( _L("Camera <= CCamImageEncoder::DoCancel") );
  }


// ---------------------------------------------------------------------------
// virtual RunL
// ---------------------------------------------------------------------------
//
void 
CCamImageEncoder::RunL()
  {
  PRINT1( _L("Camera => CCamImageEncoder::RunL, iStatus:%d"), iStatus.Int() );

  switch( iStatus.Int() )
    {
    case KErrNone :
      {
      SetInputBuffer( NULL );

      // Notify observer
      NotifyObserver( KErrNone );
      
      // Source data and encoder not needed anymore
      ReleaseEncoder();
      
      break;
      }
    case KErrCancel :
    default :
      {
      User::Leave( iStatus.Int() );
      break;
      }
    }

  PRINT( _L("Camera <= CCamImageEncoder::RunL") );
  }


// ---------------------------------------------------------------------------
// virtual RunError
// ---------------------------------------------------------------------------
//
TInt 
CCamImageEncoder::RunError( TInt aError )
  {
  PRINT1( _L("Camera <= CCamImageEncoder::RunError(%d)"), aError );

  ReleaseEncoder();
  SetInputBuffer( NULL );

  // Leave has occurred in RunL.
  // Notify observer with error and NULL data.
  NotifyObserver( aError );

  PRINT( _L("Camera <= CCamImageEncoder::RunError") );
  return KErrNone;
  }


// ===========================================================================
// Private methods

// ---------------------------------------------------------------------------
// SetInputBuffer
// ---------------------------------------------------------------------------
//
void
CCamImageEncoder::SetInputBuffer( CCamBufferShare* aBuffer )
  {
  if( iSharedInput )
    {
    iSharedInput->Release();
    iSharedInput = NULL;
    }

  iSharedInput = aBuffer;

  if( iSharedInput )
    {
    iSharedInput->Reserve();
    }
  }

void 
CCamImageEncoder::ReleaseEncoder()
  {
  delete iEncoder;
  iEncoder = NULL;

  delete iEncodedData;
  iEncodedData = NULL;
  }


// ---------------------------------------------------------------------------
// NotifyObservers
// ---------------------------------------------------------------------------
//
void
CCamImageEncoder::NotifyObserver( TInt aStatus )
  {
  iObserver.ImageEncoded( aStatus, iEncodedData );
  iEncodedData = NULL;  // Ownership to observer
  }

// ===========================================================================
// private constructors

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void
CCamImageEncoder::ConstructL()
  {
  CActiveScheduler::Add( this );
  }

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
CCamImageEncoder::CCamImageEncoder( MCamImageEncoderObserver& aObserver )
  : CActive  ( KPriority ),
    iObserver( aObserver )
  {
  }

// end of file
