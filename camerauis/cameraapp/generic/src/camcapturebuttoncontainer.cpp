/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for custom camera capture button 
*
*/

#include <AknIconUtils.h>
#include <touchfeedback.h>

#include "camcapturebuttoncontainer.h"
#include "CamPreCaptureViewBase.h"
#include "camlogging.h"
#include "cameraapp.mbg"
#include "CamAppUi.h"

// CONSTANTS
_LIT(KCamBitmapFile, "z:\\resource\\apps\\cameraapp.mif");
const TInt KCaptureButtonOrdinalPriority( 1 );
const TSize KIconMargin( 7, 7 );

// -----------------------------------------------------------------------------
// CCamCaptureButtonContainer::CCamCaptureButtonContainer
// -----------------------------------------------------------------------------
//
CCamCaptureButtonContainer::CCamCaptureButtonContainer( CCamAppController& aController,
                                                        CAknView& aView,
                                                        CCamPreCaptureContainerBase& aContainer,
                                                        TCamCameraMode aCameraMode ) 
    : iController( aController ), 
      iView( aView ),
      iParentContainer( aContainer ),
      iCameraMode( aCameraMode )
    {
    }

// -----------------------------------------------------------------------------
// CCamCaptureButtonContainer::ConstructL
// -----------------------------------------------------------------------------
//
void CCamCaptureButtonContainer::ConstructL( const TRect& aRect )
    {
    PRINT( _L("Camera => CCamCaptureButtonContainer::ConstructL") );
    CreateWindowL();
    SetRect( aRect );
    Window().SetBackgroundColor( KRgbTransparent );

    // Keep hidden until explicitly made visible
    MakeVisible( EFalse );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

    // Load capture icon
    TInt bitmapId = EMbmCameraappQgn_indi_cam4_capture_image;
    TInt maskId   = EMbmCameraappQgn_indi_cam4_capture_image_mask;
    TInt bitmapPressedId = EMbmCameraappQgn_indi_cam4_capture_image_pressed;
    TInt maskPressedId = EMbmCameraappQgn_indi_cam4_capture_image_pressed_mask;

    if ( iCameraMode == ECamControllerVideo )
        {
        bitmapId = EMbmCameraappQgn_indi_cam4_capture_video;
        maskId   = EMbmCameraappQgn_indi_cam4_capture_video_mask;
        bitmapPressedId = EMbmCameraappQgn_indi_cam4_capture_video_pressed;
        maskPressedId = EMbmCameraappQgn_indi_cam4_capture_video_pressed_mask;
        }
    
    iCaptureRect = aRect;
    iCaptureRect.Move(-iCaptureRect.iTl.iX, -iCaptureRect.iTl.iY );
    iCaptureRect.Shrink( KIconMargin );
    
    AknIconUtils::CreateIconL(
             iCaptureIcon,
             iCaptureIconMask,
             KCamBitmapFile(),
             bitmapId,
             maskId );
    AknIconUtils::SetSize( iCaptureIcon, iCaptureRect.Size() );
    AknIconUtils::SetSize( iCaptureIconMask, iCaptureRect.Size() );
    
    AknIconUtils::CreateIconL(
                 iCaptureIconPressed,
                 iCaptureIconPressedMask,
                 KCamBitmapFile(),
                 bitmapPressedId,
                 maskPressedId );
    AknIconUtils::SetSize( iCaptureIconPressed, iCaptureRect.Size() );
    AknIconUtils::SetSize( iCaptureIconPressedMask, iCaptureRect.Size() );
    
    iFeedback = MTouchFeedback::Instance();
    PRINT( _L("Camera <= CCamCaptureButtonContainer::ConstructL") );
    }

// -----------------------------------------------------------------------------
// CCamCaptureButtonContainer::~CCamCaptureButtonContainer
// -----------------------------------------------------------------------------
//
CCamCaptureButtonContainer::~CCamCaptureButtonContainer()
    {
    delete iCaptureIcon;
    delete iCaptureIconMask;
    delete iCaptureIconPressed;
    delete iCaptureIconPressedMask;
    }

// -----------------------------------------------------------------------------
// CCamCaptureButtonContainer::NewLC
// -----------------------------------------------------------------------------
//
CCamCaptureButtonContainer* CCamCaptureButtonContainer::NewLC( CCamAppController& aController,
                                                               CAknView& aView,
                                                               CCamPreCaptureContainerBase& aContainer,
                                                               const TRect& aRect,
                                                               TCamCameraMode aCameraMode )
    {
    CCamCaptureButtonContainer* self = new (ELeave) CCamCaptureButtonContainer( aController, 
                                                                                aView,
                                                                                aContainer,
                                                                                aCameraMode );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CCamCaptureButtonContainer::NewL
// -----------------------------------------------------------------------------
//
CCamCaptureButtonContainer* CCamCaptureButtonContainer::NewL( CCamAppController& aController,
                                                              CAknView& aView,
                                                              CCamPreCaptureContainerBase& aContainer,
                                                              const TRect& aRect, 
                                                              TCamCameraMode aCameraMode )
    {
    CCamCaptureButtonContainer* self = CCamCaptureButtonContainer::NewLC( aController, 
                                                                          aView,
                                                                          aContainer,
                                                                          aRect,
                                                                          aCameraMode );
    CleanupStack::Pop(self);
    return self;
    }

// -------------------------------------------------------------
// CCamCaptureButtonContainer::HandlePointerEventL
// -------------------------------------------------------------
//
void CCamCaptureButtonContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    PRINT3( _L("Camera => CCamCaptureButtonContainer::HandlePointerEventL type:%d position:%d,%d"), 
                     aPointerEvent.iType, aPointerEvent.iPosition.iX, aPointerEvent.iPosition.iY );    

    if( iCaptureButtonShown )
        {
        PRINT( _L("Camera <> Capture button shown") );
        // Button down -> pressed (highlighted)
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
            SetPointerCapture( ETrue );
            ClaimPointerGrab( ETrue );
            iCaptureButtonPressed = ETrue;
            if ( !iFeedback )
                {
                iFeedback = MTouchFeedback::Instance();
                }
            // Feedback on button press
            if ( iFeedback )
                {
                iFeedback->InstantFeedback( ETouchFeedbackBasicButton );        
                }
            DrawNow();
            }
        // Button up after button down (within button area) -> start capture
        else if ( aPointerEvent.iType == TPointerEvent::EButton1Up
                  && iCaptureButtonPressed && Rect().Contains(aPointerEvent.iPosition) )
            {
            PRINT( _L("Camera <> starting capture") );
            ClaimPointerGrab( EFalse );
            SetPointerCapture( EFalse );
            iCaptureButtonPressed = EFalse;

            // Give feedback on button release
            if ( iFeedback )
                {
                iFeedback->InstantFeedback( ETouchFeedbackBasicButton );        
                }

            iParentContainer.PrepareForCapture();
            if ( iCameraMode == ECamControllerVideo ) 
                {
                iView.HandleCommandL( ECamCmdRecord );
                }
            else
                {
                iController.SetTouchCapture( ETrue );
                iView.HandleCommandL( ECamCmdCaptureImage );
                }
            }
        // Drags can potentially start from inside button area
        else if ( iCaptureButtonPressed && aPointerEvent.iType != TPointerEvent::EDrag )
            {
            PRINT( _L("Camera <> outside button region - button to not-pressed state") );
            ClaimPointerGrab( EFalse );
            SetPointerCapture( EFalse );
            iCaptureButtonPressed = EFalse;
            DrawNow();
            }
        else
            {
            PRINT( _L("Camera <> unhandled case") );
            // Avoid compiler warning
            }
        }

    PRINT( _L("Camera <= CCamCaptureButtonContainer::HandlePointerEventL") );
    }

// -------------------------------------------------------------
// CCamCaptureButtonContainer::SetCaptureButtonShown
// -------------------------------------------------------------
//
void CCamCaptureButtonContainer::SetCaptureButtonShown( TBool aShown )
    {
    PRINT1( _L("Camera => CCamCaptureButtonContainer::SetCaptureButtonShown shown:%d"), aShown );
    iCaptureButtonShown = aShown;

    if ( !aShown )
        {
        MakeVisible( EFalse );
        }
    else if ( !IsVisible() )
        {
        // Ensure button is in non-pressed state when making visible 
        iCaptureButtonPressed = EFalse;

        PRINT( _L("Camera <> Making the capture button visible") );
        DrawableWindow()->SetOrdinalPosition( 0, KCaptureButtonOrdinalPriority );
        MakeVisible( ETrue );
        DrawDeferred();
        }
    else
        {
        PRINT( _L("Camera <> Already visible!") );
        }

    PRINT( _L("Camera <= CCamCaptureButtonContainer::SetCaptureButtonShown") );
    }

// -----------------------------------------------------------------------------
// CCamCaptureButtonContainer::Draw
// -----------------------------------------------------------------------------
//
void CCamCaptureButtonContainer::Draw( const TRect& aRect ) const
    {
    PRINT( _L("Camera => CCamCaptureButtonContainer::Draw") );

    CWindowGc& gc = SystemGc();
    if ( iCaptureButtonShown )
        {
        DrawCaptureButton( gc );
        }
    else
        {
        PRINT( _L("Camera <> button hidden") );
        }

    PRINT( _L("Camera <= CCamCaptureButtonContainer::Draw") );
    }

// -------------------------------------------------------------
// CCamCaptureButtonContainer::DrawCaptureButton
// -------------------------------------------------------------
//
void CCamCaptureButtonContainer::DrawCaptureButton( CBitmapContext& aGc ) const
    {
    PRINT( _L("Camera => CCamCaptureButtonContainer::DrawCaptureButton") );
    TRect boundingRect( iCaptureRect );
    boundingRect.Move( -boundingRect.iTl.iX, -boundingRect.iTl.iY );

    // Clear the window
    aGc.SetPenStyle( CGraphicsContext::ENullPen );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    aGc.SetBrushColor( TRgb::Color16MA( 0 ) );
    aGc.Clear( boundingRect ); 

    if ( iCaptureButtonPressed )
        {
        aGc.BitBltMasked( iCaptureRect.iTl, iCaptureIconPressed, boundingRect, iCaptureIconPressedMask, EFalse );
        }
    else
        {
        aGc.BitBltMasked( iCaptureRect.iTl, iCaptureIcon, boundingRect, iCaptureIconMask, EFalse );

        }

    PRINT( _L("Camera <= CCamCaptureButtonContainer::DrawCaptureButton") );
    }

