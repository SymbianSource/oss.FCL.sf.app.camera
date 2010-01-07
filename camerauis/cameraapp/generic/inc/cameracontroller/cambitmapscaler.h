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
* Description:  Camera Bitmap Scaler class and observer interface.
*
*/


#ifndef CAM_BITMAPSCALER_H
#define CAM_BITMAPSCALER_H

// ===========================================================================
// Included headers
#include <e32std.h>
#include <f32file.h>

// ===========================================================================
// Forward declarations
class CFbsBitmap;
class CBitmapScaler;

// ===========================================================================
// Classes

/**
* Interface for Camera Bitmap Scaler Observers
*/
class MCamBitmapScalerObserver
  {
  public:
  
    /**
    * Notify observer that the bitmap scaling has finished.
    * @param aStatus Status code describing the success of the operation.
    *        KErrNone if all went as planned.
    * @param aBitmap Scaled bitmap. NULL if errors in scaling.
    */
    virtual void BitmapScaled( TInt aStatus, const CFbsBitmap* aBitmap ) = 0;

  };
  

/**
* Image Decoder class
*/
class CCamBitmapScaler : public CActive
  {
  // =======================================================
  public:

    /**
    * 2-phase constructor.
    */
    static CCamBitmapScaler* NewL( MCamBitmapScalerObserver& aObserver );
    
    /**
    * Destructor.
    */
    virtual ~CCamBitmapScaler();

  // -------------------------------------------------------
  // From CActive
  protected: 

    /**
    * Called when Cancel is requested and this AO is active.
    * @see CActive
    */
    virtual void DoCancel();

    /**
    * We get notified of the scaling success by a call to RunL
    * when scaling finishes.
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

    void InitScalingL( const TSize&        aTargetSize,
                       const TDisplayMode& aTargetDisplayMode,
                             TBool         aMaintainAspect );

    void StartScaling( CFbsBitmap& aSourceBitmap );

  private:

    /**
    * 2nd phase constructor.
    */
    void ConstructL();

    /**
    * 1st phase constructor.
    * Cannot leave.
    */
    CCamBitmapScaler( MCamBitmapScalerObserver& aObserver );

  // =======================================================
  // Data
  private:

    MCamBitmapScalerObserver& iObserver;
    CBitmapScaler*            iScaler;
    CFbsBitmap*               iScaledBitmap;
    TBool                     iMaintainAspect;
    
  // =======================================================
  };

// ===========================================================================
#endif // CAM_BITMAPSCALER_H

// end of file
