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
* Description:  Implementation for camera dynamic configuration.
*                
*
*/


#ifndef CAMCONFIGURATION_H
#define CAMCONFIGURATION_H

// INCLUDES

#include <e32base.h>
#include "CamSettingsInternal.hrh"
#include "CamPSI.h"
#include "camconfigurationtypes.h"
#include "imagingconfigmanager.h"
#include "CamSettings.hrh"

// FORWARD DECLARATIONS

class CImagingConfigManager;
struct TVideoQualitySet;
struct TImageQualitySet;

// CLASS DECLARATION

/**
* Class CCamConfiguration
* Queries supported image and video qualities for ICM and creates the related 
* data and does the mappings needed by cameraapp application. These include
* quality descriptions and icons.
* 
* In the initial phase, emulates the product specific configuration code,
* so only minimal changes are needed in cameraapp UI code.
*/
class CCamConfiguration: public CBase
{  
  public: // Constructors and destructors
   
   	/**
    * Destructor.
    */
    ~CCamConfiguration(); 
    
    /**
    * Two-phased constructor.
    * @return pointer to the created CCamConfiguration object
    */  
    static CCamConfiguration* NewL();
    
  public: // New functions
    
    /**
    * Returns number of image qualities configured using ICM. Setting data
    * for these qualities is accessible via ImageQuality() function.
    * @return Number of qualities
    */
    TInt NumImageQualities() const;
    
    /**
    * Returns index for primary camera MMS quality
    * @return MMS quality index or KErrNotFound
    */   
    TInt MmsImageQuality() const;
    
    /**
    * Returns index for secondary camera imagequality.
    * @return Secondary image quality index or KErrNotFound
    */    
    TInt SecondaryCameraImageQuality() const;   
    
    /**
    * Returns settings of the requested quality
    * @param aIndex Index of the quality (Must be <NumImageQualities())
    * @return Quality settings
    */
    const TCamPhotoQualitySetting& ImageQuality( TInt aIndex ) const;
        
    /**
    * Returns number of video qualities configured using ICM. Setting data
    * for these qualities is accessible via VideoQuality() function.
    * @return Number of video qualities
    */
    TInt NumVideoQualities() const;
  
    /**
    * Returns number of primary camera image qualities configured using ICM.
    * Setting data for these qualities is accessible via ImageQuality( TInt aIndex ) function.
    * @return Number of primary camera image qualities
    */
    TInt NumPrimaryImageQualities() const;

    /**
    * Returns index for secondary camera video quality. Currently this is
    * always same as last quality for primary camera.
    * @return Secondary video quality index
    */
    TInt SecondaryCameraVideoQuality() const; 
  
    /**
    * Returns the quality setting for a required index from iVideoQualities array
    * in camera video quality.
    * @return iVideoQualities array index
    */
    TInt VideoQualitySetting( TInt aIndex ) const;

    /**
    * Returns settings data of the requested video quality
    * @param aIndex Index of the quality (Must be <NumImageQualities())
    * @return Quality settings
    */
    const TVideoQualitySettings& VideoQuality( TInt aIndex ) const;
    
    /**
    * Gives description id for given image/video quality
    * Image: TCamImageQualitySetting => TCamPhotoQualityDescription
    * Video:  TCamVideoQualitySettings => TCamVideoQualityDescription
    * @param aVideoMode EFalse if image mode
    * @param aQualityVal Quality value
    * @return Description id
    *
    */
    TInt MapSupportedQualityToQualityDescriptor( TBool aVideoMode,
                                                 TInt aQualityVal ) const;      
                                                 
    /**
    * Returns the the following image quality related integer arrays:
    * - ECamPsiSupportedStillResolutions
    * - ECamPsiJpegDataSizes
    * - ECamPsiSequencePostProcessDataSizes
    * - ECamPsiSupportedStillQualities
    * - ECamPsiStillQualityIconIds
    * - ECamPsiPrecapStillQualityIconIds
    * And following video quality related arrays:
    * - ECamPsiSupportedVideoQualities
    * - ECamPsiPrecapVideoQualityIconIds
    * - ECamPsiVideoQualityIconIds
    * - ECamPsiVideoQualityFileTypeId
    *
    * @param aKey   Key
    * @param aArray Destination array
    */
    void GetPsiIntArrayL( TCamPsiKey aKey, RArray<TInt>& aArray ) const; 
                                                 
    /**
    * Provides mapping between TCamImageResolution and TCamPhotoSizeId types
    * @param aResolution
    * @return Photo Size
    */
    static TCamPhotoSizeId MapResolutionToPhotoSize( TCamImageResolution aResolution );
      
    /**
    * Provides mapping between TCamPhotoSizeId and TCamImageResolution types
    * @param aSizeId
    * @return Resolution
    */  
    static TCamImageResolution MapPhotoSizeToResolution( TCamPhotoSizeId aSizeId );  
  
    /**
    * Calculate number of images than can still be taken with the current
    * image size
    * @param aStorage Storage media
    * @param aSizeId The current photo resolution setting
    * @param aCriticalLevel Critical threshold on the specified drive 
    * @param aBurstActive Whether or not burst mode is active
    * @return Number of images
    */        
    TInt ImagesRemaining( TCamMediaStorage aStorage,
                          TCamPhotoSizeId aSizeId, 
                          TInt aCriticalLevel, 
                          TBool aBurstActive ) const;          
                          
    /**
    * Returns the CMR average video bitrate scaler (obtained from ICM)
    * This is used to scale bitrates for remaining video time calculation
    * @return scaler
    */
    TReal CMRAvgVideoBitRateScaler() const;
    
    /**
    * Returns the default VideoQuality setting which has the highest 
    * iVideoQualitySetLevel value (obtained from ICM)
    * This is used to get the default video quality setting when restore
    * settings or frist start camera
    * @return video quality
    */
	TCamVideoQualitySettings GetDefaultVideoQualityFromIcmL();
    
    /** 
    * Returns the index to the image quality required by embedding application
    * @return index to icm image configuration array
    */
    TInt GetRequiredImageQualityIndex( const TSize& aResolution ) const;

    /** 
    * Returns the resolution that matches closest to the required resolution by embedding application
    * @return TSize of the resolution
    */
    TSize MapRequiredResolutionToActualResolutionPhoto( const TSize& aRequiredResolution ) const;

    /** 
    * Returns the index to the video quality required by embedding application
    * @return index to icm video configuration array
    */
    TInt GetRequiredVideoQualityIndex( const TSize& aResolution ) const;

    /** 
    * Returns the resolution that matches closest to the required resolution by embedding application
    * @return TSize of the resolution
    */
    TSize MapRequiredResolutionToActualResolutionVideo( const TSize& aRequiredResolution ) const;

                                                 
  private:  
    /**
    * C++ default constructor.
    * @since 2.8
    */        
    CCamConfiguration();
    
    /**
    * 2nd phase constructor.
    */
    void ConstructL();
    
    /**
    * Queries image qualities from ICM, converts enabled qualities to internal
    * data formats and adds them to ordered arrays.
    */
    void InitializeImageQualitiesL();
    
    /**
    * Initializes TCamImageQualityData contents based on TImageQualitySet, which
    * we have received from the ICM.
    * @param aSet Image quality set from ICM
    * @param aData CamConfiguration image quality data structure
    */
    void InitializeImageQualityL( const TImageQualitySet& aSet, TCamImageQualityData& aData );
    
    /**
    * Returns all data of the requested quality
    * @param aIndex Index of the quality (Must be <NumImageQualities())
    * @return Quality data
    */
    const TCamImageQualityData& ImageQualityData( TInt aIndex ) const;    
    
    /**
    * Queries video qualities from ICM, converts enabled qualities to internal
    * data formats and adds them to ordered arrays.
    */    
    void InitializeVideoQualitiesL();
    
    /**
    * Creates TCamVideoQualityData contents based on TVideoQualitySet, which we
    * have received from ICM. Not const, because memory allocations are needed
    * for string data.
    * @param aSet ICM video quality data
    * @return Video quality data in CamConfiguration data structure
    */
    TCamVideoQualityData CreateVideoQualityDataL( const TVideoQualitySet& aSet );
       
    /**
    * Adds the given image quality data to correct place in ordered image
    * qualities data array. Ordering is done based on number of pixels in the
    * image quality.
    * @param aQuality Image quality data
    * @param aArray Ordered image quality data array
    */
    static void AddToOrderedImageQualitiesArrayL( TCamImageQualityData& aQuality, 
                                                  RArray<TCamImageQualityData>& aArray );

    /**
    * Adds the given video quality data to correct place in ordered video
    * qualities data array. Ordering is done based on the image quality
    * description enumeration. (Highest quality first)
    * @param aQuality Image quality data
    * @param aArray Ordered image quality data array
    */                                                  
    static void AddToOrderedVideoQualitiesArrayL( TCamVideoQualityData& aQuality,
                                                  RArray<TCamVideoQualityData>& aArray );   
                                                                                              
    /**
    * Checks if the value for aValue is used for type aCheckType if the 
    * supported image qualities. If so, Appends the aAppendType value 
    * (or aValue, if aAppendType is ECamItemSame) of that quality to
    * the array.
    * 
    * @param aCheckType Type for checking existence of value
    * @param aValue Item value (of checking type)
    * @param aArray Integer array
    * @param aAppendType Type of correspondin value to be appended
    */                                                  
    void AppendIfSupportedL( TCamItemType aCheckType, TInt aValue,
                             RArray<TInt>&aArray, TCamItemType aAppendType=ECamItemSame ) const;
   
    /**
    * Appends value of item of type aType from image quality data to aArray.
    * @param aType Item type
    * @param aArray Destination array
    * @param aData Image quality data
    */                          
    static void AppendTypeL( TCamItemType aType, RArray<TInt>&aArray, const TCamImageQualityData& aData );
    
    /**
    * @param aValue Video resolution value
    * @param aArray Destination array
    */
    void AppendVideoResIfSupportedL( TInt aValue, RArray<TInt>&aArray ) const;
  
    /**
    * Allocates memory for asciiz string given in aData. Returns pointer to the
    * string in the allocated memory.
    * @param aData
    * @return String
    */  
    const TUint8*  AllocString8L( const TUint8* aData );
    
    /**
    * Allocates memory for asciiz string given in aData, converts it to 16-bit
    * and returns pointer to the 16-bit string in the allocated memory.
    * @param aData
    * @return String
    */      
    const TUint16* AllocString16L( const TUint8* aData ); 
  
  private: // Data
  
    // Imaging configuration manager 
    CImagingConfigManager* iIcm;
    
    // Camera display ids. Used when querying ICM for supported image and 
    // video qualities.
    TInt iPrimaryCameraDisplayId;
    TInt iSecondaryCameraDisplayId;
    
    // Image qualities data for primary and secondary camera
    RArray<TCamImageQualityData> iPrimaryCameraPhotoQualities;
    RArray<TCamImageQualityData> iSecondaryCameraPhotoQualities;
    
    // Video quality data. Last item is shared between primary camera MMS
    // quality and secondary camera.
    RArray<TCamVideoQualityData> iVideoQualities;  
    TInt                         iSecondaryVideoQualityIndex;
    
    // Arrays for storing (video qualities) string data from ICM
    // All the content is owned by CCamConfiguration
    RPointerArray<HBufC8> iStrings8;
    RPointerArray<HBufC16> iStrings16;  
    
    // Average video bitrate scaler - needed for remaining time calculation
    TReal iCMRAvgVideoBitRateScaler;
};

#endif // CAMCONFIGURATION_H

// End of file

