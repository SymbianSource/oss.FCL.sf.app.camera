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
* Description:  Setting conversion utility class declaration
*
*/


#ifndef CAM_SETTINGCONVERSION_H
#define CAM_SETTINGCONVERSION_H

#include "camcameracontrollerflags.hrh" // Keep first

#include <gdi.h>
#include <e32std.h>

#include <ecam.h>
#include <ecamadvsettings.h>
#include <ECamOrientationCustomInterface2.h>

#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "camcameracontrollersettings.h"


// ===========================================================================

/**
* CCamSettingConversion utility class
* 
* Convert the value used in UI side settings to values suitable for CCamera
* or Camera Controller
*/
class CCamSettingConversion : public CBase
  {
  public:
    static CCamera::TWhiteBalance
      Map2CameraWb( const TCamWhiteBalanceId& aWbId );
      
    static CCamera::TFlash
      Map2CameraFlash( const TCamFlashId& aFlashId );

    static TCamFlashId
      Map2FlashId( const CCamera::TFlash& aCameraFlash );

    static CCamera::TFormat
      Map2CameraFormat( const TDisplayMode& aDisplayMode );

    static CCamera::TExposure
      Map2CameraExposureMode( const TCamSceneAEMode& aExpModeId );

#ifdef CAMERAAPP_CAPI_V2_IP
    /**
    * Map2CameraEffect
    * Map colour effect id from UI to Image Processing effect id.
    */
    static CCamera::CCameraImageProcessing::TEffect
      Map2CameraEffect( const TCamColourFilterId& aColourFilterId );
#endif

    /**
    * Map UI Setting Model setting id to Camera Controller setting id.
    */
    static NCamCameraController::TCamCameraSettingId
      Map2CameraControllerSettingId( const TCamSettingItemIds& aUiSettingId );
  
#if defined( CAMERAAPP_CAPI_V2_ADV ) || defined( CAMERAAPP_CAPI_V2_IP )
    /**
    * Map Camera Controller setting id to ECAM event UID value.
    * @see Map2CameraSetting
    */
    static TInt
      Map2EventUidValue( 
        const NCamCameraController::TCamCameraSettingId& aSettingId );

    /**
    * Map ECAM event UID value to Camera Controller setting id.
    * @see Map2EventUidValue
    */
    static NCamCameraController::TCamCameraSettingId
      Map2CameraSetting( TInt aUidValue );
      
    /**
    * Converts autofocus range from camera ui TCamSceneAFMode type
    * to CCamera TFocusRange type
    * @param aAfMode Autofocus mode in camera ui type
    * @return Autofocus range in TFocusRange type
    */        
    static CCamera::CCameraAdvancedSettings::TFocusRange
      Map2CameraAutofocus( const TCamSceneAFMode& aAfMode );      
      
#endif // defined( CAMERAAPP_CAPI_V2_ADV ) || defined( CAMERAAPP_CAPI_V2_IP )

    /**
    * Convert UI orienation setting value of type TCamImageOrientation
    * to MCameraOrientation TOrientation value.
    */
    static MCameraOrientation::TOrientation 
      Map2CameraOrientation( const TCamImageOrientation& aSettingOrientation );
  };
  

#endif // CAM_SETTINGCONVERSION_H

// ===========================================================================
// end of file
