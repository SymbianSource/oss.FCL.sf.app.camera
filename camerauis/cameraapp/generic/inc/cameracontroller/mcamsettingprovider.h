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
* Description:  Camera setting provider interface.
*
*/


#ifndef CAM_MCAMSETTINGPROVIDER_H
#define CAM_MCAMSETTINGPROVIDER_H

#include "camcameracontrollersettings.h"

/**
 *  MCamSettingProvider mixin.
 */
class MCamSettingProvider
  {
  public:
    
    /**
    * Get given camera parameters.
    * The associated data is returned in the TAny pointer.
    * Actual data type depends on the parameter id and is documented in
    * TCamCameraParamId documentation.
    *
    * These parameters represent overall settings of camera, e.g.
    * image quality and format, viewfinder size etc. The setting version
    * (ProvideCameraSettingL) is used to get more exact settings like
    * flash mode, exposure compensation setting etc.
    *
    * @param aParamId   Id of the parameter which the user is interested in.
    * @param aParamData Pointer to the parameter data structure. The actual
    *                   data type depends on parameter id.
    * @see TCamCameraParamId
    */
    virtual void 
      ProvideCameraParamL( 
        const NCamCameraController::TCamCameraParamId& aParamId,
              TAny*                                    aParamData ) = 0;

    /**
    * Get given camera setting.
    * The associated data is returned in the TAny pointer.
    * Actual data type depends on the setting id and is documented in
    * TCamCameraSettingId documentation.
    * 
    * This method is used to get specific settings for camera, e.g.
    * exposure compensation, white balance etc. Some of these settings
    * still combine mode and value, so TAny pointer is used to be able
    * to return setting data of variable size and type.
    *
    * @see TCamCameraSettingId
    */
    virtual void 
      ProvideCameraSettingL( 
        const NCamCameraController::TCamCameraSettingId& aSettingId,
              TAny*                                      aSettingData ) = 0;

    /**
    * Provide the currently pending setting changes.
    *
    */
    virtual void 
      ProvidePendingSettingChangesL( 
        RArray<NCamCameraController::TCamCameraSettingId>& aSettingIds ) = 0;
        
    /**
    * Number of currently pending setting changes.
    */
    virtual TInt PendingSettingChangeCount() const = 0;

    /**
    * Set the supported ISO rates.
    */
    virtual void SetSupportedISORatesL( const RArray<TInt>& aSupportedIsoRates ) = 0;
    
    /**
    * Virtual destructor.
    * Need to be defined in order to be able to gracefully delete
    * inherited class entities through MCamSettingProvider pointer.
    * If not declared here, User-42 panic occurs on delete.
    */
    virtual ~MCamSettingProvider() {};
  };

#endif // CAM_MCAMSETTINGPROVIDER_H

// end of file
