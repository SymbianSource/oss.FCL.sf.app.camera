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
 * cxesettings.h
 *
 *  Created on: Dec 30, 2008
 *
 */
#ifndef CXESETTINGSMAPPERSYMBIAN_H_
#define CXESETTINGSMAPPERSYMBIAN_H_

#include <ecam.h>
#include <ecamadvsettings.h>
#include <ECamOrientationCustomInterface2.h>
#include "cxeautofocuscontrol.h"
#include "cxenamespace.h"
/*
* Class to access all kind of Settings
*/
class CxeSettingsMapperSymbian
{

  public: // mappings functions that maps UI enum value to CCamera specific enum
    static CCamera::TWhiteBalance Map2CameraWb(int aWbId );
    static CCamera::TFlash Map2CameraFlash(int aFlashId );
    static CCamera::TExposure Map2CameraExposureMode(int aExpModeId );
    static CCamera::CCameraImageProcessing::TEffect Map2CameraEffect(int aColourFilterId );
    static CCamera::CCameraAdvancedSettings::TFocusRange Map2CameraAutofocus(CxeAutoFocusControl::Mode afMode);
	static MCameraOrientation::TOrientation Map2CameraOrientation(Cxe::DeviceOrientation orientation);
};

#endif /*CXESETTINGSMAPPERSYMBIAN_H_*/

// end  of file
