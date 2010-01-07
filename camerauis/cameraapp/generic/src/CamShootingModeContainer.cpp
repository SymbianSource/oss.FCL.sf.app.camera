/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include <AknBidiTextUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
#include <barsread.h>    // resource reader
#include <aknview.h> 
#include <touchfeedback.h> 
#include <StringLoader.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>  

#include "CamShootingModeContainer.h"
#include "CamUtility.h"
#include "CamAppUi.h"
#include "CameraUiConfigManager.h"
#include "CamSceneListBox.h"
#include "CamSceneListBoxModel.h"
#include "CamPanic.h"
#include "camactivepalettehandler.h"

#include <aknlayoutscalable_apps.cdl.h>
#include "akntoolbar.h"

// CONSTANTS

const TInt KInfoTooltipDelay = 0; // time (milliseconds) delay when showing the tooltip
const TInt KInfoTooltipDisplayTime = 5000; // maximum time (milliseconds) the tooltip is displayed
const TInt KExplTxtGranularity = 6;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamShootingModeContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamShootingModeContainer* 
CCamShootingModeContainer::NewL( const TRect&             aRect, 
                                       CAknView&          aView,
                                       TCamCameraMode     aMode,
                                       CCamAppController& aController,
                                       TBool              aUserBaseScenes )
  {
  CCamShootingModeContainer* self = 
      new( ELeave ) CCamShootingModeContainer( aMode, 
                                               aController,
                                               aView, 
                                               aUserBaseScenes );
  CleanupStack::PushL( self );
  self->ConstructL( aRect );
  CleanupStack::Pop( self );
  return self;
  }

// ---------------------------------------------------------------------------
// CCamShootingModeContainer::~CCamShootingModeContainer
// Destructor
// ---------------------------------------------------------------------------
//
CCamShootingModeContainer::~CCamShootingModeContainer()
  {
  PRINT( _L( "Camera => ~CCamShootingModeContainer" ) );
  
  iSummaryBitmapArray.ResetAndDestroy();
  iTitleArray.ResetAndDestroy();
  iDescArray.ResetAndDestroy();
  iExplTxtLinesLayout.Close();  
   
  if ( iFeedback ) 
    {
    iFeedback->RemoveFeedbackForControl( iListBox );
    }

  if ( iListBox )
	  {
	  delete iListBox;
	  iListBox = NULL;
	  }
  
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

  PRINT( _L( "Camera <= ~CCamShootingModeContainer" ) );    
  }
  
// ---------------------------------------------------------
// CCamShootingModeContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void 
CCamShootingModeContainer::ConstructL( const TRect& aRect )
    {
	PRINT(_L("Camera => CCamShootingModeContainer::ConstructL") )
    
    CCamContainerBase::BaseConstructL( aRect );

    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       iLayoutAreaRect );
    // layout area rectangle contains the area, where components need to be 
    // drawn to. the container size is the whole screen, but the layouts are 
    // for the client area. aRect is the container size that might include or
    // might not include statuspane area. calculating area self will
    // go around the problem
    TInt resource = SelectSummaryResourceL();
    if( 0 <= resource )
      {
      ConstructSummaryDetailsFromResourceL( resource );
      }

    TInt scene = 0;
    if ( iUserBaseScenes )
        {
        scene = ECamSettingItemUserSceneBasedOnScene;
        }
    else if ( ECamControllerImage == iMode )
        {
        scene = ECamSettingItemDynamicPhotoScene;
        }
    else
        {
        scene = ECamSettingItemDynamicVideoScene;
        }                      

    // construct the listbox
	iListBox =	new( ELeave ) CCamSceneListBox( iMode, this, iController );                           
	iListBox->ConstructL( iController, this, iTitleArray, iDescArray, 
	                      iUserBaseScenes  );	
	iListBox->InitializeL( iController.IntegerSettingValue( scene ) );
	iListBox->SetContainerWindowL( *this );
	iListBox->CreateScrollBarFrameL( ETrue ); 
    iListBox->DisableSingleClick( ETrue );
   
	TRect listboxRect;
    if ( CamUtility::IsNhdDevice() ) 
        {
        listboxRect = TouchLayout();
        }
    else
        {
        iCurrentDescLineArray  = 
                     new( ELeave ) CArrayFixFlat<TPtrC>( KExplTxtGranularity );
        listboxRect = NonTouchLayout();
        }
	
    // Determine the height of the listbox; 
    // Height needs to be based on the number of items, because the layout
    // height (maxListBoxHeight )might be different 
    // than combined height of the max visible listbox items
    
    CEikScrollBarFrame::TScrollBarVisibility visibility = 
                                                      CEikScrollBarFrame::EOff;
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
   
    // calculate the layout for the listbox with changed height
    const AknLayoutUtils::SAknLayoutControl listboxLayout =
    {   ELayoutEmpty, iListboxLayoutRect.Rect().iTl.iX, listboxRect.iTl.iY,
        ELayoutEmpty, ELayoutEmpty, 
        iListboxLayoutRect.Rect().Width(), listBoxHeight };
   
    AknLayoutUtils::LayoutControl( iListBox, 
            iLayoutAreaRect, listboxLayout);

    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                                        CEikScrollBarFrame::EOff, visibility );
    iListBox->MakeVisible( ETrue );
    iListBox->SetFocus( EFalse, ENoDrawNow );
    iListBox->UpdateScrollBarsL();
    iListBox->ScrollBarFrame()->DrawScrollBarsNow();
    iListBox->SetListBoxObserver(this);
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    appUi->SetActivePaletteVisibility( EFalse );

    // Prevents the system filling invalidated areas with the background 
    // colour - helps reduce flicker.
    Window().SetBackgroundColor();
    
    if ( iMode == ECamControllerImage )
        {
        ConstructContainerTitleFromResourceL( R_CAM_STILL_SCENE_SETTING_TITLE ); 
        }
    else 
        {
        ConstructContainerTitleFromResourceL( R_CAM_VIDEO_SCENE_SETTING_TITLE );
        }
     
    if ( ! iTooltipController )
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
    iTooltipController->SetPositionAndAlignment( 
                             TPoint( center.iX, rect.iTl.iY ), EHLeftVBottom );
     
    iTooltipController->SetTimeDelayBeforeShow( KInfoTooltipDelay );
    iTooltipController->SetTimePopupInView( KInfoTooltipDisplayTime );
    
    iFeedback = MTouchFeedback::Instance();  
    if ( iFeedback )  
        {
        iFeedback->SetFeedbackArea( iListBox, 0, iListBox->Rect(), 
                                    ETouchFeedbackBasic, ETouchEventStylusDown );
        }

	PRINT(_L("Camera <= CCamShootingModeContainer::ConstructL") )    
    }
   

// ---------------------------------------------------------------------------
// SelectSummaryResourceL
// ---------------------------------------------------------------------------
//
TInt 
CCamShootingModeContainer::SelectSummaryResourceL()
  {
  TInt  resourceId = KErrNotSupported;
  TBool secondCameraOn = 
    static_cast<CCamAppUi*>( iEikonEnv->AppUi() )->IsSecondCameraEnabled();
  // -------------------------------------------------------
  // Photo mode  
  if ( ECamControllerImage == iMode )
    {
    if ( secondCameraOn )
      {
      //Product does not currently support shooting modes for secondary camera photo mode
      }
    else
      {
      resourceId = R_CAM_CAPTURE_SETUP_LIST_PHOTO_SHOOTING_MODE_SUMMARY;
      }
    }
  // -------------------------------------------------------
  // Video mode  
  else
    {
    if ( secondCameraOn )
      {
      //Products do not currently support shooting modes for secondary camera video mode           
      }
    else
      {
      resourceId = R_CAM_CAPTURE_SETUP_LIST_VIDEO_SHOOTING_MODE_SUMMARY;
      }
    }
  // -------------------------------------------------------

  return resourceId;
  }


// ---------------------------------------------------------------------------
// CCamShootingModeContainer::CCamShootingModeContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamShootingModeContainer::CCamShootingModeContainer( 
        TCamCameraMode aMode,
        CCamAppController& aController,
        CAknView& aView,
        TBool aUserBaseScenes )
: CCamContainerBase( aController, aView ),
    iListBox( NULL ),
    iMode( aMode ), 
    iUserBaseScenes( aUserBaseScenes ),
    iTooltipController( NULL ),
    iListboxTitle( NULL ),
    iTooltipIndex( -1 ),
    iActivateOnTouchRelease( EFalse )
    {
    }

// ----------------------------------------------------------------
// CCamShootingModeContainer::UserSceneHighlighted
// Returns ETrue if the current selected scene is User
// ----------------------------------------------------------------
//
TBool CCamShootingModeContainer::UserSceneHighlighted()
    {
    return (CurrentSettingItemValue() == ECamSceneUser );
    }

// ----------------------------------------------------------------
// CCamShootingModeContainer::SaveSceneSettingIfChangedL
// Stores the currently selected scene in the dynamic settings model
// if it differs from current scene mode
// ----------------------------------------------------------------
//
TBool CCamShootingModeContainer::SaveSceneSettingIfChangedL()
    {
    TInt scene = 0;
    if ( iUserBaseScenes )
      {
      scene = ECamSettingItemUserSceneBasedOnScene;
      }
    else if ( ECamControllerImage == iMode )
      {
      scene = ECamSettingItemDynamicPhotoScene;
      }
    else
      {
      scene = ECamSettingItemDynamicVideoScene;
      }

    TInt currentSceneMode = iController.IntegerSettingValue( scene );
    if ( currentSceneMode != CurrentSettingItemValue() )
        {
        // The scene mode selection has changed, store new mode
        return SaveSceneSettingL();
        }
    else
        {
        // No change, no need to save
        return EFalse;
        }
    }

// ----------------------------------------------------------------
// CCamShootingModeContainer::SaveSceneSetting
// Stores the currently selected scene in the dynamic settings model
// ----------------------------------------------------------------
//
TBool CCamShootingModeContainer::SaveSceneSettingL()
  {
  TInt scene = 0;
  if ( iUserBaseScenes )
    {
    scene = ECamSettingItemUserSceneBasedOnScene;
    }
  else if ( ECamControllerImage == iMode )
    {
    scene = ECamSettingItemDynamicPhotoScene;
    }
  else
    {
    scene = ECamSettingItemDynamicVideoScene;
    }
  
  iController.SetIntegerSettingValueL( scene,
              CurrentSettingItemValue() );
  static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() )->APHandler()->UpdateActivePaletteL();        
              

  // If user selected sports scene from list.
  if ( CurrentSettingItemValue() == ECamSceneSports )
    {
    // if the selection was for user scene base scene, and user 
    // scene is active, but request was cancelled, return false.
    if ( ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene )
         == ECamSceneUser ) &&
         ( iController.IntegerSettingValue( ECamSettingItemUserSceneBasedOnScene )
         != ECamSceneSports ) )
        {
        return EFalse;
        }
    // if the selection was for the photo/video scene, but request was 
    // cancelled, return false.
    if ( ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene )
         != ECamSceneUser )  && 
         ( iController.IntegerSettingValue( scene ) != ECamSceneSports ) )
        {
        return EFalse;
        }
    }
  // Otherwise, if the user selected user scene from the list, but request
  // was cancelled, return false.
  else if ( ( CurrentSettingItemValue() 
            == ECamSceneUser ) &&
            ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene )
           != ECamSceneUser ) )
      {
      return EFalse;
      }

  // Remove lint warning.
  else
      {
      }

  // In all other cases, return true.
  return ETrue;
  }
     
// -----------------------------------------------------------------------------
// CCamShootingModeContainer::CurrentSettingItemValue
// Returns the Current item's setting value ID
// -----------------------------------------------------------------------------
TInt CCamShootingModeContainer::CurrentSettingItemValue() const
    {
    return (static_cast<CCamSceneListBoxModel *>(iListBox->Model())->ItemValue(iListBox->CurrentItemIndex()));
    }
    
// -----------------------------------------------------------------------------
// CCamShootingModeContainer::IndexForValueId() const
// Returns the index of the item in the array with the specified ValueId
// -----------------------------------------------------------------------------   
TInt CCamShootingModeContainer::IndexForValueId( TInt aValueId ) const
    {
    return static_cast<MCamListboxModel*>( iListBox->Model() )->ValueIndex( aValueId );
    } 

// -----------------------------------------------------------------------------
// CCamShootingModeContainer::HandleSettingValueUpdateL
// Handles a change to the setting value of the slider
// -----------------------------------------------------------------------------
//
void CCamShootingModeContainer::HandleSettingValueUpdateL( TInt /*aNewValue*/ )
    {   
    }
    

// ---------------------------------------------------------
// CCamShootingModeContainer::CountComponentControls 
// Returns the number of controls owned
// ---------------------------------------------------------
//
TInt CCamShootingModeContainer::CountComponentControls() const
    {
    return 1;
    }

// ---------------------------------------------------------
// CCamShootingModeContainer::ComponentControl
// Returns the requested component control
// ---------------------------------------------------------
//
CCoeControl* CCamShootingModeContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// ---------------------------------------------------------
// CCamShootingModeContainer::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamShootingModeContainer::Draw( const TRect& aRect ) const
    {
	PRINT(_L("Camera => CCamShootingModeContainer::Draw") )

    CWindowGc& gc = SystemGc();
    if ( CamUtility::IsNhdDevice() )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsDrawUtils::Background( skin, iBgContext, gc, aRect );
        TRgb color;
        AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                                                EAknsCIQsnTextColorsCG6 );  
        iTitleTextRectLayout.DrawText( gc, *iListboxTitle, ETrue, color ); 
        }
    else
        {  
        TRAP_IGNORE( DrawSummaryTextL( gc ) );
        }

 
	PRINT(_L("Camera <= CCamShootingModeContainer::Draw") )
    }

// ----------------------------------------------------------------
// CCamShootingModeContainer::OfferKeyEventL
// Handles this application view's command keys. Forwards other
// keys to child control(s).
// ----------------------------------------------------------------
//
TKeyResponse CCamShootingModeContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
   
    iController.StartIdleTimer();

    // If the Ok button is pressed, select the current item
    if ( aKeyEvent.iCode == EKeyOK && aKeyEvent.iRepeats == 0 && aType == EEventKey )
        {
        TKeyResponse response = iListBox->OfferKeyEventL( aKeyEvent, aType );
        iView.HandleCommandL( EAknSoftkeySelect );
        return response;
        }
    TKeyResponse returnvalue = iListBox->OfferKeyEventL( aKeyEvent, aType );

    // for non touch, we use key presses to scroll thru the scene modes
    // for touch with keyboard, key pressing can also scroll thru the scene modes
    // after every up and down key presses we display the tool tip
    if ( EStdKeyUpArrow == aKeyEvent.iScanCode ||
         EStdKeyDownArrow == aKeyEvent.iScanCode )
        {
        if ( CamUtility::IsNhdDevice() )  
            {
            ShowTooltipL(); 
            }
        else
            {
            DrawDeferred(); // Update explanation text
            }
        }
        
    return returnvalue;
    }

// -----------------------------------------------------------------
// CCamShootingModeContainer::ConstructSummaryDetailsFromResourceL
// Sets up the layouts of the summary items
// -----------------------------------------------------------------
void CCamShootingModeContainer::ConstructSummaryDetailsFromResourceL(TInt aResourceId)
    {
     // Find the name and path of the MBM file for bitmaps
    TFileName mbmFileName;
    CamUtility::ResourceFileName( mbmFileName );
     
    TResourceReader reader;                                                                                     
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId ); 
    const TInt count = reader.ReadInt16();

    TInt i              = -1;
    TInt bitmapId 	    = -1;
    TInt summarySceneId = -1;
    HBufC16* title = NULL;
    HBufC16* descr = NULL;
    
    // Read all of the summary entries from the resource file
    for ( i = 0; i < count; i++ )
        {
        summarySceneId = reader.ReadInt16();
        bitmapId       = reader.ReadInt32();
        title          = reader.ReadHBufC16L();
        descr          = reader.ReadHBufC16L();

        // Only add supported scenes.
        if( IsSupportedScene(summarySceneId) )
          {
          CFbsBitmap* icon = AknIconUtils::CreateIconL( mbmFileName, bitmapId );
          CleanupStack::PushL( icon );
          User::LeaveIfError( iSummaryBitmapArray.Append( icon ) );
          CleanupStack::Pop( icon );

          User::LeaveIfError( iTitleArray.Append( title ) );
          if ( summarySceneId == ECamSceneScenery )
            {
            CCamAppUi* appUi   = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
            if ( appUi && appUi->AppController().UiConfigManagerPtr()
                 && appUi->AppController().UiConfigManagerPtr()->IsAutoFocusSupported() )
                {
                // the description of the icon
                HBufC* Description;
                TInt description = R_CAM_SCENE_DESCRIPTION_LANDSCAPE_AUTOFOCUS;
                Description = StringLoader::LoadL( description );
                User::LeaveIfError( iDescArray.Append( Description ) );
                }
            else 
                {
                User::LeaveIfError( iDescArray.Append( descr ) );
                }
            }
          else 
            {
            User::LeaveIfError( iDescArray.Append( descr ) );
            }
          }
        else
          {
          delete title;
          title = NULL;
          delete descr;
          descr = NULL;
          }          
        }    
    CleanupStack::PopAndDestroy(); // reader       
    }





// -----------------------------------------------------------------
// IsSupportedScene
//
// Get whether the given scene is supported.
// -----------------------------------------------------------------
//
TBool 
CCamShootingModeContainer::IsSupportedScene( TInt aSceneId ) const
  {
  // NOTE: 
  //   CamUtility in product changed to return also user scene
  //   as one of supported scenes.

  CCamAppUi* appUi   = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
  TBool      imageMode = (ECamControllerImage == iMode);
  
  // Scene must be in supported scenes list.
  TBool supported = iController.IsSceneSupported( aSceneId, imageMode );
  
  // Still, user scene is never supported in user scene base list.
  TBool userException = (ECamSceneUser == aSceneId && iUserBaseScenes );

  if( supported && !userException )
    {
    return ETrue;
    }
  else
    {
    return EFalse;
    }
  }

 // --------------------------------------------------------------------------
// CCamShootingModeContainer::HandlePointerEventL
 // --------------------------------------------------------------------------
 //    
 void CCamShootingModeContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent ) 
    {
    
    RDebug::Print(_L("CCamShootingModeContainer::HandlePointerEventL iType=%d iPosition=(%d, %d)"),
        aPointerEvent.iType,
        aPointerEvent.iPosition.iX,
        aPointerEvent.iPosition.iY );
    iListBox->HandlePointerEventL(aPointerEvent);
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
   	
    	// no need for tooltips anymore
    	HideCurrentTooltipL();
    	
    	if ( UserSceneHighlighted() )
    		{
    		iView.HandleCommandL( ECamCmdUserSceneSelected );
    		}
    	else 
    		{
    		iView.HandleCommandL( EAknSoftkeySelect );
    		}
    	}
   
    */
    } // end of HandlePointerEventL
    

// --------------------------------------------------------------------------
// CCamShootingModeContainer::ShowTooltipL
// --------------------------------------------------------------------------
//
void CCamShootingModeContainer::ShowTooltipL()
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
        iTooltipController->SetPositionByHighlight( iListBox->HighlightRect() );

        iTooltipIndex = iListBox->CurrentItemIndex();
        }
    
    iTooltipController->ShowInfoPopupNote();   
    }


// --------------------------------------------------------------------------
// CCamShootingModeContainer::HideCurrentTooltipL
// Hides tooltip for the selected listbox item, if one is visible
// --------------------------------------------------------------------------
//
void CCamShootingModeContainer::HideCurrentTooltipL()
    {
    
    if ( !iTooltipController || ( iDescArray[iListBox->CurrentItemIndex()]->Length() <= 0 ) )
        {
        return;
        }
    
    iTooltipController->HideInfoPopupNote();
    
    }

 
// --------------------------------------------------------------------------
// CCamShootingModeContainer::ConstructContainerTitleFromResourceL
// --------------------------------------------------------------------------
//
void CCamShootingModeContainer::ConstructContainerTitleFromResourceL( TInt aResourceId )
    {
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId ); // cleanupstack
    
    iListboxTitle = reader.ReadHBufC16L();    
    
    CleanupStack::PopAndDestroy(); // reader
    }

// --------------------------------------------------------------------------
// CCamShootingModeContainer::SelectItem
// --------------------------------------------------------------------------
//
TBool CCamShootingModeContainer::SelectItem( TInt aItemIndex )
	{
	
    TInt oldListItemIndex = -1;
    TBool returnSelectionSuccess = EFalse;
   
    if ( iListBox )
    	{
    	oldListItemIndex = iListBox->CurrentItemIndex();
	
	    if ( oldListItemIndex != aItemIndex )
	    	{
	    	iListBox->SetCurrentItemIndex( aItemIndex );
	       	}
	    else
	    	{
	    	// already selected
	    	returnSelectionSuccess = EFalse;
	    	}
    	}
    else
    	{
    	// iListBox == NULL
    	returnSelectionSuccess = EFalse;
    	}
	
    return returnSelectionSuccess;
	}

// -----------------------------------------------------------------
// CCamShootingModeContainer::DrawSummaryText (Scene settings)
// Draws summary title and description text
// -----------------------------------------------------------------

void CCamShootingModeContainer::DrawSummaryTextL( CWindowGc& aGc ) const  
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    appUi->SetTitleL( *iListboxTitle );     
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background( skin, iBgContext, aGc, Rect() );
  
    // Draw summary frame 
    TRect explTxtRect = iExplanationRect;
    TInt border = iExplIconLayout.Rect().iTl.iX - iExplanationRect.iTl.iX;
    explTxtRect.Shrink( border/2 , border/2 ); //Magic: half of frame
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
// CCamShootingModeContainer::TouchLayout
// --------------------------------------------------------------------------
//
TRect CCamShootingModeContainer::TouchLayout()
    {
    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane,
                                       statusPaneRect );
    iLayoutAreaRect.BoundingRect( statusPaneRect );
    
    // get the rect size for listbox in the layout
    TAknLayoutRect wholeListboxLayoutRect;
    wholeListboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                      AknLayoutScalable_Apps::main_cset_listscroll_pane( 3 ) );
    
    // get layout for the listbox rect
    TInt listBoxHeight = iListBox->CalcHeightBasedOnNumOfItems( 
                                          iListBox->Model()->NumberOfItems() );
    if ( wholeListboxLayoutRect.Rect().Height() < listBoxHeight )
        {
        // if scrollbars are used, use scrollbar layout
        iListboxLayoutRect.LayoutRect( wholeListboxLayoutRect.Rect(), 
                            AknLayoutScalable_Apps::main_cset_list_pane( 0 ) );
        AknLayoutUtils::LayoutVerticalScrollBar( iListBox->ScrollBarFrame(), 
                            wholeListboxLayoutRect.Rect(),
                            AknLayoutScalable_Apps::scroll_pane_cp028( 0 ) );
        }
    else
        {
        iListboxLayoutRect.LayoutRect( wholeListboxLayoutRect.Rect(), 
                            AknLayoutScalable_Apps::main_cset_list_pane( 1 ) ); 
        }
    
    // text for the listbox title
    iTitleTextRectLayout.LayoutText( iLayoutAreaRect, 
                           AknLayoutScalable_Apps::main_cam_set_pane_t1( 3 ) );
    
    return wholeListboxLayoutRect.Rect();
    }

// --------------------------------------------------------------------------
// CCamShootingModeContainer::NonTouchLayout (Scene mode)
// --------------------------------------------------------------------------
//
TRect CCamShootingModeContainer::NonTouchLayout()
    {
    TAknLayoutRect wholeListboxLayoutRect;
    wholeListboxLayoutRect.LayoutRect( iLayoutAreaRect, 
                     AknLayoutScalable_Apps::main_cset6_listscroll_pane( 3 ) );
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect( iLayoutAreaRect, 
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
    explParent.LayoutRect( iLayoutAreaRect,
                          AknLayoutScalable_Apps::main_cset6_text2_pane( 0 ) );
    TAknLayoutRect explLayoutRect;
    explLayoutRect.LayoutRect( explParent.Rect(), 
              AknLayoutScalable_Apps::bg_popup_preview_window_pane_cp03( 0 ) );
    iExplanationRect = explLayoutRect.Rect();

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
    ret.Move( 0, -titlePaneRect.Height() );
    return ret;
    }

// --------------------------------------------------------------------------
// CCamShootingModeContainer::HandleListBoxEventL 
// Handles Listbox events
// --------------------------------------------------------------------------
//

void CCamShootingModeContainer::HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
              {
              HideCurrentTooltipL();
              if ( UserSceneHighlighted() )
                  {
                  iView.HandleCommandL( ECamCmdUserSceneSelected );
                  }
              else 
                  {
                  iView.HandleCommandL( EAknSoftkeySelect );
                  }
              }
              break;  
        
        case EEventItemClicked:
            {
            ShowTooltipL();
            }
            break;
        default:
            break;
        }
    }
// End of File  
