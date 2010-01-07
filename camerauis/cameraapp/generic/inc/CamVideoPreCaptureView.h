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
* Description:  View class for video pre capture view*
*/


#ifndef CAMVIDEOPRECAPTUREVIEW_H
#define CAMVIDEOPRECAPTUREVIEW_H

//  INCLUDES
#include "CamPreCaptureViewBase.h"


// FORWARD DECLARATIONS
class CAknButton;
class MAknsSkinInstance;
class TAknsItemID;

// CLASS DECLARATION

/**
*  View class for video capture 
*
*  @since 2.8
*/
class CCamVideoPreCaptureView : public CCamPreCaptureViewBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aController Reference to either the application controller 
        *        base class or test base class
        * @since 2.8
        * @return pointer to the created CCamVideoPreCaptureView object
        */
        static CCamVideoPreCaptureView* NewLC( CCamAppController& aController );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamVideoPreCaptureView();

    public: // Functions from base classes

        /**
        * From CAknView.
        * @since 2.8
        * @return UID of view
        */
        TUid Id() const;

        /**
        * From CAknView.
        * @since 2.8
        */
        void DoDeactivate();

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
        * From CCamViewBase
        * Handle change of focus to another application
        * This only handles another application coming to the foreground when
        * the camera application is already in the background behind an eikon
        * server window
        * @since 2.8
        */
        void HandleFocusLossL();

        /**
        * From MCamControllerObserver
        * @since 2.8
        * @param aEvent The enumerated code for the event received
        * @param aError The error code associated with the event
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );

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
        * From CCamViewBase.
        * Called to identify the help context for this view
        * @since 2.8
        * @param aContext Help context object to populate with control context
        * information
        */
        virtual void GetHelpContext( TCoeHelpContext& aContext ) const;
        
		/**
        * From CCamViewBase
        * Update the softkeys for the current situation.
        * @since 2.8
        */
        void UpdateCbaL();  

    protected:  // Functions from base classes
  
        /**
        * From CCamPreCaptureViewBase 
        * Set the menu bar resource.
        */
        virtual void SetMenuBar();

        /**
        * From CCamPreCaptureViewBase Handle commands
        * Creates a video capture setup menu
        * @since 2.8
        */
        void CreateCaptureSetupMenuL( );

        /**
        * Enter scene setting mode.
        * Creates a container to replace the normal container on the control stack. 
        * Updates the Cba, title pane and navi pane.
        * @since 2.8
        */
        void SwitchToSceneSettingModeL();
        
        
       /*
        *  Updates Video flash icon if supported
        */
        void UpdateVideoFlashIconsL();

       /*
        *  Updates Video flash icon if supported
        */
        void UpdateVideoSceneModeIconsL();

       /*
        *  Updates Video flash icon if supported
        */
        void UpdateVideoColorToneIconsL();

       /*
        *  Updates Video flash icon if supported
        */
        void UpdateVideoWhitebalanceIconsL();

    public:     // New functions		

        /**
        * Update fixed toolbar icons according to current settings
        * @since S60 5.0
        */
        void UpdateToolbarIconsL();

        /**
        * Try to start the capture with MSK command. 
        * @since 9.1
        * @return ETrue if started.
        */
        TBool StartMskCaptureL();

    private:    // New functions

        /**
        * C++ default constructor.
        * @param aController Reference to either the application controller 
        *        base class or test base class
        * @since 2.8
        */
        CCamVideoPreCaptureView( CCamAppController& aController );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 2.8
        */
        void ConstructL();
        
    private:    // Functions from base classes

        /**
        * From CCamViewBase
        * Set the view's title text
        * @since 2.8
        */
        void SetTitlePaneTextL();

        /**
        * From CCamPreCaptureViewBase
        * Enter Viewfinder mode
        * @since 2.8
        */
        void StartViewFinder();

        /**
        * From CCamPreCaptureViewBase
        * Exit Viewfinder mode
        * @since 2.8
        */
        void StopViewFinder();
        
        /**
        * From CCamViewBase
        * Create the container associated with this view.
        * @since 2.8
        */
        void CreateContainerL();

        /**
        * Mute or unmute audio in embedded video
        * @since 2.8
        * @param aMuteAudio true or false
        */
        void MuteEmbeddedAudioL( TBool aMuteAudio );

// from base class MAknToolbarObserver

        void DynInitToolbarL( TInt aResourceId, CAknToolbar* aToolbar );

        void OfferToolbarEventL( TInt aCommand );

    private:    // From MEikMenuObserver
        /**
        * From MEikMenuObserver Changes menu pane dynamically
        * @param Resource Id
        * @param Handle to menu pane
        * @since 2.8
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:
        // Whether audio is enabled for video
        // Have mute/unmute option in embedded video menu
        TInt iAudioEnabled;
        
    };

#endif      // CAMVIDEOPRECAPTUREVIEW_H 

// End of File
