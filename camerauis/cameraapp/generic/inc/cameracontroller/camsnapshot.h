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
* Description:  CCamSnapshot class definition
*
*/


#ifndef CAM_SNAPSHOT_H
#define CAM_SNAPSHOT_H

#include <e32base.h>
#include <ecam.h>
#include "mcamcameraobserver.h" // MCamCameraObserver, MCamCameraObservable
#include "camimagedecoder.h"    // MCamImageDecoderObserver
#include "cambitmapscaler.h"    // MCamBitmapScalerObserver


class CCamBufferShare;

/**
* @class CCamSnapshot
*
*/
class CCamSnapshot : public CBase,
//                     public MCameraSnapshot,
                     public MCamCameraObserver,
                     public MCamImageDecoderObserver,
                     public MCamBitmapScalerObserver
  {
  // -------------------------------------------------------
  // Constructors and destructor
  public:

    static CCamSnapshot* NewL( CCamera&              aCamera, 
                               MCameraObserver2&     aObserver,
                               MCamCameraObservable& aObservable );

    ~CCamSnapshot();

  // -------------------------------------------------------
  // from MCameraSnapshot
  public:
  
    TUint32 SupportedFormats();
    
    void PrepareSnapshotL( CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio);
    void PrepareSnapshotL( CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio );
    
    void SetBgColorL( const TRgb& aBgColor ); 
    void SetPositionL( const TPoint& aPosition );
    
    TBool IsSnapshotActive() const;
    void StartSnapshot();
    void StopSnapshot();
          
    MCameraBuffer& SnapshotDataL( RArray<TInt>& aFrameIndexOrder );

  // -------------------------------------------------------
  // From MCamCameraObserver
  public:

    /**
    * @see MCamCameraObserver
    */
    virtual void HandleCameraEventL( TInt              aStatus, 
                                     TCamCameraEventId aEventId, 
                                     TAny*             aEventData = NULL );

  // -------------------------------------------------------
  // from MCamImageDecoderObserver
  public:
  
    /**
    * @see MCamImageDecoderObserver
    */
    virtual void ImageDecoded( TInt aStatus, const CFbsBitmap* aBitmap );

  // -------------------------------------------------------
  // from MCamBitmapScalerObserver
  public:
  
    /**
    * @see MCamBitmapScalerObserver
    */
    virtual void BitmapScaled( TInt aStatus, const CFbsBitmap* aBitmap );

  // -------------------------------------------------------
  // New methods
  public:

    void StartSnapshotProcessing( CCamBufferShare* aBuffer,
                                  TInt             aError );

    static TInt SnapshotReadyCallback( TAny* aSelf );

  private:

    void DoStartSnapshotProcessingL( CCamBufferShare* aBuffer,
                                     TInt             aError );

    TDisplayMode Format2DisplayMode( CCamera::TFormat aFormat ) const;

    void SetImageData( CCamBufferShare* aImageData );

  private:

    void ConstructL();
    CCamSnapshot( CCamera&              aCamera, 
                  MCameraObserver2&     aObserver,
                  MCamCameraObservable& aObservable );

  // =======================================================
  // data
  private:

    MCameraObserver2&     iObserver;
    MCamCameraObservable& iObservable;

    TInt                  iCameraHandle;

    CCamImageDecoder*     iDecoder;
    CCamBitmapScaler*     iScaler;

    CCamBufferShare*      iImageData;
    CFbsBitmap*           iSnapshotBitmap;    

    TBool                 iSnapshotOn;
    TInt                  iStatus;
    
    // Video snapshot is taken from VF frames.
    // Flags to control the process.
    TBool                 iUseNextVfFrame;
    TBool                 iVideoMode;

    CCamera::TFormat      iFormat;
    TPoint                iPosition;
    TSize                 iSize;
    TRgb                  iBackgroundColor;
    TBool                 iMaintainAspectRatio;

    CIdle*                iIdle;

  // =======================================================
  };

#endif

// end of file

