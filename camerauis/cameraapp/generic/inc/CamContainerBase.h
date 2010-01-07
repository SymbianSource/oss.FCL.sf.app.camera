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
* Description:  Base class for all camera application containers
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/



#ifndef CAMCONTAINERBASE_H
#define CAMCONTAINERBASE_H



// INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>
#include "CamAppUiBase.h"             //  For TCamAppViewIds

// FORWARD DECLARATIONS
class CCamAppController;
class CAknView;
class MAknsControlContext;

class CEnhancedSoftKeys;

class CCamNaviCounterControl;
class CCamNaviProgressBarControl;
class MActivePalette2UI;
class CCamBackupContainer;

const TInt KCamPreCaptureWindowOrdinalPos = 100;
const TInt KCamPostCaptureWindowOrdinalPos = 101;


// CLASS DECLARATION

/**
* Container base class
*/
class CCamContainerBase : public CCoeControl
  {
  // =======================================================
  // Typenames
  public:

    // Was defined in multiple child classes, now moved here.
    enum TVfState // ViewFinder states
      {
      EVfStateActive,
      EVfStateFrozenDimmed,
      EVfStateActiveMasked
      };    
    
    
    // Enumeration used to indicate the pane currently visible
    enum TCamActiveNaviPane
      {
      ECamPaneUndefined,
      ECamPaneCounter,
      ECamPaneProgress
      };
    
  // =======================================================
  // Methods

  // Constructors and destructor
  public: 
        
    /**
    * Destructor.
    * @since 2.8
    */
    virtual ~CCamContainerBase();

  // -------------------------------------------------------
  // From CCoeControl
  public: 
    
    /**
    * Return this controls window.
    * This is needed for direct viewfinding.
    * Protected in CCoeControl. (not virtual)
    */
    RWindow& Window() const;

    /**
    * Handle events sending app to foreground or background
    * @since 2.8
    * @param aForeground whether the event brings the app to fore or background
    */
    virtual void HandleForegroundEventL( TBool aForeground );
    
    /** 
    * Gets an object whose type is encapsulated by the specified TTypeUid object.
    * @since 2.8
    * @param aId Encapsulates the Uid that identifies the type of object required.
    * @return Encapsulates the pointer to the object provided. 
    */
    TTypeUid::Ptr MopSupplyObject( TTypeUid aId );
        
    /**
    * This method should change its client rect area and redraw properly.
    * @since 2.8
    * @param aType resource change
    */
    virtual void HandleResourceChange( TInt aType );

    /**
    * Handle application level event.
    * For example focus gained and lost are notified through this method.
    * Default implementation is empty, to be replaced in inherited classes.
    * @param aEvent Event type
    */
    virtual void HandleAppEvent( const TCamAppEvent& aEvent );



    /**
    * @since 2.8
    * @param aKeyEvent the key event
    * @param aType the type of the event
    * @return TKeyResponse key event was used by this control or not
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                       TEventCode aType );  

  // -------------------------------------------------------
  // New functions
  public: 
    
    /**
    * Passes the value of the previous view to the container
    * @since 2.8
    * @param aId The ID of the previous view
    */
    void SetPreviousViewId( TCamAppViewIds aId );

    /**
    * Sets up a navipane for use by this container
    * @param aNaviPane specifies which navipane to set up.
    * @since 3.0
    */
    void SetupNaviPanesL( TCamActiveNaviPane aNaviPane );
    
    /**
    * Will show the zoom pane for a set time
    * Only implemented by pre capture containers
    * @since 2.8
    */
    virtual void ShowZoomPaneWithTimer();
         
    /**
    * Create the ActivePalette control
    * @since 3.0
    */
    void CreateActivePaletteL();
    
    /**
    * Destroys the ActivePalette control
    * @since 3.0
    */
     static void DestroyActivePalette();      

    /**
    * Requests that the camera application redraws the supplied area
    * @since 3.0
    * @param aArea the area to redraw
    */
    virtual void Redraw(const TRect& aArea);
        
      
    /**
    * Load layouts for the text      
    */      
    void PrepareProcessingBackgroundL();

    /**
    * Load processing image/video text from resources.
    */
    void PrepareProcessingTextL( TBool aStillMode );  
    
    /**
    * Delete and null processing text, after which
    * the text will no longer be drawn.
    */
    void DismissProcessingText();
    
    /**
     * Load processing image text for burst from resources.
     */
    void PrepareBurstProcessingTextL();  

    /**
     * Update processing image text for burst.
     * @param aHandled number of imagedata received
     * @param aTotal number of snapshots received
     */
    void UpdateBurstProcessingTextL( TInt aHandled, TInt aTotal );  

    /**
     * Delete and null processing text, after which
     * the text will no longer be drawn.
     */
    void DismissBurstProcessingText();
    
    /**
    * Viewfinder layout rect
	* @param aTargetMode target mode for which viewfinder rect is required. Default(ECamControllerIdle) corresponds to current active mode.
    * @return TRect viewfinder rect from layout
    */
    TRect ViewFinderFrameRect(TCamCameraMode aTargetMode = ECamControllerIdle) const; 
 
    /**
    * We check for the file name whenever we call the UMP
    * or Properties view and come back.
    */
    virtual void CheckForFileName( TBool aDoCheck );
        
  protected: 

    /**
    * Whether or not this key is considered to be capture key
    * in the current context
    * @since 3.0
    * @param aKeyEvent the key event
    * @param aType the type of event
    * @return ETrue if this is a capture key, otherwise EFalse
    */
    TBool IsCaptureKeyL( const TKeyEvent& aKeyEvent,
                               TEventCode aType ) ;

    /**
    * Whether or not this key is considered to be shutter key
    * in the current context
    * @since 3.0
    * @param aKeyEvent the key event
    * @param aType the type of event
    * @return ETrue if this is a shutter key, otherwise EFalse
    */
    TBool IsShutterKeyL( const TKeyEvent& aKeyEvent,
                               TEventCode aType ) ;                 

    /**
    * Whether or not this key is considered to be zoom key
    * in the current context
    * @since 3.0
    * @param aKeyEvent the key event
    * @param aType the type of event
    * @return ETrue if this is a zoom key, otherwise EFalse
    */
    TBool IsZoomKeyL( const TKeyEvent& aKeyEvent,
                            TEventCode aType ) ;
    /**
    * Is zoom operation currently possible?
    * @since 3.0
    */               
    TBool ZoomAvailable();
    
    /**
    * Is Primary Capture Key
    * @since 9.1
    */               
    TBool IsPrimaryCameraCaptureKey( const TKeyEvent& aKeyEvent ) const ;     
    
    /**
    * Is Secondary Capture Key
    * @since 9.1 ( checking for scan codes )
    */               
    TBool IsSecondaryCameraCaptureKey( const TKeyEvent& aKeyEvent ) const ;    
  // -------------------------------------------------------
  // Constructors
  protected: 

    /**
    * C++ constructor
    * @since 2.8
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    */
    CCamContainerBase( CCamAppController& aController,
                       CAknView& aView );
    /**
    * Symbian OS 2nd phase constructor.
    * @since 2.8
    * @param aRect Frame rectangle for container.
    */
    void BaseConstructL( const TRect& aRect ); 
    
  protected:
    /**
     * Create background context (iBgContext). May be overridden by
     * derived classes.
     * @since S60 v5.0
     */
    virtual void CreateBackgroundContextL();
    
public:
    
    /**
    * Returns the number of component controls
    * @since 3.0
    * @return The number of owned controls to display
    */       
    TInt CountComponentControls() const;
    
    /**
    * Returns the specified component control
    * @since 3.0
    * @param aIndex The index of the control to return
    * @return Pointer to the specified control
    */               
    CCoeControl* ComponentControl( TInt aIndex ) const;
    
  // =======================================================
  private:
    /*
    * Checks for the zoom key with the mapped key's scan code
    */
    TBool CheckForZoomKey( const TKeyEvent& aKeyEvent );  
    
    /*
    * Checks for the zoom-in/out key from the mapped key's scan code 
    * from the zoom array
    */
    TBool CheckInZoomKeyArray( const TKeyEvent& aKeyEvent );
    
  // Data
  protected: 

    CAknView& iView;
    CCamAppController& iController; 
    
    // The ID of the previous view
    TCamAppViewIds iPrevViewId;
    
    // context for skin background drawing
    MAknsControlContext* iBgContext;

    // The current pane displayed in the Application Pane
    TCamActiveNaviPane iPaneInUse;
    
    // Instance of Navi Counter control (not owned)
    CCamNaviCounterControl* iNaviCounterControl;
    
    // Instance of Navi Progress control (not owned)
    CCamNaviProgressBarControl* iNaviProgressBarControl;
              
    // Instance of ActivePalette2 control (not owned)
    static MActivePalette2UI*   iActivePalette;

    // ETrue if shutter key (half-press) has been made
    TBool iKeyShutter;

    // zoom keys for primary and secondary camera
    RArray<TInt> iPrimaryZoomInKeys;
    RArray<TInt> iPrimaryZoomOutKeys;
    RArray<TInt> iSecondaryZoomInKeys;
    RArray<TInt> iSecondaryZoomOutKeys;

    // capture keys
    RArray<TInt> iPrimaryCameraCaptureKeys;
    RArray<TInt> iSecondaryCameraCaptureKeys;
    
    // AF key or half capture key press scan codes
    RArray<TInt> iPrimaryCameraAFKeys;    
    
    /// Remember current orientation, so we only redraw when necessary
    TCamOrientation iCamOrientation;

  TAknLayoutText iProcessingTextLayout;
  HBufC* iProcessingText;
      TAknLayoutText iBurstProcessingTextLayout;
      HBufC* iBurstProcessingText;
  
  //Backup child container to keep the background of viewfinder window always opaque
  CCamBackupContainer* iBackupContainer;
  };

#endif // CAMCONTAINERBASE_H

// End of File
