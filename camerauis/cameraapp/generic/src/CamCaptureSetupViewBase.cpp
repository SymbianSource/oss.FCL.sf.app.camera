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
* Description:  Base class for pre capture views*
*/


// INCLUDE FILES
#include <eikmenub.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamCaptureSetupViewBase.h"
#include "CamAppUiBase.h"
#include "CamAppController.h"
#include "CamUtility.h"
#include "CamCaptureSetupControlHandler.h"
#include "CamCaptureSetupContainer.h"
#include "CamShootingModeContainer.h"
#include "CamInfoListBoxContainer.h"
#include "CamPanic.h"
#include "Cam.hrh"
#include "CamZoomPane.h"
#include "CamTimeLapseUtility.h"
#include "camactivepalettehandler.h"
#include "CamAppUi.h"

#include <akntoolbar.h>

// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase destructor
// 
// ---------------------------------------------------------------------------
//
CCamCaptureSetupViewBase::~CCamCaptureSetupViewBase()
  {
  PRINT( _L("Camera => ~CCamCaptureSetupViewBase") );
  if( AppUi() )
    {
    if ( iCaptureSetupContainer )
      {
      AppUi()->RemoveFromStack( iCaptureSetupContainer );
      delete iCaptureSetupContainer;
      }
    if ( iSceneSettingContainer )
      {
      AppUi()->RemoveFromStack( iSceneSettingContainer );
      delete iSceneSettingContainer;
      iSceneSettingContainer = NULL;
      }
    if ( iInfoListBoxContainer )
      {
      AppUi()->RemoveFromStack( iInfoListBoxContainer );
      delete iInfoListBoxContainer;
      }        
    }
  PRINT( _L("Camera <= ~CCamCaptureSetupViewBase") );
  }

// -----------------------------------------------------------------------------
// CCamCaptureSetupViewBase::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::HandleCommandL( TInt aCommand )
    {
    PRINT( _L("Camera => CCamCaptureSetupViewBase::HandleCommandL") );
    TUid view = Id();
    
    TBool userSceneActive = ( view.iUid == ECamViewIdPhotoUserSceneSetup );
    
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    
    switch ( aCommand )
        {
        case ECamCmdCaptureSetupWhiteBalanceStill: // fallthrough
        case ECamCmdCaptureSetupColourFilterStill:
        case ECamCmdCaptureSetupExposureStill:
        case ECamCmdCaptureSetupWhiteBalanceVideo:
        case ECamCmdCaptureSetupColourFilterVideo:
        case ECamCmdCaptureSetupBrightnessStill:
        case ECamCmdCaptureSetupContrastStill:
        case ECamCmdCaptureSetupBrightnessVideo:
        case ECamCmdCaptureSetupContrastVideo:
        case ECamCmdCaptureSetupImageSharpnessStill:
        case ECamCmdCaptureSetupFlashStill:
        case ECamCmdCaptureSetupSelfTimer:
            {
            SwitchToCaptureSetupModeL( aCommand );
            }
            break;
        case ECamCmdCaptureSetupSceneStill:
        case ECamCmdCaptureSetupSceneVideo:
            {
            SwitchToSceneSettingModeL();
            
             
            }
            break;
        case EAknSoftkeyBack:
            {
            if( appUi && appUi->APHandler()->AccessedViaAP() )
                {
                if( !userSceneActive)
                	{
                	appUi->APHandler()->APOperationComplete();
                	}
                
                if ( iSceneSettingModeActive )
                    {
                    iSceneSettingContainer->SaveSceneSettingIfChangedL();
                    }
                
                iChangeButtonPressed = EFalse;
                ExitAllModesL();
                }
            else
                {
				if( iSceneSettingModeActive )
               	    {
	           	   	ExitSceneSettingModeL();
	           	    }	
	      		else if ( iInfoListBoxActive )
	           	    {
	           	   	ExitInfoListBoxL();
	           		}
                }
            }
            break;
        case EAknSoftkeyCancel:
            {
            if ( iCaptureSetupModeActive && iCaptureSetupControlHandler )
                {
                TCamSettingItemIds iSettingItemId = iCaptureSetupControlHandler->SettingType();		
                
                if ( iSettingItemId == ECamSettingItemDynamicSelfTimer )					//judge if current operation selfTimer
                  {
                  iController.SetSlideInSelfTimerPreview(EFalse);
                  TInt iSelfTimerVal = iController.IntegerSettingValueUnfiltered( ECamSettingItemDynamicSelfTimer );	//get the actual selfTimer value, not the focus one 
                       
                  iController.IntSettingChangedL(iSettingItemId, iSelfTimerVal);			//set the actual status to the actual index
                  }

                iController.CancelPreviewChangesL();
                // fallthrough to exit CaptureSetup mode
                }
            else if ( iInfoListBoxActive )
                {
                iController.CancelPreviewChangesL();
                ExitAllModesL();
                break;
                }
            else if ( iSceneSettingModeActive )
                {
                iChangeButtonPressed = EFalse;
                ExitAllModesL();
                break;
                }
            else
                {
                // do nothing, the command is handled by the framework
                break;
                }
            }
            //lint -fallthrough
        case EAknSoftkeyOk:
            {
            CaptureSetupModeSelection();
                
            }
            break;
        case ECamMSKCmdAppChange:
        case EAknSoftkeySelect:
            {
            if ( iCaptureSetupModeActive && iCaptureSetupControlHandler )
                {
                CaptureSetupModeSelection();
                // fallthrough to exit CaptureSetup mode
                }
            else if ( iSceneSettingContainer )
                {
                
                if ( iSceneSettingContainer->UserSceneHighlighted() )
                    {                  
                    // use context specific ok options menu if user scene is highlighted
                    CEikMenuBar* menuBar = MenuBar();            
                    if ( menuBar )
                        {
                        menuBar->SetMenuTitleResourceId( R_CAM_SCENE_SETTING_OK_MENUBAR );
                        MenuBar()->SetMenuType( CEikMenuBar::EMenuContext );
                        menuBar->TryDisplayMenuBarL();
                        menuBar->SetMenuTitleResourceId( R_CAM_SCENE_SETTING_MENUBAR );
                        //Here we again set back the type of menu to "Options" when pressed LSK
            			menuBar->SetMenuType( CEikMenuBar::EMenuOptions );
                        }
                    break;
                    }
                
                } 
            }
            //lint -fallthrough
        case ECamCmdSelect:
            {
            if ( iSceneSettingContainer )
                {
                // If successfully saved the scene change to the settings model
                // then exit the scene setting mode.
                if ( iSceneSettingContainer->SaveSceneSettingL() )
                    {
                    if( appUi && appUi->APHandler()->AccessedViaAP() )
                        {
                        if( !userSceneActive)
		                	{
		                	appUi->APHandler()->APOperationComplete();
		                	}
		                	
                        ExitAllModesL();
                        }
                    else
                        {
                    	ExitSceneSettingModeL();
                        }
                    }
                }
            else if( iInfoListBoxContainer )
            		{
            		HandleInfoListBoxSelectionL();	
 					}
            }
            break;
        case ECamCmdEdit:
            {
            // If 'change' user scene selected from scene list, exit all
            // modes in the precapture view and switch view to user scene.
            if ( iSceneSettingModeActive )
                {
                iChangeButtonPressed = ETrue;
            	AppUi()->HandleCommandL( ECamCmdUserScene );
            	AppUi()->StatusPane()->MakeVisible( ETrue );
                }
            
            }
            break;
        case ECamCmdUserSceneSelected:
            {
            // check as a precaution that there is a container and userscene is highlighted
            if ( iSceneSettingContainer && iSceneSettingContainer->UserSceneHighlighted() )
                {
                                
                // use context specific ok options menu if user scene is highlighted
                CEikMenuBar* menuBar = MenuBar();            
                if ( menuBar )
                    {
                    MenuBar()->SetMenuType( CEikMenuBar::EMenuContext );
                    menuBar->SetMenuTitleResourceId( R_CAM_SCENE_SETTING_OK_MENUBAR );
                    menuBar->TryDisplayMenuBarL();
                    menuBar->SetMenuType( CEikMenuBar::EMenuOptions );
                    menuBar->SetMenuTitleResourceId( R_CAM_SCENE_SETTING_MENUBAR );
                    }
                }
            
            break;
            }
        case ECamCmdInternalExit:
            {
            if ( appUi )
                {
                if ( !appUi->ZoomPane()->IsZoomAtMinimum() )
                    {
                    appUi->ZoomPane()->ResetZoomTo1x();
                    }
                }
            ExitAllModesL();
            CCamViewBase::HandleCommandL( aCommand );
            }
            break;
        case ECamCmdRedrawScreen:
            {
            if( iCaptureSetupContainer )
                {
                iCaptureSetupContainer->DrawDeferred();
                }
            CCamViewBase::HandleCommandL( aCommand );
            }
            break;
        default:
            {
            CCamViewBase::HandleCommandL( aCommand );
            break;
            }
        }
    PRINT( _L("Camera <= CCamCaptureSetupViewBase::HandleCommandL") );
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupViewBase::HandleFocusLossL
// Handle focus loss
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::HandleFocusLossL()
    {
	// We dont exit any of the modes when we loose focus
	// we do it when we gain focus.
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::DoDeactivate
// From CAknView deactivates the view
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::DoDeactivate()
    {
    if ( iSceneSettingContainer )
        {
        AppUi()->RemoveFromStack( iSceneSettingContainer );
    
        delete iSceneSettingContainer;
        iSceneSettingContainer = NULL;
        }
    CCamViewBase::DoDeactivate();
    }


// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::CCamCaptureSetupViewBase
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamCaptureSetupViewBase::CCamCaptureSetupViewBase( CCamAppController& aController )
    : CCamViewBase( aController ),
      iForceAvkonCBA(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::CreateCaptureSetupControlHandlerL
// Create a control handler for the correct capture setup item
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::CreateCaptureSetupControlHandlerL( TInt aSetupCommand )
    {
    __ASSERT_DEBUG( !iCaptureSetupControlHandler, CamPanic( ECamPanicInvalidState ) );
    TCamSettingItemIds settingItemId = ECamSettingItemDynamicPhotoWhiteBalance;
    switch ( aSetupCommand )
        {
        case ECamCmdCaptureSetupWhiteBalanceStill:
            {
            settingItemId = ECamSettingItemDynamicPhotoWhiteBalance;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
            }
            break;
        case ECamCmdCaptureSetupColourFilterStill:
            {
            settingItemId = ECamSettingItemDynamicPhotoColourFilter;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
            }
            break;

        case ECamCmdCaptureSetupExposureStill:
            {
            settingItemId = ECamSettingItemDynamicPhotoExposure;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
            }
            break;

        case ECamCmdCaptureSetupFlashStill:
            {
            settingItemId = ECamSettingItemDynamicPhotoFlash;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
            }
            break;
        case ECamCmdCaptureSetupSelfTimer:
            {
            settingItemId = ECamSettingItemDynamicSelfTimer;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
            }
            break;
            
        case ECamCmdCaptureSetupWhiteBalanceVideo:
            {
            settingItemId = ECamSettingItemDynamicVideoWhiteBalance;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_VIDEO;
            }
            break;
        case ECamCmdCaptureSetupColourFilterVideo:
            {
            settingItemId = ECamSettingItemDynamicVideoColourFilter;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_VIDEO;
            }
            break;

        // User scene settings pages.
        case ECamCmdCaptureSetupWhiteBalanceUser:
            {
            settingItemId = ECamSettingItemUserSceneWhitebalance;
            iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;
        case ECamCmdCaptureSetupColourFilterUser:
            {
            settingItemId = ECamSettingItemUserSceneColourFilter;
            iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;
        case ECamCmdCaptureSetupExposureUser:
            {
            settingItemId = ECamSettingItemUserSceneExposure;
            iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;
        case ECamCmdCaptureSetupFlashUser:
            {
            settingItemId = ECamSettingItemUserSceneFlash;
            iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;

		case ECamCmdCaptureSetupBrightnessStill:
			{
			settingItemId = ECamSettingItemDynamicPhotoBrightness;
			iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
			}
			break;
		case ECamCmdCaptureSetupContrastStill:
			{
			settingItemId = ECamSettingItemDynamicPhotoContrast;
			iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
			}
			break;
		case ECamCmdCaptureSetupBrightnessVideo:
			{
			settingItemId = ECamSettingItemDynamicVideoBrightness;
			iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_VIDEO;
			}
			break;
        case ECamCmdCaptureSetupContrastVideo:
			{
			settingItemId = ECamSettingItemDynamicVideoContrast;
			iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_VIDEO;
			}
			break;
        case ECamCmdCaptureSetupBrightnessUser:
            {
            settingItemId = ECamSettingItemUserSceneBrightness;
			iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;
        case ECamCmdCaptureSetupContrastUser:
            {
            settingItemId = ECamSettingItemUserSceneContrast;
			iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;

        case ECamCmdCaptureSetupImageSharpnessStill:
            {
            settingItemId = ECamSettingItemDynamicPhotoImageSharpness;
			iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
            }
            break;
        case ECamCmdCaptureSetupImageSharpnessUser:
            {
            settingItemId = ECamSettingItemUserSceneImageSharpness;
			iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;

        case ECamCmdCaptureSetupLightSensitivityStill:
            {
            settingItemId = ECamSettingItemDynamicPhotoLightSensitivity;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_STILL;
            }
            break;
        case ECamCmdCaptureSetupLightSensitivityVideo:
            {
            settingItemId = ECamSettingItemDynamicVideoLightSensitivity;
            iSettingModeTitleResourceId = R_CAM_CAPTURE_SETUP_TITLE_VIDEO;
            }
            break;
        case ECamCmdCaptureSetupLightSensitivityUser:
            {
            settingItemId = ECamSettingItemUserSceneLightSensitivity;
            iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;
        case ECamCmdSetUserDefault:
            { 
            settingItemId = ECamSettingItemUserSceneDefault;
            iSettingModeTitleResourceId = R_CAM_USER_SCENE_SETUP_TITLE;
            }
            break;
        default:
            {
            User::Leave( KErrNotSupported );
            }
            break;
        }

    iCaptureSetupControlHandler = new( ELeave )
        CCamCaptureSetupControlHandler( iController, settingItemId );
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::CleanupCaptureSetupContainer
// Cleanup the capture setup controls if a leave occurs
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::CleanupCaptureSetupContainer( TAny* aAny )
    {
    CCamCaptureSetupViewBase* view = static_cast<CCamCaptureSetupViewBase*>( aAny );
    delete view->iCaptureSetupControlHandler;
    view->iCaptureSetupControlHandler = NULL;

    if ( view->iCaptureSetupContainer )
        {
        view->AppUi()->RemoveFromStack( view->iCaptureSetupContainer );
        delete view->iCaptureSetupContainer;
        view->iCaptureSetupContainer = NULL;
        }
    // reset the active flag
    view->SetCaptureSetupModeActive(EFalse);

    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( view->AppUi() );
    appUi->SetPreCaptureMode(ECamPreCapViewfinder);
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::SwitchToCaptureSetupModeL
// Enter capture setup mode
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::SwitchToCaptureSetupModeL( TInt aSetupCommand, TBool aFullScreenVF )
    {
    __ASSERT_DEBUG( !iCaptureSetupControlHandler && !iCaptureSetupContainer, CamPanic( ECamPanicResourceLeak ) );

    CreateCaptureSetupControlHandlerL( aSetupCommand );
    // Cleanup the view correctly if a leave occurs
    CleanupStack::PushL( TCleanupItem( CleanupCaptureSetupContainer, this ) );
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );

    TRect rect = appUi->ApplicationRect();
    if ( !CamUtility::IsNhdDevice() ) 
        {
        // Space for visible statuspane for non-touch device
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        }
    iCaptureSetupContainer = CCamCaptureSetupContainer::NewL( iController, 
                                   *this, *iCaptureSetupControlHandler, rect, aFullScreenVF );
    appUi->AddToStackL( *this, iCaptureSetupContainer );
    iCaptureSetupContainer->ActivateL();
    iCaptureSetupContainer->DrawNow();

    // UpdateCbaL need this boolean to be set in order
    // to return the correct CBA
    SetCaptureSetupModeActive(ETrue);
    
    iCaptureSetupContainer->DrawableWindow()->SetOrdinalPosition(
            KCamPreCaptureWindowOrdinalPos );

    UpdateCbaL();
    SetTitlePaneTextL();
    appUi->PushDefaultNaviPaneL();
    CleanupStack::Pop(); // CleanupCaptureSetupContainer
    // after all leaving functions have completed
    // remove the container from the stack
    appUi->RemoveFromStack( iContainer );
    iContainer->MakeVisible( EFalse );
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::ExitCaptureSetupModeL
// Exit capture setup mode
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::ExitCaptureSetupModeL()
    {
    PRINT( _L("Camera => CCamCaptureSetupViewBase::ExitCaptureSetupModeL()") );	   
    if ( !iController.InVideocallOrRinging() )
        {
        iController.CancelPreviewChangesL();
        }
    if ( !iCaptureSetupModeActive )
		{
        return;
		}

    // Cleanup the view correctly if a leave occurs
    CleanupStack::PushL( TCleanupItem( CleanupExit, this ) );
    AppUi()->AddToStackL( *this, iContainer );
    iContainer->MakeVisible( ETrue );   
    //Draw already here to prevent the activepalette showing briefly before it is updated
    if ( !iController.IsTouchScreenSupported() )
        {
        iContainer->DrawNow();
        }
    SetCaptureSetupModeActive(EFalse);

    UpdateCbaL();

    if ( Cba() )
        {
        Cba()->DrawNow();
        }
		
    SetTitlePaneTextL();  
    CleanupStack::Pop(); // CleanupExit

    // delete the capture setup controls when all
    // leaving functions have completed
    AppUi()->RemoveFromStack( iCaptureSetupContainer );
    delete iCaptureSetupContainer;
    iCaptureSetupContainer = NULL;
    delete iCaptureSetupControlHandler;
    iCaptureSetupControlHandler = NULL;
    PRINT( _L("Camera <= CCamCaptureSetupViewBase::ExitCaptureSetupModeL()") );	   
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::CleanupSceneSettingContainer
// Cleanup the capture setup controls if a leave occurs
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::CleanupSceneSettingContainer( TAny* aAny )
    {
    CCamCaptureSetupViewBase* view = static_cast<CCamCaptureSetupViewBase*>( aAny );
    if ( view->iSceneSettingContainer )
        {
        view->AppUi()->RemoveFromStack( view->iSceneSettingContainer );
        delete view->iSceneSettingContainer;
        view->iSceneSettingContainer = NULL;
        }
    // reset the active mode
    view->SetSceneSettingMode(EFalse);
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::CleanupInfoListBox
// Cleanup the InfoListBox controls if a leave occurs
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::CleanupInfoListBox( TAny* aAny )
    {
    CCamCaptureSetupViewBase* view = static_cast<CCamCaptureSetupViewBase*>( aAny );
    if ( view->iInfoListBoxContainer )
        {
        view->AppUi()->RemoveFromStack( view->iInfoListBoxContainer );
        delete view->iInfoListBoxContainer;
        view->iInfoListBoxContainer = NULL;
        }
    // reset the active mode
    view->SetInfoListBoxMode(EFalse);
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::SwitchToSceneSettingModeL
// Enter scene setup mode
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::SwitchToSceneSettingModeL()
    {
    // Cleanup the view correctly if a leave occurs
    CleanupStack::PushL( TCleanupItem( CleanupSceneSettingContainer, this ) );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    iSceneSettingContainer->SetMopParent( this );  
    appUi->AddToStackL( *this, iSceneSettingContainer );
    iSceneSettingContainer->ActivateL();
    
    if ( iChangeButtonPressed && iSceneSettingContainer )
    	{
    	iSceneSettingContainer->SelectItem( iSceneSettingContainer->IndexForValueId( ECamSceneUser ) );
    	// iChangeButtonPressed is set to EFalse in SetSceneSettingMode method
    	}
    
    // UpdateCbaL need this boolean to be set in
    // order to return the correct CBA
    SetSceneSettingMode(ETrue);
    
    appUi->SetToolbarVisibility();
    
    iForceAvkonCBA = ETrue;
    UpdateCbaL();
    SetTitlePaneTextL();
    appUi->PushDefaultNaviPaneL();
    CleanupStack::Pop(); // CleanupSceneSettingContainer
    // after all leaving functions have completed
    // remove the container from the stack and
    // set the capture setup mode active 
    appUi->RemoveFromStack( iContainer );
    iContainer->MakeVisible( EFalse );  

    CEikMenuBar* menuBar = MenuBar();
    // if menuBar exists
    if ( menuBar )
        {
        menuBar->SetMenuTitleResourceId( ROID(R_CAM_SCENE_SETTING_MENUBAR_ID));
        }
    
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::SwitchToInfoListBoxL
// Show InfoListBox
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::SwitchToInfoListBoxL( TCamInfoListBoxMode aMode, TBool aFullySkinned )
    {
    // Cleanup the view correctly if a leave occurs
    CleanupStack::PushL( TCleanupItem( CleanupInfoListBox, this ) );
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    iInfoListBoxContainer->SetMopParent( this );  
    appUi->AddToStackL( *this, iInfoListBoxContainer );
    iInfoListBoxContainer->ActivateL();
    // UpdateCbaL need this boolean to be set in
    // order to return the correct CBA
    SetInfoListBoxMode(ETrue, aFullySkinned);
    UpdateCbaL();
    SetTitlePaneTextL();
    appUi->PushDefaultNaviPaneL();
    CleanupStack::Pop(); // CleanupSceneSettingContainer

    // after all leaving functions have completed
    // remove the container from the stack and
    // set the capture setup mode active 
    appUi->RemoveFromStack( iContainer );
    iContainer->MakeVisible( EFalse );  

    CEikMenuBar* menuBar = MenuBar();
    // if menuBar exists
    if ( menuBar )
        {
        // Use same menu resource as scene setting
        menuBar->SetMenuTitleResourceId( R_CAM_INFOLISTBOX_MENUBAR );
        }
    }


// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::HandleInfoListBoxSelectionL()
// Handle selection events from InfoListBox
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::HandleInfoListBoxSelectionL()
		{            		   
		CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );            		   
		
		TUid view = Id();           		   
		TBool userSceneActive = ( view.iUid == ECamViewIdPhotoUserSceneSetup );
		
		if( iInfoListBoxContainer->SettingValueChanged() )
				{
				// Store the selected setting
				switch( iInfoListBoxMode )
						{
						case EInfoListBoxModeTimeLapse:
								{
  		  				TCamTimeLapse settingValue = static_cast<TCamTimeLapse>( iInfoListBoxContainer->CurrentSettingItemValue() );
   			 				TInt command = CamTimeLapseUtility::EnumToCommand( settingValue ); 
		    				TTimeIntervalMicroSeconds interval = CamTimeLapseUtility::EnumToInterval( settingValue );  
            		
		    				// Update the current mode (single/burst/timelapse)
  		  				if( appUi )
    								{
 		   							TRAP_IGNORE( appUi->HandleCommandL( command ) );            		
   			     				}
						
								// Update timelapse interval
 			       		iController.SetTimeLapseInterval( interval ); 
  		     			}
  		     			break;
       			
						case EInfoListBoxModeISO:
								{
								TInt settingValue = iInfoListBoxContainer->CurrentSettingItemValue();
								
								// For the light sensitivity setting, PreviewSettingChangeL and CommitPreviewChanges
								// must be used, instead of UpdateEngineWithSettingL, in order to actually change
								// the setting value.
								iController.PreviewSettingChangeL( ECamSettingItemDynamicPhotoLightSensitivity, settingValue );
								iController.CommitPreviewChanges();
								}            	  
								break;
						
						default:
								break;												
						}				
				}
        	  
            	            			
    if( appUi && appUi->APHandler()->AccessedViaAP() )
    		{ 
    		if( !userSceneActive)
		    	{
		    	// If user scene is active, we aren't yet done
		      appUi->APHandler()->APOperationComplete();
		      }	
		                		                	
        ExitAllModesL();
        }
    else
    		{
        	ExitInfoListBoxL();
        	}                     
       
		}

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::CleanupExit
// Stop the scene setting mode from exiting if a leave occurs
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::CleanupExit( TAny* aAny )
    {
    CCamCaptureSetupViewBase* view = static_cast<CCamCaptureSetupViewBase*>( aAny );
    view->AppUi()->RemoveFromStack( view->iContainer );
    view->iContainer->MakeVisible( EFalse );
    if ( view->iSceneSettingContainer )
        {
        // reset the active mode
        view->SetSceneSettingMode(ETrue);
        }
    if ( view->iCaptureSetupContainer )
        {
        // reset the active mode
        view->SetCaptureSetupModeActive(ETrue);
        }
    if ( view->iInfoListBoxContainer )
        {
        // reset the active mode
        view->SetInfoListBoxMode(ETrue);
        }        
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::ExitSceneSettingModeL
// Exit scene setup mode
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::ExitSceneSettingModeL()
    {   
    PRINT( _L( "Camera => CCamCaptureSetupViewBase::ExitSceneSettingModeL" ) );
    iForceAvkonCBA = EFalse;
    if ( !iSceneSettingModeActive )
        {
        AppUi()->RemoveFromStack( iSceneSettingContainer );  
		
        if ( iSceneSettingContainer )
        	{
        	delete iSceneSettingContainer;
    	    iSceneSettingContainer = NULL;
        	}
		return;
        }
    	
    // Cleanup the view correctly if a leave occurs
    CleanupStack::PushL( TCleanupItem( CleanupExit, this ) );
    AppUi()->AddToStackL( *this, iContainer );
    
    if ( iChangeButtonPressed == EFalse )
    	{
        AppUi()->RemoveFromStack( iSceneSettingContainer );               
        if ( iSceneSettingContainer )
        	{
    	    delete iSceneSettingContainer;
    	    iSceneSettingContainer = NULL;
        	} 
        iContainer->MakeVisible( ETrue );
    	  //Draw already here to prevent the activepalette showing briefly before it is updated
        if ( !iController.IsTouchScreenSupported() )
            {
            iContainer->DrawNow();
            }
      	SetSceneSettingMode(EFalse);
        UpdateCbaL();

       if ( Cba() )
    	    {
    	    Cba()->DrawNow();
    	    }
	
        SetTitlePaneTextL();
        CleanupStack::Pop(); // CleanupExit
        

        
    	}
    else // iChangeButtonPressed == ETrue
    	{
    	iContainer->MakeVisible( EFalse );
    	SetSceneSettingMode(EFalse);
        SetTitlePaneTextL();
        CleanupStack::Pop(); // CleanupExit
    	}
    
    
    CEikMenuBar* menuBar = MenuBar();
    // if menuBar exists
    if ( menuBar )
        {
        menuBar->SetMenuTitleResourceId( iPreviousMenuResourceId );
        }
	PRINT( _L( "Camera <= CCamCaptureSetupViewBase::ExitSceneSettingModeL" ) );        
    }


// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::ExitInfoListBoxL
// Exit scene setup mode
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::ExitInfoListBoxL()
    {   
	PRINT( _L( "Camera => CCamCaptureSetupViewBase::ExitInfoListBoxL" ) );
    
    if ( !iInfoListBoxActive )
		{
        return;
    } 
    
    // Cleanup the view correctly if a leave occurs
    CleanupStack::PushL( TCleanupItem( CleanupExit, this ) );
    AppUi()->AddToStackL( *this, iContainer );
    iController.SetViewfinderWindowHandle( &iContainer->Window() );
  	PRINT( _L( "Camera => CCamCaptureSetupViewBase::ExitInfoListBoxL removing iInfoListBoxContainer" ) );
    AppUi()->RemoveFromStack( iInfoListBoxContainer );
    delete iInfoListBoxContainer;
    iInfoListBoxContainer = NULL;
	PRINT( _L( "Camera => CCamCaptureSetupViewBase::ExitInfoListBoxL removed iInfoListBoxContainer" ) );   
   
    iContainer->MakeVisible( ETrue );  
    //Draw already here to prevent the activepalette showing briefly before it is updated
    if ( !iController.IsTouchScreenSupported() )
        {
        iContainer->DrawNow();
        }
    SetInfoListBoxMode(EFalse);
    UpdateCbaL();

    if ( Cba() )
        {
        Cba()->DrawNow();
        }
		
    SetTitlePaneTextL();
    CleanupStack::Pop(); // CleanupExit

    CEikMenuBar* menuBar = MenuBar();
    // if menuBar exists
    if ( menuBar )
        {
        menuBar->SetMenuTitleResourceId( iPreviousMenuResourceId );
        }
        
		iInfoListBoxActive = EFalse;
		
		// StartViewFinderL();      
	PRINT( _L( "Camera <= CCamCaptureSetupViewBase::ExitInfoListBoxL" ) );        
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::SetupModeTitlePaneResourceId
// Return the resource id for the title pane text while in setup mode
// ---------------------------------------------------------------------------
//
TInt CCamCaptureSetupViewBase::SetupModeTitlePaneResourceId()
    {
    return iSettingModeTitleResourceId;
    }


void CCamCaptureSetupViewBase::CaptureSetupModeSelection()
    {
    
    TUid view = Id();
    
    TBool userSceneActive = ( view.iUid == ECamViewIdPhotoUserSceneSetup );
    
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    
    if ( iCaptureSetupModeActive && iCaptureSetupControlHandler )
        {
        TCamSettingItemIds iSettingItemId = iCaptureSetupControlHandler->SettingType();
        if ( iSettingItemId == ECamSettingItemDynamicSelfTimer )
            {
            iController.SetSlideInSelfTimerPreview(EFalse);
            }
    
        iController.CommitPreviewChanges();    
        // fallthrough to exit CaptureSetup mode
        }
    if( appUi && appUi->APHandler()->AccessedViaAP() )
       {
       if( !userSceneActive)
            {
            appUi->APHandler()->APOperationComplete();
            }
       ExitAllModesL();
       }
    
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::DynInitMenuPaneL
// Dynamically initialise the options menu
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    PRINT( _L("Camera => CCamCaptureSetupViewBase::DynInitMenuPaneL"))
    iController.StartIdleTimer();
    if( iSceneSettingModeActive && aResourceId == ROID(R_CAM_SCENE_SETTING_MENU_ID))
        {
        if ( iSceneSettingContainer && !iSceneSettingContainer->UserSceneHighlighted() )
            {
            aMenuPane->SetItemDimmed( ECamCmdEdit, ETrue );
            }
        else
            {
            // do nothing
            }
        }
    else
        {
        // do nothing
        }
    PRINT( _L("Camera <= CCamCaptureSetupViewBase::DynInitMenuPaneL"))
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::ExitAllModesL
// Revert to normal mode
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::ExitAllModesL()
    {
    // intentionally doing nothing
    PRINT( _L("Camera =><= CCamCaptureSetupViewBase::ExitAllModesL empty impl."))
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::SetSceneSettingMode
// Sets the iSceneSettingModeActive flag
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::SetSceneSettingMode(TBool aActive)
    {
    TUid view = Id();
    TBool userSceneActive = ( view.iUid == ECamViewIdPhotoUserSceneSetup );
    // This should be the only place iSceneSettingModeActive is set!
    // We need to inform the AppUi
    iSceneSettingModeActive = aActive;

    CCamAppUi* appUi = static_cast<CCamAppUi*>(AppUi()); //static_cast<CCamAppUiBase*>( AppUi() );
    
    if ( appUi )
        {
        if ( aActive )
            {
            appUi->SetPreCaptureMode(ECamPreCapSceneSetting);
            iChangeButtonPressed = EFalse; 
            } // userSceneActive to avoit toolbar flicker after back from scene setting
        else if ( iChangeButtonPressed || userSceneActive )
        	{
        	// transfering to user scene setup view
        	 appUi->SetPreCaptureMode(ECamPreCapSceneSetting);
        	}
        else
            {
           	appUi->SetPreCaptureMode(ECamPreCapViewfinder);
            }        
        }
          
    }
    
// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::SetInfoListBoxMode
// Sets the iInfoListBoxActive flag
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::SetInfoListBoxMode( TBool aActive, TBool aFullySkinned )
    {
    // We need to inform the AppUi
    iInfoListBoxActive = aActive;

    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    
    if ( appUi)
        {
        if ( aActive )
            {
            iPrevCaptureMode = appUi->PreCaptureMode();
            appUi->SetPreCaptureMode(ECamPreCapGenericSetting);
            }
        else
            {
            appUi->SetPreCaptureMode( iPrevCaptureMode );
            }        
        }
    }    
    
// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::SetCaptureSetupModeActive
// Sets the iSceneSettingModeActive flag
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupViewBase::SetCaptureSetupModeActive(TBool aActive)
    {
    // This should be the only place iCaptureSetupModeActive is set!
    // We need to inform the AppUi
    iCaptureSetupModeActive = aActive;
    PRINT1( _L("Camera <> CCamCaptureSetupViewBase::SetCaptureSetupModeActive iCaptureSetupModeActive=%d"),iCaptureSetupModeActive);
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    
    if ( appUi)
        {
        if ( aActive )
            {
            appUi->SetPreCaptureMode(ECamPreCapCaptureSetup);
            }
        else
            {
            appUi->SetPreCaptureMode(ECamPreCapViewfinder);
            }        
        }
    }
// ---------------------------------------------------------------------------
// CCamCaptureSetupViewBase::HandleForegroundEventL
// Called to notify of change of app fore/background state
// ---------------------------------------------------------------------------
//
 void CCamCaptureSetupViewBase::HandleForegroundEventL( TBool aForeground )
     {
     
     PRINT1( _L("Camera => CCamCaptureSetupViewBase::HandleForegroundEventL foreground:%d"), aForeground );
     
     if( aForeground )
         {
         
         if ( iCaptureSetupModeActive && iCaptureSetupContainer )
             {
             iCaptureSetupContainer->HandleForegroundEventL( aForeground );
             }
         }
     }
//  End of File  
