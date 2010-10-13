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
* Description:  Still image Post-capture view class for Camera application*
*/


// INCLUDE FILES

#include "CamAppUiBase.h"
#include "CamAppController.h"
#include "CamStillPostCaptureView.h"
#include "CamPostCaptureContainer.h"
#include "CamLogger.h"
#include "CamAppUi.h"
#include <eikmenub.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <AiwCommon.hrh>
#include <akntoolbar.h>
#include <aknbutton.h>
#include "CamUtility.h"

#include "Cam.hrh"

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include <sendnorm.rsg>
#include <sendui.h>
#ifndef __WINS__
//#include <SFIUtilsAppInterface.h>

#include <aknnotewrappers.h>  // CAknInformationNote
#endif

#include "camactivepalettehandler.h"
#include "camoneclickuploadutility.h"
#include "CameraUiConfigManager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamStillPostCaptureViewTraces.h"
#endif


//CONSTANTS
_LIT( KCamImageMimeType, "image/jpeg" );

// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStillPostCaptureView* CCamStillPostCaptureView::NewLC( CCamAppController& aController )
    {
    CCamStillPostCaptureView* self = 
        new( ELeave ) CCamStillPostCaptureView( aController );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CCamStillPostCaptureView destructor
// 
// ---------------------------------------------------------------------------
//
CCamStillPostCaptureView::~CCamStillPostCaptureView()
  {
  }

// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::Id
// Returns UID of view
// ---------------------------------------------------------------------------
//
TUid CCamStillPostCaptureView::Id() const
    {
    return TUid::Uid( ECamViewIdStillPostCapture );
    }
  
// -----------------------------------------------------------------------------
// CCamStillPostCaptureView::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamStillPostCaptureView::HandleCommandL( TInt aCommand )
    {
    PRINT1( _L("Camera => CCamStillPostCaptureView::HandleCommandL (%d)"), aCommand )
    switch ( aCommand )
        {
        case EAknSoftkeySelect: 
            {
            CCamAppUiBase* appUi =  static_cast<CCamAppUiBase*>( AppUi() );    
            if ( ECamImageCaptureTimeLapse == appUi->CurrentBurstMode() )     
                {
                CCamPostCaptureViewBase::HandleCommandL( aCommand );
                }        
            // In post-capture select key is disabled when embedded 
            else if ( !iEmbedded ) 
                {
                CEikMenuBar* menuBar = MenuBar();
                if ( menuBar )
                    {
                    menuBar->SetMenuTitleResourceId( ROID(R_CAM_STILL_POST_CAPTURE_OK_MENUBAR_ID));
                    menuBar->TryDisplayMenuBarL();
                    menuBar->SetMenuTitleResourceId( ROID(R_CAM_STILL_POST_CAPTURE_MENUBAR_ID));
                    }
                CCamPostCaptureViewBase::HandleCommandL( aCommand );
                }
            else
                {
                AppUi()->HandleCommandL( ECamCmdSelect );
                }
            }
            break;
        case ECamCmdDeleteSequence:
            {
            if ( DisplayDeleteSequenceNoteL() )
                {
                HandleCommandL( ECamCmdNewPhoto );
                }
            }
            break;
        case ECamCmdSendToCallerMultimedia:
/* In-Call-Send no longer used, skip to case ECamQuickSend.
            {
#ifndef __WINS__
            DoInCallSendL();
#endif
            }
            break;
*/
    case ECamCmdQuickSend:
      {
/*#ifndef __WINS__
      if( iSFIUtils->IsCLIValidL() )
        { 
        DoInCallSendL();
        }
      else
#endif // __WINS__*/
        //{
        DoSendAsL();
        //}
      }
      break;
            
        case ECamCmdRenameImage:
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );;
            QueryRenameL( ECamControllerImage );
            // inform the Media gallery of name change
            TRAPD( ignore,
                appUi->HandleControllerEventL( ECamEventMediaFileChanged, KErrNone ) );
            if ( ignore )
                {
                // Do nothing (removes build warning)
                }           
            break;
            }

    case ECamCmdPrint:
          {
          PRINT( _L("Camera => CCamStillPostCaptureView::HandleCommandL PRINT"))

      CAiwGenericParamList& inParams  = iAiwServiceHandler->InParamListL();
            CAiwGenericParamList& outParams = iAiwServiceHandler->OutParamListL();
            // Add file path to AIW parameters
            TAiwVariant variant( iController.CurrentFullFileName() );
            TAiwGenericParam param( EGenericParamFile, variant );
            inParams.AppendL( param );

            iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdPrint, inParams, outParams );
            // we are 'embedding' Edit & Print apps
            static_cast<CCamAppUiBase*>( AppUi() ) ->SetEmbedding( ETrue );
            }
      break;
        case ECamCmdPrintPlugin:
            {
            // we are 'embedding' Edit & Print apps
            PRINT( _L("Camera => CCamStillPostCaptureView::HandleCommandL ECamCmdPrintPlugin"))
            static_cast<CCamAppUiBase*>( AppUi() ) ->SetEmbedding( ETrue );            
            }
            break;
         
        case ECamCmdPrintIntent:
            {
            // we are 'embedding' Edit & Print apps
            PRINT( _L("Camera => CCamStillPostCaptureView::HandleCommandL ECamCmdPrintIntent"))
            static_cast<CCamAppUiBase*>( AppUi() ) ->SetEmbedding( ETrue );            
            }
            break;

        default:
            {
            PRINT1( _L("Camera => CCamStillPostCaptureView::HandleCommandL default cmd (%d)"), aCommand )
            // Handle AIW commands
            TInt aiwServiceCmd = iAiwServiceHandler->ServiceCmdByMenuCmd( aCommand );
            if ( aiwServiceCmd != KAiwCmdNone )
                {
                PRINT( _L("CCamStillPostCaptureView::HandleCommandL AIW command enabling prevention of timeout") );

                CAiwGenericParamList& inParams  = iAiwServiceHandler->InParamListL();
                CAiwGenericParamList& outParams = iAiwServiceHandler->OutParamListL();
                // Add file path to AIW parameters
                TAiwVariant variant( iController.CurrentFullFileName() );
                TAiwGenericParam param( EGenericParamFile, variant );
                inParams.AppendL( param );

                TCamOrientation orientation = static_cast<CCamAppUiBase*>( AppUi() )->CamOrientation();              

                // Don't add the mime type if uploading, since Share client 
                // will figure it out from the file
                if ( aiwServiceCmd != KAiwCmdUpload ) 
                    {
                    // Set as contact call image needs image MIME type as AIW param
                    // Set as wall paper requires MIME type also.
                    TAiwVariant variant2( KCamImageMimeType );
                    TAiwGenericParam param2( EGenericParamMIMEType, variant2 );
                    inParams.AppendL( param2 );
                    }
                
                iAiwServiceHandler->ExecuteMenuCmdL( aCommand, inParams, outParams, 0, this );
                PRINT( _L("CCamStillPostCaptureView::HandleCommandL AIW command") );

                // To let camera draw the captured image after assigning an image 
                // to contact from postcapture view, don't set iEmbedding to ETrue 
                // when assign an image to contact from postcapture view.
                if (  ECamCmdEditVideo != aCommand  ) 
                    { 
                    // We are 'embedding' Edit & Print apps
                    static_cast<CCamAppUiBase*>( AppUi() ) ->SetEmbedding( ETrue );
                    }
                }
            else
                {
                CCamPostCaptureViewBase::HandleCommandL( aCommand );
                }
            }
        }
    }


// -----------------------------------------------------------------------------
// CCamStillPostCaptureView::DoActivateL
// Activates the view
// -----------------------------------------------------------------------------
//
void CCamStillPostCaptureView::DoActivateL(
        const TVwsViewId& aPreViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage )
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMSTILLPOSTCAPTUREVIEW_DOACTIVATEL, "e_CCamStillPostCaptureView_DoActivateL 1" );
    PRINT( _L("Camera => CCamStillPostCaptureView::DoActivateL"))
    PERF_EVENT_START_L2( EPerfEventStillPostCaptureViewActivation );
    CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );

    TBool timeLapseOn = (ECamImageCaptureTimeLapse == appUi->CurrentBurstMode());

    // Ensure the correct menu is used for the current mode.
    CEikMenuBar* menuBar = MenuBar();
    if ( menuBar )
        {
        if ( timeLapseOn )
            {
            menuBar->SetMenuTitleResourceId(
                    ROID(R_CAM_TIMELAPSE_POST_CAPTURE_MENUBAR_ID));
            }
        else        
            {
            menuBar->SetMenuTitleResourceId(
                    ROID(R_CAM_STILL_POST_CAPTURE_MENUBAR_ID));
            }
        }

    iAiwServiceHandler->Reset();
    CCamPostCaptureViewBase::DoActivateL(
            aPreViewId, aCustomMessageId, aCustomMessage );

    PERF_EVENT_END_L2( EPerfEventStillPostCaptureViewActivation );    

    // Attach interest for PRINT from Active Toolbar
    iAiwServiceHandler->AttachL( R_CAM_MOVE_TO_STILL_IMAGE_INTEREST_AT );

    iAiwServiceHandler->AttachMenuL( ROID(R_CAM_STILL_POST_CAPTURE_MENU_ID), 
            R_CAM_MOVE_TO_STILL_IMAGE_INTEREST );         

    // Attach share interest: toolbar item
    iAiwServiceHandler->AttachMenuL( ROID( R_CAM_STILL_POST_CAPTURE_MENU_ID),
            R_CAM_SHARE_ON_OVI_INTEREST );

    // Attach share interest: menu item
    iAiwServiceHandler->AttachMenuL( ROID( R_CAM_STILL_POST_CAPTURE_MENU_ID),
            R_CAM_AIW_VIEW_INTEREST );

    if ( iController.IntegerSettingValue(ECamSettingItemPhotoEditorSupport) )
        {    
        iAiwServiceHandler->AttachMenuL( ROID( R_CAM_STILL_POST_CAPTURE_MENU_ID), 
                R_CAM_SET_AS_CALL_IMAGE_INTEREST_EDITOR );
        }
    else
        {
        iAiwServiceHandler->AttachMenuL( ROID( R_CAM_STILL_POST_CAPTURE_MENU_ID), 
                    R_CAM_SET_AS_CALL_IMAGE_INTEREST );
        }
    
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMSTILLPOSTCAPTUREVIEW_DOACTIVATEL, "e_CCamStillPostCaptureView_DoActivateL 0" );
    }


// -----------------------------------------------------------------------------
// CCamStillPostCaptureView::DisplayDeleteNoteL()
// Display the delete confirmation note
// -----------------------------------------------------------------------------
//
TBool CCamStillPostCaptureView::DisplayDeleteNoteL()
    {
    // Display the delete confirmation note
    TInt err = KErrNone;
    HBufC* confirmationText;
    confirmationText = StringLoader::LoadLC( R_CAM_STILL_POST_CAPTURE_DELETE_NOTE_TEXT );
    CAknQueryDialog* confirmationDialog = new( ELeave )CAknQueryDialog( *confirmationText );
    CleanupStack::PopAndDestroy( confirmationText );
    if ( confirmationDialog->ExecuteLD( R_CAM_STILL_POST_CAPTURE_DELETE_NOTE ) )
        {
        if ( iEmbedded )
            {
            //To prevent sofkeys blinking when capturing, we need to blank
            //the softkeys here
            BlankSoftkeysL();
            Cba()->DrawNow();
            }
        err = iController.DeleteCurrentFile();
        if ( err ) 
            {
            User::Leave( err );
            }
        return ETrue;
        }                
    else
        {
        return EFalse;
        }
    }
    
// -----------------------------------------------------------------------------
// CCamStillPostCaptureView::DisplayDeleteSequenceNoteL()
// Display the delete confirmation note for a sequence of images
// -----------------------------------------------------------------------------
//
TBool CCamStillPostCaptureView::DisplayDeleteSequenceNoteL()
    {
    // Display the delete sequence confirmation note
    HBufC* confirmationText;

    confirmationText = StringLoader::LoadLC( R_CAM_BURST_DELETE_MULTIPLE_NOTE_TEXT, iController.TimeLapseImageCount() );    
    
    // Create the dialog with the text, and show it to the user
    CAknQueryDialog* confirmationDialog = new( ELeave )CAknQueryDialog( *confirmationText );
    CleanupStack::PopAndDestroy( confirmationText );

    if ( confirmationDialog->ExecuteLD( R_CAM_BURST_DELETE_MULTIPLE_NOTE ) )
        {
        iController.DeleteTimeLapseFiles(); 
        return ETrue;
        }                
    else
        {
        return EFalse;
        }     
    }    

// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::CCamStillPostCaptureView
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamStillPostCaptureView::CCamStillPostCaptureView( CCamAppController& aController )
    : CCamPostCaptureViewBase( aController )
    {
    }

// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamStillPostCaptureView::ConstructL()
    {
    BaseConstructL( ROID(R_CAM_STILL_POST_CAPTURE_VIEW_ID));
    CCamPostCaptureViewBase::ConstructL();
  
	iRockerKeyPress = EFalse;
    }


// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CCamStillPostCaptureView::DoDeactivate()
    {
    CCamPostCaptureViewBase::DoDeactivate();
    }

// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::SetTitlePaneTextL
// Set the view's title text
// ---------------------------------------------------------------------------
//
void CCamStillPostCaptureView::SetTitlePaneTextL()
    {   
    TBool titleAlwaysShowsFileName = EFalse;
  
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() );
    if ( appUi->CamOrientation() == ECamOrientationViewMode ||
              titleAlwaysShowsFileName )
        {
        appUi->SetTitleL( iController.CurrentImageName() );
        }
    }


// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::ProcessCommandL
// Handling ECamCmdToggleActiveToolbar from any of the PostCaptureView's
// either Image/Video. 
// ---------------------------------------------------------------------------
//
void CCamStillPostCaptureView::ProcessCommandL( TInt aCommand )
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    TCamOrientation orientation = appUi->CamOrientation();
    if( orientation == ECamOrientationCamcorder || 
        orientation == ECamOrientationCamcorderLeft ||
        orientation == ECamOrientationPortrait )
        {
        if ( aCommand == ECamCmdToggleActiveToolbar )
            {
            CCamPostCaptureViewBase::HandleCommandL( aCommand );
            }
         else
            {
			CAknView::ProcessCommandL( aCommand );	            
            }
        }
    else 
        {
        if ( aCommand == EAknSoftkeyContextOptions && !iRockerKeyPress )
            {
            iRockerKeyPress = ETrue;
            MenuBar()->SetContextMenuTitleResourceId( ROID(R_CAM_STILL_POST_CAPTURE_MENUBAR_ID) );
            // Here we launch the Context Options by Hiding Help and Exit
            // Check DynInitMenuPaneL() method and search for iRockerKeyPress for more details
            MenuBar()->SetMenuType( CEikMenuBar::EMenuContext );
            MenuBar()->TryDisplayMenuBarL();
            // Here we again set back the type of menu to "Options" when pressed LSK
            MenuBar()->SetMenuType( CEikMenuBar::EMenuOptions );
            }
        else
            {
            CAknView::ProcessCommandL( aCommand );
            }
        }
    // CALL THE BASE CLASS
    
    }
    
// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::DynInitMenuPaneL
// Changes MenuPane dynamically
// ---------------------------------------------------------------------------
//
void CCamStillPostCaptureView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    CCamPostCaptureViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );
  
    if ( iAiwServiceHandler && iAiwServiceHandler->IsAiwMenu( aResourceId ) )
        {
        CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
        TAiwVariant variant( iController.CurrentFullFileName() );
        TAiwGenericParam param( EGenericParamFile, variant );
        paramList.AppendL( param );

        // Set as contact call image needs image MIME type as AIW param
        TCamOrientation orientation = static_cast<CCamAppUiBase*>( AppUi() )->CamOrientation();              

        TAiwVariant variant2(  KCamImageMimeType );
        TAiwGenericParam param2( EGenericParamMIMEType, variant2 );
        paramList.AppendL( param2 );

        iAiwServiceHandler->InitializeMenuPaneL(
            *aMenuPane, 
            aResourceId, 
            ECamCmdAIWCommands, 
            paramList );
        }

    if ( iAiwServiceHandler )
        {
        // handle any AIW menu cascades
        if ( iAiwServiceHandler->HandleSubmenuL( *aMenuPane ) )
            {
            return;
            }
        }

    if ( aResourceId == ROID( R_CAM_STILL_POST_CAPTURE_MENU_ID ) ||
         aResourceId == ROID( R_CAM_STILL_POST_CAPTURE_OK_MENU_ID ) )
        {
        TBool showSend = ETrue;
        TBool showSendToCaller = EFalse;

/*#ifndef __WINS__
        if ( iSFIUtils->IsCLIValidL() )
            {
            showSend = EFalse;
            showSendToCaller = ETrue;
            }
#endif*/

        TCamOrientation orientation =
            static_cast<CCamAppUiBase*>( AppUi() )->CamOrientation();

        if ( orientation == ECamOrientationCamcorder || 
            orientation == ECamOrientationCamcorderLeft ||
			orientation == ECamOrientationPortrait )
            {
            showSend = EFalse;
            showSendToCaller = EFalse;
            }

        TInt itemPos = 0;
        if ( aMenuPane->MenuItemExists( ECamCmdSendToCallerMultimedia, itemPos ) )
            {
            aMenuPane->SetItemDimmed(
                ECamCmdSendToCallerMultimedia, !showSendToCaller );
            }
        
        if(iController.IntegerSettingValue(ECamSettingItemPhotoEditorSupport))
            {
            showSend = ETrue;
            }
        
        if ( aMenuPane->MenuItemExists( ECamCmdSend, itemPos ) )
            {
            aMenuPane->SetItemDimmed(
                ECamCmdSend, !showSend );
            }

        /*
         * MSK : ContextOptions --> We just hide Help and Exit from the Options Menu when
         *       the MSK is pressed in the postcapture still view
         *       iRockerKeyPress represents MSK key event in still postcapture view
         */
        if ( aMenuPane->MenuItemExists( EAknCmdHelp, itemPos ) &&
             aMenuPane->MenuItemExists( ECamCmdInternalExit, itemPos ) )
            {
            if ( iRockerKeyPress )
                { // We hide Help and Exit
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                aMenuPane->SetItemDimmed( ECamCmdInternalExit, ETrue );
                iRockerKeyPress = EFalse;
                }
            else
                { // We show Help and Exit
                aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
                aMenuPane->SetItemDimmed( ECamCmdInternalExit, EFalse );	
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::HandleNotifyL
// Handles any notification caused by asynchronous ExecuteCommandL
// or event.
// ---------------------------------------------------------------------------
//
TInt CCamStillPostCaptureView::HandleNotifyL(
    TInt /*aCmdId*/,
    TInt aEventId,
    CAiwGenericParamList& /*aEventParamList*/,
    const CAiwGenericParamList& /*aInParamList*/ )
    {
    //AIW fails to assign. eg. the contact is locked for being used now.    
    if ( aEventId == KAiwEventError && iController.IsAppUiAvailable() )
      {
      CCamAppUi* appUi =  static_cast<CCamAppUi*>( AppUi() );
      TRAP_IGNORE( appUi->HandleCameraErrorL( KErrInUse ) );
      }    
    return 0;
    }
    

// ---------------------------------------------------------------------------
// CCamStillPostCaptureView::DynInitToolbarL
// Dynamically initialize toolbar contents
// ---------------------------------------------------------------------------
//
void CCamStillPostCaptureView::DynInitToolbarL( TInt aResourceId, 
                                       CAknToolbar* aToolbar )
    {
    PRINT2( _L("Camera => CCamStillPostCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar );
    (void)aResourceId; //remove compiler warning

    if( aToolbar && iController.IsTouchScreenSupported() )
        {
        // HideItem will not do anything if a button for the given
        // command ID is not found.
		if( iEmbedded )
			{
			aToolbar->HideItem( ECamCmdSend, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdEdit, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdPhotos, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdOneClickUpload, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdDelete, ETrue, EFalse );
			}
		else
			{
            if(iController.IntegerSettingValue(ECamSettingItemPhotoEditorSupport))
                {
                aToolbar->RemoveItem( ECamCmdSend );
                CAknButton* editButton = dynamic_cast<CAknButton*>(aToolbar->ControlOrNull( ECamCmdEdit ));
                if( editButton )
                    {
                    CAknButtonState* state = editButton->State();
                    if( state )
                        {
                        HBufC* helpText = StringLoader::LoadLC( R_QTN_LCAM_TT_IMAGE_EDITOR );
                        state->SetHelpTextL(*helpText);
                        CleanupStack::PopAndDestroy(helpText);
                        }
                    }
                }
            else
                {
                aToolbar->RemoveItem( ECamCmdEdit );
                }
            
            if(iOneClickUploadUtility->OneClickUploadSupported())
                {
                aToolbar->RemoveItem( ECamCmdPhotos );
                }
            else
                {
                aToolbar->RemoveItem( ECamCmdOneClickUpload );
                }
			}
        }
    
    PRINT2( _L("Camera <= CCamStillPostCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar );
    }

//  End of File  
