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
* Description:  Implementation of CCamBuffer class.
*                Temporary own implementation of MCameraBuffer. 
*                To be replaced by ECam CCameraBuffer.
*
*/



#include <fbs.h>

#include "camlogging.h"
#include "cambuffer.h"


// ---------------------------------------------------------------------------
// NewL
// Takes ownership of aData, but not aBitmap
// ---------------------------------------------------------------------------
//
CCamBuffer* 
CCamBuffer::NewL( const CFbsBitmap& aBitmap, 
                        HBufC8*     aData )
  {
  CCamBuffer* self = CCamBuffer::NewLC( aBitmap, aData );
  CleanupStack::Pop( self );
  return self;
  }

// ---------------------------------------------------------------------------
// NewLC
// Takes ownership of aData, but not aBitmap
// ---------------------------------------------------------------------------
//
CCamBuffer* 
CCamBuffer::NewLC( const CFbsBitmap& aBitmap, 
                         HBufC8*     aData )
  {
  CCamBuffer* self = new (ELeave) CCamBuffer;
  CleanupStack::PushL( self );
  self->ConstructL( aBitmap, aData );
  return self;
  }


// ---------------------------------------------------------------------------
// NewL
// Takes ownership of both objects.
// ---------------------------------------------------------------------------
//
CCamBuffer* 
CCamBuffer::NewL( CFbsBitmap* aBitmap, 
                  HBufC8*     aData )
  {
  CCamBuffer* self = CCamBuffer::NewLC( aBitmap, aData );
  CleanupStack::Pop( self );
  return self;
  }

// ---------------------------------------------------------------------------
// NewLC
// Takes ownership of both objects.
// ---------------------------------------------------------------------------
//
CCamBuffer* 
CCamBuffer::NewLC( CFbsBitmap* aBitmap, 
                   HBufC8*     aData )
  {
  CCamBuffer* self = new (ELeave) CCamBuffer;
  CleanupStack::PushL( self );
  self->ConstructL( aBitmap, aData );
  return self;
  }

        
// ---------------------------------------------------------------------------
// NumFrames
//
// Number of frames available in the buffer.
// ---------------------------------------------------------------------------
//
TInt 
CCamBuffer::NumFrames()
  {
  return 1;
  }


// ---------------------------------------------------------------------------
// DataL
//
// Frame data as descriptor.
// *not supported here*
// ---------------------------------------------------------------------------
//
TDesC8* 
CCamBuffer::DataL( TInt aFrameIndex )
  {
  if( aFrameIndex != 0 || !iImageData )
    User::Leave( KErrNotSupported );

  return iImageData; 
  }

// ---------------------------------------------------------------------------
// BitmapL
//
// Frame data as bitmap.
// ---------------------------------------------------------------------------
//
CFbsBitmap& 
CCamBuffer::BitmapL( TInt aFrameIndex )
  {
  if( aFrameIndex != 0 || !iBitmap )
    User::Leave( KErrNotFound );

  return *iBitmap;
  }

// ---------------------------------------------------------------------------
// DataL
//
// Frame data as chunk.
// *not supported here*
// ---------------------------------------------------------------------------
//
RChunk& 
CCamBuffer::ChunkL()
  {
  User::Leave( KErrNotSupported );
  
  return iChunk;
  }

// ---------------------------------------------------------------------------
// ChunkOffsetL
//
// Frame data offset in chunk.
// *not supported here*
// ---------------------------------------------------------------------------
//
TInt 
CCamBuffer::ChunkOffsetL( TInt /*aFrameIndex*/ )
  {
  User::Leave( KErrNotSupported );
  
  return 0;
  }

// ---------------------------------------------------------------------------
// FrameSize
//
// Frame data size.
// *not supported here, as only bitmap supported*
// ---------------------------------------------------------------------------
//
TInt 
CCamBuffer::FrameSize( TInt /*aFrameIndex*/ )
  {
  return -1;
  }

// ---------------------------------------------------------------------------
// Release
//
// Release this buffer.
// Simply deletes this object.
// ---------------------------------------------------------------------------
//
void 
CCamBuffer::Release()
  {
  delete this;
  }

// ===========================================================================

// ---------------------------------------------------------------------------
// ConstructL
//
// ---------------------------------------------------------------------------
//
void 
CCamBuffer::ConstructL( const CFbsBitmap& aBitmap, 
                              HBufC8*     aData )
  {
  PRINT( _L("CamTest => CCamBuffer::ConstructL") );
    
  
  iBitmap = new (ELeave) CFbsBitmap;
  TInt error = iBitmap->Duplicate( aBitmap.Handle() );

  iImageData = aData;

  PRINT1( _L("CamTest <> duplicate bitmap status(%d)"), error );

  User::LeaveIfError( error );

  PRINT( _L("CamTest <= CCamBuffer::ConstructL") );
  }

// ---------------------------------------------------------------------------
// ConstructL
//
// ---------------------------------------------------------------------------
//
void 
CCamBuffer::ConstructL( CFbsBitmap* aBitmap, 
                        HBufC8*     aData )
  {
  PRINT( _L("CamTest => CCamBuffer::ConstructL") );
    
  iImageData = aData;
  iBitmap    = aBitmap;

  PRINT( _L("CamTest <= CCamBuffer::ConstructL") );
  }


// ---------------------------------------------------------------------------
// Constructor
//
// ---------------------------------------------------------------------------
//
CCamBuffer::CCamBuffer()
  : iBitmap( NULL ),
    iImageData( NULL ),
    iOwnBitmap( ETrue ),
    iOwnData( ETrue )
  {
  }

// ---------------------------------------------------------------------------
// Destructor
// *private, because Release() is supposed to be used.*
// ---------------------------------------------------------------------------
//
CCamBuffer::~CCamBuffer()
  {
  PRINT( _L("CamTest => ~CCamBuffer") );

  PRINT( _L("CamTest <> ~CCamBuffer: Close RChunk..") );
  iChunk.Close();
  PRINT( _L("CamTest <> ~CCamBuffer: delete CFbsBitmap..") );
  delete iBitmap;
  PRINT( _L("CamTest <> ~CCamBuffer: delete HBufC8*..") );
  delete iImageData;

  PRINT( _L("CamTest <= ~CCamBuffer") );
  }


// end of file
