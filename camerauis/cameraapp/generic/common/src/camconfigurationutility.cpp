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
* Description:  Helper class for CCamConfiguration
*
*/


#include "camlogging.h"
#include "camconfigurationutility.h"
#include "camconfigurationconstants.h"

// ---------------------------------------------------------------------------
// CamConfigurationUtility::PhotoSizeIndex
// ---------------------------------------------------------------------------
//
TInt
CamConfigurationUtility::PhotoSizeIndex( TInt aWidth, TInt aHeight )
  {
  TInt index = KErrNotFound;
  for( TInt i=0; i<ECamPhotoSizeLast; i++ )
    {
    if( KCamPhotoSizes[i].iWidth == aWidth &&
        KCamPhotoSizes[i].iHeight == aHeight )
      {   
      index = i;
      break;
      }
    }
  
  return index;  
  } 
  
// ---------------------------------------------------------------------------
// CamConfigurationUtility::VideoSizeIndex
// ---------------------------------------------------------------------------
//  
TInt
CamConfigurationUtility::VideoSizeIndex( TInt aWidth, TInt aHeight )
  {
  TInt index = KErrNotFound;
  for( TInt i=0; i<ECamVideoResolutionLast; i++ )
    {
    if( KCamVideoSizes[i].iWidth == aWidth &&
        KCamVideoSizes[i].iHeight == aHeight )
      {   
      index = i;
      break;
      }
    }
  
  return index;  
  }   
  
 
// ---------------------------------------------------------------------------
// CamConfigurationUtility::AdjustPrintQualities
// ---------------------------------------------------------------------------
//
void
CamConfigurationUtility::AdjustPrintQualities( RArray<TCamImageQualityData>& aPhotoQualities )  
  {
  TInt n = aPhotoQualities.Count();
    
  //TInt numPrint = 0;
  //TInt lastPrintIndex = KErrNotFound;
  
  for( int i=0; i<n; i++ )
    {
    TCamImageQuality& quality( aPhotoQualities[i].iPhotoQualitySetting.iPhotoQualityId );
    
    quality = static_cast<TCamImageQuality>( i );
    }
    
    /*
    if( EImageQualityPrint == quality )
      {        
      if( !numPrint )
        {
        // This is the first print quality, so it should be print high
        quality = EImageQualityPrintHigh;        
        }
                     
      numPrint++;
      lastPrintIndex = i;          
      }          
    }  
    
  if( numPrint > 2 )
    {
    // There ECamQualityPrintHigh and more than one EImageQualityPrint
    // Change last of them to ECamImageQualityPrintLow
    aPhotoQualities[lastPrintIndex].iPhotoQualitySetting.iPhotoQualityId
          = EImageQualityPrintLow;  
    }  
      
  */         
  }
  
// ---------------------------------------------------------------------------
// CamConfigurationUtility::SetVideoQualityLevels
// ---------------------------------------------------------------------------
// 
void
CamConfigurationUtility::SetVideoQualityLevels( RArray<TCamVideoQualityData>& aOrderedArray )
  {
  TInt n = aOrderedArray.Count();
  
  // Set quality setting values for each quality
  // Only 3 or 5 qualities are supported
  if( n==3 )
    {  
    aOrderedArray[0].iQualitySetting = ECamVideoQualityHigh;
    aOrderedArray[1].iQualitySetting = ECamVideoQualityNormalHigh;  
    aOrderedArray[2].iQualitySetting = ECamVideoQualitySharing;    
    aOrderedArray[2].iQualitySettings.iVideoLength = ECamVideoClipShort;
    }
  else
  if( n==5 )
    {
    aOrderedArray[0].iQualitySetting = ECamVideoQualityHigh;
    aOrderedArray[1].iQualitySetting = ECamVideoQualityNormalHigh; 
    aOrderedArray[2].iQualitySetting = ECamVideoQualityNormal;  
    aOrderedArray[3].iQualitySetting = ECamVideoQualityNormalLow; 
    aOrderedArray[4].iQualitySetting = ECamVideoQualitySharing;        
    aOrderedArray[4].iQualitySettings.iVideoLength = ECamVideoClipShort;    
    }
  else
    {
    PRINT1( _L("Camera <> CamConfigurationUtility::SetVideoQualityLevels - Invalid number of levels (%d)"), n ); 
    User::Panic( KCamConfigPanicId, ECamConfigPanicInvalidNumberOfQualities );    
    }  
  
  // Set quality level for iQualitySettings. This is just the 
  // quality's index in the array.
  for( TInt i=0; i<n; i++ )
    {
    aOrderedArray[i].iQualitySettings.iQualityLevel = i;  
    }
    
  }  
  
// ---------------------------------------------------------------------------
// CamConfigurationUtility::AddToArrayL
// ---------------------------------------------------------------------------
//  
void
CamConfigurationUtility::AddToArrayL( TInt aValue, RArray<TInt>& aArray )
  {
  if( aArray.Find( aValue ) == KErrNotFound )
    {
    aArray.AppendL( aValue );  
    }    
  }

// ---------------------------------------------------------------------------
// CamConfigurationUtility::VideoResolution
// ---------------------------------------------------------------------------
//   
TInt 
CamConfigurationUtility::VideoResolution( TInt aWidth, TInt aHeight )
  {
  RDebug::Print( _L("VideoResolution( %d, %d) "), aWidth, aHeight );
  for( TInt i=0; i<ECamVideoResolutionLast; i++ )
    {
    if( KCamVideoSizes[i].iWidth == aWidth &&
        KCamVideoSizes[i].iHeight == aHeight )
      {   
      return KCamVideoSizes[i].iVideoRes;
      }
    }
  
  User::Panic( KCamConfigPanicId, ECamConfigPanicNoVideoRes );
  return KErrNotSupported;
  }

// ---------------------------------------------------------------------------
// CamConfigurationUtility::CamVideoResolution
// ---------------------------------------------------------------------------
//  
TInt 
CamConfigurationUtility::CamVideoResolution( TInt aWidth, TInt aHeight )
  {
  RDebug::Print( _L("VideoResolution( %d, %d) "), aWidth, aHeight );
  for( TInt i=0; i<ECamVideoResolutionLast; i++ )
    {
    if( KCamVideoSizes[i].iWidth == aWidth &&
        KCamVideoSizes[i].iHeight == aHeight )
      {   
      return KCamVideoSizes[i].iCamVideoRes;
      }
    }
  
  User::Panic( KCamConfigPanicId, ECamConfigPanicNoVideoRes );
  return KErrNotSupported;
  } 

// ---------------------------------------------------------------------------
// CamConfigurationUtility::CamVideoResolution
// ---------------------------------------------------------------------------
//  
TSize 
CamConfigurationUtility::CamVideoResolution( TCamVideoResolution aResolution )
  {
  RDebug::Print( _L("VideoResolution( TCamVideoResolution: %d) "), aResolution );
  for( TInt i=0; i<ECamVideoResolutionLast; i++ )
    {
    if( KCamVideoSizes[i].iCamVideoRes == aResolution )
      {   
      return TSize(KCamVideoSizes[i].iWidth, KCamVideoSizes[i].iHeight );
      }
    }
  
  User::Panic( KCamConfigPanicId, ECamConfigPanicNoVideoRes );
  return TSize(0,0);//KErrNotSupported;
  } 

// ---------------------------------------------------------------------------
// CamConfigurationUtility::VideoType
// ---------------------------------------------------------------------------
//   
TInt 
CamConfigurationUtility::VideoType( const TText8* aMimeType )
  {
  const TPtrC8 mimeType( aMimeType );
  if( mimeType == KCamMimeMp4 )
    {
    return ECamVideoMpeg4;  
    }
  else if( mimeType == KCamMime3gpp )
    {
    return ECamVideoH263;  
    }
  
   User::Panic( KCamConfigPanicId, ECamConfigPanicNoVideoType );
   return KErrNotSupported;
  }


// ---------------------------------------------------------------------------
// CamConfigurationUtility::VideoDescId
// ---------------------------------------------------------------------------
//
TCamVideoQualityDescription
CamConfigurationUtility::VideoDescId( TInt aQualityValue )
  {
  TCamVideoQualityDescription desc = ECamVideoQualityDescSharing;
  
  for( TInt i=0; i<ECamVideoQualityDescLast; i++ )
    {
    if( aQualityValue >= KCamVideoQualityMappings[i].iCamcorderVisibleMin )
      {
      desc = KCamVideoQualityMappings[i].iDesc;
      break;  
      }
    } 
    
  return desc;  
  }
    
// ---------------------------------------------------------------------------
// CamConfigurationUtility::ImageType
// ---------------------------------------------------------------------------
//    
TCamImageType
CamConfigurationUtility::ImageType( TInt aValue )
  {
  if( aValue >= KCamImageLargeTypeMin )
    {
    return ECamImageTypeLarge;
    }
  else if( aValue >= KCamImageMediumTypeMin )
    {
    return ECamImageTypeMedium;
    }
  else
    {
    return ECamImageTypeSmall;  
    }  
  }
 
// ---------------------------------------------------------------------------
// CamConfigurationUtility::IsSupported
// ---------------------------------------------------------------------------
//   
TBool 
CamConfigurationUtility::IsSupported( TInt aValue, TCamItemType aItemType, const TCamImageQualityData& aData )
  {
    switch( aItemType )
    {
      case ECamItemPhotoSize:
        {        
        if( aValue == aData.iPhotoSize.iSizeId )
          {
          return ETrue;  
          }        
        break;  
        }
      case ECamItemPhotoQuality:
        {
        if( aValue == aData.iPhotoSize.iQualitySettingId )
          {
          return ETrue;  
          }
        break;  
        }
      default:
        {
        User::Panic( KCamConfigPanicId, ECamConfigPanicInvalidItem );   
        }  
    }
  return EFalse;
  } 




// ---------------------------------------------------------------------------
// CamConfigurationUtility::MapDynamicSettingToSceneSetting
// Get the scene setting id from dynamic setting id.
// ---------------------------------------------------------------------------
//
TInt
CamConfigurationUtility::MapDynamicSettingToSceneSetting( TInt aSettingId )
  {
  switch( aSettingId )
    {
    case ECamSettingItemDynamicVideoWhiteBalance:     // fall through
    case ECamSettingItemDynamicPhotoWhiteBalance:     
        return ECamSettingItemSceneAWBMode;

    case ECamSettingItemDynamicVideoExposure:         // fall through
    case ECamSettingItemDynamicPhotoExposure:         
        return ECamSettingItemSceneEV;

    case ECamSettingItemDynamicVideoFlash:            // fall through
    case ECamSettingItemDynamicPhotoFlash:            
        return ECamSettingItemSceneFlashMode;

    case ECamSettingItemDynamicVideoContrast:         // fall through
    case ECamSettingItemDynamicPhotoContrast:         
        return ECamSettingItemSceneContrast;

    case ECamSettingItemDynamicPhotoImageSharpness:   
        return ECamSettingItemSceneSharpness;
    
    case ECamSettingItemDynamicVideoLightSensitivity: // fall through
    case ECamSettingItemDynamicPhotoLightSensitivity: 
        return ECamSettingItemSceneISO;

    default:                                          
        return KErrNotFound;
    }
  }

// End of file 
 
