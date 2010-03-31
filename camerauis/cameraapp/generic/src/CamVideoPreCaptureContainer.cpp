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
* Description:  Container class for video image pre-capture view*
*/

// INCLUDE FILES
#include "CamAppUiBase.h"
#include "CamVideoPreCaptureContainer.h"
#include "CamLogger.h"
#include "CamPanic.h"
#include "CamUtility.h"
#include "CamAppUi.h"
#include "CamSidePane.h"
#include "CamNaviProgressBarControl.h"
#include "CamViewBase.h"
#include "CamIndicator.h"
#include "CamIndicatorData.h"
#include "CamIndicatorResourceReader.h"
#include "CameraUiConfigManager.h"
#include "camactivepalettehandler.h"
#include <eikenv.h>
#include <cameraapp.mbg>
#include <eikappui.h>	// For CCoeAppUiBase
#include <eikapp.h>		// For CEikApplication
#include <AknIconUtils.h>
#include <akntoolbar.h>
#include <barsread.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <touchfeedback.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamVideoPreCaptureContainerTraces.h"
#endif


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamVideoPreCaptureContainer* CCamVideoPreCaptureContainer::NewL(
        CCamAppController& aController,
        CAknView& aView,
        const TRect& aRect )
    {
    CCamVideoPreCaptureContainer* self = new( ELeave ) CCamVideoPreCaptureContainer(
        aController, aView ); 
    
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self ); 
    // Return newly created CCamVideoPreCaptureContainer instance
    return self;
    }

// Destructor
CCamVideoPreCaptureContainer::~CCamVideoPreCaptureContainer()
  {
  PRINT( _L("Camera => ~CCamVideoPreCaptureContainer") );
  if ( iFeedback )
      {
      iFeedback->RemoveFeedbackForControl( this );
      }
  delete iFileTypeIndicator;
  PRINT( _L("Camera <= ~CCamVideoPreCaptureContainer") );
  }

// ---------------------------------------------------------
// CCamVideoPreCaptureContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamVideoPreCaptureContainer::ConstructL( const TRect& aRect )
    {
    PRINT( _L("Camera => CCamVideoPreCaptureContainer::ConstructL"))
	PRINT( _L("Camera => CCamVideoPreCaptureContainer::ConstructL C"))
	TCamVideoResolution res = iController.GetCurrentVideoResolution();
	PRINT( _L("Camera => CCamVideoPreCaptureContainer::ConstructL D"))
	iVFRes = iController.VideoViewFinderResourceId( res );
	PRINT( _L("Camera => CCamVideoPreCaptureContainer::ConstructL E"))

    BaseConstructL( aRect );
    iShowReticule = EFalse;

    iSidePane->SetCaptureMode( ECamControllerVideo );

    CreateFiletypeIndicatorL();

    iRecordState = ECamNotRecording;
    
    // Get the co-ordinates where the resolution
    // icon should move to during video capture
    TAknLayoutRect layoutRect;
    TRect rect;
    TAknLayoutRect vidProgressPane;
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect );
        }
    else
        {
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        }
    if ( CamUtility::IsNhdDevice() )
        {
        TInt variant = Layout_Meta_Data::IsLandscapeOrientation();
        vidProgressPane.LayoutRect( rect,
            AknLayoutScalable_Apps::vid4_progress_pane( variant ) );  
        layoutRect.LayoutRect( vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid4_progress_pane_g1() );
        }
    else
        {
        TInt cba =  AknLayoutUtils::CbaLocation() == 
                    AknLayoutUtils::EAknCbaLocationLeft;
        vidProgressPane.LayoutRect( rect, 
            AknLayoutScalable_Apps::vid6_indi_pane( cba ) ); 
        layoutRect.LayoutRect( vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_g1( !cba ) );// quality
        }

    iResolutionIndicatorVidcapPosition = layoutRect.Rect();
    if ( !iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
        {
        static_cast<CCamPreCaptureContainerBase*>( this )
            ->SetupActivePaletteL( static_cast<CCamViewBase*>(&iView) );
        }
    
    if ( iController.IsTouchScreenSupported() )
        {
        // Get pointer of touch feedback instance 
        iFeedback = MTouchFeedback::Instance();
        if ( !iFeedback )
            iFeedback = MTouchFeedback::CreateInstanceL();
        }
    }

// ---------------------------------------------------------------------------
// CCamVideoPreCaptureContainer::CCamVideoPreCaptureContainer
// C++ constructor
// ---------------------------------------------------------------------------
//

CCamVideoPreCaptureContainer::CCamVideoPreCaptureContainer(
        CCamAppController& aController, 
        CAknView& aView )
: CCamPreCaptureContainerBase( aController, aView )        
    {
    }

// ---------------------------------------------------------
// CCamVideoPreCaptureContainer::HandleControllerEventL
// Receives notifications about changes in recording state from the controller
// ---------------------------------------------------------
//
void 
CCamVideoPreCaptureContainer
::HandleControllerEventL( TCamControllerEvent aEvent, TInt aError )
    {
    PRINT1( _L("Camera => CCamVideoPreCaptureContainer::HandleControllerEventL aEvent:%d"), aEvent )	
    switch( aEvent )
      {
      // ---------------------------------------------------
      case ECamEventOperationStateChanged:
        {
        TCamCaptureOperation operation = iController.CurrentOperation();
        switch( operation )
          {
          case ECamCapturing:
            {
            if ( iController.IsTouchScreenSupported() && iFeedback )
                {
                iFeedback->EnableFeedbackForControl( this, EFalse );
                }
            iRecordState = ECamRecording;
            iResolutionIndicators[iCurrentIndicator]->SetRect(iResolutionIndicatorVidcapPosition);
            break;
            }
          case ECamPaused:
            {
            iRecordState = ECamRecordPaused;
            break;
            }
          default:
            {
            iRecordState = ECamNotRecording;
            break;
            }
          }
        break;
        }
      // ---------------------------------------------------
      case ECamEventLocationSettingChanged:
      {
      // If the location setting was changed on, then the indicator should be visible
      SetLocationIndicatorVisibility();
      }
      // ---------------------------------------------------
      case ECamEventCameraChanged:
        {
        SetFileTypeIndicator();             
        break;
        }
      // ---------------------------------------------------
      case ECamEventVideoFileTypeChanged:
        {
        SetResolutionIndicator();
        SetFileTypeIndicator();
        break;
        }
      // ---------------------------------------------------
      case ECamEventVideoQualityChanged:
	    {
	    // To get correct viewfinder resolution:
		// TCamVideoResolution res = iController.GetCurrentVideoResolution();
		// iVFRes = iController.VideoViewFinderResourceId( res );
	    SetResolutionIndicator();
		}
      // ---------------------------------------------------
      case ECamEventRecordComplete:    
        {
        iResolutionIndicators[iCurrentIndicator]->SetRect( ResolutionIndicatorRect() );
        iFileTypeIndicator->SetRect( iFileTypeIndicatorPosition );
        if ( iController.IsTouchScreenSupported() && iFeedback )
            {
            iFeedback->EnableFeedbackForControl( this, ETrue );
            }
        break;
        }
    // ---------------------------------------------------
    case ECamEventInitReady:
        {
        // event only sent if UIOrientationOverride feature is supported
        if ( !iController.IsTouchScreenSupported() && 
             iController.CurrentMode()== ECamControllerVideo )
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
            if ( appUi )
                {
                if ( !appUi->ActivePalette() )
                    {
                    OstTrace0( CAMERAAPP_PERFORMANCE, CCAMVIDEOPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_SETUP 1" );
                    static_cast<CCamPreCaptureContainerBase*>( this )
                        ->SetupActivePaletteL( static_cast<CCamViewBase*>(&iView) );
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMVIDEOPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_SETUP 0" );
                    }
                else 
                    {
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMVIDEOPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_UPDATE 1" );
                    appUi->APHandler()->UpdateActivePaletteL();
                    OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMVIDEOPRECAPTURECONTAINER_HANDLECONTROLLEREVENTL, "e_CAM_APP_AP_UPDATE 0" );
                    }
                appUi->SetActivePaletteVisibility( ETrue );
                }
            }
        break;    
        }            
      // ---------------------------------------------------
      default:
        {
        // otherwise, do nothing
        }        
      // ---------------------------------------------------
      }

    CCamPreCaptureContainerBase::HandleControllerEventL( aEvent, aError );
    PRINT( _L("Camera <= CCamVideoPreCaptureContainer::HandleControllerEventL"))	
    }

// ---------------------------------------------------------
// CCamVideoPreCaptureContainer::HandleForegroundEventL
// Handle events sending app to foreground and background
// ---------------------------------------------------------

void CCamVideoPreCaptureContainer::HandleForegroundEventL( TBool aForeground )
	{
    CCamPreCaptureContainerBase::HandleForegroundEventL( aForeground );
	}


// ---------------------------------------------------------
// CCamVideoPreCaptureContainer::ViewFinderLayoutResourceIds
// Return the layout resource id for the viewfinder
// (Note: reticule not required for video viewfinder)
// ---------------------------------------------------------
//
void CCamVideoPreCaptureContainer::ViewFinderLayoutResourceIds(
                                                TInt& aViewFinderLayoutId, 
                                                TInt& /*aReticuleLayoutId*/ ) const
    {
	aViewFinderLayoutId = iVFRes;			
    }
    
// ---------------------------------------------------------
// CCamVideoPreCaptureContainer::SetResolutionIndicator
// Sets the resolution indictator to the required icon
// ---------------------------------------------------------
//
void CCamVideoPreCaptureContainer::SetResolutionIndicator()
    {
    iCurrentIndicator = iController.IntegerSettingValue( ECamSettingItemVideoQuality );
    }

// ---------------------------------------------------------
// CCamStillPreCaptureContainer::SetLocationIndicatorVisibility
// Sets the resolution indicator visibility, if it is set or not.
// Do nothing if video taken.
// ---------------------------------------------------------
//
void CCamVideoPreCaptureContainer::SetLocationIndicatorVisibility()
    {
    // The setting stored is an index of whether the location setting is on or off
    iLocationIndicatorVisible = (/*TCamLocationId::ECamLocationOn*/1 == iController.IntegerSettingValue( ECamSettingItemRecLocation ));
    }
    
// ----------------------------------------------------
// CCamStillPreCaptureContainer::LocationtionIndicatorRect
// Returns the rectangle defining the position and size
// of the location icon
// ----------------------------------------------------
//
TRect CCamVideoPreCaptureContainer::LocationIndicatorRect()
    {
    TAknLayoutRect layoutRect;
    //layoutRect.LayoutRect( Rect(), ROID(R_CAM_VIDEO_PRECAP_LOCATION_ID) );
    return layoutRect.Rect();
    }
    
// ----------------------------------------------------
// CCamVideoPreCaptureContainer::IncreaseFlashSettingL
// Move up through the flash settings
// ----------------------------------------------------
//
void CCamVideoPreCaptureContainer::IncreaseFlashSettingL()  
    {
    if ( iController.UiConfigManagerPtr()->IsVideoLightSupported() )
        {
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
        if ( !iZoomPane->IsVisible()
             && !appUi->IsSecondCameraEnabled() 
            )
            {
            TCamFlashId flash = 
            static_cast< TCamFlashId > 
                 ( iController.IntegerSettingValue( ECamSettingItemDynamicVideoFlash ) );       
            if ( flash == ECamFlashForced )
                {
                flash = ECamFlashOff;
                }
            else
                {
                flash = ECamFlashForced;
                }
            iController.SetIntegerSettingValueL(ECamSettingItemDynamicVideoFlash, flash);
            }        
        } // video light supported check
    }  
 
// ----------------------------------------------------
// CCamVideoPreCaptureContainer::DecreaseFlashSettingL
// Move down through the flash settings
// ----------------------------------------------------
//
void CCamVideoPreCaptureContainer::DecreaseFlashSettingL()   
    {
    //Given we have two flash modes in video, Increase and Decrease are functionally equivalent
	IncreaseFlashSettingL();
    }     
    
// ----------------------------------------------------
// CCamVideoPreCaptureContainer::HandleCaptureKeyEventL
// Change the current capture state
// ----------------------------------------------------
//  
TKeyResponse 
CCamVideoPreCaptureContainer::HandleCaptureKeyEventL( const TKeyEvent& aKeyEvent )
  {
  PRINT( _L("Camera => CCamVideoPreCaptureContainer::HandleCaptureKeyEventL") );
  TKeyResponse keyResponse = EKeyWasNotConsumed;

  // check our current operation state
  TCamCaptureOperation operation = iController.CurrentOperation();

  if ( (ECamCapturing == operation ) ||
       (ECamPaused    == operation ) )
    {
    // iController.StopVideoRecording();
    // Stop the video recording using ECamCmdStop command,
    // similarily to what happens when Stop softkey is pressed.
    iView.HandleCommandL( ECamCmdStop );
    }
        
  else  if ( iController.VideoRecordPending() )
    {
    PRINT( _L("Camera <> Video capture already pending") );
    // Do nothing if already pending
    keyResponse = EKeyWasConsumed;
    }
  else
    {
    // Blank out the softkeys if we are capturing
    if ( EKeyWasConsumed == keyResponse )
      {
      BlankSoftkeysL();
      }

    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    
    // neither recording nor paused
    // so attempt to start recording
    PRINT( _L("Camera <> starting capture") );
    keyResponse = appUi->StartCaptureL( aKeyEvent );

    // Hide the toolbar if we are capturing
    if( EKeyWasConsumed == keyResponse )
      {
      // Repeated key events (MSK) are ignored.
      iController.SetDemandKeyRelease( ETrue );  

      // fixed toolbar is used only with touch devices
      if ( iController.IsTouchScreenSupported() )
          {
          CAknToolbar* fixedToolbar = appUi->CurrentFixedToolbar();
          if ( fixedToolbar )
            {
            fixedToolbar->SetToolbarVisibility( EFalse );
            }
          }
      }
     } 
  PRINT( _L("Camera <= CCamVideoPreCaptureContainer::HandleCaptureKeyEventL") );
  return keyResponse;
  }


// ----------------------------------------------------
// virtual InitVfGridL
// ----------------------------------------------------
void 
CCamVideoPreCaptureContainer::InitVfGridL( const TRect& /*aRect*/ )
  {
  PRINT(_L("CCamVideoPreCaptureContainer::InitVfGridL -->"));
  // Skip constructing vf grid drawer.
  // As effect the vf grid is never drawn.
  PRINT(_L("CCamVideoPreCaptureContainer::InitVfGridL <--"));
  }


// ----------------------------------------------------
// CCamVideoPreCaptureContainer::BlankSoftKeysL
// Blank the softkeys
// ----------------------------------------------------
//
void CCamVideoPreCaptureContainer::BlankSoftkeysL()
    {
    static_cast<CCamViewBase&>(iView).SetSoftKeysL( R_CAM_SOFTKEYS_BLANK );
    }

// ----------------------------------------------------
// CCamVideoPreCaptureContainer::HandleShutterKeyEventL
// Change the current capture state following shutter 
// key events
// ----------------------------------------------------
//  
TKeyResponse CCamVideoPreCaptureContainer::HandleShutterKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                                                   TEventCode /*aType*/ )
    {    
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CCamVideoPreCaptureContainer::DrawAdditionalIcons
// Draw the video file type indicator
// ---------------------------------------------------------
//
void CCamVideoPreCaptureContainer::DrawAdditionalIcons(CBitmapContext& aGc) const
    {
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    TCamCaptureOperation videoOperation = iController.CurrentVideoOperation();

    // It is essential to check, that the current view state is right.
    // Draw request could still arrive to precapture view, 
    // when in fact video is stopped and transition to post capture 
    // is in progress. During that time, additional icons should
    // not be drawn.
    if( appUi
     && appUi->CurrentViewState() == ECamViewStatePreCapture
     && appUi->DrawPreCaptureCourtesyUI()
     && !appUi->IsSecondCameraEnabled() 
     && ( CamUtility::IsNhdDevice() || videoOperation == ECamNoOperation )
      )
      {
      iFileTypeIndicator->Draw( aGc );
      }
    }
    
// ---------------------------------------------------------
// CCamVideoPreCaptureContainer::CreateFiletypeIndicatorL
// Create the video file type indicator
// ---------------------------------------------------------
//
void CCamVideoPreCaptureContainer::CreateFiletypeIndicatorL()
    {
    // Create reader
    TResourceReader reader;
    iEikonEnv->CreateResourceReaderLC( reader, ROID(R_CAM_VIDEO_FILETYPE_PANE_ID));
    CCamIndicatorResourceReader* resourceReader = 
        CCamIndicatorResourceReader::NewLC( reader );

    // Get indicator data from reader (there will be just 1)
    CCamIndicatorData& indData = *( resourceReader->IndicatorData().At( 0 ) );
    iFileTypeIndicator = CCamIndicator::NewL( indData.IndicatorRect() );
    TInt numbitmaps = indData.IndicatorBitmapCount(); // no of bitmaps for indicator
    TInt j;
    for ( j = 0; j < numbitmaps; j++ )
        {
        // Side pane assumes that mask bitmap is defined after the
        // normal one in the resource file
        iFileTypeIndicator->AddIconL(
            indData.IndicatorBitmapId( j ),      // bitmap
            indData.IndicatorBitmapId( j + 1 )); // mask
        j++;
        }

    CleanupStack::PopAndDestroy( resourceReader );
    CleanupStack::PopAndDestroy(); // reader

    TAknLayoutRect layoutRect;
    TRect rect;
    TAknLayoutRect vidProgressPane;
    TAknLayoutRect fileTypeIconLayout;
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect );
        }
    else
        {
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, rect );
        }
    if ( CamUtility::IsNhdDevice() )
        {
        TInt variant = Layout_Meta_Data::IsLandscapeOrientation();
        vidProgressPane.LayoutRect( rect,
            AknLayoutScalable_Apps::vid4_progress_pane( variant ) );  
        layoutRect.LayoutRect( vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid4_progress_pane_g2() );
        }
    else
        {
        TInt cba =  AknLayoutUtils::CbaLocation() == 
                    AknLayoutUtils::EAknCbaLocationLeft;
        vidProgressPane.LayoutRect( rect, 
            AknLayoutScalable_Apps::vid6_indi_pane( cba ) ); 
        layoutRect.LayoutRect( vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_g2( cba ) );// type 
        fileTypeIconLayout = layoutRect;
        }

    iFileTypeIndicator->SetRect( layoutRect.Rect() );
    iFileTypeIndicatorPosition = layoutRect.Rect();
    // Initialise the indicator
    SetFileTypeIndicator();
    }

// ----------------------------------------------------
// CCamVideoPreCaptureContainer::SetFileTypeIndicator
// Sets the video file type indicator depending on the current 
// video file type setting.
// ----------------------------------------------------
//
void CCamVideoPreCaptureContainer::SetFileTypeIndicator()
    {
    if ( iFileTypeIndicator )
        {
        // If the current mode is video then show the relevant icon
        // for the current video file type.
        TCamVideoFileType fileType = static_cast< TCamVideoFileType > 
            ( iController.IntegerSettingValue( ECamSettingItemVideoFileType ) );
        if ( fileType == ECamVideoMpeg4 )
            {
            iFileTypeIndicator->SetIcon( 0 );
            }

        // Otherwise display the H263 icon.
        else
            {
            iFileTypeIndicator->SetIcon( 2 );
            }       
        }
    }
   
// ----------------------------------------------------
// CCamVideoPreCaptureContainer::ResolutionIndicatorIconPsiKey
// Returns the PSI key relating to the array of resolution
// indicator bitmaps.
// ----------------------------------------------------
//
TCamPsiKey CCamVideoPreCaptureContainer::ResolutionIndicatorIconPsiKey() const
    {
    return ECamPsiPrecapVideoQualityIconIds;
    }



// ---------------------------------------------------------------------------
// virtual GetAPResourceId 
// (From CCamPreCaptureContainerBase)
// 
// Helper method to select the AP resource used to set AP items.
// ---------------------------------------------------------------------------
//
TInt CCamVideoPreCaptureContainer::GetAPResourceId() const
    {    
    CCamAppUi* appUi =  static_cast<CCamAppUi*>( iEikonEnv->AppUi() );
    TBool embedded   = appUi->IsEmbedded();
    TInt resId = 0;
    if( embedded )
        {
        if ( !appUi->IsSecondCameraEnabled() )
            {
            resId = R_CAM_EMB_VIDEO_PRE_CAPTURE_AP_ITEMS_ADVANCED;
            }
        }
    else
        {
        if ( !appUi->IsSecondCameraEnabled() )
            {
            resId = R_CAM_VIDEO_PRE_CAPTURE_AP_ITEMS_ADVANCED;
            }
        else
            {
            resId = R_CAM_VIDEO_PRE_CAPTURE_AP_ITEMS_SECONDARY;
            }
        }
    return resId;
    }

// End of File  
