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
* Description:  Implementation of Dynamic Configuration
*
*/


#include <e32debug.h> // RDebug
#include "camconfiguration.h"
#include "camconfigurationutility.h"
#include "imagingconfigmanager.h"
#include "CamUtility.h"
#include "camconfigurationconstants.h"

const TInt KReserveedSpace = 160000;

// ---------------------------------------------------------------------------
// CCamConfiguration::CCamConfiguration
// ---------------------------------------------------------------------------
//
CCamConfiguration::CCamConfiguration():
  iPrimaryCameraDisplayId( KCamPrimaryCameraDisplayId ), 
  iSecondaryCameraDisplayId( KCamSecondaryCameraDisplayId )
  {
  } 

// ---------------------------------------------------------------------------
// CCamConfiguration::~CCamConfiguration
// ---------------------------------------------------------------------------
//  
CCamConfiguration::~CCamConfiguration()
  {
  // Quality data arrays  
  iPrimaryCameraPhotoQualities.Close();
  iSecondaryCameraPhotoQualities.Close();
  iVideoQualities.Close();
  
  delete iIcm;  
  
  // String data
  for( TInt i=iStrings8.Count()-1; i>=0; i-- )
    {
    HBufC8* str = iStrings8[i];
    iStrings8.Remove( i );
    delete str;
    }
  iStrings8.Close();    
  for( TInt i=iStrings16.Count()-1; i>=0; i-- )
    {
    HBufC16* str = iStrings16[i];
    iStrings16.Remove( i );
    delete str;
    }    
  iStrings16.Close();   
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::NewL
// ---------------------------------------------------------------------------
//
CCamConfiguration*  
CCamConfiguration::NewL()
  {
  CCamConfiguration* self = new (ELeave) CCamConfiguration;
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );
  return self; 
  }  
  
// ---------------------------------------------------------------------------
// CCamConfiguration::ConstructL
// ---------------------------------------------------------------------------
//  
void
CCamConfiguration::ConstructL()
  {
  PRINT( _L("Camera => CCamConfiguration::ConstructL") );  
  
  PRINT( _L("Camera <> CCamConfiguration::ConstructL - Instantiate ICM") );  
  iIcm = CImagingConfigManager::NewL();  

  PRINT( _L("Camera <> CCamConfiguration::ConstructL - Init image qualities") );  
  InitializeImageQualitiesL();
  
  PRINT( _L("Camera <> CCamConfiguration::ConstructL - Init video qualities") );  
  InitializeVideoQualitiesL();
  
  PRINT( _L("Camera <= CCamConfiguration::ConstructL") );  
  }  
  
// ---------------------------------------------------------------------------
// CCamConfiguration::InitializeImageQualitiesL
// ---------------------------------------------------------------------------
//  
void 
CCamConfiguration::InitializeImageQualitiesL()
  {
  PRINT( _L("Camera => CCamConfiguration::InitializeImageQualitiesL") ); 
  TInt totalLevels = iIcm->NumberOfImageQualityLevels();  
  
  CArrayFixFlat<TUint>* levels 
    = new (ELeave) CArrayFixFlat<TUint>( totalLevels );
  CleanupStack::PushL( levels );
  TImageQualitySet set;
  TCamImageQualityData data;
  
  // Get primary camera levels  
  if( iPrimaryCameraDisplayId )
    {
    PRINT( _L("Camera <> CCamConfiguration::InitializeImageQualitiesL *** Primary camera qualities ***") ); 
    iIcm->GetImageQualityLevelsL( *levels, iPrimaryCameraDisplayId );  
    TInt numLevels( levels->Count() );
    PRINT1(_L("Camera <> CCamConfiguration::InitializeImageQualitiesL - Total number of levels: %d"), numLevels );
    
    for( int i=0; i<numLevels; i++ )
      {
      iIcm->GetImageQualitySet( set, levels->At( i ), iPrimaryCameraDisplayId );
      if( set.iCamcorderVisible > 0 )
        {
        PRINT1( _L("Camera <> CCamConfiguration::InitializeImageQualitiesL - Set %d CamcorderVisible"), i );
        
        InitializeImageQualityL( set, data );
        AddToOrderedImageQualitiesArrayL( data, iPrimaryCameraPhotoQualities );
        }
      }
    }
    
  // Adjust print qualities - print => print high/print/print low
  CamConfigurationUtility::AdjustPrintQualities( iPrimaryCameraPhotoQualities );  
  TInt numQualities = iPrimaryCameraPhotoQualities.Count();  
    
  // Get secondary camera levels
  if( iSecondaryCameraDisplayId )
    {
    PRINT( _L("Camera <> CCamConfiguration::InitializeImageQualitiesL() *** Secondary camera qualities ***") );   
    iIcm->GetImageQualityLevelsL( *levels, iSecondaryCameraDisplayId );  
    TInt numLevels( levels->Count() );
    PRINT1( _L("Camera <> CCamConfiguration::InitializeImageQualitiesL - Total number of levels: %d"), numLevels );
    
    for( int i=0; i<numLevels; i++ )
      {        
      iIcm->GetImageQualitySet( set, levels->At( i ), iSecondaryCameraDisplayId );
      if( set.iCamcorderVisible > 0 )
        {              
        PRINT1( _L("Camera <> CCamConfiguration::InitializeImageQualitiesL - Set %d CamcorderVisible"), i );
        
        InitializeImageQualityL( set, data );  
        
        // Force image quality to secondary
        // data.iPhotoQualitySetting.iPhotoQualityId = EImageQualitySecondary;
        data.iPhotoQualitySetting.iPhotoQualityId 
            = static_cast<TCamImageQuality>( numQualities++ );
        data.iPhotoSize.iQualitySettingId = ECamImageQualitySecondary;
                      
        AddToOrderedImageQualitiesArrayL( data, iSecondaryCameraPhotoQualities );                      
        }
      }
    }  
            
  CleanupStack::PopAndDestroy( levels );
  
  PRINT( _L("Camera <= CCamConfiguration::InitializeImageQualitiesL") );     
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::InitializeVideoQualitiesL
// ---------------------------------------------------------------------------
//    
void
CCamConfiguration::InitializeVideoQualitiesL()
  {
  PRINT(_L("Camera => CCamConfiguration::InitializeVideoQualitiesL") );  
  TInt totalLevels = iIcm->NumberOfVideoQualityLevels();
  CArrayFixFlat<TUint>* levels 
    = new (ELeave) CArrayFixFlat<TUint>( totalLevels );
  CleanupStack::PushL( levels );
  TVideoQualitySet set;
  
  // Get primary camera levels  
  if( iPrimaryCameraDisplayId )
    {    
    iIcm->GetVideoQualityLevelsL( *levels, iPrimaryCameraDisplayId );  
    TInt numLevels( levels->Count() );
    PRINT1(_L("Camera <> CCamConfiguration::InitializeVideoQualitiesL - Total number of levels: %d"), numLevels );
    
    for( int i=0; i<numLevels; i++ )
      {
      iIcm->GetVideoQualitySet( set, levels->At( i ), iPrimaryCameraDisplayId );
      if( set.iCamcorderVisible > 0 )
        {      
        PRINT1(_L("Camera <> CCamConfiguration::InitializeVideoQualitiesL - Set %d CamcorderVisible"), i );
        
        TCamVideoQualityData data( CreateVideoQualityDataL( set ) );        
        AddToOrderedVideoQualitiesArrayL( data, iVideoQualities );                        
        }
      }
        
    // Set the video quality values now that we have all data in sorted array
    CamConfigurationUtility::SetVideoQualityLevels( iVideoQualities );  
    }

  // Secondary camera video qualities from ICM are ignored.
  // The last primary camera quality is used in seconcary camera as well.
  
  CleanupStack::PopAndDestroy( levels );

  // Get the average video bitrate scaler
  TCamcorderMMFPluginSettings mmfPluginSettings;
  User::LeaveIfError( iIcm->GetCamcorderMMFPluginSettings( mmfPluginSettings ) );
  iCMRAvgVideoBitRateScaler = mmfPluginSettings.iCMRAvgVideoBitRateScaler;
                            
  PRINT( _L("Camera <= CCamConfiguration::InitializeVideoQualitiesL") );      
  }  
  

// ---------------------------------------------------------------------------
// CCamConfiguration::InitializeVideoQualitiesL
// ---------------------------------------------------------------------------
//    
void 
CCamConfiguration::AddToOrderedImageQualitiesArrayL( TCamImageQualityData& aQuality, 
                                                     RArray<TCamImageQualityData>& aArray )
  {
  TBool added = EFalse;
  TInt numQualities = aArray.Count();
  TInt numPixels = aQuality.iPhotoSize.iWidth*aQuality.iPhotoSize.iHeight;
  
  for( TInt i=0; i<numQualities; i++ )
    {
      if( aArray[i].iPhotoSize.iWidth*aArray[i].iPhotoSize.iHeight < numPixels )
        {
        aArray.InsertL( aQuality, i ); 
        added = ETrue;
        break;      
        }
    } 
    
  if( !added )
    {
    // Item is to be last in the array
    aArray.AppendL( aQuality );
    } 
  }    

// ---------------------------------------------------------------------------
// CCamConfiguration::AddToOrderedVideoQualitiesArrayL
// ---------------------------------------------------------------------------
//  
void 
CCamConfiguration::AddToOrderedVideoQualitiesArrayL( TCamVideoQualityData& aQuality,
                                                     RArray<TCamVideoQualityData>& aArray )
  {
  TBool added( EFalse );
  const TInt numQualities( aArray.Count() );
  const TInt currValue( aQuality.iDescription );
  
  for( TInt i=0; i<numQualities; i++ )
    {
      if( aArray[i].iDescription > currValue )
        {
        // aQuality should be before quality aArray[i]  
        aArray.InsertL( aQuality, i ); 
        added = ETrue;
        break;      
        }
    } 
    
  if( !added )
    {
    // Item is to be last in the array
    aArray.AppendL( aQuality );
    }     
  }
 
// ---------------------------------------------------------------------------
// CCamConfiguration::InitializeImageQualityL
// ---------------------------------------------------------------------------
//   
void
CCamConfiguration::InitializeImageQualityL( const TImageQualitySet& aSet, TCamImageQualityData& aData )
  {
    PRINT3( _L("Camera => CCamConfiguration::InitializeImageQualityL - res: (%d, %d), compression: %d" ), 
                                          aSet.iImageWidth, aSet.iImageHeight, aSet.iCompressionQuality );   
  // Find out the image quality/resolution id for the set
  TInt index = KErrNotFound;
  for( int i=0; i<ECamPhotoSizeLast; i++ )
    {      
    if( aSet.iImageWidth == KCamPhotoSizes[i].iWidth &&
        aSet.iImageHeight == KCamPhotoSizes[i].iHeight )
      {
      index = i;
      }
    }
    
  // Check that index is valid
  User::LeaveIfError( index );
  
  // Copy the photo size data
  aData.iPhotoSize = KCamPhotoSizes[index];
  
  // --------------------------------
  // Photo quality settings
  
  // Set the photo resulution value, based on photo size value
  aData.iPhotoQualitySetting.iPhotoResolution
    = MapPhotoSizeToResolution( aData.iPhotoSize.iSizeId );
            
  // Jpeg compression  
  aData.iPhotoQualitySetting.iQualityLevel = aSet.iCompressionQuality;
      
  // Estimated file size
  aData.iFileSize = aSet.iEstimatedSize;        
     
  // Set correct image quality description, quality id and icons
  TCamImageType type = CamConfigurationUtility::ImageType( aSet.iCamcorderVisible );
  aData.iDescription = KCamImageQualityMappings[index][type].iDesc;
  
  if( ECamPhotoQualityDescLast == aData.iDescription )
    {
    // No description defined for this quality => not supported  
    User::Leave( KErrNotSupported );
    }  
  
  aData.iPhotoQualitySetting.iPhotoQualityId = KCamImageQualityMappings[index][type].iQuality;                                      
  aData.iQualityIcons = KCamImageIcons[ aData.iDescription ];                                                 
                                                                                                  
  PRINT( _L("Camera <= CCamConfiguration::InitializeImageQualityL") );
  }
  
// ---------------------------------------------------------------------------
// CCamConfiguration::GetPsiIntArrayL
// ---------------------------------------------------------------------------
//   
void 
CCamConfiguration::GetPsiIntArrayL( TCamPsiKey aKey, RArray<TInt>& aArray ) const
  {
  PRINT1( _L("Camera => CCamConfiguration::GetPsiIntArrayL Key: %d"), aKey );  
  switch( aKey )
    {
    case ECamPsiSupportedStillResolutions:
      {
      // Despite the name, returns array of TCamPhotoSizeId
      // The array contents must be ordered in TCamPhotoSizeId enum order
      for( TInt i=0; i<ECamPhotoSizeLast; i++ )
        {
        AppendIfSupportedL( ECamItemPhotoSize, i, aArray );            
        }      
      break;              
      }
      
    case ECamPsiJpegDataSizes:
      {
      // Jpeg data sizes. Must be in same order as supported still resolutions,
      // so photo size is used for indexing.  
      for( TInt i=0; i<ECamPhotoSizeLast; i++ )
        {
        AppendIfSupportedL( ECamItemPhotoSize, i, aArray, ECamItemPhotoJpegSize );            
        }      
      break;  
      }
      
    case ECamPsiSequencePostProcessDataSizes:
      {
      // Jpeg data sizes. Must be in same order as supported still resolutions,
      // so photo size is used for indexing.  
      for( TInt i=0; i<ECamPhotoSizeLast; i++ )
        {
        AppendIfSupportedL( ECamItemPhotoSize, i, aArray, ECamItemSeqPostProcSize );            
        }        
      break;  
      }
              
    case ECamPsiSupportedStillQualities:
      {
      // This is array for primary camera qualities, so do not include secondary quality.
      // The results  
      for( TInt i=0; i<ECamImageQualitySecondary; i++ )
        {
        AppendIfSupportedL( ECamItemPhotoQuality, i, aArray );            
        }        
      break;      
      } 
      
    case ECamPsiStillQualityIconIds:
      {   
      // Icon ids. Must be in the same order as ECamPsiSupportedStillQualities,
      // so that type is used for indexing.  
      // This is array for primary camera qualities, so do not include secondary quality.  
      for( TInt i=0; i<ECamImageQualitySecondary; i++ )
        {
        AppendIfSupportedL( ECamItemPhotoQuality, i, aArray, ECamItemSettingsIconId );            
        }        
      break;
      }   
      
    case ECamPsiPrecapStillQualityIconIds:
      {
      // Secondary camera quality is included  
      for( TInt i=0; i<ECamImageQualityLast; i++ )
        {
        AppendIfSupportedL( ECamItemPhotoQuality, i, aArray, ECamItemPrecapIconId );            
        }        
      break;                
      }
      
    case ECamPsiSupportedVideoResolutions:
      {
      for( TInt i=0; i<ECamVideoResolutionLast; i++ )
        {
        AppendVideoResIfSupportedL( i, aArray );  
        }
      break;  
      }
      
    case ECamPsiSupportedVideoQualities:
      {
      TInt n = iVideoQualities.Count();  
      for( TInt i=0; i<n; i++ ) 
        {
        aArray.AppendL( iVideoQualities[i].iQualitySetting );  
        }      
      break;  
      }
      
    case ECamPsiPrecapVideoQualityIconIds:      
      {
      TInt n = iVideoQualities.Count();
      for( TInt i=0; i<n; i++ )
        {
        aArray.AppendL( iVideoQualities[i].iQualityIcons.iPrecapIconId );
        aArray.AppendL( iVideoQualities[i].iQualityIcons.iPrecapIconMask );
        }  
      break;  
      }
      
    case ECamPsiVideoQualityIconIds:
      {
      TInt n = iVideoQualities.Count();
      for( TInt i=0; i<n; i++ )
        {
        aArray.AppendL( iVideoQualities[i].iQualityIcons.iSettingsIconId );
        aArray.AppendL( iVideoQualities[i].iQualityIcons.iSettingsIconMask );
        }          
      break;  
      }
      
    case ECamPsiVideoQualityFileTypeId:
      {
      TInt n = iVideoQualities.Count();
      for( TInt i=0; i<n; i++ )
        {
        aArray.AppendL( iVideoQualities[i].iFileTypeIcons.iIconId );
        aArray.AppendL( iVideoQualities[i].iFileTypeIcons.iMask );  
        } 
      break;   
      }   
          
    default:
      {
      PRINT( _L("Camera <> CCamConfiguration::GetPsiIntArrayL - Unknown key!") );
      User::Panic( KCamConfigPanicId, ECamConfigPanicUnknownKey );  
      }   
      
   }  
  PRINT( _L("Camera <= CCamConfiguration::GetPsiIntArrayL") );   
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::AppendIfSupportedL
// ---------------------------------------------------------------------------
// 
void
CCamConfiguration::AppendIfSupportedL( TCamItemType aCheckType, TInt aValue,  
                                       RArray<TInt>&aArray, TCamItemType aAppendType ) const
  {
  TInt n( iPrimaryCameraPhotoQualities.Count() );
  for( TInt i=0; i<n; i++ )
    {
    if( CamConfigurationUtility::IsSupported( aValue, aCheckType, 
                                              iPrimaryCameraPhotoQualities[i] ) )
      {
      if( ECamItemSame == aAppendType )
        {
        // Simply append the current value
        aArray.AppendL( aValue );
        }
      else
        {
        // Append something else corresponding the item  
        AppendTypeL( aAppendType, aArray, iPrimaryCameraPhotoQualities[i] );
        }
        
      return;
      }
    }
    
  n = iSecondaryCameraPhotoQualities.Count();
  for( TInt i=0; i<n; i++ )
    {
    if( CamConfigurationUtility::IsSupported( aValue, aCheckType, 
                                              iSecondaryCameraPhotoQualities[i] ) )
      {
      if( ECamItemSame == aAppendType )
        {
        // Simply append the current value
        aArray.AppendL( aValue );
        }
      else
        {
        // Append something else corresponding the item  
        AppendTypeL( aAppendType, aArray, iSecondaryCameraPhotoQualities[i] );
        }
      return;  
      }   
    }    
  }


// ---------------------------------------------------------------------------
// CCamConfiguration::AppendTypeL
// ---------------------------------------------------------------------------
// 
void
CCamConfiguration::AppendTypeL( TCamItemType aType, RArray<TInt>&aArray, const TCamImageQualityData& aData )
  {
  switch( aType )
    {
    case ECamItemSettingsIconId:
      {
      aArray.AppendL( aData.iQualityIcons.iSettingsIconId );
      aArray.AppendL( aData.iQualityIcons.iSettingsIconMask );  
      break;
      }
    case ECamItemPrecapIconId:
      {
      aArray.AppendL( aData.iQualityIcons.iPrecapIconId );
      aArray.AppendL( aData.iQualityIcons.iPrecapIconMask );  
      break;
      }      
    case ECamItemPhotoJpegSize:
      {
      aArray.AppendL( aData.iFileSize );
      break;  
      }
    case ECamItemSeqPostProcSize:
      {
      aArray.AppendL( aData.iFileSize );  
      break;
      }        
    default:
      {
      User::Panic( KCamConfigPanicId, ECamConfigPanicUnknownType );    
      }  
    }  
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::NumImageQualities
// ---------------------------------------------------------------------------
//   
TInt
CCamConfiguration::NumImageQualities() const
  {
  TInt count( iPrimaryCameraPhotoQualities.Count() );
  if( iSecondaryCameraPhotoQualities.Count() > 0 )
    {
    // Only one quality currently supported for secondary cam   
    count++;
    // count += iSecondaryCameraPhotoQualities.Count();
    }
    
  return count;   
  }
  
// ---------------------------------------------------------------------------
// CCamConfiguration::SecondaryCameraImageQuality
// ---------------------------------------------------------------------------
//   
TInt
CCamConfiguration::MmsImageQuality() const
  {
  // As the primary camera image qualities are sorted, MMS quality is the
  // last one in the qualities array.
  TInt index = iPrimaryCameraPhotoQualities.Count()-1;
  
  return index < 0 ?
    KErrNotFound :
    iPrimaryCameraPhotoQualities[index].iPhotoQualitySetting.iPhotoQualityId;
  }
  
  
// ---------------------------------------------------------------------------
// CCamConfiguration::SecondaryCameraImageQuality
// ---------------------------------------------------------------------------
//    
TInt
CCamConfiguration::SecondaryCameraImageQuality() const
  {
  if( iSecondaryCameraPhotoQualities.Count() > 0 )
    {
    return iSecondaryCameraPhotoQualities[0].iPhotoQualitySetting.iPhotoQualityId;   
    }
  
  return KErrNotFound;      
  }  


// ---------------------------------------------------------------------------
// CCamConfiguration::ImageQuality
// ---------------------------------------------------------------------------
//  
const TCamPhotoQualitySetting&
CCamConfiguration::ImageQuality( TInt aIndex ) const
  {
  const TInt count( iPrimaryCameraPhotoQualities.Count() );
  if( aIndex >= 0 && aIndex < count )
    {
    return iPrimaryCameraPhotoQualities[aIndex].iPhotoQualitySetting;  
    }
  else
    {
    TInt secIndex = aIndex - count;
    if( secIndex >= 0 &&
        secIndex < iSecondaryCameraPhotoQualities.Count() )
      {
      return iSecondaryCameraPhotoQualities[secIndex].iPhotoQualitySetting;  
      }

    User::Panic( KCamConfigPanicId, ECamConfigPanicNoQuality ); 
    return iPrimaryCameraPhotoQualities[0].iPhotoQualitySetting; // Get rid of warning   
    }
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::ImageQualityData
// ---------------------------------------------------------------------------
// 
const TCamImageQualityData&
CCamConfiguration::ImageQualityData( TInt aIndex ) const
  { 
  const TInt count( iPrimaryCameraPhotoQualities.Count() );
  if( aIndex >= 0 && aIndex < count )
    {
    return iPrimaryCameraPhotoQualities[aIndex];  
    }
  else
    {
    TInt secIndex = aIndex - count;
    if( secIndex >= 0 &&
        secIndex < iSecondaryCameraPhotoQualities.Count() )
      {
      return iSecondaryCameraPhotoQualities[secIndex];  
      }

    User::Panic( KCamConfigPanicId, ECamConfigPanicNoQuality );
    return iPrimaryCameraPhotoQualities[0]; // Get rid of warning
    }
  } 
  
// ---------------------------------------------------------------------------
// CCamConfiguration::NumVideoQualities
// ---------------------------------------------------------------------------
//  
TInt
CCamConfiguration::NumVideoQualities() const
  {
  return iVideoQualities.Count();   
  }
  
// ---------------------------------------------------------------------------
// CCamConfiguration::NumPrimaryImageQualities
// ---------------------------------------------------------------------------
//  
TInt
CCamConfiguration::NumPrimaryImageQualities() const
  {
  return iPrimaryCameraPhotoQualities.Count();
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::SecondaryCameraVideoQuality
// ---------------------------------------------------------------------------
//    
TInt
CCamConfiguration::SecondaryCameraVideoQuality() const
  {
  // Currently always the last of the video qualities  
  // Currently always the last of the video qualities
  // As the camera video qualities are sorted, sharing quality is the
  // last one in the qualities array.
  TInt index = NumVideoQualities()-1;
  
  return index < 0 ?
    KErrNotFound :
    iVideoQualities[index].iQualitySettings.iQualityLevel;
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::VideoQualitySetting
// ---------------------------------------------------------------------------
//  
TInt
CCamConfiguration::VideoQualitySetting( TInt aIndex ) const
  {
  return iVideoQualities[aIndex].iQualitySetting;
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::VideoQuality
// ---------------------------------------------------------------------------
//  
const TVideoQualitySettings&
CCamConfiguration::VideoQuality( TInt aIndex ) const
  {
  return iVideoQualities[aIndex].iQualitySettings;
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::MapSupportedQualityToQualityDescriptor
// ---------------------------------------------------------------------------
//  
TInt
CCamConfiguration::MapSupportedQualityToQualityDescriptor( TBool aVideoMode,
                                                           TInt aQualityVal ) const
  {
  if( aVideoMode )
    {
    // Video mode
    TInt n = iVideoQualities.Count();
    for( TInt i=0; i<n; i++ )
      { 
      const TCamVideoQualityData& data = iVideoQualities[i];
      if( aQualityVal == data.iQualitySetting )
        {
        return data.iDescription;
        }
      }
    
    // No description found for this quality => Not supported
    User::Panic( KCamConfigPanicId, ECamConfigPanicNoDescription );       
    return KErrNotSupported;
    }
  else
    {
    // Image mode
    TInt n = iPrimaryCameraPhotoQualities.Count();
    for( TInt i=0; i<n; i++ )
      {
      const TCamImageQualityData& data = iPrimaryCameraPhotoQualities[i];
      if( aQualityVal == data.iPhotoSize.iQualitySettingId )
        {
        return data.iDescription;
        }
      }
    
    // No description found for this quality value => Not supported
    User::Panic( KCamConfigPanicId, ECamConfigPanicNoDescription );  
    return KErrNotSupported;
    }                                                             
  
  }
  

// ---------------------------------------------------------------------------
// CCamConfiguration::MapPhotoSizeToResolution
// ---------------------------------------------------------------------------
//    
TCamImageResolution 
CCamConfiguration::MapPhotoSizeToResolution( TCamPhotoSizeId aSizeId ) 
  {
  for( TInt i=0; i<ECamPhotoSizeLast; i++ )
    {
    if( KCamPhotoSizes[i].iSizeId == aSizeId )
      {
      return KCamPhotoSizes[i].iResolution;
      }
    }  
  // Used as deafult in previous implementation  
  return EImageResolutionMMS;    
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::MapResolutionToPhotoSize
// ---------------------------------------------------------------------------
//   
TCamPhotoSizeId 
CCamConfiguration::MapResolutionToPhotoSize( TCamImageResolution aResolution )  
  {
  for( TInt i=0; i<ECamPhotoSizeLast; i++ )
    {
    if( KCamPhotoSizes[i].iResolution == aResolution )
      {
      return KCamPhotoSizes[i].iSizeId;
      }
    }  
  // Used as deafult in previous implementation  
  return ECamPhotoSizeVGA;    
  }    
 
// ---------------------------------------------------------------------------
// CCamConfiguration::ImagesRemaining
// ---------------------------------------------------------------------------
//  
TInt
CCamConfiguration::ImagesRemaining( TCamMediaStorage aStorage,
                                    TCamPhotoSizeId aSizeId, 
                                    TInt aCriticalLevel, 
                                    TBool /*aBurstActive*/ ) const
  {
    // PRINT( _L("Camera => CamUtility::ImagesRemaining") );
    TInt64 memoryfree = 0;

    // Get the current amount of free memory on the phone.
    switch ( aStorage )
        {
    case ECamMediaStoragePhone:
        memoryfree 
          = CamUtility::MemoryFree( DriveInfo::EDefaultPhoneMemory,
                                    aCriticalLevel );          
        break;

    case ECamMediaStorageCard:
	    PRINT( _L("Camera => CamUtility::ImagesRemaining, ECamMediaStorageCard") );
        memoryfree 
          = CamUtility::MemoryFree( DriveInfo::EDefaultRemovableMassStorage,
                                    aCriticalLevel );
        break;

    case ECamMediaStorageMassStorage:
	    PRINT( _L("Camera => CamUtility::ImagesRemaining, ECamMediaStorageMassStorage") );
        memoryfree 
          = CamUtility::MemoryFree( DriveInfo::EDefaultMassStorage,
                                    aCriticalLevel );
        break;
    default:
	    PRINT( _L("Camera => CamUtility::ImagesRemaining, default") );
        break;
        };    
        
  TInt size = 1;        
  
  TInt n=NumImageQualities();
  for( TInt i=0; i<n; i++ )
    {
    const TCamImageQualityData& data( ImageQualityData( i ) );
    if( aSizeId == data.iPhotoSize.iSizeId )
      {
      size = data.iFileSize;  
      }      
    }
  
   memoryfree -= KReserveedSpace;  
  // In the case of exceptional circumstances, just return zero images remaining.
  if ( memoryfree <= 0 )
  	{
  	memoryfree = 0;  // To handle exceptional situation, just return zero images.
  	} 
  
  TInt64 images = memoryfree / size;      

  images = Min( KMaxTInt, images ); // Truncate value to max possible.

  // PRINT1( _L("Camera <= CamUtility::ImagesRemaining %d"), (TInt)images );
  return static_cast<TInt>( images );  // (nothing greater than TInt)                      
  } 
 
 
// ---------------------------------------------------------------------------
// CCamConfiguration::AllocString8L
// ---------------------------------------------------------------------------
//  
const TUint8*  
CCamConfiguration::AllocString8L( const TUint8* aData )
  {  
  TPtrC8 ptr( aData );
  HBufC8* data = HBufC8::NewLC( ptr.Length() + 1 );
  data->Des().Copy( ptr );
  data->Des().Append( '\0' );
  
  iStrings8.AppendL( data );
  
  CleanupStack::Pop( data );
  
  return data->Ptr();
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::AllocString16L
// ---------------------------------------------------------------------------
// 
const TUint16*
CCamConfiguration::AllocString16L( const TUint8* aData )
  {
  TPtrC8 ptr( aData );
  HBufC16* data = HBufC16::NewLC( ptr.Length() + 1 );
  data->Des().Copy( ptr );
  data->Des().Append( '\0' );
  
  iStrings16.AppendL( data );
  
  CleanupStack::Pop( data );

  return data->Ptr();
  }  

// ---------------------------------------------------------------------------
// CCamConfiguration::CreateVideoQualityDataL
// ---------------------------------------------------------------------------
// 
TCamVideoQualityData
CCamConfiguration::CreateVideoQualityDataL( const TVideoQualitySet& aSet )
  {       
  PRINT3( _L("Camera => CCamConfiguration::CreateVideoQualityDataL - res: (%d, %d), video bitrate: %d" ), 
                                          aSet.iVideoWidth, aSet.iVideoHeight, aSet.iVideoBitRate ); 
    
  TInt resIndex( CamConfigurationUtility::VideoSizeIndex( 
            aSet.iVideoWidth, aSet.iVideoHeight ) );
  if( resIndex < 0 )
    {
    User::Panic( KCamConfigPanicId, ECamConfigPanicNoVideoRes );  
    }
        
  TCamVideoQualityDescription descType 
      = CamConfigurationUtility::VideoDescId( aSet.iCamcorderVisible ); 
      
  TInt videoType = CamConfigurationUtility::VideoType( aSet.iVideoFileMimeType );    
  
  // Convert FourCC value from TFourCC to asciiz string
  const TInt KFourCCLength = 5; // 4 characters + '\0'
  TText8 fourCCBuf[KFourCCLength ];
  TPtr8 fourCC( fourCCBuf, KFourCCLength );
  aSet.iAudioFourCCType.FourCC( &fourCC );
  fourCC.Append( '\0' );  
  
  // Initialized as here to avoid const_casts for the text pointers
  TCamVideoQualityData data =
    {
    // TVideoQualitySettings:  
      { 
      0,                                          // iQualityLevel - defined later
      KCamVideoSizes[resIndex].iCamVideoRes,      // iVideoResolution 
      aSet.iVideoFrameRate,                       // iVideoFrameRate  
      aSet.iVideoBitRate,                         // iVideoBitRate   
      aSet.iAudioBitRate,                         // iAudioBitRate    
      AllocString8L( aSet.iVideoFileMimeType ),   // iVideoMimeType   
      AllocString16L( aSet.iPreferredSupplier ),  // iPreferredSupplier
      AllocString8L( aSet.iVideoCodecMimeType ),  // iVideoCodec      
      AllocString8L( fourCCBuf ),                 // iAudioCodec
      CamConfigurationUtility::VideoType( aSet.iVideoFileMimeType ), // iVideoFileType
      ECamVideoClipMax                            // iVideoLength - changed later if necessary      
      },
      
    KCamVideoSizes[resIndex].iCamVideoRes,        // iVideoResolution  
    ECamVideoQualitySharing,                      // iQualitySetting - replaced later
    descType,                                     // iDescription
    KCamVideoIcons[descType],
    KCamVideoTypeIcons[videoType]
    };    
        
  PRINT( _L("Camera <= CCamConfiguration::CreateVideoQualityDataL" ) );   
  return data;  
  }

// ---------------------------------------------------------------------------
// CCamConfiguration::AppendVideoResIfSupportedL
// ---------------------------------------------------------------------------
// 
void
CCamConfiguration::AppendVideoResIfSupportedL( TInt aValue, RArray<TInt>&aArray ) const
  {
  TInt n( iVideoQualities.Count() );
  for( TInt i=0; i<n; i++ )
    {
    if( iVideoQualities[i].iVideoResolution == aValue )
      {
      aArray.AppendL( aValue );  
      }
    }  
  }  
    
// ---------------------------------------------------------------------------
// CCamConfiguration::CMRAvgVideoBitRateScaler
// ---------------------------------------------------------------------------
//    
TReal    
CCamConfiguration::CMRAvgVideoBitRateScaler() const
  {
  return iCMRAvgVideoBitRateScaler;  
  }
    
// ---------------------------------------------------------------------------
// CCamConfiguration::GetRequiredImageQualityIndex
// ---------------------------------------------------------------------------
//
TInt CCamConfiguration::GetRequiredImageQualityIndex( const TSize& aResolution ) const
    {
    TInt QualityIndex = 0;
    TInt MaxCount = NumPrimaryImageQualities();
    TCamImageQualityData QData;
    for ( TInt i = 0; i < MaxCount; i++ )
        {
        QData = ImageQualityData( i );
        if ( QData.iPhotoSize.iWidth == aResolution.iWidth
             && QData.iPhotoSize.iHeight == aResolution.iHeight )
            {
            QualityIndex = i;
            }
        }
    return QualityIndex;
    }

// ---------------------------------------------------------------------------
// CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto
// ---------------------------------------------------------------------------
//
TSize CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto( const TSize& aRequiredResolution ) const {
	PRINT2(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto aRequiredResolution: (%d,%d)"), aRequiredResolution.iWidth, aRequiredResolution.iHeight );
    TInt MaxCount = NumPrimaryImageQualities();
    TCamImageQualityData QData;
    TSize Reso( 0, 0 );
    TBool found = EFalse;
    for ( TInt i = 0; i < MaxCount; i++ )
        {
        QData = ImageQualityData( i );
        PRINT3(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto index: %d PhotoSize: (%d,%d)"), i,QData.iPhotoSize.iWidth, QData.iPhotoSize.iHeight );
        if ( QData.iPhotoSize.iWidth == aRequiredResolution.iWidth
             && QData.iPhotoSize.iHeight == aRequiredResolution.iHeight )
            {
                Reso = aRequiredResolution;
                found = ETrue;
            }
        }
    if ( !found && aRequiredResolution != TSize(0,0) ) { // TSize(0,0) should be returned the same as it means the mms quality
        TInt referenceQuality = aRequiredResolution.iWidth*aRequiredResolution.iHeight;
        TInt currentQuality = 0;
        TInt closestMatch = Abs(referenceQuality-currentQuality);
        PRINT1(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto closestMatch: (%d)"), closestMatch );
        TInt currentMatch = 0;
        for ( TInt i = 0; i < MaxCount; i++ )
            {
            QData = ImageQualityData( i );
            PRINT3(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto index: %d PhotoSize: (%d,%d)"), i, QData.iPhotoSize.iWidth, QData.iPhotoSize.iHeight );
            currentQuality = QData.iPhotoSize.iWidth*QData.iPhotoSize.iHeight;
            currentMatch = Abs(referenceQuality-currentQuality);
            PRINT1(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto currentMatch: (%d)"), currentMatch );
            if ( currentMatch < closestMatch ) 
                {
                Reso.SetSize( QData.iPhotoSize.iWidth, QData.iPhotoSize.iHeight );
                closestMatch = currentMatch;
                }
            }
        }
    PRINT2(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionPhoto return resolution: (%d,%d)"), Reso.iWidth, Reso.iHeight );
    return Reso;
}


// ---------------------------------------------------------------------------
// CCamConfiguration::GetRequiredVideoQualityIndex
// ---------------------------------------------------------------------------
//
TInt CCamConfiguration::GetRequiredVideoQualityIndex( const TSize& aResolution ) const
    {
    PRINT2(_L("Camera <=> CCamConfiguration::GetRequiredVideoQualityIndex aResolution: (%d,%d)"), aResolution.iWidth, aResolution.iHeight );
    TInt QualityIndex = 0;
    TInt MaxCount = NumVideoQualities();
    PRINT1(_L("Camera <=> CCamConfiguration::GetRequiredVideoQualityIndex MaxCount: %d"), MaxCount );
    TCamVideoQualityData QData;
    TSize VideoResolution(0,0);
    //for ( TInt i = 0; i < MaxCount; i++ )
    for ( TInt i = MaxCount-1; i >= 0; i-- )
        {
        QData = iVideoQualities[i];
        VideoResolution = CamConfigurationUtility::CamVideoResolution( QData.iVideoResolution );
        PRINT3(_L("Camera <=> CCamConfiguration::GetRequiredVideoQualityIndex index: %d VideoResolution: (%d,%d)"), i, VideoResolution.iWidth, VideoResolution.iHeight );
        if ( VideoResolution.iWidth == aResolution.iWidth
             && VideoResolution.iHeight == aResolution.iHeight )
            {
            QualityIndex = i;
            }
        }
    PRINT1(_L("Camera <=> CCamConfiguration::GetRequiredVideoQualityIndex return: %d"), QualityIndex );
    return QualityIndex;
    }

// ---------------------------------------------------------------------------
// CCamConfiguration::MapRequiredResolutionToActualResolutionVideo
// ---------------------------------------------------------------------------
//
TSize CCamConfiguration::MapRequiredResolutionToActualResolutionVideo( const TSize& aRequiredResolution ) const {
    PRINT2(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionVideo aRequiredResolution: (%d,%d)"), aRequiredResolution.iWidth, aRequiredResolution.iHeight );
    TInt MaxCount = NumVideoQualities();
    TCamVideoQualityData QData;
    TSize Reso( 0, 0 );
    TBool found = EFalse;
    TSize VideoResolution(0,0);
    for ( TInt i = 0; i < MaxCount; i++ )
        {
        QData = iVideoQualities[i];
        VideoResolution = CamConfigurationUtility::CamVideoResolution( QData.iVideoResolution );
        PRINT3(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionVideo index: %d VideoResolution: (%d,%d)"), i, VideoResolution.iWidth, VideoResolution.iHeight );
        if ( VideoResolution.iWidth == aRequiredResolution.iWidth
             && VideoResolution.iHeight == aRequiredResolution.iHeight )
            {
                Reso = aRequiredResolution;
                found = ETrue;
            }
        }
    if ( !found && aRequiredResolution != TSize(0,0) ) { // TSize(0,0) should be returned the same as it means the mms quality
        TInt referenceQuality = aRequiredResolution.iWidth*aRequiredResolution.iHeight;
        TInt currentQuality = 0;
        TInt closestMatch = Abs(referenceQuality-currentQuality);
        PRINT1(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionVideo closestMatch: (%d)"), closestMatch );
        TInt currentMatch = 0;
        for ( TInt i = 0; i < MaxCount; i++ )
            {
            QData = iVideoQualities[i];
            VideoResolution = CamConfigurationUtility::CamVideoResolution( QData.iVideoResolution );
            PRINT3(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionVideo index: %d VideoResolution: (%d,%d)"), i, VideoResolution.iWidth, VideoResolution.iHeight );
            currentQuality = VideoResolution.iWidth*VideoResolution.iHeight;
            currentMatch = Abs(referenceQuality-currentQuality);
            PRINT1(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionVideo currentMatch: (%d)"), currentMatch );
            if ( currentMatch < closestMatch ) 
                {
                Reso.SetSize( VideoResolution.iWidth, VideoResolution.iHeight );
                closestMatch = currentMatch;
                }
            }
        }
    PRINT2(_L("Camera <=> CCamConfiguration::MapRequiredResolutionToActualResolutionVideo return resolution: (%d,%d)"), Reso.iWidth, Reso.iHeight );
    return Reso;
}

// ---------------------------------------------------------------------------
// CCamConfiguration::GetDefaultVideoQualityFromIcmL
// ---------------------------------------------------------------------------
//
TCamVideoQualitySettings CCamConfiguration::GetDefaultVideoQualityFromIcmL()
  {
  PRINT(_L("Camera => CCamConfiguration::GetDefaultVideoQualityFromIcmL") );  
  TInt totalLevels = iIcm->NumberOfVideoQualityLevels();
  CArrayFixFlat<TUint>* levels 
    = new (ELeave) CArrayFixFlat<TUint>( totalLevels );
  CleanupStack::PushL( levels );
  TVideoQualitySet set;
  TVideoQualitySet temp;

  if( iPrimaryCameraDisplayId )
    {    
    iIcm->GetVideoQualityLevelsL( *levels, iPrimaryCameraDisplayId );  
    TInt numLevels( levels->Count() );
    PRINT1(_L("Camera <> CCamConfiguration::GetDefaultVideoQualityFromIcmL - Total number of levels: %d"), numLevels );

    iIcm->GetVideoQualitySet( set, levels->At( 0 ), iPrimaryCameraDisplayId );
    
    for( int i=1; i<numLevels; i++ )
      {
      iIcm->GetVideoQualitySet( temp, levels->At( i ), iPrimaryCameraDisplayId );
	  if( temp.iCamcorderVisible > 0  && temp.iVideoQualitySetLevel > set.iVideoQualitySetLevel )
        {      
        set = temp;                 
        }
      }
    }  
  CleanupStack::PopAndDestroy( levels );

  TInt count = iVideoQualities.Count();
  TCamVideoQualityDescription descType 
      = CamConfigurationUtility::VideoDescId( set.iCamcorderVisible ); 
  for(TInt i = 0; i < count; i++)
    {
    if(iVideoQualities[i].iDescription == descType )
      {
      PRINT( _L("Camera <= CCamConfiguration::GetDefaultVideoQualityFromIcmL") );   
	  return iVideoQualities[i].iQualitySetting;
      }
	 }

  // In normal case application should not come to here
  // If could not find the exact video qulity setting, just return WidescreenHigh
  // Should be fixed in other product
  return ECamVideoQualityNormalHigh; 
  }

// End of file  

