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
* Description:  Implementation of Camera image decoder
*
*/


#include <imageconversion.h>
#include <ExifRead.h>
#include <ecam.h> // MCameraBuffer

#include <eikenv.h>
#include <SvgCodecImageConstants.hrh>
#include <SVGEngineInterfaceImpl.h>

#include "camlogging.h"
#include "camfilesaveutility.h"
#include "cambuffershare.h"

#include "camimagedecoder.h"

// ===========================================================================
// Local constants

namespace NCamCameraController
  {
  // Decoding flags
  static const TUint32 KConversionOptions = 
    ( CImageDecoder::EOptionNone
    | CImageDecoder::EOptionAlwaysThread
    | CImageDecoder::EPreferFastDecode
    );
  
  // EXIF reader flags
  //   Only need the thumbnail, so no need to parse the
  //   main (jpeg) image.
  static const TUint KExifReaderFlags = CExifRead::ENoJpeg;
  
  // Other constants
  static const TInt KMaxRetries = 10;
  static const TInt KPriority = CActive::EPriorityHigh;
  }

using namespace NCamCameraController;

// ===========================================================================
// public constructor and destructor

CCamImageDecoder* 
CCamImageDecoder::NewL( MCamImageDecoderObserver& aObserver )
  {
  CCamImageDecoder* self = 
      new (ELeave) CCamImageDecoder( aObserver );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;  
  }


CCamImageDecoder::~CCamImageDecoder()
  {
  PRINT( _L("Camera => ~CCamImageDecoder") );
  Cancel();

  delete iDecoder;

  SetImageData( NULL );
  if(iDecodedBitmap)
  	{
  	delete iDecodedBitmap;
  	iDecodedBitmap = NULL;
  	}
  if ( iDecodedMask )
     {
     delete iDecodedMask;
     iDecodedMask = NULL;
     }

  delete iSvgEngine;
  iFs.Close();
  PRINT( _L("Camera <= ~CCamImageDecoder") );
  }

// ===========================================================================
// public methods
void
CCamImageDecoder::StartConversionL( CCamBufferShare* aBuffer )
  {
  PRINT( _L("Camera => CCamImageDecoder::StartConversionL") );

  // Data for CImageDecoder must be available throughout the conversion.
  // Need to stop any outstanding operation before deleting the descriptor.
  Cancel();

  PRINT( _L("Camera <> CCamImageDecoder: Copying image data..") );
  
  SetImageData( aBuffer );

  // GetImageDataL leaves if no data available.
  TDesC8* imageData = GetImageDataL();

  PRINT( _L("Camera <> CCamImageDecoder: Creating decoder..") );

  delete iDecoder;
  iDecoder = NULL;
  iDecoder = CImageDecoder::DataNewL( iFs, *imageData, (CImageDecoder::TOptions)KConversionOptions );

  if( iDecoder->FrameCount() > 0 )
    {
    const TFrameInfo& info( iDecoder->FrameInfo() );
    
#ifdef _DEBUG   
    TSize size = info.iOverallSizeInPixels;
    PRINT2( _L("Camera <> CCamImageDecoder: Bmp size(%d,%d)"), size.iWidth, size.iHeight );
    PRINT1( _L("Camera <> CCamImageDecoder: Bmp dispmode(%d)"), info.iFrameDisplayMode );
#endif

    PRINT( _L("Camera <> CCamImageDecoder: Create bitmap for snapshot..") );
    if( !iDecodedBitmap ) iDecodedBitmap = new (ELeave) CFbsBitmap;
    else                  iDecodedBitmap->Reset();
    
    TInt createError = iDecodedBitmap->Create( info.iOverallSizeInPixels, 
                                               info.iFrameDisplayMode );
    if( KErrNone != createError )
      {
      delete iDecodedBitmap;
      iDecodedBitmap = NULL;
      User::Leave( createError );
      }

    PRINT( _L("Camera <> CCamImageDecoder: start conversion..") );
    iRetryCounter = 0;
    iDecoder->Convert( &iStatus, *iDecodedBitmap, 0 );
    SetActive();
    }
  else
    {
    PRINT( _L("Camera <> CCamImageDecoder: No frame provided, leave..") );
    User::Leave( KErrNotFound );    
    }

  PRINT( _L("Camera <= CCamImageDecoder::StartConversionL") );
  }

// ---------------------------------------------------------------------------
// CCamImageDecoder::StartIconConversionL
// ---------------------------------------------------------------------------
//
void CCamImageDecoder::StartIconConversionL( TDesC* aFilePath, TSize& aSize )
    {
    PRINT3( _L("Camera => CCamImageDecoder::StartIconConversionL, file:[%S], size:(%d,%d)"),
              &(*aFilePath), aSize.iWidth, aSize.iHeight );

    // Delete any previous bitmaps, if any
    delete iDecodedBitmap,
    iDecodedBitmap = NULL;
    delete iDecodedMask;
    iDecodedMask = NULL;
    
    // Create bitmap for use while decoding 
    CFbsBitmap* frameBuffer = new (ELeave) CFbsBitmap;
    CleanupStack::PushL( frameBuffer );

    TFontSpec spec;
    if ( !iSvgEngine )
        {
        iSvgEngine = CSvgEngineInterfaceImpl::NewL( frameBuffer, NULL, spec );
        }
    
    TInt domHandle = KErrNotFound;
    MSvgError* serr = iSvgEngine->PrepareDom( *aFilePath, domHandle );
    PRINT3( _L("Camera <> prepare svg dom reader, warning:%d, err code:%d, description:[%S]"), 
                serr->IsWarning(), serr->ErrorCode(), &(serr->Description()) );
    if ( serr->HasError() && !serr->IsWarning() )
        {
        PRINT1( _L("Camera <> leaving with error:%d"), serr->SystemErrorCode() );
        User::Leave( serr->SystemErrorCode() );
        }

    // create image bitmap
    PRINT( _L("Camera <> CCamImageDecoder: Create bitmap for snapshot..") );
    if( !iDecodedBitmap ) iDecodedBitmap = new (ELeave) CFbsBitmap;
    else                  iDecodedBitmap->Reset();

    if( !iDecodedMask ) iDecodedMask = new (ELeave) CFbsBitmap;
    else                iDecodedMask->Reset();

    TRAPD ( createError, 
        {
        iDecodedBitmap->Create( aSize, EColor64K );
        iDecodedMask->Create( aSize, EGray256 );
        } );
    if( createError )
      {
      PRINT1( _L("Camera <> Error while creating bitmaps:%d"), createError );
      delete iDecodedBitmap;
      iDecodedBitmap = NULL;
      delete iDecodedMask;
      iDecodedMask = NULL;
      User::Leave( createError );
      }

    // create soft mask
    iSvgEngine->SetViewportHeight((CSvgDocumentImpl *)domHandle, aSize.iHeight);
    iSvgEngine->SetViewportWidth((CSvgDocumentImpl *)domHandle, aSize.iWidth);

    // render svg image
    serr = iSvgEngine->RenderDom( domHandle, iDecodedBitmap, iDecodedMask );
    PRINT3( _L("Camera <> render svg, warning:%d, err code:%d, description:[%S]"), 
                serr->IsWarning(), serr->ErrorCode(), &(serr->Description()) );
    if ( serr->HasError() && !serr->IsWarning() )
        {
        PRINT1( _L("Camera <> leaving with error:%d"), serr->SystemErrorCode() );
        User::Leave( serr->SystemErrorCode() );
        }

    CleanupStack::PopAndDestroy( frameBuffer );
    if ( !IsActive() )
        {
        iStatus=KRequestPending;
        TRequestStatus *pS=(&iStatus);
        User::RequestComplete(pS,0);
        SetActive();
        }

    PRINT( _L("Camera <= CCamImageDecoder::StartIconConversionL") );
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TDesC8*
CCamImageDecoder::GetImageDataL()
  {
  PRINT( _L("Camera => CCamImageDecoder::GetImageDataL") );
  
  if( !iSharedImageData ||
      !iSharedImageData->SharedBuffer() )
    {
    User::Leave( KErrNotFound );
    }

  TDesC8* data = iSharedImageData->SharedBuffer()->DataL( 0 );

  if( !data )
    User::Leave( KErrNotFound );

  delete iThumbnailData;
  iThumbnailData = ReadExifThumbNail( *data ); // Returns NULL on errors

#ifdef CAMERAAPP_CREATE_TESTIMAGE
  TRAPD( saveStatus1, SaveImageDataToFileL( *data, _L("testimagefull.jpg") ) );
  PRINT1( _L("Camera <> CCamImageDecoder: Save full image to file status:%d"), saveStatus1 );
  if( iThumbnailData )
    {
    TRAPD( saveStatus2, SaveImageDataToFileL( *iThumbnailData, _L("testimagethumb.jpg") ) );
    PRINT1( _L("Camera <> CCamImageDecoder: Save thumbnail to file status:%d"), saveStatus2 );
    }
#endif


  if( iThumbnailData )
    {
    data = iThumbnailData;
    }  

  PRINT1( _L("Camera <= CCamImageDecoder::GetImageDataL, data size:%d"), data->Size() );
  return data;
  }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HBufC8*
CCamImageDecoder::ReadExifThumbNail( const TDesC8& aExifData )
  {
  PRINT( _L("Camera => CCamImageDecoder::ReadExifThumbNailL") );
  HBufC8* thumb = NULL;
  TRAP_IGNORE( 
    {
    CExifRead* reader = CExifRead::NewL( aExifData, KExifReaderFlags );
    CleanupStack::PushL( reader );
    thumb = reader->GetThumbnailL();
    CleanupStack::PopAndDestroy( reader );
    });

  PRINT( _L("Camera <= CCamImageDecoder::ReadExifThumbNailL") );
  return thumb;
  }

// ===========================================================================
// from CActive

// ---------------------------------------------------------------------------
// virtual 
// ---------------------------------------------------------------------------
//
void 
CCamImageDecoder::DoCancel()
  {
  PRINT( _L("Camera => CCamImageDecoder::DoCancel") );

  if( iDecoder )
    {
    iDecoder->Cancel();
    delete iDecoder;
    iDecoder = NULL;    
    }

  SetImageData( NULL );

  PRINT( _L("Camera <= CCamImageDecoder::DoCancel") );
  }


// ---------------------------------------------------------------------------
// virtual 
// ---------------------------------------------------------------------------
//
void 
CCamImageDecoder::RunL()
  {
  PRINT1( _L("Camera => CCamImageDecoder::RunL, iStatus:%d"), iStatus.Int() );

  switch( iStatus.Int() )
    {
    case KErrNone :
      {
      // CImageDecoder has finished using the data,
      // so we are able to free it.
      if ( iSvgEngine )
          {
          iSvgEngine->Destroy();
          delete iSvgEngine;
          iSvgEngine = NULL;
          }
      SetImageData( NULL );
      iObserver.ImageDecodedL( iStatus.Int(), iDecodedBitmap, iDecodedMask );
      break;
      }
    case KErrUnderflow :
      {
      // Decoder did not have enough data to convert.
      // CImageDecoder documentation recommends calling
      // repeatedly ContinueConvert.
      if( iRetryCounter++ < KMaxRetries )
        {
        iStatus = KErrNone;
        iDecoder->ContinueConvert( &iStatus );
        SetActive();
        }
      else
        {
        // Handled in RunError
        User::Leave( KErrUnderflow );
        }
      break;
      }
    case KErrCancel :
    default :
      {
      User::Leave( iStatus.Int() );
      break;
      }
    }

  PRINT( _L("Camera <= CCamImageDecoder::RunL") );
  }


// ---------------------------------------------------------------------------
// virtual 
// ---------------------------------------------------------------------------
//
TInt 
CCamImageDecoder::RunError( TInt aError )
  {
  PRINT1( _L("Camera => CCamImageDecoder::RunError(%d)"), aError );

  SetImageData( NULL );
  // Leave has occurred in RunL.
  // Notify observer with error.
  TRAP_IGNORE(iObserver.ImageDecodedL( aError, NULL, NULL ));

  PRINT( _L("Camera <= CCamImageDecoder::RunError") );
  return KErrNone;
  }


// ===========================================================================
void
CCamImageDecoder::SetImageData( CCamBufferShare* aBuffer )
  {
  if( iSharedImageData )
    {
    iSharedImageData->Release();
    iSharedImageData = NULL;
    }

  iSharedImageData = aBuffer;

  if( iSharedImageData )
    iSharedImageData->Reserve();
  }

// ===========================================================================
// private constructors


void
CCamImageDecoder::ConstructL()
  {
  User::LeaveIfError( iFs.Connect() );

  CActiveScheduler::Add( this );
  }



CCamImageDecoder::CCamImageDecoder( MCamImageDecoderObserver& aObserver )
  : CActive( KPriority  ),
    iObserver( aObserver )
  {
  }

// end of file
