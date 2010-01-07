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
* Description:  Setting page for Image/Video quality.*
*/



// INCLUDE FILES
#include <gscamerapluginrsc.rsg>
#include <aknsettingitemlist.h>
#include <barsread.h>
#include "GSCamQualitySettingPage.h"
#include "GSCamQualitySettingContainer.h"
#include "CamUtility.h"
#include "MCamAppController.h"
#include "CamAppUiBase.h"
#include <aknlayoutscalable_apps.cdl.h>


// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::CGSCamQualitySettingPage
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamQualitySettingPage::CGSCamQualitySettingPage( 
    TCamCameraMode     aMode, 
    TDesC&             aSettingTitleText,
    TInt               aSettingNumber, 
    TInt               aControlType,
    TInt               aEditorResourceId, 
    TInt               aSettingPageResourceId,
    MAknQueryValue&    aQueryValue )
  : CAknSettingPage( &aSettingTitleText, 
                     aSettingNumber, 
                     aControlType,
                     aEditorResourceId, 
                     aSettingPageResourceId ),
    iQueryValue( aQueryValue ),
    iController( static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() )->AppController() ),
    iMode( aMode )
  {
  }
// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::~CGSCamQualitySettingPage
// C++ destructor
// ---------------------------------------------------------------------------
//
CGSCamQualitySettingPage::~CGSCamQualitySettingPage()
  {
  PRINT( _L("Camera => ~CGSCamQualitySettingPage") );
  delete iQualityContainer;
  
  if ( iTimer && iTimer->IsActive() )
    {
    iTimer->Cancel();
    }
  delete iTimer;
  
  TRAP_IGNORE( iController.CancelPreviewChangesL() );
  PRINT( _L("Camera <= ~CGSCamQualitySettingPage") );
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::ConstructL
// Construct the setting page
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingPage::ConstructL()
	{
    // create the timer used for callbacks
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    // this needs to be created before BaseConstructL as it sets the
    // size of the setting page which will result in SizeChanged being called
    iQualityContainer = CGSCamQualitySettingContainer::NewL( iController,
                                                             iMode,
                                                             this );
	BaseConstructL();

	}

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::UpdateSettingL
// Called when the state of quality control changes
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingPage::UpdateSettingL()
  {
  TBool settingValueEnabled = ETrue;
  iCurrentVal = iQualityContainer->SelectedQuality();
  
  if ( ECamControllerVideo == iMode )
    {
    iSettingItemId = ECamSettingItemVideoQuality;       
    }
  else
    {
    iSettingItemId = ECamSettingItemPhotoQuality;
    }
        
  // Inform the base setting page class
  SetDataValidity( settingValueEnabled );
  UpdateCbaL();

  // If the list item is enabled update the image/video count to
  // let the user know how this item would affect the remaining images/videos
  if ( settingValueEnabled )
    {
    if ( iTimer->IsActive() )
      {
      iTimer->Cancel();
      }
    iTimer->Start( 10*1000,
                   10*1000,
                   TCallBack( PreviewSettingChange, this ) );
    }
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::ProcessCommandL
// Processes events from the softkeys.
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingPage::ProcessCommandL( TInt aCommandId )
	{
	HideMenu();

	// Respond to softkey events
	switch ( aCommandId )
		{
		case EAknSoftkeyOk:
		case EAknSoftkeySelect:  
			SelectCurrentItemL(); 
			AttemptExitL( ETrue );
			iCurrentVal = iQualityContainer->SelectedQuality();
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
// CGSCamQualitySettingPage::PostDisplayCheckL
// Customises some of the functionality so that the custom quality control
// is used instead of the base classes editor control
// ---------------------------------------------------------------------------
//
TBool CGSCamQualitySettingPage::PostDisplayCheckL()
    {
    // don't use the editor control in the base class
    // use the quality control instead.
    iQualityContainer->SetObserver( this );
    return CAknSettingPage::PostDisplayCheckL();
    }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::SelectCurrentItemL
// Uses the query value to inform the setting item that
// a new quality has been selected
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingPage::SelectCurrentItemL()
	{
	iQueryValue.SetCurrentValueIndex( iQualityContainer->SelectedQuality() );
	} 

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::ConstructFromResourceL
// Constructs the quality container from the resource file
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingPage::ConstructFromResourceL( TResourceReader& aReader )
  {
  // setting page window is created in the base class
  CAknSettingPage::ConstructFromResourceL( aReader );

  TInt resourceId = ( ECamControllerVideo == iMode )
                    ? R_CAM_SETTING_VIDEO_QUALITY_SLIDER
                    : R_CAM_SETTING_PHOTO_QUALITY_SLIDER;

  TResourceReader reader;
  iCoeEnv->CreateResourceReaderLC( reader, resourceId );
  iQualityContainer->ConstructFromResourceL( reader );
  CleanupStack::PopAndDestroy(); //reader 
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::CountComponentControls
// Returns the number of component controls
// ---------------------------------------------------------------------------
//
TInt CGSCamQualitySettingPage::CountComponentControls() const
  {
  return 1; // the quality container
  }
 
// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::ComponentControl
// Returns the control at the given index
// ---------------------------------------------------------------------------
//
CCoeControl* CGSCamQualitySettingPage::ComponentControl( TInt /*aIndex*/ ) const
  {
  return iQualityContainer;
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::Draw
// Draws the setting page
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingPage::Draw( const TRect& /*aRect*/ ) const
  {
  CWindowGc& gc = SystemGc();
  gc.SetPenStyle( CGraphicsContext::ENullPen );
  gc.SetBrushColor( KRgbWhite );
  gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
  gc.DrawRect( Rect() );
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::SizeChanged
// Handles a change in the size of the page
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingPage::SizeChanged()
  {
  CAknSettingPage::SizeChanged();

  AknLayoutUtils::LayoutControl( iQualityContainer, Rect(), AknLayoutScalable_Apps::main_cam_set_pane_g1( 6 ) );
  
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::OfferKeyEventL
// Handles the key event
// ---------------------------------------------------------------------------
//
TKeyResponse 
CGSCamQualitySettingPage::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                          TEventCode aType ) 
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
    return iQualityContainer->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingPage::PreviewSettingChangeL
// Previews the setting value
// ---------------------------------------------------------------------------
//
TInt CGSCamQualitySettingPage::PreviewSettingChange( TAny* aAny )
    {
    CGSCamQualitySettingPage* settingPage = 
        static_cast<CGSCamQualitySettingPage*>( aAny );
    // preview the setting value - will 
    // update the engine and the navi couner    
    TRAP_IGNORE( settingPage->iController.PreviewSettingChangeL( 
                                                settingPage->iSettingItemId, 
                                                settingPage->iCurrentVal ) );

    // cancel the timer
    settingPage->iTimer->Cancel();

    return KErrNone;
    }
