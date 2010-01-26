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
* Description:  Implements container for the user scene setup list.*
*/


// INCLUDE FILES
#include "CamUserSceneSetupContainer.h"
#include "CamUserSceneSetupItemArray.h"
#include "CamAppController.h"
#include "CamSettings.hrh"
#include "Cam.hrh"
#include "CamPanic.h"
#include <aknlists.h>
#include <AknIconArray.h>
#include <barsread.h>
#include <aknview.h>

#include "CamUtility.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::NewLC
// Symbian two-phased constructor.
// ---------------------------------------------------------
//
CCamUserSceneSetupContainer* CCamUserSceneSetupContainer::NewLC
( const TRect& aRect, TInt aResourceId, 
    CCamAppController& aController, 
    CAknView& aView )
    {
    CCamUserSceneSetupContainer* self =
        new ( ELeave ) CCamUserSceneSetupContainer( aController, 
        aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aResourceId );
    return self;
    }

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::NewL
// Symbian two-phased constructor.
// ---------------------------------------------------------
//
CCamUserSceneSetupContainer* CCamUserSceneSetupContainer::NewL
( const TRect& aRect, TInt aResourceId, 
    CCamAppController& aController, 
    CAknView& aView )
    {
    CCamUserSceneSetupContainer* self = 
        CCamUserSceneSetupContainer::NewLC
        ( aRect, aResourceId, aController, 
       aView );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::~CCamUserSceneSetupContainer
// Destructor
// ---------------------------------------------------------
//
CCamUserSceneSetupContainer::~CCamUserSceneSetupContainer()
  {
  PRINT( _L("Camera => ~CCamUserSceneSetupContainer" ))  
  delete iUserSceneSetupList;
  delete iUserSceneSetupItemArray;
  PRINT( _L("Camera <= ~CCamUserSceneSetupContainer" ))  
  }

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::HandleChangeInItemArrayOrVisibilityL
// Handles a change in the item list visibility.
// ---------------------------------------------------------
//
void CCamUserSceneSetupContainer::UpdateDisplayL()
    {
    ListBox()->DrawNow();
    }

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::TitlePaneTextResourceId
// Returns the resource Id of the title for this container.
// ---------------------------------------------------------
//
TInt CCamUserSceneSetupContainer::TitlePaneTextResourceId()
    {
    return iTitleResourceId;
    }

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::UpdateListItems
// Updates the list items with the settings model values.
// ---------------------------------------------------------
//
void CCamUserSceneSetupContainer::UpdateListItems( void )
    {
    TInt listItemCount = iUserSceneSetupItemArray->Count();
    TInt i;
    for ( i = 0; i < listItemCount; ++i )
        {
        UpdateListItem( i );
        }
    }

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::UpdateCurrentListItem
// Updates the current list item with it's associated
// settings model value.
// ---------------------------------------------------------
//
void CCamUserSceneSetupContainer::UpdateCurrentListItem()
    {
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();   
    if ( cba )
        {
        CEikCba* eikCba = static_cast<CEikCba*>( cba->ButtonGroup() );
        if( eikCba )
            {
            // Hide LSK for single clickable settings
            eikCba->EnableItemSpecificSoftkey( ETrue );
            }
        }  
    TInt selectedItemIndex = iUserSceneSetupList->CurrentItemIndex();
    UpdateListItem( selectedItemIndex );
    }
    
// ---------------------------------------------------------
// CCamUserSceneSetupContainer::HandleListBoxEventL
// Handles a list box event.
// ---------------------------------------------------------
//
void CCamUserSceneSetupContainer::HandleListBoxEventL
( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
	{
	switch( aEventType )
		{
		case EEventEnterKeyPressed: // fallthrough
		case EEventItemDoubleClicked: 
        case EEventItemSingleClicked:
			{
            HandleSelectionL();
            }
			break;

		default:
			break;			
		}
	}

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::HandleSelectionL
// Handles the user selection of a list item.
// ---------------------------------------------------------
//
void CCamUserSceneSetupContainer::HandleSelectionL( void )
    {
    TInt CommandId = CommandIdForActivatingCurrentItemControl();
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    if ( cba && CommandId != ECamCmdCaptureSetupFlashUser )
        {
        CEikCba* eikCba = static_cast<CEikCba*>( cba->ButtonGroup() );
        if( eikCba )
            {
            // Make LSK visible for the sliders.
            // Sliders, like exposure compensation, are not listboxes, 
            // thus single click cannot be disabled for those.
            // All setting items, except flash, are double clickable and
            // they have a visible LSK
            eikCba->EnableItemSpecificSoftkey( EFalse );
            }    
        }
    iView.HandleCommandL( CommandId );
    }
    


// ---------------------------------------------------------
// CCamUserSceneSetupContainer::CCamUserSceneSetupContainer
// C++ constructor
// ---------------------------------------------------------
//
CCamUserSceneSetupContainer::CCamUserSceneSetupContainer
( CCamAppController& aController, 
    CAknView& aView )
: CCamContainerBase( aController, aView )
    {

    }

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------
//
void CCamUserSceneSetupContainer::ConstructL
( const TRect& aRect, TInt aResourceId )
    {
    const TInt KSettingItemArrayGranularity = 5;

    CreateWindowL();

    // Create the listbox in the right style
    iUserSceneSetupList = new ( ELeave ) CAknSettingStyleListBox;
    iUserSceneSetupList->ConstructL( this, CEikListBox::ELeftDownInViewRect );

    TPoint pos = iEikonEnv->EikAppUi()->ClientRect().iTl;
    TSize size = iUserSceneSetupList->MinimumSize();
    SetExtent( pos, size ); 


    // Setup the scroll bar
    iUserSceneSetupList->CreateScrollBarFrameL( ETrue );
    iUserSceneSetupList->ScrollBarFrame()->
    SetScrollBarVisibilityL( CEikScrollBarFrame::EOn, 
            CEikScrollBarFrame::EAuto );
            
    iTitleResourceId = aResourceId;

            
    // get array of supported resources from psi
    RArray<TInt> userSceneResourceIds;
    CleanupClosePushL( userSceneResourceIds );
    
    User::LeaveIfError( CamUtility::GetPsiIntArrayL( ECamPsiUserSceneResourceIds, userSceneResourceIds ) );

    // ...get the number of items in this user scene setup list.
    TInt itemCount = userSceneResourceIds.Count(); 
	iUserSceneSetupItemArray = new ( ELeave ) 
        CCamUserSceneSetupItemArray( itemCount );
    iUserSceneSetupItemArray->ConstructL();
	
	// Make the icon array
	CArrayPtrFlat<CGulIcon>* iconArray = new ( ELeave )
        CAknIconArray( KSettingItemArrayGranularity );
	CleanupStack::PushL( iconArray );

    // Create all the user scene setup list items defined in the resource.
	TInt i;
	for ( i=0; i < itemCount; ++i )
		{
        CCamUserSceneSetupListItem* listItem = 
            CCamUserSceneSetupListItem::NewL( userSceneResourceIds[i] );
        CleanupStack::PushL( listItem );
        // Ownership is transferred
        iUserSceneSetupItemArray->AppendL( listItem ); 
        CleanupStack::Pop( listItem );
		}
		
	
	// Update the display
	UpdateDisplayL();

	// Looks like we have no choice but to give ownership 
    // of this to the listbox.
	// There is no flag as there is for the item data
	ListBox()->ItemDrawer()->FormattedCellData()->SetIconArrayL( iconArray ); 

    // iconArray now has ownership transferred
	CleanupStack::Pop( iconArray );  
    
    CleanupStack::PopAndDestroy( &userSceneResourceIds );	

	CTextListBoxModel* model= iUserSceneSetupList->Model();
	model->SetItemTextArray( iUserSceneSetupItemArray );
    // Ownership retained by SettingItemList
	model->SetOwnershipType( ELbmDoesNotOwnItemArray ); 

   	TAknLayoutRect layoutRect;
	layoutRect.LayoutRect( TRect( 
        TPoint( 0, 0 ), iAvkonAppUi->ClientRect().Size() ), 
        AKN_LAYOUT_WINDOW_list_gen_pane( 0 ) );
	iUserSceneSetupList->SetRect( layoutRect.Rect() );

	ListBox()->SetCurrentItemIndex( 0 );
	ListBox()->SetListBoxObserver( this );
	ListBox()->UpdateScrollBarsL();

    SetRect( aRect );
    ActivateL();

    UpdateListItems();
    }



// ---------------------------------------------------------
// CCamUserSceneSetupContainer::UpdateListItem
// Updates a list item with its corresponding settings model value.
// ---------------------------------------------------------
//
void CCamUserSceneSetupContainer::UpdateListItem( TInt aIndex )
    {     
    // Get the current value for this list item from the settings model.
    // ...get settings model id for list item.
    TInt settingsModelId = 
            iUserSceneSetupItemArray->At( aIndex )->SettingsModelTypeId();
    if ( settingsModelId != ECamSettingItemUserSceneReset )
        {
        // ...use id to get current value.
        TInt currentVal = iController.IntegerSettingValue( settingsModelId );
    
        // Update list item with the current value.
        iUserSceneSetupItemArray->At( aIndex )->SetCurrentValue( currentVal );
        }
    }


// ----------------------------------------------------
// CCamUserSceneSetupContainer::CommandIdForActivatingCurrentItemControl
// Returns the command id for activating the current list item control.
// ----------------------------------------------------
//
TInt CCamUserSceneSetupContainer::CommandIdForActivatingCurrentItemControl()
    {
    // Get the settings model type id for the selected item.
    // ...get settings model id for list item.  
    TInt currentIndex = iUserSceneSetupList->CurrentItemIndex();
    
    // ...get settings model id for list item.
    TInt settingsModelId = 
        iUserSceneSetupItemArray->At( currentIndex )->
            SettingsModelTypeId();
    // Convert settings model id to command id that activates the
    // control for the selected item.
    TInt commandId = -1;
    switch ( settingsModelId )
        {
        case ECamSettingItemUserSceneBasedOnScene:
            {
            commandId = ECamCmdCaptureSetupSceneUser;
            }
            break;
        case ECamSettingItemUserSceneWhitebalance:
            {
            commandId = ECamCmdCaptureSetupWhiteBalanceUser;
            }
            break;
        case ECamSettingItemUserSceneExposure:
            {
            commandId = ECamCmdCaptureSetupExposureUser;
            }
            break;
        case ECamSettingItemUserSceneColourFilter:
            {
            commandId = ECamCmdCaptureSetupColourFilterUser;
            }
            break;
        case ECamSettingItemUserSceneFlash:
            {
            commandId = ECamCmdCaptureSetupFlashUser;
            }
            break;
        case ECamSettingItemUserSceneReset:
            {
            commandId = ECamCmdUserSceneReset;
            }
            break;
        case ECamSettingItemUserSceneBrightness:
            {
            commandId = ECamCmdCaptureSetupBrightnessUser;
            }
            break;
        case ECamSettingItemUserSceneContrast:
            {
            commandId = ECamCmdCaptureSetupContrastUser;
            }
            break;

        case ECamSettingItemUserSceneImageSharpness:
            {
            commandId = ECamCmdCaptureSetupImageSharpnessUser;
            }
            break;

        case ECamSettingItemUserSceneLightSensitivity:
            {
            commandId = ECamCmdCaptureSetupLightSensitivityUser;
            }
            break;
        default:
            {
            CamPanic( ECamPanicCaptureSetupMenuUnknownItem );
            }
        }
    return commandId;
    }



// ---------------------------------------------------------
// CCamUserSceneSetupContainer::CountComponentControls
// ---------------------------------------------------------
//
TInt CCamUserSceneSetupContainer::CountComponentControls() const
    {
	return 1;
	}

// ---------------------------------------------------------
// CCamUserSceneSetupContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CCamUserSceneSetupContainer::ComponentControl
( TInt aIndex ) const
    {
    switch( aIndex )
        {
        case 0:
            return iUserSceneSetupList;
        default:
            return NULL;            
        }
        
    // Should never get here
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupContainer::OfferKeyEventL
// Key event handling
// ---------------------------------------------------------------------------
//
TKeyResponse CCamUserSceneSetupContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
	return iUserSceneSetupList->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupContainer::ListBox
// Returns a pointer to the list box
// ---------------------------------------------------------------------------
//
CEikFormattedCellListBox* CCamUserSceneSetupContainer::ListBox()
	{
	return ( iUserSceneSetupList );
	}

// ---------------------------------------------------------------------------
// CCamUserSceneSetupContainer::IsUserSceneResetHighlighted
// Returns wheather user scene restore is highlighted
// ---------------------------------------------------------------------------
//
TBool CCamUserSceneSetupContainer::IsUserSceneResetHighlighted()
	{
    TInt currentIndex = iUserSceneSetupList->CurrentItemIndex();
    // ...get settings model id for list item.
    TInt settingsModelId = 
        iUserSceneSetupItemArray->At( currentIndex )->SettingsModelTypeId();
    if ( settingsModelId == ECamSettingItemUserSceneReset )
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }
	}

// End of File  
