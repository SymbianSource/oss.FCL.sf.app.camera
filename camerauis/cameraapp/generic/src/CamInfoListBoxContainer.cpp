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
* Description:  Setting page for Image/Video quality.*
*/

#include "CamInfoListBoxContainer.h"
#include "CamUtility.h"
#include <AknBidiTextUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <barsread.h>    // resource reader
#include <aknview.h> 
#include <aknlayoutscalable_apps.cdl.h>
#include <touchfeedback.h> 
#include <akntoolbar.h>


#include "CamAppUi.h"

#include "CamInfoListBox.h"
#include "CamInfoListBoxModel.h"
#include "CamPanic.h"
#include <AknInfoPopupNoteController.h>  // CAknInfoPopupNoteController

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

// CONSTANTS
const TInt KInfoTooltipDelay = 0; // time (milliseconds) delay when showing the tooltip
const TInt KInfoTooltipDisplayTime = 5000; // maximum time (milliseconds) the tooltip is displayed
const TInt KExplTxtGranularity = 6;
const TUint32 KToolbarExtensionBgColor = 0x00000000;
const TInt KToolBarExtensionBgAlpha = 0x7F;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamInfoListBoxContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamInfoListBoxContainer* CCamInfoListBoxContainer::NewL( const TRect& aRect, 
                                                            CAknView& aView,                                                            
                                                            CCamAppController& aController,
                                                            TInt aListBoxResource,
                                                            TInt aSummaryResource,
                                                            TInt aSettingValue,
                                                            TInt aTitleResource,
                                                            TBool aSkinnedBackGround )
    {
    CCamInfoListBoxContainer* self = 
                  new( ELeave ) CCamInfoListBoxContainer(  aController,
                                                           aView,
                                                           aSettingValue,
                                                           aSkinnedBackGround );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aListBoxResource, aSummaryResource, aTitleResource);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamInfoListBoxContainer::~CCamInfoListBoxContainer
// Destructor
// ---------------------------------------------------------------------------
//
CCamInfoListBoxContainer::~CCamInfoListBoxContainer()
  {
    PRINT( _L( "Camera => ~CCamInfoListBoxContainer" ) );

    iSummaryBitmapArray.ResetAndDestroy();
    iTitleArray.ResetAndDestroy();
    iDescArray.ResetAndDestroy();
    iExplTxtLinesLayout.Close();  

    delete iListBox;

    if ( iTooltipController )
        {
        delete iTooltipController;
        iTooltipController = NULL;
        }
    
    if ( iListboxTitle )
        {
        delete iListboxTitle;
        iListboxTitle = NULL;
        }

    if ( iCurrentDescLineArray ) 
        {
        iCurrentDescLineArray->Reset();
        delete iCurrentDescLineArray;
        }
        	
	PRINT( _L( "Camera <= ~CCamInfoListBoxContainer" ) );    
  }
  
// ---------------------------------------------------------
// CCamInfoListBoxContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamInfoListBoxContainer::ConstructL( const TRect& aRect, TInt aListBoxResource, TInt aSummaryResource, TInt aTitleResource )
    {
	PRINT(_L("Camera => CCamInfoListBoxContainer::ConstructL") )
    
    CCamContainerBase::BaseConstructL( aRect );

    if ( iController.IsTouchScreenSupported() )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
        CAknToolbar* fixedToolbar = appUi->CurrentFixedToolbar();
        if ( fixedToolbar )
            {
            // Hide toolbar from Light sensitivity etc. settings view 
            fixedToolbar->SetToolbarVisibility( EFalse );
            }
        }

    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       iLayoutAreaRect );
    
    // layout area rectangle contains the area, where components need to be 
    // drawn to. the container size is the whole screen, but the layouts are 
    // for the client area. aRect is the container size that might include or
    // might not include statuspane area. calculating area self will
    // go around the problem

    // Construct the listbox
    iListBox =  new( ELeave ) CCamInfoListBox( this, iController );                           
    iListBox->ConstructL( iController, this, iTitleArray, iDescArray, aListBoxResource,
            iSkinnedBackGround );
    iListBox->DisableSingleClick( ETrue );

    // Highlight the currently active setting value
    iListBox->InitializeL( iSettingValue );

    iListBox->SetContainerWindowL( *this );
    iListBox->CreateScrollBarFrameL( ETrue );
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() ); 
    TRect listboxRect; 
    if ( AknLayoutUtils::PenEnabled() ) 
        {
        listboxRect = TouchLayout();
        }
    else
        {
        iCurrentDescLineArray  = 
                     new( ELeave ) CArrayFixFlat<TPtrC>( KExplTxtGranularity ); 
        appUi->StatusPane()->MakeVisible( ETrue ); 
        SetRect( appUi->ClientRect() );
        listboxRect = NonTouchLayout();
        }
    ConstructSummaryDetailsFromResourceL( aSummaryResource );   

    // Determine the height of the listbox; 
    // Height needs to be based on the number of items, because the layout
    // height (maxListBoxHeight )might be different 
    // than combined height of the max visible listbox items
    
    CEikScrollBarFrame::TScrollBarVisibility visibility = 
                                                      CEikScrollBarFrame::EOff;
    TInt listBoxHeight = iListBox->CalcHeightBasedOnNumOfItems( 
                                          iListBox->Model()->NumberOfItems() );
    if ( listboxRect.Height() < listBoxHeight )
        {
        // there are more items than would fit to the listbox rectangle
        
        // the maximum height is the total height of items 
        // that can fit to the maxlistboxheight
        TInt itemHeight = iListBox->ItemHeight();
        TInt itemsVisible = listboxRect.Height() / itemHeight;
        listBoxHeight = itemsVisible * itemHeight;
        
        // the scrollbar needs to be turned on
        visibility = CEikScrollBarFrame::EOn;
        }  
    
    // calculate the layout for the listbox with changed height
    const AknLayoutUtils::SAknLayoutControl listboxLayout =
    {   ELayoutEmpty, listboxRect.iTl.iX, listboxRect.iTl.iY, 
        ELayoutEmpty, ELayoutEmpty, 
        iListboxLayoutRect.Rect().Width(), listBoxHeight };
    
    AknLayoutUtils::LayoutControl( iListBox, iLayoutAreaRect, listboxLayout);

    // set scrollbars for the listbox
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                                         visibility );
    iListBox->MakeVisible( ETrue );
    iListBox->SetFocus( EFalse, ENoDrawNow );
    iListBox->UpdateScrollBarsL();
    iListBox->ScrollBarFrame()->DrawScrollBarsNow();
    iListBox->SetListBoxObserver(this); 
    appUi->SetActivePaletteVisibility( EFalse );

    // Prevents the system filling invalidated areas with the background colour - 
    // helps reduce flicker.
    Window().SetBackgroundColor();
       
    ConstructContainerTitleFromResourceL( aTitleResource ); 
       
    if ( !iTooltipController )
        {
        iTooltipController = CAknInfoPopupNoteController::NewL();
        }
        
    iTooltipController->SetTimeDelayBeforeShow( KInfoTooltipDelay );
    iTooltipController->SetTimePopupInView( KInfoTooltipDisplayTime );
    
    // Position the tooltip
    TPoint position;
    if ( AknsUtils::GetControlPosition( this, position ) != KErrNone )
        {
        position = PositionRelativeToScreen();
        }
    TRect rect( position, Size() );
    TPoint center = rect.Center();
    iTooltipController->SetPositionAndAlignment( TPoint( center.iX, rect.iTl.iY ), EHLeftVBottom );
     
    iTooltipController->SetTimeDelayBeforeShow( KInfoTooltipDelay );
    iTooltipController->SetTimePopupInView( KInfoTooltipDisplayTime );
        
    iController.SetViewfinderWindowHandle( &Window() );
    PRINT(_L("Camera <= CCamInfoListBoxContainer::ConstructL") )    
    }
   
// ---------------------------------------------------------------------------
// CCamInfoListBoxContainer::CCamInfoListBoxContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamInfoListBoxContainer::CCamInfoListBoxContainer(     CCamAppController& aController,
                                                        CAknView& aView,
                                                        TInt aSettingValue,
                                                        TBool aSkinnedBackGround )
: CCamContainerBase( aController, aView ), iSettingValue( aSettingValue ),
iTooltipController(NULL),
iShowTooltip(EFalse),
iTooltipIndex(-1),
iActivateOnTouchRelease(EFalse),
iSkinnedBackGround(aSkinnedBackGround)

    {
    }

     
// -----------------------------------------------------------------------------
// CCamInfoListBoxContainer::CurrentSettingItemValue
// Returns the Current item's setting value ID
// -----------------------------------------------------------------------------
TInt CCamInfoListBoxContainer::CurrentSettingItemValue() const
    {
    return static_cast<MCamListboxModel*>(iListBox->Model())->ItemValue(iListBox->CurrentItemIndex());
    }
    
    
TBool CCamInfoListBoxContainer::SettingValueChanged() const
		{
		return CurrentSettingItemValue() != iSettingValue;
		}    
    
// -----------------------------------------------------------------------------
// CCamInfoListBoxContainer::IndexForValueId() const
// Returns the index of the item in the array with the specified ValueId
// -----------------------------------------------------------------------------   
TInt 
CCamInfoListBoxContainer::IndexForValueId( TInt aValueId ) const
    {
    return static_cast<MCamListboxModel*>( iListBox->Model() )->ValueIndex( aValueId );
    } 

// -----------------------------------------------------------------------------
// CCamInfoListBoxContainer::HandleSettingValueUpdateL
// Handles a change to the setting value of the slider
// -----------------------------------------------------------------------------
//
void CCamInfoListBoxContainer::HandleSettingValueUpdateL( TInt aNewValue )
    {
    iController.PreviewSettingChangeL( ECamSettingItemDynamicPhotoLightSensitivity, aNewValue );
    }
    
// ---------------------------------------------------------
// CCamInfoListBoxContainer::CountComponentControls 
// Returns the number of controls owned
// ---------------------------------------------------------
//
TInt CCamInfoListBoxContainer::CountComponentControls() const
    {
    return CCamContainerBase::CountComponentControls() + 1;
    }

// ---------------------------------------------------------
// CCamInfoListBoxContainer::ComponentControl
// Returns the requested component control
// ---------------------------------------------------------
//
CCoeControl* CCamInfoListBoxContainer::ComponentControl( TInt aIndex ) const
    {
    CCoeControl* control = CCamContainerBase::ComponentControl( aIndex );
    if( control == NULL)
        {
        control = iListBox;
        }
    return control;
    }

// ---------------------------------------------------------
// CCamInfoListBoxContainer::Draw
// Draw control
// ----------------------------------------------------------
//
void CCamInfoListBoxContainer::Draw( const TRect& aRect ) const
    {
	PRINT(_L("Camera => CCamInfoListBoxContainer::Draw") )

	CWindowGc& gc = SystemGc();
    if ( AknLayoutUtils::PenEnabled() )
        {
        TRgb color;
        if( iSkinnedBackGround )
            {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsDrawUtils::Background( skin, iBgContext, gc, aRect );
        // draw the title text   
        AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                                                EAknsCIQsnTextColorsCG6 );
            }
        else
            {
            // Fill control with transparency bg colour
            gc.SetPenStyle( CGraphicsContext::ENullPen );
            gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
            color = TRgb( 0,0 );
            gc.SetBrushColor( color );
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.DrawRect( aRect );
            gc.SetBrushColor( color );
            gc.DrawRect( iTitleTextRectLayout.TextRect() );            
            // Reset the brush after use (otherwise anything drawn
            // after the viewfinder will also show viewfinder frames)

            gc.SetBrushColor( TRgb( KToolbarExtensionBgColor, KToolBarExtensionBgAlpha ) );
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.DrawRect( TRect( iLayoutAreaRect.iTl.iX,
                                iLayoutAreaRect.iTl.iY,
                                iLayoutAreaRect.iBr.iX,
                                iTitleTextRectLayout.TextRect().iBr.iY ) );
            
            gc.SetBrushStyle( CGraphicsContext::ENullBrush );
            

            CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );            
            appUi->StatusPane()->MakeVisible( EFalse );
            
            color = TRgb( KRgbWhite );
            }
        iTitleTextRectLayout.DrawText( gc, *iListboxTitle, ETrue, color );        
        }
    else
        { 
        TRAP_IGNORE( DrawSummaryTextL( gc ) ); 
        }
    
	PRINT(_L("Camera <= CCamInfoListBoxContainer::Draw") )
    }

// ----------------------------------------------------------------
// CCamInfoListBoxContainer::OfferKeyEventL
// Handles this application view's command keys. Forwards other
// keys to child control(s).
// ----------------------------------------------------------------
//
TKeyResponse CCamInfoListBoxContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if( iController.CurrentMode() == ECamControllerIdle || 
        iController.CurrentMode() == ECamControllerShutdown )
        {
        if( !iController.IsViewFinding() && !iSkinnedBackGround
            && ( IsCaptureKeyL( aKeyEvent, aType ) 
            || IsShutterKeyL( aKeyEvent, aType ) ) )
            {
            PRINT( _L("Camera <> CCamInfoListBoxContainer::OfferKeyEventL coming back from standby" ))
            ReserveAndStartVF();
            }
        return EKeyWasNotConsumed;
        }
    iController.StartIdleTimer();
    
    // If the Ok button or shutter key is pressed, select the current item
    if ( ( aKeyEvent.iCode == EKeyOK && aKeyEvent.iRepeats == 0 && aType == EEventKey ) ||
         ( aType == EEventKey && 
         ( IsCaptureKeyL( aKeyEvent, aType ) || IsShutterKeyL( aKeyEvent, aType ) ) ) )
        {
        TKeyResponse response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        iView.HandleCommandL( EAknSoftkeySelect );
        return response;
        }
    
    TKeyResponse returnvalue = iListBox->OfferKeyEventL( aKeyEvent, aType );

    if ( EStdKeyUpArrow == aKeyEvent.iScanCode ||
           EStdKeyDownArrow == aKeyEvent.iScanCode )
        {
        ShowTooltipL();
        }
    else // No tooltip
        {
        DrawDeferred(); // Update explanation text
        }
        
    return returnvalue;
    }

// -----------------------------------------------------------------
// CCamInfoListBoxContainer::ConstructSummaryDetailsFromResourceL
// Sets up the layouts of the summary items
// -----------------------------------------------------------------
void CCamInfoListBoxContainer::ConstructSummaryDetailsFromResourceL(TInt aResourceId)
    {
     // Find the name and path of the MBM file for bitmaps
    TFileName mbmFileName;
    CamUtility::ResourceFileName( mbmFileName );
     
    TResourceReader reader;                                                                                     
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId ); 
    const TInt count = reader.ReadInt16();

    TInt i;
    TInt bitmapId;

    // Read all of the summary entries from the resource file
    for ( i = 0; i < count; i++ )
        {
        reader.ReadInt16(); // The current item enum is not needed
        bitmapId = reader.ReadInt32();
        User::LeaveIfError( iSummaryBitmapArray.Append( AknIconUtils::CreateIconL( mbmFileName, bitmapId ) ) );
       
        User::LeaveIfError( iTitleArray.Append( reader.ReadHBufC16L() ) );
        User::LeaveIfError( iDescArray.Append( reader.ReadHBufC16L() ) );        
        } 
           
    CleanupStack::PopAndDestroy();         
    }


// --------------------------------------------------------------------------
// CCamInfoListBoxContainer::HandlePointerEventL
// --------------------------------------------------------------------------
//    
void CCamInfoListBoxContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent ) 
    {
    
    RDebug::Print(_L("CCamInfoListBoxContainer::HandlePointerEventL iType=%d iPosition=(%d, %d)"),
            aPointerEvent.iType,
            aPointerEvent.iPosition.iX,
            aPointerEvent.iPosition.iY );
  
    if( !iController.IsViewFinding() && !iSkinnedBackGround )
        {
        PRINT ( _L("Camera <> CCamInfoListBoxContainer::HandlePointerEventL  start viewfinder") );
        ReserveAndStartVF();
        }
    else
        {
        PRINT ( _L("Camera <> CCamInfoListBoxContainer::HandlePointerEventL  handle selected item") );
        iListBox->HandlePointerEventL(aPointerEvent);
        }

    /*TInt oldListItemIndex = -1;
    TInt newListItemIndex = -1;
    TBool handleItemActivation = EFalse;
   
    if ( iListBox )
        {
        oldListItemIndex = iListBox->CurrentItemIndex();
        }
   
    // send to coecontrol -> will inform controls in this container about pointer events
    // (listbox will handle focus changes and scrolling)
    CCoeControl::HandlePointerEventL( aPointerEvent );       
       
    if ( iListBox )
        {
        newListItemIndex = iListBox->CurrentItemIndex();
        }
       
    // figure out if item was activated by touch down + release combination on same item
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {

        if ( iListBox->Rect().Contains( aPointerEvent.iPosition )  ) 
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
             
            // show tooltip always when touch event down happens inside listbox
            ShowTooltipL();
            }
        else
            {
            iActivateOnTouchRelease = EFalse;
            }
        
        
        }
    else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        if ( iActivateOnTouchRelease && ( newListItemIndex == oldListItemIndex ) )
            {
            // only if list item index has not changed during event 
            // and iActivateOnTouchRelease is true
            TInt pointedItemIndex = 0;
            TBool focusableItemPointed = 
                iListBox->View()->XYPosToItemIndex( aPointerEvent.iPosition, 
                                                    pointedItemIndex );
            // check that pointer is in focusable area
            if ( focusableItemPointed )
                {
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

            // with drag always update the tooltip
            ShowTooltipL();
            }

        }


    if ( handleItemActivation )
        {
        iView.HandleCommandL( EAknSoftkeySelect ); 
        }*/
   }
            
// --------------------------------------------------------------------------
// CCamInfoListBoxContainer::HandleListBoxEventL
//  Handles Listbox events
// --------------------------------------------------------------------------
// 

void CCamInfoListBoxContainer::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventItemDoubleClicked:
              {
              iView.HandleCommandL( EAknSoftkeySelect ); 
              }
              break;  
        
        case EEventPenDownOnItem:
            {            
            ShowTooltipL();
            }
            break;
                    
        case EEventItemClicked:
        case EEventItemSingleClicked:
            {
            TInt settingValue = CurrentSettingItemValue();

            if ( iView.Id().iUid == ECamViewIdPhotoUserSceneSetup )
                {                   
                iController.PreviewSettingChangeL( ECamSettingItemUserSceneLightSensitivity, settingValue );
                }
     	    else
       	        {
		        iController.PreviewSettingChangeL( ECamSettingItemDynamicPhotoLightSensitivity, settingValue );
          	    }
            }
            break;
        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CCamInfoListBoxContainer::ShowTooltipL
// --------------------------------------------------------------------------
//
void CCamInfoListBoxContainer::ShowTooltipL()
    {
    
    if ( !iTooltipController || ( iDescArray[iListBox->CurrentItemIndex()]->Length() <= 0 ) )
           {
           return;
           }
       
    if ( iTooltipIndex != iListBox->CurrentItemIndex() )
        {
        // make sure that possible already open tooltip is hidden
        iTooltipController->HideInfoPopupNote();

        iTooltipController->SetTextL( *iDescArray[iListBox->CurrentItemIndex()] );     
        TRect hl = iListBox->HighlightRect();
        TPoint tt = hl.Center();
        if ( AknLayoutUtils::LayoutMirrored() )
            {
            tt.iX -= hl.Width();
            }
        iTooltipController->SetPositionAndAlignment( tt, EHLeftVBottom ); 
        
        iTooltipIndex = iListBox->CurrentItemIndex();
        }
    

    iTooltipController->ShowInfoPopupNote(); 
    
    }
    
 
// --------------------------------------------------------------------------
// CCamInfoListBoxContainer::ConstructContainerTitleFromResourceL
// --------------------------------------------------------------------------
//
void CCamInfoListBoxContainer::ConstructContainerTitleFromResourceL( TInt aResourceId )
    {
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId ); // cleanupstack
    
    iListboxTitle = reader.ReadHBufC16L();    
    
    CleanupStack::PopAndDestroy(); // reader
    }


// -----------------------------------------------------------------
// CCamInfoListBoxContainer::DrawSummaryText (ISO setting)
// Draws summary title and description text
// -----------------------------------------------------------------

void CCamInfoListBoxContainer::DrawSummaryTextL( CWindowGc& aGc ) const
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    appUi->SetTitleL( *iListboxTitle );     
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background( skin, iBgContext, aGc, Rect() );
  
    // Draw summary frame
    TRect explTxtRect = iExplanationRect;
    TInt border = iExplIconLayout.Rect().iTl.iX - iExplanationRect.iTl.iX;
    explTxtRect.Shrink( border/2 , border/2 );
    AknsDrawUtils::DrawFrame( skin, aGc, iExplanationRect, explTxtRect,
                              KAknsIIDQsnFrInput, KAknsIIDNone );   //frame
    
    AknIconUtils::SetSize( iSummaryBitmapArray[iListBox->CurrentItemIndex()], 
                           iExplIconLayout.Rect().Size() ); 
    iExplIconLayout.DrawImage( aGc, 
                   iSummaryBitmapArray[ iListBox->CurrentItemIndex() ], NULL );
    
    TRgb explTitleTxtColor; // same as list highlight text
    TRgb explTxtColor;      // same as main area text
    AknsUtils::GetCachedColor( skin, explTitleTxtColor, KAknsIIDQsnTextColors,
                               EAknsCIQsnTextColorsCG10 );
    AknsUtils::GetCachedColor( skin, explTxtColor, KAknsIIDQsnTextColors, 
                               EAknsCIQsnTextColorsCG6 );
    iExplTitleLayout.DrawText( aGc, 
                            iTitleArray[ iListBox->CurrentItemIndex() ]->Des(), 
                            ETrue, explTitleTxtColor ); 
    AknTextUtils::WrapToArrayL( *iDescArray[iListBox->CurrentItemIndex()],
                                iExplTxtLinesLayout[0].TextRect().Width(),
                                *iExplTxtLinesLayout[0].Font(), 
                                *iCurrentDescLineArray );
    TInt lCount = Min( iExplLineCount, iCurrentDescLineArray->Count() );                               
    for ( TInt i = 0; i < lCount; i++ )  
        {
        iExplTxtLinesLayout[i].DrawText( aGc, (*iCurrentDescLineArray)[i], 
                                         ETrue, explTxtColor ); 
        }     
    }

// --------------------------------------------------------------------------
// CCamInfoListBoxContainer::TouchLayout
// --------------------------------------------------------------------------
//
TRect CCamInfoListBoxContainer::TouchLayout()  
    {
    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, 
                                       statusPaneRect );
    iLayoutAreaRect.BoundingRect( statusPaneRect );
    // get the rect size for listbox in the layout
    TAknLayoutRect wholeListboxLayoutRect;
    wholeListboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                      AknLayoutScalable_Apps::main_cset_listscroll_pane( 3 ) );   
    TInt listBoxHeight = iListBox->CalcHeightBasedOnNumOfItems( 
                                          iListBox->Model()->NumberOfItems() );
    // get layout for the listbox rect
    if ( wholeListboxLayoutRect.Rect().Height() < listBoxHeight )
        {
        // if scrollbars are used, use scrollbar layout
        iListboxLayoutRect.LayoutRect( wholeListboxLayoutRect.Rect(), 
                            AknLayoutScalable_Apps::main_cset_list_pane( 0 ) );
        }
    else
        {
        iListboxLayoutRect.LayoutRect( iLayoutAreaRect, 
              AknLayoutScalable_Apps::main_cset_listscroll_pane( 2 ) ); //Same as WB
        }
   
    iTitleTextRectLayout.LayoutText( iLayoutAreaRect, 
                           AknLayoutScalable_Apps::main_cam_set_pane_t1( 3 ) );
    
    return iListboxLayoutRect.Rect();
    }

// --------------------------------------------------------------------------
// CCamInfoListBoxContainer::NonTouchLayout
// --------------------------------------------------------------------------
//
TRect CCamInfoListBoxContainer::NonTouchLayout()
    {
    TAknLayoutRect wholeListboxLayoutRect;
    wholeListboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                     AknLayoutScalable_Apps::main_cset6_listscroll_pane( 3 ) );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect( iLayoutAreaRect, //1
                     AknLayoutScalable_Apps::main_cset6_listscroll_pane( 4 ) );
    TInt listBoxHeight = iListBox->CalcHeightBasedOnNumOfItems( 
                                          iListBox->Model()->NumberOfItems() );
    if ( wholeListboxLayoutRect.Rect().Height() < listBoxHeight )
        {
        // if scrollbars are used, use scrollbar layout
        iListboxLayoutRect.LayoutRect( listLayoutRect.Rect(),
                      AknLayoutScalable_Apps::main_cset_list_pane_copy1( 0 ) );
        AknLayoutUtils::LayoutVerticalScrollBar( iListBox->ScrollBarFrame(), 
                        listLayoutRect.Rect(),
                        AknLayoutScalable_Apps::scroll_pane_cp028_copy1( 0 ) );
        }
    else
        {
        iListboxLayoutRect.LayoutRect( listLayoutRect.Rect(), 
                      AknLayoutScalable_Apps::main_cset_list_pane_copy1( 1 ) );  
        }
    
    // Explanation ikon, title, and text layout
    TAknLayoutRect explParent;
    explParent.LayoutRect( iLayoutAreaRect,//2
                          AknLayoutScalable_Apps::main_cset6_text2_pane( 0 ) );
    TAknLayoutRect explLayoutRect;
    explLayoutRect.LayoutRect( explParent.Rect(), 
              AknLayoutScalable_Apps::bg_popup_preview_window_pane_cp03( 0 ) );
    iExplanationRect = explLayoutRect.Rect();

    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane, 
                                       statusPaneRect );
    iExplanationRect.Move( 0, -statusPaneRect.Height() );
    
    iExplIconLayout.LayoutRect( iExplanationRect, 
            AknLayoutScalable_Apps::main_cset6_text2_pane_g1( 0 ) );  
    iExplTitleLayout.LayoutText( iExplanationRect, 
            AknLayoutScalable_Apps::main_cset6_text2_pane_t1( 0 ) ); 
    iExplTitleLayout.LayoutText( iExplanationRect, 
            AknLayoutScalable_Apps::main_cset6_text2_pane_t1( 0 ) ); 

    TAknLayoutRect explTxtParent;
    explTxtParent.LayoutRect( iExplanationRect,
                             AknLayoutScalable_Apps::list_cset_text2_pane(0) );
    TAknLayoutScalableParameterLimits listLimits = 
              AknLayoutScalable_Apps::list_cset_text2_pane_t1_ParamLimits( 0 );
    iExplLineCount = listLimits.LastRow() + 1;
    for ( TInt i = 0; i < iExplLineCount; i++ )
        {
        TAknLayoutText layoutText;
        layoutText.LayoutText( explTxtParent.Rect(), 
                  AknLayoutScalable_Apps::list_cset_text2_pane_t1( 0, 0, i ) );
        (void) iExplTxtLinesLayout.Append( layoutText ); // Errors are ignored
        } 
    
    TRect ret = wholeListboxLayoutRect.Rect();
    // Non-touch has a visible title & status panes
    TRect titlePaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, 
                                       titlePaneRect );
    ret.Move( 0, -titlePaneRect.Height() - statusPaneRect.Height() );
    return ret;
    }
// End of File  
