/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class used to for resizing burst mode thumbnails*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>

#include "CamBurstThumbnailGridSizer.h"
#include "camlogging.h"





// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Symbian 2 phase constructor <<static>>
// -----------------------------------------------------------------------------
//
CScaleTask* 
CScaleTask::NewL( const CFbsBitmap& aBitmap, 
                  const TSize&      aSize,
                  TInt32            aInfo )
  {
  CScaleTask* self = new (ELeave) CScaleTask( aSize, aInfo );
  CleanupStack::PushL( self );
  self->ConstructL( aBitmap );
  CleanupStack::Pop( self );
  return self;
  }

// -----------------------------------------------------------------------------
// CScaleTask destructor <<virtual>>
// -----------------------------------------------------------------------------
//
CScaleTask::~CScaleTask()
  {
  delete iSrc;
  iSrc = NULL;
  }

// -----------------------------------------------------------------------------
// CScaleTask constructor <<virtual>>
// -----------------------------------------------------------------------------
//
CScaleTask::CScaleTask( const TSize&      aSize,
                        TInt32            aInfo )
  : iSize ( aSize ),
    iInfo ( aInfo )
  {
  }

// -----------------------------------------------------------------------------
// CScaleTask::ConstructL
// -----------------------------------------------------------------------------
//
void 
CScaleTask::ConstructL( const CFbsBitmap& aBitmap )
  {
  iSrc = new (ELeave) CFbsBitmap;
  User::LeaveIfError( iSrc->Duplicate( aBitmap.Handle() ) );
  }



// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::CCamThumbnailGridSizer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamThumbnailGridSizer::CCamThumbnailGridSizer( MBitmapScaler* aObserver, TSize& aSize ) 
        : CActive( EPriorityStandard ),
          iThumbSize( aSize )
    {
    iObserver = aObserver;
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamThumbnailGridSizer::ConstructL()
    {
    iScaler = CBitmapScaler::NewL();
    }

// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamThumbnailGridSizer* CCamThumbnailGridSizer::NewL( MBitmapScaler* aObserver, TSize& aSize )
    {
    CCamThumbnailGridSizer* self = new( ELeave ) CCamThumbnailGridSizer( aObserver, aSize );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCamThumbnailGridSizer::~CCamThumbnailGridSizer()
  {
  PRINT( _L("Camera => ~CCamThumbnailGridSizer") );
  Cancel();
  delete iScaler;
  
  // Delete any intermediate temporary bitmap
  delete iTmpBitmap;
  
  // Cancel all pending scaling operations
  iScaleTaskArray.ResetAndDestroy();            
  PRINT( _L("Camera <= ~CCamThumbnailGridSizer") );
  }

// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::SetSize
// -----------------------------------------------------------------------------
//
void CCamThumbnailGridSizer::SetSize( const TSize aThumbSize )
    {
    PRINT2( _L("Camera == CCamThumbnailGridSizer::SetSize width = %d, height = %d"), aThumbSize.iWidth, aThumbSize.iHeight );
    iThumbSize.iWidth = aThumbSize.iWidth;
    iThumbSize.iHeight = aThumbSize.iHeight;
    }

// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::StartScaleL
// -----------------------------------------------------------------------------
//
void CCamThumbnailGridSizer::StartScaleL( const CFbsBitmap& aSrc )
    {
    PRINT( _L("Camera == CCamThumbnailGridSizer::StartScaleL") );
    // Forward request on with default destination size
    StartScaleL( aSrc, iThumbSize, 0 );
    }
    
    
// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::StartScaleL
// Allows specifying of specific size to go to where required.
// -----------------------------------------------------------------------------
//
void CCamThumbnailGridSizer::StartScaleL( const CFbsBitmap& aSrc, TSize& aSize, TInt32 aInfo )
    {
    PRINT3(_L("Camera => CCamThumbnailGridSizer::StartScaleL size %d x %d info 0x%x"), aSize.iWidth, aSize.iHeight, aInfo );

    // Add it to the queue of things to scale
    CScaleTask* task = CScaleTask::NewL( aSrc, aSize, aInfo );    
    CleanupStack::PushL( task );    
    User::LeaveIfError( iScaleTaskArray.Append( task ) );    // takes ownership of "task"
    CleanupStack::Pop( task );

    StartScaleL( task );    
    PRINT ( _L("Camera <= CCamThumbnailGridSizer::StartScaleL") );
    }
    
// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::StartScaleL
// Allows specifying of specific sizing task to start
// -----------------------------------------------------------------------------
//
void CCamThumbnailGridSizer::StartScaleL( CScaleTask* aTask )
    {    
    PRINT( _L("Camera => CCamThumbnailGridSizer::StartScaleL task") );
    // If scaling not currently underway, start it.
    // If scaler is busy, it will be started when current operation completes.            
    if ( !IsActive() )
        {
        // Check that we don't have any temporary images floating around
        if ( iTmpBitmap )
            {
            delete iTmpBitmap;
            iTmpBitmap = NULL;
            }    
        
        TSize        trgSize( aTask->Size()                   );
        const TSize& bmpSize( aTask->Bitmap()->SizeInPixels() );
        TRect        srcRect( bmpSize                         );

        TReal bmpRatio( TReal( bmpSize.iWidth ) / TReal( bmpSize.iHeight ) );
        TReal trgRatio( TReal( trgSize.iWidth ) / TReal( trgSize.iHeight ) );

        PRINT2( _L("Camera <> CCamThumbnailGridSizer - source size(%d,%d)"), 
                bmpSize.iWidth, bmpSize.iHeight );

        PRINT2( _L("Camera <> CCamThumbnailGridSizer - target size(%d,%d)"), 
                trgSize.iWidth, trgSize.iHeight );

        if( bmpRatio > trgRatio )
          {
          // Thumbnail aspect ratio is wider than the target size.
          // Scale initially to a size wider than target and then crop.
          trgSize.iWidth = Max( trgSize.iHeight * bmpRatio, 1 );
          }
        if( bmpRatio < trgRatio )
          {
          // Thumbnail aspect ratio is wider than the target size.
          // Scale initially to a size higher than target and then crop.
          trgSize.iHeight = Max( trgSize.iWidth * bmpRatio, 1 );
          }

        PRINT2( _L("Camera <> CCamThumbnailGridSizer - intermediate size(%d,%d)"), 
                trgSize.iWidth, trgSize.iHeight );


        // Create a temporary bitmap
        iTmpBitmap = new ( ELeave ) CFbsBitmap();
        iTmpBitmap->Create( trgSize, aTask->Bitmap()->DisplayMode() );        
       

        // Start the scaling operation
        iScaler->Scale( &iStatus, 
                        const_cast<CFbsBitmap&>(*aTask->Bitmap()), 
                        *iTmpBitmap, 
                        ETrue );
        SetActive();
        }
    PRINT( _L("Camera <= CCamThumbnailGridSizer::StartScaleL task") );
    }    

// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::DoCancel
// -----------------------------------------------------------------------------
//
void CCamThumbnailGridSizer::DoCancel()
    {    
    PRINT( _L("Camera => CCamThumbnailGridSizer::DoCancel") );
    iScaler->Cancel();    
        
    // Delete any intermediate temporary bitmap
    if ( iTmpBitmap )
        {
        delete iTmpBitmap;
        iTmpBitmap = NULL;
        }
    
    // Cancel all pending scaling operations
    iScaleTaskArray.ResetAndDestroy();
    PRINT( _L("Camera <= CCamThumbnailGridSizer::DoCancel") );
    }

// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::RunL
// -----------------------------------------------------------------------------
//
void CCamThumbnailGridSizer::RunL()
    {
    PRINT2( _L("Camera => CCamThumbnailGridSizer::RunL .. observer 0x%x err %d"), iObserver, iStatus.Int() );
    // If there is an observer, notify them of the scaling completion
    if ( iObserver )
        {
        PRINT1( _L("Camera <> CCamThumbnailGridSizer::RunL .. scale array count %d"), iScaleTaskArray.Count() );
        // Note; passing iTmpBitmap in this call is passing ownership of the 
        // bitmap to the observer.  This also places the restriction that there
        // can only be one observer to an implementation of the GridSizer, which
        // is fine for our requirements
        iObserver->BitmapScaleCompleteL( iStatus.Int(), iTmpBitmap, iScaleTaskArray[0]->Info() );
        iTmpBitmap = NULL;
        }
        
    CScaleTask* task = NULL;
    if ( iScaleTaskArray.Count() > 0 )
        {
        // Remove from the array and delete this (completed) task
        task = iScaleTaskArray[0];        
        iScaleTaskArray.Remove(0);        
        delete task;
        task = NULL;        
        }
        
    // If completed successfully...     
    // ... and there are more tasks on the queue...
    // ... and the task is valid, then do next task    
    if ( iStatus.Int() == KErrNone && 
         iScaleTaskArray.Count() > 0 )
        {
        task = iScaleTaskArray[0];        
        if ( task )
            {
            StartScaleL( task );    
            }        
        }
    PRINT ( _L("Camera <= CCamThumbnailGridSizer::RunL") );
    }


//  End of File  
