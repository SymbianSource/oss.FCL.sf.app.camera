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
* Description:  Setting item type for "Add to album"*
*/



// INCLUDE FILES
 
#include <cameraapp.mbg>
#include <gscamerapluginrsc.rsg>

#include "CamAlbumSettingItem.h"
#include "CamSettings.hrh"
#include "CamUtility.h"
#include "CameraappPrivateCRKeys.h"
#include "CamStaticSettingsModel.h"
#include "camerauiconfigmanager.h"
#include "CamAppUiBase.h"
#include "GSCamcorderPlugin.h"


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCamAlbumSettingItem::CCamAlbumSettingItem
// Two phase construction
// ---------------------------------------------------------------------------
//
CCamAlbumSettingItem::CCamAlbumSettingItem( TInt aSettingItemId, 
                                            TInt& aValue)
: CAknEnumeratedTextPopupSettingItem( aSettingItemId, aValue )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamAlbumSettingItem::~CCamAlbumSettingItem()
  {
  PRINT( _L("Camera => ~CCamAlbumSettingItem") );
  PRINT( _L("Camera <= ~CCamAlbumSettingItem") );
  }

// ---------------------------------------------------------------------------
// CCamAlbumSettingItem::HandleSettingPageEventL
// Handle events in the setting page
// ---------------------------------------------------------------------------
//
void CCamAlbumSettingItem::HandleSettingPageEventL( CAknSettingPage* /*aSettingPage*/, 
                                                    TAknSettingPageEvent aEventType )
    {
    // Get the value of the currently selected item on the setting page
    MAknQueryValue* settingPageValues = QueryValue();
    TInt selectedIndex = settingPageValues->CurrentValueIndex();
    CArrayPtr<CAknEnumeratedText>* items = EnumeratedTextArray();
    TInt selectedValue = items->At( selectedIndex )->EnumerationValue();

    // If the user has Okayed the "Yes" option.
    if ( aEventType == EEventSettingOked  && selectedValue == ECamSettYes )
        {
        // Launch the album selection list popup
        DisplayAlbumSelectionListL();
        }
    UpdateListBoxTextL();
    }

// ---------------------------------------------------------------------------
// CCamAlbumSettingItem::CompleteConstructionL
// Gets the correct text for a currently selected album
// ---------------------------------------------------------------------------
//	
void CCamAlbumSettingItem::CompleteConstructionL()
    {
    PRINT( _L("Camera => CCamAlbumSettingItem::CompleteConstructionL") );
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
    if ( ExternalValue() == ECamSettYes )
        {
        // We update the list box only if the setting item is true
        // In this case, we stop displaying "Yes" and replace it with
        // current album name.
        UpdateListTextL();
        }
    PRINT( _L("Camera <= CCamAlbumSettingItem::CompleteConstructionL") );
    }

// ---------------------------------------------------------------------------
// CCamAlbumSettingItem::UpdateListTextL
// Update the text in the setting list to show the current capture album name
// ---------------------------------------------------------------------------
//
void CCamAlbumSettingItem::UpdateListTextL()
    {
    PRINT( _L("Camera => CCamAlbumSettingItem::UpdateListTextL") );
 
        CAknViewAppUi *AppUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
        CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
                                ( AppUi->View( KGSCamcorderGSPluginUid ) );
        // If "Add to album" is on
        if ( ExternalValue() == ECamSettYes )
            {
            // A default capture album exists
            // Replace the "Yes" text with the current capture album name
            CArrayPtr<CAknEnumeratedText>* itemArray = EnumeratedTextArray();
            TInt numberOfItems = itemArray->Count();    // item count
            TInt index = 0;
            TBool found = EFalse;
            while ( !found && index < numberOfItems )
                {
                TInt value = itemArray->At( index )->EnumerationValue();
                // This is the "Yes" option 
                if (  value == ECamSettYes )
                    {
                    // Replace the text with the album name
                    // We get the album name from CenRep key
                    HBufC* albumName = parent->TextSettingValue( 
                                ECamSettingItemDefaultAlbumName ).AllocLC();

                    CAknEnumeratedText* newItem = new( ELeave ) 
                                  CAknEnumeratedText( value, albumName );
                    CleanupStack::Pop( albumName );
                    CleanupStack::PushL( newItem );
                    CAknEnumeratedText* oldItem = itemArray->At( index );
                    itemArray->Delete( index );
                    delete oldItem;
                    oldItem = NULL;
                    itemArray->InsertL( index, newItem );
                    CleanupStack::Pop( newItem );
                    found = ETrue;
                    }
                index++;
                }
            } // End of ExternalValue        
    UpdateListBoxTextL();
    PRINT( _L("Camera <= CCamAlbumSettingItem::UpdateListTextL C") );
    }

// ---------------------------------------------------------------------------
// CCamAlbumSettingItem::DisplayAlbumSelectionListL
// Launch the album selection list popup
// ---------------------------------------------------------------------------
//
void CCamAlbumSettingItem::DisplayAlbumSelectionListL()
    {

        CAknViewAppUi *AppUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
        CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
                                ( AppUi->View( KGSCamcorderGSPluginUid ) );
        // Launch the Pop-up menu using the Gallery API
        // allowMultipleSelection = ETrue --> Multiple Selection
        // allowMultipleSelection = EFalse --> Single Selection
        TBool   allowMultipleSelection = EFalse;
        RArray<TUint32> selectedAlbumIds;
        CleanupClosePushL( selectedAlbumIds );
        // Launching the Pop-up menu with the list of albums
        TRAPD ( err, TGlxCollectionSelectionPopup::ShowPopupL(
                                        selectedAlbumIds,
                                        KGlxCollectionTypeIdAlbum,
                                        allowMultipleSelection,
                                        ETrue
                                        ) );
        
        TInt defaultAlbumId = 0; // default album id in CenRep
        
        TInt prevDefaultId = parent->IntegerSettingValue( 
                                          ECamSettingItemDefaultAlbumId );
         if ( selectedAlbumIds.Count() > 0 && err == KErrNone )
            {
            // this is the new album which is set as default
            defaultAlbumId = static_cast<TInt>( selectedAlbumIds[0] );
            }
         else if ( prevDefaultId != 0 )
            {
            // UseCase: If the user selects cancel when the pop-up active
            //          we then go back to the prev album selected
            defaultAlbumId = prevDefaultId;
            }
         else
            {
            }
          CleanupStack::PopAndDestroy( &selectedAlbumIds );
          
          parent->SetIntegerSettingValueL( ECamSettingItemDefaultAlbumId,
                                          defaultAlbumId );

     }

// ---------------------------------------------------------------------------
// CCamAlbumSettingItem::HandleAlbumError
// Deal with loss of capture album or album information
// ---------------------------------------------------------------------------
//
void CCamAlbumSettingItem::HandleAlbumError()
    {
    // Set "Add to album" off
    SetInternalValue( ECamSettNo );
    SetExternalValue( ECamSettNo );
    }

//  End of File  
