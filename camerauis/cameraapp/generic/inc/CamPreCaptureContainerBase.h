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
* Description:  Container base class pre capture views
*
*/



#ifndef CAMPRECAPTURECONTAINERBASE_H
#define CAMPRECAPTURECONTAINERBASE_H

// ===========================================================================
// INCLUDES
#include "CamAppController.h"
#include "CamContainerBase.h"
#include "CamControllerObservers.h"
#include "CamBurstModeObserver.h"
#include "CamZoomPane.h"
#include "CamPSI.h" // Product Specific Information
#include "cambatterypanecontroller.h"
#include "mcamcameraobserver.h" // MCamCameraObserver <CAMERAAPP_CAPI_V2_MIGRATION/>

#include "CamLocationIconController.h"


// ===========================================================================
// FORWARD DECLARATIONS
class CWindowGC;
class CAknView;

class CCamViewBase;
class CCamSidePane;
class CCamZoomPane;
class CCamIndicator;
class CCamTimeLapseSlider;
class MCamVfGridDrawer;



// ===========================================================================
// CLASS DECLARATION

/**
* Container for still image pre capture view
*/
class CCamPreCaptureContainerBase : public CCamContainerBase,
                                    public MCamControllerObserver,
                                    public MCamCameraObserver,
                                    public MCamSettingsModelObserver, // <CAMERAAPP_CAPI_V2_MIGRATION/>
//                                    public MCamViewFinderObserver, 

                                    public MCamBurstModeObserver,
                                    public MCamBatteryPaneObserver,
                                    public MCamLocationIconObserver

  {
  // =========================================================================
  // Typenames
  public:

    // enum TVfState moved to CCamContainerBase. Used in multiple child classes.

    enum TCamPreCaptureSubControls
      {             
      // ECamTimeLapseControl         
      ECamActivePalette
      };

    enum TFocusState
      {
      EFocusStateNormal,
      EFocusStateFocusing,
      EFocusStateFocusFailed,
      EFocusStateFocusAchieved
      };
        
  // =========================================================================
  // Public constructors and destructor
  public: 
        
    /**
    * Destructor.
    */
    virtual ~CCamPreCaptureContainerBase();

  // -------------------------------------------------------------------------
  // From CoeControl
  public:
  
    /**
    * @since 2.8
    * @return number of contained controls
    */
    virtual TInt CountComponentControls() const;
    
    /**
    * @since 2.8
    * @param aIndex The index of the control required
    * @return The requested control
    */
    virtual CCoeControl* ComponentControl(TInt aIndex) const;
      
    /**
    * Locally takes actions needed due to resource changes, then
    * calls the implementation in CCamContainerBase
    * @since 2.8
    * @param aType resource change
    */
    virtual void HandleResourceChange( TInt aType );    


    /**
    * @since 2.8
    * @param aKeyEvent the key event
    * @param aType the type of the event
    * @return TKeyResponse key event was used by this control or not
    */
    virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                               TEventCode aType );


    /**
    * Handle events sending app to foreground or background
    * @since 2.8
    * @param aForeground whether the event brings the app to fore or background
    */
    virtual void HandleForegroundEventL( TBool aForeground );

    /**
    * Handle application level event.
    * For example focus gained and lost are notified through this method.
    * @param aEvent Event type
    */
    virtual void HandleAppEvent( const TCamAppEvent& aEvent );

    /**
    * @since 2.8
    * @param aVisible ETrue if the controls should be visible
    */
    virtual void MakeVisible( TBool aVisible );

    /**
     * From CCoeControl
     * Handle pointer events. 
     * @since S60 5.0
     * @param aPointerEvent information about the pointerevent
     */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);

  protected:

    /**
    * @since 2.8
    * @param aRect area where to draw
    */
    virtual void Draw( const TRect& aRect ) const;

  // -------------------------------------------------------------------------
  // From MCamControllerObserver
  public:

    /**
    * @since 2.8
    * @param aEvent The specific event which occurred
    * @param aError The error code associated with the event
    */
    virtual void HandleControllerEventL( TCamControllerEvent aEvent, 
                                         TInt                aError );

  // <CAMERAAPP_CAPI_V2_MIGRATION>
  // -------------------------------------------------------------------------
  // From MCamCameraObserver
  public:

    /**
    * @since Camera 4.0
    * @see MCamCameraObserver
    */
    virtual void HandleCameraEventL( TInt              aStatus, 
                                     TCamCameraEventId aEventId, 
                                     TAny*             aEventData = NULL );

  // -------------------------------------------------------------------------
  // From MCamSettingsModelObserver
  public:

    /**
    * Notification for change in integer setting item value.
    *
    */
    virtual void IntSettingChangedL( TCamSettingItemIds aSettingItem, 
                                     TInt               aSettingValue );

    /**
    * Notification for change in text setting item value.
    *
    */
    virtual void TextSettingChangedL(       TCamSettingItemIds aSettingItem, 
                                      const TDesC&             aSettingValue );


  // </CAMERAAPP_CAPI_V2_MIGRATION>    
  // -------------------------------------------------------------------------
  // From MCamBatteryPaneObserver
  public:
    /**
    * Called when battery pane has changed and redraw is needed
    * @since 3.1
    */  
    void BatteryPaneUpdated();    
    

  // -------------------------------------------------------------------------
  // From MCamLocationIconObserver
  public:
    /**
    * Called when location icon status is changed
    * @since 3.1
    */  
    void LocationIconUpdated(); 

  // -------------------------------------------------------------------------
  public:

    /**
    * Requests that the camera application redraws the supplied area
    * @since 3.0
    * @param aArea the area to redraw
    */
    virtual void Redraw(const TRect& aArea);

  public: // New functions
    
    /**
    * Handles CBA and Options menu keys
    * @since 3.0
    * @param aCommand The command to handle
    */        
    void HandleCommand( TInt aCommand );
    
    /**
    * Will show the zoom pane for a set time
    * @since 2.8
    */
    void ShowZoomPaneWithTimer();
    
    /**
    * Constructs Active Palette if not done yet.
    * If Active Palette already exists, clears any existing items.
    * Sets new items based on resource from GetAPResourceId.
    * Registers the view for Active Palette.
    */
    virtual void SetupActivePaletteL( CCamViewBase* aView );

  protected: 

        /**
        * Callback for zoom timer when zoom pane needs to be hidden.
        * @since 2.8
    * @param aObject - Pointer to instance of CCamPreCaptureContainerBase
        * @return KErrNone
        */
    static TInt ZoomTimerCallback( TAny* aObject );
    
        /**
        * Callback for reticule timer, 
        * @since 3.0
    * @param aObject - Pointer to instance of CCamPreCaptureContainerBase
        * @return KErrNone
        */
    static TInt ReticuleTimerCallback( TAny* aObject );

        /**
        * Member function called when zoom timer expires.
        * @since 2.8
        */
    void ZoomTimerTick();

        /**
        * Allows derived classes to draw mode-specific icons
        * @param aGc The context to draw with
        * @since 3.0
        */
    virtual void DrawAdditionalIcons(CBitmapContext& aGc) const = 0;

        /**
        * Returns the rect of the resolution indicator
        * @return The rect of the resolution icon as a TRect
        * @since 3.0
        */
    TRect ResolutionIndicatorRect() const; 

        /**
        * Returns the ID of the array containing the resolution indicator icons
        * @return The array ID
        * @since 3.0
        */
    virtual TCamPsiKey ResolutionIndicatorIconPsiKey() const = 0;

    protected:         

        /**
        * Draw viewfinder grid.
        * Delegates call to iVfGridDrawer.
        * Does not draw anything if grid has not been set visible.
        * @param aGc   The graphics context
        */
        virtual void DrawVfGrid( CBitmapContext& aGc ) const;

        /**
        * Redraw part of viewfinder grid.
        * Delegates call to iVfGridDrawer.
        * Does not draw anything if grid has not been set visible.
        * @param aRect The area to redraw
        * @param aGc   The graphics context
        */
        virtual void ReDrawVfGrid( const TRect&          aRect,
                                         CBitmapContext& aGc   ) const;

        /**
        * Init the viewfinder grid.
        * Creates the grid drawer if not done yet.
        * Set drawing rectangle and visibility.
        */
        virtual void InitVfGridL( const TRect& aRect ) = 0;

        /**
        * Get the current setting for viewfinder grid visibility.
        */
        virtual TBool GetVFGridVisibilitySetting() const;

        /**
        * Reset the viewfinder grid drawer visibility property with
        * value asked from controller.
        */
        virtual void ResetVFGridVisibility();

  protected:

    /**
    * Symbian OS 2nd phase constructor.
    * @since 2.8
    * @param aRect Frame rectangle for container.
    */
    void BaseConstructL( const TRect& aRect );
    
    /**
    * C++ constructor
    * @since 2.8
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    */
    CCamPreCaptureContainerBase( CCamAppController& aController,                   
                                 CAknView&          aView );

  private: // New Functions

    /**
    * Performs the actual BitBlt of the viewfinder frame
    * @since 2.8
    * @param aGc the graphics context
    * @param aFrame pointer to the bitmap. 
    */
    void DrawFrameNow( CBitmapContext& aGc, const CFbsBitmap* aFrame ) const;
        
  protected:
    /**
    * Autofocus indication layout from LAF
    *
    * @since S60 S60 v5.0
    */    
    void SizeChanged();  
    
  private:
    /**
    * Draws the reticule over the viewfinder
    * @since 3.0
    * @param aGc the graphics context
    */
    void DrawReticule( CBitmapContext& aGc ) const;

    /**
    * Draw part of the reticule.
    * Used to redraw areas left dirty by hiding AP tooltips.
    * @param aRect The area to redraw
    * @param aGc   The graphics context
    */
    void ReDrawReticule( const TRect&          aRect,
                               CBitmapContext& aGc   ) const;
        
    /**
    * Force the side-pane or zoom-pane to redraw. Called when they
    * are required to be overlaid on the viewfinder.
    * @since 2.8
    */
    void ForceSideZoomPaneDraw() const;

    /**
    * Return the id of the layout resource for the viewfinder frame
    * @since 2.8
    * @param aViewFinderLayoutId on return this contains the resource id
    *           of the viewfinder layout
    * @param aReticuleLayoutId on return this contains the resource id 
    *           of the reticule layout if used, otherwise 0
    */
    virtual void ViewFinderLayoutResourceIds( TInt& aViewFinderLayoutId, 
                                             TInt& aReticuleLayoutId ) const = 0;

    /**
    * Create the resolution indicator
    * @since 2.8
    */
    void CreateResolutionIndicatorL();

    /**
    * Sets the resolution indicator to the required icon
    * @since 2.8
    */
    virtual void SetResolutionIndicator() = 0;
        
    /**
    * Sets the location indicator icon if the setting is enabled
    */
    virtual void SetLocationIndicatorVisibility() = 0;
    
    /**
    * Burst mode activation has changed
    * @since 2.8
    * @param aActive whether or not burst mode is active
    * @param aStillModeActive whether or not still capture is active
    */
    void BurstModeActiveL( TBool aActive, TBool aStillModeActive );
    
    /**
    * Process a key event that should change the current capture state
    * E.g. start/pause recording, start/stop capture
    * @since 3.0
    * @param aKeyEvent the key press that initiates the capture
    * @return whether or not the key was consumed
    */ 
    virtual TKeyResponse HandleCaptureKeyEventL( const TKeyEvent& aKeyEvent ) = 0;
        
    /**
    * Process a key event that could change the current capture state
    * E.g. start autofocus, revert from post-capture to pre-capture views        
    * @since 3.0
    * @param aKeyEvent the key press that initiates the capture
    * @param aType the event code type
    * @return whether or not the key was consumed
    */ 
    virtual TKeyResponse HandleShutterKeyEventL( const TKeyEvent& aKeyEvent,
                                                 TEventCode aType ) = 0;        
    
    /**
    * Move up through the flash settings
    * @since 3.0
    */
    virtual void IncreaseFlashSettingL();
   
   /**
    * Move down through the flash settings
    * @since 3.0
    */ 
    virtual void DecreaseFlashSettingL();  

    /**
    * Initiate behaviour in response to a navi key left/right arrow 
    * press 
    * @since 3.0
    * @param aKeyEvent the key that has been pressed
    * @param aType the Key type
    * @return whether or not the key was consumed
    */        
    TKeyResponse HandleLeftRightNaviKeyL( const TKeyEvent& aKeyEvent,
                                          TEventCode aType );
    /**
    * Draw side or zoom pane, softkeys and reticule
    * @since 3.0
    * @param aGc context to draw into
    */
    void DrawScreenFurniture( CBitmapContext& aGc ) const;
    
    /**
    * Draw navi pane items
    * @since 3.0
    * @param aGc context to draw into
    */
    void DrawNaviControls( CBitmapContext& aGc ) const;
    
    /**
    * Draw active palette
    * @since 3.1
    */
    void DrawActivePalette() const;
    
    /**
    * Draw active pallette
    * @since 3.1
    * @param aGc context to draw into
    */
    void DrawActivePalette( CBitmapContext& aGc ) const;

    /**
    * Sets the iTimeLapseVisible flag
    * @param aActive What to set iTimeLapseVisible to
    */
    void SetTimeLapseVisible(TBool aActive);

        void HandleOperationStateChangeEventL();
  
    /**
    * Gets the resource id of AP items.
    * Needs to be implemented in inheriting classes.
    */
    virtual TInt GetAPResourceId() const = 0;
    
      /**
      * Draws Screen content to graphics context
      * @param aGc The context to draw with
      * @param aFrame View Finder Frame to be drawn
      */
    void DrawToContext(CBitmapContext& aGc, const CFbsBitmap*  aFrame);

    /**
     * Creates a colored Autofocus indication icon
     *
     * @since S60 S60 v5.0
     * @param aColor indication color
     * @return TInt Created icon's offset in icons array 
     */ 
    TInt CreateAfIconL( TRgb aColor );
    
    /**
     * Creates a colored autofocus indication icon corner
     *
     * @since S60 S60 v5.0
     * @param aColor indication color
     * @param aFileBitmapId ID of the bitmap in the file
     */ 
    void CreateAfCornerL( TRgb aColor, TInt aFileBitmapId );

    /**
     * Sets size of autofocus indication icon 
     *
     * @since S60 S60 v5.0
     * @param aOffset Offset in colored corner icon array
     */ 
    void SetAfIconSize( TInt aOffset );
    
    /**
     * Draws corners of autofocus indication icon 
     *
     * @since S60 S60 v5.0
     * @param aGc Bitmap graphics context
     * @param aOffset Offset in colored corner icon array
     */ 
    void DrawAf( CBitmapContext& aGc, TInt aOffset ) const;  

    /**
    * Read nontouch layout
    */
    void NonTouchLayout();  

    /**
    * Read touch layout
    */
    void TouchLayout();


  // =========================================================================
  // Data
  protected:
    
    // Pointer to the instance of the side pane
    CCamSidePane* iSidePane;
    
    // Pointer to the instance of the zoom pane
    CCamZoomPane* iZoomPane;  
    
    // Array of pointer to the resolution indicators
    RPointerArray<CCamIndicator> iResolutionIndicators;
    
    // The current indicator
    TInt iCurrentIndicator;
    
    // The visibility of the location indicator
    TBool iLocationIndicatorVisible;
    CCamLocationIconController* iLocationIconController;

    // Is reticule to be displayed or not (for when flashing)
    TBool iShowReticule;

    // Whether the current Photo Scene requires a reticule (some scenes do not focus)
    TBool iPhotoSceneUsesReticule;

    TBool iNaviPaneActive;
    
    // If set, redraw calls are ignored.
    TBool iDisableRedraws;
    
    // Viewfinder rect.
    TRect iRect;

    // Timer used to remove zoom pane after a short delay
    CPeriodic* iZoomTimer;
    
    // Keys to use for detecting zoom in and out key events
    TInt iZoomInKey;
    TInt iZoomOutKey;
    
    // whether we are left or right handed
    TCamHandedness iHandedness;


    // Reticule location and size
    TRect iReticuleRect;

    /**
     * Autofocus indication icons array.  
     */
    RPointerArray<CFbsBitmap> iAfIcons; 
    
    /**
     * Autofocus indication: White ready icon.  
     */
    TInt iAfReadyIcon;

    /**
     * Autofocus indication: Green focused icon.
     */
    TInt iAfFocusIcon;

    /**
     * Autofocus indication: Yellow error icon.
     */
    TInt iAfErrIcon;
    
    /**
     * Autofocus indication icons layout
     */      
    RArray<TAknLayoutRect> iLayouts;
    
    /**
     * Autofocus layout
     */
    TSize iAfIconCornerSize; 
   
    // Timer used for the flashing of the reticule
    CPeriodic* iReticuleTimer;

    // Whether the reticule is visible or not during 'flash' on/off
    TBool iReticuleFlashOn;
    
    // Current state of focus (reflected by reticule)
    TFocusState iFocusState;

    // Determines if a stored image should be used in Draw() functions
    TBool iFreezeFrame;
    
    // Determine from PSI if side & zoom pane are overlaid on VF
    TBool iOverlayViewFinder;
    
    // width of the zoom pane
    TInt iZoomPaneWidth;
    
    // The Time Lapse control
    CCamTimeLapseSlider* iTimeLapseSlider;
    
    // Whether the Time Lapse control is visible
    TBool iTimeLapseVisible;
    
    // Layout rect for the Timelapse control
    TAknLayoutRect iTimeLapseLayout;        
    
    // Determines if the zoom pane background needs redrawing
    TBool iRedrawPaneBackground;
    
    // Determines if the viewfinder should use an off screen bitmap
    TBool iUseOffScreenBitmap;
    
    // Offscreen drawing members
    CFbsBitmap* iOffScreenBitmap;
    CFbsBitGc* iBitmapGc;
    CFbsBitmapDevice* iBitmapDevice;
    
    // True if this container has received any viewfinder frames
    TBool iReceivedVfFrame;
    
    // True if application is shutting down
    TBool iShuttingDown;
    
    TRect iVFRect;

    TRect             iVfGridRect;
    MCamVfGridDrawer* iVfGridDrawer;
        
    CCamBatteryPaneController* iBatteryPaneController;

    CFbsBitmap* iViewFinderBackup;
  // =========================================================================
  };
    
#endif // CAMPRECAPTURECONTAINERBASE_H

// End of File
