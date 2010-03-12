/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Video pre-capture view class for Camera application*
*/


// INCLUDE FILES

 
#include "CamUtility.h"
#include "CamAppUiBase.h"
#include "CamAppController.h"
#include "CamVideoPreCaptureView.h"
#include "CamLogger.h"
#include "CamContainerBase.h"
#include "CamAppUi.h"
#include <eikmenub.h>
#include <eikapp.h>
#include <avkon.rsg>
#include <CaeEngine.h>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <gulicon.h>
#include <AknIconUtils.h>
#include <aknbutton.h>
#include <StringLoader.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <cameraapp.mbg>
#include "CamCaptureSetupMenu.h"
#include "CamPanic.h"

#include "CamShootingModeContainer.h"
#include "CamVideoPreCaptureContainer.h"
#include "Cam.hrh"
#include "CamAppUid.h"
#include <csxhelp/lcam.hlp.hrh>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include "CamUtility.h"
#include "camactivepalettehandler.h"
#include "CameraUiConfigManager.h"
#include "CamLocalViewIds.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamVideoPreCaptureViewTraces.h"
#endif




// ========================= MEMBER FUNCTIONS ================================
// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamVideoPreCaptureView* CCamVideoPreCaptureView::NewLC( CCamAppController& aController )
    {
    CCamVideoPreCaptureView* self = 
        new( ELeave ) CCamVideoPreCaptureView( aController );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView destructor
// 
// ---------------------------------------------------------------------------
//
CCamVideoPreCaptureView::~CCamVideoPreCaptureView()
    {
    }
    
// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::Id
// Returns UID of view
// ---------------------------------------------------------------------------
//
TUid CCamVideoPreCaptureView::Id() const
    {
  	return TUid::Uid( ECamViewIdVideoPreCapture );
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::DoDeactivate()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMVIDEOPRECAPTUREVIEW_DODEACTIVATE, "e_CCamVideoPreCaptureView_DoDeactivate 1" );
    PERF_EVENT_START_L2( EPerfEventVideoPreCaptureViewDeactivation ); 
    CCamPreCaptureViewBase::DoDeactivate();
    PERF_EVENT_END_L2( EPerfEventVideoPreCaptureViewDeactivation ); 
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMVIDEOPRECAPTUREVIEW_DODEACTIVATE, "e_CCamVideoPreCaptureView_DoDeactivate 0" );
    }

// -----------------------------------------------------------------------------
// CCamVideoPreCaptureView::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void 
CCamVideoPreCaptureView::HandleCommandL( TInt aCommand )
  {
  PRINT( _L("Camera => CCamVideoPreCaptureView::HandleCommandL") );
  // check our current operation state
  TCamCaptureOperation operation = iController.CurrentVideoOperation();
  CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() ); 
  TCamOrientation orientation = appUi->CamOrientation();
  switch ( aCommand )
    {
    case ECamMSKCmdSelect:
    case ECamMSKCmdAppRecord:
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
            	static_cast<CCamVideoPreCaptureContainer*>
            			( Container() )->OfferKeyEventL( aKeyEvent, EEventKey );	
       		    }
        	}
        else if ( aCommand == ECamMSKCmdAppRecord )
             // if the Camera in the Portrait mode
             // MSK event is handled like a capture key
            {
            if ( iController.IsViewFinding() && appUi->CheckMemoryL() )
                {
                iController.StartVideoRecordingL();
                // fixed toolbar is used only with touch devices
                if ( iController.IsTouchScreenSupported() )
                    {
                    CAknToolbar* fixedToolbar = Toolbar();
                    if ( fixedToolbar )
                        {
                        fixedToolbar->SetToolbarVisibility( EFalse );
                        }
                    }
                }
            }	
        break;
    	}
    case ECamCmdRecord:
        {
        	TInt callType( EPSCTsyCallTypeUninitialized );
            RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType, callType );
            if ( callType != EPSCTsyCallTypeH324Multimedia && iController.IsViewFinding() && appUi->CheckMemoryL() )
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
            appUi->SetLensCoverExit( EFalse );
            iController.StartVideoRecordingL();
            // fixed toolbar is used only with touch devices
            if ( iController.IsTouchScreenSupported() )
                {
                CAknToolbar* fixedToolbar = Toolbar();
                if ( fixedToolbar )
                    {
                    fixedToolbar->SetToolbarVisibility( EFalse );
                    }
                }
            }
        break;
        }
        
    case EAknSoftkeySelect:
      {
      switch( operation )
        {
        // -------------------------
        case ECamNoOperation:
          {
          if ( !iController.VideoRecordPending() )
            {                    
            CCamPreCaptureViewBase::HandleCommandL( aCommand );
            }
          break;
          }
        // -------------------------
        // ...Otherwise, if we are in video recording or paused mode
        // ...then stop the recording.
        case ECamCapturing:
        case ECamPaused:
          {
          iController.StopVideoRecording();
          break;
          }
        // -------------------------
        default:
          break;
        // -------------------------
        }
      break;
      }
    // ---------------------------------------------------
    case ECamCmdPause:
      {
      iController.PauseVideoRecording();
      break;
      }
    // ---------------------------------------------------
    case ECamCmdContinue:
      {
      iController.ContinueVideoRecording();
      break;
      }
    // ---------------------------------------------------
    case ECamCmdShootSetup:
      {
      // If currently recording or paused recording then don't
      // popup the capture setup menu.
      if ( ECamPaused     == operation 
        || ECamCapturing == operation )
        {
        return;
        }
      // Otherwise popup the capture setup menu
      else
        {
        CCamPreCaptureViewBase::HandleCommandL( aCommand );
        }
      break;
      }
    // ---------------------------------------------------
    case ECamCmdGoToStandby:
      {
      PRINT( _L("Camera <> CCamVideoPreCaptureView::HandleCommandL: case ECamCmdGoToStandby") );

      //PRINT1( _L("Camera => CCamVideoPreCaptureView::HandleCommandL standby error (%d)"), iStandbyError)
      //SwitchToStandbyModeL( ECamViewIdVideoPreCapture, iStandbyError );
      SwitchToStandbyModeL( ECamViewIdVideoPreCapture, appUi->StandbyStatus() );
      break;
      }
    // ---------------------------------------------------
    case ECamCmdExitStandby:
      {
      PRINT( _L("Camera <> CCamVideoPreCaptureView::HandleCommandL: case ECamCmdExitStandby") );
      ExitStandbyModeL();
      UpdateToolbarIconsL();
      break;
      }
    // ---------------------------------------------------
    case ECamCmdMute:
      {
      MuteEmbeddedAudioL( ETrue );
      break;
      }
    // ---------------------------------------------------
    case ECamCmdUnmute:
      {
      MuteEmbeddedAudioL( EFalse );
      break;
      }
    // ---------------------------------------------------
    case ECamCmdStop:
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
      appUi->EndCapture();
      break;
      }
    // ---------------------------------------------------
    case ECamCmdToggleVideoFlash:
      {
      if ( iController.UiConfigManagerPtr()->IsVideoLightSupported() )
          {
          TCamFlashId flash = static_cast< TCamFlashId > 
                        ( iController.IntegerSettingValue( ECamSettingItemDynamicVideoFlash ) );       
          if( flash == ECamFlashForced )
            {
            flash = ECamFlashOff;
            }
          else
            {
            flash = ECamFlashForced;
            }
          iController.SetIntegerSettingValueL(ECamSettingItemDynamicVideoFlash, flash);             

          CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
          if ( appUi && 
               appUi->APHandler() &&
               ( appUi->CamOrientation() == ECamOrientationCamcorder || appUi->CamOrientation() == ECamOrientationCamcorderLeft ) )
            {
            appUi->APHandler()->UpdateActivePaletteL();
            }          
          }
      UpdateToolbarIconsL();
      break;
      }
    // ---------------------------------------------------
    default:
      {
      CCamPreCaptureViewBase::HandleCommandL( aCommand );
      break;
      }
    // ---------------------------------------------------
    }

  PRINT( _L("Camera <= CCamVideoPreCaptureView::HandleCommandL") );
  }

// -----------------------------------------------------------------------------
// CCamVideoPreCaptureView::HandleForegroundEventL
// Handle foreground event
// -----------------------------------------------------------------------------
//
void 
CCamVideoPreCaptureView::HandleForegroundEventL( TBool aForeground )
  {
  PRINT1( _L( "Camera => CCamVideoPreCaptureView::HandleForegroundEventL %d" ), aForeground );
  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
  
  // -------------------------------------------------------
  // reset menu and cba as appropriate
  if( aForeground )
    {
    // ignore event if received in postcapture or if uiorientationoverride
    // is supported and going to stillmode
    if ( ECamViewStatePostCapture == appUi->CurrentViewState() ||
       ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() &&
         appUi->TargetMode() == ECamControllerImage ) ) 
      {
      return;	
      }	
      
    if ( appUi->IsInPretendExit() )
      {
      // The view can get a foreground event while the application is
      // actually in a pretend exit situation. This occurs when the view switch
      // was called before the exit event, but didn't complete until after the
      // exit event. In this case the view should not register an interest in
      // the engine as the application is really in the background and the resources
      // need to be released
      PRINT( _L( "Camera <= CCamVideoPreCaptureView::HandleForegroundEventL, in pretend exit" ) );
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

    // make sure CBA is correct
    UpdateCbaL();

    // check for silent profile whenever we come to foreground, we dont need it here,
    // but we keep track of this in appcontroller.
    iController.IsProfileSilent();

    // set embedded flag here
    iEmbedded = appUi->IsEmbedded();
    PRINT1( _L("Camera <> CCamVideoPreCaptureView::HandleForegroundEventL - iEmbedded:%d"), iEmbedded );

    // update menu bar as required
    SetMenuBar();

    // if video is currently saving, it can be in either of the states:
    // ECamVideoRecordInProgress orECamVideoRecordPaused
    // but the recording has already been stopped
    if ( iController.CurrentlySavingVideo() )
      {
      UpdateCbaL();
      // if the app was behind an eikon server window then the resources
      // were not released. 
      if ( iContinueInBackground )
        {
        iContinueInBackground = EFalse;
        PRINT( _L( "Camera <= CCamVideoPreCaptureView::HandleForegroundEvent, returned behind notification" ) );                
        return;
        }

      // Register that we want to use the engine
      IncrementCameraUsers();
      PRINT( _L( "Camera <= CCamVideoPreCaptureView::HandleForegroundEvent, waiting for postcapture" ) );                
      return;
      }

    }
  // -------------------------------------------------------
  // if going to the background, stop any current video recording
  // unless already saving or it is only an eikon server window
  else
    {
    TCamCaptureOperation operation( iController.CurrentVideoOperation() );

    PRINT1( _L("Camera <> CCamVideoPreCaptureView::HandleForegroundEventL - operation=%d"), operation );
    if( !iController.CurrentlySavingVideo() && 
    	( ( ECamCapturing == operation || ECamPaused    == operation ) 
    	   && appUi->AppInBackground( EFalse )
    	   || ( iController.UiConfigManagerPtr()->IsKeyLockWatcherSupported() && iController.IsKeyLockOn() ) ) )
      {
        PRINT( _L("Camera <> CCamVideoPreCaptureView::HandleForegroundEventL  stopping video") );
      // If the video stops for an active call then inform the controller
      // this info is needed so that the stop tone isn't played
      if ( appUi->ForegroundAppIsPhoneApp() )
        {
        PRINT( _L("Camera <> CCamVideoPreCaptureView::HandleForegroundEventL  SetVideoStoppedForPhoneApp") );
        iController.SetVideoStoppedForPhoneApp( ETrue );
        }
      
      // If AknCapServer has NOT got foreground, (which would possibly 
      // indicate a "charging" note) stop video recording
      iController.StopVideoRecording();
      // reset the stopped for call flag
      iController.SetVideoStoppedForPhoneApp( EFalse );
      }
    
    /*
    const TInt KWindowGroupId( iCoeEnv->WsSession().GetFocusWindowGroup() );
    TCamCaptureOperation operation( iController.CurrentVideoOperation() );

    if ( KWindowGroupId != iEikonServerWindowGroupId
      && KWindowGroupId != iBTServerWindowGroupId
      && ( ECamCapturing == operation
        || ECamPaused    == operation ) )
      {
      if ( !CamUtility::IdMatchesName( KWindowGroupId, KAknCapServer ) )
        {
        // If the video stops for an active call then inform the controller
        // this info is needed so that the stop tone isn't played
        if ( appUi->ForegroundAppIsPhoneApp() )
          {
          iController.SetVideoStoppedForPhoneApp( ETrue );
          }

        // If AknCapServer has NOT got foreground, (which would possibly 
        // indicate a "charging" note) stop video recording
        iController.StopVideoRecording();
        // reset the stopped for call flag
        iController.SetVideoStoppedForPhoneApp( EFalse );
        }
      }
      */
    }
  // -------------------------------------------------------

  CCamPreCaptureViewBase::HandleForegroundEventL( aForeground );

  PRINT( _L( "Camera <= CCamVideoPreCaptureView::HandleForegroundEventL" ) );    
  }

// -----------------------------------------------------------------------------
// CCamVideoPreCaptureView::HandleFocusLossL
// Handle change of focus
// -----------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::HandleFocusLossL()
  {
  PRINT( _L( "Camera => CCamVideoPreCaptureView::HandleFocusLossL" ) );    
  // if the application was already in the background, but behind an
  // eikon server window, then handle backgrounding properly now.
  // Otherwise the backgrounding will be handled in the normal
  // HandleForeground() method.
  if ( iContinueInBackground )
    {
    const TCamCaptureOperation operation( iController.CurrentOperation() );
    if ( ECamCapturing == operation
      || ECamPaused    == operation )
      {
      iController.StopVideoRecording();
      }
    }
  CCamPreCaptureViewBase::HandleFocusLossL();
  PRINT( _L( "Camera <= CCamVideoPreCaptureView::HandleFocusLossL" ) );    
  }

// -----------------------------------------------------------------------------
// CCamVideoPreCaptureView::HandleControllerEventL
// Handle controller events
// -----------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::HandleControllerEventL( TCamControllerEvent aEvent, 
                                                     TInt aError )
  {
  PRINT2( _L("Camera => CCamVideoPreCaptureView::HandleControllerEventL event (%d) error (%d)"), aEvent, aError );
  switch( aEvent )
    {
    // -----------------------------------------------------
    // If the recording state of the camera has changed (started/paused/stopped)
    // we may need to update the CBA labels
    case ECamEventOperationStateChanged:
      {
      if( iController.IsAppUiAvailable() )
          {
          const TCamCaptureOperation operation( iController.CurrentOperation() );
          if ( ECamCapturing == operation )
            {
            const TCamOrientation camOrientation = 
                static_cast<CCamAppUiBase*>( AppUi() )->CamOrientation();
            // if in camcorder mode need to show navi-ctr
            if( ECamOrientationCamcorder     == camOrientation 
             || ECamOrientationCamcorderLeft == camOrientation 
             || ECamOrientationPortrait == camOrientation )
              {
              iContainer->SetupNaviPanesL( CCamContainerBase::ECamPaneProgress );
              }
            else
              {
              static_cast<CCamAppUiBase*>( AppUi() )->PushNaviPaneProgressBarL(); 
              }
            }
          else if ( ECamCompleting == operation )
              {        
              iContainer->SetupNaviPanesL( CCamContainerBase::ECamPaneCounter );
              }
        else
          {
          // empty else statement to remove LINT error
          }
        UpdateCbaL();
        }
      break;
      }
    // -----------------------------------------------------
    case ECamEventRecordComplete:
      {
      static_cast<CCamAppUiBase*>( AppUi() )->PushNaviPaneCounterL();
      // if the recording failed due to a problem with the MMC
      if ( KErrNone != aError
              && iController.IntegerSettingValue( ECamSettingItemVideoMediaStorage ) 
              == ECamMediaStorageCard 
              && CamUtility::MemoryCardStatus() != ECamMemoryCardInserted )
          {
          StopViewFinder();
          StartViewFinder();
          }
      if ( static_cast<CCamAppUi*>( AppUi() )->ShowPostCaptureView() == EFalse 
              && !iController.IsCaptureStoppedForUsb() )
          {
          // fixed toolbar is used only with touch devices
          if ( iController.IsTouchScreenSupported() )
              {
              CAknToolbar* toolbar = Toolbar();
              if ( toolbar )
                  {
                  toolbar->SetToolbarVisibility( ETrue );
                  }
              }
          }

      if ( iController.IsCaptureStoppedForUsb() )
          {
          iContainer->SetupNaviPanesL( CCamContainerBase::ECamPaneCounter );
          }

      if ( EFalse == static_cast<CCamAppUi*>( AppUi() )->ShowPostCaptureView() )          
          {
          // Vf was stopped when stopping video recording, need to restart here
          // if postcapture is off.
          StartViewFinder();  
          } 
      break;
      }
    // -----------------------------------------------------
    case ECamEventEngineStateChanged:
      {
      CCamPreCaptureViewBase::HandleControllerEventL( aEvent, aError );
      break;
      }
    // -----------------------------------------------------
    case ECamEventSliderOpen:
        {
        iController.StartViewFinder();
        break;
        }
    default:
      {
      // Other events ignored
      break;
      }
    // -----------------------------------------------------
    }  
  PRINT ( _L("Camera <= CCamVideoPreCaptureView::HandleControllerEventL") );
  }


// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::CCamVideoPreCaptureView
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamVideoPreCaptureView::CCamVideoPreCaptureView( CCamAppController& aController )
    : CCamPreCaptureViewBase( aController )
    {
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::ConstructL()
    {
    BaseConstructL( ROID(R_CAM_VIDEO_PRE_CAPTURE_VIEW_ID));
    if(iController.UiConfigManagerPtr()->IsXenonFlashSupported())
        {
        CreateAndSetToolbarL(R_CAM_VIDEO_PRECAPTURE_TOOLBAR);
        UpdateToolbarIconsL();
        }
    else
        {
        CreateAndSetToolbarL(R_CAM_VIDEO_PRECAPTURE_TOOLBAR_VIDEOLIGHT);
        }
    CCamPreCaptureViewBase::ConstructL();
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::UpdateCbaL
// Update softkeys to reflect current state
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::UpdateCbaL()
  {
  PRINT( _L("Camera => CCamVideoPreCaptureView::UpdateCbaL") );

  CCamAppUi* appui = static_cast<CCamAppUi*>( AppUi() );
 
  // if videocall is active, set the softkeys already here 
  if ( iController.InVideocallOrRinging() )
      {
      SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );	
      }	
  // if the view is in capture setup menu mode
  if (  iCaptureSetupMenuModeActive 
    && !iCaptureSetupModeActive 
    && !iSceneSettingModeActive )
    {
    SetSoftKeysL( R_AVKON_SOFTKEYS_SELECT_BACK );
    }
  // if the view is in capture setup mode
  else if ( iCaptureSetupModeActive )
    {
    SetSoftKeysL( R_AVKON_SOFTKEYS_OK_CANCEL__OK );
    }
  // if the view is in scene settings mode
  else if ( iSceneSettingModeActive )
    {
    // R_AVKON_SOFTKEYS_OPTIONS_BACK
    SetSoftKeysL( R_CAM_SOFTKEYS_SETTINGS_SELECT_BACK__CHANGE ); //R_CAM_SOFTKEYS_OPTIONS_BACK__CHANGE );
    }
  else if ( iStandbyModeActive )
    {
    if( KErrNone == appui->StandbyStatus() )
      SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CONTINUE );
    else
      SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );
    }
        
  else // otherwise, show the default softkeys
    {
    switch ( iController.CurrentVideoOperation() )
      {
      // -------------------------------
      case ECamCapturing:
        {
        if(appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera())
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_PAUSE_STOP_SECONDARY );
            }
        else
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_PAUSE_STOP );
            }
        break;
        }
      // -------------------------------
      case ECamPaused:
        {
        if(appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera())
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_CONTINUE_STOP_SECONDARY );
            }
        else
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_CONTINUE_STOP );
            }
        break;
        }
      // -------------------------------
      case ECamNoOperation:
        {
        if ( iController.CaptureModeTransitionInProgress() )
          {
          if(iEmbedded )
              {
              if(appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera())
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
              if(appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera())
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
          if ( appui->TargetViewState() != ECamViewStatePostCapture )
            {
            if ( iEmbedded )
                {
                if(appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera())
                    {
                    SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK__RECORD_SECONDARY );
                    }
                else
                    {
                    SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK__RECORD );
                    }
                
                }
            else 
                {
                if ( !appui->IsToolBarVisible()
                    && appui->IsSecondCameraEnabled() 
                    && !appui->IsQwerty2ndCamera())
                    {
                    SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__RECORD_TXT );
                    }
                else if ( appui->DrawPreCaptureCourtesyUI() )
                    {
                    if(appui->IsSecondCameraEnabled() && !appui->IsQwerty2ndCamera())
                        {
                        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__RECORD_SECONDARY );
                        }
                    else
                        {
                        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__RECORD );
                        }
                    }
                else
                    {
                    SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
                    }
                }            
            }
          }
        break;
        }
      // -------------------------------
      case ECamCompleting:
        {
        if ( iController.IntegerSettingValue( ECamSettingItemVideoShowCapturedVideo ) == ECamSettOn &&
             !iController.IsCaptureStoppedForUsb() && appui->TargetViewState() == ECamViewStatePostCapture )
          { 
          //We need to blank the precapture view CBA here to prevent
          //blinking when changing back to precapture from postcapture.
          SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
          }
        break;	
        }
      // -------------------------------
      default:
        {
        // do nothing
        break;
        }
      // -------------------------------
      }
    }
  PRINT( _L("Camera <= CCamVideoPreCaptureView::UpdateCbaL") );
  }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::SetTitlePaneTextL
// Set the view's title text
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::SetTitlePaneTextL()
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
        TInt titleResourceId = R_CAM_VIDEO_PRE_CAPTURE_TITLE_NAME;
       
        if ( iCaptureSetupModeActive || iSceneSettingModeActive )
            {
            titleResourceId = CCamCaptureSetupViewBase::SetupModeTitlePaneResourceId();
            }

        appUi->SetTitleL( titleResourceId );
        }
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::StartViewFinder
// Enter viewfinder mode
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::StartViewFinder()
    {
    iController.EnterViewfinderMode( ECamControllerVideo );
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::StopViewFinder
// Exit viewfinder mode
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::StopViewFinder()
    {
    iController.ExitViewfinderMode( ECamControllerVideo );
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::CreateContainerL
// Create container control
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::CreateContainerL()
    {
    PRINT( _L("Camera => CCamVideoPreCaptureView::CreateContainerL" ) )

    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    iContainer = CCamVideoPreCaptureContainer::NewL(
        iController,
        *this,
        screen );

    iContainer->SetMopParent( this );

    CCamPreCaptureViewBase::CreateContainerL();
    PRINT( _L("Camera <= CCamVideoPreCaptureView::CreateContainerL" ) )
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::DynInitMenuPaneL
// Changes MenuPane dynamically
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    PRINT( _L("Camera => CCamVideoPreCaptureView::DynInitMenuPaneL" ));

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

//    if ( aResourceId == R_CAM_STANDBY_MENU && iStandbyError != KErrNone )
    if ( aResourceId == R_CAM_STANDBY_MENU && appUi->StandbyStatus() != KErrNone )
        {
        aMenuPane->SetItemDimmed( ECamCmdExitStandby, ETrue );
        }
    else if ( aResourceId == ROID(R_CAM_VIDEO_PRE_CAPTURE_MENU_ID))
        {
        DynInitSwitchCameraMenuItemL( aMenuPane ); 
        }
    else // otherwise, not in burst mode or menu pane is not default
        {
        // if embedded video set mute/unmute option appropriately, and set up
        // Switch camera item
        if( aResourceId == ROID(R_CAM_VIDEO_EMBEDDED_PRE_CAPTURE_MENU_ID))
            {
            DynInitSwitchCameraMenuItemL( aMenuPane );
            iAudioEnabled = iController.IntegerSettingValue( ECamSettingItemVideoAudioRec );
            // if audio off replace mute with unmute
            if( iAudioEnabled == ECamSettOff )
                {
                aMenuPane->SetItemDimmed( ECamCmdMute, ETrue );
                aMenuPane->SetItemDimmed( ECamCmdUnmute, EFalse );
                }
            else
                {
                aMenuPane->SetItemDimmed( ECamCmdMute, EFalse );
                aMenuPane->SetItemDimmed( ECamCmdUnmute, ETrue );
                }
/*#ifndef __WINS__
            // if embedded and not in in-call send
            if ( !appUi->IsInCallSend() )
#endif*/
        //        {
                aMenuPane->SetItemDimmed( ECamCmdNewPhoto, ETrue );
        //        }
            }

        CCamCaptureSetupViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );
        }
    PRINT( _L("Camera <= CCamVideoPreCaptureView::DynInitMenuPaneL" ))  
    }


// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::CreateCaptureSetupMenuL
// Creates a video capture setup menu
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::CreateCaptureSetupMenuL()
    {
    TInt resource;
    // check if we are Western or Arabic/Hebrew layout
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        if( AknLayoutUtils::Variant() == EApacVariant )
            {
            resource = ROID(R_CAM_CAPTURE_SETUP_MENU_VIDEO_DATA_APAC_ID);
            }
        else
            {
            // use Western resource
            resource = ROID(R_CAM_CAPTURE_SETUP_MENU_VIDEO_DATA_ID);
            }
        }
    else
        {
        resource = ROID(R_CAM_CAPTURE_SETUP_MENU_VIDEO_DATA_AH_ID);
        }

    // Use capture setup menu for second camera
    if ( static_cast<CCamAppUiBase*>( AppUi() )->IsSecondCameraEnabled() )
        {
        resource = ROID(R_CAM_CAPTURE_SETUP_MENU_VIDEO_DATA_ID);
        }

    iCaptureSetupMenuContainer = 
        CCamCaptureSetupMenu::NewL( iController, *this, AppUi()->ClientRect(),
        resource, iCaptureSetupMenuLastItemIndex );

    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SwitchToSceneSettingModeL
// Switches the current mode to scene setting and activates a 
// specific control.
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::SwitchToSceneSettingModeL()
    {
    if ( !iSceneSettingContainer )
    	{
	    // Create the scene setting container for video mode.
	    iSceneSettingContainer = CCamShootingModeContainer::NewL( AppUi()->ApplicationRect(),
	                                                           *this,
	                                                           ECamControllerVideo,
	                                                           iController );
	    iSceneSettingContainer->DrawableWindow()->SetOrdinalPosition(-1);
    	}
    
    iSettingModeTitleResourceId = R_CAM_VIDEO_SCENE_SETTING_TITLE;

    if ( iEmbedded )
        {
        iPreviousMenuResourceId = ROID(R_CAM_VIDEO_EMBEDDED_PRE_CAPTURE_MENUBAR_ID);
        }
    else
        {
        iPreviousMenuResourceId = ROID(R_CAM_VIDEO_PRE_CAPTURE_MENUBAR_ID);
        }
    // Remove the view's main container, and add the capture setup 
    // control associated with the input command to the container stack.
    CCamCaptureSetupViewBase::SwitchToSceneSettingModeL();

    // only remove the capture setup menu container after 
    // the switch completes successfully
    RemoveCaptureSetupMenuContainers();

    // Stop the viewfinder as it isn't required for scene settings
    StopViewFinder();
    
    }
 

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void 
CCamVideoPreCaptureView::DoActivateL( const TVwsViewId& aPrevViewId, 
                                            TUid        aCustomMessageId,
                                      const TDesC8&     aCustomMessage )
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMVIDEOPRECAPTUREVIEW_DOACTIVATEL, "e_CCamVideoPreCaptureView_DoActivateL 1" );
    PRINT( _L("Camera => CCamVideoPreCaptureView::DoActivateL") );
    PERF_EVENT_START_L2( EPerfEventVideoPreCaptureViewActivation );        
    // Ensure the correct menu is used for the current mode.
    CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );
    
    // fixed toolbar is used only with touch devices
    if ( iController.IsTouchScreenSupported() )
        {
        if ( appUi->IsEmbedded() )
            {
            CAknToolbar* toolbar = Toolbar();
            toolbar->SetToolbarVisibility(EFalse);
            }
        }
    else
        {
        // handled in CamPreCaptureViewBase
        }

    // Check if view is activated internally or externally
    TUid appUid = aPrevViewId.iAppUid;
    if( appUid != appUi->Application()->AppDllUid() )
      {
      // set capture mode 
      appUi->SetCaptureMode( ECamControllerVideo );
      }

    SetMenuBar();

    CCamPreCaptureViewBase::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );  
              
    PERF_EVENT_END_L2( EPerfEventVideoPreCaptureViewActivation );
    PRINT( _L("Camera <= CCamVideoPreCaptureView::DoActivateL") );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMVIDEOPRECAPTUREVIEW_DOACTIVATEL, "e_CCamVideoPreCaptureView_DoActivateL 0" );
    }

// ---------------------------------------------------------------------------
// SetMenuBar <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamVideoPreCaptureView::SetMenuBar()
  {
  PRINT( _L("Camera => CCamVideoPreCaptureView::SetMenuBar") );  
  CEikMenuBar* menuBar = MenuBar();
  if ( menuBar )
    {
    if ( iStandbyModeActive ) 
      {
      PRINT( _L("Camera <> CCamVideoPreCaptureView::SetMenuBar: setting standby menubar..") );
      menuBar->SetMenuTitleResourceId( R_CAM_STANDBY_MENUBAR );
      }
    else if ( iEmbedded )
      {
      PRINT( _L("Camera <> CCamVideoPreCaptureView::SetMenuBar: setting menu resource to R_CAM_VIDEO_EMBEDDED_PRE_CAPTURE_MENUBAR_ID") );
      menuBar->SetMenuTitleResourceId( ROID(R_CAM_VIDEO_EMBEDDED_PRE_CAPTURE_MENUBAR_ID));
      }
    else
      {
      PRINT( _L("Camera <> CCamVideoPreCaptureView::SetMenuBar: setting menu resource to R_CAM_VIDEO_PRE_CAPTURE_MENUBAR_ID") );
      menuBar->SetMenuTitleResourceId( ROID(R_CAM_VIDEO_PRE_CAPTURE_MENUBAR_ID) );
      }
    }
  PRINT( _L("Camera <= CCamVideoPreCaptureView::SetMenuBar") );
  }


// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::GetHelpContext
// Called to identify the help context for this view
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( KCameraappUID );
     
    // First check to see if the scene settings list is open
    if ( iSceneSettingModeActive )
        {
        aContext.iContext = KLCAM_HLP_SCENES_VIDEO;
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
        aContext.iContext = KLCAM_HLP_VIEWFINDER_VIDEO_EM;
        }
    // Else this is the plain photo viewfinder
    else
        {
        aContext.iContext = KLCAM_HLP_VIEWFINDER_VIDEO;
        }
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::MuteEmbeddedAudioL
// Mute or unmute audio for embedded video
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::MuteEmbeddedAudioL( TBool aMuteAudio )
    {
    if( aMuteAudio )
        {
        iController.SetIntegerSettingValueL( ECamSettingItemVideoAudioRec, ECamSettOff );
        }
    else
        {
        iController.SetIntegerSettingValueL( ECamSettingItemVideoAudioRec, ECamSettOn );
        }
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::DynInitToolbarL
// Dynamically initialize toolbar contents
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::DynInitToolbarL( TInt aResourceId, 
                                       CAknToolbar* aToolbar )
    {
    PRINT2( _L("Camera => CCamVideoPreCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar )
    (void)aResourceId; // remove compiler warning
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );

    iController.StartIdleTimer();
    // fixed toolbar is used only with touch devices
    if ( iController.IsTouchScreenSupported() )
        {
        UpdateToolbarIconsL();
        if( aToolbar )
            {            
            if ( iEmbedded && appUi->IsSecondCameraEnabled() )
                {
                aToolbar->SetItemDimmed(ECamCmdNewPhoto, ETrue, ETrue);
                }
            CAknToolbarExtension* extension = aToolbar->ToolbarExtension();
            if( extension )
                {
                if ( iEmbedded )
                    {
					extension->HideItemL( ECamCmdNewPhoto, ETrue );
                    extension->HideItemL( ECamCmdPhotos, ETrue );
                    }
                else
                    {
                    extension->HideItemL( ECamCmdNewPhoto, EFalse );
                    extension->HideItemL( ECamCmdPhotos, EFalse );
                    }
                if(aResourceId == ECamCmdToolbarExtension)
                    {
                    appUi->ZoomPane()->MakeVisible(EFalse,ETrue);
                    }
                }
            }
        }


    PRINT2( _L("Camera <= CCamVideoPreCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar )
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::UpdateToolbarIconsL
// Update fixed toolbar icons according to current settings
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::UpdateToolbarIconsL()
    {
    // fixed toolbar is used only with touch devices
    if (!iController.IsTouchScreenSupported() )
        return;
     UpdateVideoFlashIconsL();
     UpdateVideoSceneModeIconsL();
     UpdateVideoColorToneIconsL();
     UpdateVideoWhitebalanceIconsL();
	 RedrawToolBar();
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::OfferToolbarEventL
// Handle commands from toolbar
// ---------------------------------------------------------------------------
//
void CCamVideoPreCaptureView::OfferToolbarEventL( TInt aCommand )
    {
    PRINT1( _L("Camera => CCamVideoPreCaptureView::OfferToolbarEventL(%d)" ), aCommand );

    iController.StartIdleTimer();
    // fixed toolbar is used only with touch devices
    if ( iController.IsTouchScreenSupported() )
        {
        CCamPreCaptureViewBase::OfferToolbarEventL( aCommand );


        // Redraw video light button
        CAknToolbar* toolbar = Toolbar();
        if ( aCommand == ECamCmdToggleVideoFlash && 
             toolbar )
            {
             CAknButton* button = static_cast<CAknButton*>( 
                                  toolbar->ControlOrNull( aCommand ) );
			 if ( button )
                {
                button->DrawDeferred();
                }
            } 
        }
       
    PRINT1( _L("Camera <= CCamVideoPreCaptureView::OfferToolbarEventL(%d)" ), aCommand );
    }


// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::StartMskCapture()
// Try to start the capture with MSK command. Return true if started.
// ---------------------------------------------------------------------------
//
TBool CCamVideoPreCaptureView::StartMskCaptureL()
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    TCamOrientation orientation = appUi->CamOrientation();
    TBool startedCapture(EFalse);
    if( orientation == ECamOrientationCamcorder || 
        orientation == ECamOrientationCamcorderLeft ||
		orientation == ECamOrientationPortrait )
    	{ 
   		if ( appUi && !appUi->IsToolBarVisible()
       		 && iController.UiConfigManagerPtr() 
       		 && !iController.UiConfigManagerPtr()->IsOpticalJoyStickSupported() )
   		    {
    		// In case of MSK if no AP on screen, it should work as record
    		// If optical joystic is not available, we will start the 
    		// recording here. Otherwise it is handled
    		// in CCamVideoPreCaptureContainer
        	TInt callType( EPSCTsyCallTypeUninitialized );
            RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType, callType );
            if ( callType != EPSCTsyCallTypeH324Multimedia && iController.IsViewFinding() && appUi->CheckMemoryL() )
            {
                SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
                iController.StartVideoRecordingL();
                startedCapture = ETrue;
                // fixed toolbar is used only with touch devices
                if ( iController.IsTouchScreenSupported() )
                    {
                    CAknToolbar* fixedToolbar = Toolbar();
                    if ( fixedToolbar )
                        {
                        fixedToolbar->SetToolbarVisibility( EFalse );
                        }
                    }
                }
   		    }
	    // else let the calling function take care of capturing
    	}
        return startedCapture;
    }


// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::UpdateVideoFlashIconsL()
// ---------------------------------------------------------------------------
//
 void CCamVideoPreCaptureView::UpdateVideoFlashIconsL()
     {
     CAknButton* button = ToolbarButtonById(ECamCmdToggleVideoFlash);
     if(!button)
         return;
     MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
     TFileName iconFileName;
     CamUtility::ResourceFileName( iconFileName );
     TCamFlashId flash = static_cast< TCamFlashId > 
         ( iController.IntegerSettingValue(
                 ECamSettingItemDynamicVideoFlash ) );

     CAknButtonState* state = button->State();
     if ( state )
         {
         switch ( flash )
             {
             case ECamFlashOff:
                 {
                 SetIconL(
                     button,
                     iconFileName,
                     EMbmCameraappQgn_indi_cam4_tb_light_off,
                     EMbmCameraappQgn_indi_cam4_tb_light_off_mask,
                     skinInstance,
                     KAknsIIDQgnIndiCam4TbLightOff );
                 HBufC* tooltipText = 
                     StringLoader::LoadLC( R_QTN_LCAM_TT_VIDEOFLASH );
                 state->SetHelpTextL( *tooltipText );
                 CleanupStack::PopAndDestroy( tooltipText );
                 break;
                 }
             case ECamFlashForced:
                 {
                 SetIconL(
                     button,
                     iconFileName,
                     EMbmCameraappQgn_indi_cam4_tb_light,
                     EMbmCameraappQgn_indi_cam4_tb_light_mask,
                     skinInstance,
                     KAknsIIDQgnIndiCam4TbLight ); 
                 HBufC* tooltipText =
                     StringLoader::LoadLC( R_QTN_LCAM_TT_FLASHOFF );
                 state->SetHelpTextL( *tooltipText );
                 CleanupStack::PopAndDestroy( tooltipText );
                 break;
                 }
             default:
                 break;
             }
         }
     }

 // ---------------------------------------------------------------------------
 // CCamVideoPreCaptureView::UpdateVideoSceneModeIconsL()
 // ---------------------------------------------------------------------------
 //
 void CCamVideoPreCaptureView::UpdateVideoSceneModeIconsL()
     {
     CAknButton* button = ToolbarButtonById(ECamCmdCaptureSetupSceneVideo);
     if(!button)
         {
         return;
         }
         
     MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
     TFileName iconFileName;
     CamUtility::ResourceFileName( iconFileName );
     TCamSceneId scene = static_cast< TCamSceneId > ( 
         iController.IntegerSettingValue( ECamSettingItemDynamicVideoScene ) );
     switch ( scene )
         {
         case ECamSceneAuto:
         case ECamSceneNormal:
             {
             SetIconL(
                 button,
                 iconFileName,
                 EMbmCameraappQgn_indi_cam4_mode_auto,
                 EMbmCameraappQgn_indi_cam4_mode_auto_mask,
                 skinInstance,
                 KAknsIIDQgnIndiCam4ModeAuto );
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
             break;
             }
         case ECamSceneLowLight: 
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
         default:
             {
             //Do Nothing
             }
             break;
         }
     }

 // ---------------------------------------------------------------------------
 // CCamVideoPreCaptureView::UpdateVideoColorToneIconsL()
 // ---------------------------------------------------------------------------
 //
void CCamVideoPreCaptureView::UpdateVideoColorToneIconsL()
    {
    CAknButton* button = ToolbarButtonById(ECamCmdCaptureSetupColourFilterVideo);
    if(!button)
        return;
    if ( !iController.UiConfigManagerPtr()->IsColorToneFeatureSupported() )
        {
        HideButton(ECamCmdCaptureSetupColourFilterVideo);
        return;
        }
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    TFileName iconFileName;
    CamUtility::ResourceFileName( iconFileName );

    TCamColourFilterId color = static_cast< TCamColourFilterId > 
        ( iController.IntegerSettingValue( ECamSettingItemDynamicVideoColourFilter ) );
        
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

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureView::UpdateVideoWhitebalanceIconsL()
// ---------------------------------------------------------------------------
//
 void CCamVideoPreCaptureView::UpdateVideoWhitebalanceIconsL()
     {
     CAknButton* button = ToolbarButtonById(ECamCmdCaptureSetupWhiteBalanceVideo);
     if(!button)
         return;
     MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
     TFileName iconFileName;
     CamUtility::ResourceFileName( iconFileName );
     if ( !iController.UiConfigManagerPtr()->IsWhiteBalanceFeatureSupported() )
         {
         HideButton( ECamCmdCaptureSetupWhiteBalanceVideo);
         return;
         }
     TCamWhiteBalanceId wb = static_cast< TCamWhiteBalanceId > 
         ( iController.IntegerSettingValue( ECamSettingItemDynamicVideoWhiteBalance ) );
         
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


//  End of File  
