/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <eikbtgpc.h>
#include <eikmenub.h>
#include <musresourceproperties.h>
#include <cameraplatpskeys.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <aknbutton.h>
#include <eiksoftkeypostingtransparency.h>
#include <AknsUtils.h>
#include <gulicon.h>

#include "Cam.hrh"
#include "CamAppUi.h"
#include "CamUtility.h"
#include "CamAppController.h"
#include "CamPreCaptureContainerBase.h"
#include "CamLogger.h"
#include "CamPanic.h"
#include "CamCaptureSetupMenu.h"
#include "CamStandbyContainer.h"
#include "CamSelfTimer.h"
#include "CamTimer.h"
#include "camactivepalettehandler.h"
#include "CamPreCaptureViewBase.h"
#include "CameraUiConfigManager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamPreCaptureViewBaseTraces.h"
#endif


// ===========================================================================
// Constants



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase destructor
// 
// ---------------------------------------------------------------------------
//
CCamPreCaptureViewBase::~CCamPreCaptureViewBase()
  {
  PRINT( _L("Camera => ~CCamPreCaptureViewBase") );
  if ( iGestureFw ) 
      {
      delete iGestureFw;
      }
  PRINT( _L("Camera <= ~CCamPreCaptureViewBase") );
  }

// -----------------------------------------------------------------------------
// CCamPreCaptureViewBase::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::HandleCommandL( TInt aCommand )
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::HandleCommandL") );

    iLastMovement = 0;
    switch ( aCommand )
        {              
        case ECamCmdShootSetup:
            {
            CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
            TBool selfTimerEnabled = appUi->SelfTimerEnabled();

            if ( selfTimerEnabled )
                {                
                CCamSelfTimer* selftimer = appUi->SelfTimer();

                // If self-timer is enabled, we need to check that it is in
                // self-timer standby, ie Enabled but not Active (ie not 
                // counting down)
                if ( selftimer && !selftimer->IsActive() )
                    {
                    // In self-timer standby, pressing "select" button starts 
                    // self-timer count down.  Pass this event on to AppUi.
                    appUi->HandleCommandL( ECamCmdSelfTimerActivate );
                    }
                }
            else 
                {
                // we are not in self -timer mode and check we are
                // not already in capture setup mode
                if ( !iCaptureSetupMenuModeActive )
                    {
                    // Ensure first item is selected in CSM.
                    iCaptureSetupMenuLastItemIndex = 0;
                    SwitchToCaptureSetupMenuModeL();
                    }
                    
                }            
            }
            break;

        // If an item in the capture setup menu has been selected.
        case EAknSoftkeySelect:  
            {
            if ( ActiveSettingMode() == ECamSettingModeCaptureMenu )
                {
                iCaptureSetupMenuContainer->HandleCommandL( aCommand );
                }
            else
                {          
                CCamCaptureSetupViewBase::HandleCommandL( aCommand );             
                }
            }
            break;
            
        // If capture setup menu is active and user presses softkey back,
        //  deactivate the menu. This is EEikBidCancel which has the same
        //  enumeration value as EAknSoftkeyCancel.
        case EAknSoftkeyCancel:
            {            
            if ( ActiveSettingMode() == ECamSettingModeCaptureMenu )
                {
                ExitCaptureSetupMenuModeL();
                }
            else if ( iController.SequenceCaptureInProgress() )                         
                // Check if currently capturing a burst, if so, cancel it
                {
                // If already received all the snapshots
                if ( iController.AllSnapshotsReceived() ) 
                    {
                    // stop burst immediately
                    iController.StopSequenceCaptureL();
                    }
                else
                    {
                    // capture the min images (2), before stopping the burst
                    iController.SetKeyUp( ETrue );
                    iController.SoftStopBurstL( ETrue );
                    }
                // Update CBA for setup pane usage
                SetSoftKeysL( R_CAM_SOFTKEYS_BLANK ); 
                }     
            else
                {
                CCamCaptureSetupViewBase::HandleCommandL( aCommand );                                                                                       
                }                        
            }
            break;
        case EAknSoftkeyOptions:
            {
            // do nothing, the command is handled by the framework
            }
            break;
            
        // Note: CaptureSetupMenu mode also uses a 'Back' softkey but this has
        // the command value EEikBidCancel            
        case EAknSoftkeyBack:
            {                                          
            // exit to embedded app if we're embedded and in plain pre-capture
            if ( iEmbedded 
                && ActiveSettingMode() != ECamSettingModeScene 
                && ActiveSettingMode() != ECamSettingModeInfoListBox )
                {
                CCamCaptureSetupViewBase::HandleCommandL( ECamCmdInternalExit );
                }
            else
                {
                CCamCaptureSetupViewBase::HandleCommandL( aCommand );              
                }                           
               
            }
            break;
            
        case EAknSoftkeyOk:
            {                                                                
                // Else pass on to lower levels
                CCamCaptureSetupViewBase::HandleCommandL( aCommand );                                     
            }
            break;
            
        case ECamCmdToggleCourtesyUI:
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
            
            if ( appUi )
                {
                appUi->SetAlwaysDrawPreCaptureCourtesyUI( !appUi->AlwaysDrawPreCaptureCourtesyUI() );
                }
            }
            break;
       case ECamCmdPopUpMenuZoom: 
            {
            iContainer->ShowZoomPaneWithTimer();
            }     
            break;
        case ECamCmdCaptureSetupFlashStill:
            {
            iToolbarExtensionInvisible = ETrue;
           	CCamCaptureSetupViewBase::HandleCommandL( aCommand );
            }     
            break;
        case ECamCmdInternalExit:
            {
            const TCamCaptureOperation operation( 
                    iController.CurrentOperation() );
            
                 if ( ECamPaused == operation )
                   {
                   iController.StopVideoRecording();
                   iController.SetCaptureKeyPressedWhileImageSaving( ETrue );
                   }
            StopViewFinder();
            CCamCaptureSetupViewBase::HandleCommandL( aCommand );
            if ( iController.IsTouchScreenSupported() )
                {
                CAknToolbar* fixedToolbar = Toolbar();
                if ( fixedToolbar )
                    {
                    CAknToolbarExtension* extension = 
                        fixedToolbar->ToolbarExtension();
                    if ( extension )
                        {
                        // Reset toolbar extension button and view to closed state
                        extension->SetShown( EFalse );
                        extension->SetCurrentState( 0, EFalse );
                        }
                    }
                }
            }     
            break;
        default:
            {                                       
            CCamCaptureSetupViewBase::HandleCommandL( aCommand );                            
            }
        }
    PRINT( _L("Camera <= CCamPreCaptureViewBase::HandleCommandL") );
    }

// -----------------------------------------------------------------------------
// CCamPreCaptureViewBase::HandleForegroundEventL
// Handle a move to the foreground
// -----------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::HandleForegroundEventL( TBool aForeground )
  {
  PRINT1( _L( "Camera => CCamPreCaptureViewBase::HandleForegroundEventL %d" ), aForeground );    

  iLastMovement = 0;
  CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );                
  __ASSERT_DEBUG(appUi, CamPanic(ECamPanicNullPointer));

  // -----------------------------------------------------
  // coming to the foreground
  if ( aForeground )
    {
    // Ignore foregrounding if view switch to postcapture anyway in progress.
    if( appUi->TargetViewState() == ECamViewStatePostCapture
     || appUi->TargetViewState() == ECamViewStateBurstThumbnail ) {
      PRINT( _L( "Camera <= CCamPreCaptureViewBase::HandleForegroundEventL, ignored as going to postcapture") );    
      return;
    }
    
    
    if ( !( iController.UiConfigManagerPtr() && 
            iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() ) )
        {
        if ( !iController.IsTouchScreenSupported() )
            {
            if ( !appUi->ActivePalette() )
                {
                SetupActivePaletteL();
                }
            else
                {
                appUi->APHandler()->UpdateActivePaletteL();
                }
            }
        }
    UpdateCbaL();
    
    // restart idle timer in any case
     iController.StartIdleTimer();
                                 
    // If capture setup menu is active then ensure background is faded.
    if ( iCaptureSetupMenuContainer )
      {
      iCaptureSetupMenuContainer->FadeBehind( ETrue );
      }       

    // coming to the foreground from an eikon server window
    if ( iContinueInBackground )
      {
      iContinueInBackground = EFalse;

      if ( !iController.IsViewFinding()  && !iSceneSettingModeActive )
        {
      	iController.StartViewFinder();
        }
      else if( iLocationTrailConnecting )
          {
          iLocationTrailConnecting = EFalse;
          iController.StopViewFinder();
          iController.StartViewFinder();
          }
      CCamCaptureSetupViewBase::HandleForegroundEventL( aForeground );

      UpdateCbaL();
      if ( iController.CameraSwitchQueued() )
          {
          appUi->HandleCommandL( ECamCmdSwitchCamera );
          }
      PRINT( _L( "Camera <= CCamPreCaptureViewBase::HandleForegroundEventL - continuing in background" ) );            
      return;
      }
    
    // This only happens when the app is first launched        
    if ( !appUi->IsConstructionComplete() && 
         !iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported())    
      {
      iActivateToStandby = ETrue;
      if ( !iStandbyModeActive )
        {
        //iStandbyError = KErrNone;
        appUi->SetStandbyStatus( KErrNone );        
        PRINT( _L( "Camera <> CCamPreCaptureViewBase::HandleForegroundEventL: sending ECamCmdGoToStandby" ) );    
        HandleCommandL( ECamCmdGoToStandby );
        SetTitlePaneTextL();
        }            
      }

    // Register that we want to use the engine
    IncrementCameraUsers();
    
    if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() )
        {
        if ( iController.CameraSwitchQueued() )
            {
            appUi->HandleCommandL( ECamCmdSwitchCamera );
            }
        else if ( appUi->TargetViewState() != ECamViewStatePostCapture )      
            {   
            TCamCameraMode mode = iController.CurrentMode(); 
            if ( mode == ECamControllerVideo )
                {
                iController.EnterViewfinderMode( ECamControllerVideo );
                }       
            else if( mode == ECamControllerImage && !iSceneSettingModeActive )
                {
                iController.EnterViewfinderMode( ECamControllerImage );
                }  
            }
        UpdateCbaL();
        }
      else
        {
        // Lens cover is not supported
        if ( iController.CameraSwitchQueued() )   
            {
            appUi->HandleCommandL( ECamCmdSwitchCamera );
            }
        }


      // Start the viewfinder unless the view is activating into standby mode
      // in other modes than scene setting mode (there is no VF and this causes problems with DSA VF)
      if ( ( !iStandbyModeActive || !iActivateToStandby )  && !iSceneSettingModeActive )
        {
        // If in embedded mode and image has been captured but view is not yet in
        // postcapture view VF should not be started.
        if( !iAssumePostCaptureView ) 
            {
            PRINT( _L("Camera <> CCamPreCaptureViewBase::HandleForegroundEventL start VF"))
            StartViewFinder();
            }
        }
    // Check if currently selected storagemedia has available memory for next capturing
    appUi->CheckMemoryAvailableForCapturing();                                   
    
    // <CAMERAAPP_CAPI_V2_MIGRATION/>
    // if ( iController.EngineState() == ECamEngineStillCapturePrepared ||
    //      iController.EngineState() == ECamEngineVideoCapturePrepared )
    // if ( ( ECamImageOn|ECamVideoOn )& ( iController.CameraState() ) )
    TCamCameraState state = iController.CameraState();
    if( ECamCameraPreparedImage == state ||
        ECamCameraPreparedVideo == state )     
      {
      // Now that the view is initialised, we may need to put up the
      // mmc removal note. Perform this is required.
      appUi->ShowMMCRemovalNoteIfRequiredL();                        
      
      if ( appUi && !iSceneSettingModeActive &&
           !( iController.UiConfigManagerPtr() && 
           iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() ) )
        {
        appUi->RaisePreCaptureCourtesyUI(ETrue);
        }
      }
    }
  // -----------------------------------------------------
  // Going to the background
  else 
      {
      PRINT( _L( "Camera <> CCamPreCaptureViewBase::HandleForegroundEventL: to background" ) );    
      // Not behind an eikon/avkon server window
	  // or keylock on when camera needs to be released.
      TBool totalBackground = ETrue;
      if ( iController.IsAppUiAvailable() && !iController.IsKeyLockOn() )
          {
          totalBackground    = appUi->AppInBackground( EFalse );
          }
      // Either in foreground (moving to another view of our's)
      // or if in background, then not just behind a notifier dialog.
      if ( totalBackground )
          {
          ReleaseResources();
          // hide the AP to avoid blinking when coming back from bg
         if ( iController.UiConfigManagerPtr() && iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
            {
            appUi->SetActivePaletteVisibility( EFalse );	
            }      
          }
      // Behind a server window
      else 
          {
          iContinueInBackground = ETrue;            
          }

      // reset post capture assumption flag since post capture is not
      // used when video is stopped for usb 
      if ( iController.IsCaptureStoppedForUsb() )
          {
          iAssumePostCaptureView = EFalse;
          }
      }
  // -----------------------------------------------------

  if ( !iStandbyModeActive )
    {
    // Inform the container of foreground state change
    PRINT( _L( "Camera <> CCamPreCaptureViewBase::HandleForegroundEventL: inform container.." ) );        
    iContainer->HandleForegroundEventL( aForeground );
    }
  else
    {
    // if in videocall, don't exit standby	
    if ( iController.InVideocallOrRinging() )
        {
        iActivateToStandby = EFalse;
        }
    else    		
        {   	
        // If the view has been set to standby at activation then
        // the first foreground event should not be used to exit standby
        if ( !iActivateToStandby )
           {
           // inform standby container
           PRINT( _L( "Camera <> CCamPreCaptureViewBase::HandleForegroundEventL: inform standby container.." ) );        
           iStandbyContainer->HandleForegroundEventL( aForeground );
           }  
        else
           {
           iActivateToStandby = EFalse;
           }
        }
    }
         CCamCaptureSetupViewBase::HandleForegroundEventL( aForeground );
  PRINT( _L( "Camera <= CCamPreCaptureViewBase::HandleForegroundEventL" ) );        
  }

// -----------------------------------------------------------------------------
// CCamPreCaptureViewBase::SetLocationTrailConnecting
// Set iLocationTrailConnecting flag.
// -----------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SetLocationTrailConnecting()
    {
    iLocationTrailConnecting = ETrue;
    }

// -----------------------------------------------------------------------------
// CCamPreCaptureViewBase::SetPostCaptureViewAsumption
// Set iAssumePostCaptureView flag.
// -----------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SetPostCaptureViewAsumption( TBool aAssumePostCaptureView )
    {
    iAssumePostCaptureView = aAssumePostCaptureView;
    }


// -----------------------------------------------------------------------------
// CCamPreCaptureViewBase::HandleControllerEventL
// Handle controller events
// -----------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::HandleControllerEventL( TCamControllerEvent aEvent, 
                                                     TInt /*aError*/ )
  {
  PRINT( _L( "Camera => CCamPreCaptureViewBase::HandleControllerEventL" ) );        
  CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );

  switch( aEvent )
    { 
    // ---------------------------------------------------
    case ECamEventEngineStateChanged:
      {
      // Do not do softkey update if we are already exiting
      // (AppUi is not available)
      if( iController.IsAppUiAvailable() )
      {          
      PRINT( _L( "Camera <> case ECamEventEngineStateChanged" ) );        
      // <CAMERAAPP_CAPI_V2_MIGRATION/>
      // if ( iController.EngineState() == ECamEngineStillCapturePrepared ||
      //      iController.EngineState() == ECamEngineVideoCapturePrepared )
      if ( ECamCameraPreparedVideo == iController.CameraState() || 
           ECamCameraPreparedImage == iController.CameraState() )
        {
        // Now that the view is initialised, we may need to put up the
        // mmc removal note. Perform this is required.
        PRINT( _L( "Camera <> show mmc note if needed" ) );        
        appUi->ShowMMCRemovalNoteIfRequiredL();

        // Do not update (show) precapture view toolbar if capturing is almost
        // finished, otherwise there is flickering and finally inactive toolbar 
        // when returning to prepactureview after image deletion. 
        // This will happen only if camera is started directly to portrait 
        // secondary camera i.e. Lens cover feature is supported
        TBool noToolbar = (ECamCompleting == iController.CurrentOperation() ) && appUi->IsSecondCameraEnabled();

        if ( ECamCameraPreparedImage == iController.CameraState() &&
               !appUi->IsBurstEnabled() && !noToolbar )
            {
            // Also fixed toolbar might need to be enabled.
            appUi->SetToolbarVisibility();
            }
        }
      //Do not update CBA if we are capturing or paused to prevent blinking
	  //during video capture saving and completing
      if ( iController.CurrentVideoOperation() != ECamCapturing
             && iController.CurrentVideoOperation() != ECamPaused )
          {
          PRINT( _L( "Camera <> Update CBA" ) );        
          UpdateCbaL();
          }
      }
      break;
      }

    case ECamEventSliderClosed:
      {
      // if embedded and the slider is closed, 
      // the menu items may change, close the menu   
      if( iEmbedded )
        {
        PRINT( _L("Camera <> CCamPreCaptureViewBase: calling StopDisplayingMenuBar") );
        StopDisplayingMenuBar();
        }
      break;
      }
    default:
      {
      // Other events => no action.
      break;
      }
    } // switch
  PRINT( _L( "Camera <= CCamPreCaptureViewBase::HandleControllerEventL" ) );        
  }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::DoActivateL( const TVwsViewId& aPrevViewId, 
                                          TUid aCustomMessageId,
                                          const TDesC8& aCustomMessage )                                                                                    
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMPRECAPTUREVIEWBASE_DOACTIVATEL, "e_CCamPreCaptureViewBase_DoActivateL 1" );
    PRINT( _L( "Camera => CCamPreCaptureViewBase::DoActivateL" ) );
    iAssumePostCaptureView = EFalse;
    iLastMovement = 0;

    ShowToolbarOnViewActivation( EFalse );
    /*if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsLocationSupported() )
        {
        if( ECamLocationOn == iController.IntegerSettingValue( ECamSettingItemRecLocation ) )
            {
            if( ECamActiveCameraPrimary == iController.ActiveCamera() && ECamStandby != iController.CurrentOperation() )
                {
                PRINT( _L("Camera: CCamPreCaptureViewBase::DoActivateL - primary camera, start location/gps") )
                iController.StartLocationTrailL();
                }
            else // Secondary camera 
                {
                PRINT( _L("Camera: CCamPreCaptureViewBase::DoActivateL - secondary camera, stop location trail") )
                iController.StopLocationTrail();
                }
            }
        }*/
 
    CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );
	__ASSERT_DEBUG(appUi, CamPanic(ECamPanicNullPointer));
    if(!appUi->DrawPreCaptureCourtesyUI() && !iController.IsTouchScreenSupported())
        {
        if ( !appUi->IsSecondCameraEnabled() )
            {	
            SetCourtesySoftKeysL();
            }
        appUi->SetActivePaletteVisibility( EFalse );
        }
    else if (appUi->DrawPreCaptureCourtesyUI())
        {
         Cba()->MakeVisible( ETrue );
        }

    if ( appUi->SettingsLaunchedFromCamera() )
    	{
    	if ( TUid::Uid( ECamCmdInternalExit ) == aCustomMessageId )
    	    {
            appUi->SettingsPluginExitedL( ECameraInternalExit );
    	    }
    	else if ( TUid::Uid ( EAknSoftkeyBack ) == aCustomMessageId )
    	    {
    	    appUi->SettingsPluginExitedL( ECameraPluginExit );
    	    }
    	else
    	    {
    	    // do nothing
    	    }
    	}

    CCamCaptureSetupViewBase::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );

    if ( appUi->IsSecondCameraEnabled() && !appUi->IsQwerty2ndCamera() )  
        {
        StatusPane()->MakeVisible( ETrue );
        }
    else
        {
        StatusPane()->MakeVisible( EFalse );
        }
    if ( iController.IsTouchScreenSupported() )
        {
        CAknToolbar* fixedToolbar = Toolbar();
        if ( fixedToolbar )
            {
            // If video call is active, toolbar is set to dimmed, that user cannot use it
            TInt callType( EPSCTsyCallTypeUninitialized );
            RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType, callType );
            if ( callType == EPSCTsyCallTypeH324Multimedia)
                {
                fixedToolbar->SetDimmed( ETrue ); 
                }
            else
                {
                fixedToolbar->SetDimmed( EFalse ); 
                }
            fixedToolbar->SetToolbarObserver( this );
            appUi->SetToolbarVisibility( );

            // Make sure toolbar extension button has no background
            CAknToolbarExtension* extension = fixedToolbar->ToolbarExtension();
            if ( extension )
                {
                extension->SetButtonFlags(
                    extension->ButtonFlags() | KAknButtonNoFrame );
                extension->DrawDeferred();
                }
            }
        }

    appUi->PushNaviPaneCounterL();

    // Pass on the ID of the previous view to the container    
    TCamAppViewIds viewId = static_cast<TCamAppViewIds>( aPrevViewId.iViewUid.iUid );
    iContainer->SetPreviousViewId( viewId );

    iController.AddControllerObserverL( this );    

    TBool transparentCba = EFalse;

    UpdateCbaL();
    
    switch( aCustomMessageId.iUid )
      {
      // -------------------------------
      // Standby with or without error
      case ECamViewMessageStandby:
        PRINT( _L( "Camera <> CCamPreCaptureViewBase::DoActivateL: custom message ECamViewMessageStandby" ) );
        //iStandbyError = KErrNone;
        //appUi->SetStandbyStatus( KErrNone );
        SetSceneSettingMode( EFalse );
        iActivateToStandby = ETrue;
        HandleCommandL( ECamCmdGoToStandby );
        return;
//        break;
      // -------------------------------
      // Scene setting mode
      case ECamViewMessageSceneSettingList:
        PRINT( _L( "Camera <> CCamPreCaptureViewBase::DoActivateL: custom message ECamViewMessageSceneSettingList" ) );
        SwitchToSceneSettingModeL();
        break;
      // -------------------------------
      default:
        // Other precapture modes have transparent CBA buttons
        // except secondary camera
        if ( !appUi->IsSecondCameraEnabled() || appUi->IsQwerty2ndCamera() )  
            {
            transparentCba = ETrue;
            }
        // don't start VF if faster startup is used when cameraswitch is queued,
        // need to switch to maincamera first
        if ( !( iController.UiConfigManagerPtr() && 
                iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() &&
                iController.CameraSwitchQueued() ) )
            {
            StartViewFinder();
            }
        
        break;
      // -------------------------------
      }
    
    // Here we check if the previous view is ViaPlayer play mode
    if ( !iSceneSettingModeActive  && 
         ( aPrevViewId.iViewUid.iUid != ECamViewIdViaPlayerPlayBack ) && 
         !( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() ) )
        {
        appUi->RaisePreCaptureCourtesyUI(ETrue);
        }
    
    // Reset iContinueInBackground flag, which is set when app temporarily
    // goes behind a system window and there's no need to free resources.
    // Resetting this flag only in ReleaseResources causes problems (at least)
    // if power key is pressed during burst capture.
    iContinueInBackground = EFalse;
    
    // Reset iDeactivateToStandby flag, which is used when the PreCapture view
    // goes to the Standby mode, at which point the UI should not reactivate the
    // viewfinder.
    iDeactivateToStandby = EFalse;


    EikSoftkeyPostingTransparency::MakeTransparent(
        *Cba(), transparentCba );
    // Make sure appuis CBA is not visible
    CEikButtonGroupContainer* cba = AppUi()->Cba();
    if( cba )
        {
        cba->MakeVisible( EFalse );
        }

    PRINT( _L( "Camera <= CCamPreCaptureViewBase::DoActivateL" ) );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMPRECAPTUREVIEWBASE_DOACTIVATEL, "e_CCamPreCaptureViewBase_DoActivateL 0" );
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::DoDeactivate()
    {                              
    PRINT( _L( "Camera => CCamPreCaptureViewBase::DoDeactivate" ) );
    CCamCaptureSetupViewBase::DoDeactivate();
                               
    iController.RemoveControllerObserver( this );
    
    if ( iController.IsTouchScreenSupported() )
        {
        CAknToolbar* fixedToolbar = Toolbar();
        if ( fixedToolbar )
            {
            CAknToolbarExtension* extension = fixedToolbar->ToolbarExtension();
            if ( extension )
                {
                // Reset toolbar extension button and view to closed state
                extension->SetShown( EFalse );
                extension->SetCurrentState( 0, EFalse );
                }
            }
        }
    ReleaseResources();

    if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() &&
         iController.ActiveCamera() == ECamActiveCameraSecondary ) 
        {
        if ( iController.CameraSwitchQueued() )
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
            // If camera switch fails, main camera is used instead
            TRAP_IGNORE( appUi->HandleCommandL( ECamCmdSwitchCamera ) );
            }
        }

    PRINT( _L( "Camera <= CCamPreCaptureViewBase::DoDeactivate" ) );
    }

// -----------------------------------------------------------------------------
// CCamPreCaptureViewBase::HandleFocusLossL
// Handle change of focus
// -----------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::HandleFocusLossL()
    {
    PRINT( _L( "Camera => CCamPreCaptureViewBase::HandleFocusLossL" ) );    
    // if the application was already in the background, but behind an
    // eikon server window, then handle backgrounding properly now.
    // Otherwise the backgrounding will be handled in the normal
    // HandleForeground() method.
    if ( iContinueInBackground )
        {
        ReleaseResources();
        // Inform the container of foreground state change
        iContainer->HandleForegroundEventL( EFalse );
        }
    CCamCaptureSetupViewBase::HandleFocusLossL();

    // dismiss menu bar if open
    StopDisplayingMenuBar();
    PRINT( _L( "Camera <= CCamPreCaptureViewBase::HandleFocusLossL" ) );
    }
                          
// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SwitchToCaptureSetupModeL
// Switches the current mode to capture setup and activates a 
// specific control..
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SwitchToCaptureSetupModeL( TInt aSetupCommand )
    {
    
    PRINT( _L("Camera => CCamPreCaptureViewBase::SwitchToCaptureSetupModeL") );
    
    // Remove the view's main container, and add the capture setup 
    // control associated with the input command to the container stack.
    CCamCaptureSetupViewBase::SwitchToCaptureSetupModeL( aSetupCommand );

    // only remove the capture setup menu container after 
    // the switch completes successfully
    RemoveCaptureSetupMenuContainers();

    SetCaptureSetupModeActive(ETrue);
    
    PRINT( _L("Camera <= CCamPreCaptureViewBase::SwitchToCaptureSetupModeL") );
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SwitchToSceneSettingModeL
// Switches the current mode to scene setting and activates a 
// specific control.
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SwitchToSceneSettingModeL()
    {
    // Remove the view's main container, and add the capture setup 
    // control associated with the input command to the container stack.
    CCamCaptureSetupViewBase::SwitchToSceneSettingModeL();
 
    // only remove the capture setup menu container after 
    // the switch completes successfully
    RemoveCaptureSetupMenuContainers();
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ExitCaptureSetupModeL
// Exits capture setup mode and activates the capture setup menu control.
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::ExitCaptureSetupModeL()
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::ExitCaptureSetupModeL") )
     
    
    iController.SetViewfinderWindowHandle( &iContainer->Window() );
    CCamCaptureSetupViewBase::ExitCaptureSetupModeL();
    
    if( !iController.IsViewFinding() && !iController.InVideocallOrRinging() )
      {
      TCamCameraMode mode = iController.CurrentMode();    
	  PRINT1( _L("Camera => CCamPreCaptureViewBase::ExitCaptureSetupModeL - CurrentMode %d"), mode );        
      if (mode == ECamControllerVideo )
    	{
   	    PRINT( _L("Camera => CCamPreCaptureViewBase::ExitCaptureSetupModeL - ECamControllerVideo") );        
    	iController.EnterViewfinderMode( ECamControllerVideo );
    	}    	
      else
    	{
   	    PRINT( _L("Camera => CCamPreCaptureViewBase::ExitCaptureSetupModeL - ECamControllerImage") );        
      iController.EnterViewfinderMode( ECamControllerImage );
    	}    	               
      }
    
    // make sure app is not left in an invalid state if a leave
    // occurs after exiting the capture setup
    iCaptureSetupMenuModeActive = EFalse;
    static_cast<CCamAppUiBase*>( AppUi() )->PushNaviPaneCounterL();
	CCamAppUiBase* appUi =  static_cast<CCamAppUiBase*>( AppUi() );
	if( appUi->ActiveCamera() == ECamActiveCameraSecondary )
		{
		PRINT( _L("Camera => CCamPreCaptureViewBase::ExitCaptureSetupModeL ###") )
    	SwitchToCaptureSetupMenuModeL();
		}
	
	UpdateCbaL();
    PRINT( _L("Camera <= CCamPreCaptureViewBase::ExitCaptureSetupModeL") )
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ExitSceneSettingModeL
// Exits scene setting mode and activates the capture setup menu control.
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::ExitSceneSettingModeL()
    {
    CCamCaptureSetupViewBase::ExitSceneSettingModeL();
    // make sure app is not left in an invalid state if a leave
    // occurs after exiting the scene setting mode
    iCaptureSetupMenuModeActive = EFalse;
    static_cast<CCamAppUiBase*>( AppUi() )->PushNaviPaneCounterL();

	CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    if ( appUi->ActiveCamera() == ECamActiveCameraPrimary )
        {
        if ( iDeactivateToStandby || iController.InVideocallOrRinging() )
            {
            // don't restart VF if we are going to standby mode
            }
        else
            {
            StartViewFinder();
            }
        }
    else
		{
		SwitchToCaptureSetupMenuModeL();
		}
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ExitInfoListBoxL
// Exits InfoListBox
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::ExitInfoListBoxL()
    {
    CCamCaptureSetupViewBase::ExitInfoListBoxL();
    // make sure app is not left in an invalid state if a leave
    // occurs after exiting the scene setting mode
    iCaptureSetupMenuModeActive = EFalse;
    static_cast<CCamAppUiBase*>( AppUi() )->PushNaviPaneCounterL();

	CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    if ( appUi->ActiveCamera() == ECamActiveCameraPrimary )
        {
        StartViewFinder();
        }
    else
		{
		SwitchToCaptureSetupMenuModeL();
		}
    }


// -----------------------------------------------------------------------------
// CCamPreCaptureViewBase::ExitAllModesL
// Revert to normal mode
// -----------------------------------------------------------------------------
//
void 
CCamPreCaptureViewBase::ExitAllModesL()
  {
  PRINT( _L("Camera => CCamPreCaptureViewBase::ExitAllModesL" ) );
  iController.DeepSleepTimerCancel();
  TCamSettingMode currentMode = ActiveSettingMode();
  
  CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );
  
  switch ( currentMode )
    {
    case ECamSettingModeCaptureMenu:
      {
      PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitAllModesL cap setup menu" ))
      ExitCaptureSetupMenuModeL();
      break;
      }
    case ECamSettingModeCapture:
      {
      PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitAllModesL capture" ))
      
      ExitCaptureSetupModeL();
      if( appUi->ActiveCamera() == ECamActiveCameraSecondary  )
        {
        PRINT( _L("Camera <> CamPreCaptureViewBase::ExitAllModesL ##" )) 
        ExitCaptureSetupMenuModeL();
        }
      break;
      }
    case ECamSettingModeScene:
      {
      PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitAllModesL scene" ))
	  ExitSceneSettingModeL();
      if( appUi->ActiveCamera() == ECamActiveCameraSecondary  )
        {
        PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitAllModesL ##" )) 
        ExitCaptureSetupMenuModeL();
        }
	  if(Id() == TUid::Uid(ECamViewIdVideoPreCapture))
		{
		iToolbarExtensionInvisible = ETrue;
		}
      break;
      }
    case ECamSettingModeInfoListBox:
      {
      PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitAllModesL generic" ))
      ExitInfoListBoxL();
      if( appUi->ActiveCamera() == ECamActiveCameraSecondary  )
        {
        PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitAllModesL ##" )) 
        ExitCaptureSetupMenuModeL();
        }            
      break;
      }            
    default:
      {
      iToolbarExtensionInvisible = ETrue;    	
      }
      break;
    }

  if ( iController.InVideocallOrRinging() )
      {
      return;
      }	
      
  appUi->HandleCommandL(ECamCmdSwitchToPrecapture);

  if ( !( iController.UiConfigManagerPtr() && 
            iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() ) )
    {
    TCamOrientation orientation = appUi->CamOrientation();
    if ( orientation == ECamOrientationCamcorder || orientation == ECamOrientationCamcorderLeft )
      {
      // force update of active palette
      appUi->APHandler()->UpdateActivePaletteL();                    
      }

    if ( appUi && !iSceneSettingModeActive )
      {
      appUi->RaisePreCaptureCourtesyUI(ETrue);
      }
    }
    
  if ( iController.IsTouchScreenSupported() )
      {
      CAknToolbar* fixedToolbar = Toolbar();
      CAknToolbarExtension* extension = fixedToolbar->ToolbarExtension();
      
      //Only active view will set toolbar extension visibility.

      if(this->IsForeground())
          {
          if ( extension && iToolbarExtensionInvisible == EFalse )
              {
              extension->SetShown( ETrue );
              } 
          else if( extension )
              {
              extension->SetShown( EFalse );
              }
          }

      iToolbarExtensionInvisible = EFalse;
      }
    
  PRINT( _L("Camera <= CCamPreCaptureViewBase::ExitAllModesL" ) );
  }


// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ActiveSettingMode
// Returns the active setting mode
// ---------------------------------------------------------------------------
//
CCamPreCaptureViewBase::TCamSettingMode 
CCamPreCaptureViewBase::ActiveSettingMode()
    {
    if ( iCaptureSetupModeActive )
        {
        return ECamSettingModeCapture;
        }
    else if ( iSceneSettingModeActive )
        {
        return ECamSettingModeScene;
        }
    else if ( iCaptureSetupMenuModeActive )
        {
        return ECamSettingModeCaptureMenu;
        }
    else if ( iInfoListBoxActive )
      {
      return ECamSettingModeInfoListBox;
        }
    else
        {
        return ECamSettingModeNone;
        }
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::CCamPreCaptureViewBase
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamPreCaptureViewBase::CCamPreCaptureViewBase( CCamAppController& aController )
    : CCamCaptureSetupViewBase( aController )   
    {
    iCaptureSetupMenuModeActive = EFalse;
    iAssumePostCaptureView = EFalse;
    }                          

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::ConstructL()
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::ConstructL") )    
    /* Use AppUi to get this info.
    // Get the id of Eikon Server window group
    iEikonServerWindowGroupId =
        iCoeEnv->WsSession().FindWindowGroupIdentifier( 0, KEikonServer );

    iBTServerWindowGroupId =  
    		static_cast<CCamAppUi*>( iEikonEnv->AppUi() )->BTServerId();
    */
    UpdateToolbarIconsL();
    PRINT( _L("Camera <= CCamPreCaptureViewBase::ConstructL") )    
    }
                          
// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::CleanupCaptureSetupMenu
// Cleanup the capture menu control if a leave occurs
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::CleanupCaptureSetupMenu( TAny* aAny )
    {
    CCamPreCaptureViewBase* view = static_cast<CCamPreCaptureViewBase*>( aAny );
    if ( view->iCaptureSetupMenuContainer )
        {
        view->AppUi()->RemoveFromStack( view->iCaptureSetupMenuContainer );
        delete view->iCaptureSetupMenuContainer;
        view->iCaptureSetupMenuContainer = NULL;
        }
    // reset the active mode
    view->iCaptureSetupMenuModeActive = EFalse;
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SwitchToCaptureSetupMenuModeL
// Switches the current mode to capture setup menu and activates 
// the capture setup menu control
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SwitchToCaptureSetupMenuModeL()
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::SwitchToCaptureSetupMenuModeL"))
    __ASSERT_DEBUG( !iCaptureSetupMenuContainer, CamPanic( ECamPanicResourceLeak ) );

    // Create the relevant capture setup menu, e.g. video or photo...
    CreateCaptureSetupMenuL();

    // Cleanup the view correctly if a leave occurs
    CleanupStack::PushL( TCleanupItem( CleanupCaptureSetupMenu, this ) );
    
    PRINT( _L("Camera => CCamPreCaptureViewBase::SwitchToCaptureSetupMenuModeL start vf"))
    StartViewFinder();
    iCaptureSetupMenuContainer->SetMopParent( this );   
    // Activate the menu control and mode.
    AppUi()->AddToStackL( *this, iCaptureSetupMenuContainer );
    iCaptureSetupMenuContainer->ActivateL();
    // UpdateCbaL need this boolean to be set in
    // order for the correct CBA to be returned
    iCaptureSetupMenuModeActive = ETrue;
    UpdateCbaL();
    CleanupStack::Pop(); // CleanupCaptureSetupMenu

    // Fade the container that is behind.
    iCaptureSetupMenuContainer->FadeBehind( ETrue );
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ExitCaptureSetupMenuModeL
// Exits capture setup menu mode and activates the original precapture
// view container that was active before entering this mode.
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::ExitCaptureSetupMenuModeL()
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::ExitCaptureSetupMenuModeL() ") );
    
    if ( !iCaptureSetupMenuModeActive )
        {
        return;
        }
    iCaptureSetupMenuModeActive = EFalse;
    iContainer->MakeVisible( ETrue );  

    iCaptureSetupMenuContainer->FadeBehind( EFalse );
    AppUi()->RemoveFromStack( iCaptureSetupMenuContainer );
    delete iCaptureSetupMenuContainer;
    iCaptureSetupMenuContainer = NULL;
    UpdateCbaL();
    SetTitlePaneTextL();
    
    PRINT( _L("Camera <= CCamPreCaptureViewBase::ExitCaptureSetupMenuModeL() ") );
    }                         

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SwitchToStandbyModeL
// Exits pre capture mode and activates the standby container
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SwitchToStandbyModeL( TCamAppViewIds aViewId, TInt aError )
    {
    iAssumePostCaptureView = EFalse;
    iDeactivateToStandby = ETrue; // avoid viewfinder startup
    iContinueInBackground = EFalse; 
    
    switch ( ActiveSettingMode() )
        {
        case ECamSettingModeCaptureMenu:   
            {
            ExitCaptureSetupMenuModeL();            
            }
            break;
        case ECamSettingModeScene:
            {
            ExitSceneSettingModeL();
            ExitCaptureSetupMenuModeL();
            }
            break;
        case ECamSettingModeCapture:
            {
            ExitCaptureSetupModeL();
            ExitCaptureSetupMenuModeL();
            }
            break;
			case ECamSettingModeInfoListBox:
            {
            ExitInfoListBoxL();
            ExitCaptureSetupMenuModeL();
            }				            
            break;
        default:  
            break;
        }    


    // Hide the active palette
    static_cast<CCamAppUi*>( iEikonEnv->AppUi() )
        ->SetActivePaletteVisibility( EFalse );
    
    // Hide the toolbar in standby mode
    if ( iController.IsTouchScreenSupported() )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
        CAknToolbar* fixedToolbar = appUi->CurrentFixedToolbar();
        if ( fixedToolbar )
            {
            fixedToolbar->SetToolbarVisibility( EFalse );
            }
        }
  
    // stop viewfinding
    StopViewFinder();

    CCamViewBase::SwitchToStandbyModeL( aViewId, aError );
    
    iDeactivateToStandby = EFalse;
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ExitStandbyModeL
// Exits standby mode and activates the previous pre-capture container
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::ExitStandbyModeL()
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::ExitStandbyModeL" ) )
    // start view finding
    iAssumePostCaptureView = EFalse;
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    if( appUi->AppInBackground(ETrue ))
        {
        PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitStandbyModeL: global note in foreground." ) )
        return;
        }
    if( iController.UiConfigManagerPtr() && 
        iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() &&
        iController.CurrentMode() != ECamControllerIdle )
        {
        PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitStandbyModeL: not in standby, returning without exit" ) )    
        return;    
        } 
    
    if( !appUi->DrawPreCaptureCourtesyUI() && 
        !iController.IsTouchScreenSupported() &&
        !appUi->IsSecondCameraEnabled() )
        {
        SetCourtesySoftKeysL();
        }
    
    // In case coming back after dismissing videocall, need
    // to register as using engine    
    if( !appUi->IsInPretendExit() )
        {
        IncrementCameraUsers();
        }
        
    PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitStandbyModeL: start vf" ) )
    StartViewFinder();

    //static_cast<CCamAppUi*>( iEikonEnv->AppUi() )
    //    ->APHandler()->UpdateActivePaletteL();
    TBool uiOverride = iController.UiConfigManagerPtr() && iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported();
    if ( !uiOverride )
        {
        SetupActivePaletteL();
        }

    // Dimming is set if video call is active, remove dimming if video call is not active any more
    TInt callType( EPSCTsyCallTypeUninitialized );
    RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType, callType );
    if ( callType != EPSCTsyCallTypeH324Multimedia
         && iController.IsTouchScreenSupported() )
        {
        CAknToolbar* toolbar = Toolbar();
        if (toolbar)
            {
            toolbar->SetDimmed( EFalse );
            }
        }    
    CCamViewBase::ExitStandbyModeL();
    
    // Need to update appui viewstate
    PRINT( _L("Camera <> CCamPreCaptureViewBase::ExitStandbyModeL: emit ECamCmdSwitchToPrecapture" ) )
    appUi->HandleCommandL( ECamCmdSwitchToPrecapture );
    // Check if currently selected storagemedia has available memory for next capturing
    appUi->CheckMemoryAvailableForCapturing();      
        
    if ( uiOverride )
        {
        appUi->SetActivePaletteVisibility( EFalse );    
        }
    else
        {    
        appUi->RaisePreCaptureCourtesyUI(ETrue);
        }
    PRINT( _L("Camera <= CCamPreCaptureViewBase::ExitStandbyModeL" ) )
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::IsOkOptionsMenuAllowed
// Checks whether or not the ok aoptions menu should be displayed
// ---------------------------------------------------------------------------
//
#if 0
TBool CCamPreCaptureViewBase::IsOkOptionsMenuAllowed()
    {
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    // Check if we are in selftimer mode, scene setting mode,
    // capture setup menu mode or if a capture mode preparation is
    // taking place.  If so, do NOT show the menu
    if ( ( appUi && !appUi->SelfTimerEnabled() ) &&
        !iController.CaptureModeTransitionInProgress() &&
            ActiveSettingMode() == ECamSettingModeNone && !appUi->IsEmbedded() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
#endif // 0

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::RemoveCaptureSetupMenuContainers
// Removes the capture setup menu and original container from
// the container stack
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::RemoveCaptureSetupMenuContainers()
    {
    // Store the current item index value for the menu, so that it can
    // be reused when the CSM container is reactivated from leaving
    // a capture setup control.
    if ( iCaptureSetupMenuContainer )
        {
        iCaptureSetupMenuLastItemIndex = 
            iCaptureSetupMenuContainer->CurrentItemIndex();
        iCaptureSetupMenuContainer->FadeBehind( EFalse );
        
        // Remove the Capture Setup Menu from the container stack.
        AppUi()->RemoveFromStack( iCaptureSetupMenuContainer );
        }
    delete iCaptureSetupMenuContainer;
    iCaptureSetupMenuContainer = NULL;
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::DynInitSwitchCameraMenuItemL
// Set up the text for switch camera in the options menu
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::DynInitSwitchCameraMenuItemL( CEikMenuPane* aMenuPane )
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    // Check if only 1 camera available
    if ( !iController.UiConfigManagerPtr()->IsSecondaryCameraSupported() )
        {
        aMenuPane->SetItemDimmed( ECamCmdSwitchCamera, ETrue );
        }
    else 
        {
        // if secondary camera is enabled
        if ( appUi->IsSecondCameraEnabled() )
            {
            if( iController.UiConfigManagerPtr()->IsLensCoverSupported() )
                {
                TInt slideState = iController.CameraSlideState();
                if ( slideState == CameraPlatPSKeys::EClosed )
                    {
                    aMenuPane->SetItemDimmed( ECamCmdSwitchCamera, ETrue );
                    }
                else
                    {                               
                    aMenuPane->SetItemTextL( ECamCmdSwitchCamera, 
                            R_CAM_PRE_CAPTURE_MENU_PRIMARY_CAMERA );
                    }   
                }
            else
                {
                aMenuPane->SetItemTextL( ECamCmdSwitchCamera, 
                        R_CAM_PRE_CAPTURE_MENU_PRIMARY_CAMERA );
                }
            }
        }
    }


// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ReleaseResources
// Stop the viewfinder and inform the controller that this view has finished
// with the engine
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::ReleaseResources()
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::ReleaseResources") );
    iContinueInBackground = EFalse;
    StopViewFinder();
  
    // Make sure that video recording is stopped (if ongoing).
    iController.StopVideoRecording();
    
    // if the application is not closing then self-timer mode must be disabled.
    // otherwise it will be deleted as part of the application shut down process
    if ( !iController.IsInShutdownMode() )
        {
        // Do not do softkey update if we are already exiting
        // (AppUi is not available)
        if( iController.IsAppUiAvailable() )
            {
            // Ensure AppUi has had self-timer mode disabled
            CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
            if ( appUi )
                {
                TRAP_IGNORE( appUi->SelfTimerEnableL( ECamSelfTimerDisabled ) );
                }
            }
        }
    // Register that we no longer need the engine
    // Checks if we incremented the count.
    DecrementCameraUsers();
    PRINT( _L("Camera <= CCamPreCaptureViewBase::ReleaseResources") );
    }
  
// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::IsPreCapture
// Returns whether the view is a precapture view
// ---------------------------------------------------------------------------
//
TBool CCamPreCaptureViewBase::IsPreCapture()
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::OfferToolbarEventL
// Handle fixed toolbar and toolbar extension events
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::OfferToolbarEventL( TInt aCommand )
    {
    iController.StartIdleTimer();
    HandleCommandAoL( aCommand );
    
    if ( aCommand == ECamCmdSceneSettingList ||
         aCommand == ECamCmdCaptureSetupFlashStill ||
         aCommand == ECamCmdCaptureSetupSelfTimer ||
         aCommand == ECamCmdCaptureSetupSceneStill ||
         aCommand == ECamCmdCaptureSetupSceneVideo ||
         aCommand == ECamCmdCaptureSetupColourFilterStill ||
         aCommand == ECamCmdCaptureSetupColourFilterVideo ||
         aCommand == ECamCmdCaptureSetupWhiteBalanceStill ||
         aCommand == ECamCmdCaptureSetupWhiteBalanceVideo ||
         aCommand == ECamCmdCaptureSetupExposureStill ||
         aCommand == ECamCmdCaptureSetupLightSensitivityStill ||
         aCommand == ECamCmdCaptureSetupContrastStill ||
         aCommand == ECamCmdCaptureSetupImageSharpnessStill )
        {
        // These commands imply a container switch.
        // iItemAccessedViaAP must be set so that softkeys work correctly.
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
        appUi->APHandler()->iItemAccessedViaAP = ETrue;
        iController.DeepSleepTimerStart();
        }
    }


// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::SetIconL
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SetIconL(
        CAknButton* aButton,
        const TDesC& aIconFilename,
        TInt aIconId,
        TInt aMaskId,
        MAknsSkinInstance* aSkinInstance, 
        const TAknsItemID& aSkinItemId )
    {
    CGulIcon* icon = AknsUtils::CreateGulIconL(
        aSkinInstance,
        aSkinItemId,
        aIconFilename,
        aIconId,
        aMaskId );
    icon->SetBitmapsOwnedExternally( EFalse ); // CGulIcon owns bitmaps
    CAknButtonState* state = aButton->State( 0 );
    if ( state )
        {
        state->SetIcon( icon );
        icon = NULL;
        }
    else
        {
        delete icon;
        icon = NULL;
        }
	
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::HandleAppEvent <<virtual>>
//
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureViewBase::HandleAppEvent( const TCamAppEvent& aEvent )
    {
    PRINT1( _L("Camera => CCamPreCaptureViewBase::HandleAppEvent, event:%d"), aEvent );
    // Reset last pinch movement direction
    iLastMovement = 0;
    CCamViewBase::HandleAppEvent( aEvent );

    if ( aEvent == ECamAppEventFocusGained && iController.InVideocallOrRinging() )
        {
        TRAP_IGNORE( BlankSoftkeysL() );
        if ( iController.IsTouchScreenSupported() )
            {
            CAknToolbar* toolbar = Toolbar();
            if ( toolbar )
                {
                toolbar->SetToolbarVisibility( EFalse );
                }
            }
        }
    
    if ( aEvent == ECamAppEventFocusGained )
        {
        if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() && 
             iController.CameraSwitchQueued() ) // fake exit done
            {
            iController.CheckSlideStatus(); // Do we still need to change cam?
            CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
            // How about portrait/landscape 2nd camera? 
            if ( !iController.CameraSwitchQueued() && 
                 appUi->IsSecondCameraEnabled() ) 
                {
                TBool landscape2ndCam = appUi->IsQwerty2ndCamera( ETrue );
                if( appUi->CamOrientation() != ECamOrientationPortrait )
                    {
                    if ( !landscape2ndCam )
                        {
                        PRINT( _L("Camera => CCamPreCaptureViewBase::HandleAppEvent, change to portrait 2nd cam."));
                        iController.SetCameraSwitchRequired( 
                                                 ESwitchSecondaryOrientation );  
                        }
                    }
                else
                    {
                    if ( landscape2ndCam )
                        {
                        PRINT( _L("Camera => CCamPreCaptureViewBase::HandleAppEvent, change to landscape 2nd cam."));
                        iController.SetCameraSwitchRequired( 
                                                 ESwitchSecondaryOrientation );
                        }
                    }
                }
            }
        if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() && 
             iController.ActiveCamera() == ECamActiveCameraPrimary )
            {
            if ( iController.CameraSwitchQueued() )
                {
                CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
                // If camera switch fails, main camera is used instead
                TRAP_IGNORE( appUi->HandleCommandL( ECamCmdSwitchCamera ) );
                }
            }
        }
     
    PRINT( _L("Camera <= CCamPreCaptureViewBase::HandleAppEvent") );
    }

// ---------------------------------------------------------------------------
// SetupActivePaletteL
// 
// Helper method to set up the Active Palette.
// If needed, a new instance of AP is created.
// Otherwise the existing AP items are cleared.
// New AP items are setup with resource from GetAPResourceId.
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::SetupActivePaletteL()
    {
    if( iContainer && !iController.IsTouchScreenSupported() )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
        TCamOrientation orientation = appUi->CamOrientation();

        if( ECamOrientationCamcorder == orientation
                || ECamOrientationCamcorderLeft == orientation 
                || ECamOrientationPortrait == orientation )
            {
            // Creates Active Palette, sets items and sets view.
            // Only AP2 supported.
            static_cast<CCamPreCaptureContainerBase*>( iContainer )
            ->SetupActivePaletteL( this );
            }
        else
            {
            // No AP  
            }
        }
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::ToolbarButtonById
//
// ---------------------------------------------------------------------------
//
 CAknButton* CCamPreCaptureViewBase::ToolbarButtonById(const TCamCmd aCommandId) const
     {
     CAknButton* button = NULL;
     if(iController.IsTouchScreenSupported())
         {
         CAknToolbar* toolbar = Toolbar();
         if(toolbar)
             {
             button = static_cast<CAknButton*>
                         (toolbar->ControlOrNull(aCommandId));
			 if(NULL == button)
                 {
                 CAknToolbarExtension* extension = toolbar->ToolbarExtension();
                 if(extension)
                     {
                     button = static_cast<CAknButton*>
                                 (extension->ControlOrNull(aCommandId));
					 }
                 }
             }
         }
     return button;
     }
     
// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::HideButton
//
// ---------------------------------------------------------------------------
//
 void CCamPreCaptureViewBase::HideButton(const TCamCmd aCommandId)
     {
     if(iController.IsTouchScreenSupported())
          {
          CAknToolbar* toolbar = Toolbar();
          if(toolbar)
              {
              if(toolbar->ControlOrNull(aCommandId))            
                  {
                  toolbar->HideItem(aCommandId,ETrue,ETrue);
                  }
              else if( toolbar->ToolbarExtension() && 
				toolbar->ToolbarExtension()->ControlOrNull(aCommandId))
                  {
                  TRAP_IGNORE(toolbar->ToolbarExtension()->HideItemL(aCommandId,ETrue));
                  }
			  else
				 {
				 //Do Nothing
				 }
              }
          }
     }
	 
// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::RedrawToolBar
//
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::RedrawToolBar()
	{
	CEikonEnv::Static()->WsSession().ClearAllRedrawStores();
	}

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::CreateContainerL
//
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::CreateContainerL()
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::CreateContainerL") );

    // Create gesture fw object, set observer and gesture interest
    if ( iGestureFw ) 
        {
        delete iGestureFw;
        }

    iGestureFw = CAknTouchGestureFw::NewL( *this, *iContainer );
    iGestureFw->SetGestureInterestL( EAknTouchGestureFwGroupPinch | EAknTouchGestureFwGroupTap );

    PRINT( _L("Camera <= CCamPreCaptureViewBase::CreateContainerL") );            
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureViewBase::HandleTouchGestureL
//
// ---------------------------------------------------------------------------
//
void CCamPreCaptureViewBase::HandleTouchGestureL( MAknTouchGestureFwEvent& aEvent )
    {
    PRINT( _L("Camera => CCamPreCaptureViewBase::HandleTouchGestureL") );
    
    // Skipped modes here
    if ( iController.ActiveCamera() == ECamActiveCameraSecondary )
        {
        PRINT( _L("Camera <= CCamPreCaptureViewBase::HandleTouchGestureL - skipped") );
        return;
        }

    MAknTouchGestureFwPinchEvent *pinch = AknTouchGestureFwEventPinch( aEvent );
    if ( pinch && (ECamNoOperation == iController.CurrentOperation()) )
        {
        // Determine the direction of pinch: +ve -> pinch outward / zoom / widen VF
        TInt currMove = pinch->Movement();
        TBool wide = (currMove > 0) ? ETrue : EFalse;  
        PRINT1( _L("Camera <> CCamPreCaptureViewBase::HandleTouchGestureL - pinch, movement:%d"), currMove );

        if ( ( iLastMovement >= 0 && currMove < 0 ) || 
             ( iLastMovement <= 0 && currMove > 0 ) )
            {
            // Enable the blinking for resolution indicators and toggle image/video quality
            // level between top widescreen and vga levels
            CCamPreCaptureContainerBase* container = static_cast<CCamPreCaptureContainerBase*>( iContainer );
            container->BlinkResolutionIndicatorOnChange( ETrue );

            // Hide the zoom pane in case of pinch
            CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
            appUi->ZoomPane()->MakeVisible( EFalse, ETrue );

            if ( iController.ToggleWideScreenQuality( wide ) )
                {
                iLastMovement = currMove;
                }
            else
                {
                // if the quality level wasn't changed, then disable the blinking
                container->BlinkResolutionIndicatorOnChange( EFalse );
                }
            }
        }
    else if ( EAknTouchGestureFwDoubleTap == aEvent.Type() )
        {
        PRINT( _L("Camera <> *** double tap event") );
        CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
        CCamZoomPane *zoomPane = appUi->ZoomPane();
        
        CCamPreCaptureContainerBase* container = static_cast<CCamPreCaptureContainerBase*>( iContainer );
        container->ShowZoomPaneWithTimer();

        // Zoom to max (if not already at max) zoom level, otherwise zoom out to min level
        if ( !zoomPane->IsZoomAtMaximum() )
            {
            PRINT( _L("Camera <> Zooming to max level") );
            zoomPane->ZoomToMaximum();
            }
        else
            {
            PRINT( _L("Camera <> Zooming out to min level") );
            zoomPane->ZoomToMinimum();
            }
        }
    else
        {
        PRINT1( _L("Camera <> HandleTouchGestureL - gesture not used, type:%d"), aEvent.Type() );
        }

    PRINT( _L("Camera <= CCamPreCaptureViewBase::HandleTouchGestureL") );
    }

//  End of File  
