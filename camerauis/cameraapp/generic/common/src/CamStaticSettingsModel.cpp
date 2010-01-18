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
* Description:  Manages all static settings data.*
*/



// ===========================================================================
// Includes

#include <StringLoader.h>
#include <barsread.h>
#include <AknQueryDialog.h>
#include <ecam.h>
#ifndef CAMERAAPP_PLUGIN_BUILD
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include "camsettingconversion.h"
#else
#include <gscamerapluginrsc.rsg>
#endif

#include "CamStaticSettingsModel.h"
#include "CamUtility.h"
#include "CamPanic.h"
#include "CamAppUiBase.h"
#include "CamVideoQualityLevel.h"
#include "CameraappPrivateCRKeys.h" // CR keys
#include "CameraUiConfigManager.h"
#include "camconfiguration.h"


// ===========================================================================
// Constants

const TInt KCamCRStringInitialLength = 64;
const TInt KCamUseDefaultVideoQuality = -1;

// ===========================================================================
// Local methods

inline TBool SettingIdMatches( const TInt*        aSettingId, 
                               const TIntSetting& aSettingItem )
  {
  return (*aSettingId == aSettingItem.iItemId);
  };


// ===========================================================================
// Class methods

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::NewL
// Symbian OS two-phased constructor 
// ---------------------------------------------------------------------------
//
CCamStaticSettingsModel* 
CCamStaticSettingsModel::NewL( CCamConfiguration& aConfiguration )
    {
    CCamStaticSettingsModel* self 
      = CCamStaticSettingsModel::NewLC( aConfiguration );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStaticSettingsModel*
CCamStaticSettingsModel::NewLC( CCamConfiguration& aConfiguration )
    {
    CCamStaticSettingsModel* self 
      = new( ELeave ) CCamStaticSettingsModel( aConfiguration );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::IntegerSettingValue
// Returns the current integer value for the specified setting
// ---------------------------------------------------------------------------
//
TInt 
CCamStaticSettingsModel::IntegerSettingValue( TInt aSettingItem ) const
    {
    TInt value = KErrNotFound;
    // If setting item is in the static photo settings return it's value.
    if( ECamSettingItemStaticPhotoRangeMax > aSettingItem
        && ECamSettingItemStaticPhotoRangeMin < aSettingItem )
      {
      TInt settingIndex = SearchInSettingsListFor(
                                                  iStaticPhotoIntSettings,
                                                  aSettingItem );
      value = iStaticPhotoIntSettings[settingIndex]->iValueId;
      }
    // Otherwise, if setting item is in the static video settings return it's value.
    else if( ECamSettingItemStaticVideoRangeMax > aSettingItem 
          && ECamSettingItemStaticVideoRangeMin < aSettingItem )
      {
      TInt settingIndex = SearchInSettingsListFor( iStaticVideoIntSettings, aSettingItem );
      value = iStaticVideoIntSettings[settingIndex]->iValueId;
      }
    // Look in static common settings.
    else if( ECamSettingItemStaticCommonRangeMax > aSettingItem
          && ECamSettingItemStaticCommonRangeMin < aSettingItem )
      {
      TInt settingIndex = SearchInSettingsListFor( iStaticCommonIntSettings, aSettingItem );
      value = iStaticCommonIntSettings[settingIndex]->iValueId;
      }
    else
      {
      PRINT( _L("Camera <> Not found, PANIC !! ECamPanicUnknownSettingItem" ))
      CamPanic( ECamPanicUnknownSettingItem );
      }
    return value;
    }


// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SetIntegerSettingValueL
// Sets a new integer value for the specified setting
// ---------------------------------------------------------------------------
//
void 
CCamStaticSettingsModel::SetIntegerSettingValueL( TInt aSettingItem,
                                                  TInt aSettingValue )
  {
  TCamSettingItemIds settingId(
      static_cast<TCamSettingItemIds>(aSettingItem) );

  // Static image settings
  if( ECamSettingItemStaticPhotoRangeMin < settingId
        && ECamSettingItemStaticPhotoRangeMax > settingId )
    {
    PRINT( _L("Camera <> static photo setting") );
    TInt settingIndex = SearchInSettingsListFor( iStaticPhotoIntSettings, settingId );
    iStaticPhotoIntSettings[settingIndex]->iValueId = aSettingValue;
    SaveStaticSettingL( settingId );

    // Video / photo storage settings follow eachother
    if ( ECamSettingItemPhotoMediaStorage == settingId )
      {
      TInt index = SearchInSettingsListFor( iStaticVideoIntSettings, 
                                            ECamSettingItemVideoMediaStorage );
      if ( index != KErrNotFound )
        {
        iStaticVideoIntSettings[index]->iValueId = aSettingValue;
        SaveStaticSettingL( ECamSettingItemVideoMediaStorage );
        }
      }
    }
  // -------------------------------------------------------
  // Static video settings
  else if( ECamSettingItemStaticVideoRangeMin < settingId
        && ECamSettingItemStaticVideoRangeMax > settingId )
    {
    TInt settingIndex = SearchInSettingsListFor( iStaticVideoIntSettings, settingId );
    iStaticVideoIntSettings[settingIndex]->iValueId = aSettingValue;
    SaveStaticSettingL( settingId );

    // Video / photo storage settings follow eachother
    if ( ECamSettingItemVideoMediaStorage == settingId )
      {
      TInt index = SearchInSettingsListFor( iStaticPhotoIntSettings, 
                                            ECamSettingItemPhotoMediaStorage );
      if ( index != KErrNotFound )
        {
        iStaticPhotoIntSettings[index]->iValueId = aSettingValue;
        SaveStaticSettingL( ECamSettingItemPhotoMediaStorage );
        }
      }
    }
  // -------------------------------------------------------
  // Static common settings
  else if( ECamSettingItemStaticCommonRangeMin < settingId
        && ECamSettingItemStaticCommonRangeMax > settingId )
    {
    TInt settingIndex = SearchInSettingsListFor( iStaticCommonIntSettings, settingId );
    iStaticCommonIntSettings[settingIndex]->iValueId = aSettingValue;
    SaveStaticSettingL( settingId );
    }
  // -------------------------------------------------------
  else
    {
    // Ignored at the moment
    PRINT( _L("Camera <> Setting item not found !!!") );
    }
  // -------------------------------------------------------
  }


// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SetTextSettingValueL
// Sets a new text value for the specified setting
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::SetTextSettingValueL( TInt aSettingItem,
                                                const TDesC& aSettingValue )
    {
switch ( aSettingItem )
        {
        case ECamSettingItemPhotoNameBase:
            {
            iPhotoBaseName = aSettingValue;
            break;
            }
        case ECamSettingItemVideoNameBase:
            {
            iVideoBaseName = aSettingValue;
            break;
            }
        case ECamSettingItemDefaultAlbumName:
            {
            iDefaultAlbumName = aSettingValue;
            break;
            }
        default:
            {
            PRINT( _L("Camera <> CCamSettingsModel::ECamPanicUnknownSettingItem 4" ))
            CamPanic( ECamPanicUnknownSettingItem );
            return;
            }
        }
    SaveStaticSettingL( static_cast<TCamSettingItemIds>(aSettingItem) );    
    }



// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::TextSettingValue
// Returns the current text value for the specified setting
// ---------------------------------------------------------------------------
//
TPtrC CCamStaticSettingsModel::TextSettingValue( TInt aSettingItem ) const
    {
    switch ( aSettingItem )
        {
        case ECamSettingItemPhotoNameBase:            return iPhotoBaseName;
        case ECamSettingItemVideoNameBase:            return iVideoBaseName;
        case ECamSettingItemDefaultAlbumName:         return iDefaultAlbumName;
        default:
        {
        PRINT( _L("Camera =><= CCamSettingsModel::TextSettingValue, PANIC!!!" ) );
        CamPanic( ECamPanicUnknownSettingItem );
        }
        }
      return NULL;
    }

#ifndef CAMERAAPP_PLUGIN_BUILD
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::LoadStaticSettingsL
// Loads the static settings from shared data. Required to update
// the settings whenever get foreground event, incase of external
// changes to the settings.
// Note that static settings cannot be loaded until the AppUi has been created,
// as which settings to use is dependent on if app is embedded or not.
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::LoadStaticSettingsL( TBool aIsEmbedded )
    {
    PRINT(_L("Camera => CCamStaticSettingsModel::LoadStaticSettingsL" ))
    iEmbedded = aIsEmbedded;

    // Free all memory currently used by the static settings.
    UnloadStaticSettings();
    
    // Settings that depend on embedded status
    // If this application is embedded in MMS load
    // the embedded defaults for static settings.
#ifndef CAMERAAPP_PLUGIN_BUILD
    if ( iEmbedded )
        {
        LoadEmbeddedSettingsL();
        }
#endif //CAMERAAPP_PLUGIN_BUILD

    // Settings that *do not* depend on embedded status
    LoadPhotoStaticSettingsL( EFalse );
    LoadVideoStaticSettingsL( EFalse );
    LoadCommonStaticSettingsL( EFalse );

    PRINT(_L("Camera <= CCamSettingsModel::LoadStaticSettingsL" ))
  }

#endif //#ifndef CAMERAAPP_PLUGIN_BUILD

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::ReadCenRepIntL
// Reads the specified setting from Central Repository to the specified 
// settings model array
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::ReadFromCenRepL( const TInt aMinRange, 
                                           const TInt aMaxRange,
                                           RPointerArray <TIntSetting>& aArray )
    {
    TUint32 crKey;
    TBool ignore(EFalse);
    for ( TInt i = aMinRange+1; i < aMaxRange; i++ )
        {
        ignore = EFalse;
        if ( i == ECamSettingItemPhotoNameBase )
            {
            LoadPhotoBaseNameL();
            }
        else if ( i == ECamSettingItemVideoNameBase )
            {
            LoadVideoBaseNameL();
            }
        else if ( i == ECamSettingItemDefaultAlbumName )
            {
            LoadDefaultAlbumNameL();
            }
        else
            {
            /*
            * Settings that depend on embedded status, we ignore reading the settings
            * if we are loading settings for embedded camera.
            */
            if ( iEmbedded && 
                 ( i == ECamSettingItemPhotoQuality ||
                   i == ECamSettingItemShowCapturedPhoto ||
                   i == ECamSettingItemVideoQuality ||
                   i == ECamSettingItemVideoShowCapturedVideo ||
                   i == ECamSettingItemVideoAudioRec ) )  
                {
                ignore = ETrue;
                }
            if ( !ignore )
                {
                crKey = MapSettingItem2CRKey( static_cast<TCamSettingItemIds>( i ),
                                              iEmbedded );
                ReadCenRepIntL( static_cast<TCamSettingItemIds>( i ),
                                crKey, 
                                aArray );
                }
            }
        }
    }


#ifndef CAMERAAPP_PLUGIN_BUILD
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::LoadEmbeddedSettingsL();
// Reads the specified setting from Central Repository to the specified 
// settings model array
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::LoadEmbeddedSettingsL()
    {
    LoadSettingsFromResourceL( R_CAM_PHOTO_EMBEDDED_STATIC_SETTINGS_DATA,
                               iStaticPhotoIntSettings );
    LoadSettingsFromResourceL( R_CAM_VIDEO_EMBEDDED_STATIC_SETTINGS_DATA,
                               iStaticVideoIntSettings );

   // Video - Record Audio (not shared with standalone)
   ReadCenRepIntL( ECamSettingItemVideoAudioRec,
                   KCamCrEmbeddedVideoAudRec,
                   iStaticVideoIntSettings );
    	
    TIntSetting* newSetting = new (ELeave) TIntSetting;
    CleanupStack::PushL( newSetting );

    // Read the data for this setting item from resource.
    newSetting->iItemId = ECamSettingItemPhotoQuality;

    CCamAppUiBase* appUi = 0;
    TSize requiredReso, requiredResolution;
    // if using the second camera
    if( static_cast<CCamAppUiBase*>( 
          CEikonEnv::Static()->AppUi() )->ActiveCamera() == ECamActiveCameraSecondary )
    	{
		PRINT(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL Setting secondary camera image quality" ))
    	newSetting->iValueId = iConfiguration.SecondaryCameraImageQuality();
    	}
     else
     	{
     	appUi = static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() );
    	requiredReso = appUi->RequestedNewFileResolution();
     	requiredResolution = iConfiguration.MapRequiredResolutionToActualResolutionPhoto(requiredReso);
     	if ( requiredResolution == TSize(0,0) ) 
     		{
            PRINT(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL Setting mms image quality" ))
            newSetting->iValueId = iConfiguration.MmsImageQuality();
     		}
     	else 
     	    {
     	    PRINT(_L("Camera <=> CCamStaticSettingsModdel::LoadEmbeddedSettingsL Setting custom image quality"));
     	    TSize resolutionToGet = requiredResolution;
            PRINT2(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL image resolutionToGet(%d,%d)"), resolutionToGet.iWidth, resolutionToGet.iHeight );
     	    TInt QualityIndex = iConfiguration.GetRequiredImageQualityIndex( resolutionToGet );
            PRINT1(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL image QualityIndex: %d"), QualityIndex );
     	    newSetting->iValueId = iConfiguration.ImageQuality( QualityIndex ).iPhotoQualityId;
            PRINT1(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL image newSetting->iValueId: %d"), newSetting->iValueId );
     	    }
    	}
    
    // Add the new setting item and its associated key
    // to the parallel arrays for static photo settings.
    iStaticPhotoIntSettings.AppendL( newSetting );
    CleanupStack::Pop( newSetting );
    	
    newSetting = new (ELeave) TIntSetting;
    CleanupStack::PushL( newSetting );

    // Read the data for this setting item from resource.
    newSetting->iItemId = ECamSettingItemVideoQuality;
    
    // if using the second camera
    if( static_cast<CCamAppUiBase*>( 
          CEikonEnv::Static()->AppUi() )->ActiveCamera() == ECamActiveCameraSecondary )
    	{
		PRINT(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL Setting secondary camera image quality" ))
    	newSetting->iValueId = iConfiguration.SecondaryCameraVideoQuality();
    	}
     else
     	{
     	appUi = static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() );
     	requiredReso = appUi->RequestedNewFileResolution();
     	requiredResolution = iConfiguration.MapRequiredResolutionToActualResolutionVideo(requiredReso);
     	if ( requiredResolution == TSize(0,0) ) 
     		{
            PRINT(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL Setting mms video quality" ))
            newSetting->iValueId = iConfiguration.SecondaryCameraVideoQuality();
     		}
     	else 
     	    {
     	    TSize resolutionToGet = requiredResolution;
            PRINT2(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL resolutionToGet(%d,%d)"), resolutionToGet.iWidth, resolutionToGet.iHeight );
     	    TInt QualityIndex = iConfiguration.GetRequiredVideoQualityIndex( resolutionToGet );
            PRINT1(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL QualityIndex: %d"), QualityIndex );
     	    newSetting->iValueId = iConfiguration.VideoQualitySetting( QualityIndex );
            PRINT1(_L("Camera <=> CCamStaticSettingsModel::LoadEmbeddedSettingsL newSetting->iValueId: %d"), newSetting->iValueId );
     	    }
    	}
    
    // Add the new setting item and its associated key
    // to the parallel arrays for static photo settings.
    iStaticVideoIntSettings.AppendL( newSetting );
    CleanupStack::Pop( newSetting );
    }

#endif //CAMERAAPP_PLUGIN_BUILD

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::ReadCenRepIntL
// Reads the specified setting from Central Repository to the specified 
// settings model array
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::ReadCenRepIntL( TCamSettingItemIds aUiId, 
                                              TInt aCenRepID,
                                              RPointerArray <TIntSetting>& aArray )
  {        
  PRINT2(_L("Camera => CCamStaticSettingsModel::ReadCenRepIntL set id:%d, cenrep id: 0x%02X" ), aUiId, aCenRepID )
  TIntSetting* newSetting = new( ELeave ) TIntSetting;
  CleanupStack::PushL( newSetting );
  newSetting->iItemId = aUiId;
#ifndef CAMERAAPP_PLUGIN_BUILD
  // if using the second camera, then get the photo\video quality from
  // the cached value, not the shared data file
  if( ( aUiId == ECamSettingItemPhotoQuality ||
        aUiId == ECamSettingItemVideoQuality ) &&
        static_cast<CCamAppUiBase*>( 
          CEikonEnv::Static()->AppUi() )->ActiveCamera() == ECamActiveCameraSecondary )
    {
    if ( aUiId == ECamSettingItemPhotoQuality )
      {
      newSetting->iValueId = iSecondaryCameraSettings.iPhotoQuality;
      }
    else
      {
      newSetting->iValueId = iSecondaryCameraSettings.iVideoQuality;
      }
    }
  else // get the value from the shared data file
#endif //CAMERAAPP_PLUGIN_BUILD  
    {
    User::LeaveIfError( iRepository->Get( aCenRepID , newSetting->iValueId ) );
    
   // When getting the default video qulity, should get the default setting
   // from ICM
   if(aUiId == ECamSettingItemVideoQuality && 
   	  newSetting->iValueId == KCamUseDefaultVideoQuality)
   	{
    newSetting->iValueId = iConfiguration.GetDefaultVideoQualityFromIcmL();
    }
    }

  aArray.AppendL( newSetting );
  CleanupStack::Pop( newSetting );
  PRINT(_L("Camera <= CCamStaticSettingsModel::ReadCenRepIntL" ))
  }

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SaveCenRepItemL
// Writes the specified setting to the Central Repository
// ---------------------------------------------------------------------------
//
void
CCamStaticSettingsModel::SaveCenRepItemL( 
    TCamSettingItemIds                aItemId,
    TInt                              aCenRepId,
    const RPointerArray<TIntSetting>& aArray )
  {
  PRINT( _L("Camera => CCamStaticSettingsModel::SaveCenRepItemL") );
  TInt settingIndex = SearchInSettingsListFor( aArray, aItemId );
  
  if ( settingIndex != KErrNotFound )
    {
	TInt settingValue = aArray[ settingIndex ]->iValueId;
    User::LeaveIfError( iRepository->Set( aCenRepId, settingValue ) );
    }
  else
    {
    User::Leave( settingIndex );
    }
  PRINT( _L("Camera <= CCamStaticSettingsModel::SaveCenRepItemL") );
  }    

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SaveSettingsL
// Saves the static settings to the shared data ini file
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::SaveSettingsL() 
  {
  // do nothing, from now whenever a setting is changed,
  // we call savestaticsettingL.
  }


// --------------------------------------------
// CCamStaticSettingsModel::SaveStaticSettingL
// --------------------------------------------
//
void
CCamStaticSettingsModel::SaveStaticSettingL( TCamSettingItemIds aSettingId )
  {
  PRINT( _L("CCamStaticSettingsModel => SaveStaticSettingL") ); 

  switch( aSettingId )
	{
	// Special cases for text settings.
	// KCamCrLastUsedMonthFolder is also in text form, but it cannot be accessed
	// with a setting item id.
	case ECamSettingItemPhotoNameBase:    SavePhotoBaseNameL();    break;
	case ECamSettingItemVideoNameBase:    SaveVideoBaseNameL();    break;
	case ECamSettingItemDefaultAlbumName: SaveDefaultAlbumNameL(); break;

      // Only save photo and video quality for primary camera
    case ECamSettingItemPhotoQuality:
      {
      if ( !iEmbedded )
        {
 #ifndef CAMERAAPP_PLUGIN_BUILD
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() );
        if ( appUi->ActiveCamera() == ECamActiveCameraPrimary )
 #endif //CAMERAAPP_PLUGIN_BUILD
          {
          SaveCenRepItemL( ECamSettingItemPhotoQuality,
                           KCamCrPhotoQuality, 
                           iStaticPhotoIntSettings );
          }
        }  
      break;
      }
    case ECamSettingItemVideoQuality:
      {
      if ( !iEmbedded )
        {
#ifndef CAMERAAPP_PLUGIN_BUILD
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() );  
        if ( appUi->ActiveCamera() == ECamActiveCameraPrimary )
#endif //CAMERAAPP_PLUGIN_BUILD
          {
          SaveCenRepItemL( ECamSettingItemVideoQuality, 
                           KCamCrVideoQuality, 
                           iStaticVideoIntSettings );
          }
        }
      break;
      }
      // Text settings which require no special handling
    case ECamSettingItemImageToolbarItems:
    case ECamSettingItemImageToolbarDefaultItems:
      {
      TUint32 crKey = MapSettingItem2CRKey( aSettingId, iEmbedded );
      TPtrC   value = TextSettingValue( aSettingId );

      User::LeaveIfError( iRepository->Set( crKey, value ) );
      break;
      }
    case ECamSettingItemShowCapturedPhoto:
    case ECamSettingItemVideoShowCapturedVideo:
        {
        if ( iEmbedded )
            {
            // if in the embedded mode, we dont save value to the CenRep
            // because in Embedded mode show photo/video capture setting is
            // always on.
            return;
            }
        else
            {
            }
        }
    // Integer settings which require no special handling
    default:
      {
      RPointerArray<TIntSetting>* settingArray = 
          MapSettingItem2SettingsList( aSettingId );
      
      if( settingArray )
        {
        TUint32 crKey = MapSettingItem2CRKey( aSettingId, iEmbedded );
        SaveCenRepItemL( aSettingId, crKey, *settingArray );
        }
      else
        {
        CamPanic( ECamPanicUnknownSettingItem );
        }
      break;
      }
    } // switch
  
  PRINT( _L("CCamStaticSettingsModel <= SaveStaticSettingL") ); 
  }


// ---------------------------------------------------------------------------
// MapSettingItem2CRKey <<static>>
// ---------------------------------------------------------------------------
//
TUint32
CCamStaticSettingsModel::MapSettingItem2CRKey( TCamSettingItemIds aSettingId, 
                                         TBool              aEmbedded )
  {
  PRINT( _L("Camera => CCamStaticSettingsModel::MapSettingItem2CRKey") );
  TUint32 crKey( 0 );

  // -------------------------------------------------------
  // static photo settings
  if( ECamSettingItemStaticPhotoRangeMin < aSettingId
   && ECamSettingItemStaticPhotoRangeMax > aSettingId )
    {
    PRINT( _L(" Camera <=> CCamStaticSettingsModel ECamSettingItemStaticPhoto") );
    switch( aSettingId )
      {
      case ECamSettingItemPhotoQuality:             crKey = KCamCrPhotoQuality;             break;
      case ECamSettingItemPhotoSize:                crKey = KCamCrPhotoSize;                break;
      case ECamSettingItemPhotoShowFocusPoint:      crKey = KCamCrFocusPoint;               break;
      case ECamSettingItemPhotoStoreInAlbum:        crKey = KCamCrPhotoStoreAlbum;          break; //KCamCrDefaultAlbumId
      case ECamSettingItemShowCapturedPhoto:        crKey = KCamCrPhotoShowCaptured;        break;
      case ECamSettingItemPhotoCaptureTone:         crKey = KCamCrPhotoCaptureTone;         break;
      case ECamSettingItemFaceTracking:             crKey = KCamCrPhotoFaceTracking;        break;
      case ECamSettingItemPhotoMediaStorage:        crKey = KCamCrPhotoMemInUse;            break;
  	  case ECamSettingItemPhotoNameBase:            crKey = KCamCrPhotoNameBase;            break;
  
      case ECamSettingItemPhotoNameBaseType:        crKey = KCamCrPhotoNameType;            break;
      case ECamSettingItemPhotoNumber:              crKey = KCamCrPhotoImgCount;            break;
      case ECamSettingItemPhotoDigitalZoom:         crKey = KCamCrPhotoExtDigZoom;          break;
      case ECamSettingItemImageRotation:            crKey = KCamCrPhotoRotation;            break;
      case ECamSettingItemFlickerCancel:            crKey = KCamCrFlickerCancellation;      break;

      case ECamSettingItemImageToolbarItems:        crKey = KCamCrImageToolbarItems;        break;
      case ECamSettingItemImageToolbarDefaultItems: crKey = KCamCrImageToolbarDefaultItems; break;
      case ECamSettingItemContinuousAutofocus: 
          crKey = KCamCrContinuousAutofocus;
          break;

      default:                                      CamPanic( ECamPanicUnknownSettingItem );
                                                    break;
      }
    }
  // -------------------------------------------------------
  // static video settings        
  else if( ECamSettingItemStaticVideoRangeMin < aSettingId
        && ECamSettingItemStaticVideoRangeMax > aSettingId )
    {
    PRINT( _L(" Camera <=> CCamStaticSettingsModel : ECamSettingItemStaticVideoRange") );
    switch( aSettingId )
      {   
      case ECamSettingItemVideoAudioRec:          
        if( aEmbedded ) crKey = KCamCrEmbeddedVideoAudRec;
        else            crKey = KCamCrVideoAudRec;
        break;
      case ECamSettingItemContinuousAutofocus: 
          crKey = KCamCrContinuousAutofocus;
          break;
      case ECamSettingItemVideoResolution:        crKey = KCamCrVideoRes;           break;
      case ECamSettingItemVideoClipLength:        crKey = KCamCrVideoClipLen;       break;
      case ECamSettingItemVideoFileType:          crKey = KCamCrVideoFileType;      break;
      case ECamSettingItemVideoStoreInAlbum:      crKey = KCamCrVideoStoreAlbum;    break; //KCamCrDefaultAlbumId
      case ECamSettingItemVideoShowCapturedVideo: crKey = KCamCrVideoShowCaptured;  break;
      case ECamSettingItemVideoMediaStorage:      crKey = KCamCrVideoMemInUse;      break;
      case ECamSettingItemVideoNameBase:          crKey = KCamCrVideoNameBase;      break;
      case ECamSettingItemVideoNameBaseType:      crKey = KCamCrVideoNameType;      break; 
      case ECamSettingItemVideoNumber:            crKey = KCamCrVideoCount;         break;       
      case ECamSettingItemVideoQuality:           crKey = KCamCrVideoQuality;       break;
      case ECamSettingItemVideoDigitalZoom:       crKey = KCamCrVideoExtDigZoom;    break;
      case ECamSettingItemVideoStab:              crKey = KCamCrVideoStabilisation; break;
      default:                                    CamPanic( ECamPanicUnknownSettingItem );
                                                  break;
      }
    }        
  // -------------------------------------------------------
  // static common settings
  else if( ECamSettingItemStaticCommonRangeMin < aSettingId
        && ECamSettingItemStaticCommonRangeMax > aSettingId )
    {
    PRINT( _L("Camera <=> CCamStaticSettingsModel ECamSettingItemStaticCommon") );
    switch( aSettingId )
      {
      case ECamSettingItemUserMode:           crKey = KCamCrUserMode;           break;
      case ECamSettingItemDefaultAlbumId:     crKey = KCamCrDefaultAlbumId;     break;
      case ECamSettingItemDefaultAlbumName:   crKey = KCamCrDefaultAlbumTitle;  break;
      case ECamSettingItemRecLocation:        crKey = KCamCrPhotoStoreLocation; break;
      default:                                CamPanic( ECamPanicUnknownSettingItem );
                                              break;
      }
    }        
  // -------------------------------------------------------
  else if( ECamSettingItemUserSceneRangeMin < aSettingId
        && ECamSettingItemUserSceneRangeMax > aSettingId )
    {
    PRINT( _L("Camera <=> CCamStaticSettingsModel ECamSettingItemUserSceneRange") );
    switch( aSettingId )
      {
      case ECamSettingItemUserSceneBasedOnScene:      crKey = KCamCrUserSceneBaseScene;         break;
      case ECamSettingItemUserSceneWhitebalance:      crKey = KCamCrUserSceneWhiteBalance;      break;
      case ECamSettingItemUserSceneColourFilter:      crKey = KCamCrUserSceneColourFilter;      break;
      case ECamSettingItemUserSceneExposure:          crKey = KCamCrUserSceneExposure;          break;
      case ECamSettingItemUserSceneFlash:             crKey = KCamCrUserSceneFlash;             break;
      case ECamSettingItemUserSceneBrightness:        crKey = KCamCrUserSceneBrightness;        break;
      case ECamSettingItemUserSceneContrast:          crKey = KCamCrUserSceneContrast;          break;
      case ECamSettingItemUserSceneImageSharpness:    crKey = KCamCrUserSceneImageSharpness;    break;
      case ECamSettingItemUserSceneColourSaturation:  crKey = KCamCrUserSceneColourSaturation;  break;
      case ECamSettingItemUserSceneLightSensitivity:  crKey = KCamCrUserSceneLightSensitivity;  break;
      default:                                        CamPanic( ECamPanicUnknownSettingItem );
                                                      break;
      }
    }
  else
    {
    PRINT( _L(" Camera <=> CCamStaticSettingsModel CamPanic( ECamPanicUnknownSettingItem )") );
    CamPanic( ECamPanicUnknownSettingItem );
    }

  PRINT( _L("Camera <= CCamSettingsModel::MapSettingItem2CRKey") );
  return crKey;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
RPointerArray<TIntSetting>*
CCamStaticSettingsModel::MapSettingItem2SettingsList( TCamSettingItemIds aSettingId )
  {
  PRINT( _L("Camera => CCamStaticSettingsModel::MapSettingItem2SettingsList") );

  RPointerArray<TIntSetting>* array( NULL );

  if( ECamSettingItemStaticPhotoRangeMin < aSettingId &&
      ECamSettingItemStaticPhotoRangeMax > aSettingId )
    {
    if( ECamSettingItemPhotoNameBase == aSettingId )
      array = NULL;
    else
      array = &iStaticPhotoIntSettings;
    }
  else if( ECamSettingItemStaticVideoRangeMin < aSettingId &&
           ECamSettingItemStaticVideoRangeMax > aSettingId )
    {
    if( ECamSettingItemVideoNameBase == aSettingId )
      array = NULL;
    else
      array = &iStaticVideoIntSettings;
    }
  else if( ECamSettingItemStaticCommonRangeMin < aSettingId &&
           ECamSettingItemStaticCommonRangeMax > aSettingId )
    {
    array = &iStaticCommonIntSettings;
    }
  else
    {
    CamPanic( ECamPanicUnknownSettingItem );
    array = NULL;
    }

  PRINT( _L("Camera <= CCamStaticSettingsModel::MapSettingItem2SettingsList") );
  return array;
  }


// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::~CCamStaticSettingsModel
// Destructor
// ---------------------------------------------------------------------------
//
CCamStaticSettingsModel::~CCamStaticSettingsModel()
  {
  PRINT( _L("Camera => ~CCamStaticSettingsModel") );
  iStaticCommonIntSettings.ResetAndDestroy();
  iStaticCommonIntSettings.Close();
  
  iStaticPhotoIntSettings.ResetAndDestroy();
  iStaticPhotoIntSettings.Close();
  
  iStaticVideoIntSettings.ResetAndDestroy();
  iStaticVideoIntSettings.Close();
  delete iRepository;
  delete iConfigManager;
  iConfigManager = NULL;
  PRINT( _L("Camera <= ~CCamStaticSettingsModel") );
  }
  

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::CCamStaticSettingsModel
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamStaticSettingsModel::CCamStaticSettingsModel( CCamConfiguration& aConfiguration ) 
  : iEmbedded( EFalse ),
    iConfiguration( aConfiguration )
  {
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
  }
  

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::ConstructL()
    {
    PRINT( _L("Camera => CCamStaticSettingsModel::ConstructL") );
    iRepository = CRepository::NewL( KCRUidCameraappSettings );
    iConfigManager = CCameraUiConfigManager::NewL();
    PRINT( _L("Camera <= CCamStaticSettingsModel::ConstructL") );
    }


// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SearchInSettingsListFor
// Searches in a settings list for a particular setting item.
// ---------------------------------------------------------------------------
//
TInt 
CCamStaticSettingsModel::SearchInSettingsListFor( 
    const RPointerArray<TIntSetting>& aSettingsList, 
          TInt                        aSettingItem  ) const
  {
  return aSettingsList.Find( aSettingItem, SettingIdMatches );
  }

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::ResetRepository
// Reset Camera central repository file
// ---------------------------------------------------------------------------
//   
 void CCamStaticSettingsModel::ResetRepository()
 	{
 	if( iRepository )
 		{
 		iRepository->Reset();
 		}
 	}


// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::UnloadStaticSettings
// Remove any previously loaded static settings.
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::UnloadStaticSettings()
    {
    PRINT( _L("Camera => CCamStaticSettingsModel::UnloadStaticSettings()" ))
  	iStaticCommonIntSettings.ResetAndDestroy();
    iStaticPhotoIntSettings.ResetAndDestroy();
    iStaticVideoIntSettings.ResetAndDestroy();
    PRINT( _L("Camera <= CCamStaticSettingsModel::UnloadStaticSettings()" ))
    }

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::LoadPhotoBaseNameL
// Loads the photo base name either from resource or from shared data
// as appropriate
// ---------------------------------------------------------------------------
//
void 
CCamStaticSettingsModel::LoadPhotoBaseNameL()
  {
  User::LeaveIfError( iRepository->Get( KCamCrPhotoNameBase , iPhotoBaseName ) );
  // If photoname base not defined
  if ( iPhotoBaseName.Length() == 0 )
    {
	// Read base file name from resources and set the shared data key.
	StringLoader::Load( iPhotoBaseName, R_CAM_NAMEBASE_IMAGE );
    }
  }

//
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::LoadVideoBaseNameL
// Loads the video base name either from resource or from shared data
// as appropriate
// ---------------------------------------------------------------------------
void 
CCamStaticSettingsModel::LoadVideoBaseNameL()
  {
  User::LeaveIfError( iRepository->Get( KCamCrVideoNameBase , iVideoBaseName ) );
  if ( iVideoBaseName.Length() == 0 )
    {
    // Read base file name from resources and set the shared data key.
	StringLoader::Load( iVideoBaseName, R_CAM_NAMEBASE_VIDEO );
    }
  }


//
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::LoadVideoBaseNameL
// Loads the video base name either from resource or from shared data
// as appropriate
// ---------------------------------------------------------------------------
void 
CCamStaticSettingsModel::LoadDefaultAlbumNameL()
  {
  User::LeaveIfError( iRepository->Get( KCamCrDefaultAlbumTitle,
                                        iDefaultAlbumName ) );
  }
  
  
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SavePhotoBaseNameL
// Saves the photo base name after comparing against resource to see if we
// have switched back to the default base
// ---------------------------------------------------------------------------
//
void 
CCamStaticSettingsModel::SavePhotoBaseNameL()
  {
  TBuf<KMaxNameBaseLength> savedBaseName;
  TBuf<KMaxNameBaseLength> resourceBaseName;
  TBuf<KMaxNameBaseLength> sharedDataBaseName = iPhotoBaseName;
  
  User::LeaveIfError( iRepository->Get( KCamCrPhotoNameBase,
                                        savedBaseName ) );
  StringLoader::Load(resourceBaseName, R_CAM_NAMEBASE_IMAGE );
  
  if ( savedBaseName.Length() == 0 && iPhotoBaseName == resourceBaseName )
    {
    sharedDataBaseName = KNullDesC;
    }
  User::LeaveIfError( iRepository->Set( KCamCrPhotoNameBase,
                                        sharedDataBaseName ) );
  }

//
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SaveVideoBaseNameL
// Saves the video base name after comparing against resource to see if we
// have switched back to the default base
// ---------------------------------------------------------------------------
void CCamStaticSettingsModel::SaveVideoBaseNameL()
    {
    TBuf<KMaxNameBaseLength> savedBaseName;
    TBuf<KMaxNameBaseLength> resourceBaseName;
    TBuf<KMaxNameBaseLength> sharedDataBaseName = iVideoBaseName;

	User::LeaveIfError( iRepository->Get( KCamCrVideoNameBase,
	                                      savedBaseName ) );
    StringLoader::Load(resourceBaseName, R_CAM_NAMEBASE_VIDEO );

    if ( savedBaseName.Length() == 0 && 
         iVideoBaseName == resourceBaseName )
		{
		sharedDataBaseName = KNullDesC;
		}

	User::LeaveIfError( iRepository->Set( KCamCrVideoNameBase,
	                                      sharedDataBaseName ) );     
    }


//
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::SaveDefaultAlbumNameL
// Saves the name of the default album set
// ---------------------------------------------------------------------------
void CCamStaticSettingsModel::SaveDefaultAlbumNameL()
  {
	User::LeaveIfError( iRepository->Set( KCamCrDefaultAlbumTitle,
	                                      iDefaultAlbumName       ) );     
  }


// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::ReadCenRepStringL
// ---------------------------------------------------------------------------
//
HBufC*  
CCamStaticSettingsModel::ReadCenRepStringL( TInt aCenRepKeyId )
  {
  PRINT1( _L("Camera => CCamSettingsModel::ReadCenRepStringL, key:0x%02x"), aCenRepKeyId );

  HBufC* string = HBufC::NewLC( KCamCRStringInitialLength );
    
  TPtr ptr    = string->Des();
  TInt size   = 0;
  TInt status = iRepository->Get( aCenRepKeyId, ptr, size );
		    	
  // Did not fit into the string, reserve more memory and try again
  if( KErrOverflow == status ) 
    {
    PRINT2( _L("Camera <> CCamSettingsModel: need bigger buffer, length: %d -> %d"), ptr.MaxLength(), size );
    CleanupStack::PopAndDestroy(); // String
    string = HBufC::NewLC( size );
    ptr    = string->Des();

    status = iRepository->Get( aCenRepKeyId, ptr, size );
    }

  User::LeaveIfError( status );   
  CleanupStack::Pop(); // string   
  PRINT1( _L("Camera <= CCamSettingsModel::ReadCenRepStringL, got string:[%S]"), string);
  
  return string;
  }

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::ResetSettingItem
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::ResetSettingItem( const TInt aCenRepKeyId )
    {
    TInt err = iRepository->Reset( aCenRepKeyId );
    if ( KErrNone != err )
        {
        // Handle the error case
        }
    }


// ---------------------------------------------------------------------------
// CCamSettingsModel::StorePrimaryCameraSettingsL
// Stores the primary camera settings so they can be reapplied when
// changing from front to back camera
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::StorePrimaryCameraSettingsL()
    {
    PRINT( _L("Camera => CCamStaticSettingsModel::StorePrimaryCameraSettingsL"))
    TInt settingIndex = SearchInSettingsListFor( iStaticPhotoIntSettings, 
                                                 ECamSettingItemPhotoQuality );
    if ( settingIndex != KErrNotFound )
        {
        iPrimaryCameraSettings.iPhotoQuality = 
            iStaticPhotoIntSettings[settingIndex]->iValueId;
        }

    settingIndex = SearchInSettingsListFor( iStaticVideoIntSettings, 
                                            ECamSettingItemVideoQuality );
    if ( settingIndex != KErrNotFound )
        {
        iPrimaryCameraSettings.iVideoQuality = 
            iStaticVideoIntSettings[settingIndex]->iValueId;
        }
    PRINT( _L("Camera <= CCamStaticSettingsModel::StorePrimaryCameraSettingsL"))    
    }

// ---------------------------------------------------------------------------
// CCamSettingsModel::RestorePrimaryCameraSettingsL
// Restores the primary camera settings when
// changing from front to back camera
// ---------------------------------------------------------------------------
//
void CCamStaticSettingsModel::RestorePrimaryCameraSettingsL()
    {
    // set the stored primary camera settings
    SetIntegerSettingValueL( ECamSettingItemPhotoQuality, 
                             iPrimaryCameraSettings.iPhotoQuality );
    SetIntegerSettingValueL( ECamSettingItemVideoQuality, 
                             iPrimaryCameraSettings.iVideoQuality );
    // set the secondary camera settings back to defaults
    iSecondaryCameraSettings.iPhotoQuality = iConfiguration.SecondaryCameraImageQuality();
    iSecondaryCameraSettings.iVideoQuality = iConfiguration.SecondaryCameraVideoQuality();
    }

#ifndef CAMERAAPP_PLUGIN_BUILD 
// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::LoadDynamicSettingsL
// Loads the dynamic settings from resource file for a 
// particular group of settings. 
// ---------------------------------------------------------------------------
//
void 
CCamStaticSettingsModel::LoadSettingsFromResourceL(
                                        TInt aResourceId, 
                                        RPointerArray<TIntSetting>& aSettingsList )
    {
    // Create resource reader for reading photo static settings
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC( reader, aResourceId );
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

        // Add the new setting item and its associated key
        // to the parallel arrays for static photo settings.
        aSettingsList.AppendL( newSetting );
        CleanupStack::Pop( newSetting );
        }

    CleanupStack::PopAndDestroy(); // reader
    }
#endif //CAMERAAPP_PLUGIN_BUILD

// ---------------------------------------------------------------------------
// CCamStaticSettingsModel::Configuration
// 
// ---------------------------------------------------------------------------
//
CCamConfiguration&
CCamStaticSettingsModel::Configuration() const
    {
    return iConfiguration;      
    }

//  
// CCamStaticSettingsModel::LoadPhotoStaticSettingsL
//
void CCamStaticSettingsModel::LoadPhotoStaticSettingsL( const TBool aResetFromPlugin )
    {
    if ( aResetFromPlugin )
        {
        LoadCommonStaticSettingsL( aResetFromPlugin );
        iStaticPhotoIntSettings.ResetAndDestroy();
        }
    // Load Photo/Image Settings    
    ReadFromCenRepL( static_cast<TInt>( ECamSettingItemStaticPhotoRangeMin ),
                     static_cast<TInt>( ECamSettingItemPhotoCompression ),
                     iStaticPhotoIntSettings );

    }

//
// CCamStaticSettingsModel::LoadVideoStaticSettingsL
//
void CCamStaticSettingsModel::LoadVideoStaticSettingsL( const TBool aResetFromPlugin )
    {
    if ( aResetFromPlugin )
        {
        LoadCommonStaticSettingsL( aResetFromPlugin );
        iStaticVideoIntSettings.ResetAndDestroy();
        }
    // Load Video Settings    
    ReadFromCenRepL( static_cast<TInt>( ECamSettingItemStaticVideoRangeMin ), 
                     static_cast<TInt>( ECamSettingItemVideoOpZoomOff ), 
                     iStaticVideoIntSettings );
    
    }

//
// CCamStaticSettingsModel::LoadCommonStaticSettingsL
//
void CCamStaticSettingsModel::LoadCommonStaticSettingsL( const TBool aResetFromPlugin )
    {
    if ( aResetFromPlugin )
        {
        iStaticCommonIntSettings.ResetAndDestroy();
        }
    // Load Common Settings
    ReadFromCenRepL( static_cast<TInt>( ECamSettingItemStaticCommonRangeMin ),
                     static_cast<TInt>( ECamSettingItemStaticCommonRangeMax ),
                     iStaticCommonIntSettings );
    }

/*
 *  Handle to Camera Ui Config Manager
*/
CCameraUiConfigManager* CCamStaticSettingsModel::UiConfigManagerPtr()
    {
    return iConfigManager;
    }
//End of File
