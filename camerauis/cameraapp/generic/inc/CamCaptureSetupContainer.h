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
* Description:  Class for displaying capture set-up controls*
*/


#ifndef CAMCAPTURESETUPCONTAINER_H
#define CAMCAPTURESETUPCONTAINER_H

// INCLUDES
#include <AknUtils.h>
#include <eiklbo.h>
 
#include "CamContainerBase.h"
#include "CamControllerObservers.h"
#include "mcamcameraobserver.h"


// FORWARD DECLARATIONS
class CCamCaptureSetupControlHandler;
class MTouchFeedback; 


// CLASS DECLARATION

/**
* Container base class
*/
class CCamCaptureSetupContainer : public CCamContainerBase,
                                  public MCamCameraObserver,
                                  public MEikListBoxObserver
//                                public MCamViewFinderObserver
  {
  public: // Constructors and destructor

    /**
    * Symbian OS two-phased constructor
    * @since 2.8
    * @param aController Controller reference
    * @param aView View reference
    * @param aControlHandler Handler reference
    * @param aRect Frame rectangle for container.
    */
    static CCamCaptureSetupContainer* NewL( 
              CCamAppController&              aController, 
              CAknView&                       aView,
              CCamCaptureSetupControlHandler& aControlHandler,
              const TRect&                    aRect  );
        
    /**
    * Destructor.
    * @since 2.8
    */
    virtual ~CCamCaptureSetupContainer();
        

  // -----------------------------------------------------
  // From CCoeControl
  private:  

    /**
    * @since 2.8
    * @return number of contained controls
    */
    TInt CountComponentControls() const;
    
    /**
    * @since 2.8
    * @param aIndex the index of the control
    * @return pointer to the component control
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
    * @since 2.8
    * @param aRect area where to draw
    */
    void Draw( const TRect& aRect ) const;

    /**
    * @since 2.8
    * @param aKeyEvent the key event
    * @param aType the type of the event
    * @return TKeyResponse key event was used by this control or not
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                       TEventCode aType );
    
    /*
     * From CoeControl
     * @param aPointerEvent the pointer event to handle
     */
    void HandlePointerEventL(  const TPointerEvent& aPointerEvent );
    /**
    * From MEikListBoxObserver
    */
    void HandleListBoxEventL(CEikListBox* aListBox, 
                             TListBoxEvent aEventType );

  // -----------------------------------------------------
  // From MCamCameraObserver 
  public: 

      
    /**
      * Handle events sending app to foreground or background
      * @param aForeground whether the event brings the app to fore or background
    */   
      
      void HandleForegroundEventL( TBool aForeground );
      
      
      /**
    * @since Camera 4.0
    * @see MCamCameraObserver
    */
    virtual void HandleCameraEventL( TInt              aStatus, 
                                     TCamCameraEventId aEventId, 
                                     TAny*             aEventData = NULL );

  // -----------------------------------------------------
  // Other methods
  private:

    /**
    * Calculates the position for the settings editing control
    * @since 2.8
    * @return the top left corner position for the control
    */
    TPoint ControlPositionL() const;
    
    /**
    * Read nontouch layout
    * @return TRect, listbox rectangle
    */
    TRect NonTouchLayout();
    
    /**
    * Read touch layout
    * @return TRect, listbox rectangle
    */
    TRect TouchLayout();
    
    /**
    * Enters viewfinder mode either in image or videomode
    * depending current visible view. Also starts idle timer.
    */
    void ReserveAndStartVF();
    
  // -----------------------------------------------------
  // Constructors
  private:

    /**
    * C++ constructor
    * @param aController Controller reference
    * @param aView View reference
    * @param aControlHandler Handler reference
    * @since 2.8
    */
    CCamCaptureSetupContainer( 
        CCamAppController& aController,
        CAknView& aView,
        CCamCaptureSetupControlHandler& aControlHandler );

    /**
    * Symbian OS 2nd phase constructor.
    * @since 2.8
    * @param aRect Frame rectangle for container.
    */
    void ConstructL( const TRect& aRect );


  // =====================================================
  // Data
  protected:

    // The control handler that creates the setting editing control
    CCamCaptureSetupControlHandler& iControlHandler;
    // The control that edits the setting value
    CCoeControl* iCaptureSetupControl;
    // The text shown as the title of the container
    HBufC* iTitleText;
    // Whether or not the container displays a viewfinder preview pane
    TBool iViewFinding;
    
    // Layout rect for cropped viewfinder display
    TAknLayoutRect iViewFinderRectLayout;
    
    TAknLayoutRect iSetupControlRectLayout;
    
    // Layout rect for title text
    TAknLayoutText iTitleTextRectLayout;
    
    // width of list-box
    TInt iListBoxWidth;
    
    TBool iActivateOnTouchRelease;
    MTouchFeedback* iFeedback; // not own
    TRect iLayoutAreaRect; 
  };

#endif // CAMCAPTURESETUPCONTAINER_H

// End of File
