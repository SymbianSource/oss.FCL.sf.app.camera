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
* Description:  Class for displaying the images captured by a Burst session.*
*/



#ifndef CAMBURSTTHUMBNAILGRID_H
#define CAMBURSTTHUMBNAILGRID_H

// ===========================================================================
//  INCLUDES

#include    <e32base.h>
#include    <e32std.h>
#include    <coecntrl.h>
#include    <eiksbobs.h>    // For Scroll component
#include    <eikscrlb.h>    // For ScrollModel
#include    <AknUtils.h>    // For TAknLayoutRect

#include    "CamBurstThumbnailGridModel.h"


// ===========================================================================
// CONSTANTS
const TInt KGridWidth = 3;   // Number of cells left to right

const TInt KGridHeight= 3;   // Number of cells top to bottom

const TInt KGridSize = KGridWidth*KGridHeight;


const TInt KBurstGridDefaultThumbnails = 6;
const TInt KBurstGridMaxVisibleThumbnails = 9;

enum BurstGridLayoutTypes
    {
    EDefaultOrLessThumbnails = 0,
    EMaxVisibleThumbnails,
    EScrollbarNeeded
    };
    
const TInt KNumberOfBurstGridLayoutTypes = 3;

// MACROS

// ===========================================================================
// DATA TYPES

// ===========================================================================
// FUNCTION PROTOTYPES

// ===========================================================================
// FORWARD DECLARATIONS
class CFbsBitmap;
class CCamBurstThumbnailGridModel;
class CBitmapScaler;
class CEikScrollBar;


// ===========================================================================
// CLASS DECLARATION

/**
* Utility class used to group together related layout information into one
* object, and construct itself using a supplied resource reader.
*
*  @since 2.8
*/
class TCamBurstGridCellLayout
    {
public:  
    /**
    * Constructs the layout information for the object
    * @since 2.8
    * @param aReader The resource reader to use for construction 
    * @param aRect The parent rect for the layout entries
    */
    void ConstructFromResource( TResourceReader& aReader, TRect& aRect );
    
public:
    TAknLayoutRect iCellRect;   // Rectangle of the cell
    TAknLayoutRect iHighRect;   // Rectangle of the Highlight 
    TAknLayoutRect iMarkRect;   // Rectangle of the "mark"
    TAknLayoutRect iThmbRect;   // Rectangle of the thumbnail
    };


/**
*  Handles the drawing of the Burst Post-Capture control
*
*  @since 2.8
*/
class CCamBurstThumbnailGrid : public CCoeControl, 
                               public MThumbModelObserver,
                               public MEikScrollBarObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCamBurstThumbnailGrid* NewL( const CCoeControl* aParent, CCamBurstThumbnailGridModel& aModel );
        
        /**
        * Destructor.
        */
        virtual ~CCamBurstThumbnailGrid();

    public: // New functions

        /**
        * Constructs the layout information for the grid
        * @since 2.8
        */
        void ReadLayout();
        
        /**
        * Returns the ID of the resource to use for the grid structure
        * @since 2.8
        * @returns The resource ID to use
        */
        TInt ResourceId();    
        
        /**
         * Tests if the given coordinates match to any item on burst grid
         * @param aPosition Coordinates to be tested
         * @return TInt Grid item index that macthes the coordinates. Negative value returned
         * if position does not match any grid item
         */
        TInt HitTestGridItems( TPoint aPosition );

    public: // Functions from base classes

        /**
        * From CCoeControl
        * @since 2.8
        * @param aRect Area to be redrawn        
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl
        * @since 2.8
        * @return The number of controls owned by this control
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * @since 2.8
        * @param aIndex The index of the required control
        * @return Returns the requested control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl
        * @since 2.8        
        */
        void SizeChanged();

        /**
        * From CCoeControl
        * @since 2.8
        * @param aKeyEvent Details of the key event
        * @param aType The keypress type
        * @return TKeyResponse whether the key event was handled or ignored
        */     
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType ) ;
        
        /**
        * From MThumbModelObserver.
        * Called when the highlight has changed
        * @since 2.8                
        */
        void HighlightChanged();
        
        /**
        * From MThumbModelObserver
        * Called when an image has been deleted.
        * @since 2.8        
        */        
        void ImagesDeleted();

        /**
        * From MThumbModelObserver
        * Called when a new thumbnail has been generated       
        * @since 2.8        
        */        
        void ThumbnailGenerated();

        /**
        * From MEikScrollBarObserver
        * Called when a new thumbnail has been generated       
        * @since 2.8        
        * @param aScrollBar
        * @param aEventType
        */        
        void HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );

        /**
        * Creates the scrollbar control.  SetMopParent must be called before this.
        * @since 3.0
        */        
        void CreateScrollBarL();

        /**
        * Creates the scrollbar control.  
        * @since 3.0
        */        
        void DeleteScrollBar();

        /**
        * Sets up the scroll bar, used on start up and when items are deleted
        * to update the state
        * @since 3.0
        */
        void SetupScrollbarL();
        
        /**
        * Returns the height of the grid control.
        * @since 3.0
        * @return Height of the grid control in cells
        */
        TInt GridSize() const; 
        
    protected:  // New functions
    
        /**
         * Updates layouts. Used to reload layouts when images are deleted.  
         */
        void UpdateLayout();
        
    protected:  // Functions from base classes        

    private:

        /**
        * C++ default constructor.
        */
        CCamBurstThumbnailGrid( CCamBurstThumbnailGridModel& aModel );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CCoeControl* aParent );



    public:     // Data
    
    protected:  // Data

    private:    // Data        
        // Passed in by (and owned by) View
        CCamBurstThumbnailGridModel& iModel;                

        // Array of layout objects read from the resource file.  
        // Each object represents a "cell" on the grid
        TCamBurstGridCellLayout iGridCellLayout[KGridSize];

        // Bitmap and mask for the "mark" check box for an image
        CFbsBitmap* iBitmapMark;        
        CFbsBitmap* iBitmapMarkMask;  
        
        CEikScrollBarFrame* iScrollFrame;
        TAknDoubleSpanScrollBarModel ivModel; // model for double span (side) scrollbar
        TAknLayoutRect iScrollLayout;
        
        // As the grid size is potentially variable depending on whether
        // limited or unlimited capture is used, need to keep track of the
        // grid size used.
        TInt iGridSize;
        
    };



#endif      // CAMBURSTTHUMBNAILGRID_H

// ===========================================================================
// End of File


