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
* Description:  Declaration of CCamBuffer class.
*                Temporary own implementation of MCameraBuffer. 
*                
*
*/




#ifndef C_CAM_BUFFER_H
#define C_CAM_BUFFER_H


#include <e32base.h> // CBase
#include <ecam.h>    // MCameraBuffer


class CFbsBitmap;


class CCamBuffer : public CBase, 
                   public MCameraBuffer
  {
  // =======================================================
  public:

    static CCamBuffer* NewL( const CFbsBitmap& aBitmap, 
                                   HBufC8*     aData );
    static CCamBuffer* NewLC( const CFbsBitmap& aBitmap, 
                                    HBufC8*     aData );

    static CCamBuffer* NewL( CFbsBitmap* aBitmap, 
                             HBufC8*     aData );
    static CCamBuffer* NewLC( CFbsBitmap* aBitmap, 
                              HBufC8*     aData );

        
  // from MCameraBuffer
  public:

    virtual TInt NumFrames();
    virtual TDesC8* DataL( TInt aFrameIndex );
    virtual CFbsBitmap& BitmapL( TInt aFrameIndex );
    virtual RChunk& ChunkL();
    virtual TInt ChunkOffsetL( TInt aFrameIndex );
    virtual TInt FrameSize( TInt aFrameIndex );
    virtual void Release();

  // new methods
  public:
  protected:
  private:
    
    void ConstructL( const CFbsBitmap& aBitmap, 
                           HBufC8*     aData );

    void ConstructL( CFbsBitmap* aBitmap, 
                     HBufC8*     aData );

    CCamBuffer();

    ~CCamBuffer(); /** private to force use of Release() */

  // =======================================================
  // Data
  public:
    // Defined in MCameraBuffer
    //     TInt iIndexOfFirstFrameInBuffer;
    //     TTimeIntervalMicroSeconds iElapsedTime;
  protected:
  private:
  
    CFbsBitmap* iBitmap;
    HBufC8*     iImageData;
    RChunk      iChunk; // Needed to reference

    TBool iOwnBitmap;
    TBool iOwnData;
  // =======================================================
  };  


#endif // C_CAM_BUFFER_H

// end of file
