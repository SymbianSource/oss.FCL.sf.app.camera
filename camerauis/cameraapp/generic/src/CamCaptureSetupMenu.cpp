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
* Description:  Container for the capture setup menu list box.*
*/



// INCLUDE FILES
#include <barsread.h>
#include <AknsFrameBackgroundControlContext.h>
#include <aknview.h>
#include <AknUtils.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamCaptureSetupMenu.h"
#include "CamCaptureSetupMenuListBox.h"
#include "CamAppController.h"
#include "CamPanic.h"
#include "CamSettings.hrh"
#include "Cam.hrh"
#include "camlogging.h"

const TInt KNumComponentComtrols = 1;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCamCaptureSetupMenu::NewL
// Two-phased constructor.
// ---------------------------------------------------------
//
CCamCaptureSetupMenu* CCamCaptureSetupMenu::NewL( 
    CCamAppController& aController,
    CAknView& aView,
    const TRect& aRect,
    TInt aResourceId,
    TInt aSelectedItemIndex )
    {
    CCamCaptureSetupMenu* self = new( ELeave ) CCamCaptureSetupMenu
        ( aController, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aResourceId, aSelectedItemIndex );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CCamCaptureSetupMenu::~CCamCaptureSetupMenu()
  {
  PRINT( _L("Camera => ~CCamCaptureSetupMenu") );
  delete iCaptureSetupListBox;
  // iBgContext is deleted in base class
  PRINT( _L("Camera <= ~CCamCaptureSetupMenu") );
  }

// ---------------------------------------------------------
// Returns the list index of the currently selected item.
// ---------------------------------------------------------
//
TInt CCamCaptureSetupMenu::CurrentItemIndex() const
    {
    return iCaptureSetupListBox->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupMenu::FadeBehind
// Fades everything behind this menu.
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupMenu::FadeBehind( TBool aFade )
    {
    iPopupFader.FadeBehindPopup( this, this, aFade );
    }


// ---------------------------------------------------------------------------
// CCamCaptureSetupMenu::CountFadedComponents
// Returns the number of components that are not faded
// when the fader object is active.
// ---------------------------------------------------------------------------
//
TInt CCamCaptureSetupMenu::CountFadedComponents()
    {
    return KNumComponentComtrols;
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupMenu::FadedComponent
// Returns a pointer to a non-faded component.
// ---------------------------------------------------------------------------
//
CCoeControl* CCamCaptureSetupMenu::FadedComponent(TInt aIndex)
    {
	switch (aIndex)
		{
		case 0:
			return this;
		default:
			return NULL;
        }
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupMenu::HandleCommandL
// Handle commands
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupMenu::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeySelect:
            {
            HandleSelectionL();
            break;
            }
        default:
            {
            CamPanic( ECamPanicUnhandledCommand );
            break;
            }
        }
    }



// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CCamCaptureSetupMenu::CCamCaptureSetupMenu( CCamAppController& aController,
                          CAknView& aView )
: CCamContainerBase( aController, aView )
    {
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenu::ConstructL()
// 2nd phase constructor
// ---------------------------------------------------------
//
void CCamCaptureSetupMenu::ConstructL( const TRect& aRect, TInt aResourceId, TInt aSelectedItemIndex )
    {
    CreateWindowL();
    TRect listBoxRect = aRect;
	CreateCaptureSetupListBoxL( listBoxRect, aResourceId, aSelectedItemIndex );
    SetRect( listBoxRect );
    // set up the skin background context
    iBgContext = CAknsFrameBackgroundControlContext::NewL( 
            KAknsIIDQsnFrPopup, Rect(), Rect(), EFalse );
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenu::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CCamCaptureSetupMenu::CountComponentControls() const
    {
	// this control contains a listbox
    return 1; 
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenu::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CCamCaptureSetupMenu::ComponentControl(TInt /*aIndex*/) const
    {
	return iCaptureSetupListBox;
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenu::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CCamCaptureSetupMenu::Draw(const TRect& /*aRect*/) const
    {
    }

// ----------------------------------------------------
// CCamCaptureSetupMenu::OfferKeyEventL
// Handles this application view's command keys. Forwards
// other keys to child control(s).
// ----------------------------------------------------
//
TKeyResponse CCamCaptureSetupMenu::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    // now it's iListBox's job to process the key event
    TKeyResponse response = iCaptureSetupListBox->OfferKeyEventL( aKeyEvent, aType );
    // If it hasn't been processed pass up to the base class.
    if ( response == EKeyWasNotConsumed )
        {
        return CCamContainerBase::OfferKeyEventL( aKeyEvent, aType );
        }

    iController.StartIdleTimer();
    return response;
    }

// ----------------------------------------------------
// CCamCaptureSetupMenu::CreateCaptureSetupListBoxL
// Creates the list box control.
// ----------------------------------------------------
//
void 
CCamCaptureSetupMenu::CreateCaptureSetupListBoxL( TRect& aRect, TInt aResourceId, TInt aSelectedItemIndex )
	{
	PRINT( _L("Camera => CCamCaptureSetupMenu::CreateCaptureSetupListBoxL") );
    // Determine the complete drawing rectangle.
    TRect appUiRect = aRect;
    appUiRect.iTl.iX = 0;
    appUiRect.iTl.iY = 0;

    // Create the resource reader for this list box.
    TResourceReader reader;                                                                                     
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId );    

    // Read the layout for the list box.
    TAknLayoutRect listBoxLayoutRect;
    listBoxLayoutRect.LayoutRect( aRect, reader );

    // Create the listbox.
    iCaptureSetupListBox = CCamCaptureSetupMenuListBox::NewL
        ( this, reader, iController, aSelectedItemIndex );

    aRect = listBoxLayoutRect.Rect();

    // Set the layout for the list box control.
    const AknLayoutUtils::SAknLayoutControl listboxLayout =
        { ELayoutEmpty, 0, 0, ELayoutEmpty, ELayoutEmpty, aRect.Width(), aRect.Height() };
    AknLayoutUtils::LayoutControl( iCaptureSetupListBox, appUiRect, listboxLayout );

    // Set the height of a list item
    TInt listItemHeight = reader.ReadInt16();
    iCaptureSetupListBox->SetItemHeightL( listItemHeight );

    iCaptureSetupListBox->MakeVisible( ETrue );
    iCaptureSetupListBox->SetFocus( EFalse, EDrawNow );

    CleanupStack::PopAndDestroy(); // reader
	PRINT( _L("Camera <= CCamCaptureSetupMenu::CreateCaptureSetupListBoxL") );
	}

// ----------------------------------------------------
// CCamCaptureSetupMenu::CommandIdForActivatingCurrentItemControl
// Returns the command id for activating the current list item control.
// ----------------------------------------------------
//
TInt CCamCaptureSetupMenu::CommandIdForActivatingCurrentItemControl()
    {
    // Get the index of the currently selected item.
    TInt selectedItemIndex = iCaptureSetupListBox->CurrentItemIndex();
    // Get the setting model id for the currently selected item.
    TInt selectedItemSettingsId = iCaptureSetupListBox->ItemSettingsId( selectedItemIndex );
    // Convert settings model id to command id that activates the
    // control for the selected item.
    TInt commandId = -1;
    switch ( selectedItemSettingsId )
        {
        case ECamSettingItemDynamicPhotoScene:
            commandId = ECamCmdCaptureSetupSceneStill;
            break;
        case ECamSettingItemDynamicPhotoWhiteBalance:
            commandId = ECamCmdCaptureSetupWhiteBalanceStill;
            break;

        case ECamSettingItemDynamicPhotoExposure:
            commandId = ECamCmdCaptureSetupExposureStill;
            break;

        case ECamSettingItemDynamicPhotoColourFilter:
            commandId = ECamCmdCaptureSetupColourFilterStill;
            break;
        case ECamSettingItemDynamicPhotoFlash:
            commandId = ECamCmdCaptureSetupFlashStill;
            break;
        case ECamSettingItemDynamicVideoScene:
            commandId = ECamCmdCaptureSetupSceneVideo;
            break;
        case ECamSettingItemDynamicVideoWhiteBalance:
            commandId = ECamCmdCaptureSetupWhiteBalanceVideo;
            break;
        case ECamSettingItemDynamicVideoColourFilter:
            commandId = ECamCmdCaptureSetupColourFilterVideo;
            break;
        case ECamSettingItemDynamicPhotoBrightness:
            commandId = ECamCmdCaptureSetupBrightnessStill;
            break;
        case ECamSettingItemDynamicPhotoContrast:
            commandId = ECamCmdCaptureSetupContrastStill;
            break;
        case ECamSettingItemDynamicVideoBrightness:
            commandId = ECamCmdCaptureSetupBrightnessVideo;
            break;
        case ECamSettingItemDynamicVideoContrast:
            commandId = ECamCmdCaptureSetupContrastVideo;
            break;
        case ECamSettingItemDynamicPhotoImageSharpness:
            commandId = ECamCmdCaptureSetupImageSharpnessStill;
            break;
        default:
            CamPanic( ECamPanicCaptureSetupMenuUnknownItem );
        }
    return commandId;
    }

// ----------------------------------------------------
// CCamCaptureSetupMenu::HandleSelectionL
// Handles the pressing of the softkey selection.
// ----------------------------------------------------
//
void CCamCaptureSetupMenu::HandleSelectionL()
    {
    TInt CommandId = CommandIdForActivatingCurrentItemControl();
    iView.HandleCommandL( CommandId );
    }


// End of File  
