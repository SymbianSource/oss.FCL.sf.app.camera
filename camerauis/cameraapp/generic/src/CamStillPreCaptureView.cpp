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
* Description:  Still image pre-capture view class for Camera application*
*/


// INCLUDE FILES
#include "CamAppUi.h"
#include "CamAppController.h"
#include "CamStillPreCaptureView.h"
#include "CamLogger.h"
#include "CamUtility.h"
#include "CamContainerBase.h"
#include "CamAppUiBase.h"
#include <eikmenub.h>
#include <eikapp.h>
#include <avkon.rsg>
#include "CamCaptureSetupMenu.h"
#include "CamPanic.h"
#include "Cam.hrh"
#include "CamAppUid.h"
#include <csxhelp/lcam.hlp.hrh>
#include "CamShootingModeContainer.h"
#include "CamInfoListBoxContainer.h"
#include "CamStillPreCaptureContainer.h"
#include "CamSelfTimer.h"
#include "CamTimeLapseUtility.h"
#include <CaeEngine.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <aknbutton.h>
#include <AknsUtils.h>
#include <cameraapp.mbg>
#include <AknIconUtils.h>
#include <gulicon.h>


#include "StringLoader.h"
#include "camactivepalettehandler.h"
#include "CameraUiConfigManager.h"
#include "CamSidePane.h"

#include "CamLocalViewIds.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamStillPreCaptureViewTraces.h"
#endif


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStillPreCaptureView* CCamStillPreCaptureView::NewLC( CCamAppController& aController )
    {
    CCamStillPreCaptureView* self = 
        new( ELeave ) CCamStillPreCaptureView( aController );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView destructor
// 
// ---------------------------------------------------------------------------
//
CCamStillPreCaptureView::~CCamStillPreCaptureView()
  {
  }
  
// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::Id
// Returns UID of view
// ---------------------------------------------------------------------------
//
TUid CCamStillPreCaptureView::Id() const
    {
   	return TUid::Uid( ECamViewIdStillPreCapture );	
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::DoDeactivate()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMSTILLPRECAPTUREVIEW_DODEACTIVATE, "e_CCamStillPreCaptureView_DoDeactivate 1" );
    PERF_EVENT_START_L2( EPerfEventStillPreCaptureViewDeactivation );    
    PRINT( _L("Camera => CCamStillPreCaptureView::DoDeactivate" ) )

    CCamPreCaptureViewBase::DoDeactivate();
    PERF_EVENT_END_L2( EPerfEventStillPreCaptureViewDeactivation );
    PRINT( _L("Camera <= CCamStillPreCaptureView::DoDeactivate" ) )
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMSTILLPRECAPTUREVIEW_DODEACTIVATE, "e_CCamStillPreCaptureView_DoDeactivate 0" );
    }

// -----------------------------------------------------------------------------
// CCamStillPreCaptureView::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamStillPreCaptureView::HandleCommandL( TInt aCommand )
    {
    PRINT( _L("Camera => CCamStillPreCaptureView::HandleCommandL") );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    TCamOrientation orientation = appUi->CamOrientation();
    switch ( aCommand )
        {
        case ECamMSKCmdSelect:
        case ECamMSKCmdAppCapture:
        	{
        	if( orientation == ECamOrientationCamcorder || 
        	    orientation == ECamOrientationCamcorderLeft ||
				orientation == ECamOrientationPortrait )
        		{
    		    if ( !StartMskCaptureL() )
        		    {
            		// If in the Landscape mode, MSK event is mapped 
            	  	// like a normal selection key so that AP items
            	  	// are selected and respective setting pages are launched
            	    TKeyEvent aKeyEvent;
    	    		aKeyEvent.iCode =  aKeyEvent.iRepeats = aKeyEvent.iModifiers = 0;
        			aKeyEvent.iScanCode = EStdKeyDevice3;
            		static_cast<CCamStillPreCaptureContainer*>
            				( Container() )->OfferKeyEventL( aKeyEvent, EEventKey );	
        		    }
            	}
            else if ( aCommand == ECamMSKCmdAppCapture )
               // if the Camera in the Portrait mode
          	   // MSK event is handled like a capture key
          		{
          		OstTrace0( CAMERAAPP_PERFORMANCE, DUP8_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHUTTER_RELEASE_LAG 1" );
          		if ( iController.IsViewFinding() && appUi->CheckMemoryL() )
                	{
                    OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_APP_SHOT_TO_SNAPSHOT 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHOT_TO_SNAPSHOT 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHOT_TO_SAVE 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHOT_TO_SHOT 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_APP_SHOT_TO_STILL 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_APP_CAPTURE_START 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP6_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SERIAL_SHOOTING 1" );
                	SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
                	iController.Capture();
                	}
          		}
            break;
        	}
        case ECamCmdCaptureImage:
            {
      	    OstTrace0( CAMERAAPP_PERFORMANCE, DUP10_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHUTTER_RELEASE_LAG 1" );
      	    if ( iController.IsViewFinding() && appUi->CheckMemoryL() &&
                !iController.CaptureModeTransitionInProgress() &&
                !iController.IsProcessingCapture() ) 
            	{
            	SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );

            	// fixed toolbar is used only with touch devices
            	if ( iController.IsTouchScreenSupported() )
            	    {
                    CAknToolbar* fixedToolbar = Toolbar();
                    if ( fixedToolbar )
                        {
                        fixedToolbar->SetToolbarVisibility( EFalse );
                        }
            	    }
            	
                // Start the autofocus operation now, the capture will be queued
                // after focus completes
                if ( !appUi->SelfTimerEnabled() 
                    && iController.ActiveCamera() != ECamActiveCameraSecondary
                    && iController.UiConfigManagerPtr()
                    && iController.UiConfigManagerPtr()->IsAutoFocusSupported()
                    && !iController.CurrentSceneHasForcedFocus() )
                    {
                    // Next trace should be just before "e_CAM_PRI_SHUTTER_RELEASE_LAG 1", but only when AF is supported
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP9_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_AF_LOCK 1" );
                    if( iController.CurrentOperation() == ECamNoOperation )
                        {
                        iController.SetAfNeeded( ETrue );
                        }    
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP7_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_APP_AF 1" );
                    iController.StartAutoFocus();
                    }
                else // AF not needed, capture will start next
                    {
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP11_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_APP_SHOT_TO_SNAPSHOT 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP12_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHOT_TO_SNAPSHOT 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP13_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHOT_TO_SAVE 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP14_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SHOT_TO_SHOT 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP15_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_APP_SHOT_TO_STILL 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP16_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_APP_CAPTURE_START 1" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP17_CCAMSTILLPRECAPTUREVIEW_HANDLECOMMANDL, "e_CAM_PRI_SERIAL_SHOOTING 1" );
                    }
            	TKeyEvent keyEvent;
            	appUi->StartCaptureL( keyEvent );

            	}
            break;
            }
        case ECamCmdTimeLapseSlider:
            {               
            TCamTimeLapse settingValue = ECamTimeLapseOff;

            if ( appUi && !appUi->IsBurstEnabled() )
                {
                settingValue = ECamTimeLapseMin;
                }                        

            TInt command = CamTimeLapseUtility::EnumToCommand( settingValue ); 
            TTimeIntervalMicroSeconds interval = CamTimeLapseUtility::EnumToInterval( settingValue );  

            // Update the current mode (single/burst/timelapse)
            if( appUi )
                {
                TRAP_IGNORE( appUi->HandleCommandL( command ) );                    
                }

            // Update timelapse interval
            iController.SetTimeLapseInterval( interval ); 

            // stop and start the viewfinder in order to update the settings
            StopViewFinder();
            StartViewFinder();
            
            // fixed toolbar is used only with touch devices
            if ( iController.IsTouchScreenSupported() )
                {
                UpdateToolbarIconsL();
                }
            
            }     
            break;
            
        case ECamCmdCaptureSetupLightSensitivityStill:
        // case ECamCmdCaptureSetupLightSensitivityVideo:
        	{
        	SwitchToInfoListBoxL( EInfoListBoxModeISO );
        	}
        	break;        	
   
        case ECamCmdGoToStandby:
            {
            // SwitchToStandbyModeL( ECamViewIdStillPreCapture, iStandbyError );
            SwitchToStandbyModeL( ECamViewIdStillPreCapture, appUi->StandbyStatus() );
            }
            break;
        case ECamCmdExitStandby:
            {
            ExitStandbyModeL();
            }
            break;
        case ECamCmdViewfinderGrid:
            {
            // Switch grid setting
            CCamPreCaptureViewBase::HandleCommandL( aCommand );
			UpdateToolbarIconsL();
            }
            break;			
        case ECamCmdToggleFacetracking: 
            {
             // Switch facetracking setting
             CCamPreCaptureViewBase::HandleCommandL( aCommand );
             UpdateToolbarIconsL();
            
            break;
           }    
        default:
            {
            CCamPreCaptureViewBase::HandleCommandL( aCommand ); 
            }
        }
    PRINT( _L("Camera <= CCamStillPreCaptureView::HandleCommandL") );
    }   

// -----------------------------------------------------------------------------
// CCamStillPreCaptureView::HandleForegroundEventL
// Handle foreground event
// -----------------------------------------------------------------------------
//
void 
CCamStillPreCaptureView::HandleForegroundEventL( TBool aForeground )
  {
  PRINT( _L("Camera => CCamStillPreCaptureView::HandleForegroundEventL") );

  CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );

  // -------------------------------------------------------
  // Foreground
  //
  // reset menu and cba as appropriate
  if( aForeground )
    {
    PRINT( _L("Camera <> CCamStillPreCaptureView::HandleForegroundEventL: to foreground") );
    PRINT1( _L("Camera <> CCamStillPreCaptureView .. Current controller operation [%s]"), KCamCaptureOperationNames[iController.CurrentOperation()] );
    PRINT1( _L("Camera <> CCamStillPreCaptureView .. Current controller mode      [%s]"), KCamModeNames[iController.CurrentMode()] );
    if( ECamViewStateStandby == appUi->CurrentViewState() )
      {
      CAknToolbar* fixedToolbar = appUi->CurrentFixedToolbar();
      if( fixedToolbar )
        {
        fixedToolbar->SetToolbarVisibility( EFalse );    
        }
      }


    if( iController.SequenceCaptureInProgress() )
      {
      PRINT( _L("Camera <= CCamStillPreCaptureView::HandleForegroundEventL: sequence capture ongoing") );
      return;
      }

    if ( appUi->IsInPretendExit() )
      {
      PRINT( _L("Camera <= CCamStillPreCaptureView::HandleForegroundEventL: app in pretend exit so not grabbing resources") )
      // The view can get a foreground event while the application is
      // actually in a pretend exit situation. This occurs when the view switch
      // was called before the exit event, but didn't complete until after the
      // exit event. In this case the view should not register an interest in
      // the engine as the application is really in the background and the resources
      // need to be released
      return;
      }
    // if foreground event is received while in videocall, go to standby with error
    if ( iController.InVideocallOrRinging() && ECamNoOperation == iController.CurrentOperation() )
        {
        ExitAllModesL();
        appUi->SetStandbyStatus( KErrInUse );
        appUi->HandleCommandL( ECamCmdGoToStandby );
        SetTitlePaneTextL();	        
        return;
        }
        
    // check for silent profile whenever we come to foreground, we dont need it here,
    // but we keep track of this in appcontroller.
    iController.IsProfileSilent();

    // set embedded flag here
    iEmbedded = appUi->IsEmbedded();
    // make sure CBA is correct
    UpdateCbaL();

    SetMenuBar();
    
    if ( iContinueInBackground && !iStandbyModeActive )
        {
        // make sure that CCamAppController is in view finder mode
        if ( iController.CurrentImageMode() == ECamImageCaptureNone )
            {
            StartViewFinder();
            }
        }
    }
  // -------------------------------------------------------
  // Background
  //
  // If going to the background, cancel any current autofocus.
  // This makes sure that the focus indicator is removed from the side pane
  // if the key release event is lost.
  // Cancelling focus does nothing if a capture has already been requested
  else
    {
    PRINT( _L("Camera <> CCamStillPreCaptureView::HandleForegroundEventL: to background") );
    // Cancel any outstanding capture
    iController.CancelFocusAndCapture();

    // stop any current sequence capture unless it is only a notification
    if( iController.SequenceCaptureInProgress() 
     && ( appUi->AppInBackground( EFalse ) 
       || appUi->ForegroundAppIsPhoneApp()
        )
      )
      {
      iController.StopSequenceCaptureL();
      }
     /*
    // stop any current sequence capture unless it is only an eikon server or AknCapServer window
    if ( iCoeEnv->WsSession().GetFocusWindowGroup() != iEikonServerWindowGroupId
        && iController.SequenceCaptureInProgress() )
      {
      TInt groupId = iCoeEnv->WsSession().GetFocusWindowGroup();
      if ( !CamUtility::IdMatchesName( groupId, KAknCapServer ) )
        {
        // If AknCapServer has NOT got foreground, (which would possibly 
        // indicate a "charging" note) stop sequence capture
        iController.StopSequenceCaptureL();
        }
      }
    */
    }

  CCamPreCaptureViewBase::HandleForegroundEventL( aForeground );

  PRINT( _L("Camera <= CCamStillPreCaptureView::HandleForegroundEventL") );
  }
    
// -----------------------------------------------------------------------------
// CCamStillPreCaptureView::HandleFocusLossL
// Handle change of focus
// -----------------------------------------------------------------------------
//
void CCamStillPreCaptureView::HandleFocusLossL()
    {
    PRINT( _L( "Camera => CCamStillPreCaptureView::HandleFocusLossL" ) );    
    // if the application was already in the background, but behind an
    // eikon server window, then handle backgrounding properly now.
    // Otherwise the backgrounding will be handled in the normal
    // HandleForeground() method.
    if ( iContinueInBackground )
        {
        if ( iController.SequenceCaptureInProgress() )
            {
            iController.StopSequenceCaptureL();
            }
        }
    CCamPreCaptureViewBase::HandleFocusLossL();
    PRINT( _L( "Camera <= CCamStillPreCaptureView::HandleFocusLossL" ) );    
    }    

// -----------------------------------------------------------------------------
// CCamStillPreCaptureView::HandleControllerEventL
// Handle controller events
// -----------------------------------------------------------------------------
//
void CCamStillPreCaptureView::HandleControllerEventL( TCamControllerEvent aEvent, 
                                                     TInt aError )
    {
    PRINT( _L("Camera => CCamStillPreCaptureView::HandleControllerEventL") );
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    if ( ECamEventControllerReady == aEvent )
            {
            iSoftkeyBlankIsNeeded = EFalse;
            UpdateCbaL();
            }
            else if ( aEvent == ECamEventOperationStateChanged )
        {
        PRINT( _L("Camera <> CCamStillPreCaptureView: ECamEventOperationStateChanged") );
        TCamCaptureOperation operation = iController.CurrentOperation();
        switch ( operation )
            {
            case ECamCompleting:
                {
                UpdateCbaL();
                break;
                }

            case ECamFocusing:
                {
                // Hide the AP during focussing
                if ( appUi->AlwaysDrawPreCaptureCourtesyUI()
                    && !iController.CurrentSceneHasForcedFocus() )
                    {
                    appUi->SetActivePaletteVisibility( EFalse );
                    }
                break;
                }

            case ECamFocused:
                {
                // Hide the AP during focussing
                if ( appUi->AlwaysDrawPreCaptureCourtesyUI()
                    && !iController.CurrentSceneHasForcedFocus() )
                    {
                    appUi->SetActivePaletteVisibility( EFalse );
                    }
                UpdateCbaL();
                break;
                }                
                
            case ECamNoOperation:
                {
                if( iController.IsAppUiAvailable() )
	                {
	                //Update softkey state.
	                UpdateCbaL();
	                	                
	                // Hide the AP if we already have an autofocus request but operation doesn't
	                // have it (likely shutter was half-pressed before precap view opened)
	                if ( iController.PendingAFRequest() != 0 && 
	                     appUi->AlwaysDrawPreCaptureCourtesyUI() &&
	                     !iController.CurrentSceneHasForcedFocus() )
	                    {
	                    appUi->SetActivePaletteVisibility( EFalse );
	                    UpdateCbaL();
	                    }

	                // Show the AP again if the shutter key is released
	                // without taking a picture, don't show if uiorientationoverride
	                // feature is on and VF not running (prevents AP flashing in startup)
	                if ( iPreviousControllerOperation !=ECamStandby &&
	                     appUi->CurrentViewState() == ECamViewStatePreCapture && 
	                     appUi->AlwaysDrawPreCaptureCourtesyUI() && 
	                     !iSceneSettingModeActive && 
	                     !( iController.UiConfigManagerPtr() && 
	                     iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() && 
	                     !iController.IsViewFinding() ) ) 
	                    {
	                    appUi->SetActivePaletteVisibility( ETrue );
                        }
                    }
                    
                break;
                }
 
            case ECamCapturing:
                {
                // If in burst, make sure the correct CBA is shown
                if( appUi->IsBurstEnabled() )
                    {
                    UpdateCbaL();
                    // show image counter
                    iContainer->SetupNaviPanesL( CCamContainerBase::ECamPaneCounter );
                    }
                break;
                }
            default:
                {
                // Hide the AP if we already have an autofocus request but operation doesn't
                // have it (likely shutter was half-pressed before precap view opened)
                if ( iController.PendingAFRequest() != 0 && 
                     appUi->AlwaysDrawPreCaptureCourtesyUI() &&
                     !iController.CurrentSceneHasForcedFocus() )
                    {
                    appUi->SetActivePaletteVisibility( EFalse );
                    UpdateCbaL();
                    }
                break;
                }
            }
        
        iPreviousControllerOperation = operation;

        }
    else
        {
        PRINT( _L("Camera <> CCamStillPreCaptureView: call CCamPreCaptureViewBase::HandleControllerEventL") );
        CCamPreCaptureViewBase::HandleControllerEventL( aEvent, aError );
        }
    PRINT( _L("Camera <= CCamStillPreCaptureView::HandleControllerEventL") );        
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::CreateCaptureSetupMenuL
// Creates a photo capture setup menu
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::CreateCaptureSetupMenuL( )
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMSTILLPRECAPTUREVIEW_CREATECAPTURESETUPMENUL, "e_CCamStillPreCaptureView_CreateCaptureSetupMenuL 1" );
    TInt resource;
    // check if we are Western or Arabic/Hebrew layout
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        // Do we need APAC layout
        if( AknLayoutUtils::Variant() == EApacVariant )
            {
            resource = ROID(R_CAM_CAPTURE_SETUP_MENU_PHOTO_DATA_APAC_ID);
            }
        else
            {
            // use Western resource
            resource = ROID(R_CAM_CAPTURE_SETUP_MENU_PHOTO_DATA_ID);
            }
        }
    else
        {
        resource = ROID(R_CAM_CAPTURE_SETUP_MENU_PHOTO_DATA_AH_ID);
        }

    // Use capture setup menu for second camera
    if ( static_cast<CCamAppUiBase*>( AppUi() )->IsSecondCameraEnabled() )
        {
        resource = ROID(R_CAM_CAPTURE_SETUP_MENU_PHOTO_DATA_ID);
        }

    iCaptureSetupMenuContainer = 
        CCamCaptureSetupMenu::NewL( iController, *this, AppUi()->ClientRect(),
        resource, iCaptureSetupMenuLastItemIndex );        
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMSTILLPRECAPTUREVIEW_CREATECAPTURESETUPMENUL, "e_CCamStillPreCaptureView_CreateCaptureSetupMenuL 0" );
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::SetMenuBar()
// Sets the menu bar according to camera state
// ---------------------------------------------------------------------------
//
void 
CCamStillPreCaptureView::SetMenuBar()
  {
  PRINT( _L("Camera => CCamStillPreCaptureView::SetMenuBar") );

  // update menu bar as required
  CEikMenuBar* menuBar = MenuBar();

  // if menuBar exists
  if ( menuBar )
    {       
    if ( iStandbyModeActive ) //&& iActivateToStandby )
      {
      PRINT( _L("Camera <> CCamStillPreCaptureView::SetMenuBar: setting standby menubar..") );
      menuBar->SetMenuTitleResourceId( R_CAM_STANDBY_MENUBAR );
      }
    else if ( iEmbedded && !iSceneSettingModeActive )
      {
      PRINT( _L("Camera <> CCamStillPreCaptureView::SetMenuBar: setting embedded menubar..") );
      menuBar->SetMenuTitleResourceId( ROID(R_CAM_STILL_EMBEDDED_PRE_CAPTURE_MENUBAR_ID));
      }
    else
      {
      if ( iSceneSettingModeActive )
        {
        PRINT( _L("Camera <> CCamStillPreCaptureView::SetMenuBar: setting scene setting menubar..") );
        menuBar->SetMenuTitleResourceId( ROID(R_CAM_SCENE_SETTING_MENUBAR_ID));
        }
      else
        {
        PRINT( _L("Camera <> CCamStillPreCaptureView::SetMenuBar: setting normal menubar..") );
		    menuBar->SetMenuTitleResourceId( ROID(R_CAM_STILL_PRE_CAPTURE_MENUBAR_ID));
		    }
      }
    }
  PRINT( _L("Camera <= CCamStillPreCaptureView::SetMenuBar") );
  }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::CCamStillPreCaptureView
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamStillPreCaptureView::CCamStillPreCaptureView( CCamAppController& aController )
    : CCamPreCaptureViewBase( aController ),iSoftkeyBlankIsNeeded( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::ConstructL()
    {
    BaseConstructL( ROID(R_CAM_STILL_PRE_CAPTURE_VIEW_ID));

    iPreviousControllerOperation = ECamNoOperation;
    
    CCamPreCaptureViewBase::ConstructL();
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateCbaL
// Update softkeys to reflect current state
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateCbaL()
  {
  PRINT( _L("Camera => CCamStillPreCaptureView::UpdateCbaL") );        
    
  CCamAppUi* appui = static_cast<CCamAppUi*>( AppUi() );
  TBool burstEnabled = appui->IsBurstEnabled();
 
  TCamCaptureOperation operation = iController.CurrentOperation();
  TBool isSecondaryPortrait = appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera();
  if ( iSoftkeyBlankIsNeeded )
        {
        isSecondaryPortrait?SetSoftKeysL( R_CAM_SOFTKEYS_BLANK_EXIT_SECONDARY ):SetSoftKeysL( R_CAM_SOFTKEYS_BLANK_EXIT );
        }
  // if video call is active
  else if ( iController.InVideocallOrRinging() )
      {
      SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );	
      }	
  // if the view is in capture setup menu mode
  else  if ( iCaptureSetupMenuModeActive && 
      !iCaptureSetupModeActive  &&
      !iSceneSettingModeActive )
      {
      SetSoftKeysL( R_AVKON_SOFTKEYS_SELECT_BACK );
      }          
  // if the view is in capture setup mode
  else if ( iCaptureSetupModeActive )
      {
      SetSoftKeysL( R_CAM_SOFTKEYS_SETTINGS_SELECT_BACK__CHANGE_TRANSPARENT );
      }
  // if the view is in scene settings mode
  else if ( iSceneSettingModeActive )
      {
      SetSoftKeysL( R_AVKON_SOFTKEYS_SELECT_CANCEL );
      }
  else if ( iStandbyModeActive )
    {
    PRINT( _L("Camera <> CCamStillPreCaptureView::UpdateCbaL: Setting standby softkeys..") );        
    if( KErrNone == appui->StandbyStatus() )
	  {
      SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CONTINUE );
	  }
    else if( appui->IsRecoverableStatus() )
	  {
	  SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );
	  }
    }
  else if( iInfoListBoxActive )
  	{
    if( !iForceAvkonCBA )
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_SETTINGS_SELECT_BACK__CHANGE_TRANSPARENT );    
        }
    else
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_SETTINGS_SELECT_BACK__CHANGE );    
        }    
  	}
  else if (  ( operation == ECamFocusing || operation == ECamFocused  || operation == ECamFocusFailed )
         && !iController.CurrentSceneHasForcedFocus() )
      {
      // If AutoFocus operation in progress.  Can occur for still or
      // burst capture
      SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
      }
  // If timelapse capture is ongoing the keys should be blank and cancel
  else if ( burstEnabled && 
            appui->CurrentBurstMode() == ECamImageCaptureTimeLapse && 
            iController.SequenceCaptureInProgress() )
    {
    SetSoftKeysL( R_CAM_SOFTKEYS_BLANK_STOP );
    }
  // If "Burst" capture is starting
  else if ( burstEnabled && operation == ECamCapturing ) 
    {
    
    SetSoftKeysL( R_CAM_SOFTKEYS_BLANK_STOP );
    }
  // If "Burst" capture is completing
  else if ( burstEnabled && operation == ECamCompleting )   
    {
    SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );               
    }   
  else if ( appui->SelfTimerEnabled() )                   
    {
    // If self-timer enabled, check the precise state to show the 
    // correct CBA state.
    CCamSelfTimer* selftimer = appui->SelfTimer();
    if ( selftimer->IsActive() )
      {
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_SELFTIMER_BLANK_CANCEL );
        }                                
      }
    else if( iEmbedded && operation == ECamCompleting )
      {
      isSecondaryPortrait?SetSoftKeysL( R_CAM_SOFTKEYS_BLANK_SECONDARY ):SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
      }    
    else
      {
      ViewCba()->MakeVisible( ETrue );
      SetSoftKeysL( R_CAM_SOFTKEYS_ACTIVATE_CANCEL__SELECT );                
      }
    }
  else // otherwise, show the default softkeys
    {                        
    // options key is disabled if currently in capture mode
    // transition (i.e. switching to video mode)
    if ( iController.CaptureModeTransitionInProgress() )
      {
      SetSoftKeysL( iEmbedded
                  ? R_CAM_SOFTKEYS_BACK
                  : R_CAM_SOFTKEYS_BLANK_EXIT );
      if(iEmbedded )
            {
            if(isSecondaryPortrait)
                {
                SetSoftKeysL(R_CAM_SOFTKEYS_BACK_SECONDARY);
                }
            else
                {
                SetSoftKeysL(R_CAM_SOFTKEYS_BACK);
                }
            }
        else
            {
            if(isSecondaryPortrait)
                {
                SetSoftKeysL(R_CAM_SOFTKEYS_BLANK_EXIT_SECONDARY);
                }
            else
                {
                SetSoftKeysL(R_CAM_SOFTKEYS_BLANK_EXIT);
                }
            }
      }
    else
      {
      if ( iEmbedded )
        {
        if ( operation == ECamCapturing || operation == ECamCompleting )
           {
           SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
           }
        else 
           {
           if(appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera())
               {
               SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK__CAPTURE_SECONDARY );
               }
           else
               {
               SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK__CAPTURE );
               }
           		
           }
        }
      else
        {
        if ( operation != ECamCompleting 
            && ( !appui->ShowPostCaptureView() 
                || iController.IsViewFinding() ) )
          {
          const TCamOrientation orientation = appui->CamOrientation();
          if ( iController.IsTouchScreenSupported()
               && (ECamOrientationCamcorder     == orientation 
               || ECamOrientationCamcorderLeft == orientation) )
              {
              SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE );
              }
          else 
              {
              if ( !appui->IsToolBarVisible()
                  && appui->IsSecondCameraEnabled()
                  && !iController.IsTouchScreenSupported() )
                  {
                  SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE_TXT );
                  }
              else
                  {
                  appui->IsSecondCameraEnabled()?SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE_SECONDARY ):
                      SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE );
                  }
              }
          }
        else // operation == ECamCompleting
          {
          SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
          }

        }
      }
    }
  PRINT( _L("Camera <= CCamStillPreCaptureView::UpdateCbaL") );        
  }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::SetTitlePaneTextL
// Set the view's title text
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::SetTitlePaneTextL()
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
#ifndef __WINS__
    if ( iEmbedded /*&& !appUi->IsInCallSend()*/ )
        {
        // only set the title if the construction is complete
        // otherwise the application is not yet set up correctly
        // as embedded
        if ( appUi->IsConstructionComplete() )
            {
            // set title to name of embedding application
            appUi->SetTitleEmbeddedL();
            }                  
        }
    else
#endif   
        {
        TInt titleResourceId = R_CAM_STILL_PRE_CAPTURE_TITLE_NAME;
     
        if ( iCaptureSetupModeActive || iSceneSettingModeActive || iInfoListBoxActive )
            {
            titleResourceId = CCamCaptureSetupViewBase::SetupModeTitlePaneResourceId();
            }

        appUi->SetTitleL( titleResourceId );
        }
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::StartViewFinder
// Enter viewfinder mode
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::StartViewFinder()
    {
    iController.EnterViewfinderMode( ECamControllerImage );
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::StopViewFinder
// Exit viewfinder mode
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::StopViewFinder()
    {
    iController.ExitViewfinderMode( ECamControllerImage );
    }
    
// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::CreateContainerL
// Create container control
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::CreateContainerL()
    {
    PRINT( _L("Camera => CCamStillPreCaptureView::CreateContainerL" ) )
     
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    iContainer = CCamStillPreCaptureContainer::NewL(
        iController,
        *this,
        screen );
    iContainer->SetMopParent( this );
    
    CCamPreCaptureViewBase::CreateContainerL();
    PRINT( _L("Camera <= CCamStillPreCaptureView::CreateContainerL" ) )
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::DynInitMenuPaneL
// Changes MenuPane dynamically
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    PRINT( _L("Camera => CamStillPreCaptureView::DynInitMenuPaneL"))

    if ( iContainer )
        {
        iContainer->Window().SetNonFading( ETrue );
        }

    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
	__ASSERT_DEBUG(appUi, CamPanic(ECamPanicNullPointer));
    TInt itemPos(0);
    if( aMenuPane->MenuItemExists(ECamCmdToggleCourtesyUI, itemPos) )
        {
        if ( !appUi->AlwaysDrawPreCaptureCourtesyUI() )
            {
            aMenuPane->SetItemTextL( ECamCmdToggleCourtesyUI, 
                                      R_CAM_CAPTURE_DISPLAY_ON_TEXT);
            }
        }

    // if menu pane is default still capture menu
    if ( aResourceId == ROID(R_CAM_STILL_PRE_CAPTURE_MENU_ID))
        {
        DynInitSwitchCameraMenuItemL( aMenuPane );
        }
//    else if ( aResourceId == R_CAM_STANDBY_MENU && iStandbyError != KErrNone )
    else if ( aResourceId == R_CAM_STANDBY_MENU )
        {
        if( appUi->StandbyStatus() != KErrNone )
        aMenuPane->SetItemDimmed( ECamCmdExitStandby, ETrue );
        }
            
    else if( aResourceId == ROID(R_CAM_STILL_EMBEDDED_PRE_CAPTURE_MENU_ID))
        {
        DynInitSwitchCameraMenuItemL( aMenuPane );
        
/*#ifndef __WINS__
        // if embedded and not in in-call send
        if ( !appUi->IsInCallSend() )
#endif
            {
            aMenuPane->SetItemDimmed( ECamCmdNewVideo, ETrue );
			}*/
		aMenuPane->SetItemDimmed( ECamCmdNewVideo, ETrue );	
        }        
    else // otherwise, not embedded, not in burst mode or menu pane is not default
        {
        CCamCaptureSetupViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );        
        }
    PRINT( _L("Camera <= CamStillPreCaptureView::DynInitMenuPaneL"))
    }


// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SwitchToSceneSettingModeL
// Switches the current mode to scene setting and activates a 
// specific control.
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::SwitchToSceneSettingModeL()
    {
    if ( !iSceneSettingContainer )
    	{
    	iSceneSettingContainer = CCamShootingModeContainer::NewL( AppUi()->ApplicationRect(),
                                                             *this,
                                                             ECamControllerImage,
                                                             iController );
        iSceneSettingContainer->DrawableWindow()->SetOrdinalPosition(-1);
    	}
    
    iSettingModeTitleResourceId = R_CAM_STILL_SCENE_SETTING_TITLE;

    if ( iEmbedded )
        {
        iPreviousMenuResourceId = ROID(R_CAM_STILL_EMBEDDED_PRE_CAPTURE_MENUBAR_ID);
        }
    else
        {
        iPreviousMenuResourceId = ROID(R_CAM_STILL_PRE_CAPTURE_MENUBAR_ID);
        }
    // Remove the view's main container, and add the capture setup 
    // control associated with the input command to the container stack.
    //CCamCaptureSetupViewBase::SwitchToSceneSettingModeL();
    
    CCamPreCaptureViewBase::SwitchToSceneSettingModeL();
  

    // only remove the capture setup menu container after 
    // the switch completes successfully
    RemoveCaptureSetupMenuContainers();

    // Stop the viewfinder as it isn't required for scene settings
    StopViewFinder();
    }


// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SwitchToInfoListBoxL
// Switches the current mode to scene setting and activates a 
// specific control.
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::SwitchToInfoListBoxL( TCamInfoListBoxMode aMode )
    {   
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iCoeEnv->AppUi() );  	
    
    TBool modeSelected = EFalse;
    TInt listBoxResource;
    TInt summaryResource;
    TInt initialValue;
    TInt titleResource;
    TBool skinnedbackground = EFalse;
    
    switch( aMode )
    		{
    		case EInfoListBoxModeTimeLapse:	
    	 			{
    				// Find out whether we are in burst mode	
    				TBool burstmode = EFalse;
    				
    				// if capturing burst with 0 interval then this is normal sequence mode  
    				if ( appUi->IsBurstEnabled() )
    						{
        				burstmode = ETrue;
        				}    
    
    				// Time lapse / sequence mode selector initialization values
    				initialValue = CamTimeLapseUtility::IntervalToEnum( iController.TimeLapseInterval(), burstmode );	
    				listBoxResource = R_CAM_CAPTURE_SETUP_LIST_SEQUENCE_MODE;
    				summaryResource = R_CAM_CAPTURE_SETUP_LIST_SEQUENCE_MODE_SUMMARY;
    				titleResource = R_CAM_TIME_LAPSE_TITLE;
    				    
    				modeSelected = ETrue;
    				}
    				break;
    		
				case EInfoListBoxModeISO:
    				{
    				// Light sensitivity selector initialization values	
    				initialValue = iController.IntegerSettingValue( ECamSettingItemDynamicPhotoLightSensitivity );	
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
    				titleResource = R_CAM_LIGHT_SENSITIVITY_TITLE;   					    				
 					modeSelected = ETrue;
 					skinnedbackground = EFalse;
    				}
    				break;
    		
    		default: 
    				break;
				}    					
    				    	
		if( modeSelected )
				{
				iInfoListBoxContainer = CCamInfoListBoxContainer::NewL( AppUi()->ApplicationRect(),
                                                             		*this,                                                            
                                                             		iController,
                                                             		listBoxResource,
                                                             		summaryResource,
                                                             		initialValue, titleResource,
                                                             		skinnedbackground );		
				
				iInfoListBoxContainer->DrawableWindow()->SetOrdinalPosition(-1); 
				iInfoListBoxContainer->SetMopParent( this ); 
				iInfoListBoxMode = aMode;          
				iSettingModeTitleResourceId = titleResource;                                                   					

    		if ( iEmbedded )
        		{
        		iPreviousMenuResourceId = ROID(R_CAM_STILL_EMBEDDED_PRE_CAPTURE_MENUBAR_ID);
        		}
    		else
        		{
        		iPreviousMenuResourceId = ROID(R_CAM_STILL_PRE_CAPTURE_MENUBAR_ID);
        		}
    		// Remove the view's main container, and add the capture setup 
    		// control associated with the input command to the container stack.
    		CCamCaptureSetupViewBase::SwitchToInfoListBoxL( aMode, skinnedbackground );

    		// only remove the capture setup menu container after 
    		// the switch completes successfully
    		RemoveCaptureSetupMenuContainers();
    		if( skinnedbackground )
    		    {
    		// Stop the viewfinder
    		StopViewFinder();    		    	        
    		    }
    		}
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::DoActivateL( const TVwsViewId& aPrevViewId, 
                                          TUid aCustomMessageId,
                                          const TDesC8& aCustomMessage )
  {
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMSTILLPRECAPTUREVIEW_DOACTIVATEL, "e_CCamStillPreCaptureView_DoActivateL 1" );
  PRINT( _L("Camera => CCamStillPreCaptureView::DoActivateL") );
  PERF_EVENT_START_L2( EPerfEventStillPreCaptureViewActivation );    
  // Ensure the correct menu is used for the current mode.

  // If we are coming back from settings plugin wait for the sequence to complete
    // during that time, update the softkey with BLANK_EXIT softkey
    if ( TUid::Uid ( EAknSoftkeyBack ) == aCustomMessageId )
      {
      iSoftkeyBlankIsNeeded = ETrue;
      }
  CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );
  
  // fixed toolbar is used only with touch devices
  if ( iController.IsTouchScreenSupported() )
      {
      if ( appUi && appUi->IsEmbedded() )
          {
          CAknToolbar* toolbar = Toolbar();
          toolbar->SetToolbarVisibility(EFalse);
          }
      }
  else
      {
      // handled in CamPreCaptureViewBase
      }
  if ( appUi )
    {
    // inform appui of current capture mode
    TCamImageCaptureMode mode = appUi->CurrentBurstMode();
    
    if ( ECamImageCaptureNone != mode )
      {
      appUi->SetCaptureMode( ECamControllerImage, mode );
      }
    else
      {
      appUi->SetCaptureMode( ECamControllerImage, ECamImageCaptureSingle );
      }
    }


  CCamPreCaptureViewBase::DoActivateL( aPrevViewId, 
                                       aCustomMessageId, 
                                       aCustomMessage );                    


  SetMenuBar();


    PERF_EVENT_END_L2( EPerfEventStillPreCaptureViewActivation );
  PRINT( _L("Camera <= CCamStillPreCaptureView::DoActivateL") );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMSTILLPRECAPTUREVIEW_DOACTIVATEL, "e_CCamStillPreCaptureView_DoActivateL 0" );
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::GetHelpContext
// Called to identify the help context for this view
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( KCameraappUID );
    
    // First check to see if the scene settings list is open
    if ( iSceneSettingModeActive )
        {
        aContext.iContext = KLCAM_HLP_SCENES_PHOTO;
        }
    else if( iInfoListBoxActive && EInfoListBoxModeTimeLapse == iInfoListBoxMode )
    	{
     
    	// aContext.iContext = KLCAM_HLP_SCENES_PHOTO;
    	aContext.iContext = KLCAM_HLP_ADVANCED_SEQUENCE;
    	}
    else if( iInfoListBoxActive  && EInfoListBoxModeISO == iInfoListBoxMode )
    	{
     
    	// aContext.iContext = KLCAM_HLP_SCENES_PHOTO;
    	aContext.iContext = KLCAM_HLP_LIGHT_SENSITIVITY;
    	}
    // Else check for standby mode
    else if ( iStandbyModeActive )
        {
        // Also check to see if this is embedded standby
        if ( iEmbedded )
            {
            aContext.iContext = KLCAM_HLP_STANDYBY_EM;
            }
        else
            {
            aContext.iContext = KLCAM_HLP_STANDYBY_EM;
            }        
        }
    // Else check for embedded mode
    else if ( iEmbedded )
        {
        aContext.iContext = KLCAM_HLP_VIEWFINDER_PHOTO_EM;
        }
    // Else this is the plain photo viewfinder
    else
        {
        aContext.iContext = KLCAM_HLP_VIEWFINDER_PHOTO;
        }
    }  

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::DynInitToolbarL
// Dynamically initialize toolbar contents
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::DynInitToolbarL( TInt aResourceId, 
                                       CAknToolbar* aToolbar )
    {
    PRINT2( _L("Camera => CCamStillPreCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar );
    (void)aResourceId; // remove compiler warning

    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    iController.StartIdleTimer();

    // fixed toolbar is used only with touch devices
    if ( iController.IsTouchScreenSupported() )
        {
        UpdateToolbarIconsL();
        if( aToolbar )
            {            
            if ( iEmbedded /* && appUi->IsSecondCameraEnabled()*/ )
                {
                // Dim mode switch icon and disable tooltip
                aToolbar->SetItemDimmed( ECamCmdNewVideo, ETrue, ETrue );
               
                CAknButton* videoModeButton = static_cast<CAknButton*>(
                        aToolbar->ControlOrNull( ECamCmdNewVideo ) );
                if ( videoModeButton ) 
                    {
                    videoModeButton->SetDimmedHelpTextL( KNullDesC );
                    }
               
            
            
                if ( appUi && appUi->IsSecondCameraEnabled() )
                    {
                    aToolbar->SetItemDimmed( ECamCmdPhotos, ETrue, ETrue );
                    CAknButton* photosButton = static_cast<CAknButton*>(
                            aToolbar->ControlOrNull( ECamCmdPhotos ) );
                    if ( photosButton ) 
                        {
                        // do not show tooltip for dimmed item
                        photosButton->SetDimmedHelpTextL( KNullDesC );
                        }
                    }
                }
            CAknToolbarExtension* extension = aToolbar->ToolbarExtension();
        
            if( extension )
                {
        
                if ( iEmbedded )
                    {
                    extension->HideItemL( ECamCmdNewVideo, ETrue );
                    extension->HideItemL( ECamCmdTimeLapseSlider, ETrue );
                    extension->HideItemL( ECamCmdPhotos, ETrue );
                    }
                else
                    {
                    extension->HideItemL( ECamCmdNewVideo, EFalse );
                    extension->HideItemL( ECamCmdToggleFacetracking, EFalse );
                    extension->HideItemL( ECamCmdPhotos, EFalse );
                    }
                if(aResourceId == ECamCmdToolbarExtension)
                    {
                    appUi->ZoomPane()->MakeVisible(EFalse,ETrue);
                    }
                }
            }
        }
    
    PRINT2( _L("Camera <= CCamStillPreCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar );
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateToolbarIconsL
// Update fixed toolbar icons according to current settings
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateToolbarIconsL()
    {
    PRINT( _L("Camera => CCamStillPreCaptureView::UpdateToolbarIconsL") );
	OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMSTILLPRECAPTUREVIEW_UPDATETOOLBARICONSL, "e_CCamStillPreCaptureView_UpdateToolbarIconsL 1" );
    // fixed toolbar is used only with touch devices
    if(!iController.IsTouchScreenSupported())
        return;
	UpdateFlashIconsL();
	UpdateLightSensitivityIconsL();
	UpdateColorToneIconsL();
	UpdateWhiteBalanceIconsL();
	UpdateSharpnessIconsL();
	UpdateExposureIconsL();
	UpdateContrastIconsL();
	UpdateSceneModeIconsL();            
	UpdateSelfTimerIconsL();
	UpdateVFGridIconsL();
	UpdateBurstModeIconsL();
	UpdateFaceTrackingIconsL(); 
	RedrawToolBar();
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMSTILLPRECAPTUREVIEW_UPDATETOOLBARICONSL, "e_CCamStillPreCaptureView_UpdateToolbarIconsL 0" );
    PRINT( _L("Camera <= CCamStillPreCaptureView::UpdateToolbarIconsL") );
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateFlashIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateFlashIconsL()
    {
    CAknButton* button = ToolbarButtonById(ECamCmdCaptureSetupFlashStill);
    if(!button)
        return;
    TCamFlashId flash = static_cast< TCamFlashId > 
                        ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoFlash ) );
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    TFileName iconFileName;
    CamUtility::ResourceFileName( iconFileName );
    switch(flash)
        {
        case ECamFlashAuto:
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_autoflash,
                EMbmCameraappQgn_indi_cam4_autoflash_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4Autoflash );
                
            break;
            }
        case ECamFlashOff:
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_flash_off,
                EMbmCameraappQgn_indi_cam4_flash_off_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4FlashOff );
            break;
            }
        case ECamFlashAntiRedEye:
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_antired,
                EMbmCameraappQgn_indi_cam4_antired_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4Antired );
            break;
            }
        case ECamFlashForced:
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_flash_on,
                EMbmCameraappQgn_indi_cam4_flash_on_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4FlashOn );
            break;
            }
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateContrastIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateContrastIconsL()
    {
    CAknButton* button = ToolbarButtonById(ECamCmdCaptureSetupContrastStill);
    if ( button )
        {
        if ( !iController.UiConfigManagerPtr()->IsContrastSupported() )
            {
            HideButton(ECamCmdCaptureSetupContrastStill);
            return;
            }
        TInt contrast = iController.IntegerSettingValue( ECamSettingItemDynamicPhotoContrast );
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );
        if (contrast > 0)
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_contrast_plus,
                EMbmCameraappQgn_indi_cam4_contrast_plus_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4ContrastPlus );
           }
        else if (contrast < 0)
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_contrast_minus,
                EMbmCameraappQgn_indi_cam4_contrast_minus_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4ContrastMinus );
            }
        else 
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_contrast,
                EMbmCameraappQgn_indi_cam4_contrast_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4Contrast );
            }
        }                
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateExposureIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateExposureIconsL()
    {
    CAknButton* button = ToolbarButtonById( ECamCmdCaptureSetupExposureStill );
    if ( button )
        {
        if ( !iController.UiConfigManagerPtr()->IsEVSupported() )
            {
            HideButton(ECamCmdCaptureSetupExposureStill);
            return;
            }
        TInt exposure = iController.IntegerSettingValue( ECamSettingItemDynamicPhotoExposure );
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );
        if (exposure > 0)
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_exposure_plus,
                EMbmCameraappQgn_indi_cam4_exposure_plus_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4ExposurePlus );
            }
        else if (exposure < 0)
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_exposure_minus,
                EMbmCameraappQgn_indi_cam4_exposure_minus_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4ExposureMinus );
            }
        else 
            {
            SetIconL(
                button,
                iconFileName,
                EMbmCameraappQgn_indi_cam4_exposure,
                EMbmCameraappQgn_indi_cam4_exposure_mask,
                skinInstance,
                KAknsIIDQgnIndiCam4Exposure );
            }
        }                
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateSharpnessIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateSharpnessIconsL()
    {
    CAknButton* button = ToolbarButtonById(ECamCmdCaptureSetupImageSharpnessStill);
    if ( button )
        {
        if ( !iController.UiConfigManagerPtr()->IsSharpnessFeatureSupported() )
            {
            HideButton(ECamCmdCaptureSetupImageSharpnessStill);
            return;
            }
        TCamImageSharpnessId sharpness = 
                  static_cast< TCamImageSharpnessId > 
                    ( iController.IntegerSettingValue(
                            ECamSettingItemDynamicPhotoImageSharpness ) ); 
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );
        static_cast< TCamImageSharpnessId > 
            ( iController.IntegerSettingValue(
                    ECamSettingItemDynamicPhotoImageSharpness ) );

        switch ( sharpness )
            {
            case ECamImageSharpnessHard:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_sharpness_hard,
                    EMbmCameraappQgn_indi_cam4_sharpness_hard_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4SharpnessHard );
                break;
                }
            case ECamImageSharpnessNorm:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_sharpness_normal,
                    EMbmCameraappQgn_indi_cam4_sharpness_normal_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4SharpnessNormal );
                break;
                }
            case ECamImageSharpnessSoft:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_sharpness_soft,
                    EMbmCameraappQgn_indi_cam4_sharpness_soft_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4SharpnessSoft );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateWhiteBalanceIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateWhiteBalanceIconsL()
    {
    CAknButton* button = ToolbarButtonById( ECamCmdCaptureSetupWhiteBalanceStill );
    if ( button )
        {
        if ( !iController.UiConfigManagerPtr()->IsWhiteBalanceFeatureSupported() )
            {
            HideButton(ECamCmdCaptureSetupWhiteBalanceStill);
            return;
            }
        TCamWhiteBalanceId wb = static_cast< TCamWhiteBalanceId > 
            ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoWhiteBalance ) );
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );    
        switch ( wb )
            {
            case ECamWhiteBalanceAWB:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_wb_auto,
                    EMbmCameraappQgn_indi_cam4_wb_auto_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4WbAuto );
                break;
                }
            case ECamWhiteBalanceDaylight:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_wb_sunny,
                    EMbmCameraappQgn_indi_cam4_wb_sunny_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4WbSunny );
                break;
                }
            case ECamWhiteBalanceCloudy:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_wb_cloudy,
                    EMbmCameraappQgn_indi_cam4_wb_cloudy_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4WbCloudy );
                break;
                }
            case ECamWhiteBalanceTungsten:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_wb_tungsten,
                    EMbmCameraappQgn_indi_cam4_wb_tungsten_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4WbTungsten );
                break;
                }
            case ECamWhiteBalanceFlourescent:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_wb_fluorecent,
                    EMbmCameraappQgn_indi_cam4_wb_fluorecent_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4WbFluorecent );
                break;
                }
            default:
                break;
            }
        }
    
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateColorToneIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateColorToneIconsL()
    {
    CAknButton* button = ToolbarButtonById( ECamCmdCaptureSetupColourFilterStill );
    if ( button )
        {
        if ( !iController.UiConfigManagerPtr()->IsColorToneFeatureSupported() )
            {
            HideButton(ECamCmdCaptureSetupColourFilterStill);
            return;
            }
        TCamColourFilterId color = static_cast< TCamColourFilterId > 
            ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoColourFilter ) );
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );    
        switch ( color )
            {
            case ECamColourFilterColour:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_colour_normal,
                    EMbmCameraappQgn_indi_cam4_colour_normal_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ColourNormal );
                break;
                }
            case ECamColourFilterBlackAndWhite:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_colour_bw,
                    EMbmCameraappQgn_indi_cam4_colour_bw_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ColourBw );
                break;
                }
            case ECamColourFilterSepia:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_colour_sepia,
                    EMbmCameraappQgn_indi_cam4_colour_sepia_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ColourSepia );
                break;
                }
            case ECamColourFilterNegative:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_colour_negative,
                    EMbmCameraappQgn_indi_cam4_colour_negative_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ColourNegative );
                break;
                }
            case ECamColourFilterVivid:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_colour_vivid,
                    EMbmCameraappQgn_indi_cam4_colour_vivid_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ColourVivid );
                break;
                }
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateLightSensitivityIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateLightSensitivityIconsL()
    {
    CAknButton* button = ToolbarButtonById( ECamCmdCaptureSetupLightSensitivityStill );
    if ( button )
        {
        if (!iController.UiConfigManagerPtr()->IsISOSupported())
            {
            HideButton(ECamCmdCaptureSetupLightSensitivityStill);
            return;
            }
        TCamLightSensitivityId iso = static_cast< TCamLightSensitivityId > 
            ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoLightSensitivity ) );
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );     
        switch ( iso )
            {
            case ECamLightSensitivityAuto:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_iso_auto,
                    EMbmCameraappQgn_indi_cam4_iso_auto_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4IsoAuto );
                break;
                }
            case ECamLightSensitivityLow:
            //case ECamLightSensitivityLowMed:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_iso_low,
                    EMbmCameraappQgn_indi_cam4_iso_low_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4IsoLow );
                break;
                }
            case ECamLightSensitivityMed:
            //case ECamLightSensitivityMedHigh:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_iso_medium,
                    EMbmCameraappQgn_indi_cam4_iso_medium_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4IsoMedium );
                break;
                }
            case ECamLightSensitivityHigh:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_iso_high,
                    EMbmCameraappQgn_indi_cam4_iso_high_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4IsoHigh );
                break;
                }
            default:
                break;
            }
        }
    
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::StartMskCapture()
// Try to start the capture with MSK command. Return true if started.
// ---------------------------------------------------------------------------
//
TBool CCamStillPreCaptureView::StartMskCaptureL()
    {
    PRINT( _L("Camera => CCamStillPreCaptureView::StartMskCapture") );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    TCamOrientation orientation = appUi->CamOrientation();
    TBool startedCapture(EFalse);
	if( orientation == ECamOrientationCamcorder || 
	    orientation == ECamOrientationCamcorderLeft ||
		orientation == ECamOrientationPortrait )
		{
		// In case of MSK if no AP on screen, it should work as capture
		// If optical joystic is not available, we will start the 
		// capturing with auto-focusing here. Otherwise it is handled
		// in CCamStillPreCaptureContainer
		if ( appUi && !appUi->IsToolBarVisible()
		     && iController.UiConfigManagerPtr() 
		     && !iController.UiConfigManagerPtr()->IsOpticalJoyStickSupported() )
		    {
      	    OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMSTILLPRECAPTUREVIEW_STARTMSKCAPTUREL, "e_CAM_PRI_AF_LOCK 1" );
      	    if ( iController.IsViewFinding() && appUi->CheckMemoryL() &&
                !iController.CaptureModeTransitionInProgress() ) 
            	{
            	SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );

            	// fixed toolbar is used only with touch devices
            	if ( iController.IsTouchScreenSupported() )
            	    {
                    CAknToolbar* fixedToolbar = Toolbar();
                    if ( fixedToolbar )
                        {
                        fixedToolbar->SetToolbarVisibility( EFalse );
                        }
            	    }
                    // Start the autofocus operation now, the capture will be queued
                    // after focus completes
                    if ( !appUi->SelfTimerEnabled() )
                        {
                        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSTILLPRECAPTUREVIEW_STARTMSKCAPTUREL, "e_CAM_APP_AF 1" );
                        iController.StartAutoFocus();
                        }
                	
                	TKeyEvent keyEvent;
                	appUi->StartCaptureL( keyEvent );
                	startedCapture = ETrue;
    		    }
    		}
	    // else let the calling function take care of capturing
    	}
	PRINT( _L("Camera <= CCamStillPreCaptureView::StartMskCapture") );
  	return startedCapture;
  	}

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateSelfTimerIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateSelfTimerIconsL()
    {
    CAknButton* button = ToolbarButtonById( ECamCmdCaptureSetupSelfTimer );
    if ( button )
        {
        TCamSelfTimerId selftimer = static_cast< TCamSelfTimerId > 
            ( iController.IntegerSettingValue( ECamSettingItemDynamicSelfTimer ) );
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );     
        switch ( selftimer )
            {
            case ECamSelfTimerOff:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_selftimer_off,
                    EMbmCameraappQgn_indi_cam4_selftimer_off_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4SelftimerOff );
                break;
                }
            case ECamSelfTimer2:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_selftimer_2sec,
                    EMbmCameraappQgn_indi_cam4_selftimer_2sec_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4Selftimer2sec );
                break;
                }
            case ECamSelfTimer10:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_selftimer_10s,
                    EMbmCameraappQgn_indi_cam4_selftimer_10s_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4Selftimer10s );
                break;
                }
            case ECamSelfTimer20:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_selftimer_20s,
                    EMbmCameraappQgn_indi_cam4_selftimer_20s_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4Selftimer20s );
                break;
                }
            default:
                break;
            }
        }

    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateSceneModeIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateSceneModeIconsL()
    {
    CAknButton* button = ToolbarButtonById( ECamCmdCaptureSetupSceneStill );

    if ( button )
        {
        TCamSceneId scene = static_cast< TCamSceneId > ( 
            iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) );
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );     

        // For use with scene indicator
        TInt32 iconId = EMbmCameraappQgn_indi_cam4_mode_auto;
        TInt32 maskId = EMbmCameraappQgn_indi_cam4_mode_auto_mask;

        switch ( scene )
            {
            case ECamSceneAuto:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_auto,
                    EMbmCameraappQgn_indi_cam4_mode_auto_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModeAuto );
                iconId = EMbmCameraappQgn_indi_cam4_mode_auto;
                maskId = EMbmCameraappQgn_indi_cam4_mode_auto_mask;
                break;
                }
            case ECamSceneUser:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_userscene,
                    EMbmCameraappQgn_indi_cam4_mode_userscene_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModeUserscene );
                iconId = EMbmCameraappQgn_indi_cam4_mode_userscene;
                maskId = EMbmCameraappQgn_indi_cam4_mode_userscene_mask;
                break;
                }                    
            case ECamSceneMacro:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_closeup,
                    EMbmCameraappQgn_indi_cam4_mode_closeup_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModeCloseup );
                iconId = EMbmCameraappQgn_indi_cam4_mode_closeup;
                maskId = EMbmCameraappQgn_indi_cam4_mode_closeup_mask;
                break;
                }          
            case ECamScenePortrait:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_portrait,
                    EMbmCameraappQgn_indi_cam4_mode_portrait_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModePortrait );
                iconId = EMbmCameraappQgn_indi_cam4_mode_portrait;
                maskId = EMbmCameraappQgn_indi_cam4_mode_portrait_mask;
                break;
                }
            case ECamSceneScenery:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_landscape,
                    EMbmCameraappQgn_indi_cam4_mode_landscape_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModeLandscape );
                iconId = EMbmCameraappQgn_indi_cam4_mode_landscape;
                maskId = EMbmCameraappQgn_indi_cam4_mode_landscape_mask;
                break;
                }
            case ECamSceneNight:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_night, 
                    EMbmCameraappQgn_indi_cam4_mode_night_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModeNight );
                iconId = EMbmCameraappQgn_indi_cam4_mode_night;
                maskId = EMbmCameraappQgn_indi_cam4_mode_night_mask;
                break;
                }
            case ECamSceneSports:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_sport,
                    EMbmCameraappQgn_indi_cam4_mode_sport_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModeSport );
                iconId = EMbmCameraappQgn_indi_cam4_mode_sport;
                maskId = EMbmCameraappQgn_indi_cam4_mode_sport_mask;
                break;
                }
            case ECamSceneNightScenery:
            case ECamSceneNightPortrait:
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_mode_portrait_night, 
                    EMbmCameraappQgn_indi_cam4_mode_portrait_night_mask, 
                    skinInstance,
                    KAknsIIDQgnIndiCam4ModePortraitNight );
                iconId = EMbmCameraappQgn_indi_cam4_mode_portrait_night;
                maskId = EMbmCameraappQgn_indi_cam4_mode_portrait_night_mask;
                break;
                }
            case ECamSceneCandlelight:
            default:
                break;
            }
        
        // Update the icon in the side pane
        if ( iController.UiConfigManagerPtr()->IsCustomCaptureButtonSupported() )
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
            CCamSidePane* sidePane = appUi->SidePane();
    
            if ( sidePane )
                {
                PRINT( _L("Camera <> CCamStillPreCaptureView::UpdateSceneModeIconsL - Updating side pane indicator") );
                sidePane->UpdateSceneIndicatorL( iconId, maskId );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateVFGridIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateVFGridIconsL()
    {
    CAknButton* button = ToolbarButtonById( ECamCmdViewfinderGrid  );
    if ( button )
        {
        CAknButtonState* state = button->State();
        if ( state )
            {
            TInt grid = iController.IntegerSettingValue( ECamSettingItemViewfinderGrid );
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
            TFileName iconFileName;
            CamUtility::ResourceFileName( iconFileName );     

            if ( grid == ECamViewfinderGridOff )
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_viewfinder_off,
                    EMbmCameraappQgn_indi_cam4_viewfinder_off_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ViewfinderOff );
                HBufC* buttonText = StringLoader::LoadLC( R_QTN_LCAM_TB_GRID );
                state->SetTextL( *buttonText );
                CleanupStack::PopAndDestroy( buttonText );
                }
            else
                {
                SetIconL(
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_viewfinder_on,
                    EMbmCameraappQgn_indi_cam4_viewfinder_on_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4ViewfinderOn );
                HBufC* buttonText = StringLoader::LoadLC( R_QTN_LCAM_TB_GRID );
                state->SetTextL( *buttonText );
                CleanupStack::PopAndDestroy( buttonText );
                }
            }
        }
    
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateVFGridIconsL
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateBurstModeIconsL()
	{
	if (!iController.UiConfigManagerPtr()->IsBurstModeSupported())
		{
		HideButton( ECamCmdTimeLapseSlider );
		return;
		}
	CAknButton* button = ToolbarButtonById( ECamCmdTimeLapseSlider );
    if ( button )
        {
        CAknButtonState* state = button->State();
        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        TFileName iconFileName;
        CamUtility::ResourceFileName( iconFileName );     
        if ( state )
            {
            TCamImageCaptureMode captureMode = iController.CurrentImageModeSetup();
            if ( captureMode == ECamImageCaptureSingle )
                {
                SetIconL(
                        button,
                        iconFileName,
                        EMbmCameraappQgn_indi_cam4_sequence_burst,
                        EMbmCameraappQgn_indi_cam4_sequence_burst_mask,
                        skinInstance,
                        KAknsIIDQgnIndiCam4Sequence );

                HBufC* buttonText = StringLoader::LoadLC( R_QTN_LCAM_TB_SEQUENCE );
                state->SetTextL( *buttonText );
                CleanupStack::PopAndDestroy( buttonText );
                }
            else if ( captureMode == ECamImageCaptureBurst )
                {
                SetIconL(
                        button,
                        iconFileName,
                        EMbmCameraappQgn_indi_cam4_sequence,
                        EMbmCameraappQgn_indi_cam4_sequence,
                        skinInstance,
                        KAknsIIDQgnIndiCam4SequenceBurst );

                HBufC* buttonText = StringLoader::LoadLC( R_QTN_LCAM_TB_TURN_SEQUENCE_OFF );
                state->SetTextL( *buttonText );
                CleanupStack::PopAndDestroy( buttonText );

                }
            else
                {
                // pass
                }    
            }
        }	
	}

// ---------------------------------------------------------------------------
// CCamStillPreCaptureView::UpdateFaceTrackingIconsL 
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureView::UpdateFaceTrackingIconsL()
    {
    CAknButton* button = ToolbarButtonById(  ECamCmdToggleFacetracking  );
      
    if ( button )
        {
        CAknButtonState* state = button->State();
        if ( state )
            {
            TInt face = iController.IntegerSettingValue( 
                ECamSettingItemFaceTracking );
                 
            MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
            TFileName iconFileName;
            CamUtility::ResourceFileName( iconFileName );  
            if ( face == ECamSettOff )
                {
                PRINT( _L( "Camera => UpdateFaceTrackingIconL off" ) ); 
                SetIconL (
                    button,
                    iconFileName,
                    EMbmCameraappQgn_indi_cam4_tb_facedet_off,
                    EMbmCameraappQgn_indi_cam4_tb_facedet_off_mask,
                    skinInstance,
                    KAknsIIDQgnIndiCam4TbFacedetOff );
                }
            else
                {
                PRINT( _L( "Camera => UpdateFaceTrackingIconL on" ) ); 
                SetIconL (
                          button,
                          iconFileName,
                          EMbmCameraappQgn_indi_cam4_tb_facedet,
                          EMbmCameraappQgn_indi_cam4_tb_facedet_mask,
                          skinInstance,
                          KAknsIIDQgnIndiCam4TbFacedet );
                }
            }
        }
    // Update the status indicator too.
    iController.NotifyControllerObservers( ECamEventFaceTrackingStateChanged );
    }

//  End of File  
