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
* Description:  A drawer class for the capture setup list box items*
*/



// INCLUDE FILES
#include <eikappui.h>	// For CCoeAppUiBase
#include <eikapp.h>		// For CEikApplication
#include <AknUtils.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <barsread.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <aknenv.h>
#include <aknlayoutscalable_avkon.cdl.h> // list_highlight_pane_g1_cp1

#include <cameraapp.mbg>
#ifndef CAMERAAPP_PLUGIN_BUILD
  #include <cameraapp.rsg>
  #include <vgacamsettings.rsg>
#else
  #include <gscamerapluginrsc.rsg>
#endif //CAMERAAPP_PLUGIN_BUILD


#include "CamCaptureSetupListItemDrawer.h"
#include "CamCaptureSetupListBoxModel.h"
#include "CamUtility.h"
#include "camlogging.h"

#include <aknlayoutscalable_apps.cdl.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif //RD_UI_TRANSITION_EFFECTS_LIST

// CONSTANTS
static const TInt KRadioButtonSelectedIconIndex = 0;
static const TInt KRadioButtonUnselectedIconIndex = 1;


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupListItemDrawer* 
CCamCaptureSetupListItemDrawer::NewL( MCamListboxModel& aListBoxModel )
    {
    CCamCaptureSetupListItemDrawer* self = new( ELeave ) 
                    CCamCaptureSetupListItemDrawer( aListBoxModel );               
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer Destructor
//
// -----------------------------------------------------------------------------
//
CCamCaptureSetupListItemDrawer::~CCamCaptureSetupListItemDrawer()
    {
    PRINT( _L("Camera => ~CCamCaptureSetupListItemDrawer") );
    if ( iRadioButtonIconArray )
        {
        delete iRadioButtonIconArray;
        iRadioButtonIconArray = NULL;
        }
    PRINT( _L("Camera <= ~CCamCaptureSetupListItemDrawer") );
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer::CCamCaptureSetupListItemDrawer
// C++ constructor
// -----------------------------------------------------------------------------
//
CCamCaptureSetupListItemDrawer
::CCamCaptureSetupListItemDrawer( MCamListboxModel& aListBoxModel )
  : iModel( aListBoxModel )
    {
	}

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer::ConstructL
// 2nd phase construction
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListItemDrawer::ConstructL()
	{    


    iCaptureSetupItemHeight = iModel.ListItemLayoutData().Rect().Height();
    

	const TInt KIconArrayGranularity = 4;
	iRadioButtonIconArray = 
                          new( ELeave ) CAknIconArray( KIconArrayGranularity );
    TResourceReader reader;
    
    CCoeEnv::Static()->CreateResourceReaderLC(reader, R_AVKON_SETTING_PAGE_RADIOBUTTON_ICONS);

    reader.ReadInt16(); //not needed, simple or complex

    HBufC* bmpFile = reader.ReadHBufCL();
    TInt count = reader.ReadInt16(); // count
   
    TInt32 bmpSelected = reader.ReadInt32();
    TInt32 bmpSelectedM = reader.ReadInt32();
    TInt32 bmp = reader.ReadInt32();
    TInt32 bmpM = reader.ReadInt32();
    CleanupStack::PopAndDestroy(); // reader

    CleanupStack::PushL(bmpFile);

    // create normally colored icons
    CreateIconAndAddToArrayL( iRadioButtonIconArray,
                              KAknsIIDQgnIndiRadiobuttOn,
                              EAknsCIQsnIconColorsCG14,
                              *bmpFile,
                              bmpSelected,
                              bmpSelectedM );

    CreateIconAndAddToArrayL( iRadioButtonIconArray,
                              KAknsIIDQgnIndiRadiobuttOff,
                              EAknsCIQsnIconColorsCG14,
                              *bmpFile,
                              bmp,
                              bmpM );

    // create highlight colored icons
    CreateIconAndAddToArrayL( iRadioButtonIconArray,
                              KAknsIIDQgnIndiRadiobuttOn,
                              EAknsCIQsnIconColorsCG18,
                              *bmpFile,
                              bmpSelected,
                              bmpSelectedM );

    CreateIconAndAddToArrayL( iRadioButtonIconArray,
                              KAknsIIDQgnIndiRadiobuttOff,
                              EAknsCIQsnIconColorsCG18,
                              *bmpFile,
                              bmp,
                              bmpM );
    
    const TSize radioButtonRectSize = 
        TSize( iModel.RadioButtonLayoutData().Rect().Width(),
               iModel.RadioButtonLayoutData().Rect().Height() );
    AknIconUtils::SetSize( 
        iRadioButtonIconArray->At( KRadioButtonSelectedIconIndex )->Bitmap(), 
                                   radioButtonRectSize );
    AknIconUtils::SetSize( 
        iRadioButtonIconArray->At( KRadioButtonUnselectedIconIndex )->Bitmap(), 
                                   radioButtonRectSize );

    
	ReadLayout(); 
	
    CleanupStack::PopAndDestroy(); // bmpfile

	}

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer::MinimumCellSize
// Calculate the minimum size of a cell
// ---------------------------------------------------------------------------
//
TSize 
CCamCaptureSetupListItemDrawer::MinimumCellSize() const
  {
  PRINT( _L("Camera => CCamCaptureSetupListItemDrawer::MinimumCellSize") );

  TSize cellSize;
  cellSize.iWidth = iModel.ListItemLayoutData().Rect().Width();
  cellSize.iHeight = iModel.ListItemLayoutData().Rect().Height();


  PRINT2( _L("Camera => CCamCaptureSetupListItemDrawer::MinimumCellSize, return (%d x %d)"), cellSize.iWidth, cellSize.iHeight );
  return cellSize;
  }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer::DrawActualItem
// Draw the highlight, bitmap, radio button and item text.
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListItemDrawer::DrawActualItem( TInt aItemIndex, 
                                                    const TRect& aActualItemRect,
                                                    TBool aItemIsCurrent,
                                                    TBool /*aViewIsEmphasized*/,
                                                    TBool /*aViewIsDimmed*/,
                                                    TBool aItemIsSelected ) const
    { 
	
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal *transApi = CAknListLoader::TfxApiInternal( iGc );
    if ( transApi )
        {
        transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    // Draw unhighlighted rectangle that encapsulates the item text and bitmap.
	DrawItemRect( aActualItemRect );       

#ifdef RD_UI_TRANSITION_EFFECTS_LIST   
   	if ( transApi )
   	    {
        transApi->StopDrawing();
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    // If this item is currently selected, draw highlight        
	if ( aItemIsCurrent )
		{
		DrawHighlight( aActualItemRect );
		}

#ifdef RD_UI_TRANSITION_EFFECTS_LIST   
   	if ( transApi )
   	    {
        transApi->StartDrawing( MAknListBoxTfxInternal::EListItem );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    // Determine drawing colour for text and radio buttons, from current skin
    // ...text colour index if this item is not selected.
    TRgb color;
    TAknsQsnTextColorsIndex index = EAknsCIQsnTextColorsCG7;
    // ...text colour index if this item is selected.
    if( Flags() & CListItemDrawer::ESingleClickDisabledHighlight )
        {
        // Single clickable items don't have a highlight
        aItemIsCurrent = EFalse;
        }

    if ( aItemIsCurrent )
        {
        index = EAknsCIQsnTextColorsCG10; // highlighted text colour
        }

    // ...get colour from current skin for text colour index
	MAknsSkinInstance *skin = AknsUtils::SkinInstance();	   
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors, index );

    // ...make sure text/bitmap background colour does not overwrite skin background colour
	iGc->SetBrushStyle( CGraphicsContext::ENullBrush );
	
	if ( !iModel.ShootingModeListBox() )
        {

        // Draw text
        // ...Create a text layout object for drawing the text
        // ...inside of the list item's rectangle
        TAknLayoutText layoutText;
        layoutText.LayoutText( aActualItemRect, iTxtLayout );  
        // ...Pass the text to be drawn, into the text layout object
        // ...and draw it.    
        layoutText.DrawText( *iGc, iModel.ItemText( aItemIndex ), 
                             ETrue, color );
        // Draw the bitmap.
        // ...Create a rect layout object for drawing the bitmap
        // ...inside of the list item's rectangle.
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect( aActualItemRect, iBmpLayout );
        // ...Pass the bitmap to be drawn, into the rect layout object
        // ...and draw it.
        CFbsBitmap* bitmap = iModel.Bitmap( aItemIndex );
        // if the item has a bitmap
        if ( bitmap )
            {
            CFbsBitmap* bitmapMask = iModel.BitmapMask( aItemIndex );
            layoutRect.DrawImage( *iGc, bitmap, bitmapMask );
            }        


        DrawRadioButton( aActualItemRect, aItemIsSelected, color );
        }
    else 
        {
        // draw without radiobuttons
        
        // Draw text
        // ...Create a text layout object for drawing the text
        // ...inside of the list item's rectangle
        TAknLayoutText layoutText;
        layoutText.LayoutText( aActualItemRect, iTxtWithRbLayout );
        // ...Pass the text to be drawn, into the text layout object
        // ...and draw it.    
        layoutText.DrawText( *iGc, iModel.ItemText( aItemIndex ), ETrue, color );

        // Draw the bitmap.
        // ...Create a rect layout object for drawing the bitmap
        // ...inside of the list item's rectangle.
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect( aActualItemRect, iBmpWithRbLayout );
        // ...Pass the bitmap to be drawn, into the rect layout object
        // ...and draw it.
        CFbsBitmap* bitmap = iModel.Bitmap( aItemIndex );
        // if the item has a bitmap
        if ( bitmap )
            {
            CFbsBitmap* bitmapMask = iModel.BitmapMask( aItemIndex );
            layoutRect.DrawImage( *iGc, bitmap, bitmapMask );
            }        

        
        }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        transApi->StopDrawing();
        }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer::DrawHighlight
// Draw the highlight as a bitmap before any other part of the item
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListItemDrawer::DrawHighlight( const TRect& aActualItemRect ) const
    {
    if( Flags() & CListItemDrawer::ESingleClickDisabledHighlight )
        {
        return;
        }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
	   	if ( transApi )
	   	    {
            transApi->BeginRedraw( MAknListBoxTfxInternal::EListHighlight, aActualItemRect );
            transApi->StartDrawing( MAknListBoxTfxInternal::EListHighlight );
	   	    }
#endif //RD_UI_TRANSITION_EFFECTS_LIST    	
    TAknLayoutRect highlightRectLayout;
    highlightRectLayout.LayoutRect( aActualItemRect, iHighlightLayout );
    //highlightRectLayout.DrawOutLineRect(*iGc);
    TRect highlightRect( highlightRectLayout.Rect() );


       
    
    // Draw highlight using the same highlight that is used for the Avkon settings list radio controls.
	MAknsSkinInstance *skin = AknsUtils::SkinInstance();	
    //TBool highlightDrawn = EFalse;

    TAknLayoutRect innerHighlightLayRect;
    innerHighlightLayRect.LayoutRect( highlightRect, 
                       AknLayoutScalable_Avkon::list_highlight_pane_g1_cp1() ); 

	//highlightDrawn = 
    if ( Flags()&EPressedDownState )
        {
        // This handles the darker pressed down highlights
        AknsDrawUtils::DrawFrame( skin, 
            *iGc, 
            highlightRect, 
            innerHighlightLayRect.Rect(), 
            KAknsIIDQsnFrListPressed,
            KAknsIIDQsnFrListCenterPressed );
        }
    else
        {
        AknsDrawUtils::DrawFrame( skin, 
            *iGc, 
            highlightRect, 
            innerHighlightLayRect.Rect(), 
            KAknsIIDQsnFrList, //KAknsIIDQsnFrSetOptFoc other option?
            KAknsIIDDefault );
                
        }

	//(void)highlightDrawn; // avoid compiler warning
	/*
    // if the highlight has not be drawn
	if ( !highlightDrawn )
		{
		TAknLayoutRect highlightshadow;
		TAknLayoutRect highlight;
		highlightshadow.LayoutRect( highlightRect, 
            AKN_LAYOUT_WINDOW_List_pane_highlight_graphics__various__Line_1( highlightRect ) );
		highlight.LayoutRect( highlightRect, 
            AKN_LAYOUT_WINDOW_List_pane_highlight_graphics__various__Line_2( highlightRect ) );
		highlightshadow.DrawRect( *iGc );
		highlight.DrawRect( *iGc );
		}
		*/
#ifdef RD_UI_TRANSITION_EFFECTS_LIST    
        if ( transApi )
            {
            transApi->StopDrawing();
            transApi->EndRedraw( MAknListBoxTfxInternal::EListHighlight );
            }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItemDrawer::DrawRadioButton
// Draw the radio button
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListItemDrawer::DrawRadioButton( const TRect& aActualItemRect, 
                                                      TBool aItemIsSelected, const TRgb& /*aColor*/ ) const
    {
    PRINT( _L("Camera => CCamCaptureSetupListItemDrawer::DrawRadioButton"))

    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( aActualItemRect, AknLayoutScalable_Apps::cset_list_set_pane_g1( 0 ) );

    // Pass the bitmap to be drawn into the rect layout object, and draw it
	TInt index;
	if (aItemIsSelected)
		{
		index = KRadioButtonSelectedIconIndex;
		}
	else
		{
		index = KRadioButtonUnselectedIconIndex;
		}

	layoutRect.DrawImage( *iGc,
	        iRadioButtonIconArray->At(index)->Bitmap(),
	        iRadioButtonIconArray->At(index)->Mask() );           

    }


// ---------------------------------------------------------
// CCamCaptureSetupListItemDrawer::DrawItemRect
// Draws a rectangle for an item.
// ---------------------------------------------------------
//
void CCamCaptureSetupListItemDrawer::DrawItemRect( 
    const TRect& aActualItemRect ) // the rectangular area to be drawn
    const
    {
    
    /*
    iGc->SetBrushColor( AKN_LAF_COLOR_STATIC(0) );
	iGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
	iGc->SetPenStyle( CGraphicsContext::ENullPen );	
    iGc->DrawRect( aActualItemRect );
    
  */

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    const CCoeControl* control = iParentControl;
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( control );
    
    TBool bgDrawn( EFalse );
	if ( control )
		{
            bgDrawn = AknsDrawUtils::Background(
                skin, cc, control, *iGc, aActualItemRect,
                KAknsDrawParamNoClearUnderImage | 
                KAknsDrawParamBottomLevelRGBOnly );
		}
            
	 if ( !bgDrawn )
        {
        iGc->Clear( aActualItemRect );
        }
    

    /*
    // Draw background to be the same colour as that for the Avkon settings list radio controls.
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
//    AknsDrawUtils::DrawCachedImage(skin, *iGc, aActualItemRect, KAknsIIDQsnFrSetOpt);	  
    AknsDrawUtils::DrawCachedImage(skin, *iGc, aActualItemRect, KAknsIIDQsnBgScreenMp);	  
*/

    }

// ---------------------------------------------------------
// CCamCaptureSetupListItemDrawer::SetControl
// Sets the parent control of the listbox
// ---------------------------------------------------------
//
void CCamCaptureSetupListItemDrawer::SetParentControl( 
    const CCoeControl* aControl )
    {
    iParentControl = aControl;
    }

// ---------------------------------------------------------
// CCamCaptureSetupListItemDrawer::ReadLayout
// ---------------------------------------------------------
//
void CCamCaptureSetupListItemDrawer::ReadLayout()  
    {
    if ( CamUtility::IsNhdDevice() ) 
        {
        iTxtLayout = AknLayoutScalable_Apps::cset_list_set_pane_t1( 0 );
        iBmpLayout = AknLayoutScalable_Apps::cset_list_set_pane_g3( 0 );
        iTxtWithRbLayout = AknLayoutScalable_Apps::cset_list_set_pane_t1( 1 );
        iBmpWithRbLayout = AknLayoutScalable_Apps::cset_list_set_pane_g3( 1 );
        iHighlightLayout = 
                       AknLayoutScalable_Apps::list_highlight_pane_cp021( 0 ); 
        }
    else
        {
        iTxtLayout = AknLayoutScalable_Apps::cset_list_set_pane_t1_copy1( 0 );
        iBmpLayout = AknLayoutScalable_Apps::cset_list_set_pane_g3_copy1( 0 );
        iTxtWithRbLayout = 
                     AknLayoutScalable_Apps::cset_list_set_pane_t1_copy1( 1 );
        iBmpWithRbLayout = 
                     AknLayoutScalable_Apps::cset_list_set_pane_g3_copy1( 1 );
        iHighlightLayout = 
                 AknLayoutScalable_Apps::list_highlight_pane_cp021_copy1( 0 );
        }
    }
 
void CCamCaptureSetupListItemDrawer::CreateIconAndAddToArrayL(
    CArrayPtr<CGulIcon>*& aIconArray,
    const TAknsItemID& aId,
    const TInt aColorIndex,
    const TDesC& aBmpFile,
    const TInt32 aBmp,
    const TInt32 aBmpM )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = 0;
    CFbsBitmap* mask = 0;

    AknsUtils::CreateColorIconLC( skin,
                                  aId,
                                  KAknsIIDQsnIconColors,
                                  aColorIndex,
                                  bitmap,
                                  mask,
                                  aBmpFile,
                                  aBmp,
                                  aBmpM,
                                  KRgbBlack );

    CGulIcon* gulicon = CGulIcon::NewL( bitmap, mask ); // ownership passed
    CleanupStack::PushL( gulicon );
    
    aIconArray->AppendL( gulicon );
    CleanupStack::Pop( 3 ); // mask, bitmap, gulicon
    }

//End of file


