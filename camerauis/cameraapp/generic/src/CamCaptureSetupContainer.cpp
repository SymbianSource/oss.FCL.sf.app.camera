/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This implements a control which acts as a container for the*
*/


// INCLUDE FILES
#include <gulutil.h>
#include <aknview.h>
#include <avkon.hrh>
#include <AknUtils.h>
#include <barsread.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <touchfeedback.h> 
#include <akntoolbar.h>

 
#include "CamCaptureSetupContainer.h"
#include "CamAppController.h"
#include "CamCaptureSetupControlHandler.h"
#include "CamUtility.h"
#include "CamCaptureSetupListBox.h"


#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamAppUi.h"

#include "camcameraevents.h"
#include "mcamlistboxmodel.h"

// ===========================================================================
// CONSTANTS

const TUint KCameraEventInterest = ECamCameraEventClassVfData;
const TUint32 KToolbarExtensionBgColor = 0x00000000;
const TInt KToolBarExtensionBgAlpha = 0x7F;

// ===========================================================================
// MEMBER FUNCTIONS 

// ---------------------------------------------------------------------------
// CCamCaptureSetupContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamCaptureSetupContainer* CCamCaptureSetupContainer::NewL( 
                              CCamAppController& aController, 
                              CAknView& aView,
                              CCamCaptureSetupControlHandler& aControlHandler,
                              const TRect& aRect, TBool aFullScreenVF )
    {
    CCamCaptureSetupContainer* self = new( ELeave ) CCamCaptureSetupContainer( 
                                                             aController, 
                                                             aView,
                                                             aControlHandler,
                                                             aFullScreenVF );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    // return newly created CCamCaptureSetupContainer instance
    return self;
    }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::~CCamCaptureSetupContainer
// Destructor
// ---------------------------------------------------------
//
CCamCaptureSetupContainer::~CCamCaptureSetupContainer()
  {
  PRINT( _L("Camera => ~CCamCaptureSetupContainer") );
  // <CAMERAAPP_CAPI_V2_MIGRATION/>
  
  iController.ViewfinderWindowDeleted( &Window() );
  
  iController.RemoveCameraObserver( this );

  delete iCaptureSetupControl;
  delete iTitleText;
  PRINT( _L("Camera <= ~CCamCaptureSetupContainer") );
  }
  
// ---------------------------------------------------------
// CCamCaptureSetupContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamCaptureSetupContainer::ConstructL( const TRect& aRect )
    {
    PRINT( _L("Camera => CCamCaptureSetupContainer::ConstructL ") );
    
    SetMopParent( &iView );
    CCamContainerBase::BaseConstructL( aRect );
    if ( iController.IsTouchScreenSupported() )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
        CAknToolbar* fixedToolbar = appUi->CurrentFixedToolbar();
        if ( fixedToolbar )
            {
            // Remove toolbar from the settings view, before drawing VF, 
            // in order to avoid flickering
            fixedToolbar->SetToolbarVisibility( EFalse );
            }
        }
    iViewFinding = iControlHandler.ControlUsesViewFinder();
    
    if( iViewFinding )
        {
        iCaptureSetupControl = iControlHandler.CreateCaptureSetupControlL( this, !iFullScreenVF );
        }
    else
        {
        iCaptureSetupControl = iControlHandler.CreateCaptureSetupControlL( this, ETrue );        
        }
    iCaptureSetupControl->SetContainerWindowL( *this );

    // If the control requires a viewfinder then add it
    if ( iViewFinding )
        {
        // <CAMERAAPP_CAPI_V2_MIGRATION>
        iController.AddCameraObserverL( this, KCameraEventInterest );
        // iController.AddViewFinderObserverL( this );
        // </CAMERAAPP_CAPI_V2_MIGRATION>
        }   

    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       iLayoutAreaRect );
        
    // layout area rectangle contains the area, where components need to be 
    // drawn to. the container size is the whole screen, but the layouts are 
    // for the client area. aRect is the container size that might include or
    // might not include statuspane area. calculating area self will
    // go around the problem

    TRect listboxRect;  
    if ( AknLayoutUtils::PenEnabled() ) 
        {
        listboxRect = TouchLayout();
        }
    else
        {
        listboxRect = NonTouchLayout();
        } 
    
    if ( iControlHandler.ControlIsListBox() )
        {       
        CCamCaptureSetupListBox* listbox = 
                   static_cast<CCamCaptureSetupListBox*>(iCaptureSetupControl);
        
        // Determine the height of the listbox; Either the height based on the 
        // number of items, or the maximum allowed listbox height.
        CEikScrollBarFrame::TScrollBarVisibility visibility = 
                                                      CEikScrollBarFrame::EOff;
        TInt maxListBoxHeight = listboxRect.Height();
        TInt listBoxHeight = listbox->CalcHeightBasedOnNumOfItems( 
                                           listbox->Model()->NumberOfItems() );
        if ( maxListBoxHeight < listBoxHeight )
            {
            listBoxHeight = maxListBoxHeight;
            visibility = CEikScrollBarFrame::EOn;
            }
            
        const AknLayoutUtils::SAknLayoutControl listboxLayout =
            { ELayoutEmpty, listboxRect.iTl.iX, listboxRect.iTl.iY, 
              ELayoutEmpty, ELayoutEmpty, listboxRect.Width(), listBoxHeight };

        AknLayoutUtils::LayoutControl( iCaptureSetupControl, iLayoutAreaRect, 
                                       listboxLayout);

        listbox->CreateScrollBarFrameL( ETrue );
        listbox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                                        CEikScrollBarFrame::EOff, visibility );
        listbox->MakeVisible( ETrue );
        listbox->SetFocus( EFalse, ENoDrawNow );
        listbox->UpdateScrollBarsL();
        listbox->ScrollBarFrame()->DrawScrollBarsNow();             
        listbox->SetListBoxObserver(this);
        }
       
    iCaptureSetupControl->MakeVisible( ETrue );
    iCaptureSetupControl->SetFocus( EFalse, EDrawNow );
    iTitleText = iEikonEnv->AllocReadResourceL( 
                                    iControlHandler.ControlTitleResourceId() );  
    iController.SetViewfinderWindowHandle( &Window() );
    
    PRINT( _L("Camera <= CCamCaptureSetupContainer::ConstructL") );
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupContainer::ControlPositionL
// Return the top left position for the control
// ---------------------------------------------------------------------------
//
TPoint CCamCaptureSetupContainer::ControlPositionL() const
    {
    TPoint pos;

    // If the viewfinder preview is showing
    if ( iViewFinding )
        {
    /*    
         // Get the control pos from resource
        TResourceReader reader;
        TInt resource = iControlHandler.ControlPositionResourceId();
        iEikonEnv->CreateResourceReaderLC( reader, resource );        

        pos.iX = reader.ReadInt16( );
        pos.iY = reader.ReadInt16( );
        iCaptureSetupControl->SetPosition( pos ); 
    
        // Cleanup the resource reader.
        CleanupStack::PopAndDestroy();  // resource reader
        */
        
        }
    else // The control should be centered horizontally
        {
        
        /*
        pos.iX = ( Rect().Width() - iListBoxWidth ) /2;
        pos.iY = ( Rect().Height() - 
                                  iCaptureSetupControl->MinimumSize().iHeight ) /2;      
                                  */
                                  
                                  
        }
    return pos;
    }
    
// ---------------------------------------------------------------------------
// CCamCaptureSetupContainer::CCamCaptureSetupContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamCaptureSetupContainer::CCamCaptureSetupContainer( 
                             CCamAppController& aController, 
                             CAknView& aView,
                             CCamCaptureSetupControlHandler& aControlHandler,
                             TBool aFullScreenVF )
: CCamContainerBase( aController, aView ), 
iControlHandler( aControlHandler ),
iActivateOnTouchRelease(EFalse),
iFullScreenVF(aFullScreenVF)
    {
    }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::CountComponentControls 
// Returns the number of controls owned
// ---------------------------------------------------------
//
TInt CCamCaptureSetupContainer::CountComponentControls() const
    {
    return CCamContainerBase::CountComponentControls() + 1; // Return the number of controls inside this container
    }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CCamCaptureSetupContainer::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = CCamContainerBase::ComponentControl( aIndex );
    if( control == NULL)
        {
        control = iCaptureSetupControl;
        }
    return control;
    }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamCaptureSetupContainer::Draw( const TRect& aRect ) const
    {  
    PRINT( _L("Camera => CCamCaptureSetupContainer::Draw"))
    CWindowGc& gc = SystemGc();

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background( skin, iBgContext, gc, aRect/*Rect()*/ );
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    
	PRINT( _L("Camera => CCamCaptureSetupContainer::Draw direct VF"))
    
    if( appUi->IsDirectViewfinderActive() &&
        iViewFinding )
        {
        // Fill control with transparency bg colour
        gc.SetPenStyle( CGraphicsContext::ENullPen );
      
        gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb::Color16MA( 0 ) );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        if( iFullScreenVF )
            {
            gc.DrawRect( aRect );
            }
        else
            {
        gc.DrawRect( iViewFinderRectLayout.Rect() );
            }
        
        // Reset the brush after use (otherwise anything drawn
        // after the viewfinder will also show viewfinder frames)
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
        }

    // draw the title text   
    TRgb color;
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                                            EAknsCIQsnTextColorsCG6 );
    if ( AknLayoutUtils::PenEnabled() )
        {
        if( appUi->IsDirectViewfinderActive() &&
            iViewFinding && iFullScreenVF )
            {
            gc.SetBrushColor( TRgb( KToolbarExtensionBgColor, KToolBarExtensionBgAlpha ) );            
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.DrawRect( TRect( iLayoutAreaRect.iTl.iX,
                                iLayoutAreaRect.iTl.iY,
                                iLayoutAreaRect.iBr.iX,
                                iTitleTextRectLayout.TextRect().iBr.iY ) );
            
            gc.SetBrushStyle( CGraphicsContext::ENullBrush );
            
            color=TRgb( KRgbWhite );
            iTitleTextRectLayout.DrawText( gc, *iTitleText, ETrue, color );
            }
        else
            {
            iTitleTextRectLayout.DrawText( gc, *iTitleText, ETrue, color );
            }
        }
    else
        { 
        TRAP_IGNORE( appUi->SetTitleL( *iTitleText ));     
        }
    PRINT( _L("Camera <= CCamCaptureSetupContainer::Draw"))
    }

// ----------------------------------------------------
// CCamCaptureSetupContainer::OfferKeyEventL
// ----------------------------------------------------
//
TKeyResponse CCamCaptureSetupContainer::OfferKeyEventL( 
                                                const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    if( iController.CurrentMode() == ECamControllerIdle || 
        iController.CurrentMode() == ECamControllerShutdown )
        {
        if( !iController.IsViewFinding() && iViewFinding 
            && ( IsCaptureKeyL( aKeyEvent, aType )
            || IsShutterKeyL( aKeyEvent, aType ) ) )
            {
            PRINT( _L("Camera <> CCamCaptureSetupContainer::OfferKeyEventL coming back from standby" ))
            ReserveAndStartVF();
            }
        return EKeyWasNotConsumed;
        }
    
    PRINT( _L("Camera => CCamCaptureSetupContainer::OfferKeyEventL" ))
    iController.StartIdleTimer();

    if( !iController.IsViewFinding() && iViewFinding 
       && IsCaptureKeyL( aKeyEvent, aType ) )             
        {
        // If VF was stopped by stand-by-timer, restart VF here
        ReserveAndStartVF();
        }
    
    // If the Ok button or shutter key is pressed, select the current item
    if ( ( aKeyEvent.iCode == EKeyOK && aKeyEvent.iRepeats == 0 && aType == EEventKey ) ||
         ( aType == EEventKey && 
         ( IsCaptureKeyL( aKeyEvent, aType ) || IsShutterKeyL( aKeyEvent, aType ) ) ) )  
        {
        TKeyResponse response = iCaptureSetupControl->OfferKeyEventL( aKeyEvent, aType );
        if ( aType == EEventKey && response == EKeyWasNotConsumed && aKeyEvent.iCode != EKeyOK &&
            ( ECamSettingItemDynamicPhotoFlash == iControlHandler.SettingType() ||
            ECamSettingItemDynamicSelfTimer == iControlHandler.SettingType() ) )
            {
            iView.HandleCommandL( EAknSoftkeyCancel );
            }
            else
            {
            iView.HandleCommandL( EAknSoftkeyOk );
            }
        return EKeyWasConsumed;
        }
    return iCaptureSetupControl->OfferKeyEventL( aKeyEvent, aType );
    }

// <CAMERAAPP_CAPI_V2_MIGRATION>
// ===========================================================================
// From MCamCameraObserver

// ---------------------------------------------------------------------------
// virtual HandleCameraEventL
// ---------------------------------------------------------------------------
//
void 
CCamCaptureSetupContainer::HandleCameraEventL( TInt              aStatus, 
                                               TCamCameraEventId aEventId, 
                                               TAny*             aEventData /*= NULL*/ )
  {
  if ( iViewFinding
    && ECamCameraEventVfFrameReady == aEventId
    && KErrNone                    == aStatus )
    {
    CFbsBitmap* frame = static_cast<CFbsBitmap*>( aEventData );
    if( frame )
      {
      // Draw a portion of the viewfinder
      CWindowGc& gc = SystemGc();
      gc.Activate( *DrawableWindow() );
      TRect rect = iViewFinderRectLayout.Rect();
      RWindow window = Window();
      window.Invalidate( rect );
      window.BeginRedraw( rect );
      TRect bitmapPortion = rect;
      TSize bitmapSize = frame->SizeInPixels();
      // if the bitmap is too short to offset vertically
      // display from its top edge
      if ( bitmapSize.iHeight - rect.iTl.iY < rect.Height() )
        {
        bitmapPortion.Move( 0, 0 - rect.iTl.iY );
        }    
      // if the bitmap is too narrow to offset horizontally
      // display from its left edge
      if ( bitmapSize.iWidth - rect.iTl.iX < rect.Width() )
        {
        bitmapPortion.Move( 0 - rect.iTl.iX, 0 );
        }       
      gc.BitBlt( rect.iTl, frame, bitmapPortion );

      window.EndRedraw();
      gc.Deactivate();
      }
    }
  }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::HandlePointerEventL
// ---------------------------------------------------------
//
void CCamCaptureSetupContainer::HandlePointerEventL(  const TPointerEvent& aPointerEvent ) 
	{
    
    RDebug::Print(_L("CCamCaptureSetupContainer::HandlePointerEventL iType=%d iPosition=(%d, %d)"),
        aPointerEvent.iType,
        aPointerEvent.iPosition.iX,
        aPointerEvent.iPosition.iY );
    if( !iController.IsViewFinding() && iViewFinding )
        {
        PRINT ( _L("Camera <> CCamCaptureSetupContainer::HandlePointerEventL start viewfinder") );
        ReserveAndStartVF();
        }
    else
        {
        PRINT ( _L("Camera <> CCamCaptureSetupContainer::HandlePointerEventL handle selected item") );
        iCaptureSetupControl->HandlePointerEventL( aPointerEvent );
        }
    
    /*if( iController.CurrentMode() == ECamControllerIdle || 
        iController.CurrentMode() == ECamControllerShutdown )
        {
        return;
        }

    if( !iController.IsViewFinding() && iViewFinding )
        {
        // First tap just starts viewfinder
        iController.StartViewFinder();
        iController.StartIdleTimer();
        return;
        }
    
    if ( iControlHandler.ControlIsListBox() && iCaptureSetupControl )
    	{
    	
        TInt oldListItemIndex = -1;
        TInt newListItemIndex = -1;
        TBool handleItemActivation = EFalse;
    	
        CCamCaptureSetupListBox* listbox = static_cast<CCamCaptureSetupListBox*>(iCaptureSetupControl);
        
    	if ( listbox )
    		{
    	  	oldListItemIndex = listbox->CurrentItemIndex();
    		}
    	
    	CCoeControl::HandlePointerEventL( aPointerEvent ); 
    	
    	if ( listbox )
    		{
    		newListItemIndex = listbox->CurrentItemIndex();
    		}
    	    
    	if ( oldListItemIndex != -1 && newListItemIndex != -1 )
    		{
    		
    	    // figure out if item was activated by touch down + release combination on same item
    	    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
    	    	{
    	    	if ( newListItemIndex != oldListItemIndex )
    	    		{
    	    		iActivateOnTouchRelease = EFalse;
    	    		}
    	    	else 
    	    		{
    	    		// set the current item to be activated on touch release
    	    		iActivateOnTouchRelease = ETrue;
    	    		}
    	    	}
    	    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
    	    	{
    	    	if ( iActivateOnTouchRelease && ( newListItemIndex == oldListItemIndex ) )
    	    		{
                    TInt pointedItemIndex;
                    TBool focusableItemPointed = listbox->View()->XYPosToItemIndex(aPointerEvent.iPosition, pointedItemIndex );
                    // check that pointer is in focusable area
                    if ( focusableItemPointed )
                        {
                        // only if list item index has not changed during event and iActivateOnTouchRelease is true
                        handleItemActivation = ETrue;
                        iActivateOnTouchRelease = EFalse;            			
                        }
                    else
                        {
                        iActivateOnTouchRelease = EFalse;            			
                        }
    	    		}
    	    	}
    	    else 
    	    	{
    	    	// aPointerEvent.iType == TPointerEvent::EDrag
    	    	
    	    	if ( newListItemIndex != oldListItemIndex )
    	    		{
    	    		// change only when drag event changes the listitem index
    	    		iActivateOnTouchRelease = EFalse;
    	    		}

    	    	}
	    
    		} // end if ( oldListItemIndex != -1 && newListItemIndex != -1 )
    	
	    if ( handleItemActivation )
	    	{
	    	iView.HandleCommandL( EAknSoftkeyOk );
	    	}
		    
    	} // end if ( iControlHandler.ControlIsListBox() && iCaptureSetupControl )
    else 
    	{
    	// make sure that base class functionality is called even if
    	// container has some other control than listbox
    	CCoeControl::HandlePointerEventL( aPointerEvent );  
    	}*/
	 //iCaptureSetupControl->HandlePointerEventL( aPointerEvent );
    }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::TouchLayout
// ---------------------------------------------------------
//
TRect CCamCaptureSetupContainer::TouchLayout()
    {
    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane,
                                       statusPaneRect );
    iLayoutAreaRect.BoundingRect( statusPaneRect );
    
    if ( iControlHandler.ControlIsListBox() )
        {        
        TAknLayoutRect listboxLayoutRect; 
        if ( iViewFinding )
            {
            listboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                      AknLayoutScalable_Apps::main_cset_listscroll_pane( 2 ) );
            }
        else 
            {
            listboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                      AknLayoutScalable_Apps::main_cset_listscroll_pane( 3 ) );
            }   
        iViewFinderRectLayout.LayoutRect( iLayoutAreaRect, 
                      AknLayoutScalable_Apps::main_cam_set_pane_g2( 2 ) );
        iTitleTextRectLayout.LayoutText( iLayoutAreaRect, 
                      AknLayoutScalable_Apps::main_cam_set_pane_t1( 3 ) );
        return listboxLayoutRect.Rect();
        } 
    else // slider
        {      
        TAknLayoutRect settingLayout;
        settingLayout.LayoutRect( iLayoutAreaRect, 
                          AknLayoutScalable_Apps::main_cam_set_pane( 0 ) );
        TRect settingRect( settingLayout.Rect() );
        AknLayoutUtils::LayoutControl( iCaptureSetupControl, settingRect, 
                          AknLayoutScalable_Apps::main_cset_slider_pane( 1 ) );
    
        iViewFinderRectLayout.LayoutRect( iLayoutAreaRect, 
                          AknLayoutScalable_Apps::main_cam_set_pane_g2( 3 ) );
        iTitleTextRectLayout.LayoutText( iLayoutAreaRect, 
                          AknLayoutScalable_Apps::main_cam_set_pane_t1( 5 ) ); 
        return settingRect;
        }
    }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::NonTouchLayout
// ---------------------------------------------------------
//
TRect CCamCaptureSetupContainer::NonTouchLayout() 
    {                                 
    // Non-touch has a visible title & status panes
    TRect titlePaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, 
                                       titlePaneRect );
    iLayoutAreaRect.Move( 0, -titlePaneRect.Height() );

    if ( iControlHandler.ControlIsListBox() )
        {        
        TAknLayoutRect listboxLayoutRect; 
        if ( iViewFinding )
            {
            listboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                     AknLayoutScalable_Apps::main_cset6_listscroll_pane( 2 ) );
            }
        else 
            {
            listboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                     AknLayoutScalable_Apps::main_cset6_listscroll_pane( 3 ) );
            }   
        iViewFinderRectLayout.LayoutRect( iLayoutAreaRect, 
                      AknLayoutScalable_Apps::main_cam6_set_pane_g2( 2 ) );
        return listboxLayoutRect.Rect();
        } 
    else // slider
        {      
        TAknLayoutRect settingLayout;
        settingLayout.LayoutRect( iLayoutAreaRect, 
                         AknLayoutScalable_Apps::main_cam6_set_pane() );
        TRect settingRect( settingLayout.Rect() );
        AknLayoutUtils::LayoutControl( iCaptureSetupControl, settingRect, 
                         AknLayoutScalable_Apps::main_cset6_slider_pane( 1 ) );
        iViewFinderRectLayout.LayoutRect( iLayoutAreaRect,  
                         AknLayoutScalable_Apps::main_cam6_set_pane_g2( 3 ) );
        return settingRect;
        }
    }

// ----------------------------------------------------
// CCamContainerBase::HandleForegroundEvent
// Called to notify of change of app fore/background state
// ----------------------------------------------------
//
void CCamCaptureSetupContainer::HandleForegroundEventL( TBool aForeground )
    {
    
    if ( aForeground )
        {
        DrawDeferred();
        }
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupContainer::HandleListBoxEventL
// Handles Listbox events
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupContainer::HandleListBoxEventL( CEikListBox* aListBox, 
                                                    TListBoxEvent aEventType )
    {
    if( iController.CurrentMode() == ECamControllerIdle || 
        iController.CurrentMode() == ECamControllerShutdown )
        {
        return;
        }

    if( !iController.IsViewFinding() && iViewFinding )
        {
        PRINT( _L("Camera <> CCamCaptureSetupContainer::HandleListBoxEventL start viewfinder") );
        ReserveAndStartVF();
        return;
        }
    
    switch( aEventType )
        {
        
        case EEventItemDoubleClicked:
              {
              iView.HandleCommandL(EAknSoftkeyOk);
              }
              break;  
        
        case EEventItemClicked:
        case EEventPenDownOnItem: // Fall through
            {
            MCamListboxModel* model = static_cast<MCamListboxModel*>( aListBox->Model() );
            iControlHandler.HandleSettingValueUpdateL( model->ItemValue( aListBox->CurrentItemIndex() ) ); 
            }
            break;
        case EEventItemSingleClicked:
            {
            MCamListboxModel* model = static_cast<MCamListboxModel*>( aListBox->Model() ); //Warning: downcasting
            iControlHandler.HandleSettingValueUpdateL( model->ItemValue( aListBox->CurrentItemIndex() ) ); 
            iView.HandleCommandL(EAknSoftkeyOk);
            }
            break;
        default:
            break;
        }
    }
/*
// ---------------------------------------------------------
// CCamCaptureSetupContainer::ShowViewFinderFrame
// Receives new viewfinder frame when one is available
// ---------------------------------------------------------
//
void CCamCaptureSetupContainer::ShowViewFinderFrame( const CFbsBitmap* aFrame, TBool /aFrozen/ )
    {
    // If a bitmap pointer was passed
    if ( aFrame && iViewFinding )
        {
        // draw a portion of the viewfinder
        // temporary values used, waiting for specification        
        CWindowGc& gc = SystemGc();
        gc.Activate( *DrawableWindow() );
        TRect rect = iViewFinderRectLayout.Rect();
        RWindow window = Window();
        window.Invalidate( rect );
        window.BeginRedraw( rect );
        TRect bitmapPortion = rect;
        TSize bitmapSize = aFrame->SizeInPixels();
        // if the bitmap is too short to offset vertically
        // display from its top edge
        if ( bitmapSize.iHeight - rect.iTl.iY < rect.Height() )
            {
            bitmapPortion.Move( 0, 0 - rect.iTl.iY );
            }    
        // if the bitmap is too narrow to offset horizontally
        // display from its left edge
        if ( bitmapSize.iWidth - rect.iTl.iX < rect.Width() )
            {
            bitmapPortion.Move( 0 - rect.iTl.iX, 0 );
            }       
        gc.BitBlt( rect.iTl, aFrame, bitmapPortion );
        window.EndRedraw();
        gc.Deactivate();
        }
    else // otherwise a null bitmap pointer was passed
        {

        }
    }
*/

// </CAMERAAPP_CAPI_V2_MIGRATION>

// End of File  
