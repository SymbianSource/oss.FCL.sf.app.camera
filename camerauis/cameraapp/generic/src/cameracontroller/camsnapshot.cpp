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
* Description:  Implemantation of CCamSnapshot class.
*                Temporary own implementation of MCameraSnapshot. 
*                To be replaced by ECam CCamera::CCameraSnapshot.
*
*/


#include "camcameracontrollerflags.hrh"

#include <e32base.h>
#include <fbs.h>

#include <ecam/camerasnapshot.h>

#include "camlogging.h"
#include "mcamcameraobserver.h"
#include "camcameraevents.h"
#include "cambuffer.h"
#include "cambuffershare.h"
#include "camimagedecoder.h"
#include "cambitmapscaler.h"
#include "camsnapshot.h"






// ===========================================================================
// Local constants
static const TPoint  KDefaultPosition          = TPoint(0,0);
static const TRgb    KDefaultBackgroudColor    = KRgbWhite;
static const TUint32 KSupportedSnapshotFormats = 
   ( CCamera::EFormatFbsBitmapColor4K
   | CCamera::EFormatFbsBitmapColor64K
   | CCamera::EFormatFbsBitmapColor16M
   | CCamera::EFormatFbsBitmapColor16MU
   );                                 
static const TInt    KCallbackPriority         = CActive::EPriorityIdle;

static const TUint   KEventInterest            = ( ECamCameraEventClassImage
                                                 | ECamCameraEventClassVideo
                                                 | ECamCameraEventClassVfData
                                                 );

// ===========================================================================
// Local methods

#ifndef CAMERAAPP_CAPI_V2

// Not needed when new CAPI support ready.
TECAMEvent::TECAMEvent( TUid aEventType, 
                        TInt aErrorCode )
  : iErrorCode( aErrorCode )
  {
  iEventType.iUid = aEventType.iUid;
  #pragma message("camsnapshot.cpp, temporarily define TECAMEvent constructor")
  }
#endif


#include "campointerutility.inl"
using namespace NCamCameraController;


// ===========================================================================
// public constructors and destructor

// ---------------------------------------------------------------------------
// static 2-phase constructor
// ---------------------------------------------------------------------------
//
CCamSnapshot* 
CCamSnapshot::NewL( CCamera&              aCamera, 
                    MCameraObserver2&     aObserver,
                    MCamCameraObservable& aObservable )
  {
  CCamSnapshot* self = 
      new (ELeave) CCamSnapshot( aCamera, aObserver, aObservable );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;  
  }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamSnapshot::~CCamSnapshot()
  {
  PRINT( _L("Camera => ~CCamSnapshot") );
  iObservable.DetachObserver( this );

  delete iSnapshotBitmap;
  
  // No need to Cancel(), destructors do it already.
  delete iIdle;
  delete iDecoder;
  delete iScaler;

  SetImageData( NULL );

  PRINT( _L("Camera <= ~CCamSnapshot") );
  }

// ===========================================================================
// from MCameraSnapshot

// ---------------------------------------------------------------------------
// SupportedFormats
// ---------------------------------------------------------------------------
//
TUint32 
CCamSnapshot::SupportedFormats()
  {
  return KSupportedSnapshotFormats;
  }


// ---------------------------------------------------------------------------
// PrepareSnapshotL
// ---------------------------------------------------------------------------
//  
void 
CCamSnapshot::PrepareSnapshotL( CCamera::TFormat aFormat, 
                                const TPoint& aPosition, 
                                const TSize& aSize, 
                                const TRgb& aBgColor, 
                                TBool aMaintainAspectRatio )
  {
  PRINT( _L("Camera => CCamSnapshot::PrepareSnapshotL") );

  if( !(KSupportedSnapshotFormats & aFormat) )
    {
    PRINT( _L("Camera <> Not supported format, LEAVE") );
    User::Leave( KErrNotSupported );
    }
  else
    {
    PRINT( _L("Camera <> do prepare..") );
    iSnapshotOn = EFalse;
    iStatus     = KErrNotReady;
    iDecoder->Cancel();
    iScaler->Cancel();
    iSnapshotBitmap->Reset();

    PRINT( _L("Camera <> Init bitmap scaler..") );
    iScaler->InitScalingL( aSize, 
                           Format2DisplayMode( iFormat ), 
                           aMaintainAspectRatio );

    PRINT1( _L("Camera <> Attach as controller observer, interest: %032b"), KEventInterest );
    iObservable.AttachObserverL( this, KEventInterest );

    PRINT( _L("Camera <> Store parameters..") );
    iFormat              = aFormat;
    iPosition            = aPosition;
    iSize                = aSize;
    iBackgroundColor     = aBgColor;
    iMaintainAspectRatio = aMaintainAspectRatio;
    }

  PRINT( _L("Camera <= CCamSnapshot::PrepareSnapshotL") );
  }


// ---------------------------------------------------------------------------
// PrepareSnapshotL
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::PrepareSnapshotL( CCamera::TFormat aFormat,
                                const TSize& aSize, 
                                TBool aMaintainAspectRatio )
  {
  PrepareSnapshotL( aFormat, 
                    KDefaultPosition, 
                    aSize, 
                    KDefaultBackgroudColor, 
                    aMaintainAspectRatio );
  }


// ---------------------------------------------------------------------------
// SetBgColorL
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::SetBgColorL( const TRgb& aBgColor )
  {
  iBackgroundColor = aBgColor;
  }


// ---------------------------------------------------------------------------
// SetPositionL
// ---------------------------------------------------------------------------
//
void
CCamSnapshot::SetPositionL( const TPoint& aPosition )
  {
  iPosition = aPosition;
  }


// ---------------------------------------------------------------------------
// IsSnapshotActive
// ---------------------------------------------------------------------------
//
TBool 
CCamSnapshot::IsSnapshotActive() const
  {
  return iSnapshotOn;
  }

// ---------------------------------------------------------------------------
// StartSnapshot
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::StartSnapshot()
  {
  iSnapshotOn = ETrue;
  }


// ---------------------------------------------------------------------------
// StopSnapshot
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::StopSnapshot()
  {
  iSnapshotOn = EFalse;

  iUseNextVfFrame = EFalse;
  }


// ---------------------------------------------------------------------------
// SnapshotDataL
// ---------------------------------------------------------------------------
//
MCameraBuffer& 
CCamSnapshot::SnapshotDataL( RArray<TInt>& aFrameIndexOrder )
  {
  PRINT( _L("Camera => CCamSnapshot::SnapshotDataL") );

  // Leave if not ready or other error.
  User::LeaveIfError( iStatus );

  if( !iSnapshotOn || !iSnapshotBitmap )
    {
    User::Leave( KErrNotReady );
    }

  // Set the frame order
  aFrameIndexOrder.Reset();
  User::LeaveIfError( aFrameIndexOrder.Append( 0 ) );

  // Client *must* call Release() for the buffer
  CCamBuffer* buffer = CCamBuffer::NewL( *iSnapshotBitmap, NULL );

  PRINT( _L("Camera <= CCamSnapshot::SnapshotDataL") );
  return *buffer;
  }


// ===========================================================================
// From MCamCameraObserver

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::HandleCameraEventL( TInt              aStatus, 
                                  TCamCameraEventId aEventId, 
                                  TAny*             aEventData /*= NULL*/ )
  {
  if( iSnapshotOn )
    {
    switch( aEventId )
      {
      // ---------------------------------------------------
      // Viewfinder frame ready event
      //
      case ECamCameraEventVfFrameReady:
        {
        if( iUseNextVfFrame )
          {
          iUseNextVfFrame = EFalse;
  
          // Take the viewfinder frame just as it would be 
          // a still image, just in bitmap format.
          PRINT( _L("Camera <> CCamSnapshot: Storing VF frame as snapshot") );


          // TEMP ==>
          CCamBufferShare* share = NULL;
          TRAPD( error,
            {
            if( KErrNone == aStatus )
              {
              CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
              CleanupStack::PushL( bitmap );
              User::LeaveIfError( bitmap->Duplicate( static_cast<CFbsBitmap*>( aEventData )->Handle() ) );
  
              CCamBuffer* buffer = CCamBuffer::NewL( bitmap, NULL );
              CleanupStack::Pop( bitmap );

              CleanupStack::PushL( buffer );             
              share = new (ELeave) CCamBufferShare( buffer );
              CleanupStack::Pop( buffer );
              }
            });
          if( KErrNone != error )
            {
            aStatus = error;
            }
          if( share ) share->Reserve();
          // <== TEMP 


          StartSnapshotProcessing( share, aStatus );

          // TEMP ==>
          if( share ) share->Release();
          // <== TEMP 
          }
        break;
        }
      // ---------------------------------------------------
      // Image captured event 
      //
      // Need to decode the snapshot from the image data.
      case ECamCameraEventImageData:
        {
        if( !iVideoMode )
          {
          PRINT( _L("Camera <> CCamSnapshot: Starting to decode snapshot") );
          CCamBufferShare* share = static_cast<CCamBufferShare*>( aEventData );
          StartSnapshotProcessing( share, aStatus );
          }
        break;
        }
      // ---------------------------------------------------
      // Video init might come before snapshot is 
      // initialized, so we do not rely on this event
      // case ECamCameraEventVideoInit:
      //  {        
      //  PRINT( _L("Camera <> CCamSnapshot: Video mode entered") );
      //  break;
      //  }
      // ---------------------------------------------------
      case ECamCameraEventVideoRelease:
        {
        PRINT( _L("Camera <> CCamSnapshot: Video mode left") );
        iVideoMode = EFalse;
        break;
        }
      // ---------------------------------------------------
      // Video started event
      //
      // Use next vf frame as snapshot.
      case ECamCameraEventVideoStart:
        {
        PRINT( _L("Camera <> CCamSnapshot: Video started, will use next VF frame as snapshot") );
        SetImageData( NULL );
        if( KErrNone == aStatus )
          {
          iVideoMode      = ETrue;
          iUseNextVfFrame = ETrue;
          }
        break;
        }
      // ---------------------------------------------------
      // Video stopped. 
      //
      // Provide the vf frame as snapshot.
      case ECamCameraEventVideoStop:
        {
        if( iVideoMode )
          {
          PRINT( _L("Camera <> CCamSnapshot: Video stopped, provide the VF frame snapshot..") );
  
          // Cancel any activity, if not ready for some reason.
          if( iIdle    ) iIdle->Cancel();
          if( iScaler  ) iScaler->Cancel();
          if( iDecoder ) iDecoder->Cancel(); 

          // Snapshot is in iSnapshotBitmap
          SetImageData( NULL );

          // Need to clear this flag as no events are notified
          // when this flag is on.
          iVideoMode = EFalse;

          PRINT( _L("Camera <> CCamSnapshot: Start snapshot callback") );
          iIdle->Start( TCallBack( SnapshotReadyCallback, this ) );        
          }
        break;
        }
      // ---------------------------------------------------
      default:
        {
        // Other events ignored
        break;
        }
      // ---------------------------------------------------
      }
    }
  }




// ===========================================================================
// from MCamImageDecoderObserver

// ---------------------------------------------------------------------------
// ImageDecoded <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::ImageDecoded( TInt aStatus, const CFbsBitmap* aBitmap, const CFbsBitmap* /*aMask*/ )
  {
  PRINT1( _L("Camera => CCamSnapshot::ImageDecoded, status in: %d"), aStatus );
  if( iSnapshotOn )
    {
    iStatus = aStatus;

    iSnapshotBitmap->Reset();

    if( iStatus == KErrNone )
      {
      iStatus = iSnapshotBitmap->Duplicate( aBitmap->Handle() );
      }
    PRINT1( _L("Camera <> status after bitmap duplicate: %d"), iStatus );

    // Release image data, as not used anymore.
    // All that is needed is iSnapshotBitmap.
    SetImageData( NULL );

    if( iStatus == KErrNone )
      {
      iScaler->StartScaling( *iSnapshotBitmap );
      }
    else
      {
      iIdle->Start( TCallBack( SnapshotReadyCallback, this ) );
      }
    }
  PRINT( _L("Camera <= CCamSnapshot::ImageDecoded") );
  }

// ===========================================================================
// from MCamBitmapScalerObserver

// ---------------------------------------------------------------------------
// BitmapScaled <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::BitmapScaled( TInt aStatus, const CFbsBitmap* aBitmap )
  {
  // Release any data we hold, as it's not needed anymore.
  SetImageData( NULL );

  if( iSnapshotOn )
    {
    iStatus = aStatus;
    iSnapshotBitmap->Reset();

    if( KErrNone == iStatus )
      {
      iStatus = iSnapshotBitmap->Duplicate( aBitmap->Handle() );
      }

    TECAMEvent event( KUidECamEventCameraSnapshot, iStatus );
    iObserver.HandleEvent( event );
    }
  }  


// ===========================================================================
// public new methods

// ---------------------------------------------------------------------------
// StartSnapshotProcessing
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::StartSnapshotProcessing( CCamBufferShare* aBuffer,
                                       TInt             aError )
  {
  PRINT1( _L("Camera => CCamSnapshot::StartSnapshotProcessing, status in:%d"), aError );

  if( iSnapshotOn )
    {
    TRAP( iStatus, DoStartSnapshotProcessingL( aBuffer, aError ) );

    if( iStatus != KErrNone )
      {
      iIdle->Start( TCallBack( SnapshotReadyCallback, this ) );
      }
    }

  PRINT1( _L("Camera <= CCamSnapshot::StartSnapshotProcessing, status out:%d"), aError );
  }


// ---------------------------------------------------------------------------
// SnapshotReadyCallback <<static>>
// ---------------------------------------------------------------------------
//
// static 
TInt
CCamSnapshot::SnapshotReadyCallback( TAny* aSelf )
  {
  PRINT( _L("Camera => CCamSnapshot::SnapshotReadyCallback <<static>>") );
  CCamSnapshot* self( static_cast<CCamSnapshot*>( aSelf ) );

  if( !self->iVideoMode )
    {
    TECAMEvent event( KUidECamEventCameraSnapshot, self->iStatus );
    self->iObserver.HandleEvent( event );
    }
  else
    {
    // In video mode we must wait for the video stopped event
    // even to report error. 
    PRINT( _L("Camera <> CCamSnapshot: Video mode, wait until video stopped.") );
    }

  PRINT( _L("Camera <= CCamSnapshot::SnapshotReadyCallback <<static>>") );
  return EFalse;
  }



// ===========================================================================
// private methods

// ---------------------------------------------------------------------------
// DoStartSnapshotProcessingL
//
// Helper method for StartSnapshotProcessing.
// ---------------------------------------------------------------------------
//
void
CCamSnapshot::DoStartSnapshotProcessingL( CCamBufferShare* aBuffer,
                                          TInt             aError )
  {
  PRINT1( _L("Camera => CCamSnapshot::DoStartSnapshotProcessingL, status in:%d"), aError );

  // Cancel all ongoing activity.
  iDecoder->Cancel();
  iScaler->Cancel();
  SetImageData( NULL ); // Release any old.

  // Check that we have the data needed.
  CheckNonNullL( aBuffer,                 KErrNotFound );
  CheckNonNullL( aBuffer->SharedBuffer(), KErrNotFound );
  User::LeaveIfError( aError );


  SetImageData( aBuffer );
  MCameraBuffer* buffer = iImageData->SharedBuffer();

  // First try to use bitmap data if available
  TRAP( aError, 
    {
    CFbsBitmap& bitmap = buffer->BitmapL( 0 );
    PRINT( _L("Camera <> Using bitmap data, just scale..") );
    // Simulate that the bitmap has been decoded now.
    ImageDecoded( aError, &bitmap, NULL );
    });

  // If problems with bitmap data, try encoded data.
  if( KErrNone != aError )
    {    
    PRINT( _L("Camera <> Using encoded data, decode first") );
    // If not able to use bitmap (or there is none),
    // start converting bitmap from encoded image data.
    TRAP( aError, iDecoder->StartConversionL( iImageData ) );
    }


  // Neither of the formats could be used.
  // Release the image data.
  if( KErrNone != aError )
    {
    SetImageData( NULL );
    User::Leave( aError );
    }
  PRINT( _L("Camera <= CCamSnapshot::DoStartSnapshotProcessingL") );
  }


// ---------------------------------------------------------------------------
// Format2DisplayMode
// ---------------------------------------------------------------------------
//
TDisplayMode 
CCamSnapshot::Format2DisplayMode( CCamera::TFormat aFormat ) const
  {
  switch( aFormat )
    {
    case CCamera::EFormatFbsBitmapColor4K:   return EColor4K;
    case CCamera::EFormatFbsBitmapColor64K:  return EColor64K;
    case CCamera::EFormatFbsBitmapColor16M:  return EColor16M;
    case CCamera::EFormatFbsBitmapColor16MU: return EColor16MU;
    default:                                 return EColor16MU;
    }
  }



// ---------------------------------------------------------------------------
// SetImageData
//
// Release old shared buffer (if any exists) and store pointer to new one and
// reserve it (if any provided). Can be used to just release any existing
// share with NULL parameter.
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::SetImageData( CCamBufferShare* aImageData )
  {
  if( iImageData )
    {
    iImageData->Release();
    }

  iImageData = aImageData;

  if( iImageData )
    {
    iImageData->Reserve();
    }    
  }



// ===========================================================================
// private constructors

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void 
CCamSnapshot::ConstructL()
  {
  iSnapshotBitmap = new (ELeave) CFbsBitmap;

  iDecoder = CCamImageDecoder::NewL( *this );
  iScaler  = CCamBitmapScaler::NewL( *this );
  
  iIdle = CIdle::NewL( KCallbackPriority );
  }

// ---------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------
//
CCamSnapshot::CCamSnapshot( CCamera&              aCamera, 
                            MCameraObserver2&     aObserver,
                            MCamCameraObservable& aObservable )
  : iObserver( aObserver ),
    iObservable( aObservable ),
    iCameraHandle( aCamera.Handle() ),
    iSnapshotOn( EFalse ),
    iStatus( KErrNotReady )
  {
  }

// end of file
