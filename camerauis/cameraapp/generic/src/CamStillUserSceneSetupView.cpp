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
* Description:  Ensures that the container for the user scene setup is constructed*
*/


// INCLUDE FILES
#include "CamStillUserSceneSetupView.h"
#include "CamUserSceneSetupContainer.h"
#include "CamShootingModeContainer.h"
#include "CamAppUiBase.h"
#include "Cam.hrh"
#include "CamAppController.h"
#include "CamUtility.h"

#include "CamAppUi.h"
#include "CamInfoListBoxContainer.h"
#include "CamTimeLapseUtility.h"
#include "CameraUiConfigManager.h"

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStillUserSceneSetupView* CCamStillUserSceneSetupView::NewLC( CCamAppController& aController )
    {
    CCamStillUserSceneSetupView* self = 
        new ( ELeave ) CCamStillUserSceneSetupView( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::~CCamStillUserSceneSetupView
// Destructor
// ---------------------------------------------------------------------------
//
CCamStillUserSceneSetupView::~CCamStillUserSceneSetupView()
    {
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::Id
// Returns the id of this view.
// ---------------------------------------------------------------------------
//
TUid CCamStillUserSceneSetupView::Id() const
    {
    return TUid::Uid( ECamViewIdPhotoUserSceneSetup );
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::CreateContainerL
// Creates the user scene setup list.
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::CreateContainerL()
    {
    iContainer = 
        CCamUserSceneSetupContainer::NewL( AppUi()->ClientRect(),
            R_CAM_PHOTO_USER_SCENE_SETUP_TITLE_NAME , iController, 
            *this );
    }




// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::StartViewFinder
// Enter viewfinder mode
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::StartViewFinder()
    {
    iController.EnterViewfinderMode( ECamControllerImage );
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::StopViewFinder
// Exit viewfinder mode
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::StopViewFinder()
    {
    iController.ExitViewfinderMode( ECamControllerImage );
    }


// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::ExitCaptureSetupModeL
// Exit capture setup mode
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::ExitCaptureSetupModeL()
    {
    static_cast<CCamUserSceneSetupContainer*> ( iContainer )->
        UpdateCurrentListItem();
    CCamUserSceneSetupViewBase::ExitCaptureSetupModeL();
    
    StatusPane()->MakeVisible(ETrue);
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::CCamStillUserSceneSetupView
// C++ Constructor
// ---------------------------------------------------------------------------
//
CCamStillUserSceneSetupView::CCamStillUserSceneSetupView
( CCamAppController& aController )
: CCamUserSceneSetupViewBase( aController )
    {
    }


// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::ConstructL
// 2nd phase Constructor
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::ConstructL()
    {
    BaseConstructL( ROID(R_CAM_USER_SCENE_SETUP_VIEW_ID));
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::ExitUserSceneSetupModeL
// Exits the user scene setup mode.
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::ExitUserSceneSetupModeL()
    {
    CCamUserSceneSetupViewBase::ExitUserSceneSetupModeL();
    AppUi()->HandleCommandL( ECamCmdSceneSettingList );
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::SwitchToUserSceneSetupModeL
// Switches to the user scene setup mode.
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::SwitchToUserSceneSetupModeL()
    {
    CCamUserSceneSetupViewBase::SwitchToUserSceneSetupModeL();
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::SwitchToSceneSettingModeL
// Switches the current mode to scene setting and activates a 
// specific control.
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::SwitchToSceneSettingModeL()
    {
    CCamUserSceneSetupViewBase::ExitUserSceneSetupModeL();

    if ( !iSceneSettingContainer )
    	{
	    // Create the scene setting container for still mode.
	    iSceneSettingContainer = CCamShootingModeContainer::NewL( AppUi()->ApplicationRect(),
	                                                             *this,
	                                                             ECamControllerImage,
	                                                             iController,
	                                                             ETrue );
    	}
    
    iSceneSettingContainer->DrawableWindow()->SetOrdinalPosition(-1);

    iSettingModeTitleResourceId = R_CAM_STILL_SCENE_SETTING_TITLE;

    iPreviousMenuResourceId = ROID(R_CAM_USER_SCENE_SETUP_MENUBAR_ID);

    // Remove the view's main container, and add the capture setup 
    // control associated with the input command to the container stack.
    CCamCaptureSetupViewBase::SwitchToSceneSettingModeL();
    }

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::SwitchToInfoListBoxL
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::SwitchToInfoListBoxL( TCamInfoListBoxMode aMode )
    {   
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iCoeEnv->AppUi() );  	

    TBool modeSelected = EFalse;
    TInt listBoxResource;
    TInt summaryResource;
    TInt initialValue;
    TInt titleResource;


    switch( aMode )
        {
        // Only ISO mode is supported for InfoListBox in user scene setup
        case EInfoListBoxModeISO:
            {
            // Light sensitivity selector initialization values	
            initialValue    = iController.IntegerSettingValue( ECamSettingItemUserSceneLightSensitivity );	
            // initialValue = 0;
			if ( iController.UiConfigManagerPtr()->IsExtendedLightSensitivitySupported() )
			    {
				listBoxResource = R_CAM_CAPTURE_SETUP_LIST_EXTENDED_LIGHT_SENSITIVITY;
				summaryResource = R_CAM_CAPTURE_SETUP_LIST_EXTENDED_LIGHT_SENSITIVITY_SUMMARY;
			    }
			else
			    {
				listBoxResource = R_CAM_CAPTURE_SETUP_LIST_LIGHT_SENSITIVITY;
				summaryResource = R_CAM_CAPTURE_SETUP_LIST_LIGHT_SENSITIVITY_SUMMARY;
			    }
            titleResource   = R_CAM_LIGHT_SENSITIVITY_TITLE;   					    				
            modeSelected    = ETrue;
            }
            break;
        default: 
            break;
        }    					

    if( modeSelected)
        {


        StatusPane()->MakeVisible( EFalse );

        CCamUserSceneSetupViewBase::ExitUserSceneSetupModeL();  

        iInfoListBoxContainer = CCamInfoListBoxContainer::NewL( AppUi()->ClientRect(),
                *this,                                                            
                iController,
                listBoxResource,
                summaryResource,
                initialValue,
                titleResource );		
        iInfoListBoxMode = aMode;          
        iSettingModeTitleResourceId = titleResource;                                                   										    			

        iPreviousMenuResourceId = ROID(R_CAM_USER_SCENE_SETUP_MENUBAR_ID);

        // Remove the view's main container, and add the capture setup 
        // control associated with the input command to the container stack.
        CCamCaptureSetupViewBase::SwitchToInfoListBoxL( aMode );        
        }
    }
    
// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::HandleInfoListBoxSelectionL()
// Handle selection events from InfoListBox
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::HandleInfoListBoxSelectionL()
	{            		   
	CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );            		   
		
	if( iInfoListBoxContainer->SettingValueChanged() )
		{
		// Store the selected setting
		switch( iInfoListBoxMode )
			{
			// Only ISO mode is supported for InfoListBox in user scene setup
			case EInfoListBoxModeISO:
				{
				TInt settingValue = iInfoListBoxContainer->CurrentSettingItemValue();
				iController.SetIntegerSettingValueL( ECamSettingItemUserSceneLightSensitivity, settingValue );	
				}            	  
				break;
						
		default:
				break;												
			}				
		}
    
  ExitInfoListBoxL();        	        
	}    

// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::ExitSceneSettingModeL
// Exit scene setting mode
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::ExitSceneSettingModeL()
  {
  static_cast<CCamUserSceneSetupContainer*>( iContainer )->
      UpdateCurrentListItem();
  CCamUserSceneSetupViewBase::ExitSceneSettingModeL();
  }


// ---------------------------------------------------------------------------
// CCamStillUserSceneSetupView::DynInitMenuPaneL
// Dynamically initialise the options menu
// ---------------------------------------------------------------------------
//
void CCamStillUserSceneSetupView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    PRINT( _L("Camera => CCamStillUserSceneSetupView::DynInitMenuPaneL"))
    if ( aResourceId != R_CAM_USER_SCENE_SETUP_MENU )
        {
        CCamCaptureSetupViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );        
        }
    PRINT( _L("Camera <= CCamStillUserSceneSetupView::DynInitMenuPaneL"))
    }


// End of File  
