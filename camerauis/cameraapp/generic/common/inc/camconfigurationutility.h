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
* Description:  Helper class for camera dynamic configuration
*                
*
*/

#ifndef CAMCONFIGURATIONUTILITY_H
#define CAMCONFIGURATIONUTILITY_H

#include <e32base.h>
#include "camconfiguration.h"

class CamConfigurationUtility
  {    
  public:
      
    /**
    * Returns index in KPhotoSizes table for given photo resolution
    * @param aWidth  Width
    * @param aHeight Height
    * @return Size index, or KErrNotFound
    */
    static TInt PhotoSizeIndex( TInt aWidth, TInt aHeight );
      
    /**
    * Returns index in KVideoSizes table for given video resolution
    * @param aWidth  Width
    * @param aHeight Height
    * @return Size index, or KErrNotFound
    */
    static TInt VideoSizeIndex( TInt aWidth, TInt aHeight );
                                                                             
    /**
    * Adjusts print qualities from ECamQualityPrint to ECamQualityPrintHigh and Low,
    * where necessary.
    * @param aPhotoQualities Array of image quality data
    */                                    
    static void AdjustPrintQualities( RArray<TCamImageQualityData>& aPhotoQualities );
  
    /**
    * Sets video quality levels for an ordered array of video qualities data.
    * 
    * @param aOrderedArray
    */
    static void SetVideoQualityLevels( RArray<TCamVideoQualityData>& aOrderedArray );  
                                                              
    /**
    * Appends integer value to array, if the value is not yet present in the array.
    * @param aValue Integer value
    * @param aArray Integer array
    */
    static void AddToArrayL( TInt aValue, RArray<TInt>& aArray );                                    
                                                                                                                               
    /**
    * Returns index in KPhotoSizes table for given photo resolution
    * @param aWidth  Width
    * @param aHeight Height
    * @return Size id, or KErrNotFound
    */
    static TInt VideoResolution( TInt aWidth, TInt aHeight );
    
    /**
    * Returns index in KPhotoSizes table for given photo resolution
    * @param aWidth  Width
    * @param aHeight Height
    * @return Size id, or KErrNotFound
    */  
    static TInt CamVideoResolution( TInt aWidth, TInt aHeight );
    
    /**
    * Returns TSize of for given photo resolution of the KPhotoSizes table
    * @param Size id
    * @return TSize resolution, or KErrNotFound
    */  
    static TSize CamVideoResolution( TCamVideoResolution aResolution );

    /**
    * 
    * @param aMimeType type
    * @return Mime type
    */
    static TInt VideoType( const TText8* aMimeType );
                                   
    /**
    * Converts CamcorderVisible value into video description id
    * @param aQualityValue
    * @return Description id
    */
    static TCamVideoQualityDescription VideoDescId( TInt aQualityValue );
    
    /**
    * Converts CamcorderVisible value into image type
    * @param aValue
    * @return Image type
    */  
    static TCamImageType ImageType( TInt aValue );
    
    
    static TBool IsSupported( TInt aValue, TCamItemType aItemType, 
                              const TCamImageQualityData& aData );

    /**
    * Get the scene setting id from dynamic setting id.
    */
    static TInt MapDynamicSettingToSceneSetting( TInt aSettingId );
  };

#endif CAMCONFIGURATIONUTILITY_H

// End of file

