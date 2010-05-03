/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
/*
* Symbian specific settings handling
*/

#include <ecamadvsettingsuids.hrh>

#include "cxeautofocuscontrol.h"
#include "cxesettingsmappersymbian.h"
#include "cxutils.h"
#include "cxenamespace.h"

using namespace Cxe;

/*
* Map White balance UI value to its corresponding CCamera value
*/

CCamera::TWhiteBalance
CxeSettingsMapperSymbian::Map2CameraWb(int wbId )
{
  CCamera::TWhiteBalance wb( CCamera::EWBAuto );
  switch(wbId) {
    case WhitebalanceAutomatic:         wb = CCamera::EWBAuto;        break;
    case WhitebalanceSunny:             wb = CCamera::EWBDaylight;    break;
    case WhitebalanceCloudy:            wb = CCamera::EWBCloudy;      break;
    case WhitebalanceIncandescent:      wb = CCamera::EWBTungsten;    break;
    case WhitebalanceFluorescent:       wb = CCamera::EWBFluorescent; break;
    default:                  
      break;
  }
  return wb;
}



// Map2CameraFlash

CCamera::TFlash
CxeSettingsMapperSymbian::Map2CameraFlash(int flashId)
{
  CCamera::TFlash flash( CCamera::EFlashAuto );
  switch (flashId) {
    case FlashAuto:            flash = CCamera::EFlashAuto;          break;
    case FlashAntiRedEye:      flash = CCamera::EFlashRedEyeReduce;  break;
    case FlashOn:              flash = CCamera::EFlashForced;        break;
    case FlashOff:             flash = CCamera::EFlashNone;          break;
    default:                  
      break;
  }
  return flash;
}


// ---------------------------------------------------------------------------
// Map2CameraExposureMode
// ---------------------------------------------------------------------------
//
CCamera::TExposure
CxeSettingsMapperSymbian::Map2CameraExposureMode(int expModeId )
  {
  CCamera::TExposure expo( CCamera::EExposureAuto );

  switch ( expModeId )
    {
    case ExposureAuto:       expo = CCamera::EExposureAuto; break;
    case ExposureNight:      expo = CCamera::EExposureNight; break;
    case ExposureBacklight:  expo = CCamera::EExposureBacklight; break;
    case ExposureSport:      expo = CCamera::EExposureSport; break;
    default:
      break;
    }

  return expo;
  }




// ---------------------------------------------------------------------------
// Map2CameraEffect
// ---------------------------------------------------------------------------
//
CCamera::CCameraImageProcessing::TEffect
CxeSettingsMapperSymbian::Map2CameraEffect(int colourFilterId)
  {
  CCamera::CCameraImageProcessing::TEffect effect(
            CCamera::CCameraImageProcessing::EEffectNone );

  switch( colourFilterId )
    {
    case ColortoneNormal:        effect = CCamera::CCameraImageProcessing::EEffectNone;       break;
    case ColortoneBlackAndWhite: effect = CCamera::CCameraImageProcessing::EEffectMonochrome; break;
    case ColortoneSepia:         effect = CCamera::CCameraImageProcessing::EEffectSepia;      break;
    case ColortoneNegative:      effect = CCamera::CCameraImageProcessing::EEffectNegative;   break;
    case ColortoneVivid:         effect = CCamera::CCameraImageProcessing::EEffectVivid;      break;
    default:
      break;
    }
  return effect;
  }



// ---------------------------------------------------------------------------
// CxeSettingsMapperSymbian::Map2CameraAutofocus
// ---------------------------------------------------------------------------
//
CCamera::CCameraAdvancedSettings::TFocusRange
CxeSettingsMapperSymbian::Map2CameraAutofocus(CxeAutoFocusControl::Mode afMode )
{
  CCamera::CCameraAdvancedSettings::TFocusRange value;
  switch( afMode ) {
    case CxeAutoFocusControl::Macro:
      value = CCamera::CCameraAdvancedSettings::EFocusRangeMacro;
      break;
    case CxeAutoFocusControl::Portrait:
      value = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
      break;
    case CxeAutoFocusControl::Infinity:
      value = CCamera::CCameraAdvancedSettings::EFocusRangeInfinite;
      break;
    case CxeAutoFocusControl::Hyperfocal:
      value = CCamera::CCameraAdvancedSettings::EFocusRangeHyperfocal;
      break;
    case CxeAutoFocusControl::Auto:     // Fall through
    default:
      value = CCamera::CCameraAdvancedSettings::EFocusRangeAuto;
      break;
  }
  return value;
}



// CxeSettingsMapperSymbian::Map2CameraOrientation

MCameraOrientation::TOrientation 
CxeSettingsMapperSymbian::Map2CameraOrientation(DeviceOrientation uiOrientation)
{
  MCameraOrientation::TOrientation cameraOrientation;
  switch(uiOrientation) {
    case Orientation90:
	    cameraOrientation = MCameraOrientation::EOrientation90;
		break;
    case Orientation180:
	    cameraOrientation = MCameraOrientation::EOrientation180;
		break;
    case Orientation270:
	    cameraOrientation = MCameraOrientation::EOrientation270;
		break;
    case Orientation0: // default
    default:                  
	    cameraOrientation = MCameraOrientation::EOrientation0;
		break;
    }
  return cameraOrientation;
}

// end of file
