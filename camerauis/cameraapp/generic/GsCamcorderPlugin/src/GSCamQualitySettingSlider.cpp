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
* Description:  Class for showing quality slider*
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
#include <AknsDrawUtils.h>
#include <gscamerapluginrsc.rsg>
#include <aknlayoutscalable_apps.cdl.h>
#include "GSCamQualitySettingSlider.h"
#include "CamPanic.h"    // Panic codes
#include "CamUtility.h"
#include "CamPSI.h"

#include "camconfiguration.h"

// CONSTANTS

const TInt KDivisorFactor = 1024; // Avoids using TReal maths, use factor of 2

const TInt KMinQuality                = 0;
const TInt KMaxBackCameraPhotoQuality = 4;  // Can be 3 or 5 steps => 2 or 4
const TInt KMaxBackCameraVideoQuality = 4;

// Image quality value indexes
const TInt KImageQualityHigh   = 0;
const TInt KImageQualityMedium = 1;
const TInt KImageQualityLow    = 2;
const TInt KImageQualityLower  = 3;
const TInt KImageQualityLowest = 4;

// Video quality value indexes
const TInt KVideoQualityTVHigh          = 0;
const TInt KVideoQualityWide            = 1;
const TInt KVideoQualityEmailHigh       = 2;
const TInt KVideoQualityEmailNormal     = 3;
const TInt KVideoQualitySharing         = 4;

// Video quality type indexes
const TInt KVideoQualityTVHighType      = 5;
const TInt KVideoQualityWideType        = 6;
const TInt KVideoQualityEmailHighType   = 7;
const TInt KVideoQualityEmailNormalType = 8;
const TInt KVideoQualitySharingType     = 9;

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CCamSliderLegend::~CCamSliderLegend
// destructor 
// ---------------------------------------------------------
//
CCamSliderLegend::~CCamSliderLegend()
  {
  delete iText;
  delete iBitmap;
  delete iMask;
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
        MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                                                EAknsCIQsnTextColorsCG6 );

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
// CCamSliderLegend::ConstructL
// Constructs the layout entry
// ---------------------------------------------------------
//    
void CCamSliderLegend::ConstructL( const TAknLayoutRect& aLayoutRect, 
								   TInt aBitmap, TInt aMask )
	{
	TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
	TPtrC resname = resFileName;
	
	iItem = ECamSliderLegendItemIcon;
  	iIconRect = aLayoutRect;

	AknIconUtils::CreateIconL( iBitmap, iMask, resname, aBitmap, aMask );
    AknIconUtils::SetSize( iBitmap, iIconRect.Rect().Size() );
	}

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::CGSCamQualitySettingSlider
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGSCamQualitySettingSlider::CGSCamQualitySettingSlider( 
    MCamSettingValueObserver* aObserver, TCamSettingItemIds aSettingType,
    TInt aSteps, CCamConfiguration* aConfiguration ) : 
    iSettingObserver( aObserver ), iSettingType( aSettingType ), 
    iSteps( aSteps ), iConfiguration ( aConfiguration )
    {
    switch ( iSettingType )
        {
        case ECamSettingItemPhotoQuality:
            {
            if( CamUtility::GetCommonPsiInt( 
                ECamPsiNumberSupportedStillQualities, iMaxSliderValue) != 
                KErrNone )
                {
                iMaxSliderValue = KMaxBackCameraPhotoQuality;
                }
            iMinSliderValue = KMinQuality;
            iNumSliderValues = iMaxSliderValue + 1;
            }
            break;        
            
        case ECamSettingItemVideoQuality:
            {
            if( CamUtility::GetCommonPsiInt( 
                ECamPsiNumberSupportedVideoQualities, iMaxSliderValue) !=
                KErrNone )
                {
                iMaxSliderValue = KMaxBackCameraVideoQuality;
                }
            iMinSliderValue = KMinQuality;
            iNumSliderValues = iMaxSliderValue + 1;
            }
            break;
		default:
			{
            }
            break;
        }

    // Prefill iBitmapShafts and iBitmapShaftMasks with NULL
    for ( TInt i = 0; i <= BITMAP_SHAFT_MAX_DIVISIONS; i++)
        {
            iBitmapShafts[i] = static_cast<CFbsBitmap*>(NULL);
            iBitmapShaftsMasks[i] = static_cast<CFbsBitmap*>(NULL);
        }
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingSlider::ConstructL( const CCoeControl* aParent )
    {
    __ASSERT_DEBUG( aParent!=NULL, CamPanic( ECamPanicNullPointer ) );

    iParentControl = aParent;
    SetContainerWindowL( *iParentControl );  

    TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
    TPtrC resname = resFileName;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance(); 

    if ( IsThreeTicksSlider() ) 
        {
        // Create component bitmaps 
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Image1,
                             iBitmapShafts[KImageQualityHigh], 
                             iBitmapShaftsMasks[KImageQualityHigh], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_image_1,
                             EMbmCameraappQgn_graf_nslider_cam4_image_1_mask );
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Image2,
                             iBitmapShafts[KImageQualityMedium], 
                             iBitmapShaftsMasks[KImageQualityMedium], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_image_2,
                             EMbmCameraappQgn_graf_nslider_cam4_image_2_mask );
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Image3,
                             iBitmapShafts[KImageQualityLow], 
                             iBitmapShaftsMasks[KImageQualityLow], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_image_3,
                             EMbmCameraappQgn_graf_nslider_cam4_image_3_mask );
        }
    else // iSettingType == ECamSettingItemVideoQuality 
        {
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Video1,
                             iBitmapShafts[KVideoQualityTVHigh], 
                             iBitmapShaftsMasks[KVideoQualityTVHigh], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_video_1,
                             EMbmCameraappQgn_graf_nslider_cam4_video_1_mask );
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Video2, 
                             iBitmapShafts[KVideoQualityWide], 
                             iBitmapShaftsMasks[KVideoQualityWide], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_video_2,
                             EMbmCameraappQgn_graf_nslider_cam4_video_2_mask );
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Video3, 
                             iBitmapShafts[KVideoQualityEmailHigh], 
                             iBitmapShaftsMasks[KVideoQualityEmailHigh], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_video_3,
                             EMbmCameraappQgn_graf_nslider_cam4_video_3_mask );
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Video4, 
                             iBitmapShafts[KVideoQualityEmailNormal], 
                             iBitmapShaftsMasks[KVideoQualityEmailNormal], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_video_4,
                             EMbmCameraappQgn_graf_nslider_cam4_video_4_mask );
        AknsUtils::CreateIconL( skin, KAknsIIDQgnGrafNsliderCam4Video5, 
                             iBitmapShafts[KVideoQualitySharing], 
                             iBitmapShaftsMasks[KVideoQualitySharing], 
                             resname, 
                             EMbmCameraappQgn_graf_nslider_cam4_video_5,
                             EMbmCameraappQgn_graf_nslider_cam4_video_5_mask );
        }
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSCamQualitySettingSlider* CGSCamQualitySettingSlider::NewL( 
                                           const CCoeControl* aParent, 
                                           MCamSettingValueObserver* aObserver,
                                           TCamSettingItemIds aSettingItem,
                                           TInt aSteps,
                                           CCamConfiguration* aConfiguration )
    {
    CGSCamQualitySettingSlider* self = 
        new( ELeave ) CGSCamQualitySettingSlider( aObserver, aSettingItem, 
                                                  aSteps, aConfiguration);
    CleanupStack::PushL( self );
    self->ConstructL( aParent );
    CleanupStack::Pop( self );
    self->iConfiguration = aConfiguration;    
    return self;
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::~CGSCamQualitySettingSlider
// Destructor
// -----------------------------------------------------------------------------
//    
CGSCamQualitySettingSlider::~CGSCamQualitySettingSlider()
  {
  for ( TInt i = 0; i <= BITMAP_SHAFT_MAX_DIVISIONS; i++)
    {
        delete iBitmapShafts[i];
        delete iBitmapShaftsMasks[i];
    }

  // Destroy legend strings
  iLegendArray.ResetAndDestroy();
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::InitializeL
// Sets up the slider with the initial value
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingSlider::InitializeL( TInt aValue, 
                                              HBufC* aTopSubtitle,
                                              HBufC* aBottomSubtitle )
    {
    // Check the initial value is valid.  If no, leave.
    if ( aValue < iMinSliderValue ||
         aValue > iMaxSliderValue )
        {
        PRINT3( _L("Camera <> CGSCamQualitySettingSlider::InitializeL - illegal value: %d (min %d, max %d)" ),
                aValue, iMinSliderValue, iMaxSliderValue );  
        User::Leave( KErrArgument );
        }
           
    iValue = aValue;
    iTopSubtitle = aTopSubtitle;
    iBottomSubtitle = aBottomSubtitle;
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::MinValue
// Returns the minimum slider value
// -----------------------------------------------------------------------------
//
TInt CGSCamQualitySettingSlider::MinValue() const
    {
    return iMinSliderValue;
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::MaxValue
// Returns the maximum slider value
// -----------------------------------------------------------------------------
//
TInt CGSCamQualitySettingSlider::MaxValue() const
    {
    return iMaxSliderValue;
    }
    
      
// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::SetRange
// Sets the minimum and maximum values of the slider control
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingSlider::SetRange( TInt aMin, TInt aMax ) 
    {
    ASSERT( aMin < aMax );
    iMinSliderValue = aMin;
    iMaxSliderValue = aMax;    
    iNumSliderValues = iMaxSliderValue + 1;
    }
    

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::Draw
// Draws the slider 
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingSlider::Draw( const TRect& /*aRect*/ ) const
    {
    CFbsBitmap* shaft; 
    CFbsBitmap* shaftMask;
    CWindowGc& gc = SystemGc();
    TRgb color;

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                               EAknsCIQsnTextColorsCG6 );

    MAknsControlContext *cc = AknsDrawUtils::ControlContext( iParentControl ); 
    //AknsDrawUtils::Background( skin, cc, iParentControl, gc, Rect() );
  
    // draw top and bottom subtitles
    iLayoutSubtitleTop.DrawText( gc, iTopSubtitle->Des(), ETrue, color );
    iLayoutSubtitleBottom.DrawText( gc, iBottomSubtitle->Des(), ETrue, color );
    
    // Draw the legend icons
    TInt count = iLegendArray.Count();
    TInt i = 0;
    for ( i = 0; i < count; i++ )
        {
        iLegendArray[i]->Draw( gc );
        }

    // the quality slider has a different shaft icon for each slider value
    if ( iMaxSliderValue <= BITMAP_SHAFT_MAX_DIVISIONS
         && iValue <= BITMAP_SHAFT_MAX_DIVISIONS
         && iMaxSliderValue >= 0
         && iValue >= 0 )
        {
        shaft = iBitmapShafts[iValue];
        shaftMask = iBitmapShaftsMasks[iValue];
        }
    else
        {
        shaft = NULL;
        }

    // Draw the shaft bitmap    
    if ( shaft )
        {
        iShaftLayout.DrawImage( gc, shaft, shaftMask );
        }
    }


// ---------------------------------------------------------
// CGSCamQualitySettingSlider::SizeChanged
// Calculates the new minimum size
// ---------------------------------------------------------
//
void CGSCamQualitySettingSlider::SizeChanged()
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
// CGSCamQualitySettingSlider::OfferKeyEventL
// Allows the user to change the current EV value via key presses.
// -----------------------------------------------------------------------------
//
TKeyResponse CGSCamQualitySettingSlider::OfferKeyEventL( 
                                 const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aType == EEventKey )
        {
        if ( aKeyEvent.iScanCode == EStdKeyUpArrow ) 
            {
            if ( iValue <  iMaxSliderValue )
                {
                iValue ++;
                // Tell observer about change
                iSettingObserver->HandleSettingValueUpdateL( iValue );  
                DrawNow();
                }

            return EKeyWasConsumed;
            }
        else if ( aKeyEvent.iScanCode == EStdKeyDownArrow ) 
            {
            if ( iValue > iMinSliderValue )
                {
                iValue --;
                // Tell observer about change
                iSettingObserver->HandleSettingValueUpdateL( iValue );  
                DrawNow();
                }
            
            return EKeyWasConsumed;
            }
        else if ( aKeyEvent.iCode == EKeyOK && aKeyEvent.iRepeats == 0 )
            {
            return EKeyWasConsumed;
            }
        // No allowing left and right key events thansfer to other view.
        else if( aKeyEvent.iScanCode == EStdKeyLeftArrow
              || aKeyEvent.iScanCode == EStdKeyRightArrow )
            {
            return EKeyWasConsumed;
            }
        // otherwise, do nothing
        else
            {
            // empty statement to remove Lint error, MISRA required rule 60
            }
        }
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// CGSCamQualitySettingSlider::MinimumSize
// ---------------------------------------------------------
//
TSize CGSCamQualitySettingSlider::MinimumSize()
    {
    TSize zerosize = TSize( 0, 0 );
    if ( iMinimumSize == zerosize )
        {
        SizeChanged();
        }
    return iMinimumSize; 
    }


// ---------------------------------------------------------
// CGSCamQualitySettingSlider::ReadLayoutL
// ---------------------------------------------------------
//
void CGSCamQualitySettingSlider::ReadLayoutL()
    {
    // We can use Rect() as the parent rectangle for subcomponents, because
    // CGSCamQualitySettingSlider uses the main_cset_slider_pane layout

    // We're starting again from scratch, so delete the old layout(s)
    iLegendArray.ResetAndDestroy();

    if( iSettingType == ECamSettingItemVideoQuality )
    	{
        if ( CamUtility::IsNhdDevice() )
            {
            TouchLayoutVideo();
            }
        else
            {
            NonTouchLayoutVideo();
            }

        AppendLegendArrayL( ECamPsiVideoQualityIconIds );
        AppendLegendArrayL( ECamPsiVideoQualityFileTypeId, iNumSliderValues );
    	}   
    else // iSettingType == ECamSettingItemPhotoQuality 
    	{
        if ( CamUtility::IsNhdDevice() )
            {
            TouchLayoutPhoto();
            }
        else
            {
            NonTouchLayoutPhoto();
            }
        AppendLegendArrayL( ECamPsiStillQualityIconIds );                       
    	}
    
    // set the shaft icon sizes
    for ( TInt i = 0; i < BITMAP_SHAFT_MAX_DIVISIONS+1; i++ ) 
        {
        AknIconUtils::SetSize( iBitmapShafts[ i ], iShaftLayout.Rect().Size(),
                               EAspectRatioNotPreserved );
        }

    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::TouchLayoutVideo
// Generate layouts for slider legend icons
// -----------------------------------------------------------------------------
//      
void CGSCamQualitySettingSlider::TouchLayoutVideo() 
    {
    // Set the shaft layout structure
    iShaftLayout.LayoutRect( Rect(), 
                               AknLayoutScalable_Apps::cset_slider_pane( 4 ) );  
    // set subtitle text layouts
    iLayoutSubtitleTop.LayoutText( Rect(), 
                       AknLayoutScalable_Apps::main_cset_slider_pane_t1( 4 ) );  
    iLayoutSubtitleBottom.LayoutText( Rect(), 
                       AknLayoutScalable_Apps::main_cset_slider_pane_t7( 4 ) );    
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::NonTouchLayoutVideo
// Generate layouts for slider legend icons                       
// -----------------------------------------------------------------------------
//      
void CGSCamQualitySettingSlider::NonTouchLayoutVideo() 
    {
    // Set the shaft layout structure
    iShaftLayout.LayoutRect( Rect(), 
                               AknLayoutScalable_Apps::cset6_slider_pane( 4 ));
    // set subtitle text layouts
    iLayoutSubtitleTop.LayoutText( Rect(), 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_t1( 4 ) );
    iLayoutSubtitleBottom.LayoutText( Rect(), 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_t7( 4 ) );
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::TouchLayoutPhoto
// Generate layouts for slider legend icons
// -----------------------------------------------------------------------------
//      
void CGSCamQualitySettingSlider::TouchLayoutPhoto() 
    {
    iShaftLayout.LayoutRect( Rect(), 
                             AknLayoutScalable_Apps::cset_slider_pane( 2 )  );  
    // set subtitle text layouts
    iLayoutSubtitleTop.LayoutText( Rect(), 
                       AknLayoutScalable_Apps::main_cset_slider_pane_t1( 2 ) );
    iLayoutSubtitleBottom.LayoutText( Rect(), 
                       AknLayoutScalable_Apps::main_cset_slider_pane_t7( 2 ) ); 
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::NonTouchLayoutPhoto
// Generate layouts for slider legend icons                       
// -----------------------------------------------------------------------------
//      
void CGSCamQualitySettingSlider::NonTouchLayoutPhoto() 
    {
    // Set the shaft layout structure
    iShaftLayout.LayoutRect( Rect(), 
                               AknLayoutScalable_Apps::cset6_slider_pane( 2 ));
    // set subtitle text layouts
    iLayoutSubtitleTop.LayoutText( Rect(), 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_t1( 2 ) );
    iLayoutSubtitleBottom.LayoutText( Rect(), 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_t7( 2 ) );
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::AppendLegendArrayL  
// -----------------------------------------------------------------------------
//  
void CGSCamQualitySettingSlider::AppendLegendArrayL( TCamPsiKey aKey, 
                                                  TInt aNumSliderValues )
    {
    RArray<TInt> array;
    CleanupClosePushL( array );

    TAknLayoutRect layout;
    iConfiguration->GetPsiIntArrayL( aKey, array );
    for( TInt i = 0; i < array.Count(); i+=2 )
        {
        // get positioning info
        if ( iSettingType == ECamSettingItemVideoQuality )
            {
            if ( CamUtility::IsNhdDevice() )
                {
                layout = GetTouchVideoLegendLayout( Rect(), 
                                                    i/2 + aNumSliderValues );   
                }
            else
                {
                layout = GetNonTouchVideoLegendLayout( Rect(), 
                                                    i/2 + aNumSliderValues );  
                }
            }
        else
            {  
            if ( CamUtility::IsNhdDevice() )
                {
                if ( IsThreeTicksSlider() )
                    {
                    // Touch layouts supports only three ticks sliders
                    layout = GetTouchPhotoLegendLayout( Rect(), 
                                                      i/2 + aNumSliderValues );   
                    }
                else
                    {
                    // Five ticks slider is taken from NonTouch layouts
                    layout = GetNonTouchPhotoLegend5Layout( Rect(), 
                                                      i/2 + aNumSliderValues );   
                    }
                }
            else
                {                
                if ( IsThreeTicksSlider() )
                    {
                    layout = GetNonTouchPhotoLegend3Layout( Rect(), 
                                                    i/2 + aNumSliderValues ); 
                    }
                else
                    {
                    layout = GetNonTouchPhotoLegend5Layout( Rect(), 
                                                    i/2 + aNumSliderValues );  
                    }
                }
            }

        CCamSliderLegend* legend = new ( ELeave ) CCamSliderLegend();
        CleanupStack::PushL( legend );
        legend->ConstructL( layout, array[i], array[i+1] );

        User::LeaveIfError( iLegendArray.Append( legend ) );
        CleanupStack::Pop( legend );
        }
    CleanupStack::PopAndDestroy( &array );
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::GetTouchVideoLegendLayout
// Select layout for slider legend icons
// -----------------------------------------------------------------------------
//  
TAknLayoutRect CGSCamQualitySettingSlider::GetTouchVideoLegendLayout( 
    const TRect& aParentRect, 
    TInt aIndex ) const
    {
    TAknLayoutRect res;
    switch ( aIndex )
        {
        case KVideoQualityTVHigh :
            {
            res.LayoutRect( aParentRect, 
                       AknLayoutScalable_Apps::main_cset_slider_pane_g7( 0 ) );  
            break;
            }
        case KVideoQualityTVHighType :
            {
            res.LayoutRect( aParentRect, 
                       AknLayoutScalable_Apps::main_cset_slider_pane_g8( 0 ) );  
            break;
            }
        case KVideoQualityWide :
            {
            res.LayoutRect( aParentRect,  
                      AknLayoutScalable_Apps::main_cset_slider_pane_g11( 0 ) ); 
                    
            break;
            }
        case KVideoQualityWideType :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset_slider_pane_g12( 0 ) );
            break;
            }
        case KVideoQualityEmailHigh :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset_slider_pane_g16( 0 ) );  
            break;
            }
        case KVideoQualityEmailHighType :
            {
            res.LayoutRect( aParentRect,  
                      AknLayoutScalable_Apps::main_cset_slider_pane_g17( 0 ) ); 
            break;
            }
        case KVideoQualityEmailNormal :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset_slider_pane_g13( 0 ) );  
            break;
            }
        case KVideoQualityEmailNormalType :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset_slider_pane_g14( 0 ) );  
            break;
            }
        case KVideoQualitySharing :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset_slider_pane_g16( 0 ) );  
            break;
            }
        case KVideoQualitySharingType :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset_slider_pane_g17( 0 ) );  
            break;
            }
        default:
            {
            break;
            }
        }
    return res;
    }
   
// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::GetNonTouchVideoLegendLayout        
// Select layout for slider legend icons
// -----------------------------------------------------------------------------
//  
TAknLayoutRect CGSCamQualitySettingSlider::GetNonTouchVideoLegendLayout( 
    const TRect& aParentRect, 
    TInt aIndex ) const
    {
    TAknLayoutRect res;
    switch ( aIndex )
        {
        case KVideoQualityTVHigh :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g7( 0 ) );
            break;
            }
        case KVideoQualityTVHighType :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g8( 0 ) ); 
            break;
            }
        case KVideoQualityWide :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g11( 0 ) ); 
            break;
            }
        case KVideoQualityWideType :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g12( 0 ) );  
            break;
            }
        case KVideoQualityEmailHigh :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g16( 0 ) );  
            break;
            }
        case KVideoQualityEmailHighType :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g17( 0 ) );
            break;
            }       
        case KVideoQualityEmailNormal :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g13( 0 ) );  
            break;
            }
        case KVideoQualityEmailNormalType :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g14( 0 ) );  
            break;
            }
        case KVideoQualitySharing :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g16( 0 ) );  
            break;
            }
        case KVideoQualitySharingType :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g17( 0 ) );  
            break;
            }          
        default:
            {
            break;
            }
        }
    return res;
    }
   
// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::GetTouchPhotoLegendLayout
// Select layout for slider legend icons
// -----------------------------------------------------------------------------
//  
TAknLayoutRect CGSCamQualitySettingSlider::GetTouchPhotoLegendLayout( 
    const TRect& aParentRect, 
    TInt aIndex ) const
    {
    TAknLayoutRect res;  
    switch ( aIndex )
        {
        case KImageQualityHigh :
            {
            res.LayoutRect( aParentRect, 
                       AknLayoutScalable_Apps::main_cset_slider_pane_g3( 0 ) );  
            break;
            }
        case KImageQualityMedium :
            {
            res.LayoutRect( aParentRect, 
                       AknLayoutScalable_Apps::main_cset_slider_pane_g4( 0 ) );  
            break;
            }
        case KImageQualityLow :
            {
            res.LayoutRect( aParentRect, 
                       AknLayoutScalable_Apps::main_cset_slider_pane_g5( 0 ) );  
            break;
            }
        default:
            {
            res.LayoutRect( aParentRect, 
                       AknLayoutScalable_Apps::main_cset_slider_pane_g5( 0 ) ); 
            break;
            }
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::GetNonTouchPhotoLegend3Layout
// Select layout for slider legend icons
// -----------------------------------------------------------------------------
//  
TAknLayoutRect CGSCamQualitySettingSlider::GetNonTouchPhotoLegend3Layout( 
    const TRect& aParentRect, 
    TInt aIndex ) const
    {
    TAknLayoutRect res;  
    switch ( aIndex )
        {
        case KImageQualityHigh :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g7( 0 ) );
            break;
            }           
        case KImageQualityMedium :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g4( 0 ) );  
            break;
            }           
        case KImageQualityLow :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g5( 0 ) );
            break;
            }        
        default:
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g5( 0 ) );
            break;
            }
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::GetNonTouchPhotoLegend5Layout
// Select layout for slider legend icons
// -----------------------------------------------------------------------------
//  
TAknLayoutRect CGSCamQualitySettingSlider::GetNonTouchPhotoLegend5Layout( 
    const TRect& aParentRect, 
    TInt aIndex ) const
    {
    TAknLayoutRect res;  
    switch ( aIndex )
        {
        case KImageQualityHigh :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g7( 0 ) );
            break;
            }           
        case KImageQualityMedium :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g9( 0 ) );
            break;
            }           
        case KImageQualityLow :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g4( 0 ) );  
            break;
            }           
        case KImageQualityLower :
            {
            res.LayoutRect( aParentRect, 
                     AknLayoutScalable_Apps::main_cset6_slider_pane_g13( 0 ) );
                                              
            break;
            }           
        case KImageQualityLowest :
            {
            res.LayoutRect( aParentRect, 
                      AknLayoutScalable_Apps::main_cset6_slider_pane_g5( 0 ) ); 
            break;
            }            
        default:
            {
            res.LayoutRect( aParentRect, 
                       AknLayoutScalable_Apps::main_cset6_slider_pane_g5( 0 ) );  

            break;
            }
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::IsThreeTicksSlider
// Select layout for slider legend icons
// -----------------------------------------------------------------------------
//  
TBool CGSCamQualitySettingSlider::IsThreeTicksSlider() const 
    {
    if ( !iConfiguration )
        {
        return ETrue;
        }
    TInt count; 
    if ( iSettingType == ECamSettingItemVideoQuality )
        {
        count = iConfiguration->NumVideoQualities();
        }
    else
        {
        count = iConfiguration->NumPrimaryImageQualities();
        }

    PRINT1( _L("Camera <> CGSCamQualitySettingSlider::IsThreeTicksSlider count=%d"), count ); 
    return count < 4; // Magic: less than three steps
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingSlider::HandlePointerEventL
// Handles slider pointer events
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingSlider::HandlePointerEventL(
                                           const TPointerEvent& aPointerEvent )
    {   
   
    CCoeControl::HandlePointerEventL( aPointerEvent );
       
    // This is the max bitmap offset from the top of the shaft that the 
    // selection can be
    TInt maxVOffset = iShaftLayout.Rect().Size().iHeight;

    // This is the no. of pixels for a "step"
    TInt stepInPixels = ( maxVOffset * KDivisorFactor ) / iNumSliderValues;

    // Calculate new setting value     
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
    // Tell observer about change
    iSettingObserver->HandleSettingValueUpdateL( iValue );  
    DrawNow();
    
    }
    
//  End of File  

