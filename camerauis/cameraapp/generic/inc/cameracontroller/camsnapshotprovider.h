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
* Description:  CCamSnapshotProvider class definition
*
*/


#ifndef CAM_SNAPSHOTPROVIDER_H
#define CAM_SNAPSHOTPROVIDER_H


#include "camcameracontrollerflags.hrh" // build flags, keep first
#include <ecam/mcamerasnapshot.h>


class CCamera;
class MCameraObserver2;
class MCamCameraObservable;
class CPeriodic;
class CCamSnapshot;


#ifdef CAMERAAPP_CAPI_V2_SS    
  #define CAMERASNAPSHOT CCamera::CCameraSnapshot
#else
  // To be able to link without additional build time variation 
  // in source files.
  #define CAMERASNAPSHOT CCamSnapshot
#endif


// ===========================================================================
// 
class CCamSnapshotProvider :  public CBase,
                              public MCameraSnapshot
  {
  // -------------------------------------------------------
  // Constructors and destructor
  public:

    static CCamSnapshotProvider* NewL( CCamera&              aCamera,
                                       MCameraObserver2&     aObserver,
                                       MCamCameraObservable& aObservable );

    virtual ~CCamSnapshotProvider();

  private:

    /**
    * 2nd phase constructor
    */
    void ConstructL( CCamera&              aCamera, 
                     MCameraObserver2&     aObserver,
                     MCamCameraObservable& aObservable );

    /**
    * 1st phase constructor
    */
    CCamSnapshotProvider();

  // -------------------------------------------------------
  // From MCameraSnapshot
  //
  // All inlines as just relayed to the currently used
  // snapshot producer.
  public:

  	inline virtual TUint32        SupportedFormats();
    inline virtual void           PrepareSnapshotL( CCamera::TFormat aFormat, const TPoint& aPosition, const TSize& aSize, const TRgb& aBgColor, TBool aMaintainAspectRatio );
  	inline virtual void           PrepareSnapshotL( CCamera::TFormat aFormat, const TSize& aSize, TBool aMaintainAspectRatio );
  	inline virtual void           SetBgColorL( const TRgb& aBgColor);
  	inline virtual void           SetPositionL( const TPoint& aPosition);
    inline virtual TBool          IsSnapshotActive() const;
    inline virtual void           StartSnapshot();
  	inline virtual void           StopSnapshot();
  	inline virtual MCameraBuffer& SnapshotDataL( RArray<TInt>& aFrameIndexOrder );
  	inline virtual void           Release();

  // =======================================================
  // Data  
  private:

    CAMERASNAPSHOT* iSs1;
    CCamSnapshot*   iSs2;

  // =======================================================
  };

#include "camsnapshotprovider.inl"

// ===========================================================================
#endif // CAM_SNAPSHOTPROVIDER_H

// end of file
