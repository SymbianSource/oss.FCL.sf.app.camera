/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for creating, initializing and observing*
*/



// INCLUDE FILES
 
#include "CamCaptureSetupControlHandler.h"
#include "CamAppController.h"
#include "CamLogger.h"

#include "CamCaptureSetupListBox.h"
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include "CamCaptureSetupSlider.h"

#include "CamUtility.h"
#include "CameraUiConfigManager.h"

#include <AknUtils.h>

// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::CCamCaptureSetupControlHandler
// Constructor
// -----------------------------------------------------------------------------
//
CCamCaptureSetupControlHandler::CCamCaptureSetupControlHandler( 
                                             CCamAppController& aController,
                                             TCamSettingItemIds aSettingType )
: iController( aController ), iSettingType( aSettingType)
    {   
    }
    
// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler Destructor
// 
// -----------------------------------------------------------------------------
//
CCamCaptureSetupControlHandler::~CCamCaptureSetupControlHandler()
  {
  PRINT( _L("Camera => ~CCamCaptureSetupControlHandler") );
  PRINT( _L("Camera <= ~CCamCaptureSetupControlHandler") );
  }


// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::CreateCaptureSetupControlL
// Create a new capture setup control with self as observer.
// Ownership is transferred to the calling class.
// -----------------------------------------------------------------------------
//
CCoeControl* CCamCaptureSetupControlHandler::CreateCaptureSetupControlL( const CCoeControl* aParent,
        TBool aSkinnedBackGround )
    {
    // create a new control determined by iSettingType.
    // set self as observer
	switch( iSettingType )
		{
		case ECamSettingItemDynamicPhotoExposure:
		case ECamSettingItemUserSceneExposure:
		case ECamSettingItemDynamicPhotoBrightness:
		case ECamSettingItemDynamicPhotoContrast:
		case ECamSettingItemDynamicVideoBrightness:
		case ECamSettingItemDynamicVideoContrast:
        case ECamSettingItemUserSceneBrightness:
        case ECamSettingItemUserSceneContrast:
			{
			// create and return a slider control
            CCamCaptureSetupSlider* captureSetupControl = 
					CCamCaptureSetupSlider::NewL( aParent, this, iSettingType, 0, aSkinnedBackGround );
            CleanupStack::PushL( captureSetupControl );                    
            captureSetupControl->InitializeL
                ( iController.IntegerSettingValue( iSettingType ) );
            CleanupStack::Pop( captureSetupControl );

            // pass ownership of the control to the calling class
            return captureSetupControl;  
			}
		default:
			{
			TInt resourceId = ResourceForControl();
			if ( resourceId != 0 )
				{
				// create and return a radio-button listbox    
				CCamCaptureSetupListBox* captureSetupControl = 
						new( ELeave ) CCamCaptureSetupListBox( this,
										iController, aSkinnedBackGround ); 

				CleanupStack::PushL( captureSetupControl );
				captureSetupControl->ConstructL( aParent, resourceId, iSettingType );
				captureSetupControl->InitializeL
					( iController.IntegerSettingValue( iSettingType ) ); 
				CleanupStack::Pop( captureSetupControl );
                if( iSettingType == ECamSettingItemDynamicPhotoColourFilter ||
                    iSettingType == ECamSettingItemDynamicPhotoWhiteBalance ||
                    iSettingType == ECamSettingItemDynamicPhotoImageSharpness || 

                    iSettingType == ECamSettingItemDynamicVideoColourFilter ||
                    iSettingType == ECamSettingItemDynamicVideoWhiteBalance || 
                    iSettingType == ECamSettingItemDynamicVideoBrightness ||

                    iSettingType == ECamSettingItemUserSceneImageSharpness ||
                    iSettingType == ECamSettingItemUserSceneWhitebalance ||
                    iSettingType == ECamSettingItemUserSceneColourFilter )
                    {
                    captureSetupControl->DisableSingleClick( ETrue );
                    }
				// pass ownership of the control to the calling class
				return captureSetupControl;
				}
			}
			break;
		}
    return NULL;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::HandleSettingValueUpdateL
// Update the camera setting to the new value
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupControlHandler::HandleSettingValueUpdateL( TInt aNewValue )
    {
    PRINT( _L("Camera => CCamCaptureSetupControlHandler::HandleSettingValueUpdateL" ))
    iController.PreviewSettingChangeL( iSettingType, aNewValue );
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::ControlTitleResourceId
// Return the resource id of the text to display in the title pane
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupControlHandler::ControlTitleResourceId() const
    {
    TInt resourceId = 0;
    switch ( iSettingType )
        {
        case ECamSettingItemDynamicPhotoWhiteBalance: // fallthrough
        case ECamSettingItemDynamicVideoWhiteBalance:
        case ECamSettingItemUserSceneWhitebalance:
            {
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_WHITEBALANCE;
            }
            break;
        case ECamSettingItemDynamicVideoColourFilter: // fallthrough
        case ECamSettingItemDynamicPhotoColourFilter:
        case ECamSettingItemUserSceneColourFilter:
            {
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_COLOUREFFECT;
            }
            break;

        case ECamSettingItemDynamicPhotoExposure:
        case ECamSettingItemUserSceneExposure:
            {
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_EV;
            }
            break;

        case ECamSettingItemDynamicPhotoFlash:
        case ECamSettingItemUserSceneFlash:
            {
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_FLASH;
            }
            break;
		case ECamSettingItemDynamicPhotoBrightness:
		case ECamSettingItemDynamicVideoBrightness:
        case ECamSettingItemUserSceneBrightness:
			{
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_BRIGHTNESS;
            }
			break;
		case ECamSettingItemDynamicPhotoContrast:
		case ECamSettingItemDynamicVideoContrast:
        case ECamSettingItemUserSceneContrast:
			{
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_CONTRAST;
            }
			break;
        case ECamSettingItemDynamicPhotoImageSharpness:
        case ECamSettingItemUserSceneImageSharpness:
            {
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_IMAGESHARPNESS;
            }
            break;
        case ECamSettingItemDynamicSelfTimer:
            {
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_SELF_TIMER;
            break;
            }
        case ECamSettingItemUserSceneLightSensitivity:
        case ECamSettingItemDynamicPhotoLightSensitivity:
        case ECamSettingItemDynamicVideoLightSensitivity:
            {
            resourceId = R_CAM_CAPTURE_SETUP_TITLE_NAME_LIGHT_SENSITIVITY;
            }
            break;
        default:
            break;
        }
    return resourceId;
    }


// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::ControlPositionResourceId
// Return the resource id for the control's position.
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupControlHandler::ControlPositionResourceId() const
    {
    TInt resourceId = 0;
    switch ( iSettingType )
        {
        case ECamSettingItemDynamicPhotoWhiteBalance: // fallthrough
        case ECamSettingItemDynamicVideoWhiteBalance:
        case ECamSettingItemDynamicVideoColourFilter: 
        case ECamSettingItemDynamicPhotoColourFilter:
        case ECamSettingItemDynamicPhotoFlash:
        case ECamSettingItemDynamicSelfTimer:
        case ECamSettingItemUserSceneWhitebalance:    
        case ECamSettingItemUserSceneColourFilter:
        case ECamSettingItemUserSceneFlash:
        case ECamSettingItemDynamicPhotoImageSharpness:
        case ECamSettingItemUserSceneImageSharpness:
        case ECamSettingItemUserSceneLightSensitivity:
        case ECamSettingItemDynamicPhotoLightSensitivity:
        case ECamSettingItemDynamicVideoLightSensitivity:
            {
            if ( !AknLayoutUtils::LayoutMirrored() )
                {
                resourceId = ROID(R_CAM_CAPTURE_SETUP_CONTROL_POSITION_ID);
                }
            else
                {
                resourceId = ROID(R_CAM_CAPTURE_SETUP_CONTROL_POSITION_AH_ID);
                }
            }
            break;

        case ECamSettingItemDynamicPhotoExposure:     // fallthrough
        case ECamSettingItemUserSceneExposure:

		case ECamSettingItemDynamicPhotoBrightness:
		case ECamSettingItemDynamicVideoBrightness:
		case ECamSettingItemDynamicPhotoContrast:
		case ECamSettingItemDynamicVideoContrast:
        case ECamSettingItemUserSceneBrightness:
        case ECamSettingItemUserSceneContrast:

            {
            if ( !AknLayoutUtils::LayoutMirrored() )
                {
                resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_CONTROL_POSITION_ID);
                }
            else
                {
                resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_CONTROL_POSITION_AH_ID);
                }
            break;
            }
        default:
            break;
        }
    return resourceId;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::ControlUsesViewFinder
// Whether or not the control requires a viewfinder preview pane
// -----------------------------------------------------------------------------
//
TBool CCamCaptureSetupControlHandler::ControlUsesViewFinder() const
    {
    return ETrue;
    }


// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::ControlIsListBox
// Whether or not the control is of listbox type
// -----------------------------------------------------------------------------
//
TBool CCamCaptureSetupControlHandler::ControlIsListBox() const
    {
    return ( ( iSettingType != ECamSettingItemDynamicPhotoExposure ) 
        && ( iSettingType != ECamSettingItemUserSceneExposure )    
        && ( iSettingType != ECamSettingItemDynamicPhotoBrightness )    
        && ( iSettingType != ECamSettingItemDynamicPhotoContrast )    
        && ( iSettingType != ECamSettingItemDynamicVideoBrightness )    
        && ( iSettingType != ECamSettingItemDynamicVideoContrast )    
        && ( iSettingType != ECamSettingItemUserSceneBrightness )    
        && ( iSettingType != ECamSettingItemUserSceneContrast )    
           );
    }


// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::ControlIsSlider
// Whether or not the control is of slider type
// -----------------------------------------------------------------------------
//
TBool CCamCaptureSetupControlHandler::ControlIsSlider() const
    {
    return ( ( iSettingType == ECamSettingItemDynamicPhotoExposure ) 
        || ( iSettingType == ECamSettingItemUserSceneExposure )    
        || ( iSettingType == ECamSettingItemDynamicPhotoBrightness )    
        || ( iSettingType == ECamSettingItemDynamicPhotoContrast )    
        || ( iSettingType == ECamSettingItemDynamicVideoBrightness )    
        || ( iSettingType == ECamSettingItemDynamicVideoContrast )    
        || ( iSettingType == ECamSettingItemUserSceneBrightness )    
        || ( iSettingType == ECamSettingItemUserSceneContrast )    
           );
    }


// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::ResourceForControl
// Return the resource for list population
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupControlHandler::ResourceForControl() const
    {
    TInt resourceId = 0;
    switch ( iSettingType )
        {
        case ECamSettingItemDynamicPhotoWhiteBalance:// fallthrough
        case ECamSettingItemUserSceneWhitebalance:
            resourceId = ROID(R_CAM_CAPTURE_SETUP_LIST_STILL_WHITEBALANCE_ID);
            break;
        case ECamSettingItemDynamicPhotoColourFilter:// fallthrough
        case ECamSettingItemUserSceneColourFilter:
            resourceId = ROID(R_CAM_CAPTURE_SETUP_LIST_STILL_COLOUREFFECT_ID);
            break;
        case ECamSettingItemDynamicPhotoFlash:// fallthrough
        case ECamSettingItemUserSceneFlash:
            resourceId = ROID(R_CAM_CAPTURE_SETUP_LIST_STILL_FLASH_ID);
            break;
        case ECamSettingItemDynamicVideoWhiteBalance:
            resourceId = ROID(R_CAM_CAPTURE_SETUP_LIST_VIDEO_WHITEBALANCE_ID);
            break;
        case ECamSettingItemDynamicVideoColourFilter:
            resourceId = ROID(R_CAM_CAPTURE_SETUP_LIST_VIDEO_COLOUREFFECT_ID);
            break;
        case ECamSettingItemDynamicPhotoImageSharpness:
        case ECamSettingItemUserSceneImageSharpness:
        	resourceId = ROID(R_CAM_CAPTURE_SETUP_LIST_PHOTO_IMAGESHARPNESS_ID);
            break;
        case ECamSettingItemDynamicSelfTimer:
            resourceId = ROID(R_CAM_CAPTURE_SETUP_SELF_TIMER_ID); 
            break;
        case ECamSettingItemUserSceneLightSensitivity:
        case ECamSettingItemDynamicPhotoLightSensitivity:
        case ECamSettingItemDynamicVideoLightSensitivity:
            if ( iController.UiConfigManagerPtr()->IsExtendedLightSensitivitySupported()) 
                {
    			resourceId = R_CAM_CAPTURE_SETUP_STILL_EXTENDED_LIGHT_SENSITIVITY; 
                }
            else
                {
    			resourceId = R_CAM_CAPTURE_SETUP_STILL_LIGHT_SENSITIVITY; 
                }
			break;
        default:
            break;
        }
    return resourceId;
    }


// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::ViewfinderResourceId
// Return the viewfinder resource to use for the appropriate control
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupControlHandler::ViewfinderResourceId() const
    {
	TInt resourceId = 0;
    switch ( iSettingType )
        {
		case ECamSettingItemDynamicPhotoBrightness:
		case ECamSettingItemDynamicVideoBrightness:
		case ECamSettingItemDynamicPhotoContrast:
		case ECamSettingItemDynamicVideoContrast:
        case ECamSettingItemUserSceneBrightness:
        case ECamSettingItemUserSceneContrast:
        case ECamSettingItemDynamicPhotoExposure:
        case ECamSettingItemDynamicVideoExposure:
			{
			if ( !AknLayoutUtils::LayoutMirrored() )
                {
				resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_CONTVF_RECT_ID);
				}
			else
				{
				resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_CONTVF_RECT_AH_ID);
				}
			}
			break;
		default:
			{
			if ( !AknLayoutUtils::LayoutMirrored() )
                {
				resourceId = ROID(R_CAM_CAPTURE_SETUP_CONTVF_RECT_ID);
				}
			else
				{
				resourceId = ROID(R_CAM_CAPTURE_SETUP_CONTVF_RECT_AH_ID);
				}
			}
			break;
		}
	return resourceId;
	}

// -----------------------------------------------------------------------------
// CCamCaptureSetupControlHandler::SettingType
// Identifies which setting is being edited
// -----------------------------------------------------------------------------
//
TCamSettingItemIds CCamCaptureSetupControlHandler::SettingType() const
    {
    return iSettingType;
    }


// End of file
