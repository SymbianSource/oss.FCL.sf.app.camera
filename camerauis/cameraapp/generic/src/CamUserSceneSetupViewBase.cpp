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
* Description:  Controls the switching between different control modes*
*/


// INCLUDE FILES
#include "CamUserSceneSetupViewBase.h"
#include "CamAppUiBase.h"
#include "CamAppUi.h"
#include "CamPanic.h"
#include "CamUserSceneSetupContainer.h"
#include "Cam.hrh"

#include "CamPreCaptureContainerBase.h"

#include <eiksoftkeypostingtransparency.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamUtility.h"

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::~CCamUserSceneSetupViewBase
// Destructor
// ---------------------------------------------------------------------------
//
CCamUserSceneSetupViewBase::~CCamUserSceneSetupViewBase()
  {
  PRINT( _L("Camera => ~CCamUserSceneSetupViewBase" ))  
  delete iContainer;   
  iContainer = NULL;
  PRINT( _L("Camera <= ~CCamUserSceneSetupViewBase" ))  
  }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::HandleCommandL
// From CAknView Handle commands
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::HandleCommandL( TInt aCommand )
    {
    PRINT1( _L("Camera => CCamUserSceneSetupViewBase::HandleCommandL %d"), aCommand );   		
    switch ( aCommand )
        {
        case ECamMSKCmdAppChange:
            {
           
            if ( IsOnlyUserSceneSetupModeActive() )
                {
                CCamUserSceneSetupContainer* userSceneSetup = 
                    static_cast<CCamUserSceneSetupContainer*>( iContainer );
                userSceneSetup->HandleSelectionL();
                }
            else
                {
                CCamCaptureSetupViewBase::HandleCommandL( ECamCmdSelect );

                // Update the User Scene Setup with all the new values.
                CCamUserSceneSetupContainer* userSceneSetup = 
                    static_cast<CCamUserSceneSetupContainer*>( iContainer );
                userSceneSetup->UpdateListItems();   
                }
            break;
            }
        // Handle the user scene scene list navi-key select selection
        case EAknSoftkeySelect:
            {
            CCamCaptureSetupViewBase::HandleCommandL( aCommand );
            // Update the User Scene Setup with all the new values.
            CCamUserSceneSetupContainer* userSceneSetup = 
                static_cast<CCamUserSceneSetupContainer*>( iContainer );
            userSceneSetup->UpdateListItems();
            break;
            }
        case ECamCmdSetUserDefault:  
            {
            TInt currentVal = iController.IntegerSettingValue( ECamSettingItemUserSceneDefault );
            if ( currentVal )
              {
              currentVal = ECamSettNo; 
              }
            else
              {
              currentVal = ECamSettYes;
              }
            TRAP_IGNORE( iController.SetIntegerSettingValueL( ECamSettingItemUserSceneDefault, currentVal) );
            CCamUserSceneSetupContainer* userSceneSetup = static_cast<CCamUserSceneSetupContainer*>( iContainer );
            userSceneSetup->UpdateListItems();  
            userSceneSetup->UpdateDisplayL();
            }
            break;
        case ECamCmdUserSceneReset:
            {
            DisplayResetUserSceneDlgL();
            }
            break;
        case ECamCmdCaptureSetupWhiteBalanceUser: // fallthrough
        case ECamCmdCaptureSetupColourFilterUser:
        case ECamCmdCaptureSetupExposureUser:
        case ECamCmdCaptureSetupFlashUser:
        case ECamCmdCaptureSetupBrightnessUser:
        case ECamCmdCaptureSetupContrastUser:
        case ECamCmdCaptureSetupImageSharpnessUser:
        // case ECamCmdCaptureSetupLightSensitivityUser:
            {
            SwitchToCaptureSetupModeL( aCommand );
            }
            break;           
        case ECamCmdCaptureSetupSceneUser:
            {
            StatusPane()->MakeVisible( EFalse );
            SwitchToSceneSettingModeL();
            }
            break;
     	case ECamCmdCaptureSetupLightSensitivityUser:
      		{
      		SwitchToInfoListBoxL( EInfoListBoxModeISO, EFalse );//No skin
      		}    
			break;
        // If capture setup menu is active and user presses softkey cancel,
        // deactivate the menu.
        case EAknSoftkeyBack:
            {
            
            if ( IsOnlyUserSceneSetupModeActive() )
                {
                ExitUserSceneSetupModeL();
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

        case ECamCmdExitStandby:
        	{
        	CCamViewBase::ExitStandbyModeL();
        	break;
        	}
        case ECamCmdInternalExit:
            {
            PRINT( _L("Camera CCamUserSceneSetupViewBase::HandleCommandL ECamCmdInternalExit") );

            CCamCaptureSetupViewBase* preCaptureView =
            static_cast<CCamCaptureSetupViewBase*>( AppUi()->View( TUid::Uid( ECamViewIdStillPreCapture ) ) );
            // reset scene setting flag so that precapture view returns to normal mode in next startup
            // Note: this is NOT the proper way to reset precapture view to normal state but seems
            // to be the only way that works here. None of the Exit.. or SwitchTo... functions work
            // here because precapture view is not the active view.
            if ( preCaptureView )
                {
                preCaptureView->SetSceneSettingMode( EFalse );
                if ( iController.IsTouchScreenSupported() )
                    {
                    // re-create the toolbar because it's gone for some reason
                    preCaptureView->CreateAndSetToolbarL( R_CAM_STILL_PRECAPTURE_TOOLBAR );
                    }
                }
            CCamCaptureSetupViewBase::HandleCommandL( ECamCmdInternalExit );
            
  
            PRINT( _L("Camera CCamUserSceneSetupViewBase::HandleCommandL ECamCmdInternalExit OK") );
            break;
            }
        case EAknSoftkeyCancel:
            {
            PRINT1( _L( "Camera <> CCamUserSceneSetupViewBase::HandleCommandL EAknSoftkeyCancel iCancelRequest=%d" ),iCancelRequest);
            if( ! iCancelRequest ) // EFalse
                {
                if( !iController.IsViewFinding( ) && iVFRequested )
                    {
                    PRINT( _L( "Camera <> CCamUserSceneSetupViewBase::HandleCommandL viewfinder inactive" ));

                    //viewfinder is not active then set to ETrue ...
                    //and wait until controller is ready.
                    iCancelRequest=ETrue;
                    }
                else
                    {
                    CCamCaptureSetupViewBase::HandleCommandL( aCommand );
                    }                
                }
            break;
            }
            
        case EAknSoftkeyOk:            
            {
            PRINT1( _L( "Camera <> CCamUserSceneSetupViewBase::HandleCommandL EAknSoftkeyOk iOkRequest=%d" ),iOkRequest);
            if( ! iOkRequest ) // EFalse
                {
                if( !iController.IsViewFinding( ) && iVFRequested )
                    {
                    PRINT( _L( "Camera <> CCamUserSceneSetupViewBase::HandleCommandL viewfinder inactive" ));

                    //viewfinder is not active then set to ETrue ...
                    //and wait until controller is ready.
                    iOkRequest=ETrue;
                    }
                else
                    {
                    CCamCaptureSetupViewBase::HandleCommandL( aCommand );
                    }                
                }
            break;
            }
        default:
            CCamCaptureSetupViewBase::HandleCommandL( aCommand );
            
        }
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::HandleCommandL ") );   		    
    }
                 
    
// -----------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::HandleForegroundEventL
// Handle foreground event
// -----------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::HandleForegroundEventL( TBool aForeground )
    {
    PRINT1( _L("Camera => CCamUserSceneSetupViewBase::HandleForegroundEventL %d"), aForeground );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    if ( aForeground )
        {
        // if foreground event is received while in videocall, go to standby with error
        if ( iController.InVideocallOrRinging() && ECamNoOperation == iController.CurrentOperation() )
            {
            ExitAllModesL();
            appUi->SetStandbyStatus( KErrInUse );
            appUi->HandleCommandL( ECamCmdGoToStandby );
            return;
            }
        else if ( !appUi->IsInPretendExit()  && !iNotifierPopupShowing )
            {
            // Register that we want to use the engine
            IncrementCameraUsers();
            iController.StartIdleTimer();
            
            // copied from still precapture handleforegroundevent implementations
            iController.IsProfileSilent();
            UpdateCbaL();
            
                        
            // start viewfinder unless the is activating to standby or scene settings
            // SwitchToInfoListBoxL( EInfoListBoxModeISO, EFalse ) where FullySkinned is EFalse, it
            // indicate VF need to start in case of iInfoListBoxActive
            if ( !iStandbyModeActive && !iSceneSettingModeActive && 
                 !iUserSceneSetupModeActive )
                {
                iVFRequested=ETrue;
                StartViewFinder();
                }
                        
            }
        else
            {
            // The view can get a foreground event while the application is
            // actually in a pretend exit situation. This occurs when the view switch
            // was called before the exit event, but didn't complete until after the
            // exit event. In this case the view should not register an interest in
            // the engine as the application is really in the background and the resources
            // need to be released
            }
        }        
    // To background
    else if( !aForeground )
        {
        iNotifierPopupShowing = appUi->AppInBackground( ETrue );
        PRINT( _L("Camera <> CCamUserSceneSetupViewBase::HandleForegroundEventL dec engine count") );
        // Register that we nolonger need the engine
        if( !iNotifierPopupShowing )
            DecrementCameraUsers();
        }
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::HandleForegroundEventL ") );
    }


// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::DoActivateL
// From CAknView activate the view
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::DoActivateL( const TVwsViewId& aPrevViewId, 
            TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::DoActivateL") );   	
    iCancelRequest=EFalse;
    iOkRequest=EFalse;
    iVFRequested=EFalse;

    StatusPane()->MakeVisible( ETrue );
        	
    CCamViewBase::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    SwitchToUserSceneSetupModeL();

    // Need to monitor the controller for standby events
    iController.AddControllerObserverL( this ); 
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::DoActivateL") );   	
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::DoDeactivate
// From CAknView deactivates the view
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::DoDeactivate()
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::DoDeactivateL") );   		
    CCamViewBase::DoDeactivate();
    
    iController.RemoveControllerObserver( this ); 
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::DoDeactivateL") );   		
    }

// -----------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::HandleControllerEventL
// Handle controller events
// -----------------------------------------------------------------------------
//
void 
CCamUserSceneSetupViewBase
::HandleControllerEventL( TCamControllerEvent aEvent, 
                          TInt                /*aError*/ )
  {
  PRINT( _L( "Camera => CCamUserSceneSetupViewBase::HandleControllerEventL" ) );    
  
  switch( aEvent )
    {
/*
    // ---------------------------------
    case ECamEventOperationStateChanged:
      {
      switch ( iController.CurrentOperation() )
        {
        // Standby event
        case ECamStandby:
          {                               
          ExitAllModesL();                              
          // Get the UI to move to standby in the appropriate
          // pre capture view
          AppUi()->HandleCommandL( ECamCmdGoToStandby );
          break;
          }
        default:
          {
          break;
          }
        }
      break;
      }
*/
    // ---------------------------------
    case ECamEventControllerReady:
      {
      PRINT1( _L( "Camera <> CCamUserSceneSetupViewBase::HandleControllerEventL ECamEventControllerReady iCancelRequest=%d" ),iCancelRequest);
      PRINT1( _L( "Camera <> CCamUserSceneSetupViewBase::HandleControllerEventL ECamEventControllerReady iOkRequest=%d" ),iOkRequest);      
      iVFRequested=EFalse;

      if( iCancelRequest )
          {          
          //CAncel has been requested and Cameraengine is now ready to
          //switch view and continue cancel.
          iCancelRequest=EFalse;
          TRAP_IGNORE( HandleCommandL( EAknSoftkeyCancel ) );
          }
      else if( iOkRequest )
          {
          //Ok has been requested and Cameraengine is now ready to
          //switch view and continue with Ok.
          iOkRequest=EFalse;
          TRAP_IGNORE( HandleCommandL( EAknSoftkeyOk ) );
          }
      
      break;
      }
    default:
      break;
    // ---------------------------------
    }
  PRINT( _L( "Camera <= CCamUserSceneSetupViewBase::HandleControllerEventL") );    
  }


// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::CCamUserSceneSetupViewBase
// C++ Constructor
// ---------------------------------------------------------------------------
//
CCamUserSceneSetupViewBase::CCamUserSceneSetupViewBase
( CCamAppController& aController )
: CCamCaptureSetupViewBase( aController )
    {
    }
    
// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::ExitUserSceneSetupModeL
// Exits the user scene setup mode
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::ExitUserSceneSetupModeL()
    {    
    iUserSceneSetupModeActive = EFalse;
    iContainer->MakeVisible( ETrue );  

    AppUi()->RemoveFromStack( iContainer );
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::UpdateCbaL
// Updates the command button array for this view
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::UpdateCbaL()
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::UpdateCbaL()") );    	
    // if videocall is active, set the softkeys already here	
    if ( iController.InVideocallOrRinging() )
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );	
        }		
    // if the view is in capture setup mode
    else if ( iCaptureSetupModeActive )
        {
        if( iForceAvkonCBA )
            {
            SetSoftKeysL( R_AVKON_SOFTKEYS_OK_CANCEL__OK ); //Avkon softkeys. Not transparent
            }
        else
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_SETTINGS_SELECT_BACK__CHANGE_TRANSPARENT );//color etc.
            }
        }
    else if ( iSceneSettingModeActive )
    	{
    	SetSoftKeysL( R_CAM_SOFTKEYS_SETTINGS_SELECT_BACK__CHANGE  );
        if( iForceAvkonCBA )
            {
            EikSoftkeyPostingTransparency::MakeTransparent(
                    *ViewCba(), EFalse );            
            }
    	}
    else if ( iInfoListBoxActive )
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_SETTINGS_SELECT_BACK__CHANGE_TRANSPARENT );
        EikSoftkeyPostingTransparency::MakeTransparent(   
                            *ViewCba(), !iForceAvkonCBA );
        }
    // if the view is user scene setup
    else
        {
		// R_AVKON_SOFTKEYS_OPTIONS_BACK --> 
		//						R_CAM_SOFTKEYS_OPTIONS_BACK__CHANGE
        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK__CHANGE );
        }
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::UpdateCbaL()") );    	    
    }



// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::SetTitlePaneTextL
// Set the view's title text
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::SetTitlePaneTextL()
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::SetTitlePaneTextL()") );    		
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );   
    // It is known that the container for this view is of the special
    // type CCamUserSceneSetupContainer, and so we can use it's methods.
    CCamUserSceneSetupContainer* userSceneSetup = 
    static_cast<CCamUserSceneSetupContainer*>( iContainer );
    appUi->SetTitleL( userSceneSetup->TitlePaneTextResourceId() );
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::SetTitlePaneTextL()") );    		
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::SwitchToUserSceneSetupModeL
// Switches to the user scene setup mode.
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::SwitchToUserSceneSetupModeL()
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::SwitchToUserSceneSetupModeL()") );  	   
    iContainer->SetMopParent( this );   
    
    // Activate the menu control and mode.
    AppUi()->AddToStackL( iContainer );
    iContainer->ActivateL();
    iContainer->MakeVisible( ETrue );
    iUserSceneSetupModeActive = ETrue;
    
    StatusPane()->MakeVisible( ETrue );
    
    // Update the command button array.
    iForceAvkonCBA=EFalse;
    UpdateCbaL();
    SetTitlePaneTextL();
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::SwitchToUserSceneSetupModeL()") );  	   
    }
   

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::SwitchToCaptureSetupModeL
// Switches the current mode to capture setup and activates a 
// specific control..
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::SwitchToCaptureSetupModeL( TInt aSetupCommand )
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::SwitchToCaptureSetupModeL()") );  	   	
    iUserSceneSetupModeActive = EFalse;
    SetCaptureSetupModeActive(ETrue);
    iContainer->MakeVisible( ETrue );  
    AppUi()->RemoveFromStack( iContainer );
    iVFRequested=ETrue;    
    StartViewFinder();
    iController.StartIdleTimer();

    
    // Remove the view's main container, and add the capture setup 
    // control associated with the input command to the container stack.
    CCamCaptureSetupViewBase::SwitchToCaptureSetupModeL( aSetupCommand, ETrue );
   StatusPane()->MakeVisible( EFalse ); 	
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::SwitchToCaptureSetupModeL()") );  	   	
    }


// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::ExitCaptureSetupModeL
// Exit capture setup mode.
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::ExitCaptureSetupModeL()
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::ExitCaptureSetupModeL()") );  	   		
    StopViewFinder();
	
    StatusPane()->MakeVisible( ETrue );
    CCamCaptureSetupViewBase::ExitCaptureSetupModeL();
    
    SwitchToUserSceneSetupModeL(); 
    
    iContainer->DrawDeferred();
    
    SetCaptureSetupModeActive(EFalse);
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::ExitCaptureSetupModeL()") );  	   		
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::ExitSceneSettingModeL
// Exit scene setting mode.
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::ExitSceneSettingModeL()
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::ExitSceneSettingModeL()") );  	   			
    CCamCaptureSetupViewBase::ExitSceneSettingModeL();
    SwitchToUserSceneSetupModeL();
    
    iContainer->DrawDeferred();    
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::ExitSceneSettingModeL()") );  	   			
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::ExitInfoListBoxL
// Exit scene setting mode.
// ---------------------------------------------------------------------------    
//
void CCamUserSceneSetupViewBase::ExitInfoListBoxL()
	{
	PRINT( _L("Camera => CCamUserSceneSetupViewBase::ExitInfoListBoxL()") );  	   				
	StopViewFinder(); 
		   				
	CCamCaptureSetupViewBase::ExitInfoListBoxL();
	SwitchToUserSceneSetupModeL();
	
	iContainer->DrawDeferred();
	
	StatusPane()->MakeVisible( ETrue );
	PRINT( _L("Camera <= CCamUserSceneSetupViewBase::ExitInfoListBoxL()") );  	   				
	}

// -----------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::ExitAllModesL
// Revert to normal mode
// -----------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::ExitAllModesL()
    {
    PRINT( _L("Camera => CCamUserSceneSetupViewBase::ExitAllModesL()") );  	   					
    if ( iSceneSettingModeActive )
        {
        ExitSceneSettingModeL();
        }
    if ( iCaptureSetupModeActive )
        {
        ExitCaptureSetupModeL();
        }
    if( iInfoListBoxActive )
    	{
    	ExitInfoListBoxL();
    	}
    PRINT( _L("Camera <= CCamUserSceneSetupViewBase::ExitAllModesL()") );  	   						
    }


// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::IsOnlyUserSceneSetupModeActive
// Returns true if only the user scene setup mode is active.
// ---------------------------------------------------------------------------
//
TBool CCamUserSceneSetupViewBase::IsOnlyUserSceneSetupModeActive()
    {
    return ( iUserSceneSetupModeActive && !iCaptureSetupModeActive 
        && !iSceneSettingModeActive && !iUserScenePageModeActive );
    }

// ---------------------------------------------------------------------------
// CCamUserSceneSetupViewBase::DisplayResetUserSceneDlgL
// Display reset user setting confirmation dialog
// ---------------------------------------------------------------------------
//
void CCamUserSceneSetupViewBase::DisplayResetUserSceneDlgL()
	{
	HBufC* confirmationText;
    confirmationText = StringLoader::LoadLC( R_CAM_RESET_USER_SCENE_NOTE_TEXT );
    CAknQueryDialog* confirmationDialog = new( ELeave )CAknQueryDialog( *confirmationText );
    CleanupStack::PopAndDestroy( confirmationText );
    
    if ( confirmationDialog->ExecuteLD( R_CAM_RESET_USER_SCENE_NOTE ) )
        {
        iController.ResetUserSceneL();
        CCamUserSceneSetupContainer* userSceneSetup = 
                static_cast<CCamUserSceneSetupContainer*>( iContainer );

        userSceneSetup->UpdateListItems();
        userSceneSetup->UpdateDisplayL();
        } 
	}

//  End of File




