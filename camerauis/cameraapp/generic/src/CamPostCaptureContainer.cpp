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
* Description:  Container class for still image and video post-capture views*
*/



// INCLUDE FILES
#include <eikmenub.h>
#include <aknview.h>
#include <avkon.hrh>
#include <gulutil.h>
#include <AknUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>            

#include "CamPanic.h"
#include "CamAppUi.h"
#include "CamPostCaptureContainer.h"
#include "CamAppController.h"
#include "CamLogger.h"
#include "CamUtility.h"
#include "CamNaviCounterControl.h"
#include "CamNaviProgressBarControl.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamPostCaptureContainerTraces.h"
#endif



// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamPostCaptureContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamPostCaptureContainer* CCamPostCaptureContainer::NewL(
        CCamAppController& aController,
        CAknView& aView,
        const TRect& aRect )
    {
    CCamPostCaptureContainer* self = new( ELeave ) CCamPostCaptureContainer(
        aController, aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop( self );
    // return newly created CCamPostCaptureContainer instance
    return self;
    }

// Destructor
CCamPostCaptureContainer::~CCamPostCaptureContainer()
  {
  PRINT( _L("Camera => ~CCamPostCaptureContainer" ))

  iController.RemoveControllerObserver( this );
  delete iBatteryPaneController;
  PRINT( _L("Camera <= ~CCamPostCaptureContainer" ))
  }
  
// ---------------------------------------------------------
// CCamPostCaptureContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamPostCaptureContainer::ConstructL( const TRect& aRect )
    {
    CCamContainerBase::BaseConstructL( aRect );
    iController.AddControllerObserverL( this );

    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, iScreenSize );
   CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi()); 
    // Do not construct battery pane controller, if secondary camera is in use
    if ( iController.ActiveCamera() != ECamActiveCameraSecondary || 
         appUi->IsQwerty2ndCamera() ) 
  	    {
	    iBatteryPaneController = CCamBatteryPaneController::NewL( *this, ETrue);
	    }

    if( ECamControllerImage == iController.CurrentMode() )
      {
      // Still mode - processing text needed  
      PrepareProcessingTextL( ETrue );
      } 

    // Make sure fixed toolbar is on top of this window
    DrawableWindow()->SetOrdinalPosition( KCamPostCaptureWindowOrdinalPos );
    }
    
// ---------------------------------------------------------------------------
// CCamPostCaptureContainer::CCamPostCaptureContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamPostCaptureContainer::CCamPostCaptureContainer(
        CCamAppController& aController,
        CAknView& aView )
: CCamContainerBase( aController, aView ),
  iCheckForFileNameChange( EFalse )
    {
    }

// ---------------------------------------------------------
// CCamViewFinderContainer::CountComponentControls 
// Returns the number of controls owned
// ---------------------------------------------------------
//
TInt CCamPostCaptureContainer::CountComponentControls() const
    {
	TInt count = 0;
    if( ( iPaneInUse == ECamPaneCounter ) || ( iPaneInUse == ECamPaneProgress ) )
	    {
	    count++;
		}
		
    if ( !iController.IsTouchScreenSupported() )
        {
        if(iActivePalette && iActivePalette->CoeControl()->IsVisible())
            {
            count++; //Active Palette 
            }
        }
    
	  return count;
    }
    
// -----------------------------------------------------------------------------
// CCamPostCaptureContainer::ComponentControl
// Return specified control owned by this control
// -----------------------------------------------------------------------------
//
CCoeControl* CCamPostCaptureContainer::ComponentControl( TInt aIndex ) const
    {
	CCoeControl* con = NULL;
   	switch( aIndex )
    	{
	    case ECamPostCaptureSubControlNaviPane:
	    	{
	    	switch( iPaneInUse )
	    		{
	    		case ECamPaneCounter:
	    			{
	    			con = iNaviCounterControl;
	    			break;
	    			}
	    		case ECamPaneProgress:
	    			{
	    			con = iNaviCounterControl;
	    			break;
	    			}
	    		case ECamPaneUndefined:
	    			{
	    			break;
	    			}
	    		}
	    	break;
	    	}
	    case ECamPostCaptureActivePalette:
	        {
	        if(iActivePalette && iActivePalette->CoeControl()->IsVisible())	        
	            {
	            con = iActivePalette->CoeControl();
	            }	            
	        break;
	        }
      
        default:
            break;
	    }	        
    return con;	
	
    }

// ---------------------------------------------------------
// CCamPostCaptureContainer::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamPostCaptureContainer::Draw( const TRect& /*aRect*/ ) const
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi());

    if ( appUi && appUi->Embedding() && !appUi->ReadyToDraw() &&  
         iController.ActiveCamera() == ECamActiveCameraPrimary ) 
        {
        // Flickering can be reduced if view is not drawn due to 'wrong' 
        // orientation change. False redraw events may come if device 
        // orientation is changed - even if camera app is in the background.
        // 
        // Primary camera does not support other than landscape mode, thus we
        // must always return to it when embedded application is closed.   
        return;
        }

    if ( appUi && iCheckForFileNameChange )
        {
        // whenever we come back from Properties view
        // we check if the file name has changed already
        appUi->StartFileNameCheck();
        }
    // Draw the current image - snapshot or thumbnail
    CWindowGc& gc = SystemGc();
    gc.SetFaded( EFalse );

    gc.SetBrushColor( KRgbBlack );
    gc.Clear();

    const CFbsBitmap* snapshot = iController.SnapshotImage();
    TRect containerRect = Rect();

    // If snapshot contains a bitmap
    if ( snapshot )
        {
        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMPOSTCAPTURECONTAINER_DRAW, "e_CAM_APP_SNAPSHOT_DRAW 1" ); //CCORAPP_SNAPSHOT_DRAW_START
        
        TSize snapshotSize = snapshot->SizeInPixels();
        TRect bmCropRect( snapshotSize );
        
        // Get the required snapshot layout rect
        TRect vfRect( ViewFinderFrameRect() ); 


        // Standard CIF size snapshot, this could be either a video or
        // still image snapshot. Get the size to display and
        // crop the bitmap if necessary.
        
        if ( iView.Id().iUid == ECamViewIdVideoPostCapture )
            {
            // This is a video snapshot - work out if it needs cropping

            TInt res = iController.GetCurrentVideoResolution();

            if ( res != ECamVideoResolutionNHD )
                {
                // Work out the rectangle of the viewfinder bitmap to draw
                TInt borderX = ( snapshotSize.iWidth - vfRect.Width() ) / 2;
                TInt borderY = ( snapshotSize.iHeight - vfRect.Height() ) / 2;
            	bmCropRect.SetRect( borderX, 
                	                borderY, 
                    	            snapshotSize.iWidth - borderX, 
                        	        snapshotSize.iHeight - borderY );
                }
            }

        gc.BitBlt( vfRect.iTl, snapshot, bmCropRect );
        
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMPOSTCAPTURECONTAINER_DRAW, "e_CAM_APP_SNAPSHOT_DRAW 0" );    //CCORAPP_SNAPSHOT_DRAW_END
        }

  TBool lateOperation = ( ECamCapturing == iController.CurrentOperation() 
                       || ECamCompleting == iController.CurrentOperation() );

  if( iProcessingText &&  //iProcessingBg && 
      iController.ActiveCamera() != ECamActiveCameraSecondary && 
      lateOperation
    )  
    {
    PRINT( _L("Camera <> CCamPostCaptureContainer: draw processing text..") );
    // When iProcessingText exists (and we are in correct state), the processing
    // image text needs to be drawn.
    //iProcessingBg->Draw( gc, Rect() );
    
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ENullBrush );
    iProcessingTextLayout.DrawText( gc, *iProcessingText, ETrue, KRgbBlack ); 
    }

	if( iBatteryPaneController )
		{
		if( iPaneInUse == ECamPaneCounter )
	    		{
	    		iNaviCounterControl->DrawNaviCtr( gc );
	    		}
	    // Draw the battery pane
 	 	iBatteryPaneController->Draw( gc );
		}
	
	 if ( iController.ActiveCamera() == ECamActiveCameraSecondary )
	     {
	     if( iPaneInUse == ECamPaneCounter )
	        {
	        iNaviCounterControl->DrawNaviCtr( gc );
	        }
	     }
     }

// ----------------------------------------------------
// CCamPostCaptureContainer::CheckForFileName
// 
// ----------------------------------------------------
//
void CCamPostCaptureContainer::CheckForFileName( TBool aDoCheck )
    {
    iCheckForFileNameChange = aDoCheck;
    }

// ----------------------------------------------------
// CCamPostCaptureContainer::OfferKeyEventL
// Handles this application view's command keys. Forwards other
// keys to child control(s).
// ----------------------------------------------------
//
TKeyResponse CCamPostCaptureContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi());
	TBool embedded = appUi->IsEmbedded();

    if ( appUi && appUi->CheckCourtesyKeyEventL(aKeyEvent, aType, EFalse) == EKeyWasConsumed )
        {
        return EKeyWasConsumed;
        }

    // only handle the shutter key if not in embedded mode
    if ( !embedded )
        {
        if( aType == EEventKeyUp )
            {
            iController.SetDemandKeyRelease( EFalse ); 
            }
        
        TBool captureKey = IsCaptureKeyL( aKeyEvent, aType );
        
        // consume the ECameraKey and exit post capture
        if ( iView.IsForeground() && 
                ( ( captureKey ) || 
                ( IsShutterKeyL( aKeyEvent, aType ) && 
                  ( appUi->CamOrientation() == ECamOrientationCamcorder || 
                   appUi->CamOrientation() == ECamOrientationCamcorderLeft ) ) )
            )
            {
            if ( aType == EEventKeyDown )
                {
                TInt command = ECamCmdNewPhoto;

                if ( iView.Id().iUid == ECamViewIdVideoPostCapture )
                    {
                    command = ECamCmdNewVideo;
                    }
                iView.HandleCommandL( command );
                // Demand to relese capture key in pre capture view
                iController.SetDemandKeyRelease( ETrue ); 
                }
            return EKeyWasConsumed;
            }
        }
        
    if ( aType == EEventKey )
    	{
    	TKeyResponse keyResponse = EKeyWasNotConsumed;
    	// Check for 'C' key, to be taken as a delete event
        if ( aKeyEvent.iScanCode == EStdKeyBackspace )
	    	{
	        iView.HandleCommandL( ECamCmdDelete );                
	        keyResponse = EKeyWasConsumed;
	        }    
        
		else if ( aKeyEvent.iScanCode == EStdKeyYes )
	    	{
	    	// Only perform the quick send functionality if *not* in 
	    	// embedded mode.  If we are embedded, we are technically
	    	// already in the process of sending.
	    	//
	    	// In touch UI, the send key launches the dialler application.
	    	if ( !embedded && !AknLayoutUtils::PenEnabled() )
	    	    {
	    	    iView.HandleCommandL( ECamCmdQuickSend );        
	    	    }
	        
	        keyResponse = EKeyWasConsumed;
	        }   
		else
		    {
		    // do nothing	
		    }	                 

	    if ( !iController.IsTouchScreenSupported() )
	        {
	        if(iActivePalette && iActivePalette->CoeControl()->OfferKeyEventL(aKeyEvent,aType)==EKeyWasConsumed)
	            {
	            keyResponse = EKeyWasConsumed;
	            }
	        }
        	        
		if ( keyResponse == EKeyWasConsumed )
			{
			return keyResponse;
			}	        
    	} 	
    return CCamContainerBase::OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CCamPostCaptureContainer::HandleControllerEventL
// Handle user menu selections
// -----------------------------------------------------------------------------
//
void CCamPostCaptureContainer::HandleControllerEventL( TCamControllerEvent aEvent, 
                                       TInt /*aError*/ )
  {
  PRINT( _L("Camera => CCamPostCaptureContainer::HandleControllerEventL") );
  // If this is a capture complete event
  if ( aEvent == ECamEventCaptureComplete )
    {
    DrawNow();
    }
  else if ( aEvent == ECamEventSnapshotReady )
    {
    DrawNow();
    }    
  PRINT( _L("Camera <= CCamPostCaptureContainer::HandleControllerEventL") );
  }

// ---------------------------------------------------------------------------
// CCamContainerBase::ShowZoomPaneWithTimer
// Makes Zoom Pane visible for the period of the pane timer.
// ---------------------------------------------------------------------------
//
void CCamPostCaptureContainer::ShowZoomPaneWithTimer()
    {
    // No implementation: only meaningful in pre-capture modes
    PRINT(_L("Camera => CCamPostCaptureContainer::ShowZoomPaneWithTimer() - No implementation!"));
    }

// -----------------------------------------------------------------------------
// CCamPostCaptureContainer::Redraw
// -----------------------------------------------------------------------------
void CCamPostCaptureContainer::Redraw(const TRect& aArea)
    {
    PRINT( _L("Camera => CCamPostCaptureContainer::Redraw") );
    CWindowGc& gc = SystemGc();

    const CFbsBitmap* snapshot = iController.SnapshotImage();
    
	if( iBatteryPaneController )
		{
	    if( iPaneInUse == ECamPaneCounter )
    		{
    		iNaviCounterControl->DrawNaviCtr( gc );
    		}
		}

    // If snapshot contains a bitmap
    if ( snapshot )
        {
        TRect ssRect( ViewFinderFrameRect() ); 
        TRect outdatedArea( aArea );
        TSize snapshotSize = snapshot->SizeInPixels();
        // If necessary, the outdated area (aArea) is moved according to the
        // cropped bitmap.
        if ( iView.Id().iUid == ECamViewIdVideoPostCapture && 
             iController.GetCurrentVideoResolution() != 
             ECamVideoResolutionNHD )
            {
            // Get the required snapshot layout
            // Work out the rectangle of the viewfinder bitmap to draw
            TInt borderX = 
                    ( snapshotSize.iWidth - ssRect.Width() ) / 2;
            TInt borderY = 
                    ( snapshotSize.iHeight - ssRect.Height() ) / 2;

            if ( borderX < 0 )  //the display zone is reduced
            	{
                if ( iActivePalette && iActivePalette->CoeControl()->IsVisible() ) //active palette is exist
                   {
                   gc.SetBrushColor( KRgbBlack );
                   gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
                   gc.Clear( outdatedArea ); //Clear the initial whole zone
                   }
            	}

            outdatedArea.Move( borderX, borderY ); //new zone
            }

         if ( iController.ActiveCamera() == ECamActiveCameraSecondary )
            {
            // Secondary camera snapshot image doesn't fill whole screen.
            // Black stripes are handled here. 
            outdatedArea.Move( 0, -ssRect.iTl.iY );             
            }

        if ( outdatedArea.iBr.iX > snapshotSize.iWidth )
            {
            // Draw black background for the tooltip.
            // Mirrored layout is not supported.
            TRect blackRect = outdatedArea; 
            blackRect.iTl.iX += snapshotSize.iWidth - outdatedArea.iTl.iX;
            gc.SetBrushColor( KRgbBlack );
	        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
	        gc.SetPenStyle( CGraphicsContext::ESolidPen );
            gc.DrawRect( blackRect );
            }

        if ( ssRect.Contains( outdatedArea.iTl ) || !iController.IsTouchScreenSupported() )  
            {
            PRINT( _L("Camera <> CCamPostCaptureContainer::Redraw snapshot") );
            gc.BitBlt( aArea.iTl, snapshot, outdatedArea);
            }
		}
    
    if( iBatteryPaneController )
        {
        iBatteryPaneController->Draw( gc );
        }
    PRINT( _L("Camera <= CCamPostCaptureContainer::Redraw") );
  }

// -----------------------------------------------------------------------------
// CCamPostCaptureContainer::BatteryPaneUpdated
//
// Called by CCamBatteryPaneController when battery pane content has been updated. 
// Re-draws background under the battery pane and the battery pane itself.
// -----------------------------------------------------------------------------    
//
void CCamPostCaptureContainer::BatteryPaneUpdated()
	{
    CCamAppUi* appUi = static_cast<CCamAppUi*>(CEikonEnv::Static()->AppUi());
	
    // during sequence capture the battery icon is drawn by precapture container
    // in sequence postcapture there is not battery icon, so there is no need
    // to redraw battery or processing text here while sequence mode is enabled
	if ( iBatteryPaneController && appUi && !appUi->IsBurstEnabled() )
		{	
		// Get the battery pane rectangle	
		TRect rect = iBatteryPaneController->Rect();	

		// Activate Gc 
		ActivateGc();	

		// Invalidate the battery pane area
		RWindow window = Window();  
		window.Invalidate( rect );
		window.BeginRedraw( rect );			

		// Redraw the background in that area
		iUpdatingBatteryPane = ETrue;
		Redraw( rect );	
		iUpdatingBatteryPane = EFalse;

		// Draw the battery pane
		CWindowGc& gc = SystemGc();

		
		//Redraw the prosessing text before battery icon 
        if ( iProcessingText &&  appUi && 
            iController.ActiveCamera() != ECamActiveCameraSecondary &&
            ( ECamCapturing == iController.CurrentOperation() ||
             ECamCompleting == iController.CurrentOperation() ) )  
            {
            // When iProcessingText exists (and we are in correct state), the processing
            // image text needs to be drawn.
            gc.SetBrushColor( KRgbWhite );
            gc.SetBrushStyle( CGraphicsContext::ENullBrush );
            iProcessingTextLayout.DrawText( gc, *iProcessingText, ETrue, KRgbBlack );      
            }


 	 	//iBatteryPaneController->Draw( gc );
 		 		
 		// Tell the window redraw is finished and deactivate Gc 		
 		window.EndRedraw();
		DeactivateGc();        
		}
	
	}
	
// -----------------------------------------------------------------------------
// CCamPostCaptureContainer::HandleResourceChange
//
// Passes resource changes to battery pane controller
// -----------------------------------------------------------------------------    
//  
void
CCamPostCaptureContainer::HandleResourceChange( TInt aType )
    {  
    if( iBatteryPaneController )
        {
        // Inform battery pane of the resource change	
        iBatteryPaneController->HandleResourceChange( aType );
         }	
  	
    CCamContainerBase::HandleResourceChange( aType );	
    }	


// ---------------------------------------------------------------------------
// HandleAppEvent <<virtual>>
//
// ---------------------------------------------------------------------------
//
void 
CCamPostCaptureContainer::HandleAppEvent( const TCamAppEvent& aEvent )
  {
  PRINT( _L("Camera => CCamPostCaptureContainer::HandleAppEvent") );

  switch( aEvent )
    {
    case ECamAppEventFocusGained:
      {
      iCheckForFileNameChange = ETrue;
      if( iBatteryPaneController )
        iBatteryPaneController->Pause( EFalse );
      break;
      }
    case ECamAppEventFocusLost:
      {
      if( iBatteryPaneController )
        iBatteryPaneController->Pause( ETrue );
      break;
      }
    default:
      {
      break;
      }
    }

  PRINT( _L("Camera <= CCamPostCaptureContainer::HandleAppEvent") );
  }

// End of File  
