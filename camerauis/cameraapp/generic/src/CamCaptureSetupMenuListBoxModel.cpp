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
* Description:  Model class of capture setup menu list box.*
*/


// INCLUDE FILES
#include <barsread.h>
#include <eikenv.h>
#include "CamCaptureSetupMenuListBoxModel.h"
#include "CamAppController.h"
#include "CamCaptureSetupMenuListItem.h"
#include "CamPanic.h"
#include "camlogging.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::NewLC
// Returns CCamCaptureSetupMenuListBoxModel*: Pointer to the created model.
// ---------------------------------------------------------
//
CCamCaptureSetupMenuListBoxModel* CCamCaptureSetupMenuListBoxModel::NewLC(
    CCamAppController& aController,
    TResourceReader& aReader )  
    {
    CCamCaptureSetupMenuListBoxModel* self = 
        new( ELeave ) CCamCaptureSetupMenuListBoxModel( aController );
    CleanupStack::PushL( self );
    self->ConstructFromResourceL( aReader );
    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CCamCaptureSetupMenuListBoxModel::~CCamCaptureSetupMenuListBoxModel()
  {
  PRINT( _L("Camera => ~CCamCaptureSetupMenuListBoxModel") );
  iListItemPtrArray.ResetAndDestroy();
  iListItemPtrArray.Close();
  PRINT( _L("Camera <= ~CCamCaptureSetupMenuListBoxModel") );
  }

// -----------------------------------------------------------------------------
// ItemHasIconText
// Returns whether the current item has icon text.
// -----------------------------------------------------------------------------
//
TBool CCamCaptureSetupMenuListBoxModel::ItemHasIconText( TInt aItemIndex ) const
	{
	const TInt settingVal = iController.IntegerSettingValue( 
                iListItemPtrArray[aItemIndex]->SettingsModelItemId() );
	return iListItemPtrArray[aItemIndex]->IconText( settingVal ).Length() > 0;
	}

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::BitmapForItem()
// Returns a pointer to the bitmap associated with a specific list item.
// ---------------------------------------------------------
//
CFbsBitmap* CCamCaptureSetupMenuListBoxModel::BitmapForItem( 
  TInt aItemIndex, TBool aMask )
    {
    // If the value in the settings model for this list item has changed
    // since the last time we asked for a bitmap, then let list item know
    // what it has changed to.
    TInt itemCurrentValue = iListItemPtrArray[aItemIndex]->CurrentValue();
    TInt itemSettingsModelItemId = 
        iListItemPtrArray[aItemIndex]->SettingsModelItemId();
    TInt itemNewValue = iController.IntegerSettingValue( 
                                        itemSettingsModelItemId );
    if ( itemCurrentValue != itemNewValue )
        {
        iListItemPtrArray[aItemIndex]->SetValueTo( itemNewValue );
        }
    // Return the bitmap that represents the list item value.
    if ( aMask )
        {
        return iListItemPtrArray[aItemIndex]->Mask();
        }
    else
        {
        return iListItemPtrArray[aItemIndex]->Bitmap();
        }
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::TextForItem() const
// Returns the text associated with a specific list item.
// ---------------------------------------------------------
//
const TDesC& CCamCaptureSetupMenuListBoxModel::TextForItem( 
    TInt aItemIndex ) const
    {
    return iListItemPtrArray[aItemIndex]->Text();
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::IconTextForItem() const
// Returns the text to be used in place of an icon for a particular list item.
// ---------------------------------------------------------
//
const TDesC& CCamCaptureSetupMenuListBoxModel::IconTextForItem( 
    TInt aItemIndex ) const
    {
	const TInt settingVal = iController.IntegerSettingValue( 
            iListItemPtrArray[aItemIndex]->SettingsModelItemId() );
    return iListItemPtrArray[aItemIndex]->IconText( settingVal );
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::SettingsModelIdAssociatedWithItem() const
// Returns the settings model id associated with a particular list item.
// ---------------------------------------------------------
//
TInt CCamCaptureSetupMenuListBoxModel::SettingsModelIdAssociatedWithItem
( TInt aItemIndex ) const
    {
    return iListItemPtrArray[aItemIndex]->SettingsModelItemId();
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::NumberOfItems() const
// Returns number of items that are in array
// ---------------------------------------------------------
//
TInt CCamCaptureSetupMenuListBoxModel::NumberOfItems() const
    {
    return iListItemPtrArray.Count();
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::MatchableTextArray() const
// Returns pointer to MDesCArray
// ---------------------------------------------------------
//
const MDesCArray* CCamCaptureSetupMenuListBoxModel::MatchableTextArray() const
    {
    return iMatchableTextArray;
    }

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CCamCaptureSetupMenuListBoxModel::CCamCaptureSetupMenuListBoxModel
( CCamAppController& aController ) 
: iController( aController )
    {
    }

// ---------------------------------------------------------
// CCamCaptureSetupMenuListBoxModel::ConstructFromResourceL()
// 2nd phase constructor.
// ---------------------------------------------------------
//
void CCamCaptureSetupMenuListBoxModel::ConstructFromResourceL( 
    TResourceReader& aReader ) 
    {
    // Create all list items from the resource reader.
    TInt itemCount = aReader.ReadInt16();
    TInt i;
    for ( i = 0; i < itemCount; ++i )
        {
        // Create a list item from the resource at current reading position.
        CCamCaptureSetupMenuListItem* newListItem = 
            CCamCaptureSetupMenuListItem::NewL( aReader );

        // Add it to the array of list items.
        CleanupStack::PushL( newListItem );
        User::LeaveIfError( iListItemPtrArray.Append( newListItem ) );
        CleanupStack::Pop( newListItem );
        }
    }


//  End of File  

