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
* Description:  Implements a user scene setup list item.*
*/


// INCLUDE FILES
#include "CamUserSceneSetupListItem.h"
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "CamPanic.h"
#include <barsread.h>
#include <e32std.h>
#include <eikenv.h>
#include "CamUtility.h"

// CONSTANTS
const TInt KValuesArrayGranularity = 6;


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CCamUserSceneSetupListItem::NewLC
// Symbian two-phased constructor.
// ---------------------------------------------------------
//
CCamUserSceneSetupListItem* CCamUserSceneSetupListItem::NewLC
( TInt aResId )
    {
    CCamUserSceneSetupListItem* self =
        new ( ELeave ) CCamUserSceneSetupListItem;
    CleanupStack::PushL( self );
    self->ConstructL( aResId );
    return self;
    }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::NewL
// Symbian two-phased constructor.
// ---------------------------------------------------------
//
CCamUserSceneSetupListItem* CCamUserSceneSetupListItem::NewL
(  TInt aResId )
    {
    CCamUserSceneSetupListItem* self = 
        CCamUserSceneSetupListItem::NewLC( aResId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::~CCamUserSceneSetupListItem
// Destructor.
// ---------------------------------------------------------
//
CCamUserSceneSetupListItem::~CCamUserSceneSetupListItem()
  {
  PRINT( _L("Camera => ~CCamUserSceneSetupListItem" ))  
  iValues.ResetAndDestroy();
  iValues.Close();
  PRINT( _L("Camera <= ~CCamUserSceneSetupListItem" ))  
  }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::ListBoxText
// Returns the formatted text of this item to be used in the list box
// ---------------------------------------------------------
//
TPtrC16 CCamUserSceneSetupListItem::ListBoxText()
    {
    // Create the title part of the text.
    _LIT( KSettingItemSingleSpace, " " );
    _LIT( KListBoxTextSeparator, "\t" );

	TPtr listBoxTextPtr = iListBoxText.Des();
    listBoxTextPtr.Copy( KSettingItemSingleSpace );
	listBoxTextPtr.Append( KListBoxTextSeparator );
	listBoxTextPtr.Append( SettingName() );
   	listBoxTextPtr.Append( KListBoxTextSeparator );
	listBoxTextPtr.Append( KListBoxTextSeparator );

    // Append the second part of the text to the title part.
    // ...If there is value text for this item then append it.
    if ( iValues.Count() > 0 )
        {
	    TPtrC settingPtr = SettingText();
	    listBoxTextPtr.Append( settingPtr );
        }

    return iListBoxText;
    }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::SettingsModelTypeId
// Returns the settings model item id that is associated with this
// list item.
// ---------------------------------------------------------
//
TInt CCamUserSceneSetupListItem::SettingsModelTypeId()
    {
    return iSettingsModelItemId;
    }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::SetCurrentValue
// Sets the current value of this list item
// ---------------------------------------------------------
//
void CCamUserSceneSetupListItem::SetCurrentValue( TInt aNewValue )
    {
    PRINT1( _L("Camera => CCamUserSceneSetupListItem::SetCurrentValue = %d" ), aNewValue )
    PRINT1( _L("iSettingsModelItemId = %d" ), iSettingsModelItemId )
    // Find the value-text pair that match the new current value,
    // and store the index.
    // Currently the brightness and contrast items only show 3 icons
    // 1 for normal setting i.e. 0
    // 1 for any positive setting i.e. above 0 and
    // 1 for any negative setting i.e. below 0
    // The code below matches positive/negative values to the correct icon
    if ( iSettingsModelItemId == ECamSettingItemUserSceneBrightness )
        {
        if ( aNewValue < ECamBrightnessNorm )
            {
            iCurrentPairIndex = ECamBrightnessNeg;
            return;
            }
        if ( aNewValue > ECamBrightnessNorm )
            {
            iCurrentPairIndex = ECamBrightnessPlus;
            return;
            }
        }
    else if ( iSettingsModelItemId == ECamSettingItemUserSceneContrast )
        {
        if ( aNewValue < ECamContrastNorm )
            {
            PRINT( _L("iCurrentPairIndex = ECamContrastNeg" ) )
            iCurrentPairIndex = ECamContrastNeg;
            return;
            }
        if ( aNewValue > ECamContrastNorm )
            {
            iCurrentPairIndex = ECamContrastPlus;
            PRINT( _L("iCurrentPairIndex = ECamContrastPlus" ) )
            return;
            }
        }
    else
        {
        // lint
        }


    TInt i = 0;
    TBool foundPair = EFalse;
    do 
        {
        __ASSERT_DEBUG( i <= iValues.Count(), 
            CamPanic( ECamPanicUSSPairNotFound ) );
        if ( aNewValue == iValues[i]->iSettingsModelId )
            {
            foundPair = ETrue;
            iCurrentPairIndex = i;
            }
        else
            {
            ++i;
            }
        }
    while ( !foundPair );
    
   PRINT1( _L("iCurrentPairIndex = %d" ), iCurrentPairIndex )   
   }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::CCamUserSceneSetupListItem
// Constructor
// ---------------------------------------------------------
//
CCamUserSceneSetupListItem::CCamUserSceneSetupListItem()
:iValues( KValuesArrayGranularity )
    {
    }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::ConstructFromResourceL
// 2nd phase Constructor
// ---------------------------------------------------------
//
void CCamUserSceneSetupListItem::ConstructL
( TInt aResId )
    {
    // Create a resource reader for reading only this list items data.
    TResourceReader localReader;
    CEikonEnv::Static()->CreateResourceReaderLC
        ( localReader, aResId );

    // Read the data for this list item from resource.
    // ...get setting model item id associated with this list item.
    iSettingsModelItemId = localReader.ReadInt16();
    // ...get the text name of this list item.
    iName = localReader.ReadTPtrC();
    // ...get the count of all possible values 
    // ...that this list item can have, and repeatedly get these values
    // ...and their associated textual description.
    TInt valuesCount = localReader.ReadInt16();
	TInt i;
    for ( i = 0; i < valuesCount; ++i )
        {
        // ...create the pair object.
        CValueTextPair* pair = new ( ELeave ) CValueTextPair;
        // ...set the data.
        pair->iSettingsModelId = localReader.ReadInt16();
        pair->iText = localReader.ReadTPtrC();

        // ...add the pair to the pair list.
        CleanupStack::PushL( pair );
        iValues.AppendL( pair );
        CleanupStack::Pop( pair );
        }    

    // Pop local resource reader.
    CleanupStack::PopAndDestroy();  
    }

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::SettingName
// Returns the name of the setting type represented by this
// list item.
// ---------------------------------------------------------
//
TPtrC CCamUserSceneSetupListItem::SettingName() const
	{
	return iName;
	}

// ---------------------------------------------------------
// CCamUserSceneSetupListItem::SettingText
// Returns the text for the current setting value of this item
// ---------------------------------------------------------
//
TPtrC CCamUserSceneSetupListItem::SettingText()
	{
    iValueText = iValues[iCurrentPairIndex]->iText;
    return iValueText;
    }





// End of File  
