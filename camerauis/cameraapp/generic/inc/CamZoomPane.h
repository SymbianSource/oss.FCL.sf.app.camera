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
* Description:  Class for Camera Zoom Pane*
*/



#ifndef CAMZOOMPANE_H
#define CAMZOOMPANE_H

//  INCLUDES
#include <coecntrl.h>
#include <eiklabel.h>
#include <AknUtils.h>               // for TAknLayout
#include "CamPSI.h"                 // For Product Specific Information
#include "CamAppController.h"   // For controller definition
#include "CamSettings.hrh"          // For TCamPhotoSizeId
#include "CamSettingsInternal.hrh"
#include "CamResourceLoader.h"
#include "CamZoomModel.h"
#include "CamBmpRotatorAo.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCamRemConObserver;
class MTouchFeedback;

// CLASS DECLARATION

/**
*  Class for showing the current zoom level in a side pane.
*
*  @since 2.8
*/
class CCamZoomPane : public CBase, 
                     public MCamControllerObserver,
                     public MCamResourceLoader
    {
    // Private data type used internally by the zoom pane 
    // to store the orientation it should be drawn    
    enum TZPOrientation
        {
        EZPOrientationVertical = 0,
        EZPOrientationHorizontal = 1
        };
    
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aController handle to camera application controller
        * @param aOverlayViewFinder whether zoom pane is drawn over view finder
        */
        static CCamZoomPane* NewL( CCamAppController& aController,
                                   TBool aOverlayViewFinder );
        
        /**
        * Destructor.
        */
        virtual ~CCamZoomPane();

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
        * Sets the range of values to be represented on the Zoom Pane "shaft".
        * These values should be retrieved from the CCaeEngine
        * @since 2.8
        * @param aMin The minimum supported value (typically zero)
        * @param aMax The maximum supported value (one or more)
        * @return KErrNone if parameters are valid, else KErrArgument
        */
        TInt SetZoomRange( TInt aMin, TInt aMax );

        /**
        * Sets the current zoom level.  Must be within (inclusive) range passed
        * to the SetZoomRange function.
        * @since 2.8
        * @param aZoom New zoom value to use.
        * @return KErrNone if parameter valid, else KErrArgument
        */    
        TInt SetZoomValue( TInt aZoom );

        /**
        * Sets the keys to be intepreted as zoom keys
        * @since 3.0
        * @param aZoomIn Zoom in key scancode
        * @param aZoomOut Zoom out key scancode
        */
        void SetZoomKeys( const RArray<TInt>& aZoomIn,
                          const RArray<TInt>& aZoomOut );

        /**
        * Gets the current zoom step
        * @since 2.8
        * @returns The current zoom step
        */
        TInt ZoomValue() const;

        /**
        * Returns whether or not the Zoom Pane can currently be shown.
        * @since 2.8
        * @return ETrue if the Zoom Pane is allowed to be shown, else EFalse
        */    
        TBool OkToShowPane() const;

        /**
        * Is the zoom pane is visible
        * @since 2.8
        * @return ETrue if the zoom pane is visible
        */   
        TBool IsVisible() const;

        /**
        * Tells the zoom pane whether it is on display
        * @since 2.8
        * @param aVisible ETrue to make the zoom pane invisible
        * @param aRedraw Whether to force a redraw event
        */   
        void MakeVisible( TBool aVisible, TBool aRedraw );

        /**
        * Draws the zoom pane
        * @since 2.8
        * @param aGc The graphics context to use for drawing
        */
        void Draw( CBitmapContext& aGc ) const;

        /**
        * Set draw rect
        * @since 2.8
        * @param aRect Rect zoom pane draws into
        */
        void SetRect( TRect aRect );

        /**
        * Handles zoom-key operation, passed in from the container.
        * @since 2.8
        * @param aKeyEvent The key event
        * @param aType the key type
        * @return Whether this key event was consumed or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Returns the zoom pane rect
        * @since 3.0
        * @return the zoom pane rect
        */
        TRect Rect() const;
        
        /**
        * Called when the parent gains or loses foreground
        * @param aForeground ETrue if gained foreground, else EFalse
        * @since 3.0
        */
        void HandleForegroundEvent( TBool aForeground );

        /**
        * Sets the number of zoom steps for each type of zoom to be
        * reflected in the zoom pane shaft
        * @param aOptSteps The number of optical zoom steps
        * @param aStdSteps The number of digital zoom steps
        * @param aExtSteps The number of extended digital zoom steps
        * @since 3.0
        */
        void SetZoomSteps( TInt aOptSteps, TInt aStdSteps, TInt aExtSteps );
        
        /**
        * Returns whether there is currently an ongoing zoom operation.
        * @since 3.0
        * @return ETrue if zooming, else EFalse
        */
        TBool IsCurrentlyZooming() const;
        
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
        
        /**
        * Sets a point position to be based on another point plus an optional offset.
        * Used for moving along the slider.  Whether the X or Y part of the point 
        * changes is dependent on the current orientation
        * @since 3.0
        * @param aPoint This point will be overwritten with the new X/Y values
        * @param aOffset This is the basis for the new value for 'aPoint'
        * @param aAddditional This is an additional modification to 'aOffset' value
        */       
        void SetPoint( TPoint& aPoint, const TPoint& aOffset, TInt aAdditional = 0) const; 

        /**
        * Returns whether the current zoom value is the minimum zoom
        * @since 3.1
        * @return Whether the current zoom value is the minimum zoom
        */
        TBool IsZoomAtMinimum() const;

        /**
        * Stops all zooming activity
        */
        void StopZoom();
        
        /***
        * Resets the zoom (digital, and, if available, optical) to 1x
        * @since 3.0
        */                        
        void ResetZoomTo1x();

        /***
        * Handle pointer events
        * @since S60 v5.0
        * @param TPointerEvent, pointer event
        * @return TBool, ETrue if the pointer event was used
        */  
        TBool HandlePointerEventL( const TPointerEvent& aPointerEvent ); 
        
        /***
        * Handle pointer events
        * @since S60 v5.0
        * @param TInt, Pointer's Y-position
        * @return TBool, ETrue if the pointer event was used
        */  
        TBool StartTouchZoomL( TInt aPointer ); 
        
        

    public: // Functions from base classes        
    protected:  // New functions        
    protected:  // Functions from CCoeControl        

        /**
        * Handles events from the controller
        * @since 2.8
        * @param aEvent The controller event that has occurred
        * @param aError An associated error code
        */
        void HandleControllerEventL( TCamControllerEvent aEvent, TInt aError );

    protected:  // Functions from base classes
    private:

        /**
        * C++ default constructor.
        * since 2.8
        * @param aRect parent rect
        * @param aController handle to camera application controller
        * @param aOverlayViewFinder whether zoom pane is drawn over view finder
        */
        CCamZoomPane( CCamAppController& aController, 
                      TBool aOverlayViewFinder );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();        

        /**
        * Reads layout information from a resource
        * @since 2.8
        */
        void ReadLayoutL();

        /**
        * Updates Layout and size of control
        * @since S60 5.0
        */
        void SizeChanged(); 

        /**
        * Draws the thumb on the zoom pane
        * @since 3.0
        * @param aGc The graphics context to draw to
        */
        void DrawThumb( CBitmapContext& aGc ) const;

        /**
        * Draws the shaft of the zoom pane
        * @since 3.0
        * @param aGc The graphics context to draw to
        */
        void DrawShaft( CBitmapContext& aGc ) const;
        
        /**
        * Draws the plus/minus icons for the zoom pane
        * @since 3.0
        * @param aGc The graphics context to draw to
        */
        void DrawPlusMinus( CBitmapContext& aGc ) const;
        
        /**
        * Returns the current Zoom Pane orientation based on stored
        * settings and whether the current camera supports horizontal zoom bar
        * @since 3.0
        * @return The orientation (veritcal or horizontal)
        */
        TZPOrientation PaneOrientation() const;

        /**
        * Read nontouch layout
        */
        void TouchLayout();

        /**
        * Read touch layout
        */
        void NonTouchLayout(); 

        /*
        * Reads zoom keys from the appropriate array
        */
        void ReadZoomKeys( const RArray<TInt>& aSource,
                           RArray<TInt>& aTarget );
        /*
        * Checks for the zoom key with the mapped key's scan code
        */
        TBool CheckForZoomKey( const TKeyEvent& aKeyEvent,
                               const RArray<TInt>& aKeys );

        /**
        * Touchfeedback
        * @since S60 v5.0
        */
        void Touchfeedback(); 

    public:     // Data
    
    protected:  // Data

    private:    // Data

        // The bitmaps for the graphical components.
        CFbsBitmap* iIconZoomTop[2];  
        CFbsBitmap* iIconZoomMiddle[2];
        CFbsBitmap* iIconZoomBottom[2];
        CFbsBitmap* iIconZoomMarker[2];
        CFbsBitmap* iIconZoomMarkerGray[2]; 
        CFbsBitmap* iIconZoomMin[2];
        CFbsBitmap* iIconZoomMax[2];
        CFbsBitmap* iIconZoomMarkerCurrent;

        TRect iZoomPaneRect;
        TRect iSliderParentRect;
        TPoint iPlusPoint;
        TPoint iMinusPoint;
        TPoint iTopPoint;
        TPoint iMiddlePoint;
        TPoint iBottomPoint;

        //Holds the maximum 'travel' of the thumb along the slider 
        TInt iMaxOffset; 
        
        // Minimum supported zoom level
        TInt iMinZoom;    

        // Maximum supported zoom level
        TInt iMaxZoom;

        // Actual zoom level
        TInt iCurZoom;
        
        // Targeted zoom level to be indicated by the thumb
        TInt iTrgZoom;

        // Previous pointer Y-position
        TInt iPointerPrevi; 

        // Whether we are currently recording video or not.
        TBool iRecordingVideo;

        // Reference to the Controller
        CCamAppController& iController;

        // Represents a bitmask of TCamVideoZoom for video zoom features
        TInt iZoomSupport;

        // Determine from PSI if zoom pane are overlaid on VF
        TBool iOverlayViewFinder;

        // is the zoom pane visible
        TBool iVisible;
        
        // Determine whether or not touch is active
        TBool iTouchActive;

        // The current engine state, updated if it changes
        TCamCameraState iCameraState;

        // The zoom model, owned by this control
        CCamZoomModel* iModel;

        // The number of key repeats received in a row.
        TInt iRepeatCounter;

        // The optical, digital and extended zoom steps to be shown on the zoom bar
        TInt iOptSteps;
        TInt iStdSteps;
        TInt iExtSteps;
        
        // Stores the look and feel info of the zoom bar
        TCamZoomLAF iZoomLAF;
         
        // Stores the orientation of the zoom pane (vertical or horizontal)
        // as read from the Central Repository
        TZPOrientation iOrientation;

        // Whether or not product uses volume keys for zoom
        TBool iZoomUsingVolumeKeys;

        // Whether or not product uses navi-key left and right for zoom
        TBool iZoomUsingNaviKeys;

        // Performs zoom frame/thumb bitmap rotation when needed
        CCamBmpRotatorAo* iRotatorAo;

		// Timer used to remove zoom pane after a short delay
		CCamTimer* iZoomTimer;
        MTouchFeedback* iFeedback; // Not own

        // observer to receive volume key presses
		CCamRemConObserver* iVolumeKeyObserver;
        
        RArray<TInt> iZoomInKeys;
        RArray<TInt> iZoomOutKeys;
    };

#endif      // CAMZOOMPANE_H
            
// End of File
