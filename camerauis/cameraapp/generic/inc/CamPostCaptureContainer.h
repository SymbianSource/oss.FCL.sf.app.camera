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
* Description:  Container for still Post capture view
*
*/



#ifndef CAMPOSTCAPTURECONTAINER_H
#define CAMPOSTCAPTURECONTAINER_H

// INCLUDES
#include "CamContainerBase.h"
#include "CamControllerObservers.h"

#include "cambatterypanecontroller.h"

// FORWARD DECLARATIONS
class CCamAppController;
class CAknView;

// CLASS DECLARATION

/**
* Container for still image Post capture view
*/
class CCamPostCaptureContainer : public CCamContainerBase,
                                 public MCamControllerObserver,
                                 public MCamBatteryPaneObserver
    {
    public:
    
    enum TCamPostCaptureSubControls
        {
        ECamPostCaptureSubControlNaviPane = 1,
        ECamPostCaptureActivePalette
        };
    
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aController reference to CCamAppControllerBase instance
        * @param aView reference to the view containing this container
        * @param aRect Frame rectangle for container.
        */
        static CCamPostCaptureContainer* NewL( CCamAppController& aController, 
                                               CAknView& aView,
                                               const TRect& aRect );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamPostCaptureContainer();

    private:
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to CCamAppControllerBase instance
        * @param aView reference to the view containing this container
        */
        CCamPostCaptureContainer( CCamAppController& aController,
                                  CAknView& aView );
                                  
    public: // Functions from base classes
        /**
        * From CamContainerBase 
        * @since 2.8
        * @param aKeyEvent the key event
        * @param aType the type of the event
        * @return TKeyResponse key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
        * Will show the zoom pane for a set time
        * Only implemented by pre capture containers
        * @since 2.8
        */
		void ShowZoomPaneWithTimer();
        
        /**
        * We check for the file name whenever we call the UMP
        * or Properties view and come back.
        */
        void CheckForFileName( TBool aDoCheck );
        
        /**
        * Requests that the camera application redraws the supplied area
        * @since 3.0
        * @param aArea the area to redraw
        */
        virtual void Redraw(const TRect& aArea);

        /**
        * From CCamContainerBase / CCoeControl
        * Locally takes actions needed due to resource changes, then
        * calls the implementation in CCamContainerBase        
        * @since 2.8
        * @param aType resource change
        */
		virtual void HandleResourceChange( TInt aType );		        

        /**
        * Handle application level event.
        * For example focus gained and lost are notified through this method.
        * @param aEvent Event type
        */
        virtual void HandleAppEvent( const TCamAppEvent& aEvent );
        
        /**
        * From MCamBatteryPaneObserver
        * Called when battery pane has changed and redraw is needed
        * @since 3.1
        */		
		void BatteryPaneUpdated();
		
	    /**
	    * From CoeControl
    	* @param aPointerEvent the pointer event to handle
	    */
		void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    protected: // Functions from base classes
        /**
        * From CoeControl
        * @since 2.8
        * @return number of contained controls
        */
        TInt CountComponentControls() const;
        
        /**
        * From CoeControl
        * @since 2.8
        * @return number of contained controls
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl
        * @since 2.8
        * @param aRect area where to draw
        */
        void Draw( const TRect& aRect ) const;


        /**
        * From MCamControllerObserver
        * @since 2.8
        * @param aEvent The enumerated code for the event received
        * @param aError The error code associated with the event
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );

    private: // Data
    // The full screen dimensions
	TSize iScreenSize;
	
	CCamBatteryPaneController* iBatteryPaneController;
	TBool iCheckForFileNameChange;
    
	TBool iUpdatingBatteryPane; //Needed for drawing black background behind battery pane in Video mode
	
	// Play icon bitmaps
	CFbsBitmap* iPlayIcon;
	CFbsBitmap* iPlayIconMask;
	
	CFbsBitmap* iPlayIconBg;
	CFbsBitmap* iPlayIconBgMask;
	
	CFbsBitmap* iPlayIconBgPressed;
	CFbsBitmap* iPlayIconBgPressedMask;
	
	//Bools to control play button feedback
	TBool iPlayIconPressed;
	TBool iDraggedInAlready;
    };

#endif

// End of File
