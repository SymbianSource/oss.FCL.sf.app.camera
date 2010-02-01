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
* Description:  Side Pane control*
*/


// INCLUDE FILES
#include <aknview.h>

#include <eikenv.h>
#include <eikappui.h>   // For CCoeAppUiBase
#include <eikapp.h>     // For CEikApplication

#include <barsc.h>
#include <barsread.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include "CamAppUiBase.h"
#include "CamAppUi.h"

#include "CamAppController.h"
#include "CamSidePane.h"

#include "CamIndicator.h"
#include "CamIndicatorData.h"
#include "CamIndicatorResourceReader.h"
#include "CamTimer.h"
#include "CamSelfTimer.h"
#include "CamLogger.h"
#include "CamSettings.hrh"
#include "CamUtility.h"
#include "CameraUiConfigManager.h"
#include "Cam.hrh"

    
// CONSTANTS


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamSidePane::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamSidePane* CCamSidePane::NewL( CCamAppController& aController,
                                  TBool aOverlayViewFinder )
    {
    CCamSidePane* self = new( ELeave ) CCamSidePane( aController, 
                                                     aOverlayViewFinder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    return self;
    }

// Destructor
CCamSidePane::~CCamSidePane()
  {
  PRINT( _L("Camera => ~CCamSidePane") );
  RemoveObservers();
  CCamSidePane::UnloadResourceData();
  PRINT( _L("Camera <= ~CCamSidePane") );
  }

// ---------------------------------------------------------
// CCamSidePane::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamSidePane::ConstructL()
    {
    LoadResourceDataL();

    // side pane is a controller, self timer and burst mode observer
    RegisterObserversL();
    iVisible = ETrue;

    UpdateLayout();
    }

// -----------------------------------------------------------------------------
// CCamSidePane::LoadResourceDataL()
// Reads in all information needed from resources
// -----------------------------------------------------------------------------
//
void CCamSidePane::LoadResourceDataL()
    {
    TResourceReader reader;                                                                                     
    CEikonEnv* eikEnv = CEikonEnv::Static();
    eikEnv->CreateResourceReaderLC( reader, ROID(R_CAM_SIDEPANE_ID));

    // create reader
    CCamIndicatorResourceReader* resourceReader = 
        CCamIndicatorResourceReader::NewLC( reader );
    // get indicator data from reader   
    CArrayPtrFlat<CCamIndicatorData>& indArray = resourceReader->IndicatorData();

    // set up indicator data
    TInt numindicators = indArray.Count(); // number of indicators
    TInt numbitmaps = 0;
    CCamIndicator* indicator = NULL;

    TInt i = 0;

    for ( i = 0; i < ECamIndicatorTotal; i++ )
        {
        User::LeaveIfError( iIndicators.Append( static_cast<CCamIndicator*>(NULL) ) );
        }

    for ( i = 0; i < numindicators; i++ )
        {
        CCamIndicatorData& indData = *( indArray.At( i ) );
        indicator = CCamIndicator::NewL( indData.IndicatorRect() );
        CleanupStack::PushL( indicator );
        numbitmaps = indData.IndicatorBitmapCount(); // no of bitmaps for indicator
        TInt j;
        for ( j = 0; j < numbitmaps; j++ )
            {
            // side pane assumes that mask bitmap is defined after the
            // normal one in the resource file
            indicator->AddIconL( indData.IndicatorBitmapId( j ), // bitmap
                                 indData.IndicatorBitmapId( j + 1 )); // mask
            j++; // increment past the mask
            }
        iIndicators[indData.IndicatorId()] = indicator;
        CleanupStack::Pop( indicator );
        }

    CleanupStack::PopAndDestroy( resourceReader );
    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CCamSidePane::ReloadResourceDataL()
// Refreshes all resource-based information stored in the class
// -----------------------------------------------------------------------------
//
void CCamSidePane::ReloadResourceDataL()
    {
    UnloadResourceData();
    LoadResourceDataL();
    }

// -----------------------------------------------------------------------------
// CCamSidePane::UnloadResourceData()
// Frees all dynamic resources allocated in LoadResourceDataL
// -----------------------------------------------------------------------------
//
void CCamSidePane::UnloadResourceData()
    {
    iIndicators.ResetAndDestroy();
    iIndicators.Close();
    }

// ---------------------------------------------------------------------------
// CCamSidePane::CCamSidePane
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamSidePane::CCamSidePane( CCamAppController& aController, 
                            TBool aOverlayViewFinder )
:   iController( aController ), 
    iOverlayViewFinder( aOverlayViewFinder )
    {
    }
  
// ---------------------------------------------------------
// CCamSidePane::HandleControllerEventL
// Handle an event from CCamAppController.
// ---------------------------------------------------------
//
void CCamSidePane::HandleControllerEventL(
    TCamControllerEvent aEvent, TInt /* aError */ )
    {
    switch ( aEvent )
        {        
        case ECamEventFlashStateChanged:
            {
            if ( iIndicators[ECamIndicatorFlash]
                && iMode != ECamControllerVideo 
                && !IsSecondaryCameraEnabled() 
                )
                {
                TCamFlashId flash = static_cast< TCamFlashId > 
                    ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoFlash ) );
                switch ( flash )
                    {
                    case ECamFlashAuto:
                        {
                        iIndicators[ECamIndicatorFlash]->SetIcon( 0 );
                        }
                        break;
                    case ECamFlashOff:
                        {
                        iIndicators[ECamIndicatorFlash]->SetIcon( 2 );
                        }
                        break;
                    case ECamFlashAntiRedEye:
                        {
                        iIndicators[ECamIndicatorFlash]->SetIcon( 4 );
                        }
                        break;
                    case ECamFlashForced:
                        {
                        iIndicators[ECamIndicatorFlash]->SetIcon( 6 );
                        }
                        break;
                    default:
                        break;
                    }
                iIndicators[ECamIndicatorFlash]->SetFlashing( EFalse );
                }
            else
                {
                // empty statement to remove Lint error.
                }

            if ( iController.UiConfigManagerPtr()->IsVideoLightSupported() )
                {
                if ( iIndicators[ECamIndicatorVideoFlash]
                    && iMode == ECamControllerVideo 
                    && !IsSecondaryCameraEnabled() 
                    )
                    {
                    TCamFlashId flash = static_cast< TCamFlashId > 
                        ( iController.IntegerSettingValue( ECamSettingItemDynamicVideoFlash ) );
                    switch ( flash )
                        {
                        case ECamFlashOff:
                            {
                            iIndicators[ECamIndicatorVideoFlash]->SetIcon( 0 );
                            }
                            break;
                        case ECamFlashForced:
                            {
                            iIndicators[ECamIndicatorVideoFlash]->SetIcon( 2 );
                            }
                            break;
                        default:
                            break;
                        }
                    iIndicators[ECamIndicatorVideoFlash]->SetFlashing( EFalse );
                    }
                else
                    {
                    // empty statement to remove Lint error.
                    }
                }
            } // end case statement
            break;
        case ECamEventSceneStateChanged:
            {
            if ( iMode == ECamControllerVideo )
                {
                SetVideoSceneIndicator();
                }
            else
                {
                SetPhotoSceneIndicator();
                }
            }
            break;          

        case ECamEventCameraChanged:
            {
            if ( !IsSecondaryCameraEnabled() )
                {
                if ( iMode == ECamControllerVideo )
                    {
                    SetVideoSceneIndicator();
                    if ( !iController.UiConfigManagerPtr()->IsVideoLightSupported() )
                        {
                        if ( iIndicators[ECamIndicatorFlash] )
                            {
                            iIndicators[ECamIndicatorFlash]->ClearIcon();
                            }                        
                        }
                    }
                else
                    {
                    SetPhotoSceneIndicator();
                    if ( !iController.UiConfigManagerPtr()->IsVideoLightSupported() )
                        {
                        if ( iIndicators[ECamIndicatorFlash] )
                            {
                            iIndicators[ECamIndicatorFlash]->DisplayIcon();
                            }                        
                        }
                    }
                }
            }
            break;

#ifdef PRODUCT_USES_GENERIC_SETUP_INDICATOR
#ifndef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
        case ECamEventSetupStateChanged:
            {
            SetGenericSetupIndicator();
            }
            break;
#endif // !PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
#endif // PRODUCT_USES_GENERIC_SETUP_INDICATOR

        case ECamEventVideoStabChanged:
            {
            if ( iController.UiConfigManagerPtr()->IsVideoStabilizationSupported() )
                {
                SetVideoStabilisationIndicator();                
                }
            break;
            }

        case ECamEventFaceTrackingStateChanged:
            {
            if ( iController.UiConfigManagerPtr()->IsFaceTrackingSupported() )
                {
                SetFaceTrackingIndicator();
                }
            break;
            }

        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// CCamSidePane::IsVisible
// Is the side pane invisible
// -----------------------------------------------------------------------------
//
TBool CCamSidePane::IsVisible()
    {
    return iVisible;
    }

// -----------------------------------------------------------------------------
// CCamSidePane::MakeVisible
// Makes the side pane visible
// -----------------------------------------------------------------------------
//
void CCamSidePane::MakeVisible( TBool aVisible )
    {
    iVisible = aVisible;
    }

// ----------------------------------------------------
// CCamSidePane::Draw
// Draws side pane indicators
// ----------------------------------------------------
//
void CCamSidePane::Draw( CBitmapContext& aGc ) const
    {
    // if not overlaying the viewfinder then
    // just clear the side pane area
    if ( !iOverlayViewFinder )
        {
        aGc.Clear( iRect );
        }
    TInt numIndicators = iIndicators.Count();
    TInt i;
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

    if ( appUi )
        {
        appUi->DrawPreCaptureCourtesyUI();
        }

    for ( i = 0; i < numIndicators; i++ )
        {
        if ( iIndicators[i] )
            {
            iIndicators[i]->Draw( aGc );
            }
        }
    }


// ----------------------------------------------------
// CCamSidePane::SetRect
// Sets rect to draw into
// ----------------------------------------------------
//
void 
CCamSidePane::SetRect( TRect& aRect ) 
  {
  iRect.SetRect( aRect.iTl, aRect.iBr );
  }


// ----------------------------------------------------
// CCamSidePane::SetCaptureMode
// Sets rect to draw into
// ----------------------------------------------------
//
void 
CCamSidePane::SetCaptureMode( TCamCameraMode aMode )
  {
  iMode = aMode;
  SetInitialState();
  }

// ----------------------------------------------------
// CCamSidePane::SetInitialState
// Notification that the burst mode has been activated/deactivated
// ----------------------------------------------------
//
void CCamSidePane::SetInitialState()
  {
  CCamAppUiBase* appUi = 
      static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() );

  // set up indicator data
  TInt numindicators = iIndicators.Count(); // number of indicators
  // set initial state for each indicator
  for ( TInt i = 0; i < numindicators; i++ )
    {
    if ( iIndicators[i] )
      {
      switch( i )
        {
        // -------------------------------------------------
        // Scene indicator
        case ECamIndicatorScene:
          {
          if( ECamControllerVideo == iMode )
            SetVideoSceneIndicator();
          else
            SetPhotoSceneIndicator();
          break;
          }
        // -------------------------------------------------
        // Flash indicator
        case ECamIndicatorFlash:
          {
          // don't show the icon if in video mode or the second 
          // camera is active
          // second camera doesn't support flash
          if ( ECamControllerVideo == iMode 
            || IsSecondaryCameraEnabled() 
             )
            {
            iIndicators[ECamIndicatorFlash]->ClearIcon();
            }
          else
            {
            TInt iconIndex = GetFlashIconIndex( iMode );
            if( KErrNotFound != iconIndex )
              {
              iIndicators[ECamIndicatorFlash]->SetIcon( iconIndex );
              }
            }
          break;
          }
        // -------------------------------------------------
        // Video Flash indicator
        case ECamIndicatorVideoFlash:
          {
          // don't show the icon if in video mode or the second 
          // camera is active
          // second camera doesn't support flash
          if ( iController.UiConfigManagerPtr()->IsVideoLightSupported()  )
              {
              if ( ECamControllerVideo != iMode 
                || IsSecondaryCameraEnabled() 
                 )
                {
                iIndicators[ECamIndicatorVideoFlash]->ClearIcon();
                }
              else
                {
                TInt iconIndex = GetFlashIconIndex( iMode );
                if( KErrNotFound != iconIndex )
                  {
                  iIndicators[ECamIndicatorVideoFlash]->SetIcon( iconIndex );
                  }
                }              
              }
          break;
          }
        // -------------------------------------------------
        // Burst mode indicator
        case ECamIndicatorBurstMode:
          {
          if ( appUi && appUi->IsBurstEnabled() )
            {
            iIndicators[ECamIndicatorBurstMode]->SetIcon( 0 );
            }
          else
            {
            iIndicators[ECamIndicatorBurstMode]->ClearIcon();
            }
          break;
          }
        // -------------------------------------------------
        // Self timer indicator
        case ECamIndicatorSelfTimer:
          {
          iIndicators[ECamIndicatorSelfTimer]->ClearIcon();
          break;
          }            
        // -------------------------------------------------
        case ECamIndicatorVideoStabiliser:
          {
          if ( iController.UiConfigManagerPtr()->IsVideoStabilizationSupported() )
              {
              SetVideoStabilisationIndicator();              
              }
          else
              {
              iIndicators[ECamIndicatorVideoStabiliser]->ClearIcon(); 
              }
          break;
          }
        // -------------------------------------------------
        case ECamIndicatorFaceTracking:
          {
          if ( iController.UiConfigManagerPtr()->IsFaceTrackingSupported() )
              {
              SetFaceTrackingIndicator();
              }
          else 
              {
              iIndicators[ECamIndicatorFaceTracking]->ClearIcon();
              }
          break;
          }
        // -------------------------------------------------
        case ECamIndicatorCaptureMode:
          {
          if( appUi && !appUi->IsSecondCameraEnabled() || 
              appUi &&  appUi->IsQwerty2ndCamera() )  
            {
            if ( ECamControllerVideo == iMode ) 
                {
                iIndicators[ECamIndicatorCaptureMode]->SetIcon( 2 );
                }
            else
              iIndicators[ECamIndicatorCaptureMode]->SetIcon( 0 );
            }
          else
            iIndicators[ECamIndicatorCaptureMode]->ClearIcon();
          break;
          }
        // -------------------------------------------------
        // other indicators
        default:
          {
          iIndicators[i]->ClearIcon();
          break;
          }
        // -------------------------------------------------
        } // switch
      }
    } // for
  }


// ---------------------------------------------------------------------------
// GetFlashIconIndex
// ---------------------------------------------------------------------------
//
TInt 
CCamSidePane::GetFlashIconIndex( const TCamCameraMode& aMode )
  {
  TInt index( KErrNotFound );
  if( ECamControllerVideo == aMode )
    {
    TCamFlashId flashId = 
      static_cast<TCamFlashId>(
        iController.IntegerSettingValue( ECamSettingItemDynamicVideoFlash ) );
    switch ( flashId )
      {
      case ECamFlashOff:    index = 0; break;
      case ECamFlashForced: index = 2; break;
      default:                         break;
      }
    }
  else
    {
    TCamFlashId flashId = 
      static_cast<TCamFlashId> ( 
        iController.IntegerSettingValue( ECamSettingItemDynamicPhotoFlash ) );

    switch ( flashId )
      {
      case ECamFlashAuto:       index = 0; break;
      case ECamFlashOff:        index = 2; break;
      case ECamFlashAntiRedEye: index = 4; break;
      case ECamFlashForced:     index = 6; break;

      default:                             break;
      }
    }
  return index;
  }


// ----------------------------------------------------
// CCamSidePane::RegisterObserversL
// Register with observed classes
// ----------------------------------------------------
//
void CCamSidePane::RegisterObserversL()
    {
    CEikonEnv* eikEnv = CEikonEnv::Static();
    CCamAppUi* appUi = static_cast<CCamAppUi*>( eikEnv->AppUi() );

    if ( appUi )
        {
// burst mode icon is displayed in either the navipane or sidepane
#ifndef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 
        // Register as burst mode observer
        appUi->AddBurstModeObserverL( this );
#endif // !PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE
        }
    // Register as controller observer
    iController.AddControllerObserverL( this );
    }

// ----------------------------------------------------
// CCamSidePane::RemoveObservers
// Dergister with observed classes
// ----------------------------------------------------
//
void CCamSidePane::RemoveObservers()
    {
    iController.RemoveControllerObserver( this );
    // if shutting down then self timer will remove the observer
    if ( !iController.IsInShutdownMode() )
        {
        CEikonEnv* eikEnv = CEikonEnv::Static();
        CCamAppUi* appUi  = static_cast<CCamAppUi*>( eikEnv->AppUi() );
        if ( appUi )
            {
// burst mode icon is displayed in either the navipane or sidepane
#ifndef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 
            // Deregister as burst mode observer
            appUi->RemoveBurstModeObserver( this );
#endif // !PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE
            }
        }
    }

#ifdef PRODUCT_USES_GENERIC_SETUP_INDICATOR
#ifndef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
// ----------------------------------------------------
// CCamSidePane::SetDefaultSetupIndicator
// Sets the default setup indicator depending on the 
// current setup settings.
// ----------------------------------------------------
//
void CCamSidePane::SetGenericSetupIndicator()
    {
    if ( iIndicators[ECamIndicatorGenericSetup] )
        {
        // If the current mode is video and the video setup settings
        // are generic to the current video scene then do not show 
        // the generic setup tampered indicator.
        if ( ( iMode == ECamControllerVideo ) && 
             ( iController.VideoSceneDefaultsAreSet() ) )
            {
            iIndicators[ECamIndicatorGenericSetup]->ClearIcon();
            }
        // Otherwise, if the current mode is photo and the photo setup settings
        // are generic to the current photo scene then do not show 
        // the generic setup tampered indicator.
        else if ( ( iMode != ECamControllerVideo ) &&
                  ( iController.PhotoSceneDefaultsAreSet() ) )
            {
            iIndicators[ECamIndicatorGenericSetup]->ClearIcon();
            }
        // Otherwise do not show the indicator.
        else
            {
            iIndicators[ECamIndicatorGenericSetup]->SetIcon( 0 );
            }
        }
    }
#endif // !PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
#endif // PRODUCT_USES_GENERIC_SETUP_INDICATOR


// ----------------------------------------------------
// CCamSidePane::SetVideoStabilisationIndicator
// Sets the image stabilisation indicator for video capture mode
// ----------------------------------------------------
//
void CCamSidePane::SetVideoStabilisationIndicator()
    {
    if ( iController.UiConfigManagerPtr()->IsVideoStabilizationSupported() )
        {
        if ( iIndicators[ECamIndicatorVideoStabiliser] )
            {
            TCamSettingsOnOff stabilisation  = static_cast< TCamSettingsOnOff > 
                ( iController.IntegerSettingValue( ECamSettingItemVideoStab ) );
                
            // If the current mode is video
            if ( stabilisation == ECamSettOn && 
                    iMode == ECamControllerVideo &&
                    !IsSecondaryCameraEnabled() )
                {
                iIndicators[ECamIndicatorVideoStabiliser]->SetIcon( 0 );
                } 
            // Otherwise, draw nothing.
            else
                {
                iIndicators[ECamIndicatorVideoStabiliser]->ClearIcon();
                }
            }        
        }
    }

// ----------------------------------------------------
// CCamSidePane::SetFaceTrackingIndicator
// Sets the facetracking indicator for video capture mode
// ----------------------------------------------------
//
void CCamSidePane::SetFaceTrackingIndicator()
    {
    if ( iController.UiConfigManagerPtr()->IsFaceTrackingSupported() )
        {
        if ( iIndicators[ECamIndicatorFaceTracking] )
            {
            TCamSettingsOnOff facetracking  = static_cast< TCamSettingsOnOff > 
                ( iController.IntegerSettingValue( ECamSettingItemFaceTracking ) );
                
            // If the current mode is still image
            if ( facetracking == ECamSettOn && iMode == ECamControllerImage && !IsSecondaryCameraEnabled() ) 
                {
                iIndicators[ECamIndicatorFaceTracking]->SetIcon( 0 );
                } 
            // Otherwise, draw nothing.
            else
                {
                iIndicators[ECamIndicatorFaceTracking]->ClearIcon();
                }
            }        
        }
    }

// ----------------------------------------------------
// CCamSidePane::SetVideoSceneIndicator
// Set the scene indicator depending on the current video scene setting
// ----------------------------------------------------
//
void CCamSidePane::SetVideoSceneIndicator()
    {
    if ( iIndicators[ECamIndicatorScene] )
        {
        TCamSceneId scene = static_cast< TCamSceneId > ( 
            iController.IntegerSettingValue( ECamSettingItemDynamicVideoScene ) );
        switch ( scene )
            {
            case ECamSceneNormal:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 0 );
                }
                break;
            case ECamSceneNight:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 2 );
                }
                break;
            case ECamSceneUser:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 4 );
                }
                break;
            default:
                {
                iIndicators[ECamIndicatorScene]->ClearIcon();
                }
                break;
            }
        }
    }

// ----------------------------------------------------
// CCamSidePane::SetPhotoSceneIndicator
// Set the scene indicator depending on the current photo scene setting
// ----------------------------------------------------
//
void CCamSidePane::SetPhotoSceneIndicator()
    {
    if ( iIndicators[ECamIndicatorScene] )
        {
        TCamSceneId scene = static_cast< TCamSceneId > ( 
            iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) );
        switch ( scene )
            {
            case ECamSceneAuto:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 4 );
                }
                break;
            case ECamSceneUser:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 6 );
                }
                break;
            case ECamSceneMacro:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 8 );
                }
                break;
            case ECamScenePortrait:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 10 );
                }
                break;
            case ECamSceneScenery:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 12 );
                }
                break;
            case ECamSceneNight:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 14 );
                }
                break;
            case ECamSceneSports:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 16 );
                }
                break;
            case ECamSceneNightScenery:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 22 );
                }
                break;
            case ECamSceneNightPortrait:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 24 );
                }
                break;
            case ECamSceneCandlelight:
                {
                iIndicators[ECamIndicatorScene]->SetIcon( 26 );
                }
                break;
            default:
                {
                iIndicators[ECamIndicatorScene]->ClearIcon();
                }
                break;
            }
        }
    }  

// ----------------------------------------------------
// CCamSidePane::BurstModeActiveL
// Notification that the burst mode has been activated/deactivated
// ----------------------------------------------------
//
// Burst mode icon is displayed in either the navipane or sidepane
#ifndef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 
void CCamSidePane::BurstModeActiveL( TBool aActive, TBool /*aStillModeActive*/ )
    {
    if ( iIndicators[ECamIndicatorBurstMode] )
        {
        // If burst mode has been activated
        if ( aActive )
            {
            iIndicators[ECamIndicatorBurstMode]->SetIcon( 0 );
            }
        else // otherwise, burst mode has been deactivated
            {
            iIndicators[ECamIndicatorBurstMode]->ClearIcon();
            }
        }
    }
#endif // !PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE 

// ----------------------------------------------------
// CCamSidePane::IsSecondaryCameraEnabled
// Checks if the secondary camera is enabled
// ----------------------------------------------------
//
TBool CCamSidePane::IsSecondaryCameraEnabled() const
    {
    return static_cast<CCamAppUiBase*>( 
        CEikonEnv::Static()->AppUi() )->IsSecondCameraEnabled();
    }

// ----------------------------------------------------
// CCamSidePane::UpdateLayout
// ----------------------------------------------------
//
void CCamSidePane::UpdateLayout()
    {
    if ( CamUtility::IsNhdDevice() )
        {
        TouchLayout();
        }
    else
        {
        NonTouchLayout();
        }
    }

// ----------------------------------------------------
// CCamSidePane::NonTouchLayout
// ----------------------------------------------------
void CCamSidePane::NonTouchLayout() const
    {
    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, rect );
    TInt cba =  AknLayoutUtils::CbaLocation() == 
                AknLayoutUtils::EAknCbaLocationLeft;
    TAknLayoutRect parent;
    parent.LayoutRect( rect, AknLayoutScalable_Apps::cam6_mode_pane( cba ) );  

    // update coords for each indicator
    for ( TInt i = 0; i < iIndicators.Count(); i++ )
        {
        TAknLayoutRect l;
        if ( iIndicators[i] )
            {
            switch ( i )
                {
                case ECamIndicatorCaptureMode:
                    {
                    l.LayoutRect( parent.Rect(),
                        AknLayoutScalable_Apps::cam6_mode_pane_g1( cba ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }
                case ECamIndicatorBurstMode:
                    {
                    l.LayoutRect( parent.Rect(),
                        AknLayoutScalable_Apps::cam6_mode_pane_g2( cba ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }    
                case ECamIndicatorImageShakeWarning:
                    {
                    l.LayoutRect( parent.Rect(),
                        AknLayoutScalable_Apps::cam6_mode_pane_g3( cba ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }
                case ECamIndicatorVideoStabiliser: 
                    {
                    l.LayoutRect( parent.Rect(),
                        AknLayoutScalable_Apps::cam6_mode_pane_g3( cba ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }
                case ECamIndicatorFaceTracking: 
                    {
                    l.LayoutRect( parent.Rect(),
                        AknLayoutScalable_Apps::cam6_mode_pane_g4( cba ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCamSidePane::TouchLayout
// ---------------------------------------------------------------------------
void CCamSidePane::TouchLayout() const
    {  
    TSize screenSize;
    AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, screenSize );
    TRect rect( TPoint(), screenSize );
    TInt variant = Layout_Meta_Data::IsLandscapeOrientation();

    // update coords for each indicator
    for ( TInt i = 0; i < iIndicators.Count(); i++ )
        {
        TAknLayoutRect l;
        if ( iIndicators[i] )
            {
            switch ( i )
                {
                case ECamIndicatorCaptureMode:
                    {
                    if ( ECamControllerVideo == iMode )
                        {
                        l.LayoutRect( rect,
                           AknLayoutScalable_Apps::main_video4_pane_g1( variant ) );
                        }
                    else
                        {
                        l.LayoutRect( rect,
                           AknLayoutScalable_Apps::main_camera4_pane_g1( variant ) );
                        }
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }
                case ECamIndicatorBurstMode:
                    {
                    l.LayoutRect( rect,
                        AknLayoutScalable_Apps::main_camera4_pane_g2( variant ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }                    
                case ECamIndicatorImageShakeWarning:
                    {
                    l.LayoutRect( rect,
                        AknLayoutScalable_Apps::main_camera4_pane_g3( variant ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }
                case ECamIndicatorVideoStabiliser: 
                    {
                    l.LayoutRect( rect,
                            AknLayoutScalable_Apps::main_video4_pane_g3( variant ) );
                    iIndicators[i]->SetRect( l.Rect() );
                    break;
                    }
                case ECamIndicatorFaceTracking: 
                    {
                     l.LayoutRect( rect,
                       AknLayoutScalable_Apps::main_camera4_pane_g2( variant ) );
                     iIndicators[i]->SetRect( l.Rect() );
                     break;  
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }
    }
   
// End of File
