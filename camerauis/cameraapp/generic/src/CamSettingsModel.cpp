/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manages all settings data.*
*/


// ===========================================================================
// Includes
#include <StringLoader.h>
#include <barsread.h>
#include <AknQueryDialog.h>
#include <ecam.h>
#include <data_caging_path_literals.hrh>  // KDC_APP_RESOURCE_DIR

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamSettingsModel.h"
#include "CamPanic.h"
#include "mcamsettingsmodelobserver.h"
#include "CamAppUiBase.h"
#include "CamSharedDataKeys.h"
#include "CamUtility.h"
#include "CamVideoQualityLevel.h"

#include "CameraVariant.hrh" // Variant CR key values
#include "CameraappPrivateCRKeys.h" // CR keys
#include "camactivepalettehandler.h"
#include "CamAppUi.h"
#include "camsettingconversion.h"
#include "camconfiguration.h"
#include "CamStaticSettingsModel.h"
#include "CameraUiConfigManager.h"

// ===========================================================================
// Constants

const TInt KDynamicArrayGranularity  =  3;
_LIT( KCamNhdResourceFileName, "z:nhdcamsettings.rss" );  
_LIT( KCamVgaResourceFileName, "z:vgacamsettings.rss" );  


// ===========================================================================
// Local methods
inline TBool SettingIdMatches( const TInt*        aSettingId, 
                               const TIntSetting& aSettingItem )
  {
  return (*aSettingId == aSettingItem.iItemId);
  };

inline TInt FindIndex( const RPointerArray<TIntSetting>& aSettingArray,
                       const TInt&                       aSettingId    )
  {
  return aSettingArray.Find( aSettingId, SettingIdMatches );
  };

inline TInt FindIndex( const RPointerArray<TIntSetting>& aSettingArray,
                       const TIntSetting&                aSettingItem  )
  {
  return FindIndex( aSettingArray, aSettingItem.iItemId );
  };


// ===========================================================================
// Class methods


// ---------------------------------------------------------------------------
// TCamVariantInfo constructor
// ---------------------------------------------------------------------------
//
TCamVariantInfo::TCamVariantInfo() 
  : iFlags           ( -1 ),
    iTimeLapseSupport( ECamSupportUnknown ),
    iAlwaysOnSupport ( ECamSupportUnknown ),
    iVersion         ( -1, 0, 0           ) 
  {
  }
    
// ===========================================================================

// ---------------------------------------------------------------------------
// CCamSettingsModel::NewL
// Symbian OS two-phased constructor 
// ---------------------------------------------------------------------------
//
CCamSettingsModel* CCamSettingsModel::NewL( CCamConfiguration& aConfiguration )
    {
    CCamSettingsModel* self = CCamSettingsModel::NewLC( aConfiguration );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamSettingsModel* CCamSettingsModel::NewLC( CCamConfiguration& aConfiguration )
    {
    CCamSettingsModel* self = new( ELeave ) CCamSettingsModel( aConfiguration );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::ResetUserSceneWithoutActivatingChangesL
// Resets all user scene settings to their defaults without activating them.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::ResetUserSceneWithoutActivatingChangesL()
    {
    PRINT( _L("Camera => CCamSettingsModel::ResetUserSceneWithoutActivatingChangesL"))		

    // Reset the user scene data in the repository
    iStaticModel->ResetSettingItem( KCamCrUserSceneBaseScene );
    iStaticModel->ResetSettingItem( KCamCrUserSceneWhiteBalance );
    iStaticModel->ResetSettingItem( KCamCrUserSceneColourFilter );
    iStaticModel->ResetSettingItem( KCamCrUserSceneFlash );
    iStaticModel->ResetSettingItem( KCamCrUserSceneExposure );

    if ( iUiConfigManager->IsISOSupported() )
        {
        iStaticModel->ResetSettingItem( KCamCrUserSceneLightSensitivity );
        }
    iStaticModel->ResetSettingItem( KCamCrUserSceneContrast );
    iStaticModel->ResetSettingItem( KCamCrUserSceneImageSharpness );
    iStaticModel->ResetSettingItem( KCamCrUserSceneDefault ); 

    // Reload the static user scene settings
    iUserSceneSettings.ResetAndDestroy();
    iStaticModel->ReadFromCenRepL(
                    static_cast<TInt>( ECamSettingItemUserSceneRangeMin ),
                    static_cast<TInt>( ECamSettingItemUserSceneRangeMax ),
                    iUserSceneSettings
                   );
    PRINT( _L("Camera <= CCamSettingsModel::ResetUserSceneWithoutActivatingChangesL"))		
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::ResetUserSceneL
// Resets all user scene settings to their defaults.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::ResetUserSceneL()
    {
    PRINT( _L("Camera => CCamSettingsModel::ResetUserSceneL"))	
    // Reset the user scene based on scene in the repository to get the default value
    ResetUserSceneWithoutActivatingChangesL();
    TInt scene = IntegerSettingValue( ECamSettingItemDynamicPhotoScene );
    // If current scene is user scene
    if ( ECamSceneUser == scene )
        {
        // if the user scene is currently the photo scene then
        // activate all the default scene settings to the AP
        ActivateUserSceneSettingsL();
        }
    PRINT( _L("Camera <= CCamSettingsModel::ResetUserSceneL"))	    
    }



// ---------------------------------------------------------------------------
// CCamSettingsModel::IntegerSettingValue
// Returns the current integer value for the specified setting
// ---------------------------------------------------------------------------
//
TInt 
CCamSettingsModel::IntegerSettingValue( TInt aSettingItem ) const
  {
  PRINT2( _L("Camera => CCamSettingsModel::IntegerSettingValue, id:%d [%s]"), 
          aSettingItem,
          KCamSettingItemNames[aSettingItem] );

  TInt value = KErrNotFound;
  // If the requested item is the photo quality value, this must
  // be filtered based on the current scene.
  if ( aSettingItem == ECamSettingItemPhotoQuality )
    {
    value = CurrentPhotoQualityValue();
    }
  // If the requested item is the photo size, this must be filtered based on
  // the current scene
  else if ( aSettingItem == ECamSettingItemPhotoSize )
    {
    value = CurrentPhotoResolution();
    }
  // If the requested item is the photo compression level, this must
  // be filtered based on the current scene, and burst mode.
  else if ( aSettingItem == ECamSettingItemPhotoCompression )
    {
    value = CurrentPhotoCompression();
    }
  else
    {
    // If we have previews active, check first if requested
    // setting is found among them. Settings model must pretend
    // that the current value is the one set to preview.
    TInt previewIndex( FindIndex( iSettingPreviews, aSettingItem ) );
    if( KErrNotFound != previewIndex )
      {
      PRINT( _L("Camera <> Setting item value in previewed currently!") );
      value = iSettingPreviews[previewIndex]->iValueId;
      
      if (ECamSettingItemDynamicSelfTimer == aSettingItem)     		//If the current setting Item is selftimer  
        {
        TInt settingIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings, aSettingItem );  
        value = iDynamicPhotoIntSettings[settingIndex]->iValueId;	//Get the actual selftime setting value but not preview value
        }
      }
    // Otherwise, if setting item is in the dynamic photo settings return it's value.
    else if( ECamSettingItemDynamicPhotoRangeMax > aSettingItem 
          && ECamSettingItemDynamicPhotoRangeMin < aSettingItem )
      {
      TInt settingIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings, aSettingItem );
      value = iDynamicPhotoIntSettings[settingIndex]->iValueId;
      }
    // Otherwise, if setting item is in the dynamic video settings return it's value.
    else if( ECamSettingItemDynamicVideoRangeMax > aSettingItem 
          && ECamSettingItemDynamicVideoRangeMin < aSettingItem )
      {
      TInt settingIndex = SearchInSettingsListFor( iDynamicVideoIntSettings, aSettingItem );
      value = iDynamicVideoIntSettings[settingIndex]->iValueId;
      }
    // Otherwise, if setting item is in the user scene settings return it's value.
    else if( ECamSettingItemUserSceneRangeMax > aSettingItem 
          && ECamSettingItemUserSceneRangeMin < aSettingItem )
      {
      TInt settingIndex = SearchInSettingsListFor( iUserSceneSettings, aSettingItem );
      value = iUserSceneSettings[settingIndex]->iValueId;
      }
    // Otherwise, if setting item is in the static video settings return it's value.
    else if( ECamSettingItemVideoClipLength == aSettingItem )
        {
        // Video clip length workaround
        // PRINT( _L("Camera <> ECamSettingItemVideoClipLength") );  
        TInt videoQuality = IntegerSettingValue( ECamSettingItemVideoQuality );
        value = iQualityLevels[videoQuality]->VideoLength();
        }
    else
        {
        value = iStaticModel->IntegerSettingValue( aSettingItem );
        }
    }

  PRINT1( _L("Camera <= CCamSettingsModel::IntegerSettingValue, value:%d"), value );
  return value;
  }




// ---------------------------------------------------------------------------
// CCamSettingsModel::SceneSettingValue
// Returns the setting value for a specific field of a scene 
// ---------------------------------------------------------------------------
//
TInt 
CCamSettingsModel::SceneSettingValue( TInt aSceneId, 
                                      TInt aSceneField ) const
  {
  PRINT2( _L("CCamSettingsModel::SceneSettingValue, scene[%s] setting[%s]"), 
          KCamSceneNames[aSceneId],
          KCamSettingItemNames[aSceneField] );

  // If setting item is a scene in the photo scene list
  TInt sceneIndex = FindSceneInSceneList( aSceneId, iPhotoScenes );
  if ( sceneIndex != KErrNotFound )
    {
    // Search for the field in the scene's field list.
    const RPointerArray<TIntSetting>& sceneSettings = 
                                        iPhotoScenes[sceneIndex]->iSettings;
    TInt fieldCount = sceneSettings.Count();
    for ( TInt i = 0; i < fieldCount; ++i )
      {
      if ( sceneSettings[i]->iItemId == aSceneField )
        {
        return sceneSettings[i]->iValueId;
        }
      }
    }

  // Otherwise if setting item is a scene in the video scene list
  sceneIndex = FindSceneInSceneList( aSceneId, iVideoScenes );
  if ( sceneIndex != KErrNotFound )
    {
    // Search for the field in the scene's field list.
    const RPointerArray<TIntSetting>& sceneSettings = 
                                        iVideoScenes[sceneIndex]->iSettings;
    TInt fieldCount = sceneSettings.Count();
    for ( TInt i = 0; i < fieldCount; ++i )
      {
      if ( sceneSettings[i]->iItemId == aSceneField )
        {
        return sceneSettings[i]->iValueId;
        }
      }
    }
  
  else
    {
    PRINT( _L("Camera => CCamSettingsModel::ECamPanicUnknownSettingItem 3" ))
    CamPanic( ECamPanicUnknownSettingItem );
    }
  
  return sceneIndex;
  }

// ---------------------------------------------------------------------------
// CCamSettingsModel::SetIntegerSettingValueL
// Sets a new integer value for the specified setting
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::SetIntegerSettingValueL( TInt aSettingItem, 
                                            TInt aSettingValue )
  {
  PRINT ( _L("Camera => CCamSettingsModel::SetIntegerSettingValueL") );
  CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEnv->AppUi() );
  TCamSettingItemIds settingId( 
      static_cast<TCamSettingItemIds>(aSettingItem) );

  PRINT2( _L("Camera <> Setting item[%s] value:%d"), KCamSettingItemNames[settingId], aSettingValue );

  // -------------------------------------------------------
  // Dynamic image settings
  if( ECamSettingItemDynamicPhotoRangeMin < settingId
   && ECamSettingItemDynamicPhotoRangeMax > settingId )
    {
    TInt settingIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings, 
                                                 settingId );
    iDynamicPhotoIntSettings[settingIndex]->iValueId = aSettingValue;
    
    // Changing scene
    if ( ECamSettingItemDynamicPhotoScene == settingId )
      {
       if ( ECamActiveCameraNone != appUi->ActiveCamera() || ( ECamActiveCameraNone == appUi->ActiveCamera() && ECamSceneAuto == aSettingValue ) )
           {
           PhotoSceneHasChangedL( aSettingValue );
           }
      }
    }
  // -------------------------------------------------------
  // Dynamic video settings
  else if( ECamSettingItemDynamicVideoRangeMin < settingId
        && ECamSettingItemDynamicVideoRangeMax > settingId )
    {
    TInt settingIndex = SearchInSettingsListFor( iDynamicVideoIntSettings, 
                                                 settingId );
    iDynamicVideoIntSettings[settingIndex]->iValueId = aSettingValue;
    if ( settingId == ECamSettingItemDynamicVideoScene )
      {
      VideoSceneHasChangedL( aSettingValue );
      }
    }
  // -------------------------------------------------------
  // User scene settings
  else if( ECamSettingItemUserSceneRangeMin < settingId
        && ECamSettingItemUserSceneRangeMax > settingId )
    {
    TBool userSceneModeSettingChanged = EFalse;
    // If the based-on scene has changed
    if ( settingId == ECamSettingItemUserSceneBasedOnScene )
      {
      TInt scene = IntegerSettingValue( ECamSettingItemDynamicPhotoScene );
      // If current scene is user scene
      if ( ECamSceneUser == scene )
        {
        // Handle other changes to the settings required by the change in scene.
        UserSceneHasChangedL( aSettingValue, ETrue );
        }
      // Otherwise the user scene hasn't been selected as the active scene
      // - the change option has just been selected in the scene setting list
      else 
        {
        // just handle a change in the scene
        UserSceneHasChangedL( aSettingValue, EFalse );
        }
      userSceneModeSettingChanged = ETrue;
      }
    // Writing values to CenRep for all the settings.
    TInt settingIndex = SearchInSettingsListFor( iUserSceneSettings, settingId );
    iUserSceneSettings[settingIndex]->iValueId = aSettingValue;
    SaveStaticSettingL( settingId );
    
    // If there is only one setting changed other than scene mode setting 
    // and if the Dynamic Scene mode is "User Scene" we update the new setting
    // value to the camera dynamic settings
    if ( !userSceneModeSettingChanged && settingId != ECamSettingItemUserSceneDefault ) 
      {
      TInt scene = IntegerSettingValue( ECamSettingItemDynamicPhotoScene );
      // If user scene is the currently active photo scene then activate the
      // new user scene settings.
      if ( ECamSceneUser == scene )
        {
        CopyUserSceneSettingToCaptureSettingL( settingId );
        }
      }
    
    }
  // -------------------------------------------------------
  else
    {
    // use the static settings model
    iStaticModel->SetIntegerSettingValueL( aSettingItem, aSettingValue );
    }
  // -------------------------------------------------------
 
  NotifyObservers( settingId, aSettingValue ); 

  PRINT ( _L("Camera <= CCamSettingsModel::SetIntegerSettingValueL") );
  }


// ---------------------------------------------------------------------------
// CCamSettingsModel::SetTextSettingValueL
// Sets a new text value for the specified setting
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::SetTextSettingValueL( TInt aSettingItem,
                                              const TDesC& aSettingValue )
    {
    iStaticModel->SetTextSettingValueL( aSettingItem, aSettingValue );
    NotifyObservers( (TCamSettingItemIds)aSettingItem, aSettingValue );
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::TextSettingValue
// Returns the current text value for the specified setting
// ---------------------------------------------------------------------------
//
TPtrC 
CCamSettingsModel::TextSettingValue( TInt aSettingItem ) const
  {
  return iStaticModel->TextSettingValue( aSettingItem );
  }

// ---------------------------------------------------------------------------
// CCamSettingsModel::SettingValueEnabled
// Returns ETrue if the setting value is currently enabled
// ---------------------------------------------------------------------------
//
TBool CCamSettingsModel::SettingValueEnabled( TInt aSettingItem, 
                                             TInt aSettingValue ) const
    {
    // The high quality video setting is not supported if
    // the second camera is enabled
    if ( aSettingItem == ECamSettingItemVideoQuality && 
            aSettingValue == ECamVideoQualityHigh &&
            static_cast<CCamAppUiBase*>( 
            iEnv->AppUi() )->IsSecondCameraEnabled() )
        {
        return EFalse;
        }
    // All other setting item values are allowed
    return ETrue;
    }



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel
::AttachObserverL( const MCamSettingsModelObserver* aObserver )
  {
  if( aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if( KErrNotFound == index )
      {
      iObservers.AppendL( aObserver );
      }
    }
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSettingsModel
::DetachObserver( const MCamSettingsModelObserver* aObserver )
  {
  if( aObserver )
    {
    TInt index = iObservers.Find( aObserver );
    if( KErrNotFound != index )
      {
      iObservers.Remove( index );
      }
    }
  }



// ---------------------------------------------------------------------------
// CCamSettingsModel::LoadSettingsL
// Loads the settings from shared data, or if unable to from the resource file. 
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::LoadSettingsL()   
  {
  PRINT( _L("Camera => CCamSettingsModel::LoadSettingsL"))
  // Remove any previously loaded values.
  UnloadSettings();
  
  PRINT( _L("Camera => CCamSettingsModel::LoadSettingsL B"))
  LoadDynamicSettingsL( R_CAM_PHOTO_DYNAMIC_SETTINGS_DATA, 
                        iDynamicPhotoIntSettings, 
                        iDynamicPhotoIntDefaults );
    
  PRINT( _L("Camera => CCamSettingsModel::LoadSettingsL C"))
  LoadDynamicSettingsL( R_CAM_VIDEO_DYNAMIC_SETTINGS_DATA, 
                        iDynamicVideoIntSettings, 
                        iDynamicVideoIntDefaults );
  
  PRINT( _L("Camera => CCamSettingsModel::LoadSettingsL D"))
  // Load image scene data
  LoadSceneDataL();

  // Load video scene data
  LoadSceneDataL( EFalse );
  
  PRINT( _L("Camera => CCamSettingsModel::LoadSettingsL E"))
  //LoadScenesDataL( R_CAM_VIDEO_SCENES_DATA, iVideoScenes );
  
  PRINT( _L("Camera <= CCamSettingsModel::LoadSettingsL"))
  }
    



// ---------------------------------------------------------------------------
// DynamicSettingDefaultValue
//
// ---------------------------------------------------------------------------
//
TInt 
CCamSettingsModel::DynamicSettingDefaultValue( TInt aSettingId ) const
  {
  TInt value = KErrNotFound;

  TInt index = SearchInSettingsListFor( iDynamicPhotoIntSettings, aSettingId );
  if( KErrNotFound != index )
    {
  	value = iDynamicPhotoIntDefaults[index];
    }
  index = SearchInSettingsListFor( iDynamicVideoIntSettings, aSettingId );
  if( KErrNotFound != index )
    {
  	value = iDynamicVideoIntDefaults[index];
    }    

  return value;
  }

// ---------------------------------------------------------------------------
// CCamSettingsModel::SetDynamicSettingsToDefaults
// Sets all dynamic settings to their defaults. 
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::SetDynamicSettingsToDefaults()
    {
    // Set photo dynamic settings to their defaults.
    TInt i;
    TInt settingsListCount = iDynamicPhotoIntSettings.Count();
    for ( i = 0; i < settingsListCount; ++i )
        {
        iDynamicPhotoIntSettings[i]->iValueId = iDynamicPhotoIntDefaults[i];
        }
    // Set video dynamic settings to their defaults.
    settingsListCount = iDynamicVideoIntSettings.Count();
    for ( i = 0; i < settingsListCount; ++i )
        {
        iDynamicVideoIntSettings[i]->iValueId = iDynamicVideoIntDefaults[i];
        }
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::SetDynamicSettingsToDefault
// Sets a dynamic setting to its default. 
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::SetDynamicSettingToDefault( TInt aSettingItem )
    {
    // Set photo dynamic settings to their defaults.
    TInt i;
    TBool found = EFalse;
    TInt settingsListCount = iDynamicPhotoIntSettings.Count();
    for ( i = 0; i < settingsListCount && !found; ++i )
        {
        if ( aSettingItem == iDynamicPhotoIntSettings[i]->iItemId )
            {
            iDynamicPhotoIntSettings[i]->iValueId = iDynamicPhotoIntDefaults[i];
            found = ETrue;
            }
        }
    // Set video dynamic settings to their defaults.
    settingsListCount = iDynamicVideoIntSettings.Count();
    for ( i = 0; i < settingsListCount && !found; ++i )
        {
        if ( aSettingItem == iDynamicVideoIntSettings[i]->iItemId )
            {
            iDynamicVideoIntSettings[i]->iValueId = iDynamicVideoIntDefaults[i];
            found = ETrue;
            }
        }

    // NB no error is reported is it's not found
    }


// ---------------------------------------------------------------------------
// virtual SetDynamicSettingToSceneDefaultL
//
// Sets a dynamic setting to scene default. 
// ---------------------------------------------------------------------------
//
TInt
CCamSettingsModel::SetDynamicSettingToSceneDefaultL( TInt  aSceneId,
                                                     TInt  aSceneField,
                                                     TInt  aSettingId,
                                                     TBool aVideoScene )
  {
  PRINT( _L("Camera => CCamSettingsModel::SetDynamicSettingToSceneDefault") );

  RPointerArray<TIntSetting>* sceneSettings = NULL;
  TInt                        sceneIndex    = KErrNotFound;

  if( !aVideoScene )
    { 
    sceneIndex = FindSceneInSceneList( aSceneId, iPhotoScenes );
    if( KErrNotFound != sceneIndex )
      {
      sceneSettings = &(iPhotoScenes[sceneIndex]->iSettings);
      }
    }
  else
    {
    sceneIndex = FindSceneInSceneList( aSceneId, iVideoScenes );
    if( KErrNotFound != sceneIndex )
      {
      sceneSettings = &(iVideoScenes[sceneIndex]->iSettings);
      }
    }
 
  TInt settingIndex = KErrNotFound;
  if( sceneSettings )
    {
    settingIndex = SearchInSettingsListFor( *sceneSettings, aSceneField );
    if( settingIndex != KErrNotFound )
      {
      SetIntegerSettingValueL( aSettingId, 
                              (*sceneSettings)[settingIndex]->iValueId );
      }
    }

  PRINT( _L("Camera <= CCamSettingsModel::SetDynamicSettingToSceneDefault") );
  return settingIndex;
  }



// ---------------------------------------------------------------------------
// CCamSettingsModel::LoadStaticSettingsL
// Loads the static settings from shared data. Required to update
// the settings whenever get foreground event, incase of external
// changes to the settings.
// Note that static settings cannot be loaded until the AppUi has been created,
// as which settings to use is dependent on if app is embedded or not.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::LoadStaticSettingsL( TBool aIsEmbedded )
    {
    PRINT(_L("Camera => CCamSettingsModel::LoadStaticSettingsL" ))
    iEmbedded = aIsEmbedded;

    // The model needs to register as an observer of changes in burst mode
    // activation, as burst mode limits some settings values.
    // The model registers with the AppUi which is created after the model.
    // Registration is done here so we can guarantee that the AppUi has 
    // been created.
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
    appUi->AddBurstModeObserverL( this );

    // Free all memory currently used by the static settings.
    UnloadStaticSettings();
    
    iStaticModel->ReadFromCenRepL(
                    static_cast<TInt>( ECamSettingItemUserSceneRangeMin ),
                    static_cast<TInt>( ECamSettingItemUserSceneRangeMax ),
                    iUserSceneSettings
                   );
    iStaticModel->LoadStaticSettingsL( aIsEmbedded );
    if ( appUi->ReturningFromPretendExit() || appUi->IsFirstBoot() )
      {
      SetUserSceneDefault(); 
      }

    PRINT(_L("Camera <= CCamSettingsModel::LoadStaticSettingsL" ))

    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::SaveSettingsL
// Saves the static settings to the shared data ini file
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::SaveSettingsL() 
  {
  // This method is removed as we save to the CenRep whenever some setting
  // is changed directly .. refer to SaveStaticSettingsL method
  }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void
CCamSettingsModel::SaveStaticSettingL( TCamSettingItemIds aSettingId )
  {
  PRINT1( _L("Camera => CCamSettingsModel::SaveStaticSettingL, setting:[%s]"), 
          KCamSettingItemNames[aSettingId] );
  TBool callStaticModel = ETrue;
  
    {
    // All the User Specific Settings are saved here in the CenRep
    // and the remaining static settings are handled in the static model
    RPointerArray<TIntSetting>* settingArray = 
          MapSettingItem2SettingsList( aSettingId );

    if( settingArray == &iUserSceneSettings )
        {
        callStaticModel = EFalse;
        TUint32 crKey = CCamStaticSettingsModel::MapSettingItem2CRKey( aSettingId, iEmbedded );
        iStaticModel->SaveCenRepItemL( aSettingId, crKey, *settingArray );
        }
    }
  if ( callStaticModel )
    {
    iStaticModel->SaveStaticSettingL( aSettingId );
    }
  PRINT( _L("Camera <= CCamSettingsModel::SaveStaticSettingL") ); 
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
RPointerArray<TIntSetting>*
CCamSettingsModel::MapSettingItem2SettingsList( TCamSettingItemIds aSettingId )
  {
  PRINT( _L("Camera => CCamSettingsModel::MapSettingItem2SettingsList") );

  RPointerArray<TIntSetting>* array( NULL );

  if( ECamSettingItemUserSceneRangeMin < aSettingId
        && ECamSettingItemUserSceneRangeMax > aSettingId )
    {
    array = &iUserSceneSettings;
    }
  else if( ECamSettingItemDynamicPhotoRangeMin < aSettingId
        && ECamSettingItemDynamicPhotoRangeMax > aSettingId )
    {
    array = &iDynamicPhotoIntSettings;
    }
  else if( ECamSettingItemDynamicVideoRangeMin < aSettingId
        && ECamSettingItemDynamicVideoRangeMax > aSettingId )
    {
    array = &iDynamicVideoIntSettings;
    }
  else
    {
    array = iStaticModel->MapSettingItem2SettingsList( aSettingId );
    }

  PRINT( _L("Camera <= CCamSettingsModel::MapSettingItem2SettingsList") );
  return array;
  }


// ---------------------------------------------------------------------------
// CCamSettingsModel::~CCamSettingsModel
// Destructor
// ---------------------------------------------------------------------------
//
CCamSettingsModel::~CCamSettingsModel()
  {
  PRINT( _L("Camera => ~CCamSettingsModel") );

  UnLoadResources(); 

  iObservers.Reset();
  iObservers.Close();

  iSettingPreviews.ResetAndDestroy();
  iSettingPreviews.Close();

 
  iUserSceneSettings.ResetAndDestroy();
  iUserSceneSettings.Close();
  
  iUserSceneSettingsBackup.ResetAndDestroy();
  iUserSceneSettingsBackup.Close();
  
  iDynamicPhotoIntSettings.ResetAndDestroy();
  iDynamicPhotoIntSettings.Close();
  
  iDynamicPhotoIntSettingsBackup.ResetAndDestroy();
  iDynamicPhotoIntSettingsBackup.Close();
  
  iDynamicPhotoIntDefaults.Close();
  
  iDynamicVideoIntSettings.ResetAndDestroy();
  iDynamicVideoIntSettings.Close();
  
  iDynamicVideoIntSettingsBackup.ResetAndDestroy();
  iDynamicVideoIntSettingsBackup.Close();
    
  iDynamicVideoIntDefaults.Close();

  iPhotoScenes.ResetAndDestroy();
  iPhotoScenes.Close();
  
  iVideoScenes.ResetAndDestroy();
  iVideoScenes.Close();
  
  iQualityLevels.ResetAndDestroy();
  iQualityLevels.Close();
  
  iVideoResolutions.Close();
  iPhotoQualityLevels.Close();
  iPhotoResolutions.Close();
  
  if ( iStaticModel )
    {
    delete iStaticModel;
    iStaticModel = NULL;
    }

  PRINT( _L("Camera <= ~CCamSettingsModel") );
  }
  
// ---------------------------------------------------------------------------
// CCamSettingsModel::CCamSettingsModel
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamSettingsModel::CCamSettingsModel( CCamConfiguration& aConfiguration ) 
  : iDynamicPhotoIntDefaults( KDynamicArrayGranularity ), 
  	iDynamicVideoIntDefaults( KDynamicArrayGranularity ), 
  	iEngineReadyForVideoPrepare( ETrue ), 
  	iConfiguration( aConfiguration )
    {
    PRINT( _L("Camera => CCamSettingsModel::CCamSettingsModel") );  

    // set up secondary camera defaults
    // these values are not stored in the shared data file because
    // they do not persist when the camera is changed or the app is closed

    iSecondaryCameraSettings.iVideoQuality 
    = iConfiguration.SecondaryCameraVideoQuality();
    iSecondaryCameraSettings.iPhotoQuality 
    = iConfiguration.SecondaryCameraImageQuality();

    // Check that configuration can provide us secondary camera qualities
    // If either of them is unavailable, all we can do is panic
    PRINT( _L("Camera <> Checking secondary camera qualities") );  
    __ASSERT_ALWAYS( iSecondaryCameraSettings.iVideoQuality > 0, 
            CamPanic( ECamPanicSecondaryQualityMissing ) );

    __ASSERT_ALWAYS( iSecondaryCameraSettings.iPhotoQuality > 0, 
            CamPanic( ECamPanicSecondaryQualityMissing ) );

    // Check that configuration can provide us a MMS quality.
    // If not available, all we can do is panic. MMS quality is
    // needed in embedded mode.
    PRINT( _L("Camera <> Checking MMS quality") ); 
    __ASSERT_ALWAYS( iConfiguration.MmsImageQuality() > 0, 
                      CamPanic( ECamPanicMMSQualityMissing ) ); 
                      
                      
    PRINT( _L("Camera <= CCamSettingsModel::CCamSettingsModel") );                        
    }



// ---------------------------------------------------------------------------
// CCamSettingsModel::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::ConstructL()
    {
    PRINT( _L("Camera => CCamSettingsModel::ConstructL"))
    // Create reference to the CEikonEnv object, since it is used alot.
    iEnv = CEikonEnv::Static();

    LoadResourcesL(); 
                
    iStaticModel = CCamStaticSettingsModel::NewL( iConfiguration );
    
    iUiConfigManager = iStaticModel->UiConfigManagerPtr();
    
    if ( !iUiConfigManager )CamPanic( ECamPanicNullPointer );
    
    PRINT( _L("Camera => CCamSettingsModel::ConstructL create respository"))

    // Load settings.
    LoadSettingsL();

    // Load the variant flags from the ini file
    ReadVariantFlagsL();
    
    // Load flag that specifies whether the engine should be updated with
    // settings after an engine Prepare..() call.
    // Create resource reader for reading photo static settings
    TResourceReader reader;
    iEnv->CreateResourceReaderLC( reader, R_CAM_ENGINE_UPDATE );
    iUpdateAfterPrepare = reader.ReadInt16();   
    // Pop resource reader.
    CleanupStack::PopAndDestroy();  

    // read in video quality levels
    TInt i;
    
    const TInt nVidQualities( iConfiguration.NumVideoQualities() );
    for ( i=0; i < nVidQualities; i++ )
        {
        TVideoQualitySettings set = iConfiguration.VideoQuality( i );
        CCamVideoQualityLevel* lev = CCamVideoQualityLevel::NewL( set );
        CleanupStack::PushL( lev );
        User::LeaveIfError( iQualityLevels.Append( lev ) );
        CleanupStack::Pop( lev );
        }                

    // read in (supported) video resolutions
    TResourceReader videoResReader;
    iEnv->CreateResourceReaderLC( videoResReader, R_CAM_VIDEO_RESOLUTION_ARRAY );
    
    // get array of supported values
    RArray<TInt> supportedVideoRes;
    CleanupClosePushL( supportedVideoRes );
    iConfiguration.GetPsiIntArrayL( ECamPsiSupportedVideoResolutions, supportedVideoRes );

    TInt numLevels = videoResReader.ReadInt16();

    for( i = 0; i < numLevels; i++ )
        {
        TSize videoRes;
        videoRes.iWidth = videoResReader.ReadInt16();
        videoRes.iHeight = videoResReader.ReadInt16();

        User::LeaveIfError( iVideoResolutions.Append( videoRes ) );   
        }
        
    // Close array of support vid resolutions
    CleanupStack::PopAndDestroy( &supportedVideoRes );

    // Pop resource reader.
    CleanupStack::PopAndDestroy();


    const TInt numQualities( iConfiguration.NumImageQualities() );
    for( i=0; i<numQualities; i++ )
      {
      User::LeaveIfError( 
        iPhotoQualityLevels.Append( iConfiguration.ImageQuality( i ) ) );        
      }      

    // read in photo resolutions
    TResourceReader photoResReader;
    iEnv->CreateResourceReaderLC( photoResReader, R_CAM_PHOTO_RESOLUTION_ARRAY );
    numLevels = photoResReader.ReadInt16();

    // get array of supported values
    RArray<TInt> supportedStillRes;
    CleanupClosePushL( supportedStillRes );
    
    iConfiguration.GetPsiIntArrayL( ECamPsiSupportedStillResolutions, supportedStillRes ); 

    for( i = 0; i < numLevels; i++ )  
        {
        TSize photoRes;
        photoRes.iWidth = photoResReader.ReadInt16();
        photoRes.iHeight = photoResReader.ReadInt16();

        User::LeaveIfError( iPhotoResolutions.Append( photoRes ) );
            
        }        
        
    iPreviousFaceTrack = ECamSettOn;
    iPreviousSceneMode = ECamSceneAuto;
    // Close array of support photo resolutions
    CleanupStack::PopAndDestroy( &supportedStillRes );
        
    // Pop resource reader.
    CleanupStack::PopAndDestroy();
    
    PRINT( _L("Camera <= CCamSettingsModel::ConstructL"))
    }




// ---------------------------------------------------------------------------
// NotifyObservers
//
// Notify setting model observers, that a setting item value has changed.
// Overload for integer setting items.
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::NotifyObservers( TCamSettingItemIds aSettingId, 
                                    TInt               aSettingValue )
  {
  PRINT1( _L("Camera => CCamSettingsModel::NotifyObservers, observers:%d"), iObservers.Count() );
  for( TInt i = 0; i < iObservers.Count(); i++ )
    {
    TRAP_IGNORE( iObservers[i]->IntSettingChangedL( aSettingId, 
                                                    aSettingValue ) );
    } 
  PRINT ( _L("Camera <= CCamSettingsModel::NotifyObservers") );
  }

// ---------------------------------------------------------------------------
// NotifyObservers
//
// Notify setting model observers, that a setting item value has changed.
// Overload for text setting items.
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::NotifyObservers( TCamSettingItemIds aSettingId, 
                                    const TDesC&       aSettingValue )
  {
  for( TInt i = 0; i < iObservers.Count(); i++ )
    {
    TRAP_IGNORE( iObservers[i]->TextSettingChangedL( aSettingId, 
                                                     aSettingValue ) );
    } 
  }


// ---------------------------------------------------------------------------
// ReadVariantFlagsL
//
// Read Central Repository key values in KCRUidCameraappVariant repository.
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::ReadVariantFlagsL()
  {
  TInt crValue( 0 );
  CRepository* cr = CRepository::NewLC( KCRUidCameraappVariant );

  // Get variant info general flags
  User::LeaveIfError( cr->Get( KCamCrVariantFlags, crValue ) );
  iVariantInfo.iFlags = crValue;

  // Get 'time lapse enabled' flag
  User::LeaveIfError( cr->Get( KCamCrTimeLapseSupport, crValue ) );

  iVariantInfo.iTimeLapseSupport = (KCamCrTimeLapseIsSupported == crValue)
                                 ? ECamSupportOn
                                 : ECamSupportOff;

  // Get 'app always running' flag
  User::LeaveIfError( cr->Get( KCamCrAppAlwaysRunning, crValue ) );
  iVariantInfo.iAlwaysOnSupport = (KCamCrAppAlwaysRunningIsOn == crValue)
                                ? ECamSupportOn
                                : ECamSupportOff;


  // Get Public Release version
  User::LeaveIfError( cr->Get( KCamCrVersionMajor, crValue ) );
  iVariantInfo.iVersion.iMajor = (TInt8) crValue;

  User::LeaveIfError( cr->Get( KCamCrVersionMinor, crValue ) );
  iVariantInfo.iVersion.iMinor = (TInt8) crValue;

  User::LeaveIfError( cr->Get( KCamCrBuildNumber, crValue ) );
  iVariantInfo.iVersion.iBuild = (TInt16) crValue;

  CleanupStack::PopAndDestroy( cr );  
  }


// ---------------------------------------------------------------------------
// CCamSettingsModel::SettingIsForEngine
// Returns whether a particular setting is an engine setting, or not.
// ---------------------------------------------------------------------------
//
TBool CCamSettingsModel::SettingIsForEngine( TInt aSettingItem )
    {
    if ( ( aSettingItem == ECamSettingItemPhotoQuality ) ||
         ( aSettingItem == ECamSettingItemVideoAudioRec ) ||
         ( aSettingItem == ECamSettingItemVideoResolution ) ||
         ( aSettingItem == ECamSettingItemVideoFileType ) || 
         ( aSettingItem == ECamSettingItemVideoQuality ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::SearchInSettingsListFor
// Searches in a settings list for a particular setting item.
// ---------------------------------------------------------------------------
//
TInt CCamSettingsModel::SearchInSettingsListFor
( const RPointerArray<TIntSetting>& aSettingsList, TInt aSettingItem ) const
    {
    TInt count = aSettingsList.Count();
    PRINT2( _L("Camera => CCamSettingsModel::SearchInSettingsListFor item (%d) count (%d)"), aSettingItem, count)
    TInt i;
    for ( i = 0; i < count; ++i )
        {
        if ( aSettingsList[i]->iItemId == aSettingItem )
            {
            return i;
            }
        }

    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::FindSceneInSceneList
// Finds a scene in a particular scene list. 
// ---------------------------------------------------------------------------
//
TInt CCamSettingsModel::FindSceneInSceneList
( TInt aSceneItemId, const RPointerArray<TSceneData >& aSceneList ) const
    {
    TInt count = aSceneList.Count();
    TInt i;
    for ( i = 0; i < count; ++i )
        {
        if ( aSceneList[i]->iSceneId == aSceneItemId )
            {
            return i;
            }
        }

    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::UserSceneHasChangedL
// Handles a change in the value for the user scene setting.
// That is, it updates the rest of the user setings.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::UserSceneHasChangedL( TInt aSceneId, TBool aActivate )
    {

    // Set the user setup flash setting to that of the new scene.
    TInt sceneFlash = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneFlashMode );
    SetIntegerSettingValueL( ECamSettingItemUserSceneFlash, sceneFlash );

    // Set the user setup whitebalance to that of the new scene
    TInt whiteBal = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneAWBMode );
    SetIntegerSettingValueL( ECamSettingItemUserSceneWhitebalance, whiteBal );

    // Set the user setup sharpness to that of the new scene
    TInt sharpness = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneSharpness );
    SetIntegerSettingValueL( ECamSettingItemUserSceneImageSharpness, sharpness );

    // Set the user setup contrast to that of the new scene
    TInt contrast = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneContrast );
    SetIntegerSettingValueL( ECamSettingItemUserSceneContrast, contrast );
    
    // Set the scene
    TInt settingIndex = SearchInSettingsListFor( iUserSceneSettings, 
        ECamSettingItemUserSceneBasedOnScene );
    iUserSceneSettings[settingIndex]->iValueId = aSceneId;

    // Save the new values.
    SaveSettingsL();

    if ( aActivate )
        {
        // Activate user scene settings.
        ActivateUserSceneSettingsL();
        }
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::PhotoSceneHasChangedL
// Handles a change in the value for the photo scene setting.
// That is, it updates the rest of the photo dynamic setings.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::PhotoSceneHasChangedL( TInt aSceneId )
    {
    PRINT( _L("Camera => CCamSettingsModel::PhotoSceneHasChangedL") )
    // If the scene has changed to a scene other than the "User" scene
    // set capture setup values to defaults and the flash to scene flash.
    if ( aSceneId != ECamSceneUser )
        {
        // Set all the photo capture setup values to their defaults, except
        // for the scene setting.
        TInt settingsCount = iDynamicPhotoIntSettings.Count();
        TInt i;
        for ( i = 0; i < settingsCount; ++i )
            {
            // If the setting item is not the scene change it's value, 
            // and update the engine
            if ( iDynamicPhotoIntSettings[i]->iItemId != ECamSettingItemDynamicPhotoScene )
                {
                SetIntegerSettingValueL( iDynamicPhotoIntSettings[i]->iItemId, 
                    iDynamicPhotoIntDefaults[i] );
                }
            }

        // Set the capture setup flash setting to that of the new scene.
        TInt sceneFlash = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneFlashMode );
        SetIntegerSettingValueL( ECamSettingItemDynamicPhotoFlash, sceneFlash );

        // Set the user setup whitebalance to that of the new scene
        TInt whiteBal = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneAWBMode );
        SetIntegerSettingValueL( ECamSettingItemDynamicPhotoWhiteBalance, whiteBal );

        // Set the user setup sharpness to that of the new scene
        TInt sharpness = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneSharpness );
        SetIntegerSettingValueL( ECamSettingItemDynamicPhotoImageSharpness, sharpness );

        // Set the user setup contrast to that of the new scene
        TInt contrast = DefaultSettingValueForScene( aSceneId, ECamSettingItemSceneContrast );
        SetIntegerSettingValueL( ECamSettingItemDynamicPhotoBrightness, contrast );

        // Update the engine with the scene settings.
        //UpdateEngineWithSceneSettingsL( iPhotoScenes, aSceneId );
        }

    // Otherwise, activate the user scene settings
    else
        {
        ActivateUserSceneSettingsL();
        }
        
      
	  if ( iUiConfigManager->IsFaceTrackingSupported() ) // FT supported
        {
        PRINT( _L("Camera <> Face tracking supported") ) 
        if ( ECamSceneScenery == aSceneId ||
             ECamSceneSports == aSceneId ||
             ECamSceneMacro == aSceneId)
            {
            PRINT( _L("Camera <> New scene mode is scenery, sports or macro") )
            if ( ECamSceneScenery != iPreviousSceneMode && 
                 ECamSceneSports != iPreviousSceneMode &&
                 ECamSceneMacro != iPreviousSceneMode )
                {	
                PRINT( _L("Camera <> Previous scene mode is not scenery, sports or macro -> Set iPreviousFaceTrack to current value") )
                iPreviousFaceTrack = TCamSettingsOnOff( IntegerSettingValue( ECamSettingItemFaceTracking ) );	
                }
            PRINT( _L("Camera <> Switch face tracking OFF") )
            SetIntegerSettingValueL( ECamSettingItemFaceTracking, ECamSettOff );	
            }
        else if ( ( ECamSceneScenery == iPreviousSceneMode ||
                     ECamSceneSports == iPreviousSceneMode ||
                     ECamSceneMacro == iPreviousSceneMode ) &&
                   ( ECamSettOff == TCamSettingsOnOff( IntegerSettingValue( ECamSettingItemFaceTracking ) ) ) )
            {
            PRINT( _L("Camera <> Previous scene mode was scenery, sports or macro AND face tracking is OFF") )
            PRINT( _L("Camera <> Set face tracking to iPreviousFaceTrack") )
            SetIntegerSettingValueL( ECamSettingItemFaceTracking, iPreviousFaceTrack );
            PRINT( _L("Camera <> Set iPreviousFaceTrack to current face tracking state") )
            iPreviousFaceTrack = TCamSettingsOnOff( IntegerSettingValue( ECamSettingItemFaceTracking ) );
            CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEnv->AppUi() );
            TRAP_IGNORE( appUi->APHandler()->UpdateActivePaletteL() );
            }
        }              
          

    iPreviousSceneMode = TCamSceneId( aSceneId ); // store scene mode setting
    PRINT( _L("Camera <= CCamSettingsModel::PhotoSceneHasChangedL()") )
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::CopyUserSceneSettingToCaptureSettingL
// Copies a user scene setting across to the capture setup settings.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::CopyUserSceneSettingToCaptureSettingL( TInt aSettingItem )
    {
    // Determine the indexes of the user scene setting and 
    // the corresponding photo capture setting.

    // ...determine the index of the user scene setting
    TInt userIndex = SearchInSettingsListFor( iUserSceneSettings,
        aSettingItem );

    // ...determine the index of the corresponding photo capture setting
    TInt capIndex = 0;
    switch ( aSettingItem )
        {
        case ECamSettingItemUserSceneWhitebalance:
            {
            capIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoWhiteBalance );
            }
            break;

        case ECamSettingItemUserSceneExposure:
            {
            capIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoExposure );
            }
            break;

        case ECamSettingItemUserSceneColourFilter:
            {
            capIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoColourFilter );
            }
            break;
        case ECamSettingItemUserSceneFlash:
            {
            capIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoFlash );
            }
            break;

        case ECamSettingItemUserSceneBrightness:
            {
            capIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoBrightness );
            }
            break;
        case ECamSettingItemUserSceneContrast:
            {
            capIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoContrast );
            }
            break;

        case ECamSettingItemUserSceneImageSharpness:
            {
            capIndex = SearchInSettingsListFor( iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoImageSharpness );
            }
            break;

        case ECamSettingItemUserSceneLightSensitivity:
            {
            capIndex = SearchInSettingsListFor(iDynamicPhotoIntSettings,
                ECamSettingItemDynamicPhotoLightSensitivity );
            }
            break;
        default:
        	PRINT( _L("Camera => CCamSettingsModel::ECamPanicUnknownSettingItem 6" ))
            CamPanic( ECamPanicUnknownSettingItem );
            break;
        }

    // Copy the user scene setting across to the photo capture setting.
    iDynamicPhotoIntSettings[capIndex]->iValueId = 
        iUserSceneSettings[userIndex]->iValueId;

    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::ActivateUserSceneSettings
// Activates the current user scene settings.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::ActivateUserSceneSettingsL()
    {
    PRINT( _L("Camera => CCamSettingsModel::ActivateUserSceneSettingsL"))
    // Copy all the user scene settings to capture settings.
    CopyUserSceneSettingToCaptureSettingL( 
        ECamSettingItemUserSceneWhitebalance );
    CopyUserSceneSettingToCaptureSettingL( 
        ECamSettingItemUserSceneExposure );
    CopyUserSceneSettingToCaptureSettingL( 
        ECamSettingItemUserSceneColourFilter );
    CopyUserSceneSettingToCaptureSettingL( 
        ECamSettingItemUserSceneFlash );
    CopyUserSceneSettingToCaptureSettingL( 
        ECamSettingItemUserSceneBrightness );
    CopyUserSceneSettingToCaptureSettingL( 
        ECamSettingItemUserSceneContrast );
    CopyUserSceneSettingToCaptureSettingL( 
        ECamSettingItemUserSceneImageSharpness );

    if ( iUiConfigManager->IsISOSupported() )
        {
        CopyUserSceneSettingToCaptureSettingL(
            ECamSettingItemUserSceneLightSensitivity );
        }

    // Handled in appcontroller
    //UpdateEngineWithPhotoCaptureSetupSettingsL();
    PRINT( _L("Camera <= CCamSettingsModel::ActivateUserSceneSettingsL"))			
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::VideoSceneHasChangedL
// Handles a change in the value for the video scene setting.
// That is, it updates the rest of the video dynamic setings.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::VideoSceneHasChangedL( TInt aSettingValue )
    {
    // Set all the video capture setup values to their defaults,
    // except for the scene setting.
    TInt settingsCount = iDynamicVideoIntSettings.Count();
    TInt i;
    for ( i = 0; i < settingsCount; ++i )
      {
      //video light is used for LED flash not for xenon flash
      if ( iDynamicVideoIntSettings[i]->iItemId == ECamSettingItemDynamicVideoFlash )
        {
        if ( iUiConfigManager->IsVideoLightSupported() && !iUiConfigManager->IsXenonFlashSupported() )
          {
          // Set the capture setup flash setting to that of the new scene.
          TInt sceneFlash = DefaultSettingValueForVideoScene( aSettingValue, ECamSettingItemSceneFlashMode );      
          SetIntegerSettingValueL( ECamSettingItemDynamicVideoFlash, sceneFlash );          
          }
        }
      // If the setting item is not the scene change it's value.
      else if ( iDynamicVideoIntSettings[i]->iItemId != ECamSettingItemDynamicVideoScene ) 
        {
        SetIntegerSettingValueL( iDynamicVideoIntSettings[i]->iItemId, 
              iDynamicVideoIntDefaults[i] );
        }
      }

    // Update the engine with the scene settings.
    //UpdateEngineWithSceneSettingsL( iVideoScenes, aSettingValue );
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::DefaultSettingValueForVideoScene
// Returns the default value of a setting for a video scene item.
// ---------------------------------------------------------------------------
//
TInt CCamSettingsModel::DefaultSettingValueForVideoScene( TInt aSceneId, TInt aSettingId ) const
    {
    TInt sceneIndex = FindSceneInSceneList( aSceneId, iVideoScenes );
   
    TInt settingIndex = SearchInSettingsListFor( 
        iVideoScenes[sceneIndex]->iSettings, aSettingId );
    
    return iVideoScenes[sceneIndex]->iSettings[settingIndex]->iValueId;
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::PhotoSceneMaximumQuality
// Returns the highest allowed quality for the current photo scene.
// ---------------------------------------------------------------------------
//
TInt CCamSettingsModel::PhotoSceneMaximumQuality( TInt aSceneId ) const
    {
     // If the scene is the user scene return the maximum amount.
     // This is to be changed once user scene has been implemented.
     if ( aSceneId == ECamSceneUser )
         {
         // Get the scene that the user scene is based on and
         // get the maximum resolution for this scene.
         TInt settingIndex = SearchInSettingsListFor( 
            iUserSceneSettings, ECamSettingItemUserSceneBasedOnScene );
         TInt sceneId = iUserSceneSettings[settingIndex]->iValueId;

         return PhotoSceneMaximumQuality( sceneId );
         }

    // Find the index to the scene.
    TInt sceneIndex = FindSceneInSceneList( aSceneId, iPhotoScenes );
    // Search for the maximum resolution item in this scene's item list.
    TInt settingsCount = iPhotoScenes[sceneIndex]->iSettings.Count();
    TInt i;
    for ( i = 0; i < settingsCount; ++i )
        {
        if ( iPhotoScenes[sceneIndex]->iSettings[i]->iItemId == ECamSettingItemSceneMaxRes )
            {
            return iPhotoScenes[sceneIndex]->iSettings[i]->iValueId;
            }
        }
	PRINT( _L("Camera => CCamSettingsModel::ECamPanicUnknownSettingItem 7" ))
    CamPanic( ECamPanicUnknownSettingItem );
    // Remove warning
    return KErrNotFound;  
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::CurrentPhotoQualityValue
// Returns the current photo quality setting.
// ---------------------------------------------------------------------------
//
TInt CCamSettingsModel::CurrentPhotoQualityValue() const
    {
    return iStaticModel->IntegerSettingValue( ECamSettingItemPhotoQuality );
    }
    
// ---------------------------------------------------------------------------
// CCamSettingsModel::PhotoQualityArrayIndex
// Returns the photo quailty array index of a specified photo quality id
// ---------------------------------------------------------------------------
//

TInt CCamSettingsModel::PhotoQualityArrayIndex(TInt aPhotoQualityId) const
    {
    for(TInt i=0;i<iPhotoQualityLevels.Count();i++)
        {
        if(iPhotoQualityLevels[i].iPhotoQualityId == aPhotoQualityId)
            {
            return i;
            }
        }
    return KErrNotFound;        
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::CurrentPhotoCompression
// Returns the current photo quality level (value 1-100).
// ---------------------------------------------------------------------------
//
TInt CCamSettingsModel::CurrentPhotoCompression() const
    {
    TInt currentQualityLevel = PhotoQualityArrayIndex(CurrentPhotoQualityValue());

    if ( currentQualityLevel != KErrNotFound )
        {
        // Get the compression level for the current quality setting.
        TCamPhotoQualitySetting quality = 
            iPhotoQualityLevels[currentQualityLevel];
        TInt compressionLevel = quality.iQualityLevel;
        return compressionLevel;
        }

	PRINT( _L("Camera => CCamSettingsModel::ECamPanicUnknownSettingItem 10" ))
    CamPanic( ECamPanicUnknownSettingItem );
    // Remove warning
    return KErrNotFound;
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::CurrentPhotoResolution
// Returns the photo resolution from current Settings
// ---------------------------------------------------------------------------
//
TCamPhotoSizeId CCamSettingsModel::CurrentPhotoResolution() const
    {
    return PhotoResolution( CurrentPhotoQualityValue() );
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::PhotoResolution
// Returns the photo resolution for a particular quality index
// ---------------------------------------------------------------------------
//
TCamPhotoSizeId CCamSettingsModel::PhotoResolution( TInt aQualityIndex ) const
    {
    TInt qualityValue = PhotoQualityArrayIndex( aQualityIndex );

    if ( qualityValue != KErrNotFound )
        {
        TCamPhotoQualitySetting quality = 
                iPhotoQualityLevels[qualityValue];
                
        return iConfiguration.MapResolutionToPhotoSize( quality.iPhotoResolution );
        }

	PRINT( _L("Camera => CCamSettingsModel::ECamPanicUnknownSettingItem 11" ))
    CamPanic( ECamPanicUnknownSettingItem );
    // Remove warning
    return ECamPhotoSizeVGA; // We can never get here anyway
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::StorePrimaryCameraSettingsL
// Stores the primary camera settings so they can be reapplied when
// changing from front to back camera
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::StorePrimaryCameraSettingsL()
    {
    PRINT( _L("Camera => CCamSettingsModel::StorePrimaryCameraSettingsL"))
    iStaticModel->StorePrimaryCameraSettingsL();
    CopySettingsL(iDynamicPhotoIntSettings, iDynamicPhotoIntSettingsBackup);
    CopySettingsL(iDynamicVideoIntSettings, iDynamicVideoIntSettingsBackup);
    PRINT( _L("Camera <= CCamSettingsModel::StorePrimaryCameraSettingsL"))
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::RestorePrimaryCameraSettingsL
// Restores the primary camera settings when
// changing from front to back camera
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::RestorePrimaryCameraSettingsL()
    {
    // set the stored primary camera settings
    iStaticModel->RestorePrimaryCameraSettingsL();
    CopySettingsL(iDynamicPhotoIntSettingsBackup, iDynamicPhotoIntSettings, ETrue);
    CopySettingsL(iDynamicVideoIntSettingsBackup, iDynamicVideoIntSettings, ETrue);
    CopySettingsL(iUserSceneSettingsBackup, iUserSceneSettings, ETrue);
    }

// ---------------------------------------------------------------------------
// VariantInfo
// ---------------------------------------------------------------------------
//
const TCamVariantInfo& 
CCamSettingsModel::VariantInfo()
  {
  if( 0 > iVariantInfo.iFlags )
    {
    TRAP_IGNORE( ReadVariantFlagsL() );
    }
  return iVariantInfo;
  }


// ---------------------------------------------------------------------------
// CCamSettingsModel::ResetRepository
// Reset Camera central repository file
// ---------------------------------------------------------------------------
//   
 void CCamSettingsModel::ResetRepository()
 	{
 	iStaticModel->ResetRepository();
 	}




// ===========================================================================
// From base class MCamSettingPreviewHandler

// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::ActivatePreviewL( const TCamSettingItemIds& aSettingId,
                                           TInt                aSettingValue )
  {
  TInt index = FindIndex( iSettingPreviews, aSettingId );
  if( KErrNotFound == index )
    {
    // Preview for this setting id did not exist yet.
    TIntSetting* settingItem = new (ELeave) TIntSetting;
    CleanupStack::PushL( settingItem );

    settingItem->iItemId  = aSettingId;
    settingItem->iValueId = aSettingValue;

    iSettingPreviews.AppendL( settingItem );
    CleanupStack::Pop( settingItem );
    }
  else
    {
    // If a preview exists, only update the value.
    // Only one preview per setting item active at once.
    iSettingPreviews[index]->iValueId = aSettingValue;
    }
  // The preview is considered as current setting value.
  // Notify observers as if the setting value had changed.
  NotifyObservers( aSettingId, aSettingValue );
  }


// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::CancelPreview( const TCamSettingItemIds& aSettingId )
  {
  TInt index = FindIndex( iSettingPreviews, aSettingId );

  if( KErrNotFound != index )
    {
    TIntSetting* settingItem = iSettingPreviews[index];
    iSettingPreviews.Remove( index );
    delete settingItem;
    settingItem = NULL;
    }
  }
    

// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::CancelPreviews( const RArray<TCamSettingItemIds> aSettingIds )
  {
  // Check if there is anything to cancel
  if( 0 < iSettingPreviews.Count() )
    {
    for( TInt i = 0; i < aSettingIds.Count(); i++ )
      {
      CancelPreview( aSettingIds[i] );
      }
    }
  }
    

// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::CancelAllPreviews()
  {
  iSettingPreviews.ResetAndDestroy();
  }


// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::CommitPreviewL( const TCamSettingItemIds& aSettingId )
  {
  TInt index = FindIndex( iSettingPreviews, aSettingId );

  if( KErrNotFound != index )
    {
    TIntSetting* settingItem = iSettingPreviews[index];

    iSettingPreviews.Remove( index );

    CleanupStack::PushL( settingItem );
    SetIntegerSettingValueL( settingItem->iItemId, settingItem->iValueId );
    CleanupStack::PopAndDestroy( settingItem );
    }
  else
    {
    User::Leave( KErrNotFound );
    }
  }


// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::CommitPreviewsL( const RArray<TCamSettingItemIds>& aSettingIds )
  {
  if( 0 < iSettingPreviews.Count() )
    {
    TInt all   ( KErrNone );
    TInt single( KErrNone );

    for( TInt i = 0; i < aSettingIds.Count(); i++ )
      {
      TRAP( single, CommitPreviewL( aSettingIds[i] ) );

      if( KErrNone != single )
        all = single;
      }

    // Leave if there was problem with any of the previews.  
    User::LeaveIfError( all );
    }
  }


// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::CommitAllPreviewsL()
  {
  TInt all   ( KErrNone );
  TInt single( KErrNone );
  for( TInt i = 0; i < iSettingPreviews.Count(); i++ )
    {    
    TRAP( single, SetIntegerSettingValueL( iSettingPreviews[i]->iItemId, 
                                           iSettingPreviews[i]->iValueId ) );

    if( KErrNone != single )
      all = single;
    }

  // Even if all commits are not successful, deactivate all previews.
  iSettingPreviews.ResetAndDestroy();

  // Leave if there was problem with any of the previews.  
  User::LeaveIfError( all );
  }


// ---------------------------------------------------------------------------
//   
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel
::ActivePreviewsL( RArray<TCamSettingItemIds>& aSettingIds ) const
  {
  aSettingIds.Reset();
  aSettingIds.ReserveL( iSettingPreviews.Count() );
  for( TInt i = 0; i < iSettingPreviews.Count(); i++ )
    {
    aSettingIds.AppendL( (TCamSettingItemIds)iSettingPreviews[i]->iItemId );
    }
  }

// ===========================================================================



// ---------------------------------------------------------------------------
// CCamSettingsModel::ImageResolutionFromIndex
// Returns the dimensions of the photo (in pixels) based on the size ID passed in
// ---------------------------------------------------------------------------
//
TSize CCamSettingsModel::ImageResolutionFromIndex( TCamPhotoSizeId aIndex )
    {    
    // TCamImageResolution imageRes = iConfiguration.MapPhotoSizeToResolution( aIndex );
    return iPhotoResolutions[aIndex];
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::DefaultSettingValueForScene
// Returns the default value of a setting for a particular scene item.
// ---------------------------------------------------------------------------
//
TInt CCamSettingsModel::DefaultSettingValueForScene( TInt aSceneId, TInt aSettingId ) const
    {
    TInt sceneIndex = FindSceneInSceneList( aSceneId, iPhotoScenes );

    TInt settingIndex = SearchInSettingsListFor( 
        iPhotoScenes[sceneIndex]->iSettings, aSettingId );

    return iPhotoScenes[sceneIndex]->iSettings[settingIndex]->iValueId;
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::LoadDynamicSettingsL
// Loads the dynamic settings from resource file for a 
// particular group of settings, and stores their defaults. 
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::LoadDynamicSettingsL( 
    TInt aResourceId, 
    RPointerArray<TIntSetting>& aSettingsList, 
    RArray<TInt>&               aDefaultsList )
  {
  PRINT( _L("Camera => CCamSettingsModel::LoadDynamicSettingsL" ))
  // Create resource reader for reading photo static settings
  TResourceReader reader;
  iEnv->CreateResourceReaderLC( reader, aResourceId );
  TInt count = reader.ReadInt16();

  // for each entry in the resource, create a new setting item.
  TInt i;
  for ( i = 0; i < count; ++i )
        {

        TIntSetting* newSetting = new (ELeave) TIntSetting;
        CleanupStack::PushL( newSetting );

        // Read the data for this setting item from resource.
        newSetting->iItemId = reader.ReadInt16();
        newSetting->iValueId = reader.ReadInt16();
        aDefaultsList.AppendL( newSetting->iValueId );

        // Add the new setting item and its associated key
        // to the parallel arrays for static photo settings.
        aSettingsList.AppendL( newSetting );
        CleanupStack::Pop( newSetting );
        }    

    CleanupStack::PopAndDestroy(); // reader
    PRINT( _L("Camera <= CCamSettingsModel::LoadDynamicSettingsL" ))
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::LoadDynamicSettingsL
// Loads the dynamic settings from resource file for a 
// particular group of settings. 
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::LoadDynamicSettingsL( TInt aResourceId, 
                        RPointerArray<TIntSetting>& aSettingsList )
    {
    iStaticModel->LoadSettingsFromResourceL( aResourceId, aSettingsList );
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::LoadScenesDataL
// Loads the scene data from resource file for a particular group of scenes. 
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::LoadScenesDataL( TInt aResourceId, 
    RPointerArray<TSceneData >& aSceneDataArray )
    {
    // Add array of defaults for static video settings
    TResourceReader reader;
    // Add array of defaults for static video settings
    iEnv->CreateResourceReaderLC( reader, aResourceId );
    TInt sceneCount = reader.ReadInt16();

    // for each entry in the resource array, create a new scene data item
    TInt i;
    for ( i = 0; i < sceneCount; i++ )
        {
        TSceneData * sceneData = new (ELeave) TSceneData ;
        CleanupStack::PushL( sceneData );

        // Create a resource reader for reading only this scene's data.
        TInt resourceID = reader.ReadInt32();
        TResourceReader localReader;
        iEnv->CreateResourceReaderLC
            ( localReader, resourceID );

        // Read the data for this list item from resource.
        // ...read scene id for the scene data
        sceneData->iSceneId = localReader.ReadInt16();
        // ...read settings for the scene data.
        TInt settingsCount = localReader.ReadInt16();
        TInt j;
        for ( j = 0; j < settingsCount; ++j )
            {
            TIntSetting* setting = new (ELeave) TIntSetting;
            CleanupStack::PushL( setting );
            sceneData->iSettings.AppendL( setting );
            CleanupStack::Pop( setting );
            sceneData->iSettings[j]->iItemId = localReader.ReadInt16();
            sceneData->iSettings[j]->iValueId = localReader.ReadInt16();
            }

        CleanupStack::PopAndDestroy(); // local reader

        // Add the scene data to the scenes list.
        aSceneDataArray.AppendL( sceneData );
        CleanupStack::Pop( sceneData );
        }  
    
    CleanupStack::PopAndDestroy(); // reader
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::LoadSceneDataL
// Loads the scene data from resource file for a particular group of scenes. 
// ---------------------------------------------------------------------------
//    
void CCamSettingsModel::LoadSceneDataL( TBool aPhotoSceneSettings )
    {
    RArray<TSceneSettings> supportedSceneSettings;
    CleanupClosePushL( supportedSceneSettings );
    
    iUiConfigManager->SupportedSceneModesAndSettingsL( supportedSceneSettings,
                                                      aPhotoSceneSettings );
   
    for ( TInt i = 0; i < supportedSceneSettings.Count(); i++ )
        {
        //
        TSceneSettings scene = supportedSceneSettings[i];
        if ( scene.iSupported ) // only if scenes are supported its appened to the list
            {
            TSceneData* sceneData = new ( ELeave ) TSceneData;
            CleanupStack::PushL( sceneData );

            // identifier
            sceneData->iSceneId = scene.iIdentifier;

            // exposure mode
            TIntSetting* setting = new ( ELeave ) TIntSetting;
            CleanupStack::PushL( setting );

            setting->iItemId = ECamSettingItemSceneAEMode;
            setting->iValueId = scene.iExposureMode;

            sceneData->iSettings.AppendL( setting );
            CleanupStack::Pop( setting );

            // white balance
            setting = new ( ELeave ) TIntSetting;
            CleanupStack::PushL( setting );

            setting->iItemId = ECamSettingItemSceneAWBMode;
            setting->iValueId = scene.iWhiteBalance;

            sceneData->iSettings.AppendL( setting );
            CleanupStack::Pop( setting );

            // flash
            setting = new ( ELeave ) TIntSetting;
            CleanupStack::PushL( setting );

            setting->iItemId = ECamSettingItemSceneFlashMode;
            setting->iValueId = scene.iFlash;

            sceneData->iSettings.AppendL( setting );
            CleanupStack::Pop( setting );

            // contrast
            setting = new ( ELeave ) TIntSetting;
            CleanupStack::PushL( setting );

            setting->iItemId = ECamSettingItemSceneContrast;
            setting->iValueId = scene.iContrastMode;

            sceneData->iSettings.AppendL( setting );
            CleanupStack::Pop( setting );

            // focal distance
            setting = new ( ELeave ) TIntSetting;
            CleanupStack::PushL( setting );

            setting->iItemId = ECamSettingItemSceneAFMode;
            setting->iValueId = scene.iFocalDistance;

            sceneData->iSettings.AppendL( setting );
            CleanupStack::Pop( setting );

            
            if ( aPhotoSceneSettings )
                { 
                // load settings which are specific to photo scene
                
                // sharpness
                setting = new ( ELeave ) TIntSetting;
                CleanupStack::PushL( setting );

                setting->iItemId = ECamSettingItemSceneSharpness;
                setting->iValueId = scene.iSharpnessMode;

                sceneData->iSettings.AppendL( setting );
                CleanupStack::Pop( setting );
                
                
                // ISO
                setting = new ( ELeave ) TIntSetting;
                CleanupStack::PushL( setting );

                setting->iItemId = ECamSettingItemSceneISO;
                setting->iValueId = scene.iLightSensitivityMode;

                sceneData->iSettings.AppendL( setting );
                CleanupStack::Pop( setting );

                // EV
                setting = new ( ELeave ) TIntSetting;
                CleanupStack::PushL( setting );

                setting->iItemId = ECamSettingItemSceneEV;
                setting->iValueId = scene.iExposureCompensationValue;

                sceneData->iSettings.AppendL( setting );
                CleanupStack::Pop( setting );
                }
            else
                {
                // settings which are specific to video scene
                // framerate
                setting = new ( ELeave ) TIntSetting;
                CleanupStack::PushL( setting );
                
                setting->iItemId = ECamSettingItemSceneFramerate;
                setting->iValueId = scene.iFrameRate;
                
                sceneData->iSettings.AppendL( setting );
                CleanupStack::Pop( setting );
                }
            // Add the scene data to the scenes list.
            if ( aPhotoSceneSettings )
                {
                iPhotoScenes.AppendL( sceneData );                
                }
            else
                {
                iVideoScenes.AppendL( sceneData );                
                }
            CleanupStack::Pop( sceneData );
            } // end if 
        } // end for
    CleanupStack::PopAndDestroy( &supportedSceneSettings );
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::UnloadSettings
// Remove any previously loaded settings.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::UnloadSettings()
    {
    PRINT( _L("Camera => CCamSettingsModel::UnloadSettings()" ))

    iDynamicPhotoIntSettings.ResetAndDestroy();
    iDynamicVideoIntSettings.ResetAndDestroy();

    iPhotoScenes.ResetAndDestroy();
    iVideoScenes.ResetAndDestroy();
    PRINT( _L("Camera <= CCamSettingsModel::UnloadSettings()" ))
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::UnloadStaticSettings
// Remove any previously loaded static settings.
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::UnloadStaticSettings()
    {
    PRINT( _L("Camera => CCamSettingsModel::UnloadStaticSettings()" ))
    iUserSceneSettings.ResetAndDestroy();
    PRINT( _L("Camera <= CCamSettingsModel::UnloadStaticSettings()" ))
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::BurstModeActiveL
// Track burst mode activation
// ---------------------------------------------------------------------------
//
void 
CCamSettingsModel::BurstModeActiveL( TBool aActive, TBool /*aStillModeActive*/ )
  {
  iBurstEnabled = aActive;
  }


// ---------------------------------------------------------------------------
// CCamSettingsModel::VideoResolutions
// Return ref. to video resolutions array
// ---------------------------------------------------------------------------
//
const RArray<TSize>& 
CCamSettingsModel::VideoResolutions()
  {
  return iVideoResolutions;
  }

// ---------------------------------------------------------------------------
// CCamSettingsModel::VideoQualityArray
// Return ref. to video video quality level array
// ---------------------------------------------------------------------------
//
const RPointerArray<CCamVideoQualityLevel>& 
CCamSettingsModel::VideoQualityArray()
  {
  return iQualityLevels;
  }
  
// ---------------------------------------------------------------------------
// StaticSettingsModel
// 
// Gets the reference to the settings model
// ---------------------------------------------------------------------------
//
MCamStaticSettings& 
CCamSettingsModel::StaticSettingsModel() const
  {
  return *iStaticModel;
  }

// ===========================================================================
// TSceneData methods

// ---------------------------------------------------------------------------
// TSceneData::~TSceneData
// Destructor
// ---------------------------------------------------------------------------
//
TSceneData::~TSceneData()
    {
    iSettings.ResetAndDestroy();
    iSettings.Close();
    }

/*
* Is Image Scene supported
*/
 TBool CCamSettingsModel::IsImageSceneSupported( const TInt aSceneId ) const
 {
 if ( ECamSceneMacro == aSceneId && !iUiConfigManager->IsAutoFocusSupported() )
     {
     // if the photo scene is macro and if autofocus is not supported
     // the macro scene mode is not visible in scene mode settings
     return EFalse;
     }
 return ( KErrNotFound != FindSceneInSceneList( aSceneId, iPhotoScenes ) );
 }

 /*
 * Is Image Scene supported
 */
  TBool CCamSettingsModel::IsVideoSceneSupported( const TInt aSceneId ) const
  {
  return ( KErrNotFound != FindSceneInSceneList( aSceneId, iVideoScenes ) );
  } 
 
/*
*  Handle to Camera Ui Config Manager
*/
CCameraUiConfigManager* CCamSettingsModel::UiConfigManagerPtr()
    {
    return iUiConfigManager;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::LoadResourcesL()   
    {
    PRINT( _L("Camera => CCamSettingsModel::LoadResourcesL" ) )
    if (iResourceFileOffset == 0)
        {
        TFileName resourceFile;
        TParse parse;
        if ( CamUtility::IsNhdDevice() )  
            {
            parse.Set( KCamNhdResourceFileName, &KDC_APP_RESOURCE_DIR, NULL ); 
            }
        else
            {
            parse.Set( KCamVgaResourceFileName, &KDC_APP_RESOURCE_DIR, NULL ); 
            }
        resourceFile = parse.FullName();
        BaflUtils::NearestLanguageFile( iEnv->FsSession(), resourceFile );
        iResourceFileOffset = iEnv->AddResourceFileL( resourceFile );
        }
    PRINT( _L("Camera <= CCamSettingsModel::LoadResourcesL" ) )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCamSettingsModel::UnLoadResources()
    {
    if (iResourceFileOffset > 0)
        {
        iEnv->DeleteResourceFile( iResourceFileOffset );
        iResourceFileOffset = 0;
        }
    } 
    
// ---------------------------------------------------------------------------
// CCamSettingsModel::StoreFaceTrackingValue
//
// Store user defined FT value outside Sports and Scenery mode that setting
// can be rolled back. 
// ---------------------------------------------------------------------------
//   
void CCamSettingsModel::StoreFaceTrackingValue()
   {
   PRINT( _L("Camera => CCamSettingsModel::StoreFaceTrackingValue()" ) )	
   
   if ( !iUiConfigManager->IsFaceTrackingSupported() ) // FT not supported
      {
      PRINT( _L("Camera <= CCamSettingsModel::StoreFaceTrackingValue(), FT _not_ supported" ) )	
      return;	
      }
   TInt activeScene = IntegerSettingValue( ECamSettingItemDynamicPhotoScene );	
   if ( ECamSceneScenery == activeScene ||
        ECamSceneSports == activeScene ||
        ECamSceneMacro == activeScene) 	
      {
      PRINT( _L("Camera <> CCamSettingsModel::StoreFaceTrackingValue(), Scenery or Sports mode" ) )		      	      	
      SetIntegerSettingValueL( ECamSettingItemFaceTracking, iPreviousFaceTrack );   
      }
      
      
      
   PRINT( _L("Camera <= CamSettingsModel::StoreFaceTrackingValue()" ) )	
   }

// ---------------------------------------------------------------------------
// CCamSettingsModel::CopySettingsL
//
// Backup and Restore Settings array
// ---------------------------------------------------------------------------
// 
void CCamSettingsModel::CopySettingsL(RPointerArray<TIntSetting>& aSrc,
                 RPointerArray<TIntSetting>& aDest,
                 TBool aRestore)
    {
    if(aRestore)
        {
        __ASSERT_DEBUG(aSrc.Count() == aDest.Count(), CamPanic(ECamPanicInvalidState));
        TInt count = aSrc.Count();
        TInt index = 0;
        do
            {
            if(aSrc[index]->iItemId == aDest[index]->iItemId)
                {
                if(aSrc[index]->iValueId != aDest[index]->iValueId)
                    {
                    aDest[index]->iValueId = aSrc[index]->iValueId;
                    }
                }
            index++;
            }while(index < count);
        }
    else
        {
        __ASSERT_DEBUG(aSrc.Count(), CamPanic(ECamPanicInvalidState));
        aDest.ResetAndDestroy();
        TInt count = aSrc.Count();
        TInt index = 0;
        do
            {
            TIntSetting* setting = new(ELeave) TIntSetting();
            CleanupStack::PushL(setting);
            setting->iItemId = aSrc[index]->iItemId;
            setting->iValueId = aSrc[index]->iValueId;
            aDest.AppendL(setting);
            CleanupStack::Pop(setting);
            index++;
            }while(index < count);
        }
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::StoreUserSceneSettingsL
//
// Store UserScene Settings
// ---------------------------------------------------------------------------
// 
void CCamSettingsModel::StoreUserSceneSettingsL()
    {
    CopySettingsL(iUserSceneSettings, iUserSceneSettingsBackup);   
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::SetUserSceneDefault
//
// Set userScene as default 
// ---------------------------------------------------------------------------
// 
void CCamSettingsModel::SetUserSceneDefault()
    {
    PRINT( _L("Camera => CCamSettingsModel::SetUserSceneDefault ") );
    TInt settingsCount = iDynamicPhotoIntSettings.Count();
    TInt userVal = 0;
    for ( TInt i = 0; i < settingsCount; ++i )
       {
       if ( iDynamicPhotoIntSettings[i]->iItemId == ECamSettingItemDynamicPhotoScene )
         {
         TInt sceneSetCount = iUserSceneSettings.Count();
         for ( TInt j = 0; j < sceneSetCount; ++j )
             {
             if ( iUserSceneSettings[j]->iItemId == ECamSettingItemUserSceneDefault )
                 {
                 userVal = iUserSceneSettings[j]->iValueId;
                 }
             }
         if ( userVal )
             {
             PRINT1( _L("Camera <> CCamSettingsModel::SetUserSceneDefault set userVal= %d"), userVal);
             iDynamicPhotoIntSettings[i]->iValueId = ECamSceneUser;
             ActivateUserSceneSettingsL();
             }
         
         }
       }
    
    PRINT( _L("Camera <= CCamSettingsModel::SetUserSceneDefault ") );
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::GetPreviousFaceTrack
//
// Returns the face tracking state as it was before the latest scene mode change
// ---------------------------------------------------------------------------
//
TCamSettingsOnOff CCamSettingsModel::GetPreviousFaceTrack()
    {
    return iPreviousFaceTrack;
    }
    
// ---------------------------------------------------------------------------
// CCamSettingsModel::SetPreviousFaceTrack
// ---------------------------------------------------------------------------    
//
void CCamSettingsModel::SetPreviousFaceTrack( TCamSettingsOnOff aPreviousFaceTrack )
    {
    iPreviousFaceTrack = aPreviousFaceTrack;
    }  
    
// ---------------------------------------------------------------------------
// CCamSettingsModel::GetPreviousSceneMode
//
// Returns the scene mode that was in use before the current scene mode was selected
// ---------------------------------------------------------------------------    
//
TCamSceneId CCamSettingsModel::GetPreviousSceneMode()
    {
    return iPreviousSceneMode;
    }
    
// ---------------------------------------------------------------------------
// CCamSettingsModel::SetPreviousSceneMode
// ---------------------------------------------------------------------------    
//
void CCamSettingsModel::SetPreviousSceneMode( TCamSceneId aPreviousSceneMode )
    {
    iPreviousSceneMode = aPreviousSceneMode;
    }      
    
    
// ===========================================================================
// end of File  
