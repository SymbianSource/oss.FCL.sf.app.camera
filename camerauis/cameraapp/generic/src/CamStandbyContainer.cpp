/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for standby mode*
*/


// INCLUDE FILES
#include <aknview.h>
#include <eiklabel.h>
#include <StringLoader.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknsDrawUtils.h>
#include <AknBidiTextUtils.h>
#include <textresolver.h>
#include "CamStandbyContainer.h"
#include "CamTimer.h"
#include "Cam.hrh"
#include "CamUtility.h"
#include "CamAppUi.h"
#include "CamViewBase.h"
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <ctsydomainpskeys.h>

// CONSTANTS

const TInt KForegroundDelayTimeout = 500;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamStandbyContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStandbyContainer* CCamStandbyContainer::NewL( const TRect& aRect, CAknView& aView,
                                                  CCamAppController& aController,
                                                  TInt aError )

    {
    CCamStandbyContainer* self = 
                  new( ELeave ) CCamStandbyContainer( aController, aView, aError );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aError );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamStandbyContainer::~CCamStandbyContainer
// Destructor
// ---------------------------------------------------------------------------
//
CCamStandbyContainer::~CCamStandbyContainer()
  {
  PRINT( _L("Camera => ~CCamStandbyContainer"))
  if( iForegroundTimer )
    {
    iForegroundTimer->Cancel();
    delete iForegroundTimer;
    }
    
  delete iText;
  delete iWrappedLabelText;    
  PRINT( _L("Camera => ~CCamStandbyContainer") );
  }
  
// ---------------------------------------------------------
// CCamStandbyContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamStandbyContainer::ConstructL( const TRect& aRect, TInt aError )
    {
    PRINT( _L("Camera => CCamStandbyContainer::ConstructL"))
    CCamContainerBase::BaseConstructL( aRect );

    iText = new ( ELeave ) CEikLabel;
    iText->SetContainerWindowL( *this );

    iText->SetAlignment( EHCenterVCenter );
    iText->SetExtent( TPoint( 0, 0 ), aRect.Size() );
    // tell the CEikLabel that it should not do the visual 
    // conversion (Arabic right to left) because the app 
    // will do it via 
    // AknBidiTextUtils::ConvertToVisualAndWrapToStringL
    iText->UseLogicalToVisualConversion( EFalse );

    // Get label text from resource
    HBufC* labelText;
    if ( static_cast<CCamAppUi*>( iEikonEnv->AppUi() )->IsConstructionComplete() )
        {
	    switch( aError )
	        {
	        case KErrNone:
	            labelText = StringLoader::LoadLC( R_CAM_STANDBY_TEXT );
	            break;
	        case KErrInUse:
	            labelText = StringLoader::LoadLC( R_CAM_STANDBY_CAMERA_IN_USE_TEXT );
	            break;
	        case KErrNoMemory:
	        	labelText = StringLoader::LoadLC( R_CAM_MEMLO_NOT_ENOUGH_MEMORY );
	            break;
	        case KErrPermissionDenied:
	            labelText = StringLoader::LoadLC( R_CAM_STANDBY_CAMERA_DISABLED_TEXT );
                break;
	        default: // any other error
	            labelText = StringLoader::LoadLC( R_CAM_STANDBY_CAMERA_UNAVAILABLE_TEXT );
	            break;
	        }
        }
	 else
        {
        labelText = StringLoader::LoadLC( R_CAM_STANDBY_EMPTY_TEXT );
        }        


    iText->SetTextL( *labelText );
    // Wrap the text, where it doesn't fit.
    // ...Create the line width array necessary for ConvertToVisual..()
    const TInt KLabelMaxLines = 5;
    CArrayFix<TInt>* lineWidths = 
        new ( ELeave ) CArrayFixFlat<TInt> ( KLabelMaxLines );
    CleanupStack::PushL( lineWidths );
    for (TInt i = 0; i < KLabelMaxLines; ++i )
        {
        lineWidths->AppendL( aRect.Width() );            
        }
    // ...Create the text buffer that will receive the wrapped text.
    iWrappedLabelText = HBufC::NewL(
        labelText->Length() + KLabelMaxLines * ( KAknBidiExtraSpacePerLine + 1 ) );
    TPtr ptr = iWrappedLabelText->Des();
    // ...Get the wrapped text conversion.
    AknBidiTextUtils::ConvertToVisualAndWrapToStringL(
        labelText->Des(),
        *lineWidths,
        *AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont ),
        ptr,
        ETrue
        );
    // ...Cleanup
    CleanupStack::PopAndDestroy( lineWidths );  
    CleanupStack::PopAndDestroy( labelText );

    // Set the wrapped text to the label.
    iText->SetTextL( *iWrappedLabelText );

    // Initialise foreground timer - used to delay the exit
    // from standby when foreground is gained
    iForegroundTimer = CCamTimer::NewL( KForegroundDelayTimeout,
                  TCallBack( CCamStandbyContainer::ForegroundTimeout, this ) );

    // Make sure fixed toolbar is on top of this window
    DrawableWindow()->SetOrdinalPosition( KCamPreCaptureWindowOrdinalPos );
    }
    
// ---------------------------------------------------------------------------
// CCamStandbyContainer::CCamStandbyContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamStandbyContainer::CCamStandbyContainer( CCamAppController& aController,
                                                        CAknView& aView, TInt aError )
: CCamContainerBase( aController, aView ), iStandbyError( aError )
    {
    }

// ---------------------------------------------------------
// CCamStandbyContainer::CountComponentControls
// Returns the number of controls owned
// ---------------------------------------------------------
//
TInt CCamStandbyContainer::CountComponentControls() const
    {
    TInt count = 1;
    return count; // Return the number of controls inside this container
    }

// ---------------------------------------------------------
// CCamStandbyContainer::ComponentControl
// Return pointer to a contained control
// ---------------------------------------------------------
//
CCoeControl* CCamStandbyContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iText;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ---------------------------------------------------------
// CCamStandbyContainer::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamStandbyContainer::Draw( const TRect& /*aRect*/ ) const
    {
    PRINT( _L("Camera => CCamStandbyContainer::Draw"))
    CWindowGc& gc = SystemGc();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsDrawUtils::Background( skin, iBgContext, gc, Rect() );

    // override the label color
    TRgb color;
    AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                                            EAknsCIQsnTextColorsCG6 );
    TRAPD( ignore, AknLayoutUtils::OverrideControlColorL( *iText, 
        EColorLabelText, color ) );
    if ( ignore )
        { 
        // Do nothing ( removes build warning )
        }     
    }

// ----------------------------------------------------------------
// CCamStandbyContainer::OfferKeyEventL
// Handles this application view's command keys
// ----------------------------------------------------------------
//
TKeyResponse CCamStandbyContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( iStandbyError != KErrNone )
        {
        return EKeyWasConsumed;
        }

    // if the select (OK) key is pressed
    if ( ( aType == EEventKey && aKeyEvent.iCode == EKeyOK )
        // if shutter key is pressed in main camera
        || ( aType == EEventKey && 
           ( aKeyEvent.iScanCode == iPrimaryCameraCaptureKeys[0] ||
               CCamContainerBase::IsShutterKeyL(aKeyEvent, aType) ) &&
               !static_cast<CCamAppUi*>( iEikonEnv->AppUi() )->IsSecondCameraEnabled() )
        || ( IsCaptureKeyL(aKeyEvent, aType)) )
        {
        // in case we receive an enter key or MSK key event in standby mode
        // we should not call HandleCommandL function with ECamCmdExitStandby
        // because by default application will get a left softkey command id
        // when MSK is pressed. In standby mode left softkey command id is
        // ECamCmdExitStandby
        if ( !( ( aType == EEventKey || 
                  aType == EEventKeyDown || 
                  aType == EEventKeyUp ) && 
                ( aKeyEvent.iScanCode == EStdKeyDevice3 ||
                  aKeyEvent.iScanCode == EStdKeyEnter ) ) )
             {
             iView.HandleCommandL( ECamCmdExitStandby );
             }
        return EKeyWasConsumed;
        }
    // handle navi-key up/down, left/right
    else if ( aType == EEventKey &&
        ( aKeyEvent.iScanCode == EStdKeyDownArrow ||
          aKeyEvent.iScanCode == EStdKeyUpArrow ||
          aKeyEvent.iScanCode == EStdKeyLeftArrow ||
          aKeyEvent.iScanCode == EStdKeyRightArrow ) )
        {
        iView.HandleCommandL( ECamCmdExitStandby );
        return EKeyWasConsumed;
        }
    else if ( ( aType == EEventKeyDown || aType == EEventKeyUp ) && 
    	      ( aKeyEvent.iScanCode != EStdKeyYes ) )
        {
        if ( aType == EEventKeyDown )
            {
            if( IsZoomKeyL(aKeyEvent, aType ) )
            	{
            	iView.HandleCommandL( ECamCmdExitStandby );
            	}
            }           
        // consume key up/down presses
        return EKeyWasConsumed;
        }
    else
        {
        return CCamContainerBase::OfferKeyEventL( aKeyEvent, aType );
        }
    }

// ----------------------------------------------------------------
// CCamStandbyContainer::HandleForegroundEvent
// Handle foreground event
// ----------------------------------------------------------------
//
void CCamStandbyContainer::HandleForegroundEventL( TBool aForeground )
    {
    if ( aForeground )
        {
        // start timer to delay exit from standby
        if ( !iForegroundTimer->IsActive() )
            {
            iForegroundTimer->StartTimer();
            }
        }
    else
        {
        // set text label as blank
        TRAP_IGNORE( iText->SetTextL( KNullDesC ) );
        
        // cancel timer if active
        if ( iForegroundTimer->IsActive() )
            {
            iForegroundTimer->Cancel();
            }
        }
    }
    
// ----------------------------------------------------------------
// CCamStandbyContainer::GetStandbyError
// Returns the current error
// ----------------------------------------------------------------
//
TInt CCamStandbyContainer::GetStandbyError() const
    {
    return iStandbyError;
    }    

// ----------------------------------------------------------------
// CCamStandbyContainer::ForegroundTimeout
// static callback function for foreground gain timeout
// ----------------------------------------------------------------
//
TInt CCamStandbyContainer::ForegroundTimeout( TAny *aPtr )
    {
    return static_cast<CCamStandbyContainer*>( aPtr )->DoForegroundTimeout();
    }

// ----------------------------------------------------------------
// CCamStandbyContainer::DoForegroundTimeout
// Implementation function for foreground gain timeout
// ----------------------------------------------------------------
//
TInt CCamStandbyContainer::DoForegroundTimeout()
    {
    TRAPD( ignore, iView.HandleCommandL( ECamCmdExitStandby ) );
    if ( ignore )
        { 
        // Do nothing ( removes build warning )
        }             
    
    return EFalse;
    }

// ----------------------------------------------------------------
// CCamStandbyContainer::HandlePointerEventL
// ----------------------------------------------------------------
//
void CCamStandbyContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    RDebug::Print(_L("CCamStandbyContainer::HandlePointerEventL iType=%d iPosition=(%d, %d)"),
        aPointerEvent.iType,
        aPointerEvent.iPosition.iX,
        aPointerEvent.iPosition.iY );

      TInt callType( EPSCTsyCallTypeUninitialized );
      RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType, callType );
       if ( callType != EPSCTsyCallTypeH324Multimedia)
         {
    static_cast<CCamViewBase&>( iView ).HandleCommandAoL( ECamCmdExitStandby );
         }
         else
         {
         }
    }

// -----------------------------------------------------------------------------
// CCamStandbyContainer::CreateBackgroundContextL
// -----------------------------------------------------------------------------
//     
void CCamStandbyContainer::CreateBackgroundContextL()
    {
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
        KAknsIIDQsnBgAreaMain, Rect(), EFalse );
    }

// End of File  
