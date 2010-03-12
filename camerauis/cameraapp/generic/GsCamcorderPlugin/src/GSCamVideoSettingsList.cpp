/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Video settings list class
*
*/


// INCLUDE FILES
 
#include <CAknMemorySelectionSettingItem.h> // CAknMemorySelectionSettingItem
#include <bldvariant.hrh>
#include <featmgr.h>
#include <gscamerapluginrsc.rsg>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <AknWaitDialog.h>
#include <CAknMemorySelectionSettingItemMultiDrive.h>
#include <driveinfo.h>          // DriveInfo

#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "GSCamVideoSettingsList.h"
#include "CamPanic.h"
#include "CamAppUiBase.h"
#include "GSCamcorderPlugin.h"
#include "CamUtility.h"
#include "MCamAppController.h"
#include "CamStaticSettingsModel.h"
#include "GSCamQualitySettingItem.h"
#include "GSCamDefaultNameSettingItem.h"
#include "GSCustomQualitySettingItem.h"
#include "CameraUiConfigManager.h"

#include "CamCollectionManagerAo.h"
#include "CamWaitDialog.h"

#include "camconfiguration.h"
#include "CamLocationSettingItem.h"

// FORWARD DECLARATIONS

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamVideoSettingsList::CGSCamVideoSettingsList
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamVideoSettingsList::CGSCamVideoSettingsList( CCamStaticSettingsModel& aModel )
:iModel( aModel )
    {
    iSettings.Reset();
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsList::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CGSCamVideoSettingsList::ConstructL( TBool aLaunchedFromGS,
                                          TBool aSecondaryCameraSettings )
    {
    CAknViewAppUi *AppUi = 
                  static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );

    CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
        					     ( AppUi->View( KGSCamcorderGSPluginUid ) );

    if ( parent )
        {
        parent->LoadVideoStaticSettingsL( ETrue );
        parent->LoadPhotoStaticSettingsL( ETrue );
        }

    iModel.Configuration().GetPsiIntArrayL( ECamPsiSupportedVideoQualities, iSupportedVideoQualities );
                                                     
    iLaunchedFromGS = aLaunchedFromGS;
    iSecondaryCameraSettings = aSecondaryCameraSettings;
    iSettings.ResetAndDestroy();

    TInt resId;
    if ( iModel.UiConfigManagerPtr() &&
         iModel.UiConfigManagerPtr()->IsLocationSupported() )
        {
        resId =  iLaunchedFromGS?R_CAM_VIDEO_GSSETTINGS_WITH_LOCATION_ITEM_LIST_CAMCORDER:
                                 R_CAM_VIDEO_SETTINGS_WITH_LOCATION_ITEM_LIST_CAMCORDER;
        }
    else
        {
        resId =  iLaunchedFromGS?R_CAM_VIDEO_GSSETTINGS_ITEM_LIST_CAMCORDER:
                                 R_CAM_VIDEO_SETTINGS_ITEM_LIST_CAMCORDER;
        }

    if ( iSecondaryCameraSettings )
        {
        resId =  R_CAM_VIDEO_SETTINGS_ITEM_LIST_PORTRAIT;
        }

    ConstructFromResourceL( resId );
    }


// Destructor
//CGSCamVideoSettingsList::~CGSCamVideoSettingsList()
//
CGSCamVideoSettingsList::~CGSCamVideoSettingsList()
    {
    iSettings.ResetAndDestroy();
    iSettings.Close();
    iSupportedVideoQualities.Close();
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsList::CreateSettingItemL
// From CAknSettingItemList Handles creating setting items
// ---------------------------------------------------------------------------
//
CAknSettingItem* CGSCamVideoSettingsList::CreateSettingItemL( TInt aIdentifier )
    {
    CAknViewAppUi *AppUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
    CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
        					     ( AppUi->View( KGSCamcorderGSPluginUid ) );
    // Add storage place to storage list of settings values. 
    // ...create the storage place.
    TIntSetting* newSetting = new( ELeave ) TIntSetting;
    CleanupStack::PushL( newSetting );
    // ...set the identifier for this setting item.
    newSetting->iItemId = aIdentifier;
    if( aIdentifier != ECamSettingItemRestoreCameraSettings )
        {
        if ( parent )
            {
            newSetting->iValueId = parent->IntegerSettingValue( aIdentifier );
            }
        }
    else
        {
        newSetting->iValueId = 0;     
        }
    // ...add the new storage place item to the storage list.
    iSettings.AppendL( newSetting );
    CleanupStack::Pop( newSetting );

    // Create the list setting item
    CAknSettingItem* settingItem = NULL;
    TInt lastItem = iSettings.Count() - 1;
    // Setting items take reference to the value to be manipulated.
    TInt& valueId( iSettings[lastItem]->iValueId );

    switch ( aIdentifier )
        {
        case ECamSettingItemVideoAudioRec:
        case ECamSettingItemVideoShowCapturedVideo:
        case ECamSettingItemVideoOpZoomOff:
            {
            settingItem = 
                new( ELeave ) CAknBinaryPopupSettingItem( aIdentifier, valueId );
            }
            break;
        case ECamSettingItemVideoDigitalZoom:
            {
            if ( iModel.UiConfigManagerPtr() &&
                 iModel.UiConfigManagerPtr()->IsExtendedDigitalZoomSupported() )
                {
                settingItem = new( ELeave ) CAknBinaryPopupSettingItem( aIdentifier, valueId );          
                }
            break;
            }
        case ECamSettingItemVideoQuality:
            {
            if ( parent )
                {
                iQualityValue = parent->IntegerSettingValue( ECamSettingItemVideoQuality);
                }
            if ( !iLaunchedFromGS )
      	        {
                settingItem = new( ELeave )
                    CGSCamQualitySettingItem( ECamControllerVideo,
                                              aIdentifier,
                                              iQualityValue,
                                              iModel );
                }
            else
                {
                settingItem = new( ELeave ) 
                    CGSCustomQualitySettingItem( aIdentifier,
                                                 iQualityValue,
                                                 iModel,
                                                 ECamControllerVideo );
                }
            }
            break;
 
	    case ECamSettingItemRecLocation:
      	  {
          if ( !iLaunchedFromGS )
            {
            settingItem = 
                new( ELeave ) CCamLocationSettingItem( ECamControllerVideo,
                                                       aIdentifier, 
                                                       valueId );
            }
          else
            {
            settingItem = new( ELeave ) 
                CAknBinaryPopupSettingItem( aIdentifier, valueId );
            }
          }
      	  break;

        case ECamSettingItemVideoMediaStorage:
            {
            // Convert the setting to a memory enum
            iMemVal = static_cast<DriveInfo::TDefaultDrives>
                    ( CamUtility::MapToSettingsListMemory( 
                                  iSettings[lastItem]->iValueId ) );

            __ASSERT_DEBUG( iMemVal != KErrNotFound,
                            CamPanic( ECamPanicUnhandledCreateSettingItem ) );

		    // Get the root path of the default mass storage memory.
		    TInt driveInt;
		    TInt err = DriveInfo::GetDefaultDrive( iMemVal, driveInt );
		    iDrive = static_cast<TDriveNumber>(driveInt);

		    settingItem = new ( ELeave ) 
		                  CAknMemorySelectionSettingItemMultiDrive( aIdentifier, 
                                                                    iDrive );
            break;
            }
        case ECamSettingItemVideoNameBaseType:
            {
            if ( parent )
                {
                iDefaultNameBase = parent->TextSettingValue( 
                                            ECamSettingItemVideoNameBase );
                }
            TBool flag = ( iLaunchedFromGS || iSecondaryCameraSettings );
            settingItem = new( ELeave ) 
                          CGSCamDefaultNameSettingItem( iDefaultNameBase, 
                                                        ECamControllerVideo, 
                                                        aIdentifier, 
                                                        valueId,
                                                        flag );
      }
      break;


    case ECamSettingItemVideoStab:
      {
      if ( iModel.UiConfigManagerPtr() &&
           iModel.UiConfigManagerPtr()->IsVideoStabilizationSupported() )
          {
          settingItem = 
            new( ELeave ) CAknBinaryPopupSettingItem( aIdentifier, valueId );          
          }
      }
      break;
    case ECamSettingItemRestoreCameraSettings:
      {
      settingItem = new ( ELeave ) CAknBigSettingItemBase( aIdentifier );
      }
      break;
    case ECamSettingItemContinuousAutofocus: 
        {
        if ( iModel.UiConfigManagerPtr() &&
                   iModel.UiConfigManagerPtr()->IsContinuosAutofocusSupported() )
                  {
                  settingItem = 
                    new( ELeave ) CAknBinaryPopupSettingItem( aIdentifier, valueId );          
                  }
        
        break;
        }
    default:
      __ASSERT_DEBUG( EFalse, CamPanic( ECamPanicUnhandledCreateSettingItem ) );
      break;
    }

  PRINT( _L("Camera <= CCamVideoSettingsList::CreateSettingItemL") );
  return settingItem;
  }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsList::EditItemL
// Launch the setting page for the current item by calling
// EditItemL on it.
// ---------------------------------------------------------------------------
//
void CGSCamVideoSettingsList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {      
    PRINT( _L("Camera => CCamVideoSettingsList::EditItemL" ))
    CAknViewAppUi *AppUi = static_cast<CAknViewAppUi*>( 
                           CCoeEnv::Static()->AppUi() );

    CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
        					    ( AppUi->View( KGSCamcorderGSPluginUid ) );

    CAknSettingItemArray* itemArray = SettingItemArray();
    CAknSettingItem* editedItem = itemArray->At(aIndex);
    
    if ( editedItem->Identifier() == ECamSettingItemRestoreCameraSettings )
        {
        TInt response = 0;
        if ( parent )
        	{
        	response = parent->DisplayRestoreSettingsDlgL();
        	}
        // If launched from GS and response "Yes" reload settings page to update values 
        if ( iLaunchedFromGS && response )
        	{
        	AppUi->ActivateLocalViewL( KGSCamcorderGSPluginUid );
        	}
        return;
        }

    // Edit item via setting page or in-place.
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
    // Write the new value for the edited control to its stored data.
    editedItem->StoreL();

    // Update the settings model so that the navipane is updated.
    TInt settingValue;
    // ...If the changed setting is the media storage setting
    // ...then convert the memory value to settings model type value.
    if ( editedItem->Identifier() == ECamSettingItemVideoMediaStorage )
        {
        iMemVal = static_cast<DriveInfo::TDefaultDrives>(CamUtility::GetDriveTypeFromDriveNumber( iDrive ));
        settingValue = CamUtility::MapFromSettingsListMemory( iMemVal );
        //__ASSERT_DEBUG( settingValue != KErrNotFound, CamPanic( ECamPanicUnknownSettingItem ) );
        }
    else if ( editedItem->Identifier() == ECamSettingItemVideoQuality )
        {
        settingValue = iQualityValue;
        }
    // ...Otherwise, no conversion necessary.
    else
        {
        settingValue = ValueOfSetting( editedItem->Identifier() );
        }
    // ...Update the settings model with the new value.
    if ( parent )
        {
        parent->SetIntegerSettingValueL( editedItem->Identifier(),
               							 settingValue );
        }

    // If the setting is for the video base name then store the name.
    if ( editedItem->Identifier() == ECamSettingItemVideoNameBaseType  
         && parent )
        {
        parent->SetTextSettingValueL( ECamSettingItemVideoNameBase,
                                      iDefaultNameBase );
        }

    PRINT( _L("Camera <= CCamVideoSettingsList::EditItemL" ))
    }

// ---------------------------------------------------------------------------
// CGSCamVideoSettingsList::ValueOfSetting
// Returns the current value of the a setting for this list.
// Note, this is not the value currently stored in the settings model, as these
// may differ.
// ---------------------------------------------------------------------------
//
TInt CGSCamVideoSettingsList::ValueOfSetting( TInt aItemId ) const
    {
    TInt itemIndex = IndexOfSetting( aItemId );
    return iSettings[itemIndex]->iValueId;
    }

// ---------------------------------------------------------------------------
// CGSCamVideoSettingsList::IndexOfSetting
// Returns the array index of a setting for this list.
// ---------------------------------------------------------------------------
//
TInt CGSCamVideoSettingsList::IndexOfSetting( TInt aItemId ) const
    {
    TInt i;
  TInt itemTotal = iSettings.Count();
    for ( i = 0; i < itemTotal; ++i )
        {
        if ( iSettings[i]->iItemId == aItemId )
            {
            return i;
            }
        }

    // Should never get here.
    // Remove lint warning
    return KErrNotFound;
    }

   
// ---------------------------------------------------------------------------
// CGSCamVideoSettingsList::SizeChanged
// Set the size and position of component controls.
// ---------------------------------------------------------------------------
//
void CGSCamVideoSettingsList::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();
    CEikListBox* lb = ListBox();
    if( lb )
        {
        lb->SetRect( Rect() );  // Set container's rect to listbox
        }
    }


// End of File

