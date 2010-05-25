/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for all camera application containers*
*/



// INCLUDE FILES
#include <eikmenub.h>
#include <aknview.h>
#include <avkon.hrh>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknUtils.h>
// System includes for changing the orientation.
#include <eikenv.h>
#include <eikappui.h>

#include <activepalette2ui.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <StringLoader.h>

#include "CamContainerBase.h"
#include "CamAppController.h"
#include "CamLogger.h"
#include "CamUtility.h"
#include "CamAppUi.h"
#include "CamNaviCounterControl.h"
#include "CamNaviProgressBarControl.h"
#include "CamZoomPane.h"
#include "camactivepalettehandler.h"
#include "CameraUiConfigManager.h"
#include "CamViewBase.h"
#include "cambackupcontainer.h"

  static const TInt KCamBurstCounterLength = 2;
  _LIT(KCamBurstCounterFormat, "%02d");

// ================= STATIC MEMBER VARIABLES ================
	
MActivePalette2UI* CCamContainerBase::iActivePalette = NULL;

// ================= MEMBER FUNCTIONS =======================

// Destructor
CCamContainerBase::~CCamContainerBase()
  {
  PRINT( _L("Camera => ~CCamContainerBase") );
  delete iBgContext;


  iPrimaryZoomInKeys.Close();
  iPrimaryZoomOutKeys.Close();
  iSecondaryZoomInKeys.Close();
  iSecondaryZoomOutKeys.Close();
  
  iPrimaryCameraCaptureKeys.Close();
  iSecondaryCameraCaptureKeys.Close();
  
  iPrimaryCameraAFKeys.Close();

  delete iProcessingText;
    delete iBurstProcessingText;
  if(iBackupContainer)
      {
      delete iBackupContainer;
      iBackupContainer = NULL;
      }
  
  PRINT( _L("Camera <= ~CCamContainerBase") );
  }
  
// ---------------------------------------------------------------------------
// CCamContainerBase::CCamContainerBase
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamContainerBase::CCamContainerBase( CCamAppController& aController,
                                      CAknView&          aView       )
  : iView( aView ), 
    iController( aController )
  {
  }

// ---------------------------------------------------------
// CCamContainerBase::BaseConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamContainerBase::BaseConstructL( const TRect& aRect )
  {
  CreateWindowL();
  Window().SetBackgroundColor( KRgbBlack );
  SetRect( aRect );
  EnableDragEvents(); 

  CCamAppUiBase* appUi =  static_cast<CCamAppUiBase*>( iEikonEnv->EikAppUi() );
  iCamOrientation = appUi->CamOrientation();

  // set up the skin background context
  CreateBackgroundContextL();
  
  iBackupContainer = CCamBackupContainer::NewL(iController,iView,*this);
  
  // zoom keys
  iPrimaryZoomInKeys.Reset();
  iPrimaryZoomOutKeys.Reset();
  iSecondaryZoomInKeys.Reset();
  iSecondaryZoomOutKeys.Reset();
  
  // capture keys
  iPrimaryCameraCaptureKeys.Reset();
  iSecondaryCameraCaptureKeys.Reset();
  
  // AF keys
  iPrimaryCameraAFKeys.Reset();
  
  if ( iController.UiConfigManagerPtr() )
      {
      // Reading the zoom keys for primary and secondary camera
      iController.UiConfigManagerPtr()->
          SupportedPrimaryCameraZoomInKeysL( iPrimaryZoomInKeys );
      iController.UiConfigManagerPtr()->
                SupportedPrimaryCameraZoomOutKeysL( iPrimaryZoomOutKeys );
      
      // secondary camera zoom-in/out keys
      iController.UiConfigManagerPtr()->
                SupportedSecondaryCameraZoomInKeysL( iSecondaryZoomInKeys );
      iController.UiConfigManagerPtr()->
                SupportedSecondaryCameraZoomOutKeysL( iSecondaryZoomOutKeys );
      
      // Reading the capture keys for primary and secondary camera
      iController.UiConfigManagerPtr()->
                SupportedPrimaryCameraCaptureKeyL( iPrimaryCameraCaptureKeys );
      
      iController.UiConfigManagerPtr()->
                      SupportedSecondaryCameraCaptureKeyL( iSecondaryCameraCaptureKeys );
      
      // Reading AF or half capture key press  Keys
      iController.UiConfigManagerPtr()->SupportedPrimaryCameraAutoFocusKeyL(
                                                       iPrimaryCameraAFKeys );
      }
  if ( iPrimaryZoomInKeys.Count() <= 0 || iPrimaryZoomOutKeys.Count() <= 0 ||
       iSecondaryZoomInKeys.Count() <= 0 || iSecondaryZoomOutKeys.Count() <= 0 ||
       iPrimaryCameraCaptureKeys.Count() <= 0 || iSecondaryCameraCaptureKeys.Count() <= 0 ||       
       iPrimaryCameraAFKeys.Count() <= 0
      )
      {
      // Leave here, if there are no configured items so that we dont panic 
      //in any container classes derived from this base class
      User::Leave( KErrNotSupported );
      }
    }
    

// ---------------------------------------------------------
// CCamContainerBase::IsCaptureKeyL
// Whether or not this key is considered to be a capture key
// in the current context
// ---------------------------------------------------------
//
TBool CCamContainerBase::IsCaptureKeyL( const TKeyEvent& aKeyEvent,
                                       TEventCode aType ) 
  {
  PRINT1( _L( "Camera => CCamContainerBase::IsCaptureKey event type (%d)" ),  aType ); 
  
  // Find out if this key is a capture key
  TBool isCaptureKey( EFalse) ;
  
  // Repeat events are not classed as capture keys
  if ( aKeyEvent.iRepeats != 0 )
    {
    PRINT( _L( "IsCaptureKey returning false (repeat)" ) );
    return isCaptureKey;
    }
  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  if ( appUi && appUi->AppInBackground( ETrue ) )
      {
      PRINT( _L("IsCaptureKey returning false (AppInBackground() = true") );
      return isCaptureKey;
      }
  
  if ( ECamActiveCameraPrimary == iController.ActiveCamera() )
      {
      isCaptureKey = IsPrimaryCameraCaptureKey( aKeyEvent );
      }
  else // secondary camera
      {
      isCaptureKey = IsSecondaryCameraCaptureKey( aKeyEvent );
      }  

  // Each product defines which key/s are treated as capture keys.
  // Different keys can be mapped for each active camera.
  // Look for capture event or key up event (note that capture
  // event is on the EEventKey for navi-key select press)
  if ( isCaptureKey )
      {
      if ( aType == EEventKeyDown || aType == EEventKey || aType == EEventKeyUp || aType == EEventKeyUp )
        {
        if ( isCaptureKey &&
             ( iController.UiConfigManagerPtr()->IsKeyLockWatcherSupported() && iController.IsKeyLockOn() ) )
            {
            isCaptureKey = !iController.IsKeyLockOn();
            }
        }
      else
        {
        // key types doesnt match so dont consider as capture key press.
        isCaptureKey = EFalse;
        }  
      }
  PRINT1( _L( "IsCaptureKey returning %d" ), isCaptureKey );
  return isCaptureKey;
  }

// ---------------------------------------------------------
// CCamContainerBase::IsShutterKeyL
// Whether or not this key is considered to be a shutter key
// in the current context
// ---------------------------------------------------------
//
TBool CCamContainerBase::IsShutterKeyL( const TKeyEvent& aKeyEvent, TEventCode /* aType */ )
    {
    PRINT1( _L( "Camera => CCamContainerBase::IsShutterKeyL scancode (%d)" ),  aKeyEvent.iScanCode ); 

    if ( iController.UiConfigManagerPtr()
        && iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
#ifndef __WINS__
        if ( aKeyEvent.iScanCode == iPrimaryCameraAFKeys[0] )
#else
        if ( aKeyEvent.iScanCode == EProductKeyCaptureHalf )
#endif 
            {
            return ETrue;
            }
        }
    return EFalse;
    }


//
// CCamZoomPane::CheckForZoomKey
//
TBool CCamContainerBase::CheckForZoomKey( const TKeyEvent& aKeyEvent )
    {
    return CheckInZoomKeyArray( aKeyEvent );
    }


//
//
//
TBool CCamContainerBase::CheckInZoomKeyArray( const TKeyEvent& aKeyEvent )
    {
    TCamActiveCamera camera = iController.ActiveCamera();
    TBool zoomKeyFound ( EFalse );

    if ( camera == ECamActiveCameraPrimary )
        {
        zoomKeyFound = ( KErrNotFound != 
                         iPrimaryZoomInKeys.Find( aKeyEvent.iScanCode ) );
        if ( !zoomKeyFound )
            {
            // Its not zoom-in key, check if its zoom out key
            zoomKeyFound = ( KErrNotFound != 
                             iPrimaryZoomOutKeys.Find( aKeyEvent.iScanCode ) );
            }
        }
    else if ( camera == ECamActiveCameraSecondary )
        {
        zoomKeyFound = ( KErrNotFound != 
                         iSecondaryZoomInKeys.Find( aKeyEvent.iScanCode ) );
        if ( !zoomKeyFound )
            {
            // Its not zoom-in key, check if its zoom out key
            zoomKeyFound = ( KErrNotFound != 
                             iSecondaryZoomOutKeys.Find( aKeyEvent.iScanCode ) );
            }        
        }
    else
        {
        // when we reach here, it means we have no zoom keys set, we return back.
        }
    return zoomKeyFound;
    }


// ---------------------------------------------------------
// CCamContainerBase::IsZoomKeyL
// Whether or not this key is considered to be a shutter key
// in the current context
// ---------------------------------------------------------
//
TBool CCamContainerBase::IsZoomKeyL( const TKeyEvent& aKeyEvent,
                                        TEventCode aType ) 
    {
    PRINT1( _L( "Camera => CCamContainerBase::IsZoomKeyL event type (%d)" ),  aType );  

    CCamZoomPane* zoom = static_cast<CCamAppUiBase*>( iEikonEnv->EikAppUi() )->ZoomPane();    	
    
    // 
    if( !ZoomAvailable() )
    	{
    	return EFalse;
    	}
  	
  	PRINT( _L( "Camera => CCamContainerBase::IsZoomKeyL " ) );  

    // It's a possible zoom key if it's an up OR down event, OR
    // if it's a key event AND we're currently zooming 
    if ( aType == EEventKeyUp || aType == EEventKeyDown || aType == EEventUser ||
         ( aType == EEventKey && 
           zoom &&
           zoom->IsCurrentlyZooming() ) )
        {
        PRINT( _L( "Camera => CCamContainerBase::IsZoomKeyL A" ) )
        return CheckForZoomKey( aKeyEvent );
        }
	     
	PRINT( _L( "Camera => CCamContainerBase::IsZoomKeyL no" ) )
    return EFalse;
    }


//
// CCamContainerBase::ZoomAvailable()
//
TBool CCamContainerBase::ZoomAvailable()
	{
	PRINT( _L("Camera => CCamContainerBase::ZoomAvailable"))

	// Ignore any zoom keys that come when either capturing a burst or still
  if ( iController.SequenceCaptureInProgress() 
    || ( ECamControllerImage == iController.CurrentMode()
      && ECamCapturing       == iController.CurrentOperation() ) 
    || ECamCompleting == iController.CurrentOperation()  )
    {
    PRINT( _L("Camera => CCamContainerBase::ZoomAvailable A false"))
    return EFalse;
    }
	PRINT( _L("Camera => CCamContainerBase::ZoomAvailable yes"))
	return ETrue;
	}

// ---------------------------------------------------------------------------
// Window
// ---------------------------------------------------------------------------
//
RWindow& 
CCamContainerBase::Window() const
  {
  return CCoeControl::Window();
  }


// ----------------------------------------------------
// CCamContainerBase::OfferKeyEventL
// Handles this application view's command keys. Forwards other
// keys to child control(s).
// ----------------------------------------------------
//
TKeyResponse 
CCamContainerBase::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                         TEventCode aType )
  {
  PRINT1( _L("Camera => CCamContainerBase::OfferKeyEventL scan code (%d)"), aKeyEvent.iScanCode )
  // if the cancel softkey is pressed
  if ( aKeyEvent.iScanCode == EStdKeyDevice1 )
    {        
    iView.HandleCommandL( EAknSoftkeyCancel );
    }
  // or if the select (OK) key is pressed
  //
  // (Note: intentionally ignoring repeat key presses here - this is a
  // work-around as the framework is passing navi-key select repeats
  // which we have to ignore)
  else if ( aType == EEventKey && 
            aKeyEvent.iCode == EKeyOK && 
            aKeyEvent.iRepeats == 0 )
    {
    iView.HandleCommandL( EAknSoftkeySelect );
    return EKeyWasConsumed; // indicate key press was processed
    }
    // stop video or sequence if get applications key press (short or long)
    // in touch also send key stops
  else if( aKeyEvent.iCode == EKeyApplication ||
         ( iController.IsTouchScreenSupported() && 
           aKeyEvent.iScanCode == EStdKeyYes ) ) 
  	{
    if ( ECamControllerVideo == iController.CurrentMode() )
      {
      TCamCaptureOperation operation = iController.CurrentOperation();
      if( ECamCapturing == operation
       || ECamPaused    == operation )
        {
        // Stop video 
        iController.StopVideoRecording();
        }
      }
    else if ( iController.SequenceCaptureInProgress() )
      {
      // Stop sequence capture
      iController.StopSequenceCaptureL();
      }
    else
    	{
    	// empty statement to remove Lint error, MISRA required rule 60
    	}
  	}
  else
    {
    // empty statement to remove Lint error, MISRA required rule 60
    }

  return EKeyWasNotConsumed;  // indicate key press was not processed
  }

// ----------------------------------------------------
// CCamContainerBase::HandleForegroundEventL
// Called to notify of change of app fore/background state
// ----------------------------------------------------
//
void CCamContainerBase::HandleForegroundEventL( TBool /*aForeground*/ )
    {
    }

// ---------------------------------------------------------------------------
// CCamContainerBase::MopSupplyObject
// Gets an object whose type is encapsulated by the specified TTypeUid object
// Supplies the skin context to child controls using the MObjectProvider chain
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CCamContainerBase::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId )
        {
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }

// ----------------------------------------------------
// CCamContainerBase::HandleResourceChange
// Change Client Rect and redraw when resolution changes
// ----------------------------------------------------
//
void CCamContainerBase::HandleResourceChange( TInt aType )
    {
    PRINT( _L("Camera => CCamContainerBase::HandleResourceChange" ))
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->EikAppUi() );
    if ( KEikDynamicLayoutVariantSwitch == aType )
        {
	    TCamOrientation orientation = appUi->CamOrientation();

        if( iCamOrientation != orientation )
            {
            iCamOrientation = orientation;
            if( orientation == ECamOrientationCamcorder || orientation == ECamOrientationCamcorderLeft )
                {
                PRINT( _L("Camera => CCamContainerBase::HandleResourceChange camcorder" ))
                SetRect( appUi->ApplicationRect() );
                }
            else
                {
                PRINT( _L("Camera => CCamContainerBase::HandleResourceChange not camcorder" ))
                SetRect( appUi->ClientRect() );
                }
            }
        }
            
    CCoeControl::HandleResourceChange( aType );
    if ( iController.IsTouchScreenSupported() ) 
        {
        // Update toolbar because HandleResourceChange might reload the default version
        // Only update toolbaricons if in precaptureview.
        if( appUi->CurrentViewState() != ECamViewStatePostCapture )
            {
            TRAP_IGNORE( static_cast<CCamViewBase&>( iView ).UpdateToolbarIconsL() );
            }
        }  
    }


// virtual 
void 
CCamContainerBase::HandleAppEvent( const TCamAppEvent& /*aEvent*/ )
  {
  // No action
  }


// ---------------------------------------------------------------------------
// CCamContainerBase::SetPreviousViewId
// Sets the ID of the previous view
// ---------------------------------------------------------------------------
//
void CCamContainerBase::SetPreviousViewId( TCamAppViewIds aId )
    {
    iPrevViewId = aId;
    }


// ---------------------------------------------------------
// CCamContainerBase::SetupNaviPanesL
// Set up navi-pane and progress bar if required
// ---------------------------------------------------------
//    
void CCamContainerBase::SetupNaviPanesL( TCamActiveNaviPane aNaviPane )
	{
	PRINT( _L("Camera => CCamContainerBase::SetupNaviPanesL" ))
	TRect rect( Rect() );
	
	if( aNaviPane == ECamPaneCounter )
		{
        iNaviCounterControl = 
            static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->NaviCounterControl();
	    iNaviCounterControl->SetMopParent( this );
	    iNaviCounterControl->SetContainerWindowL( *this );  
	    iNaviCounterControl->SetRect( Rect() );            
	    iNaviCounterControl->MakeVisible( ETrue );
        if ( iNaviProgressBarControl )
            {
            iNaviProgressBarControl->MakeVisible( EFalse );
            }

	    iPaneInUse = ECamPaneCounter;
		}
	else if( aNaviPane == ECamPaneProgress )
		{
		PRINT( _L("Camera => CCamContainerBase::SetupNaviPanesL progress" ))
        iNaviProgressBarControl = 
            static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->NaviProgressBarControl();
	    iNaviProgressBarControl->SetMopParent( this );
	    iNaviProgressBarControl->SetContainerWindowL( *this );            
	    iNaviProgressBarControl->SetRect( Rect() );            
	    iNaviProgressBarControl->MakeVisible( ETrue );    
        if ( iNaviCounterControl )
            {
            iNaviCounterControl->MakeVisible( EFalse );
            }

	    iPaneInUse = ECamPaneProgress;
		}
	else
		{
		iPaneInUse = ECamPaneUndefined;
		}
	PRINT( _L("Camera <= CCamContainerBase::SetupNaviPanesL" ))
	}

// ---------------------------------------------------------------------------
// CCamContainerBase::ShowZoomPaneWithTimer
// Makes Zoom Pane visible for the period of the pane timer.
// ---------------------------------------------------------------------------
//
void CCamContainerBase::ShowZoomPaneWithTimer()
    {
    // No implementation: only meaningful in pre-capture modes
    PRINT(_L("Camera => CCamContainerBase::ShowZoomPaneWithTimer() - No implementation!"));
    }

// ---------------------------------------------------------------------------
// CCamContainerBase::CreateActivePaletteL
// Creates the Active Palette control
// ---------------------------------------------------------------------------
//
void CCamContainerBase::CreateActivePaletteL()
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    appUi->APHandler()->CreateActivePaletteL( *this, this );
    iActivePalette = appUi->ActivePalette();
    }

// ---------------------------------------------------------------------------
// CCamContainerBase::DestroyActivePaletteL
// Destroys the Active Palette control
// ---------------------------------------------------------------------------
//
void CCamContainerBase::DestroyActivePalette()
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    appUi->APHandler()->DestroyActivePalette();
    iActivePalette = NULL;
    }

// -----------------------------------------------------------------------------
// CCamContainerBase::Redraw
// -----------------------------------------------------------------------------
void CCamContainerBase::Redraw(const TRect& /*aArea*/)
    {
    // Do nothing by default; may be overridden
    }
   	


// -----------------------------------------------------------------------------
// CCamContainerBase::PrepareProcessingTextL
// -----------------------------------------------------------------------------
//   
void CCamContainerBase::PrepareProcessingTextL( TBool aStillMode )
    { 
    TAknLayoutRect vidProgressPane;
    
    TInt cba =  AknLayoutUtils::CbaLocation() == 
                AknLayoutUtils::EAknCbaLocationLeft? 
                AknLayoutUtils::EAknCbaLocationLeft : 0;
    
    if ( CamUtility::IsNhdDevice() )
        {
        vidProgressPane.LayoutRect( Rect(),
                AknLayoutScalable_Apps::vid4_progress_pane( 
                Layout_Meta_Data::IsLandscapeOrientation() ) );
                
        iProcessingTextLayout.LayoutText( vidProgressPane.Rect(), 
                AknLayoutScalable_Apps::vid4_progress_pane_t3());  
        }
    else
        {
        vidProgressPane.LayoutRect( Rect(), 
                AknLayoutScalable_Apps::vid6_indi_pane( cba ) ); 
                
        iProcessingTextLayout.LayoutText( vidProgressPane.Rect(), 
                AknLayoutScalable_Apps::vid6_indi_pane_t4( 2 ));  
        }
        
    // Delete old text 
    if( iProcessingText )
        {
        delete iProcessingText;
        iProcessingText = NULL;  
        }
    if( aStillMode )
        {
        iProcessingText = StringLoader::LoadL( R_QTN_PROCESSING_IMAGE_TEXT );  
        }        
/*        
    else
        {
        iProcessingText = StringLoader::LoadL( R_QTN_PROCESSING_VIDEO_TEXT );  
        }                    
*/        
    }
    
// -----------------------------------------------------------------------------
// CCamContainerBase::DismissProcessingTextL
// -----------------------------------------------------------------------------
//     
void CCamContainerBase::DismissProcessingText()
    {
    delete iProcessingText;
    iProcessingText = NULL;
    }    
    
// -----------------------------------------------------------------------------
// CCamContainerBase::PrepareBurstProcessingTextL
// -----------------------------------------------------------------------------
//   
void CCamContainerBase::PrepareBurstProcessingTextL()
    {
    PRINT( _L("Camera => CCamContainerBase::PrepareBurstProcessingTextL" ))	
    TAknLayoutRect vidProgressPane;  
    
    TInt cba =  AknLayoutUtils::CbaLocation() == 
                AknLayoutUtils::EAknCbaLocationLeft? 
                AknLayoutUtils::EAknCbaLocationLeft : 0;
    
    
    if ( CamUtility::IsNhdDevice() )
        {
        vidProgressPane.LayoutRect(
                Rect(),
                AknLayoutScalable_Apps::vid4_progress_pane( 
                        Layout_Meta_Data::IsLandscapeOrientation() ) );

        iBurstProcessingTextLayout.LayoutText( 
                vidProgressPane.Rect(), 
                AknLayoutScalable_Apps::vid4_progress_pane_t3() );  
        }
    else
        {
        
        vidProgressPane.LayoutRect(
                Rect(),
                AknLayoutScalable_Apps::vid6_indi_pane( cba ) );
                
        iBurstProcessingTextLayout.LayoutText( 
                vidProgressPane.Rect(), 
                AknLayoutScalable_Apps::vid6_indi_pane_t4( 2 ) );
        
        }
    
    // Delete old text 
    if( iBurstProcessingText )
        {
        delete iBurstProcessingText;
        iBurstProcessingText = NULL;  
        }
    iBurstProcessingText = StringLoader::LoadL( 
                           R_QTN_PROCESSING_IMAGE_TEXT );
    PRINT( _L("Camera <= CCamContainerBase::PrepareBurstProcessingTextL" ))	                       
    }
 
// -----------------------------------------------------------------------------
// CCamContainerBase::UpdateBurstProcessingTextL
// -----------------------------------------------------------------------------
//   
void CCamContainerBase::UpdateBurstProcessingTextL( TInt aHandled, TInt aTotal )
    {
    PRINT( _L("Camera => CCamContainerBase::UpdateBurstProcessingTextL" ))		

    // Delete old text 
    if( iBurstProcessingText )
        {
        delete iBurstProcessingText;
        iBurstProcessingText = NULL;  
        }
    CDesCArrayFlat* values 
    = new (ELeave) CDesCArrayFlat( 2 );

    CleanupStack::PushL( values );
    
    TBuf<KCamBurstCounterLength> handled;
    TBuf<KCamBurstCounterLength> total;
    handled.Format( KCamBurstCounterFormat, aHandled );
    total.Format( KCamBurstCounterFormat, aTotal );
    
    values->AppendL( handled );
    values->AppendL( total );

#ifndef __WINS__
    iBurstProcessingText = StringLoader::LoadL( 
            R_QTN_PROCESSING_BURST_IMAGE_TEXT, *values );
#endif
    
    CleanupStack::PopAndDestroy( values );                           
    PRINT( _L("Camera <= CCamContainerBase::UpdateBurstProcessingTextL" ))		                           
    }
    
// -----------------------------------------------------------------------------
// CCamContainerBase::DismissBurstProcessingText
// -----------------------------------------------------------------------------
//     
void CCamContainerBase::DismissBurstProcessingText()
    {
    delete iBurstProcessingText;
    iBurstProcessingText = NULL;
    }    
        
// ----------------------------------------------------
// CCamContainerBase::CheckForFileName
// 
// ----------------------------------------------------
//
void CCamContainerBase::CheckForFileName( TBool aDoCheck )
    {
    }

// -----------------------------------------------------------------------------
// CCamContainerBase::CreateBackgroundContextL
// -----------------------------------------------------------------------------
//     
void CCamContainerBase::CreateBackgroundContextL()
    {
    if( iCamOrientation == ECamOrientationCamcorder ||
        iCamOrientation == ECamOrientationCamcorderLeft )
        {
        iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgScreen, Rect(), EFalse );
        }
    else
        {
        iBgContext = CAknsBasicBackgroundControlContext::NewL( 
            KAknsIIDQsnBgAreaMain, Rect(), EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CCamContainerBase::ViewFinderFrameRect
// -----------------------------------------------------------------------------
//     
TRect
CCamContainerBase::ViewFinderFrameRect(TCamCameraMode aTargetMode) const   
    {
    PRINT( _L("Camera => CCamContainerBase::ViewFinderFrameRect") );
    const TCamAppViewIds viewId = static_cast<TCamAppViewIds>( iView.Id().iUid );

    TCamCameraMode cameraMode = (aTargetMode != ECamControllerIdle)
                                        ? aTargetMode
                                        : iController.CurrentMode();
    
    // In post capture view both target and current mode can be in idle mode, which
    // would cause the use of invalid viewfinder layouts
    if ( viewId == ECamViewIdVideoPostCapture && cameraMode == ECamControllerIdle )
        {
        cameraMode = ECamControllerVideo;
        }
    else if ( viewId == ECamViewIdStillPostCapture && cameraMode == ECamControllerIdle  )
        {
        cameraMode = ECamControllerImage;
        }
    
    const TInt resolution = (cameraMode == ECamControllerVideo)
                                ? iController.GetCurrentVideoResolution()
                                : iController.GetCurrentImageResolution();

    TRect rect = CamUtility::ViewfinderLayout( cameraMode, resolution );
    PRINT( _L("Camera <= CCamContainerBase::ViewFinderFrameRect") );
    return rect;
    }

// -----------------------------------------------------------------------------
// CCamContainerBase::IsPrimaryCameraCaptureKey
// -----------------------------------------------------------------------------
//     
TBool CCamContainerBase::IsPrimaryCameraCaptureKey( const TKeyEvent& aKeyEvent ) const
    {
    TBool isCapturekey( KErrNotFound != iPrimaryCameraCaptureKeys.Find( aKeyEvent.iScanCode ) );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi 
         && ( !appUi->IsToolBarVisible() || !appUi->DrawPreCaptureCourtesyUI() )
         && ( aKeyEvent.iScanCode == EStdKeyDevice3
              || aKeyEvent.iScanCode == EStdKeyEnter
              || aKeyEvent.iScanCode == EStdKeyNkpEnter ) )
        {
        isCapturekey = ETrue;
        }
    return isCapturekey;
    }

// -----------------------------------------------------------------------------
// CCamContainerBase::IsSecondaryCameraCaptureKey
// -----------------------------------------------------------------------------
//     
TBool CCamContainerBase::IsSecondaryCameraCaptureKey( const TKeyEvent& aKeyEvent ) const
    {
    TBool isCapturekey( KErrNotFound != iSecondaryCameraCaptureKeys.Find( aKeyEvent.iScanCode ) );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi 
         && !appUi->IsToolBarVisible()
         && ( aKeyEvent.iScanCode == EStdKeyDevice3
              || aKeyEvent.iScanCode == EStdKeyEnter
              || aKeyEvent.iScanCode == EStdKeyNkpEnter ))
        {
        isCapturekey = ETrue;
        }
    return isCapturekey;            
    }

// -----------------------------------------------------------------------------
// CCamContainerBase::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CCamContainerBase::CountComponentControls() const
    {
    TInt index = 0;
    if(iBackupContainer)
        index++;
    return index;
    }

// -----------------------------------------------------------------------------
// CCamContainerBase::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CCamContainerBase::ComponentControl(TInt aIndex) const
    {
    CCoeControl* control = NULL;
     switch(aIndex)
         {
         case 0:
             {
             control = iBackupContainer;
             }
             break;
         default:
             break;
         }
    return control; 
    }

// ---------------------------------------------------------
// CCamCaptureSetupContainer::ReserveAndStartVF
// ---------------------------------------------------------
//
void CCamContainerBase::ReserveAndStartVF()
    {
    PRINT ( _L("Camera => CCamContainerBase::ReserveAndStartVF") );
    TCamCameraMode mode = iController.CurrentMode();
    if(iController.IsAppUiAvailable())
        {
        TVwsViewId currentViewId;
        CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
        appUi->GetActiveViewId( currentViewId );
        switch ( currentViewId.iViewUid.iUid )
            {
            case ECamViewIdPhotoUserSceneSetup:
            case ECamViewIdStillPreCapture:
                {
                mode = ECamControllerImage;
                }
                break;
            case ECamViewIdVideoPreCapture:
                {
                mode = ECamControllerVideo;
                }
                break;
            default:
                {
                //Assume imagemode if view cannot be determined.
                if(mode==ECamControllerIdle)
                    {
                    mode = ECamControllerImage;                
                    }
                }
                break;
            }
        }
    //Don't reserve camera and start viewfinder if shutting down.
    if(mode!=ECamControllerShutdown)
        {
        iController.EnterViewfinderMode( mode );
        iController.StartIdleTimer();
        }
    PRINT ( _L("Camera <= CCamContainerBase::ReserveAndStartVF") );        
    }

// End of File  
