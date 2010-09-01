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




#ifndef CAMIMAGESAVEACTIVE_H
#define CAMIMAGESAVEACTIVE_H

//  INCLUDES
 
#include <e32base.h>
#include <badesca.h>
#include <f32file.h>
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>

#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
#include <harvesterclient.h>
#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)

// DATA TYPES
/**
* Save event type
*/
enum TCamSaveEvent
    {
    ECamSaveEventStarted,
    ECamSaveEventComplete,
    ECamSaveEventBurstComplete,
    ECamSaveEventUserCancelled,
    ECamSaveEventDeleted,
    ECamSaveEventSaveError, 
    ECamSaveEventCriticalError
    };

// CONSTANTS
#ifdef _DEBUG
const TUint16* const KCamSaveEventNames[] =
  {
  (const TUint16* const)_S16("ECamSaveEventStarted"),
  (const TUint16* const)_S16("ECamSaveEventComplete"),
  (const TUint16* const)_S16("ECamSaveEventBurstComplete"),
  (const TUint16* const)_S16("ECamSaveEventUserCancelled"),
  (const TUint16* const)_S16("ECamSaveEventDeleted")
  };
#endif

// FORWARD DECLARATIONS
class CAknProgressDialog;
class CEikProgressInfo;
class CCamBufferShare;
class CCamAppController;
class CCamBurstCaptureArray;

// CLASS DECLARATIONS

/**
* Abstract API for image save observer. 
* Provides an asynchronous 'callback' method to inform derived classes
* that the save operation has been completed.
*
*  @since 2.8
*/
class MCamImageSaveObserver
    {
    public:
        /**
        * The image saving operation has completed
        * @since 2.8
        * @param aEvent the type of event
        */
        virtual void HandleSaveEvent( TCamSaveEvent aEvent ) = 0;
        
        /**
        *
        */
        virtual void HandleFileHarvestingComplete() = 0;
    };

/**
*  Class to manage the asynchronous saving and synchronous deletion 
*  of images.
*  @since 2.8
*/
class CCamImageSaveActive : public CActive
#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
                            ,private MHarvestObserver
#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
                            , public MThumbnailManagerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aObserver Informed when image saving has completed
        */
        static CCamImageSaveActive* NewL( MCamImageSaveObserver& aObserver, 
                                          CCamAppController&  aController );
        
        /**
        * Destructor.
        */
        virtual ~CCamImageSaveActive();

    public: // Functions from base classes

        /**
        * From CActive, cancels the active object
        * Deletes all files remaining in the save arrays except any currently
        * saving file. If currently saving, the active object waits until the 
        * save completes then performs cleanup.. This method is synchronous 
        * even when it involves a wait for save completion.
        * @since 2.8
        */
        void Cancel();

    public: // New functions
    
        /**
        * A request for a file to be saved asynchronously, will be scheduled 
        * immediately if no other save is current, otherwise the save is queued
        * @since 2.8
        * @param aFilename the full path and filename to save the image to
        * @param aImageData the image data to save
        * @return Whether or not the image data has been successfully added
        * to the pending save list.
        */
        TBool AddToSave( const TDesC&     aFilename, 
                         CCamBufferShare* aImageData );

        /**
        * A request for a file to be deleted. Will prevent any pending save. 
        * If the file to delete is currently saving the deletion will occur
        * when the save operation completes.
        * @since 2.8
        * @param aFilename the full path and name of the file to be deleted
        * @param aSaveRequested whether or not a previous request to save this file
        *                     has been made.
        * @return System wide error code to indicate success or failure of the deletion
        */
        TInt DeleteFile( const TDesC& aFilename, TBool aSaveRequested );

        /**
        * Returns whether a particular file has already been saved.
        * @since 2.8
        * @param aFilename the name of the file to check.
        * @return ETrue if the file has been saved.
        */   
        TBool AlreadySavedFile( const TDesC& aFilename ) const;

        /**
        * Returns whether a particular file is in the process of being saved.
        * @since 2.8
        * @param aFilename the name of the file to check.
        * @return ETrue if the file is in the process of being saved.
        */   
        TBool CurrentlySavingFile( const TDesC& aFilename ) const;
        
        /**
        * A request for the progress note to be displayed
        * @since 2.8
        * @param aExpectedImages the number of images to be saved
        */
        void DisplayProgressNote( TInt aExpectedImages );

        /**
        * A request for the progress note to be dismissed when the save completes
        * @since 2.8
        */
        void DismissProgressNote();

        /**
        * Set the "Add to album" setting for photos
        * determines whether or not photos are added to the default capture album
        * after a successful save
        * @since 2.8
        * @param aAdding ETrue if "Add to album" is on, otherwise EFalse
        */
        void SetAddImageToAlbum( const TBool aAdding, 
                                 const TUint32 aDefaultAlbumId );

        /**
        * Add the specified file to the default capture album
        * Called internally for images when save completes. Called externally for
        * videos as video saving is handled elsewhere. Calls AddToAlbumL().
        * @since 2.8
        * @param aFilename The photo or video to add to the capture album
        * @param aPhoto Indicates if the specified file is a photo or video
        */
        void AddToAlbum( const TDesC& aFilename, 
                         const TBool aPhoto, 
                         const TUint32 aDefaultAlbumId );

        /**
        * The number of items in the saving arrays
        * @since 2.8
        * @return A count of items
        */
        TInt Count() const;

        /**
        * Retrieves the file size of the specified file.        
        * @since 3.0
        * @param aFilename The file to return the size of.
        * @return The size of the file in bytes, or KErrNotFound
        */                
        TInt FileSize( const TDesC& aFilename ) const;

        /**
        * Stores filename and snapshot handle for thumbnail creation
        */
        void CreateThumbnailsL( const CCamBurstCaptureArray& aArray );
                
        /**
        * Calls thumbnailmanager to create thumbnails
        */
        void DoCreateThumbnailL();

        /**
         * Cancels thumbnail creation for a given snapshot.
         * @param aSnapshotIndex Index of the snapshot in burst array.
         */
        void CancelThumbnail( TInt aSnapshotIndex );
        
        void ForceCancel();
        /**
        *  From MThumbnailManagerObserver, not used
        */
        void ThumbnailPreviewReady( MThumbnailData& aThumbnail, 
                                    TThumbnailRequestId aId );
                                    
        /**
        *  From MThumbnailManagerObserver, not used
        */  
        void ThumbnailReady( TInt aError, 
                             MThumbnailData& aThumbnail, 
                             TThumbnailRequestId aId );
        
#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
        /*
        * RegisterForHarvesterEvents
        */
        void RegisterForHarvesterEvents();
        
        /*
        * DeRegisterHarverterClientEvents
        */
        void DeRegisterHarverterClientEvents();

    private: // From MHarvestObserver

        /*
        * HarvestingComplete
        */
        void HarvestingComplete( TDesC& aURI
#ifdef RD_MDS_2_5             
                               , TInt aError
#endif //RD_MDS_2_5                                 
                                );

#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)

    private:

        /**
        * Constructor
        * @since 2.8
        * @param aObserver Informed when image saving has completed
        */
        CCamImageSaveActive( MCamImageSaveObserver& aObserver,  
                             CCamAppController&  aController );

        /**
        * 2nd phase construction
        * @since 2.8
        */
        void ConstructL();

        /**
        * Schedule to run immediately
        * @since 2.8
        */
        void SetActiveAndCompleteRequest();

        /**
        * Cancels the active object
        * @since 2.8
        */
        void DoCancel();

        /**
        * Perform the next scheduled task
        * @since 2.8
        */
        void RunL();

        /**
        * Handle leave in RunL.
        * @param aError The error (leave) code from RunL.
        */
        TInt RunError( TInt aError );

        /**
        * Tidy up after a previous save has completed
		* @param aDoFlush Should an asyncrhonous flush be done
        * @since 2.8
        */
        void CompleteSaveOperation( TBool aDoFlush );

        /**
        * Carry out the next pending save operation
        * @since 2.8
        */
        void DoSaveL();

        /**
        * Display the progress note
        * @since 2.8
        */
        void DoDisplayProgressNoteL();

        /**
        * Dismiss the progress note when the saves have completed
        * @since 2.8
        */
        void DoDismissProgressNote();

        /**
        * Indicate to observers that saving has completed
        * @param aEvent the cause of completion
        * @since 2.8
        */
        void ReportToObserver( TCamSaveEvent aEvent );
        
        /**
        * Helper method to release all the shared buffers in the array.
        */
        void ReleaseAllAndDestroy( RPointerArray<CCamBufferShare>& aArray );
   
       
    private:

        MCamImageSaveObserver& iObserver; // informed when image saving has completed      
        CCamAppController& iController;

        CDesCArray* iSaveArray;                     // reserved filenames 
        RPointerArray<CCamBufferShare> iImageArray; // image data (order reflects iSaveArray)
        CDesCArray* iNameArray;                     // filenames for thumbnail creation
        RPointerArray<CFbsBitmap> iSnapArray;       // snapshots for thumbnail creation
        CThumbnailManager* iManager;                // TN manager
                
        RFile iFile;                  // Handle to a file object
        TBool iSaveCurrent;           // Indicates if a saving operation is ongoing
        TBool iDeleteCurrent;         // If the currently saving file needs deleting
        TBool iDoCancel;              // If further saves should be cancelled
        TBool iShowNote;              // If the progress note should be displayed
        TBool iDismissNote;           // If the progress note should be dismissed 
                                      // when all saves complete
        TInt iExpectedImages;         // The number of images expected for progress info
        TInt iSaveProgressCount;      // The count of images saved for this progress
        TBool iCompletedBurst;        // If a set of burst images has completed
        CActiveSchedulerWait iWait;   // To make asychronous synchronous during cancellation
        // Progress note shown when the applicaiton must wait for saving to complete
        CAknProgressDialog* iSaveProgressDialog;
        // The progress bar shown in iSaveProgressDialog
        CEikProgressInfo* iSaveProgressInfo;
        TBool iAddImageToAlbum;       // Whether or not images should be added to album
        TUint32 iDefaultAlbumId;      // To store the default album id from appcontroller
        TBool iFlushing;
        RArray<TThumbnailRequestId> iThumbnailRequests; // thumbnail request ids

#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
        RHarvesterClient iHarvesterClient;
        TBool iRegisteredForHarvesterEvents;
        TBool iHarvesterClientConnected;
#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
    };

#endif      // CAMIMAGESAVEACTIVE_H
            
// End of File
