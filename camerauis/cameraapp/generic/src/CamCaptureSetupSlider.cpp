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
* Description:  Class for showing the EV slider*
*/



// INCLUDE FILES
#include "CamCaptureSetupSlider.h"
#include "CamPanic.h"    // Panic codes
#include "CamUtility.h"
#include "CamAppUi.h"
#include "CamPSI.h"
#include "camconfiguration.h"
#include "CameraUiConfigManager.h"

#include <e32base.h>
#include <e32std.h>
#include <coemain.h>
#include <eikenv.h>
#include <cameraapp.mbg>
#include <eikappui.h>    // For CCoeAppUiBase
#include <eikapp.h>      // For CEikApplication
#include <barsread.h>    // resource reader
#include <AknIconUtils.h>
#include <AknsDrawUtils.h>
#include <touchfeedback.h> 

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <aknlayoutscalable_apps.cdl.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KDivisorFactor = 1024; // Avoids using TReal maths, use factor of 2

// Contrast slider value indexes
const TInt KContrastPlusInd = 0;
const TInt KContrastMinusInd = 1;
const TInt KContrastIconInd = 2;

// EV slider value indexes
const TInt KEVPlus20Ind = 0;
const TInt KEVPlus15Ind = 1;
const TInt KEVPlus10Ind = 2;
const TInt KEVPlus05Ind = 3;
const TInt KEV0Ind = 4;
const TInt KEVMinus05Ind = 5;
const TInt KEVMinus10Ind = 6;
const TInt KEVMinus15Ind = 7;
const TInt KEVMinus20Ind = 8;
const TUint32 KToolbarExtensionBgColor = 0x00000000;
const TInt KToolBarExtensionBgAlpha = 0x7F;




// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CCamSliderLegend::~CCamSliderLegend
// destructor 
// ---------------------------------------------------------
//
CCamSliderLegend::~CCamSliderLegend()
  {
  PRINT( _L("Camera => ~CCamSliderLegend") );
  delete iText;
  delete iBitmap;
  delete iMask;
  PRINT( _L("Camera <= ~CCamSliderLegend") );
  }

CCamSliderLegend::CCamSliderLegend( TBool aFullySkinned )
: iFullySkinned( aFullySkinned )
    {    
    }

// ---------------------------------------------------------
// CCamSliderLegend::Draw
// Draws the legend 
// ---------------------------------------------------------
//
void CCamSliderLegend::Draw( CWindowGc& aGc ) const
    {
    if ( iItem == ECamSliderLegendItemText )
        {
        TRgb color;
        if( iFullySkinned )
            {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                                                EAknsCIQsnTextColorsCG6 );
            }
        else
            {            
            color=KRgbWhite;
            }
        iPosition.DrawText( aGc, iText->Des(), ETrue, color );
        }
    else
        {
        iIconRect.DrawImage( aGc, iBitmap, iMask );
        }
    }

// ---------------------------------------------------------
// CCamSliderLegend::Rect
// Returns the legend rect 
// ---------------------------------------------------------
//
TRect CCamSliderLegend::Rect() const
    {
    if ( iItem == ECamSliderLegendItemText )
        {
        return iPosition.TextRect();
        }
    else
        {
        return iIconRect.Rect();
        }
    }


// ---------------------------------------------------------
// CCamSliderLegend::ConstructTextL
// Constructs a text element from resources with given layout
// ---------------------------------------------------------
//
void CCamSliderLegend::ConstructTextL(
        TResourceReader& aReader,
        const TRect& aRect,
        const TAknTextComponentLayout& aLayout )
    {      
	iItem = static_cast<TCamSliderLegendItem>( aReader.ReadInt16() );
	if ( iItem == ECamSliderLegendItemText )
		{ // must be a text	
		iPosition.LayoutText( aRect, aLayout );
		iText = aReader.ReadHBufC16L();
        aReader.ReadInt32(); // bitmap
        aReader.ReadInt32(); // mask
		}
    }
 
// ---------------------------------------------------------
// CCamSliderLegend::ConstructIconL
// Constructs an icon from resources with given layout
// ---------------------------------------------------------
//
void CCamSliderLegend::ConstructIconL(
        TResourceReader& aReader,
        const TRect& aRect,
        const TAknWindowComponentLayout& aLayout )
    {      
	iItem = static_cast<TCamSliderLegendItem>( aReader.ReadInt16() );
	if ( iItem == ECamSliderLegendItemIcon )
	    { // must be an icon	
		iIconRect.LayoutRect( aRect, aLayout );
		iText = aReader.ReadHBufC16L();
		TInt bitmap = aReader.ReadInt32();
		TInt mask = aReader.ReadInt32();
		
		// Create component bitmaps 
        TFileName resFileName;
        CamUtility::ResourceFileName( resFileName );
        TPtrC resname = resFileName;
		AknIconUtils::CreateIconL( iBitmap, iMask, resname, bitmap, mask );
        AknIconUtils::SetSize( iBitmap, iIconRect.Rect().Size() );
		}
    }


// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::CCamCaptureSetupSlider
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupSlider::CCamCaptureSetupSlider( MCamSettingValueObserver* aObserver,
                                                TCamSettingItemIds aSettingType,
                                                TInt aSteps,
                                                TBool aFullySkinned ) 
: iSettingObserver( aObserver ), iSettingType( aSettingType ), iSteps( aSteps ), iFullySkinned( aFullySkinned )
    {
    }


/*
 * CCamCaptureSetupSlider::InitializeSliderValuesL
 */

void CCamCaptureSetupSlider::InitializeSliderValuesL()
    {
    CCameraUiConfigManager* configManager = static_cast<CCamAppUi*>
                           ( iCoeEnv->AppUi() )->AppController().UiConfigManagerPtr();
    TBool configureRunTime = EFalse;
    RArray<TInt> range;
    CleanupClosePushL( range );
    
    switch ( iSettingType )
        {
        case ECamSettingItemDynamicPhotoBrightness:
        case ECamSettingItemDynamicVideoBrightness:
        case ECamSettingItemUserSceneBrightness:
            {
            if ( configManager && configManager->IsBrightnessSupported() )
                {
                configManager->SupportedBrightnessRangeL( range );
                configureRunTime = ETrue;
                }
            break;
            }
        case ECamSettingItemDynamicPhotoContrast:
        case ECamSettingItemDynamicVideoContrast:
        case ECamSettingItemUserSceneContrast:
            {
            if ( configManager && configManager->IsContrastSupported() )
                {
                configManager->SupportedContrastRangeL( range );
                configureRunTime = ETrue;
                }           
            }
            break;

        case ECamSettingItemDynamicPhotoExposure:
        case ECamSettingItemDynamicVideoExposure:
        case ECamSettingItemUserSceneExposure:
            {
            // get range of EV values from product specific utility
            TCamEvCompRange evRange = 
                static_cast<CCamAppUi*>(
                        iCoeEnv->AppUi() )->AppController().EvRange();

            iMaxSliderValue = evRange.iMaxValue * evRange.iStepsPerUnit;
            iMinSliderValue = evRange.iMinValue * evRange.iStepsPerUnit;
            iNumSliderValues = iMaxSliderValue - iMinSliderValue;
            }
            break;

		default:
			{
            }
            break;
        }

    if ( configureRunTime )
        {
        if ( range.Count() > 0 )
            {
            iMinSliderValue = range[0]; //min value
            iMaxSliderValue = range[1]; // max value
            iNumSliderValues = range[range.Count()-1]; // steps
            }
        }

    CleanupStack::PopAndDestroy( &range );
    
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::ConstructL( const CCoeControl* aParent )
    {
    __ASSERT_DEBUG( aParent!=NULL, CamPanic( ECamPanicNullPointer ) );
    
    InitializeSliderValuesL();
    
    iParentControl = aParent;
    SetContainerWindowL( *iParentControl );  

    TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
    TPtrC resname = resFileName;

    // Create component bitmaps 
    AknIconUtils::CreateIconL( iBitmapShaft, 
                               iBitmapShaftMask, 
                               resname, 
                               EMbmCameraappQgn_graf_nslider_cam4_empty,
                               EMbmCameraappQgn_graf_nslider_cam4_empty_mask );

    AknIconUtils::CreateIconL( iBitmapThumb, 
                               iBitmapThumbMask, 
                               resname, 
                               EMbmCameraappQgn_graf_nslider_cam4_marker,
                               EMbmCameraappQgn_graf_nslider_cam4_marker_mask );

    AknIconUtils::CreateIconL( iBitmapThumbSelected, 
                               iBitmapThumbSelectedMask, 
                               resname, 
                               EMbmCameraappQgn_graf_nslider_cam4_marker_selected,
                               EMbmCameraappQgn_graf_nslider_cam4_marker_selected_mask );
    iBitmapThumbCurrent = iBitmapThumb;
    iBitmapThumbCurrentMask = iBitmapThumbMask;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupSlider* CCamCaptureSetupSlider::NewL( const CCoeControl* aParent, 
                                                      MCamSettingValueObserver* aObserver,
                                                      TCamSettingItemIds aSettingItem,
                                                      TInt aSteps,
                                                      TBool aFullySkinned )
    {
    CCamCaptureSetupSlider* self = 
        new( ELeave ) CCamCaptureSetupSlider( aObserver, aSettingItem, aSteps,
                aFullySkinned );
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::~CCamCaptureSetupSlider
// Destructor
// -----------------------------------------------------------------------------
//    
CCamCaptureSetupSlider::~CCamCaptureSetupSlider()
  {
  PRINT( _L("Camera => ~CCamCaptureSetupSlider") );

  // Destroy legend strings
  iLegendArray.ResetAndDestroy();

  delete iBitmapShaft;
  delete iBitmapShaftMask;
  delete iBitmapThumb;
  delete iBitmapThumbMask;
  delete iBitmapThumbSelected;
  delete iBitmapThumbSelectedMask;
  PRINT( _L("Camera <= ~CCamCaptureSetupSlider") );
  }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::InitializeL
// Sets up the slider with the initial value
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::InitializeL( TInt aValue )
    {
    // Check the initial value is valid.  If no, leave.
    if ( aValue < iMinSliderValue ||
         aValue > iMaxSliderValue )
        {
        User::Leave( KErrArgument );
        }
           
    iValue = aValue;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::MinValue
// Returns the minimum slider value
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupSlider::MinValue() const
    {
    return iMinSliderValue;
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::MaxValue
// Returns the maximum slider value
// -----------------------------------------------------------------------------
//
TInt CCamCaptureSetupSlider::MaxValue() const
    {
    return iMaxSliderValue;
    }
    
      
// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::SetRange
// Sets the minimum and maximum values of the slider control
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::SetRange( TInt aMin, TInt aMax ) 
    {
    ASSERT( aMin < aMax );
    iMinSliderValue = aMin;
    iMaxSliderValue = aMax;    
    iNumSliderValues = iMaxSliderValue;
    }
    

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::Draw
// Draws the slider 
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    if( iFullySkinned )
        {
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        MAknsControlContext *cc = AknsDrawUtils::ControlContext( iParentControl ); 
        AknsDrawUtils::Background( skin, cc, iParentControl, gc, Rect() );
        }
    else
        {
        gc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
        gc.SetBrushColor( TRgb( KToolbarExtensionBgColor, KToolBarExtensionBgAlpha ) );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( Rect() );            
        // Reset the brush after use (otherwise anything drawn
        // after the viewfinder will also show viewfinder frames)    
        gc.SetDrawMode( CGraphicsContext::EDrawModePEN );
        TSize penSize( 1, 1 );
        gc.SetPenSize( penSize );
        gc.SetPenStyle( CGraphicsContext::EDottedPen );
        gc.SetPenColor( KRgbWhite );
        gc.SetBrushStyle( CGraphicsContext::ENullBrush );
        gc.DrawRect( Rect() );                            
        }
    
    // STEP 1: Draw the legend text
    TInt count = iLegendArray.Count();
    TInt i = 0;
    for ( i = 0; i < count; i++ )
        {
        iLegendArray[i]->Draw( gc );
        }

    CFbsBitmap* shaft = iBitmapShaft;
    CFbsBitmap* shaftMask = iBitmapShaftMask;

    // Step 2: Draw the shaft bitmap    
    if ( shaft )
        {
        iShaftLayout.DrawImage( gc, shaft, shaftMask );
        }

    // Step 3: Work out the position of the thumb
    TPoint thumbPoint = iThumbPoint;  
    
    // This is the max bitmap offset from the top of the shaft that the thumb can be
    TInt maxVOffset = iShaftLayout.Rect().Size().iHeight - iBitmapThumb->SizeInPixels().iHeight;

    // This is the no. of pixels for a "step"
    TInt stepInPixels = ( maxVOffset * KDivisorFactor ) / iNumSliderValues;

    // Thumb position = top of shaft + delta
    TInt nbrSteps = iMaxSliderValue - iValue;
    thumbPoint.iY = iShaftLayout.Rect().iTl.iY + stepInPixels *  nbrSteps / KDivisorFactor;
        
    // Step 4: Blit the thumb bitmap
    gc.BitBltMasked( thumbPoint, iBitmapThumbCurrent, iBitmapThumb->SizeInPixels(), iBitmapThumbCurrentMask, ETrue );
    }


// ---------------------------------------------------------
// CCamCaptureSetupSlider::SizeChanged
// Calculates the new minimum size
// ---------------------------------------------------------
//
void CCamCaptureSetupSlider::SizeChanged()
    {        
    TRAPD( ignore, ReadLayoutL() );
    if ( ignore )
        { 
        // Do nothing ( removes build warning )
        }                 

    // Work out the minimum size
    TInt count = iLegendArray.Count();

    TRect minimumRect;

    // Start min rect as first legend text
    if ( count > 0 )
        {
        minimumRect = iLegendArray[0]->Rect();
        }

    // Take into account the other legend texts
    TInt i;
    for ( i = 1; i < count; i++ )
        {            
        minimumRect.BoundingRect( iLegendArray[i]->Rect() );
        }

    // Take into account the shaft bitmap
    minimumRect.BoundingRect( iShaftLayout.Rect() );

    iMinimumSize = minimumRect.Size();
    }   


// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::OfferKeyEventL
// Allows the user to change the current EV value via key presses.
// -----------------------------------------------------------------------------
//
TKeyResponse CCamCaptureSetupSlider::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    PRINT( _L("CCamCaptureSetupSlider::OfferKeyEventL") );
    if ( aType == EEventKey )
        {
        if ( aKeyEvent.iScanCode == EStdKeyUpArrow ) 
            {
            if ( iValue <  iMaxSliderValue )
                {
                iValue ++;
                iSettingObserver->HandleSettingValueUpdateL( iValue );  // Tell observer about change
                DrawNow();
                }

            return EKeyWasConsumed;
            }
        else if ( aKeyEvent.iScanCode == EStdKeyDownArrow ) 
            {
            if ( iValue > iMinSliderValue )
                {
                iValue --;
                iSettingObserver->HandleSettingValueUpdateL( iValue );  // Tell observer about change
                DrawNow();
                }
            
            return EKeyWasConsumed;
            }
        else if ( aKeyEvent.iCode == EKeyOK && aKeyEvent.iRepeats == 0 )
            {
            return EKeyWasConsumed;
            }
        // otherwise, do nothing
        else
            {
            // empty statement to remove Lint error
            }
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CCamCaptureSetupSlider::MinimumSize
// ---------------------------------------------------------
//
TSize CCamCaptureSetupSlider::MinimumSize()
    {
    TSize zerosize = TSize( 0, 0 );
    if ( iMinimumSize == zerosize )
        {
        SizeChanged();
        }
    return iMinimumSize; 
    }


// ---------------------------------------------------------
// CCamCaptureSetupSlider::ReadLayoutL
// ---------------------------------------------------------
//
void CCamCaptureSetupSlider::ReadLayoutL()
    {
    TRect shaftRect;
    TSize shaftSize;

    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                       iLayoutAreaRect );

    // layout area rectangle contains the area, where components need to be 
    // drawn to. the container size is the whole screen, but the layouts are 
    // for the client area. aRect is the container size that might include or
    // might not include statuspane area. calculating area self will
    // go around the problem

    // We're starting again from scratch, so delete the old legend layouts
    iLegendArray.ResetAndDestroy();
    if ( CamUtility::IsNhdDevice() ) 
        {
        TouchLayoutL();
        }
    else
        {
        NonTouchLayoutL();
        }
      
    AknIconUtils::SetSize( iBitmapThumb, 
                           iThumbLayout.Rect().Size(), EAspectRatioPreserved );
    AknIconUtils::SetSize( iBitmapThumbSelected, 
                           iThumbLayout.Rect().Size(), EAspectRatioPreserved );

    // set the shaft icon size
    shaftRect = iShaftLayout.Rect();
    shaftSize = shaftRect.Size();
    AknIconUtils::SetSize( iBitmapShaft, shaftSize, EAspectRatioNotPreserved );

    // calculate initial thumb position
    iThumbPoint = TPoint( shaftRect.iTl.iX + shaftSize.iWidth/2 - 
                          iBitmapThumb->SizeInPixels().iWidth/2, 
                          shaftRect.iTl.iY + shaftSize.iHeight/2 - 
                          iBitmapThumb->SizeInPixels().iHeight/2 );
    }

// ---------------------------------------------------------
// CCamCaptureSetupSlider::TouchLayoutL
// ---------------------------------------------------------
//
void CCamCaptureSetupSlider::TouchLayoutL()
    {      
    TRect statusPaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EStatusPane,
                                       statusPaneRect );
    iLayoutAreaRect.BoundingRect( statusPaneRect );
    
    // Get the slider layout
    TAknLayoutRect sliderLayout;
    sliderLayout.LayoutRect( iLayoutAreaRect, 
                          AknLayoutScalable_Apps::main_cset_slider_pane( 1 ) );
    TRect sliderRect( sliderLayout.Rect() );

    // Set the thumb layout and icon size
    iThumbLayout.LayoutRect( sliderRect, 
                      AknLayoutScalable_Apps::main_cset_slider_pane_g18( 0 ) );

    if ( iSettingType == ECamSettingItemDynamicPhotoContrast ||
         iSettingType == ECamSettingItemDynamicVideoContrast ||
         iSettingType == ECamSettingItemUserSceneContrast )
        {
        // Set the shaft layout and icon size for Contrast slider
        iShaftLayout.LayoutRect( sliderRect, 
                              AknLayoutScalable_Apps::cset_slider_pane( 7 )  );
        // set Contrast slider legend layouts      
        TouchContrastLayoutL( sliderRect ); 
        }   
    else  
        {
        // Set the shaft layout and icon size for EV slider
        iShaftLayout.LayoutRect( sliderRect, 
                              AknLayoutScalable_Apps::cset_slider_pane( 7 )  );
        // set EV slider legend layouts                  
        TouchEVLayoutL( sliderRect );
        }   
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::NonTouchLayout
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::NonTouchLayoutL()
    {   
    // Non-touch has a visible title & status panes
    TRect titlePaneRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::ETitlePane, 
                                       titlePaneRect );
    iLayoutAreaRect.Move( 0, -titlePaneRect.Height() );
    
    iThumbLayout.LayoutRect( Rect(), 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g1( 2 ) );

    if ( iSettingType == ECamSettingItemDynamicPhotoContrast ||
         iSettingType == ECamSettingItemDynamicVideoContrast ||
         iSettingType == ECamSettingItemUserSceneContrast )
        {
        // Set the shaft layout for Contrast slider
        iShaftLayout.LayoutRect( Rect(), 
                              AknLayoutScalable_Apps::cset6_slider_pane( 5 ) );
        // set Contrast slider legend layouts      
        NonTouchContrastLayoutL( Rect() );
        }   
    else  
        {
        // Set the shaft layout for EV slider
        iShaftLayout.LayoutRect( Rect(), 
                              AknLayoutScalable_Apps::cset6_slider_pane( 6 ) ); 
        // set EV slider legend layouts   
        NonTouchEVLayoutL( Rect() );
        }      
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::TouchContrastLayoutL
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::TouchContrastLayoutL( const TRect& aParentRect )
    {
    TInt resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_CONTRAST_ARRAY_ID);
    TResourceReader reader; 
    iEikonEnv->CreateResourceReaderLC( reader, resourceId );    

    const TInt count = reader.ReadInt16();
    
    // Read all EV entries from the resource file and construct them with layout
    for ( TInt i = 0; i < count; i++ )
        {
        CCamSliderLegend* legend = new ( ELeave ) CCamSliderLegend( iFullySkinned );
        CleanupStack::PushL( legend );
        switch ( i )
            {
            case KContrastPlusInd :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t14( 5 ) );
                }
			    break;
            case KContrastMinusInd :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t15( 5 ) );
                }
			    break;
            case KContrastIconInd :
			    {
                legend->ConstructIconL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_g15( 2 ) );
                }
			    break;
            default:
			    {
                }
			    break;
            }
        User::LeaveIfError( iLegendArray.Append( legend ) );
        CleanupStack::Pop( legend );
        }
        
    CleanupStack::PopAndDestroy(); // reader
	}
	
// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::NonTouchContrastLayoutL
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::NonTouchContrastLayoutL( 
                                                     const TRect& aParentRect )
    {
    TInt resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_CONTRAST_ARRAY_ID);
    TResourceReader reader; 
    iEikonEnv->CreateResourceReaderLC( reader, resourceId );    

    const TInt count = reader.ReadInt16();
    
    // Read all EV entries from the resource file 
    // and construct them with layout
    for ( TInt i = 0; i < count; i++ )
        {
        CCamSliderLegend* legend = new ( ELeave ) CCamSliderLegend;
        CleanupStack::PushL( legend );
        switch ( i )
            {
            case KContrastPlusInd :
                {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t14( 5 ) );  
                }
                break;
            case KContrastMinusInd :
                {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t15( 5 ) );
                }
                break;
            case KContrastIconInd :
                {
                legend->ConstructIconL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_g15( 2 ) );
                }
                break;
            default:
                {
                }
                break;
            }
        User::LeaveIfError( iLegendArray.Append( legend ) );
        CleanupStack::Pop( legend );
        }
        
    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::TouchEVLayoutL
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::TouchEVLayoutL( const TRect& aParentRect )
	{
	TInt resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_EV_ARRAY_ID);
    TResourceReader reader; 
    iEikonEnv->CreateResourceReaderLC( reader, resourceId );    

    const TInt count = reader.ReadInt16();
    
    // Read all Contrast entries from the resource file 
    // and construct them with layout
    for ( TInt i = 0; i < count; i++ )
        {
        CCamSliderLegend* legend = new ( ELeave ) CCamSliderLegend( iFullySkinned );
        CleanupStack::PushL( legend );
        switch ( i )
            {
            case KEVPlus20Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t1( 6 ) );
                }
			    break;
            case KEVPlus15Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t8( 2 ) );
                }
			    break;
            case KEVPlus10Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t2( 3 ) );
                }
			    break;
            case KEVPlus05Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t9( 2 ) );
                }
			    break;
            case KEV0Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t3( 3 ) );
                }
			    break;
            case KEVMinus05Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t10( 2 ) );
                }
			    break;
            case KEVMinus10Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t4( 3 ) );
                }
			    break;
            case KEVMinus15Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t11( 2 ) );
                }
			    break;
            case KEVMinus20Ind :
			    {
                legend->ConstructTextL( 
                    reader, 
                    aParentRect, 
                    AknLayoutScalable_Apps::main_cset_slider_pane_t5( 3 ) );
                }
			    break;
            default:
			    {
                }
			    break;
            }
        User::LeaveIfError( iLegendArray.Append( legend ) );
        CleanupStack::Pop( legend );
        }
        
    CleanupStack::PopAndDestroy(); // reader
	}



// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::NonTouchEVLayoutL
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::NonTouchEVLayoutL( const TRect& aParentRect )
    {
    TInt resourceId = ROID(R_CAM_CAPTURE_SETUP_SLIDER_EV_ARRAY_ID);
    TResourceReader reader; 
    iEikonEnv->CreateResourceReaderLC( reader, resourceId );    

    const TInt count = reader.ReadInt16();
    
    // Read all Contrast entries from the resource file 
    // and construct them with layout
    for ( TInt i = 0; i < count; i++ )
        {
        CCamSliderLegend* legend = new ( ELeave ) CCamSliderLegend;
        CleanupStack::PushL( legend );
        switch ( i )
            {
            case KEVPlus20Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t1( 5 ) );
                }
                break;
            case KEVPlus15Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t8( 2 ) );
                }
                break;
            case KEVPlus10Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t2( 3 ) ); 
                }
                break;
            case KEVPlus05Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t9( 2 ) );
                }
                break;
            case KEV0Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t3( 3 ) ); 
                }
                break;
            case KEVMinus05Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t10( 2 ) );
                }
                break;
            case KEVMinus10Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t4( 3 ) );
                }
                break;
            case KEVMinus15Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t11( 2 ) );
                }
                break;
            case KEVMinus20Ind :
                {
                legend->ConstructTextL( reader, aParentRect, 
                    AknLayoutScalable_Apps::main_cset6_slider_pane_t5( 3 ) );
                }
                break;
            default:
                break;
            }
        User::LeaveIfError( iLegendArray.Append( legend ) );
        CleanupStack::Pop( legend );
        }
        
    CleanupStack::PopAndDestroy(); // reader
    }

// -----------------------------------------------------------------------------
// CCamCaptureSetupSlider::HandlePointerEventL
// Handles slider pointer events
// -----------------------------------------------------------------------------
//
void CCamCaptureSetupSlider::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {   
    if( !Rect().Contains(aPointerEvent.iPosition) && iBitmapThumbCurrent != iBitmapThumbSelected )
        {
        return;
        }
    CCoeControl::HandlePointerEventL( aPointerEvent );

    // This is the max bitmap offset from the top of the shaft that the thumb can be
    TInt maxVOffset = iShaftLayout.Rect().Size().iHeight - iBitmapThumb->SizeInPixels().iHeight;

    // This is the no. of pixels for a "step"
    TInt stepInPixels = ( maxVOffset * KDivisorFactor ) / iNumSliderValues;

    // Calculate new setting value     
    TInt oldValue = iValue; 
    TInt deltaY = aPointerEvent.iPosition.iY - iShaftLayout.Rect().iTl.iY;
    TInt nbrSteps = deltaY / ( stepInPixels / KDivisorFactor );
    iValue = iMaxSliderValue - nbrSteps;   
    
    if ( iValue < iMinSliderValue )
        {
        iValue = iMinSliderValue;
        }
    
    if (iValue > iMaxSliderValue)
        {
        iValue = iMaxSliderValue;
        }
 
    if ( iValue != oldValue && aPointerEvent.iType == TPointerEvent::EDrag )  
        {
        MTouchFeedback* feedback = MTouchFeedback::Instance(); 
        if ( feedback )
            {
            feedback->InstantFeedback( ETouchFeedbackSensitive );        
            }
        }
    
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        // Change thumb marker to selected
        iBitmapThumbCurrent = iBitmapThumbSelected; 
        iBitmapThumbCurrentMask = iBitmapThumbSelectedMask; 
        }    
    
    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        // Change thumb marker to normal
        iBitmapThumbCurrent = iBitmapThumb; 
        iBitmapThumbCurrentMask = iBitmapThumbMask; 
        }
  
    iSettingObserver->HandleSettingValueUpdateL( iValue );  // Tell observer about change
    DrawNow();
    static_cast<CCamAppUi*>( iCoeEnv->AppUi() )->
        AppController().StartIdleTimer();
    }
    
//  End of File  

