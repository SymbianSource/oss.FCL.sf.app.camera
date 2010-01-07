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


#ifndef CAMBURSTTHUMBNAILVIEW_H
#define CAMBURSTTHUMBNAILVIEW_H

//  INCLUDES
#include "CamViewBase.h"
#include "CamPostCaptureViewBase.h"
#include "CamBurstThumbnailGridModel.h"
#include "AiwServiceHandler.h"  // for CAiwServiceHandler

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  View class to display a grid of thumbnails representing the images captured by
*  a single burst capture operation
* 
*  @since 2.8
*/
class CCamBurstThumbnailView :  public CCamPostCaptureViewBase, 
                                public MThumbModelObserver,
                                public MAiwNotifyCallback
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aController Reference to either the application controller 
        *        base class or test base class
        * @return pointer to the created CCamBurstThumbnailView object
        */
        static CCamBurstThumbnailView* NewLC( CCamAppController& aController );
        
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamBurstThumbnailView();

    public: // New functions

        /**
        * Shows the delete note for the required number of files, and if the 
        * user confirms, will trigger the deletion of the marked files
        * @since 2.8
        * @return ETrue if ok to delete, else EFalse
        */
        TBool DisplayDeleteNoteL();
        
        /**
        * Deletes the currently marked or highlighted items
        * @since 3.0
        */
        void DoDeleteL();

    public: // Functions from base classes

        /**
        * From CAknView.
        * @since 2.8
        * @return UID of view
        */
        TUid Id() const;

        /**
        * From CAknView Handle commands        
        * @since 2.8                
        * @param aResourceId Resource Id of the menu 
        * @param aMenuPane Pointer to the pane showing the menu
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CAknView Handle commands
        * @since 2.8
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView Handles the foreground/background event
        * @since 2.8
        * @param aForeground ETrue if this view is on foreground, else EFalse.
        */
        void HandleForegroundEventL( TBool aForeground );

        /**
        * From CAknView.
        * @since 2.8
        * @param aPrevViewId the ID for previous view we are switching from
        * @param aCustomMessageId the Uid for message passed to this view
        * @param aCustomMessage descriptor containing data content for view specific message
        */
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                                                    const TDesC8& aCustomMessage );
        /**
        * From base class MAiwNotifyCallback.
        * Handles any notification caused by asynchronous ExecuteCommandL
        * or event.
        * @param aCmdId Service command associated to the event.
        * @param aEventId Occured event
        * @param aEventParamList Event parameters, if any, as defined per
        * each event.
        * @param aInParamList Input parameters, if any, given in the
        *        related HandleCommmandL
        * @return error code for the callback
        */
        TInt HandleNotifyL(
            TInt aCmdId,
            TInt aEventId,
            CAiwGenericParamList& aEventParamList,
            const CAiwGenericParamList& aInParamList );


        /**
        * From CAknView.
        * @since 2.8
        */
        void DoDeactivate();

        /**
        * From MThumbModelObserver.
        * Called when the highlight has changed
        * @since 2.8                
        */
        void HighlightChanged();

        /**
        * From MThumbModelObserver.
        * Called when files have been deleted
        * @since 2.8                        
        */
        void ImagesDeleted();

        /**
        * Check if renaming to the given name is valid.
        * @since 2.8
        * @param aFilename The new name to check.
        * @return ETrue if the name is valid.
        */
        TBool IsNewNameValidL( const TDesC& aFilename );
        
        
        /**
        * Returns whether the view is a burst capture postcapture view or not
        * @since 3.0
        * @returns ETrue if view is burst post capture postcapture
        */
        TBool IsBurstPostCapture();
        
        
        /**
         * Notifies view that image file(s) have been deleted outside of camera
         */
        void ImageFilesDeleted();
        

    protected:  // New functions
        
        /**
        * C++ default constructor.
        * @since 2.8
        * @param aController Reference to either the application controller 
        * base class or test base class
        */
        CCamBurstThumbnailView( CCamAppController& aController );

        /**
        * Symbian 2nd phase constructor.
        * @since 2.8
        */
        void ConstructL();
        
        /**
        * Create message.
        * @since 2.8
        */
        virtual void DoSendAsL() const;

        /**
        * If no items are marked, returns the ID of the currently
        * highlighted item.  Otherwise returns the ID of the first
        * marked item (to be called when up to 1 items are marked).
        * @return Index of item that is marked / highlighted 
        * @since 3.0
        */
        TInt SingleSelectionGridIndexL() const;

#ifndef __WINS__
        /**
        * Send file to current caller using SFI functionality
        * @since 2.8
        */
        //virtual void DoInCallSendL() const;
#endif

        /**
        * Dim one-click upload button if more than one image is marked.
        * @since S60 v5.0
        */
        void UpdateOneClickUploadButton();

        /**
         * Handles dynamic initialisation for aiw menus
         * @since 9.1
         */
        void DynInitAiwMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
    private:    // Functions from base classes

        /**
        * From CCamViewBase
        * Create the container associated with this view.
        * @since 2.8
        */
        void CreateContainerL();

        /**
        * From CCamViewBase
        * Set the view's title text
        * @since 2.8
        */
        void SetTitlePaneTextL();
        
        /**
         * Calculates the size of a single thumbnail based on the infromation 
         * how many images are alltogether on the burst grid
         * @param aNumberOfBurstImages Total amount of burst images
         */
        TSize CalculateThumbnailSize( TInt aNumberOfBurstImages );

    private:
        // The model describing the contents of the thumbnail grid
        CCamBurstThumbnailGridModel* iGridModel;

        // Whether we should reset the model when view deactivated
        TBool iResetModelOnDeactivate;
    };

#endif      // CAMBURSTTHUMBNAILVIEW_H   
            
// End of File
