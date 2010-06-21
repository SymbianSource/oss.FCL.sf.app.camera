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
* Description:  A model class for CCamCaptureSetupListBox*
*/


// INCLUDE FILES
#include <barsread.h>
#include <eikenv.h>

#ifndef CAMERAAPP_PLUGIN_BUILD
    #include <cameraapp.rsg>
    #include <vgacamsettings.rsg>
#else
    #include <gscamerapluginrsc.rsg>
#endif //CAMERAAPP_PLUGIN_BUILD

#include "CamProductSpecificSettings.hrh"
#include "CamUtility.h"
#include "camlayoututils.h"
#include "CamCaptureSetupListItem.h"
#include "CamCaptureSetupListBoxModel.h"
#include "CameraUiConfigManager.h"

#include <aknlayoutscalable_apps.cdl.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCamCaptureSetupListBoxModel::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupListBoxModel* CCamCaptureSetupListBoxModel::NewLC( 
#ifndef CAMERAAPP_PLUGIN_BUILD
                                                                  CCamAppController& aController,
#else // location setting will be moved to the plugin
                                                                  MCamAppController& aController,
#endif
        														   TInt aArrayResourceId,
        														   TInt aSettingItem,
                                                    			   TBool aShootingModeListBox,
                                                    			   TBool aLocationSettingListBox,
                                                                   TRect aListBoxRect
                                                    			   )
    {
    CCamCaptureSetupListBoxModel* self = new( ELeave ) CCamCaptureSetupListBoxModel( aController,
    																				 aShootingModeListBox,
    																				 aLocationSettingListBox
    																				 );

    CleanupStack::PushL( self );
    self->ConstructL( aArrayResourceId, aSettingItem, aListBoxRect );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBoxModel destructor
// 
// ---------------------------------------------------------------------------
//
CCamCaptureSetupListBoxModel::~CCamCaptureSetupListBoxModel()
	{
  PRINT( _L("Camera => ~CCamCaptureSetupListBoxModel") );
  iItemArray.ResetAndDestroy();
  iItemArray.Close();
  PRINT( _L("Camera <= ~CCamCaptureSetupListBoxModel") );
	}

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::NumberOfItems
// Returns number of items that are in array
// ---------------------------------------------------------
//
TInt CCamCaptureSetupListBoxModel::NumberOfItems() const
	{
	return iItemArray.Count();
	}

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::MatchableTextArray
// Returns null, no support for incremental matching
// ---------------------------------------------------------
//
const MDesCArray* CCamCaptureSetupListBoxModel::MatchableTextArray() const
	{
    return NULL;
	}

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::ItemText
// Returns a pointer to the item text
// ---------------------------------------------------------
//
TPtrC CCamCaptureSetupListBoxModel::ItemText( TInt aItemIndex ) const
    {
    return iItemArray[ aItemIndex ]->ItemText();
    }

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::ItemValue
// Returns the value id of the specified item
// ---------------------------------------------------------
//
TInt CCamCaptureSetupListBoxModel::ItemValue( TInt aItemIndex ) const 
    {
    return iItemArray[aItemIndex]->ItemValue();
    }

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::Bitmap
// Returns a pointer to the bitmap of the specified item
// ---------------------------------------------------------
//
CFbsBitmap* CCamCaptureSetupListBoxModel::Bitmap( TInt aItemIndex ) const
    {
    return iItemArray[ aItemIndex ]->Bitmap();
    }

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::BitmapMask
// Returns a pointer to the mask of the specified item
// ---------------------------------------------------------
//
CFbsBitmap* CCamCaptureSetupListBoxModel::BitmapMask( TInt aItemIndex ) const
    {
    return iItemArray[ aItemIndex ]->BitmapMask();
    }

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::ItemIndexFromValueId
// Returns the index of the item with a matching value id
// ---------------------------------------------------------
//
TInt CCamCaptureSetupListBoxModel::ValueIndex( TInt aValueId ) const
    {
    return iItemArray.Find( aValueId, CCamInfoListboxItemBase::EqualIds );
    }

// ---------------------------------------------------------
// CCamCaptureSetupListBoxModel::ShootingModeListBox
// Returns whether the model represents a shooting mode listbox
// ---------------------------------------------------------
//
TBool CCamCaptureSetupListBoxModel::ShootingModeListBox() const
    {
    return iShootingModeListBox;
    }
    

TAknLayoutRect CCamCaptureSetupListBoxModel::IconLayoutData() const
    {
    return iIconLayoutData;
    }
TAknLayoutRect CCamCaptureSetupListBoxModel::RadioButtonLayoutData() const
    {
    return iRadioButtonLayoutData;
    }
TAknLayoutText CCamCaptureSetupListBoxModel::TextLayoutData() const
    {
    return iTextLayoutData;
    }
TAknLayoutRect CCamCaptureSetupListBoxModel::ListItemLayoutData() const
    {
    return iListItemLayoutData;
    }
// ---------------------------------------------------------------------------
// CCamCaptureSetupListBoxModel::CCamCaptureSetupListBoxModel
// C++ constructor
// ---------------------------------------------------------------------------
//

CCamCaptureSetupListBoxModel::CCamCaptureSetupListBoxModel( 
#ifndef CAMERAAPP_PLUGIN_BUILD
                                                            CCamAppController& aController,
#else // location setting will be moved to the plugin
                                                            MCamAppController& aController,
#endif
															TBool aShootingModeListBox,
															TBool aLocationSettingListBox
															)
: iController( aController ), iShootingModeListBox ( aShootingModeListBox ),
  iLocationSettingListBox ( aLocationSettingListBox )
	{
	}

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBoxModel::ConstructL
// 2nd phase construction
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListBoxModel::ConstructL( TInt aArrayResourceId, TInt aSettingItem, TRect aListBoxRect )
	{
    ReadLayoutData( aListBoxRect ); 
        
    // create array from resource
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC( reader, aArrayResourceId );
    const TInt count = reader.ReadInt16();
    
#ifndef CAMERAAPP_PLUGIN_BUILD
    iSupportedFlashModes = iController.SupportedFlashModes();
    iSupportedWBModes = iController.SupportedWBModes();
#endif //CAMERAAPP_PLUGIN_BUILD

  // for each entry in the resource array, create a new list item
  for ( TInt i = 0; i < count; i++ )
    {
    CCamCaptureSetupListItem* listItem = 
      CCamCaptureSetupListItem::NewLC( reader, iIconLayoutData );
                               
        if( SettingValueSupportedL( listItem, aSettingItem ) )
        	{
        	iItemArray.AppendL( listItem );
        	}
        
        CleanupStack::Pop( listItem );
        }    
    CleanupStack::PopAndDestroy(); // reader
	}

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBoxModel::ReadLayoutData
// Read in layout data from resource
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListBoxModel::ReadLayoutData( TRect aListBoxRect )
    {
    if ( CamUtility::IsNhdDevice() )
        { 
        iIconLayoutData.LayoutRect( aListBoxRect, 
                          AknLayoutScalable_Apps::cset_list_set_pane_g3( 0 ) );
        iRadioButtonLayoutData.LayoutRect( aListBoxRect, 
                          AknLayoutScalable_Apps::cset_list_set_pane_g1( 0 ) );
        iListItemLayoutData.LayoutRect( aListBoxRect, 
                          AknLayoutScalable_Apps::cset_list_set_pane( 0 ) );
        }
    else
        {   
        TRect mnPane;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, mnPane );
        TAknLayoutRect listLayoutRect;
        listLayoutRect.LayoutRect( mnPane, 
                    AknLayoutScalable_Apps::main_cset6_listscroll_pane( 4 ) );  
        iIconLayoutData.LayoutRect( listLayoutRect.Rect(), 
                    AknLayoutScalable_Apps::cset_list_set_pane_g3_copy1( 0 ) );
        iRadioButtonLayoutData.LayoutRect( listLayoutRect.Rect(), 
                    AknLayoutScalable_Apps::cset_list_set_pane_g1_copy1( 0 ) );
        TAknWindowLineLayout l =  AknLayoutScalable_Apps::cset_list_set_pane_copy1( 0 ); 
        if( !l.iH )
            {
            l.iH = 73; // TODO Temporarily defined!
            }
        iListItemLayoutData.LayoutRect( listLayoutRect.Rect(), l );
        }
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBoxModel::SettingValueSupportedL
// ---------------------------------------------------------------------------
//
TBool 
CCamCaptureSetupListBoxModel::SettingValueSupportedL( CCamInfoListboxItemBase* aItem, TInt aSettingItem )
  {
  TBool supported = EFalse;
  TBool checkConfiguredItems = ETrue;
  TInt val = aItem->ItemValue();
  
  CCameraUiConfigManager* dynamicSettings = iController.UiConfigManagerPtr();
  RArray<TInt> supportedItems;
  CleanupClosePushL( supportedItems );
  supportedItems.Reset();

    // just for flash to start with
    if( aSettingItem == ECamSettingItemDynamicPhotoFlash )
        {
#ifndef CAMERAAPP_PLUGIN_BUILD
        if ( dynamicSettings )
            {
            dynamicSettings->SupportedFlashModeSettingsL( supportedItems );
            }
#endif //CAMERAAPP_PLUGIN_BUILD
		}
    else if( ECamSettingItemDynamicPhotoWhiteBalance == aSettingItem ||
             ECamSettingItemDynamicVideoWhiteBalance == aSettingItem )
		{
#ifndef CAMERAAPP_PLUGIN_BUILD
        if ( dynamicSettings )
            {
            dynamicSettings->SupportedWhiteBalanceSettingsL( supportedItems );
            }
#endif //CAMERAAPP_PLUGIN_BUILD
	    }
    else if ( ECamSettingItemDynamicPhotoColourFilter == aSettingItem ||
              ECamSettingItemDynamicVideoColourFilter == aSettingItem || 
              ECamSettingItemUserSceneColourFilter    == aSettingItem )
        {
#ifndef CAMERAAPP_PLUGIN_BUILD
        if ( dynamicSettings )
            {
            dynamicSettings->SupportedColorToneSettingsL( supportedItems );
            }
#endif //CAMERAAPP_PLUGIN_BUILD
        }
	else
		{
		checkConfiguredItems = EFalse;
		supported = ETrue;
		}
  
    if ( checkConfiguredItems && supportedItems.Count() > 0 )
        {
        supported = KErrNotFound != supportedItems.Find( val );
        }
  
    CleanupStack::PopAndDestroy( &supportedItems );
	return supported;
	}
//  End of File  

