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
* Description:  Container class for still image pre-capture view*
*/

// INCLUDE FILES
#include <AknIconUtils.h>
#include <cameraapp.mbg>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <aknlayoutscalable_apps.cdl.h>

#include "CamStillPreCaptureContainer.h"
#include "CamLogger.h"
#include "CamPreCaptureViewBase.h"
#include "CamAppUi.h"
#include "CamSidePane.h"
#include "CamSelfTimer.h"
#include "CamUtility.h"
#include "camactivepalettehandler.h"
#include "CameraUiConfigManager.h"
#include "OstTraceDefinitions.h"
#include "CamIndicator.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamStillPreCaptureContainerTraces.h"
#endif

#include "camvfgridinterface.h"
#include "camlinevfgriddrawer.h"
#include "CamPanic.h"

// Constants
const TUid KGridDrawerUid = { KCamLineDrawVfGridUid };
const TInt KGridHrzLines  = 2;
const TInt KGridVrtLines  = 2;
const TInt KGridThickness = 2;
const TRgb KGridColor     = KRgbGray;
const CGraphicsContext::TPenStyle KGridStyle = CGraphicsContext::ESolidPen;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStillPreCaptureContainer* CCamStillPreCaptureContainer::NewL(
        CCamAppController& aController,
        CAknView& aView,
        const TRect& aRect )
    {
    CCamStillPreCaptureContainer* self = new( ELeave ) CCamStillPreCaptureContainer(
        aController, aView );

    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    // Return newly created CCamStillPreCaptureContainer instance
    return self;
    }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::~CCamStillPreCaptureContainer
// Destructor
// ---------------------------------------------------------------------------
//
CCamStillPreCaptureContainer::~CCamStillPreCaptureContainer()
  {
  PRINT( _L("Camera => ~CCamStillPreCaptureContainer" ))

  if ( iAPCreateAO )
      {
      iAPCreateAO->Cancel();
      delete iAPCreateAO;
      }

  if ( iXenonFlashSupported )
      {
      // Unsubscribe blink events
      if( CCamFlashStatus* flashStatus = iController.FlashStatus() )
        flashStatus->Subscribe( NULL );

      delete iFlashBitmap;
      delete iFlashBitmapMask;
      }
  PRINT( _L("Camera <= ~CCamStillPreCaptureContainer" ))
  }

// ---------------------------------------------------------
// CCamStillPreCaptureContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamStillPreCaptureContainer::ConstructL( const TRect& aRect )
  {
  PRINT( _L("Camera => CCamStillPreCaptureContainer::ConstructL" ))
  BaseConstructL( aRect );

  if ( iController.UiConfigManagerPtr() &&
       iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
      {
      iAPCreateAO = CIdle::NewL(CActive::EPriorityStandard);
      }

  if ( iController.UiConfigManagerPtr() &&
       iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
      {
      if( iController.IntegerSettingValue( ECamSettingItemPhotoShowFocusPoint ) )
        {
        iShowReticule = EFalse;
        }
      else
        {
        iShowReticule = ETrue;
        }
    }

  iSidePane->SetCaptureMode( ECamControllerImage );


  iXenonFlashSupported = iController.UiConfigManagerPtr()->IsXenonFlashSupported();
  if ( iXenonFlashSupported )
      {
      InitFlashIconL();
      LayoutFlashIcon();
      }

  PrepareProcessingTextL( ETrue );

  if ( !( iController.UiConfigManagerPtr() &&
       iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() ) )
      {
      OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSTILLPRECAPTURECONTAINER_CONSTRUCTL, "e_CAM_APP_AP_SETUP 1" );
      static_cast<CCamPreCaptureContainerBase*>( this )
          ->SetupActivePaletteL( static_cast<CCamViewBase*>(&iView) );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMSTILLPRECAPTURECONTAINER_CONSTRUCTL, "e_CAM_APP_AP_SETUP 0" );
      }
  PRINT( _L("Camera <= CCamStillPreCaptureContainer::ConstructL" ))
  }

// ---------------------------------------------------------
// CCamContainerBase::BaseConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamStillPreCaptureContainer::BaseConstructL( const TRect& aRect )
    {
    CCamPreCaptureContainerBase::BaseConstructL( aRect );
    // Layout the reticule.
    if ( iController.UiConfigManagerPtr() &&
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        SizeChanged(); // Init layout
        }
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::CCamStillPreCaptureContainer
// C++ constructor
// ---------------------------------------------------------------------------
//

CCamStillPreCaptureContainer::CCamStillPreCaptureContainer(
        CCamAppController& aController,
        CAknView& aView )
: CCamPreCaptureContainerBase( aController, aView )
    {
    }


// ---------------------------------------------------------
// CCamStillPreCaptureContainer::HandleControllerEventL
// Receives notifications about changes in recording state from the controller
// ---------------------------------------------------------
//
void CCamStillPreCaptureContainer::HandleControllerEventL( TCamControllerEvent aEvent, TInt aError )
  {
  PRINT1( _L("Camera => CCamStillPreCaptureContainer::HandleControllerEventL aEvent:%d" ), aEvent )
  CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi());

  switch( aEvent )
    {
    // ---------------------------------------------------
  	case ECamEventImageQualityChanged:
      {
      PRINT( _L("Camera <> CCamStillPreCaptureContainer - ECamEventImageQualityChanged") );
      // If our viewfinder area is already set and image quality changes,
      // do re-layout.
      if( !iRect.IsEmpty() )
        {
        const TRect r( ViewFinderFrameRect() );
        if( iRect != r )
          {
          PRINT( _L("Camera <> CCamStillPreCaptureContainer - doing re-layout..") );
          iRect = r;
          SizeChanged();
          }
        }
      SetResolutionIndicator();
      break;
      }
    // ---------------------------------------------------
  	case ECamEventLocationSettingChanged:
      {
      // If the location setting was changed on, then the indicator should be visible
      SetLocationIndicatorVisibility();
      break;
      }
    // ---------------------------------------------------
  	case ECamEventCameraChanged:
      {
      break;
      }
    // ---------------------------------------------------
  	case ECamEventOperationStateChanged:
  	  {
      if( ECamCapturing == iController.CurrentOperation() )
        {
        // If capture has just begun, and it's burst capture,
        // will need to set the softkeys to Blank/Cancel (via UpdateCbaL call)
        if ( appUi->IsBurstEnabled() )
            {
            // Burst *is* enabled, so force the update of the CBA
            static_cast<CCamViewBase&>(iView).UpdateCbaL();
            }
        else // if burst is not enabled show the saving image wait note
       	    {
			Window().Invalidate();
       	    }
        }
      else
       	{
       	}
      break;
  	  }
    // ---------------------------------------------------
    case ECamEventSetupStateChanged:
      {
      appUi->APHandler()->UpdateActivePaletteL();
      break;
      }
    // ---------------------------------------------------
    case ECamEventEngineStateChanged:
        {
        if( iController.CurrentImageModeSetup() == ECamImageCaptureBurst &&
            !iController.SequenceCaptureInProgress() )
            {
            // Burst mode - processing text needed
            PrepareBurstProcessingTextL();
            }
         break;
         }
    // ---------------------------------------------------
    case ECamEventInitReady:
        {
        // event only received if UIOrientationOverride feature is supported
        if( iAPCreateAO->IsActive() )
            {
            iAPCreateAO->Cancel();
            }
		iAPCreateAO->Start( TCallBack( BackgroundAPL,this ) );
        break;
        }
    // ---------------------------------------------------
    default:
      {
      // otherwise, do nothing
      break;
      }
    // ---------------------------------------------------
    }
    CCamPreCaptureContainerBase::HandleControllerEventL( aEvent, aError );
    PRINT( _L("Camera <= CCamStillPreCaptureContainer::HandleControllerEventL" ) )
    }


//----------------------------------------------
// BackgroundAPL
// Create AP ItemActive object Callback
//----------------------------------------------
TInt CCamStillPreCaptureContainer::BackgroundAPL( TAny *aparam )
    {
    CCamStillPreCaptureContainer* camStillCaptureContainer = reinterpret_cast<CCamStillPreCaptureContainer*> (aparam);
    return camStillCaptureContainer->SetupActivePaletteCallbackL();
    }


//----------------------------------------------
// SetupActivePaletteCallbackL
// Initializes ActivePalette
//----------------------------------------------
TInt CCamStillPreCaptureContainer::SetupActivePaletteCallbackL()
    {
    PRINT( _L("Camera => CCamStillPreCaptureContainer::SetupActivePaletteCallbackL" ) )
    CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi());
    if ( !iController.IsTouchScreenSupported() &&
         iController.CurrentMode()== ECamControllerImage )
        {
        if ( appUi )
            {
            appUi->SetActivePaletteVisibility( EFalse);
            if ( !appUi->ActivePalette() )
                {
                OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSTILLPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_SETUP 1" );
                static_cast<CCamPreCaptureContainerBase*>( this )
                    ->SetupActivePaletteL( static_cast<CCamViewBase*>(&iView) );
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMSTILLPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_SETUP 0" );
                }
            else
                {
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMSTILLPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_UPDATE 1" );
                appUi->APHandler()->UpdateActivePaletteL();
                OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMSTILLPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_UPDATE 0" );
                }
            appUi->SetActivePaletteVisibility( ETrue );
            }
        else
            {
            }
        }
    PRINT( _L("Camera <= CCamStillPreCaptureContainer::SetupActivePaletteCallbackL" ) )
    return EFalse;
    }

// ---------------------------------------------------------
// CCamStillPreCaptureContainer::OfferKeyEventL
// Handle key events
// ---------------------------------------------------------
//
TKeyResponse
CCamStillPreCaptureContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                    TEventCode aType )
    {
    // if the cancel softkey is pressed

    // If currently capturing a burst
    if ( iController.SequenceCaptureInProgress() )
        {
        if ( aKeyEvent.iScanCode == EStdKeyDevice1 )
            {
            // Just return so the key press does not pass up to the base
            // class This will now be handled in HandleCommandL()
            return EKeyWasNotConsumed;
            }
        }
    return CCamPreCaptureContainerBase::OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CCamStillPreCaptureContainer::ViewFinderLayoutResourceIds
// Return the layout resource id for the viewfinder
// ---------------------------------------------------------
//
void CCamStillPreCaptureContainer::ViewFinderLayoutResourceIds(
                                                TInt& aViewFinderLayoutId,
                                                TInt& aReticuleLayoutId ) const
    {
        {
        aViewFinderLayoutId = ROID(R_CAM_VIEWFINDER_RECT_STILL_ID);
        CCamAppUiBase* appui = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
        if ( appui->IsSecondCameraEnabled() )
            {
            aViewFinderLayoutId = ROID(R_CAM_STILL_DISPLAYED_RECT_SECONDARY_ID);
            }
        }
    if ( iController.UiConfigManagerPtr() &&
         iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        // set the layouts for the still capture viewfinder and reticule
        aReticuleLayoutId = R_CAM_IMG_PRECAP_RETICULE;
        }
    else
        {
        aReticuleLayoutId = 0;
        }
    }

// ---------------------------------------------------------
// CCamStillPreCaptureContainer::SetResolutionIndicator
// Sets the resolution indicator to the required icon
// ---------------------------------------------------------
//
void CCamStillPreCaptureContainer::SetResolutionIndicator()
    {
    // The setting stored is an index into the array of supported qualities,
    // and since this matches the array of supported quality icons, we do
    // not need to do any further lookup on this value.
    iCurrentIndicator = iController.IntegerSettingValue( ECamSettingItemPhotoQuality );
    }

// ---------------------------------------------------------
// CCamStillPreCaptureContainer::SetLocationIndicatorVisibility
// Sets the resolution indicator visibility, if it is set or not
// ---------------------------------------------------------
//
void CCamStillPreCaptureContainer::SetLocationIndicatorVisibility()
    {
    // The setting stored is an index of whether the location setting is on or off
    iLocationIndicatorVisible = (/*TCamLocationId::ECamLocationOn*/1 == iController.IntegerSettingValue( ECamSettingItemRecLocation ));
    }

// ----------------------------------------------------
// CCamStillPreCaptureContainer::IncreaseFlashSettingL
// Move up through the flash settings
// ----------------------------------------------------
//
void CCamStillPreCaptureContainer::IncreaseFlashSettingL()
    {
    if ( iZoomPane->IsVisible()
         || static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() )->IsSecondCameraEnabled()
        )
        {
        return;
        }
    // get current setting
    TInt setting =
            iController.IntegerSettingValue( ECamSettingItemDynamicPhotoFlash );

    // Max/Min settings are ECamFlashAuto/ECamFlashOff
    // and navi-key right moves down the list of settings
    if ( setting == ECamFlashOff )
        {
        setting = ECamFlashAuto;
        }
    else if ( setting >= ECamFlashAuto )
        {
        setting++;
        }
    else
        {
        // remove Lint warning
        }

    // only update if setting is invalid
    if ( setting >= ECamFlashAuto && setting <= ECamFlashOff )
        {
        iController.SetIntegerSettingValueL(
                                ECamSettingItemDynamicPhotoFlash, ( setting ) );
        }
    }

// ----------------------------------------------------
// CCamStillPreCaptureContainer::DecreaseFlashSettingL
// Move down through the flash settings
// ----------------------------------------------------
//
void CCamStillPreCaptureContainer::DecreaseFlashSettingL()
    {
    if ( iZoomPane->IsVisible()
         || static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() )->IsSecondCameraEnabled()
        )
        {
        return;
        }
    // get current setting
    TInt setting =
            iController.IntegerSettingValue( ECamSettingItemDynamicPhotoFlash );

    // Max/Min settings are ECamFlashAuto/ECamFlashOff
    // and navi-key left moves up the list of settings
    if ( setting == ECamFlashAuto )
        {
        setting = ECamFlashOff;
        }
    else if ( setting > ECamFlashAuto )
        {
        setting--;
        }
    else
        {
        // remove Lint warning
        }

    // only update if setting is invalid
    if ( setting >= ECamFlashAuto && setting <= ECamFlashOff )
        {
        iController.SetIntegerSettingValueL(
                                ECamSettingItemDynamicPhotoFlash, ( setting ) );
        }
    }

// ----------------------------------------------------
// CCamStillPreCaptureContainer::HandleCaptureKeyEventL
// Change the current capture state
// ----------------------------------------------------
//
TKeyResponse
CCamStillPreCaptureContainer::HandleCaptureKeyEventL( const TKeyEvent& aKeyEvent )
  {
  PRINT( _L("Camera => CCamStillPreCaptureContainer::HandleCaptureKeyEventL"))
  TKeyResponse keyResponse = EKeyWasNotConsumed;
  CCamAppUi* appui = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
  __ASSERT_DEBUG(appui, CamPanic(ECamPanicNullPointer));
  TBool MSKCapture(EFalse);
  // First handle middle softkey and enter key capture event
  if ( !appui->IsToolBarVisible()
       && ( aKeyEvent.iScanCode == EStdKeyDevice3
            || aKeyEvent.iScanCode == EStdKeyEnter ) )
    {
    if ( iController.UiConfigManagerPtr() )
        {
        if ( iController.UiConfigManagerPtr()->IsOpticalJoyStickSupported() )
            {
            MSKCapture = ETrue;
            }
        else
            {
            if ( iController.UiConfigManagerPtr()->IsXenonFlashSupported() )
                {
                if ( iController.CheckFlash()
                       && static_cast<CCamPreCaptureViewBase*>( &iView )
                           ->StartMskCaptureL() )
                    {
                    return EKeyWasConsumed;
                    }
                }
             else if ( static_cast<CCamPreCaptureViewBase*>( &iView )->StartMskCaptureL() )
                {
                return EKeyWasConsumed;
                }
            }
        }
    }

  TBool selfTimerEnabled = appui->SelfTimerEnabled();
  TBool timeLapseEnabled = appui->CurrentBurstMode() == ECamImageCaptureTimeLapse;

  // if currently capturing a sequence
  if ( iController.SequenceCaptureInProgress() )
    {
    // if capturing an advanced timelapse under the press to start, press to stop rules
    // (i.e. not selftimer which captures 6 images)
    if ( timeLapseEnabled && !selfTimerEnabled )
      {
      // Stop the capture now
      iController.StopSequenceCaptureL();
      // Update CBA for setup pane usage
      TRAP_IGNORE( static_cast<CCamPreCaptureViewBase*>( &iView )->UpdateCbaL() );
      PRINT( _L("Camera <= CCamStillPreCaptureContainer: sequence capture stopped"))
      return EKeyWasConsumed;
      }
    else
      {
      // For certain products normal sequence capture, if we
      // are already capturing a burst, just consume but ignore it.
      PRINT( _L("Camera <= CCamStillPreCaptureContainer: ignored during sequence"))
      return EKeyWasConsumed;
      }
    }

  if ( iController.UiConfigManagerPtr() &&
       iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
      {
      // If the shutter key has continued to a full press OR
      // we are already focussed (as scene is a forced focus mode)
      if ( iKeyShutter
        || !iPhotoSceneUsesReticule
         )
        {
        iKeyShutter = EFalse;

        // Check if we are in Burst Mode with the controller not currently focussed.
        if ( appui->IsBurstEnabled()
          && iController.CurrentOperation() != ECamFocused &&
             iController.CurrentOperation() != ECamFocusFailed )
          {
          // We want to start focus if in multishot and NOT in self-timer mode
          // and current scene allows autofocus (ie if it's not forced)
          if ( !selfTimerEnabled
            && iPhotoSceneUsesReticule
             )
            {
            if( iXenonFlashSupported && timeLapseEnabled && !iController.CheckFlash() )
              {
              // In time lapse mode, do not start focusing with full press if
              // Xenon flash is not ready. This is not the case for normal
              // burst mode, because flash is not used there.
              iKeyShutter = ETrue;
              PRINT( _L("Camera <= CCamStillPreCaptureContainer: Flash not ready in timelapse mode, ignore key"))
              return EKeyWasConsumed;
              }
            // Below code commented out - fast capture should be supported also
            // in burst/sequence capture mode. Also, in case focusing is already
            // in progress, the call won't do anything.
            // iController.StartAutoFocus();
            }
          }
        }
#ifndef __WINS__
      else
        {
        // This means we've received a full press without a half press -
        // probably because the half press was consumed by the post capture view
        // and triggered a switch to this view.  In this case, we don't want to
        // take a picture, therefore ignore the event.
        if ( !appui->IsSecondCameraEnabled()
             && !MSKCapture )
          {
          PRINT( _L("Camera <= CCamStillPreCaptureContainer: full press without half press ignored"))
          return EKeyWasConsumed;
          }
        }
#endif // __WINS__
      }
  else
      {
      // in case AF is not supported, check memory here before capturing
      if ( !iController.IsViewFinding() || !appui->CheckMemoryL() )
          {
          return EKeyWasConsumed; 
          }         
      }
      
  if( iXenonFlashSupported
   && !appui->IsSecondCameraEnabled()
   && !MSKCapture
   && !iController.CheckFlash() )
    {
    if( !appui->IsBurstEnabled() || (timeLapseEnabled && selfTimerEnabled) )
      {
      // For non-burst case, or time lapse captures with self timer,
      // we do not allow primary camera capture if flash is required,
      // but not ready.
      iKeyShutter = ETrue;
      PRINT( _L("Camera <= CCamStillPreCaptureContainer: Flash not ready, ignore key"))
      return EKeyWasNotConsumed;
      }
    }

  // Go straight to capture
  keyResponse = appui->StartCaptureL( aKeyEvent );

  // Blank out the softkeys if we are capturing
  if ( EKeyWasConsumed == keyResponse
    && !iController.SequenceCaptureInProgress()
    && !iController.CaptureModeTransitionInProgress()
     )
    {
    // Only blank the cba if self timer is not enabled
    // or it is enabled but not counting down
    if ( !selfTimerEnabled
      || (  selfTimerEnabled && !appui->SelfTimer()->IsActive() )
       )
      {
      static_cast<CCamViewBase*>( &iView )->BlankSoftkeysL();
      }
     if ( iController.IsTouchScreenSupported() )
         {
         CAknToolbar* fixedToolbar = appui->CurrentFixedToolbar();
         if ( fixedToolbar )
             {
             fixedToolbar->SetToolbarVisibility( EFalse );
             }
         }
    }

  if ( iController.IsTouchScreenSupported() )
      {
      // Hide the toolbar in case full press in burstmode
      if ( appui->IsBurstEnabled() )
          {
          CAknToolbar* fixedToolbar = appui->CurrentFixedToolbar();
          if ( fixedToolbar )
              {
              fixedToolbar->SetToolbarVisibility( EFalse );
              }
          }
      }

  PRINT( _L("Camera <= CCamStillPreCaptureContainer::HandleCaptureKeyEventL"))
  return keyResponse;
  }


// ----------------------------------------------------
// CCamStillPreCaptureContainer::HandleShutterKeyEventL
// Change the current capture state following shutter
// key events
// ----------------------------------------------------
//
TKeyResponse
CCamStillPreCaptureContainer::HandleShutterKeyEventL( const TKeyEvent& aKeyEvent,
                                                            TEventCode aType )
  {
  CCamAppUi* appui = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
  __ASSERT_DEBUG(appui, CamPanic(ECamPanicNullPointer));
  TBool MSKCapture(EFalse);
  if ( !appui->IsToolBarVisible()
       && aKeyEvent.iScanCode == EStdKeyDevice3
       && iController.UiConfigManagerPtr()
       && iController.UiConfigManagerPtr()->IsOpticalJoyStickSupported() )
    {
    MSKCapture = ETrue;
    }

  // If the key is a half-press and down
#ifndef __WINS__
    if ( aKeyEvent.iScanCode == iPrimaryCameraAFKeys[0]
#else
    if ( aKeyEvent.iScanCode == EProductKeyCaptureHalf &&
         !( aKeyEvent.iModifiers & EModifierShift )
#endif //__WINS__
        && iController.UiConfigManagerPtr()
        && iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
    {
    if ( aType == EEventKeyDown )
      {
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMSTILLPRECAPTURECONTAINER_HANDLESHUTTERKEYEVENTL, "e_CAM_PRI_AF_LOCK 1" );
      PRINT( _L("Camera => CCamStillPreCaptureContainer::HandleHalfKeyEventL DOWN"))
      PERF_MESSAGE_L2( EPerfMessageCaptureKeyHalfPressed );
      // If the shutter key has been half pressed
      iKeyShutter = ETrue; // Shutter key - first stage press -> start focussing...

      // check for active viewfinder and available space before proceeding with capture
      if ( !iController.IsViewFinding() || !appui->CheckMemoryL() )
        {
        // no memory available - stop capture
        return EKeyWasConsumed;
        }

      if ( iController.UiConfigManagerPtr()
           && iController.UiConfigManagerPtr()->IsAutoFocusSupported()
           && !appui->SelfTimerEnabled()
           && iPhotoSceneUsesReticule )
        {
        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSTILLPRECAPTURECONTAINER_HANDLESHUTTERKEYEVENTL, "e_CAM_APP_AF 1" );  //CCORAPP_AF_START
        iController.StartAutoFocus();
        }

      CAknToolbar* fixedToolbar = appui->CurrentFixedToolbar();

      if ( fixedToolbar )
        {
        CAknToolbarExtension* extension = fixedToolbar->ToolbarExtension();
        if ( extension )
            {
            extension->SetShown( EFalse );
            }
        }
      iZoomPane->MakeVisible( EFalse, ETrue );

      return EKeyWasConsumed;
      }
    else if ( aType == EEventKeyUp && !MSKCapture )
      {
      PRINT( _L("Camera => CCamStillPreCaptureContainer::HandleKeyEventL EProductKeyCaptureHalf up "))
      // If full shutter press is released
      // The order of conditions is important here as half shutter only release resets the
      // value of iKeyShutter
      if ( !iKeyShutter )
        {
        // Cancelling of a press-and-hold burst is handled
        // when the full-press is released, so nothing to
        // do when release the half-press apart from consume it
        PRINT( _L("Camera <= CCamStillPreCaptureContainer::HandleKeyEventL no action, return EKeyWasConsumed"))
        return EKeyWasConsumed;
        }
      else
        {
        // if half shutter only is released
        iKeyShutter = EFalse;
        TCamCaptureOperation operation = iController.CurrentOperation();
        // if not in self timer mode or about to start a capture
        if ( !appui->SelfTimerEnabled()
          && !iController.CapturePending()
          || ECamFocusing == operation
           )
          {
          // In timelapse mode, when stopping with capture key,
          // need this condition to avoid unnecessary entering
          // to no-operation while already completing.
          if( ECamCompleting != operation )
            {
            iController.CancelFocusAndCapture();
            }

          // if not currently capturing
          if ( ECamCapturing != operation )
            {
            appui->UpdateCba();
            }
          }
        return EKeyWasConsumed;
        }
      }
    else
      {
      // Removal of warning
      }
    }
  return EKeyWasNotConsumed;
  }



// ---------------------------------------------------------------------------
// DrawAdditionalIcons
//
// ---------------------------------------------------------------------------
//
void
CCamStillPreCaptureContainer::DrawAdditionalIcons( CBitmapContext& aGc ) const
  {
  if( iXenonFlashSupported && iFlashIconVisible && ECamActiveCameraSecondary != iController.ActiveCamera() )
    {
    DrawFlashIcon( aGc );
    }
  }

// ----------------------------------------------------
// CCamStillPreCaptureContainer::ResolutionIndicatorIconPsiKey
// Returns the PSI key relating to the array of resolution
// indicator bitmaps.
// ----------------------------------------------------
//
TCamPsiKey CCamStillPreCaptureContainer::ResolutionIndicatorIconPsiKey() const
    {
    return ECamPsiPrecapStillQualityIconIds;
    }

// ---------------------------------------------------------------------------
// virtual GetAPResourceId
// (From CCamPreCaptureContainerBase)
//
// Helper method to select the AP resource used to set AP items.
// ---------------------------------------------------------------------------
//
TInt CCamStillPreCaptureContainer::GetAPResourceId() const
    {
    CCamAppUi* appUi =  static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    TBool embedded   = appUi->IsEmbedded();
    TInt resId = 0;
    if( embedded )
        {
        if ( !appUi->IsSecondCameraEnabled() )
            {
            resId = R_CAM_EMB_STILL_PRE_CAPTURE_AP_ITEMS_ADVANCED;
            }
        }
    else
        {
        if ( !appUi->IsSecondCameraEnabled() )
            {
            resId = R_CAM_STILL_PRE_CAPTURE_AP_ITEMS_ADVANCED;
            }
        else
            {
            resId = R_CAM_STILL_PRE_CAPTURE_AP_ITEMS_SECONDARY;
            }
        }
    return resId;
    }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::SizeChanged
// Called by framework when the component size is changed
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::SizeChanged()
  {
  CCamPreCaptureContainerBase::SizeChanged();
  if ( iXenonFlashSupported )
      {
      LayoutFlashIcon();
      }
  }


// ---------------------------------------------------------------------------
// HandleForegroundEventL
// ---------------------------------------------------------------------------
//
void
CCamStillPreCaptureContainer::HandleForegroundEventL( TBool aForeground )
  {
  PRINT( _L("Camera => CCamStillPreCaptureContainer::HandleForegroundEventL") )

  CCamPreCaptureContainerBase::HandleForegroundEventL( aForeground );
  if ( iXenonFlashSupported )
      {

      // Subscribe / unsubscribe from flash related events
      // When going to background -> unsubscribe
      // Otherwise -> subscribe
      SubscribeFlashEvents( aForeground );
      }

  PRINT( _L("Camera => CCamStillPreCaptureContainer::HandleForegroundEventL") )
  }



// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::FlashIconVisible
// Called when Xenon flash icon visibility changes
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::FlashIconVisible( TBool aVisible )
  {
  PRINT( _L("Camera => CCamStillPreCaptureContainer::FlashIconVisible") )
  if( ECamActiveCameraSecondary == iController.ActiveCamera() )
    {
    // Do not draw the flash icon even if visible, if using secondary
    // camera, as it does not use the flash at all.
    return;
    }


  if( ECamCapturing  == iController.CurrentOperation() ||
      ECamCompleting == iController.CurrentOperation() )
    {
    if ( !iFlashIconVisible && aVisible  )
      {
      // During capture, if flash icon is currently invisible,
      // do not make it visible. Also if the state has not changed,
      // we don't need to redraw anything
      return;
      }
    }

  if( iFlashIconVisible != aVisible )
    {
    // Icon state has changed. Redraw.

    iFlashIconVisible = aVisible;

    // State changed, need to redraw
    ActivateGc();

    // Invalidate the flash icon area
    RWindow window = Window();
    window.Invalidate( iFlashIconRect );
    window.BeginRedraw( iFlashIconRect );

    // Redraw the background in that area
    Redraw( iFlashIconRect );

    // Draw the flash icon itself
    CWindowGc& gc = SystemGc();
    if( aVisible )
      {
      DrawFlashIcon( gc );
      }

    // Tell the window redraw is finished and deactivate Gc
    window.EndRedraw();
    DeactivateGc();
    }
  PRINT( _L("Camera <= CCamStillPreCaptureContainer::FlashIconVisible") )
  }



// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::FlashError
// Called when there was an error while recharging
// the Xenon flash.
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::FlashError()
  {
  }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::InitFlashIconL
//
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::InitFlashIconL()
  {
  iFlashIconVisible = EFalse;

  // Init flash icon bitmap and mask
  TFileName resFileName;
  CamUtility::ResourceFileName( resFileName );
  TPtrC resname = resFileName;

  AknIconUtils::CreateIconL( iFlashBitmap,
                           iFlashBitmapMask,
                           resname,
                           EMbmCameraappQgn_indi_cam4_flash_on,
                           EMbmCameraappQgn_indi_cam4_flash_on_mask );

  // Subscribe to blink events
  if( CCamFlashStatus* flashStatus = iController.FlashStatus() )
    {
    flashStatus->Subscribe( this );
    iFlashIconVisible = flashStatus->FlashIconVisible();
    PRINT( _L("Camera :: CCamStillPreCaptureContainer::InitFlashIconL - Flash events subscribed") )
    }
  else
    {
    iFlashIconVisible = EFalse;
    }
  }


// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::LayoutFlashIcon
//
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::LayoutFlashIcon()
  {
  // Set rectangle
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect );
    TInt cba =  AknLayoutUtils::CbaLocation() ==
                AknLayoutUtils::EAknCbaLocationLeft;
    TAknLayoutRect parent;
    parent.LayoutRect( rect, AknLayoutScalable_Apps::cam6_mode_pane( cba ) );
    TAknLayoutRect l;
    l.LayoutRect( parent.Rect(),
                  AknLayoutScalable_Apps::main_camera4_pane_g3( 1 ) );
    iFlashIconRect = l.Rect();
    if( iFlashBitmap )
      {
      AknIconUtils::SetSize( iFlashBitmap, iFlashIconRect.Size() );
      }
  }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::DrawFlashIcon
//
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::DrawFlashIcon( CBitmapContext& aGc ) const
  {
  if( iFlashBitmap )
    {
    // aGc.DrawBitmap( iFlashIconRect, iFlashBitmap );
    aGc.BitBltMasked( iFlashIconRect.iTl, iFlashBitmap, iFlashIconRect.Size(), iFlashBitmapMask, ETrue );
    }
  }

// ---------------------------------------------------------------------------
// SubscribeFlashEvents
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::SubscribeFlashEvents( TBool aSubscribe )
  {
  if( CCamFlashStatus* flashStatus = iController.FlashStatus() )
    {
    if( aSubscribe )
      {
      // Re-subscribe when going to foreground
      // Duplicate subscription is acceptable since only one observer can subscribe to
      // flash related events.
      flashStatus->Subscribe( this );
      iFlashIconVisible = flashStatus->FlashIconVisible();
      PRINT( _L("Camera :: CCamStillPreCaptureContainer::SubscribeFlashEvents - Flash events subscribed") )
      }
    else
      {
      // Un-subscribe when going to background
      flashStatus->Subscribe( NULL );
      iFlashIconVisible = EFalse;
      PRINT( _L("Camera :: CCamStillPreCaptureContainer::SubscribeFlashEvents - Flash events unsubscribed") )
      }
    }
  }

// ---------------------------------------------------------------------------
// CCamStillPreCaptureContainer::MakeVisible
//
// Used to subscribe / unsubscribe from flash events when the control is
// activated / deactivated (standby mode is enabled)
// ---------------------------------------------------------------------------
//
void CCamStillPreCaptureContainer::MakeVisible( TBool aVisible )
  {
  PRINT1( _L("Camera => CCamPreCaptureContainerBase::MakeVisible %d"), aVisible )
  if ( iXenonFlashSupported )
      {
      // Subscribe / unsubscribe from flash related events
      // When going to background -> unsubscribe
      // Otherwise -> subscribe
      SubscribeFlashEvents( aVisible );

      }
  CCamPreCaptureContainerBase::MakeVisible( aVisible );
  PRINT( _L("Camera <= CCamPreCaptureContainerBase::MakeVisible") )
  }


// -----------------------------------------------------------------------------
// CCamStillPreCaptureContainer::InitVfGridL
// -----------------------------------------------------------------------------
// 
void 
CCamStillPreCaptureContainer::InitVfGridL( const TRect& /*aRect*/ )
  {
  PRINT( _L("Camera => CCamStillPreCaptureContainer::InitVfGridL") );  
  iVfGridRect = ViewFinderFrameRect(ECamControllerImage);

  // No vf grid in secondary camera.
  if( ECamActiveCameraSecondary != iController.ActiveCamera() )
    {
    // Create grid drawer if not done yet
    if( !iVfGridDrawer )
      {
      iVfGridDrawer = CCamVfGridFactory::CreateVfGridDrawerL( KGridDrawerUid );
      }
    ResetVFGridVisibility();
  
    PRINT( _L("Camera <> CCamStillPreCaptureContainer: Setting vf grid lines..") );  
    
    // Setting the lines for grid is specific operation
    // for CCamLineVfGridDrawer class not defined in MCamVfGridDrawer interface.
    CCamLineVfGridDrawer* ptr = static_cast<CCamLineVfGridDrawer*>( iVfGridDrawer );
    ptr->SetLinesL  ( iVfGridRect, KGridHrzLines, KGridVrtLines, EFalse );
    ptr->SetPenStyle( KGridStyle ); 
    ptr->SetPenSize ( TSize( KGridThickness, KGridThickness ) );
    ptr->SetPenColor( KGridColor );
    }
  else
    {
    PRINT( _L("Camera <> CCamStillPreCaptureContainer: Secondary cam active, no vf grid") );  
    }
  PRINT( _L("Camera <= CCamStillPreCaptureContainer::InitVfGridL") );  
  }
// End of File
