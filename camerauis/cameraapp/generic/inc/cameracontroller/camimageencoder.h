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
* Description:  Image Encoder class.
*
*/


#ifndef CAM_IMAGEENCODER_H
#define CAM_IMAGEENCODER_H

// ===========================================================================
// Included headers
#include <e32std.h>
#include <f32file.h>

// ===========================================================================
// Forward declarations
class CImageEncoder;
class CCamBufferShare;


// ===========================================================================
// Classes

/**
* Interface for Image Encoder Observers
*/
class MCamImageEncoderObserver
  {
  public:
  
    /**
    * Notify observer that the image has been encoded.
    * @param aStatus Status code describing the success of the operation.
    *                KErrNone if all went as planned.
    * @param aData   Encoded image data. NULL if errors in decoding.
    *                Ownership transferred to observer.
    */
    virtual void ImageEncoded( TInt aStatus, HBufC8* aData ) = 0;
  };
  

/**
* Image Encoder class
*/
class CCamImageEncoder : public CActive
  {
  // =======================================================
  public:

    /**
    * 2-phase constructor.
    */
    static CCamImageEncoder* NewL( MCamImageEncoderObserver& aObserver );
    
    /**
    * Destructor.
    */
    virtual ~CCamImageEncoder();

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

  private:

    /**
    * Store the buffer and release any previous buffer.
    * Can be called with NULL to release current buffer.
    */  
    void SetInputBuffer( CCamBufferShare* aBuffer );

    /**
    * Free all encoding related resources.
    */
    void ReleaseEncoder();
    
    /**
    * Notify observer. 
    * Transfers the encoding data ownership to the observer.
    */
    void NotifyObserver( TInt aStatus );

  private:

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

    /**
    * 1st phase constructor.
    * Cannot leave.
    */
    CCamImageEncoder( MCamImageEncoderObserver& aObserver );

  // =======================================================
  // Data
  private:

    MCamImageEncoderObserver& iObserver;

    CCamBufferShare*          iSharedInput;
    HBufC8*                   iEncodedData;

    CImageEncoder*            iEncoder;

  // =======================================================
  };

// ===========================================================================
#endif // CAM_IMAGEENCODER_H

// end of file
