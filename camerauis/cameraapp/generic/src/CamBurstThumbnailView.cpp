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
* Description:  View class to display a thumbnail grid after burst capture*
*/


// INCLUDE FILES

#include <eikmenub.h>
#include <StringLoader.h>       
#include <AknQueryDialog.h>
#include <TSendingCapabilities.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <sendnorm.rsg>
#include <sendui.h>
#include <SenduiMtmUids.h>
#include <CMessageData.h>
#include <aknlayoutscalable_apps.cdl.h> // AknLayoutScalable_Apps
//#include <SFIUtilsAppInterface.h>
#include <aknnotewrappers.h>  // CAknInformationNote
#include <akntoolbar.h> // CAknToolbar
#include <aknbutton.h>
#include <eiksoftkeypostingtransparency.h> // EikSoftkeyPostingTransparency

#include "CamAppUi.h"
#include "CamUtility.h"
//#include "camproductvariant.h"
#include "CamBurstThumbnailView.h"
#include "CamAppUiBase.h"
#include "CamBurstThumbnailContainer.h"
#include "Cam.hrh"
#include "CamBurstThumbnailGridModel.h"
#include "CamBurstThumbnailGrid.h"
#include "CamBurstCaptureArray.h"
#include "camoneclickuploadutility.h"
#include "CameraUiConfigManager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamBurstThumbnailViewTraces.h"
#endif



//CONSTANTS
_LIT( KCamImageMimeType, "image/jpeg" );

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamBurstThumbnailView* CCamBurstThumbnailView::NewLC( CCamAppController& aController )
    {
    CCamBurstThumbnailView* self = 
        new( ELeave ) CCamBurstThumbnailView( aController );

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView destructor
// 
// ---------------------------------------------------------------------------
//
CCamBurstThumbnailView::~CCamBurstThumbnailView()
  {
  PRINT( _L("Camera => ~CCamBurstThumbnailView") );
  // Free some space by deleting the model
  delete iGridModel;

  // NOTE: iAiwServiceHandler owned and deleted by base class
  PRINT( _L("Camera <= ~CCamBurstThumbnailView") );
  }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::Id
// Returns UID of view
// ---------------------------------------------------------------------------
//
TUid CCamBurstThumbnailView::Id() const
    {
    return TUid::Uid( ECamViewIdBurstThumbnail );
    }


// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::DynInitMenuPaneL
// Changes MenuPane dynamically
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    PRINT( _L("Camera => CCamBurstThumbnailView::DynInitMenuPaneL"))
       
    // initializes the aiw service parameters for aiw commands
    DynInitAiwMenuPaneL( aResourceId, aMenuPane );
                
    if ( iAiwServiceHandler )
        {
        // handle any AIW menu cascades
        if ( iAiwServiceHandler->HandleSubmenuL( *aMenuPane ) )
            {
            return;
            }
        }

    if ( aResourceId == R_CAM_BURST_THUMBNAIL_MENU )
        {
        // If there are >1 images marked remove the 
        // open image item.
        if ( iGridModel->NoOfMarkedImages() > 1 )
            {
            aMenuPane->SetItemDimmed( ECamCmdOpenPhoto, ETrue );
            TInt itemPos = 0;
            if( aMenuPane->MenuItemExists( KAiwCmdUpload, itemPos ) )
                {
                PRINT( _L("Camera => CCamBurstThumbnailView::DynInitMenuPaneL SetItemDimmed: KAiwCmdUpload"))
                aMenuPane->SetItemDimmed(
                    KAiwCmdUpload, ETrue );
                }
            if( aMenuPane->MenuItemExists( ECamCmdShareOnOvi, itemPos ) )
                {
                PRINT( _L("Camera => CCamBurstThumbnailView::DynInitMenuPaneL SetItemDimmed: ECamCmdShareOnOvi"))
                aMenuPane->SetItemDimmed(
                    ECamCmdShareOnOvi, ETrue );
                }
            }
        
        // In touch ui the delete option is available on the fixed toolbar
        // there is no need to display it in the options menu
        if ( iController.IsTouchScreenSupported() )
            {
            TInt itemPos = 0;
            if ( aMenuPane->MenuItemExists( ECamCmdDelete, itemPos ) )
                {
                aMenuPane->SetItemDimmed( ECamCmdDelete, ETrue );
                }
            }
        } // ends: if ( aResourceId == R_CAM_BURST_THUMBNAIL_MENU )
    
 
    // ------------------------------------------------------------------
    if ( aResourceId == R_CAM_BURST_THUMBNAIL_MENU ||
         aResourceId == R_CAM_BURST_THUMBNAIL_OK_MENU )
        {
        TBool showSend = ETrue;
        TBool showSendToCaller = EFalse;

/*#ifndef __WINS__
        if ( iSFIUtils->IsCLIValidL() )
            {
            showSend = EFalse;
            showSendToCaller = ETrue;

            if ( iGridModel->NoOfMarkedImages() > 1 )
                {
                // Send to caller is disabled if multiple images
                // have been marked.
                showSendToCaller = EFalse;
                }
            }
#endif*/

        if ( iController.IsTouchScreenSupported() &&
             !iOneClickUploadUtility->OneClickUploadSupported() &&
             aResourceId == R_CAM_BURST_THUMBNAIL_MENU )
            {
            // In touch UI, when one click upload is not supported,
            // there will be a Send button in the fixed toolbar. Options
            // menu items are not needed. Send is kept in the context menu.
            showSend = EFalse;
            showSendToCaller = EFalse;
            }

        TInt itemPos = 0;
        if( aMenuPane->MenuItemExists( ECamCmdSendToCallerMultimedia, itemPos ) )
            {
            aMenuPane->SetItemDimmed(
                ECamCmdSendToCallerMultimedia, !showSendToCaller );
            }

        if( aMenuPane->MenuItemExists( ECamCmdSend, itemPos ) )
            {
            aMenuPane->SetItemDimmed(
                ECamCmdSend, !showSend );
            }
        }

    if ( aResourceId == R_AVKON_MENUPANE_MARKABLE_LIST_IMPLEMENTATION )
        {
        if ( iGridModel->IsMarkedL( iGridModel->HighlightedGridIndex() ) )
            {
            // If marked, hide the MARK option
            aMenuPane->SetItemDimmed( EAknCmdMark, ETrue );
            }
        else
            {
            // If unmarked, hide the UNMARK option
            aMenuPane->SetItemDimmed( EAknCmdUnmark, ETrue );        
            }
        
        if ( iGridModel->NoOfMarkedImages() == 0 )
            {
            // If NO marked cells, hide the UNMARK ALL option
            aMenuPane->SetItemDimmed( EAknUnmarkAll, ETrue );        
            }

        if ( iGridModel->NoOfMarkedImages() == iGridModel->NoOfValidCells() )
            {
            // If ALL cells are marked, hide the MARKALL option
            aMenuPane->SetItemDimmed( EAknMarkAll, ETrue ); 
            }    
        }    

    PRINT( _L("Camera <= CCamBurstThumbnailView::DynInitMenuPaneL"))
    }   

// -----------------------------------------------------------------------------
// CCamBurstThumbnailView::HandleCommandL
// Handle commands
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailView::HandleCommandL( TInt aCommand )
    {
    
    PRINT1( _L("Camera => CCamBurstThumbnailView::HandleCommandL (aCommand: %d)"), aCommand )
    
    // sometimes commands come even after DoDeactivate() has been called.
    // this makes sure that those are ignored.
    if ( iGridModel == NULL )
        {
        return;
        }
    
    switch ( aCommand )
        {
        case ECamCmdOneClickUpload:
            {
            TInt markedItems = iGridModel->NoOfMarkedImages();
            // If there are no images marked use the highlighted image
            // set it now for safety just in case no marked images are found
            iController.SetAsCurrentImage(
                    iGridModel->HighlightedBurstIndex() );

            if ( markedItems > 0 )
                {
                // Upload marked images

                CDesC16ArrayFlat* fileNames =
                    new ( ELeave ) CDesC16ArrayFlat( markedItems );
                CleanupStack::PushL( fileNames );

                for ( TInt i = 0; i < iGridModel->NoOfImages(); i++ )
                    {
                    if ( iGridModel->IsMarkedL( i ) )
                        {
                        fileNames->AppendL( iGridModel->ImageFileName( i ) );
                        }
                    }
                iOneClickUploadUtility->UploadL(
                        *fileNames );
                CleanupStack::PopAndDestroy( fileNames );
                }
            else
                {
                // Upload highlighted image
                iOneClickUploadUtility->UploadL(
                        iController.CurrentFullFileName() );
                }
            break;
            }
        case ECamCmdRenameImage:
            {
            iController.SetAsCurrentImage( 
                iGridModel->ConvertFromValidToGlobalIndex (
                    SingleSelectionGridIndexL() ) );
            QueryRenameL( ECamControllerImage );
            break;
            }

        case ECamCmdDelete:        
            {
            if ( DisplayDeleteNoteL() )
                {
                DoDeleteL();
                }
            
            break;
            }
            
        case ECamCmdNewPhoto:
            {
            // the thumbnail scaling needs to be stopped in
            // preparation for the app ui switching the view
            iGridModel->CancelThumbnails();
            CCamViewBase::HandleCommandL( aCommand ); // Pass on to AppUi
            break;
            }
            
        case ECamCmdToolbarMarkUnmark:
            {
            TBool mark = iGridModel->IsMarkedL( iGridModel->HighlightedGridIndex() );                        
            iGridModel->SetCurrentMark( !mark );
      //      UpdateOneClickUploadButton();
            break;
            }
                      
        case EAknCmdMark:        
            {
            iGridModel->SetCurrentMark( ETrue );
    //        UpdateOneClickUploadButton();
            break;
            }
    
        case EAknCmdUnmark:
            {
            iGridModel->SetCurrentMark( EFalse );
 //           UpdateOneClickUploadButton();
            break;
            }
    
        case EAknMarkAll:
            {
            iGridModel->MarkAll( ETrue );
  //          UpdateOneClickUploadButton();
            break;
            }
    
        case EAknUnmarkAll:
            {
            iGridModel->MarkAll( EFalse );
  //          UpdateOneClickUploadButton();
            break;
            }
    
        case EAknSoftkeyBack:
            {
            // the thumbnail scaling needs to be stopped in
            // preparation for the app ui switching the view
            iGridModel->CancelThumbnails();            
            // About to be closed by user, so will want to delete the model
            // on deactivation
            if ( IsForeground() )
                {
                iResetModelOnDeactivate = ETrue;    
                }
            CCamViewBase::HandleCommandL( aCommand ); // Pass on to AppUi
            break;
            }
        case ECamMSKCmdAppChange:
        case EAknSoftkeySelect:
            {   
            if ( iGridModel->NoOfMarkedImages() == 0 )
                {
                // select the focused item and open it to still postcapture
                iResetModelOnDeactivate = EFalse;   

                // This selects the highlighted image
                iController.SetAsCurrentImage( iGridModel->HighlightedGridIndex() );
                // ConvertFromValidToGlobalIndex used so that appropriate images are displayed
                iController.SetAsCurrentImage( iGridModel->ConvertFromValidToGlobalIndex( iGridModel->HighlightedGridIndex() ) );    
                CCamViewBase::HandleCommandL( ECamCmdOpenPhoto );
                }
            else    
                {   
                // If at least one image is marked, open context sensitive menu
                CEikMenuBar* menuBar = MenuBar();            
                if ( menuBar )
                    {
                    menuBar->SetMenuTitleResourceId( R_CAM_BURST_THUMBNAIL_OK_MENUBAR);
                    menuBar->SetMenuType( CEikMenuBar::EMenuContext );
                    menuBar->TryDisplayMenuBarL();
                    // Here we again set back the type of menu to "Options" when pressed LSK
                    menuBar->SetMenuType( CEikMenuBar::EMenuOptions );
                    menuBar->SetMenuTitleResourceId( R_CAM_BURST_THUMBNAIL_MENUBAR);
                    }
                }
            break;
            }

        case EAknSoftkeyOptions:
            {
            // do nothing - handled by the framework
            break;
            }

        case EAknSoftkeyCancel:
            {
            // do nothing - handled by framework
            break;
            }
            
        case ECamCmdSend:
            {
            CCamPostCaptureViewBase::HandleCommandL( aCommand );
            break;
            }

        case ECamCmdSendToCallerMultimedia:
            {
/*#ifndef __WINS__
            DoInCallSendL();
#endif*/
            }
            break;
            
		case ECamCmdQuickSend:
       		{
/*#ifndef __WINS__
            if( iSFIUtils->IsCLIValidL() )
                {
                // only do send to caller if no images are marked
                // or one image is marked
                if ( iGridModel->NoOfMarkedImages() < 2 )
                    {
                    DoInCallSendL();
                    }
                }
            else
#endif // __WINS__*/
                //{
                DoSendAsL();
                //}
            }
            break;            

       case ECamCmdExitStandby:
        	{
       		PRINT( _L("Camera :: CCamBurstThumbnailView::HandleCommandL ECamCmdExitStandby in") )	
        	CCamViewBase::ExitStandbyModeL();
        	PRINT( _L("Camera :: CCamBurstThumbnailView::HandleCommandL ECamCmdExitStandby out") )
        	break;
        	}

        case ECamCmdOpenPhoto:
            {
            iResetModelOnDeactivate = EFalse;   
            TInt markedItems = iGridModel->NoOfMarkedImages();
            // If there are no images marked use the highlighted image 
            // set it now for safety just in case no marked images are found
            iController.SetAsCurrentImage( iGridModel->HighlightedBurstIndex() );

            if ( markedItems != 0 )  // this command should never be available if there is >1 marked item
                {
                TInt totalItems = iGridModel->NoOfImages();
                TInt index = 0;
                TBool found = EFalse;
                while ( index < totalItems && !found )
                    {
                    if ( iGridModel->IsMarkedL( index ) )
                        {
                        found = ETrue;
                        iController.SetAsCurrentImage( index );
                        }
                    index++;
                    }
                }
            }
            //lint -fallthrough 
        default: 
            {
            // Handle AIW commands
            TInt aiwServiceCmd = iAiwServiceHandler->ServiceCmdByMenuCmd( aCommand );
            if ( aiwServiceCmd != KAiwCmdNone )
                {
                CAiwGenericParamList& inParams  = iAiwServiceHandler->InParamListL();
                CAiwGenericParamList& outParams = iAiwServiceHandler->OutParamListL();
                // Add file path/s to AIW parameters
                TInt markedItems = iGridModel->NoOfMarkedImages();
                // If there are no images marked use the highlighted image
                if ( markedItems == 0 )
                    {
                    TAiwVariant variant( iGridModel->ImageFileName( 
                                         iGridModel->HighlightedBurstIndex() ) );
                    TAiwGenericParam param( EGenericParamFile, variant );
                    inParams.AppendL( param );
                    
                    // Set as contact call iamge needs image MIME type as AIW param
                    // Set as wall paper requires MIME type also.
                    TAiwVariant variant2(  KCamImageMimeType );
                    TAiwGenericParam param2( EGenericParamMIMEType, variant2 );
                    inParams.AppendL( param2 );
                    
                    }
                else // there are marked items, add them to the list
                    {
                    TInt totalItems = iGridModel->NoOfImages();
                    TInt index = 0;
                    TInt found = 0;
                    while ( index < totalItems && found < markedItems )
                        {
                        if ( iGridModel->IsMarkedL( index ) )
                            {
                            found++;
                            TAiwVariant variant( iGridModel->ImageFileName( index ) );
                            TAiwGenericParam param( EGenericParamFile, variant );
                            inParams.AppendL( param );
                            
                            // Set as contact call iamge needs image MIME type as AIW param
                            // Set as wall paper requires MIME type also.
                            TAiwVariant variant2(  KCamImageMimeType );
                            TAiwGenericParam param2( EGenericParamMIMEType, variant2 );
                            inParams.AppendL( param2 );
                            }
                        index++;
                        }
                    }

                iAiwServiceHandler->ExecuteMenuCmdL( aCommand, inParams, outParams, 0, this );
                }
            else
                {
                CCamViewBase::HandleCommandL( aCommand );
                }
            break;
            }
        }    
    
    PRINT1( _L("Camera <= CCamBurstThumbnailView::HandleCommandL (aCommand: %d)"), aCommand )
    }


// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::HandleNotifyL
// Handles any notification caused by asynchronous ExecuteCommandL
// or event.
// ---------------------------------------------------------------------------
//
TInt CCamBurstThumbnailView::HandleNotifyL(
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

// -----------------------------------------------------------------------------
// CCamBurstThumbnailView::DisplayDeleteNoteL()
// Display confirmation note for deletion of images
// -----------------------------------------------------------------------------
//
TBool CCamBurstThumbnailView::DisplayDeleteNoteL()
    {
    // Display the delete confirmation note
    HBufC* confirmationText;

    // Find the number of images marked, to work out which note to display.
    TInt noteText = 0;
    TInt noteDlg = 0;
    TInt noOfMarkedImages = iGridModel->NoOfMarkedImages();

    // Check if no of marked images is zero (if we are to delete the current
    // highlighted object) or one (if we are to delete a single marked item).
    if ( noOfMarkedImages  <= 1)    
        {
        noteText = R_CAM_STILL_POST_CAPTURE_DELETE_NOTE_TEXT;
        noteDlg  = R_CAM_STILL_POST_CAPTURE_DELETE_NOTE;
        confirmationText = StringLoader::LoadLC( noteText );    
        }
    else //  noOfMarkedImages > 1 
        {   
        noteText = R_CAM_BURST_DELETE_MULTIPLE_NOTE_TEXT;
        noteDlg  = R_CAM_BURST_DELETE_MULTIPLE_NOTE;
        confirmationText = StringLoader::LoadLC( noteText, noOfMarkedImages );    
        }
    
    // Create the dialog with the text, and show it to the user
    CAknQueryDialog* confirmationDialog = new( ELeave )CAknQueryDialog( *confirmationText );
    CleanupStack::PopAndDestroy( confirmationText );

    return confirmationDialog->ExecuteLD( noteDlg );
    }

// -----------------------------------------------------------------------------
// CCamBurstThumbnailView::DoDeleteL
// Marks the required files for deletion
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailView::DoDeleteL()
    {
    if ( iGridModel->NoOfMarkedImages() > 0 )
        {
        // Confirmation; mark the files for deletion.
        iGridModel->DeleteMarkedL();
        }
    else
        {
        // Confirmation; mark the highlighted item
        iGridModel->DeleteHighlightL();
        }
 //   UpdateOneClickUploadButton();
    }



// -----------------------------------------------------------------------------
// CCamBurstThumbnailView::HandleForegroundEventL
// Handle foreground event
// -----------------------------------------------------------------------------
//
void CCamBurstThumbnailView::HandleForegroundEventL( TBool aForeground )
  {
  PRINT( _L("Camera => CCamBurstThumbnailView::HandleForegroundEventL") );

  if ( aForeground )
    {
    CCamAppUiBase* appui = static_cast<CCamAppUiBase*>( AppUi() );
    if ( !appui->IsInPretendExit() )
      {
      IncrementCameraUsers();
      iController.DeepSleepTimerStart();
      }
    }
  else
    {
    // Only decrements if we have incremented.
    DecrementCameraUsers();
    }

  PRINT( _L("Camera <= CCamBurstThumbnailView::HandleForegroundEventL") );
  }


// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                                                    const TDesC8& aCustomMessage )
    {    
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMBURSTTHUMBNAILVIEW_DOACTIVATEL, "e_CCamBurstThumbnailView_DoActivateL 1" );
    PRINT( _L( "Camera => CCamBurstThumbnailView::DoActivateL" ) );
    PERF_EVENT_START_L2( EPerfEventBurstThumbnailViewActivation );
    if ( aPrevViewId.iViewUid.iUid == ECamViewIdStillPreCapture || !iGridModel )
        {
        iResetModelOnDeactivate = EFalse;

        // Precautionary tidyup, though should never happen. 
        if ( iGridModel )
            {
            delete iGridModel;
            iGridModel = NULL;
            }

        // If we've come from Pre-Capture view, we shouldn't have an active model.
        // Create a new one and register with it.
        iGridModel = CCamBurstThumbnailGridModel::NewL();        
        iGridModel->AddModelObserverL( this, 
                                      CCamBurstThumbnailGridModel::EModelEventDeleted | 
                                      CCamBurstThumbnailGridModel::EModelEventHighlight );

        // Guaranteed to only be called right after a burst capture         
        iGridModel->SetBurstArrayL( iController.BurstCaptureArray() );
        }
    else
        {
        // We still need to re-register for model events, else if we return to 
        // grid view from post-capture, we won't receive delete/highlight events
        // any more.
        iGridModel->AddModelObserverL( this, 
                                      CCamBurstThumbnailGridModel::EModelEventDeleted | 
                                      CCamBurstThumbnailGridModel::EModelEventHighlight );

        iGridModel->RefreshL();
        }
          
    if ( iController.IsTouchScreenSupported() )
        {
        // set toolbar
		if ( iOneClickUploadUtility->OneClickUploadSupported() )
            {
            CreateAndSetToolbarL( R_CAM_BURST_POSTCAPTURE_TOOLBAR_UPLOAD );
            UpdateToolbarIconsL();
            }
        CAknToolbar* fixedToolbar = Toolbar();
        if ( fixedToolbar )
            {
            fixedToolbar->SetToolbarObserver( this );
            fixedToolbar->SetToolbarVisibility( ETrue );           
            }
        }
    
    
 
    CCamPostCaptureViewBase::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    
    iAiwServiceHandler->Reset();   

    // initialise AIW services for touch menu
    iAiwServiceHandler->AttachMenuL( R_CAM_BURST_THUMBNAIL_MENU, 
                                     R_CAM_MOVE_TO_STILL_IMAGE_SEQUENCE_INTEREST );
    iAiwServiceHandler->AttachMenuL( R_CAM_BURST_THUMBNAIL_MENU, 
            R_CAM_SET_AS_CALL_IMAGE_INTEREST );
    iAiwServiceHandler->AttachMenuL( R_CAM_BURST_THUMBNAIL_MENU, 
            R_CAM_SHARE_ON_OVI_INTEREST );
    // SHARE_AIW
    iAiwServiceHandler->AttachMenuL( R_CAM_BURST_THUMBNAIL_MENU,
                                     R_CAM_AIW_VIEW_INTEREST );
    
    static_cast<CCamAppUiBase*>( AppUi() )->PushDefaultNaviPaneL();
    PERF_EVENT_END_L2( EPerfEventBurstThumbnailViewActivation );
    PRINT( _L( "Camera <= CCamBurstThumbnailView::DoActivateL" ) );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMBURSTTHUMBNAILVIEW_DOACTIVATEL, "e_CCamBurstThumbnailView_DoActivateL 0" );
    }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::DoDeactivate()
    { 
    PRINT( _L( "Camera => CCamBurstThumbnailView::DoDeactivateL" ) );          
    iGridModel->RemoveModelObserver( this,  CCamBurstThumbnailGridModel::EModelEventDeleted | 
                                            CCamBurstThumbnailGridModel::EModelEventHighlight );

    CCamPostCaptureViewBase::DoDeactivate();

    // Must delete this last (after everything had a chance to de-register)
    // also delete model if all images have been deleted
    if ( iResetModelOnDeactivate || iController.BurstCaptureArray()->ImagesRemaining() == 0 )
        {   
        PRINT( _L( "Camera: CCamBurstThumbnailView::DoDeactivateL resetting model on deactivate" ) );             
        delete iGridModel;
        iGridModel = NULL;
        }
	PRINT( _L( "Camera <= CCamBurstThumbnailView::DoDeactivateL" ) );        
    }


// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::HighlightChanged
// Called when the highlighted item in the grid view changes
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::HighlightChanged()
    {
    // Check title is correct as highlight may have changed
    TRAPD( ignore, SetTitlePaneTextL() );
    if ( ignore )
        { 
        // Do nothing ( removes build warning )
        }
    }


// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::ImagesDeleted
// Called when images have been deleted from the grid view
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::ImagesDeleted() 
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );

    // inform the media gallery
    TRAPD( ignore, appUi->HandleControllerEventL( ECamEventMediaFileChanged, KErrNone ) );
    // Check there is still at least one image 
    if ( iGridModel->NoOfValidCells() == 0 )
        {
        iResetModelOnDeactivate = ETrue; 

        CAknToolbar* toolbar = Toolbar();
        if (toolbar)
            {
            toolbar->SetToolbarVisibility( EFalse );
            }
        // Close thumbnail view, go back to Burst PreCapture
        TRAP( ignore, HandleCommandL( EAknSoftkeyBack ) );
        }
    else
        {
        iGridModel->UpdateViewableGrid( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CCamViewBase::IsNewNameValidL
// Check if renaming to the given name is valid.
// ---------------------------------------------------------------------------
//
TBool 
CCamBurstThumbnailView::IsNewNameValidL( const TDesC& aFilename )
    {
    TInt currentIndex = iGridModel->HighlightedGridIndex();

    TInt i = 0;
    TInt burstItemsCount = iGridModel->NoOfValidCells();
    for ( i = 0; i < burstItemsCount; ++i )
        {
        TPtrC imageName = iGridModel->ImageName( i );

        if ( i != currentIndex )
            {
            if ( CamUtility::CompareIgnoreCase( aFilename,
                 imageName ) )
                {
                // New name already exists on grid
                return EFalse;
                }
            }

        }
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::CCamBurstThumbnailView
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamBurstThumbnailView::CCamBurstThumbnailView( CCamAppController& aController )
    : CCamPostCaptureViewBase( aController )
    {
    }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::ConstructL()
    {    
    PRINT( _L("Camera => CCamBurstThumbnailView::ConstructL") );
    
    BaseConstructL( R_CAM_BURST_THUMBNAIL_VIEW );
    CCamPostCaptureViewBase::ConstructL();
    
    PRINT( _L("Camera <= CCamBurstThumbnailView::ConstructL") );
    }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::CreateContainerL
// Create container control
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::CreateContainerL()
    {    
    PRINT( _L("Camera => CCamBurstThumbnailView::CreateContainerL") );
    CCamBurstThumbnailContainer* cont = new (ELeave) CCamBurstThumbnailContainer( 
                                                           iController,
                                                           *this, 
                                                           *iGridModel );
    CleanupStack::PushL( cont );
    cont->SetMopParent( this );
    TRect screen;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    cont->ConstructL( screen );
    CleanupStack::Pop( cont );
    iContainer = cont;
    PRINT( _L("Camera <= CCamBurstThumbnailView::CreateContainerL") );
    }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::SetTitlePaneTextL
// Set the view's title text
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::SetTitlePaneTextL()
    {    
    PRINT( _L("Camera => CCamBurstThumbnailView::SetTitlePaneTextL") );
    // Do not want to set the title to the filename if it's shown in the application pane
    _LIT(KSpaces, "  ");
    static_cast<CCamAppUi*>( iEikonEnv->AppUi() )->SetTitleL( KSpaces );
    PRINT( _L("Camera <= CCamBurstThumbnailView::SetTitlePaneTextL") );
    }
    
// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::DoSendAsL
// Use SendUI to create a message containing burst thumbnails
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::DoSendAsL() const
    {
    PRINT( _L("Camera => CCamBurstThumbnailView::DoSendAsL") );
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( AppUi() ); 
    
    if ( !appUi->SendAsInProgress() )
        {
        appUi->SetSendAsInProgressL( ETrue ); 
        CMessageData* messageData = CMessageData::NewLC();

        TInt totalItems = iGridModel->NoOfImages();
        TInt markedItems = iGridModel->NoOfMarkedImages();

        if ( markedItems == 0 )
            {
            // if no images are marked use the highlighted item instead
            messageData->AppendAttachmentL( iGridModel->ImageFileName( 
                    iGridModel->HighlightedBurstIndex() ) );
            }
        else
            {
            // attach all marked items
            TInt index = 0;
            TInt found = 0;
            while ( index < totalItems && found < markedItems )
                {
                if ( iGridModel->IsMarkedL( index ) )
                    {
                    found++;
                    messageData->AppendAttachmentL( iGridModel->ImageFileName( index ) );
                    }
                index++;
                }
            }

        TRAP_IGNORE( iSendAppUi->ShowQueryAndSendL( messageData, iSendingCapabilities ) )
        
        appUi->SetSendAsInProgressL( EFalse );
        CleanupStack::PopAndDestroy( messageData );
        }
    
    
    PRINT( _L("Camera <= CCamBurstThumbnailView::DoSendAsL") );
    }

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::SingleSelectionGridIndexL()
// Returns the array index of the first marked item,
// or highlighted item if none marked.
// ---------------------------------------------------------------------------
//
TInt CCamBurstThumbnailView::SingleSelectionGridIndexL() const
    {
    TInt ret = KErrNotFound;

    if( iGridModel->NoOfMarkedImages() == 0 )
        {
        ret = iGridModel->HighlightedGridIndex();
        }
    else
        {
        TInt totalItems = iGridModel->NoOfImages();
        for( TInt index = 0; index < totalItems; index++ )
            {
            if ( iGridModel->IsMarkedL( index ) )
                {
                ret = index;
                break;
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CCamViewBase::IsBurstPostCapture
// Returns whether the view is a burst mode postcapture view
// ---------------------------------------------------------------------------
//
TBool CCamBurstThumbnailView::IsBurstPostCapture()
    {
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CCamViewBase::ImageFilesDeleted
// Notifies view that images files have been deleted outside of camera
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::ImageFilesDeleted()
    {
    PRINT( _L("Camera => CCamBurstThumbnailView::ImageFilesDeleted") )
    if ( iGridModel )
        {
        iGridModel->ImageFilesDeleted();
        }
    PRINT( _L("Camera <= CCamBurstThumbnailView::ImageFilesDeleted") )
    }


/*#ifndef __WINS__
// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::DoInCallSendL
// Handle send to caller functionality
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::DoInCallSendL() const
    {
    // Ignore if more than one item marked
    if( iGridModel->NoOfMarkedImages() < 2 )
        {
        TInt index = SingleSelectionGridIndexL();
        if ( iSFIUtils && iSFIUtils->IsCLIValidL() && index >=0 )
            {
            iSFIUtils->SendMediaFileL( iGridModel->ImageFileName( index ) );
            }
        }
    }
#endif // __WINS__*/

// ---------------------------------------------------------------------------
// CCamBurstThumbnailView::CalculateThumbnailSize
// Calculates the size of a single thumbnail based on the infromation 
// how many images are alltogether shown on the burst grid
// ---------------------------------------------------------------------------
//
TSize CCamBurstThumbnailView::CalculateThumbnailSize( TInt aNumberOfBurstImages )
	{
	TSize thumb;
	TAknLayoutRect gridLayout;
	TAknLayoutRect cellLayout;
	TAknLayoutRect thumbLayout;
	
	thumb.iHeight = 0;
	thumb.iWidth = 0;
	
	if ( aNumberOfBurstImages <= KBurstGridDefaultThumbnails )
		{
		 gridLayout.LayoutRect( ClientRect(), AknLayoutScalable_Apps::grid_cam4_burst_pane( 1 ) );
		 cellLayout.LayoutRect( gridLayout.Rect(), AknLayoutScalable_Apps::cell_cam4_burst_pane( 1, 0, 0 ) ); 
		 thumbLayout.LayoutRect( cellLayout.Rect(), AknLayoutScalable_Apps::cell_cam4_burst_pane_g1( 0, 0, 0 ) ); 
		}
	else if ( aNumberOfBurstImages <= KBurstGridMaxVisibleThumbnails )
		{
		 gridLayout.LayoutRect( ClientRect(), AknLayoutScalable_Apps::grid_cam4_burst_pane( 3 ) );
		 cellLayout.LayoutRect( gridLayout.Rect(), AknLayoutScalable_Apps::cell_cam4_burst_pane( 3, 0, 0 ) ); 
		 thumbLayout.LayoutRect( cellLayout.Rect(), AknLayoutScalable_Apps::cell_cam4_burst_pane_g1( 1, 0, 0 ) ); 
		}
	else
		{
		 gridLayout.LayoutRect( ClientRect(), AknLayoutScalable_Apps::grid_cam4_burst_pane( 5 ) );
		 cellLayout.LayoutRect( gridLayout.Rect(), AknLayoutScalable_Apps::cell_cam4_burst_pane( 5, 0, 0 ) ); 
		 thumbLayout.LayoutRect( cellLayout.Rect(), AknLayoutScalable_Apps::cell_cam4_burst_pane_g1( 2, 0, 0 ) ); 
		}
	
	thumb.iHeight = thumbLayout.Rect().Height();
	thumb.iWidth = thumbLayout.Rect().Width();
	
	return thumb;
	}


void CCamBurstThumbnailView::DynInitAiwMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {

    if ( aResourceId == R_CAM_BURST_THUMBNAIL_MENU )
        {
        if ( iAiwServiceHandler && iAiwServiceHandler->IsAiwMenu( aResourceId ) )
            {

            CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
            TInt markedItems = iGridModel->NoOfMarkedImages();
            // If there are no images marked use the highlighted image

            TCamOrientation orientation = static_cast<CCamAppUiBase*>( AppUi() )->CamOrientation();
            
            if ( markedItems == 0 )
                {
                TAiwVariant variant( iGridModel->ImageFileName( 
                        iGridModel->HighlightedBurstIndex() ) );
                TAiwGenericParam param( EGenericParamFile, variant );
                paramList.AppendL( param );

                // Set as contact call image needs image MIME type as AIW param
                //if ( orientation == ECamOrientationCamcorder || orientation == ECamOrientationCamcorderLeft )
                {
                TAiwVariant variant2(  KCamImageMimeType );
                TAiwGenericParam param2( EGenericParamMIMEType, variant2 );
                paramList.AppendL( param2 );
                }

                }
            else if ( markedItems == 1 )
                {
                TInt totalItems = iGridModel->NoOfImages();
                TInt index = 0;
                while ( index < totalItems )
                    {
                    if ( iGridModel->IsMarkedL( index ) )
                        {

                        TAiwVariant variant( iGridModel->ImageFileName( index ) );
                        TAiwGenericParam param( EGenericParamFile, variant );
                        paramList.AppendL( param );

                        //if ( orientation == ECamOrientationCamcorder || orientation == ECamOrientationCamcorderLeft )
                        {
                        TAiwVariant variant2(  KCamImageMimeType );
                        TAiwGenericParam param2( EGenericParamMIMEType, variant2 );
                        paramList.AppendL( param2 );
                        }

                        break;
                        }
                    index++;
                    }
                }
            else // there are more than on marked item
                {
                // removing lint warning
                }

            iAiwServiceHandler->InitializeMenuPaneL(
                    *aMenuPane, 
                    aResourceId, 
                    ECamCmdAIWCommands, 
                    paramList );
            }

        }

    }

// ---------------------------------------------------------------------------
// Dim 1-click upload button if more than one image is marked
// ---------------------------------------------------------------------------
//
void CCamBurstThumbnailView::UpdateOneClickUploadButton()
    {
    CAknToolbar* toolbar = Toolbar();
    if( toolbar && iGridModel )
        {
        TBool shouldBeDimmed = ( iGridModel->NoOfMarkedImages() > 1 );

        CAknButton* button = static_cast<CAknButton*>(
                toolbar->ControlOrNull( ECamCmdOneClickUpload ) );
		
		if ( button )
            {
            TBool isDimmed = button->IsDimmed();
            if ( ( shouldBeDimmed && !isDimmed ) ||
                 ( !shouldBeDimmed && isDimmed ) )
                 {
                 button->SetDimmed( shouldBeDimmed );
                 button->DrawDeferred();
                 }
             }
        }
    }


//  End of File  
