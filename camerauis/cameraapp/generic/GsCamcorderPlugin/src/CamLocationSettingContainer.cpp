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
* Description:  Implements the location settings for image settings*
*/



// INCLUDE FILES

#include <gscamerapluginrsc.rsg>
#include <barsread.h>
#include <AknQueryValue.h>
#include <AknIconUtils.h>
#include <akntitle.h>  // CAknTitlePane
#include <aknappui.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <StringLoader.h>
#include <cameraapp.mbg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <touchfeedback.h> 

#include "CamLocationSettingContainer.h"
#include "CamUtility.h"
#include "CamCaptureSetupListBox.h"
#include "CamCaptureSetupListBoxModel.h"
#include "CamPanic.h"
#include "camuiconstants.h"
#include "camuidconstants.h"
#include "CamAppUiBase.h"
#include "CamStaticSettingsModel.h"
#include "GSCamcorderPlugin.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::CCamLocationSettingContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamLocationSettingContainer::CCamLocationSettingContainer( MCamAppController& aController,
                                                            TCamCameraMode aMode,
                                                            CCoeControl* aParent )
:   iParent( aParent ),
    iController( aController ), 
    iMode( aMode ),
    iActivateOnTouchRelease( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamLocationSettingContainer::ConstructL()
    {
    PRINT(_L("Camera => CCamLocationSettingContainer::ConstructL") )
	PRINT(_L("Camera <= CCamLocationSettingContainer::ConstructL") )
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamLocationSettingContainer* 
CCamLocationSettingContainer::NewL( MCamAppController& aController,
                                    TCamCameraMode     aMode,
                                    CCoeControl*       aParent )
    {
    CCamLocationSettingContainer* self = 
        new( ELeave ) CCamLocationSettingContainer( aController, aMode, aParent );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

    
// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::~CCamLocationSettingContainer
// Destructor
// -----------------------------------------------------------------------------
//
CCamLocationSettingContainer::~CCamLocationSettingContainer()
  {
  PRINT( _L("Camera => ~CCamLocationSettingContainer") );
  CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
  if ( appUi )
    {
    TInt resId = ( ECamControllerVideo == iMode )
               ? R_CAM_VIDEO_SETTINGS_TITLE_NAME
               : R_CAM_PHOTO_SETTINGS_TITLE_NAME;
    
    TRAP_IGNORE( appUi->SetTitleL( resId ) );
    }
    iDescArray.ResetAndDestroy();
    iDescLayoutArray.Close();
    if ( iCurrentDescLineArray )
        {
        iCurrentDescLineArray->Reset();
        delete iCurrentDescLineArray;
        } 
    delete iListBox;
    
    if ( iTextTitle )
        {
        delete iTextTitle;
        iTextTitle = NULL;
        }
    
  PRINT( _L("Camera <= ~CCamLocationSettingContainer") );
  }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::SelectedQuality
// Returns the value selected by the listbox
// -----------------------------------------------------------------------------
//
TInt CCamLocationSettingContainer::SelectedValue() const
    {
    CCamCaptureSetupListBoxModel *model = static_cast<CCamCaptureSetupListBoxModel *>( iListBox->Model() );
    return model->ItemValue( iListBox->CurrentItemIndex() ); 
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::ConstructFromResourceL
// Constructs the container from the resource
// -----------------------------------------------------------------------------
//
void CCamLocationSettingContainer::ConstructFromResourceL( TResourceReader& /*aReader*/ )
    {
    CAknViewAppUi* viewAppUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
    CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
                               ( viewAppUi->View( KGSCamcorderGSPluginUid ) );

   // set up the container windows here as setting page window
    // isn't created until CAknSettingPage::ConstructFromResourceL is called
    SetContainerWindowL( *iParent );
    
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
    
    // set the layouts for the rect containing the description
    if ( AknLayoutUtils::PenEnabled() ) 
        {
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            iSummaryRect.LayoutRect( layoutAreaRect, AknLayoutScalable_Apps::main_cset_text2_pane( 0 ) );
            }
        else
            {
            iSummaryRect.LayoutRect( layoutAreaRect, AknLayoutScalable_Apps::main_cset_text2_pane( 1 ) );
            }
        }
    else
        {
        iSummaryRect.LayoutRect( layoutAreaRect, AknLayoutScalable_Apps::main_cset6_text2_pane( 0 ) );
        }
    
	TRAPD( ignore, 
	       ConstructSummaryDetailsFromResourceL( R_CAM_OFF_ON_LOC_TEXT_LIST_SUMMARY ); 
	     )
    if ( ignore )
        { 
        // Do nothing ( removes build warning )
        }
 
	iCurrentDescLineArray  = new( ELeave ) CArrayFixFlat<TPtrC>( 5 );
	
    // set layout for 5 lines of descriptive text
    iDescLayoutArray.Reset();
    TAknLayoutText layout;
    TAknLayoutScalableParameterLimits listLimits = 
        AknLayoutScalable_Apps::main_cset_text2_pane_t1_copy1_ParamLimits( 0 );
    TInt lineCount = listLimits.LastRow() + 1;
    for ( TInt i = 0; i < lineCount; i++ )
        {
        if ( AknLayoutUtils::PenEnabled() ) 
            {
            layout.LayoutText( iSummaryRect.Rect(), 
                AknLayoutScalable_Apps::main_cset_text2_pane_t1( i ) );
            }
        else
            {
            layout.LayoutText( iSummaryRect.Rect(), 
                AknLayoutScalable_Apps::main_cset_text2_pane_t1_copy1(0,0,i) );
            }
        User::LeaveIfError( iDescLayoutArray.Append( layout ) );
        }
    
	TInt listInt = ECamSettingItemRecLocation;
	iListBox =	new( ELeave ) CCamCaptureSetupListBox( this, iController );
	iListBox->ConstructL( this, 
	                      R_CAM_OFF_ON_LOC_TEXT_LIST, 
	                      TInt( ECamSettingItemRecLocation ), 
	                      0, 
	                      EFalse, 
	                      ETrue );
	if ( parent )
	    {
	    iListBox->InitializeL
		    ( parent->IntegerSettingValue( listInt ) );
		}
	iListBox->SetContainerWindowL( *iParent );
	
	TRect listboxRect;
	ReadListboxLayoutL(listboxRect);
    
    // Determine the height of the listbox; Either the height based on the number of items,
    // or the maximum allowed listbox height.
    CEikScrollBarFrame::TScrollBarVisibility visibility = CEikScrollBarFrame::EOff;
    TInt maxListBoxHeight = listboxRect.Height();
    TInt listBoxHeight = iListBox->CalcHeightBasedOnNumOfItems( 
                                        iListBox->Model()->NumberOfItems() );
    if ( maxListBoxHeight < listBoxHeight )
        {
        // there are more items than would fit to the listbox rectangle
        
        // the maximum height is the total height of items 
        // that can fit to the maxlistboxheight
        TInt itemHeight = iListBox->ItemHeight();
        TInt itemsVisible = maxListBoxHeight / itemHeight;
        listBoxHeight = itemsVisible * itemHeight;
        
        // the scrollbar needs to be turned on
        visibility = CEikScrollBarFrame::EOn;
        }

    // get layout for the listbox rect
    TAknLayoutRect listboxLayoutRect;
    if ( visibility == CEikScrollBarFrame::EOn )
        {
        // if scrollbars are used, use scrollbar layout
        if ( AknLayoutUtils::PenEnabled() ) 
            {
            listboxLayoutRect.LayoutRect( listboxRect, 
                AknLayoutScalable_Apps::main_cset_list_pane( 0 ) );
            }
        else
            {
            listboxLayoutRect.LayoutRect( listboxRect, 
                AknLayoutScalable_Apps::main_cset_list_pane_copy1( 0 ) );
            }
        }
    else
        {
        if ( AknLayoutUtils::PenEnabled() ) 
            {
            listboxLayoutRect.LayoutRect( listboxRect, 
                AknLayoutScalable_Apps::main_cset_list_pane( 1 ) );
            }
        else
            {
            listboxLayoutRect.LayoutRect( listboxRect, 
                AknLayoutScalable_Apps::main_cset_list_pane_copy1( 1 ) );
            }
        }
    
    // layout scrollbarpane (will not be visible if not needed)
    TAknLayoutRect scrollbarLayoutRect;
    if ( AknLayoutUtils::PenEnabled() ) 
        {
        scrollbarLayoutRect.LayoutRect( listboxRect, 
            AknLayoutScalable_Apps::scroll_pane_cp028( 0 ) );
        }
    else
        {
        scrollbarLayoutRect.LayoutRect( listboxRect, 
            AknLayoutScalable_Apps::scroll_pane_cp028_copy1( 0 ) );
        }
    
    // calculate the layout for the listbox with changed height
    const AknLayoutUtils::SAknLayoutControl listboxLayout =
    {   ELayoutEmpty, listboxRect.iTl.iX, listboxRect.iTl.iY, 
        ELayoutEmpty, ELayoutEmpty, 
        listboxLayoutRect.Rect().Width(), listBoxHeight };
    
    AknLayoutUtils::LayoutControl( iListBox, 
            layoutAreaRect, listboxLayout);
    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                                            CEikScrollBarFrame::EOff, 
                                            visibility );
    iListBox->MakeVisible( ETrue );
    iListBox->SetFocus( EFalse, ENoDrawNow );
    iListBox->UpdateScrollBarsL();
    iListBox->ScrollBarFrame()->DrawScrollBarsNow();
    iListBox->SetListBoxObserver(this);
    SplitCurrentTextL();
    
    // Prevents the system filling invalidated areas with the background colour - 
    // helps reduce flicker.
    Window().SetBackgroundColor();

    ConstructContainerTitleFromResourceL( R_CAM_PHOTO_LOCATION_SETTINGS_TITLE_NAME ); 
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
    appUi->SetTitleL( iTextTitle->Des() );
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::CountComponentControls
// return the number of component controls
// -----------------------------------------------------------------------------
//
TInt CCamLocationSettingContainer::CountComponentControls() const
    {
    return 1; // the listbox
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::ComponentControl
// return the control at a given index
// -----------------------------------------------------------------------------
//
CCoeControl* CCamLocationSettingContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::Draw
// Draws the container
// -----------------------------------------------------------------------------
//
void CCamLocationSettingContainer::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    // Draw skin background
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, gc, Rect() );
 
     // Draw summary text                              
    DrawSummaryText( gc );
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::SizeChanged
// Handles a change in the size of the control
// -----------------------------------------------------------------------------
//
void CCamLocationSettingContainer::SizeChanged()
    {
    // Do this only if the listbox has been created, ie. ConstructFromResourceL() has been called
    if (iListBox)
        {
        TRect listboxRect;
        TRAPD( ignore, ReadListboxLayoutL(listboxRect) );
        if ( ignore )
            { 
            // Just return if the resource reader fails
            return;
            }   
    
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
    
        // Determine the height of the listbox; Either the height based on the number of items,
        // or the maximum allowed listbox height.
        CEikScrollBarFrame::TScrollBarVisibility visibility = CEikScrollBarFrame::EOff;
        TInt maxListBoxHeight = listboxRect.Height();
        TInt listBoxHeight = iListBox->CalcHeightBasedOnNumOfItems( 
                                            iListBox->Model()->NumberOfItems() );
        if ( maxListBoxHeight < listBoxHeight )
            {
            // there are more items than would fit to the listbox rectangle
            
            // the maximum height is the total height of items 
            // that can fit to the maxlistboxheight
            TInt itemHeight = iListBox->ItemHeight();
            TInt itemsVisible = maxListBoxHeight / itemHeight;
            listBoxHeight = itemsVisible * itemHeight;
            
            // the scrollbar needs to be turned on
            visibility = CEikScrollBarFrame::EOn;
            }
    
        // get layout for the listbox rect
        TAknLayoutRect listboxLayoutRect;
        if ( visibility == CEikScrollBarFrame::EOn )
            {
            // if scrollbars are used, use scrollbar layout
            if ( AknLayoutUtils::PenEnabled() ) 
                {
                listboxLayoutRect.LayoutRect( listboxRect, 
                    AknLayoutScalable_Apps::main_cset_list_pane( 0 ) );
                }
            else
                {
                listboxLayoutRect.LayoutRect( listboxRect, 
                    AknLayoutScalable_Apps::main_cset_list_pane_copy1( 0 ) );
                }
            }
        else
            {
            if ( AknLayoutUtils::PenEnabled() ) 
                {
                listboxLayoutRect.LayoutRect( listboxRect, 
                    AknLayoutScalable_Apps::main_cset_list_pane( 1 ) );
                }
            else
                {
                listboxLayoutRect.LayoutRect( listboxRect, 
                    AknLayoutScalable_Apps::main_cset_list_pane_copy1( 1 ) );
                }
            }
        
        // layout scrollbarpane (will not be visible if not needed)
        TAknLayoutRect scrollbarLayoutRect;
        if ( AknLayoutUtils::PenEnabled() ) 
            {
            scrollbarLayoutRect.LayoutRect( listboxRect, 
                AknLayoutScalable_Apps::scroll_pane_cp028( 0 ) );
            }
        else
            {
            scrollbarLayoutRect.LayoutRect( listboxRect, 
                AknLayoutScalable_Apps::scroll_pane_cp028_copy1( 0 ) );
            }
        
        // calculate the layout for the listbox with changed height
        const AknLayoutUtils::SAknLayoutControl listboxLayout =
        {   ELayoutEmpty, listboxRect.iTl.iX, listboxRect.iTl.iY, 
            ELayoutEmpty, ELayoutEmpty, 
            listboxLayoutRect.Rect().Width(), listBoxHeight };
        
        AknLayoutUtils::LayoutControl( iListBox, 
                layoutAreaRect, listboxLayout);
        }
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CCamLocationSettingContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                          TEventCode aType ) 
    {
    TKeyResponse keyResponse = iListBox->OfferKeyEventL( aKeyEvent, aType );
    if ( keyResponse == EKeyWasConsumed )
        {
        ReportEventL( MCoeControlObserver::EEventStateChanged );
        }
    return keyResponse;
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::HandleSettingValueUpdateL
// Handles a change to the setting value
// -----------------------------------------------------------------------------
//
void CCamLocationSettingContainer::HandleSettingValueUpdateL( TInt /*aNewValue*/ )
    {
    SplitCurrentTextL();
    ActivateGc();
    Window().Invalidate( iSummaryRect.Rect() );
    DeactivateGc();
    }

// -----------------------------------------------------------------------------
// CCamLocationSettingContainer::ReadListboxLayoutL
// Reads the listbox layout
// -----------------------------------------------------------------------------
//
void CCamLocationSettingContainer::ReadListboxLayoutL( TRect& aRect )
    {
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
    
    // get the rect size for listbox in the layout
    TAknLayoutRect listboxLayoutRect;
    if ( AknLayoutUtils::PenEnabled() ) 
        {
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            listboxLayoutRect.LayoutRect( layoutAreaRect, 
                AknLayoutScalable_Apps::main_cset_listscroll_pane( 4 ) );
            }
        else
            {
            listboxLayoutRect.LayoutRect( layoutAreaRect, 
                AknLayoutScalable_Apps::main_cset_listscroll_pane( 5 ) );
            }
        }
    else
        {
        listboxLayoutRect.LayoutRect( layoutAreaRect, 
            AknLayoutScalable_Apps::main_cset6_listscroll_pane( 4 ) );
        }
    aRect.SetRect(listboxLayoutRect.Rect().iTl, listboxLayoutRect.Rect().Size() );
    }

// -----------------------------------------------------------------
// CCamLocationSettingContainer::ConstructSummaryDetailsFromResourceL
// Sets up the layouts of the summary items
// -----------------------------------------------------------------
void CCamLocationSettingContainer::ConstructSummaryDetailsFromResourceL(TInt aResourceId)
    {
    TResourceReader reader;                                                                                     
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId ); 
    const TInt count = reader.ReadInt16();

    TInt i              = -1;
    // bitmapID and SummarySceneId are needed since we are using
    // a resource that requires fetching those values also.
    // The reason why the resource itself is not changed is
    // that we are using a CamCaptureSetupListBox that needs those.
    // This requires much less work if not creating own listbox type that
    // would otherwise have to be created. CamCaptureSetupListBox is now
    // used here in such a way that the listbox icon are is set to zero width
    // and no bitmap icon is showed.
    TInt bitmapId = -1;
    // To remove warning about setting variable but never using it
    TInt summarySceneId = bitmapId;
    // made to remove warning. (explained above comments)
    bitmapId = summarySceneId;
     
    HBufC16* descr = NULL;
    
    // Read all of the summary entries from the resource file
    for ( i = 0; i < count; i++ )
        {
        summarySceneId = reader.ReadInt16();
        bitmapId       = reader.ReadInt32();
        descr          = reader.ReadHBufC16L();

	    // This hack because an empty resource string which comes as NULL from the resource reader.
	    // This would create crash later on.
	    if ( NULL == descr )
	        {
	        _LIT(KText,"");
	        HBufC* buf;
	        buf = HBufC::NewL(0);
	        *buf = KText;
	        User::LeaveIfError( iDescArray.Append( buf ) );
	        }
	    else 
	        {
	        User::LeaveIfError( iDescArray.Append( descr ) );
	        }
        }    
    CleanupStack::PopAndDestroy(); // reader       
    }


// -----------------------------------------------------------------
// CCamLocationModeContainer::DrawSummaryText
// Draws summary title and description text
// -----------------------------------------------------------------
void CCamLocationSettingContainer::DrawSummaryText( CWindowGc& aGc ) const
    {
//    aGc.SetBrushColor( KRgbShadedWhite );
//    aGc.Clear( iSummaryRect.Rect() );

    TInt count = iCurrentDescLineArray->Count();
    TInt layoutCount = iDescLayoutArray.Count();
    TRgb color; 
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG6 );

    if ( layoutCount < count )
        {
        count = layoutCount;
        }
    TInt i;        
    for ( i = 0; i < count; i++ )
        {
        iDescLayoutArray[i].DrawText( aGc, ( *iCurrentDescLineArray )[i], ETrue, color );
        }
    }
    
// -----------------------------------------------------------------
// CCamLocationModeContainer::SplitCurrentTextL
// Splits the summary title and description text into multiple lines that fit the status layouts
// -----------------------------------------------------------------
void CCamLocationSettingContainer::SplitCurrentTextL()
    {
    // Use the layout info from the first line for width and font
    TAknLayoutText layout = iDescLayoutArray[0];
   	AknTextUtils::WrapToArrayL( *iDescArray[iListBox->CurrentItemIndex()],
                                layout.TextRect().Width(),
                                *( layout.Font() ),
                                *iCurrentDescLineArray );
    }


// --------------------------------------------------------------------------
// CCamLocationSettingContainer::ConstructContainerTitleFromResourceL
// --------------------------------------------------------------------------
//
void CCamLocationSettingContainer::ConstructContainerTitleFromResourceL( TInt aResourceId )
    {
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId ); // cleanupstack
    
    iTextTitle = reader.ReadHBufC16L();    
    
    CleanupStack::PopAndDestroy(); // reader
    }

// --------------------------------------------------------------------------
// CCamLocationSettingContainer::HandlePointerEventL
// --------------------------------------------------------------------------
//    
void CCamLocationSettingContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent ) 
    {
    RDebug::Print(_L("CCamLocationSettingContainer::HandlePointerEventL iType=%d iPosition=(%d, %d)"),
            aPointerEvent.iType,
            aPointerEvent.iPosition.iX,
            aPointerEvent.iPosition.iY );
    iListBox->HandlePointerEventL( aPointerEvent );
    /*TInt oldListItemIndex = -1;
    TInt newListItemIndex = -1;
    TBool handleItemActivation = EFalse;
   
    if ( iListBox )
        {
        oldListItemIndex = iListBox->CurrentItemIndex();
        }
  
    CCoeControl::HandlePointerEventL( aPointerEvent );       
       
    if ( iListBox )
        {
        newListItemIndex = iListBox->CurrentItemIndex();
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
                TBool focusableItemPointed = iListBox->View()->XYPosToItemIndex(aPointerEvent.iPosition, pointedItemIndex );
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
        TKeyEvent key;
        key.iRepeats = 0;
        key.iCode = EKeyOK;
        key.iModifiers = 0;
        iEikonEnv->SimulateKeyEventL( key, EEventKey );
        }*/
        
    } // end of HandlePointerEventL
// ---------------------------------------------------------------------------
// CCamLocationSettingContainer::HandleListBoxEventL
// Handles Listbox events
// ---------------------------------------------------------------------------
//
void CCamLocationSettingContainer::HandleListBoxEventL( CEikListBox* aListBox, 
                                                    TListBoxEvent aEventType )
    {
    
    switch( aEventType )
        {
        
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
              {
               TKeyEvent key;
               key.iRepeats = 0;
               key.iCode = EKeyOK;
               key.iModifiers = 0;
               iEikonEnv->SimulateKeyEventL( key, EEventKey );
              }
              break;  
        case EEventItemClicked:
            {
            HandleSettingValueUpdateL( iListBox->CurrentItemIndex() );          
            }
            break;
        default:
            break;
        }
    }
//  End of File  
