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
* Description:  Encapsulates the capture setup menu list box.*
*/


// INCLUDE FILES
#include <barsread.h>
#include <eikenv.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamCaptureSetupMenuListBox.h"
#include "CamCaptureSetupMenuListBoxModel.h"
#include "CamCaptureSetupMenuListItemDrawer.h"
#include "camlogging.h"


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// NewL
// Two-phased constructor.
// Returns: CCamCaptureSetupMenuListBox*: Pointer to the created list item.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupMenuListBox* CCamCaptureSetupMenuListBox::NewL( 
    const CCoeControl* aParent,         // container for this list box
    TResourceReader& aReader,            // used to populate the list box
    CCamAppController& aController, // used by the list box model to get list item values
    TInt aSelectedItemIndex )
    {
    CCamCaptureSetupMenuListBox* self = new( ELeave ) CCamCaptureSetupMenuListBox;
    CleanupStack::PushL( self );
    self->ConstructL( aParent, aReader, aController,aSelectedItemIndex );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCamCaptureSetupMenuListBox::~CCamCaptureSetupMenuListBox()
	{
	}

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBox::ItemSettingsId()
// Returns the settings model id that corresponds with the
// specified list item index.
// ---------------------------------------------------------
//
TInt CCamCaptureSetupMenuListBox::ItemSettingsId( TInt aItemIndex )
    {
    return static_cast<CCamCaptureSetupMenuListBoxModel*>( iModel )
        ->SettingsModelIdAssociatedWithItem( aItemIndex );
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBox::MakeViewClassInstanceL()
// This method creates instance of CCamCaptureSetupMenuListBoxView
// ---------------------------------------------------------
//
CListBoxView* CCamCaptureSetupMenuListBox::MakeViewClassInstanceL()
	{
	return new( ELeave ) CListBoxView;
	}

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBox::OfferKeyEventL
// Handles a user pressing the key.
// ---------------------------------------------------------
//
TKeyResponse CCamCaptureSetupMenuListBox::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, // The key event
    TEventCode aType)           // The type of key event
	{
    // If the navikey select has been pressed don't process it.
    if ( aType == EEventKey && aKeyEvent.iCode == EKeyOK )
        {
        return EKeyWasNotConsumed; // indicate that not been processed.
        }

	return CEikListBox::OfferKeyEventL( aKeyEvent, aType );
	}

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CCamCaptureSetupMenuListBox::CCamCaptureSetupMenuListBox( )
	{
	}

// ---------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------
//
void CCamCaptureSetupMenuListBox::ConstructL(
    const CCoeControl* aParent,         // container for this list box
    TResourceReader& aReader,            // resource reader that will be used to populate the list box
    CCamAppController& aController, // used by the list box model to get list item values
    TInt aSelectedItemIndex )
    {
    // Set the container window to be the parent.
    CEikListBox::SetContainerWindowL( *aParent );

    // Create the model for the list box.
    CCamCaptureSetupMenuListBoxModel* model = 
        CCamCaptureSetupMenuListBoxModel::NewLC( aController, aReader );

    // Create the item drawer for the list box.
    CCamCaptureSetupMenuListItemDrawer* itemDrawer = 
        CCamCaptureSetupMenuListItemDrawer::NewL( model, aReader );
    itemDrawer->SetParentControl( aParent );

	// Transfering ownership to CEikListBox.
    // Do not need the model or drawer on the cleanup stack when call ConstructL,
    // because CEikListBox assigns objects as member variables before
    // ConstructL calls any leaving functions.
    CleanupStack::Pop( model );
	CEikListBox::ConstructL( model, itemDrawer, aParent, EAknListBoxMarkableList );

    // Set the border style.
    iBorder = TGulBorder::EDeepRaised;

    // Set the selected item index
    iView->SetCurrentItemIndex( aSelectedItemIndex );
	}





