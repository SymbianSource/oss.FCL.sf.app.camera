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
* Description:  Container for still pre capture view*
*/



#ifndef CAMSTILLPRECAPTURECONTAINER_H
#define CAMSTILLPRECAPTURECONTAINER_H

// INCLUDES
#include "CamPreCaptureContainerBase.h"

#include "camflashstatus.h" // MFlashStatusObserver

class CFbsBitmap;



// CLASS DECLARATION

/**
* Container for still image pre capture view
*/
class CCamStillPreCaptureContainer : public CCamPreCaptureContainerBase
                                    ,public MFlashStatusObserver
  {
  public: // Constructors and destructor
    
    /**
    * Symbian OS two-phased constructor
    * @since 2.8
    * @param aController reference to CCamAppControllerBase instance
    * @param aView reference to the view containing this container
    * @param aSetupPaneHandler handle to horizontal setup pane (product specific)
    * @param aRect Frame rectangle for container.
    * @return Pointer to newly constructed CCamStillPreCaptureContainer object
    */
    static CCamStillPreCaptureContainer* NewL( CCamAppController& aController, 
                                               CAknView& aView,									   
                                               const TRect& aRect );

    /**
    * Destructor.
    */
    virtual ~CCamStillPreCaptureContainer();

    /**
    * From MCamControllerObserver
    * @since 2.8
    * @param aEvent The specific event which occurred
    * @param aError The error code associated with the event
    */
    void HandleControllerEventL( TCamControllerEvent aEvent, TInt aError );
        
  public: // Functions from base classes
        
    /**
    * From CCoeControl
    * @since 2.8
    * @param aKeyEvent the key event
    * @param aType the type of the event
    * @return TKeyResponse key event was used by this control or not
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                 TEventCode aType );         


    /**
    * From CCoeControl
    * @since 3.0
    */
    void SizeChanged();

                                 
    /**
    * From CCamContainerBase
    * Handle events sending app to foreground or background
    * @since 2.8
    * @param aForeground whether the event brings the app to fore or background
    */
    void HandleForegroundEventL( TBool aForeground );


    /**
    * Gets notification of the container being enabled / disabled 
    * @param aVisible, true->enabled and visible and vice versa
    */    
    void MakeVisible( TBool aVisible );	

    
    public: // From MFlashStatusObserver 
        /*
        * Flash icon visibility has changed.
        * @param aVisible new visibility status
        */  
        virtual void FlashIconVisible( TBool aVisible );      
        
        /**
        * Called every time when camera driver reports error in
        * flash recharging.                
        */
        virtual void FlashError();    
        
        // support for creating long running AO to create AP
        static TInt BackgroundAPL( TAny *aparam );
        TBool SetupActivePaletteCallbackL();

  private:

    /**
    * Symbian OS 2nd phase constructor.
    * @since 2.8
    * @param aRect Frame rectangle for container.
    */
    void ConstructL( const TRect& aRect );

    /**
    * Symbian OS 2nd phase constructor.
    * @param aRect Frame rectangle for container.
    */
    void BaseConstructL( const TRect& aRect );  

    /**
    * C++ constructor
    * @since 2.8
    * @param aController reference to CCamAppControllerBase instance
    * @param aSetupPaneHandler handle to horizontal setup pane (product specific)
    * @param aView reference to the view containing this container
    */
    CCamStillPreCaptureContainer( CCamAppController& aController,
                                  CAknView& aView );

  private: // Methods from base classes

    /**
    * From CCamPreCaptureContainerBase
    * Return the id of the layout resource for the viewfinder frame
    * @since 2.8
    * @param aViewFinderLayoutId on return this contains the resource id
    *           of the viewfinder layout
    * @param aReticuleLayoutId on return this contains the resource id 
    *           of the reticule layout if used, otherwise 0
    */
    void ViewFinderLayoutResourceIds( TInt& aViewFinderLayoutId, 
                                      TInt& aReticuleLayoutId ) const;
                                      
    /**
    * From CCamPreCaptureContainerBase
    * Sets the resolution indicator to the required icon
    * @since 2.8
    */
    void SetResolutionIndicator();   

    /**
    * Sets the location indicator icon if the setting is enabled
    */
    void SetLocationIndicatorVisibility();

    /**
    * From CCamPreCaptureContainerBase
    * Move up through the flash settings
    * @since 3.0
    */
    void IncreaseFlashSettingL();
   
    /**
    * From CCamPreCaptureContainerBase
    * Move down through the flash settings
    * @since 3.0
    */ 
    void DecreaseFlashSettingL();                                         
                                      
    /**
    * From CCamPreCaptureContainerBase
    * Process a key event that should change the current capture state
    * E.g. start/pause recording, start/stop capture
    * @since 3.0
    * @param aKeyEvent the key press that initiates the capture
    * @return whether or not the key was consumed
    */ 
    TKeyResponse HandleCaptureKeyEventL( const TKeyEvent& aKeyEvent );                                          
    
    /**
    * Process a key event that could change the current capture state
    * E.g. start autofocus, revert from post-capture to pre-capture views        
    * @since 3.0
    * @param aKeyEvent the key press that initiates the capture
    * @param aType the key type for the event
    * @return whether or not the key was consumed
    */         
    TKeyResponse HandleShutterKeyEventL( const TKeyEvent& aKeyEvent,
                                         TEventCode aType );

    /**
    * Allows additional mode-specific icons to be drawn
    * @param aGc The context to draw with
    */
    virtual void DrawAdditionalIcons(CBitmapContext& aGc) const;

    /**
    * Returns the ID of the array containing the resolution indicator icons
    * @return The array ID
    * @since 3.0
    */
    virtual TCamPsiKey ResolutionIndicatorIconPsiKey() const;

    /**
    * Gets the resource id of AP items.
    * Resource is selected based on embedded status and user mode.
    */
    virtual TInt GetAPResourceId() const;
    
    /**
    * Init the viewfinder grid.
    * Creates the grid drawer if not done yet.
    * Set drawing rectangle and visibility.
    */
    virtual void InitVfGridL( const TRect& aRect );
    
  private: // New functions    
    /**
    * Loads the flash icon file and subscribes to blink events
    */
    void InitFlashIconL();

    /**
    * Loads flash icon layout from resources, and sets icon size
    */
    void LayoutFlashIcon();    

    /**
    * Draws the flash icon to select
    * @param aGc
    */
    void DrawFlashIcon( CBitmapContext& aGc ) const;
    
    /**
    * Utility function to subscribe / unsubscribe from flash related events
    * @param aSubscribe, true->subscribe, else unsubscribe 
    */        
    void SubscribeFlashEvents( TBool aSubscribe );
    

  private: // Data
    TBool iFlashIconVisible;
    TRect iFlashIconRect;
    // Xenon flash bitmap
    CFbsBitmap* iFlashBitmap;
    CFbsBitmap* iFlashBitmapMask;
    
    TBool iXenonFlashSupported;
    
    CIdle* iAPCreateAO;
    
  };

#endif // CAMSTILLPRECAPTURECONTAINER_H
    
// End of File 
