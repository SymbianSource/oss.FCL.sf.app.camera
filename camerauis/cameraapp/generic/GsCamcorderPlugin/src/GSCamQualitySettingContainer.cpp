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
* Description:  Implements the quality slider for image and video settings*
*/



// INCLUDE FILES
#include <gscamerapluginrsc.rsg>
#include <cameraapp.mbg>
#include <barsread.h>
#include <AknQueryValue.h>
#include <AknIconUtils.h>
#include <StringLoader.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <akniconconfig.h>
#include <akntitle.h>  // CAknTitlePane
#include <aknappui.h>
#include <touchfeedback.h>  

#include "CamAppUiBase.h"
#include "GSCamQualitySettingContainer.h"
#include "GSCamQualitySettingSlider.h"
#include "CamUtility.h"
#include "camuiconstants.h"
#include "MCamAppController.h"
#include "CamControllerObservers.h"
#include "MCamStaticSettings.h"

#include "camconfiguration.h"

// CONSTANTS
const TUint KMaxRemainingImagesShown = 9999;

_LIT( KRemainingImagesNumberFormat, "%04d" );

// MACROS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::CGSCamQualitySettingContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGSCamQualitySettingContainer::CGSCamQualitySettingContainer(
    MCamAppController& aController,
    TCamCameraMode     aMode,
    CCoeControl*       aParent )
  : iController( aController ),
    iParent( aParent ),
    iMode      ( aMode       )
  {

  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingContainer::ConstructL()
  {
  CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() );
  MCamStaticSettings& model = appUi->StaticSettingsModel();
  iController.AddControllerObserverL( this );

  TCamSettingItemIds settingType = ( ECamControllerVideo == iMode )
                                   ? ECamSettingItemVideoQuality
                                   : ECamSettingItemPhotoQuality;

  iSlider = CGSCamQualitySettingSlider::NewL( iParent, this, settingType, 0,
                                              &(model.Configuration()) );

    
  iSelectedQuality = model.IntegerSettingValue( settingType );
  
  iTimeFormat = ( ECamControllerVideo == iMode )                                   
              ? iEikonEnv->AllocReadResourceL( R_QTN_TIME_DURAT_LONG )
              : KRemainingImagesNumberFormat().AllocL();

  TFileName resFileName;
  CamUtility::ResourceFileName( resFileName );
  TPtrC resname = resFileName;
  
  // ...phone memory icon
  AknIconUtils::CreateIconL( iPhoneIcon,
                             iPhoneIconMask,
                             resname,
                             EMbmCameraappQgn_indi_cam4_memory_phone,
                             EMbmCameraappQgn_indi_cam4_memory_phone_mask );

  // ...mmc icon
  AknIconUtils::CreateIconL( iMMCIcon,
                             iMMCIconMask,
                             resname, 
                             EMbmCameraappQgn_indi_cam4_memory_mmc,
                             EMbmCameraappQgn_indi_cam4_memory_mmc_mask );

   // ...mass storage icon
  AknIconUtils::CreateIconL( iMassStorageIcon,
                             iMassStorageIconMask,
                             resname, 
                             EMbmCameraappQgn_indi_cam4_memory_mass,
                             EMbmCameraappQgn_indi_cam4_memory_mass_mask );

  UpdateRemainingTextL();
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSCamQualitySettingContainer* 
CGSCamQualitySettingContainer::NewL( MCamAppController& aController,
                                     TCamCameraMode     aMode,
                                     CCoeControl*       aParent )
  {
  CGSCamQualitySettingContainer* self = 
      new( ELeave ) CGSCamQualitySettingContainer( aController, aMode, aParent );
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop();
  return self;
  }

    
// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::~CGSCamQualitySettingContainer
// Destructor
// -----------------------------------------------------------------------------
//
CGSCamQualitySettingContainer::~CGSCamQualitySettingContainer()
  {
  PRINT( _L("Camera => ~CGSCamQualitySettingContainer") );
  CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
  if ( appUi )
    {
    TInt resId = ( ECamControllerVideo == iMode )
               ? R_CAM_VIDEO_SETTINGS_TITLE_NAME
               : R_CAM_PHOTO_SETTINGS_TITLE_NAME;
    
    TRAP_IGNORE( appUi->SetTitleL( resId ) );
    }
  iController.RemoveControllerObserver( this );
  delete iPhoneIcon;
  delete iPhoneIconMask;
  delete iMMCIcon;
  delete iMMCIconMask;
  delete iMassStorageIcon;
  delete iMassStorageIconMask;

  delete iTimeFormat;

  // delete descriptors from arrays
  if ( iTitleArray )
    {
    iTitleArray->Reset();
    delete iTitleArray;
    }
  if ( iDescArray )
    {
    iDescArray->Reset();
    delete iDescArray;
    }
  if ( iCurrentDescLineArray )
    {
    iCurrentDescLineArray->Reset();
    delete iCurrentDescLineArray;
    }

  iLayoutSettingDescLines.Reset();
  iEnumArray.Reset();

  delete iSlider;
  delete iTextTitle;
  delete iTextTopSubtitle;
  delete iTextBottomSubtitle;
  delete iTextRemainingTitle;

  iSupportedQualitiesArray.Close();
  PRINT( _L("Camera <= ~CGSCamQualitySettingContainer") );
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::SelectedQuality
// Returns the quality selected by the slider
// -----------------------------------------------------------------------------
//
TInt CGSCamQualitySettingContainer::SelectedQuality() const
  {
  return iSelectedQuality;
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::ConstructFromResourceL
// Constructs the container from the resource
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingContainer::ConstructFromResourceL( TResourceReader& aReader )
  {
  CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
  // set up the container windows here as setting page window
  // isn't created until CAknSettingPage::ConstructFromResourceL is called
  SetContainerWindowL( *iParent );
  iSlider->SetContainerWindowL( *iParent );

  // read the texts from the resource file
  iTextTitle = aReader.ReadHBufCL();
  appUi->SetTitleL( iTextTitle->Des() );
  iTextTopSubtitle    = aReader.ReadHBufCL();
  iTextBottomSubtitle = aReader.ReadHBufCL();
  iTextRemainingTitle = aReader.ReadHBufCL();
  
  TCamPsiKey key = ( ECamControllerVideo == iMode )
                   ? ECamPsiSupportedVideoQualities
                   : ECamPsiSupportedStillQualities;


  iConfiguration = &(appUi->StaticSettingsModel().Configuration());
  iConfiguration->GetPsiIntArrayL( key, iSupportedQualitiesArray );  

  iTitleArray = new( ELeave ) CDesCArrayFlat( 1 );
  iDescArray  = new( ELeave ) CDesCArrayFlat( 1 );

  // read the descriptive text and split the text into 3 lines
  ReadInTextArrayL( aReader, *iTitleArray );
  ReadInTextArrayL( aReader, *iDescArray ); 
            
  // Read the setting enums that match with the text arrays.
  TInt count = aReader.ReadInt16();
  TInt suppCount = iSupportedQualitiesArray.Count();
   
  TInt i = 0;    
  TInt j = 0;
   
  for ( i = 0; i < count && j < suppCount ; i++ )
    {
    TInt val = aReader.ReadInt16();
    
    // check if it's a supported quality level
    if ( i == iSupportedQualitiesArray[j] )
      {
      if ( ECamControllerVideo == iMode )
        {
        User::LeaveIfError( iEnumArray.Append( static_cast<TCamVideoQualitySettings>( val ) ) );                
        }
      else
        {
        User::LeaveIfError( iEnumArray.Append( static_cast<TCamImageQualitySetting>( val ) ) );
        }                           
      j++;
      }
    }

  // TCamImageQualitySetting enum starts with highest quality == 0
  // so convert this to value the slider understands i.e. 0 == lowest
  TInt sliderValue = iEnumArray.Count() - 1 - iSelectedQuality;

  iSlider->InitializeL( sliderValue, iTextTopSubtitle, iTextBottomSubtitle );
      
  // Set the new range of values
  iSlider->SetRange( 0, iEnumArray.Count() - 1 );
          
  iCurrentDescLineArray = new( ELeave ) CArrayFixFlat<TPtrC>( 3 );
  // Use the layout info from the first line for width and font
  TAknLayoutText layout = iLayoutSettingDescLines[0];
  AknTextUtils::WrapToArrayL( ( *iDescArray )[iSelectedQuality], 
                              layout.TextRect().Width(), 
                              *( layout.Font() ), 
                              *iCurrentDescLineArray );   
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::CountComponentControls
// return the number of component controls
// -----------------------------------------------------------------------------
//
TInt CGSCamQualitySettingContainer::CountComponentControls() const
  {
  return 1; // the slider
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::ComponentControl
// return the control at a given index
// -----------------------------------------------------------------------------
//
CCoeControl* CGSCamQualitySettingContainer::ComponentControl( TInt /*aIndex*/ ) const
  {
  return iSlider;
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::Draw
// Draws the container
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingContainer::Draw( const TRect& /*aRect*/ ) const
  {
  PRINT( _L("Camera => CGSCamQualitySettingContainer::Draw") );
  CWindowGc& gc = SystemGc();

  // Draw skin background
  MAknsSkinInstance* skin = AknsUtils::SkinInstance();
  MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
  AknsDrawUtils::Background( skin, cc, gc, Rect() );
  
  // draw the title text   
  TRgb color;
  AknsUtils::GetCachedColor( skin, color, KAknsIIDQsnTextColors,
                             EAknsCIQsnTextColorsCG6 );
  
  // draw the quality description text
  DrawQualityTexts( gc, color );

  // draw the remaining images/remaining video time
  iLayoutRemainingTitle.DrawText( gc, iTextRemainingTitle->Des(), ETrue, color );
  
  // draw the remaining images/remaining video time
  TBufC<40> Buf( iTextRemaining );
  TPtr Pointer = Buf.Des();
  AknTextUtils::LanguageSpecificNumberConversion( Pointer );
  iLayoutRemainingText.DrawText( gc, /*iTextRemaining*/Buf, ETrue, color );

  // draw the current location icon
  DrawStorageIcon( gc );

  PRINT( _L("Camera <= CGSCamQualitySettingContainer::Draw") );
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::SizeChanged
// Handles a change in the size of the control
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingContainer::SizeChanged()
  {
  TRAP_IGNORE( ReadLayoutL() );
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::OfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse CGSCamQualitySettingContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                                            TEventCode aType ) 
  {
  TKeyResponse keyResponse = iSlider->OfferKeyEventL( aKeyEvent, aType );
  if ( keyResponse == EKeyWasConsumed )
    {
    ReportEventL( MCoeControlObserver::EEventStateChanged );
    }
  return keyResponse;
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::HandleSettingValueUpdateL
// Handles a change to the setting value of the slider
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingContainer::HandleSettingValueUpdateL( TInt aNewValue )
  {
  ASSERT ( aNewValue >= 0 && 
           aNewValue < iEnumArray.Count() );            
  
  // The slider values range from Max at the top (eg 5) to Min at the bottom (0).
  // For quality settings enum (0 is Max, 4 is Min) need to invert this
  TInt index = iSlider->MaxValue() - aNewValue;
  if ( ECamControllerVideo == iMode )
      {
      iSelectedQuality = index;
      }
  else
      {
      iSelectedQuality = index;
      }
      
  // remove the old lines
  iCurrentDescLineArray->Reset();
  // Use the layout info from the first line for width and font
  TAknLayoutText layout = iLayoutSettingDescLines[0];
  AknTextUtils::WrapToArrayL( ( *iDescArray )[iSelectedQuality], 
                              layout.TextRect().Width(), 
                              *( layout.Font() ), 
                              *iCurrentDescLineArray );

  /* -
  // Update the number of images remaining
  UpdateRemainingTextL();
  */
  // +
  ResetRemainingTextL();

  // Results in a call to CCamQualitySettingPage::UpdateSettingL()
  ReportEventL( MCoeControlObserver::EEventStateChanged );

  /*ActivateGc();
  Window().Invalidate( iLayoutDescRect.Rect() );
  DeactivateGc();*/
  DrawNow();
  }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::HandleControllerEventL
// Handles a change in the controller
// -----------------------------------------------------------------------------
//

void CGSCamQualitySettingContainer::HandleControllerEventL( 
                         TCamControllerEvent aEvent, TInt /*aError*/ )

  {
  PRINT( _L("Camera => CGSCamQualitySettingContainer::HandleControllerEventL") );
  CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() );
  TBool redrawNeeded = EFalse;
  // If this is a capture complete event, or the image quality or save
  // location has changed...
  if ( aEvent == ECamEventImageQualityChanged ||
       aEvent == ECamEventSaveLocationChanged ||
       aEvent == ECamEventVideoQualityChanged  )
    {
    UpdateRemainingTextL();
  
    if ( aEvent == ECamEventSaveLocationChanged )
      {
      /*TCamSettingItemIds storageLocation = ( ECamControllerVideo == iMode) 
                                         ? ECamSettingItemVideoMediaStorage 
                                         : ECamSettingItemPhotoMediaStorage;*/
      // update location
      /*if ( static_cast<CCamAppUi*>( iEikonEnv->AppUi() )->IsMMCRemovedNotePending() )
      	{
      	iStorageLocation = static_cast< TCamMediaStorage >
      		( iController.IntegerSettingValueUnfiltered( storageLocation ) );
      	}
      else
      	{
      	iStorageLocation = static_cast< TCamMediaStorage > 
      		( iController.IntegerSettingValue( storageLocation ) );
      	}*/
      }

    redrawNeeded = ETrue;
    }
  else
    {
    // Other events: no action
    }

  
  if ( redrawNeeded )        
    {
    DrawNow();
    }
  PRINT( _L("Camera <= CGSCamQualitySettingContainer::HandleControllerEventL") );
  }


// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::ReadLayoutL
// Reads the layout information from the resource file
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingContainer::ReadLayoutL()
    {
    TAknLayoutRect settingLayout;
    settingLayout.LayoutRect( Rect(), AknLayoutScalable_Apps::main_cam_set_pane( 0 ) );
    TRect settingRect( settingLayout.Rect() );

    AknLayoutUtils::LayoutControl( iSlider, settingRect, AknLayoutScalable_Apps::main_cset_slider_pane( 2 ) );

    // set the layouts for the rects
    // containing the descriptions
    TRect infoboxRect;
    iLayoutDescRect.LayoutRect( settingRect, AknLayoutScalable_Apps::main_cset_text_pane( 0 ) );
    infoboxRect = iLayoutDescRect.Rect();
    
    // set quality setting title layout
    iLayoutSettingTitle.LayoutText( infoboxRect,  AknLayoutScalable_Apps::main_cset_text_pane_t1( 0 ) );
    
    // set layouts for 3 lines of descriptive text
    iLayoutSettingDescLines.Reset();
    TAknLayoutText layout;
    layout.LayoutText( infoboxRect, AknLayoutScalable_Apps::main_cset_text_pane_t2( 0 ) );
    User::LeaveIfError( iLayoutSettingDescLines.Append( layout ) );
    layout.LayoutText( infoboxRect, AknLayoutScalable_Apps::main_cset_text_pane_t3( 0 ) );
    User::LeaveIfError( iLayoutSettingDescLines.Append( layout ) );
    layout.LayoutText( infoboxRect, AknLayoutScalable_Apps::main_cset_text_pane_t4( 0 ) );
    User::LeaveIfError( iLayoutSettingDescLines.Append( layout ) );

    // set remaining text layouts
    iLayoutRemainingTitle.LayoutText( infoboxRect, AknLayoutScalable_Apps::main_cset_text_pane_t6( 0 ) );
    iLayoutRemainingText.LayoutText( infoboxRect, AknLayoutScalable_Apps::main_cset_text_pane_t7( 0 )  );
    // read the storage icon layout
    iLayoutIcon.LayoutRect( infoboxRect, AknLayoutScalable_Apps::main_cset_text_pane_g1( 0 ) );

    // set size for different memory type icons
    TSize iconSize ( iLayoutIcon.Rect().Size() );
    AknIconUtils::SetSize( iPhoneIcon, iconSize, EAspectRatioPreserved );
    AknIconUtils::SetSize( iMMCIcon, iconSize, EAspectRatioPreserved );
    AknIconUtils::SetSize( iMassStorageIcon, iconSize, EAspectRatioPreserved );
    }

// -----------------------------------------------------------------------------
// CGSCamQualitySettingContainer::DrawQualityTexts
// Draws the quality information texts
// -----------------------------------------------------------------------------
//
void CGSCamQualitySettingContainer::DrawQualityTexts( CWindowGc& aGc, TRgb aColor ) const
    {
    // Set the brushstyle to ENullBrush in order to not show the 
    // text background box. Otherwise it would show slightly from
    // beneath the shaded background
    //aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
    iLayoutSettingTitle.DrawText
        ( aGc, ( *iTitleArray )[iSelectedQuality], ETrue, aColor );

    TInt count = Min( iCurrentDescLineArray->Count(), iLayoutSettingDescLines.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        if ( i < count )
            {
            iLayoutSettingDescLines[i].
                DrawText( aGc, ( *iCurrentDescLineArray )[i], ETrue, aColor );
            }
        }       
    }

// ---------------------------------------------------------
// CGSCamQualitySettingContainer::DrawStorageIcon
// Draw the icon for the media storage location
// ---------------------------------------------------------
//
void CGSCamQualitySettingContainer::DrawStorageIcon( CWindowGc& aGc ) const
    {
    TInt key = (ECamControllerVideo == iMode)
             ? ECamSettingItemVideoMediaStorage
             : ECamSettingItemPhotoMediaStorage;
    TCamMediaStorage storageLocation = 
        static_cast<TCamMediaStorage>( iController.IntegerSettingValue( key ) );
    CFbsBitmap* icon = NULL;
    CFbsBitmap* mask = NULL;

    switch( storageLocation )
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

    iLayoutIcon.DrawImage( aGc, icon, mask );
    }

// ---------------------------------------------------------
// CGSCamQualitySettingContainer::DrawRemainingText
// Draw the remaining image count/video time
// ---------------------------------------------------------
//
void CGSCamQualitySettingContainer::DrawRemainingText( CWindowGc& aGc ) const
    {
    // Set the brushstyle to ENullBrush in order to not show the 
    // text background box. Otherwise it would show slightly from
    // beneath the shaded background
    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
    aGc.Clear( iLayoutRemainingText.TextRect() );
    iLayoutRemainingText.DrawText( aGc, iTextRemaining );
    }

// ---------------------------------------------------------
// CGSCamQualitySettingContainer::UpdateRemainingTextL
// Update the remaining image count/video time remaining 
// ---------------------------------------------------------
//
void CGSCamQualitySettingContainer::UpdateRemainingTextL()
  {
  PRINT( _L("Camera => CGSCamQualitySettingContainer::UpdateRemainingTextL") );

  if ( ECamControllerVideo == iMode )
    {
    // Get the total remaining record time from the controller
    TTime time ( iController.RecordTimeRemaining().Int64() );
/* -
    HBufC* timeFormat = 
      iEikonEnv->AllocReadResourceLC( R_QTN_TIME_DURAT_MIN_SEC_WITH_ZERO );
    time.FormatL( iTextRemaining, *timeFormat );
    CleanupStack::PopAndDestroy( timeFormat );
*/
// +
    time.FormatL( iTextRemaining, *iTimeFormat );
    }
  else
    {
    // use the stored image resolution to calculated the remaining images
    TBool burstActive = static_cast<CCamAppUiBase*>
                        ( iEikonEnv->AppUi() )->IsBurstEnabled();
    
    TInt imagesLeft = iController.ImagesRemaining( ECamMediaStorageCurrent, 
                                                   burstActive, 
                                                   iSelectedQuality );
    TInt showLeft   = Min( imagesLeft, KMaxRemainingImagesShown );
// -
//    iTextRemaining.Format( KRemainingImagesNumberFormat, showLeft );
// +
    iTextRemaining.Format( *iTimeFormat, showLeft );
    }

  PRINT( _L("Camera <= CGSCamQualitySettingContainer::UpdateRemainingTextL") );
  }

// ---------------------------------------------------------
// ResetRemainingTextL
// ---------------------------------------------------------
//
void 
CGSCamQualitySettingContainer::ResetRemainingTextL()
  {
  /* mm: Disabled for now. Below code sets to 00:00.
         Proposed --:-- for video, no action for image (too fast).
  if ( ECamControllerVideo == iMode )
    {
    TTime time( TInt64( 0 ) );
    time.FormatL( iTextRemaining, *iTimeFormat );
    }
  else 
    {
    }
  */
  }
 
// ---------------------------------------------------------
// CGSCamQualitySettingContainer::ReadInTextArrayL
// Populate a text array from the resource definition of available
// quality texts using the supported quality values
// ---------------------------------------------------------
//   
void CGSCamQualitySettingContainer::ReadInTextArrayL( TResourceReader& aReader, CDesCArrayFlat& aArray )
  {               
  CDesCArrayFlat* tempArray;
  tempArray = aReader.ReadDesC16ArrayL();
  
  TInt descCount = tempArray->Count();
  TInt suppCount = iSupportedQualitiesArray.Count();
   
  TInt i = 0;    
   
  for ( i = 0; i < suppCount; i++ )
    {
    TInt descriptionIndex = 
      iConfiguration->MapSupportedQualityToQualityDescriptor( 
          iMode == ECamControllerVideo, 
          iSupportedQualitiesArray[i] ); 

    if ( descriptionIndex >= 0 
      && descriptionIndex < descCount )   
      {
      aArray.AppendL( ( *tempArray )[descriptionIndex] );
      }        
    } 
  tempArray->Reset();
  delete tempArray;     
  tempArray = NULL;   
  } 

//  End of File  
