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
* Description:  SidePane Control*
*/


#ifndef CAMSIDEPANE_H
#define CAMSIDEPANE_H

// INCLUDES
 
#include "CamSelfTimerObserver.h"
#include "CamBurstModeObserver.h"
#include "CamResourceLoader.h"

// FORWARD DECLARATIONS
class CCamIndicator;
class CCamIndicatorData;
class CCamIndicatorResourceReader;

// CLASS DECLARATION

enum ECamSidePaneFlashState
    { // Used to indicate whether to turn flashing indicators on or off
    ECamSidePaneFlashOff,
    ECamSidePaneFlashOn
    };

/**
* Side Pane control
*/
class CCamSidePane : public CBase,
                                public MCamControllerObserver,
                                public MCamResourceLoader
#ifndef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 
                                ,public MCamBurstModeObserver
#endif // !PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE
  {
  // =========================================================================
  public: // Constructors and destructor
    
    /**
    * Symbian OS two-phased constructor
    * @since 2.8
    * @param aController reference to CCamAppControllerBase instance
    * @param aOverlayViewFnder - whether sidepane is drawn on top of the viewfinder
    * @return created CCamSidePane object
    */
    static CCamSidePane* NewL( CCamAppController& aController,
                               TBool              aOverlayViewFinder );
  
    /**
    * Destructor.
    */
    virtual ~CCamSidePane();

  public: // From MCamResourceLoader

    /**
    * From MCamResourceLoader
    * @since 3.0
    */
    void LoadResourceDataL();

    /**
    * From MCamResourceLoader
    * @since 3.0
    */
    void UnloadResourceData();

    /**
    * From MCamResourceLoader
    * @since 3.0
    */
    void ReloadResourceDataL();

  public: // New functions

    /**
    * Is the zoom pane visible?
    * @since 2.8
    * @return ETrue if the zoom pane is visible
    */   
    TBool IsVisible();

    /**
    * Tells the zoom pane whether it is on display
    * @since 2.8
    * @param aVisible ETrue to make the zoom pane invisible
    */   
    void MakeVisible( TBool aVisible );

    /**
    * Draws all the indicators that are displayed in the side pane
    * @since 2.8
    * @param aGc The graphics context to use for drawing
    */
    void Draw( CBitmapContext& aGc ) const;

    /**
    * Set draw rect
    * @since 2.8
    * @param Rect to draw into
    */
    void SetRect( TRect& aRect );

    /**
    * Set current capture mode
    * @since 2.8
    * @param aMode Capture mode
    */
    void SetCaptureMode( TCamCameraMode aMode );
    
    /**
    * Set up initial state of side pane
    * @since 2.8
    */
    void SetInitialState();
   
    /**
     * Updates the layouts for each indicator in sidepane
     * @since S60 5.0
     */
    void UpdateLayout();    

  protected: // Functions from base classes

    /**
    * From MCamControllerObserver
    * Handle an event from CCamAppControllerBase.
    * @since 2.8
    * @param aEvent the type of event
    * @param aError error code
    */
    virtual void HandleControllerEventL( TCamControllerEvent aEvent,
                                         TInt                aError );


  private:

    /**
    * Get the icon index of current flash setting in given mode.
    * @param aMode Capture mode for which the flash icon index is needed.
    */
    TInt GetFlashIconIndex( const TCamCameraMode& aMode );


    /**
    * Register this object with observed classes.
    * @since 2.8
    */
    void RegisterObserversL();

    /**
    * Deregister this object from observed classes
    * @since 2.8
    */
    void RemoveObservers();

// Generic setup icon is displayed either in the navipane or sidepane
#ifdef PRODUCT_USES_GENERIC_SETUP_INDICATOR
#ifndef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
    /**
    * Sets the default setup indicator depending on the 
    * current setup settings.
    * @since 2.8
    */
    void SetGenericSetupIndicator();
#endif // !PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
#endif // PRODUCT_USES_GENERIC_SETUP_INDICATOR

    /**
    * Sets the image stabilisation indicator
    * if the current capture mode is video
    * @since 3.0
    */
    void SetVideoStabilisationIndicator();

    /**
    * Sets the facetracking indicator
    * if the current capture mode is still image
    * @since 3.0
    */
    void SetFaceTrackingIndicator();

    /**
    * Set the video scene indicator depending on current setting
    * @since 2.8
    */
    void SetVideoSceneIndicator();

    /**
    * Set the photo scene indicator depending on current setting
    * @since 2.8
    */
    void SetPhotoSceneIndicator();

// Burst mode icon is displayed in either the navipane or sidepane
#ifndef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 
    /**
    * From MCamBurstModeObserver
    * @since 2.8
    * @param aActive whether or not burst mode is active
    * @param aStillModeActive whether or not still capture is active
    */
    void BurstModeActiveL( TBool aActive, TBool aStillModeActive );
#endif  // !PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 

    /**
    * Checks if the secondary camera is enabled
    * @since 2.8
    * @return ETrue if the second camera is enabled
    */
    TBool IsSecondaryCameraEnabled() const;
    
    /**
    * Read nontouch layout
    */
    void NonTouchLayout() const; 

    /**
    * Read touch layout
    */
    void TouchLayout() const; 

  private:

    /**
    * Symbian OS 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();

    /**
    * C++ constructor
    * @since 2.8
    * @param aController reference to CCamAppControllerBase instance
    * @param aOverlayViewFnder - whether sidepane is drawn on top of the viewfinder
    */
    CCamSidePane( CCamAppController& aController, 
                  TBool              aOverlayViewFinder );

  // =========================================================================
  // Data
  private: 
  
    CCamAppController& iController;
    RPointerArray<CCamIndicator> iIndicators; // contained indicators
   
    TCamCameraMode iMode; // capture state used to determine which settings to retrieve

    TBool iOverlayViewFinder;     // whether sidepane is drawn over the viewfinder 
    TRect iRect; // the side pane rect
    TBool iVisible; // whether the side pane is visible

  // =========================================================================
  };

#endif

// End of File
