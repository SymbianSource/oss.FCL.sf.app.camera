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
* Description:  Base class for all views used in the Camera application*
*/


// INCLUDE FILES

#include "CamAppUi.h"
#include "CamViewBase.h"
#include "CamContainerBase.h"
#include "CamStandbyContainer.h"
#include "CamUtility.h"
#include "CamPanic.h"
#include <eikmenub.h>
 

#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <akntoolbar.h>

#include "CamUtility.h"
#include <AknWaitDialog.h>
#include "CamWaitDialog.h"
#include "CamCommandHandlerAo.h"
#include "CameraUiConfigManager.h"


static const TRect KCamCbaHiddenRect = TRect(640,640,640,640);


// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CCamViewBase destructor
// 
// ---------------------------------------------------------------------------
//
CCamViewBase::~CCamViewBase()
  {
  PRINT( _L("Camera => ~CCamViewBase" ))  
  if ( iStandbyContainer )
    {
    CAknViewAppUi* appUi = AppUi();
    
    if ( appUi )
      {
      appUi->RemoveFromStack( iStandbyContainer );
      }
        
    delete iStandbyContainer;
    iStandbyContainer = NULL;
    }

  delete iContainer;
  iContainer = NULL;
  
  delete iCommandHandlerAo;
  iCommandHandlerAo = NULL;
  
  PRINT( _L("Camera <= ~CCamViewBase" ))  
  }

// -----------------------------------------------------------------------------
// CCamViewBase::HandleFocusLossL
// Handle focus loss
// -----------------------------------------------------------------------------
//
void CCamViewBase::HandleFocusLossL()
    {
    // intentionally doing nothing
    }
    
// ---------------------------------------------------------------------------
// HandleAppEvent <<virtual>>
//
// ---------------------------------------------------------------------------
//
void 
CCamViewBase::HandleAppEvent( const TCamAppEvent& aEvent )
  {
  PRINT1( _L("Camera => CCamViewBase::HandleAppEvent, event:%d"), aEvent );
  if( iContainer )
    {
    iContainer->HandleAppEvent( aEvent );
    }
  PRINT( _L("Camera <= CCamViewBase::HandleAppEvent") );
  }


// -----------------------------------------------------------------------------
// CCamViewBase::PrepareForModeChange
// Prepare for mode change
// -----------------------------------------------------------------------------
//
void CCamViewBase::PrepareForModeChange()   
    {
    // intentionally doing nothing
    }
  
// -----------------------------------------------------------------------------
// CCamViewBase::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamViewBase::HandleCommandL( TInt aCommand )
    {
    PRINT( _L("Camera => CCamViewBase::HandleCommandL"))
    switch(aCommand)
        {
        case ECamCmdInternalExit:
            {   
            TInt standbyError = KErrNone;        
            if ( iStandbyModeActive && iStandbyContainer )
                {
                standbyError = iStandbyContainer->GetStandbyError();
                ExitStandbyModeL();
                }

            if ( standbyError != KErrNone )
                {
                AppUi()->HandleCommandL( EEikCmdExit );
                }
            else
                {
                AppUi()->HandleCommandL( aCommand );
                }

            // dismiss menu - if open
            StopDisplayingMenuBar();
            }
            break;
            
        case ECamCmdRedrawScreen:
            {
            if( iContainer )
            	{
            	iContainer->DrawDeferred();
            	}           
            }
            break;
            
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCamViewBase::HandleForegroundEventL
// Handle foreground event
// -----------------------------------------------------------------------------
//
void CCamViewBase::HandleForegroundEventL( TBool /*aForeground*/ )
    {
    PRINT( _L("CCamViewBase::HandleForegroundEventL") );
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );  
    iEmbedded = appUi->IsEmbedded();
    PRINT1( _L("CCamViewBase::HandleForegroundEventL embedded %d"), iEmbedded );    
    }

// ---------------------------------------------------------------------------
// CCamViewBase::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void 
CCamViewBase::DoActivateL( const TVwsViewId& /*aPostvViewId*/, 
                                 TUid        aCustomMessageId, 
                           const TDesC8&     /*aCustomMessage*/ )
  {
  PRINT( _L("Camera => CCamViewBase::DoActivateL"));    

  CCamContainerBase::DestroyActivePalette();     


  if(ECamViewMessageDeactivateFirst == aCustomMessageId.iUid)
    {
    PRINT( _L("CCamViewBase::DoActivateL call deactivate"));    
    TBool wasCameraUser = IsCameraUser();    

    PrepareDeactivate();
    // since we still use it
    if( wasCameraUser )
      {
      IncrementCameraUsers();
      }
    }
  
  CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );  
  iEmbedded = appUi->IsEmbedded();

  PRINT1( _L("CCamViewBase::DoActivateL Embedded %d"), iEmbedded);    
  if ( !iContainer )
    {
    CreateContainerL();
    
    if( iTempContainer )
        {
        AppUi()->RemoveFromStack( iTempContainer );
        }
    AppUi()->AddToStackL( *this, iContainer );
    iContainer->ActivateL();
    }
  PostDeactivate();
  
  SetTitlePaneTextL();
  
  TInt orientation;
  TCamPsiKey queryValue = ECamPsiOrientation;
  CCameraUiConfigManager* configManager = iController.UiConfigManagerPtr(); 

  if ( configManager && configManager->IsSecondaryCameraSupported() )
      {
      if ( appUi->IsSecondCameraEnabled() && !appUi->IsQwerty2ndCamera() ) // if front camera is active         
          {
          queryValue = ECamPsiSecondaryCameraOrientation;
          }
      else // back camera is active
          {
          queryValue = ECamPsiPrimaryCameraOrientation;
          }
      }
    
  CamUtility::GetPsiInt( queryValue, orientation );

  AppUi()->SetOrientationL( ( CAknAppUiBase::TAppUiOrientation ) orientation );

  TCamOrientation camOrientation = appUi->CamOrientation();

  // if in camcorder mode need to show navi-ctr
  if( camOrientation == ECamOrientationCamcorder ||  
      camOrientation == ECamOrientationCamcorderLeft || 
      camOrientation == ECamOrientationPortrait )
    {
		iContainer->SetupNaviPanesL( CCamContainerBase::ECamPaneCounter );
		}
  PRINT( _L("Camera <= CCamViewBase::DoActivateL"));    
  }

// ---------------------------------------------------------------------------
// CCamViewBase::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CCamViewBase::DoDeactivate()
  {
  PRINT( _L("Camera => CCamViewBase::DoDeactivate" ))
	// Both standard container and standby container should be deactivated
    if ( iStandbyContainer ) // implies IsInStandbyMode
        {
        AppUi()->RemoveFromStack( iStandbyContainer );
        delete iStandbyContainer;
        iStandbyContainer = NULL;
        SetStandbyModeActive( EFalse );
        }

	if ( iContainer )
		{
  		AppUi()->RemoveFromStack( iContainer );
    	delete iContainer;
    	iContainer = NULL;
    	}
    	
	PRINT( _L("Camera <= CCamViewBase::DoDeactivate" ))
	}

// ---------------------------------------------------------------------------
// CCamViewBase::PrepareDeactivate
// Prepare deactivation of this view
// ---------------------------------------------------------------------------
//
void CCamViewBase::PrepareDeactivate()
    {
    PRINT( _L("Camera => CCamViewBase::PrepareDeactivate" ));
      // Both standard container and standby container should be deactivated
      // Store them for temporal variables until new containers have been
      // created.
      if ( iStandbyContainer ) // implies IsInStandbyMode
          {
          AppUi()->RemoveFromStack( iStandbyContainer );
          iTempStandbyContainer=iStandbyContainer;
          iStandbyContainer = NULL;
          SetStandbyModeActive( EFalse );
          }

      if ( iContainer )
          {
          iTempContainer=iContainer;
          iContainer = NULL;
          }          
    PRINT( _L("Camera <= CCamViewBase::PrepareDeactivate" ));    
    }


// ---------------------------------------------------------------------------
// CCamViewBase::PostDeactivate
// Completed prepared deactivation of this view
// ---------------------------------------------------------------------------
//
void CCamViewBase::PostDeactivate()
    {
    PRINT( _L("Camera => CCamViewBase::PostDeactivate" ));    
    // Both standard container and standby container should be deactivated
    if ( iTempStandbyContainer ) // implies IsInStandbyMode
        {
        delete iTempStandbyContainer;
        iTempStandbyContainer = NULL;
        }

    if ( iTempContainer )
        {
        delete iTempContainer;
        iTempContainer = NULL;
        }              
    PRINT( _L("Camera <= CCamViewBase::PostDeactivate" ));    
    }

// ---------------------------------------------------------------------------
// CCamViewBase::CCamViewBase
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamViewBase::CCamViewBase( CCamAppController& aController )
    : iController( aController )
    {
    }
    
// ---------------------------------------------------------------------------
// CCamViewBase::UpdateCbaL
// Update softkeys to reflect current state
// ---------------------------------------------------------------------------
//
void CCamViewBase::UpdateCbaL()
    {
    }
    

// ----------------------------------------------------
// CCamViewBase::BlankSoftKeysL
// Blank the softkeys
// ----------------------------------------------------
//
void CCamViewBase::BlankSoftkeysL()
    {
    // blank the cba 
    SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
    }



// ---------------------------------------------------------------------------
// CCamViewBase::SwitchToStandbyModeL
// Switches to standby mode
// ---------------------------------------------------------------------------
//
void CCamViewBase::SwitchToStandbyModeL( TCamAppViewIds aViewId, TInt aError )
    {
    PRINT( _L("Camera => CCamViewBase::SwitchToStandbyModeL") );
    // check if already in standby mode
    if( iStandbyModeActive )
    	{
    	PRINT( _L("Camera <= CCamViewBase::SwitchToStandbyModeL: already in standby") );
    	return;
    	}

    StatusPane()->MakeVisible( ETrue );
    delete iStandbyContainer;
    iStandbyContainer = NULL;
    iStandbyContainer = CCamStandbyContainer::NewL( AppUi()->ClientRect(),
                                                    *this, iController, aError );
    // dismiss menu - if open
    StopDisplayingMenuBar();

    // save options menu id for reactivation
    if ( aViewId == ECamViewIdStillPreCapture )
        {
        if ( iEmbedded )
            {
            iPreviousMenuResourceId = ROID(R_CAM_STILL_EMBEDDED_PRE_CAPTURE_MENUBAR_ID);
            }
        else
            {
            iPreviousMenuResourceId = ROID(R_CAM_STILL_PRE_CAPTURE_MENUBAR_ID);
            }
        }
    else if ( aViewId == ECamViewIdVideoPreCapture )
        {
        if ( iEmbedded )
            {
            iPreviousMenuResourceId = ROID(R_CAM_VIDEO_EMBEDDED_PRE_CAPTURE_MENUBAR_ID);
            }
        else
            {
            iPreviousMenuResourceId = ROID(R_CAM_VIDEO_PRE_CAPTURE_MENUBAR_ID);
            }
        }
    else if ( aViewId == ECamViewIdVideoSettings )
        {
        iPreviousMenuResourceId = ROID(R_CAM_SETTINGS_LIST_MENUBAR_ID);
        }
    else if ( aViewId == ECamViewIdPhotoSettings )
        {
        iPreviousMenuResourceId = ROID(R_CAM_SETTINGS_LIST_MENUBAR_ID);
        }
    else
        {
        // remove Lint error
        }

    // activate standby container and deactivate current container
    iStandbyContainer->SetMopParent( this );   
    
    CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );
    appUi->AddToStackL( *this, iStandbyContainer );
    iStandbyContainer->ActivateL();
    appUi->RemoveFromStack( iContainer );
    iContainer->MakeVisible( EFalse );   
    iStandbyContainer->MakeVisible( ETrue );
    iStandbyContainer->DrawNow();

    UnsetCourtesySoftKeysL();

    // update softkeys
    if ( !appUi->IsConstructionComplete() )
    	{
    	BlankSoftkeysL();
    	}
    else if ( appUi->StandbyStatus() == KErrInUse ||
              appUi->StandbyStatus() == KErrPermissionDenied ||
              appUi->StandbyStatus() == KErrAccessDenied )
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );	
        }	
    else if ( iEmbedded )
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK__CONTINUE );
        }
    else
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CONTINUE );
        if( Cba() && !appUi->IsRecoverableStatus() )
            {
            Cba()->MakeCommandVisible( ECamCmdExitStandby, EFalse );
            }
        }

    // change options menu
    CEikMenuBar* menuBar = MenuBar();
    if ( menuBar )
        {
        menuBar->SetMenuTitleResourceId( R_CAM_STANDBY_MENUBAR );
        }

//    if ( iStandbyError != KErrNone )
    if ( appUi->StandbyStatus() != KErrNone && appUi->IsRecoverableStatus())
        {
        // stop idle timer - non recoverable error
        iController.StopIdleTimer();
        }

    SetStandbyModeActive(ETrue);

    // Release the camera hardware
    iController.ReleaseCamera();
    

    // cancel self timer mode if active
    if ( appUi->SelfTimerEnabled() )
        {
        appUi->SelfTimerEnableL( ECamSelfTimerDisabled );
        }
  	PRINT( _L("Camera <= CCamViewBase::SwitchToStandbyModeL") );
    }

// ---------------------------------------------------------------------------
// CCamViewBase::ExitStandbyModeL
// Exits standby mode
// ---------------------------------------------------------------------------
//
void CCamViewBase::ExitStandbyModeL()
    {
    PRINT( _L("Camera => CCamViewBase::ExitStandbyModeL") );    
    if ( !iStandbyModeActive )
        {
        return;
        }

    // delete standby container and reactivate previous container
    TRAPD( err, AppUi()->AddToStackL( *this, iContainer ) )
    iStandbyContainer->MakeVisible( EFalse );
    AppUi()->RemoveFromStack( iStandbyContainer );
    delete iStandbyContainer;
    iStandbyContainer = NULL;
    User::LeaveIfError( err );

    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );


    iContainer->MakeVisible( ETrue );   
    SetStandbyModeActive( EFalse );
    
    if ( appUi->IsSecondCameraEnabled() && !appUi->IsQwerty2ndCamera() ) 
        {
        StatusPane()->MakeVisible( ETrue );
        }
    else
        {
        StatusPane()->MakeVisible( EFalse );
        }

    CCameraUiConfigManager* configManager = iController.UiConfigManagerPtr();
    if ( !( configManager && configManager->IsUIOrientationOverrideSupported() ) ) 
        {
        appUi->RaisePreCaptureCourtesyUI(ETrue);
        }

    if ( iPreviousMenuResourceId == ROID(R_CAM_SETTINGS_LIST_MENUBAR_ID)
            || iPreviousMenuResourceId == ROID(R_CAM_STILL_EMBEDDED_PRE_CAPTURE_MENUBAR_ID)
            || iPreviousMenuResourceId == ROID(R_CAM_VIDEO_EMBEDDED_PRE_CAPTURE_MENUBAR_ID))
        {
        if ( iController.CaptureModeTransitionInProgress() )
            {
            SetSoftKeysL( R_AVKON_SOFTKEYS_BACK ); 
            }
        else
            {
            SetSoftKeysL( R_AVKON_SOFTKEYS_OPTIONS_BACK );
            }
        }
    else
        {
        SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );
        }

    // restore options menu
    CEikMenuBar* menuBar = MenuBar();
    if ( menuBar )
        {
        menuBar->SetMenuTitleResourceId( iPreviousMenuResourceId );
        }

    //SetStandbyModeActive(EFalse);
    SetTitlePaneTextL();    

    // restart idle timer
    iController.StartIdleTimer();
    PRINT( _L("Camera <= CCamViewBase::ExitStandbyModeL") );    
    }


// ---------------------------------------------------------------------------
// ExitAllModesL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamViewBase::ExitAllModesL()
  {
  // Empty default implementation.
  }


// ---------------------------------------------------------------------------
// CCamViewBase::GetHelpContext
// Called to identify the help context for this view
// ---------------------------------------------------------------------------
//
void CCamViewBase::GetHelpContext( TCoeHelpContext& /*aContext*/ ) const
    {
    // No context
    }

// ---------------------------------------------------------------------------
// CCamViewBase::QueryRenameL
// Switches orientation if required - calls DoQueryRenameL
// ---------------------------------------------------------------------------
//
void CCamViewBase::QueryRenameL( TCamCameraMode aMode )
    {
    // Ensure application is in editing orientation
    TInt error = KErrNone;
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    TInt editingOrientation = CAknAppUiBase::EAppUiOrientationUnspecified;

    error = CamUtility::GetPsiInt( ECamPsiOrientation, editingOrientation );   
    
    if ( !error )
        {
        appUi->SwitchOrientationIfRequiredL( editingOrientation );
        }    
    // Trap this to make sure the orientation is reverted back to normal        
    TRAPD( leaveErr, DoQueryRenameL( aMode ) )
    appUi->SwitchOrientationIfRequiredL( CAknAppUiBase::EAppUiOrientationUnspecified );
    User::LeaveIfError( leaveErr );    
    }
    
// ---------------------------------------------------------------------------
// CCamViewBase:DoQueryRenameL
// Show rename query and rename file.
// ---------------------------------------------------------------------------
//
void CCamViewBase::DoQueryRenameL( TCamCameraMode aMode )
  {
  TBool done = EFalse;
  HBufC* lastFilePath = HBufC::NewLC( iController.CurrentFullFileName().Length() );
  TBuf<KMaxNameBaseLengthPlusNumbering> filename( iController.CurrentImageName() );
  
  // Repeat until successfully renamed or cancelled
  while ( !done )
    {
    // Show rename query
    if ( ShowRenameQueryL( filename, aMode ) )
      {
      // Try to rename file
      TRAPD( err, done = RenameCurrentFileL( filename, aMode ) );
      if ( KErrAlreadyExists == err )
        {
        // File exists -> show overwrite confirmation query
        if ( ShowOverwriteConfirmationQueryL( filename ) )
          {
          // Yes, overwrite
          TPtr lastFilePathPtr = lastFilePath->Des();
          lastFilePathPtr = iController.CurrentFullFileName();
          TParsePtr parseptr( lastFilePathPtr );
          TFileName fullpath = parseptr.DriveAndPath();
          fullpath.Append( filename );
          fullpath.Append( parseptr.Ext() );

          // Delete the file that gets overwritten.
          if ( ECamControllerVideo != aMode )
            {
            CamUtility::DeleteStillImageL( fullpath );
            }
          else
            {
            CamUtility::DeleteVideoL( fullpath );
            }

          // Rename
          RenameCurrentFileL( filename, aMode );
          done = ETrue;
          }
        else  // No, don't overwrite
          {
          // Generate a suitable suggestion for an unique name
          TInt key             = ( ECamControllerVideo == aMode )
                                 ? ECamSettingItemVideoNumber
                                 : ECamSettingItemPhotoNumber;         
          TInt fileNameCounter = iController.IntegerSettingValue( key );

          TParsePtrC parse( iController.CurrentFullFileName() );
          TFileName filePath = parse.DriveAndPath();
          
          const TUint KMax3Digits = 999;

          //if  0 < fileNameCounter <= 999, formatted file name like "Photo001", the length of number part ("001") is fixed 3
          const TUint KFixedNumberLength = 3; 
        
          TBool exceedMaxFileNameLength = EFalse;
          //if  0 < fileNameCounter <= 999, formatted file name like "Photo001"
          if ( fileNameCounter > 0 && 
             fileNameCounter <= KMax3Digits && 
             filename.Length() + KFixedNumberLength > filename.MaxLength() )
            {
            exceedMaxFileNameLength = ETrue;
            }         
          //if  fileNameCounter > 999, formatted file name like "Photo1234"
          else if ( fileNameCounter > KMax3Digits )
            {
            _LIT( KCamNameFormatFileNameCounter, "%d" );
            TBuf<KMaxNameBaseLengthPlusNumbering> strFileNameCounter;
            strFileNameCounter.Format( KCamNameFormatFileNameCounter, fileNameCounter );
            if ( filename.Length() + strFileNameCounter.Length() > filename.MaxLength() )
              {
              exceedMaxFileNameLength = ETrue;
              }
            }
        
          if ( !exceedMaxFileNameLength )
            {
            CamUtility::GetUniqueNameL( filePath, filename, fileNameCounter, parse.Ext() );
            }
                    

          }       
        }
      else
        {
        // If unknown error
        User::LeaveIfError( err );
        }
      }
    else
      {
      // User cancelled (ShowRenameQueryL() returned false)
      done = ETrue;
      }
    }
  CleanupStack::PopAndDestroy(); // lastFilePath
  }

// ---------------------------------------------------------------------------
// CCamViewBase::ShowOverwriteConfirmationQueryL
// Show an overwrite confirmation query. This is used
// when trying to overwrite a file using rename.
// ---------------------------------------------------------------------------
//
TBool CCamViewBase::ShowOverwriteConfirmationQueryL( 
    const TDesC& aFilename )
    {
    HBufC* prompt =  StringLoader::LoadLC( R_QTN_FLDR_OVERWRITE_QUERY, 
                                           aFilename, CEikonEnv::Static() );

    CAknQueryDialog* overwriteQuery = 
        new ( ELeave ) CAknQueryDialog( *prompt );
    TBool result = overwriteQuery->ExecuteLD( R_CAM_OVERWRITE_QUERY );

    CleanupStack::PopAndDestroy(); // prompt

    return result;
    }

// ---------------------------------------------------------------------------
// CCamViewBase::ShowRenameQueryL
// Show rename query.
// ---------------------------------------------------------------------------
//
TBool CCamViewBase::ShowRenameQueryL( TDes& aFilename, TCamCameraMode aMode )
    {
    TBool done = EFalse;
    TBool res = ETrue;

    while ( !done )
        {
        // Show data query
        HBufC* prompt = NULL;
        if ( ECamControllerVideo == aMode )
            {
            prompt = 
                StringLoader::LoadLC( R_CAM_QUERY_VIDEO_NAME );
            }
        else
            {
            prompt = 
                StringLoader::LoadLC( R_CAM_QUERY_IMAGE_NAME );
            }

        CAknTextQueryDialog* nameQuery = 
            CAknTextQueryDialog::NewL( aFilename );
        nameQuery->SetMaxLength( KMaxNameBaseLengthPlusNumbering );   
        nameQuery->PrepareLC( R_CAM_RENAME_QUERY );
        nameQuery->SetPromptL( *prompt );

        TBool dialogRes = ( nameQuery->RunLD() == EAknSoftkeyOk );
        if ( dialogRes )
            {
            if ( !IsNewNameValidL( aFilename ) )
                {
                // New name is not valid for another reason.
                // In practise, the user has attempted to overwrite
                // an image already existing in the burst grid.

                HBufC* text = StringLoader::LoadLC(
                    	R_QTN_FLDR_CANT_RENAME_ITEM,
                    	iController.CurrentImageName() );

                CAknNoteWrapper* note = new ( ELeave ) CAknNoteWrapper();
                note->ExecuteLD( R_CAM_UNABLE_TO_RENAME, *text );

                CleanupStack::PopAndDestroy(); // text

                // Don't show dialog again
                res = EFalse;
                done = ETrue;
                }
            else if ( CamUtility::CheckFileNameValidityL( aFilename ) )
                {
                // New file name seems ok
                res = ETrue;
                done = ETrue;
                }
            else
                {
                // File name is not suitable, ask again
                }
            }
        else
            {
            // User cancelled rename
            res = EFalse;
            done = ETrue;
            }

        CleanupStack::PopAndDestroy(); // prompt
        }

    return res;
    }

// ---------------------------------------------------------------------------
// CCamViewBase::IsNewNameValidL
// Check if renaming to the given name is valid.
// ---------------------------------------------------------------------------
//
TBool CCamViewBase::IsNewNameValidL( const TDesC& /* aFilename */ )
    {
    // This should be overridden in derived classes if needed
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCamViewBase::RenameCurrentFileL
// Renames the current capture array file.
// ---------------------------------------------------------------------------
//
TBool 
CCamViewBase::RenameCurrentFileL( const TDesC& aNewName, 
                                  TCamCameraMode aMode )
  {
  return iController.RenameCurrentFileL( aNewName, aMode );
  }

// ---------------------------------------------------------------------------
// CCamViewBase::Container
// Returns the currently used container in the view.
// ---------------------------------------------------------------------------
//
CCamContainerBase* CCamViewBase::Container()
    {
    if ( iStandbyContainer )
        {
        return iStandbyContainer;
        }
    else
        {
        return iContainer;
        }
    }

// ---------------------------------------------------------------------------
// CCamViewBase::SetSoftKeysL
// Sets the softkeys to the specified Resource ID
// ---------------------------------------------------------------------------
//
void CCamViewBase::SetSoftKeysL(TInt aResource)
	{
    PRINT1( _L("Camera => CCamViewBase::SetSoftKeysL SoftKeys:0x%x"), aResource );

    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );	  
    // If courtesy softkeys are enabled, check whether the UI can be drawn without being explicitly raised
    // (rare, but possible: e.g. when video recording starts
    if(aResource == R_CAM_SOFTKEYS_BLANK &&
       appUi->IsSecondCameraEnabled() &&
       !appUi->IsQwerty2ndCamera())
        {
        aResource = R_CAM_SOFTKEYS_BLANK_SECONDARY;
        }
    if ( iCourtesySoftkeys )
        {
        if ( appUi )
            {
            if (   (IsPreCapture()  && appUi->DrawPreCaptureCourtesyUI())
                || (IsPostCapture() && appUi->DrawPostCaptureCourtesyUI()) )
                {
                // Disable the courtesy softkeys, otherwise nothing will be drawn when the rest of the
                // courtesy UI has been turned back on
                iCourtesySoftkeys = EFalse;

                //when disabling the courtesy softkeys, Cba should be recovered to be visible
                Cba()->MakeVisible( ETrue );
                }
            }
        }
        
    if ( iCourtesySoftkeys || !Cba() )
        {
        // If the courtesy (blank) softkeys are being shown, make a note of what to change back to.
        iPreviousCbaResource = aResource;
        }
    else if ( iCurrentCbaResource != aResource )
        {
        iPreviousCbaResource = iCurrentCbaResource;
        iCurrentCbaResource = aResource;

        Cba()->SetCommandSetL( aResource );
        Cba()->DrawDeferred();

        }

    PRINT( _L("Camera <= CCamViewBase::SetSoftKeysL") );
	}


// ---------------------------------------------------------------------------
// CCamViewBase::SetCourtesySoftKeysL
// Blanks the softkeys for the Courtesy UI Off state. 
// ---------------------------------------------------------------------------
//
void CCamViewBase::SetCourtesySoftKeysL()
    {
    if ( !iCourtesySoftkeys )
        {
        Cba()->MakeVisible( EFalse );
        iPreviousCbaResource = iCurrentCbaResource;
        iCourtesySoftkeys = ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CCamViewBase::UnsetCourtesySoftKeysL
// Returns the softkeys to their previous state after UI has been turned back on
// ---------------------------------------------------------------------------
//
void CCamViewBase::UnsetCourtesySoftKeysL()
    {
    if ( iCourtesySoftkeys )
        {
        iCourtesySoftkeys = EFalse;
        SetSoftKeysL(iPreviousCbaResource);
        Cba()->MakeVisible( ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CCamViewBase::IsPreCapture
// Returns whether the view is a precapture view
// ---------------------------------------------------------------------------
//
TBool CCamViewBase::IsPreCapture()
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCamViewBase::IsPostCapture
// Returns whether the view is a postcapture view
// ---------------------------------------------------------------------------
//
TBool CCamViewBase::IsPostCapture()
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCamViewBase::IsBurstPostCapture
// Returns whether the view is a burst mode postcapture view
// ---------------------------------------------------------------------------
//
TBool CCamViewBase::IsBurstPostCapture()
    {
    return EFalse;
    }

    
// ---------------------------------------------------------------------------
// CCamViewBase::IsInStandbyMode
// Returns whether the view is in standby mode
// ---------------------------------------------------------------------------
//
TBool CCamViewBase::IsInStandbyMode()
    {
    return iStandbyModeActive;
    }


// ---------------------------------------------------------------------------
// CCamViewBase::SetStandbyModeActive
// Sets the iStandbyModeActive flag
// ---------------------------------------------------------------------------
//
void CCamViewBase::SetStandbyModeActive(TBool aActive)
    {
    // This should be the only place iStandbyModeActive is set!
    // We need to inform the AppUi
    iStandbyModeActive = aActive;
     if( iController.IsAppUiAvailable() )
     {
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    
    if ( appUi)
        {
        if ( aActive )
            {
            appUi->SetPreCaptureMode(ECamPreCapStandby);
            }
        else
            {
            appUi->SetPreCaptureMode(ECamPreCapViewfinder);
            }        
        }
        }
    }


// ---------------------------------------------------------------------------
// CCamViewBase::IncrementCameraUsers
//
// ---------------------------------------------------------------------------
//
void 
CCamViewBase::IncrementCameraUsers()
    {
    if( !iIncEngineCount ) 
        {
        PRINT( _L("Camera == CCamViewBase::IncrementCameraUsers - incrementing") );
        iIncEngineCount = ETrue;
        iController.IncCameraUsers();
        }
    else
        {
        PRINT( _L("Camera == CCamViewBase::IncrementCameraUsers - already incremented, skipping") );
        }
    }
        

// ---------------------------------------------------------------------------
// CCamViewBase::DecrementCameraUsers
//
// ---------------------------------------------------------------------------
//
void 
CCamViewBase::DecrementCameraUsers()
    {
    if( iIncEngineCount ) 
        {
        PRINT( _L("Camera == CCamViewBase::DecrementCameraUsers - decrementing") );
        iIncEngineCount = EFalse;
        iController.DecCameraUsers();
        }
    else
        {
        PRINT( _L("Camera == CCamViewBase::DecrementCameraUsers - not using camera, skipped") );
        }
    }


// ---------------------------------------------------------------------------
// CCamViewBase::IsCameraUser
//
// ---------------------------------------------------------------------------
//
TBool 
CCamViewBase::IsCameraUser() const
    {
    return iIncEngineCount;
    }



// -----------------------------------------------------------------------------
// CCamViewBase::RedrawAPArea
// -----------------------------------------------------------------------------
void CCamViewBase::RedrawAPArea(const TRect& aArea)
    {
    if ( iContainer )
        {
        iContainer->Redraw(aArea);
        }
    }


// -----------------------------------------------------------------------------
// CCamViewBase::ViewCba
// -----------------------------------------------------------------------------
CEikButtonGroupContainer* CCamViewBase::ViewCba()
    {
    return Cba();
    }

// -----------------------------------------------------------------------------
// CCamViewBase::UpdateToolbarIconsL
// 
void CCamViewBase::UpdateToolbarIconsL()
    {
    
    }
// -----------------------------------------------------------------------------
// CCamViewBase::HandleCommandAoL
// -----------------------------------------------------------------------------    
    
void CCamViewBase::HandleCommandAoL( TInt aCommand )
    {
    if ( !iCommandHandlerAo )
        {
        iCommandHandlerAo = CCamCommandHandlerAo::NewL( this );
        }
    iCommandHandlerAo->HandleCommandL( aCommand );
    }    

// -----------------------------------------------------------------------------
// CCamViewBase::SetStandbyStatusL
// -----------------------------------------------------------------------------    
 
void CCamViewBase::SetStandbyStatusL( TInt aError )
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    __ASSERT_DEBUG( appUi, CamPanic( ECamPanicNullPointer ) );
    if( iStandbyModeActive )
        {
        appUi->SetStandbyStatus( aError );
        iStandbyContainer->SetStandbyErrorL( aError );
        if ( aError == KErrInUse ||
             aError == KErrPermissionDenied ||
             aError == KErrAccessDenied )
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT );    
            }   
        else if ( iEmbedded )
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK__CONTINUE );
            }
        else
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CONTINUE );
            if( Cba() && !appUi->IsRecoverableStatus() )
                {
                Cba()->MakeCommandVisible( ECamCmdExitStandby, EFalse );
                }
            else
                {
                Cba()->MakeCommandVisible( ECamCmdExitStandby, ETrue );
                }
            }
        }
    }
//  End of File  
