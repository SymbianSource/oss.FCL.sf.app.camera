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
* Description:  Base class for Post capture views*
*/


#ifndef CAMPOSTCAPTUREVIEWBASE_H
#define CAMPOSTCAPTUREVIEWBASE_H

//  INCLUDES
 
#include "CamViewBase.h"
#include "MCamAddToAlbumObserver.h"
#include "CamControllerObservers.h"
#include "AiwServiceHandler.h"  // for CAiwServiceHandler
#include <sendui.h>
#include <akntoolbarobserver.h>


// FORWARD DECLARATIONS
class CSendUi;
class CCamOneClickUploadUtility;

#ifndef __WINS__
class CSFIUtilsAppInterface;
#endif // !defined(__WINS__) 
// CLASS DECLARATION

class CCamCollectionManagerAO;
class MCamAddToAlbumObserver;

/**
*  Base class for post capture views
*  
*  @since 2.8
*/
class CCamPostCaptureViewBase : public CCamViewBase,
                                public MCamControllerObserver,
                                public MAknToolbarObserver            
                                ,public MCamAddToAlbumObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamPostCaptureViewBase();

    public: // Functions from base classes

        /**
        * From CAknView Handle commands
        * @since 2.8
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From MCamControllerObserver
        * @since 2.8
        * @param aEvent The enumerated code for the event received
        * @param aError The error code associated with the event
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );

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
        * From CAknView.
        * @since 2.8
        */
        void DoDeactivate();

        /**
        * From MEikMenuObserver Changes MenuPane dynamically
        * @param aResourceId Resource Id
        * @param aMenuPane Handle to menu pane
        * @since 2.8
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );


    public: // From MCamAddToAlbumObserver
        /*
        * From MCamAddToAlbumObserver
        * Notification once the aysnchronous operations using
        * CollectionManager interface are done completly
        * Status of the operation are returned using aStatus
        * e.g. KErrNone
        */
        void AddToAlbumIdOperationComplete();
        
        /**
        * From MCamAddToAlbumObserver
        * Notification once the asynchronous operations using
        * CollectionManager interface have completed.
        *
        * @param aAlbumExists ETrue if album exists
        * @param aAlbumTitle  Title of the album
        */
        void CheckAlbumIdOperationCompleteL(
                TBool /*aAlbumExists*/, const TDesC& /*aAlbumTitle*/ ) {}

    public: // From MCamFileHarvestingObserver
        /**
        * From MCamFileHarvestingObserver
        * Notification once the aysnchronous operations using
        * harverter client interface are done completly
        * the file that is being harvested is sent as paramerter
        */
        void HandleHarvestingComplete( const TDesC& aUri );
        

    public:  // New functions

        /**
        * Displays the delete image/video confirmation note
        * @since 2.8
        * @return EFalse if the user cancels the deletion, otherwise ETrue
        */
        virtual TBool DisplayDeleteNoteL() = 0;
        
        /**
        * Update fixed toolbar icons according to current call state
        * @since 5.0
        */
        void UpdateToolbarIconsL();

    protected:  // New functions
        
        /**
        * C++ default constructor.
        * @since 2.8
        * @param aController Reference to either the application controller 
        * base class or test base class
        */
        CCamPostCaptureViewBase( CCamAppController& aController );

        /**
        * Symbian 2nd phase constructor.
        * @since 2.8
        */
        void ConstructL();

        
    private:    // Functions from base classes

        /**
        * From CCamViewBase
        * Create the container associated with this view.
        * @since 2.8
        */
        void CreateContainerL();

    private:
       /*
        * Create Active Palette once the image/video is saved.
        */
        void UpdateActivePaletteItemsL();

    protected:
        /*
        * Create message of specified by
        * @param aCommand represents desired message type (eamil, MMS, BT )
        * @since 2.8
        */
        virtual void DoSendAsL() const;

#ifndef __WINSCW__
        /**
        * Send file to current caller using SFI functionality
        * @since 2.8
        */
        virtual void DoInCallSendL() const;
#endif //!WINSCW

        /**
        * Returns whether the view is a postcapture view or not
        * @since 3.0
        * @returns ETrue
        */
        virtual TBool IsPostCapture();

        /**
        * Hides or shows the 'Send' and 'SendAs' items in the active toolbar
        * @since 3.0
        * @param aApHandler handler for the active toolbar
        */
        void ResetSendAvailabilityL( CCamActivePaletteHandler* aApHandler );

// from base class MAknToolbarObserver

        void OfferToolbarEventL( TInt aCommand );

    protected:
    
        CSendUi* iSendAppUi; 
        TSendingCapabilities iSendingCapabilities;     
        
#ifndef __WINS__
        CSFIUtilsAppInterface* iSFIUtils;
#endif
        CArrayFixFlat<TUid>* iSendMtmsToDim;

        // set to EFalse when still image save completes
        TBool iWaitForImageSave;        

        // AIW handler for dynamic menu options.
        CAiwServiceHandler* iAiwServiceHandler;

        // Utility for handling one-click upload operations.
        // Own.
        CCamOneClickUploadUtility* iOneClickUploadUtility;

    private: // New Methods
		/**
        * Complete add to album request
        * @since 3.1
        */
        void StartAddToAlbumOperationL();
        
         /*
        * Shows the radiobutton setting page to 
        * actually proceed with add to album operation
        */
        TInt ShowAddToAlbumConfirmationQueryL(); 

    private:
        CCamCollectionManagerAO* iCollectionManagerCallBack;
        TBool iAddToAlbumRequestOngoing;
        TBool iPreviousViewStillPreCapture;
        // suppress postcapture AP until camera ready
        TBool iSuppressAPUntilCameraReady;
        TBool iPartialBackground;
    };

#endif      // CAMPOSTCAPTUREVIEWBASE_H   
            
// End of File
