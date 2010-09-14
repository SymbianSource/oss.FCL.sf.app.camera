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
* Description:  Camera Zoom Pane; shows the zoom level graphically*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <coemain.h>
#include <eikenv.h>
#include <cameraapp.mbg>
#include <eikappui.h>    // For CCoeAppUiBase
#include <eikapp.h>      // For CEikApplication
#include <barsread.h>    // resource reader
#include <AknIconUtils.h>
#include <centralrepository.h>
#include <layoutmetadata.cdl.h>         
#include <aknlayoutscalable_apps.cdl.h> 
#include <touchfeedback.h> 
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include "CameraUiConfigManager.h"
#include "CamPanic.h"    // Panic codes
#include "CamUtility.h"
#include "CamZoomPane.h"
#include "CamZoomModel.h"
#include "CamAppUi.h"
#include "CamBmpRotatorAo.h"
#include "CameraappPrivateCRKeys.h"
#include "CamTimer.h"


// CONSTANTS
const TInt KDivisorFactor = 10000;    // integer scaling factor to avoid the 
                                     // use of floating point arithmetic
const TInt KTouchAreaExpansion = 35; // Pixels to grow touchable area                                    
const TInt KFastZoomMultiplier = 4;  // Multiplier to skip some zoom levels 
                                     // to make zooming faster. Used with double tap.

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamZoomPane::CCamZoomPane
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamZoomPane::CCamZoomPane( CCamAppController& aController, TBool aOverlayViewFinder )
    : iController( aController ),
      iOverlayViewFinder( aOverlayViewFinder )
    {
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamZoomPane::ConstructL()
    {
    PRINT( _L( "Camera => CCamZoomPane::ConstructL " ) );
    // Load the zoom pane orientation value from the Central Repository
 	CRepository* repository = CRepository::NewL( KCRUidCameraappSettings );        
 	TInt val = 0;
    TInt err = repository->Get( KCamCrZoomPaneOrientation, val );

    // If there is an error then assume standard (vertical) orientation
    if ( err != KErrNone )
        {
        iOrientation = EZPOrientationVertical;
        }
    else 
        {
        iOrientation = static_cast<TZPOrientation>( val );
        }
#if !( defined(__WINS__) || defined(__WINSCW__) )
    // Get Central Repository key indicating if product uses volume keys for zoom
    err = repository->Get( KCamCrZoomUsingVolumeKeys, val );

    // If there is an error then assume volume keys not used
    if ( err != KErrNone )
        {
        iZoomUsingVolumeKeys = EFalse;
        }
    else 
        {
        iZoomUsingVolumeKeys = val;
        }
    
        iZoomUsingVolumeKeys = EFalse;
    
    // Get Central Repository key indicating if product uses navi-keys for zoom
    err = repository->Get( KCamCrZoomUsingNaviKeys, val );

    // If there is an error then assume navi-keys not used
    if ( err != KErrNone )
        {
        iZoomUsingNaviKeys = EFalse;
        }
    else 
        {
        iZoomUsingNaviKeys = val;
        }
#else
    iZoomUsingVolumeKeys = EFalse;   
    iZoomUsingNaviKeys = ETrue;  
#endif // !( defined(__WINS__) || defined(__WINSCW__) )       
        
    delete repository;
    repository = NULL;
        
    // Find the name and path of the MBM file for zoom pane bitmaps
    TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
    TPtrC resname = resFileName;
           
    // Create component bitmaps 
    AknIconUtils::CreateIconL( iIconZoomTop[0], 
                               iIconZoomTop[1], 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_zoom_top, 
                               EMbmCameraappQgn_indi_cam4_zoom_top_mask );

    AknIconUtils::CreateIconL( iIconZoomMiddle[0], 
                               iIconZoomMiddle[1], 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_zoom_middle, 
                               EMbmCameraappQgn_indi_cam4_zoom_middle_mask );
                            
    AknIconUtils::CreateIconL( iIconZoomBottom[0], 
                               iIconZoomBottom[1], 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_zoom_bottom, 
                               EMbmCameraappQgn_indi_cam4_zoom_bottom_mask );   
        
    AknIconUtils::CreateIconL( iIconZoomMarker[0], 
                               iIconZoomMarker[1], 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_zoom_marker, 
                               EMbmCameraappQgn_indi_cam4_zoom_marker_mask );

    AknIconUtils::CreateIconL( iIconZoomMarkerGray[0], 
                               iIconZoomMarkerGray[1], 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_zoom_marker_selected, 
                               EMbmCameraappQgn_indi_cam4_zoom_marker_selected_mask );

    AknIconUtils::CreateIconL( iIconZoomMin[0], 
                               iIconZoomMin[1], 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_zoom_min, 
                               EMbmCameraappQgn_indi_cam4_zoom_min_mask );
                               
    AknIconUtils::CreateIconL( iIconZoomMax[0], 
                               iIconZoomMax[1], 
                               resname, 
                               EMbmCameraappQgn_indi_cam4_zoom_max, 
                               EMbmCameraappQgn_indi_cam4_zoom_max_mask );   

    SizeChanged(); // Initialize layouts and set sizes   
    CamUtility::SetAlphaL( iIconZoomTop[0], iIconZoomTop[1] ); 
    CamUtility::SetAlphaL( iIconZoomMiddle[0], iIconZoomMiddle[1] ); 
    CamUtility::SetAlphaL( iIconZoomBottom[0], iIconZoomBottom[1] );  
    CamUtility::SetAlphaL( iIconZoomMarker[0], iIconZoomMarker[1] );  
    CamUtility::SetAlphaL( iIconZoomMarkerGray[0], iIconZoomMarkerGray[1] ); 
    CamUtility::SetAlphaL( iIconZoomMin[0], iIconZoomMin[1] ); 
    CamUtility::SetAlphaL( iIconZoomMax[0], iIconZoomMax[1] );  
    iIconZoomMarkerCurrent = iIconZoomMarker[0]; 

    // Retrieve product-specific information about zoom support
    CamUtility::GetPsiInt( ECamPsiVideoZoomSupport, iZoomSupport );

    // Retrieve the timing and step values for each of the zoom bar 
    // segments/modes eg Optical/Digital/Extended    
    TPckgBuf <TCamZoomLAF> pckg;
    CamUtility::GetPsiAnyL( ECamPsiZoomBarLAF, &pckg);
    iZoomLAF = pckg();
        
    // Register for notification of controller events
    iController.AddControllerObserverL( this );    
    
    iModel = CCamZoomModel::NewL( iController, this );
    iFeedback = MTouchFeedback::Instance();   

    PRINT( _L( "Camera <= CCamZoomPane::ConstructL " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::LoadResourceDataL()
// Reads in all information needed from resources
// -----------------------------------------------------------------------------
//
void CCamZoomPane::LoadResourceDataL()
    {
    PRINT( _L( "Camera => CCamZoomPane::LoadResourceDataL " ) );
    ReadLayoutL();
    PRINT( _L( "Camera <= CCamZoomPane::LoadResourceDataL " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::UnloadResourceData()
// Frees all dynamic resources allocated in LoadResourceDataL
// -----------------------------------------------------------------------------
//
void CCamZoomPane::UnloadResourceData()
    {
    PRINT( _L( "Camera => CCamZoomPane::UnloadResourceData " ) );
    PRINT( _L( "Camera <= CCamZoomPane::UnloadResourceData " ) );
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::ReloadResourceDataL()
// Refreshes all resource-based information stored in the class
// -----------------------------------------------------------------------------
//
void CCamZoomPane::ReloadResourceDataL()
    {
    PRINT( _L( "Camera => CCamZoomPane::ReloadResourceDataL " ) );
    UnloadResourceData();
    LoadResourceDataL();
    PRINT( _L( "Camera <= CCamZoomPane::ReloadResourceDataL " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamZoomPane* CCamZoomPane::NewL( CCamAppController& aController,
                                  TBool aOverlayViewFinder )
    {
    PRINT( _L( "Camera => CCamZoomPane::NewL " ) );
    CCamZoomPane* self = new( ELeave ) CCamZoomPane ( aController, aOverlayViewFinder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    PRINT( _L( "Camera <= CCamZoomPane::NewL " ) );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::~CCamZoomPane
// Destructor.
// -----------------------------------------------------------------------------
//
CCamZoomPane::~CCamZoomPane()
    {
    PRINT( _L("Camera => ~CCamZoomPane" ))  
    iController.RemoveControllerObserver( this );

    delete iIconZoomTop[0];  
    delete iIconZoomMiddle[0];
    delete iIconZoomBottom[0];
    delete iIconZoomMarker[0];
    delete iIconZoomMarkerGray[0]; 
    delete iIconZoomMin[0];
    delete iIconZoomMax[0];
    delete iIconZoomTop[1];  
    delete iIconZoomMiddle[1];
    delete iIconZoomBottom[1];
    delete iIconZoomMarker[1];
    delete iIconZoomMarkerGray[1]; 
    delete iIconZoomMin[1];
    delete iIconZoomMax[1];
 
    delete iModel;
  
    if ( iRotatorAo &&
         iRotatorAo->IsActive() )
        {
        iRotatorAo->Cancel();
        }
    delete iRotatorAo;


    iZoomInKeys.Close();
    iZoomOutKeys.Close();

    PRINT( _L("Camera <= ~CCamZoomPane" ))  
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::Draw
// Draws the zoom pane
// -----------------------------------------------------------------------------
//
void CCamZoomPane::Draw( CBitmapContext& aGc ) const
    {
    PRINT( _L( "Camera => CCamZoomPane::Draw " ) );
    if ( !iMaxZoom )
        {        
        PRINT( _L( "Camera <= CCamZoomPane::Draw not set up yet" ) );
        return; // Not set up yet
        }
        
    if ( !iOverlayViewFinder )
        {
        aGc.Clear( iZoomPaneRect );
        }
    
    DrawPlusMinus( aGc );
    DrawShaft( aGc );
    DrawThumb( aGc );    
    PRINT( _L( "Camera <= CCamZoomPane::Draw " ) );
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::DrawPlusMinus
// Draws the plus and minus icons
// -----------------------------------------------------------------------------
//    
void CCamZoomPane::DrawPlusMinus( CBitmapContext& aGc ) const
    {    
    PRINT( _L( "Camera => CCamZoomPane::DrawPlusMinus " ) );
    aGc.BitBlt( iPlusPoint,  iIconZoomMax[0] );    
    aGc.BitBlt( iMinusPoint, iIconZoomMin[0] );   
    PRINT( _L( "Camera <= CCamZoomPane::DrawPlusMinus " ) );                 
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::DrawShaft
// Draws the shaft of the zoom pane
// -----------------------------------------------------------------------------
//    
void CCamZoomPane::DrawShaft( CBitmapContext& aGc ) const  
    {
    PRINT( _L( "Camera => CCamZoomPane::DrawShaft " ) );
    aGc.BitBlt( iTopPoint,    iIconZoomTop[0] );
    aGc.BitBlt( iMiddlePoint, iIconZoomMiddle[0] );
    aGc.BitBlt( iBottomPoint, iIconZoomBottom[0] );
    PRINT( _L( "Camera <= CCamZoomPane::DrawShaft " ) );
    }
   
// -----------------------------------------------------------------------------
// CCamZoomPane::SetPoint
// Sets the value of a point based on another point and an (optional) modifier value.
// This was added to simplify the bar drawing code, so the changes based on orientation 
// are (as much as possible) contained in this function
// -----------------------------------------------------------------------------
//    
void 
CCamZoomPane::SetPoint(       TPoint& aPoint, 
                        const TPoint& aOffset, 
                              TInt    aAdditional ) const
    {
    PRINT( _L( "Camera => CCamZoomPane::SetPoint " ) );
    if ( PaneOrientation() == EZPOrientationVertical )
        {
        aPoint.iY = aOffset.iY + aAdditional;
        }
    else
        {
        aPoint.iX = aOffset.iX - aAdditional;
        }
    PRINT( _L( "Camera <= CCamZoomPane::SetPoint " ) );
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::PaneOrientation
// Returns the current zoom pane orientation (taking into account whether this
// should be forced or not if on secondary camera)
// -----------------------------------------------------------------------------
//        
CCamZoomPane::TZPOrientation CCamZoomPane::PaneOrientation() const
    {
    PRINT( _L( "Camera => CCamZoomPane::PaneOrientation " ) );
    if ( iController.ActiveCamera() == ECamActiveCameraSecondary )
        {
        PRINT( _L( "Camera <= CCamZoomPane::PaneOrientation EZPOrientationVertical" ) );
        return EZPOrientationVertical;
        }
    else 
        {
        PRINT( _L( "Camera <= CCamZoomPane::PaneOrientation iOrientation" ) );
        return iOrientation;
        }
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::DrawThumb
// Draws the thumb of the zoom pane
// -----------------------------------------------------------------------------
//       
void CCamZoomPane::DrawThumb( CBitmapContext& aGc ) const
    {   
    PRINT( _L( "Camera => CCamZoomPane::DrawThumb " ) );
    TPoint newThumbPos;                            
    // Knob bitmap has an empty area on the edge, thus it starts upper than top
    // of zoom slider 
    // topPointY is iTl.iY of knob bitmap when maximum zoom is used. 
    newThumbPos.iX = iZoomPaneRect.iTl.iX;        
        
    TInt pixelsPerStep = ( iMaxOffset * KDivisorFactor ) / (iMaxZoom+1);
    TInt pixelsFromBase;
    pixelsFromBase = ( iTrgZoom * pixelsPerStep );
    newThumbPos.iY = iTopPoint.iY + iMaxOffset -
             (pixelsFromBase + pixelsPerStep/2 +
              iIconZoomMarker[0]->SizeInPixels().iHeight * 
              KDivisorFactor / 2) / KDivisorFactor;                                                        
        
    // Centralize thumb and slider X-axis.
    TRect old = TRect(newThumbPos,iIconZoomMarkerCurrent->SizeInPixels());
    newThumbPos.iX += iSliderParentRect.Center().iX - old.Center().iX;

    aGc.BitBlt( newThumbPos, iIconZoomMarkerCurrent ); 
    PRINT( _L( "Camera <= CCamZoomPane::DrawThumb " ) );                   
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::SetRect
// Sets the rect that zoom pane is drawn into
// -----------------------------------------------------------------------------
//
void CCamZoomPane::SetRect( TRect aRect )
    {
    PRINT( _L( "Camera => CCamZoomPane::SetRect " ) );
    iZoomPaneRect = aRect;
    PRINT( _L( "Camera <= CCamZoomPane::SetRect " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::SetZoomRange
// Specifies the range of values zoom supports
// aMax should always be more than aMin. 
// -----------------------------------------------------------------------------
//
TInt CCamZoomPane::SetZoomRange( TInt aMin, TInt aMax )
    {        
    PRINT( _L( "Camera => CCamZoomPane::SetZoomRange " ) );
    // The minimum value should always be less than the max.
    if ( aMin >= aMax ||
         aMin < 0 )
        {    
        PRINT( _L( "Camera <= CCamZoomPane::SetZoomRange KErrArgument" ) );
        return KErrArgument;
        }

    iMinZoom = aMin;
    iMaxZoom = aMax;
    PRINT( _L( "Camera <= CCamZoomPane::SetZoomRange " ) );
    return KErrNone;
    } 

// -----------------------------------------------------------------------------
// CCamZoomPane::SetZoomSteps
// Sets the number of steps allowed in each category
// -----------------------------------------------------------------------------
//
void CCamZoomPane::SetZoomSteps( TInt aOptSteps, TInt aStdSteps, TInt aExtSteps )
    {  
    PRINT( _L( "Camera => CCamZoomPane::SetZoomSteps " ) );      
    ASSERT( aOptSteps >= 0 && aStdSteps >= 0 && aExtSteps >= 0 );
    
    iOptSteps = aOptSteps;
    iStdSteps = aStdSteps;    
    iExtSteps = aExtSteps;
    PRINT( _L( "Camera <= CCamZoomPane::SetZoomSteps " ) );        
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::SetZoomValue
// Specifies the current level of Zoom.
// -----------------------------------------------------------------------------
//
TInt CCamZoomPane::SetZoomValue( TInt aZoom )
    { 
    PRINT1( _L( "Camera => CCamZoomPane::SetZoomValue aZoom=%d" ), aZoom );
    if ( iController.UiConfigManagerPtr()->IsExtendedDigitalZoomSupported() )
        {
        // this may happen if in the extended zoom range
        // and we switch the extended range off - the
        // zoom value in the engine will be greater than
        // the max value allowed
        if ( aZoom > iMaxZoom )
            {
            aZoom = iMaxZoom;
            }        
        }

    // Check supplied value is within valid range
    if ( aZoom < iMinZoom || aZoom > iMaxZoom )
        {
        PRINT( _L( "Camera <= CCamZoomPane::SetZoomValue KErrArgument" ) );
        return KErrArgument;
        }

    iCurZoom = aZoom;
    iTrgZoom = aZoom;

    if ( iVisible )
        {
        // force redraw of zoom pane
        MakeVisible( ETrue, ETrue );
        }

    PRINT( _L( "Camera <= CCamZoomPane::SetZoomValue " ) );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::ZoomValue
// Returns the current zoom value (as the ZoomPane knows it)
// -----------------------------------------------------------------------------
//
TInt CCamZoomPane::ZoomValue() const
    {   
    PRINT( _L( "Camera => CCamZoomPane::ZoomValue " ) );
    PRINT1( _L( "Camera <= CCamZoomPane::ZoomValue iCurZoom=%d" ), iCurZoom );
    return iCurZoom;
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::IsZoomAtMinimum
// Returns whether the current zoom value is the minimum zoom
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::IsZoomAtMinimum() const
    {
    PRINT2(_L("Camera =><= CCamZoomPane::IsZoomAtMinimum iCurZoom = %d, iMinZoom = %d"), iCurZoom, iMinZoom);
    return iCurZoom == iMinZoom;
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::IsZoomAtMaximum
// Returns whether the current zoom value is the maximum zoom
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::IsZoomAtMaximum() const
    {
    PRINT2(_L("Camera =><= CCamZoomPane::IsZoomAtMaximum iCurZoom = %d, iMaxZoom = %d"), iCurZoom, iMaxZoom);
    return iCurZoom == iMaxZoom;
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::OkToShowPane
// Returns whether or not the Zoom Pane can currently be shown.
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::OkToShowPane() const
    {    
    PRINT( _L( "Camera => CCamZoomPane::OkToShowPane " ) );
    // Are we currently recording video?
    if ( iRecordingVideo )
        {   
        // Are we allowed to zoom when recording video?
        if ( ( iZoomSupport & ECamZoomWhenRecord ) )
            {
            PRINT( _L( "Camera <= CCamZoomPane::OkToShowPane ETrue" ) );
            return ETrue;
            }
        else // If not allowed, return false
            {
            PRINT( _L( "Camera <= CCamZoomPane::OkToShowPane EFalse" ) );
            return EFalse;
            }
        }

    // If a sequence capture is in progress
    if ( iController.SequenceCaptureInProgress() )
        {
        PRINT( _L( "Camera <= CCamZoomPane::OkToShowPane EFalse" ) );
        return EFalse;
        }
        
    PRINT( _L( "Camera <= CCamZoomPane::OkToShowPane ETrue" ) );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::HandleControllerEventL
// Handle controller events, specifically to find out if video is currently
// being recorded.  Only called if Zooming while recording is NOT allowed
// -----------------------------------------------------------------------------
//
void CCamZoomPane::HandleControllerEventL( TCamControllerEvent aEvent, 
                                           TInt                /*aError*/ )
    {
    PRINT( _L( "Camera => CCamZoomPane::HandleControllerEventL " ) );
    switch ( aEvent )
        {
        case ECamEventOperationStateChanged:
            {
            iRecordingVideo = ECamControllerVideo == iController.CurrentMode()
                           && ECamCapturing == iController.CurrentOperation();
            break;
            }
        case ECamEventCameraChanged:
            {
            ReadLayoutL();
            break;
            }
        case ECamEventCaptureComplete:
            {
            // Release knob if image is captured with HW button while zooming.
            iIconZoomMarkerCurrent = iIconZoomMarker[0];
            break;
            }
        default:
            break;
        }
    PRINT( _L( "Camera <= CCamZoomPane::HandleControllerEventL " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::Rect
// Returns the whole zoom pane rect
// -----------------------------------------------------------------------------
//
TRect CCamZoomPane::Extent() const
    {
    PRINT( _L( "Camera =><= CCamZoomPane::Extent " ) );
    return iExtent;      
    }

// ---------------------------------------------------------
// CCamZoomPane::ReadLayoutL
// ---------------------------------------------------------
//
void CCamZoomPane::ReadLayoutL()
    {   
    PRINT( _L( "Camera =><= CCamZoomPane::ReadLayoutL " ) );
    
    if ( AknLayoutUtils::PenEnabled() ) 
        {
        TouchLayout();
        }
    else
        {
        NonTouchLayout();
        }
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::IsVisible
// Is the zoom pane invisible
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::IsVisible() const
    {
    PRINT( _L( "Camera =><= CCamZoomPane::IsVisible " ) );
    return iVisible;
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::MakeVisible
// Makes the zoom pane visible
// -----------------------------------------------------------------------------
//
void CCamZoomPane::MakeVisible( TBool aVisible, TBool aRedraw )
    {
    PRINT2( _L( "Camera => CCamZoomPane::MakeVisible aVisible=%d aRedraw=%d" ), aVisible, aRedraw );    
    iVisible = aVisible;

    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );    
    
    // No zoom in secondary camera view
    if ( appUi && iController.ActiveCamera() == ECamActiveCameraPrimary )
        {
        if ( aVisible ) 
            {
            appUi->ShowZoomPane(aRedraw);
            }
        else
            {
            appUi->HideZoomPane(aRedraw);
            iVisible = EFalse;  
            }
        }
    PRINT( _L( "Camera <= CCamZoomPane::MakeVisible " ) );    
    }

void CCamZoomPane::SetZoomKeys( const RArray<TInt>& aZoomIn,
                                const RArray<TInt>& aZoomOut )
    {
    PRINT( _L( "Camera => CCamZoomPane::SetZoomKeys CAMERAAPP_MULTIPLE_ZOOM_KEYS" ) );    
    iZoomInKeys.Reset();
    ReadZoomKeys( aZoomIn, iZoomInKeys );
    iZoomOutKeys.Reset();
    ReadZoomKeys( aZoomOut, iZoomOutKeys );
    PRINT( _L( "Camera <= CCamZoomPane::SetZoomKeys " ) );    
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::ReadZoomKeys
// -----------------------------------------------------------------------------
//
void CCamZoomPane::ReadZoomKeys( const RArray<TInt>& aSource,
                                 RArray<TInt>& aTarget )
    {
    PRINT( _L( "Camera => CCamZoomPane::ReadZoomKeys " ) );
    for ( TInt i = 0; i < aSource.Count(); i++ )
        {
        // here we ignore the error 
        aTarget.Append( aSource[i] );
        }
    PRINT( _L( "Camera <= CCamZoomPane::ReadZoomKeys " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::CheckForZoomKey
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::CheckForZoomKey( const TKeyEvent& aKeyEvent,
                                     const RArray<TInt>& aKeys )
    {
    PRINT( _L( "Camera =><= CCamZoomPane::CheckForZoomKey " ) );
    return (KErrNotFound != aKeys.Find( aKeyEvent.iScanCode ));
    }


// -----------------------------------------------------------------------------
// CCamZoomPane::OfferKeyEventL
// Handles key events for the zoom pane.
// -----------------------------------------------------------------------------
//
TKeyResponse CCamZoomPane::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {    
    PRINT2( _L("Camera => CCamZoomPane::OfferKeyEventL (%d) (%d)"),aKeyEvent.iScanCode, aType )
    TBool vertical = PaneOrientation() == EZPOrientationVertical;

 
    TBool foundZoomInKey =  CheckForZoomKey( aKeyEvent, iZoomInKeys  );
    TBool foundZoomOutKey = CheckForZoomKey( aKeyEvent, iZoomOutKeys );

    if ( foundZoomInKey && vertical ||
         foundZoomOutKey && !vertical )
        {
        // Block key events if touch is active
        if ( iTouchActive )
            {
            return EKeyWasConsumed;
            }
        // Handle a zoom-in key as a valid event, and start zoom
        else if ( aType == EEventKeyDown )
            {
            iModel->ZoomIn();      
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKeyDown, zoom in"))
            return EKeyWasConsumed;      
            }
        // Handle an up zoom-in key as a valid event, only if currently
        // zooming.  Can get stray up events if user woke from standby
        // with zoom key.  This should filter these out.
        else if ( aType == EEventKeyUp &&
                  IsCurrentlyZooming() )
            {
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKeyUp, stop zoom 1"))
            iModel->StopZoom();
            return EKeyWasConsumed;
            }
        // Handle special zoom-in once request
        else if ( aType == EEventUser &&
                  !IsCurrentlyZooming() )
            {
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKeyUp, zoom in once"))
            iModel->ZoomIn( ETrue );
            return EKeyWasConsumed;
            }
        // Repeat "key" events are consumed (to keep the bar visible) but
        // no need to do anything with it as zooming is timer (not key event) based
        else if ( aType == EEventKey &&
                  aKeyEvent.iRepeats > 0 )
            {            
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKey repeated 1"))
            // Do nothing with it, but consume it
            return EKeyWasConsumed;
            }
        else
            {
            // empty statement to remove Lint error.
            }
        
        }
    // If the orientation of the zoom pane changes, the keys to zoom in/out 
    // need to switch to move in the correct direction on the zoom pane.
    else if ( foundZoomOutKey && vertical ||
              foundZoomInKey && !vertical )
        {   
        // Handle a zoom-out key as a valid event, and start zoom
        if ( aType == EEventKeyDown )
            {
            iModel->ZoomOut();
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKeyDown, zoom out"))
            return EKeyWasConsumed;
            }
        // Handle an up zoom-out key as a valid event, only if currently
        // zooming.  Can get stray up events if user woke from standby
        // with zoom key.  This should filter these out.            
        else if ( aType == EEventKeyUp &&
                  IsCurrentlyZooming() )
            {
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKeyUp, calling StopZoom"))
            iModel->StopZoom();
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKeyUp, stop zoom 2"))
            return EKeyWasConsumed;
            }
        // Handle special zoom-out once request
        else if ( aType == EEventUser &&
                  !IsCurrentlyZooming() )
            {
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKeyUp, zoom out once"))
            iModel->ZoomOut( ETrue );
            return EKeyWasConsumed;
            }
        // Repeat "key" events are consumed (to keep the bar visible) but
        // no need to do anything with it as zooming is timer (not key event) based
        else if ( aType == EEventKey &&
                  aKeyEvent.iRepeats > 0 )
            {        
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKey repeated 2"))
            // Do nothing with it, but consume it
            return EKeyWasConsumed;
            }
        else
            {    
            // empty statement to remove Lint error.   
            PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKey empty statement!"))
            }
        
        }
    // otherwise, do nothing
    else
        {
        // empty statement to remove Lint error.
        PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL EEventKey empty statement!!"))
        }
	PRINT( _L("Camera <= CCamZoomPane::OfferKeyEventL not consumed"))
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::HandleForegroundEvent
// Performs required actions on gaining/losing foreground
// -----------------------------------------------------------------------------
//
void CCamZoomPane::HandleForegroundEvent( TBool aForeground )
    {
    PRINT( _L( "Camera => CCamZoomPane::HandleForegroundEvent " ) );
    if ( !aForeground )
        {
        // Ensure any ongoing zoom is stopped
        PRINT( _L( "Camera <> CCamZoomPane::HandleForegroundEvent calling StopZoom" ) );
        iModel->StopZoom();            
        }
    PRINT( _L( "Camera <= CCamZoomPane::HandleForegroundEvent " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::IsCurrentlyZooming
// Returns ETrue if the zoom model is currently zooming in/out,
// else returns EFalse
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::IsCurrentlyZooming() const
    {
    PRINT( _L( "Camera =><= CCamZoomPane::IsCurrentlyZooming " ) );
    return iModel->IsCurrentlyZooming();
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::ResetToDefaultAfterPrepare
// Sets the zoompane to reset the zoom level to default values
// next time the engine is prepared
// -----------------------------------------------------------------------------
//
void CCamZoomPane::ResetToDefaultAfterPrepare( TBool aReset )
    {
    PRINT( _L( "Camera =><= CCamZoomPane::ResetToDefaultAfterPrepare " ) );
    iModel->ResetToDefaultAfterPrepare( aReset );
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::IsResetPending
// Whether or not the zoom level is waiting to be reset to default
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::IsResetPending() const
    {
    PRINT( _L( "Camera =><= CCamZoomPane::IsResetPending " ) );
    return iModel->IsResetPending();
    }    

// -----------------------------------------------------------------------------    
// CCamZoomPane::StopZoom
// Stops zoom actions
// -----------------------------------------------------------------------------
//
void CCamZoomPane::StopZoom()
    {
    PRINT( _L( "Camera => CCamZoomPane::StopZoom " ) );
    iModel->StopZoom();
    PRINT( _L( "Camera <= CCamZoomPane::StopZoom " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::ResetZoomTo1x
// Resets the zoom level to 1x.
// -----------------------------------------------------------------------------
//
void CCamZoomPane::ResetZoomTo1x()
    {
    PRINT( _L( "Camera => CCamZoomPane::ResetZoomTo1x " ) );
    iModel->ResetZoomTo1x();
    PRINT( _L( "Camera <= CCamZoomPane::ResetZoomTo1x " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::StartTouchZoomL
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::StartTouchZoomL( TInt aPointer )  
    {     
    PRINT( _L( "Camera => CCamZoomPane::StartTouchZoomL" ) );
    
    // Block key events
    iTouchActive = ETrue;
    if ( !iMaxZoom ) // Avoid division by zero 
        {
        return EFalse;
        }
    TInt pixelsPerStep = ( iMaxOffset * KDivisorFactor ) / (iMaxZoom+1);

    PRINT1( _L( "Camera <> CCamZoomPane::StartTouchZoomL (iMaxZoom) %d" ), iMaxZoom );
    PRINT1( _L( "Camera <> CCamZoomPane::StartTouchZoomL (iMinZoom) %d" ), iMinZoom );
    PRINT1( _L( "Camera <> CCamZoomPane::StartTouchZoomL (pixelsPerStep) %d" ), pixelsPerStep );

    // New zoom according to touched point                        
    TInt base = iSliderParentRect.iBr.iY - aPointer;
    PRINT1( _L( "Camera <> CCamZoomPane::StartTouchZoomL (base) %d" ), base );

    // Target zoom level 
    iTrgZoom = base * KDivisorFactor / pixelsPerStep;
    iTrgZoom = ( iTrgZoom < iMinZoom )? iMinZoom:iTrgZoom;
    iTrgZoom = ( iTrgZoom > iMaxZoom )? iMaxZoom:iTrgZoom;
    PRINT1( _L( "Camera <> CCamZoomPane::StartTouchZoomL (iTrgZoom) %d" ), iTrgZoom );
   
    // Only update zoom when necessary
    if ( Abs( aPointer - iPointerPrevi ) >= pixelsPerStep / KDivisorFactor &&
         iTrgZoom != iCurZoom )
        {
        PRINT( _L( "Camera <> CCamZoomPane::StartTouchZoomL ZoomTo" ) );
        iModel->ZoomTo( iTrgZoom );
        iPointerPrevi = aPointer;
        PRINT( _L( "Camera <= CCamZoomPane::StartTouchZoomL ETrue" ) );
        return ETrue;
        }       
        
    // Pointer didn't move, so we don't need to update anything
    PRINT( _L( "Camera <= CCamZoomPane::StartTouchZoomL EFalse" ) );
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CCamZoomPane::HandlePointerEventL
// -----------------------------------------------------------------------------
//
TBool CCamZoomPane::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    PRINT( _L( "Camera => CCamZoomPane::HandlePointerEventL" ) );

    if ( PaneOrientation() != EZPOrientationVertical )
        {
        PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (orientation)" ) );
        return EFalse; // EZPOrientationHorizontal not supported               
        }
    
    TRect tchZoomArea = iSliderParentRect;
    tchZoomArea.Grow( KTouchAreaExpansion, KTouchAreaExpansion );
    TPointerEvent::TType type = aPointerEvent.iType;
    
    // Only make the pane appear on first touch
    if( !IsVisible() &&
        type == TPointerEvent::EButton1Down )
        {
        Touchfeedback(); 
        StopZoom();
        MakeVisible( ETrue, ETrue );
        PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (first touch)" ) );
        return ETrue;
        }

    // At this point, the zoom pane should have already been visible
    // so we activate touch zoom if the pointer is on the zoom pane
    if ( IsVisible() &&
         tchZoomArea.Contains( aPointerEvent.iPosition ) &&
         type == TPointerEvent::EButton1Down )
        {
        Touchfeedback(); 
        // Change zoom thumb to non-dimmed
        iIconZoomMarkerCurrent = iIconZoomMarkerGray[0]; // Dimmed knob
        // Force redraw of zoom pane, so knob will be dimmed
        TBool ret = ETrue;
        // Update zoom position, if necessary
        StartTouchZoomL( aPointerEvent.iPosition.iY );
        PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (touch active)" ) );
        return ret;
        }
        
    // If the zoom panel is visible, but the touch event is not in the
    // zoom area, then make the zoom pane disappear.
    if ( IsVisible() &&
         !tchZoomArea.Contains( aPointerEvent.iPosition ) &&
         type == TPointerEvent::EButton1Down )
        {
        Touchfeedback();
        // Cancel zoom if user is pushing the volume key
        StopZoom(); 
        // Hide zoom pane
        MakeVisible( EFalse, ETrue );
        PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (make disappear)" ) );
        // return EFalse so the calling component doesn't reactivate the zoom pane
        return EFalse;
        }
        
    // Only allow drag if the zoom is already active
    if ( type == TPointerEvent::EDrag &&
            tchZoomArea.Contains( aPointerEvent.iPosition ) &&
         iTouchActive )
        { 
        TBool ret = StartTouchZoomL( aPointerEvent.iPosition.iY );
        PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (dragging)" ) );
        return ret;
        }  
    
    // Clean up when the touch events are stopped
    if ( type == TPointerEvent::EButton1Up ||
            (type == TPointerEvent::EDrag &&
                    !tchZoomArea.Contains( aPointerEvent.iPosition )))
        {
        // don't do anything for stray touches
        if ( iTouchActive )
            {
            // Key events are no longer blocked
            iTouchActive = EFalse;
            // Change zoom thumb to non-dimmed
            iIconZoomMarkerCurrent = iIconZoomMarker[0]; 
            // force redraw of zoom pane, so knob is not dimmed anymore
            MakeVisible( ETrue, ETrue );
            PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (touch stopped)" ) );
            return ETrue;
            }

        PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (touch not active)" ) );
        return EFalse;
        }
         
    PRINT( _L( "Camera <= CCamZoomPane::HandlePointerEventL (not handled)" ) );
    return EFalse;
    }

// ---------------------------------------------------------
// CCamZoomPane::SizeChanged
// Called when the view size is changed
// ---------------------------------------------------------
//
void CCamZoomPane::SizeChanged() 
    {
    PRINT( _L( "Camera => CCamZoomPane::SizeChanged" ) );
    if ( AknLayoutUtils::PenEnabled() ) 
        {
        TouchLayout();
        }
    else
        {
        NonTouchLayout();
        }
                              
    PRINT( _L( "Camera <= CCamZoomPane::SizeChanged" ) );                       
    }
    
// ---------------------------------------------------------
// CCamZoomPane::TouchLayout
// ---------------------------------------------------------
void CCamZoomPane::TouchLayout() 
    {
    PRINT( _L( "Camera => CCamZoomPane::TouchLayout()" ) );
    TRect mainPaneRect;   
    iMaxOffset = 0;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow,
                                       mainPaneRect );  
    mainPaneRect.Move( -mainPaneRect.iTl ); 
    
    TInt variant = Layout_Meta_Data::IsLandscapeOrientation();                                     
    TAknWindowComponentLayout l = AknLayoutScalable_Apps::cam4_zoom_pane(variant); 

    TAknLayoutRect area;
    area.LayoutRect( mainPaneRect, l.LayoutLine() ); 
    iZoomPaneRect = area.Rect();

    l = AknLayoutScalable_Apps::cam4_zoom_cont_pane(variant); 
    area.LayoutRect( iZoomPaneRect, l.LayoutLine() );
    iSliderParentRect = area.Rect();
    iExtent = iSliderParentRect;
    
    l = AknLayoutScalable_Apps::cam4_zoom_pane_g1(variant); // +
    area.LayoutRect( iZoomPaneRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomMax[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iPlusPoint = area.Rect().iTl;   
    iExtent.BoundingRect(area.Rect());
    
    l = AknLayoutScalable_Apps::cam4_zoom_pane_g2(variant); // -
    area.LayoutRect( iZoomPaneRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomMin[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iMinusPoint = area.Rect().iTl;    
    iExtent.BoundingRect(area.Rect());
    
    l = AknLayoutScalable_Apps::cam4_zoom_cont_pane_g1(variant); // Top
    area.LayoutRect( iSliderParentRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomTop[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iTopPoint = area.Rect().iTl;                           
    iMaxOffset += area.Rect().Height();
    
    l = AknLayoutScalable_Apps::cam4_zoom_cont_pane_g3(variant); // Middle
    area.LayoutRect( iSliderParentRect, l.LayoutLine() );
    iIconZoomMiddle[0]->Resize( area.Rect().Size());
    AknIconUtils::SetSize( iIconZoomMiddle[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iMiddlePoint = area.Rect().iTl;                        
    iMaxOffset += area.Rect().Height();

    l = AknLayoutScalable_Apps::cam4_zoom_cont_pane_g2(variant); // Bottom
    area.LayoutRect( iSliderParentRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomBottom[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iBottomPoint = area.Rect().iTl;                        
    TRect bottomRect = area.Rect();
    iMaxOffset += area.Rect().Height();
    
    l = AknLayoutScalable_Apps::cam4_zoom_pane_g3(variant); // Knob
    area.LayoutRect(iSliderParentRect, l.LayoutLine() );
    TRect adj = iZoomPaneRect;
	adj.SetHeight( area.Rect().Height() );
	adj.Move( bottomRect.Center() - adj.Center() );
    AknIconUtils::SetSize( iIconZoomMarker[0], adj.Size() );  
    AknIconUtils::SetSize( iIconZoomMarkerGray[0], adj.Size() );

    if( iPlusPoint.iY < iTopPoint.iY )
        {
        // Thumb is drawn further than shaft.
        // If Max(+) and Min(-) marks are extending shaft,
        // parent rectangle covers everything.
        iExtent = iZoomPaneRect;
        }
    else
        {
        // If marks are drawn right side of shaft,
        // extend area to redraw.
        iExtent.Grow(0, adj.Size().iHeight/2);
        }
   
    }

// ---------------------------------------------------------
// CCamZoomPane::NonTouchLayout
// ---------------------------------------------------------
void CCamZoomPane::NonTouchLayout() 
    {
    TRect mainPaneRect; 
    iMaxOffset = 0;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow,
                                       mainPaneRect );  
    mainPaneRect.Move( -mainPaneRect.iTl ); 
    
    TInt variant = Layout_Meta_Data::IsLandscapeOrientation(); 

    TAknWindowComponentLayout l = AknLayoutScalable_Apps::cam6_zoom_pane(variant); 

    TAknLayoutRect area;
    area.LayoutRect( mainPaneRect, l.LayoutLine() ); 
    iZoomPaneRect = area.Rect();

    l = AknLayoutScalable_Apps::cam6_zoom_cont_pane(variant); 
    area.LayoutRect( iZoomPaneRect, l.LayoutLine() );
    iSliderParentRect = area.Rect();
    iExtent = iSliderParentRect;
    
    l = AknLayoutScalable_Apps::cam6_zoom_pane_g1(variant); // +
    area.LayoutRect( iZoomPaneRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomMax[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iPlusPoint = area.Rect().iTl;       
    iExtent.BoundingRect(area.Rect());
    
    l = AknLayoutScalable_Apps::cam6_zoom_pane_g2(variant); // -
    area.LayoutRect( iZoomPaneRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomMin[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iMinusPoint = area.Rect().iTl;       
    iExtent.BoundingRect(area.Rect());
    
    l = AknLayoutScalable_Apps::cam6_zoom_cont_pane_g1(variant); // Top
    area.LayoutRect( iSliderParentRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomTop[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iTopPoint = area.Rect().iTl; 
    iMaxOffset += area.Rect().Height();
    
    l = AknLayoutScalable_Apps::cam6_zoom_cont_pane_g3(variant); // Middle
    area.LayoutRect( iSliderParentRect, l.LayoutLine() );
    iIconZoomMiddle[0]->Resize( area.Rect().Size());
    AknIconUtils::SetSize( iIconZoomMiddle[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iMiddlePoint = area.Rect().iTl;                        
    iMaxOffset += area.Rect().Height();

    l = AknLayoutScalable_Apps::cam6_zoom_cont_pane_g2(variant); // Bottom
    area.LayoutRect( iSliderParentRect, l.LayoutLine() );
    AknIconUtils::SetSize( iIconZoomBottom[0], area.Rect().Size(), 
                           EAspectRatioNotPreserved );
    iBottomPoint = area.Rect().iTl;                        
    TRect bottomRect = area.Rect();
    iMaxOffset += area.Rect().Height();
    
    l = AknLayoutScalable_Apps::cam6_zoom_pane_g3(variant); // Knob
    area.LayoutRect(iSliderParentRect, l.LayoutLine() );
    iZoomPaneRect.iTl.iX += area.Rect().Width()/2;
    AknIconUtils::SetSize( iIconZoomMarker[0], area.Rect().Size() );  
    AknIconUtils::SetSize( iIconZoomMarkerGray[0], area.Rect().Size() );    

    if( iPlusPoint.iY < iTopPoint.iY )
        {
        // Thumb is drawn further than shaft.
        // If Max(+) and Min(-) marks are extending shaft,
        // parent rectangle covers everything.
        iExtent = iZoomPaneRect;
        }
    else
        {
        // If marks are drawn right side of shaft,
        // extend area to redraw.
        iExtent.Grow(0, area.Rect().Size().iHeight/2);
        }
    }
    
// ---------------------------------------------------------
// CCamZoomPane::Touchfeedback
// ---------------------------------------------------------
//
void CCamZoomPane::Touchfeedback() 
    {
    PRINT( _L( "Camera => CCamZoomPane::Touchfeedback" ) );
    if ( iFeedback )
        {
        iFeedback->InstantFeedback( iRecordingVideo?
                                    ETouchFeedbackNone : ETouchFeedbackBasic );        
        }
    PRINT( _L( "Camera <= CCamZoomPane::Touchfeedback" ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::ZoomToMinimum
// Zooms out to min zoom level. Should be stopped by StopZoom(), if needed
// -----------------------------------------------------------------------------
//
void CCamZoomPane::ZoomToMinimum()
    {
    iModel->SetZoomMultiplier( KFastZoomMultiplier );
    iModel->ZoomOut();
    }

// -----------------------------------------------------------------------------
// CCamZoomPane::ZoomToMaximum
// Zooms in to max zoom level. Should be stopped by StopZoom(), if needed 
// -----------------------------------------------------------------------------
//
void CCamZoomPane::ZoomToMaximum()
    {
    iModel->SetZoomMultiplier( KFastZoomMultiplier );
    iModel->ZoomIn();
    }

//  End of File  
