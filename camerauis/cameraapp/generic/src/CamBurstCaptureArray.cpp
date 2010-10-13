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
* Description:  A class that represents a single item in a burst capture
*
* Copyright © 2007-2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/


// INCLUDE FILES
#include <fbs.h>
#include <eikenv.h>
#include <barsread.h>
#include <AknUtils.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamBurstCaptureArray.h"
#include "CamImageSaveActive.h"
#include "CamPanic.h"
#include "camlogging.h"


// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCamBurstCaptureItem::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamBurstCaptureArray::CCamBurstCaptureItem* CCamBurstCaptureArray::CCamBurstCaptureItem::NewLC()
    {
    CCamBurstCaptureItem* self = new( ELeave ) CCamBurstCaptureItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem destructor
// 
// ---------------------------------------------------------------------------
//
CCamBurstCaptureArray::CCamBurstCaptureItem::~CCamBurstCaptureItem()
  {
  PRINT( _L("Camera => ~CCamBurstCaptureItem") );
  delete iFileName;
  delete iImageName;
  delete iSnapshot;
  PRINT( _L("Camera <= ~CCamBurstCaptureItem") );
  }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::SetNameL
// Sets the item's full file path and image name
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::CCamBurstCaptureItem::SetNameL( const TDesC& aFullFileName, 
                                    const TDesC& aImageName )
    {
    delete iFileName;
    iFileName = NULL;
    delete iImageName;
    iImageName = NULL;
    iFileName = aFullFileName.AllocL();
    iImageName = aImageName.AllocL();
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::SetSnapshotL
// Stores the snapshot bitmap in the item
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::CCamBurstCaptureItem::SetSnapshotL( const CFbsBitmap& aSnapshot ) 
    {
    delete iSnapshot;
    iSnapshot = NULL;

    // Create bitmap
    iSnapshot = new( ELeave ) CFbsBitmap();
    TInt createError = iSnapshot->Duplicate( aSnapshot.Handle() );

    if ( createError )
        {
        delete iSnapshot;
        iSnapshot = NULL;
        }

    User::LeaveIfError( createError );
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::SetDeleted
// Sets the item's deletion state
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::CCamBurstCaptureItem::SetDeleted( TBool aDeleted ) 
    {
    iIsDeleted = aDeleted;
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::FileName
// Return a pointer to the full file path and name
// ---------------------------------------------------------------------------
//
const TDesC& CCamBurstCaptureArray::CCamBurstCaptureItem::FileName() const
    {
    if( !iFileName )
      {
      return KNullDesC;
      }
    return *iFileName;
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::ImageName
// Return a pointer to the image name
// ---------------------------------------------------------------------------
//
const TDesC& CCamBurstCaptureArray::CCamBurstCaptureItem::ImageName() const
    {
    if( !iImageName )
        {
        return KNullDesC;
        }
    return *iImageName;
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItem::Bitmap
// Return a pointer to the bitmap
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CCamBurstCaptureArray::CCamBurstCaptureItem::Snapshot() const
    {
    return iSnapshot;
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::IsDeleted
// Whether or not the item has been marked for deletion
// ---------------------------------------------------------------------------
//
TBool CCamBurstCaptureArray::CCamBurstCaptureItem::IsDeleted() const
    {
    return iIsDeleted;
    }
      
// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::CCamBurstCaptureItem::ReplaceSnapshot
// Replaces the bitmap.
// ---------------------------------------------------------------------------
// 
TBool CCamBurstCaptureArray::CCamBurstCaptureItem::ReplaceSnapshot( 
    const CFbsBitmap* aBitmap )
    {
    if( !iSnapshot )
        {
        TRAPD( err, iSnapshot = new( ELeave ) CFbsBitmap() );
        if( err )
            {
            return EFalse;
            }
        }
    if ( iSnapshot->Duplicate( aBitmap->Handle() ) )
        {
        return EFalse;
        }
    return ETrue;
    } 

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::CCamBurstCaptureItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamBurstCaptureArray::CCamBurstCaptureItem::CCamBurstCaptureItem()
    {
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureItem::ConstructL
// Second phase construction
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::CCamBurstCaptureItem::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CCamBurstCaptureArray::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamBurstCaptureArray* CCamBurstCaptureArray::NewL( CCamImageSaveActive& aImageSaveActive )
    {
    CCamBurstCaptureArray* self = new( ELeave ) CCamBurstCaptureArray( aImageSaveActive );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray destructor
// 
// ---------------------------------------------------------------------------
//
CCamBurstCaptureArray::~CCamBurstCaptureArray()
    {
    iBurstItems.ResetAndDestroy();
    iBurstItems.Close();
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::SetNameL
// Sets an item's full file path and image name
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::SetNameL( const TDesC& aFullFileName, 
    const TDesC& aImageName, TInt aIndex )
    {
    PRINT1( _L("Camera => CCamBurstCaptureArray::SetNameL, name:[%S]"), &aFullFileName );  
    iBurstItems[aIndex]->SetNameL( aFullFileName, aImageName );
    PRINT( _L("Camera <= CCamBurstCaptureArray::SetNameL") );
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::AlreadySavedFile
// Returns whether a particular file has already been saved.
// ---------------------------------------------------------------------------
//
TBool CCamBurstCaptureArray::AlreadySavedFile( const TDesC& aFilename )
    {
    return iImageSaveActive.AlreadySavedFile( aFilename );
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::CurrentlySavingFile
// Returns whether a particular file is in the process of being saved.
// ---------------------------------------------------------------------------
//
TBool CCamBurstCaptureArray::CurrentlySavingFile( const TDesC& aFilename )
    {
    return iImageSaveActive.CurrentlySavingFile( aFilename );
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::Count
// Return the number of items in the array
// ---------------------------------------------------------------------------
//
TInt CCamBurstCaptureArray::Count() const
    {
    return iBurstItems.Count();
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::Reset
// Delete unwanted items from the array
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::Reset( TInt aRequiredCount )
    {
    PRINT1( _L("Camera => CCamBurstCaptureArray::Reset(%d)"), aRequiredCount );
    TInt count = iBurstItems.Count();
    // if the current count is less than the required count
    if ( aRequiredCount >= count )
        {
        return;
        }
    // if the required count is 0 or less, empty the array
    if ( aRequiredCount <= 0 )
        {
        iBurstItems.ResetAndDestroy();
        iNextSetName = 0;
        iNextGetName = 0;
        iNextSnapshot = 0;
        iImagesRemaining = 0;
        }
    // otherwise delete the unwanted items
    else
        {
        TInt index;
        // repeat for every extra item
        for ( index = count - 1; index >= aRequiredCount; index-- )
            {
            CCamBurstCaptureItem* item = iBurstItems[index];
            if ( !item->IsDeleted() )
                {
                iImagesRemaining--;
                }
            iBurstItems.Remove( index );
            delete item;
            }
        // reset the accessing indexes if necessary
        count = iBurstItems.Count();
        iNextSetName = Min( iNextSetName, count );
        iNextSnapshot = Min( iNextSnapshot, count );
        }
    User::Heap().Compress();    
    PRINT( _L("Camera <= CCamBurstCaptureArray::Reset") );
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::SetNextNameL
// Set the item's full file path and image name
// ---------------------------------------------------------------------------
// 
void CCamBurstCaptureArray::SetNextNameL( const TDesC& aFullFileName, const TDesC& aImageName )
    {
    PRINT1( _L("Camera => CCamBurstCaptureArray::SetNextNameL, name:[%S]"), &aFullFileName );
    CheckArraySizeL( iNextSetName );
    iBurstItems[iNextSetName]->SetNameL( aFullFileName, aImageName );
    iNextSetName++;
    PRINT( _L("Camera <= CCamBurstCaptureArray::SetNextNameL") );
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::SetNextSnapshotL
// Stores the snapshot bitmap in the item
// ---------------------------------------------------------------------------
//  
void CCamBurstCaptureArray::SetNextSnapshotL( const CFbsBitmap& aSnapshot )
    {
    PRINT( _L("Camera => CCamBurstCaptureArray::SetNextSnapshotL") );
    CheckArraySizeL( iNextSnapshot );
    TInt nextIndex = iNextSnapshot;
    iNextSnapshot++;
    iBurstItems[nextIndex]->SetSnapshotL( aSnapshot );
    PRINT( _L("Camera <= CCamBurstCaptureArray::SetNextSnapshotL") );
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::SetDeleted
// Sets the items deletion state
// ---------------------------------------------------------------------------
//   
TInt CCamBurstCaptureArray::SetDeleted( TInt aItemIndex, TBool aDeleted )
    {
    PRINT2( _L("Camera => CCamBurstCaptureArray::SetDeleted( index:%d, deleted:%d )"), aItemIndex, aDeleted );
    TInt ret = KErrNone;

    __ASSERT_DEBUG( aItemIndex < Count() && aItemIndex >= 0,
        CamPanic( ECamPanicBadIndex ) );

    if ( aDeleted == iBurstItems[aItemIndex]->IsDeleted() )
        {
        PRINT( _L("Camera <= CCamBurstCaptureArray::SetDeleted A") );
        return ret;
        }
   
    if ( aDeleted )
        {
        //iImagesRemaining--;
        TInt saveRequested = EFalse;
        // if the image save has already been requested
        if ( aItemIndex < iNextGetName )
            {
            saveRequested = ETrue;
            }
        iImageSaveActive.CancelThumbnail( aItemIndex );
        ret = iImageSaveActive.DeleteFile( iBurstItems[aItemIndex]->FileName(), saveRequested );

        PRINT1( _L("Camera <> CCamBurstCaptureArray::SetDeleted iImageSaveActive.DeleteFile() returned %d" ), ret );
        if ( ret == KErrInUse ) 
          {
          User::After( 700000 );
          ret = iImageSaveActive.DeleteFile( iBurstItems[aItemIndex]->FileName(), saveRequested );
          PRINT1( _L("Camera <> CCamBurstCaptureArray::SetDeleted iImageSaveActive.DeleteFile() Retry Delete returned %d" ), ret );
          }
        // file might have already been deleted and that's ok so ignore -NotFound errors
        if( ret == KErrNone || ret == KErrNotFound || ret == KErrPathNotFound )
            {
            iImagesRemaining--;
            }
        }
    else
        {
        iImagesRemaining++;
        }
    if( ret == KErrNone || ret == KErrNotFound || ret == KErrPathNotFound )
        {
        iBurstItems[aItemIndex]->SetDeleted( aDeleted );
        }

    PRINT( _L("Camera <= CCamBurstCaptureArray::SetDeleted B") );
    return ret;
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::IsNextImageDeleted
// Increments iNextGetName and returns ETrue if the next image is marked for 
// deletion, otherwise returns EFalse 
// ---------------------------------------------------------------------------
// 
TBool CCamBurstCaptureArray::IsNextImageDeleted()
  {
  PRINT( _L("Camera => CCamBurstCaptureArray::IsNextImageDeleted") );  
  TBool deleted( EFalse );

  if ( iNextGetName >= Count() )
    {
    PRINT2( _L("Camera <> index(%d) out of range(%d), return deleted"), iNextGetName, Count() );
    deleted = ETrue;
    }
  else if ( iBurstItems[iNextGetName]->IsDeleted() )
    {
    PRINT( _L("Camera <> next item marked deleted") );
    iNextGetName++;
    deleted = ETrue;
    }
  else
    {
    deleted = EFalse;
    }

  PRINT1( _L("Camera <= CCamBurstCaptureArray::IsNextImageDeleted, return: %d"), deleted );  
  return deleted;
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::NextFileName
// Returns a pointer to the full file path and name for the next image
// ---------------------------------------------------------------------------
//    
const TDesC& CCamBurstCaptureArray::NextFileName()
    {
    PRINT( _L("Camera => CCamBurstCaptureArray::NextFileName") );  
    if ( iNextGetName >= Count() )
        {
        PRINT( _L("Camera <> Returning KNullDesC") );  
        return KNullDesC;
        }

    TInt itemIndex = iNextGetName;
    iNextGetName++;

    PRINT( _L("Camera <= CCamBurstCaptureArray::NextFileName") );  
    return iBurstItems[itemIndex]->FileName();
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::NextFileIndex
// Returns the index of the next image
// ---------------------------------------------------------------------------
// 
TInt CCamBurstCaptureArray::NextFileIndex() const
    {
    return iNextGetName;
    }


// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::FileName
// Returns a pointer to the full file path and name
// ---------------------------------------------------------------------------
//   
const TDesC& CCamBurstCaptureArray::FileName( TInt aItemIndex ) const
    {
    if ( aItemIndex >= Count() || aItemIndex < 0 )
        {
        return KNullDesC;
        }
    return iBurstItems[aItemIndex]->FileName();
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::ImageName
// Returns a pointer to the image name 
// ---------------------------------------------------------------------------
//  
const TDesC& CCamBurstCaptureArray::ImageName( TInt aItemIndex ) const
    {
    if ( aItemIndex >= Count() || aItemIndex < 0 )
        {
        return KNullDesC;
        }
    return iBurstItems[aItemIndex]->ImageName();
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::Snapshot
// Returns a pointer to the snapshot image
// ---------------------------------------------------------------------------
// 
const CFbsBitmap* CCamBurstCaptureArray::Snapshot( TInt aItemIndex ) const
    {
    if ( aItemIndex >= Count() || aItemIndex < 0 )
        {
        return NULL;
        }
    return iBurstItems[aItemIndex]->Snapshot();
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::IsDeleted
// Returns the item's deletion state
// ---------------------------------------------------------------------------
// 
TBool CCamBurstCaptureArray::IsDeleted( TInt aItemIndex ) const
    {
    if ( aItemIndex >= Count() )
        {
        return ETrue;
        }
    return iBurstItems[aItemIndex]->IsDeleted();
    }
  
// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::ReplaceSnapshot
// Replaces the bitmap in the given index.
// ---------------------------------------------------------------------------
// 
TBool CCamBurstCaptureArray::ReplaceSnapshot( 
    const CFbsBitmap* aBitmap, 
    TInt aItemIndex )
    {
    if ( ( iBurstItems.Count() > aItemIndex ) &&
        ( aItemIndex >= 0 ) )
        {
        return iBurstItems[aItemIndex]->ReplaceSnapshot( aBitmap );
        }
    return EFalse;
    } 

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::CCamBurstCaptureArray
// C++ constructor
// ---------------------------------------------------------------------------
//   
CCamBurstCaptureArray::CCamBurstCaptureArray( CCamImageSaveActive& aImageSaveActive )
: iImageSaveActive( aImageSaveActive )
    {
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::ConstructL
// Second phase construction
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CCamBurstCaptureArray::CheckArraySizeL
// Add extra items to the array if required
// ---------------------------------------------------------------------------
//
void CCamBurstCaptureArray::CheckArraySizeL( TInt aRequiredIndex )
    {
    TInt count = Count();
    TInt i;
    for ( i = count; i <= aRequiredIndex; i++ )
        {
        CCamBurstCaptureItem* newItem = CCamBurstCaptureItem::NewLC();
        iBurstItems.AppendL( newItem );
        CleanupStack::Pop( newItem );
        iImagesRemaining++;
        }
    }

//  End of File 
