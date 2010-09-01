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



// INCLUDE FILES
#include <coecntrl.h>
#include <fbs.h>     
#include <eikscrlb.h>           // For CEikScrollBar
#include <bitmaptransforms.h>   // For CBitmapScaler
#include <eikenv.h>
#include <eiksbfrm.h>
#include <barsread.h>           // For TResourceReader
#include <cameraapp.mbg>
#include <AknIconArray.h>
#include <AknIconUtils.h>
#include <avkon.mbg> // EMbmAvkonQgn_indi_marked_grid_add

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <aknlayoutscalable_apps.cdl.h> // AknLayoutScalable_Apps

#include "camuiconstants.h"

#include "CamUtility.h"
#include "CamBurstThumbnailGrid.h"
#include "CamAppUi.h"
#include "CamPanic.h"

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
// CCamBurstThumbnailGrid::CCamBurstThumbnailGrid
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamBurstThumbnailGrid::CCamBurstThumbnailGrid( CCamBurstThumbnailGridModel& aModel ) :
    iModel( aModel )
    {
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGrid::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGrid::ConstructL( const CCoeControl* aParent )
    {// Fill in the model

    SetContainerWindowL( *aParent );  

    // Generate the grid size (either 6 for 3x2 or 9 for 3x3 grid)
    iGridSize = iModel.GridHeight() * KGridWidth;
 
    

    // Create component bitmaps 
   // TFileName resFileName;
    //CamUtility::ResourceFileName( resFileName );
    //TPtrC resname = resFileName;
    
    iBitmapMark = NULL;
    iBitmapMarkMask = NULL;
    
    AknIconUtils::CreateIconL( iBitmapMark, 
                               iBitmapMarkMask, 
                               AknIconUtils::AvkonIconFileName(),
                               EMbmAvkonQgn_indi_marked_grid_add, 
                               EMbmAvkonQgn_indi_marked_grid_add_mask );
        
    // Request notification of Deletion and new Thumbnail events
    iModel.AddModelObserverL( this,  CCamBurstThumbnailGridModel::EModelEventDeleted | 
                                    CCamBurstThumbnailGridModel::EModelEventThumb );
    
    
    UpdateLayout();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGrid::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamBurstThumbnailGrid* CCamBurstThumbnailGrid::NewL( const CCoeControl* aParent, CCamBurstThumbnailGridModel& aModel )
    {
    CCamBurstThumbnailGrid* self = new( ELeave ) CCamBurstThumbnailGrid( aModel );
    
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CCamBurstThumbnailGrid::~CCamBurstThumbnailGrid()
    {
    delete iBitmapMark;
    delete iBitmapMarkMask;        

    // Remove notification of Deletion and new Thumbnail events
    iModel.RemoveModelObserver( this, CCamBurstThumbnailGridModel::EModelEventDeleted | 
                                      CCamBurstThumbnailGridModel::EModelEventThumb );

    DeleteScrollBar();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGrid::Draw
// The control's drawing function
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGrid::Draw( const TRect& aRect ) const
    { 
    PRINT( _L("Camera => CCamBurstThumbnailGrid::Draw") );

    CWindowGc& gc = SystemGc();

    // Draw skin background
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, gc, Rect() );

    TInt index = 0;
    TInt visibleIndex = iModel.TopLeftGridIndex(); // first visible item index
    TInt y = 0;
    TInt x = 0;
    for ( y = 0; y < iModel.GridHeight(); y++ )    // height of visible grid
        {
        for ( x = 0; x < KGridWidth; x++ )
            {

            if ( visibleIndex >= iModel.NoOfValidCells() )
                {
                break; // only breaks the inner for-loop
                }

            // Draw the outline rectangle for the Cell
            //gc.SetPenColor( KGreyOutline ); // From the screenshot                                
            TRect r = iGridCellLayout[ index ].iCellRect.Rect();

            // draw background highlight for the focused item
            if ( iModel.HighlightedGridIndex() == visibleIndex )
                {
#ifdef _DEBUG
                TRect _cell = iGridCellLayout[ index ].iCellRect.Rect();
                TRect _high = iGridCellLayout[ index ].iHighRect.Rect();
                TRect _thmb = iGridCellLayout[ index ].iThmbRect.Rect();
                PRINT4( _L("Camera <> CCamBurstThumbnailGrid - grid cell rect[x:%d y:%d w:%d h%d]"), 
                        _cell.iTl.iX, _cell.iTl.iY, _cell.Width(), _cell.Height() );
                PRINT4( _L("Camera <> CCamBurstThumbnailGrid - highlight rect[x:%d y:%d w:%d h%d]"), 
                        _high.iTl.iX, _high.iTl.iY, _high.Width(), _high.Height() );
                PRINT4( _L("Camera <> CCamBurstThumbnailGrid - thumbnail rect[x:%d y:%d w:%d h%d]"), 
                        _thmb.iTl.iX, _thmb.iTl.iY, _thmb.Width(), _thmb.Height() );
#endif // _DEBUG

                TRgb highlight;
                AknsUtils::GetCachedColor( skin, highlight, KAknsIIDQsnHighlightColors,
                                                            EAknsCIQsnHighlightColorsCG1 );
                PRINT1( _L("Camera <> CCamBurstThumbnailGrid::Draw - highlight color: 0x%08x"), highlight.Internal() );
                gc.SetBrushColor( highlight );                
                gc.Clear( r );
                }


            CThumbnailGridItem* item = iModel.GridItem( visibleIndex );   

            if ( item )
                {

                TRect trgRect( iGridCellLayout[ index ].iThmbRect.Rect() );
                if ( item->iDisplayThumb )
                    {                                  
                    PRINT( _L("Camera <> CCamBurstThumbnailGrid::Draw - prescaled thumb available") );

                    // Thumbnail grid sizer provides an image that fits
                    // to the given rect by one dimension and is larger in the other.
                    // (If aspect ratios don't match.) Crop the oversize area.
                    gc.SetClippingRect( trgRect );
                    
                    const TSize& bmpSize( item->iDisplayThumb->SizeInPixels() );
                    PRINT2( _L("Camera <> CCamBurstThumbnailGrid - bmp size  [w:%d h%d]"), 
                            bmpSize.iWidth, bmpSize.iHeight );

                    PRINT2( _L("Camera <> CCamBurstThumbnailGrid - cell size [w:%d h%d]"), 
                            trgRect.Width(), trgRect.Height() );

                    // Move the start of drawing so, that the image stays centered to the available area.
                    TInt widthDif  = bmpSize.iWidth  - trgRect.Width();
                    TInt heightDif = bmpSize.iHeight - trgRect.Height();
                    TPoint drawTl  = trgRect.iTl - TPoint( widthDif/2, heightDif/2 );
                    PRINT2( _L("Camera <> CCamBurstThumbnailGrid - draw start offset [x:%d y%d]"), 
                            widthDif, heightDif );

                    gc.BitBlt( drawTl, item->iDisplayThumb );
                    gc.SetClippingRect( aRect );
                    }
                else // no rescaled bitmap exists, and we can fast-rescale snapshot
                    {                        
                    PRINT( _L("Camera <> CCamBurstThumbnailGrid::Draw - prescaled thumb NOT available") );
                    const CFbsBitmap* bmp = iModel.Snapshot( iModel.TopLeftGridIndex() + index );                        
    
                    if ( bmp )
                        {
                        TSize bmpSize( bmp->SizeInPixels() );
                        TRect srcRect( bmpSize             );
    
                        TReal bmpRatio( TReal( bmpSize.iWidth  ) / TReal( bmpSize.iHeight  ) );
                        TReal trgRatio( TReal( trgRect.Width() ) / TReal( trgRect.Height() ) );
    
                        if( bmpRatio > trgRatio )
                          {
                          // Thumbnail aspect ratio is wider than the target cell area.
                          // Crop some of the thumbnail width off and fill the target rect.
                          const TInt width = Max( bmpSize.iHeight * trgRatio, 1 );
                          const TInt crop  = ( bmpSize.iWidth - width ) / 2;
                          srcRect.Shrink( crop, 0 );
                          }
                        if( bmpRatio < trgRatio )
                          {
                          // Bitmap is higher than the screen.
                          // Crop some of the thumbnail height off and fill the target rect.
                          const TInt height = Max( bmpSize.iWidth * trgRatio, 1 );
                          const TInt crop   = ( bmpSize.iHeight - height ) / 2;
                          srcRect.Shrink( 0, crop );
                          }
    
                        PRINT2( _L("Camera <> CCamBurstThumbnailGrid - bmp size    [w:%d h%d]"), 
                                bmpSize.iWidth, bmpSize.iHeight );
                        PRINT4( _L("Camera <> CCamBurstThumbnailGrid - bmp cropped [x:%d y:%d w:%d h%d]"), 
                                srcRect.iTl.iX, srcRect.iTl.iY, srcRect.Width(), srcRect.Height() );
                        PRINT4( _L("Camera <> CCamBurstThumbnailGrid - trg rect    [x:%d y:%d w:%d h%d]"), 
                                trgRect.iTl.iX, trgRect.iTl.iY, trgRect.Width(), trgRect.Height() );
    
                        gc.DrawBitmap( trgRect, bmp, srcRect );
                        }

                    }
                
                // Draw the marked symbol
                if ( item->iMarked )
                    {           
                    PRINT2(_L("CAMERA <> CCamBurstThumbnailGrid::Draw (item marked) x:%d y:%d"), x, y )
                    
                    TRect mark = iGridCellLayout[ index ].iMarkRect.Rect();
                       
                    gc.SetBrushStyle( CGraphicsContext::ENullBrush ); // Make sure background isn't drawn                 
                    gc.BitBltMasked( iGridCellLayout[ index ].iMarkRect.Rect().iTl, 
                            iBitmapMark, 
                            iBitmapMark->SizeInPixels(), 
                            iBitmapMarkMask, 
                            EFalse );

                    }

                } // end of: if ( item )

            index++;
            visibleIndex++;
            } // for x
        } // for y

    PRINT( _L("Camera <= CCamBurstThumbnailGrid::Draw") );
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::CountComponentControls
// Returns the number of component controls
// ---------------------------------------------------------
//
TInt CCamBurstThumbnailGrid::CountComponentControls() const
    {
    TInt count = 0; // Return the number of controls inside this container    
    if ( iScrollFrame )
        {
        count = iScrollFrame->CountComponentControls();
        }
    return count;
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::SizeChanged
// Constructs the grid layout structure
// ---------------------------------------------------------
//
void CCamBurstThumbnailGrid::SizeChanged()
    {  
    
    PRINT( _L("Camera => CCamBurstThumbnailGrid::SizeChanged()") );
    
    ReadLayout();

    // set the new thumbnail size for gridmodel (to be used with thumbnail scaling)
    // use layout for the first cell (all thumbnails are same size)
    iModel.SetThumbnailSize( iGridCellLayout[0].iThmbRect.Rect().Size() );
    
    AknIconUtils::SetSize( iBitmapMark, iGridCellLayout[0].iMarkRect.Rect().Size() ); 
    AknIconUtils::SetSize( iBitmapMarkMask, iGridCellLayout[0].iMarkRect.Rect().Size() ); 
    
    PRINT( _L("Camera <= CCamBurstThumbnailGrid::SizeChanged()") );
    }   

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::ResourceId
// Returns the ID of the resource to use for the grid structure
// ---------------------------------------------------------
//
TInt CCamBurstThumbnailGrid::ResourceId()
    {
    TInt resource = 0;
    if ( iModel.NoOfValidCells() > KSmallGridCellCount )
        {
        if ( !AknLayoutUtils::LayoutMirrored() )
            {
            resource = R_CAM_BURST_POST_CAPTURE_GRID_ARRAY;
            }
        else
            {
            resource = R_CAM_BURST_POST_CAPTURE_GRID_ARRAY;
            }        
        }
    else // Else six or less image to be used
        {
        if ( !AknLayoutUtils::LayoutMirrored() )
            {
            resource = R_CAM_BURST_POST_CAPTURE_GRID_ARRAY_UPTO_SIX;
            }
        else
            {
            resource = R_CAM_BURST_POST_CAPTURE_GRID_ARRAY_UPTO_SIX_AH;
            }    
        }   
    return resource; 
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::ReadLayoutL
// ---------------------------------------------------------
//
void CCamBurstThumbnailGrid::ReadLayout()
    {
    PRINT( _L("Camera => CCamBurstThumbnailGrid::ReadLayout() ") );
     
    
    TInt layoutVariantForThumbnail = 0;
    TInt layoutVariantForCell = 1;
    
    if ( iModel.NoOfValidCells() <= KBurstGridDefaultThumbnails)
        {
        layoutVariantForThumbnail = 0;
        layoutVariantForCell = 1;
        }
    else if ( iModel.NoOfValidCells() <= KBurstGridMaxVisibleThumbnails )
        {
        layoutVariantForThumbnail = 1;
        layoutVariantForCell = 3;
        }
    else // iModel.NoOfValidCells() > KBurstGridMaxVisibleThumbnails
        {
        layoutVariantForThumbnail = 2;
        layoutVariantForCell = 5;
        }
    
    TInt i = 0;
    TInt y = 0;
    TInt x = 0;
    
    // set all layout entries for cells
    for ( y = 0; y < iModel.GridHeight(); y++ )
        { 
   	 	for ( x = 0; x < KGridWidth; x++ )
        	 {     
        	 iGridCellLayout[i].iCellRect.LayoutRect( Rect(), 
        			 AknLayoutScalable_Apps::cell_cam4_burst_pane( layoutVariantForCell, x, y ) );
        	 iGridCellLayout[i].iThmbRect.LayoutRect( iGridCellLayout[i].iCellRect.Rect(), 
        			 AknLayoutScalable_Apps::cell_cam4_burst_pane_g1( layoutVariantForThumbnail, 0, 0 ) );
        	 iGridCellLayout[i].iMarkRect.LayoutRect( iGridCellLayout[i].iThmbRect.Rect(),
        	         AknLayoutScalable_Apps::cell_cam4_burst_pane_g2( layoutVariantForThumbnail ) );
        	 i++;
        	 }
        }    

    PRINT( _L("Camera <= CCamBurstThumbnailGrid::ReadLayout() ") );
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::ComponentControl
// Returns the requested component control
// ---------------------------------------------------------
//
CCoeControl* CCamBurstThumbnailGrid::ComponentControl( TInt aIndex ) const
    {
    if ( iScrollFrame )
        {
        return iScrollFrame->ComponentControl( aIndex );
        }
    else
        {
        return NULL;
        }    
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::ThumbnailGenerated
// Called when a thumbnail has been scaled
// ---------------------------------------------------------
//        
void CCamBurstThumbnailGrid::ThumbnailGenerated()
    {
    DrawDeferred();
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::HandleScrollEventL
// ---------------------------------------------------------
//        
void CCamBurstThumbnailGrid::HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType )
    {
        
    // compare to old position, set correct starting point for drawing the grid  
   
    TInt numOfRows = ( iModel.NoOfValidCells() / KGridWidth ) ;
    if ( iModel.NoOfValidCells() % KGridWidth != 0 )
        {
        numOfRows++;
        }

    TInt oldRow = ( iModel.TopLeftGridIndex() / KGridWidth );
    if ( iModel.TopLeftGridIndex() % KGridWidth != 0 )
        {
        oldRow++;
        }
    
    TInt newRow = aScrollBar->ThumbPosition();
    PRINT1( _L("Camera <> CCamBurstThumbnailGrid::HandleScrollEventL - thumb postition %d"), newRow );
    
    
    if ( oldRow != newRow )
        {
        iModel.ScrollGrid( ETrue, newRow);
        DrawDeferred();
        }
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::ImagesDeleted
// Called when one or more images have been deleted
// ---------------------------------------------------------
//        
void CCamBurstThumbnailGrid::ImagesDeleted() 
    {
    // Check if scrollbar frame exists, and if it's currently visible, and 
    // we now have only enough elements for one screen, then hide the bar
    if ( iScrollFrame && 
         iScrollFrame->ScrollBarVisibility( CEikScrollBar::EVertical ) == CEikScrollBarFrame::EOn )
        {
        if ( iModel.NoOfValidCells() <= iGridSize )
            {
            DeleteScrollBar();
            }
        else // Check if need to adjust scrollbar proportions
            {
            // This will adjust the proportions
            TRAPD( ignore, SetupScrollbarL() );
            if ( ignore )
                { 
                // Do nothing ( removes build warning )
                }                   
            }
        }
    
    if ( iModel.NoOfValidCells() <= KBurstGridMaxVisibleThumbnails )
        {
        UpdateLayout();
        // draw parent because size of the control might have been changed
        Parent()->DrawDeferred();
        }
    DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGrid::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CCamBurstThumbnailGrid::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TInt startTopIndex = iModel.TopLeftGridIndex();
    
    if ( aType == EEventKey )
        {
        TBool changed = EFalse;
        switch ( aKeyEvent.iScanCode )
            {
        case EStdKeyUpArrow:
                {
                if ( iModel.MoveHighlight( CCamBurstThumbnailGridModel::EMoveSelectUp ) )
                    {
                    changed = ETrue;    
                    }
                break;
                }

        case EStdKeyDownArrow:
                {
                if ( iModel.MoveHighlight( CCamBurstThumbnailGridModel::EMoveSelectDown ) )
                    {
                    changed = ETrue;                
                    }

                break;
                }

        case EStdKeyLeftArrow:
                {
                CCamBurstThumbnailGridModel::TMoveSelect direction;
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    direction = CCamBurstThumbnailGridModel::EMoveSelectLeft;
                    }
                else
                    {
                    direction = CCamBurstThumbnailGridModel::EMoveSelectRight;
                    }

                if ( iModel.MoveHighlight( direction ) )
                    {
                    changed = ETrue;                
                    }
                break;
                }

        case EStdKeyRightArrow:
                {
                CCamBurstThumbnailGridModel::TMoveSelect direction;
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    direction = CCamBurstThumbnailGridModel::EMoveSelectRight;
                    }
                else
                    {
                    direction = CCamBurstThumbnailGridModel::EMoveSelectLeft;
                    }

                if ( iModel.MoveHighlight( direction ) )
                    {
                    changed = ETrue;                
                    }
                break;
                }
            
        default:
                {
                break;
                }

            } // end of switch
        
        // Update scroll bar if visible screen proportions changed
        if ( iScrollFrame )
            {
            // Work out whether the 3x2 view has moved
            TInt endTopIndex = iModel.TopLeftGridIndex();
            TInt indexDiff = endTopIndex - startTopIndex;

            if ( indexDiff != 0 )
                {
                iScrollFrame->MoveVertThumbTo( endTopIndex / KGridWidth );
                }            
            }   

        // If a change was made, update the display
        if ( changed )
            {
            DrawDeferred();
            }
        }
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGrid::SetupScrollbarL
// Checks if a scroll bar is needed, and if so, creates one.
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGrid::SetupScrollbarL()
    {
    PRINT(_L("Camera => CCamBurstThumbnailGrid::SetupScrollbarL()") );
    __ASSERT_DEBUG( iScrollFrame, CamPanic( ECamPanicNullPointer ) );
    
    
    // Work out the number of scrollable rows we need.  Take into account
    // the rounding errors with integer maths with a modulus check too.
    TInt numOfRows = ( iModel.NoOfValidCells() / KGridWidth ) ;
    if ( iModel.NoOfValidCells() % KGridWidth != 0 )
        {
        numOfRows ++;
        }

    TInt startRow = ( iModel.TopLeftGridIndex() / KGridWidth );
    if ( iModel.TopLeftGridIndex() % KGridWidth != 0 )
        {
        startRow ++;
        }
    iScrollFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOn );          
    
   
    // Set up the model accordingly
    ivModel.SetScrollSpan( numOfRows );
    ivModel.SetFocusPosition( startRow );  
    ivModel.SetWindowSize( iModel.GridHeight() );
    iScrollFrame->CreateDoubleSpanScrollBarsL( ETrue, EFalse ); // was true, false
    
    TRect rect = iScrollLayout.Rect();
    iScrollFrame->Tile( &ivModel, rect );                
    iScrollFrame->MoveVertThumbTo( startRow );
    
    
    PRINT(_L("Camera <= CCamBurstThumbnailGrid::SetupScrollbarL()") );
    }
    
    
    
// ---------------------------------------------------------
// CCamBurstThumbnailGrid::DeleteScrollBar
// Hides and deletes the scrollbar frame 
// ---------------------------------------------------------
//
void CCamBurstThumbnailGrid::DeleteScrollBar()
    {
    if ( iScrollFrame )
        {
        // Hide the scrollbar
        TRAPD( ignore, iScrollFrame->SetScrollBarVisibilityL( 
                            CEikScrollBarFrame::EOff, 
                            CEikScrollBarFrame::EOff ) );
        if ( ignore )
            { 
            // Do nothing ( removes build warning )
            }                   
        }
        
    // Delete the frame
    delete iScrollFrame;
    iScrollFrame = NULL;    
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::CreateScrollBarL
// Creates the scrollbar frame object 
// ---------------------------------------------------------
//
void CCamBurstThumbnailGrid::CreateScrollBarL()    
    {
    // Create scrollbar frame
    iScrollFrame = new ( ELeave ) CEikScrollBarFrame( this, this, ETrue );    
    }
    
    
// ---------------------------------------------------------
// CCamBurstThumbnailGrid::GridSize
// Returns the height of the thumbnail grid, in cells
// ---------------------------------------------------------
//    
TInt CCamBurstThumbnailGrid::GridSize() const
    {
    return iGridSize;
    }

// ---------------------------------------------------------
// CCamBurstThumbnailGrid::UpdateLayout()
// Updates layouts. Used to reload layouts when images are deleted.  
// ---------------------------------------------------------
//
void CCamBurstThumbnailGrid::UpdateLayout()
    {
    TInt gridSize = iModel.NoOfValidCells();
    
    TRect mainPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
            mainPaneRect );
    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane,
            statusPaneRect );

    // layout area rectangle contains the area, where components need to be 
    // drawn to. the container size is the whole screen, but the layouts are 
    // for the client area. aRect is the container size that might include or
    // might not include statuspane area. calculating area self will
    // go around the problem
    TRect layoutAreaRect;
    layoutAreaRect = mainPaneRect;
    layoutAreaRect.BoundingRect( statusPaneRect );

    //    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );

    TAknLayoutRect burstPaneRect;           
    if ( CamUtility::IsNhdDevice() )
        {
        // use layoutAreaRect for nhd (contains the statuspane)
        burstPaneRect.LayoutRect( layoutAreaRect, AknLayoutScalable_Apps::main_cam4_burst_pane() );
        // SetRect( layoutAreaRect );
        }
    else
        {
        // use mainpane for vga
        burstPaneRect.LayoutRect( mainPaneRect,  AknLayoutScalable_Apps::main_cam4_burst_pane() );
        //   SetRect( mainPaneRect );                  
        }
        
    
    TRect rect;
    if ( gridSize <= KBurstGridDefaultThumbnails)
        {
        AknLayoutUtils::LayoutControl( this, 
                burstPaneRect.Rect(), 
                AknLayoutScalable_Apps::grid_cam4_burst_pane(1) );
        }
    else if ( gridSize <= KBurstGridMaxVisibleThumbnails )
        {
        AknLayoutUtils::LayoutControl( this,
                burstPaneRect.Rect(), 
                AknLayoutScalable_Apps::grid_cam4_burst_pane(3) );
        }
    else
        {
        AknLayoutUtils::LayoutControl( this,
                burstPaneRect.Rect(), 
                AknLayoutScalable_Apps::grid_cam4_burst_pane(5) );
        }
    
    
    // set scrollbar layout
    iScrollLayout.LayoutRect( burstPaneRect.Rect(), 
            AknLayoutScalable_Apps::scroll_pane_cp30(1) );
    

    ReadLayout();
    
    // set the new thumbnail size for gridmodel (to be used with thumbnail scaling)
    // use layout for the first cell (all thumbnails are same size)
    iModel.SetThumbnailSize( iGridCellLayout[0].iThmbRect.Rect().Size() );
        
    AknIconUtils::SetSize( iBitmapMark, iGridCellLayout[0].iMarkRect.Rect().Size() ); 
    AknIconUtils::SetSize( iBitmapMarkMask, iGridCellLayout[0].iMarkRect.Rect().Size() ); 
    }
// ---------------------------------------------------------
// CCamBurstThumbnailGrid::HitTestGridItems( TPoint aPosition ) 
// returns the grid item index that the position corresponds
// if position does not match to any grid item return value is negative
// ---------------------------------------------------------
TInt CCamBurstThumbnailGrid::HitTestGridItems( const TPoint aPosition ) 
	{
    
    TInt retVal = -1;
          
	for ( TInt i = 0; i < KGridSize; i++ )
		{
		if ( iGridCellLayout[i].iCellRect.Rect().Contains( aPosition ) )
			{
			retVal = i;
			break;
			}
		} // end for
	
	// check if the scrollbar position affects to the grid item index
	if ( iScrollFrame )
	    {
	    retVal = retVal + iScrollFrame->VerticalScrollBar()->ThumbPosition()*KGridWidth;
	    }
    	
    return retVal < iModel.NoOfValidCells() ? retVal : -1;
	}

// -----------------------------------------------------------------------------
// CCamBurstThumbnailGrid::HighlightChanged
// Sets the grid to be redrawn
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailGrid::HighlightChanged() 
    {
    // does nothing at the moment
    }

// ---------------------------------------------------------
// TCamBurstGridCellLayout::ConstructFromResource
// Constructs the layout entries from the specified resource 
// ---------------------------------------------------------
//
void TCamBurstGridCellLayout::ConstructFromResource( TResourceReader& aReader, TRect& aRect )
    {    
    iCellRect.LayoutRect( aRect, aReader );
    iHighRect.LayoutRect( aRect, aReader );
    iMarkRect.LayoutRect( aRect, aReader );
    iThmbRect.LayoutRect( aRect, aReader );
    }

//  End of File  
