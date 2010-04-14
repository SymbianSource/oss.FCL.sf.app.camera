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
* Description:  Controls the state of the zoom*
*/



#ifndef CAMZOOMMODEL_H
#define CAMZOOMMODEL_H

//  INCLUDES
#include <e32base.h>
#include "CamPSI.h" 
#include "CamAppController.h"
#include "CamControllerObservers.h"    // MCamControllerObserver
#include "CamZoomUpdateManager.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCamZoomPane;

// CLASS DECLARATION

/**
*  This class contains the logic for handling zoom operations
*  with support for Optical, Digital and Extended Digital zoom
*
*  @since 3.0
*/
class CCamZoomModel : public CBase,
                      public MCamControllerObserver
  {
  // =========================================================================
  // Types
  public: 

    enum TCamZoomModelState
        {
        ECamZoomModelStateZoomNone,
        ECamZoomModelStateZoomIn,
        ECamZoomModelStateZoomOut,
        ECamZoomModelStateZoomTo
        };

    /* Moved to CamSettingsInternal.hrh   
    // Used to represent the current zoom mode used with the Engine
    enum TCamZoomMode
        {
        ECamZoomModeUndefined,
        ECamZoomModeOptical,
        ECamZoomModeDigital, 
        ECamZoomModeExtended
        }; */
            
    // Used to describe the current zoom mode boundary.
    enum TCamZoomBoundary
        {
        ECamZoomBoundaryUndefined, // Not checked yet for current zoom step
        ECamZoomBoundaryNone,      // Checked, but not at a boundary
        ECamZoomBoundaryOptDig,    // Checked, and at Optical/Digital boundary
        ECamZoomBoundaryDigExt     // Checked, and at Digital/Extended boundary
        };
        
    // PauseState represents handling of pausing zooming in at
    // zoom mode boundarys (between optical/digital and digital/extended)
    enum TPauseState
        {
        EPauseStateNone,    // Not at a boundary
        EPauseStatePaused,  // Just hit a boundary, all zoom-in events ignored until released
        EPauseStateReleased // Just released zoom key, allow next zoom 
        };

  // =========================================================================
  // Methods

  // -------------------------------------------------------------------------
  // Constructors and destructor
  public:  
        
    /**
    * Two-phased constructor.
    */
    static CCamZoomModel* NewL( CCamAppController& aController, CCamZoomPane* aZoomPane );
    
    /**
    * Destructor.
    */
    virtual ~CCamZoomModel();

  // -------------------------------------------------------------------------
  // From CCamAppControllerBase
  public: 

    /**
    * Handle controller event.
    * @since 3.0
    * @param aEvent the controller event that has occurred
    * @param aError any reported error
    */
    virtual void HandleControllerEventL( TCamControllerEvent aEvent,
                                         TInt aError );
       



  // -------------------------------------------------------------------------
  // New functions
  public: 
        
    /**
    * Attempts to zoom in one step, if possible.
    * Zooming will continue until StopZoom() is called.
    * @since 3.0        
    */
    void ZoomIn( TBool aOneClick = EFalse );
    
    /**
    * Attempts to zoom out one step, if possible.
    * Zooming will continue until StopZoom() is called.
    * @since 3.0        
    */        
    void ZoomOut( TBool aOneClick = EFalse );
    
    /**
    * Attempts to zoom to a specific point, if possible.
    * 
    * @since 5.0        
    */   
    void ZoomTo( TInt aValue );
    
    /**
    * Stops an ongoing zoom in/out operation
    * @since 3.0        
    */
    void StopZoom();
    
    /**
    * Checks the current state of the Camera application, and adjusts the
    * min/max range of the zoom pane accordingly.        
    * @since 3.0        
    */                
    void RefreshSettings();
    
    /**
    * Timer callback, used during a zoom in/out to trigger repeat
    * zoom steps while the key is held down.
    * @param aObject Pointer to a CCamZoomModel object
    * @return TInt KErrNone if ok
    * @since 3.0        
    */                        
    static TInt ZoomTimerCallback( TAny* aObject );   
            
    /**
    * Returns whether the zoom model is currently zooming in or out
    * @since 3.0
    * @return ETrue if zooming, else EFalse
    */
    TBool IsCurrentlyZooming() const;

    /**
    * Returns zoom model state
    * @since S60 v5.0
    * @return TCamZoomModelState
    */
    TCamZoomModelState ZoomingState();  
    
    /**
    * Indicates that the zoom level should be reset to default
    * when the engine is next prepared
    * @since 2.8
    * @param aReset ETrue if the zoom pane should be reset, else EFalse
    */
    void ResetToDefaultAfterPrepare( TBool aReset );
    
    /**
    * Indicates whether or not the zoom level is waiting to be reset to default
    * @since 3.0
    * @return TBool ETrue if a reset is pending, othewise EFalse
    */       
    TBool IsResetPending() const;
    
    /***
    * Resets the zoom (digital, and, if available, optical) to 1x
    * @since 3.0
    */                        
    void ResetZoomTo1x();
    
    /***
     * Use an external zoom multiplier factor to zoom quickly
     * with lesser number of transitions
     * @since 5.1
     */
    void SetZoomMultiplier( TInt aZoomStepMultiplier );

  private:
    
    /**
    * Called by the timer callback function to perform 
    * the operation of one timer "tick". Will zoom in or out
    * one step to continue the ongoing zoom operation.
    * @since 3.0
    */        
    void ZoomTimerTick();
    
    /**
    * Starts the zoom timer if not already running.
    * @since 3.0
    */        
    void StartZoomTimer();
    
    /**
    * Returns the maximum zoom step from the supplied step array.
    * The returned step takes into account the currently active 
    * camera and capture mode.
    * @param aStepArray The step array from the PSI
    * @since 3.0
    * @return The maximum step from the array.
    */                
    TInt MaxZoomStep( const TCamMaxZoomSteps& aStepArray ) const;
    
    /***
    * Checks the zoom mode is that expected.  If not, will be set.
    * @param aMode The zoom mode to check for
    * @since 3.0
    */                        
    void CheckZoomMode( TCamZoomMode aMode );
    
    /***
    * Reads the digital zoom setting and updates internal state
    * @since 3.0
    */                                        
    void ReadDigitalZoomSetting();
    
    /***
    * Reads the optical zoom setting and updates internal state
    * @since 3.0
    */                                                
    void ReadOpticalZoomSetting();
    
    /***
    * Checks that the current zoom levels for optical/digital zoom
    * do not exceed current limits.  If they do, the level is set
    * to the available limit.
    * @since 3.0
    */                                        
    void CheckZoomLimit();
    
    /***
    * Returns whether EXTENDED zoom is allowed based on 
    * settings for the current resolution, and user options.
    * @since 3.0
    * @returns ETrue if allowed, else EFalse
    */           
    TBool AllowExtZoom() const;
    
    /***
    * Returns whether OPTICAL zoom is allowed based on 
    * settings for the current resolution, and user options.
    * @since 3.0
    * @returns ETrue if allowed, else EFalse
    */                   
    TBool AllowOptZoom() const;
    
    /***
    * Returns whether DIGITAL zoom is allowed based on 
    * settings for the current resolution, and user options.
    * @since 3.0
    * @returns ETrue if allowed, else EFalse
    */                           
    TBool AllowDigZoom() const;
    
    /***
    * Returns whether zooming should be paused at the 
    * specified zoom boundary.
    * @since 3.0
    * @param aBoundary
    * @returns ETrue if pause required, else EFalse
    */                           
    TBool PauseAtBoundary( TCamZoomBoundary aBoundary ) const;
    
    /***
    * Checks whether doing the next zoom operation will move 
    * from one type of zoom to another.
    * 
    * Will return any of: 
    *   - ECamZoomBoundaryNone (not on a boundary)
    *   - ECamZoomBoundaryOptDig (on optical/digital boundary)
    *   - ECamZoomBoundaryDigExt (on digital/extended boundary)
    *                              
    * @since 3.0
    * @returns The current zoom boundary state
    */                        
    TCamZoomBoundary CheckBoundary() const;
    
    /***
    * Returns the current zoom type (Optical/Digital/Extended), based
    * on the current zoom level.  Optionally takes a parameter, to 
    * see what zoom type would be used if zoom level adjusted by 
    * the stepmodifier (positive or negative)
    * @since 3.0
    * @param aStepModifier Value to add to current zoom
    * @returns The zoom mode of current zoom value + step modifier
    */                        
    TCamZoomMode CurrentZoomType( TInt aStepModifier = 0 ) const;
    
    /***
    * Returns the number of zoom steps that the next zoom will jump.
    * @since 3.0        
    * @returns The number of zoom steps to jump.
    */                        
    TInt ZoomStepsToJump() const;
    
    /***
    * Returns (via references) how the zoom steps should be split up
    * between the optical, digital and extended zoom modes.
    * @since 3.0
    * @param aOpt The optical steps to move.
    * @param aDig The digital steps to move.
    * @param aExt The extended steps to move.
    */                        
    void ZoomStepsToJump( TInt& aOpt, TInt& aDig, TInt& aExt ) const;
    
    /**
    * Returns the current combined (optical + digital) zoom         
    * for the current zoom model state.
    * @since 3.0
    * @returns The current combined zoom
    */
    TInt CurrentZoom() const;
    
    /**
    * Read the current resolution id.
    * 
    */
    void ReadCurrentResolution();

  // -------------------------------------------------------------------------
  // Private constructors
  private:
  
    /**
    * C++ default constructor.
    */
    CCamZoomModel( CCamAppController& aController, CCamZoomPane* aPane );
    
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

  // =========================================================================
  // Data
  private:

    // Arrays from the PSI, describing maximum zoom steps for 
    // different zoom modes and different resolutions and media
    TCamMaxZoomSteps iOptZoomSteps; // Optical zoom steps
    TCamMaxZoomSteps iDigZoomSteps; // Standard digital zoom steps
    TCamMaxZoomSteps iExtZoomSteps; // Extended digital zoom steps
    
    // These values are the maximum zoom steps for different zoom modes.
    // Will be updated when RefreshSettings() is called.
    TInt iMaxZoomStepOpt;	// Max Optical step
    TInt iMaxZoomStepDig;	// Max Digital step
    TInt iMaxZoomStepExt;	// Max Extended step
    
    // Zoom setting: Allow digital and/or extended, where to pause.
    TCamSettingsDigitalZoom iZoomSetting;
    
    // Whether to allow Optical zoom during video
    TBool iAllowOptZoom;
    
    // The current optical and digital zoom steps
    TInt iCurZoomStepOptical;
    TInt iCurZoomStepDigital;
    
    // Max allowed zoom step (taking into account optical, digital and extended)
    TInt iMaxZoomStep;

    // Zoom step/jump multiplier for skipping some zoom levels 
    // (for quick zooming to max and min zoom levels)  
    TInt iZoomStepMultiplier;
    
    // Stores the current state of the model, and the last-notified
    // engine state.
    TCamZoomModelState iState;
    //        TCamEngineState iEngineState;
    TUint iCameraState;

    // Stores the latest video or image resolution id.
    TInt iCurrentResolution;
    
    // Reference to the controller used for setting the zoom step and mode.
    CCamAppController& iController;
    
    // Update manager is used to minimize amount of updates sent to driver.
    CCamZoomUpdateManager* iCamZoomUpdateManager;
    
    // Timer for zooming smoothly
    CPeriodic* iZoomTimer;   
    CCamZoomPane* iPane;
    
    // Keeps track of the current zoom mode
    TCamZoomMode iZoomMode;
    
    // Stores a look-and-feel structure for the zoom bar.         
    TCamZoomLAF iZoomLAF;
    
    // The current pause state (none/paused/released)
    TPauseState iPauseState;        
    
    // Whether the camera app is currently recording video
    // Used to identify when we *stop* recording
    TBool iRecordingVideo;
    
    // Whether or not the zoompane should reset to default value on next prepare
    TBool iResetPending;

  // =========================================================================
  };

#endif      // CAMZOOMMODEL_H   
            
// End of File
