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
* Description:  Class to model the contents of the burst post-capture view*
*/



// INCLUDE FILES
#include <fbs.h>     
#include <barsread.h>
#include <eikenv.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <StringLoader.h>
#include <aknnotewrappers.h>

 
#include "CamBurstThumbnailGridModel.h"
#include "CamBurstThumbnailGrid.h"
#include "CamBurstCaptureArray.h"
#include "camlogging.h"
#include "CamPanic.h"

// Uncommenting the define below will allow wrapping of the grid from the first
// element of the grid to the very last, and vice-versa.
#define ALLOW_GRID_WRAPPING


// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

CCamBurstThumbnailGridModel::CCamBurstThumbnailGridModel() 
    {
    }


// -----------------------------------------------------------------------------
// CCamBurstCaptureItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamBurstThumbnailGridModel* CCamBurstThumbnailGridModel::NewL()
    {
    CCamBurstThumbnailGridModel* self = new( ELeave ) CCamBurstThumbnailGridModel();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::ConstructL()
    {
    iThumbSize.iWidth = 0;
    iThumbSize.iHeight = 0;

    iGridSizer = CCamThumbnailGridSizer::NewL( this, iThumbSize );        
    iTopLeftIndex = 0;
    }


// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::GridItem
// Returns pointer to the requested item, or NULL if index is invalid
// -----------------------------------------------------------------------------
//
CThumbnailGridItem* CCamBurstThumbnailGridModel::GridItem( TInt aIndex )
    {
    if ( aIndex >= iValidThumbGrid.Count() )
        {
        return NULL;
        }
    else 
        {
        return iValidThumbGrid[ aIndex ];
        }
    }

// Note: Returns ONLY non-deleted cells
// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::Snapshot
// Returns pointer to the requested item, or NULL if index is invalid
// -----------------------------------------------------------------------------
//
const CFbsBitmap* CCamBurstThumbnailGridModel::Snapshot( TInt aIndex )
    {
    if ( aIndex >= iValidThumbGrid.Count() )
        {
        return NULL;
        }
    else 
        {        
        TInt ind = ConvertFromValidToGlobalIndex( aIndex );
        if ( ind != KErrNotFound )
            {
            return iBurstArray->Snapshot( ind );
            }
        else 
            {
            return NULL;
            }
        }
    }

// Destructor
CCamBurstThumbnailGridModel::~CCamBurstThumbnailGridModel()
  {
  PRINT( _L("Camera => ~CCamBurstThumbnailGridModel") );

  if ( iGridSizer )
      {
      iGridSizer->Cancel();
      }
  delete iGridSizer;
  
  // Delete the array and the objects pointed to
  iAllThumbGrid.ResetAndDestroy();

  // Note: As this points to the same thing as the array above, just 
  // delete the array, not the objects
  iValidThumbGrid.Reset();

  // Do NOT destroy the objects pointed to, just the list.
  iThumbObserver.Reset();     
  iHighlightObserver.Reset();
  iDeleteObserver.Reset();
  PRINT( _L("Camera <= ~CCamBurstThumbnailGridModel") );
  }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::AddCellL
// Adds the cell to the internal structures.  Takes ownership of the cell.
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::AddCellL( CThumbnailGridItem* aCell )
    {
    // Add to the master list of grid items
    User::LeaveIfError( iAllThumbGrid.Append( aCell ) );

    // Add to the list of VALID (non deleted) grid items
    User::LeaveIfError( iValidThumbGrid.Append( aCell ) );
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::StartThumbnailL
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::StartThumbnailL( TInt aIndex )
    {        
    // Burst array may be released by the app when swicthing
    // views so do a check that it is still valid
    if ( iBurstArray )
        {
        const CFbsBitmap* source = iBurstArray->Snapshot( aIndex );
        if ( source )
            {
            iGridSizer->StartScaleL( *source, iThumbSize, aIndex );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::CancelThumbnails
// Cancels any outstanding thumbnail generation operation
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::CancelThumbnails()
    {
    if ( iGridSizer->IsActive() )
        {
        iGridSizer->Cancel();
        }
    }    

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::ImageFilesDeletedL
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::ImageFilesDeleted()
    {
    
    TBool highlightDeleted = EFalse;
    
    TInt highlightIndex = ConvertFromValidToGlobalIndex( iCurrHighlight );
    
    for ( TInt i = 0; i < iBurstArray->Count(); i++ )
        {
        if ( iBurstArray->IsDeleted( i ) )
            {
            if ( highlightIndex == i )
                {
                highlightDeleted = ETrue;
                }
            
            TRAPD( error, DeleteItemL( i ) );
            
            // DeleteItemL() leaves only when file can't be deleted and that should never
            // happen here as the files are already deleted
            __ASSERT_DEBUG( error == KErrNone, CamPanic( ECamPanicUi ) );
            error++; // remove compile warnings
            }
        }
    
    DoPostDeleteGridCheck( highlightDeleted );
    }

// -----------------------------------------------------------------------------
// CCamThumbnailGridSizer::SetThumbnailSize
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::SetThumbnailSize( const TSize aThumbSize )
    {
    
    if ( aThumbSize != iThumbSize )
        {
        iThumbSize = aThumbSize;
		iGridSizer->SetSize( iThumbSize );
        // delete all thumbs because size has changed and they need to be regenerated
        TInt imagesRemaining = iBurstArray->ImagesRemaining();
        for ( TInt i = 0; i < imagesRemaining; i++ )
            {
            // delete display thumbnail
            delete iValidThumbGrid[i]->iDisplayThumb;
            iValidThumbGrid[i]->iDisplayThumb = NULL;   
            }

        // recalculate needed thumbs
        RecalculateThumbs();

        }
    }



// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::BitmapScaleCompleteL
// Callback called when a bitmap scaling operation has completed.
// We take ownership of the 'aBitmap' object
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::BitmapScaleCompleteL( TInt aErr, CFbsBitmap* aBitmap, TInt32 aInfo )
    {
    if ( aErr == KErrNone )
        {       
        // Update internal structure
        CThumbnailGridItem* item = iAllThumbGrid[aInfo];
        item->iDisplayThumb = aBitmap;

        // Inform observer(s) to ensure the screen is updated     
        NotifyModelObserver( EModelEventThumb );
        }    
    }      
    
// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::RecalculateThumbs
// Called to check which thumbnails need creating, and start the procedure
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::RecalculateThumbs()
    {        
    // If in the grid below, items 3-8 were visible in the screen grid, 
    // items 0-2 and 9-11 would also have thumbnails generated to attempt
    // to reduce visible artifacts from calculating on the fly.
    //    [ 0 ][ 1 ][ 2 ]   <= Row above (not visible)
    //    [ 3 ][ 4 ][ 5 ]   <= First visible row
    //    [ 6 ][ 7 ][ 8 ]   <= Second visible row
    //    [ 9 ][ 10][ 11]   <= Row below (not visible)
    //
    
    // If we get to recalculate thumbnails, we need to cancel any outstanding 
    // rescale operation
    CancelThumbnails();
    
    // Work out the top left index. This is the top-left cell of the VIEWABLE
    // grid, less one row (which serves as a thumbnail cache)        
    TInt topLeftIndex = iTopLeftIndex - KGridWidth;
    if ( topLeftIndex < 0 )
        {
        topLeftIndex = 0;
        }

    // Work out the bottom right index. This is the bottom-right cell of the 
    // VIEWABLE grid, plus one row (which serves as a thumbnail cache)
    TInt botRightIndex = iTopLeftIndex + ( KGridWidth * iGridHeight ) - 1 + KGridWidth;
    if ( botRightIndex > iValidThumbGrid.Count() - 1 )
        {
        botRightIndex = iValidThumbGrid.Count() - 1;
        }
    
    // Delete the thumbnails for cells that are not immediately visible, and are
    // not in the next or last row.
    TInt i;
    TInt imagesRemaining = iBurstArray->ImagesRemaining();
    for ( i = 0; i < imagesRemaining; i++ )
        {
        if ( ( i < topLeftIndex || i > botRightIndex ) &&
            iValidThumbGrid[i]->iDisplayThumb ) 
            {
            // delete display thumbnail
            delete iValidThumbGrid[i]->iDisplayThumb;
            iValidThumbGrid[i]->iDisplayThumb = NULL;   
            }
        }
    
    // Queue up the thumbnails that need to be resized
    TRAPD( ignore, QueueThumbnailsL() );
    if ( ignore )
        { 
        // Do nothing ( removes build warning )
        }                   
    }


// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::QueueThumbnailsL
// Queues the thumbnails to be resized as needed
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::QueueThumbnailsL()
    {
    //    [ 0 ][ 1 ][ 2 ]   <= Row above (not visible)
    //    [ 3 ][ 4 ][ 5 ]   <= First visible row
    //    [ 6 ][ 7 ][ 8 ]   <= Second visible row
    //    [ 9 ][ 10][ 11]   <= Row below (not visible)
    //    
    TInt start = iTopLeftIndex;
    TInt index = KErrNotFound; 
    TInt max = iTopLeftIndex + KGridSize;
    if ( max > iValidThumbGrid.Count() )
        {
        max = iValidThumbGrid.Count();
        }
    
    // Top priority is the set of thumbs visible AT THIS MOMENT
    TInt i;
    for ( i = start; i < max ; i++ )
        {
        if ( !iValidThumbGrid[i]->iDisplayThumb )
            {
            // If we get here, then 'i' is the index to the VALID set of items.
            // Need to convert this to the index in the GLOBAL set of items
            index = ConvertFromValidToGlobalIndex( i );
            StartThumbnailL( index );
            }
        }

    // SECOND top priority is the set of thumbs BELOW the visible set
    if ( iValidThumbGrid.Count() >= iTopLeftIndex + KGridSize ) 
        {
        start = iTopLeftIndex+KGridSize;
        max = i + KGridWidth;
        if ( max > iValidThumbGrid.Count() )
            {
            max = iValidThumbGrid.Count();
            }

        // If we have got here, we can cache the row *below* the 2nd visible row
        for ( i = start; i < max; i++ )
            {
            if ( !iValidThumbGrid[i]->iDisplayThumb )
                {
                // If we get here, then 'i' is the index to the VALID set of items.
                // Need to convert this to the index in the GLOBAL set of items
                index = ConvertFromValidToGlobalIndex( i );
                StartThumbnailL( index );
                }
            }
        }

    // THIRD priority is the set of thumbs ABOVE the visible set
    if ( iTopLeftIndex >= KGridWidth ) // ONLY if there IS a previous row
        {
        start = iTopLeftIndex - KGridWidth;
        max = iTopLeftIndex;
        // If we have got here, we can cache the row *above* this one.
        for ( i = start; i < max; i++ )
            {
            if ( !iValidThumbGrid[i]->iDisplayThumb )
                {
                // If we get here, then 'i' is the index to the VALID set of items.
                // Need to convert this to the index in the GLOBAL set of items
                index = ConvertFromValidToGlobalIndex( i );
                StartThumbnailL( index );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::ConvertFromValidToGlobalIndex
// Converts from a valid-index (non-deleted only) to global (del & non-del) index
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::ConvertFromValidToGlobalIndex( TInt aValidIndex )   
    {       
    if ( aValidIndex < iValidThumbGrid.Count() )
        {
        CThumbnailGridItem* item = iValidThumbGrid[aValidIndex];
        return iAllThumbGrid.Find( item );    
        }
    else
        {
        return KErrNotFound;
        }
    }      

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::SetBurstArrayL
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::SetBurstArrayL( CCamBurstCaptureArray* aArray )
    {
    PRINT( _L("Camera => CCamBurstThumbnailGridModel::SetBurstArrayL") );

    // If we've not been initialised yet, and have no data, then use the array
    // just passed in.  
    if ( iAllThumbGrid.Count() == 0 )
        {
        iBurstArray = aArray;
        
        // Build up model to match burst array
        if ( iBurstArray )
            {
            TInt i;
            TInt count = iBurstArray->Count();
            PRINT1( _L("Camera <> got %d thumbnails"), count );
            
            // Create a set of items to match the number in the burst array
            for ( i = 0; i < count; i++ )
                {
                CThumbnailGridItem* item = new ( ELeave ) CThumbnailGridItem();

                CleanupStack::PushL( item );
                item->iDisplayThumb = NULL;
                item->iMarked = EFalse;
                AddCellL( item );
                CleanupStack::Pop( item );

                if ( !iBurstArray->Snapshot( i ) )
                    {
                    DeleteItemL( i );
                    }
                }

            // Setup the base grid height based on the number of items
            if ( NoOfValidCells() <= KSmallGridCellCount )
                {
                iGridHeight = KSmallGridHeight;
                }
            else 
                {
                iGridHeight = KLargeGridHeight;
                }
            
            // Start the generation of thumbnails
            RecalculateThumbs();
            }
        }
    PRINT( _L("Camera <= CCamBurstThumbnailGridModel::SetBurstArrayL") );
    }



// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::AddModelObserverL
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::AddModelObserverL( MThumbModelObserver* aObserver, TInt aMask )
    {
    if ( aMask & EModelEventHighlight )
        {
        User::LeaveIfError( iHighlightObserver.Append( aObserver ) );
        }

    if ( aMask & EModelEventDeleted )
        {
        User::LeaveIfError( iDeleteObserver.Append( aObserver ) );
        }

    if ( aMask & EModelEventThumb )
        {
        User::LeaveIfError( iThumbObserver.Append( aObserver ) );
        }   
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::RemoveModelObserver
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::RemoveModelObserver( MThumbModelObserver* aObserver, TInt aMask )
    {
    TInt pos;
    if ( aMask & EModelEventHighlight )
        {
        pos = iHighlightObserver.Find( aObserver );
        if ( pos != KErrNotFound )
            {
            iHighlightObserver.Remove( pos );
            }
        }
    if ( aMask & EModelEventDeleted )
        {
        pos = iDeleteObserver.Find( aObserver );
        if ( pos != KErrNotFound )
            {
            iDeleteObserver.Remove( pos );
            }
        }
    if ( aMask & EModelEventThumb )
        {
        pos = iThumbObserver.Find( aObserver );
        if ( pos != KErrNotFound )
            {
            iThumbObserver.Remove( pos );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::NotifyModelObserver
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::NotifyModelObserver( TModelEvent aEvent, TInt /*aParam*/ )
    {
    TInt i;
    TInt count; 

    switch ( aEvent )        
        {        
        case EModelEventHighlight:
            {
            count = iHighlightObserver.Count();
            for ( i = 0; i < count; i++ )  
                {
                iHighlightObserver[i]->HighlightChanged();
                }
            break;
            }
            
        case EModelEventDeleted:
            {
            count = iDeleteObserver.Count();
            for ( i = 0; i < count; i++ )  
                {
                iDeleteObserver[i]->ImagesDeleted();
                }    
            break;
            }

        case EModelEventThumb:
            {
            count = iThumbObserver.Count();
            for ( i = 0; i < count; i++ )  
                {
                iThumbObserver[i]->ThumbnailGenerated();
                }
            break;
            }
        default:
            break;  
        }
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::NoOfValidCells
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::NoOfValidCells()
    {
    //return iBurstArray->ImagesRemaining();
    return iValidThumbGrid.Count();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::MoveHighlight
// Tries to move the selection.  If no change is made, returns EFalse.
// If change is made, returns ETrue (to inform of a redraw)
// -----------------------------------------------------------------------------
//
TBool CCamBurstThumbnailGridModel::MoveHighlight( TMoveSelect aDir )
    {
    TInt oldTopLeft = iTopLeftIndex;
    TInt oldHighlight = iCurrHighlight;

    // cellX and cellY store the cell position in the X and Y axis as seen on 
    // the screen
    TInt cellX = ( iCurrHighlight - iTopLeftIndex ) % KGridWidth; // 0 - 2
    TInt cellY = ( iCurrHighlight - iTopLeftIndex ) / KGridWidth; // 0 - 1

    switch ( aDir )
        {
        case EMoveSelectLeft:
            {
            if ( iCurrHighlight > 0 )
                {
                iCurrHighlight --;

                if ( cellX == 0 && cellY == 0 ) // At top left entry
                    {
                    iTopLeftIndex -= KGridWidth;
                    }
                }
#ifdef ALLOW_GRID_WRAPPING
            else 
                {
                TInt cells = iValidThumbGrid.Count();
                TInt bottomX = ( cells ) % KGridWidth;
                TInt bottomY = ( cells ) / KGridWidth;
                // If over the allowed number of rows, and bottom row is full
                if ( bottomY >= iGridHeight && bottomX == 0 )  
                    {
                    iTopLeftIndex = ( bottomY - iGridHeight ) * KGridWidth;    // 
                    }
                else if ( bottomY >= ( iGridHeight - 1 ) ) // Over 1 row, 
                    {
                    iTopLeftIndex = ( bottomY - ( iGridHeight - 1 ) ) * KGridWidth;    
                    }
                else // Only one (incomplete) row
                    { 
                    iTopLeftIndex = ( bottomY  ) * KGridWidth;    
                    }
                
                iCurrHighlight = cells - 1; // Last entry
                }
#endif // ALLOW_GRID_WRAPPING
            break;
            }

        case EMoveSelectRight:
            {
            if ( iCurrHighlight < ( iBurstArray->ImagesRemaining() - 1 ) )
                {
                iCurrHighlight ++;

                if ( cellX == ( KGridWidth - 1 ) && cellY == ( iGridHeight - 1 ) ) 
                    {
                    iTopLeftIndex += KGridWidth;
                    }
                }
#ifdef ALLOW_GRID_WRAPPING
            else 
                {
                iCurrHighlight = 0; // Top left entry
                iTopLeftIndex = 0;
                }
#endif // ALLOW_GRID_WRAPPING

            break;
            }

        case EMoveSelectUp:
            {
            if ( iCurrHighlight >= KGridWidth )
                {
                iCurrHighlight -= KGridWidth;

                // if in TOP row, and we CAN move up, shift viewpoint up
                if ( cellY == 0 ) 
                    {
                    iTopLeftIndex -= KGridWidth;
                    }
                }
#ifdef ALLOW_GRID_WRAPPING
            else // Then we need to wrap to the bottom of the previous grid
                {
                TInt cells = iValidThumbGrid.Count();

                // startX holds the "x" value of initial selection
                TInt startX = ( iCurrHighlight ) % KGridWidth; 
                TInt bottomY = ( cells ) / KGridWidth; 

                // If not in left-most grid, we'll be moving left and to bottom
                if ( startX > 0 )
                    {
                    startX --;                    
                    }
                else // Else in left most grid, so "jump" to right most.
                    {
                    if ( cells < KGridWidth )
                        {
                        startX = cells - 1;
                        }
                    else
                        {
                        startX = KGridWidth - 1;
                        }                    
                    }

                // If all rows are full (so modulus is 0) bottomY division will
                // be one too great, so compensate
                if ( cells % KGridWidth == 0 )
                    { 
                    bottomY -- ;
                    }
                iCurrHighlight = ( bottomY * KGridWidth ) + startX;
                
                // Check this item is filled in, if not, move "up" a row
                if ( iCurrHighlight >= cells )
                    { 
                    // If more than one rows worth of items, can move to second to last row
                    if ( cells > KGridWidth )
                        {
                        bottomY --;
                        }
                    else // If only one row (or less)
                        {
                        startX = cells % KGridWidth;
                        }                    

                    // Will need to recalculate curr highlight following changes
                    iCurrHighlight = ( bottomY * KGridWidth ) + startX;
                    }
                UpdateViewableGrid( EFalse );
                }
#endif // ALLOW_GRID_WRAPPING
            break;
            }

        case EMoveSelectDown:
            {
            if ( ( iCurrHighlight + KGridWidth ) <= ( iBurstArray->ImagesRemaining() - 1 ) )
                {
                iCurrHighlight += KGridWidth;

                if ( cellY == ( iGridHeight-1 ) ) // if in BOTTOM row, and we CAN move down, shift viewpoint down
                    {
                    iTopLeftIndex += KGridWidth;
                    }
                }
#ifdef ALLOW_GRID_WRAPPING
            else // Then we need to wrap to the top of the next grid
                {
                TInt startX = ( iCurrHighlight ) % KGridWidth; 
                TInt cells = iValidThumbGrid.Count();
                TInt rightMostGrid;

                if ( cells < KGridWidth )
                    {
                    rightMostGrid = cells;
                    }
                else
                    {
                    rightMostGrid = KGridWidth;
                    }
                
                // If not in left-most grid, we'll be moving left and to bottom
                if ( startX < ( rightMostGrid - 1 )  )
                    {                    
                    // Check we don't have less-than-a-row of cells left
                    if ( startX < ( cells - 1 ) )
                        {
                        startX ++;                    
                        }
                    else // If that is the case, move to the last one.
                        {
                        startX = cells - 1;
                        }
                    
                    }
                else // Else in left most grid, so "jump" to right most.
                    {
                    startX = 0;
                    }                

                // Calculate new highlight poisition.
                iCurrHighlight = startX;

                UpdateViewableGrid( ETrue );
                }
#endif // ALLOW_GRID_WRAPPING
            break;
            }
        }
        
    // Work out if the viewed items has changed.  If so, check whether we need 
    // to calculate any new thumbnails for display.
    if ( oldTopLeft != iTopLeftIndex )
        {
        RecalculateThumbs();
        }

    // Notify observers of the change
    NotifyModelObserver( EModelEventHighlight );    

    if ( iCurrHighlight != oldHighlight )
        {
        return ETrue;   // Highlight HAS changed
        }
    else 
        {
        return EFalse;  // No change
        }    
    }

// Sets the higlighted burst item
// informs model observer about the change
// returns ETrue if highlight changed
// returns EFalse if highlight was not changed
TBool CCamBurstThumbnailGridModel::SetHighlight( TInt aIndex )
	{
	PRINT1( _L("Camera => CCamBurstThumbnailGridModel::SetHighlight( %d )"), aIndex );
	
	TInt oldHighlight = iCurrHighlight;
	iCurrHighlight = aIndex;  

    PRINT( _L("Camera <= CCamBurstThumbnailGridModel::SetHighlight()") );
    
    if ( iCurrHighlight != oldHighlight )
        {
        return ETrue;   // Highlight HAS changed
        }
    else 
        {
        return EFalse;  // No change
        }  

	}

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::UpdateViewableGrid
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::UpdateViewableGrid( TBool aMovingUp )
    {          
    if ( NoOfValidCells() <= KBurstGridMaxVisibleThumbnails )
        {
        // no need to update if there are less cells than fit to screen at once
        return;
        }
    
    TInt posY = iCurrHighlight / KGridWidth; 

    // If moving up, when adjust the viewable area, the highlighted grid is 
    // in the top-most row.
    if ( aMovingUp )
        {        
        if ( posY >= ( iGridHeight - 1 ) )
            {
            iTopLeftIndex = ( posY - ( iGridHeight - 1 ) ) * KGridWidth;    
            }
        else // Only one (incomplete) row
            { 
            iTopLeftIndex = ( posY ) * KGridWidth;    
            }
        }
    // If moving down, when adjust the viewable area, the highlighted grid item
    // is in the bottom-most row
    else
        {   
        if ( posY >= ( iGridHeight - 1 ) )
            {
            iTopLeftIndex = ( posY - ( iGridHeight - 1 ) ) * KGridWidth;    
            }
        else // Only one (incomplete) row
            { 
            iTopLeftIndex = ( posY ) * KGridWidth;    
            }
        }
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::HighlightedGridIndex
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::HighlightedGridIndex()
    {
    return iCurrHighlight;
    }


// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::TopLeftGridIndex
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::TopLeftGridIndex()
    {   
    return iTopLeftIndex;
    }  

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::HighlightedBurstIndex
// As used for getting the index in the CONTROLLER's array, which contains
// both non-deleted AND DELETED items in the array.
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::HighlightedBurstIndex()
    {
    TInt index = -1;
    if ( iCurrHighlight != KErrNotFound )
        {
        // The "Current Highlight" is the index to the VALID array (non-deleted only)
        // Need to find the equivalent index in the GLOBAL array (del and non-del)
        index = ConvertFromValidToGlobalIndex( iCurrHighlight );
        }    

    return index;
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::HighlightedImageName
// -----------------------------------------------------------------------------
//
TPtrC CCamBurstThumbnailGridModel::HighlightedImageName()
    {
    if ( iCurrHighlight != KErrNotFound )
        {
        // The "Current Highlight" is the index to the VALID array (non-deleted only)
        // Need to find the equivalent index in the GLOBAL array (del and non-del)
        TInt index = ConvertFromValidToGlobalIndex( iCurrHighlight );
        if ( index != KErrNotFound )
            {
            return iBurstArray->ImageName( index );
            }
        }    
    return TPtrC();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::ImageName
// Returns the image name (no path or extension) of an item.
// -----------------------------------------------------------------------------
//
TPtrC CCamBurstThumbnailGridModel::ImageName( TInt aIndex )
    {
    // The "Current Highlight" is the index to the VALID array (non-deleted only)
    // Need to find the equivalent index in the GLOBAL array (del and non-del)
    TInt index = ConvertFromValidToGlobalIndex( aIndex );
    if ( index != KErrNotFound )
        {
        return iBurstArray->ImageName( index );
        }
    return TPtrC();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::ImageFullName
// Returns the image name (with path and extension) of an item.
// -----------------------------------------------------------------------------
//
TPtrC CCamBurstThumbnailGridModel::ImageFileName( TInt aIndex ) const
    {
    return iBurstArray->FileName( aIndex );
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::RefreshL
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::RefreshL()
    {
    // If this function is called, it is to check whether the currently 
    // highlighted image has not been deleted yet.  This may occur if it has
    // been viewed and deleted in the post-capture view.

    // Get the global index for the highlighted item
    TInt index = ConvertFromValidToGlobalIndex( iCurrHighlight );

    // Check it's deleted state
    TBool deleted = iBurstArray->IsDeleted( index );

    // If the item has been deleted from the BurstArray, we need to update
    // our state, so run the delete highlight code for the GridModel
    if ( deleted )
        {
        // Update internal state accordingly
        DeleteHighlightL();
        }
    }


// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::SetCurrentMark
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::SetCurrentMark( TBool aSet )
    {    
    if ( aSet ) // Setting the mark ON
        {
        iValidThumbGrid[iCurrHighlight]->iMarked = ETrue;
        iImagesMarked++;
        }    
    else    // Setting the mark OFF
        {
        iValidThumbGrid[iCurrHighlight]->iMarked = EFalse;
        iImagesMarked--;
        }
    NotifyModelObserver( EModelEventThumb );   // Force thumbnail redraw
    }


// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::MarkAll
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::MarkAll( TBool aMark )
    {
    TInt count = iValidThumbGrid.Count();
    TInt i;

    // Go through each valid (non-deleted) item
    for ( i = 0; i < count; i++ )
        {
        // If it's not already in the required state (marked or unmarked)
        if ( !( iValidThumbGrid[i]->iMarked == aMark ) )
            {
            // ...change the state of the item
            iValidThumbGrid[i]->iMarked = aMark;

            // ...and update the internal count
            if ( aMark )
                {
                iImagesMarked++;
                }
            else
                {
                iImagesMarked--;
                }
            }
        }
    NotifyModelObserver( EModelEventThumb );   // Force thumbnail redraw
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::NoOfMarkedImages
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::NoOfMarkedImages() const
    {
    return iImagesMarked;
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::NoOfImages
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::NoOfImages() const
    {
    return iAllThumbGrid.Count();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::IsMarkedL
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::IsMarkedL( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iValidThumbGrid.Count() )
        {
        User::Leave( KErrArgument );
        }
    return iValidThumbGrid[aIndex]->iMarked;
    }



// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::DeleteMarkedL
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::DeleteMarkedL()
    {
    // Keep track of if we've just deleted the highlighted item.  
    // If so, we'll need to inform the view, later.
    TBool highlightDeleted =  iValidThumbGrid[iCurrHighlight]->iMarked ;        

    TInt i;
    TInt count = iAllThumbGrid.Count();
    TInt error = KErrNone;
    for ( i = 0; i < count; i++ )
        {
        if ( iAllThumbGrid[i]->iMarked )
            {    
            TRAP( error, DeleteItemL( i ) );
            if ( error )
                {
                break;
                }
            }
        }
    
    // Do the recalculation required to adjust the positioning of items and
    // the hightlight
    DoPostDeleteGridCheck( highlightDeleted );
    
    if ( error && error != KErrInUse )
        {
        User::Leave( error );
       }
    }


// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::DeleteHighlightL
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::DeleteHighlightL()
    {
    // Gets the global index of the currently highlighted item
    TInt globalIndex = ConvertFromValidToGlobalIndex( iCurrHighlight );

    // Delete that item
    
    TRAPD( error, DeleteItemL( globalIndex ) );
    
    
    // Check the internal state for any adjustments of visible items, new
    // highlight position etc.
    DoPostDeleteGridCheck( ETrue );
    
    if ( error && error != KErrInUse )
        {
        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::DeleteItemL
// Internal function for deleting a particular item
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::DeleteItemL( TInt aGlobalIndex )
    {
    PRINT( _L("Camera => CCamBurstThumbnailGridModel::DeleteItemL") );
    // If this item was marked, clear the mark and adjust internal state
    TInt err = KErrNone;
    if ( !iBurstArray->IsDeleted( aGlobalIndex ) )
        {
        PRINT( _L("Camera <> iBurstArray->IsDeleted" ))
        err = iBurstArray->SetDeleted( aGlobalIndex, ETrue );
        PRINT1( _L("Camera <> CCamBurstThumbnailGridModel::DeleteItemL: First SetDelete returned %d"), err );
        }
    if ( err )
        { 
        err = iBurstArray->SetDeleted( aGlobalIndex, ETrue );
        PRINT1( _L("Camera <> CCamBurstThumbnailGridModel::DeleteItemL: Second SetDelete returned %d"), err );
        if ( err )
            {
            if ( err == KErrInUse )
                {
                // set error note text
                const TDesC& fullPath = iBurstArray->FileName( aGlobalIndex );
                TInt pos = fullPath.LocateReverse( '\\' );
                TPtrC fileName = fullPath.Right( fullPath.Length() - pos - 1 );
                HBufC* text = StringLoader::LoadLC( R_QTN_FLDR_CANT_DELETE_FILE_OPEN, fileName );
                // show error note
                CAknInformationNote* dlg = new (ELeave) CAknInformationNote(ETrue);
                dlg->ExecuteLD(text->Des());
                CleanupStack::PopAndDestroy( text );
                }
            User::Leave( err );
            //return;
            }
        }
    if ( iAllThumbGrid[aGlobalIndex]->iMarked )
        {            
        iAllThumbGrid[aGlobalIndex]->iMarked = EFalse;
        iImagesMarked--;    // No of marked items
        }

    // Delete the display thumbnail
    delete iAllThumbGrid[aGlobalIndex]->iDisplayThumb;
    iAllThumbGrid[aGlobalIndex]->iDisplayThumb = NULL;                

    // Remove from VALID thumb list.            
    CThumbnailGridItem* item = iAllThumbGrid[aGlobalIndex];
    TInt index = iValidThumbGrid.Find( item );    

    if ( index != KErrNotFound )
        {
        iValidThumbGrid.Remove( index );
        }
    PRINT( _L("Camera <= CCamBurstThumbnailGridModel::DeleteItem") );
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::DoPostDeleteGridCheck
// Internal function called after an item has been deleted, to ensure the 
// correct items are visible, the highlight is valid etc.
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::DoPostDeleteGridCheck( TBool /*aHighlightDeleted*/ )
    {   
    // Check the "highlighted" item.  If it's now out of range, move the 
    // index to be the last valid entry.
    if ( iCurrHighlight >= iValidThumbGrid.Count() )
        { 
        iCurrHighlight = iValidThumbGrid.Count() - 1;

        // Required to update the title pane with a new filename
        NotifyModelObserver( EModelEventHighlight );
        }

    // Else the highlight after a delete is still within range, so
    // just inform the observers, so they can update the title pane with the
    // correct filename
    else 
        {
        NotifyModelObserver( EModelEventHighlight );
        }


    TInt imagesRemaining = iBurstArray->ImagesRemaining();
    // Check whether the highlighted item is now visible... it may not be.
    if ( iTopLeftIndex >= imagesRemaining ||
         imagesRemaining <= KGridWidth * iGridHeight )
        {
        // If this has happened it's because the highlighted cell was one of 
        // a large number of cells that has been deleted.  The "top left" 
        // visible cell therefore needs to be updated to be the row above the 

        // If over a screens worth of items remaining, move "top left" to show 
        // the bottom two rows
        if ( imagesRemaining > ( KGridWidth * iGridHeight ) )
            {
            //            =     [ Total number of rows available   ] - [ two rows ]    * [Width]
            iTopLeftIndex = ( ( ( iCurrHighlight + 1 ) / KGridWidth ) - iGridHeight ) * KGridWidth;            
            }
        else // Less than a screen's worth of items left, so set the first row as top left
            {
            iTopLeftIndex = 0;
            }
        }

    // Notify observers of a deletion.
    NotifyModelObserver( EModelEventDeleted );
        
    // Check the thumbnails to see if any need remaking
    RecalculateThumbs();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::ScrollGrid( TBool aScrollDown )
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGridModel::ScrollGrid( TBool aScrollDown, TInt aNewScrollPos )
    {
    
    // scrollPosition tells where the scroll is (new starting row?)
    
    // calculate the new iTopLeftIndex
    
    iTopLeftIndex = aNewScrollPos * KGridWidth;
    
    PRINT1( _L("Camera <> CCamBurstThumbnailGridModel::ScrollGrid - new postition %d"), iTopLeftIndex );
    
    
    }



// -----------------------------------------------------------------------------
// CCamBurstThumbnailGridModel::GridHeight
// Returns the height of the visible grid, typically 2 or 3 cells high.
// -----------------------------------------------------------------------------
//
TInt CCamBurstThumbnailGridModel::GridHeight()
    {
    return iGridHeight;
    }
// -----------------------------------------------------------------------------
// MThumbModelObserver::HighlightChanged
// Default implementation of the function
// -----------------------------------------------------------------------------
//
void MThumbModelObserver::HighlightChanged() 
    {
    // intentionally doing nothing
    }

// -----------------------------------------------------------------------------
// MThumbModelObserver::ImagesDeleted
// Default implementation of the function
// -----------------------------------------------------------------------------
//
void MThumbModelObserver::ImagesDeleted() 
    {
    // intentionally doing nothing
    }

// -----------------------------------------------------------------------------
// MThumbModelObserver::ThumbnailGenerated
// Default implementation of the function; does nothing.
// -----------------------------------------------------------------------------
//
void MThumbModelObserver::ThumbnailGenerated()
    {
    // intentionally doing nothing
    }

// -----------------------------------------------------------------------------
// CThumbnailGridItem::CThumbnailGridItem
// -----------------------------------------------------------------------------
//
CThumbnailGridItem::CThumbnailGridItem()
    {
    // intentionally doing nothing
    }

// -----------------------------------------------------------------------------
// CThumbnailGridItem::~CThumbnailGridItem
// -----------------------------------------------------------------------------
//
CThumbnailGridItem::~CThumbnailGridItem()
    {   
    delete iDisplayThumb;        
    }


//  End of File  
