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
* Description:  Control for displaying remaining images/videos in Navi Pane
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/


// INCLUDE FILES
#include <eiklabel.h>
#include <eikapp.h>   // For CEikApplication
#include <eikenv.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <aknconsts.h>
#include <StringLoader.h> // StringLoader
#include <e32base.h>
#include <barsread.h>    // TResourceReader
#include <AknBidiTextUtils.h>
#include <cameraapp.mbg>
#include <AknLayoutFont.h>
#include <AknsFrameBackgroundControlContext.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknnavide.h>
#include <layoutmetadata.cdl.h>

#include "CamNaviCounterModel.h"
#include "CamAppUi.h"
#include "CamUtility.h"
#include "CamSelfTimer.h"

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamLogger.h"
#include "CamDecorator.h"
#include "camtextitem.h"
#include "CameraUiConfigManager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamNaviCounterModelTraces.h"
#endif


// CONSTANTS
const TInt KMaxRemainingImagesShown = 9999; 
const TInt KCamDefaultCapturedImagesTextLen = 30;
_LIT( KNumberFormat, "%04d" );
_LIT( KTimeLapseImageFormat, "%04d/%04d" );
    
const TAknsItemID KNaviPaneMajorColour = KAknsIIDQsnComponentColors;
const TInt KNaviPaneMinorColour = EAknsCIQsnComponentColorsCG2;

// Sequence mode related constants that define the amount of pictures taken
// with sequence mode.
const TInt KShortBurstCount  = 18;   // number of images captured during burst

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------
// CCamNaviCounterModel::NewL
// Factory construction function
// ---------------------------------------------------------
//
CCamNaviCounterModel* 
CCamNaviCounterModel::NewL( CCamAppController& aController )
  {
  CCamNaviCounterModel* self = 
      new( ELeave ) CCamNaviCounterModel( aController );
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );
  return self;
  }

// Destructor
CCamNaviCounterModel::~CCamNaviCounterModel()
  {
  iController.RemoveControllerObserver( this );
  
  delete iVideoTextItem;
  delete iPhotoTextItem;

  delete iSequenceImageTextItem;
  delete iSequenceCapturedTextItem;
  delete iTimeLapseCountdownTextItem;

  CCamNaviCounterModel::UnloadResourceData();
  
  delete iMpeg4Icon;
  delete iMpeg4IconMask;
  delete i3GPIcon;
  delete i3GPIconMask;
  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi());
  if ( appUi )
    {
    if ( !iController.IsInShutdownMode() )
      {
      appUi->RemoveBurstModeObserver( this );
      }
    delete iSequenceIcon;
    delete iSequenceMask;
    }

  delete iSelfTimerIcon;
  delete iSelfTimerMask;
  
  // If not shutting down, deregister from SelfTimer observers.
  // If we try to deregister when shutting down, the object has already been 
  // deleted
  if ( !iController.IsInShutdownMode() && appUi )
    {
    // ...Remove self as an observer of the self timer.    
    CCamSelfTimer* selftimer = appUi->SelfTimer();
    if ( selftimer )
      {
      selftimer->RemoveObserver( this );
      }            
    }
    
#ifdef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
  delete iGenericIcon;
  delete iGenericMask;    
#endif // PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR

  delete iAudioMuteIcon;
  delete iAudioMuteMask;
    
  
#ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE      
  delete iSubtitlePhotoIcon;
  delete iSubtitlePhotoMask;
  delete iSubtitleVideoIcon;
  delete iSubtitleVideoMask;
#endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE

  delete iTimeFormat;
  delete iCountDownText;
  delete iObserverHandler;
  }

// -----------------------------------------------------------------------------
// CCamNaviCounterModel::LoadResourceDataL()
// Reads in all information needed from resources
// -----------------------------------------------------------------------------
//
void CCamNaviCounterModel::LoadResourceDataL()
    {
OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMNAVICOUNTERMODEL_LOADRESOURCEDATAL, "e_CCamNaviCounterModel_LoadResourceDataL 1" );

    PRINT( _L("Camera => CCamNaviCounterModel::LoadResourceDataL") );
    
    // Read the time format resource
    PRINT( _L("Camera <> construct timeformat..") );
    delete iTimeFormat;
    iTimeFormat = NULL;
    if ( !Layout_Meta_Data::IsLandscapeOrientation() ) 
        {
        iTimeFormat = CEikonEnv::Static()->AllocReadResourceL( R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO );
        }
    else 
        {
        iTimeFormat = CEikonEnv::Static()->AllocReadResourceL( R_QTN_TIME_DURAT_LONG );
        }    

    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    
    delete iPhotoTextItem;
    iPhotoTextItem = NULL;
    iPhotoTextItem = CCamTextItem::NewL();

    delete iVideoTextItem;
    iVideoTextItem = NULL;
    iVideoTextItem = CCamTextItem::NewL();

    delete iSequenceImageTextItem;
    iSequenceImageTextItem = NULL;   
    iSequenceImageTextItem = CCamTextItem::NewL();
    
    delete iTimeLapseCountdownTextItem;
    iTimeLapseCountdownTextItem = NULL;
    iTimeLapseCountdownTextItem = CCamTextItem::NewL();
    
    delete iSequenceCapturedTextItem;
    iSequenceCapturedTextItem = NULL;
    iSequenceCapturedTextItem = CCamTextItem::NewL();   
        
    iCamOrientation = appUi->CamOrientation();

    if ( Layout_Meta_Data::IsLandscapeOrientation() || CamUtility::IsNhdDevice() )
        {
        TSize screenSize;
        AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, screenSize ); 
        iExtent = TRect( TPoint(), screenSize );
        }
    else
        {
        TRect screenRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
        iExtent = screenRect;
        }

    if ( CamUtility::IsNhdDevice() )
        {
        TouchLayoutL();
        }
    else
        {
        if ( !Layout_Meta_Data::IsLandscapeOrientation() )
            {
            NonTouchLayoutSecondaryL();
            }
        else
            {
            NonTouchLayoutL();
            }
        }
    
    iVidPostStorageIconRect = iVidPreStorageIconRect;
    

    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsLocationSupported() )
        {
        iLocationIconVisible = (/*TCamLocationId::ECamLocationOn*/1 == iController.IntegerSettingValue( ECamSettingItemRecLocation ));
        // If location setting is on, then the decoration should be wider to fit the location icon
        if ( iLocationIconVisible )
            {
            iPhotoPrecapDecorator = CCamDecorator::NewL( iController, R_CAM_STILL_PRECAP_NAVICOUNTER_DECORATIONS_CAMCORDER_LOCATION );
            }
        else
            {
            iPhotoPrecapDecorator = CCamDecorator::NewL( iController, R_CAM_STILL_PRECAP_NAVICOUNTER_DECORATIONS_CAMCORDER );
            }
        }
    else
        {
        iPhotoPrecapDecorator = CCamDecorator::NewL( iController, R_CAM_STILL_PRECAP_NAVICOUNTER_DECORATIONS_CAMCORDER );
        }
    
    iVideoPrecapDecorator = CCamDecorator::NewL( iController, R_CAM_VIDEO_PRECAP_NAVICOUNTER_DECORATIONS_CAMCORDER );
   
    iPhotoPostcapDecorator = CCamDecorator::NewL( iController, R_CAM_STILL_POSTCAP_NAVICOUNTER_DECORATIONS_CAMCORDER );
    iVideoPostcapDecorator = CCamDecorator::NewL( iController, R_CAM_VIDEO_POSTCAP_NAVICOUNTER_DECORATIONS_CAMCORDER );
    
    iSequenceInCaptureDecorator = CCamDecorator::NewL( iController, R_CAM_TIMELAPSE_IN_CAPTURE_NAVICOUNTER_DECORATIONS );
    iTimeLapsePostCaptureDecorator = CCamDecorator::NewL( iController, R_CAM_TIMELAPSE_POST_CAPTURE_NAVICOUNTER_DECORATIONS );

    TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
    TPtrC resname = resFileName;
    
    // Create component bitmaps 
    TSize size = iVidPostStorageIconRect.Rect().Size();
    TCamOrientation orientation = appUi->CamOrientation();

    AknIconUtils::CreateIconL( iPhoneIcon, 
                               iPhoneIconMask, 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_memory_phone,
                               EMbmCameraappQgn_indi_cam4_memory_phone_mask );

    AknIconUtils::SetSize( iPhoneIcon, size );

    AknIconUtils::CreateIconL( iMMCIcon, 
                               iMMCIconMask, 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_memory_mmc, 
                               EMbmCameraappQgn_indi_cam4_memory_mmc_mask );

    AknIconUtils::SetSize( iMMCIcon, size );
    
    if ( orientation != ECamOrientationCamcorder && orientation != ECamOrientationCamcorderLeft )
        {
        AknIconUtils::CreateIconL( iMassStorageIcon, 
                                   iMassStorageIconMask, 
                                   resname, 
                               EMbmCameraappQgn_indi_cam4_memory_mass,  
                               EMbmCameraappQgn_indi_cam4_memory_mass_mask );
        }
    else
        {
        AknIconUtils::CreateIconL( iMassStorageIcon, 
                                   iMassStorageIconMask, 
                                   resname, 
                               EMbmCameraappQgn_indi_cam4_memory_mass,  
                               EMbmCameraappQgn_indi_cam4_memory_mass_mask );
        }
                               
     AknIconUtils::SetSize( iMassStorageIcon, size );
	PRINT( _L("Camera <= CCamNaviCounterModel::LoadResourceDataL") );	
OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMNAVICOUNTERMODEL_LOADRESOURCEDATAL, "e_CCamNaviCounterModel_LoadResourceDataL 0" );

    }
    
// -----------------------------------------------------------------------------
// CCamNaviCounterModel::UnloadResourceData()
// Frees all dynamic resources allocated in LoadResourceDataL
// -----------------------------------------------------------------------------
//
void CCamNaviCounterModel::UnloadResourceData()
    {
#ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE
    delete iSubtitleVideoText;
    iSubtitleVideoText = NULL;
    delete iSubtitlePhotoText;
    iSubtitlePhotoText = NULL;
#endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE

    delete iPhotoPrecapDecorator;
    iPhotoPrecapDecorator = NULL;
    delete iVideoPrecapDecorator;
    iVideoPrecapDecorator = NULL;
    delete iPhotoPostcapDecorator;
    iPhotoPostcapDecorator = NULL;
    delete iVideoPostcapDecorator;
    iVideoPostcapDecorator = NULL;
    delete iSequenceInCaptureDecorator;
    iSequenceInCaptureDecorator = NULL;
    delete iTimeLapsePostCaptureDecorator;
    iTimeLapsePostCaptureDecorator = NULL;
    
    delete iPhoneIcon;
    iPhoneIcon = NULL;
    delete iPhoneIconMask;
    iPhoneIconMask = NULL;
    delete iMMCIcon;
    iMMCIcon = NULL;
    delete iMMCIconMask;
    iMMCIconMask = NULL;
    delete iMassStorageIcon;
    iMassStorageIcon = NULL;
    delete iMassStorageIconMask;
    iMassStorageIconMask = NULL;
    }
    
// -----------------------------------------------------------------------------
// CCamNaviCounterModel::ReloadResourceDataL()
// Refreshes all resource-based information stored in the class
// -----------------------------------------------------------------------------
//
void CCamNaviCounterModel::ReloadResourceDataL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMNAVICOUNTERMODEL_RELOADRESOURCEDATAL, "e_CCamNaviCounterModel_ReloadResourceDataL 1" );
    UnloadResourceData();
    LoadResourceDataL();
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMNAVICOUNTERMODEL_RELOADRESOURCEDATAL, "e_CCamNaviCounterModel_ReloadResourceDataL 0" );
    }

// ---------------------------------------------------------
// CCamNaviCounterModel::UpdateCounter
// Update counter 
// ---------------------------------------------------------
//
void CCamNaviCounterModel::UpdateCounter()
  {
  PRINT( _L("Camera => CCamNaviCounterModel::UpdateCounter") );

  const TInt imagesLeft = Min( 
      iController.ImagesRemaining( ECamMediaStorageCurrent, iBurstActive ), 
      KMaxRemainingImagesShown );
  iCounterText.Format( KNumberFormat, imagesLeft );
  
  if ( iPhotoTextItem )
      {
      TRAP_IGNORE( iPhotoTextItem->SetTextL( iCounterText ) );
      }

  PRINT( _L("Camera <= CCamNaviCounterModel::UpdateCounter") );
  }

// ---------------------------------------------------------
// CCamNaviCounterModel::UpdateTimeLapseCountdownL
// Update the counter for remaining time until the next capture
// ---------------------------------------------------------
//
void CCamNaviCounterModel::UpdateTimeLapseCountdownL()
    {
    // Get the actual remaining time till next capture
    TTime time ( iController.TimeLapseCountdown().Int64() );
    // The format function always rounds down to the next whole number of seconds
    // so add on 0.5 secs to force round up if closer to the next higher whole number of seconds
    // This gives a countdown from max to 1, to get a countdown from max-1 to 0 remove this
    const TTimeIntervalMicroSeconds KHalfSecInMicroSecs = TInt64( 500000 );
    time += KHalfSecInMicroSecs; 
    
    TBuf<15> timebuf;
	// use 00:00 format as normal in secondary cam
	// but long format for primary cam. 2nd cam uses sharing quality. With primary cam sharing 
	// we just use the same layout for sharing and other qualities.
	HBufC* timeFormat;
    if ( ECamActiveCameraSecondary == iController.ActiveCamera() ) 
    	{
    	timeFormat = CEikonEnv::Static()->AllocReadResourceLC( R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO );
    	}
    else 
    	{
    	timeFormat = CEikonEnv::Static()->AllocReadResourceLC( R_QTN_TIME_DURAT_LONG );
    	}
    time.FormatL( timebuf, *timeFormat );
    CleanupStack::PopAndDestroy( timeFormat );
        
    delete iCountDownText;
    iCountDownText = NULL;
    iCountDownText = StringLoader::LoadL( R_CAM_TIMELAPSE_COUNTDOWN_TIME, timebuf ); 
    }  
    
// ---------------------------------------------------------
// CCamNaviCounterModel::UpdateSequenceImageCount
// Update the counter of captured and remaining images for 
// timelapse and burst during capture
// ---------------------------------------------------------
//
void CCamNaviCounterModel::UpdateSequenceImageCount()
  {
  PRINT( _L("Camera => CCamNaviCounterModel::UpdateSequenceImageCount" ) )
  TInt imagesLeft   = iController.ImagesRemaining( ECamMediaStorageCurrent, iBurstActive );
  TInt showLeft     = Min( imagesLeft, KMaxRemainingImagesShown );
  TInt showCaptured = 0;

  if ( ECamImageCaptureTimeLapse == iImageMode )
    {
    // Use the count of captured images
    showCaptured = iController.TimeLapseImageCount();
    }
  else if ( ECamImageCaptureBurst == iImageMode )       
    {
    // Use the count of burst capture moments
    showCaptured = iController.CurrentCapturedCount();
    // The images have not been saved yet so the remaining count does not account
    // for the ones already captured. Reduce the remaining count by the captured count.
    // Commented out - The burst images are saved immediately after capturing.
    // However, we cannot know if all the previous images have already been saved
    // so this number is not 100% accurate.
    // showLeft -= showCaptured; 
	if( ( iOriginalValueForEachBurst > 0 ) && ( iCounterNeedUpdate ) )
		{
	       iOriginalValueForEachBurst--; 
		}
    	if( ( iController.SequenceCaptureInProgress() ) )   
    		{
    		if( !iController.IsRemainingImageStored() )
    		    {
    	            iOriginalValueForEachBurst = showLeft;
		     iController.SetRemainingImageStored();
    		    }
            iCounterText.Format( KNumberFormat, iOriginalValueForEachBurst );
    		}
		else
       iCounterText.Format( KNumberFormat, iController.ImagesRemaining( ECamMediaStorageCurrent, iBurstActive ) );
	   showLeft = iOriginalValueForEachBurst;
    }
  else
    {
    // no action
    }
  
  // show only the KShortBurstCount value if there is enough space to take all
  // pictures, otherwise display the number of images that can be shown
  TInt showSequenceMax = Min( showLeft, KShortBurstCount );
  iSequenceImageText.Format( KTimeLapseImageFormat, showCaptured, showSequenceMax );
  }   

// ---------------------------------------------------------
// CCamNaviCounterModel::UpdateRecordTimeAvailableL
// Update record time remaining 
// ---------------------------------------------------------
//
void CCamNaviCounterModel::UpdateRecordTimeAvailableL()
    {
        PRINT( _L("Camera => CCamNaviCounterModel::UpdateRecordTimeAvailableL" ) )
    // Get the total remaining record time from the controller
    TTime time ( iController.RecordTimeRemaining().Int64() );
    HBufC* timeFormat;

    // Use 00:00 format in secondary cam
	if ( ECamActiveCameraSecondary == iController.ActiveCamera() ) 
        {
	   	timeFormat = CEikonEnv::Static()->AllocReadResourceLC( R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO );
        }
	// Use 0:00:00 format in primary cam
	else 
        {
	    timeFormat = CEikonEnv::Static()->AllocReadResourceLC( R_QTN_TIME_DURAT_LONG );
        }
	
    time.FormatL( iRemainingTimeText, *timeFormat );
    if ( iVideoTextItem )
        {
        iVideoTextItem->SetTextL( iRemainingTimeText );
        }
    CleanupStack::PopAndDestroy( timeFormat );
    PRINT( _L("Camera <= CCamNaviCounterModel::UpdateRecordTimeAvailableL" ) )
    }

// ---------------------------------------------------------
// CCamNaviCounterModel::SetCaptureModeL
// Set capture mode (defines which counter to use)
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::SetCaptureModeL( TCamCameraMode       aMode,
                                       TCamImageCaptureMode aImageMode )
  {
  iMode      = aMode;
  iImageMode = aImageMode;

  TInt key = ( ECamControllerVideo == iMode )
             ? ECamSettingItemVideoMediaStorage
             : ECamSettingItemPhotoMediaStorage;

  // Get the total remaining record time from the controller
  iStorageLocation = 
    static_cast<TCamMediaStorage>( iController.IntegerSettingValue( key ) );
    
  if (ECamMediaStorageMassStorage ==iStorageLocation)
   {
   iStorageLocation = iController.ExistMassStorage()?ECamMediaStorageMassStorage:ECamMediaStoragePhone;
   }
  }

// ---------------------------------------------------------
// CCamNaviCounterModel::HandleSelfTimerEvent
// Handle an event from CCamSelfTimer.
// ---------------------------------------------------------
//
void CCamNaviCounterModel::HandleSelfTimerEvent( TCamSelfTimerEvent aEvent, TInt aCountDown )
  {
  switch ( aEvent )
    {
    case ECamSelfTimerEventTimerOn:
      {
      iSelfTimerText.Num( TInt64( aCountDown ) );
      iDrawSelfTimer = ETrue;
      }
      break;
    case ECamSelfTimerEventTimerOff:
      {
      iDrawSelfTimer = EFalse;
      }
      break;
    default:
      break;
    }
  }

// ---------------------------------------------------------
// CCamNaviCounterModel::CCamNaviCounterModel
// C++ constructor
// ---------------------------------------------------------
//
CCamNaviCounterModel::CCamNaviCounterModel( CCamAppController& aController )
  : iController( aController )
  {
  }

// ---------------------------------------------------------
// CCamNaviCounterModel::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamNaviCounterModel::ConstructL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMNAVICOUNTERMODEL_CONSTRUCTL, "e_CCamNaviCounterModel_ConstructL 1" );
    PRINT( _L("Camera => CCamNaviCounterModel::ConstructL") );
    iExtent = TRect(0, 0, 0, 0);
    
    UpdateCounter();
    iController.AddControllerObserverL( this );

    PRINT( _L("Camera <> Load resource data..") );

    LoadResourceDataL();

    TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
    TPtrC resname = resFileName;
    
#ifdef PRODUCT_SUPPORTS_POST_CAPTURE_INDICATORS		
    AknIconUtils::CreateIconL( iMpeg4Icon, 
                               iMpeg4IconMask, 
                               resname, 
                               EMbmCameraappQgn_prop_cam4_codec_mp4, 
                               EMbmCameraappQgn_prop_cam4_codec_mp4_mask );

    AknIconUtils::CreateIconL( i3GPIcon, 
                               i3GPIconMask, 
                               resname, 
                               EMbmCameraappQgn_prop_cam4_codec_3gp, 
                               EMbmCameraappQgn_prop_cam4_codec_3gp_mask );
#endif

    // wait for engine to initialise video recorder
    iVideoInitialised = EFalse;       

    PRINT( _L("Camera <> construct navi self timer..") );
    ConstructNaviSelfTimerL( resname );


    PRINT( _L("Camera <> construct navi sequence..") );
    ConstructNaviSequenceL( resname );

#ifdef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
    PRINT( _L("Camera <> construct generic setup..") );
    ConstructNaviGenericSetupL( resname );
#endif 

    PRINT( _L("Camera <> construct navi audio mute..") );
    ConstructNaviAudioMuteL( resname );    

#ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE
    PRINT( _L("Camera <> construct navi subtitle..") );
    ConstructNaviModeSubtitleL( resname );
#endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE

    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    iBurstActive = appUi->IsBurstEnabled();
#if defined( PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE ) 
    iDrawSequence = iBurstActive;
#endif // PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 

    PRINT( _L("Camera <> construct observer handler..") );
    iObserverHandler = CCamObserverHandler::NewL();
    
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsLocationSupported() )
        {
        iLocationIconVisible = (/*TCamLocationId::ECamLocationOn*/1 == iController.IntegerSettingValue( ECamSettingItemRecLocation ));
        }
    PRINT( _L("Camera <= CCamNaviCounterModel::ConstructL") );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMNAVICOUNTERMODEL_CONSTRUCTL, "e_CCamNaviCounterModel_ConstructL 0" );
    }                

// ---------------------------------------------------------
// CCamNaviCounterModel::Draw
// Draw the control
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::DrawNaviCtr(       CBitmapContext& aGc, 
                                   const CCoeControl*    aControl ) const
  {
  DrawCounter( aGc, aControl );
  }
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawCounter
// Draw the control
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::DrawCounter(       CBitmapContext& aGc, 
                                   const CCoeControl*    /*aControl*/ ) const
  {
  PRINT_FRQ( _L("Camera => CCamNaviCounterModel::DrawCounter" ))
  MAknsSkinInstance* skin = AknsUtils::SkinInstance();

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  const TCamViewState& viewState( appUi->CurrentViewState() );
  
  
  // -------------------------------------------------------
  // Precapture state
  if ( ECamViewStatePreCapture == viewState )
    { 
    TBool inSequence  = iController.SequenceCaptureInProgress();
    TBool inTimeLapse = inSequence && (ECamImageCaptureTimeLapse == iImageMode);
    TBool inBurst     = inSequence && (ECamImageCaptureBurst     == iImageMode);    

    TBool stillCapturing = ECamImageCaptureSingle == iImageMode  
                          && iController.IsProcessingCapture();

    
    // Audio mute icon should be drawn even though courtesy UI applies.
    if ( iMode == ECamControllerVideo )
      {
      DrawNaviAudioMute( aGc );
      }
    // these are drawn when sequence capture is in progress and 
    // should be drawn even when courtesy stuff is off
    if ( inTimeLapse || inBurst )                
        {
        iSequenceInCaptureDecorator->Draw( aGc, iExtent );
        }         
    else
        {
        // To prevent Lint warning
        }
    // Draw the navipane sequence icon if current performing a timelapse or burst capture
    if ( inTimeLapse || inBurst )
        {
        DrawNaviSequence( aGc );
        }
    if ( inTimeLapse && iController.TimeLapseSupported() )
        {
        // Draw the captured images and captured remaining text
        DrawSequenceImageText( skin, aGc );
        // Draw the timelapse countdown counter
        DrawTimeLapseCountdown( skin, aGc );
        }
    else if ( inBurst )  
        {
        // Draw the captured images and captured remaining text
        DrawSequenceImageText( skin, aGc );     
        }

    if ( appUi && ( appUi->DrawPreCaptureCourtesyUI() || 
       ( !appUi->DrawPreCaptureCourtesyUI() && appUi->IsSecondCameraEnabled() ) ) )
      {
     
      if ( !inTimeLapse && !inBurst && !iController.IsProcessingCapture() )
        {
        // Draw the icon for storage location (Phone/MediaCard)
        DrawStorageIcon( aGc );    
        }
                             
#ifdef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE
      if ( iDrawSequence )
        {
        DrawNaviSequence( aGc );
        }           
#endif // PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE


#ifdef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
      DrawNaviGenericSetup( aGc );
#endif // PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR       
        
      if ( iMode == ECamControllerVideo )
        {
        // only draw remaining time when engine has initialised video
        if ( iVideoInitialised && iVideoTextItem && !iController.IsProcessingCapture() )
          {
          iVideoTextItem->Draw( aGc );
          }
        }
      else if ( !stillCapturing && iPhotoTextItem && !inBurst)
        {
        iPhotoTextItem->Draw( aGc );
        }
      else
        {
        // To prevent Lint warning
        }
      }
    }
            
  // -------------------------------------------------------
  // Post capture state
  else if ( ECamViewStatePostCapture    == viewState
         || ECamViewStateBurstThumbnail == viewState )
    {
    if ( appUi->DrawPostCaptureCourtesyUI() )
      {
           
      TRAP_IGNORE( DrawCurrentFileSizeL( skin, aGc ) );            

#ifdef PRODUCT_SUPPORTS_POST_CAPTURE_INDICATORS 
      // Draw the icon for storage location (Phone/MediaCard)
      DrawStorageIcon( aGc );                     

      if ( iMode == ECamControllerVideo )
        {            
        // Draw the video file type indicator
        DrawVideoFileTypeIndicator( aGc );
        }
#endif // PRODUCT_SUPPORTS_POST_CAPTURE_INDICATORS

      if ( appUi->CurrentBurstMode() == ECamImageCaptureTimeLapse )
        {
        // Draw images captured text background
        PRINT( _L("Camera <> Drawing timelapse postcapture decorator.. #######################") );
        iTimeLapsePostCaptureDecorator->Draw( aGc, iExtent );  
        // Draw images capture text
        TRAP_IGNORE( DrawImagesCapturedTextL( skin, aGc ) );
        }

      }
    }   
  // -------------------------------------------------------
  // otherwise, do nothing
  else
    {
    // empty statement to remove Lint error, MISRA required rule 60
    }        
  // -------------------------------------------------------

  DrawNaviSelfTimer( aGc, skin );
  
#ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE
  DrawNaviModeSubtitle( aGc, skin );    
#endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE

  PRINT_FRQ( _L("Camera <= CCamNaviCounterModel::DrawCounter" ))
  }


// ---------------------------------------------------------
// CCamNaviCounterModel::DrawCounter
// Draw the control
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::DrawCounterToBitmaps( CFbsBitGc& aBmpGc, 
                                            CFbsBitGc& aBmpMaskGc ) const
  {
  PRINT( _L("Camera => CCamNaviCounterModel::DrawCounterToBitmaps" ))
  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );    
  // Draw bitmaps to use in the real navi pane in the settings views
  if ( appUi->CurrentViewState() == ECamViewStateSettings )
    {  
    DrawStorageIconToBitmap( aBmpGc, aBmpMaskGc );  
    const CFont* font = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
    // TRgb color = layoutText.Color();
    TRgb color = KRgbWhite;
    // Color is not updated if it not found from the skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor( skin, color, KNaviPaneMajorColour, KNaviPaneMinorColour );    
    aBmpGc.SetPenColor(color);
    aBmpGc.UseFont(font);
    if ( iMode == ECamControllerVideo )
      {

      TBufC<KMaxTextLength> buf( iRemainingTimeText );
      TPtr pointer = buf.Des();
      AknTextUtils::LanguageSpecificNumberConversion( pointer );
      aBmpGc.DrawText( buf, TPoint( 35, font->HeightInPixels()) );
      }
    else
      {
      TBufC<KMaxTextLength> buf( iCounterText );
      TPtr pointer = buf.Des();
      AknTextUtils::LanguageSpecificNumberConversion( pointer );
      aBmpGc.DrawText( buf, TPoint( 35, font->HeightInPixels()) );
      }                 
    aBmpGc.DiscardFont();
    
    aBmpMaskGc.SetPenColor(KRgbBlack);
    aBmpMaskGc.UseFont(font);
    if ( iMode == ECamControllerVideo )
      {
      TBufC<KMaxTextLength> buf( iRemainingTimeText );
      TPtr pointer = buf.Des();
      AknTextUtils::LanguageSpecificNumberConversion( pointer );
      aBmpMaskGc.DrawText( buf, TPoint( 35,font->HeightInPixels()) );

      }
    else
      {

      TBufC<KMaxTextLength> buf( iCounterText );
      TPtr pointer = buf.Des();
      AknTextUtils::LanguageSpecificNumberConversion( pointer );
      aBmpMaskGc.DrawText( buf, TPoint( 35,font->HeightInPixels()) );

      }               
    aBmpMaskGc.DiscardFont();  
    }  
  PRINT( _L("Camera <= CCamNaviCounterModel::DrawCounterToBitmaps" ))
  }

// ---------------------------------------------------------
// CCamNaviCounterModel::DrawStorageIconToBitmap
// Draw the icon for the media storage location
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::DrawStorageIconToBitmap( CFbsBitGc& aBmpGc, 
                                               CFbsBitGc& aBmpMaskGc  ) const
  {
  CFbsBitmap* icon = NULL;
  CFbsBitmap* mask = NULL;
  if ( iStorageLocation == ECamMediaStoragePhone )
    {
    icon = iPhoneIcon;
    mask = iPhoneIconMask;
    }
  else if ( iStorageLocation == ECamMediaStorageMassStorage )
    {
    icon = iMassStorageIcon;
    mask = iMassStorageIconMask;
    }
  else
    {
    icon = iMMCIcon;
    mask = iMMCIconMask;
    }
  // Should use layout                
  aBmpGc.BitBlt(TPoint(0,0), icon);
  aBmpMaskGc.BitBlt(TPoint(0,0), mask);
  }

// ---------------------------------------------------------
// CCamNaviCounterModel::DrawStorageIcon
// Draw the icon for the media storage location
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::DrawStorageIcon( CBitmapContext& aGc ) const
  {
  CFbsBitmap* icon = NULL;
  CFbsBitmap* mask = NULL;
  if ( iStorageLocation == ECamMediaStoragePhone )
    {
    icon = iPhoneIcon;
    mask = iPhoneIconMask;
    }
  else if ( iStorageLocation == ECamMediaStorageMassStorage )
    {
    icon = iMassStorageIcon;
    mask = iMassStorageIconMask;
    }
  else
    {
    icon = iMMCIcon;
    mask = iMMCIconMask;
    }
  
  if ( iMode == ECamControllerVideo )
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );    
  
    // Postcapture state
    if ( appUi->CurrentViewState() == ECamViewStatePostCapture ) 
      {
      aGc.BitBltMasked( iVidPostStorageIconRect.Rect().iTl, icon, icon->SizeInPixels(), mask, ETrue );
      }
    else
      {
      aGc.BitBltMasked( iVidPreStorageIconRect.Rect().iTl, icon, icon->SizeInPixels(), mask, ETrue );
      }
    }
  else
    {
    aGc.BitBltMasked( iImgStorageIconRect.Rect().iTl, icon, icon->SizeInPixels(), mask, ETrue );                 
    }
  }

// ---------------------------------------------------------
// CCamNaviCounterModel::DrawVideoFileTypeIndicator
// Draw the icon for the video file type indicator
// ---------------------------------------------------------
//
void CCamNaviCounterModel::DrawVideoFileTypeIndicator( CBitmapContext& aGc ) const
    {
    CFbsBitmap* icon = NULL;
    CFbsBitmap* mask = NULL;

    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi && appUi->DrawPreCaptureCourtesyUI() )
        {
        TCamVideoFileType fileType = static_cast< TCamVideoFileType > 
            ( iController.IntegerSettingValue( ECamSettingItemVideoFileType ) );
        
        if ( fileType == ECamVideoMpeg4 )
            {
            icon = iMpeg4Icon;
            mask = iMpeg4IconMask;
            }

        // Otherwise display the H263 icon.
        else
            {
            icon = i3GPIcon;
            mask = i3GPIconMask;
            }    

        aGc.BitBltMasked( iVideoFileTypeIconRect.Rect().iTl, icon, icon->SizeInPixels(), mask, ETrue );           
        }
    }
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawText
// Draw the counter text (photo mode)
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::DrawText( MAknsSkinInstance* /* aSkin */,
                                const TDesC& /* aText */, 
                                CBitmapContext& /* aGc */ ) const
    {
    PRINT( _L("camera <> CCamNaviCounterModel::DrawText() not implemented" ) );
    }
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawTimeLapseCountdown
// Draw the timelapse image counter text (captured and remaining images)
// ---------------------------------------------------------
//  
void CCamNaviCounterModel::DrawTimeLapseCountdown( MAknsSkinInstance* /*aSkin*/,
                                                   CBitmapContext& aGc ) const
    {
    PRINT( _L("Camera => CCamNaviCounterModel::DrawTimeLapseCountdown") );
    if( iCountDownText )
        {
        TBufC<32> buf( *iCountDownText ); // 00:00:00 as Unicode, 32 should be enough
        TPtr pointer = buf.Des();
        AknTextUtils::LanguageSpecificNumberConversion( pointer );            
        if ( iTimeLapseCountdownTextItem )
            {
            TRAP_IGNORE( iTimeLapseCountdownTextItem->SetTextL( buf ) );
            iTimeLapseCountdownTextItem->Draw( aGc );
            }
        }  
    else
        {
        PRINT( _L("Camera <> CCamNaviCounterModel::DrawTimeLapseCountdown .. [WARNING] no countdown text!") );
        }
    PRINT( _L("Camera <= CCamNaviCounterModel::DrawTimeLapseCountdown") );
    }

// ---------------------------------------------------------
// CCamNaviCounterModel::DrawSequenceImageText
// Draw the captured and remaining image count during timelapse capture
// ---------------------------------------------------------
//
void CCamNaviCounterModel::DrawSequenceImageText( MAknsSkinInstance* /*aSkin*/,
                                                  CBitmapContext& aGc ) const
    {
    PRINT1( _L("CCamNaviCounterModel::DrawSequenceImageText %S" ), &iSequenceImageText )

    if ( iSequenceImageTextItem )
        {
        TRAP_IGNORE( iSequenceImageTextItem->SetTextL( iSequenceImageText ) );
        iSequenceImageTextItem->Draw( aGc );
        }
    }

// ---------------------------------------------------------
// CCamNaviCounterModel::DrawImagesCapturedTextL
// Draw the count of images captured in timelapse postcapture view
// ---------------------------------------------------------
//    
void CCamNaviCounterModel::DrawImagesCapturedTextL(
        MAknsSkinInstance* /*aSkin*/,
        CBitmapContext& aGc ) const  
    {
    // Draw count of captured images
    const TInt imagesCaptured = iController.TimeLapseImageCount();
    HBufC* capturedImagesText = StringLoader::LoadLC( R_CAM_TIMELAPSE_IMAGES_CAPTURED, imagesCaptured );    

    HBufC* buf = HBufC::NewLC( KCamDefaultCapturedImagesTextLen );
    if ( capturedImagesText->Length() > buf->Length() )
      {
      buf->ReAlloc( capturedImagesText->Length() );
      }

    buf = capturedImagesText;
    TPtr pointer = buf->Des();
    AknTextUtils::LanguageSpecificNumberConversion( pointer );

    if ( iSequenceCapturedTextItem )
        {
        iSequenceCapturedTextItem->SetTextL( *buf );
        iSequenceCapturedTextItem->Draw( aGc );
        }

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( capturedImagesText );
    }
    
#ifdef PRODUCT_SUPPORTS_NAVIPANE_FILENAME
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawCurrentFileName
// Draw the file name 
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::DrawCurrentFileName( MAknsSkinInstance* aSkin, 
                                           CBitmapContext& aGc ) const
    {
    TAknLayoutText layoutText;
    if ( iMode == ECamControllerVideo )
        {
        layoutText = iVideoNameLayout;
        }
    else
        {
        layoutText = iPhotoNameLayout;
        }
    // Draw counter text
    TRgb color = layoutText.Color();

    // Color is not updated if it not found from the skin
    AknsUtils::GetCachedColor( aSkin, color,
        KNaviPaneMajorColour, KNaviPaneMinorColour );

	
    layoutText.DrawText( aGc, iController.CurrentImageName() , ETrue, KRgbWhite );
    }
#endif // PRODUCT_SUPPORTS_NAVIPANE_FILENAME       
        

// ---------------------------------------------------------
// CCamNaviCounterModel::DrawCurrentFileSizeL
// Draw the file size 
// ---------------------------------------------------------
//

void 
CCamNaviCounterModel::DrawCurrentFileSizeL( MAknsSkinInstance* aSkin, 
                                            CBitmapContext& aGc ) const
    {
    TAknLayoutText layoutText;
    if ( iMode == ECamControllerVideo )
        {
        layoutText = iVideoSizeLayout;
        }
    else
        {
        layoutText = iPhotoSizeLayout;
        }
    // Draw counter text
    TRgb color = layoutText.Color();

    // Color is not updated if it not found from the skin
    AknsUtils::GetCachedColor( aSkin, color,
        KNaviPaneMajorColour, KNaviPaneMinorColour );

    // Show the filename or burst name (if burst mode)

	//START: NOT DISPLAYING FILESIZE
	/*
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    TInt size = 0;
    
    // Get the filesize
    if ( appUi->CurrentViewState() == ECamViewStatePostCapture )
        {
        TPtrC filename = iController.CurrentFullFileName();
        size = iController.FileSize( filename );                        
        }
    else if ( appUi->CurrentViewState() == ECamViewStateBurstThumbnail )
        {
        }
    // otherwise, do nothing
    else
        {
        // empty statement to remove Lint error, MISRA required rule 60
        }
      */
	//END: NOT DISPLAYING FILESIZE

    if ( iMode == ECamControllerVideo )        
        {
        // Draw the file size string
        TBuf <15> timebuf;
        TBuf <30> fullbuf;
        
        // Generate the string for video length (time)
        TTimeIntervalMicroSeconds interval = iController.RecordTimeElapsed();
        TTime time( interval.Int64() );
        time.FormatL( timebuf, *iTimeFormat ); 
            aGc.SetBrushColor( KRgbWhite );
            TBufC<KMaxTextLength> buf( timebuf );
            TPtr pointer = buf.Des();
            AknTextUtils::LanguageSpecificNumberConversion( pointer );
    	    layoutText.DrawText( aGc, pointer, EFalse, KRgbBlack ); 
        }

    // START: NOT DISPLAYING FILESIZE
    /*
    else        
        {
        if ( size > 0 )
            {        
            // Draw the file size string
            HBufC* fileSizeStr = StringLoader::LoadLC( R_QTN_SIZE_KB, size / KKilo );    
            layoutText.DrawText( aGc, fileSizeStr->Des() , ETrue, KRgbWhite );        
            CleanupStack::PopAndDestroy( fileSizeStr );
            }            
        }
    */
    // END: NOT DISPLAYING FILESIZE
        
    }


// ----------------------------------------------------
// CCamNaviCounterModel::BurstModeActiveL
// Notification that the burst mode has been activated/deactivated
// ----------------------------------------------------
//
// burst mode icon is displayed in either the navipane or sidepane
void CCamNaviCounterModel::BurstModeActiveL( TBool aActive, TBool /*aStillModeActive*/ )
    {
#ifdef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 
    // Update internal state, and trigger a redraw
    iDrawSequence = aActive;
    DrawDeferred();
#endif // PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 

    // update remaining images
    iBurstActive = aActive;
    UpdateCounter();
    }


// ---------------------------------------------------------
// CCamNaviCounterModel::ForceNaviPaneUpdate
// Force update of navi-pane (i.e after dismissal of MMC removed error note)
// ---------------------------------------------------------
//
void CCamNaviCounterModel::ForceNaviPaneUpdate()
  {
  PRINT( _L("Camera => CCamNaviCounterModel::ForceNaviPaneUpdate" ))
  // update counters
  // get current storage location
  TInt key = ( ECamControllerVideo == iMode ) 
             ? ECamSettingItemVideoMediaStorage 
             : ECamSettingItemPhotoMediaStorage;

  if ( static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->IsMMCRemovedNotePending() )
    {
    PRINT( _L("Camera <> CCamNaviCounterModel::ForceNaviPaneUpdate pend") )
    iStorageLocation = 
        static_cast<TCamMediaStorage>( 
            iController.IntegerSettingValueUnfiltered( key ) );
    }
  else
    {
    PRINT( _L("Camera <> CCamNaviCounterModel::ForceNaviPaneUpdate yyy") )
    iStorageLocation = 
        static_cast<TCamMediaStorage>( 
            iController.IntegerSettingValue( key ) );
    }
  
  if (ECamMediaStorageMassStorage ==iStorageLocation)
    {
    iStorageLocation = iController.ExistMassStorage()?ECamMediaStorageMassStorage:ECamMediaStoragePhone;
    }
  UpdateCounter();    

  TRAP_IGNORE( UpdateRecordTimeAvailableL() );
  PRINT( _L("Camera <= CCamNaviCounterModel::ForceNaviPaneUpdate" ))
  }


// ---------------------------------------------------------
// CCamNaviCounterModel::HandleControllerEventL
// Handle an event from CCamAppController.
// ---------------------------------------------------------
//
void 
CCamNaviCounterModel::HandleControllerEventL( TCamControllerEvent aEvent, 
                                              TInt                /* aError */ )
  {
  PRINT1( _L("Camera => CCamNaviCounterModel::HandleControllerEventL %d" ), aEvent )	
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  iCounterNeedUpdate = EFalse;

  switch( aEvent )
    {
    // -----------------------------------------------------
    // If this is a capture complete event, or the image quality or save
    // location has changed...
//    case ECamEventCaptureComplete:
    case ECamEventVideoQualityChanged:
    case ECamEventImageQualityChanged:
    case ECamEventSaveLocationChanged:
      {
      if( iMode == ECamControllerVideo )
          {	
          UpdateRecordTimeAvailableL();
          }
      else
          {
          UpdateCounter();	
          }	    

      if ( ECamEventSaveLocationChanged == aEvent )
        {
        TCamSettingItemIds storageId = (ECamControllerVideo == iMode)
                                       ? ECamSettingItemVideoMediaStorage
                                       : ECamSettingItemPhotoMediaStorage;

        // update location
        if ( appUi->IsMMCRemovedNotePending() )
          {
          iStorageLocation = static_cast< TCamMediaStorage > 
              ( iController.IntegerSettingValueUnfiltered( storageId ) );
          }
        else
          {
          iStorageLocation = static_cast< TCamMediaStorage > 
              ( iController.IntegerSettingValue( storageId ) );
           }
        //CreateNaviBitmapsL( ETrue );
        
        if (ECamMediaStorageMassStorage ==iStorageLocation)
          {
          iStorageLocation = iController.ExistMassStorage()?ECamMediaStorageMassStorage:ECamMediaStoragePhone;
          }
        }
      BroadcastEvent( ECamObserverEventNaviModelUpdated );
      break;
      }
    // -----------------------------------------------------
    case ECamEventLocationSettingChanged:
      {
      iLocationIconVisible = (/*TCamLocationId::ECamLocationOn*/1 == iController.IntegerSettingValue( ECamSettingItemRecLocation ));
      // We reload the resource data so that the control is drawn correctly
      ReloadResourceDataL();
      //BroadcastEvent( ECamObserverEventNaviModelUpdated );
      break;
      }
    // -----------------------------------------------------
    case ECamEventEngineStateChanged:
      {
      if( ECamControllerVideo == iMode )
        {
        // wait until engine has initialised video before updating time remaining
        if ( !iVideoInitialised )
          {
          // <CAMERAAPP_CAPI_V2_MIGRATION/>
          // if ( iController.EngineState() == ECamEngineVideoCapturePrepared )
          if ( iController.CameraState() == ECamCameraPreparedVideo )
            {
            iVideoInitialised = ETrue;
            }
          }
        if ( iVideoInitialised )
          {
          UpdateRecordTimeAvailableL();
          if ( iController.IsViewFinding() )
            {
            BroadcastEvent( ECamObserverEventNaviModelUpdated );
            }           
          }
        }
      else
        {
        // exited from video mode
        iVideoInitialised = EFalse;
        }
      break;
      }

    // -----------------------------------------------------
    case ECamEventSaveComplete:
    case ECamEventRecordComplete:
      {
      if( ECamControllerVideo == iMode )
        {
        UpdateRecordTimeAvailableL();
        }
      else
        {
        switch( iImageMode )
          {
          case ECamImageCaptureBurst:
          case ECamImageCaptureTimeLapse:
            // Sequence: captured / remaining
            UpdateSequenceImageCount();
            break;
          default:
            // Remaining images
            UpdateCounter();
            break;
          }
        }
      // no broadcast if in burst and all snapshots have not been received  
      if( !( iMode == ECamControllerImage && 
             iImageMode == ECamImageCaptureBurst && 
             !iController.AllSnapshotsReceived()) )
          {
          BroadcastEvent( ECamObserverEventNaviModelUpdated );
          }
      break;
      }
    case ECamEventAudioMuteStateChanged:
      {
      if ( appUi->IsDirectViewfinderActive() )
        {
        appUi->HandleCommandL( ECamCmdRedrawScreen );
        }
      //Remaining Recording time is updated before drawing 
      //CreateNaviBitmapsL( ETrue );
      break;
      }
    // -----------------------------------------------------
    case ECamEventCounterUpdated:
      {
      if( iController.SequenceCaptureInProgress() 
       && ( ECamImageCaptureTimeLapse == iImageMode  
         || ECamImageCaptureBurst     == iImageMode ) )
        {
        // UpdateCounter;
        iCounterNeedUpdate = ETrue;
        UpdateSequenceImageCount();
        if ( ECamImageCaptureTimeLapse == iImageMode )
            {
            // Update remaining time till next capture
            UpdateTimeLapseCountdownL();
            }    
        if( iController.AllSnapshotsReceived() )
            {    
            PRINT( _L("CCamNaviCounterModel BroadcastEvent( ECamObserverEventNaviModelUpdated )" ) )                
            BroadcastEvent( ECamObserverEventNaviModelUpdated );
            }
        }
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
// CCamNaviCounterModel::ConstructNaviSelfTimerL
// Creating the member variables required for showing the 
// self timer state in the navipane
// ---------------------------------------------------------
//
void CCamNaviCounterModel::ConstructNaviSelfTimerL( TPtrC& aResname )
    {        
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    
    // Setup self timer icon.
    // ...Create the icon
    TSize size = iSelfTimerIconRect.Rect().Size();
    AknIconUtils::CreateIconL( iSelfTimerIcon, 
                               iSelfTimerMask, 
                               aResname, 
                               EMbmCameraappQgn_indi_cam4_selftimer, 
                               EMbmCameraappQgn_indi_cam4_selftimer_mask );
    AknIconUtils::SetSize( iSelfTimerIcon, size );

    // ...Add self as an observer of the self timer.    
    CCamSelfTimer* selftimer = appUi->SelfTimer();
    if ( selftimer )
        {
        selftimer->AddObserverL( this );
        }
    }
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawNaviSelfTimer
// Draw the self timer icon
// ---------------------------------------------------------
//             
void 
CCamNaviCounterModel::DrawNaviSelfTimer( CBitmapContext& aGc, 
                                         MAknsSkinInstance* /*aSkin*/ ) const
    {
    // Draw self timer icons, if set to on.
    if ( iDrawSelfTimer )
        {
        if ( CamUtility::IsNhdDevice() )
            {
            aGc.SetBrushColor( KRgbWhite );
            aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
            iSelfTimerTextLayout.DrawText( aGc, iSelfTimerText, EFalse, KRgbBlack ); 
        
            aGc.BitBltMasked( iSelfTimerIconRect.Rect().iTl, iSelfTimerIcon, 
                iSelfTimerIcon->SizeInPixels(), iSelfTimerMask, ETrue );
            }
        else
            {
            aGc.SetBrushColor( KRgbBlack );
            aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
            iSelfTimerTextLayout.DrawText( aGc, iSelfTimerText, EFalse, KRgbWhite ); 
        
            aGc.BitBltMasked( iSelfTimerIconRect.Rect().iTl, iSelfTimerIcon, 
                iSelfTimerIcon->SizeInPixels(), iSelfTimerMask, ETrue );
            }
        }    
    }        

// ---------------------------------------------------------
// CCamNaviCounterModel::ConstructNaviSequenceL
// Creating the member variables required for showing the 
// sequence capture state in the navipane
// ---------------------------------------------------------
//
void CCamNaviCounterModel::ConstructNaviSequenceL( TPtrC& aResname )
    {
    // Setup sequence icon.
    // ...Create the icon
    TSize size = iSequenceIconRect.Rect().Size();
    AknIconUtils::CreateIconL( iSequenceIcon, 
                               iSequenceMask, 
                               aResname, 
                               EMbmCameraappQgn_indi_cam4_sequence_burst, 
                               EMbmCameraappQgn_indi_cam4_sequence_burst_mask );
    AknIconUtils::SetSize( iSequenceIcon, size );

    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

    // ...Add self as an observer of burst mode
    appUi->AddBurstModeObserverL( this );
    }
    
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawNaviSequence
// Draw the sequence capture icon
// ---------------------------------------------------------
//         
void CCamNaviCounterModel::DrawNaviSequence( CBitmapContext& aGc ) const
    {        
    aGc.BitBltMasked( iSequenceIconRect.Rect().iTl, iSequenceIcon, 
        iSequenceIcon->SizeInPixels(), iSequenceMask, ETrue);
    }

#ifdef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
// ---------------------------------------------------------
// CCamNaviCounterModel::ConstructNaviGenericSetupL
// Creating the member variables required for showing the 
// generic setup state in the navipane
// ---------------------------------------------------------
//
void CCamNaviCounterModel::ConstructNaviGenericSetupL( TPtrC& aResname )
    {
    // Setup generic setup icon.
    // ...Create the icon
    TSize size = iGenericIconRect.Rect().Size();
    AknIconUtils::CreateIconL( iGenericIcon, 
                               iGenericMask, 
                               aResname, 
                               EMbmCameraappQgn_indi_lcam_generic, 
                               EMbmCameraappQgn_indi_lcam_generic_mask );
    AknIconUtils::SetSize( iGenericIcon, size );
    }
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawNaviGenericSetup
// Draw the generic setup icon
// ---------------------------------------------------------
//        
void CCamNaviCounterModel::DrawNaviGenericSetup( CBitmapContext& aGc ) const
    {
    if ( ( iMode == ECamControllerVideo && !iController.VideoSceneDefaultsAreSet() ) ||
         ( iMode != ECamControllerVideo && !iController.PhotoSceneDefaultsAreSet() ) )
        {
        aGc.BitBltMasked( iGenericIconRect.Rect().iTl, iGenericIcon, 
            iGenericIcon->SizeInPixels(), iGenericMask, ETrue);
        }    
    }
#endif // PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR




// ---------------------------------------------------------
// CCamNaviCounterModel::ConstructNaviAudioMuteL
// Creating the member variables required for showing the 
// audio mute state in the navipane
// ---------------------------------------------------------
//
void CCamNaviCounterModel::ConstructNaviAudioMuteL( TPtrC& aResname )
    {        
    // Setup generic setup icon.

    // ...Create the icon
    TSize size = iAudioMuteIconRect.Rect().Size();
    AknIconUtils::CreateIconL( iAudioMuteIcon, 
                               iAudioMuteMask, 
                               aResname, 
                               EMbmCameraappQgn_indi_vid4_audio_mute, 
                               EMbmCameraappQgn_indi_vid4_audio_mute_mask );
    AknIconUtils::SetSize( iAudioMuteIcon, size );
    }
    
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawNaviAudioMute
// Draw the audio mute icon
// ---------------------------------------------------------
//    
void CCamNaviCounterModel::DrawNaviAudioMute( CBitmapContext& aGc ) const
    {            
    TCamSettingsOnOff audio = static_cast< TCamSettingsOnOff > 
        ( iController.IntegerSettingValue( ECamSettingItemVideoAudioRec ) );

    // If the current mode is in video and the audio has been
    // muted, display the audio mute indicator.
    if ( ( ECamSettOff == audio ) && ( iMode == ECamControllerVideo ) )
        {
        // Draw icon
        aGc.BitBltMasked( iAudioMuteIconRect.Rect().iTl, iAudioMuteIcon, 
            iAudioMuteIcon->SizeInPixels(), iAudioMuteMask, ETrue);        
        }
    }
    



#ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE
// ---------------------------------------------------------
// CCamNaviCounterModel::ConstructNaviModeSubtitleL
// Creating the member variables required for showing the 
// current mode in the navipane
// ---------------------------------------------------------
//
void CCamNaviCounterModel::ConstructNaviModeSubtitleL( TPtrC& aResname )
    {            
    // Setup generic setup icon.
       
    // ...Create the icon (photo icon)
    TSize size = iSubtitlePhotoRect.Rect().Size();
    AknIconUtils::CreateIconL( iSubtitlePhotoIcon, 
                               iSubtitlePhotoMask, 
                               aResname, 
                               EMbmCameraappQgn_indi_cam_photomode,            
                               EMbmCameraappQgn_indi_cam_photomode_mask );     
    AknIconUtils::SetSize( iSubtitlePhotoIcon, size );
    
    // ...Create the icon (video icon)
    size = iSubtitlePhotoRect.Rect().Size();
    AknIconUtils::CreateIconL( iSubtitleVideoIcon, 
                               iSubtitleVideoMask, 
                               aResname, 
                               EMbmCameraappQgn_indi_cam_videomode,            
                               EMbmCameraappQgn_indi_cam_videomode_mask );     
    AknIconUtils::SetSize( iSubtitleVideoIcon, size );        
    }
    
// ---------------------------------------------------------
// CCamNaviCounterModel::DrawNaviModeSubtitle
// Draw the subtitle text and icon
// ---------------------------------------------------------
//
void CCamNaviCounterModel::DrawNaviModeSubtitle( CBitmapContext& aGc, MAknsSkinInstance* aSkin ) const
    {
    CWindowGc& gc = SystemGc();
    // Set drawing colour
    TRgb color = iSelfTimerTextLayout.Color();
    // ...Color is not updated if it not found from the skin
    AknsUtils::GetCachedColor( aSkin, color, 
        KNaviPaneMajorColour, KNaviPaneMinorColour );

    if ( iMode == ECamControllerVideo )
        {
		
        iSubtitleVideoTextLayout.DrawText( gc, iSubtitleVideoText->Des(), ETrue, KRgbWhite );    
        aGc.BitBltMasked( iSubtitleVideoRect.Rect().iTl, iSubtitleVideoIcon, 
            iSubtitleVideoIcon->SizeInPixels(), iSubtitleVideoMask, ETrue);        
        }
    else 
        {
	
        iSubtitlePhotoTextLayout.DrawText( gc, iSubtitlePhotoText->Des(), ETrue, KRgbWhite );    
        aGc.BitBltMasked( iSubtitlePhotoRect.Rect().iTl, iSubtitlePhotoIcon, 
            iSubtitlePhotoIcon->SizeInPixels(), iSubtitlePhotoMask, ETrue);                        
        }    
    }    
#endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE



// ---------------------------------------------------------
// CCamNaviCounterModel::SetExtentL
// Sets where the navicounter should be rendered
// ---------------------------------------------------------
//
void CCamNaviCounterModel::SetExtentL( TRect aExtent )
    {
    if ( aExtent != iExtent )
        {
        iExtent = aExtent;
        }
    }
            
// ---------------------------------------------------------
// CCamNaviCounterModel::RegisterObserverL
// Registers an observer
// ---------------------------------------------------------
//
void CCamNaviCounterModel::RegisterObserverL(MCamObserver* aObserver)
    {
    iObserverHandler->RegisterObserverL(aObserver);
    }

// ---------------------------------------------------------
// CCamNaviCounterModel::DeregisterObserver
// Deregisters an observer
// ---------------------------------------------------------
//
void CCamNaviCounterModel::DeregisterObserver(MCamObserver* aObserver)
    {
    iObserverHandler->DeregisterObserver(aObserver);
    }

// ---------------------------------------------------------
// CCamNaviCounterModel::BroadcastEvent
// Broadcasts an event code to all observers
// ---------------------------------------------------------
//
void CCamNaviCounterModel::BroadcastEvent(TCamObserverEvent aEvent)
    {
    iObserverHandler->BroadcastEvent(aEvent);
    }

// ---------------------------------------------------------------------------
// CCamNaviCounterModel::CreateNaviBitmapsL
// Set up the memory icon and counter in the navipane
// ---------------------------------------------------------------------------
// 
void CCamNaviCounterModel::CreateNaviBitmapsL( const TBool aDrawIcons )
	{
	PRINT( _L("Camera => CCamNaviCounterModel::CreateNaviBitmapsL") );
	CCamAppUi* camAppUi = ( CCamAppUi* )iAvkonAppUiBase;
    if ( camAppUi && camAppUi->SettingsLaunchedFromCamera() )
        {
	    CEikStatusPane* sp = camAppUi->StatusPane();

	    TUid npUid ;
	    npUid.iUid = EEikStatusPaneUidNavi;
	    CAknNavigationControlContainer* naviPane = 
	        ( CAknNavigationControlContainer* )sp->ControlL( npUid );
	    
	    // dispose of old decorator and bitmaps  
	    if( iNaviDec )
	        {
	        // do we need to pop it off the navi pane stack first?
		    delete iNaviDec;
		    iNaviDec = NULL;
		    iNaviBitmap = NULL;
		    iNaviBitmapMask = NULL;
	        }

    	if( iNaviBitmap )
	    	{
		    delete iNaviBitmap;
		    iNaviBitmap = NULL;
		    }

	    iNaviBitmap = new ( ELeave ) CFbsBitmap();
	    User::LeaveIfError( iNaviBitmap->Create( TSize(250,20), EColor64K ) );
        CFbsBitmapDevice* bmpDevice = NULL;
        bmpDevice = CFbsBitmapDevice::NewL( iNaviBitmap );
        CleanupStack::PushL( bmpDevice );
        CFbsBitGc* bmpGc = NULL;
        User::LeaveIfError( bmpDevice->CreateContext( bmpGc ) ); 
        CleanupStack::PushL( bmpGc );       

	    if( iNaviBitmapMask )
		    {
		    delete iNaviBitmapMask;
		    iNaviBitmapMask = NULL;
		    }
	    iNaviBitmapMask = new ( ELeave ) CFbsBitmap();
	    User::LeaveIfError( iNaviBitmapMask->Create( TSize(250,20), EColor64K ) );
        CFbsBitmapDevice* bmpMaskDevice = NULL;
        bmpMaskDevice = CFbsBitmapDevice::NewL( iNaviBitmapMask );
        CleanupStack::PushL( bmpMaskDevice );  
        CFbsBitGc* bmpMaskGc = NULL;
        User::LeaveIfError( bmpMaskDevice->CreateContext( bmpMaskGc ) );   
        CleanupStack::PushL( bmpMaskGc );       
        
        if ( aDrawIcons )
            {
            if ( iMode == ECamControllerVideo )
	            {
	            UpdateRecordTimeAvailableL();
    	        }
            DrawCounterToBitmaps( *bmpGc, *bmpMaskGc );
            }
    	// Create the new decorator and push on to navi stack
	    iNaviDec = naviPane->CreateNavigationImageL( iNaviBitmap,
	                                                 iNaviBitmapMask );
    	CleanupStack::PopAndDestroy( bmpMaskGc );
 	    CleanupStack::PopAndDestroy( bmpMaskDevice );
 	    CleanupStack::PopAndDestroy( bmpGc );
 	    CleanupStack::PopAndDestroy( bmpDevice );
    	naviPane->PushL( *iNaviDec );
        }
    PRINT( _L("Camera <= CCamNaviCounterModel::CreateNaviBitmapsL") );
	}

// ---------------------------------------------------------------------------
// CCamNaviCounterModel::NonTouchLayoutL
// ---------------------------------------------------------------------------
void CCamNaviCounterModel::NonTouchLayoutL()   
    {
    TInt variant = 0;
    
    TInt cba =  AknLayoutUtils::CbaLocation() == 
                AknLayoutUtils::EAknCbaLocationLeft? 
                AknLayoutUtils::EAknCbaLocationLeft : 0;

    TAknLayoutRect camIndicatorPane;
    camIndicatorPane.LayoutRect( iExtent, 
            AknLayoutScalable_Apps::cam6_indi_pane( variant ));

    iImgStorageIconRect.LayoutRect( camIndicatorPane.Rect(),
            AknLayoutScalable_Apps::cam6_indi_pane_g2( cba ) );  
   
    iPhotoTextItem->SetLayoutL( camIndicatorPane.Rect(),
                AknLayoutScalable_Apps::cam6_indi_pane_t1( cba ) );  

    TAknLayoutRect vidIndicatorPane;
    vidIndicatorPane.LayoutRect( iExtent,
            AknLayoutScalable_Apps::vid6_indi_pane( cba ) );  

    iVidPreStorageIconRect.LayoutRect( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_g3( cba ) );  
      
    iVideoTextLayout.LayoutText( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t1( cba ) ); 
  
    iVideoTextItem->SetLayoutL( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t1( cba ) ); 

    TAknLayoutRect vidProgressPane;
    vidProgressPane.LayoutRect( iExtent,
            AknLayoutScalable_Apps::vid6_indi_pane( cba ) );  
    
    iSequenceImageTextItem->SetLayoutL(
            vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t4( 2 ) );  

    iTimeLapseCountdownTextItem->SetLayoutL(
            vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t4( 2 ) ); 

    iSequenceCapturedTextItem->SetLayoutL(
            vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t4( 2 ) ); 
    

    // ...Load layout rect for self timer image.
    TAknLayoutRect timerParent;  
    timerParent.LayoutRect( iExtent, 
            AknLayoutScalable_Apps::cam6_timer_pane( cba ) );
    iSelfTimerIconRect.LayoutRect( timerParent.Rect(),   
            AknLayoutScalable_Apps::cam6_timer_pane_g1( cba ) );
    iSelfTimerTextLayout.LayoutText( timerParent.Rect(),  
            AknLayoutScalable_Apps::cam6_timer_pane_t1( cba ) );

    iVideoSizeLayout.LayoutText( vidProgressPane.Rect(), 
            AknLayoutScalable_Apps::vid6_indi_pane_t4( 2 ) ); // Magic: layout Opt2
    iPhotoSizeLayout = iVideoSizeLayout; 

    // ...Load layout rect for sequence image.
    iSequenceIconRect.LayoutRect( iExtent,
            AknLayoutScalable_Apps::cam6_mode_pane_g2( cba ) ); 
     
    iAudioMuteIconRect.LayoutRect( iExtent,  
            AknLayoutScalable_Apps::cam6_mode_pane_g2( cba ) ); 

    }

// ---------------------------------------------------------------------------
// CCamNaviCounterModel::NonTouchLayoutSecondaryL
// ---------------------------------------------------------------------------
void CCamNaviCounterModel::NonTouchLayoutSecondaryL()
    {
    // Magic numbers used for layout variation    
    TAknLayoutRect camIndicatorPane;
    camIndicatorPane.LayoutRect( iExtent, 
            AknLayoutScalable_Apps::cam6_indi_pane( 3 ));

    iImgStorageIconRect.LayoutRect( camIndicatorPane.Rect(),
            AknLayoutScalable_Apps::cam6_indi_pane_g2( 2 ) );
   
     iPhotoTextItem->SetLayoutL( camIndicatorPane.Rect(),
                    AknLayoutScalable_Apps::cam6_indi_pane_t1( 4 ) );

    TAknLayoutRect vidIndicatorPane;
    vidIndicatorPane.LayoutRect( iExtent,
            AknLayoutScalable_Apps::vid6_indi_pane( 3 ) );  

    iVidPreStorageIconRect.LayoutRect( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_g3( 2 ) );
      
    iVideoTextLayout.LayoutText( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t1( 1 ) );
  
    iVideoTextItem->SetLayoutL( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t1( 1 ) );

    iVideoSizeLayout.LayoutText( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_t1( 1 ) );
    iPhotoSizeLayout = iVideoSizeLayout; 

    iAudioMuteIconRect.LayoutRect( iExtent,  
            AknLayoutScalable_Apps::cam6_mode_pane_g1( 0 ) ); 

    }

// ---------------------------------------------------------------------------
// CCamNaviCounterModel::TouchLayoutL
// ---------------------------------------------------------------------------
void CCamNaviCounterModel::TouchLayoutL() 
    {   
    TInt variant = 0; // portrait
    TInt indicatorVariant = 1;
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        // Landscape layout variant
        variant = 1; // landscape
        indicatorVariant = 0;
        }

    TAknLayoutRect camIndicatorPane;
    camIndicatorPane.LayoutRect( iExtent,
            AknLayoutScalable_Apps::cam4_indicators_pane( variant ) );  

    iImgStorageIconRect.LayoutRect( camIndicatorPane.Rect(),
            AknLayoutScalable_Apps::cam4_indicators_pane_g2( indicatorVariant ) );
   
    iPhotoTextItem->SetLayoutL( camIndicatorPane.Rect(),
                AknLayoutScalable_Apps::cam4_indicators_pane_t1( indicatorVariant ) );

    TAknLayoutRect vidIndicatorPane;
    vidIndicatorPane.LayoutRect( iExtent,
            AknLayoutScalable_Apps::vid4_indicators_pane( variant ) );

    iVidPreStorageIconRect.LayoutRect( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid4_indicators_pane_g3( indicatorVariant ) );
      
    #ifdef PRODUCT_SUPPORTS_POST_CAPTURE_INDICATORS
    iVideoFileTypeIconRect.LayoutRect( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid4_indicators_pane_g2( indicatorVariant ) );
    #endif

    iVideoTextLayout.LayoutText( vidIndicatorPane.Rect(),
            AknLayoutScalable_Apps::vid4_indicators_pane_t1( indicatorVariant ) );
  
    iVideoTextItem->SetLayoutL( vidIndicatorPane.Rect(),
                AknLayoutScalable_Apps::vid4_indicators_pane_t1( indicatorVariant ) );

      
    TAknLayoutRect vidProgressPane;
    vidProgressPane.LayoutRect(
            iExtent,
            AknLayoutScalable_Apps::vid4_progress_pane( variant ) );

    iSequenceImageTextItem->SetLayoutL(
            vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid4_progress_pane_t3() );

    iTimeLapseCountdownTextItem->SetLayoutL(
            vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid4_progress_pane_t3() );

    iSequenceCapturedTextItem->SetLayoutL(
            vidProgressPane.Rect(),
            AknLayoutScalable_Apps::vid4_progress_pane_t3() );

    
    // ...Load layout rect for self timer image.
    iSelfTimerIconRect.LayoutRect( iExtent,
            AknLayoutScalable_Apps::main_camera4_pane_g5( variant ) );
    iSelfTimerTextLayout.LayoutText( iExtent,
            AknLayoutScalable_Apps::main_camera4_pane_t1( variant ) );

    #ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        iSubtitlePhotoTextLayout.LayoutText( iExtent, 
                ROID(R_CAM_SUBTITLE_PHOTOMODE_TEXT_LAYOUT_ID));
        iSubtitleVideoTextLayout.LayoutText( iExtent, 
                ROID(R_CAM_SUBTITLE_VIDEOMODE_TEXT_LAYOUT_ID));
        iSubtitlePhotoRect.LayoutRect( iExtent, 
                ROID(R_CAM_SUBTITLE_PHOTOMODE_ICON_RECT_ID));
        iSubtitleVideoRect.LayoutRect( iExtent, 
                ROID(R_CAM_SUBTITLE_VIDEOMODE_ICON_RECT_ID));
        }
    else
        {
        iSubtitlePhotoTextLayout.LayoutText( iExtent, 
                ROID(R_CAM_SUBTITLE_PHOTOMODE_TEXT_LAYOUT_AH_ID));
        iSubtitleVideoTextLayout.LayoutText( iExtent, 
                ROID(R_CAM_SUBTITLE_VIDEOMODE_TEXT_LAYOUT_AH_ID));
        iSubtitlePhotoRect.LayoutRect( iExtent, 
                ROID(R_CAM_SUBTITLE_PHOTOMODE_ICON_RECT_AH_ID));
        iSubtitleVideoRect.LayoutRect( iExtent, 
                ROID(R_CAM_SUBTITLE_VIDEOMODE_ICON_RECT_AH_ID));
        }
            
    // ...Read the strings from the resource    
    iSubtitleVideoText = iCoeEnv->AllocReadResourceAsDes16L( 
            ROID(R_CAM_STILL_PRE_CAPTURE_SUBTITLE_TEXT_VIDEO_ID));
    iSubtitlePhotoText = iCoeEnv->AllocReadResourceAsDes16L( 
            ROID(R_CAM_STILL_PRE_CAPTURE_SUBTITLE_TEXT_PHOTO_ID));
    #endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE

    #ifdef PRODUCT_SUPPORTS_NAVIPANE_FILENAME
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        iPhotoNameLayout.LayoutText( iExtent, 
                ROID(R_CAM_POST_CAPTURE_IMAGE_NAME_LAYOUT_ID));            
        iVideoNameLayout.LayoutText( iExtent, 
                ROID(R_CAM_POST_CAPTURE_VIDEO_NAME_LAYOUT_ID));
        }
    else
        {
        iPhotoNameLayout.LayoutText( iExtent, 
                ROID(R_CAM_POST_CAPTURE_IMAGE_NAME_LAYOUT_AH_ID));
        iVideoNameLayout.LayoutText( iExtent, 
                ROID(R_CAM_POST_CAPTURE_VIDEO_NAME_LAYOUT_AH_ID));
        }

    #endif // PRODUCT_SUPPORTS_NAVIPANE_FILENAME

                
    iVideoSizeLayout.LayoutText( vidProgressPane.Rect(), 
            AknLayoutScalable_Apps::vid4_progress_pane_t3( indicatorVariant ));
        
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        iPhotoSizeLayout.LayoutText( iExtent, 
                ROID(R_CAM_POST_CAPTURE_IMAGE_SIZE_LAYOUT_ID));            
        }
    else
        {
        iPhotoSizeLayout.LayoutText( iExtent, 
                ROID(R_CAM_POST_CAPTURE_IMAGE_SIZE_LAYOUT_AH_ID));
        }

    // ...Load layout rect for sequence image.
    iSequenceIconRect.LayoutRect( iExtent,
            AknLayoutScalable_Apps::main_camera4_pane_g2( variant ) );

    #ifdef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        iGenericIconRect.LayoutRect( iExtent, 
                ROID(R_CAM_GENERIC_SETUP_ICON_RECT_ID));
        }
    else
        {
        iGenericIconRect.LayoutRect( iExtent, 
                ROID(R_CAM_GENERIC_SETUP_ICON_RECT_AH_ID));
        }       
    #endif // PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
            
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi->IsSecondCameraEnabled() && !appUi->IsQwerty2ndCamera() )  
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                mainPaneRect ); 
        iAudioMuteIconRect.LayoutRect(
            mainPaneRect,
            AknLayoutScalable_Apps::main_video4_pane_g2( variant ) );
        }
    else
        {
        iAudioMuteIconRect.LayoutRect(
            iExtent,
            AknLayoutScalable_Apps::main_video4_pane_g3( variant ) );
        }
    }

// End of File
