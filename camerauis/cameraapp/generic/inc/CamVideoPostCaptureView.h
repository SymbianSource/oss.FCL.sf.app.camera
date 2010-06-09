/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  View class for video post capture view*
*/


#ifndef CAMVIDEOPOSTCAPTUREVIEW_H
#define CAMVIDEOPOSTCAPTUREVIEW_H

//  INCLUDES
#include "CamPostCaptureViewBase.h"

// FORWARD DECLARATIONS
class CCamAppController;
class CCamContainerBase;
class CCamTimer;

// CLASS DECLARATION

/**
*  View class for video capture 
*
*  @since 2.8
*/
class CCamVideoPostCaptureView : public CCamPostCaptureViewBase, public MAiwNotifyCallback
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aController Reference to either the application controller 
        *        base class or test base class
        * @return pointer to the created CCamVideoPostCaptureView object
        */
        static CCamVideoPostCaptureView* NewLC( CCamAppController& aController );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamVideoPostCaptureView();

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
        * @param aCommand command to be handled
        */
        void ProcessCommandL( TInt aCommand );
        
        
        /**
        * From CAknView Handle commands
        * @since 2.8
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * Displays the delete image/video confirmation note
        * @since 2.8
        * @return ETrue if file deleted, otherwise EFalse
        */
        TBool DisplayDeleteNoteL();
        
        /**
        * From CAknView.
        * @since 2.8
        * @param aPrevViewId the ID for previous view we are switching from
        * @param aCustomMessageId the Uid for message passed to this view
        * @param aCustomMessage descriptor containing data content for view specific message
        */
        void DoActivateL( const TVwsViewId& aPreViewId, TUid aCustomMessageId,
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

		/*
		* Hdmi Timer callback
		*/        
        static TInt HdmiTimerCallback( TAny* aSelf );

		/*
		* Hdmi Timer callback handler
		*/        
        void DoHdmiTimerCallbackL();
    
    private:    // New functions

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aController Reference to either the application controller 
        *        base class or test base class
        */
        CCamVideoPostCaptureView( CCamAppController& aController );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 2.8
        */
        void ConstructL();

    private:    // Functions from base classes

        /**
        * From CAknView.
        * @since 2.8
        */
        void DoDeactivate();

        /**
        * From CCamViewBase
        * Set the view's title text
        * @since 2.8
        */
        void SetTitlePaneTextL();

    private:    // From MEikMenuObserver
        /**
        * From MEikMenuObserver Changes MenuPane dynamically
        * @param Resource Id
        * @param Handle to menu pane
        * @since 2.8
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        // from base class MAknToolbarObserver

        void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );



    private:    // Data
    
    	TBool iRockerKeyPress;
    	
    	CCamTimer* iHdmiTimer;
    };

#endif      // CAMVIDEOPOSTCAPTUREVIEW_H   
            
// End of File
