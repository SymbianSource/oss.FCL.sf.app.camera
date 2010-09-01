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
* Description:  Setting page for CAknEnumeratedItems.*
*/



// INCLUDE FILES
#include <AknSettingItemList.h>
#include "CamContextAwareSettingPage.h"
#include "CamAppController.h"
#include "CamAppUiBase.h"
#include "camlogging.h"

// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CCamContextAwareSettingPage::CCamContextAwareSettingPage
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamContextAwareSettingPage::CCamContextAwareSettingPage( 
    const TDesC* aSettingText, 
    TInt aSettingNumber, 
    TInt aControlType,
    TInt aEditorResourceId, 
    TInt aSettingPageResourceId, 
    MAknQueryValue& aQueryValue,
    CAknEnumeratedTextPopupSettingItem& aParent,
    CCamAppController& aController )
    : CAknPopupSettingPage( aSettingText, aSettingNumber, aControlType,
    aEditorResourceId, aSettingPageResourceId, aQueryValue ), 
    iParent( aParent ), iController( aController )
    {
    }

/**
* Destructor
*/
CCamContextAwareSettingPage::~CCamContextAwareSettingPage()
  {
  PRINT( _L("Camera => ~CCamContextAwareSettingPage") );
  if ( iTimer )
    {
    iTimer->Cancel();
    delete iTimer;
    }

	TRAP_IGNORE( iController.CancelPreviewChangesL() );
  PRINT( _L("Camera <= ~CCamContextAwareSettingPage") );
  }

// ---------------------------------------------------------------------------
// CCamContextAwareSettingPage::ConstructL
// Construct the setting page
// ---------------------------------------------------------------------------
//
void CCamContextAwareSettingPage::ConstructL()
	{
    CAknPopupSettingPage::ConstructL();
    // create the timer used for callbacks
    iTimer = CPeriodic::NewL( CActive::EPriorityStandard );
	}

// ---------------------------------------------------------------------------
// CCamContextAwareSettingPage::PostDisplayCheckL
// Pushes the navicounter onto the navipane, and then calls the base class
// PostDisplayCheckL()
// ---------------------------------------------------------------------------
//
TBool CCamContextAwareSettingPage::PostDisplayCheckL()
    {
    // This appears to be the best place to push the navicounter,
    // so that it overwrites the navipane created by the 
    // CAknSettinPage::ExecuteLD()
    CCamAppUiBase* appUi = (CCamAppUiBase*)iEikonEnv->EikAppUi();
    appUi->PushNaviPaneCounterL();

    return CAknPopupSettingPage::PostDisplayCheckL();
    }

// ---------------------------------------------------------------------------
// CCamContextAwareSettingPage::UpdateSettingL
// Called when the state of the list box changes.
// ---------------------------------------------------------------------------
//
void CCamContextAwareSettingPage::UpdateSettingL()
    {
    // Determine whether the currently highlighted list item
    // is enabled or not, and update the cba.
    // ...Get the current list item index.
    TInt currentIndex = ListBoxControl()->View()->CurrentItemIndex();
    // ...Dertemine the settings model id associated with the current
    // ...list item.
    iCurrentVal = iParent.EnumeratedTextArray()->
        At( currentIndex )->EnumerationValue();
    // ...Determine whether the settings model value is enabled or disabled.
    TBool settingValueEnabled = 
        iController.SettingValueEnabled( iParent.Identifier(), iCurrentVal );
    // ...Inform the base setting page class, and update the cba
	SetDataValidity( settingValueEnabled );
	UpdateCbaL();

    // If the list item is enabled update the navi-pane to 
    // let the user know how this item would affect the available photo
    // counter.
    if ( settingValueEnabled )
        {
        if ( iTimer->IsActive() )
            {
            iTimer->Cancel();
            }
        iTimer->Start( 0, 0,  TCallBack( PreviewSettingChange, this ) );
        }
    }

// ---------------------------------------------------------------------------
// CCamContextAwareSettingPage::ProcessCommandL
// Processes events from the softkeys.
// ---------------------------------------------------------------------------
//
void CCamContextAwareSettingPage::ProcessCommandL(TInt aCommandId)
	{
	HideMenu();

	// Respond to softkey events
	switch (aCommandId)
		{
		case EAknSoftkeyOk:
		case EAknSoftkeySelect:  
			SelectCurrentItemL(); // has non-trivial implementations in listbox type 
										// controls to put the selection on the current item
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
// CCamContextAwareSettingPage::PreviewSettingChangeL
// Previews the setting value
// ---------------------------------------------------------------------------
//
TInt CCamContextAwareSettingPage::PreviewSettingChange( TAny* aAny )
    {
    CCamContextAwareSettingPage* settingPage = 
        static_cast<CCamContextAwareSettingPage*>( aAny );

    // preview the setting value - will 
    // update the engine and the navi couner
    TRAPD( ignore, settingPage->iController.PreviewSettingChangeL( 
        settingPage->iParent.Identifier(), settingPage->iCurrentVal ) );

	if(ignore)
		{
		// Lint error removal
		}

    // cancel the timer
    settingPage->iTimer->Cancel();

    return KErrNone;
    }
