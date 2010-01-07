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
* Description:  Control for displaying recording progress bar in navi pane.
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*/


#ifndef CAMNAVIPROGRESSBARMODEL_H
#define CAMNAVIPROGRESSBARMODEL_H

// INCLUDES
#include <bldvariant.hrh>
 
#include <coecntrl.h>
#include <AknUtils.h>
#include "CamAppController.h"
#include "CamSettings.hrh"
#include "CamResourceLoader.h"
#include "CamAppUiBase.h"
#include "CamObservable.h"
#include "CamObserver.h"
#include "mcamcameraobserver.h"

class CCamObserverHandler;
class CCamTextItem;

// CONSTANTS
const TInt KMaxRecordTimeTextLength = 40;

// FORWARD DECLARATIONS
class MAknsSkinInstance;
class CCamDecorator;

// CLASS DECLARATION

/**
* Control for displaying recording time info in navi pane.
* @since 2.8
*/
class CCamNaviProgressBarModel : public CBase,
                                 public MCamCameraObserver,
                                 public MCamControllerObserver,
                                 public MCamResourceLoader,
                                 public MCamObservable,
                                 public MCamObserver
  {
  public: // Constructors and destructor

    /**
    * Static factory construction function
    * @since 2.8
    * @param aController Reference to CCamAppController
    * @return pointer to constructed instance of CCamNaviProgressBarModel
    */
    static CCamNaviProgressBarModel* NewL( CCamAppController& aController );

    /**
    * Destructor.
    */
    virtual ~CCamNaviProgressBarModel();

  // -------------------------------------------------------
  // From MCamCameraObserver
  public:

    virtual void  HandleCameraEventL( TInt              aStatus, 
                                      TCamCameraEventId aEventId, 
                                      TAny*             aEventData = NULL );

  // -------------------------------------------------------
  public:

    /**
    * Draw Navi-progress bar
    * @since 3.0
    * @param aGc Handle to Window graphics context
    * @param aControl Pointer to the Control
    */
    void DrawProgressBar( CBitmapContext& aGc, const CCoeControl* aControl ) const;
    
    /**
    * Sets a new extent to render into
    * @param aExtent New extent
    */
    void SetExtentL( const TRect& aExtent );
        
  protected:

    /**
    * C++ constructor.
    * @since 2.8
    * @param aController Reference to CCamAppController
    */
    CCamNaviProgressBarModel( CCamAppController& aController );
    
    /**
    * Symbian OS 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();
    
  private: // Functions from base classes

    /**
    * From CCoeControl Draw the view
    * @since 3.0
    * @param aGc Handle to Window graphics context
    * @param aControl Pointer to the control
    */
    virtual void DrawProgBar( CBitmapContext& aGc, const CCoeControl* aControl ) const;
    
    
    /**
    * From MCamControllerObserver
    * Handle an event from CCamAppController.
    * @since 2.8
    * @param aEvent the type of event
    * @param aError error code
    */
    virtual void HandleControllerEventL( TCamControllerEvent aEvent,
                                         TInt                aError );

  public:  // From MCamResourceLoader

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

  public: // From MCamObservable

    /**
    * From MCamObservable
    * @param aObserver Observer to register
    * @since 3.0
    */
    void RegisterObserverL(MCamObserver* aObserver);
    
    /**
    * From MCamObservable
    * @param aObserver Observer to deregister
    * @since 3.0
    */
    void DeregisterObserver(MCamObserver* aObserver);
    
    /**
    * From MCamObservable
    * @param aEvent Event code to broadcast
    * @since 3.0
    */
    void BroadcastEvent(TCamObserverEvent aEvent);

  public: // From MCamObserver

    /**
    * From MCamObserver
    * @param aEvent The event code
    * @since 3.0
    */
    virtual void HandleObservedEvent(TCamObserverEvent aEvent);

  private: // New functions
    
    /**
    * Formats the elapsed/remaining record times
    * @since 2.8
    */
    void FormatTimeL();
    
    /**
    * Draws the elapsed record time to the navi pane 
    * @since 2.8
    */
    void DrawElapsedTimeText( CBitmapContext& aGc ) const;
    
    /**
    * Draws the remaining record time to the navi pane 
    * @since 2.8
    */
    void DrawRemainingTimeText( CBitmapContext&    aGc, 
                                MAknsSkinInstance* aSkin ) const;

    /**
    * Draws the progress bar in the navi pane
    */
    void DrawProgressBar( CBitmapContext& aGc ) const;

    /**
    * Callback for the minute-minder flash timer
    */
    static TInt FlashCallBack( TAny* aAny );
    
    /**
    * Read nontouch layout for primary camera
    */
    void NonTouchLayoutL();
    
    /**
    * Read nontouch layout for secondary camera
    */    
    void NonTouchLayoutSecondaryL();

    /**
    * Read touch layout
    */    
    void TouchLayoutL();

  protected: //data
    
    CCamAppController& iController; 
    
    TTimeIntervalMicroSeconds iRecordTimeElapsed;
    TTimeIntervalMicroSeconds iRecordTimeRemaining;
    
    TBuf<KMaxRecordTimeTextLength> iElapsedTimeText;
    TBuf<KMaxRecordTimeTextLength> iRemainingTimeText;
    
    // Rectangle for drawing the elapsed time text to.
    TAknLayoutText iElapsedTimeTextLayout; 
    TAknLayoutText iRemainingTimeTextLayout; 

    // own
    CCamTextItem* iElapsedTimeTextItem;

    // own
    CCamTextItem* iRemainingTimeTextItem;
    
    CFbsBitmap* iPhoneIcon;
    CFbsBitmap* iPhoneIconMask;
    CFbsBitmap* iMMCIcon;
    CFbsBitmap* iMMCIconMask;
    CFbsBitmap* iMassStorageIcon;
    CFbsBitmap* iMassStorageIconMask;

    /**
    * Progress bar graphic.
    * 
    * Own.
    */
    CFbsBitmap* iProgressBarBitmap;

    /**
    * Mask for progress bar graphic.
    * 
    * Own.
    */
    CFbsBitmap* iProgressBarMask;

    /**
    * Background for progress bar area.
    * 
    * Own.
    */
    CFbsBitmap* iProgressBarFrame;

    TCamMediaStorage iStorageLocation;
    
    // Resource string used to format the time text
    HBufC* iTimeFormat;
    
    TAknLayoutRect iProgressIconRect;
    TAknLayoutRect iProgressBarRect;
    
    // Layout information for the video storage icon.
    TAknLayoutRect iVidStorageIconRect;
    
    TCamOrientation iCamOrientation;
        
    CCamDecorator* iDecorator;
        
    // Rect to draw into
    TRect iExtent;
    
    // Handles observers
    CCamObserverHandler* iObserverHandler;

    // Timer for the flashing of the elapsed time every minute
    CPeriodic* iFlashingTimer;

    // When true, the elapsed time isn't drawn
    TBool iFlash;
  };

#endif // CAMNAVIPROGRESSBARMODEL_H

// End of File
