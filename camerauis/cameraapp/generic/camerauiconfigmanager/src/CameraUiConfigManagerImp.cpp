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
* Description:  Helper class which handles reading and storing the cr keys 
 *                and its values.
 *
*/



// Includes
#include <StringLoader.h>
#include <barsread.h>
#include <e32base.h>
#include <e32const.h>
#include <centralrepository.h>
#include "CameraUiConfigManagerImp.h"
#include "CameraConfigurationCrKeys.h"
#include "camlogging.h"
#include "CamSettings.hrh"


// ===========================================================================
// Constants

const TInt KCamCRStringInitialLength = 1024;
_LIT( KItemsSeparator, "," );


// ===========================================================================

inline TBool SettingIdMatches( const TInt*        aSettingId, 
                               const TSupportedSettingsData& aSettingItem )
  {
  return ( *aSettingId == aSettingItem.iSettingId );
  };

// ===========================================================================


// Class methods

// ---------------------------------------------------------------------------
// CCameraUiConfigManagerImp::NewL
// Symbian OS two-phased constructor 
// ---------------------------------------------------------------------------
//
CCameraUiConfigManagerImp* CCameraUiConfigManagerImp::NewL()
    {
    CCameraUiConfigManagerImp* self = CCameraUiConfigManagerImp::NewLC();
    CleanupStack::Pop( self );
    return self;
    }



// ---------------------------------------------------------------------------
// CCameraUiConfigManagerImp::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCameraUiConfigManagerImp* CCameraUiConfigManagerImp::NewLC()
    {
    CCameraUiConfigManagerImp* self = new( ELeave ) CCameraUiConfigManagerImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CCameraUiConfigManagerImp::~CCameraUiConfigManagerImp
// Destructor
// ---------------------------------------------------------------------------
//
CCameraUiConfigManagerImp::~CCameraUiConfigManagerImp()
  {
  for( TInt index = 0;index <  iSupportedSettings.Count();index++ )
        {
        if ( iSupportedSettings[index] )
            {
            iSupportedSettings[index]->iSupportedSettingItems.Close();            
            }
        }
  iSupportedSettings.ResetAndDestroy();
  iSupportedSettings.Close();
  
  delete iRepository;
  }
  

  

// ---------------------------------------------------------------------------
// CCameraUiConfigManagerImp::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCameraUiConfigManagerImp::ConstructL()
    {
    iRepository = CRepository::NewL( KCameraDynamicConfigurationCrKeys );
    LoadAllDynamicSettingsL();
    }



// ----------------------------------------------------------------------------------
// CCameraUiConfigManagerImp::IsFeatureSupported
// ----------------------------------------------------------------------------------
//
TInt CCameraUiConfigManagerImp::IsFeatureSupported(
                                    const TCamDynamicSettings aSettingId ) const
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::IsFeatureSupportedL" ))
    
    TInt settingIndex ( KErrNotFound );
    TInt value(0); // setting not supported
    
    if ( iSupportedSettings.Count() > 0 )
        {
        settingIndex = SearchInSettingsListFor( iSupportedSettings,
                                                static_cast<TInt>( aSettingId ) );
        }
    else
        {
        // if there are no supported settings
        }

    if ( KErrNotFound != settingIndex )
        {
        if ( iSupportedSettings[settingIndex] )
            {
            value = iSupportedSettings[settingIndex]->iSupportedValue;
            }
        }
    else
        {
        // setting item not found; hence not supported.
        }
    PRINT1( _L("Camera <= CCameraUiConfigManagerImp::IsFeatureSupportedL = %d" ), value );
    return value;
    }



//
//CCameraUiConfigManagerImp::SupportedSettingItemsL
//
void CCameraUiConfigManagerImp::SupportedSettingItemsL(
                                    const TCamDynamicSettings aSettingId,
                                    RArray<TInt>& aSupportedValues )
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::SupportedSettingItemsL" ))
    TInt settingIndex ( KErrNotFound );
    aSupportedValues.Reset();

    if ( iSupportedSettings.Count() > 0 )
        {
        settingIndex = SearchInSettingsListFor( iSupportedSettings,
                                                static_cast<TInt>( aSettingId ) );
        }
    else
        {
        // do nothing
        }
    if ( KErrNotFound != settingIndex )
        {
        // if the index is valid, copy all corresponding setting items to the array
        for ( TInt i = 0; 
              i < iSupportedSettings[settingIndex]->iSupportedSettingItems.Count();
              i++ )
            {
            if ( iSupportedSettings[settingIndex] )
                {
                aSupportedValues.AppendL( 
                        iSupportedSettings[settingIndex]->iSupportedSettingItems[i] );
                }
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    PRINT( _L("Camera <= CCameraUiConfigManagerImp::SupportedSettingItemsL" ))
    }



// ---------------------------------------------------------------------------
// CCameraUiConfigManager::LoadAllDynamicSettingsL
// Load all dynamic settings
// ---------------------------------------------------------------------------
//
void CCameraUiConfigManagerImp::LoadAllDynamicSettingsL()
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::LoadAllDynamicSettingsL()" ))	
    RArray<TDynamicSettingsData> settingItemDefaultData;
    CleanupClosePushL( settingItemDefaultData );
    
    TInt supportedValue = 0;
    TInt error = KErrNone;
    
    for( TInt settingId = ( static_cast<TInt>( ECamDynamicSettingsMin ) + 1 ); 
              settingId <   static_cast<TInt>( ECamDynamicSettingsMax ); 
              settingId++ )
        {
        if ( LoadCrForFeatureSupport( settingId ) )
            {
            error = iRepository->Get( MapSettingItemToCrFeatureKeyL( settingId ),
                                      supportedValue );
            }
        else
            {
            supportedValue = 1;
            }

        if ( error ) break; // if any error reading CenRep; do proper clean-up

        switch( settingId )
            {
            case ECamDynamicSettingCaptureToneDelaySupport:
            case ECamDynamicSettingCaptureToneDelayValue:
            case ECamDynamicSettingRequiredRamMemory:
            case ECamDynamicSettingCriticalLevelRamMemory:
            case ECamDynamicSettingRequiredRamMemoryFocusGained:
            case ECamDynamicSettingCriticalLevelRamMemoryFocusGained:
            case ECamDynamicSettingPhoto:
            case ECamDynamicSettingPublishZoomState:
            case ECamDynamicSettingAutofocusSetInHyperfocalAtStartup:
            case ECamDynamicSettingLocation:
            case ECamDynamicSettingKeyLockWatcher:
            case ECamDynamicSettingSecondaryCamera:
            case ECamDynamicSettingLensCover:
            case ECamDynamicSettingXenonFlash:
            case ECamDynamicSettingLightSensitivity:
            case ECamDynamicSettingExtLightSensitivity:
            case ECamDynamicSettingFaceTracking:
            case ECamDynamicSettingOpticalJoyStick:
            case ECamDynamicSettingOneClickUpload:
            case ECamDynamicSettingVideoLight:
            case ECamDynamicSettingAutoFocus:
            case ECamDynamicSettingVideoStabilization:
            case ECamDynamicSettingOrientationSensor:
			case ECamDynamicSetting2ndCameraOrientation:
            case ECamDynamicSettingUIOrientationOverride:
            case ECamDynamicSettingThumbnailManagerAPI:
            case ECamDynamicSettingBurstMode:
			case ECamDynamicSettingContinuousAF: 
            case ECamDynamicSettingBurstFileSizeEstimateFactor:
                {
                PRINT1( _L("Camera <> CCameraUiConfigManagerImp::LoadAllDynamicSettingsL() append %d" ), settingId )
                AppendToSettingsArrayL( settingId,
                                        supportedValue,
                                        settingItemDefaultData );
                break;
                }
            case ECamDynamicSettingVideoSceneModeNormal:
            case ECamDynamicSettingVideoSceneModeNight:
            case ECamDynamicSettingVideoSceneModeLowLight:
            case ECamDynamicSettingDSAViewFinder:
            case ECamDynamicSettingImageSceneModeAuto:
            case ECamDynamicSettingImageSceneModeUser:
            case ECamDynamicSettingImageSceneModeMacro:
            case ECamDynamicSettingImageSceneModePotrait:
            case ECamDynamicSettingImageSceneModeScenery:
            case ECamDynamicSettingImageSceneModeSports:
            case ECamDynamicSettingImageSceneModeNight:
            case ECamDynamicSettingImageSceneModeNightPotrait:
            case ECamDynamicSettingPCCaptureKeys:
            case ECamDynamicSettingSCCaptureKeys:
            case ECamDynamicSettingPCAutoFocusKeys:
            case ECamDynamicSettingSCAutoFocusKeys:
            case ECamDynamicSettingPCZoomIn:
            case ECamDynamicSettingSCZoomIn:
            case ECamDynamicSettingPCZoomOut:
            case ECamDynamicSettingSCZoomOut:
            case ECamDynamicSettingWhiteBalance:
            case ECamDynamicSettingColorTone:
            case ECamDynamicSettingContrast:
            case ECamDynamicSettingBrightness:
            case ECamDynamicSettingEV:
            case ECamDynamicSettingSharpness:
            case ECamDynamicSettingFlashMode:
            case ECamDynamicSettingZoomLAF:
            case ECamDynamicSettingMaxOpticalZoomLimits:
            case ECamDynamicSettingMaxExtendedZoomLimits:
            case ECamDynamicSettingMaxDigitalZoomLimits:
            case ECamDynamicSettingZoomDelays:
                {
                PRINT1( _L("Camera <> CCameraUiConfigManagerImp::LoadAllDynamicSettingsL() %d" ), settingId )
                LoadConfiguredSettingItemValuesFromCrL( settingId,
                                                        supportedValue,
                                                        settingItemDefaultData );
                break;
                }
            default:
                {
                break;
                }
            
            }
        }
    CleanupStack::PopAndDestroy( &settingItemDefaultData );
    User::LeaveIfError( error );
    PRINT( _L("Camera <= CCameraUiConfigManagerImp::LoadAllDynamicSettingsL()" ))
    }



//
//CCameraUiConfigManagerImp::LoadConfiguredSettingItemValuesFromCr
//
void CCameraUiConfigManagerImp::LoadConfiguredSettingItemValuesFromCrL(
                                       const TInt aSettingId,
                                       const TInt aSupportedValue,
                                       RArray<TDynamicSettingsData>& aDefaultItems )
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::LoadConfiguredSettingItemValuesFromCrL" ))		
    // Load supported setting items only when feature is supported
    LoadDefaultSettingsDataL( aSettingId, aDefaultItems );
    AppendToSettingsArrayL( aSettingId,
                            aSupportedValue,
                            aDefaultItems,
                            MapSettingItemToCrItemsKeyL( aSettingId ),
                            ETrue );
    PRINT( _L("Camera <= CCameraUiConfigManagerImp::LoadConfiguredSettingItemValuesFromCrL" ))                        
    }




//
// CCameraUiConfigManagerImp::AppendToSettingsArrayL
//
void CCameraUiConfigManagerImp::AppendToSettingsArrayL( const TInt aItemId,
                                                const TInt aSupportedValue,
                                                RArray<TDynamicSettingsData>& aAllItems,
                                                TUint32 aCrItemsKey,
                                                TBool aLoadSettingItems )
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::AppendToSettingsArrayL" ))  	
    TSupportedSettingsData* newSetting = new( ELeave ) TSupportedSettingsData;
    CleanupStack::PushL( newSetting );

    newSetting->iSettingId = aItemId; // setting id
    newSetting->iSupportedValue     = aSupportedValue; // feature support
    if ( aLoadSettingItems && aSupportedValue )
        {
        // all configured setting items from cr only if the feature is supported
        GetSupportedSettingIdsL( aAllItems,
                                 newSetting->iSupportedSettingItems,
                                 aCrItemsKey ); 
        }

    iSupportedSettings.AppendL( newSetting );
    CleanupStack::Pop( newSetting );
    PRINT( _L("Camera <= CCameraUiConfigManagerImp::AppendToSettingsArrayL" ))  	
    }



// ---------------------------------------------------------------------------
// CCameraUiConfigManager::LoadAllDynamicSettingL
// Load all dynamic settings
// ---------------------------------------------------------------------------
//
void CCameraUiConfigManagerImp::LoadDefaultSettingsDataL(
                                         const TInt aSettingId, 
                                         RArray<TDynamicSettingsData>& aDefaultItems )
    {
    PRINT1( _L("Camera => CCameraUiConfigManagerImp::LoadDefaultSettingsDataL setting id [%d]" ), aSettingId )
    aDefaultItems.Reset();
    TInt i = 0;
    const TInt* settingsId = LoadSettingEnumerationArray( aSettingId );
    const TUint16* const* stringArray = LoadSettingStringArray( aSettingId );
    
    if ( settingsId && stringArray ) 
        {
        while ( settingsId[i] != KCamLastSettingItem )
            {
            TDynamicSettingsData data;
            data.iItemId = settingsId[i];
            data.iText = stringArray[i];
            aDefaultItems.AppendL( data );
            i++;
            }
        }
    PRINT( _L("Camera <= CCameraUiConfigManagerImp::LoadDefaultSettingsDataL()" ) )    
    }



//
// CCameraUiConfigManagerImp::GetSupportedSettingIdsL
//
void CCameraUiConfigManagerImp::GetSupportedSettingIdsL(
                                             RArray<TDynamicSettingsData>& aAllItems,
                                             RArray<TInt>& aSupportedValues, 
                                             const TUint32 aCrKey )
    {
    PRINT( _L("Camera => CameraUiConfigManagerImp::GetSupportedSettingIdsL" ))
    aSupportedValues.Reset();
    
    HBufC* string; // not sure but does this need to be in cleanupstack since this can later leave?
    string = GetStringDataL( aCrKey );
    TBool clearstring = EFalse;
    
    TPtrC data = string->Des();
    TPtrC currentItem( KNullDesC );
    
    while ( data.Length() >  0 )
        {
        // Find the next instance of item separator    
        const TInt separatorIndex( data.Find( KItemsSeparator ) );
            
        if( KErrNotFound != separatorIndex )    
            {
            // There is more than one item in the string still
            // Get the current item - everything before separator
            currentItem.Set( data.Left( separatorIndex ) );
            // The remaining data is now everything after the separator
            data.Set( data.Right( data.Length() - separatorIndex - 1 ) );
            }
        else
            {
            // This is the last item in the string    
            currentItem.Set( data );
            data.Set( KNullDesC );
            clearstring = ETrue;
            }
        
        // Get the item id for the current item, and if found, append to list
        TInt settingItem;
        PRINT1( _L("Camera <> settingItem: %S"), string )
        if ( currentItem[0]-48 >= 0 && currentItem[0]-48 <= 9 ||
           ( currentItem[0] == '-' && // check for negative number too
             currentItem[1]-48 >= 0 && currentItem[1]-48 <= 9 ) )
            {
            // read the numeric value equivalent to the numeric string
            TLex lex;
            if( currentItem.Length() > 2 ) 
                {
                // if it is hex value, then extract hex value
                if( currentItem[1] == 'x' || currentItem[1] == 'X' )
                    {
                    lex = TLex( currentItem.Mid(2) );
                    TRadix radix = EHex;
                    TUint value;
                    lex.Val(value, radix);
                    settingItem = value;
                    }
                else  // extract integer
                    {
                    lex = TLex( currentItem );
                    lex.Val( settingItem );
                    }
                }            
             else // extract integer
                {
                lex = TLex( currentItem );
                lex.Val( settingItem );
                }
            }
        else
            {
            // not a numeric string, check for necessary enum equivalents.
            settingItem = SettingItemId( aAllItems, currentItem );
            }
        
        PRINT1( _L("Camera <> settingItem: %d"), settingItem )
        
        if( KErrNotFound != settingItem )
            {
            aSupportedValues.AppendL( settingItem ); // here
            }
        if( clearstring )
            {
            delete string;
            }
        }
    PRINT( _L("Camera <= CameraUiConfigManagerImp::GetSupportedSettingIdsL" ))
    }



//
// CCameraUiConfigManagerImp::SettingItemId
//
TInt CCameraUiConfigManagerImp::SettingItemId(
                                RArray<TDynamicSettingsData>& aPossibleSettings,
                                const TDesC& aItemString ) const
    {
    PRINT( _L("Camera => CameraUiConfigManagerImp::SettingItemId" ))
    for( TInt i = aPossibleSettings.Count()-1; i >= 0; i-- )
        {     
        if( aItemString == aPossibleSettings[i].iText )
            {
            PRINT( _L("Camera <= CameraUiConfigManagerImp::SettingItemId" ))
            return aPossibleSettings[i].iItemId;
            }  
        }    
    
    PRINT( _L("Camera <= CameraUiConfigManagerImp::SettingItemId" ))
    // No id found for the argument string    
    return KErrNotFound;
    }


//
// CCameraUiConfigManagerImp::GetStringDataL
//
HBufC* CCameraUiConfigManagerImp::GetStringDataL( const TUint32 aCrKey )
    {
    PRINT( _L("Camera => CameraUiConfigManagerImp::GetStringDataL" ))
    HBufC* string = HBufC::NewLC( KCamCRStringInitialLength );
    
    TPtr ptr    = string->Des();
    TInt size   = 0;
    TInt status = iRepository->Get( aCrKey, ptr, size );
		    	
    // Did not fit into the string, reserve more memory and try again
    if( KErrOverflow == status ) 
        {
        CleanupStack::PopAndDestroy( string ); // String
        string = HBufC::NewLC( size );
        ptr    = string->Des();
        status = iRepository->Get( aCrKey, ptr, size );
        }

    CleanupStack::Pop( string ); // string
    User::LeaveIfError( status );   
    
    PRINT( _L("Camera <= CameraUiConfigManagerImp::GetStringDataL" ))
    return string;
    }



// ---------------------------------------------------------------------------
// CCameraUiConfigManager::SearchInSettingsListFor
// Searches in a settings list for a particular setting item.
// ---------------------------------------------------------------------------
//
TInt CCameraUiConfigManagerImp::SearchInSettingsListFor( 
    const RPointerArray<TSupportedSettingsData>& aSettingsList, 
          TInt                        aSettingItem  ) const
  {
  return aSettingsList.Find( aSettingItem, SettingIdMatches );
  }


//
//CCameraUiConfigManagerImp::MapSettingItemToCrFeatureKeyL
//
TUint32 CCameraUiConfigManagerImp::MapSettingItemToCrFeatureKeyL( 
                                                const TInt aSettingItem ) const
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::MapSettingItemToCrFeatureKeyL" ))	     	
    TUint32 crKey( 0 );
    switch ( aSettingItem )
        {
        case ECamDynamicSettingRequiredRamMemory:
                crKey = KCamCrFeatureRequiredRamMemory;
                break;
        case ECamDynamicSettingCaptureToneDelaySupport:
                crKey = KCamCrFeatureCaptureToneDelaySupport;
                break;
        case ECamDynamicSettingCaptureToneDelayValue:
                crKey = KCamCrFeatureCaptureToneDelayValue;
                break;
        case ECamDynamicSettingCriticalLevelRamMemory:
                crKey = KCamCrFeatureCriticalLevelRamMemory;
                break;
        case ECamDynamicSettingRequiredRamMemoryFocusGained:
                crKey = KCamCrFeatureRequiredRamMemoryFocusGained;
                break;
        case ECamDynamicSettingCriticalLevelRamMemoryFocusGained:
                crKey = KCamCrFeatureCriticalLevelRamMemoryFocusGained;
                break;
        case ECamDynamicSettingPhoto:
                crKey = KCamCrFeatureKeyPhotos;
                break;
        case ECamDynamicSettingPublishZoomState:
                crKey = KCamCrFeatureKeyPublishZoomSate;
                break;
        case ECamDynamicSettingAutofocusSetInHyperfocalAtStartup:
                crKey = KCamCrFeatureKeyAFSetInHyperfocalAtStartup;
                break;
        case ECamDynamicSettingExtDigitalZoom:
                crKey = KCamCrFeatureExtDigitalZoom;
                break;
        case ECamDynamicSettingLocation:
                crKey = KCamCrFeatureLocation;
                break;
        case ECamDynamicSettingKeyLockWatcher:
                crKey = KCamCrFeatureKeyLockWatcher;
                break;
        case ECamDynamicSettingColorTone:
                crKey = KCamCrFeatureColorTone;
                break;
        case ECamDynamicSettingSharpness:
                crKey = KCamCrFeatureSharpness;
                break;
        case ECamDynamicSettingWhiteBalance:
                crKey = KCamCrFeatureWhiteBalance;
                break;
        case ECamDynamicSettingSecondaryCamera:
                crKey = KCamCrFeatureSecondaryCamera;
                break;
        case ECamDynamicSettingLensCover:
                crKey = KCamCrFeatureLensCover;
                break;
        case ECamDynamicSettingXenonFlash:
                crKey = KCamCrFeatureXenonFlash;
                break;
        case ECamDynamicSettingOneClickUpload:
                crKey = KCamCrFeatureOneClickUpLoad;
                break;
        case ECamDynamicSettingVideoLight:
                crKey = KCamCrFeatureVideoLight;
                break;
        case ECamDynamicSettingAutoFocus:
                crKey = KCamCrFeatureAutoFocus;
                break;
        case ECamDynamicSettingExtLightSensitivity:
                crKey = KCamCrFeatureExtLightSensitivity;
                break;
        case ECamDynamicSettingContrast:
                crKey = KCamCrFeatureContrast;
                break;
        case ECamDynamicSettingBrightness:
                crKey = KCamCrFeatureBrightness;
                break;
        case ECamDynamicSettingEV:
                crKey = KCamCrFeatureEV;
                break;
        case ECamDynamicSettingFlashMode:
                crKey = KCamCrFeatureFlashMode;
                break;
        case ECamDynamicSettingLightSensitivity: 
                crKey = KCamCrFeatureLightSensitivity;
                break;
        case ECamDynamicSettingVideoStabilization: 
                crKey = KCamCrFeatureVideoStabilization;
                break;
        case ECamDynamicSettingOrientationSensor: 
                crKey = KCamCrFeatureOrientationSensor;
                break;
        case ECamDynamicSettingFaceTracking:
                crKey = KCamCrFeatureFaceTracking;
                break;
        case ECamDynamicSettingOpticalJoyStick: 
                crKey = KCamCrFeatureOpticalJoyStick;
                break;
		case ECamDynamicSetting2ndCameraOrientation: 
                crKey = KCamCrFeature2ndCameraOrientation;  
                break;		
        case ECamDynamicSettingUIOrientationOverride: 
                crKey = KCamCrFeatureUIOrientationOverride;
                break;
        case ECamDynamicSettingThumbnailManagerAPI:
                crKey = KCamCrFeatureThumbnailManagerAPI;
                break;
        case ECamDynamicSettingBurstFileSizeEstimateFactor:
                crKey = KCamCrFeatureBurstFileSizeEstimateFactor;
                break;				
        case ECamDynamicSettingBurstMode: 
                crKey = KCamCrFeatureBurstMode;
                break;  
	    case ECamDynamicSettingContinuousAF:
                crKey = KCamCrFeatureContinuousAF;   
                break;			        
        default:
            PRINT( _L("Camera <> CCameraUiConfigManagerImp::MapSettingItemToCrFeatureKeyL, leave!!!" ) )	
            User::Leave( KErrNotSupported );
        }
        PRINT( _L("Camera <= CCameraUiConfigManagerImp::MapSettingItemToCrFeatureKeyL" ) )
    return crKey;
    }



//
//CCameraUiConfigManagerImp::MapSettingItemToCrKeyL
//
TUint32
CCameraUiConfigManagerImp::MapSettingItemToCrItemsKeyL( const TInt aSettingItem ) const
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::MapSettingItemToCrItemsKeyL" ))	
    TUint32 crKey( 0 );
    switch ( aSettingItem )
        {
        case ECamDynamicSettingVideoSceneModeNormal:
                crKey = KCamCrFeatureVideoSceneNormal;
                break;                
        case ECamDynamicSettingVideoSceneModeNight:
                crKey = KCamCrFeatureVideoSceneModeNight;
                break;
        case ECamDynamicSettingVideoSceneModeLowLight:
                crKey = KCamCrFeatureVideoSceneModeLowLight;
                break;
        case ECamDynamicSettingDSAViewFinder:
                crKey = KCamCrFeatureDSAViewFinder;
                break;
        case ECamDynamicSettingImageSceneModeAuto:
                crKey = KCamCrFeatureImageSceneModeAuto;
                break;
        case ECamDynamicSettingImageSceneModeUser:
                crKey = KCamCrFeatureImageSceneModeUser;
                break;
        case ECamDynamicSettingImageSceneModeMacro:
                crKey = KCamCrFeatureImageSceneModeMacro;
                break;
        case ECamDynamicSettingImageSceneModePotrait:
                crKey = KCamCrFeatureImageSceneModePotrait;
                break;
        case ECamDynamicSettingImageSceneModeScenery:
                crKey = KCamCrFeatureImageSceneModeScenery;
                break;
        case ECamDynamicSettingImageSceneModeSports:
                crKey = KCamCrFeatureImageSceneModeSports;
                break;
        case ECamDynamicSettingImageSceneModeNight:
                crKey = KCamCrFeatureImageSceneModeNight;
                break;
        case ECamDynamicSettingImageSceneModeNightPotrait:
                crKey = KCamCrFeatureImageSceneModeNightPotrait;
                break;
        case ECamDynamicSettingPCCaptureKeys:
                crKey = KCamCrFeaturePCCaptureKeys;
                break;
        case ECamDynamicSettingSCCaptureKeys:
                crKey = KCamCrFeatureSCCaptureKeys;
                break;
        case ECamDynamicSettingPCAutoFocusKeys:
                crKey = KCamCrFeaturePCAutoFocusKeys;
                break;
        case ECamDynamicSettingSCAutoFocusKeys:
                crKey = KCamCrFeatureSCAutoFocusKeys;
                break;
        case ECamDynamicSettingColorTone:
                crKey = KCamCrFeatureColorToneItems;
                break;
        case ECamDynamicSettingWhiteBalance:
                crKey = KCamCrFeatureWhiteBalanceItems;
                break;
        case ECamDynamicSettingPCZoomIn:
                crKey = KCamCrFeaturePCZoomInItems;
                break;
        case ECamDynamicSettingPCZoomOut:
                crKey = KCamCrFeaturePCZoomOutItems;
                break;
        case ECamDynamicSettingSCZoomIn:
                crKey = KCamCrFeatureSCZoomInItems;
                break;
        case ECamDynamicSettingSCZoomOut:
                crKey = KCamCrFeatureSCZoomOutItems;
                break;
        case ECamDynamicSettingEV:
                crKey = KCamCrFeatureEVItems;
                break;
        case ECamDynamicSettingSharpness:
                crKey = KCamCrFeatureSharpnessItems;
                break;
        case ECamDynamicSettingFlashMode:
                crKey = KCamCrFeatureFlashModeItems;
                break;
        case ECamDynamicSettingMaxDigitalZoomLimits:
                crKey = KCamCrFeatureMaxDigitalZoomLimits;
                break;
        case ECamDynamicSettingMaxExtendedZoomLimits:
                crKey = KCamCrFeatureMaxExtendedZoomLimits;
                break;
        case ECamDynamicSettingMaxOpticalZoomLimits:
                crKey = KCamCrFeatureMaxOpticalZoomLimits;
                break;
        case ECamDynamicSettingZoomLAF:
                crKey = KCamCrFeatureZoomLAF;
                break;
        case ECamDynamicSettingContrast:
                crKey = KCamCrFeatureContrastItems;
                break;
        case ECamDynamicSettingBrightness:
                crKey = KCamCrFeatureBrightnessItems;
                break;
        case ECamDynamicSettingZoomDelays:
                crKey = KCamCrFeatureZoomDelays;
                break;
        default:
            PRINT( _L("Camera <> CCameraUiConfigManagerImp::MapSettingItemToCrItemsKeyL, leave!!!" ) )
            User::Leave( KErrNotSupported );
        }
    PRINT( _L("Camera <= CCameraUiConfigManagerImp::MapSettingItemToCrItemsKeyL" ) )
    return crKey;
    }


//
//CCameraUiConfigManagerImp::LoadCrForFeatureSupport
//
TBool CCameraUiConfigManagerImp::LoadCrForFeatureSupport( const TInt aSettingId ) const
    {
    TBool loadFromCr( ETrue );
    
    switch ( aSettingId )
        {
        case ECamDynamicSettingDSAViewFinder:
        case ECamDynamicSettingPCCaptureKeys:
        case ECamDynamicSettingSCCaptureKeys:
        case ECamDynamicSettingPCAutoFocusKeys:
        case ECamDynamicSettingSCAutoFocusKeys:
        case ECamDynamicSettingPCZoomIn:
        case ECamDynamicSettingPCZoomOut:
        case ECamDynamicSettingSCZoomIn:
        case ECamDynamicSettingSCZoomOut:
        case ECamDynamicSettingMaxDigitalZoomLimits:
        case ECamDynamicSettingMaxExtendedZoomLimits:
        case ECamDynamicSettingMaxOpticalZoomLimits:
        case ECamDynamicSettingZoomLAF:
        case ECamDynamicSettingImageSceneModeAuto:
        case ECamDynamicSettingImageSceneModeUser:
        case ECamDynamicSettingImageSceneModeMacro:
        case ECamDynamicSettingImageSceneModePotrait:
        case ECamDynamicSettingImageSceneModeScenery:
        case ECamDynamicSettingImageSceneModeSports:
        case ECamDynamicSettingImageSceneModeNight:
        case ECamDynamicSettingImageSceneModeNightPotrait:
        case ECamDynamicSettingVideoSceneModeNormal:
        case ECamDynamicSettingVideoSceneModeNight:
        case ECamDynamicSettingVideoSceneModeLowLight:
        case ECamDynamicSettingZoomDelays:
             loadFromCr = EFalse;
             break;
        default:
             break;
        }
    return loadFromCr;
    }

//
//CCameraUiConfigManagerImp::LoadSettingEnumerationArray
//
const TInt* CCameraUiConfigManagerImp::LoadSettingEnumerationArray( const TInt aSettingItem )
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::LoadSettingEnumerationArray" ) )	
    const TInt* enumArray = NULL;
    
    switch ( aSettingItem )
        {
        case ECamDynamicSettingWhiteBalance:
             enumArray = KCamWhiteBalanceEnums;
             break;
        case ECamDynamicSettingColorTone:
             enumArray = KCamColorToneEnums;
             break;
        case ECamDynamicSettingFlashMode:
             enumArray = KCamFlashModeEnums;
             break;
        case ECamDynamicSettingImageSceneModeAuto:
        case ECamDynamicSettingImageSceneModeUser:
        case ECamDynamicSettingImageSceneModeMacro:
        case ECamDynamicSettingImageSceneModePotrait:
        case ECamDynamicSettingImageSceneModeScenery:
        case ECamDynamicSettingImageSceneModeSports:
        case ECamDynamicSettingImageSceneModeNight:
        case ECamDynamicSettingImageSceneModeNightPotrait:
        case ECamDynamicSettingVideoSceneModeNormal:
        case ECamDynamicSettingVideoSceneModeNight:
        case ECamDynamicSettingVideoSceneModeLowLight:
            enumArray = KCamSceneModeEnums;
            break;             
        default:
             PRINT( _L("Camera <> CCameraUiConfigManagerImp::LoadSettingEnumerationArray, default!!!" ) )
             break;
        }
    PRINT( _L("Camera <= CCameraUiConfigManagerImp::LoadSettingEnumerationArray" ) )
    return enumArray;
    }
    

//
//CCameraUiConfigManagerImp::LoadSettingStringArray
//
const TUint16* const* CCameraUiConfigManagerImp::LoadSettingStringArray( 
                                                const TInt aSettingItem )
    {
    PRINT( _L("Camera => CCameraUiConfigManagerImp::LoadSettingStringArray()" ) )
    const TUint16* const* stringArray = NULL;
    
    switch ( aSettingItem )
        {
        case ECamDynamicSettingWhiteBalance:
             stringArray = KCamWhiteBalanceStrings;
             break;
        case ECamDynamicSettingColorTone:
             stringArray = KCamColorToneStrings;
             break;
        case ECamDynamicSettingFlashMode:
             stringArray = KCamFlashModeStrings;
             break;
        case ECamDynamicSettingImageSceneModeAuto:
        case ECamDynamicSettingImageSceneModeUser:
        case ECamDynamicSettingImageSceneModeMacro:
        case ECamDynamicSettingImageSceneModePotrait:
        case ECamDynamicSettingImageSceneModeScenery:
        case ECamDynamicSettingImageSceneModeSports:
        case ECamDynamicSettingImageSceneModeNight:
        case ECamDynamicSettingImageSceneModeNightPotrait:
        case ECamDynamicSettingVideoSceneModeNormal:
        case ECamDynamicSettingVideoSceneModeNight:
        case ECamDynamicSettingVideoSceneModeLowLight:
             stringArray = KCamSceneModeStrings;
             break;
        default:
             break;
        }
    PRINT( _L("Camera <= CCameraUiConfigManagerImp::LoadSettingStringArray()" ) )
    return stringArray;
    }


/*
* Organizing CenRep data to a definite structure for scene modes.
*/
void CCameraUiConfigManagerImp::OrganiseSceneSettings( 
                                            RArray<TSceneSettings>& aTargetArray,
                                            RArray<TInt>& aSceneSettingsArray,
                                            const TBool aPhotoScene )
    {
    if ( aSceneSettingsArray.Count() > 0 )
        {
        TSceneSettings scene;
        TInt index = 0;
        // The following is the order how we read values from CenRep
        // each time for each specific scene mode in Image & Video case.
        scene.iSupported                  = aSceneSettingsArray[index++];
        scene.iIdentifier                 = aSceneSettingsArray[index++];
        scene.iExposureMode               = aSceneSettingsArray[index++];
        scene.iWhiteBalance               = aSceneSettingsArray[index++];
        scene.iFlash                      = aSceneSettingsArray[index++];
        scene.iContrastMode               = aSceneSettingsArray[index++];
        scene.iFocalDistance              = aSceneSettingsArray[index++];
        
        if ( aPhotoScene )
            {
            // The following is the order how we read values from CenRep
            // each time for each specific scene mode in Image case.
            // Note: In image scene, we ignore iFrameRate as this wont be configured
            //       for any of the image scenes
            scene.iSharpnessMode              = aSceneSettingsArray[index++];
            scene.iLightSensitivityMode       = aSceneSettingsArray[index++];
            scene.iExposureCompensationValue  = aSceneSettingsArray[index++];            
            }
        else
            {
            // The following is the order how we read values from CenRep
            // each time for each specific scene mode in Video scene case.
            // Note: In image scene, we ignore iFrameRate as this wont be configured
            //       for any of the image scenes
            scene.iFrameRate                  = aSceneSettingsArray[index++];
            }
        aTargetArray.Append( scene );
        }
    }
// End of file
