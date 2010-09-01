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
* Description:  Video Post-capture view class for Camera application*
*/


// INCLUDE FILES

#include <eikmenub.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <AiwCommon.hrh>
#include <sendnorm.rsg>
#include <sendui.h>
#include <akntoolbar.h>
#include <aknbutton.h>
#ifndef __WINS__
  //#include <SFIUtilsAppInterface.h>
  #include <aknnotewrappers.h>  // CAknInformationNote
#endif

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamAppUiBase.h"
#include "CamAppController.h"
#include "CamVideoPostCaptureView.h"
#include "CamPostCaptureContainer.h"
#include "CamLogger.h"
#include "CamAppUi.h"
#include "CamAppUiBase.h"
#include "CamUtility.h"
#include "Cam.hrh"
#include "camactivepalettehandler.h"
#include "camoneclickuploadutility.h"
#include "CameraUiConfigManager.h"
#include "CamTimer.h"


//CONSTANTS
const TInt KHdmiTimeout = 1000000; //1 sec

// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamVideoPostCaptureView* CCamVideoPostCaptureView::NewLC( CCamAppController& aController )
    {
    CCamVideoPostCaptureView* self = 
        new( ELeave ) CCamVideoPostCaptureView( aController );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView destructor
// 
// ---------------------------------------------------------------------------
//
CCamVideoPostCaptureView::~CCamVideoPostCaptureView()
    {
    if( iHdmiTimer )
        {
        delete iHdmiTimer;
        iHdmiTimer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::Id
// Returns UID of view
// ---------------------------------------------------------------------------
//
TUid CCamVideoPostCaptureView::Id() const
    {
    return TUid::Uid( ECamViewIdVideoPostCapture );
    }
  
// -----------------------------------------------------------------------------
// CCamVideoPostCaptureView::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case ECamCmdRenameVideo:
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
            QueryRenameL( ECamControllerVideo );

            // inform the Media gallery of name change
            TRAPD( ignore,
                appUi->HandleControllerEventL( ECamEventMediaFileChanged, KErrNone ) );
            if ( ignore )
                {
                // Do nothing (removes build warning)
                }
            break;
            }
        case EAknSoftkeySelect: 
            {
            // In post-capture select key is disabled when embedded 
            if ( !iEmbedded )
                {
                CEikMenuBar* menuBar = MenuBar();
                if ( menuBar )
                    {
                    menuBar->SetMenuTitleResourceId( ROID(R_CAM_VIDEO_POST_CAPTURE_OK_MENUBAR_ID));
                    menuBar->TryDisplayMenuBarL();
                    menuBar->SetMenuTitleResourceId( ROID(R_CAM_VIDEO_POST_CAPTURE_MENUBAR_ID));
                    }
                CCamPostCaptureViewBase::HandleCommandL( aCommand );
                }
            else
                {
                AppUi()->HandleCommandL( ECamCmdSelect );
                }
            }
            break;
        case ECamCmdSend:
        case ECamCmdSendToCallerMultimedia:
        case ECamCmdQuickSend:
            {
            CCamPostCaptureViewBase::HandleCommandL( aCommand );
            }
            break;
        default:
            {
            PRINT1( _L("Camera => CCamVideoPostCaptureView::HandleCommandL default cmd (%d)"), aCommand )
            // Handle AIW commands
            TInt aiwServiceCmd = iAiwServiceHandler->ServiceCmdByMenuCmd( aCommand );
            if ( aiwServiceCmd != KAiwCmdNone )
                {
                PRINT1( _L("Camera => CCamVideoPostCaptureView::HandleCommandL default handle AIW cmd (%d)" ), aiwServiceCmd)
                CAiwGenericParamList& inParams  = iAiwServiceHandler->InParamListL();
                CAiwGenericParamList& outParams = iAiwServiceHandler->OutParamListL();
                // Add file path to AIW parameters
                TAiwVariant variant( iController.CurrentFullFileName() );
                TAiwGenericParam param( EGenericParamFile, variant );
                inParams.AppendL( param );
                
                // Don't add the mime type if uploading, since Share client 
                // will figure it out from the file
                if ( aiwServiceCmd != KAiwCmdUpload ) 
                    {
                    TAiwGenericParam param2( EGenericParamMIMEType, _L("video/*") );
                    inParams.AppendL( param2 );
                    }

                iAiwServiceHandler->ExecuteMenuCmdL( aCommand, inParams, outParams, 0, this );
                // we are 'embedding' Edit app
                static_cast<CCamAppUiBase*>( AppUi() )->SetEmbedding( ETrue );
                }
            else
                {
                CCamPostCaptureViewBase::HandleCommandL( aCommand );
                }
            
            }
        }
    }

// -----------------------------------------------------------------------------
// CCamVideoPostCaptureView::DisplayDeleteNoteL()
// Delete the current file
// -----------------------------------------------------------------------------
//
TBool CCamVideoPostCaptureView::DisplayDeleteNoteL()
    {
    // Display the delete confirmation note
    TInt err = KErrNone;
    HBufC* confirmationText;
    confirmationText = StringLoader::LoadLC( R_CAM_VIDEO_POST_CAPTURE_DELETE_NOTE_TEXT );
    CAknQueryDialog* confirmationDialog = 
                            new( ELeave )CAknQueryDialog( *confirmationText );
    CleanupStack::PopAndDestroy( confirmationText );
    if ( confirmationDialog->ExecuteLD( R_CAM_VIDEO_POST_CAPTURE_DELETE_NOTE ) )
        {
        if ( iEmbedded )
            {
            BlankSoftkeysL();
            Cba()->DrawNow();
            SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT );
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
        if ( iEmbedded )
            {
            BlankSoftkeysL();
            Cba()->DrawNow();
            SetSoftKeysL( R_CAM_SOFTKEYS_SELECT_DELETE__SELECT );
            }
        return EFalse;
        }
    }
    
// -----------------------------------------------------------------------------
// CCamVideoPostCaptureView::DoActivateL()
// Activate this view
// -----------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::DoActivateL(
        const TVwsViewId& aPreViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage )
    {
    // Ensure the correct menu is used for the current mode.
    CEikMenuBar* menuBar = MenuBar();
    if ( menuBar )
        {
        menuBar->SetMenuTitleResourceId(
                ROID( R_CAM_VIDEO_POST_CAPTURE_MENUBAR_ID ) );
        }

    CCamAppUi* appUi = static_cast<CCamAppUi*>( AppUi() );
    /*if ( iController.IsTouchScreenSupported() )
        {
        if ( appUi->IsSecondCameraEnabled() )
            {
            if ( iOneClickUploadUtility->OneClickUploadSupported() )
                {
                TInt resourceId = appUi->IsQwerty2ndCamera()? 
                              R_CAM_VIDEO_POSTCAPTURE_TOOLBAR_LANDSCAPE_UPLOAD:
                              R_CAM_VIDEO_POSTCAPTURE_TOOLBAR_PORTRAIT_UPLOAD; 
                CreateAndSetToolbarL( resourceId );
                }
            else
                {
                TInt resourceId = appUi->IsQwerty2ndCamera()? 
                              R_CAM_VIDEO_POSTCAPTURE_TOOLBAR_LANDSCAPE:
                              R_CAM_VIDEO_POSTCAPTURE_TOOLBAR_PORTRAIT; 
                CreateAndSetToolbarL( resourceId );
                }
            }
        else
            {
            if ( iOneClickUploadUtility->OneClickUploadSupported() )
                {
                CreateAndSetToolbarL( R_CAM_VIDEO_POSTCAPTURE_TOOLBAR_UPLOAD );
                }
            else
                {
                CreateAndSetToolbarL( R_CAM_VIDEO_POSTCAPTURE_TOOLBAR );
                }
            }
        }*/

    iAiwServiceHandler->Reset();
    iAiwServiceHandler->AttachMenuL( ROID( R_CAM_VIDEO_POST_CAPTURE_MENU_ID ),
            R_CAM_SHARE_ON_OVI_INTEREST );

    // SHARE_AIW
    iAiwServiceHandler->AttachMenuL( ROID( R_CAM_VIDEO_POST_CAPTURE_MENU_ID),
                                     R_CAM_AIW_VIEW_INTEREST );

    CCamPostCaptureViewBase::DoActivateL(
            aPreViewId, aCustomMessageId, aCustomMessage );

    if ( !iController.IsTouchScreenSupported() )
        {
        TCamOrientation orientation = appUi->CamOrientation();

        CCamActivePaletteHandler* apHandler = appUi->APHandler();
        if( !iEmbedded && ( orientation == ECamOrientationCamcorder ||
                orientation == ECamOrientationCamcorderLeft ) )
            {
            iContainer->CreateActivePaletteL();
            MActivePalette2UI* activePalette =
            appUi->APHandler()->ActivePalette();

            if(activePalette)
                {
                activePalette->SetGc();
                }        

            // install still pre-capture items     
            apHandler->InstallAPItemsL( R_CAM_VIDEO_POST_CAPTURE_AP_ITEMS );

            // Hide Send or Send to caller
            ResetSendAvailabilityL( apHandler );
            apHandler->SetView( this );

            // Make AP visible
            appUi->SetAlwaysDrawPostCaptureCourtesyUI( ETrue );
            }
        else
            {   
            }
        }
    if ( iController.IntegerSettingValue(ECamSettingItemVideoEditorSupport) != ECamNoEditorSupport )
        {
        iAiwServiceHandler->AttachMenuL( ROID( R_CAM_VIDEO_POST_CAPTURE_MENU_ID ), 
                R_CAM_SET_AS_RING_TONE_INTEREST_EDITOR );
        }
    else
        {
        iAiwServiceHandler->AttachMenuL( ROID( R_CAM_VIDEO_POST_CAPTURE_MENU_ID ), 
                R_CAM_SET_AS_RING_TONE_INTEREST );
        }
    if( iHdmiTimer->IsActive() )
        iHdmiTimer->Cancel();
    iHdmiTimer->StartTimer();
    }

// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::CCamVideoPostCaptureView
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamVideoPostCaptureView::CCamVideoPostCaptureView( CCamAppController& aController )
    : CCamPostCaptureViewBase( aController )
    {
    }

// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::ConstructL()
    {
    PRINT( _L("Camera => CCamVideoPostCaptureView::ConstructL"))
    BaseConstructL( ROID(R_CAM_VIDEO_POST_CAPTURE_VIEW_ID));
    CCamPostCaptureViewBase::ConstructL();
    iHdmiTimer = CCamTimer::NewL( KHdmiTimeout, TCallBack(HdmiTimerCallback, this));
    PRINT( _L("Camera <= CCamVideoPostCaptureView::ConstructL"))
    }

// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::DoDeactivate()
    {
    CCamPostCaptureViewBase::DoDeactivate();
    }

// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::SetTitlePaneTextL
// Set the view's title text
// ---------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::SetTitlePaneTextL()
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
// CCamVideoPostCaptureView::ProcessCommandL
// Handling ECamCmdToggleActiveToolbar from any of the PostCaptureView's
// either Image/Video. 
// ---------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::ProcessCommandL( TInt aCommand )
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
        if ( aCommand == EAknSoftkeyContextOptions )
            {
            iRockerKeyPress = ETrue;
            MenuBar()->SetContextMenuTitleResourceId( 
            				ROID(R_CAM_VIDEO_POST_CAPTURE_MENU_ID ) );
            MenuBar()->SetMenuType( CEikMenuBar::EMenuContext );
            MenuBar()->TryDisplayMenuBarL();
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
// CCamVideoPostCaptureView::DynInitMenuPaneL
// Changes MenuPane dynamically
// ---------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    PRINT( _L("Camera => CCamVideoPostCaptureView::DynInitMenuPaneL"))
    
    if ( iAiwServiceHandler )
    	{
    	PRINT( _L("Camera => CCamVideoPostCaptureView::DynInitMenuPaneL aiw"))

    	// let AIW handle it's own submenus
    	if ( iAiwServiceHandler->HandleSubmenuL( *aMenuPane ) )
    		{
    		PRINT( _L("Camera => CCamVideoPostCaptureView::DynInitMenuPaneL aiw handle sub"))
    		return;
    		}
    	}
    
    CCamPostCaptureViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );
    
    PRINT( _L("Camera => CCamVideoPostCaptureView::DynInitMenuPaneL A"))
    if ( iAiwServiceHandler && iAiwServiceHandler->IsAiwMenu( aResourceId ) )
        {
        PRINT1( _L("Camera => CCamVideoPostCaptureView::DynInitMenuPaneL res ID (%d)"),aResourceId )
        CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
        TAiwVariant variant( iController.CurrentFullFileName() );
        TAiwGenericParam param( EGenericParamFile, variant );
        paramList.AppendL( param );
        
        TAiwVariant variant2(  _L("video/*") );
        TAiwGenericParam param2( EGenericParamMIMEType, variant2 );
        paramList.AppendL( param2 );
        
        PRINT( _L("Camera => CCamVideoPostCaptureView::DynInitMenuPaneL C"))
        iAiwServiceHandler->InitializeMenuPaneL(
            *aMenuPane, 
            aResourceId, 
            ECamCmdAIWCommands, 
            paramList );
            
        PRINT( _L("Camera => CCamVideoPostCaptureView::DynInitMenuPaneL D"))     
        }

    TInt itemPos(0);

    if ( aResourceId == ROID( R_CAM_VIDEO_POST_CAPTURE_MENU_ID ) ||
         aResourceId == ROID( R_CAM_VIDEO_POST_CAPTURE_OK_MENU_ID ) )
        {

        if( aMenuPane->MenuItemExists( ECamCmdSendToCallerMultimedia, itemPos ) )
            {
            aMenuPane->SetItemDimmed(
                ECamCmdSendToCallerMultimedia, ETrue );
            }
		TInt editorSupport = iController.IntegerSettingValue(ECamSettingItemPhotoEditorSupport);
		
		if( editorSupport == ECamNoEditorSupport || 
			editorSupport == ECamEditorSupportInOptions )
            {
			if ( aMenuPane->MenuItemExists( ECamCmdSend, itemPos ) )
				{
				aMenuPane->SetItemDimmed(
					ECamCmdSend, ETrue );
				}
			}
		if( editorSupport == ECamNoEditorSupport || 
			editorSupport == ECamEditorSupportInToolbar )
			{
			if ( aMenuPane->MenuItemExists( ECamCmdEditPhoto, itemPos ) )
				{
				aMenuPane->SetItemDimmed(
					ECamCmdEditPhoto, ETrue );
				}			
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
// CCamVideoPostCaptureView::HandleNotifyL
// Handles any notification caused by asynchronous ExecuteCommandL
// or event.
// ---------------------------------------------------------------------------
//
TInt CCamVideoPostCaptureView::HandleNotifyL(
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
// CCamVideoPostCaptureView::DynInitToolbarL
// Dynamically initialize toolbar contents
// ---------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::DynInitToolbarL( TInt aResourceId, 
                                                CAknToolbar* aToolbar )
    {
    PRINT2( _L("Camera => CCamVideoPostCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar );
    (void)aResourceId; // remove compiler warning

    // fixed toolbar is used only with touch devices
    if ( iController.IsTouchScreenSupported() && aToolbar )
        {
        // HideItem will not do anything if a button for the given
        // command ID is not found.
		if( iEmbedded )
			{
			aToolbar->HideItem( ECamCmdSend, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdEdit, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdDelete, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdOneClickUpload, ETrue, EFalse );
			aToolbar->HideItem( ECamCmdPlay, ETrue, EFalse );
			}
        else
			{
			TInt editorSupport = iController.IntegerSettingValue(ECamSettingItemPhotoEditorSupport);
            if( editorSupport == ECamEditorSupportInToolbar )
                {
                aToolbar->RemoveItem( ECamCmdSend );
                CAknButton* editButton = dynamic_cast<CAknButton*>(aToolbar->ControlOrNull( ECamCmdEdit ));
                if( editButton )
                    {
                    CAknButtonState* state = editButton->State();
                    if( state )
                        {
                        HBufC* helpText = StringLoader::LoadLC( R_QTN_LCAM_TT_VIDEO_EDITOR );
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

    PRINT2( _L("Camera <= CCamVideoPostCaptureView::DynInitToolbarL(%d, 0x%X)" ), aResourceId, aToolbar );
    }
	
// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::HdmiTimerCallback
// ---------------------------------------------------------------------------
//
TInt CCamVideoPostCaptureView::HdmiTimerCallback( TAny* aSelf )
    {
    CCamVideoPostCaptureView* self = static_cast<CCamVideoPostCaptureView*>(aSelf);
    TInt err(0);
    if( self )
        {
        TRAP(err, self->DoHdmiTimerCallbackL() );
        }
    PRINT1( _L("Camera <> CCamVideoPostCaptureView::HdmiTimerCallback err=%d"), err);
    return err;
    }

// ---------------------------------------------------------------------------
// CCamVideoPostCaptureView::DoHdmlTimerCallbackL
// ---------------------------------------------------------------------------
//
void CCamVideoPostCaptureView::DoHdmiTimerCallbackL()
    {
    iController.HandlePostHdmiConnectDuringRecordingEventL();
    }

    
//  End of File  
