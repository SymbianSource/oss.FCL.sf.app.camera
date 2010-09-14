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
* Description:  Container base class for pre-capture views*
*/



// INCLUDE FILES
#include <eikmenub.h>
#include <aknview.h>
#include <w32std.h>
#include <gulutil.h>
#include <AknIconUtils.h>
#include <barsread.h>    // resource reader
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <eikenv.h>
#include <eikappui.h> // For CCoeAppUiBase
#include <eikapp.h>   // For CEikApplication
#include <layoutmetadata.cdl.h>            
#include <aknlayoutscalable_apps.cdl.h>    

#include <cameraapp.mbg>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <touchfeedback.h>
#include <akntoolbar.h>
#include <akntoolbarextension.h>

#include "CamAppUiBase.h"
#include "CamPreCaptureContainerBase.h"
#include "CamLogger.h"
#include "CamSidePane.h"
#include "CamUtility.h"
#include "CamPanic.h"
#include "Cam.hrh"
#include "CamAppUi.h"   
#include "CamIndicator.h"
#include "CamIndicatorResourceReader.h"
#include "CamIndicatorData.h"
#include "CamSelfTimer.h"
#include "CamPreCaptureViewBase.h"
#include "CamStillPreCaptureView.h"
#include "CamSettings.hrh"
#include "CamNaviCounterControl.h"
#include "CamNaviProgressBarControl.h"
#include "camuiconstants.h" // KCamDirectColorVal
#include "camactivepalettehandler.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamPreCaptureContainerBaseTraces.h"
#endif
#include "camconfiguration.h"
#include "CameraUiConfigManager.h"
#include "camstartuplogo.h"
#include "camvfgridinterface.h"
#include "camlinevfgriddrawer.h"
#include "camcapturebuttoncontainer.h"

// CONSTANTS
const TInt KZoomPanelTimeout = 4000000;     // 4s 
const TInt KReticuleFlashTimeout = 250000;  // Quarter of a second (in microseconds).
const TInt KAFIconCorners = 4; // Hip to be square 

const TInt KIndicatorBlinkDelay = 250 * 1000;
const TInt KNumberOfBlinks = 3;
const TInt KNumberOfBlinksVideo = 8;

const TSize  KCaptureButtonSize( 50, 50 );
const TSize  KAdditionalArea( 25, 11 );
const TInt32 KCaptureButtonYDelta( 10 );

// Snapshot data is needed in timelapse mode
const TUint KCameraEventInterest = ( ECamCameraEventClassVfControl      
                                   | ECamCameraEventClassVfData 
                                   | ECamCameraEventClassSsData 
                                   );



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCamPreCaptureContainerBase::~CCamPreCaptureContainerBase
// Destructor
// ---------------------------------------------------------
//
CCamPreCaptureContainerBase::~CCamPreCaptureContainerBase()
  {
  PRINT( _L("Camera => ~CCamPreCaptureContainerBase" ))

  if( iCaptureButtonContainer )
      {
      delete iCaptureButtonContainer;
      }

  iController.ViewfinderWindowDeleted( &Window() );
  
  iController.RemoveSettingsObserver  ( this );
  iController.RemoveCameraObserver    ( this );
  iController.RemoveControllerObserver( this );    
  
    if ( iZoomTimer && iZoomTimer->IsActive() )
    {    
    iZoomTimer->Cancel();
    }
  delete iZoomTimer;
  delete iVfGridDrawer;

  delete iStartupLogo;

  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
      {
      iAfIcons.ResetAndDestroy();
      iLayouts.Close(); 
      if ( iReticuleTimer && iReticuleTimer->IsActive() )
        {    
        iReticuleTimer->Cancel();
        }
      delete iReticuleTimer;    
      }

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

  iResolutionIndicators.ResetAndDestroy();
  iResolutionIndicators.Close();
  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsLocationSupported() )
      {
      PRINT( _L("Camera => ~CCamPreCaptureContainerBase - delete iLocationIconController" ))
      delete iLocationIconController;
      }

  delete iOffScreenBitmap;
  delete iBitmapDevice;
  delete iBitmapGc;
  
  // if shutting down, observers are removed automatically  
  if ( 
      iController.IsAppUiAvailable() &&
      appUi && !iController.IsInShutdownMode() )
    {
    // Deregister as burst mode observer
    appUi->RemoveBurstModeObserver( this );
    }

  delete iBatteryPaneController;
  
  delete iViewFinderBackup;
  iViewFinderBackup = NULL;
  iReceivedVfFrame = EFalse;
  if( iIndBlinkTimer ) 
    {
    if( iIndBlinkTimer->IsActive() )
      {
      iIndBlinkTimer->Cancel();
      }
    delete iIndBlinkTimer;
    iIndBlinkTimer = NULL;
    }
  PRINT( _L("Camera <= ~CCamPreCaptureContainerBase" ))
  }

// ---------------------------------------------------------
// CCamPreCaptureContainerBase::BaseConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamPreCaptureContainerBase::BaseConstructL( const TRect& aRect )
  {
  PRINT( _L("Camera => CCamPreCaptureContainerBase::BaseConstructL ") );
  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
  iZoomPane = appUi->ZoomPane();
  
  CCamContainerBase::BaseConstructL( aRect );
  
  iViewFinderBackup = NULL;
  iReceivedVfFrame = EFalse;
  
  // Check whether zoom & side pane will be on the left or the right of the screen
  TInt handedInt;
  User::LeaveIfError( CamUtility::GetPsiInt( ECamPsiSidePanePosition, handedInt ) );
  iHandedness = static_cast< TCamHandedness >( handedInt );

  TResourceReader reader;                                                                                     
  iEikonEnv->CreateResourceReaderLC( reader, ROID(R_CAM_ZOOM_PANE_WIDTH_ID));    
  iZoomPaneWidth = reader.ReadInt16();
  CleanupStack::PopAndDestroy(); // reader
  
  // Create Zoom Pane based on Rect passed in on creation
  TRect paneRect( Rect() );

  // if the side pane is on the left
  if ( iHandedness == ECamHandLeft ) 
    { 
    paneRect.iBr.iX = iZoomPaneWidth;
    }
  else // the side pane is on the right
    {
    paneRect.iTl.iX = paneRect.iBr.iX - iZoomPaneWidth;    
    }
  
  User::LeaveIfError( CamUtility::GetPsiInt( ECamPsiOverLaySidePane, iOverlayViewFinder ) );

  iController.AddControllerObserverL( this ); // For zoom state changes
  iController.AddCameraObserverL( this, KCameraEventInterest );
  iController.AddSettingsObserverL( this );

  iZoomPane->MakeVisible( EFalse, EFalse ); // On app startup, it's invisible

  if ( !appUi->IsSecondCameraEnabled() )
    {
    iZoomPane->SetZoomKeys( iPrimaryZoomInKeys, iPrimaryZoomOutKeys );
    }
  else
    {     
    iZoomPane->SetZoomKeys( iSecondaryZoomInKeys, iSecondaryZoomOutKeys );
    }
  
  iZoomTimer = CPeriodic::NewL( CActive::EPriorityUserInput );
  
  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsAutoFocusSupported() &&
       !iController.UiConfigManagerPtr()->IsFaceTrackingSupported() )
      {
      iReticuleTimer = CPeriodic::NewL( CActive::EPriorityUserInput );
    
      // Layout the reticule.                                                                                           
      iAfReadyIcon = CreateAfIconL( KRgbWhite );
      iAfFocusIcon = CreateAfIconL( KRgbGreen );
      iAfErrIcon   = CreateAfIconL( KRgbRed );
      }

  iSidePane = appUi->SidePane();
  iSidePane->SetRect( paneRect );

  // Register as burst mode observer
  appUi->AddBurstModeObserverL( this );
  
  CreateResolutionIndicatorL();
  
  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsLocationSupported() )
      {
      TRect Locrect = Rect();
      PRINT( _L("Camera => CCamPreCaptureContainerBase::BaseConstructL - create iLocationIconController") )
      iLocationIconController = CCamLocationIconController::NewL( iController, *this, ETrue, Locrect );
      SetLocationIndicatorVisibility();
      }

  TBool dsaAlways( EFalse ); // Offscreen bitmap is needed with the emulator
  #ifndef __WINS__
  dsaAlways = iController.IsDirectScreenVFSupported( ETrue ) && 
              iController.IsDirectScreenVFSupported( EFalse );
  #endif
                 
  if ( !dsaAlways )
      {
      // If bitmap VF is not possible in this configuration, 
      // offscreen bitmap is not needed
      TInt color;
      TInt gray;
  
      TDisplayMode displaymode =
          CEikonEnv::Static()->WsSession().GetDefModeMaxNumColors( color, gray );
  
      // set up the offscreen bitmap
      iOffScreenBitmap = new ( ELeave ) CFbsBitmap();  
  
      User::LeaveIfError( iOffScreenBitmap->Create( Rect().Size(), EColor16MU/*displaymode*/ ) );
  
      PRINT1( _L("Camera => CCamPreCaptureContainerBase::BaseConstructL disp mode (%d)"), displaymode )
      iBitmapDevice = CFbsBitmapDevice::NewL( iOffScreenBitmap );
      User::LeaveIfError( iBitmapDevice->CreateContext( iBitmapGc ) );
      }

  iRect = ViewFinderFrameRect();
  // if using direct viewfinding pass Rect to control


  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
      {
      // Check whether we should be showing the reticule.
      // NOTE: This will only be showed in still, not video views
      iPhotoSceneUsesReticule = !iController.CurrentSceneHasForcedFocus();
      }

  // Setup viewfinder grid
  InitVfGridL( iRect );

  // Do not construct battery pane controller, if secondary camera is in use
  if ( iController.ActiveCamera() != ECamActiveCameraSecondary || 
       appUi->IsQwerty2ndCamera() ) 
    {
    iBatteryPaneController = CCamBatteryPaneController::NewL( 
                             *this, ETrue);  
    }
    // Make sure fixed toolbar is on top of this window
    DrawableWindow()->SetOrdinalPosition( KCamPreCaptureWindowOrdinalPos );

  iController.SetViewfinderWindowHandle( &Window() );
  appUi->SetViewFinderInTransit(EFalse);
  iController.StartIdleTimer();

  if (appUi->StartupLogoController())
    {
    TRAP_IGNORE(iStartupLogo = CCamStartupLogo::NewL(*appUi->StartupLogoController(), aRect));
    }

  // Capture button container  
  if ( iController.UiConfigManagerPtr()->IsCustomCaptureButtonSupported() &&
       iController.IsTouchScreenSupported() )
      {
      TRect rect = Rect();
      TPoint center( rect.Center() );
      TRect captureRect;
      captureRect.SetRect( center.iX   - KCaptureButtonSize.iWidth/2,
                           rect.iBr.iY - KCaptureButtonSize.iHeight - KCaptureButtonYDelta, 
                           center.iX   + KCaptureButtonSize.iWidth/2,
                           rect.iBr.iY - KCaptureButtonYDelta  );
      captureRect.Grow( KAdditionalArea );

      TCamCameraMode mode = ECamControllerImage;
      if ( iView.Id() == TUid::Uid( ECamViewIdVideoPreCapture ) )
          {
          mode = ECamControllerVideo;
          }
      iCaptureButtonContainer = CCamCaptureButtonContainer::NewL( iController, iView, 
                                                                  *this, captureRect, 
                                                                  mode );
      }

  PRINT( _L("Camera <= CCamPreCaptureContainerBase::BaseConstructL ") );
  }

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::CCamPreCaptureContainerBase
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamPreCaptureContainerBase::CCamPreCaptureContainerBase(
        CCamAppController& aController,
        CAknView& aView )
  : CCamContainerBase( aController, aView ),   
    iDisableRedraws( EFalse )
  {
  }

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::ZoomTimerCallback
// Callback for Zoom Pane timer
// ---------------------------------------------------------------------------
//
TInt CCamPreCaptureContainerBase::ZoomTimerCallback( TAny* aObject )
    {
    CCamPreCaptureContainerBase* cont = static_cast< CCamPreCaptureContainerBase* >( aObject );
    cont->ZoomTimerTick();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::ReticuleTimerCallback
// Callback for flashing of the focus reticule
// ---------------------------------------------------------------------------
//
TInt CCamPreCaptureContainerBase::ReticuleTimerCallback( TAny* aObject )
    {
    CCamPreCaptureContainerBase* cont = static_cast< CCamPreCaptureContainerBase* >( aObject );
    cont->iReticuleFlashOn = !cont->iReticuleFlashOn;

    // This is where we force a redraw of the reticule, if we are using
    // Direct Viewfinding.  However, if we are in burst mode, we must
    // use Bitmap Viewfinding, so only draw reticule if NOT in burst mode
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi && appUi-> IsDirectViewfinderActive() )
        {
        cont->DrawDeferred();
        }
    return KErrNone;
    }
    

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::ShowZoomPaneWithTimer
// Makes Zoom Pane visible for the period of the pane timer.
// ---------------------------------------------------------------------------
//
void CCamPreCaptureContainerBase::ShowZoomPaneWithTimer()
    {
    __ASSERT_DEBUG( iZoomTimer, CamPanic( ECamPanicNullPointer ) );
    __ASSERT_DEBUG( iZoomPane, CamPanic( ECamPanicNullPointer ) );  
    __ASSERT_DEBUG( iSidePane, CamPanic( ECamPanicNullPointer ) ); 

    PRINT(_L("Camera => CCamPreCaptureContainerBase::ShowZoomPaneWithTimer"));    

    // If already active, cancel the timer
    if ( iZoomTimer->IsActive() )
        {    
        iZoomTimer->Cancel();
        }

    iZoomTimer->Start( KZoomPanelTimeout, KZoomPanelTimeout, TCallBack( ZoomTimerCallback , this) );
    
    iZoomPane->MakeVisible( ETrue, ETrue );

    PRINT(_L("Camera <= CCamPreCaptureContainerBase::ShowZoomPaneWithTimer"));    
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::ZoomTimerTick
// Called as a result of the Zoom Pane timer expiring, switches side panes
// to hide the Zoom pane.
// ---------------------------------------------------------------------------
//
void CCamPreCaptureContainerBase::ZoomTimerTick()
    {
    PRINT(_L("Camera => CCamPreCaptureContainerBase::ZoomTimerTick")); 
    // Cancel the (potentially) recurring timer
    iZoomTimer->Cancel();

    // If zooming is still in progress (this happens if the zoom key has
    // been held down longer than the zoom pane timer), keep the pane
    // visible
    if ( iZoomPane->IsCurrentlyZooming() )
        {
        iZoomTimer->Start( KZoomPanelTimeout,
                           KZoomPanelTimeout, 
                           TCallBack( ZoomTimerCallback , this) );
        }
    else
        {
        // Make Indicator Pane visible again and hide the ZoomPane
        iZoomPane->MakeVisible( EFalse, ETrue );
        
        // background needs redrawing behind zoom pane
        iRedrawPaneBackground = ETrue;
        }
    PRINT(_L("Camera <= CCamPreCaptureContainerBase::ZoomTimerTick")); 
    }

// ----------------------------------------------------
// CCamPreCaptureContainerBase::OfferKeyEventL
// Handles this application view's command keys. Forwards other
// keys to child control(s).
// ----------------------------------------------------
//
TKeyResponse 
CCamPreCaptureContainerBase::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
    {         
    PRINT2( _L("Camera => CCamPreCaptureContainerBase::OfferKeyEventL type %d) scan code (%d)"), aType, aKeyEvent.iScanCode )

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
    // Log event only if this is the first capture key press and we are in correct state
    TCamCaptureOperation operation = iController.CurrentOperation();

    TBool isCaptureKeyForUs  = (IsCaptureKeyL( aKeyEvent, aType )
            && ( EEventKeyDown == aType && 0 == aKeyEvent.iRepeats ) );

    TBool isOperationStateOk = (ECamNoOperation == operation);
    if( isCaptureKeyForUs && isOperationStateOk )
        { 
        PERF_EVENT_START_L1( EPerfEventKeyToCapture );  
        }
#endif      

    TBool isCaptureKeyForSymbTrace  = (IsCaptureKeyL( aKeyEvent, aType )
            && ( EEventKeyDown == aType && 0 == aKeyEvent.iRepeats ) );
    if( isCaptureKeyForSymbTrace )
        {
        PRINT( _L("Camera <> CCamPreCaptureContainerBase::OfferKeyEventL Symbian Traces Active") );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP7_CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_PRI_SHUTTER_RELEASE_LAG 1" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_APP_CAPTURE_START 1" ); //CCORAPP_CAPTURE_START_START
        if ( iFocusState != EFocusStateFocusing )
            {
            OstTrace0( CAMERAAPP_PERFORMANCE, CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_APP_SHOT_TO_SNAPSHOT 1" );   //CCORAPP_SHOT_TO_SNAPSHOT_START
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_PRI_SHOT_TO_SNAPSHOT 1" );  //CCORAPP_PRI_SHOT_TO_SNAPSHOT_START
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_PRI_SHOT_TO_SAVE 1" );  //CCORAPP_PRI_SHOT_TO_SAVE_START
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_PRI_SHOT_TO_SHOT 1" );  //CCORAPP_PRI_SHOT_TO_SHOT_START
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_APP_SHOT_TO_STILL 1" ); //CCORAPP_SHOT_TO_STILL_START
            OstTrace0( CAMERAAPP_PERFORMANCE, DUP6_CCAMPRECAPTURECONTAINERBASE_OFFERKEYEVENTL, "e_CAM_PRI_SERIAL_SHOOTING 1" );   //CCORAPP_PRI_SERIAL_SHOOTING_START
            }
        }

    iController.StartIdleTimer();
    if( aType == EEventKeyUp )
        {
        iController.SetDemandKeyRelease( EFalse ); 
        }

    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
	__ASSERT_DEBUG(appUi, CamPanic(ECamPanicNullPointer) );
    if ( appUi->CheckCourtesyKeyEventL( aKeyEvent, aType, ETrue ) == EKeyWasConsumed )
        {
        PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL courtesy key") );
        return EKeyWasConsumed;
        }

    // If a control has the focus, give it the first opportunity to
    // process the key event

    if( iActivePalette 
            && iActivePalette->CoeControl()
            && iActivePalette->CoeControl()->OfferKeyEventL(aKeyEvent,aType) == EKeyWasConsumed )
        {
        PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL AP consumed") );
        return EKeyWasConsumed;
        }

    // The key press has not been handled by a visible control
    // so will be processed by the container.

    // Handle Zoom in key if we are not saving video
    // and if toolbar extension is not visible
    PRINT( _L("Camera <> CCamPreCaptureContainerBase::OfferKeyEventL B") )
    if ( IsZoomKeyL( aKeyEvent, aType ) 
            && ECamCompleting != iController.CurrentOperation() 
            && !iController.CurrentlySavingVideo()
            && !appUi->IsSecondCameraEnabled()
            && !appUi->IsToolBarExtensionVisible() )
        {       
        // Offer the key event to the zoom pane/model
        TKeyResponse resp = iZoomPane->OfferKeyEventL( aKeyEvent, aType );

        // If it was consumed, we need to keep the zoom pane visible
        if ( resp == EKeyWasConsumed )
            {
            ShowZoomPaneWithTimer();
            }
        PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL Zoom key") );    
        return resp;
        }

    else if ( EEventKey == aType
            && EKeyOK    == aKeyEvent.iCode
            && 0         == aKeyEvent.iRepeats )
        {
        TBool MSKCapture(EFalse);
        if ( !appUi->IsToolBarVisible() && aKeyEvent.iScanCode == EStdKeyDevice3 )
            {
            MSKCapture = ETrue;
            }
        if( appUi->ActiveCamera() == ECamActiveCameraPrimary
                && !MSKCapture )
            {
            // AP needs to handle this
            PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL Ok key") );    
            return EKeyWasNotConsumed;
            }
        }

    // Handle right and left navi keys. 
    // Each product defines the behaviour that is mapped to these keys.        
    else if ( EEventKey == aType
            && ( EStdKeyRightArrow == aKeyEvent.iScanCode
                    || EStdKeyLeftArrow  == aKeyEvent.iScanCode ) )
        {
        if ( HandleLeftRightNaviKeyL( aKeyEvent, aType ) == EKeyWasConsumed )
            {
            PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL left/right navi") );    
            return EKeyWasConsumed;
            }
        }

    else
        {
        // empty else statement to remove LINT error
        }

    TBool captureKey = IsCaptureKeyL( aKeyEvent, aType );

    // Stop any zooming activity if capture key detected
    if ( captureKey )
        {
        PrepareForCapture();
        }
    TBool viewFinderRunning = iReceivedVfFrame;
    if ( iController.UiConfigManagerPtr() &&
           ( iController.UiConfigManagerPtr()->
               IsDSAViewFinderSupported( ETrue ) &&
               !appUi->IsSecondCameraEnabled() ) ||
           ( iController.UiConfigManagerPtr()->
               IsDSAViewFinderSupported( EFalse ) &&
               appUi->IsSecondCameraEnabled() ) )
        {
        viewFinderRunning = appUi->IsDirectViewfinderActive();
        }

    TBool shutterKey = IsShutterKeyL( aKeyEvent, aType );
    TBool interruptKey = captureKey;
    if ( !appUi->IsSecondCameraEnabled() &&
            iController.UiConfigManagerPtr() && 
            iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        interruptKey = shutterKey;
        }

    if(iController.IsProcessingCapture() 
            // && appUi->CurrentCaptureMode() == ECamCaptureModeStill 
            && ECamControllerImage == iController.CurrentMode()
            && EEventKeyDown == aType
            && interruptKey
            && appUi->ShowPostCaptureView()
            && !appUi->IsBurstEnabled() 
            && !appUi->SelfTimerEnabled()) 
        {
        iController.SetCaptureKeyPressedWhileImageSaving(ETrue);
        PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL 2nd press of capturekey") )
        return EKeyWasConsumed;

        }
    // Check if the key is a shutter key and...
    if ( shutterKey && viewFinderRunning )
        {
        if ( EKeyWasConsumed == HandleShutterKeyEventL( aKeyEvent, aType ) )
            {
            PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL shutter key") );    
            return EKeyWasConsumed;
            }
        }
    else if ( captureKey             // Capture key event...
            && EEventKeyDown == aType // ...it's a key down event and
            && viewFinderRunning     // ...the viewfinder is running
            && !iController.Busy()    // CamCameraController is not busy.
            && !iController.IsDemandKeyRelease() ) // ...and no need to wait key up 
        {
        PRINT( _L("Camera <> CCamPreCaptureContainerBase::OfferKeyEventL .. [KEYS] capture key down") )
        if ( HandleCaptureKeyEventL( aKeyEvent ) == EKeyWasConsumed )
            {
            PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL capture key handled") )
            return EKeyWasConsumed;
            }
        }         
    else if ( captureKey && aType == EEventKeyUp )
        {      
        PRINT( _L("Camera <> CCamPreCaptureContainerBase::OfferKeyEventL .. [KEYS] capture key up") )
        if ( iController.SequenceCaptureInProgress()  // actively capturing or   
                || appUi->IsBurstEnabled()                  // burst is enabled (for if focusing)
        )
            {
            if ( appUi->CurrentBurstMode() == ECamImageCaptureTimeLapse )
                {
                PRINT( _L("Camera <> CCamPreCaptureContainerBase::OfferKeyEventL .. timelapse mode, update CBA") )
                // if this is a timelapse capture then set the softkeys to blank/cancel.
                // The next shutter press will stop the capture.
                TRAP_IGNORE( static_cast<CCamPreCaptureViewBase*>( &iView )->UpdateCbaL() );
                }
            else
                {
                if ( !appUi->SelfTimerEnabled() )
                    {

                    if ( !iController.AllSnapshotsReceived() && (iController.CurrentCapturedCount() >= 1) )
                        {   		
                        PRINT( _L("Camera <> CCamPreCaptureContainerBase::OfferKeyEventL .. burst mode, soft stop burst..") )
                        // burst ongoing, still receiving snapshots, end it as soon as possible.
                        iController.SetKeyUp( ETrue );
                        iController.SoftStopBurstL();
                        }
                    else 
                        {
                        // ignore keypress, all snapshot received and processing image data
                        // processing only stopped from softkey
                        }
                    }
                }

            PRINT( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL .. capture key up handled") )
            return EKeyWasConsumed;
            }
        else
            {
            UpdateCaptureButton();
            }
        iController.HandlePendingHdmiEvent();
        }
    else 
        {
        // remove Lint warning                
        }

    TKeyResponse response = CCamContainerBase::OfferKeyEventL( aKeyEvent, aType );

    PRINT1( _L("Camera <= CCamPreCaptureContainerBase::OfferKeyEventL, return %d"), response )
    return response;
    }


// ---------------------------------------------------------
// CCamPreCaptureContainerBase::HandleControllerEventL
// Handles zoom events from the Controller
// ---------------------------------------------------------
//
void 
CCamPreCaptureContainerBase
::HandleControllerEventL( TCamControllerEvent aEvent, 
                          TInt                /*aError*/ )
  {
  PRINT1(_L("Camera => CCamPreCaptureContainerBase::HandleControllerEventL(%d)"), aEvent );

  __ASSERT_DEBUG( iZoomPane, CamPanic( ECamPanicNullPointer ) );
  CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );

  switch( aEvent )
    {
    // ---------------------------------------------------
    case ECamEventSaveCancelled:
      {
      // clear the side pane after saving images dialog is dismissed
      ForceSideZoomPaneDraw();
      break;
      }
    // ---------------------------------------------------
    case ECamEventCameraChanged:
      {      
      SetResolutionIndicator();
      break;
      }

    // ---------------------------------------------------
    case ECamEventCounterUpdated:
      {
      if ( iController.UiConfigManagerPtr() &&
           iController.UiConfigManagerPtr()->IsDSAViewFinderSupported() )
          {
          if( iPaneInUse == ECamPaneProgress && appUi && appUi->IsDirectViewfinderActive() )
            {
            ActivateGc();
            CWindowGc& gc = SystemGc();
            iNaviProgressBarControl->DrawProgressBar( gc );
            DeactivateGc();
            }  
          else if ( iPaneInUse == ECamPaneCounter && iController.SequenceCaptureInProgress() )
            {
            DrawDeferred();
            }
          else
            {
            // Lint
            }
          }
      break;
      }
    // ---------------------------------------------------
    case ECamEventOperationStateChanged:
      {
	  UpdateCaptureButton();

      if( iController.IsAppUiAvailable() && iController.UiConfigManagerPtr()
          && iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        // ReCheck if we have to draw the reticule.
        if ( ECamControllerImage == iController.CurrentMode() )
            {
            iPhotoSceneUsesReticule = !iController.CurrentSceneHasForcedFocus();
            HandleOperationStateChangeEventL();
            }
        }
      break;
      }
    // ---------------------------------------------------
    case ECamEventEngineStateChanged:
        {
        UpdateCaptureButton();
        
        PRINT1( _L("Camera <> Start mode indi blinking, op:%d"), iController.CurrentOperation() );

        if ( !iController.UiConfigManagerPtr()->IsCustomCaptureButtonSupported() 
                && !iController.EngineRequestsPending()
                && iController.TargetMode() == iController.CurrentMode()
                && iController.CurrentOperation() == ECamNoOperation 
                && !iBlinkResolutionIndicator )
            {
            PRINT( _L("Camera <> mode indi blinking - starting timers") );
            if ( !iIndBlinkTimer )
                {
                iIndBlinkTimer = CPeriodic::NewL( EPriorityLess );
                }
            else 
                {
                iIndBlinkTimer->Cancel();            
                }
            
            iToggleCountdown = 2 * KNumberOfBlinks;
            iBlinkModeIndicator = ETrue;          
            iIndBlinkTimer->Start( KIndicatorBlinkDelay,
                                   KIndicatorBlinkDelay,
                                   TCallBack( IndicatorVisible, this) );
            
            iDrawIndicator = ETrue;
            }
        break;
        }
    // ---------------------------------------------------
    case ECamEventExitRequested:
      {
      iShuttingDown = ETrue;                        
      break;
      }
    // ---------------------------------------------------
    case ECamEventRecordComplete:
      {
      iResolutionIndicators[iCurrentIndicator]->CancelOverridePosition();
      break;
      }    
    // ---------------------------------------------------
    // Scene has changed
    case ECamEventSceneStateChanged:
      {
      PRINT(_L("Camera <> CCamPreCaptureContainerBase::HandleControllerEventL ECamEventSceneStateChanged"));
      if ( iController.UiConfigManagerPtr() && 
           iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        // Scene state has changed, re-check whether we should 
        // be showing the reticule
        iPhotoSceneUsesReticule = !iController.CurrentSceneHasForcedFocus();
        }
      ResetVFGridVisibility();
      appUi->APHandler()->UpdateActivePaletteL();
      break;
      }
    // ---------------------------------------------------
    case ECamEventImageData:
        {
        UpdateBurstProcessingTextL( iController.CapturedImages(), iController.CurrentCapturedCount() );
        break;
        }
    // ---------------------------------------------------  
    case ECamEventSliderClosed:
        {
        //Do nothing
        }
		break;
    // ---------------------------------------------------
    case ECamEventCaptureComplete:
        {
        if ( !appUi->IsSecondCameraEnabled() 
              && iController.CurrentMode() == ECamControllerImage )
            {
            DrawNow();
            }
        break;
        }
    // ---------------------------------------------------
    case ECamEventFaceTrackingStateChanged:
        {
        if( iController.UiConfigManagerPtr()
            && iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
            {
            // ReCheck if we have to draw the reticule.
            if ( ECamControllerImage == iController.CurrentMode() )
                {
                iPhotoSceneUsesReticule = 
                    !iController.CurrentSceneHasForcedFocus();
                }
            }
        }
    // ---------------------------------------------------        
    case ECamEventImageQualityChanged:
    case ECamEventVideoQualityChanged:
        if ( iBlinkResolutionIndicator ) 
            {
            iBlinkModeIndicator = EFalse;
        
            // Force the first blink to occur right away 
            iDrawIndicator = EFalse;
            DrawResolutionIndicator();

            // Update the resolution indicator icon to be used.
            SetResolutionIndicator();

            // Start the blinking timer
            if ( !iIndBlinkTimer )
                {
                iIndBlinkTimer = CPeriodic::NewL( EPriorityLess );
                }
            else 
                {
                iIndBlinkTimer->Cancel();
                }

            iToggleCountdown = 2 * KNumberOfBlinks;
            if ( ECamEventVideoQualityChanged == aEvent )
                {
                iToggleCountdown = 2 * KNumberOfBlinksVideo;
                }
            PRINT( _L("Camera <> start timer to blink indicator") );
            iIndBlinkTimer->Start( KIndicatorBlinkDelay,
                                   KIndicatorBlinkDelay,
                                   TCallBack( IndicatorVisible, this) );

            // Force the first blink to occur right away 
            iDrawIndicator = ETrue;
            DrawResolutionIndicator();

            // Quality level has changed through pinch gesture
            // Reinitialise the viewfinder grid 
            if ( iVfGridDrawer )
                {
                delete iVfGridDrawer;
                iVfGridDrawer = NULL;
            
                TRect rect = ViewFinderFrameRect();
                InitVfGridL( rect );
                }
            }
        else
            {
            if ( iIndBlinkTimer )
                iIndBlinkTimer->Cancel();
            }
        break; 
    // ---------------------------------------------------
    default:
      {
      // Other events => no action.
      break;
      }
    // ---------------------------------------------------
    }
  PRINT(_L("Camera <= CCamPreCaptureContainerBase::HandleControllerEventL"));
  }

// ---------------------------------------------------------
// CCamPreCaptureContainerBase::HandleForegroundEvent
// Handles change of app foreground state
// ---------------------------------------------------------
//
void CCamPreCaptureContainerBase::HandleForegroundEventL( TBool aForeground )
    {
    PRINT1( _L( "Camera => CCamPreCaptureContainerBase::HandleForegroundEventL %d" ), aForeground );            
    if ( aForeground )
        {
/*#ifndef __WINS__
        CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
        if( appUi->IsInCallSend() )
            {
            // force side pane to update
            appUi->SidePane()->SetInitialState();
            }
#endif*/      
        // Check if we are on a non-standard zoom level.  Also checks if previous 
        // view was PostCapture or settings, if so, we dont want to show the zoom
        // pane either 
        if ( iZoomPane->ZoomValue() != 0  &&
             !iZoomPane->IsResetPending() &&
             iPrevViewId != ECamViewIdStillPostCapture &&
             iPrevViewId != ECamViewIdVideoPostCapture &&
             iPrevViewId != ECamViewIdPhotoSettings &&
             iPrevViewId != ECamViewIdVideoSettings )
            {
            // If we have moved to the foreground and need to show the zoom level.
            ShowZoomPaneWithTimer();
            }            

        ResetVFGridVisibility();
        }
    else    
        { // Background
        if ( iIndBlinkTimer )
            {
            iIndBlinkTimer->Cancel();
            iDrawIndicator = ETrue;
            }
        iReceivedVfFrame = EFalse;

        // Cancel timer if we're losing focus
        iZoomTimer->Cancel();  
        
        if ( iController.IsAppUiAvailable() && !iShuttingDown )
            {
            iSidePane->MakeVisible( ETrue );
            iZoomPane->MakeVisible( EFalse, EFalse );
            
            // Will stop any ongoing zoom
            iZoomPane->HandleForegroundEvent( aForeground );
            }
        
        // Update the view ID for when we come back.
        TCamAppViewIds viewId = static_cast<TCamAppViewIds>( iView.Id().iUid );
        SetPreviousViewId( viewId );
        }
    PRINT( _L( "Camera <= CCamPreCaptureContainerBase::HandleForegroundEventL" ) );        
    }


// -----------------------------------------------------------------------------
// HandleAppEvent <<virtual>>
//
// -----------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase::HandleAppEvent( const TCamAppEvent& aEvent )
  {
  PRINT( _L("Camera => CCamPreCaptureContainerBase::HandleAppEvent") );

  switch( aEvent )
    {
    case ECamAppEventFocusGained:
      {
      // Clear non-fading flag. It may have been set when showing a note
      // in CCamAppUi::ShowErrorNoteL().

      CEikMenuBar* menuBar = iView.MenuBar();
      TBool menuOpened( EFalse );
      if ( menuBar )
          {
          menuOpened = menuBar->IsDisplayed();
          }
      
      RDrawableWindow* window = DrawableWindow();
      if ( window && !menuOpened )
          {
          window->SetNonFading( EFalse );
          }

      if( iBatteryPaneController )
        iBatteryPaneController->Pause( EFalse );
      break;
      }
    case ECamAppEventFocusLost:
      {
      if( iBatteryPaneController )
        iBatteryPaneController->Pause( ETrue );
      
	  if ( iIndBlinkTimer )
          {
          iIndBlinkTimer->Cancel();
          iDrawIndicator = ETrue;
          }
		  
      break;
      }
    default:
      {
      break;
      }
    }

  PRINT( _L("Camera <= CCamPreCaptureContainerBase::HandleAppEvent") );
  }


// ----------------------------------------------------
// CCamPreCaptureContainerBase::MakeVisible
// Handles a change to the controls visibility
// ----------------------------------------------------
//
void CCamPreCaptureContainerBase::MakeVisible( TBool aVisible )
  {
  PRINT1( _L("Camera => CCamPreCaptureContainerBase::MakeVisible, visible:%d"), aVisible );

  CCamContainerBase::MakeVisible( aVisible );

  PRINT ( _L("Camera <= CCamPreCaptureContainerBase::MakeVisible") );
  }

// ---------------------------------------------------------
// CCamPreCaptureContainerBase::Draw
// Draw control
// ---------------------------------------------------------
//
void 
CCamPreCaptureContainerBase::Draw( const TRect& /*aRect*/ ) const
   {
   PRINT( _L( "Camera => CCamPreCaptureContainerBase::Draw" ) );    
   
   CWindowGc& gc = SystemGc(); 
   CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
  
   // Is direct viewfinding active
    TBool directVf = appUi ? appUi->IsDirectViewfinderActive() : EFalse;
                 
   // Using bitmap viewfinding and a frame is available
   TBool bitmapFrame = !directVf && iReceivedVfFrame && iViewFinderBackup;

   if( iCamOrientation != ECamOrientationCamcorder && 
       iCamOrientation != ECamOrientationCamcorderLeft && 
    !( iController.UiConfigManagerPtr() && 
       iController.IsDirectScreenVFSupported( EFalse ) && 
       iCamOrientation == ECamOrientationPortrait ) )
      { 
      // Using secondary camera 
      }
   else  if( !bitmapFrame )  
       {  
       // Using primary camera and direct viewfinding is active, there's no bitmap
       // viewfinder frame available, or the frame doesn't cover whole screen
       // => Paint the background with background color / color key
                            
       gc.SetPenStyle( CGraphicsContext::ENullPen );    
    
       if( iController.UiConfigManagerPtr() && directVf )
           {    
           gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
           gc.SetBrushColor( TRgb::Color16MA( 0 ) );
           }
       else
           {
           // Use the background color
           gc.SetBrushColor( TRgb( KCamPrecaptureBgColor ) );   
           }
            
       gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
       gc.DrawRect( Rect() );          
       }
      else
      	{
      	}
   if ( bitmapFrame )
      {
      // Draw the current bitmap viewfinder frame  
      DrawFrameNow( gc, iViewFinderBackup );
      }          

   gc.Reset();        
   if( directVf 
   && !iController.IsViewFinding() )
       {
       return;
       }
   else
       {
       //Do nothing
       }
   DrawScreenFurniture( gc );
   DrawNaviControls( gc );

   PRINT( _L( "Camera <= CCamPreCaptureContainerBase::Draw" ) );        
   }






// <CAMERAAPP_CAPI_V2_MIGRATION>
// ===========================================================================
// From MCamCameraObserver

// ---------------------------------------------------------------------------
// virtual HandleCameraEventL
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase
::HandleCameraEventL( TInt              aStatus, 
                      TCamCameraEventId aEventId, 
                      TAny*             aEventData /*= NULL*/ )
  {
  switch( aEventId )
    {
    // -----------------------------------------------------
    case ECamCameraEventVfStop:
    case ECamCameraEventVfRelease:
      // old vf frame being shown when returning from background.
       if ( ECamActiveCameraSecondary ==  iController.ActiveCamera() )
        {     
        ActivateGc();
        CWindowGc& gc = SystemGc();	      
        gc.SetBrushColor( KRgbBlack );
        gc.Clear( Rect() );
        DeactivateGc();		
        }    		
      iReceivedVfFrame = EFalse;
      break;
    // -----------------------------------------------------
    case ECamCameraEventVfFrameReady:
      {
      PRINT_FRQ( _L("Camera => CCamPreCaptureContainerBase::HandleCameraEventL: case ECamCameraEventVfFrameReady") );
      if( KErrNone == aStatus )
        {
        // Mark first frame arrived.
        iReceivedVfFrame = ETrue;
    
        // If status is ok, we should always receive a bitmap.
        CFbsBitmap* frame = static_cast<CFbsBitmap*>( aEventData );    
        __ASSERT_ALWAYS( frame, CamPanic( ECamPanicNullPointer ) );
    
        // Make a duplicate copy of the vfFrame in order to show
        // it later on where needed.
        if ( iViewFinderBackup )
          {
          iViewFinderBackup->Reset();
          }
        else
          {
          TRAP_IGNORE ( iViewFinderBackup = new (ELeave) CFbsBitmap() );
          }

        if ( iViewFinderBackup )
          {
          TInt dupeError = iViewFinderBackup->Duplicate(frame->Handle());
          if ( dupeError )
            {
            iViewFinderBackup->Reset();
            delete iViewFinderBackup;
            iViewFinderBackup = NULL;
            }
          }

        // Do a redraw.
        ActivateGc();
        CWindowGc& gc = SystemGc();

        PRINT_FRQ4( _L("ECamCameraEventVfFrameReady - rect is %d,%d - %d,%d"), Rect().iTl.iX, Rect().iTl.iY, Rect().iBr.iX, Rect().iBr.iY ) 

      	if( ECamCapturing == iController.CurrentVideoOperation() )
      	  {   
          // -----------------------------------------
          // Draw all content directly to system gc
      	  PRINT_FRQ( _L("Camera <> CCamPreCaptureContainerBase::ShowViewFinderFrame without offscreen bitmap") )   
      	  RWindow window = Window();
      	  window.Invalidate( Rect() );
      	  window.BeginRedraw( Rect() );    
      		DrawToContext(gc, frame);
      		window.EndRedraw();
          // -----------------------------------------
      	  }
        else
          { 
          if( iBitmapGc )
            {
            // -----------------------------------------
            // Draw all content to the offscreen bitmap
            DrawToContext( *iBitmapGc, frame );
      
            // -----------------------------------------
            // Draw the offscreen bitmap to screen
            RWindow window = Window();
            window.Invalidate( Rect() );
            window.BeginRedraw( Rect() );
            gc.BitBlt( TPoint( 0,0 ), iOffScreenBitmap );
            window.EndRedraw();
            // -----------------------------------------
            }
          else
            {
            PRINT_FRQ( _L("Camera <> CCamPreCaptureContainerBase: iBitmapGc == NULL"));
            //in DSA case draw snapshot already here if available
            const CFbsBitmap* snapshot = iController.SnapshotImage();
            if (snapshot)
              {
              TSize snapshotSize = snapshot->SizeInPixels();
              TRect bmCropRect( snapshotSize );
        
              // Get the required snapshot layout rect
              TRect vfRect( ViewFinderFrameRect() ); 

              RWindow window = Window();
              window.Invalidate( Rect() );
              window.BeginRedraw( Rect() ); 
              gc.BitBlt( vfRect.iTl, snapshot, bmCropRect );
              
              TBool lateOperation = ( ECamCapturing == iController.CurrentOperation() 
                                     || ECamCompleting == iController.CurrentOperation() );

              if( iProcessingText &&
                  iController.ActiveCamera() != ECamActiveCameraSecondary && 
                  lateOperation )
                {  
                gc.SetBrushColor( KRgbWhite );
                gc.SetBrushStyle( CGraphicsContext::ENullBrush );
                iProcessingTextLayout.DrawText( gc, *iProcessingText, ETrue, KRgbBlack ); 
                }
              
              if( iBatteryPaneController )
                {
                iBatteryPaneController->Draw( gc );
                }
              window.EndRedraw();
              }   
            }
          }
        DeactivateGc();
        PRINT_FRQ( _L("Camera <= CCamPreCaptureContainerBase::HandleCameraEventL") );
        }
      break;
      }
    // -----------------------------------------------------
    case ECamCameraEventSsReady:
      {
      PRINT( _L( "Camera => CCamPreCaptureContainerBase::HandleCameraEventL: case ECamCameraEventSsReady" ) );  

      CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi());

	  if( KErrNone == aStatus
       && ( ECamImageCaptureTimeLapse == appUi->CurrentBurstMode() ||
            ECamImageCaptureBurst     == appUi->CurrentBurstMode() ) )
        {
        // In timelapse mode, convert snapshot event to vf frame event
        PRINT( _L( "In timelapse mode, converting event to ECamCameraEventVfFrameReady" ) );
        HandleCameraEventL( aStatus, ECamCameraEventVfFrameReady, aEventData );  
        }
      else if( KErrNone == aStatus && 
               ECamControllerImage == iController.CurrentMode() &&
               appUi->ShowPostCaptureView() )
        {
        // use mask mode to draw battery pane when showing snapshot
        // and hide side pane altogether ( as there is no feasible way to draw
        // it properly on top of the snapshot because of the current CCamIndicator implementation)
        iSidePane->MakeVisible( EFalse );
        // also hide zoom pane at the same time
        iZoomPane->MakeVisible( EFalse, EFalse );
        
        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMPRECAPTURECONTAINERBASE_HANDLECAMERAEVENTL, "e_CAM_PRI_SHOT_TO_SNAPSHOT 0" ); //CCORAPP_PRI_SHOT_TO_SNAPSHOT_END
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMPRECAPTURECONTAINERBASE_HANDLECAMERAEVENTL, "e_CAM_APP_SNAPSHOT_DRAW 1" );   //CCORAPP_SNAPSHOT_DRAW_START
        HandleCameraEventL( aStatus, ECamCameraEventVfFrameReady, aEventData );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMPRECAPTURECONTAINERBASE_HANDLECAMERAEVENTL, "e_CAM_APP_SNAPSHOT_DRAW 0" );   //CCORAPP_SNAPSHOT_DRAW_END
        }
        
      PRINT( _L( "Camera <= CCamPreCaptureContainerBase::HandleCameraEventL" ) );  
      break;
      }
    // -----------------------------------------------------
    default:
      {
      break;
      }
    // -----------------------------------------------------
    }                 
  }


// ---------------------------------------------------------
// CCamPreCaptureContainerBase::DrawToContext
// Draws Screen content to graphics context
// ---------------------------------------------------------
//  
void 
CCamPreCaptureContainerBase::DrawToContext(       CBitmapContext& aGc, 
                                            const CFbsBitmap*     aFrame )
  {
  	PRINT( _L( "Camera => CCamPreCaptureContainerBase::DrawToContext" ) );  
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( !appUi->IsWaitDialog() || !appUi->IsQwerty2ndCamera() )
        {
        aGc.SetBrushColor( TRgb( KCamPrecaptureBgColor ) );
   	    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
   	    aGc.SetPenColor( TRgb( KCamPrecaptureBgColor ) );
   	    aGc.SetPenStyle( CGraphicsContext::ESolidPen );
   	    aGc.DrawRect( Rect() );
   	    

        DrawFrameNow( aGc, aFrame );
        aGc.SetBrushStyle(CGraphicsContext::ENullBrush );
        DrawScreenFurniture( aGc );       
        DrawActivePalette( aGc );       
        DrawNaviControls( aGc );
        }
  PRINT( _L( "Camera <= CCamPreCaptureContainerBase::DrawToContext" ) );  
  }

// ===========================================================================
// From MCamCameraObserver

// ---------------------------------------------------------------------------
// IntSettingChangedL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase::IntSettingChangedL( 
    TCamSettingItemIds aSettingItem, 
    TInt               aSettingValue )
  {
  switch( aSettingItem )
    {
    // -----------------------------------------------------
    case ECamSettingItemViewfinderGrid:
      {
      PRINT( _L("Camera <> CCamPreCaptureContainerBase::IntSettingChangedL: ECamSettingItemViewfinderGrid") );
      CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );

      if( iVfGridDrawer )
        {
        iVfGridDrawer->SetVisible( ECamViewfinderGridOn == aSettingValue );
        if( appUi && appUi->APHandler() )
          {
          // Disable the redrawing requested by AP as we do full redraw right after.
          // Avoids some unwanted blinking in AP area.
          iDisableRedraws = ETrue;
          // Not critical if fails.
          // Tooltip and icon shown wrong then until next update.
          // Cannot leave here, as redraws need to be enabled again.
          TRAP_IGNORE( appUi->APHandler()->UpdateActivePaletteL() );
          iDisableRedraws = EFalse;
          }
        DrawNow();
        }
      break;
      }
    // -----------------------------------------------------
    case ECamSettingItemFaceTracking:
      {
      PRINT( _L("Camera <> CCamPreCaptureContainerBase::IntSettingChangedL: ECamSettingItemFaceTracking") );
      CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
      if( appUi && appUi->APHandler() )
        {
        // Disable the redrawing requested by AP as we do full redraw right after.
        // Avoids some unwanted blinking in AP area.
        iDisableRedraws = ETrue;
        // Not critical if fails.
        // Tooltip and icon shown wrong then until next update.
        // Cannot leave here, as redraws need to be enabled again.
        TRAP_IGNORE( appUi->APHandler()->UpdateActivePaletteL() );
        iDisableRedraws = EFalse;
        }
      DrawNow();
      break;
      }
    // -----------------------------------------------------
    default: 
      break;
    // -----------------------------------------------------
    }
  }

// ---------------------------------------------------------------------------
// TextSettingChangedL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase
::TextSettingChangedL(       TCamSettingItemIds /*aSettingItem */, 
                       const TDesC&             /*aSettingValue*/ )
  {
  /*
  switch( aSettingItem )
    {
    default: 
      break;
    }
  */
  }


// ===========================================================================
// </CAMERAAPP_CAPI_V2_MIGRATION>




// ---------------------------------------------------------
// CCamPreCaptureContainerBase::DrawFrameNow
// Blit the bitmap
// ---------------------------------------------------------
//
void
CCamPreCaptureContainerBase
::DrawFrameNow(       CBitmapContext& aGc, 
                const CFbsBitmap*     aFrame ) const
    {
//    PRINT( _L("Camera => CCamPreCaptureContainerBase::DrawFrameNow"))  
    TRect frameRect = ViewFinderFrameRect();
    TSize fsize     = aFrame->SizeInPixels();  

    // If the viewfinder bitmap is LARGER than the masked bitmap
    // and we should be masked
    if ( fsize.iWidth  > frameRect.Width() 
      || fsize.iHeight > frameRect.Height() )
        {
        // Work out the rectangle of the viewfinder bitmap to draw
        TInt borderX = ( fsize.iWidth - frameRect.Width() ) / 2;
        TInt borderY = ( fsize.iHeight - frameRect.Height() ) / 2;
        TRect bmCropRect( borderX, borderY, fsize.iWidth - borderX, fsize.iHeight - borderY );

        // Draw the "crop rect" area of the viewfinder in the cropped frame
        aGc.BitBlt( frameRect.iTl, aFrame, bmCropRect );
        }
    else if( fsize.iWidth  > Rect().Width() 
          || fsize.iHeight > Rect().Height() )
      {
      // Work out the rectangle of the viewfinder bitmap to draw
        TInt borderX = ( fsize.iWidth - Rect().Width() ) / 2;
        TInt borderY = ( fsize.iHeight - Rect().Height() ) / 2;
        TRect bmCropRect( borderX, borderY, fsize.iWidth - borderX, fsize.iHeight - borderY );

        // Draw the "crop rect" area of the viewfinder in the cropped frame
        aGc.BitBlt( frameRect.iTl, aFrame, bmCropRect );
      }
     else
      {   
        // Just draw the viewfinder frame in the frame rectangle
        aGc.BitBlt( frameRect.iTl, aFrame );    
      }          
//    PRINT( _L("Camera <= CCamPreCaptureContainerBase::DrawFrameNow"))  
    }




// ---------------------------------------------------------
// CCamPreCaptureContainerBase::DrawReticule
// Draw the reticule
// ---------------------------------------------------------
//
void CCamPreCaptureContainerBase::DrawReticule( CBitmapContext& aGc ) const
  {
  PRINT_FRQ( _L("Camera => CCamPreCaptureContainerBase::DrawReticule"))  
  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
    {
    ReDrawReticule( iReticuleRect, aGc );
    }
  PRINT_FRQ( _L("Camera <= CCamPreCaptureContainerBase::DrawReticule"))               
  }

// ---------------------------------------------------------
// ReDrawReticule
//
// Handle partial redraw of reticule.
// NOTE: Could be better optimized for partial redraws.
// ---------------------------------------------------------
//
void 
CCamPreCaptureContainerBase
::ReDrawReticule( const TRect&          aRect,
                        CBitmapContext& aGc   ) const
  {
  if( iShowReticule 
    && iPhotoSceneUsesReticule 
    && iReticuleRect.Intersects( aRect )
    && iController.IsViewFinding() 
    && iController.UiConfigManagerPtr()
    && iController.UiConfigManagerPtr()->IsAutoFocusSupported()
    && !iController.UiConfigManagerPtr()->IsFaceTrackingSupported()
    )
    {
    aGc.SetClippingRect( aRect );

    // Overlay reticule (though which bitmap used depends on focus state)
    switch ( iFocusState )
      {
      case EFocusStateFocusFailed:
          DrawAf( aGc, iAfErrIcon );
        break;
      case EFocusStateFocusAchieved:                
          DrawAf( aGc, iAfFocusIcon );
        break;
      }                

    aGc.CancelClippingRect();
    }
  else
    {
    // Nothing needs to be done.
    }
  }



// ---------------------------------------------------------
// CCamPreCaptureContainerBase::ForceSideZoomPaneDraw
// Force the side-pane or zoom-pane to redraw.
// ---------------------------------------------------------
//
void CCamPreCaptureContainerBase::ForceSideZoomPaneDraw() const
    {
    if( iSidePane->IsVisible() )
        {
        ActivateGc();
        iSidePane->Draw( SystemGc() );       
        DeactivateGc();
        }
    else
        {
        ActivateGc();
        iZoomPane->Draw( SystemGc() );        
        DeactivateGc();
        }
    
    }

// ---------------------------------------------------------
// CCamPreCaptureContainerBase::CreateResolutionIndicatorL
// Create the resolution indicator
// ---------------------------------------------------------
//
void CCamPreCaptureContainerBase::CreateResolutionIndicatorL()
    {
  RArray<TInt> icons;
  CleanupClosePushL( icons );

  iController.Configuration()->GetPsiIntArrayL( ResolutionIndicatorIconPsiKey(), icons ); 
  TInt count = icons.Count();

    // Get the icon rect from the derived class
    TRect iconRect = ResolutionIndicatorRect();
    
    CCamIndicator* indicator;
  for( TInt i = 0; i < count; i+=2 )
    {
    indicator = CCamIndicator::NewL( iconRect );
    CleanupStack::PushL( indicator );
    indicator->AddIconL( icons[i], icons[i+1] ); // Bitmap & mask.
    indicator->SetRect( iconRect );
    User::LeaveIfError( iResolutionIndicators.Append( indicator ) );
    CleanupStack::Pop( indicator );
    }
  
  CleanupStack::PopAndDestroy( &icons );

    // initialise the indicator
    SetResolutionIndicator();
    }

// ----------------------------------------------------
// CCamPreCaptureContainerBase::BurstModeActiveL
// Notification that the burst mode has been activated/deactivated
// ----------------------------------------------------
//
void CCamPreCaptureContainerBase::BurstModeActiveL( TBool /*aActive*/, TBool /*aStillModeActive*/ )
    {
    SetResolutionIndicator();
    if ( !iController.IsTouchScreenSupported() )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
        if( appUi && appUi->APHandler() )
            {
            // Disable the redrawing requested by AP as we do full redraw right after.
            // Avoids some unwanted blinking in AP area.
            iDisableRedraws = ETrue;
            // Not critical if fails.
            // Tooltip and icon shown wrong then until next update.
            // Cannot leave here, as redraws need to be enabled again.
            TRAP_IGNORE( appUi->APHandler()->UpdateActivePaletteL() );
            iDisableRedraws = EFalse;
            }
        DrawNow();
        }
    }
    
// ----------------------------------------------------
// CCamPreCaptureContainerBase::IncreaseFlashSettingL
// Move up through the flash settings
// ----------------------------------------------------
//
void CCamPreCaptureContainerBase::IncreaseFlashSettingL()  
    {
    // intentionally doing nothing
    }  
 
// ----------------------------------------------------
// CCamPreCaptureContainerBase::DecreaseFlashSettingL
// Move down through the flash settings
// ----------------------------------------------------
//
void CCamPreCaptureContainerBase::DecreaseFlashSettingL()   
    {
    // intentionally doing nothing
    }  
    
// ----------------------------------------------------
// CCamPreCaptureContainerBase::HandleLeftRightNaviKeyL
// Initiate behaviour in response to a navi key left/right
// arrow press 
// ----------------------------------------------------
//   
TKeyResponse CCamPreCaptureContainerBase::HandleLeftRightNaviKeyL( 
                                                    const TKeyEvent& aKeyEvent,
        
                                                    TEventCode /*aType*/ )                                                                     
    {
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
    // Find out which behaviour is mapped to the key
    TCamPsiKey queryValue = ECamPsiLeftNaviKey;
    if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
        {
        queryValue = ECamPsiRightNaviKey;
        }
    else if ( appUi->IsSecondCameraEnabled() )
        {
        queryValue = ECamPsiLeftNaviKeyCam2;
        }
    else
        {
        // remove lint warning
        }
        
    TInt keyFunction;
    User::LeaveIfError( CamUtility::GetPsiInt( queryValue, keyFunction ) );    

    // Handle the key
    switch ( keyFunction ) 
        {
        case ECamNaviKeyIncreaseFlashSetting:
            {
            IncreaseFlashSettingL();
            return EKeyWasConsumed;
            }
        // No break as unreachable 
        case ECamNaviKeyDecreaseFlashSetting:
            {
            DecreaseFlashSettingL();
            return EKeyWasConsumed;
            }
        // No break as unreachable 

        default:
            break;
        }
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CCamPreCaptureContainerBase::CountComponentControls
// Return number of child controls owned by this control
// ----------------------------------------------------
//
TInt CCamPreCaptureContainerBase::CountComponentControls() const
  {
  TInt count = CCamContainerBase::CountComponentControls();
    if(iActivePalette && iActivePalette->CoeControl()->IsVisible())
        {
        count++; //Active Palette
        }
    
    if( iCaptureButtonContainer ) // Capture button container
        {
        count++;
        }
    PRINT1( _L("Camera <> CCamPreCaptureContainerBase::CountComponentControls %d"), count );
    return count;
    }

// ----------------------------------------------------
// CCamPreCaptureContainerBase::ComponentControl
// Return requested control
// ----------------------------------------------------
//
CCoeControl* CCamPreCaptureContainerBase::ComponentControl( TInt aIndex ) const
  {
  PRINT1( _L("Camera <> CCamPreCaptureContainerBase::ComponentControl index:%d"), aIndex );
  CCoeControl* con = CCamContainerBase::ComponentControl( aIndex );
  if( con )
      return con;

  switch ( aIndex )
      {
     /*
      case ECamTimeLapseControl:
            {
            
            if ( iTimeLapseSlider && static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->TimeLapseEnabled() )
                {
                con = iTimeLapseSlider;
                }
            else
            
                {
                con = iNaviCounterControl;
                }
            break;
            }
      */
              
      case ECamActivePalette:
          {
          if(iActivePalette && iActivePalette->CoeControl()->IsVisible())         
              {
              con = iActivePalette->CoeControl();
              }
          else if( iCaptureButtonContainer )
              {
              PRINT( _L("Camera <> capture button container A") );
              con = iCaptureButtonContainer;
              }
          else
              {
              
              }
          break;
          }     
      case ECamCaptureButton:
          {
          if( iCaptureButtonContainer )
              {
              PRINT( _L("Camera <> capture button container A") );
              con = iCaptureButtonContainer;
              }
          }
        default:
            break;
      }         
    return con; 
  }
  
 
// ----------------------------------------------------
// CCamPreCaptureContainerBase::HandleCommand
// Handle CBA key and options menu events for SetupPane
// ----------------------------------------------------
//

void CCamPreCaptureContainerBase::HandleCommand( TInt /* aCommand */ )
    {

    }    


// ---------------------------------------------------------
// CCamPreCaptureContainerBase::GetAutoFocusDelay
// 
// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::DrawScreenFurniture
// Draw reticule, side or zoom pane and softkeys 
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase
::DrawScreenFurniture( CBitmapContext& aGc ) const 
  {
  PRINT_FRQ( _L("Camera => CCamPreCaptureContainerBase::DrawScreenFurniture" ))
  CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() ); 
  

  TCamOrientation orientation = appUi->CamOrientation();   
  if ( iController.ActiveCamera() == ECamActiveCameraPrimary &&
       ( orientation == ECamOrientationCamcorder || orientation == ECamOrientationCamcorderLeft ) )
    {
    // -------------------------------
    // Viewfinder grid
    PRINT_FRQ( _L("Camera <> CCamPreCaptureContainerBase: drawing VF grid" ) );
    DrawVfGrid( aGc );
    // -------------------------------
    // Reticule 
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        PRINT_FRQ( _L("Camera <> CCamPreCaptureContainerBase: drawing reticule" ) );
        DrawReticule( aGc );
        }
    }
  else
    {
    PRINT_FRQ( _L("Camera <> CCamPreCaptureContainerBase: No reticule / grid (as secondary/none camera)") );
    }     

  // -------------------------------------------------------
  // Zoom pane
//  PRINT( _L("Camera <> CCamPreCaptureContainerBase: condition zoom pane" ))
  if ( iZoomPane->IsVisible() && 
       !iController.SequenceCaptureInProgress() && 
       iController.ActiveCamera() == ECamActiveCameraPrimary )
    {
    PRINT_FRQ( _L("Camera <> CCamPreCaptureContainerBase: drawing zoom pane" ))
    iZoomPane->Draw( aGc );
    }       


  // -------------------------------------------------------
  // Side pane indicators
  if ( iSidePane->IsVisible() )
    {
//    PRINT( _L("Camera <> CCamPreCaptureContainerBase: drawing side pane" ))
    iSidePane->Draw( aGc );
    }

//  PRINT( _L("Camera <= CCamPreCaptureContainerBase::DrawScreenFurniture" ))       
  }

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::DrawNaviControls
// Draws the navi pane items 
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase::DrawNaviControls( CBitmapContext& aGc ) const 
  {
//  PRINT( _L("Camera => CCamPreCaptureContainerBase::DrawNaviControls" ))  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );     

  if( appUi )
    {
    TCamCameraMode       mode      = iController.CurrentMode();
    TCamImageCaptureMode imageMode = iController.CurrentImageModeSetup();
    TBool seqCapturing   = iController.SequenceCaptureInProgress();
    TBool precaptureUiOn = appUi->DrawPreCaptureCourtesyUI();
    TBool paneVisible    = EFalse;

    if( iProcessingText &&  //iProcessingBg && 
        iController.ActiveCamera() != ECamActiveCameraSecondary &&
        // ECamCaptureModeStill == appUi->CurrentCaptureMode() &&
        ECamControllerImage == iController.CurrentMode() &&
        // ECamOperationCaptureInProgress == iController.OperationState() 
        ECamImageCaptureNone == appUi->CurrentBurstMode() && // No text for sequence
        ( ECamCapturing == iController.CurrentOperation() ||
          ECamCompleting == iController.CurrentOperation() ) 
     )
      { 
      // When iProcessingText exists (and we are in correct state), the processing
      // image text needs to be drawn.
      PRINT( _L("Camera <> CCamPreCaptureContainerBase: draw processing text.." ) );
      //iProcessingBg->Draw( aGc, Rect() );
      aGc.SetBrushColor( KRgbWhite );
      aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
      iProcessingTextLayout.DrawText( aGc, *iProcessingText, ETrue, KRgbBlack ); 
      }
    else if( iBurstProcessingText &&         
             iController.ActiveCamera() != ECamActiveCameraSecondary &&
             ECamControllerImage == iController.CurrentMode() &&
           ( ECamImageCaptureBurst == appUi->CurrentBurstMode() &&
           ( ECamCapturing == iController.CurrentOperation() || 
             ECamCompleting == iController.CurrentOperation() ) && 
             iController.AllSnapshotsReceived() ) )
      {
      PRINT( _L("Camera <> CCamPreCaptureContainerBase: draw burst processing text.." ) );
      aGc.SetBrushColor( KRgbWhite );
      aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
      // localizing numbers in burst processing string
      TPtr pointer = iBurstProcessingText->Des();
      AknTextUtils::LanguageSpecificNumberConversion( pointer );
      iBurstProcessingTextLayout.DrawText( aGc, *iBurstProcessingText, ETrue, KRgbBlack ); 
      }
    else if( ECamPaneCounter == iPaneInUse )
      {
      TBool showCounter  = precaptureUiOn || seqCapturing;    
      TBool focusStateOk = ETrue;
      if ( iController.UiConfigManagerPtr() && 
              iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
          {

          focusStateOk = 
          ( EFocusStateNormal == iFocusState && 
            !iController.SequenceCaptureInProgress() ) ||
          ( imageMode == ECamImageCaptureBurst &&
            ( iController.CurrentOperation() == ECamCapturing  || 
            ( iController.CurrentOperation() == ECamCompleting 
              && !iController.AllSnapshotsReceived() ) ) );

          }
      
      if( focusStateOk || 
          ( showCounter && appUi->IsSecondCameraEnabled() ) )
          {
          paneVisible = ETrue;
          iNaviCounterControl->DrawNaviCtr( aGc );
          }
      }
    else if( ECamPaneProgress == iPaneInUse )
      {
      paneVisible = ETrue;
      iNaviProgressBarControl->DrawProgressBar( aGc );
      iNaviCounterControl->DrawNaviCtr( aGc );
      }
    else
      {      
      // Navi pane is not shown
      paneVisible = EFalse;
      }

    // Do not show if saving / focusing
    TCamCaptureOperation operation      = iController.CurrentOperation();
    TCamCaptureOperation videoOperation = iController.CurrentVideoOperation();

    TBool operationOk = ( ECamNoOperation == operation
                       || ECamCompleting  != videoOperation );

    // In video mode, icon would show without panes when stopping
    // if viev state is not checked.
    if(  appUi->CurrentViewState() == ECamViewStatePreCapture
      && precaptureUiOn
      && paneVisible
      &&  operationOk 
      && !seqCapturing
      )
      {
      // Draw the resolution indicator
      if ( appUi->ActiveCamera() == ECamActiveCameraPrimary  
           && ( AknLayoutUtils::PenEnabled() 
           || videoOperation == ECamNoOperation ) )
          {
          iResolutionIndicators[iCurrentIndicator]->Draw( aGc );
         }

      DrawAdditionalIcons( aGc );
    
      if ( iController.UiConfigManagerPtr() && 
           iController.UiConfigManagerPtr()->IsLocationSupported() )
          {
          if ( iLocationIndicatorVisible ) 
              {
              // Draw the location indicator
              iLocationIconController->Draw( aGc );
              }
          }
      }
      
  if( iBatteryPaneController )
    {
      iBatteryPaneController->Draw( aGc );
    }

    }

//  PRINT( _L("Camera <= CCamPreCaptureContainerBase::DrawNaviControls" ))          
  }

  
// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::DrawActivePalette
// Draws the active palette
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase::DrawActivePalette() const 
  {    
  
  PRINT( _L("Camera => CCamPreCaptureContainerBase::DrawActivePalette") );
  if ( !iController.IsTouchScreenSupported() )
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
      
      // draw the active palette if needed
      if( iActivePalette && appUi->DrawPreCaptureCourtesyUI() )
        {
        PRINT( _L("Camera <> drawing active palette") );
        iActivePalette->RenderActivePalette( Rect() );
        }
      }
   
  PRINT( _L("Camera <= CCamPreCaptureContainerBase::DrawActivePalette") );
  }

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::DrawActivePalette
// Draws the active palette
// ---------------------------------------------------------------------------
//
void 
CCamPreCaptureContainerBase::DrawActivePalette( CBitmapContext& aGc ) const 
  {    
  PRINT_FRQ( _L("Camera => CCamPreCaptureContainerBase::DrawActivePalette( aGc )") );
  if ( !iController.IsTouchScreenSupported() )
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
      
      // draw the active palette if needed
      if( iActivePalette && appUi->DrawPreCaptureCourtesyUI() )
        {
        PRINT_FRQ( _L("Camera <> drawing active palette..") );
        iActivePalette->SetGc( &aGc );
        iActivePalette->RenderActivePalette( Rect() );
        iActivePalette->SetGc();
        }
      }
  PRINT_FRQ( _L("Camera <= CCamPreCaptureContainerBase::DrawActivePalette") );
  }


// -----------------------------------------------------------------------------
// virtual CCamPreCaptureContainerBase::DrawVfGrid
// -----------------------------------------------------------------------------
// 
void 
CCamPreCaptureContainerBase::DrawVfGrid( CBitmapContext& aGc ) const 
  {
  PRINT_FRQ( _L("Camera => CCamPreCaptureContainerBase::DrawVfGrid") );
  // Draws nothing if not set visible.
  if( iVfGridDrawer )
    {
    iVfGridDrawer->Draw( iRect, aGc );
    }
  PRINT_FRQ( _L("Camera <= CCamPreCaptureContainerBase::DrawVfGrid") );
  }

// -----------------------------------------------------------------------------
// virtual CCamPreCaptureContainerBase::ReDrawVfGrid
// -----------------------------------------------------------------------------
// 
void 
CCamPreCaptureContainerBase::ReDrawVfGrid( const TRect&          aRect, 
                                                 CBitmapContext& aGc   ) const 
  {
  PRINT_FRQ( _L("Camera => CCamPreCaptureContainerBase::ReDrawVfGrid") );
  // Draws nothing if not set visible.
  if( iVfGridDrawer 
   && iVfGridRect.Intersects( aRect ) 
    )
    {
    iVfGridDrawer->Draw( aRect, aGc );
    }
  PRINT_FRQ( _L("Camera <= CCamPreCaptureContainerBase::ReDrawVfGrid") );
  }

// -----------------------------------------------------------------------------
// virtual GetVFGridVisibilitySetting
//
// -----------------------------------------------------------------------------
//
TBool 
CCamPreCaptureContainerBase::GetVFGridVisibilitySetting() const 
  {
  TInt value = iController.IntegerSettingValue( ECamSettingItemViewfinderGrid );
  return (ECamViewfinderGridOn == value);
  }


// -----------------------------------------------------------------------------
// virtual ResetVFGridVisibility
//
// -----------------------------------------------------------------------------
//
void
CCamPreCaptureContainerBase::ResetVFGridVisibility()
  {
  if( iVfGridDrawer )
    {
    iVfGridDrawer->SetVisible( GetVFGridVisibilitySetting() );
    }
  }


// -----------------------------------------------------------------------------
// CCamPreCaptureContainerBase::Redraw
//
// Redraw only part of screen. 
// Active Palette animations need this to update its background.
// -----------------------------------------------------------------------------
void CCamPreCaptureContainerBase::Redraw( const TRect& aArea )
  {
  PRINT( _L("Camera => CCamPreCaptureContainerBase::Redraw") );  

  if( iDisableRedraws )
    {
    PRINT( _L("Camera <= CCamPreCaptureContainerBase: redraw skipped") );  
    return;
    }

  CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );

  if(  appUi &&  appUi->IsDirectViewfinderActive() )
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    gc.SetBrushColor( TRgb::Color16MA( 0 ) );
    gc.DrawRect( aArea );

    // Viewfinder grid and reticule overlap with tooltips
    // or Active Palette moving in/out the screen.
    ReDrawVfGrid( aArea, gc );

    if( iController.ActiveCamera() != ECamActiveCameraSecondary
        && iController.UiConfigManagerPtr()
        && iController.UiConfigManagerPtr()->IsAutoFocusSupported()
        && !iTimeLapseVisible )
      {
      ReDrawReticule( aArea, gc );
      }
    }
  else
    {
    if ( iBitmapGc )
      {
      if ( iReceivedVfFrame && iViewFinderBackup )
      	{
      	iBitmapGc->BitBlt( aArea.iTl, iViewFinderBackup, aArea );
      	}
      else
        {
        iBitmapGc->SetBrushColor( TRgb( KCamPrecaptureBgColor ) );
        iBitmapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
        iBitmapGc->DrawRect( aArea );
        iBitmapGc->SetBrushStyle( CGraphicsContext::ENullBrush );
        }
      ReDrawVfGrid( aArea, *iBitmapGc );

      if ( iController.UiConfigManagerPtr() && 
           iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        ReDrawReticule( aArea, *iBitmapGc );
        }
      }
    }
  PRINT( _L("Camera <= CCamPreCaptureContainerBase::Redraw") );  
  }



// -----------------------------------------------------------------------------
// HandleOperationStateChangeEventL
//
// Helper method to handle case ECamEventOperationStateChanged in
// HandleControllerEventL.
// -----------------------------------------------------------------------------
//
void
CCamPreCaptureContainerBase::HandleOperationStateChangeEventL()
  {
  PRINT( _L("Camera => CCamPreCaptureContainerBase::HandleFocusStateChangeEventL") );

    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
          if( iPhotoSceneUsesReticule )
            {
            TBool ftSupported = iController.UiConfigManagerPtr()->IsFaceTrackingSupported();    
            switch ( iController.CurrentOperation() )
              {
              // ---------------------------------------------------
              case ECamFocusing:
                {
                PRINT(_L("Camera <> CCamPreCaptureContainerBase: FOCUSING"));
                // set focusing icon - flashing
                iFocusState      = EFocusStateFocusing;
                if ( !ftSupported )
                  {
                  iReticuleFlashOn = ETrue;
          
                // Start reticule flashing timer
                if ( iReticuleTimer->IsActive() )
                  {
                  iReticuleTimer->Cancel();
                  }
                iReticuleTimer->Start( KReticuleFlashTimeout, 
                                       KReticuleFlashTimeout,
                                       TCallBack(ReticuleTimerCallback , this) );
                  }
                // Hide the navi counter                                    
                iNaviCounterControl->MakeVisible( EFalse );
          
                DrawDeferred();
                break;
                }
              // ---------------------------------------------------
              case ECamFocused:
                {
                PRINT(_L("Camera <> CCamPreCaptureContainerBase: FOCUSED"));
                // set focus achieved icon
                iFocusState = EFocusStateFocusAchieved;
                if ( !ftSupported )
                  {
                  iReticuleTimer->Cancel();
                  iReticuleFlashOn = ETrue;
                  }
                
                OstTrace0( CAMERAAPP_PERFORMANCE, CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_APP_OVERLAY_UPD 0" );    //CCORAPP_OVERLAY_UPD_END
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_APP_AF 0" );    //CCORAPP_AF_END
                
                DrawDeferred();
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP6_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_PRI_AF_LOCK 0" );
                break;
                }
              // ---------------------------------------------------
              case ECamFocusFailed:
                {
                PRINT(_L("Camera <> CCamPreCaptureContainerBase: FOCUS FAILED"));
                // set focus failed icon
                iFocusState = EFocusStateFocusFailed;
                if ( !ftSupported )
                  {
                  iReticuleTimer->Cancel();
                  iReticuleFlashOn = ETrue;
                  }
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_APP_OVERLAY_UPD 0" );
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_APP_AF 0" );

                DrawDeferred();
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP7_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_PRI_AF_LOCK 0" );
                break;
                }
                // ---------------------------------------------------
              case ECamCapturing:
                  {
                  PRINT(_L("Camera <> CCamPreCaptureContainerBase: Capturing"));
                  CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );     
                  if( appUi )
                      {
                      // If in burst mode show the navi counter again
                      if( ECamImageCaptureBurst == appUi->CurrentBurstMode() )
                          {
                          iNaviCounterControl->MakeVisible( ETrue );
                          DrawDeferred();
                          }
                      }
                  break;
                  }  
                  // ---------------------------------------------------
              case ECamNoOperation:
                  {
                  PRINT( _L("Camera <> CCamPreCaptureContainerBase:ECamNone"));
                  // Show the navi counter again
                  if ( iPaneInUse == ECamPaneCounter 
                          && iController.IsAppUiAvailable()
                  )
                      {
                      iNaviCounterControl->MakeVisible( ETrue );
                      }
                  }
                  // lint -fallthrough
                  // Captured and saving or..
              case ECamCompleting: 
                  {
                  PRINT1( _L("Camera <> CCamPreCaptureContainerBase: NOT FOCUSED ANY MORE, iFocusState=%d"), iFocusState );
                  if ( iFocusState == EFocusStateFocusing )
                    {
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_APP_OVERLAY_UPD 0" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_APP_AF 0" );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP8_CCAMPRECAPTURECONTAINERBASE_HANDLEOPERATIONSTATECHANGEEVENTL, "e_CAM_PRI_AF_LOCK 0" );
                    }
                    
                  // clear focus indicator
                  iFocusState = EFocusStateNormal;
                  if ( !ftSupported )
                    {
                    iReticuleTimer->Cancel();
                    iReticuleFlashOn = ETrue;
                    }
                  iSidePane->MakeVisible(ETrue);
                  DrawDeferred();
                  break;
                  }
                  // ---------------------------------------------------
              default:
                  break;
              } // switch
            } // if
        }

  PRINT( _L("Camera <= CCamPreCaptureContainerBase::HandleFocusStateChangeEventL") );
  }



// -----------------------------------------------------------------------------
// SetupActivePaletteL
//
// (Re)Initializes Active Palette. This includes:
//   1) Creating Active Palette if needed
//   2) Clearing Active Palette existing items if needed 
//      - 1+2 done in CCamContainerBase::CreateActivePaletteL()
//   3) Setting Active Palette items from resource
//      - resource is selected in GetAPResourceId which is
//        implemented in inheriting classes.
//   4) Registering the view for AP
//   5) Setting the focus to the default AP item
// -----------------------------------------------------------------------------
// 
void
CCamPreCaptureContainerBase::SetupActivePaletteL( CCamViewBase* aView )
  {
  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMPRECAPTURECONTAINERBASE_SETUPACTIVEPALETTEL, "e_CAM_APP_ACTIVEPALETTE_INIT 1" );
  CCamAppUi* appUi =  static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
  
  if ( !iController.IsTouchScreenSupported()
         && ( !appUi->IsSecondCameraEnabled()
         || ( !appUi->IsEmbedded() && appUi->IsSecondCameraEnabled() ) ) )
      {
      // Creates active palette if needed.
      // Clears items if active palette already exists.
      CCamContainerBase::CreateActivePaletteL();

      // Video and still precapture containers implement this method.
      TInt resId = GetAPResourceId();

      CCamActivePaletteHandler* apHandler =
        static_cast<CCamAppUi*>( iEikonEnv->AppUi() )->APHandler();

      apHandler->InstallAPItemsL( resId );
      apHandler->SetView( aView );
      
      if ( appUi->IsEmbedded() )
          {
/*#ifndef __WINS__
          if ( !appUi->IsInCallSend() )
#endif*/
              {
              // disable still/video mode switching in normal embedded mode
              apHandler->SetItemVisible( ECamCmdNewVideo, EFalse );
              apHandler->SetItemVisible( ECamCmdNewPhoto, EFalse );    
              }
          }
      
      apHandler->ResetToDefaultItem();
      }
  OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMPRECAPTURECONTAINERBASE_SETUPACTIVEPALETTEL, "e_CAM_APP_ACTIVEPALETTE_INIT 0" );
  }


// -----------------------------------------------------------------------------
// CCamPostCaptureContainer::BatteryPaneUpdated
//
// Called by CCamBatteryPaneController when battery pane content has been updated. 
// Re-draws background under the battery pane and the battery pane itself.
// -----------------------------------------------------------------------------    
//
void
CCamPreCaptureContainerBase::BatteryPaneUpdated()
  { 
  if( !iDisableRedraws && iBatteryPaneController )
    { 
    // Activate Gc 
    ActivateGc(); 

    // Draw the battery pane
    CWindowGc& gc = SystemGc();
    iBatteryPaneController->Draw( gc );

    DeactivateGc();        
    }
  
  }

// -----------------------------------------------------------------------------
// CCamPostCaptureContainer::LocationIconUpdated
//
// Called by CCamLocationIconController when battery pane content has been updated. 
// Re-draws background under the location icon and the location icon itself.
// -----------------------------------------------------------------------------    
//
void CCamPreCaptureContainerBase::LocationIconUpdated()
  { 
  PRINT( _L("Camera => CCamPreCaptureContainerBase::LocationIconUpdated()") );

  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsLocationSupported() )
      {
      if (  !iDisableRedraws && iLocationIconController 
            && iController.IsViewFinding()
          )
        {
        // Get the location icon rectangle
        TRect rect = iLocationIconController->Rect();
        // Activate Gc
        ActivateGc();
    
        // Invalidate the location icon area
        RWindow window = Window();
        window.Invalidate( rect );
        window.BeginRedraw( rect );
    
        // Redraw the background in that area
        Redraw( rect );
    
        // Draw the location icon
        CWindowGc& gc = SystemGc();
        // Other navi controls should be drawn also before drawing location icon
        // The drawing is done through DrawNaviControls() function since
        // the background decoration should be drawn before the location indicator
        DrawNaviControls(gc);
    
        // Tell the window redraw is finished and deactivate Gc
        window.EndRedraw();
        DeactivateGc();
        }
      }
  PRINT( _L("Camera <= CCamPreCaptureContainerBase::LocationIconUpdated()") );
  }

// -----------------------------------------------------------------------------
// CCamPreCaptureContainerBase::HandleResourceChange
//
// Passes resource changes to battery pane controller
// -----------------------------------------------------------------------------    
//  
void
CCamPreCaptureContainerBase::HandleResourceChange( TInt aType )
    {  
    if( iBatteryPaneController )
        {
        // Inform battery pane of the resource change 
        iBatteryPaneController->HandleResourceChange( aType );
        } 
    
    CCamContainerBase::HandleResourceChange( aType ); 
    }


//
// CCamPreCaptureContainerBase::HandlePointerEventL
//
void CCamPreCaptureContainerBase::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    PRINT3( _L("Camera => CCamPreCaptureContainerBase::HandlePointerEventL iType=%d iPosition=(%d, %d)"),
        aPointerEvent.iType,
        aPointerEvent.iPosition.iX,
        aPointerEvent.iPosition.iY );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    if ( !appUi->IsSecondCameraEnabled() )
       {
       // don't let zoom pane be used when capturing image
       if ( iController.CurrentMode() != ECamControllerImage ||
            ( iController.CurrentOperation() != ECamCapturing &&
              iController.CurrentOperation() != ECamCompleting &&
              iController.CurrentOperation() != ECamFocusing ) )
           {
           if ( iZoomPane )  
                {
                if ( iZoomPane->HandlePointerEventL( aPointerEvent ) )
                    {
                    ShowZoomPaneWithTimer(); 
                    return;
                    }
                }
           }

        if ( aPointerEvent.iType == TPointerEvent::EButton1Down &&
              !appUi->DrawPreCaptureCourtesyUI() )
            {
            appUi->HandleCommandL( ECamCmdRaiseCourtesyUI );
            }
       CCamContainerBase::HandlePointerEventL( aPointerEvent );
       }

    PRINT( _L("Camera <= CCamPreCaptureContainerBase::HandlePointerEventL") );
    }

// ---------------------------------------------------------------------------
//  returns a count, not an error
// ---------------------------------------------------------------------------
//
TInt CCamPreCaptureContainerBase::CreateAfIconL( TRgb aColor ) 
    {
    TInt AFIconCount(0);
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        CreateAfCornerL( aColor, EMbmCameraappQgn_indi_cam4_focus_frame_auto_tl );
        CreateAfCornerL( aColor, EMbmCameraappQgn_indi_cam4_focus_frame_auto_tr );
        CreateAfCornerL( aColor, EMbmCameraappQgn_indi_cam4_focus_frame_auto_bl );
        CreateAfCornerL( aColor, EMbmCameraappQgn_indi_cam4_focus_frame_auto_br );
    
        AFIconCount = iAfIcons.Count();
        }
    return AFIconCount;
    }
    
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//      
void CCamPreCaptureContainerBase::CreateAfCornerL( TRgb aColor, 
                                                   TInt aFileBitmapId )
    {
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        TFileName mbmFileName;
        CamUtility::ResourceFileName( mbmFileName );
        TPtrC resname = mbmFileName; 
        
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask = NULL; 
           
        // Create component bitmaps 
        AknIconUtils::CreateIconL( bitmap, mask, resname, 
                                   aFileBitmapId, aFileBitmapId+1 );
        
        AknIconUtils::SetSize( bitmap, iAfIconCornerSize ); 
        CleanupStack::PushL( bitmap );
        CleanupStack::PushL( mask );
    
        // Destroys mask, if doesn't leave
        CamUtility::SetAlphaL( bitmap, mask, aColor ); 
        CleanupStack::Pop(); // (deleted) mask 
        User::LeaveIfError( iAfIcons.Append( bitmap ) ); 
        CleanupStack::Pop(); // bitmap  
        }
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//    
void CCamPreCaptureContainerBase::SetAfIconSize( TInt aOffset )   
    { 
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() &&
         aOffset-- && iAfIcons.Count() > aOffset )
        {
        for ( TInt i = aOffset; i > aOffset - KAFIconCorners ; i-- ) 
            {
            AknIconUtils::SetSize( iAfIcons[i], iAfIconCornerSize );  
            }
        }    
    }
    
// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CCamPreCaptureContainerBase::DrawAf( CBitmapContext& aGc, 
                                          TInt aOffset ) const  
    {
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        TInt indx = aOffset - KAFIconCorners;
        TInt corners = 0;
        if ( aOffset && iAfIcons.Count() >= aOffset ) 
            {
            while ( corners < KAFIconCorners && iLayouts.Count() > corners )
                {
                TAknLayoutRect layout = iLayouts[corners];
    
                aGc.BitBlt( layout.Rect().iTl, iAfIcons[ indx + corners ]);
                corners++;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//        
void CCamPreCaptureContainerBase::SizeChanged() 
    {
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        if ( AknLayoutUtils::PenEnabled() ) 
            {
            TouchLayout();
            }
        else
            {
            NonTouchLayout();
            }
			
        if( !iController.UiConfigManagerPtr()->IsFaceTrackingSupported() )
            {
            SetAfIconSize( iAfReadyIcon );
            SetAfIconSize( iAfFocusIcon );
            SetAfIconSize( iAfErrIcon );
            }
        }
    else
        {
        CCoeControl::SizeChanged();
        }
    } 
    
// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::NonTouchLayout
// ---------------------------------------------------------------------------
void CCamPreCaptureContainerBase::NonTouchLayout() 
    {
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        iLayouts.Reset(); 
        TAknLayoutRect parent;  
        TAknLayoutRect tmp;  
        TBool variant = Layout_Meta_Data::IsLandscapeOrientation(); 
        TRect vfRect = ViewFinderFrameRect(); 
       
        // Autofocus parent rectangle
        parent.LayoutRect( vfRect, 
            AknLayoutScalable_Apps::cam6_autofocus_pane( variant ).LayoutLine() ); 
        iReticuleRect = parent.Rect();
        iReticuleRect.Move( vfRect.Center() - parent.Rect().Center() );
        
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam6_autofocus_pane_g1().LayoutLine() );
        iAfIconCornerSize = tmp.Rect().Size();
        iLayouts.Append( tmp ); // First corner   
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam6_autofocus_pane_g2().LayoutLine() ); 
        iLayouts.Append( tmp );
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam6_autofocus_pane_g3().LayoutLine() ); 
        iLayouts.Append( tmp );
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam6_autofocus_pane_g4().LayoutLine() ); 
        iLayouts.Append( tmp );
        }
    }

// ---------------------------------------------------------------------------
// CCamPreCaptureContainerBase::TouchLayout
// ---------------------------------------------------------------------------
void CCamPreCaptureContainerBase::TouchLayout() 
    {
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        iLayouts.Reset(); 
        TAknLayoutRect parent;  
        TAknLayoutRect tmp;  
        TBool variant = Layout_Meta_Data::IsLandscapeOrientation(); 
        TRect vfRect = ViewFinderFrameRect(); 
       
        // Autofocus parent rectangle
        parent.LayoutRect( vfRect, 
            AknLayoutScalable_Apps::cam4_autofocus_pane( variant ).LayoutLine() ); 
        iReticuleRect = parent.Rect();
        iReticuleRect.Move( vfRect.Center() - parent.Rect().Center() );
        
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam4_autofocus_pane_g1().LayoutLine() );
        iAfIconCornerSize = tmp.Rect().Size();
        iLayouts.Append( tmp ); // First corner   
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam4_autofocus_pane_g2().LayoutLine() ); 
        iLayouts.Append( tmp );
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam4_autofocus_pane_g3().LayoutLine() ); 
        iLayouts.Append( tmp );
        tmp.LayoutRect( iReticuleRect, AknLayoutScalable_Apps::
                        cam4_autofocus_pane_g3_copy1().LayoutLine() ); 
        iLayouts.Append( tmp );
        }
    }

// ----------------------------------------------------
// CCamPreCaptureContainerBase::ResolutionIndicatorRect
// Returns the rectangle defining the position and size
// of the resolution icon
// ----------------------------------------------------
//
TRect CCamPreCaptureContainerBase::ResolutionIndicatorRect() const 
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    TAknLayoutRect resolutionIconLayout;
    TAknLayoutRect indicatorPane;
    if ( AknLayoutUtils::PenEnabled() )
        {
        TInt variant = Layout_Meta_Data::IsLandscapeOrientation();
        if ( appUi->TargetMode() ==  ECamControllerVideo ) 
            {
            TRect rect;
            TAknLayoutRect vidProgressPane;
            AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect );
            vidProgressPane.LayoutRect( rect,
                AknLayoutScalable_Apps::vid4_progress_pane( variant ) );  
            resolutionIconLayout.LayoutRect( vidProgressPane.Rect(),
                AknLayoutScalable_Apps::vid4_progress_pane_g1() );
            }
        else
            {
            indicatorPane.LayoutRect( Rect(),
                AknLayoutScalable_Apps::cam4_indicators_pane( variant ) ); 

            resolutionIconLayout.LayoutRect( indicatorPane.Rect(),
                AknLayoutScalable_Apps::cam4_indicators_pane_g1() );
            }
        }
    else
        {
        TInt cba =  AknLayoutUtils::CbaLocation() == 
                    AknLayoutUtils::EAknCbaLocationLeft? 
                    AknLayoutUtils::EAknCbaLocationLeft : 0;

        if ( appUi->TargetMode() ==  ECamControllerVideo )
            {
            indicatorPane.LayoutRect( Rect(), 
                AknLayoutScalable_Apps::vid6_indi_pane( cba ));

            resolutionIconLayout.LayoutRect( indicatorPane.Rect(),
                AknLayoutScalable_Apps::vid6_indi_pane_g1( cba ) );             
            }
        else
            {
            indicatorPane.LayoutRect( Rect(), 
                AknLayoutScalable_Apps::cam6_indi_pane( cba ));

            resolutionIconLayout.LayoutRect( indicatorPane.Rect(),
                AknLayoutScalable_Apps::cam6_indi_pane_g1( cba ) );
            }
        }
    return resolutionIconLayout.Rect();
    }

// -------------------------------------------------------------
// CCamPreCaptureContainerBase::BlinkResolutionIndicatorOnChange
// -------------------------------------------------------------
//
void CCamPreCaptureContainerBase::BlinkResolutionIndicatorOnChange( TBool aBlink )
    {
    iBlinkResolutionIndicator = aBlink;
    }

// -------------------------------------------------------------
// CCamPreCaptureContainerBase::DrawResolutionIndicator
// -------------------------------------------------------------
//
void CCamPreCaptureContainerBase::DrawResolutionIndicator()
    {
    PRINT( _L("Camera => CCamPreCaptureContainerBase::DrawResolutionIndicator") );
    iToggleCountdown--;

    // State changed, need to redraw
    ActivateGc();

    // Invalidate the flash icon area
    TRect rect( (iBlinkModeIndicator) ? iSidePane->ModeIndicatorLayoutRect()
                                      : iResolutionIndicators[iCurrentIndicator]->LayoutRect() );
    RWindow window = Window();
    window.Invalidate( rect  );
    window.BeginRedraw( rect );

    // Redraw the background in that area
    Redraw( rect );

    // Draw the icon 
    CWindowGc& gc = SystemGc();
    if ( iBlinkModeIndicator )
        {
        iSidePane->DrawModeIndicator( gc, iDrawIndicator );
        }
    else
        {
        // Mode indicator should be visible, while the resolution indicator blinks
        if ( !iController.UiConfigManagerPtr()->IsCustomCaptureButtonSupported() )
            {
            iSidePane->DrawModeIndicator( gc, ETrue );
            }

        if( iDrawIndicator )
            {
            iResolutionIndicators[iCurrentIndicator]->DisplayIcon();
            }
        else
            {
            iResolutionIndicators[iCurrentIndicator]->ClearIcon();
            }
        iResolutionIndicators[iCurrentIndicator]->Draw( gc );
        
        // If blink timer is canceled abruptly(like capture image) at some places, resolution indicator may go missing in pre-capture mode.
        // So alway set resolution clear flag to EFlase after drawing resolution indicator to avoiding missing indicator.
        // This do not affect indicator blink function because this just set the flag, do not draw the indicator.        
        iResolutionIndicators[iCurrentIndicator]->DisplayIcon();
        }

    // Tell the window redraw is finished and deactivate Gc
    window.EndRedraw();
    DeactivateGc();

    // Stop the periodic timer when enough blinking has been done
    if ( iDrawIndicator && iToggleCountdown <= 0 )
        {
        iBlinkResolutionIndicator = EFalse;
        iBlinkModeIndicator = EFalse;        
        iIndBlinkTimer->Cancel();
        }

    PRINT( _L("Camera <= CCamPreCaptureContainerBase::DrawResolutionIndicator") );
    }

// -------------------------------------------------------------
// CCamPreCaptureContainerBase::IndicatorVisible
// -------------------------------------------------------------
//
TInt CCamPreCaptureContainerBase::IndicatorVisible( TAny *aSelf )
    {
    PRINT( _L("Camera => CCamPreCaptureContainerBase::IndicatorVisible") );
    CCamPreCaptureContainerBase* self = static_cast<CCamPreCaptureContainerBase*> (aSelf);

    if ( self )
        {
        self->iDrawIndicator = !self->iDrawIndicator;
        self->DrawResolutionIndicator();
        }
    PRINT( _L("Camera <= CCamPreCaptureContainerBase::IndicatorVisible") );
    return KErrNone;
    }

// -------------------------------------------------------------
// CCamPreCaptureContainerBase::FocusChanged
// -------------------------------------------------------------
//
void CCamPreCaptureContainerBase::FocusChanged( TDrawNow aDrawNow )
    {
    PRINT3( _L("Camera <> CCamPreCaptureContainerBase::FocusChanged, draw:%d, focused:%d, button focused:%d"), 
                aDrawNow, IsFocused(), 
                iCaptureButtonContainer ? iCaptureButtonContainer->IsFocused() : 0 );
    UpdateCaptureButton();
    CCoeControl::FocusChanged( aDrawNow );
    }

// -------------------------------------------------------------
// CCamPreCaptureContainerBase::UpdateCaptureButton
// -------------------------------------------------------------
//
void CCamPreCaptureContainerBase::UpdateCaptureButton()
    {
    PRINT(_L("Camera => CCamPreCaptureContainerBase::UpdateCaptureButton"));
    
    TBool buttonActive = EFalse;
    if ( iCaptureButtonContainer )
        {
        TCamCameraMode mode = iController.CurrentMode();
        
        buttonActive = ( IsFocused() || iCaptureButtonContainer->IsFocused() ) 
                       && ECamNoOperation == iController.CurrentOperation()
                       && iController.IsViewFinding()
                       && ( ECamControllerImage == mode || ECamControllerVideo == mode );

        PRINT1( _L("Camera <> capture button shown:%d"), buttonActive );
        iCaptureButtonContainer->SetCaptureButtonShown( buttonActive );
        }
    PRINT(_L("Camera <= CCamPreCaptureContainerBase::UpdateCaptureButton"));
    }

// -------------------------------------------------------------
// CCamPreCaptureContainerBase::PrepareForCapture
// -------------------------------------------------------------
//
void CCamPreCaptureContainerBase::PrepareForCapture()
    {
    if ( iZoomPane )
        {
        PRINT( _L("Camera <> CCamPreCaptureContainerBase::PrepareForCapture - StopZoom()") );
        iZoomPane->StopZoom();
        }
    
    // Stop blinking icon when capture is initiated
    if ( iIndBlinkTimer && iIndBlinkTimer->IsActive() )
        {
        iIndBlinkTimer->Cancel();
        iDrawIndicator = ETrue;
        }
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    if ( iCaptureButtonContainer && !( appUi && appUi->SelfTimerEnabled() ) )
        {
        iCaptureButtonContainer->SetCaptureButtonShown( EFalse );
        }
    }
// End of File

