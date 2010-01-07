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
* Description:  Model class holding thumbnail grid state information*
*/



#ifndef CAMBURSTTHUMBNAILGRIDMODEL_H
#define CAMBURSTTHUMBNAILGRIDMODEL_H

//  INCLUDES
#include    <e32base.h>
#include    <e32std.h>
#include    <coecntrl.h>

#include    <eiksbobs.h>    // For Scroll component
#include    <eikscrlb.h>    // For ScrollModel

#include    "CamAppController.h"
#include    "CamBurstThumbnailGridSizer.h"

// CONSTANTS
// The max number of items in the small grid layout
const TInt KSmallGridCellCount = 6;

// The height of the small grid in cells
const TInt KSmallGridHeight = 2;

// The height of the large grid in cells
const TInt KLargeGridHeight = 3;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCamThumbnailGridSizer;

// CLASS DECLARATION

/**
*  Simple class representing a burst capture image's attributes
*
*  @since 2.8
*/
class CThumbnailGridItem : public CBase
    {
public:
    // Constructor
    CThumbnailGridItem();

    // Destructor
    ~CThumbnailGridItem();
            
public:
    // Scaled-down thumbnail for display in the Grid control
    CFbsBitmap* iDisplayThumb;

    // Whether or not this item has been marked.
    TBool 		iMarked;
    };


/**
*  Mix-in class used for listening to GridModel events.
*
*  @since 2.8
*/
class MThumbModelObserver
    {
public: 
    /**
    * Called when the highlighted option changes
    * @since 2.8    
    */
    virtual void HighlightChanged();

    /**
    * Called when one or more images have been deleted
    * @since 2.8    
    */
    virtual void ImagesDeleted();

    /**
    * Called when a new thumbnail has been generated
    * @since 2.8    
    */
    virtual void ThumbnailGenerated();
    };


/**
*  Contains the state of the Burst Post-Capture control
*
*  @since 2.8
*/
class CCamBurstThumbnailGridModel :  public CBase,
                                     public MBitmapScaler
    {
    public:
        // Bitmask used to identify the events to register for.
        enum TModelEvent
        {
            EModelEventHighlight = 0x1,
            EModelEventDeleted   = 0x2,
            EModelEventThumb     = 0x4
        };

        // Used to represent the direction of highlight movement
        enum TMoveSelect 
        {
            EMoveSelectUp,
            EMoveSelectDown,
            EMoveSelectLeft,
            EMoveSelectRight,
        };

        // Used to represent the internal state for the image resizing operation
        enum TTmbState
        {
            ETmbStateScaling,       // Currently scaling an image
            ETmbStateIdle           // Image finished, but more to do
        };

    public:        
        /**
        * Two-phased constructor.
        */
        static CCamBurstThumbnailGridModel* NewL();

        /**
        * Destructor.
        */
        ~CCamBurstThumbnailGridModel();  
        
    public: // New functions
        /**
        * Attempts to move the selection highlight a specified direction
        * @since 2.8
        * @param aDir The direction to move the hightlight
        * @return TBool indicating whether the move was possible or not
        */
        TBool MoveHighlight( TMoveSelect aDir );
        
        TBool SetHighlight( TInt aIndex );

    	/**
    	 * Sets new size for the scaling
    	 * @param aThumbSize The new size
    	 */
    	void SetThumbnailSize( const TSize aThumbSize );

        /**
        * Returns the number of valid (non-deleted) cells.  Used by thumbnail
        * grid to find how many cells it may need to draw.
        * @since 2.8       
        * @return Number of cells
        */        
        TInt NoOfValidCells();

        /**
        * Returns a specific grid item based on the supplied index.
        * @since 2.8
        * @param aValidIndex The index of the grid item to retrieve
        * @return Pointer to the required item
        */
        CThumbnailGridItem* GridItem( TInt aValidIndex );

        /**
        * Returns a specific bitmap based on the supplied index.
        * @since 2.8
        * @param aValidIndex The index of the bitmap to retrieve
        * @return Pointer to the required bitmap
        */
        const CFbsBitmap* Snapshot( TInt aValidIndex );

        /**
        * Set the burst array for the model to use
        * @since 2.8
        * @param aArray pointer to the burst array
        */
        void SetBurstArrayL( CCamBurstCaptureArray* aArray );

        /**
        * Returns the grid index for the cell in the top-left of the display
        * @since 2.8
        * @return The index to top left cell
        */
        TInt TopLeftGridIndex();

        /**
        * Returns the position of the highlight.  
        * @since 2.8
        * @return The index to the valid (non-deleted) list
        */
        TInt HighlightedGridIndex();

        /**
        * Returns the index of the hightlighted entry in the CONTROLLER's
        * burst array.  This array includes deleted and NON-DELETED items
        * @since 2.8
        * @return The index to the controller's burst array
        */
        TInt HighlightedBurstIndex();        

        /**
        * Sets (or clears) the selection 'mark' of the currently highlighted
        * cell item
        * @since 2.8
        * @param aSet Whether to set or clear the mark
        */
        void SetCurrentMark( TBool aSet );

        /**
        * Marks or un-marks all non-deleted items
        * @since 2.8
        * @param aMark ETrue to set the mark, EFalse to clear it.
        */
        void MarkAll( TBool aMark );

        /**
        * Returns the number of images that have been "marked"
        * @since 2.8
        * @return The number of marked images.
        */
        TInt NoOfMarkedImages() const;

        /**
        * Returns the total number of images including 'deleted' images
        * @since 2.8
        * @return The number of images.
        */
        TInt NoOfImages() const;

        /**
        * Returns whether the specified item is marked or not.
        * @since 2.8
        * @param  aIndex The index of the item to check
        * @return ETrue if it is marked, else EFalse
        */
        TInt IsMarkedL( TInt aIndex );

        /**
        * Deletes all marked items
        * @since 2.8        
        */        
        void DeleteMarkedL();

        /**
        * Deletes currently highlighted object
        * @since 2.8        
        */   
        void DeleteHighlightL();        

        /**
        * Adds an observer to a specified set of events
        * @since 2.8
        * @param aObserver The component to be notified
        * @param aBitMask The bitmask of events to be reported
        */
        void AddModelObserverL( MThumbModelObserver* aObserver, TInt aBitMask );
        
        /**
        * Remove an observer to a specified set of events
        * @since 2.8
        * @param aObserver The component to not be notified
        * @param aBitMask The bitmask of events to not be reported
        */
        void RemoveModelObserver( MThumbModelObserver* aObserver, TInt aBitMask );

        /**
        * Notify all observers to a specific event
        * @since 2.8
        * @param aEvent The event which has occurred.
        * @param aParam Optional parameter to be passed with the event
        */
        void NotifyModelObserver( TModelEvent aEvent, TInt aParam = 0 );

        /**
        * Returns the image name (no path or extension) of the currently 
        * highlighted item.
        * @since 2.8
        * @return The image name
        */
        TPtrC HighlightedImageName();       
        
        /**
        * Returns the height of the visible grid, typically 2 or 3 cells high.
        * @since 2.8
        * @returns the height of the visible grid.
        */
        TInt GridHeight();
        
        /**
        * Returns the image name (no path or extension) of an item.
        * @since 2.8
        * @param aIndex The index of the item.
        * @return The image name
        */
        TPtrC ImageName( TInt aIndex );

        /**
        * Returns the image name (with path and extension) of an item.
        * Uses the index in the global array (including deleted items)
        * @since 2.8
        * @param aIndex The index of the item (in the global array).
        * @return The image full name and path
        */
        TPtrC ImageFileName( TInt aIndex ) const;
        
        /**
        * Refreshes internal state by checking whether the currently
        * highlighted item has been deleted.  This may happen if an item
        * is viewed in post capture, then deleted.
        * @since 2.8
        */
        void RefreshL();

        /**
        * Used to update where the top-left view on the grid is located.
        * @since 2.8
        * @param aMovingUp whether the update is because the curor is moving up.
        */
        void UpdateViewableGrid( TBool aMovingUp );

        /**
        * Converts from the "valid-only" list of items to the global "AllThumb"
        * list.
        * @since 2.8
        * @param aValidIndex The valid index to convert from.
        * @return The global index, or KErrNotFound.
        */
        TInt ConvertFromValidToGlobalIndex( TInt aValidIndex );
        
        /**
         * 
         */
        void ScrollGrid( TBool aScrollDown, TInt aNewScrollPos );

        /**
        * Cancels the thumbnail scaling
        * @since 2.8
        */
        void CancelThumbnails();
        
        /**
         * Notifies model that images have been deleted outside of camera
         */
        void ImageFilesDeleted();

    public: // Functions from base classes
        /**
        * Called by sizer utility class when a bitmap rescale operation 
        * has completed.
        * @since 2.8
        * @param aErr Indicates the completion code (standard epoc error codes possible)
        * @param aBitmap The resized bitmap (called class takes ownership)
        */
        void BitmapScaleCompleteL( TInt aErr, CFbsBitmap* aBitmap, TInt32 aInfo );

    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CCamBurstThumbnailGridModel();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Adds the specified cell to the internal structures.  Takes ownership
        * of the supplied cell object
        * @since 2.8
        * @param aCell The grid item to add
        */
        void AddCellL( CThumbnailGridItem* aCell );

        /**
        * Used for the generation of the specified thumbnail
        * @since 2.8
        * @param aIndex The index (global) of the item to resize
        */
        void StartThumbnailL( TInt aIndex );
    
        /**
        * Force a check for thumbs; deleting those not visible and generating
        * new thumbs for newly visible cells.  
        * @since 2.8        
        */
        void RecalculateThumbs();

        /**
        * Queues thumbnails to be resized as required.
        * @since 2.8        
        */
        void QueueThumbnailsL();

        /**
        * Internal function used by the various delete functions
        * @since 2.8
        * @param aGlobalIndex The global index of the item to delete
        */
        void DeleteItemL( TInt aGlobalIndex );

        /**
        * Internal function used by the various delete functions
        * @since 2.8
        * @param aHighlightDeleted Whether the delete operation this call 
        *        followed involved the deletion of the highlight
        */
        void DoPostDeleteGridCheck( TBool aHighlightDeleted );
              

    private:    // Data
        TInt iCurrHighlight;    // Grid index of current highlight
        TInt iImagesMarked;     // Number of images marked
        TInt iTmpThumbIndex;    // The record we are creating a thumb for

        // An array of ALL thumbnail grid items, the "global" list.
        // Includes delete and non-deleted
        RPointerArray <CThumbnailGridItem> iAllThumbGrid;

        // An array of VALID (non-deleted) items only.
        RPointerArray <CThumbnailGridItem> iValidThumbGrid;
        

        // Pointer to the burst mode capture array.
        CCamBurstCaptureArray* iBurstArray;     // Not owned

        // Utility class used for resizing bitmaps
        CCamThumbnailGridSizer* iGridSizer;

        // Current internal state (referring to resizing)
        TTmbState iState;

        // List of objects requesting bitmap state updates
        RPointerArray <MThumbModelObserver> iThumbObserver;

        // Array of observers for the hightlight changed
        RPointerArray <MThumbModelObserver> iHighlightObserver;

        // Array of observers for the delete events
        RPointerArray <MThumbModelObserver> iDeleteObserver;

        // Grid index of "top left" cell.
        TInt iTopLeftIndex;
        
        // Either 2 (if upto 6 images) or 3 (if more than 6 images)
        TInt iGridHeight;
        
        // Size of the scaled thumbnails
        TSize iThumbSize;
    }; 


#endif      // CAMBURSTTHUMBNAILGRIDMODEL_H
            
// End of File


