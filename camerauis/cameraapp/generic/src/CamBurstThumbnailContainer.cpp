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
* Description:  Container class for burst thumbnail view*
*/

// INCLUDE FILES
#include <aknview.h> 
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <touchfeedback.h>
#include <aknlayoutscalable_apps.cdl.h> // AknLayoutScalable_Apps

 
#include "CamBurstThumbnailContainer.h"
#include "CamBurstThumbnailGrid.h"
#include "camlogging.h"
#include "CamUtility.h"



// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamBurstThumbnailContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamBurstThumbnailContainer* CCamBurstThumbnailContainer::NewL(
        CCamAppController& aController,
        CAknView& aView,
        const TRect& aRect,
        CCamBurstThumbnailGridModel& aGridModel )
    {
    CCamBurstThumbnailContainer* self = new( ELeave ) 
        CCamBurstThumbnailContainer( aController, aView, aGridModel );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( );
    // return newly created CCamBurstThumbnailContainer instance
    return self;
    }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailContainer
// Destructor
// ---------------------------------------------------------------------------
//
CCamBurstThumbnailContainer::~CCamBurstThumbnailContainer()
    {
    delete iGridControl;
    }
  
// ---------------------------------------------------------
// CCamBurstThumbnailContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamBurstThumbnailContainer::ConstructL( const TRect& aRect )
    {

    PRINT( _L("Camera => CCamBurstThumbnailContainer::ConstructL") );
    CCamContainerBase::BaseConstructL( aRect );
    
    iGridControl = CCamBurstThumbnailGrid::NewL( this , iGridModel );
  
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

        }

    
    // layout grid
    TInt gridSize = iGridModel.NoOfValidCells();   
    
    if ( gridSize <= KBurstGridDefaultThumbnails)
    	{
	    AknLayoutUtils::LayoutControl( iGridControl, /*aRect*/
	            burstPaneRect.Rect(), 
	            AknLayoutScalable_Apps::grid_cam4_burst_pane(1) );
	    }
	else if ( gridSize <= KBurstGridMaxVisibleThumbnails )
	    {
	    AknLayoutUtils::LayoutControl( iGridControl, //aRect,
	            burstPaneRect.Rect(), 
	            AknLayoutScalable_Apps::grid_cam4_burst_pane(3) );
	    }
	else // gridSize > KBurstGridMaxVisibleThumbnails
		{
		AknLayoutUtils::LayoutControl( iGridControl, //aRect
		        burstPaneRect.Rect(), 
		        AknLayoutScalable_Apps::grid_cam4_burst_pane(5) );
		}
    
    // scroll bar
    if ( gridSize > KBurstGridMaxVisibleThumbnails )       
        {
        // Will create the scrollbar if it is needed        
        iGridControl->SetMopParent( this );
        iGridControl->CreateScrollBarL();
        iGridControl->SetupScrollbarL( );        
        }        
       
    
    iFeedback = MTouchFeedback::Instance(); 
    
    // make sure that softkeys are shown
    DrawableWindow()->SetOrdinalPosition( KCamPostCaptureWindowOrdinalPos );
    
    PRINT( _L("Camera <= CCamBurstThumbnailContainer::ConstructL") );
    }
    
// ---------------------------------------------------------------------------
// CCamBurstThumbnailContainer::CCamBurstThumbnailContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamBurstThumbnailContainer::CCamBurstThumbnailContainer( 
                            CCamAppController& aController,
                            CAknView& aView,
                            CCamBurstThumbnailGridModel& aGridModel )
: CCamContainerBase( aController, aView ), iGridModel( aGridModel )
    {
    }

// ---------------------------------------------------------
// CCamBurstThumbnailContainer::CountComponentControls 
// Returns the number of controls owned
// ---------------------------------------------------------
//
TInt CCamBurstThumbnailContainer::CountComponentControls() const
    {    
    return 1;   // Return the number of controls inside this container
    }

// ---------------------------------------------------------
// CCamBurstThumbnailContainer::ComponentControl
// Return pointer to a contained control
// ---------------------------------------------------------
//
CCoeControl* CCamBurstThumbnailContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iGridControl;
            }            
        default: 
            break;                   
        }
    
    // Should never get here
    return NULL;
    }


// ---------------------------------------------------------
// CCamBurstThumbnailContainer::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamBurstThumbnailContainer::Draw( const TRect& aRect ) const
    {
    
    CWindowGc& gc = SystemGc();
        
    // Draw skin background for the 
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, gc, aRect );
    
    //gc.Clear( Rect() );
    
    }

// ----------------------------------------------------
// CCamBurstThumbnailContainer::OfferKeyEventL
// Handles this application view's command keys. Forwards other
// keys to child control(s).
// ----------------------------------------------------
//
TKeyResponse 
CCamBurstThumbnailContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {
    PRINT( _L("Camera => CCamBurstThumbnailContainer::OfferKeyEventL") );
    TKeyResponse response( EKeyWasNotConsumed );
    TBool captureKey = IsCaptureKeyL( aKeyEvent, aType );

    TBool embedded = static_cast<CCamAppUiBase*>( 
            CEikonEnv::Static()->AppUi() )->IsEmbedded();

    // If the back softkey is pressed
    if ( aKeyEvent.iScanCode == EStdKeyDevice1 )
        {
        response = EKeyWasNotConsumed; // indicate key press was not processed
        }
    else if ( aKeyEvent.iScanCode == EStdKeyDevice3 )   // Select key
        {
        // Will pass it on to the view
        response = CCamContainerBase::OfferKeyEventL( aKeyEvent, aType );
        }
    //make sure to unset DemandKeyRelease
    else if( aType == EEventKeyUp && !embedded )
        {
        iController.SetDemandKeyRelease( EFalse ); 
        }    
    else if ( captureKey // && aKeyEvent.iScanCode == EProductKeyCapture )
            || IsShutterKeyL( aKeyEvent, aType )
            )
        {
        if ( aType == EEventKeyDown ) 
            // check needed, since EEventFocusLost produces EEventKeyUp for shutterkey
            {
            if ( !embedded )
                {
                    iGridModel.CancelThumbnails();
    
                    iView.HandleCommandL( ECamCmdNewPhoto );
                    // Demand to relese capture key in pre capture view
                    iController.SetDemandKeyRelease( ETrue ); 
                    response = EKeyWasConsumed;
                }
        
            }
        }
    else if ( aType == EEventKey && 
            aKeyEvent.iScanCode == EStdKeyBackspace )  // Handle 'C' key
        {
        iView.HandleCommandL( ECamCmdDelete );
        response = EKeyWasConsumed;
        }
    else if ( aKeyEvent.iScanCode == EStdKeyYes 
            && aType == EEventKey && !AknLayoutUtils::PenEnabled() ) 
        {
        iView.HandleCommandL( ECamCmdQuickSend );
        response = EKeyWasConsumed; 
        }        
    else 
        {
        PRINT( _L("Camera <> CCamBurstThumbnailContainer: offer key to grid control..") );
        response = iGridControl->OfferKeyEventL( aKeyEvent, aType );        
        }

    PRINT1( _L("Camera <= CCamBurstThumbnailContainer::OfferKeyEventL, return %d"), response );
    return response;
  }

void CCamBurstThumbnailContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent ) 
	{
	//CCoeControl::HandlePointerEventL( aPointerEvent );  
	
	PRINT3( _L("Camera => CCamBurstThumbnailContainer::HandlePointerEventL iType=%d iPosition=(%d, %d)"),
	        aPointerEvent.iType,
	        aPointerEvent.iPosition.iX,
	        aPointerEvent.iPosition.iY );
	    
	
	if ( aPointerEvent.iType == TPointerEvent::EButton1Up)
		{
		// check if the pointer event should be handled as grid selection
		if ( iGridControl && iGridControl->Rect().Contains( aPointerEvent.iPosition ) )
			{
			TInt gridItem = iGridControl->HitTestGridItems( aPointerEvent.iPosition );
			
			if ( gridItem >= 0 )
				{
				
				if ( iFeedback )
				    {
				    iFeedback->InstantFeedback( ETouchFeedbackBasic );        
				    }
				// inform model about the new higlighted item
				if ( iGridModel.SetHighlight( gridItem ) )
					{
					// highlight changed -> redraw
					DrawDeferred();
					}	
				else 
					{
					// highlight item did not change -> open item in postcapture view
					iView.HandleCommandL( EAknSoftkeySelect );
					}
				}
			}       
	
		}
	        
	
	PRINT( _L("Camera <= CCamBurstThumbnailContainer::HandlePointerEventL"));
	
	}

// End of File  
