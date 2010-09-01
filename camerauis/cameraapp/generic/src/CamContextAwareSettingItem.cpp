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
* Description:  Setting item type for CAknEnumeratedItems.*
*/



// INCLUDE FILES
#include <barsread.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamContextAwareSettingItem.h"
#include "CamAppController.h"
#include "CamContextAwareSettingPage.h"
#include "camlogging.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCamContextAwareSettingItem::CCamContextAwareSettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamContextAwareSettingItem::CCamContextAwareSettingItem( CCamAppController& aController,
                                                          TInt aSettingItemId, 
                                                          TInt& aValue )
: CAknEnumeratedTextPopupSettingItem( aSettingItemId, aValue ), iController( aController )
    {
    }

// ---------------------------------------------------------------------------
// CCamContextAwareSettingItem::CreateSettingPageL
// Creates setting page that switches cba depending on setting value
// that user navigates to.
// ---------------------------------------------------------------------------
//
CAknSettingPage* CCamContextAwareSettingItem::CreateSettingPageL()
	{
	TPtrC settingName = SettingName();
    TInt editorControlType = EAknCtPopupSettingList;
	return new ( ELeave ) CCamContextAwareSettingPage( 
        &settingName, SettingNumber(), editorControlType,
        SettingEditorResourceId(), SettingPageResourceId(), 
        *QueryValue(), *this, iController );
	}

// ---------------------------------------------------------------------------
// CCamContextAwareSettingItem::CompleteConstructionL
// Determines whether or not the value list is to be limited (visually) 
// and replaces the popup text array with a new array created from the 
// revised resource id.
// ---------------------------------------------------------------------------
//
void CCamContextAwareSettingItem::CompleteConstructionL()
   {
   const TInt KSettingItemArrayGranularity = 5;

   CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
   // Check if limitation applies
   TInt aRevisedResourceId;
   if ( !iController.AllOptionsVisibleForSettingItem( Identifier(), aRevisedResourceId ) )
        {  
        // Construct the array
        CArrayPtr<CAknEnumeratedText>* enumeratedTextArray; 
        enumeratedTextArray = new( ELeave ) CArrayPtrFlat<CAknEnumeratedText>( KSettingItemArrayGranularity );
        CleanupStack::PushL( enumeratedTextArray );

        // Construct a dummy array for popup text, needed to avoid a panic
        CArrayPtr<HBufC>* popupTextArray; 
        popupTextArray = new( ELeave ) CArrayPtrFlat<HBufC>( KSettingItemArrayGranularity );
        CleanupStack::PushL( popupTextArray );

        // Read in the texts to be used in the setting item list
        TResourceReader reader;
        CEikonEnv::Static()->CreateResourceReaderLC( reader, aRevisedResourceId );	
        TInt numberOfItems = reader.ReadInt16();	// item count	

        TInt index = 0;
        // Loop through all the texts 
        for ( index = 0; index < numberOfItems ; ++index )
		    {
		    TInt value = reader.ReadInt16();  
		    TPtrC text = reader.ReadTPtrC();  // LTEXT name;
		    HBufC* thisText = text.AllocLC(); 
		    TPtr thisTextDes = thisText->Des();
		    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( thisTextDes );		
		    CAknEnumeratedText* enumeratedText = new( ELeave ) CAknEnumeratedText( value, thisText );
		    CleanupStack::Pop( thisText ); 
		    CleanupStack::PushL( enumeratedText );
		    enumeratedTextArray->AppendL( enumeratedText );
		    CleanupStack::Pop( enumeratedText ); 
		    }

        CleanupStack::PopAndDestroy(); // reader
        SetEnumeratedTextArrays( enumeratedTextArray, popupTextArray );
        CleanupStack::Pop( popupTextArray );
        CleanupStack::Pop( enumeratedTextArray );
        }
   }
//  End of File  
