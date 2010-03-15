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
* Description:  Active object that manages saving and deletion of images*
*/



// INCLUDE FILES
#include <badesca.h>
#include <eikenv.h>
#include <sysutil.h>
#include <AknProgressDialog.h>
#include <eikprogi.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <pathinfo.h>

// includes to allow reporting to LifeBlog via publish and subscribe API
#include <e32property.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamImageSaveActive.h"
#include "CamPanic.h"
#include "CamUtility.h"
#include "CamPerformance.h"
#include "cambuffershare.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamImageSaveActiveTraces.h"
#endif
#include "camuidconstants.h" // KPSUidCamcorderNotifier

#include "CameraappPrivateCRKeys.h"

#include "CamAppController.h"
#include "CameraUiConfigManager.h"
#include "CamBurstCaptureArray.h"

const TUint32 KCamLatestFilePath      = 0x00000001;
const TInt    KArrayGranularity       = 2;
const TInt    KCapturedAlbumId        = 2; 

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamImageSaveActive::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamImageSaveActive* CCamImageSaveActive::NewL( MCamImageSaveObserver& aObserver,  
                                                CCamAppController&  aController )
    {
    PRINT( _L("Camera => CCamImageSaveActive::NewL") );

    CCamImageSaveActive* self = new( ELeave ) CCamImageSaveActive( aObserver, 
                                                                   aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    PRINT( _L("Camera <= CCamImageSaveActive::NewL") );
    return self;
    }

    
// -----------------------------------------------------------------------------
// CCamImageSaveActive::~CCamImageSaveActive
// Destructor
// -----------------------------------------------------------------------------
//
CCamImageSaveActive::~CCamImageSaveActive()
    {
    PRINT( _L("Camera => ~CCamImageSaveActive") );
    delete iSaveArray;

    delete iNameArray;
    iSnapArray.ResetAndDestroy();
    delete iManager;
            
    ReleaseAllAndDestroy( iImageArray );
    iImageArray.Close();

    iThumbnailRequests.Close();
    DoDismissProgressNote();

#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
    DeRegisterHarverterClientEvents();
#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)

    PRINT( _L("Camera <= ~CCamImageSaveActive") );
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::Cancel
// Cancel any current or pending actions
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::Cancel()
    {
    PRINT( _L( "Camera => CCamImageSaveActive::Cancel" ) );
 
    TBool storageOk( ETrue );
    if ( iController.SequenceCaptureInProgress() )
        {
        TCamMediaStorage storageLocation = static_cast<TCamMediaStorage> ( 
                iController.IntegerSettingValueUnfiltered(  
                        ECamSettingItemPhotoMediaStorage ) );
        if ( storageLocation == ECamMediaStorageCard )
            {
            // Avoid deleting and saving to drive which does not exist
            storageOk = CamUtility::MemoryCardStatus() == ECamMemoryCardInserted;
            PRINT1( _L( "Camera => CCamImageSaveActive::Cancel storageOk=%d" ), storageOk);
            }
        }

    // Since this function may be called in the destructor, we need to check
    // that iSaveArray is not null. 
    if(iSaveArray)
        {
        TInt count = iSaveArray->Count() - 1;
        TInt lowest = 0;
        if ( iSaveCurrent )
            {
            lowest = 1;
            }
        TInt i;

        // Delete all dummy files except any currently saving one
        for ( i = count; i >= lowest; i-- )
            {
            PRINT1( _L( "Camera Deleting file index %d" ), i );         
            if( storageOk )
                {
                DeleteFile( iSaveArray->MdcaPoint( i ), ETrue );  
                }
            }
        }
    // if a file is currently saving, delay cleanup till save completes
    // if ( iSaveCurrent )
    if ( IsActive() && storageOk )
        {
        PRINT( _L( "Camera File currently saving delay cleanup" ) );         
        iDoCancel = ETrue;
        if ( !iWait.IsStarted() )
          {
          iWait.Start();
          }
        }
    // otherwise, complete cleanup now
    else
        {
        PRINT( _L( "Camera No file saving cleanup now" ) );         
        DoDismissProgressNote();
        CActive::Cancel();
        if(iSaveArray)
            {
            iSaveArray->Reset();
            }
        ReleaseAllAndDestroy( iImageArray );
        }
    PRINT( _L( "Camera <= CCamImageSaveActive::Cancel" ) );         
    }

void CCamImageSaveActive::ForceCancel()
{
    CActive::Cancel();
}
// -----------------------------------------------------------------------------
// CCamImageSaveActive::AddToSave
// Add an image to the list of required saves, activate if needed
// -----------------------------------------------------------------------------
//
TBool CCamImageSaveActive::AddToSave( const TDesC&     aFilename, 
                                      CCamBufferShare* aImageData )
    {
    PRINT( _L("Camera => CCamImageSaveActive::AddToSave") );
    if( aImageData )
      {
      aImageData->Reserve();
      }

    // Keep note of how many items exist before we try to add a new one
    TInt count = iSaveArray->Count();
    TRAPD( err, iSaveArray->AppendL( aFilename ) );
    if ( !err )
        {
        TRAP( err, iImageArray.AppendL( aImageData ) );
        }
    // if any of the appends failed then clean out the partial item
    if ( err )
        {
        if ( iSaveArray->Count() > count )
            {
            iSaveArray->Delete( count );
            }
        if ( iImageArray.Count() > count )
            {
            iImageArray.Remove( count );
            if( aImageData )
              {
              aImageData->Release();
              }
            }
        PRINT( _L("Camera <= CCamImageSaveActive::AddToSave, error!") );
        return EFalse;
        }
    // the item is successfully added, start the save now if not currently saving
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }

    PRINT( _L("Camera <= CCamImageSaveActive::AddToSave, ok") );
    return ETrue;
    }
 
 
// -----------------------------------------------------------------------------
// CCamImageSaveActive::FileSize
// Returns the size of the data for the specified file
// -----------------------------------------------------------------------------
//    
TInt CCamImageSaveActive::FileSize( const TDesC& aFilename ) const
    {    
    TInt size = KErrNotFound;
    TInt count = iSaveArray->Count();
    TInt i;
    
    // Check to see if it is in the "to be saved" array
    for ( i = 0; i < count ; i++ )
        {
        TPtrC ptr = (*iSaveArray)[i];
        if ( aFilename.Compare( ptr ) == 0 )    
            {
            TRAP_IGNORE( size = iImageArray[i]->SharedBuffer()->DataL( 0 )->Size() );
            // size = iImageArray[i]->Size();
            return size;
            }
        }   
        
    // If we have got here, the requested file is NOT in the array
    // Check the file system to see if we have already saved it.
    RFs& fs = CEikonEnv::Static()->FsSession();
    RFile file;
    TInt ret = file.Open( fs, aFilename, EFileShareAny | EFileRead );    
    if ( ret == KErrNone )
        {
        file.Size( size );                
        }
    file.Close();        
    return size;
    }
    
    
    
// -----------------------------------------------------------------------------
// CCamImageSaveActive::DeleteFile
// Delete a file, cancelling any pending save
// -----------------------------------------------------------------------------
//
TInt CCamImageSaveActive::DeleteFile( const TDesC& aFilename, 
                                      TBool aSaveRequested )
    {
    RFs& fs = CEikonEnv::Static()->FsSession();
    if ( aSaveRequested )
        {
        TInt index;
        if ( iSaveArray->Find( aFilename, index ) == 0 )
            {
            // if the file is currently being saved
            if ( index == 0 && iSaveCurrent )
                {
                iDeleteCurrent = ETrue;
                return ETrue;
                }
            else
                {
                iSaveArray->Delete( index );
                iSaveArray->Compress();

                CCamBufferShare* item = iImageArray[index];
                iImageArray.Remove( index );
                if( item )
                  {
                  item->Release();
                  item = NULL;
                  }

                iImageArray.Compress();
                }
            } 
        }

    // Dummy files are read only for protection, make sure the file is writable
    fs.SetAtt( aFilename, 0, KEntryAttReadOnly );
    TInt ret = fs.Delete( aFilename );  
    ReportToObserver( ECamSaveEventDeleted );

    return ret;
    }

// ---------------------------------------------------------------------------
// CCamImageSaveActive::AlreadySavedFile
// Returns whether a particular file has already been saved.
// ---------------------------------------------------------------------------
//
TBool CCamImageSaveActive::AlreadySavedFile( const TDesC& aFilename ) const
    {
    TInt index;
    return iSaveArray->Find( aFilename, index );
    }

// ---------------------------------------------------------------------------
// CCamImageSaveActive::CurrentlySavingFile
// Returns whether a particular file is in the process of being saved.
// ---------------------------------------------------------------------------
//
TBool CCamImageSaveActive::CurrentlySavingFile( const TDesC& aFilename ) const
    {
    TInt index;
    if ( iSaveCurrent && ( iSaveArray->Find( aFilename, index ) == 0 ) &&
        ( index == 0 ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::DisplayProgressNote
// Display the saving progress note until cancelled or completed
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::DisplayProgressNote( TInt aExpectedImages )
    {
    __ASSERT_DEBUG( !iDismissNote, CamPanic( ECamPanicInvalidState ) );
    // Initialise for a new burst set
    iSaveProgressCount = 0;
    iExpectedImages = aExpectedImages;
    iShowNote = ETrue;
    if ( !IsActive() )
        {
        SetActiveAndCompleteRequest();
        }
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::DismissProgressNote
// Dismiss the saving progress note when completed
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::DismissProgressNote()
    {
    // if the progress note is still pending
    if ( iShowNote )
        {
        // the burst has completed already
        iShowNote = EFalse;
        ReportToObserver( ECamSaveEventBurstComplete );
        }

    // No saves are pending, dismiss the note now
    if ( !IsActive() )
        {
        DoDismissProgressNote();
        }
    else // otherwise, wait till all the saves complete
        {
        iDismissNote = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::SetAddImageToAlbum
// Set the "Add to album" feature on or off for photos
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::SetAddImageToAlbum( const TBool aAdding, 
                                              const TUint32 aDefaultAlbumId )
    {
    iAddImageToAlbum = aAdding;
    iDefaultAlbumId = aDefaultAlbumId;
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::AddToAlbum
// Add the specified photo or video to the default capture album
// -----------------------------------------------------------------------------
//
#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
void CCamImageSaveActive::AddToAlbum( const TDesC& aFilename,
                                      const TBool aAddToAlbum,
                                      const TUint32 aDefaultAlbumId )
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMIMAGESAVEACTIVE_ADDTOALBUM, "e_CCamImageSaveActive_AddToAlbum 1" );
    PRINT( _L("Camera => CCamImageSaveActive::AddToAlbum") );

    TInt err = KErrNone;
    RArray<TUint32> harvestAlbumIds;
    TUint32 harvestAlbumId;
    err = harvestAlbumIds.Append( KCapturedAlbumId ); 

    /*
    * NOTE: The default value of album Id defined as per CenRep key is 0, so 
    *       we add the any other album other than the default one as above
    *       Secondly, if there is no album set as default album from the 
    *       precapture settings views then we pass empty array to the harvester
    *       and MDS harvests the file to camera album directly i.e. captured.
    */

    if ( aAddToAlbum && aDefaultAlbumId > 0 )
       {
       harvestAlbumId = aDefaultAlbumId;
       err = harvestAlbumIds.Append( harvestAlbumId );
       }
    
    if ( !iHarvesterClientConnected )
        {
        PRINT( _L("Camera <> calling iHarvesterClient.Connect") );
        err = iHarvesterClient.Connect();
        if ( !err )
            {
            iHarvesterClientConnected = ETrue;
            }
        PRINT1( _L("Camera <> iHarvesterClient.Connect returned %d"), err );
        }
    
    if ( KErrNone == err )
        {
        RegisterForHarvesterEvents();

        PRINT( _L("Camera <> calling iHarvesterClient.HarvestFile") );

#ifdef RD_MDS_2_5   
        if ( iController.UiConfigManagerPtr() && iController.UiConfigManagerPtr()->IsLocationSupported() )
            {
            TBool location = iController.IntegerSettingValue( ECamSettingItemRecLocation );
			//Never save location information in second camera
            iHarvesterClient.HarvestFile( aFilename,
                                          harvestAlbumIds,
                                          location );
            }
        else
            {
            iHarvesterClient.HarvestFile( aFilename,
                                          harvestAlbumIds,
                                          EFalse );
            }
#else // RD_MDS_2_5
        iHarvesterClient.HarvestFile( aFilename,
                                      harvestAlbumIds );
#endif // RD_MDS_2_5
        
        PRINT( _L("Camera <> iHarvesterClient.HarvestFile returned") );
        }
    harvestAlbumIds.Close();

    PRINT( _L("Camera <= CCamImageSaveActive::AddToAlbum") );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMIMAGESAVEACTIVE_ADDTOALBUM, "e_CCamImageSaveActive_AddToAlbum 0" );
    }
#else // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
void CCamImageSaveActive::AddToAlbum( const TDesC&  /*aFilename*/,
                                      const TBool   /*aAddToAlbum*/,
                                      const TUint32 /*aDefaultAlbumId*/ )
    {
    }
#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)

// -----------------------------------------------------------------------------
// CCamImageSaveActive::Count
// The number of items in the saving array
// -----------------------------------------------------------------------------
//
TInt CCamImageSaveActive::Count() const
    {
    return iSaveArray->Count();
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::CCamImageSaveActive
// C++ constructor
// -----------------------------------------------------------------------------
//
CCamImageSaveActive::CCamImageSaveActive( MCamImageSaveObserver& aObserver,         
                                          CCamAppController&  aController )
    : CActive( EPriorityStandard ), iObserver( aObserver ), 
    iController( aController ),
    iFlushing( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::ConstructL
// 2nd phase construction
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::ConstructL()
    {
    PRINT( _L("Camera => CCamImageSaveActive::ConstructL") );

    iSaveArray = new( ELeave ) CDesCArraySeg( KArrayGranularity );
    iNameArray = new( ELeave ) CDesCArraySeg( KArrayGranularity );
    CActiveScheduler::Add( this );

    PRINT( _L("Camera <= CCamImageSaveActive::ConstructL") );
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::SetActiveAndCompleteRequest
// Schedule to run immediately
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::SetActiveAndCompleteRequest()
    {
    SetActive();
    TRequestStatus* statusPtr = &iStatus;
    User::RequestComplete( statusPtr, KErrNone );
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::DoCancel
// Cancels the active object
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::DoCancel()
    {
    PRINT( _L( "Camera => CCamImageSaveActive::DoCancel" ) );      
    iDismissNote = ETrue;
    if ( iWait.IsStarted() )
        {
        iWait.AsyncStop(); 
        }
    iDoCancel = EFalse;
    // if no new saves have been requested since the cancellation
    if ( iSaveArray->Count() == 0 )
        {
        CActive::Cancel();
        iSaveArray->Reset();

        ReleaseAllAndDestroy( iImageArray );
        }
    PRINT( _L( "Camera <= CCamImageSaveActive::DoCancel" ) );                
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::RunL
// Clean up after any previous save, then run the next pending save
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::RunL()
    {
    PRINT( _L( "Camera => CCamImageSaveActive::RunL" ) ); 
    
    if( !iFlushing )
        {
        // if the progress note has been requested
        if ( iShowNote )
            {
            iShowNote = EFalse;
            DoDisplayProgressNoteL();
            }
        // if a save has just completed
        if ( iSaveCurrent )
            {
            CompleteSaveOperation( ETrue );
            //ReportToObserver( ECamSaveEventComplete );
            }
        else
            {
            // If cancellation has been requested
            if ( iDoCancel )
                {
                DoCancel();       
                }
            }
        }
    else
        {
        iFlushing = EFalse;
        CompleteSaveOperation( EFalse );
        ReportToObserver( ECamSaveEventComplete );
        }
    
    if ( !iSaveCurrent )
        {
        // if there are more images to save
        if ( iSaveArray->Count() > 0 )
            {
            DoSaveL();
            }
        // if all saves are complete and the progress note needs to be dismissed
        else if ( iDismissNote )
            {
            DoDismissProgressNote();
            }
        else if ( iNameArray->Count() > 0 )
            {
            DoCreateThumbnailL();
            }    
        else // Do nothing
            {
            }
        }
    PRINT( _L( "Camera <= CCamImageSaveActive::RunL" ) ); 
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::RunError
//
// -----------------------------------------------------------------------------
//
TInt
CCamImageSaveActive::RunError( TInt aError )
  {
  PRINT1( _L( "Camera => CCamImageSaveActive::RunError, error(%d)"), aError ); 
  Cancel();

  if( aError == KErrDiskFull || aError == KErrNotReady )
    {
    if( aError == KErrNotReady  )
      {
      iController.SetCaptureStoppedForUsb( ETrue );
      }
    
    ReportToObserver( ECamSaveEventSaveError );  
    }
  else
    {
    ReportToObserver( ECamSaveEventCriticalError );  
    }

  PRINT ( _L( "Camera <= CCamImageSaveActive::RunError") );
  return KErrNone;
  }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::CompleteSaveOperation
// Clean up after the previous save
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::CompleteSaveOperation( TBool aDoFlush )
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMIMAGESAVEACTIVE_COMPLETESAVEOPERATION, "e_CCamImageSaveActive_CompleteSaveOperation 1" );
    PRINT( _L( "Camera => CCamImageSaveActive::CompleteSaveOperation" ) );     
    if( aDoFlush )
        {
        iFile.Flush(iStatus);
        iFlushing = ETrue;
        SetActive();
        }
    else
        {
        iSaveProgressCount++;
        // if the saving progress note is open
        if ( iSaveProgressDialog && iSaveProgressInfo )
            {
            // Reset inactivity timer.  Need to call this periodically 
            // to keep the backlight on
            User::ResetInactivityTime();
            // Increment the progress in the progress bar
            iSaveProgressInfo->IncrementAndDraw( 1 );
            }
        iSaveCurrent = EFalse;
        iFile.Close();
        // If there is an error or a pending request for deletion
        if ( iDeleteCurrent || iStatus != KErrNone )
            {
            RFs fs = CEikonEnv::Static()->FsSession();
            fs.Delete( iSaveArray->MdcaPoint( 0 ) );
            ReportToObserver( ECamSaveEventDeleted );
            }
        else // the save is successful, add to album if required
            {
            PERF_EVENT_END_L2( EPerfEventSaveImage );         
    
            AddToAlbum( iSaveArray->MdcaPoint( 0 ), 
                        iAddImageToAlbum, 
                        iDefaultAlbumId );
    
            // report to LifeBlog
            RProperty::Set( KPSUidCamcorderNotifier, KCamLatestFilePath, iSaveArray->MdcaPoint( 0 ) ); 
            }
    
        iDeleteCurrent = EFalse;
        
        // Delete the item from the saving arrays
        iSaveArray->Delete( 0 );
        iSaveArray->Compress();
    
        CCamBufferShare* share = iImageArray[0];
        iImageArray.Remove( 0 );
        if( share )
          {
          share->Release();
          share = NULL;
          }
        iImageArray.Compress();
    
        // If cancellation has been requested
        if ( iDoCancel )
            {
            DoCancel();       
            }
        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMIMAGESAVEACTIVE_COMPLETESAVEOPERATION, "e_CAM_APP_SAVE 0" ); 
        }
    PRINT1( _L("Camera <> CCamImageSaveActive .. After this file saving, total shared buffers in use: %d"), CCamBufferShare::TotalBufferShareCount() );
    PRINT( _L( "Camera <= CCamImageSaveActive::CompleteSaveOperation" ) );         
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP2_CCAMIMAGESAVEACTIVE_COMPLETESAVEOPERATION, "e_CCamImageSaveActive_CompleteSaveOperation 0" );
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::DoSaveL
// Start a saving operation
// -----------------------------------------------------------------------------
//
void 
CCamImageSaveActive::DoSaveL()
  {
  PERF_EVENT_START_L2( EPerfEventSaveImage );    
  PRINT( _L( "Camera => CCamImageSaveActive::DoSaveL" ) );     
  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMIMAGESAVEACTIVE_DOSAVEL, "e_CAM_APP_SAVE 1" );  //CCORAPP_SAVE_START
  
  TPtrC filename = iSaveArray->MdcaPoint( 0 );

  PRINT1( _L( "Camera <> CCamImageSaveActive: trying to save file:[%S]"), &filename );     

  // Check disk space
  TInt drive = 0;
  __ASSERT_ALWAYS( !RFs::CharToDrive( filename[0], drive ),
      CamPanic( ECamPanicFileSystemError ) );

  RFs& fs = CEikonEnv::Static()->FsSession();

  TDesC8* data    = iImageArray[0]->SharedBuffer()->DataL( 0 );
  TBool   noSpace = SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 
                                                           data->Length(), 
                                                           drive );
  if( noSpace )
    {
    Cancel();
    PRINT( _L( "Camera <> CCamImageSaveActive::DoSaveL .. [WARNING] Disk space below critical, LEAVE!" ) );     
    User::Leave( KErrDiskFull );
    }

  // The dummy file is read only for protection, make it writable now
  // Also unhide it so that media gallery can detect it
  fs.SetAtt( filename, 0, KEntryAttReadOnly|KEntryAttHidden );

  // Open the file
  TInt err = iFile.Open( fs, filename, EFileWrite );

  // if the file does not exist, create a new one
  if ( err == KErrNotFound )
      {
      User::LeaveIfError( iFile.Create( fs, filename, EFileWrite ) );
      }
  // if there was any other problem then leave
  else if ( err != KErrNone )
      {
      User::Leave( err );
      }
  // the file exists
  else
      {
      // Make sure the file is at the start
      TInt seek = 0;
      User::LeaveIfError( iFile.Seek( ESeekStart, seek ) );
      }

  if( iController.CurrentMode() == ECamControllerVideo )
      {
      // set size 
      User::LeaveIfError( iFile.SetSize( data->Size() ) );
      }
  // Write the image data to file
  PRINT( _L( "Camera call iFile.Write" ) );     
  iFile.Write( *(data), iStatus );
  iSaveCurrent = ETrue;
  SetActive();
  PRINT( _L( "Camera <= CCamImageSaveActive::DoSaveL" ) );     
  }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::DoDisplayProgressNoteL
// Display the saving progress note
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::DoDisplayProgressNoteL()
    {
    PRINT( _L("Camera => CCamImageSaveActive::DoDisplayProgressNoteL") )
    __ASSERT_DEBUG( iSaveProgressDialog == NULL , CamPanic( ECamPanicUi ) );
    TBool reportOnExit = ETrue;
    // if there are images left to save in this burst set
    if ( iExpectedImages > iSaveProgressCount )
        {
        iSaveProgressDialog = new( ELeave ) CAknProgressDialog( 
            reinterpret_cast<CEikDialog**>( &iSaveProgressDialog ), ETrue );
        iSaveProgressDialog->PrepareLC( R_CAM_SAVING_IMAGES_PROGRESS_NOTE );
        iSaveProgressInfo = iSaveProgressDialog->GetProgressInfoL();
        iSaveProgressInfo->SetFinalValue( iExpectedImages );
        TInt i;
        for ( i = 0; i < iSaveProgressCount; i++ )
            {
            // Increment the progress in the bar for previous saves
            iSaveProgressInfo->IncrementAndDraw( 1 );
            }
        // display the dialog and wait until it is dismissed
        TInt exit = iSaveProgressDialog->RunLD();
        // the dialog has exited and deleted itself
        iSaveProgressDialog = NULL;
        iSaveProgressInfo = NULL;
        // if the dialog was cancelled by the user
        if ( exit == 0 )
            {
            reportOnExit = EFalse;
            iCompletedBurst = ETrue;                
            }
        }
    // if the burst has now completed
    if ( reportOnExit )
        {
        ReportToObserver( ECamSaveEventBurstComplete );            
        }
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::DoDismissProgressNote
// Dismiss the progress note, if it is still open
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::DoDismissProgressNote()
    {
    iDismissNote = EFalse;
    if ( iSaveProgressDialog )
        {
        // Delete the dialog and end its RunLD()
        TRAPD( error, iSaveProgressDialog->ProcessFinishedL() ) 
        if (error != KErrNone)
            {
            delete iSaveProgressDialog;
            iSaveProgressDialog = NULL;
            iSaveProgressInfo = NULL;
            }
        }
    // if the progress note is still pending
    if ( iShowNote )
        {
        // report that the burst has completed
        iShowNote = EFalse;
        ReportToObserver( ECamSaveEventBurstComplete );                   
        }
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::ReportToObserver
// Indicate to the observer that saving has finished
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::ReportToObserver( TCamSaveEvent aEvent )
    {
    if ( aEvent == ECamSaveEventComplete && iCompletedBurst )
        {
        aEvent = ECamSaveEventBurstComplete;
        iCompletedBurst = EFalse;
        }    
    iObserver.HandleSaveEvent( aEvent );    
    }


// -----------------------------------------------------------------------------
// ReleaseAllAndDestroy
//
// -----------------------------------------------------------------------------
//
void 
CCamImageSaveActive::ReleaseAllAndDestroy( RPointerArray<CCamBufferShare>& aArray )
  {
  PRINT( _L("Camera => CCamImageSaveActive::ReleaseAllAndDestroy") );

  for( TInt i = aArray.Count()-1; i >= 0; i-- )
    {
    CCamBufferShare* share = aArray[i];
    aArray.Remove( i );
    if( share )
      {
      share->Release();
      share = NULL;
      }
    }

  __ASSERT_DEBUG( aArray.Count() == 0, User::Invariant() );

  // All the data is released.
  // CCamBufferShare destructor is not public, so cannot call ResetAndDestroy.
  aArray.Reset();

  PRINT( _L("Camera <= CCamImageSaveActive::ReleaseAllAndDestroy") );
  }

#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
// -----------------------------------------------------------------------------
// HarvestingComplete
//
// Call Back method from the harvesting client once the harvesting is complete
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::HarvestingComplete( TDesC& aURI
#ifdef RD_MDS_2_5        
                                            , TInt aError 
#endif // RD_MDS_2_5
                                             )
    {
    PRINT( _L("Camera => CCamImageSaveActive::HarvestingComplete") );
#ifdef RD_MDS_2_5        
    PRINT1( _L("Camera <> aError = %d"), aError );

    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsThumbnailManagerAPISupported() && 
         iSnapArray.Count() &&
         iNameArray->Count() )
        {
        TRAP_IGNORE( DoCreateThumbnailL() ); 
        }

#endif // RD_MDS_2_5
    iObserver.HandleFileHarvestingComplete();
    PRINT( _L("Camera <= CCamImageSaveActive::HarvestingComplete") );
    }

//
//CCamImageSaveActive::DeRegisterHarverterClientEvents
//
void CCamImageSaveActive::DeRegisterHarverterClientEvents()
    {
    PRINT( _L("Camera => CCamImageSaveActive::DeRegisterHarverterClientEvents") );

    if ( iRegisteredForHarvesterEvents && iHarvesterClientConnected )
        {
        iHarvesterClient.RemoveObserver( this );
        iRegisteredForHarvesterEvents = EFalse;
        iHarvesterClient.Close();
        iHarvesterClientConnected = EFalse;
        }
    else
        {
        // do nothing
        }

    PRINT( _L("Camera <= CCamImageSaveActive::DeRegisterHarverterClientEvents") );
    }


//
//CCamImageSaveActive::RegisterForHarvesterEvents
//
void CCamImageSaveActive::RegisterForHarvesterEvents()
    {
    PRINT( _L("Camera => CCamImageSaveActive::RegisterForHarvesterEvents") );

    if ( !iRegisteredForHarvesterEvents && iHarvesterClientConnected )
        {
        iHarvesterClient.SetObserver( this );
        iRegisteredForHarvesterEvents = ETrue;
        }
    else
        {
        // do nothing
        }

    PRINT( _L("Camera <= CCamImageSaveActive::RegisterForHarvesterEvents") );
    }

#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)

// -----------------------------------------------------------------------------
// CreateThumbnailsL
//
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::CreateThumbnailsL( const CCamBurstCaptureArray& aArray )
    {
    PRINT( _L("Camera => CCamImageSaveActive::CreateThumbnailsL") );
    TInt arrayCount = aArray.Count();
    if ( !iSnapArray.Count() )
      {
      iThumbnailRequests.Reset();
      iThumbnailRequests.ReserveL( arrayCount );
      }
    TInt count(0);
    TInt err(0);
    
    for ( TInt i=0; i < arrayCount; i++ )
        {
        count = iNameArray->Count();        

        CFbsBitmap* snapshot = new( ELeave ) CFbsBitmap();
        if ( aArray.Snapshot( i ) )
            {
            err = snapshot->Duplicate( aArray.Snapshot( i )->Handle() );
            }
        // in case the snapshot is missing, the newly created empty bitmap indicates
        // the thumbnail is created using the imagefile    
        if ( !err )
            {
            TRAP( err, iSnapArray.AppendL( snapshot ) );            
            if ( !err ) 
                {
                //append filename (includes full path)
                PRINT1( _L("Camera <> CCamImageSaveActive::CreateThumbnails append file:%S"), &aArray.FileName( i ) );    
                TRAP( err, iNameArray->AppendL( aArray.FileName( i ) ) );
                if ( !err )
                    {
                    TRAP( err, iThumbnailRequests.AppendL( KErrNotFound ) );
                    }
                }
            if ( err  )
                {
                if ( iNameArray->Count() > count )
                    {
                    iNameArray->Delete( count );
                    }
                if ( iSnapArray.Count() > count )
                    {
                    iSnapArray.Remove( count );
                    }
                if ( iThumbnailRequests.Count() > count )
                    {
                    iThumbnailRequests.Remove( count );
                    }
                }
            }
        }     
    PRINT( _L("Camera <= CCamImageSaveActive::CreateThumbnailsL") );
    }

// -----------------------------------------------------------------------------
// CCamImageSaveActive::DoCreateThumbnail
// Add images to album
// -----------------------------------------------------------------------------
//
void CCamImageSaveActive::DoCreateThumbnailL()
    {
    PRINT( _L( "Camera => CCamImageSaveActive::DoCreateThumbnailL" ) );     
    if( !iManager )
        {
        PRINT( _L( "Camera <> CCamImageSaveActive::DoCreateThumbnail create manager" ) );         
        iManager = CThumbnailManager::NewL( *this );
        }
    // create object and call thumbnailmanager
    TSize size = iSnapArray[ 0 ]->SizeInPixels();
    CThumbnailObjectSource* source;
    if ( size.iHeight == 0 || size.iWidth == 0 )
        {
        PRINT( _L( "Camera <> CCamImageSaveActive::DoCreateThumbnail only URI" ) );         
        source = CThumbnailObjectSource::NewL( iNameArray->MdcaPoint( 0 ), 
                                               KNullDesC );
        }
    else
        {
        PRINT( _L( "Camera <> CCamImageSaveActive::DoCreateThumbnail URI and bitmap" ) );             
        source = CThumbnailObjectSource::NewL( iSnapArray[ 0 ], 
                                               iNameArray->MdcaPoint( 0 ) );
        }
    PRINT1( _L("Camera <> CCamImageSaveActive::DoCreateThumbnailL  filename:%S"), &iNameArray->MdcaPoint( 0 ) );        
    CleanupStack::PushL( source );        
    TThumbnailRequestId requestId = iManager->CreateThumbnails( *source );
    TInt index = iThumbnailRequests.Count() - iSnapArray.Count();
    PRINT2( _L( "Camera <> CCamImageSaveActive::DoCreateThumbnailL array index=%d, request id=%d" ), index, requestId );
    iThumbnailRequests[index] = requestId;
    CleanupStack::PopAndDestroy( source );

    iNameArray->Delete( 0 );
    iNameArray->Compress();

    iSnapArray.Remove( 0 );
    iSnapArray.Compress();

    if ( !IsActive() && iNameArray->Count() > 0  )
        {
        SetActiveAndCompleteRequest();
        }
    PRINT( _L( "Camera <= CCamImageSaveActive::DoCreateThumbnailL" ) );         
    }

void CCamImageSaveActive::CancelThumbnail( TInt aSnapshotIndex )
    {
    PRINT1( _L( "Camera => CCamImageSaveActive::CancelThumbnail - aSnapshotIndex=%d" ), aSnapshotIndex );
    if ( aSnapshotIndex >= iThumbnailRequests.Count() )
        {
        PRINT( _L( "Camera <= CCamImageSaveActive::CancelThumbnail request array not initialized yet" ) );
        return;
        }
    TThumbnailRequestId requestId = iThumbnailRequests[aSnapshotIndex];
    if ( requestId != KErrNotFound )
        {
        if ( iManager )
            {
            PRINT1( _L( "Camera <> CCamImageSaveActive::CancelThumbnail canceling request %d"), requestId );
            TInt err = iManager->CancelRequest( requestId );
            PRINT1( _L( "Camera <> CCamImageSaveActive::CancelThumbnail CancelRequest() returned %d"), err );
            iThumbnailRequests[aSnapshotIndex] = KErrNotFound;
            }
        }
    PRINT( _L( "Camera <= CCamImageSaveActive::CancelThumbnail" ) );
    }
void CCamImageSaveActive::ThumbnailPreviewReady( MThumbnailData& aThumbnail, 
                                                 TThumbnailRequestId aId )
    {
	// empty implementation
    }                                                 
          
void CCamImageSaveActive::ThumbnailReady( TInt aError, 
                                          MThumbnailData& aThumbnail, 
                                          TThumbnailRequestId aId )
    {
    PRINT1( _L( "Camera => CCamImageSaveActive::ThumbnailReady aId=%d" ), aId );
    for ( TInt i = 0; i < iThumbnailRequests.Count(); i++ )
        {
        if ( iThumbnailRequests[i] == aId )
            {
            PRINT1( _L( "Camera <> CCamImageSaveActive::ThumbnailReady thumbnail for snapshot %d ready" ), i );
            iThumbnailRequests[i] = KErrNotFound;
            }
        }
    PRINT( _L( "Camera <= CCamImageSaveActive::ThumbnailReady" ) );
    }
                                              
//  End of File  
