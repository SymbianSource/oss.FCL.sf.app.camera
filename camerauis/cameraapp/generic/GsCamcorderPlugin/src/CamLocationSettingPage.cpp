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
* Description:  Setting page for location setting in Image&Video Settings.*
*/



// INCLUDE FILES
#include <gscamerapluginrsc.rsg>
#include <aknsettingitemlist.h>
#include <barsread.h>
#include "CamUtility.h"
#include "CamLocationSettingPage.h"
#include "MCamAppController.h"
#include "CamLocationSettingContainer.h"
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>

// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CCamLocationSettingPage::CCamLocationSettingPage
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamLocationSettingPage::CCamLocationSettingPage( 
    TCamCameraMode aMode, 
    TDesC& aSettingTitleText,
    TInt aSettingNumber, 
    TInt aControlType,
    TInt aEditorResourceId, 
    TInt aSettingPageResourceId,
    MAknQueryValue& aQueryValue,
    MCamAppController& aController )
    : CAknSettingPage(	&aSettingTitleText, 
						aSettingNumber, 
						aControlType,
						aEditorResourceId, 
						aSettingPageResourceId ),
      iQueryValue( aQueryValue ),
      iController( aController ),
      iMode( aMode )
    {
    }
// ---------------------------------------------------------------------------
// CCamLocationSettingPage::~CCamLocationSettingPage
// C++ destructor
// ---------------------------------------------------------------------------
//
CCamLocationSettingPage::~CCamLocationSettingPage()
  {
  PRINT( _L("Camera => ~CCamLocationSettingPage") );
  delete iLocationContainer;
  
  if ( iTimer && iTimer->IsActive() )
    {
    iTimer->Cancel();
    }
  delete iTimer;
  
  TRAP_IGNORE( iController.CancelPreviewChangesL() );
  PRINT( _L("Camera <= ~CCamLocationSettingPage") );
  }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::ConstructL
// Construct the setting page
// ---------------------------------------------------------------------------
//
void CCamLocationSettingPage::ConstructL()
	{
    // create the timer used for callbacks
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    // this needs to be created before BaseConstructL as it sets the
    // size of the setting page which will result in SizeChanged being called
    iLocationContainer = CCamLocationSettingContainer::NewL( iController, 
                                                             iMode, 
                                                             this );
	BaseConstructL();
	}

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::UpdateSettingL
// Called when the state of location setting changes
// ---------------------------------------------------------------------------
//
void CCamLocationSettingPage::UpdateSettingL()
    {
    TBool settingValueEnabled = ETrue;
    iCurrentVal = iLocationContainer->SelectedValue();
    iSettingItemId = ECamSettingItemRecLocation;
    // Inform the base setting page class
	SetDataValidity( settingValueEnabled );
    UpdateCbaL();
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::ProcessCommandL
// Processes events from the softkeys.
// ---------------------------------------------------------------------------
//
void CCamLocationSettingPage::ProcessCommandL( TInt aCommandId )
	{
	HideMenu();

	// Respond to softkey events
	switch ( aCommandId )
		{
		case EAknSoftkeyOk:
		case EAknSoftkeySelect:  
			SelectCurrentItemL(); 
			AttemptExitL( ETrue );
            iController.CommitPreviewChanges();
			break;
		case EAknSoftkeyCancel:
			AttemptExitL( EFalse );
            iController.CancelPreviewChangesL();
			break;
		default:
			break;
		}
	}

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::PostDisplayCheckL
// Customises some of the functionality so that the custom control
// is used instead of the base classes editor control
// ---------------------------------------------------------------------------
//
TBool CCamLocationSettingPage::PostDisplayCheckL()
    {
    // don't use the editor control in the base class
    // use the custom control instead.
    iLocationContainer->SetObserver( this );
    return CAknSettingPage::PostDisplayCheckL();
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::SelectCurrentItemL
// Uses the query value to inform the setting item that
// a new value has been selected
// ---------------------------------------------------------------------------
//
void CCamLocationSettingPage::SelectCurrentItemL()
	{
	iQueryValue.SetCurrentValueIndex( iLocationContainer->SelectedValue() );
	} 

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::ConstructFromResourceL
// Constructs the quality container from the resource file
// ---------------------------------------------------------------------------
//
void CCamLocationSettingPage::ConstructFromResourceL( TResourceReader& aReader )
    {
    // setting page window is created in the base class
    CAknSettingPage::ConstructFromResourceL( aReader );

    TResourceReader reader;
	iLocationContainer->ConstructFromResourceL( reader );
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::CountComponentControls
// Returns the number of component controls
// ---------------------------------------------------------------------------
//
TInt CCamLocationSettingPage::CountComponentControls() const
    {
    return 1; // the location container
    }
 
// ---------------------------------------------------------------------------
// CCamLocationSettingPage::ComponentControl
// Returns the control at the given index
// ---------------------------------------------------------------------------
//
CCoeControl* CCamLocationSettingPage::ComponentControl( TInt /*aIndex*/ ) const
    {
    return iLocationContainer;
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::Draw
// Draws the setting page
// ---------------------------------------------------------------------------
//
void CCamLocationSettingPage::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( Rect() );
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::SizeChanged
// Handles a change in the size of the page
// ---------------------------------------------------------------------------
//
void CCamLocationSettingPage::SizeChanged()
    {
    CAknSettingPage::SizeChanged();

    if ( AknLayoutUtils::PenEnabled() ) 
        {
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            AknLayoutUtils::LayoutControl( iLocationContainer, Rect(), AknLayoutScalable_Apps::main_cam_set_pane_g1( 7 ) );
            }
        else
            {
            AknLayoutUtils::LayoutControl( iLocationContainer, Rect(), AknLayoutScalable_Apps::main_cam_set_pane_g1( 8 ) );
            }
        }
    else
        {
        AknLayoutUtils::LayoutControl( iLocationContainer, Rect(), AknLayoutScalable_Apps::main_cam6_set_pane( 0 ) );
        }
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::OfferKeyEventL
// Handles the key event
// ---------------------------------------------------------------------------
//
TKeyResponse CCamLocationSettingPage::OfferKeyEventL( 
                                            const TKeyEvent& aKeyEvent, 
                                            TEventCode aType 
                                            ) 
    {
    // Always react to Escape key by cancelling the setting page
	if ( aType == EEventKey && aKeyEvent.iCode == EKeyEscape)
		{
		ProcessCommandL( EAknSoftkeyCancel );
		return EKeyWasConsumed;
		}

	// Only handle other key events if we're focused
	if ( IsFocused() )
		{
		// Abstraction of key events: Escape is handled like Cancel 
		// OK key is handled like the Select softkey by default
		if ( aType == EEventKey && aKeyEvent.iCode == EKeyOK )
			{
			if ( aKeyEvent.iRepeats != 0 ) 
                {
                return EKeyWasConsumed;
                }
			if ( DataValidity() )
                {
                ProcessCommandL( EAknSoftkeySelect );
                }
            return EKeyWasConsumed;
            }
		}

    // offer the key to the quality container class
    // if not handled by the setting page
    return iLocationContainer->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingPage::PreviewSettingChangeL
// Previews the setting value
// ---------------------------------------------------------------------------
//
TInt CCamLocationSettingPage::PreviewSettingChange( TAny* aAny )
    {
    CCamLocationSettingPage* settingPage = 
        static_cast<CCamLocationSettingPage*>( aAny );
    // preview the setting value - will 
    // update the engine and the navi couner
    TRAPD( ignore, settingPage->iController.PreviewSettingChangeL( 
                                                settingPage->iSettingItemId, 
                                                settingPage->iCurrentVal ) );       
	if(ignore)
		{
		// Lint error removal
		}
		
    // cancel the timer
    settingPage->iTimer->Cancel();

    return KErrNone;
    }
//End of File
