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
* Description:  Encapsulates the data for a particular Capture Setup Menu item.*
*/



// INCLUDE FILES
#include "CamCaptureSetupMenuListItem.h"
#include "CamUtility.h"
#include <eikenv.h>
#include <eikappui.h>	// For CCoeAppUiBase
#include <eikapp.h>		// For CEikApplication
#include <barsread.h>
#include <AknIconUtils.h>

const TInt KIconTextGranularity = 2;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewL
// Two-phased constructor.
// Returns: CCamCaptureSetupMenuListItem*: Pointer to the created list item.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupMenuListItem* CCamCaptureSetupMenuListItem::NewL( 
    TResourceReader& aReader ) // Used to read/construct data for this list item.
    {
    CCamCaptureSetupMenuListItem* self = 
        new (ELeave ) CCamCaptureSetupMenuListItem();
    CleanupStack::PushL( self );
    self->ConstructFromResourceL( aReader );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::~CCamCaptureSetupMenuListItem
// -----------------------------------------------------------------------------
//
CCamCaptureSetupMenuListItem::~CCamCaptureSetupMenuListItem()
  {
  PRINT( _L("Camera => ~CCamCaptureSetupMenuListItem") );

  delete iIconTexts;
  iIconTexts = NULL;

  // Destroy bitmap array.
  iBitmaps.ResetAndDestroy();
  iBitmaps.Close();
  iMasks.ResetAndDestroy();
  iMasks.Close();
  PRINT( _L("Camera <= ~CCamCaptureSetupMenuListItem") );
  }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::Bitmap
// Returns the bitmap that represents the current settings model 
// value for this list item.
// Returns: CFbsBitmap*: Pointer to the bitmap.
// -----------------------------------------------------------------------------
//
CFbsBitmap* CCamCaptureSetupMenuListItem::Bitmap() const
    {
    return iBitmaps[iCurrentSettingItemValueId];
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::Mask
// Returns the bitmap that represents the current settings model 
// value for this list item.
// Returns: CFbsBitmap*: Pointer to the bitmap.
// -----------------------------------------------------------------------------
//
CFbsBitmap* CCamCaptureSetupMenuListItem::Mask() const
    {
    return iMasks[iCurrentSettingItemValueId];
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::IconText
// Returns text used in place of an icon for this list item.
// Returns: Ref to the icon text.
// -----------------------------------------------------------------------------
//
const TDesC& CCamCaptureSetupMenuListItem::IconText( TInt aSettingVal ) const
	{
    if ( iSettingsModelItemId == ECamSettingItemDynamicPhotoBrightness ||
         iSettingsModelItemId == ECamSettingItemDynamicVideoBrightness ||
         iSettingsModelItemId == ECamSettingItemDynamicPhotoContrast ||
         iSettingsModelItemId == ECamSettingItemDynamicVideoContrast )
        {
        // get the array index if this item is brightness or contrast
        GetBrightnessContrastArrayIndex( aSettingVal );
        }

	return (*iIconTexts)[aSettingVal];
	}

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::Text
// Returns text that describes the setting represented by this list item.
// Returns: Ref to the item text.
// -----------------------------------------------------------------------------
//
const TDesC& CCamCaptureSetupMenuListItem::Text() const
    {
    return iText;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::CurrentValue
// Returns the cached settings model value that the current bitmap represents.
// Returns: TInt: The current value.
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupMenuListItem::CurrentValue() const
    {
    return iCurrentSettingItemValueId;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::SettingsModelItemId
// Returns the id of the settings model item that this list item represents.
// Returns: TInt: The settings model item id.
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupMenuListItem::SettingsModelItemId() const
    {
    return iSettingsModelItemId;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::SetValueTo
// Changes the cached settings model value and the bitmap that represents it.
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupMenuListItem::SetValueTo( 
    TInt aNewValueId ) // the new cached settings model value.
    {
    if ( iSettingsModelItemId == ECamSettingItemDynamicPhotoBrightness ||
         iSettingsModelItemId == ECamSettingItemDynamicVideoBrightness ||
         iSettingsModelItemId == ECamSettingItemDynamicPhotoContrast ||
         iSettingsModelItemId == ECamSettingItemDynamicVideoContrast )
        {
        // get the array index if this item is brightness or contrast
        GetBrightnessContrastArrayIndex( aNewValueId );
        }

    iCurrentSettingItemValueId = aNewValueId;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::CCamCaptureSetupMenuListItem
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupMenuListItem::CCamCaptureSetupMenuListItem()
    {
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::ConstructFromResourceL
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void 
CCamCaptureSetupMenuListItem::ConstructFromResourceL( 
    TResourceReader& aReader ) // reader that is used to construct the data 
  {
  PRINT( _L("Camera => CCamCaptureSetupMenuListItem::ConstructFromResourceL") );
  delete iIconTexts;
  iIconTexts = NULL;
  iIconTexts = new (ELeave) CArrayFixFlat<TIconText>( KIconTextGranularity );

  // Get the full path for the mbm file
  TFileName resFileName;
  CamUtility::ResourceFileName( resFileName );
  
  // Create a resource reader for reading only this list items data.
  TInt resourceID = aReader.ReadInt32();
  TResourceReader localReader;
  CEikonEnv::Static()->CreateResourceReaderLC( localReader, resourceID );
  
  // Read the data for this list item from resource.
  // ...get setting model item id for associated with this list item.
  iSettingsModelItemId = localReader.ReadInt16();
  // ...get the text description of this list item.
  iText = localReader.ReadTPtrC();
  // ...get the count of all possible values (bitmaps) 
  // ...that this list item can have, and repeatedly get the values.
  TInt valuesCount = localReader.ReadInt16();
  TInt i;
  for ( i = 0; i < valuesCount; ++i )
    {
    // ...read the current value's settings model id.
    TInt settingsModelId = localReader.ReadInt16();
    // ...read associated bitmap id for current value.
    TInt bitmapId = localReader.ReadInt16();
		CFbsBitmap* newBitmap = NULL;
		CFbsBitmap* mask = NULL;
		if ( bitmapId != -1 )
			{
			AknIconUtils::CreateIconLC( newBitmap, mask, resFileName, bitmapId, bitmapId );
			// ...add the bitmap to the array of bitmaps for this list item.
			iBitmaps.InsertL( newBitmap, settingsModelId );
			iMasks.InsertL( mask, settingsModelId );
			// newBitmap, mask. Don't know the order of pushing so pop anonymously.
			CleanupStack::Pop( 2 ); 
			}
		else
			{
			// Add NULL bitmaps:
			iBitmaps.InsertL( newBitmap, settingsModelId );
			iMasks.InsertL( mask, settingsModelId );
			}

		iIconTexts->InsertL( settingsModelId, localReader.ReadTPtrC() );
		}

  // Pop local resource reader.
  CleanupStack::PopAndDestroy();  
  PRINT( _L("Camera <= CCamCaptureSetupMenuListItem::ConstructFromResourceL") )  
  }


// -----------------------------------------------------------------------------
// CCamCaptureSetupMenuListItem::GetBrightnessContrastArrayIndex
// Returns the array position for brightness and contrast, as these controls
// can have negative values, so this maps to an index in the text/bitmaps arrays
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupMenuListItem::GetBrightnessContrastArrayIndex( TInt& aValue ) const
    {
    // if brightness or contrast settings - only display either
    // the plus or negative icon. There isn't going to be an icon
    // for each setting value as the values can range from -10 to +10
    if ( iSettingsModelItemId == ECamSettingItemDynamicPhotoBrightness ||
         iSettingsModelItemId == ECamSettingItemDynamicVideoBrightness )
        {
        if ( aValue > 0 )
            {
            aValue = ECamBrightnessPlus;
            }
        else if ( aValue < 0 )
            {
            aValue = ECamBrightnessNeg;
            }
        else
            {
            aValue = ECamBrightnessNorm;
            }
        }
    else if ( iSettingsModelItemId == ECamSettingItemDynamicPhotoContrast ||
              iSettingsModelItemId == ECamSettingItemDynamicVideoContrast )
        {
        if ( aValue > 0 )
            {
            aValue = ECamContrastPlus;
            }
        else if ( aValue < 0 )
            {
            aValue = ECamContrastNeg;
            }
        else
            {
            aValue = ECamBrightnessNorm;
            }
        }
    else // lint
        {
        }
    }


//  End of File  
