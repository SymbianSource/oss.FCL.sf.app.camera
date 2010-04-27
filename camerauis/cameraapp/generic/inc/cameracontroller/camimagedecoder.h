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
* Description:  Image Decoder class and observer interface.
*
*/


#ifndef CAM_IMAGEDECODER_H
#define CAM_IMAGEDECODER_H

// ===========================================================================
// Included headers
#include <e32std.h>
#include <f32file.h>

// ===========================================================================
// Forward declarations
class CFbsBitmap;
class CImageDecoder;
class MCameraBuffer;
class CCamBufferShare;


// ===========================================================================
// Classes

/**
* Interface for Image Decoder Observers
*/
class MCamImageDecoderObserver
  {
  public:
  
    /**
    * Notify observer that the image decoding has finished.
    * @param aStatus Status code describing the success of the operation.
    *        KErrNone if all went as planned.
    * @param aBitmap Decoded bitmap. NULL if errors in decoding.
    */
    virtual void ImageDecodedL( TInt aStatus, const CFbsBitmap* aBitmap, const CFbsBitmap* aMask ) = 0;

  };
  

/**
* Image Decoder class
*/
class CCamImageDecoder : public CActive
  {
  // =======================================================
  public:

    /**
    * 2-phase constructor.
    */
    static CCamImageDecoder* NewL( MCamImageDecoderObserver& aObserver );
    
    /**
    * Destructor.
    */
    virtual ~CCamImageDecoder();

  // -------------------------------------------------------
  // From CActive
  protected: 

    /**
    * Called when Cancel is requested and this AO is active.
    * @see CActive
    */
    virtual void DoCancel();

    /**
    * We get notified of the decoding success by a call to RunL
    * when decoding finishes.
    * @see CActive
    */
    virtual void RunL();
    
    /**
    * If a leave occurs in RunL, RunError gets called by the 
    * Active Scheduler.
    * @param aError Leave code from RunL.
    * @see CActive
    */
    virtual TInt RunError( TInt aError );

  // -------------------------------------------------------
  public:

    void StartConversionL( CCamBufferShare* aBuffer );
    
    void StartIconConversionL( TDesC* aFilePath );

  private:

    /**
    * Store the buffer and release any previous buffer.
    * Can be called with NULL to release current buffer.
    */  
    void SetImageData( CCamBufferShare* aBuffer );

    /**
    * Get the image data in descriptor form.
    * No new memory is allocated to the returned buffer,
    * so ownership not transferred.
    * @return Pointer to image data that is to be used for image conversion.
    *         If EXIF thumbnail is available, it is used, otherwise the
    *         full image data is used instead. If the image data cannot be
    *         read from the buffer, or the buffer has not been set,
    *         leave will occur.
    * @leave  KErrNotFound, if image buffer is not set or contains no image data. 
    *         Any other system error, e.g. KErrNoMemory.
    */
    TDesC8* GetImageDataL();

    /**
    * Read the EXIF thumbnail and return it in descriptor.
    * If any errors, return NULL.
    * @param  aExifData The full image data containing EXIF part.
    * @return The EXIF thumbnail in a heap descriptor. NULL if
    *         error occurs in reading (OOM, no data given, 
    *         no EXIF thumbnail found..).
    */
    HBufC8* ReadExifThumbNail( const TDesC8& aExifData );

  private:

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

    /**
    * 1st phase constructor.
    * Cannot leave.
    */
    CCamImageDecoder( MCamImageDecoderObserver& aObserver );

  // =======================================================
  // Data
  private:

    RFs                       iFs;
    MCamImageDecoderObserver& iObserver;
    

    CCamBufferShare* iSharedImageData;
    HBufC8*          iThumbnailData;

    CFbsBitmap*    iDecodedBitmap;
    CFbsBitmap*    iDecodedMask;
    CImageDecoder* iDecoder;    
    
    TInt           iRetryCounter;

  // =======================================================
  };

// ===========================================================================
#endif

// end of file
