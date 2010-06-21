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
* Description:  Control for displaying elapse/remaining record time
*                in Navi Pane
*
*  Copyright (c) 2007-2008 Nokia.  All rights reserved.
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
#include <eikenv.h>
#include <eikapp.h>		// For CEikApplication
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <aknconsts.h>
#include <StringLoader.h> // StringLoader
#include <e32base.h>
#include <barsread.h>    // TResourceReader
#include <AknBidiTextUtils.h>
#include <cameraapp.mbg>
#include <AknLayoutFont.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <avkon.mbg>

#include "CamNaviProgressBarModel.h"
#include "CamAppUi.h"
#include "CamLogger.h"
#include "CamUtility.h"
#include "CamDecorator.h"
#include "CamObserverHandler.h"
#include "camcameraevents.h"
#include "camcameracontrollertypes.h"
#include "camtextitem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamNaviProgressBarModelTraces.h"
#endif


// CONSTANTS
const TInt  KFlashInterval       = 200000;
const TUint KCameraEventInterest = ECamCameraEventClassVideoTimes;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------
// CCamNaviProgressBarModel::NewL
// Factory construction function
// ---------------------------------------------------------
//
CCamNaviProgressBarModel* CCamNaviProgressBarModel::NewL( CCamAppController& aController )
    {
    CCamNaviProgressBarModel* self = new( ELeave ) CCamNaviProgressBarModel( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::~CCamNaviProgressBarModel
// Destructor
// ---------------------------------------------------------
//
CCamNaviProgressBarModel::~CCamNaviProgressBarModel()
  {
  PRINT( _L("Camera => ~CCamNaviProgressBarModel") );

  delete iElapsedTimeTextItem;
  delete iRemainingTimeTextItem;

  UnloadResourceData();

  iController.RemoveControllerObserver( this );
  iController.RemoveCameraObserver( this );
  
  delete iTimeFormat;
  delete iObserverHandler;
  
  if ( iFlashingTimer && iFlashingTimer->IsActive() )
    {
    iFlashingTimer->Cancel();
    }
  delete iFlashingTimer;
  PRINT( _L("Camera <= ~CCamNaviProgressBarModel") );
  }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::CCamNaviProgressBarModel
// C++ constructor
// ---------------------------------------------------------
//
CCamNaviProgressBarModel::CCamNaviProgressBarModel(
    CCamAppController& aController )
: iController( aController )
    {
    }
    
// ---------------------------------------------------------
// CCamNaviProgressBarModel::DrawProgressBar
// Draw Navi-progress
// ---------------------------------------------------------
//
void 
CCamNaviProgressBarModel::DrawProgressBar(       CBitmapContext& aGc, 
                                           const CCoeControl*    aControl ) const
	{
	DrawProgBar( aGc, aControl );
	}

// ---------------------------------------------------------
// CCamNaviProgressBarModel::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamNaviProgressBarModel::ConstructL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMNAVIPROGRESSBARMODEL_CONSTRUCTL, "e_CCamNaviProgressBarModel_ConstructL 1" );
    iController.AddControllerObserverL( this ); 
    iController.AddCameraObserverL( this, KCameraEventInterest ); 

    LoadResourceDataL();

    iObserverHandler = CCamObserverHandler::NewL();

    iFlashingTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMNAVIPROGRESSBARMODEL_CONSTRUCTL, "e_CCamNaviProgressBarModel_ConstructL 0" );
    }

// -----------------------------------------------------------------------------
// CCamNaviProgressBarModel::LoadResourceDataL()
// Reads in all information needed from resources
// -----------------------------------------------------------------------------
//
void CCamNaviProgressBarModel::LoadResourceDataL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMNAVIPROGRESSBARMODEL_LOADRESOURCEDATAL, "e_CCamNaviProgressBarModel_LoadResourceDataL 1" );
    // Create component bitmaps
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
    TPtrC resname = resFileName;

    AknsUtils::CreateIconL(
        skin,
        KAknsIIDQgnGrafMup2BarProgress,
        iProgressBarBitmap,
        iProgressBarMask,
        resname, 
        EMbmCameraappQgn_graf_mup2_bar_progress,
        EMbmCameraappQgn_graf_mup2_bar_progress_mask );

    AknsUtils::CreateIconL(
        skin,
        KAknsIIDQgnGrafMup2BarFrame,
        iProgressBarFrame, 
        resname, 
        EMbmCameraappQgn_graf_mup2_bar_frame );

    delete iElapsedTimeTextItem;
    iElapsedTimeTextItem = NULL;
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );  
    iElapsedTimeTextItem = CCamTextItem::NewL();

    delete iRemainingTimeTextItem;
    iRemainingTimeTextItem = NULL;
    iRemainingTimeTextItem = CCamTextItem::NewL();

    iCamOrientation = appUi->CamOrientation();

    if ( CamUtility::IsNhdDevice() )
        {
        TouchLayoutL();
        }
    else
        {
        if ( ECamOrientationPortrait == appUi->CamOrientation() )
            {
            NonTouchLayoutSecondaryL();
            }
        else
            {
            NonTouchLayoutL();
            }
        }

    // Read the time format resource
    // We have to free the memory first if it has been
    // allocated already
    if ( iTimeFormat )
    	{
    	delete iTimeFormat;
    	iTimeFormat = NULL;
    	}
    if ( ECamOrientationPortrait == appUi->CamOrientation() )
    	{
    	iTimeFormat = CEikonEnv::Static()->AllocReadResourceL( R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO );
    	}
    else 
    	{
    	iTimeFormat = CEikonEnv::Static()->AllocReadResourceL( R_QTN_TIME_DURAT_LONG );
    	}

    TBool progressBarVisible = ETrue;

    if ( progressBarVisible )
        {
        iDecorator = CCamDecorator::NewL( iController, 
                ROID(R_CAM_VIDEO_CAP_LIMIT_RECORDING_ICON_ID), this );
        }
    else
        {
        iDecorator = CCamDecorator::NewL( iController, 
                ROID(R_CAM_VIDEO_CAP_UNLIMIT_RECORDING_ICON_ID), this );
        }
    
    AknIconUtils::SetSize(
        iProgressBarBitmap,
        iProgressBarRect.Rect().Size(),
        EAspectRatioNotPreserved );

    // Progress bar area background
    AknIconUtils::SetSize(
        iProgressBarFrame,
        iProgressBarRect.Rect().Size(),
        EAspectRatioNotPreserved);

    // Create component bitmaps
    TSize size;    
    if ( CamUtility::IsNhdDevice() )
        {
        size =  iProgressIconRect.Rect().Size();
        }
    else
        {
        size =  iVidStorageIconRect.Rect().Size();
        }
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

    AknIconUtils::CreateIconL( iMassStorageIcon, 
                               iMassStorageIconMask, 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_memory_mass,  
                               EMbmCameraappQgn_indi_cam4_memory_mass_mask );

                               
    AknIconUtils::SetSize( iMassStorageIcon, size );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMNAVIPROGRESSBARMODEL_LOADRESOURCEDATAL, "e_CCamNaviProgressBarModel_LoadResourceDataL 0" );
    }
    
// -----------------------------------------------------------------------------
// CCamNaviProgressBarModel::UnloadResourceData()
// Frees all dynamic resources allocated in LoadResourceDataL
// -----------------------------------------------------------------------------
//
void CCamNaviProgressBarModel::UnloadResourceData()
    {
    delete iDecorator;
    iDecorator = NULL;
    
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
    delete iProgressBarBitmap;
    iProgressBarBitmap = NULL;
    delete iProgressBarMask;
    iProgressBarMask = NULL;
    delete iProgressBarFrame;
    iProgressBarFrame = NULL;
    }

// -----------------------------------------------------------------------------
// CCamNaviProgressBarModel::ReloadResourceDataL()
// Refreshes all resource-based information stored in the class
// -----------------------------------------------------------------------------
//
void CCamNaviProgressBarModel::ReloadResourceDataL()
    {
    UnloadResourceData();
    LoadResourceDataL();
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::DrawProgBar
// Draw the control
// ---------------------------------------------------------
//
void 
CCamNaviProgressBarModel::DrawProgBar(       CBitmapContext& aGc, 
                                       const CCoeControl*    /*aControl*/ ) const
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    iDecorator->Draw( aGc, iExtent );

    CFbsBitmap* icon = NULL;
    CFbsBitmap* mask = NULL;
    switch( iStorageLocation )
      {
      case ECamMediaStoragePhone:
          {
          icon = iPhoneIcon;
          mask = iPhoneIconMask;
          }
          break;
      case ECamMediaStorageMassStorage:
          {
          icon = iMassStorageIcon;
          mask = iMassStorageIconMask;
          }
          break;            
      case ECamMediaStorageCard:
          {
          icon = iMMCIcon;
          mask = iMMCIconMask;
          }
          break;            
      case ECamMediaStorageNone:
      default:
          {
          //TODO: Get icons when none is available
          }
          break;
      }

    DrawElapsedTimeText( aGc );
    DrawRemainingTimeText( aGc, skin );
    if ( CamUtility::IsNhdDevice() )
        {
        aGc.BitBltMasked( iProgressIconRect.Rect().iTl,
            icon, icon->SizeInPixels(), mask, ETrue );
        }
    else
        {
        aGc.BitBltMasked( iVidStorageIconRect.Rect().iTl,
            icon, icon->SizeInPixels(), mask, ETrue );
        }
    
    DrawProgressBar( aGc );
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::HandleControllerEventL
// Handle an event from CCamAppController.
// ---------------------------------------------------------
//
void CCamNaviProgressBarModel::HandleControllerEventL(
    TCamControllerEvent aEvent, TInt /* aError */ )
    {
    PRINT( _L("Camera => CCamNaviProgressBarModel::HandleControllerEventL"))

    switch( aEvent )
      {
      // ---------------------------------------------------
      /* Get it from CCameraController now.
      // if this is a record counter update event
      case ECamEventCounterUpdated:
        {
        PRINT( _L("Camera => CCamNaviProgressBarModel::HandleControllerEventL update"))
        iRecordTimeElapsed   = iController.RecordTimeElapsed();
        iRecordTimeRemaining = iController.RecordTimeRemaining();
        FormatTimeL();
        BroadcastEvent( ECamObserverEventNaviModelUpdated );

        // See if we need to start blinking the elapsed time      
        TTime time( iRecordTimeElapsed.Int64() );
        TDateTime recordTime = time.DateTime();

        if ( recordTime.Minute() != 0  && 
            recordTime.Second() == 0 &&
            !iFlashingTimer->IsActive() )
            {
            iFlashingTimer->Start( KFlashInterval,
                                   KFlashInterval,
                                   TCallBack( FlashCallBack, this ) );  
            }
        break;
        }
      */
      // ---------------------------------------------------
      case ECamEventEngineStateChanged:
        {
// <CAMERAAPP_CAPI_V2_MIGRATION/>
//        if ( iController.EngineState() == ECamEngineVideoCapturePrepared )
        if ( ECamCameraPreparedVideo == iController.CameraState() )
          {
          iRecordTimeRemaining = iController.RecordTimeRemaining();
          FormatTimeL();
          BroadcastEvent( ECamObserverEventNaviModelUpdated );
          }
        else
          {
          // empty statement to remove Lint error
          }
        break;
        }
      // ---------------------------------------------------
      case ECamEventOperationStateChanged:
        {
        if ( ECamControllerVideo == iController.CurrentMode()
          && ECamCapturing       == iController.CurrentOperation() )
          {
          // get the current media storage
          iStorageLocation = static_cast< TCamMediaStorage > 
              ( iController.IntegerSettingValue( ECamSettingItemVideoMediaStorage ) );
          }
        else
          {
          // empty statement to remove Lint error
          }
        break;
        }
      // ---------------------------------------------------
      case ECamEventRecordComplete:
        {
        // reset elapsed recording time
        iController.RecordTimeElapsed( iRecordTimeElapsed );
        iRecordTimeElapsed = static_cast<TInt64>(0);
        iFlashingTimer->Cancel();
        iFlash = EFalse;
        break;
        }
      // ---------------------------------------------------
      default:
        {
    		break;
        }
      // ---------------------------------------------------
      }
    PRINT( _L("Camera <= CCamNaviProgressBarModel::HandleControllerEventL"))
    }

// -----------------------------------------------------------------------------
// HandleCameraEventL
//
// -----------------------------------------------------------------------------
//
void 
CCamNaviProgressBarModel
::HandleCameraEventL( TInt              /*aStatus*/, 
                      TCamCameraEventId aEventId, 
                      TAny*             aEventData /*= NULL*/ )
  {
  PRINT( _L("Camera => CCamNaviProgressBarModel::HandleCameraEventL") );
  switch( aEventId )
    {
    // ---------------------------------------------------
    case ECamCameraEventVideoTimes:
      {
      TCamVideoRecordingTimes* times = 
        static_cast<TCamVideoRecordingTimes*>( aEventData );
      if( times )
        {
        iRecordTimeElapsed   = times->iTimeElapsed;
        iRecordTimeRemaining = times->iTimeRemaining;
        
        // If the time we have is greater than the maximum allowed, return the
        // maximum
        TTimeIntervalMicroSeconds maxRecordingLength( static_cast<TInt64>(KMaxRecordingLength) );        
        
        if( iRecordTimeRemaining > maxRecordingLength )
          {
          iRecordTimeRemaining = maxRecordingLength;  
          }
        
        FormatTimeL();
        BroadcastEvent( ECamObserverEventNaviModelUpdated );
  
        // See if we need to start blinking the elapsed time      
        TTime time( iRecordTimeElapsed.Int64() );
        TDateTime recordTime = time.DateTime();
  
        if (  recordTime.Minute() != 0  
          &&  recordTime.Second() == 0 
          &&  !iFlashingTimer->IsActive() )
          {
          iFlashingTimer->Start( KFlashInterval,
                                 KFlashInterval,
                                 TCallBack( FlashCallBack, this ) );  
          }
        }
      break;
      }
    // ---------------------------------------------------
    default:
      break;
    // ---------------------------------------------------
    }
  PRINT( _L("Camera <= CCamNaviProgressBarModel::HandleCameraEventL") );
  }


// -----------------------------------------------------------------------------
// CCamNaviProgressBarModel::FormatTimeL 
// Formats the elapsed/remaining record time 
// -----------------------------------------------------------------------------
//
void CCamNaviProgressBarModel::FormatTimeL()
    {
    TTime time( iRecordTimeElapsed.Int64() );
    time.FormatL( iElapsedTimeText, *iTimeFormat );

    time = iRecordTimeRemaining.Int64();
    time.FormatL( iRemainingTimeText, *iTimeFormat );
    
    if ( iElapsedTimeTextItem )
        {
        iElapsedTimeTextItem->SetTextL( iElapsedTimeText );
        }

    if ( iRemainingTimeTextItem )
        {
        iRemainingTimeTextItem->SetTextL( iRemainingTimeText );
        }
    }

// -----------------------------------------------------------------------------
// CCamNaviProgressBarModel::DrawElapsedTimeText
// Draws the elapsed record time to the navi pane 
// -----------------------------------------------------------------------------
//
void 
CCamNaviProgressBarModel::DrawElapsedTimeText( CBitmapContext& aGc ) const
    {
    if ( !iFlash && iElapsedTimeTextItem )
        {
        iElapsedTimeTextItem->Draw( aGc );
        }
    }

// -----------------------------------------------------------------------------
// CCamNaviProgressBarModel::DrawRemainingTimeText
// Draws the remaining record time to the navi pane 
// -----------------------------------------------------------------------------
//
void 
CCamNaviProgressBarModel::DrawRemainingTimeText( CBitmapContext&    aGc, 
                                                 MAknsSkinInstance* /*aSkin*/ ) const
    {
    if ( iRemainingTimeTextItem )
        {
        iRemainingTimeTextItem->Draw( aGc );
        }
    }

// -----------------------------------------------------------------------------
// CCamNaviProgressBarModel::DrawProgressBar
// Draws the progress bar to the navi pane 
// -----------------------------------------------------------------------------
//
void 
CCamNaviProgressBarModel::DrawProgressBar( CBitmapContext& aGc ) const
    {
    PRINT( _L("Camera => CCamNaviProgressBarModel::DrawProgressBar"))

    // Draw frame
    TRect rect = iProgressBarRect.Rect();

    aGc.BitBlt( rect.iTl, iProgressBarFrame );
    
    // Draw the progress bar

    // Define the area of bitmap that is to be drawn 
    TRect srcRect( 0, 0, 0, iProgressBarBitmap->SizeInPixels().iHeight );
    TInt newWidth = rect.Width();

    // If remaining record time is zero, do not try to perform the percentage
    // calculation, the progress bar will be full
    if ( iRecordTimeRemaining.Int64() > 0 )
        {
        newWidth = static_cast<TInt>(( iRecordTimeElapsed.Int64() * rect.Width() ) 
                      / ( iRecordTimeElapsed.Int64() + iRecordTimeRemaining.Int64() ));
        }
    else
        {
        // empty statement to remove Lint error.
        }

    srcRect.SetWidth( newWidth );

    aGc.BitBltMasked( rect.iTl, iProgressBarBitmap, srcRect, iProgressBarMask, ETrue );
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::SetExtentL
// Sets the extent
// ---------------------------------------------------------
//
void CCamNaviProgressBarModel::SetExtentL( const TRect& aExtent )
  {
  iExtent = aExtent;
  }
    
// ---------------------------------------------------------
// CCamNaviProgressBarModel::RegisterObserverL
// Registers an observer
// ---------------------------------------------------------
//
void CCamNaviProgressBarModel::RegisterObserverL(MCamObserver* aObserver)
    {
    iObserverHandler->RegisterObserverL(aObserver);
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::DeregisterObserver
// Deregisters an observer
// ---------------------------------------------------------
//
void CCamNaviProgressBarModel::DeregisterObserver(MCamObserver* aObserver)
    {
    iObserverHandler->DeregisterObserver(aObserver);
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::BroadcastEvent
// Broadcasts an event to all observers
// ---------------------------------------------------------
//
void CCamNaviProgressBarModel::BroadcastEvent(TCamObserverEvent aEvent)
    {
    iObserverHandler->BroadcastEvent(aEvent);
    }
    
// ---------------------------------------------------------
// CCamNaviProgressBarModel::HandleObservedEvent
// Handles observed events
// ---------------------------------------------------------
//
void CCamNaviProgressBarModel::HandleObservedEvent(TCamObserverEvent aEvent)
    {
    if( aEvent == ECamObserverEventRecordingIconUpdated )
        {
        BroadcastEvent(ECamObserverEventNaviModelUpdated);
        }
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::DoCancel
// Called when the flashing timer elapses
// ---------------------------------------------------------
//
TInt CCamNaviProgressBarModel::FlashCallBack( TAny* aAny )
    {
    CCamNaviProgressBarModel* self = static_cast<CCamNaviProgressBarModel*>( aAny );
    TTime time( self->iRecordTimeElapsed.Int64() );
    TDateTime recordTime = time.DateTime();
    if ( recordTime.Second() != 0 )
        {
        self->iFlashingTimer->Cancel();
        self->iFlash = EFalse;
        }
    else
        {
        self->iFlash = !self->iFlash;
        self->BroadcastEvent( ECamObserverEventNaviModelUpdated );
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::TouchLayoutL
// ---------------------------------------------------------
void CCamNaviProgressBarModel::TouchLayoutL()
    {
    TInt variant = 0; // portrait
    TInt iconVariant = 1;
    if ( Layout_Meta_Data::IsLandscapeOrientation() )
        {
        variant = 1; // landscape
        iconVariant = 0;
        }

    TAknLayoutRect progressPaneLayout;
    progressPaneLayout.LayoutRect( iExtent,
            AknLayoutScalable_Apps::vid4_progress_pane( variant ) );
    
    TRect progressPaneRect = progressPaneLayout.Rect();
    iElapsedTimeTextItem->SetLayoutL( progressPaneRect,
        AknLayoutScalable_Apps::vid4_progress_pane_t1( iconVariant ) );
    iRemainingTimeTextItem->SetLayoutL( progressPaneRect,
        AknLayoutScalable_Apps::vid4_progress_pane_t2( iconVariant ) );
    iProgressIconRect.LayoutRect( progressPaneRect,
        AknLayoutScalable_Apps::vid4_progress_pane_g4( iconVariant ) );

    // Progress bar
    iProgressBarRect.LayoutRect( progressPaneRect,
        AknLayoutScalable_Apps::wait_bar_pane_cp07( iconVariant ) );
    
    // Minimum progress pane rect for drawing while recording
    iProgPaneRect = iRemainingTimeTextItem->Rect();
    iProgPaneRect.BoundingRect( iElapsedTimeTextItem->Rect() );
    iProgPaneRect.BoundingRect( iProgressIconRect.Rect() );
    iProgPaneRect.BoundingRect( iProgressBarRect.Rect() );    
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::NonTouchLayoutL
// Read nontouch layout for primary camera
// ---------------------------------------------------------
void CCamNaviProgressBarModel::NonTouchLayoutL()  
    {
    TInt variant = 0; // Magic: layout Opt1
    
    TInt cba =  1; // Magic: layout Opt1 
        
    TAknLayoutRect progressPaneLayout;
    
    progressPaneLayout.LayoutRect( iExtent,
        AknLayoutScalable_Apps::vid6_indi_pane( variant ) );  
    TRect progressPaneRect = progressPaneLayout.Rect();

    iVidStorageIconRect.LayoutRect( progressPaneRect,
        AknLayoutScalable_Apps::vid6_indi_pane_g3( cba ) );
    iElapsedTimeTextItem->SetLayoutL( progressPaneRect,
        AknLayoutScalable_Apps::vid6_indi_pane_t2( cba ) );
    iRemainingTimeTextItem->SetLayoutL( progressPaneRect,
        AknLayoutScalable_Apps::vid6_indi_pane_t3( cba ) );
    iProgressIconRect.LayoutRect( progressPaneRect,
        AknLayoutScalable_Apps::wait_bar_pane_cp08( 0 ) ); 

    // Progress bar
    iProgressBarRect.LayoutRect( progressPaneRect,
        AknLayoutScalable_Apps::wait_bar_pane_cp08( 0 ) );     
    
    // Minimum progress pane rect for drawing while recording
    iProgPaneRect = iRemainingTimeTextItem->Rect();
    iProgPaneRect.BoundingRect( iElapsedTimeTextItem->Rect() );
    iProgPaneRect.BoundingRect( iProgressIconRect.Rect() );
    iProgPaneRect.BoundingRect( iProgressBarRect.Rect() );    
    }

// ---------------------------------------------------------
// CCamNaviProgressBarModel::NonTouchLayoutSecondaryL
// Read nontouch layout for secondary camera
// ---------------------------------------------------------
void CCamNaviProgressBarModel::NonTouchLayoutSecondaryL()
    {
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
    iExtent = screenRect;
    // Magic numbers used for layout variation
    TAknLayoutRect progressPaneLayout;
    progressPaneLayout.LayoutRect( iExtent,
             AknLayoutScalable_Apps::vid6_indi_pane( 3 ) );  

    iVidStorageIconRect.LayoutRect( progressPaneLayout.Rect(),
            AknLayoutScalable_Apps::vid6_indi_pane_g3( 3 ) );
    
    TRect progressPaneRect = progressPaneLayout.Rect();
    iElapsedTimeTextItem->SetLayoutL( progressPaneRect,
        AknLayoutScalable_Apps::vid6_indi_pane_t2( 2 ) );
    iRemainingTimeTextItem->SetLayoutL( progressPaneRect,
        AknLayoutScalable_Apps::vid6_indi_pane_t3( 2 ) );
    iProgressIconRect.LayoutRect( progressPaneRect,
        AknLayoutScalable_Apps::wait_bar_pane_cp08( 1 ) );

    // Progress bar
    iProgressBarRect.LayoutRect( progressPaneRect,
        AknLayoutScalable_Apps::wait_bar_pane_cp08( 1 ) );
    }
    
// ---------------------------------------------------------
// CCamNaviProgressBarModel::ProgPaneRect
// 
// ---------------------------------------------------------
TRect CCamNaviProgressBarModel::ProgPaneRect()
    {
    return iProgPaneRect;    
    }
                
// End of File
