/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for video pre capture view*
*/



#ifndef CAMVIDEOPRECAPTURECONTAINER_H
#define CAMVIDEOPRECAPTURECONTAINER_H

// INCLUDES
#include "CamPreCaptureContainerBase.h"

class MTouchFeedback;

// CLASS DECLARATION

/**
* Container for video image pre capture view
*/
class CCamVideoPreCaptureContainer : public CCamPreCaptureContainerBase
  {
  public: // Constructors and destructor
        
    /**
    * Symbian OS two-phased constructor
    * @since 2.8
    * @param aController reference to CCamAppControllerBase instance
    * @param aView reference to the view containing this container
    * @param aSetupPaneHandler handle to horizontal setup pane (product specific)
    * @param aRect Frame rectangle for container.
    * @return Pointer to newly constructed CCamVideoPreCaptureContainer object
    */
    static CCamVideoPreCaptureContainer* NewL( 
                CCamAppController& aController, 
                CAknView& aView,									   
                const TRect& aRect );

    /**
    * Destructor.
    */
    virtual ~CCamVideoPreCaptureContainer();

  // -------------------------------------------------------
  // From MCamControllerObserver
  public:

    /**
    * @since 2.8
    * @param aEvent The specific event which occurred
    * @param aError The error code associated with the event
    */
    void HandleControllerEventL( TCamControllerEvent aEvent, TInt aError );

  // <CAMERAAPP_CAPI_V2_MIGRATION>
  // -------------------------------------------------------
  // From MCamCameraObserver  
  public:

    /**
    * @since Camera 4.0
    * @see MCamCameraObserver
    */
    // Base class implementation fits this class, too.
    // virtual void HandleCameraEventL( TInt              aStatus, 
    //                                  TCamCameraEventId aEventId, 
    //                                  TAny*             aEventData = NULL );

    /**
    * From MCamViewFinderObserver
    * New viewfinder frame is available 
    * @since 2.8
    * @param aFrame pointer to the bitmap. This pointer is valid until
    *               the call returns. Ownership not transferred.
    * @param aFrozen whether or not the viewfinder has been frozen
    */
    //   void ShowViewFinderFrame( const CFbsBitmap* aFrame, TBool aFrozen );

  // </CAMERAAPP_CAPI_V2_MIGRATION>
  // -------------------------------------------------------
  // From CCamPreCaptureContainerBase
  public:

    /**
    * Handle events sending app to foreground or background
    * @since 2.8
    * @param aForeground whether the event brings the app to fore or background
    */
		virtual void HandleForegroundEventL( TBool aForeground );

  protected:

    /**
    * Init the viewfinder grid.
    * Overriden from base class to always disable vf grid.
    */
    virtual void InitVfGridL( const TRect& aRect );

  private:

    /**
    * From CCamPreCaptureContainerBase
    * Return the id of the layout resource for the viewfinder frame
    * @since 2.8
    * @param aViewFinderLayoutId on return this contains the resource id
    *           of the viewfinder layout
    * @param aReticuleLayoutId on return this contains the resource id 
    *           of the reticule layout if used, otherwise 0
    */
    virtual void ViewFinderLayoutResourceIds( TInt& aViewFinderLayoutId, 
                                              TInt& aReticuleLayoutId ) const;
                                          
    /**
    * From CCamPreCaptureContainerBase
    * Sets the resolution indicator to the required icon
    * @since 2.8
    */
    virtual void SetResolutionIndicator();

    /**
    * Sets the location indicator icon if the setting is enabled
    */
    virtual void SetLocationIndicatorVisibility();

    /**
    * Returns the rect of the location indicator
    * @return The rect of the location icon as a TRect
    * @since 3.0
    */
    virtual TRect LocationIndicatorRect();
        
    /**
    * From CCamPreCaptureContainerBase
    * Move up through the flash settings
    * @since 3.0
    */
    virtual void IncreaseFlashSettingL();
   
    /**
    * From CCamPreCaptureContainerBase
    * Move down through the flash settings
    * @since 3.0
    */ 
    virtual void DecreaseFlashSettingL();                                         
                                          
    /**
    * From CCamPreCaptureContainerBase
    * Process a key event that should change the current capture state
    * E.g. start/pause recording, start/stop capture
    * @since 3.0
    * @param aKeyEvent the key press that initiates the capture
    * @return whether or not the key was consumed
    */ 
    virtual TKeyResponse HandleCaptureKeyEventL( const TKeyEvent& aKeyEvent );                                          
    
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
    * Process a key event that could change the current capture state
    * E.g. start autofocus, revert from post-capture to pre-capture views        
    * @since 3.0
    * @param aKeyEvent the key press that initiates the capture
    * @param aType the key type for the event
    * @return whether or not the key was consumed
    */         
    virtual TKeyResponse HandleShutterKeyEventL( const TKeyEvent& aKeyEvent,
                                                       TEventCode aType );
       
    /**
    * Gets the resource id of AP items.
    * Resource is selected based on embedded status and user mode.
    */
    virtual TInt GetAPResourceId() const;


  // -------------------------------------------------------
  // New methods
  private: 
    
    /**
    * Draws blank softkeys
    * @since 3.0
    */
    void BlankSoftkeysL();

    /**
    * Creates the icons used to indicate video file type
    * @since 3.0
    */
    void CreateFiletypeIndicatorL();

    /**
    * Sets the video file type icon depending on current setting
    * @since 3.0
    */
    void SetFileTypeIndicator();
    

  // -------------------------------------------------------
  // Constructors
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
    * @param aSetupPaneHandler handle to horizontal setup pane (product specific)
    * @param aView reference to the view containing this container
    */
    CCamVideoPreCaptureContainer( 
        CCamAppController& aController,
        CAknView& aView );

  // =======================================================
  // Data
  private: 
    // Values representing current recording state
    enum TCamRecordState
      {
      ECamNotRecording,
      ECamRecording,
      ECamRecordPaused
      };

    // Current recording states
    TCamRecordState iRecordState;
    
    // resource for viewfinder layout
    TInt iVFRes;
    
    // Video file type indicator
    CCamIndicator* iFileTypeIndicator;

    
    // Normal location of video file type indicator
    TRect iFileTypeIndicatorPosition;
    
    // Position to move the resolution icon to during video capture
    TRect iResolutionIndicatorVidcapPosition;
    
    MTouchFeedback *iFeedback;

  // =======================================================
  };

#endif // CAMVIDEOPRECAPTURECONTAINER_H
    
// End of File CAMVIDEOPRECAPTURECONTAINER_H
