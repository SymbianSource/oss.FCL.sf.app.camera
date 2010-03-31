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
* Description:  Image settings list class
*
*/


// INCLUDE FILES
 
#include <bldvariant.hrh>
#include <CAknMemorySelectionSettingItem.h> // CAknMemorySelectionSettingItem
#include <featmgr.h>
#include <gscamerapluginrsc.rsg>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <AknWaitDialog.h>
#include <CAknMemorySelectionSettingItemMultiDrive.h>
#include <driveinfo.h> // DriveInfo
#include <AknCommonDialogsDynMem.h>

#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "CamPanic.h"
#include "CamAppUiBase.h"
#include "CamUtility.h"
#include "MCamAppController.h"
#include "GSCamPhotoSettingsList.h"
#include "GSCamcorderPlugin.h"
#include "GSCamQualitySettingItem.h"
#include "GSCustomQualitySettingItem.h"
#include "GSCamDefaultNameSettingItem.h"
#include "CameraUiConfigManager.h"
#include "CamCollectionManagerAo.h"
#include "CamWaitDialog.h"
#include "CamLocationSettingItem.h"
#include "camconfiguration.h"
#include "GSCamCaptureToneSettingItem.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsList::CGSCamPhotoSettingsList
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamPhotoSettingsList::CGSCamPhotoSettingsList( CCamStaticSettingsModel& aModel )
: iModel( aModel )
    {
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsList::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CGSCamPhotoSettingsList::ConstructL( TBool aLaunchedFromGS,
                                          TBool aSecondaryCameraSettings )
    {
    CAknViewAppUi *AppUi = static_cast<CAknViewAppUi*>
                           ( CCoeEnv::Static()->AppUi() );

    CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
        					   ( AppUi->View( KGSCamcorderGSPluginUid ) );
    if ( parent )
        {
        parent->LoadVideoStaticSettingsL( ETrue );
        parent->LoadPhotoStaticSettingsL( ETrue );
        }

    iLaunchedFromGS = aLaunchedFromGS;
    iSecondaryCameraSettings = aSecondaryCameraSettings;
    iSettings.ResetAndDestroy();
    iModel.Configuration().GetPsiIntArrayL( ECamPsiSupportedStillQualities, iSupportedQualityLevels );
    
    TInt resId;
    if ( iModel.UiConfigManagerPtr() &&
         iModel.UiConfigManagerPtr()->IsLocationSupported() )
        {
        resId =  iLaunchedFromGS?R_CAM_PHOTO_GSSETTINGS_WITH_LOCATION_ITEM_LIST_CAMCORDER:
                                 R_CAM_PHOTO_SETTINGS_WITH_LOCATION_ITEM_LIST_CAMCORDER;
        }
    else
        {
        resId =  iLaunchedFromGS?R_CAM_PHOTO_GSSETTINGS_ITEM_LIST_CAMCORDER:
                                 R_CAM_PHOTO_SETTINGS_ITEM_LIST_CAMCORDER;
        }
    
    if ( iSecondaryCameraSettings )
        {
        resId =  R_CAM_PHOTO_SETTINGS_ITEM_LIST_PORTRAIT;
        }
    ConstructFromResourceL( resId );
    }


// Destructor
CGSCamPhotoSettingsList::~CGSCamPhotoSettingsList()
    {
    iSettings.ResetAndDestroy();
    iSettings.Close();
    iSupportedQualityLevels.Close(); 
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsList::CreateSettingItemL
// From CAknSettingItemList Handles creating setting items
// ---------------------------------------------------------------------------
//
CAknSettingItem* CGSCamPhotoSettingsList::CreateSettingItemL( TInt aIdentifier )
    {
    // Add storage place to storage list of settings values. 
    // ...create the storage place.
    CAknViewAppUi *AppUi = static_cast<CAknViewAppUi*>
                           ( CCoeEnv::Static()->AppUi() );

    CGSCamcorderPlugin* parent = static_cast<CGSCamcorderPlugin*> 
        					   ( AppUi->View( KGSCamcorderGSPluginUid ) );

    TIntSetting* newSetting = new( ELeave ) TIntSetting;
    CleanupStack::PushL( newSetting );
    // ...set the identifier for this setting item.
    newSetting->iItemId = aIdentifier;
    if( aIdentifier != ECamSettingItemRestoreCameraSettings && parent )
        {
        newSetting->iValueId = parent->IntegerSettingValue( aIdentifier );
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
    case ECamSettingItemPhotoShowFocusPoint:
    case ECamSettingItemShowCapturedPhoto:
      {
      settingItem = new( ELeave ) 
          CAknBinaryPopupSettingItem( aIdentifier, valueId );	
      }
      break;

    case ECamSettingItemPhotoDigitalZoom:
      {
      if ( iModel.UiConfigManagerPtr() &&
              iModel.UiConfigManagerPtr()->IsExtendedDigitalZoomSupported() )
          {
          settingItem = new( ELeave ) 
                          CAknEnumeratedTextPopupSettingItem( aIdentifier, valueId );          
          }
      }
      break;

    case ECamSettingItemPhotoQuality:
      {
      if ( parent )
          {
          iQualityValue = parent->IntegerSettingValue( ECamSettingItemPhotoQuality );
          }
      if ( !iLaunchedFromGS )
          {
          settingItem = new( ELeave )
                  CGSCamQualitySettingItem( ECamControllerImage,
                                            aIdentifier,
                                            iQualityValue,
                                            iModel
                                          );
          }
      else
          {
          settingItem = new( ELeave ) 
                  CGSCustomQualitySettingItem( aIdentifier, 
                                               iQualityValue,
                                               iModel,
                                               ECamControllerImage
                                              );
          }
       }
       break;
     case ECamSettingItemPhotoCaptureTone:
            {
            settingItem = new( ELeave ) 
                CGSCamCaptureToneSettingItem( aIdentifier, valueId );    
            }
            break;

        case ECamSettingItemImageRotation:
            {
            if ( iModel.UiConfigManagerPtr() &&
                 iModel.UiConfigManagerPtr()->IsOrientationSensorSupported() )
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

        case ECamSettingItemPhotoMediaStorage:
            {
            // Convert the setting to a memory enum
            iMemVal = static_cast<DriveInfo::TDefaultDrives>
                    ( CamUtility::MapToSettingsListMemory( valueId ) );
            
		    // Get the root path of the default mass storage memory.
		    if(iMemVal != KErrNotFound)
		        {
                TInt driveInt;
		        TInt err = DriveInfo::GetDefaultDrive( iMemVal, driveInt );
		        iDrive = static_cast<TDriveNumber>(driveInt);
		        }
		    else
		        {
                iDrive = static_cast<TDriveNumber>(0); //Doesn't matter
		        }
		    TInt includedMedias = AknCommonDialogsDynMem::EMemoryTypeMMCExternal |
                                 AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage;   
		    settingItem = new ( ELeave ) 
		                  CAknMemorySelectionSettingItemMultiDrive(
		                                                      aIdentifier,
                                                              iDrive
                                                              );
		    static_cast<CAknMemorySelectionSettingItemMultiDrive*>(settingItem)
		                        ->SetIncludedMediasL(includedMedias);
            }
            break;
 
        case ECamSettingItemPhotoNameBaseType:
            {
            if ( parent )
                {
                iDefaultNameBase = parent->TextSettingValue(
                                   ECamSettingItemPhotoNameBase );
                TBool flag = ( iLaunchedFromGS || iSecondaryCameraSettings );
                settingItem = 
                    new( ELeave ) CGSCamDefaultNameSettingItem( iDefaultNameBase, 
                                                                ECamControllerImage,
                                                                aIdentifier,
                                                                valueId, 
                                                                flag );
                }
             }
            break;          

    case ECamSettingItemRecLocation:
      {
      if ( !iLaunchedFromGS )
        {
        settingItem = 
            new( ELeave ) CCamLocationSettingItem( ECamControllerImage,
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

    default:
       {
       break;
       }
     }
    return settingItem;
    }



// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsList::EditItemL
// Launch the setting page for the current item by calling
// EditItemL on it.
// ---------------------------------------------------------------------------
//
void CGSCamPhotoSettingsList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknViewAppUi *AppUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
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
    if ( editedItem->Identifier() == ECamSettingItemPhotoMediaStorage )
        {
        iMemVal = static_cast<DriveInfo::TDefaultDrives>(CamUtility::GetDriveTypeFromDriveNumber( iDrive ));
        settingValue = CamUtility::MapFromSettingsListMemory( iMemVal );
        settingValue = ( (settingValue == ECamMediaStoragePhone) && 
                         parent->IntegerSettingValue(ECamSettingItemRemovePhoneMemoryUsage)  )?
                                 ECamMediaStorageNone:settingValue;
        }
    else if ( editedItem->Identifier() == ECamSettingItemPhotoQuality )
        {
        settingValue = iQualityValue;
        }
    else
        {
        settingValue = ValueOfSetting( editedItem->Identifier() );
        }
   
      if ( parent )
        {
        parent->SetIntegerSettingValueL( editedItem->Identifier(), settingValue );
        }

    // If the setting is for the photo base name then store the name.
      if ( parent )
        {
        parent->SetTextSettingValueL( ECamSettingItemPhotoNameBase,
                                      iDefaultNameBase );
        }

    }

// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsList::ValueOfSetting
// Returns the current value of the a setting for this list.
// Note, this is not the value currently stored in the settings model, as these
// may differ.
// ---------------------------------------------------------------------------
//
TInt CGSCamPhotoSettingsList::ValueOfSetting( TInt aItemId ) const
    {
    TInt itemIndex = IndexOfSetting( aItemId );
    return iSettings[itemIndex]->iValueId;
    }

// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsList::IndexOfSetting
// Returns the array index of a setting for this list.
// ---------------------------------------------------------------------------
//
TInt CGSCamPhotoSettingsList::IndexOfSetting( TInt aItemId ) const
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
// CGSCamPhotoSettingsList::SizeChanged
// Set the size and position of component controls.
// ---------------------------------------------------------------------------
//
void CGSCamPhotoSettingsList::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();

    CEikListBox* lb = ListBox();
    if( lb )
        {
        lb->SetRect( Rect() );  // Set container's rect to listbox
        }
    }

// End of File

