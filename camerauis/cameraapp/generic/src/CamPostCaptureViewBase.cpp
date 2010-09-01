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
* Description:  Post-capture view class for Camera application*
*/


// INCLUDE FILES
#include <sendnorm.rsg>
#include <sendui.h>
#include <SenduiMtmUids.h>
#ifndef __WINS__
//  #include <SFIUtilsAppInterface.h>
#endif
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <aknradiobuttonsettingpage.h>
#include <akntextsettingpage.h>
#include <akntitle.h>  // CAknTitlePane
#include <CMessageData.h>
#include "MCamAddToAlbumObserver.h"
#include "CamCollectionManagerAo.h"
#include <glxmetadataviewutility.h>
#include <glxcollectionselectionpopup.h>
#include <akntoolbar.h>
#include <aknbutton.h>
#include <StringLoader.h>


#include "Cam.hrh"
#include "CamUtility.h"
#include "CamPostCaptureViewBase.h"
#include "CamAppController.h"
#include "CamAppUi.h"
#include "CamLogger.h"
#include "camactivepalettehandler.h"
#include "CamPostCaptureContainer.h"
#include "camoneclickuploadutility.h"
#include "CameraUiConfigManager.h"



//CONSTANTS


const TInt KCamSendUiBodySize = 0;
const TInt KCamSendUiMessageSize = 0;


const TUint KCamSendAsArrayGranularity = 1;


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::~CCamPostCaptureViewBase
// Destructor
// ---------------------------------------------------------------------------
//
CCamPostCaptureViewBase::~CCamPostCaptureViewBase()
  {
  PRINT( _L("Camera => ~CCamPostCaptureViewBase") );
  delete iSendAppUi;    
  delete iSendMtmsToDim;
  
  if ( iAiwServiceHandler )
    {
    // unload service provider if any
    iAiwServiceHandler->Reset();
    delete iAiwServiceHandler;
    }

#ifndef __WINS__    
  iSFIUtils = NULL;       // Not owned
#endif // __WINS__


  if ( iCollectionManagerCallBack )
      {
      delete iCollectionManagerCallBack;
      iCollectionManagerCallBack = NULL;
      }
  delete iOneClickUploadUtility;

  PRINT( _L("Camera <= ~CCamPostCaptureViewBase") );
  }

// -----------------------------------------------------------------------------
// CCamPostCaptureViewBase::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::HandleCommandL( TInt aCommand )
    {
    PRINT1( _L("Camera => CCamPostCaptureViewBase::HandleCommandL (%d)"), aCommand )
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    switch ( aCommand )
        {
        case ECamCmdOneClickUpload:
            {
            iOneClickUploadUtility->UploadL(
                    iController.CurrentFullFileName() );
            break;
            }
        case ECamCmdToggleActiveToolbar:
            {
       		// If in the Landscape mode, MSK event is mapped 
      	  	// like a normal selection key so that AP items
       	  	// are selected and respective setting pages are launched´
       	  	// refer to @ProcessCommandL, since we get here only from there
       	    TKeyEvent aKeyEvent;
    		aKeyEvent.iCode =  aKeyEvent.iRepeats = aKeyEvent.iModifiers = 0;
   			aKeyEvent.iScanCode = EStdKeyDevice3;
   			static_cast<CCamContainerBase*>
       				( Container() )->OfferKeyEventL( aKeyEvent, EEventKey );	
            break;
            }
        case EAknSoftkeyBack:
            {
            iController.DeepSleepTimerCancel();
            appUi->HandleCommandL( aCommand );
            }
            break;

        case ECamCmdSend:
        case ECamCmdSendToCallerMultimedia:    
        case ECamCmdQuickSend:
            {
/*            TBool inCall = EFalse;
#if !defined(__WINS__) && !defined(__WINSCW__)
            inCall = iSFIUtils->IsCLIValidL();
#endif	            */
            //use both inCall and InCallOrRinging() conditions to make sure if it is in call state
// In-Call-Send no longer used
//            if ( /*inCall &&*/ iController.InCallOrRinging() )
//                {
//#ifndef __WINS__
//                DoInCallSendL();
//#endif
//                }
//            else
//                {
                DoSendAsL();                    	
//                }    
            }
            break;

        case ECamCmdExitStandby:
        	{
        	// leave standby mode
        	ExitStandbyModeL();
        	}
        	break;
        case ECamCmdToggleCourtesyUI:
            {
            /*
            * EFalse --> We just hide the AP tool bar
            * ETrue  --> We show the AP tool bar
            * In this case, we dont toggle the UI between Show/Hide icons so
            * we send EFalse as a parameter to SetAlwaysDrawPostCaptureCourtesyUI method
            */
            if ( appUi )
                {
                appUi->SetAlwaysDrawPostCaptureCourtesyUI( EFalse );
                }
            break;
            }
		case ECamCmdProperties:
            {
            // Passing the name of the current file
            // Asking the MetadataViewUtility to lauch an view for us
            CEikStatusPane* sp = StatusPane();
            CAknTitlePane* title = 
                static_cast<CAknTitlePane*>( sp->ControlL( 
                        TUid::Uid( EEikStatusPaneUidTitle ) ) );
            title->SetTextL( KNullDesC );
            sp->MakeVisible( ETrue );
            title->MakeVisible( ETrue );
            iContainer->SetRect( AppUi()->ClientRect() );
            GlxMetadataViewUtility::ActivateViewL( iController.CurrentFullFileName() );
            static_cast<CCamContainerBase*>(iContainer)->CheckForFileName( ETrue );
            title->MakeVisible( EFalse );
            sp->MakeVisible( EFalse );
            iContainer->SetRect( AppUi()->ApplicationRect() );                
			break;
            }
        // Add Photo/Video to the album
        case ECamCmdAddToAlbum:
            {

                /*
                * Status = 0 ( User confirms add to album operation )
                * Status = 1 ( User cancels add to album operation )
                */
                /*TInt status = ShowAddToAlbumConfirmationQueryL();
                if ( status == 0 )
                    {
                    StartAddToAlbumOperationL();
                    }
                else
                    { // Lint warnings
                    }*/
            StartAddToAlbumOperationL();
            }
            break;
        case KAiwCmdEdit:
        case ECamCmdEdit:
        case ECamCmdEditPhoto:
        case ECamCmdEditVideo:    
            {
            CAiwGenericParamList& inputParams = iAiwServiceHandler->InParamListL();
            TPtrC currentFullFileName(iController.CurrentFullFileName());
            TAiwGenericParam param( EGenericParamFile, TAiwVariant(currentFullFileName));
            inputParams.AppendL( param );
            
            
            TAiwVariant param2Variant;
            if(Id().iUid == ECamViewIdVideoPostCapture)
                {
                param2Variant.Set(_L("video/*"));
                }
            else
                {
                param2Variant.Set(_L("image/jpeg"));
                }
            TAiwGenericParam param2( EGenericParamMIMEType, param2Variant );
            inputParams.AppendL( param2 );

            iAiwServiceHandler->ExecuteServiceCmdL(KAiwCmdEdit, inputParams, iAiwServiceHandler->OutParamListL());        
            }
            break;
        case ECamCmdShareSettings: // SHARE_AIW
        case KAiwCmdView:
            {
            PRINT1( _L("Camera <> CCamPostCaptureViewBase::HandleCommandL - ECamCmdShareSettings start, cmd:%d"), aCommand );
            iOneClickUploadUtility->LaunchShareSettings();
            PRINT( _L("Camera <> CCamPostCaptureViewBase::HandleCommandL - ECamCmdShareSettings end") );            
            }
            break;
        default:
            {
            CCamViewBase::HandleCommandL( aCommand );
            }
        }

    PRINT1( _L("Camera <= CCamPostCaptureViewBase::HandleCommandL (%d)"), aCommand )
    }

// -----------------------------------------------------------------------------
// CCamPostCaptureViewBase::HandleControllerEventL
// Handle user menu selections
// -----------------------------------------------------------------------------
//
void 
CCamPostCaptureViewBase::HandleControllerEventL( TCamControllerEvent aEvent, 
                                                 TInt               /*aError*/ )
  {
  PRINT( _L("Camera => CCamPostCaptureViewBase::HandleControllerEventL") );
    
  switch( aEvent )
    {
    // -----------------------------------------------------
    case ECamEventCallStateChanged:
      {
      PRINT( _L("Camera <> case ECamEventCallStateChanged") );
      
      CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );      
      
      // Reset the Send and Send to caller items in the active toolbar  
      if ( !iEmbedded )
        {
		CCamActivePaletteHandler* apHandler = appUi->APHandler();    
		// Hide Send or Send to caller
        ResetSendAvailabilityL( apHandler );
		if ( iController.IsTouchScreenSupported() )
			{
			UpdateToolbarIconsL();
			}
		else
			{
			UpdateActivePaletteItemsL();
			}
        }
      break;
      }
    // -----------------------------------------------------
    case ECamEventSaveComplete:
    case ECamEventSnapshotReady:
      {
      if ( ECamEventSaveComplete == aEvent &&
           iWaitForImageSave &&
           !iController.IsTouchScreenSupported() )
        {
        UpdateActivePaletteItemsL();
        }
      static_cast<CCamContainerBase*>(iContainer)->CheckForFileName( EFalse );
      // Check that the file that has just completed is the one just taken
      if ( iController.SavedCurrentImage()
        && iController.IsAppUiAvailable()            
         )//&& iWaitForImageSave )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
        // check whether embedded or not
        if ( iEmbedded )
          {
          //Do not show postcapture softkeys if we are still capturing or
          //saving image
          if ( iController.CurrentOperation() != ECamCapturing && 
                  iController.CurrentOperation() != ECamCompleting )
              {
              if ( !appUi->IsSecondCameraEnabled() || appUi->IsQwerty2ndCamera() )  
                  {
                  SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT );
                  }
              else
                  {
                  SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT_SECONDARY );
                  }
              }
          else
              {
              SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
              }
          }
        else
          {
          appUi->RaisePostCaptureCourtesyUI();
          if ( appUi->IsSecondCameraEnabled() && !appUi->IsQwerty2ndCamera() )
              {
              SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK_TOGGLETOOLBAR_PORTRAIT );
              }
          else if ( ECamViewStateBurstThumbnail == appUi ->CurrentViewState() )
              {
              SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK_TOGGLETOOLBAR_BURST );
              }
          else
              {
              SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK_TOGGLETOOLBAR );
              }
          
          // image processing complete, enable gallery button
          if( !iEmbedded )        
              {
              // fixed toolbar is used only with touch devices
              if ( iController.IsTouchScreenSupported() )
                  {
                  CAknToolbar* fixedToolbar = Toolbar();
                  if ( fixedToolbar )
                      {
                      /*fixedToolbar->SetItemDimmed( ECamCmdPhotos, EFalse, ETrue );
                      fixedToolbar->SetItemDimmed( ECamCmdEdit, EFalse, ETrue );
                      fixedToolbar->SetItemDimmed( ECamCmdDelete, EFalse, ETrue );
                      fixedToolbar->SetItemDimmed( ECamCmdOneClickUpload, EFalse, ETrue );
                      fixedToolbar->SetItemDimmed( ECamCmdSend, EFalse, ETrue );*/
                      fixedToolbar->SetDimmed(EFalse);
                      fixedToolbar->DrawNow();
                      }
                  }
              }
          }
        iWaitForImageSave = EFalse;
        }
      break;
      }
      
    case ECamEventSliderClosed:
      {
      // if embedded and the slider is closed, close the menu
      if( iEmbedded )
        {
        PRINT( _L("Camera => CCamPostCaptureViewBase -> ECamEventSliderClosed") );
        StopDisplayingMenuBar();
        }
      }
       
    // -----------------------------------------------------
    default:
      break;
    // -----------------------------------------------------
    } // switch
  PRINT( _L("Camera <= CCamPostCaptureViewBase::HandleControllerEventL") );        
  }

// -----------------------------------------------------------------------------
// CCamPostCaptureViewBase::HandleForegroundEventL
// Handle foreground event
// -----------------------------------------------------------------------------
//
void 
CCamPostCaptureViewBase::HandleForegroundEventL( TBool aForeground )
  {    
  PRINT( _L("Camera => CCamPostCaptureViewBase::HandleForegroundEventL") )
  CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
  // -------------------------------------------------------
  // Coming to foreground
  if ( aForeground)
    {
    PRINT( _L("Camera <> CCamPostCaptureViewBase::HandleForegroundEventL: to foreground") )
    if ( static_cast<CCamAppUiBase*>( AppUi() )->IsInPretendExit() )
      {
      PRINT( _L("Camera <> CCamPostCaptureViewBase::HandleForegroundEventL: app in pretend exit so not grabbing the engine") )
      // The view can get a foreground event while the application is
      // actually in a pretend exit situation. This occurs when the view switch
      // was called before the exit event, but didn't complete until after the
      // exit event. In this case the view should not register an interest in
      // the engine as the application is really in the background and the resources
      // need to be released
      return;
      }

    // set embedded flag here
    iEmbedded = appUi->IsEmbedded();
    // need to update cba according to whether we are embedded or not
    if ( iEmbedded )
      {
      PRINT( _L("Camera <> CCamPostCaptureViewBase::HandleForegroundEventL: embedded mode") );
      //Do not show postcapture softkeys if we are still capturing or
      //saving image
      if ( iController.CurrentOperation() != ECamCapturing && 
              iController.CurrentOperation() != ECamCompleting )
          {
          if ( !appUi->IsSecondCameraEnabled() || appUi->IsQwerty2ndCamera() )  
              {
              SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT );
              }
          else
              {
              SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT_SECONDARY );
              }
          }
      else
          {
          SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
          }
      }
    else
      {
      // Update the one click, in case it was modified
      if ( iOneClickUploadUtility && iOneClickUploadUtility->OneClickUploadSupported() )
          {
          iOneClickUploadUtility->UpdateUploadIconL( Toolbar(), 
						  ( Id().iUid == ECamViewIdVideoPostCapture ) ? 
        				    ECamControllerVideo : ECamControllerImage );
          }

      // ensure cba is reset if we're not embedded
      if ( !iWaitForImageSave )
        {
        if ( static_cast<CCamAppUiBase*>( AppUi() )->IsSecondCameraEnabled() && 
            !static_cast<CCamAppUiBase*>( AppUi() )->IsQwerty2ndCamera() )
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK_TOGGLETOOLBAR_PORTRAIT );
            }
        else if ( ECamViewStateBurstThumbnail == appUi ->CurrentViewState() )
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK_TOGGLETOOLBAR_BURST );
            }
        else
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_BACK_TOGGLETOOLBAR );
            }
        }
      else
        {
        // still image not yet saved
        // disable Options softkey and cancel post capture timer
        // softkey will be re-enabled when image has been saved
        if(appUi->IsSecondCameraEnabled() && !appUi->IsQwerty2ndCamera())
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_BACK_SECONDARY );
            }
        else
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_BACK );
            }
        
        
        // disable gallery button until image is processed
        if( !iEmbedded )        
            {
            // fixed toolbar is used only with touch devices
            if ( iController.IsTouchScreenSupported() )
                {
                CAknToolbar* fixedToolbar = Toolbar();
                if ( fixedToolbar )
                    {
                    /*fixedToolbar->SetItemDimmed( ECamCmdPhotos, ETrue, ETrue );
                    fixedToolbar->SetItemDimmed( ECamCmdDelete, ETrue, ETrue );
                    fixedToolbar->SetItemDimmed( ECamCmdOneClickUpload, ETrue, ETrue );
                    fixedToolbar->SetItemDimmed( ECamCmdEdit, ETrue, ETrue );
                    fixedToolbar->SetItemDimmed( ECamCmdSend, ETrue, ETrue );*/
                    }
                }
            }
        }
      }
    // Register that we still want to use the engine (to receive final snap image)
    IncrementCameraUsers();
    
    iController.DeepSleepTimerStart();
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );

    if ( appUi && !iSuppressAPUntilCameraReady )
      {
      if(!iPartialBackground)
          {
          appUi->RaisePostCaptureCourtesyUI();
          }
      }
    else if ( iSuppressAPUntilCameraReady ) 
      {
      iSuppressAPUntilCameraReady = EFalse;
      }
    iPartialBackground = EFalse;
    
    if ( appUi && iController.CameraSwitchQueued() )   
        {
        appUi->HandleCommandL( ECamCmdSwitchCamera );
        }
    }
  // -------------------------------------------------------
  // Going to background
  else
    {
    if( iController.IsAppUiAvailable() ) 
        {
        iPartialBackground = ( (appUi->AppInBackground(ETrue) ) && (!appUi->AppInBackground(EFalse) ) );
        }
    else
        {
        iPartialBackground = EFalse;
        }
    PRINT( _L("Camera <> CCamPostCaptureViewBase::HandleForegroundEventL: to background") )
    iController.DeepSleepTimerCancel();
    // Register that we nolonger need the engine.
    // Checks if we incremented the count.
    DecrementCameraUsers();
    }
  // -------------------------------------------------------
  PRINT( _L("Camera <= CCamPostCaptureViewBase::HandleForegroundEventL") )
  }


// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                                                    const TDesC8& aCustomMessage )
    {    
    PRINT( _L( "Camera => CCamPostCaptureViewBase::DoActivateL" ) );    

    CCamAppUi* appui = static_cast<CCamAppUi*>( AppUi() );

    iAiwServiceHandler->AttachL(R_CAM_AIW_EDIT_INTEREST);
    
    CCamViewBase::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );

    // fixed toolbar is used only with touch devices
    if ( iController.IsTouchScreenSupported() )
        {
        CAknToolbar* fixedToolbar = Toolbar();

        if ( fixedToolbar )
            {
            if( iEmbedded )        
                {
                fixedToolbar->SetToolbarVisibility( EFalse );
                }
            else
                {
                fixedToolbar->SetToolbarObserver( this );
                UpdateToolbarIconsL();
                if( Id().iUid != ECamViewIdVideoPostCapture && !appui->IsSelfTimedCapture() && 
                    !iController.SavedCurrentImage() )
                    {
                    fixedToolbar->SetDimmed(ETrue);
                    }
                fixedToolbar->HideItemsAndDrawOnlyBackground( EFalse ); 
                fixedToolbar->SetToolbarVisibility( ETrue );
                }
            }
        }
    

    // show an empty pane
    appui->PushDefaultNaviPaneL();

    iController.AddControllerObserverL( this );

    // if we're embedded use ROID(R_CAM_SOFTKEYS_SELECT_DELETE_ID)softkey labels
    if ( iEmbedded )
        {
        //Do not show postcapture softkeys if we are still capturing or
        //saving image
        if ( iController.CurrentOperation() != ECamCapturing && 
                iController.CurrentOperation() != ECamCompleting )
            {
            if ( !appui->IsSecondCameraEnabled() || appui->IsQwerty2ndCamera() )  
                {
                SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT );
                }
            else
                {
                SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT_SECONDARY );
                }
            }
        else
            {
            SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
            }
        }
    
    iPreviousViewStillPreCapture = ( aPrevViewId.iViewUid.iUid == ECamViewIdStillPreCapture );
    
    // set flag for still and timelapse post capture only
    // also check if the file has already been saved (possible if, for 
    // example, MMC removal note is displayed
    if ( iPreviousViewStillPreCapture && appui->CurrentBurstMode() != ECamImageCaptureBurst && 
         !iController.SavedCurrentImage() )
        {
        PRINT( _L( "Camera <> CCamPostCaptureViewBase::DoActivateL .. set iWaitForImageSave true" ) );    
        iWaitForImageSave = ETrue;
        }
    else
        {
        PRINT( _L( "Camera <> CCamPostCaptureViewBase::DoActivateL .. set iWaitForImageSave false" ) );    
        iWaitForImageSave = EFalse;
        if ( !iController.IsTouchScreenSupported() )
            {
            UpdateActivePaletteItemsL();
            }
        }
    
    // Make sure appuis CBA is not visible
    CEikButtonGroupContainer* cba = AppUi()->Cba();
    if( cba )
        {
        cba->MakeVisible( EFalse );
        }

    PRINT( _L( "Camera <= CCamPostCaptureViewBase::DoActivateL" ) );    
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void 
CCamPostCaptureViewBase::DoDeactivate()
    {
    PRINT( _L( "Camera => CCamPostCaptureViewBase::DoDeactivate" ) );    

    CCamViewBase::DoDeactivate();

    iController.RemoveControllerObserver( this );
    CAknToolbar* fixedToolbar = Toolbar();
    if(fixedToolbar)
        {
        fixedToolbar->SetToolbarVisibility( EFalse );
        }
    
    PRINT( _L( "Camera <= CCamPostCaptureViewBase::DoDeactivate" ) );    
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::DynInitMenuPaneL
// Called when the menu pane is displayed
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::DynInitMenuPaneL( TInt /*aResourceId*/,
                                                CEikMenuPane* aMenuPane )
    {
    TInt itemPos = 0;
        
    if( !iController.IsTouchScreenSupported() )
        {
        if( aMenuPane->MenuItemExists(ECamCmdProperties, itemPos) )
            {
            aMenuPane->SetItemDimmed( ECamCmdProperties, ETrue );
            }
        if( aMenuPane->MenuItemExists(ECamCmdAddToAlbum, itemPos) )
            {
            aMenuPane->SetItemDimmed( ECamCmdAddToAlbum, ETrue );
            }
        }
    // SHARE_AIW: Hide menu item, if Share not available
    if ( !iOneClickUploadUtility->OneClickUploadSupported() && 
         aMenuPane->MenuItemExists( ECamCmdShareSettings, itemPos ) )
        {
        aMenuPane->SetItemDimmed( ECamCmdShareSettings, ETrue );
        }
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::CCamPostCaptureViewBase
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamPostCaptureViewBase::CCamPostCaptureViewBase( CCamAppController& aController )
    : CCamViewBase( aController ),
      iSendingCapabilities( KCamSendUiBodySize, 
                            KCamSendUiMessageSize,
                            TSendingCapabilities::ESupportsAttachments )
    {
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::ConstructL()
    {
  
    // create SendAs
    iSendAppUi = CSendUi::NewL();

    iSendMtmsToDim = new (ELeave) CArrayFixFlat<TUid>( KCamSendAsArrayGranularity );
    iSendMtmsToDim->AppendL( KSenduiMtmIrUid );

    iCollectionManagerCallBack = new (ELeave) 
                                     CCamCollectionManagerAO( *this );
    iAddToAlbumRequestOngoing = EFalse;


/*#ifndef __WINS__
    // Initialize SFIutils
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    iSFIUtils = appUi->SFIUtility();    
#endif*/

    iAiwServiceHandler = CAiwServiceHandler::NewL();
    
    // In touch screen devices, hide toolbar initially to prevent it from 
    // being briefly visible in embedded mode
    if ( iController.IsTouchScreenSupported() )
        {
        CAknToolbar* fixedToolbar = Toolbar();
        if ( fixedToolbar )
            {
            fixedToolbar->SetToolbarVisibility( EFalse );
            }
        }

    iOneClickUploadUtility = CCamOneClickUploadUtility::NewL();
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::CreateContainerL
// Create container control
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::CreateContainerL()
    {
    TRect rect;
    
    CCamAppUiBase* appUi =  static_cast<CCamAppUiBase*>( AppUi() );
	TCamOrientation orientation = appUi->CamOrientation();

    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );

    iContainer = CCamPostCaptureContainer::NewL( iController,
                                                 *this,
                                                 screen );

    iContainer->SetMopParent( this );
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::DoSendAsL
// Use SendUI to create a message containing current image\video clip
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::DoSendAsL() const
    {
    PRINT( _L("Camera => CCamPostCaptureViewBase::DoSendAsL") ) 
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    
    if( !appUi->SendAsInProgress() )
        {
        appUi->SetSendAsInProgressL( ETrue );
        CMessageData* messageData = CMessageData::NewLC();
        TRAP_IGNORE( 
            messageData->AppendAttachmentL( iController.CurrentFullFileName() );
            iSendAppUi->ShowQueryAndSendL( messageData,iSendingCapabilities ) 
            )
        PRINT( _L("Camera <> CCamPostCaptureViewBase::DoSendAsL - finished sending") )
        appUi->SetSendAsInProgressL( EFalse );        
        CleanupStack::PopAndDestroy( messageData );
        }
    
    PRINT( _L("Camera <= CCamPostCaptureViewBase::DoSendAsL") )
    }
    
#ifndef __WINS__
// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::DoInCallSendL
// Handle send to caller functionality
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::DoInCallSendL() const
    {
    PRINT( _L("Camera => CCamPostCaptureViewBase::DoInCallSendL()") ) 	
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    
    if( !appUi->SendAsInProgress() )
        {
        appUi->SetSendAsInProgressL( ETrue );
        CMessageData* messageData = CMessageData::NewLC();
        TRAP_IGNORE( 
            messageData->AppendAttachmentL( iController.CurrentFullFileName() );
            iSendAppUi->CreateAndSendMessageL( KSenduiMtmMmsUid, messageData, KNullUid, ETrue )
            )
        PRINT( _L("Camera <> CCamPostCaptureViewBase::DoInCallSendL() - finished sending") )
        appUi->SetSendAsInProgressL( EFalse );        
        CleanupStack::PopAndDestroy( messageData );
        }
            
    // SFIUtils replaced by using SendUI interface because 
    // it cannot handle > 600 kB images properly
    /*if ( iSFIUtils && iSFIUtils->IsCLIValidL() )
        {
        iSFIUtils->SendMediaFileL( iController.CurrentFullFileName() );
        }*/
    PRINT( _L("Camera <= CCamPostCaptureViewBase::DoInCallSendL()") ) 	    
    }
#endif

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::IsPostCapture
// Returns whether the view is a postcapture view
// ---------------------------------------------------------------------------
//
TBool CCamPostCaptureViewBase::IsPostCapture()
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::ResetSendAvailabilityL
// Hides or shows the 'Send' and 'SendAs' items in the active toolbar
 // ---------------------------------------------------------------------------
//   
void CCamPostCaptureViewBase::ResetSendAvailabilityL( CCamActivePaletteHandler* aApHandler )
    {
    PRINT( _L("Camera => CCamPostCaptureViewBase::ResetSendAvailabilityL") );
    // Hide Send or Send to caller
//    TBool inCall = EFalse;
/*#if !defined(__WINS__) && !defined(__WINSCW__)
    inCall = iSFIUtils->IsCLIValidL();
#endif	*/
    if ( aApHandler )
        {
        //use both inCall and InCallOrRinging() conditions to make sure if it is in call state
        
        // In-Call-Send no longer used. Always use ECamCmdSend regardless of phone call state.
        //aApHandler->SetItemVisible( ECamCmdSend, !( /*inCall &&*/ iController.InCallOrRinging() ) );
        //aApHandler->SetItemVisible( ECamCmdSendToCallerMultimedia, ( /*inCall &&*/ iController.InCallOrRinging() ) );
        aApHandler->SetItemVisible( ECamCmdSend, ETrue );
        
        TBool uploadSupported = EFalse;
        if ( iOneClickUploadUtility &&
             iOneClickUploadUtility->OneClickUploadSupported() )
            {
            uploadSupported = ETrue;
            }

        aApHandler->SetItemVisible( ECamCmdOneClickUpload, uploadSupported );
        }
    PRINT( _L("Camera <= CCamPostCaptureViewBase::ResetSendAvailabilityL ") );	
    }


// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::StartAddToAlbumOperationL
//
// Starts calls to PostCapIdleCallBack during processor idle time. The calls
// are continued until PostCapIdleCallBack is done.
//    
// ---------------------------------------------------------------------------
void CCamPostCaptureViewBase::StartAddToAlbumOperationL()
    {

    //Currently we set multiple selection of albums
    TBool   allowMultipleSelection = ETrue;
    RArray<TUint32> selectedAlbumIds;
    CleanupClosePushL( selectedAlbumIds );
    // Launching the Pop-up menu with the list of albums
    TRAPD( err, TGlxCollectionSelectionPopup::ShowPopupL(
            selectedAlbumIds,
            KGlxCollectionTypeIdAlbum,
            allowMultipleSelection,
            ETrue ) );
        /*
        * Use Collection Manager to add the albums and 
        * before that check for the completion of the harvesting
        */
        if ( err == KErrNone && selectedAlbumIds.Count() > 0)
            {
            PRINT( _L("Camera => StartAddToAlbumOperationL: Before CCamCollectionManagerAO( *this ); ") );
            PRINT( _L("Camera => StartAddToAlbumOperationL: After CCamCollectionManagerAO( *this ); and before calling StartCallBackAddToAlbumL()") );
            if ( iCollectionManagerCallBack )
                {
                if ( !iAddToAlbumRequestOngoing )
                    {
                    // Checking if only one request is being processed
                    iAddToAlbumRequestOngoing = ETrue;
                    iCollectionManagerCallBack->StartCallBackAddToAlbumL(
                                                selectedAlbumIds,
                                                iController.CurrentFullFileName() );
                    }
                }
            PRINT( _L("Camera => StartAddToAlbumOperationL: After StartCallBackAddToAlbumL() ") );
            }
        CleanupStack::PopAndDestroy( &selectedAlbumIds );
        }

    
// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::ShowAddToAlbumConfirmationQueryL
//
// Opens a radiobutton setting page with "Yes" or "No" options
// to actually confirm if we want to add the photo/video
// to the albums
//    
// --------------------------------------------------------------------------- 
TInt CCamPostCaptureViewBase::ShowAddToAlbumConfirmationQueryL()
    {
    TInt selectedIndex = 1; // default selection is "No"
        // Add to Album setting page "Items"
        CDesCArrayFlat* items = 
                   iCoeEnv->ReadDesC16ArrayResourceL( 
                                    R_CAM_ADDTOALBUM_SETTING_PAGE_LBX );
        CleanupStack::PushL( items );  
                
        CAknRadioButtonSettingPage* dlg = new (ELeave) 
                                    CAknRadioButtonSettingPage
                                          (
                                          R_CAM_ADDTOALBUM_SETTING_PAGE,
                                          selectedIndex,
                                          items 
                                          );
                
        dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged );
        CleanupStack::PopAndDestroy( items );
    return selectedIndex;
    }

/*
* CCamPostCaptureViewBase::AddToAlbumIdOperationComplete()
*/
void CCamPostCaptureViewBase::AddToAlbumIdOperationComplete()
    { 
    PRINT( _L("Camera => CCamPostCaptureViewBase::AddToAlbumIdOperationComplete() ... SUCCESS ") );
    iAddToAlbumRequestOngoing = EFalse;
    }


// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::UpdateToolbarIconsL
// Update fixed toolbar icons according to current call state
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::UpdateToolbarIconsL()
    {
    PRINT( _L("Camera => CCamPostCaptureViewBase::UpdateToolbarIconsL") );
    // fixed toolbar is used only with touch devices
    if ( iController.IsTouchScreenSupported() )
        {
        CAknToolbar* toolbar = Toolbar();
        if ( toolbar )
            {
            CAknButton* button = dynamic_cast<CAknButton*>(
                    toolbar->ControlOrNull( ECamCmdSend ) );
            if ( button )
                {
                CAknButtonState* state = button->State();
                if ( state )
                    {
                    HBufC* tooltipText = 
							StringLoader::LoadLC( R_QTN_LCAM_TT_SEND );
                    state->SetHelpTextL( *tooltipText );
                    CleanupStack::PopAndDestroy( tooltipText );    
                    }    
                }
            button = dynamic_cast<CAknButton*>(
                    toolbar->ControlOrNull( ECamCmdOneClickUpload ) );
            if ( button && iOneClickUploadUtility->OneClickUploadSupported())
                {
               CAknButtonState* state = button->State();
               if ( state )
                   {
                   TRAP_IGNORE( state->SetHelpTextL( iOneClickUploadUtility->ButtonTooltipL() ) );
                   iOneClickUploadUtility->UpdateUploadIconL( toolbar, iController.CurrentMode() );
                   }
               }
            button = dynamic_cast<CAknButton*>(toolbar->ControlOrNull( ECamCmdEdit ));
            if( button )
                {
                CAknButtonState* state = button->State();
                if( state )
                    {
                    TInt resource = (Id().iUid == ECamViewIdStillPostCapture)?
                                    R_QTN_LCAM_TT_IMAGE_EDITOR:
                                    R_QTN_LCAM_TT_VIDEO_EDITOR;
                    HBufC* helpText = StringLoader::LoadLC( resource );
                    state->SetHelpTextL(*helpText);
                    CleanupStack::PopAndDestroy(helpText);
                    }
                } 
           }
        }
    PRINT( _L("Camera <= CCamPostCaptureViewBase::UpdateToolbarIconsL") );    
    }
    
// --------------------------------------------------------------------------- 
// CCamPostCaptureViewBase::OfferToolbarEventL
// Handle fixed toolbar events
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::OfferToolbarEventL( TInt aCommand )
    {
    User::ResetInactivityTime();
    HandleCommandL( aCommand );
    }

// ---------------------------------------------------------------------------
// CCamPostCaptureViewBase::UpdateActivePaletteItemsL
// ---------------------------------------------------------------------------
//
void CCamPostCaptureViewBase::UpdateActivePaletteItemsL()
    {
    PRINT( _L("Camera => CCamPostCaptureViewBase::UpdateActivePaletteItemsL ") );
    CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );
    TInt resId = 0;
    if ( appUi )
        {
        TCamOrientation orientation = appUi->CamOrientation();
        CCamActivePaletteHandler* apHandler = appUi->APHandler();
        if( apHandler && !iEmbedded && ( orientation == ECamOrientationCamcorder ||
                            orientation == ECamOrientationCamcorderLeft ||
                            orientation == ECamOrientationDefault ||
                            orientation == ECamOrientationPortrait ) )
            {
            iContainer->CreateActivePaletteL();
            MActivePalette2UI* activePalette =
                appUi->APHandler()->ActivePalette();

            if( activePalette )
                {
                activePalette->SetGc();
                }
            // Install PostCapture AP items
            if ( ECamControllerImage == iController.CurrentMode() ||
                    iPreviousViewStillPreCapture )
                {
                PRINT( _L("Camera => ECamControllerImage == iController.CurrentMode() ") );
                if ( appUi->IsSecondCameraEnabled() )
                    {
                    resId = R_CAM_STILL_POST_CAPTURE_AP_ITEMS_SECONDARY;
                    }
                else
                    {     
                    resId = R_CAM_STILL_POST_CAPTURE_AP_ITEMS;
                    }
                }
            else
                {
                PRINT( _L("Camera => resId = R_CAM_VIDEO_POST_CAPTURE_AP_ITEMS; ") );
                if ( appUi->IsSecondCameraEnabled() )
                    {
                    resId = R_CAM_VIDEO_POST_CAPTURE_AP_ITEMS_SECONDARY;
					}
                else
				    {
                    resId = R_CAM_VIDEO_POST_CAPTURE_AP_ITEMS;
					}
                }

            if ( iOneClickUploadUtility &&
                 iOneClickUploadUtility->OneClickUploadSupported() )
                {
                apHandler->SetOneClickUploadTooltipL(
                        iOneClickUploadUtility->ButtonTooltipL() );
                }

            apHandler->InstallAPItemsL( resId );
            // Hide Send or Send to caller
            ResetSendAvailabilityL( apHandler );
            apHandler->SetView( this );

            // Make AP visible.
            appUi->SetAlwaysDrawPostCaptureCourtesyUI( ETrue );

            // if coming from background, in video as target mode
            if ( iController.CurrentMode() != iController.TargetMode() &&
                    ( ECamControllerVideo == iController.TargetMode() ) )
                {
                apHandler->SetVisibility( EFalse );
                iSuppressAPUntilCameraReady = ETrue;
                }
            }
        }
    PRINT( _L("Camera <= CCamPostCaptureViewBase::UpdateActivePaletteItemsL ") );
    }

//  End of File  
