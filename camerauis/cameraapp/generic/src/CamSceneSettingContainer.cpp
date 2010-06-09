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
* Description:  Container class for scene settings view*
*/

// INCLUDE FILES
#include "CamSceneSettingContainer.h"
#include "CamUtility.h"
#include "CamPanic.h"

#include <eikenv.h>
#include <eikappui.h> // For CCoeAppUiBase
#include <eikapp.h>   // For CEikApplication
#include <stringloader.h>
#include <AknBidiTextUtils.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <cameraapp.mbg>
#include <BitmapTransforms.h>
#include <AknUtils.h>
#include <AknLayoutFont.h>
#include <barsRead.h>    // resource reader

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>


// CONSTANTS

const TInt KStepSize = 4;  // Pixels the animation moves each timer event 

const TInt KCornerSize = 3;
const TInt KBorderSize = 1;

const TReal KNumIconStages = 7;

#define KRgbGray4 TRgb(0xEEEEEE)
#define KRgbGray3 TRgb(0xDDDDDD)
#define KRgbGray2 TRgb(0xCCCCCC)
#define KRgbGray1 TRgb(0xBBBBBB)


#ifdef __WINS__
static const TInt KTimerPeriod = 10000;
#else
static const TInt KTimerPeriod = 150000;
#endif

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TCamAnimationData::Initialise
// Sets up the animation data
// -----------------------------------------------------------------------------
//
void TCamAnimationData::Initialise( const TRect& aRect )
    {
    iDeltaY = KStepSize;
    // scrolling needs to be faster (2 times?) than the fade in/out
    iScrollingY = iDeltaY * 2;

    iCurrentRect = aRect;
    iPreviousRect = iCurrentRect;
    iCurrentRectOld = iCurrentRect;
    iPreviousRectOld = iCurrentRect;

    iStep = 0;
    iPenFadeOut = KRgbBlack;
    iPenFadeIn = KRgbWhite;
    iScrolling = EFalse;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamSceneSettingItem* CCamSceneSettingItem::NewL( TCamSceneId aSceneId,
                                                  const TSize& aLargeBmpSize,
                                                  const TSize& aSmallBmpSize )
    {
    CCamSceneSettingItem* self =  new( ELeave ) CCamSceneSettingItem( aSceneId );
    CleanupStack::PushL( self );
    self->ConstructL( aLargeBmpSize, aSmallBmpSize );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::~CCamSceneSettingItem
// Destructor
// ---------------------------------------------------------------------------
//
CCamSceneSettingItem::~CCamSceneSettingItem()
  {
  PRINT( _L("Camera => ~CCamSceneSettingItem") );
  delete iBitmapSmall;
  delete iBitmap1;
  delete iBitmap2;
  delete iBitmap3;
  delete iBitmap4;
  delete iBitmap5;
  delete iBitmap6;
  delete iBitmapLarge;
  delete iTitle;
  delete iDescription;
  PRINT( _L("Camera <= ~CCamSceneSettingItem") );
  }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::SceneId
// Returns the sceneId
// ---------------------------------------------------------------------------
//
TCamSceneId CCamSceneSettingItem::SceneId() const
    {
    return iSceneId;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::Icon
// Returns the bitmap specified by aSize
// ---------------------------------------------------------------------------
//
const CFbsBitmap* CCamSceneSettingItem::Icon( TCamIconSize aSize ) const
    {
    if ( aSize == ECamIconSizeLarge )
        {
        return iBitmapLarge;
        }
    else if ( aSize == ECamIconSizeSmall )
        {
        return iBitmapSmall;
        }
    else if ( aSize == ECamIconSizeOne )
        {
        return iBitmap1;
        }
    else if ( aSize == ECamIconSizeTwo )
        {
        return iBitmap2;
        }
    else if ( aSize == ECamIconSizeThree )
        {
        return iBitmap3;
        }
    else if ( aSize == ECamIconSizeFour )
        {
        return iBitmap4;
        }
    else if ( aSize == ECamIconSizeFive )
        {
        return iBitmap5;
        }
    else if ( aSize == ECamIconSizeSix )
        {
        return iBitmap6;
        }
    else
        {
        return iBitmapLarge;
        }
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::Title
// returns the title text
// ---------------------------------------------------------------------------
//
const HBufC* CCamSceneSettingItem::Title() const
    {
    return iTitle;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::Description
// returns the title text
// ---------------------------------------------------------------------------
//
const HBufC* CCamSceneSettingItem::Description() const
    {
    return iDescription;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::ConstructL
// Symbian OS second phase constructor
// ---------------------------------------------------------------------------
//
void CCamSceneSettingItem::ConstructL( const TSize& aLargeBmpSize, 
                                       const TSize& aSmallBmpSize )
    {
    TInt bmpLarge = 0;
    TInt bmpSmall = 0;
    TInt title = 0;
    TInt description = 0;
    switch ( iSceneId )
        {
        case ECamSceneNight:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_night_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_night_l;
            title = R_CAM_SCENE_TITLE_NIGHT;
            description = R_CAM_SCENE_DESCRIPTION_NIGHT;
            }
            break;
        case ECamSceneMacro:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_macro_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_macro_l;    
            title = R_CAM_SCENE_TITLE_MACRO;
            description = R_CAM_SCENE_DESCRIPTION_MACRO;
            }
            break;
        case ECamScenePortrait:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_port_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_port_l;          
            title = R_CAM_SCENE_TITLE_PORTRAIT;
            description = R_CAM_SCENE_DESCRIPTION_PORTRAIT;
            }
            break;
        case ECamSceneAuto:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_auto_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_auto_l;
            title = R_CAM_SCENE_TITLE_AUTO;
            description = R_CAM_SCENE_DESCRIPTION_AUTO;
            }
            break;
        case ECamSceneScenery:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_land_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_land_l;           
            title = R_CAM_SCENE_TITLE_LANDSCAPE;
            description = R_CAM_SCENE_DESCRIPTION_LANDSCAPE;
            }
            break;
        case ECamSceneSports:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_sport_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_sport_l;           
            title = R_CAM_SCENE_TITLE_SPORTS;
            description = R_CAM_SCENE_DESCRIPTION_SPORTS;
            }
            break;
        case ECamSceneUser:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_user_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_user_l;           
            title = R_CAM_SCENE_TITLE_USER;
            description = R_CAM_SCENE_DESCRIPTION_USER;
            }
            break;
        case ECamSceneNormal:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_auto_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_auto_l;           
            title = R_CAM_SCENE_TITLE_NORMAL;
            description = R_CAM_SCENE_DESCRIPTION_NORMAL;
            }
            break;
        case ECamSceneNightScenery:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_nightland_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_nightland_l;
            title = R_CAM_SCENE_TITLE_NIGHT_LANDSCAPE;
            description = R_CAM_SCENE_DESCRIPTION_NIGHT_LANDSCAPE;
            }
            break;
        case ECamSceneNightPortrait:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_nightport_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_nightport_l;
            title = R_CAM_SCENE_TITLE_NIGHT_PORTRAIT;
            description = R_CAM_SCENE_DESCRIPTION_NIGHT_PORTRAIT;
            }
            break;
        case ECamSceneCandlelight:
            {
            bmpSmall = EMbmCameraappQgn_ico_lcam_sce_candle_s;
            bmpLarge = EMbmCameraappQgn_ico_lcam_sce_candle_l;           
            title = R_CAM_SCENE_TITLE_CANDLELIGHT;
            description = R_CAM_SCENE_DESCRIPTION_CANDLELIGHT;
            }
            break;
        default:
            User::Leave( KErrGeneral );
            break;
        }

    // Find the name and path of the MBM file for bitmaps
    TFileName mbmFileName;
    CamUtility::ResourceFileName( mbmFileName );

    iBitmapSmall = AknIconUtils::CreateIconL( mbmFileName, bmpSmall );
    AknIconUtils::SetSize( iBitmapSmall, aSmallBmpSize, EAspectRatioNotPreserved );

    TReal xInc = TReal( aLargeBmpSize.iWidth - aSmallBmpSize.iWidth ) / KNumIconStages;
    TReal yInc = TReal( aLargeBmpSize.iHeight - aSmallBmpSize.iHeight ) / KNumIconStages;

    TSize size;
    TReal width = aSmallBmpSize.iWidth;
    TReal height = aSmallBmpSize.iHeight;
    
    width += xInc;
    height += yInc;
    size.SetSize( ( TInt ) width, ( TInt ) height );

    iBitmap1 = AknIconUtils::CreateIconL( mbmFileName, bmpLarge );
    AknIconUtils::SetSize( iBitmap1, size, EAspectRatioNotPreserved);

    width += xInc;
    height += yInc;
    size.SetSize( ( TInt ) width, ( TInt ) height );

    iBitmap2 = AknIconUtils::CreateIconL( mbmFileName, bmpLarge );
    AknIconUtils::SetSize( iBitmap2, size, EAspectRatioNotPreserved );

    width += xInc;
    height += yInc;
    size.SetSize( ( TInt ) width, ( TInt ) height );

    iBitmap3 = AknIconUtils::CreateIconL( mbmFileName, bmpLarge );
    AknIconUtils::SetSize( iBitmap3, size, EAspectRatioNotPreserved );

    width += xInc;
    height += yInc;
    size.SetSize( ( TInt ) width, ( TInt ) height );

    iBitmap4 = AknIconUtils::CreateIconL( mbmFileName, bmpLarge );
    AknIconUtils::SetSize( iBitmap4, size, EAspectRatioNotPreserved );

    width += xInc;
    height += yInc;
    size.SetSize( ( TInt ) width, ( TInt ) height );

    iBitmap5 = AknIconUtils::CreateIconL( mbmFileName, bmpLarge );
    AknIconUtils::SetSize( iBitmap5, size, EAspectRatioNotPreserved );

    width += xInc;
    height += yInc;
    size.SetSize( ( TInt ) width, ( TInt ) height );

    iBitmap6 = AknIconUtils::CreateIconL( mbmFileName, bmpLarge );
    AknIconUtils::SetSize( iBitmap6, size, EAspectRatioNotPreserved );

    iBitmapLarge = AknIconUtils::CreateIconL( mbmFileName, bmpLarge );
    AknIconUtils::SetSize( iBitmapLarge, aLargeBmpSize, EAspectRatioNotPreserved );
    
    iTitle = StringLoader::LoadL( title );
    iDescription = StringLoader::LoadL( description );
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingItem::CCamSceneSettingContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamSceneSettingItem::CCamSceneSettingItem( TCamSceneId aSceneId ) :
    iSceneId( aSceneId )
    {
    }  

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamSceneSettingContainer* 
CCamSceneSettingContainer::NewL( const TRect& aRect, 
                                 CAknView& aView,
                                 TCamCameraMode aMode,
                                 CCamAppController& aController,
                                 TBool aUserBaseScenes )
  {
  CCamSceneSettingContainer* self = 
      new( ELeave ) CCamSceneSettingContainer( aMode,
                                               aController,
                                               aView, 
                                               aUserBaseScenes );
  CleanupStack::PushL( self );
  self->ConstructL( aRect );
  CleanupStack::Pop( self );
  return self;
  }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::~CCamSceneSettingContainer
// Destructor
// ---------------------------------------------------------------------------
//
CCamSceneSettingContainer::~CCamSceneSettingContainer()
  {
  PRINT( _L( "Camera => ~CCamSceneSettingContainer" ) );
  
  iSettingItemArray.ResetAndDestroy();
  iSettingItemArray.Close();
  
  iHighlightIcons.Close();
  iNormalIcons.Close();
  iDisplacedIcons.Close();
  
  iHighlightArray.Close();
  iHighlightTitles.Close();
  iHighlightDescriptions.Close();
  
  iNormalTitles.Close();
  iDisplacedTitles.Close();
  
  if (iAnimTimer)
    {
    iAnimTimer->Cancel();
    }
  delete iAnimTimer;
  delete iScrollFrame;
  delete iAnimData.iOffScreenBitmap;
  
  PRINT( _L( "Camera <= ~CCamSceneSettingContainer" ) );    
  }
  
// ---------------------------------------------------------
// CCamSceneSettingContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamSceneSettingContainer::ConstructL( const TRect& aRect )
  {
  PRINT(_L("Camera => CCamSceneSettingContainer::ConstructL") )
  
  CCamContainerBase::BaseConstructL( aRect );

  TBool secondCameraOn = 
      static_cast<CCamAppUiBase*>( iEikonEnv->AppUi() )->IsSecondCameraEnabled();

  if( !AknLayoutUtils::LayoutMirrored() )
    {
    // get the icon positions from the resource
    ReadLayoutL( ROID(R_CAM_SCENE_ICON_HIGHLIGHT_ARRAY_ID), iHighlightIcons );
    ReadLayoutL( ROID(R_CAM_SCENE_ICON_DISPLACED_ARRAY_ID), iDisplacedIcons );
    ReadLayoutL( ROID(R_CAM_SCENE_ICON_NORMAL_ARRAY_ID), iNormalIcons );

    // get the highlight rects
    ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_LAYOUT_ARRAY_ID), iHighlightArray );

    // Check if we require APAC layouts
    if( AknLayoutUtils::Variant() == EApacVariant )
      {
      // get the layout texts
      ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_TITLE_ARRAY_APAC_ID), iHighlightTitles );
      ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_DESCRIPTION_ARRAY_APAC_ID), iHighlightDescriptions );

      ReadLayoutL( ROID(R_CAM_SCENE_TITLE_NORMAL_ARRAY_APAC_ID), iNormalTitles );
      ReadLayoutL( ROID(R_CAM_SCENE_TITLE_DISPLACED_ARRAY_APAC_ID), iDisplacedTitles );
      }
    else
      {
      // get the layout texts
      ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_TITLE_ARRAY_ID), iHighlightTitles );
      ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_DESCRIPTION_ARRAY_ID), iHighlightDescriptions );
      
      ReadLayoutL( ROID(R_CAM_SCENE_TITLE_NORMAL_ARRAY_ID), iNormalTitles );
      ReadLayoutL( ROID(R_CAM_SCENE_TITLE_DISPLACED_ARRAY_ID), iDisplacedTitles );
      }
    }
  else
    {
    // get the icon positions from the resource
    ReadLayoutL( ROID(R_CAM_SCENE_ICON_HIGHLIGHT_ARRAY_AH_ID), iHighlightIcons );
    ReadLayoutL( ROID(R_CAM_SCENE_ICON_DISPLACED_ARRAY_AH_ID), iDisplacedIcons );
    ReadLayoutL( ROID(R_CAM_SCENE_ICON_NORMAL_ARRAY_AH_ID), iNormalIcons );

    // get the highlight rects
    ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_LAYOUT_ARRAY_AH_ID), iHighlightArray );

    // get the layout texts
    ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_TITLE_ARRAY_AH_ID), iHighlightTitles );
    ReadLayoutL( ROID(R_CAM_SCENE_HIGHLIGHT_DESCRIPTION_ARRAY_AH_ID), iHighlightDescriptions );

    ReadLayoutL( ROID(R_CAM_SCENE_TITLE_NORMAL_ARRAY_AH_ID), iNormalTitles );
    ReadLayoutL( ROID(R_CAM_SCENE_TITLE_DISPLACED_ARRAY_AH_ID), iDisplacedTitles );
    }

  TResourceReader reader;                                                                                     
  iEikonEnv->CreateResourceReaderLC( reader, ROID(R_CAM_SCENE_MAX_ITEMS_TO_DISPLAY_ID));    
  iNumberOfIconsToDisplay = reader.ReadInt16();
  CleanupStack::PopAndDestroy(); // reader

  if ( iUserBaseScenes )
    {
    CreateSceneArrayL( R_CAM_SCENE_LIST_USER );
    TCamSceneId currentScene = static_cast<TCamSceneId>( 
        iController.IntegerSettingValue( 
            ECamSettingItemUserSceneBasedOnScene ) );
    SetCurrentPositionToScene( currentScene );      
    SetupScrollbarL();
    }
  else if ( ECamControllerImage == iMode )
    {
    if ( secondCameraOn )
      {
      CreateSceneArrayL( R_CAM_SCENE_LIST_PHOTO_CAM2 );
      }
    else
      {
      CreateSceneArrayL( R_CAM_SCENE_LIST_PHOTO);         
      SetupScrollbarL();
      }

    TCamSceneId currentScene = static_cast<TCamSceneId>( 
            iController.IntegerSettingValue( 
                ECamSettingItemDynamicPhotoScene ) );
    SetCurrentPositionToScene( currentScene );      
    }
  else
    {
    if ( secondCameraOn )
      {
      CreateSceneArrayL( R_CAM_SCENE_LIST_VIDEO_CAM2);
      }
    else
      {
      CreateSceneArrayL( R_CAM_SCENE_LIST_VIDEO );
      }
    
    TCamSceneId currentScene = static_cast<TCamSceneId>( 
            iController.IntegerSettingValue( 
                ECamSettingItemDynamicVideoScene ) );
    SetCurrentPositionToScene( currentScene );      
    }

  TInt index = iCurrentArrayPosition - iTopDisplayPosition;
  iAnimData.Initialise( iHighlightArray[index].Rect() );

  // create a bitmap to be used off-screen
  iAnimData.iOffScreenBitmap = new ( ELeave ) CFbsBitmap();
  User::LeaveIfError( iAnimData.iOffScreenBitmap->Create( aRect.Size(), EColor256 ) );

  TInt leftMarginDiff = iNormalIcons[0].Rect().iTl.iX - 
                        iHighlightIcons[0].Rect().iTl.iX;
  iAnimData.iIconLeftInc = TReal( leftMarginDiff / KNumIconStages );


  // Create the timeout timer
  iAnimTimer = CPeriodic::NewL( EPriorityHigh );
  
  PRINT(_L("Camera <= CCamSceneSettingContainer::ConstructL") )    
  }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::ReadLayoutL
// Read the TAknLayoutRect info from the resource file
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::ReadLayoutL( TInt aResourceId, 
                                             RArray<TAknLayoutRect>& aArray) const
  {
  TResourceReader reader;                                                                                     
  iEikonEnv->CreateResourceReaderLC( reader, aResourceId );    
  const TInt count = reader.ReadInt16();
  
  TInt i;
  // Read all of the layout entries from the resource file
  for ( i = 0; i < count; i++ )
    {
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( Rect(), reader );
    User::LeaveIfError( aArray.Append( layoutRect ) );
    }
  CleanupStack::PopAndDestroy(); // reader
  }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::ReadLayoutL
// Read the TAknLayoutText info from the resource file
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::ReadLayoutL( TInt aResourceId, 
                                             RArray<TAknLayoutText>& aArray) const
  {
  TResourceReader reader;                                                                                     
  iEikonEnv->CreateResourceReaderLC( reader, aResourceId );    
  const TInt count = reader.ReadInt16();
  
  TInt i;
  // Read all of the layout entries from the resource file
  for ( i = 0; i < count; i++ )
    {
    TAknLayoutText layoutText;
    layoutText.LayoutText( Rect(), reader );
    User::LeaveIfError( aArray.Append( layoutText ) );
    }
  CleanupStack::PopAndDestroy(); // reader
  }
    
// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::CCamSceneSettingContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamSceneSettingContainer::CCamSceneSettingContainer( 
    TCamCameraMode aMode,
    CCamAppController& aController,
    CAknView& aView,
    TBool aUserBaseScenes )
  : CCamContainerBase( aController, aView ), 
    iMode( aMode ), 
    iUserBaseScenes( aUserBaseScenes )
  {
  }

// ----------------------------------------------------------------
// CCamSceneSettingContainer::UserSceneHighlighted
// Returns ETrue if the current selected scene is User
// ----------------------------------------------------------------
//
TBool CCamSceneSettingContainer::UserSceneHighlighted()
  {
  return ( iSettingItemArray[iCurrentArrayPosition]->SceneId() == 
           ECamSceneUser );
  }

// ----------------------------------------------------------------
// CCamSceneSettingContainer::SaveSceneSetting
// Stores the currently selected scene in the dynamic settings model
// ----------------------------------------------------------------
//
TBool CCamSceneSettingContainer::SaveSceneSettingL()
  {
  TInt scene = 0;
  if ( iUserBaseScenes )
    {
    scene = ECamSettingItemUserSceneBasedOnScene;
    }
  else if ( ECamControllerImage == iMode )
    {
    scene = ECamSettingItemDynamicPhotoScene;
    }
  else
    {
    scene = ECamSettingItemDynamicVideoScene;
    }

  iController.SetIntegerSettingValueL( scene,
              iSettingItemArray[iCurrentArrayPosition]->SceneId() );

  // If user selected sports scene from list.
  if ( iSettingItemArray[iCurrentArrayPosition]->SceneId() == ECamSceneSports )
    {
    // if the selection was for user scene base scene, and user 
    // scene is active, but request was cancelled, return false.
    if ( ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene )
             == ECamSceneUser ) &&
         ( iController.IntegerSettingValue( ECamSettingItemUserSceneBasedOnScene )
             != ECamSceneSports ) )
      {
      return EFalse;
      }
    // if the selection was for the photo/video scene, but request was 
    // cancelled, return false.
    if ( ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene )
         != ECamSceneUser )  && 
         ( iController.IntegerSettingValue( scene ) != ECamSceneSports ) )
      {
      return EFalse;
      }
    }
  // Otherwise, if the user selected user scene from the list, but request
  // was cancelled, return false.
  else if ( ( iSettingItemArray[iCurrentArrayPosition]->SceneId() 
            == ECamSceneUser ) &&
            ( iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene )
           != ECamSceneUser ) )
    {
    return EFalse;
    }

  // Remove lint warning.
  else
    {
    }

  // In all other cases, return true.
  return ETrue;
  }


// ---------------------------------------------------------
// CCamSceneSettingContainer::CountComponentControls 
// Returns the number of controls owned
// ---------------------------------------------------------
//
TInt CCamSceneSettingContainer::CountComponentControls() const
  {
  TInt count = CCamContainerBase::CountComponentControls(); // Return the number of controls inside this container    
  if ( iScrollFrame )
    {
    count += iScrollFrame->CountComponentControls();
    }
  return count;
  }

// ---------------------------------------------------------
// CCamSceneSettingContainer::ComponentControl
// Returns the requested component control
// ---------------------------------------------------------
//
CCoeControl* CCamSceneSettingContainer::ComponentControl( TInt aIndex ) const
  {
  CCoeControl* control = NULL;
  if( aIndex == 0 )
      control = CCamContainerBase::ComponentControl( aIndex );
  else
      {
      if ( iScrollFrame )
          {
          return iScrollFrame->ComponentControl( aIndex - 1 );
          }
      }
  return control;
  }

// ---------------------------------------------------------
// CCamSceneSettingContainer::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamSceneSettingContainer::Draw( const TRect& /*aRect*/ ) const
  {
  PRINT(_L("Camera => CCamSceneSettingContainer::Draw") )
  
  CWindowGc& gc = SystemGc();
  // Draw the background
  gc.SetBrushColor( KRgbWhite );
  gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
  gc.SetPenStyle( CGraphicsContext::ENullPen );
  gc.DrawRect( Rect() );
  gc.SetPenStyle( CGraphicsContext::ESolidPen );
  
  TBool drawnHighlight = EFalse;
  TInt index = 0;
  
  TInt i;
  for ( i = 0; i < iNumberOfIconsToDisplay; i++ )
    {
    TInt position = iTopDisplayPosition + i;
    CCamSceneSettingItem* item = iSettingItemArray[position];
    const HBufC* title = item->Title();
    const HBufC* description = item->Description();

    // Drawing currently selected icon and text
    if ( position == iCurrentArrayPosition )
      {
      // Draw the highlight
      gc.DrawRoundRect( iHighlightArray[i].Rect(), 
                        TSize( KCornerSize, KCornerSize ) );
      
      // Draw the enlarged icon
      iHighlightIcons[i].DrawImage( gc, 
          const_cast<CFbsBitmap*>( item->Icon( ECamIconSizeLarge ) ), NULL );
      
      // Draw the enlarged header text
      iHighlightTitles[i].DrawText( gc, *title );
      
      // Draw the description text
      iHighlightDescriptions[i].DrawText( gc, *description );
      
#if 0       // debug drawing code
      gc.SetPenColor(KRgbRed);
      gc.DrawRect( iHighlightTitles[i].TextRect() );
      gc.DrawRect( iHighlightDescriptions[i].TextRect() );
#endif
      drawnHighlight = ETrue;
      }
    else    // Drawing normal items
      {
      // Draw the highlight box
      TAknLayoutRect layoutRect;
      TAknLayoutText layoutText;
      if ( drawnHighlight )
        {
        layoutRect = iNormalIcons[index];
        layoutText = iNormalTitles[index];
        }
      else
        {
        layoutRect = iDisplacedIcons[index];
        layoutText = iDisplacedTitles[index];
        }

      // Draw the standard icon
      layoutRect.DrawImage( gc, 
          const_cast<CFbsBitmap*>( item->Icon( ECamIconSizeSmall ) ), NULL );
      // Draw the header text
      layoutText.DrawText( gc, *title );
            
#if 0       // debug drawing code
      gc.SetPenColor(KRgbRed);
      gc.DrawRect( layoutText.TextRect() );
#endif
      index++;
      }
    }
  PRINT(_L("Camera <= CCamSceneSettingContainer::Draw") )
  }

// ----------------------------------------------------------------
// CCamSceneSettingContainer::OfferKeyEventL
// Handles this application view's command keys. Forwards other
// keys to child control(s).
// ----------------------------------------------------------------
//
TKeyResponse CCamSceneSettingContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    PRINT( _L( "Camera => CCamSceneSettingContainer::OfferKeyEventL" ) );

    if ( iAnimTimer->IsActive() )
        {
        return EKeyWasNotConsumed;
        }

    // Moving down the list
    if ( aKeyEvent.iScanCode == EStdKeyDownArrow && aType == EEventKey )
        {
        // at the bottom of the list and pressing down key
        if ( iCurrentArrayPosition == iSettingItemArray.Count() - 1 )
            {
            // wrap to the top of the list and redraw
            iCurrentArrayPosition = 0;
            iPreviousArrayPosition = 0;
            iTopDisplayPosition = 0;

            iAnimData.iCurrentRect = iHighlightArray[0].Rect();
            iAnimData.iPreviousRect = iAnimData.iCurrentRect;

            if ( iScrollFrame )
                {
                iScrollFrame->MoveVertThumbTo( 0 );
                }

            DrawDeferred();
            return EKeyWasConsumed;
            }

        iAnimData.iMovingDown = ETrue;
        iPreviousArrayPosition = iCurrentArrayPosition;
        iCurrentArrayPosition++;

        // need to scroll the list
        if( iCurrentArrayPosition > iNumberOfIconsToDisplay - 1 )
            {
            iAnimData.iScrolling = ETrue;
            iScrollFrame->MoveThumbsBy( 0, 1 );
            iTopDisplayPosition++;
            DrawListL();

            // blit the bitmap at above the 2nd displaced icon
            TInt yPos = iDisplacedIcons[1].Rect().iTl.iY;
            // less the difference between the top of the first icon on 
            // the off-screen bitmap and the top of the off-screen bitmap
            yPos -= iDisplacedIcons[0].Rect().iTl.iY;
            // less the scrolling value
            yPos -= iAnimData.iScrollingY;
            iAnimData.iOffScreenPos.SetXY( 0, yPos );

            TInt curIndex = iCurrentArrayPosition - iTopDisplayPosition;
            TInt defaultIndex = curIndex - 1;
            iAnimData.iTitleFadeIn = iNormalTitles[defaultIndex].TextRect();

            iAnimData.iCurrentRect = iHighlightArray[curIndex].Rect();
            TRect rect = iNormalIcons[defaultIndex].Rect();
            iAnimData.iCurrentRect.iTl.iY = rect.iTl.iY;
            iAnimData.iCurrentRect.iBr.iY = iAnimData.iCurrentRect.iTl.iY + rect.Height();

            // clear the top item from the list
            ActivateGc();
            SystemGc().Clear( iHighlightArray[0].Rect() );
            DeactivateGc();
            }
        else // just move to the next position
            {
            MoveHighlight();
            }

        StartAnimation();
        return EKeyWasConsumed;
        }
    else if ( aKeyEvent.iScanCode == EStdKeyUpArrow && aType == EEventKey )
        {
        // At the top of list and pressing the up key
        if ( iCurrentArrayPosition == 0 )
            {
            // wrap to the bottom of the list and redraw
            TInt lastItem = iSettingItemArray.Count() - 1;
            iCurrentArrayPosition = lastItem;
            iPreviousArrayPosition = lastItem;
            iTopDisplayPosition = iSettingItemArray.Count() - iNumberOfIconsToDisplay;
            
            TInt pos = iCurrentArrayPosition - iTopDisplayPosition;
            iAnimData.iCurrentRect = iHighlightArray[pos].Rect();
            iAnimData.iPreviousRect = iAnimData.iCurrentRect;

            if ( iScrollFrame )
                {
                iScrollFrame->MoveThumbsBy( 0, iTopDisplayPosition );
                }

            DrawDeferred();
            iController.StartIdleTimer();
            return EKeyWasConsumed;
            }

        iAnimData.iMovingDown = EFalse;
        iPreviousArrayPosition = iCurrentArrayPosition;
        iCurrentArrayPosition--;

        // need to scroll the list
        if ( iPreviousArrayPosition > iNumberOfIconsToDisplay - 1 )
            {
            iAnimData.iScrolling = ETrue;
            iScrollFrame->MoveThumbsBy( 0, -1 );
            // draw the list before decrementing top display pos
            DrawListL();
            iTopDisplayPosition--;

            iAnimData.iOffScreenPos.SetXY( 0, iAnimData.iScrollingY );

            TInt curIndex = iCurrentArrayPosition - iTopDisplayPosition;
            TInt defaultIndex = curIndex - 1;
            iAnimData.iTitleFadeIn = iNormalTitles[defaultIndex].TextRect();

            iAnimData.iCurrentRect = iHighlightArray[curIndex].Rect();
            TRect rect = iNormalIcons[defaultIndex].Rect();
            iAnimData.iCurrentRect.iTl.iY = rect.iTl.iY;
            iAnimData.iCurrentRect.iBr.iY = iAnimData.iCurrentRect.iTl.iY + rect.Height();
            }
        else // just move to the next position
            {
            MoveHighlight();
            }

        StartAnimation();
        iController.StartIdleTimer();
        return EKeyWasConsumed;
        }
    else
        {
        return CCamContainerBase::OfferKeyEventL( aKeyEvent, aType );
        }
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::HandleScrollEventL
// Called when a scroll event is detected
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::HandleScrollEventL( CEikScrollBar* /*aScrollBar*/, 
                                                    TEikScrollEvent /*aEventType*/ )
    {
    }

// ----------------------------------------------------------------
// CCamBurstThumbnailGrid::SetupScrollbarL
// Sets up the scrollbar for the list
// ----------------------------------------------------------------
//
void CCamSceneSettingContainer::SetupScrollbarL()
    {
    if ( !AknLayoutUtils::LayoutMirrored() )
        {
        iScrollLayout.LayoutRect( Rect(), ROID(R_CAM_SCENE_SCROLLBAR_POSITION_ID));
        }
    else
        {
        iScrollLayout.LayoutRect( Rect(), ROID(R_CAM_SCENE_SCROLLBAR_POSITION_AH_ID));
        }

    // Create scrollbar frame
    iScrollFrame = new (ELeave) CEikScrollBarFrame( this, this, ETrue );

    // Set up the model accordingly
    ivModel.SetScrollSpan( iSettingItemArray.Count() );
    ivModel.SetFocusPosition( 0 );
    ivModel.SetWindowSize( iNumberOfIconsToDisplay );
            
    iScrollFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                           CEikScrollBarFrame::EOn );
    iScrollFrame->CreateDoubleSpanScrollBarsL( ETrue, EFalse );
    
    TRect rect = iScrollLayout.Rect();
    iScrollFrame->Tile( &ivModel, rect );
    iScrollFrame->MoveVertThumbTo( iTopDisplayPosition );
    }

// ----------------------------------------------------------------
// CCamSceneSettingContainer::CreateSceneArrayL
// Populates the scene array with scenes using a resource
// ----------------------------------------------------------------
//
void CCamSceneSettingContainer::CreateSceneArrayL( TInt aResourceId )
    {
    // Create a resource reader for the scene array resource id.
    TResourceReader reader;                                                                                     
    iEikonEnv->CreateResourceReaderLC( reader, aResourceId );    

    // Create all scene list items from the resource reader.
    TInt sceneCount = reader.ReadInt16();

    TInt i;
    for ( i = 0; i < sceneCount; ++i )
        {
        TCamSceneId sceneId = static_cast<TCamSceneId> ( reader.ReadInt16() );
        if ( iController.IsSceneSupported( sceneId ) )
            {
            TSize large = iHighlightIcons[0].Rect().Size();
            TSize small = iNormalIcons[0].Rect().Size();
            CCamSceneSettingItem* settingItem = CCamSceneSettingItem::NewL( 
                                                sceneId, large, small );
            CleanupStack::PushL( settingItem );
            iSettingItemArray.AppendL( settingItem );
            CleanupStack::Pop( settingItem );            
            }

        }

    CleanupStack::PopAndDestroy(); // reader

    // If total number of scenes is less than the current number 
    // of icons to display on screen then reset the current number of
    // icons to display on screen.
    if ( sceneCount < iNumberOfIconsToDisplay )
        {
        iNumberOfIconsToDisplay = sceneCount;
        }    
    }

// ----------------------------------------------------------------
// CCamSceneSettingContainer::SetCurrentPositionToScene
// Sets the current position of the scene array to the element
// associated with a particular scene id.
// ----------------------------------------------------------------
//
void CCamSceneSettingContainer::SetCurrentPositionToScene
( TCamSceneId aSceneId )
    {
    // Find scene in scene array.
    TInt count = iSettingItemArray.Count();
    TInt j;
    for ( j = 0; j < count; j++ )
        {
        if ( aSceneId == iSettingItemArray[j]->SceneId() )
            {  
            iCurrentArrayPosition = j;
            j = count; // stop loop
            }
        }

    // If scene array position is offscreen, move top display position.
    if ( iCurrentArrayPosition >= iNumberOfIconsToDisplay )
        {
        iTopDisplayPosition = ( iCurrentArrayPosition - 
                                iNumberOfIconsToDisplay ) + 1;
        }
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::StartAnimation
// Called to start the animation
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::StartAnimation()
    {
#ifndef _ANIMATION_OFF
    iAnimTimer->Start( KTimerPeriod, KTimerPeriod, TCallBack( TimerExpired, this ) );
#else
    DrawDeferred();
#endif
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::TimerExpired
// Called when the timer expires
// ---------------------------------------------------------------------------
//
TInt CCamSceneSettingContainer::TimerExpired( TAny* aAny )
    {
    CCamSceneSettingContainer* container = 
                    static_cast<CCamSceneSettingContainer*>( aAny );
    __ASSERT_DEBUG( container != NULL, CamPanic( ECamPanicNullPointer ) );
    container->Animate();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::Animate
// Called every time the timer expires to update the animation
// The animation works by changing a single line non-highlighted item
// into a double line highlighted item.
// When a highlighted item comes into focus, the title text remains the same
// size, but its position is moved up to indicate moving from a single to
// double line.
// The description text fades in underneath the title text - the title text
// moves up to make way for the description text.
// When a highlighted item looses the focus, the title text remains the same
// size, but its position moves down to indicate moving from a double line
// to a single line.
// The description text fades out and the title text moves down into the 
// position occupied by the description text.
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::Animate()
    {
    iAnimData.iStep++;

    // update the pen/bitmaps
    UpdateAnimationData();

    // font is the same for all titles
    const CAknLayoutFont* layoutFont = CAknLayoutFont::AsCAknLayoutFontOrNull( 
                                       iHighlightTitles[0].Font() );
    __ASSERT_DEBUG( layoutFont != NULL, CamPanic( ECamPanicNullPointer ) );
    TInt baseline = layoutFont->TextPaneTopToBaseline();

    const CCamSceneSettingItem* previousItem = iSettingItemArray[iPreviousArrayPosition];
    const CCamSceneSettingItem* currentItem = iSettingItemArray[iCurrentArrayPosition];

    TInt currentPosition = iCurrentArrayPosition - iTopDisplayPosition;
    TInt previousPosition = iPreviousArrayPosition - iTopDisplayPosition;

    CWindowGc& gc = SystemGc();
    ActivateGc();

    TBool animScrolled = ETrue;
    TBool animCompleted = ETrue;
    if ( iAnimData.iScrolling )
        {
        animScrolled = ScrollList();

        // update the highlight rects
        animCompleted = UpdateHighlight();

        DrawIcon( EFalse );

        if ( iAnimData.iMovingDown )
            {
            // Save the current position, so we can rub it out later
            iAnimData.iTitleFadeInOld = iAnimData.iTitleFadeIn;

            TRect rectLimit = iHighlightTitles[currentPosition].TextRect();
            iAnimData.iTitleFadeIn.Move( 0, -iAnimData.iDeltaY );
            if ( iAnimData.iTitleFadeIn.iTl.iY <= rectLimit.iTl.iY )
                {
                // finish
                iAnimData.iTitleFadeIn.iTl.iY = rectLimit.iTl.iY;
                iAnimData.iTitleFadeIn.iBr.iY = rectLimit.iBr.iY;
                }
            DrawText( *currentItem->Title(), layoutFont, baseline, EFalse );
            }

        // draw the description text
        iHighlightDescriptions[currentPosition].DrawText( gc, 
                *currentItem->Description(), ETrue, iAnimData.iPenFadeIn );

        // draw the highlighted border
        DrawHighlight();
        }
    else
        {
        // update the highlight rects
        animCompleted = UpdateHighlight();

        // draw the highlighted border
        DrawHighlight();

/**** PREVIOUS ITEM - FADE OUT ****/
        DrawIcon( ETrue );

        // fade description text out
        iHighlightDescriptions[previousPosition].DrawText( gc, 
               *previousItem->Description(), ETrue, iAnimData.iPenFadeOut );

        // Save the current position, so we can rub it out later
        iAnimData.iTitleFadeOutOld = iAnimData.iTitleFadeOut;

        // Draw the title text if moving down the list
        TRect rectLimit;
        if ( iAnimData.iMovingDown )
            {
            rectLimit = iDisplacedTitles[previousPosition].TextRect();
            }
        else // Draw the title text if moving up the list
            {
            rectLimit = iNormalTitles[previousPosition-1].TextRect();
            } 

        iAnimData.iTitleFadeOut.Move( 0, iAnimData.iDeltaY );
        if ( iAnimData.iTitleFadeOut.iTl.iY >= rectLimit.iTl.iY )
            {
            // finish
            iAnimData.iTitleFadeOut.iTl.iY = rectLimit.iTl.iY;
            iAnimData.iTitleFadeOut.iBr.iY = rectLimit.iBr.iY;
            }

        DrawText( *previousItem->Title(), layoutFont, baseline, ETrue );

/**** NEXT ITEM - FADE IN ****/
        DrawIcon( EFalse );

        if ( iAnimData.iMovingDown )
            {
            iHighlightDescriptions[currentPosition].DrawText( gc, 
                *currentItem->Description(), ETrue, iAnimData.iPenFadeIn );
            }
        else
            {
            // don't fade in straight away otherwise looks messy
            if ( iAnimData.iStep > 3 )
                {
                iHighlightDescriptions[currentPosition].DrawText( gc, 
                    *currentItem->Description(), ETrue, iAnimData.iPenFadeIn );
                }
            }

        // Save the current position, so we can rub it out later
        iAnimData.iTitleFadeInOld = iAnimData.iTitleFadeIn;

        rectLimit = iHighlightTitles[currentPosition].TextRect();
        iAnimData.iTitleFadeIn.Move( 0, -iAnimData.iDeltaY );
        if ( iAnimData.iTitleFadeIn.iTl.iY <= rectLimit.iTl.iY )
            {
            // finish
            iAnimData.iTitleFadeIn.iTl.iY = rectLimit.iTl.iY;
            iAnimData.iTitleFadeIn.iBr.iY = rectLimit.iBr.iY;
            }
        DrawText( *currentItem->Title(), layoutFont, baseline, EFalse );
        }

    DeactivateGc();

    if ( animScrolled && animCompleted )
        {
        iAnimTimer->Cancel();
        iAnimData.iScrolling = EFalse;
        iAnimData.iPenFadeOut = KRgbBlack;
        iAnimData.iPenFadeIn = KRgbWhite;
        iAnimData.iStep = 0;
        iAnimData.iIconLeftOffset = 0;
        DrawNow();
        }
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::UpdateHighlight
// Updates the highlight rect each time Animate() is called
// ---------------------------------------------------------------------------
//
TBool CCamSceneSettingContainer::UpdateHighlight()
    {
    TInt currentPosition = iCurrentArrayPosition - iTopDisplayPosition;
    TInt previousPosition = iPreviousArrayPosition - iTopDisplayPosition;

    // Save the current position, so we can rub it out later
    iAnimData.iCurrentRectOld = iAnimData.iCurrentRect;
    iAnimData.iPreviousRectOld = iAnimData.iPreviousRect;
    
    TBool completedCurrentTop = EFalse;
    TBool completedCurrentBottom = EFalse;
    TBool completedPreviousTop = EFalse;
    TBool completedPreviousBottom = EFalse;

    TInt currentRectTopLimit = iHighlightArray[currentPosition].Rect().iTl.iY;
    TInt currentRectBottomLimit = iHighlightArray[currentPosition].Rect().iBr.iY;
        
    iAnimData.iCurrentRect.iTl.iY -= iAnimData.iDeltaY;
    if ( iAnimData.iCurrentRect.iTl.iY <= currentRectTopLimit )
        {
        iAnimData.iCurrentRect.iTl.iY = currentRectTopLimit;
        completedCurrentTop = ETrue;
        }
    iAnimData.iCurrentRect.iBr.iY += iAnimData.iDeltaY;
    if ( iAnimData.iCurrentRect.iBr.iY >= currentRectBottomLimit )
        {
        iAnimData.iCurrentRect.iBr.iY = currentRectBottomLimit;
        completedCurrentBottom = ETrue;
        }

    // not scrolling so fade the previous highlight
    if ( !iAnimData.iScrolling )
        {
        TInt previousRectTopLimit;
        TInt previousRectBottomLimit;
        if ( iAnimData.iMovingDown )
            {    
            previousRectTopLimit = iDisplacedIcons[previousPosition].Rect().iTl.iY;
            previousRectBottomLimit = iDisplacedIcons[previousPosition].Rect().iBr.iY;
            }
        else // moving up
            {
            previousRectTopLimit = iNormalIcons[previousPosition-1].Rect().iTl.iY;
            previousRectBottomLimit = iNormalIcons[previousPosition-1].Rect().iBr.iY;
            }

        iAnimData.iPreviousRect.iTl.iY += iAnimData.iDeltaY;
        if ( iAnimData.iPreviousRect.iTl.iY >= previousRectTopLimit )
            {
            iAnimData.iPreviousRect.iTl.iY = previousRectTopLimit;
            completedPreviousTop = ETrue;
            }

        iAnimData.iPreviousRect.iBr.iY -= iAnimData.iDeltaY;
        if ( iAnimData.iPreviousRect.iBr.iY <= previousRectBottomLimit )
            {
            iAnimData.iPreviousRect.iBr.iY = previousRectBottomLimit;
            completedPreviousBottom = ETrue;
            }
        }
    else // scrolling - don't draw the previous rect
        {
        completedPreviousTop = ETrue;
        completedPreviousBottom = ETrue;
        }

    return ( completedCurrentTop && completedCurrentBottom && 
             completedPreviousTop && completedPreviousBottom );
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::UpdateAnimationData
// Updates the pen/icons each time Animate() is called
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::UpdateAnimationData()
    {
    const CCamSceneSettingItem* previousItem = iSettingItemArray[iPreviousArrayPosition];
    const CCamSceneSettingItem* currentItem = iSettingItemArray[iCurrentArrayPosition];

    if ( iAnimData.iStep == 1 )
        {
        iAnimData.iPenFadeOut = KRgbDarkGray;
        iAnimData.iPenFadeIn = KRgbGray4;
        iAnimData.iIconFadeOut = previousItem->Icon( ECamIconSizeSix );
        iAnimData.iIconFadeIn = currentItem->Icon( ECamIconSizeOne );
        } 
    else if ( iAnimData.iStep == 2 )
        {
        iAnimData.iPenFadeOut = KRgbGray;
        iAnimData.iPenFadeIn = KRgbGray3;
        iAnimData.iIconFadeOut = previousItem->Icon( ECamIconSizeFive );
        iAnimData.iIconFadeIn = currentItem->Icon( ECamIconSizeTwo );
        }
    else if ( iAnimData.iStep == 3 )
        {
        iAnimData.iPenFadeOut = KRgbGray1;
        iAnimData.iPenFadeIn = KRgbGray2;
        iAnimData.iIconFadeOut = previousItem->Icon( ECamIconSizeFour );
        iAnimData.iIconFadeIn = currentItem->Icon( ECamIconSizeThree );
        }
    else if ( iAnimData.iStep == 4 )
        {
        iAnimData.iPenFadeOut = KRgbGray2;
        iAnimData.iPenFadeIn = KRgbGray1;
        iAnimData.iIconFadeOut = previousItem->Icon( ECamIconSizeThree );
        iAnimData.iIconFadeIn = currentItem->Icon( ECamIconSizeFour );
        }
    else if ( iAnimData.iStep == 5 )
        {
        iAnimData.iPenFadeOut = KRgbGray3;
        iAnimData.iPenFadeIn = KRgbGray;
        iAnimData.iIconFadeOut = previousItem->Icon( ECamIconSizeTwo );
        iAnimData.iIconFadeIn = currentItem->Icon( ECamIconSizeFive );
        }
    else if ( iAnimData.iStep == 6 )
        {
        iAnimData.iPenFadeOut = KRgbGray4;
        iAnimData.iPenFadeIn = KRgbDarkGray;

        iAnimData.iIconFadeOut = previousItem->Icon( ECamIconSizeOne );
        iAnimData.iIconFadeIn = currentItem->Icon( ECamIconSizeSix );
        }
    else if ( iAnimData.iStep == 7 )
        {
        iAnimData.iPenFadeOut = KRgbWhite;
        iAnimData.iPenFadeIn = KRgbBlack;
        iAnimData.iIconFadeOut = previousItem->Icon( ECamIconSizeSmall );
        iAnimData.iIconFadeIn = currentItem->Icon( ECamIconSizeLarge );
        }
    else
        {
        
        }
    iAnimData.iIconLeftOffset += iAnimData.iIconLeftInc;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::DrawText
// Draws the title text each time Animate() is called
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::DrawText( const TDesC& aText, 
                                          const CAknLayoutFont* aFont, 
                                          TInt aBaseline,
                                          TBool aFadeOut ) const
    {
    CWindowGc& gc = SystemGc();

    TRect oldRect;
    TRect rect;
    if ( aFadeOut )
        {
        rect = iAnimData.iTitleFadeOut;
        oldRect = iAnimData.iTitleFadeOutOld;
        }
    else
        {
        rect = iAnimData.iTitleFadeIn;
        oldRect = iAnimData.iTitleFadeInOld;
        }
    gc.UseFont( aFont );
    TLogicalRgb backgroundColour( TLogicalRgb::ESystemBackgroundColor );
    gc.SetPenColor( backgroundColour );
    // draw test aligned left or right depending on layout
    if( !AknLayoutUtils::LayoutMirrored() )
        {
        gc.DrawText( aText, oldRect, aBaseline, CGraphicsContext::ELeft );
        gc.SetPenColor( KRgbBlack );
        gc.DrawText( aText, rect, aBaseline, CGraphicsContext::ELeft );
        }
    else
        {
        gc.DrawText( aText, oldRect, aBaseline, CGraphicsContext::ERight );
        gc.SetPenColor( KRgbBlack );
        gc.DrawText( aText, rect, aBaseline, CGraphicsContext::ERight );
        }

    gc.DiscardFont();
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::DrawIcon
// Draws the icon each time Animate() is called
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::DrawIcon( TBool aFadeOut )
    {
    TRect oldRect;
    TRect bmpRect;
    const CFbsBitmap* bitmap = NULL;

    TRect largeIcon = iHighlightIcons[0].Rect();
    TRect smallIcon = iNormalIcons[0].Rect();

    if ( aFadeOut )
        {
        bitmap = iAnimData.iIconFadeOut;
        TSize bmpSize = bitmap->SizeInPixels();

        TInt iconYPos = ( iAnimData.iPreviousRect.Height() - bmpSize.iHeight ) / 2;

        TPoint iconPos( ( largeIcon.iTl.iX + (TInt)iAnimData.iIconLeftOffset ), 
                      ( iAnimData.iPreviousRect.iTl.iY + iconYPos ) );

        iAnimData.iIconRectFadeOutOld = iAnimData.iIconRectFadeOut;
        iAnimData.iIconRectFadeOut.SetRect( iconPos, bmpSize );

        oldRect = iAnimData.iIconRectFadeOutOld;
        bmpRect = iAnimData.iIconRectFadeOut;
        }
    else
        {
        bitmap = iAnimData.iIconFadeIn;
        TSize bmpSize = bitmap->SizeInPixels();
 
        TInt iconYPos = ( iAnimData.iCurrentRect.Height() - bmpSize.iHeight ) / 2;
        
        TPoint iconPos( ( smallIcon.iTl.iX - (TInt)iAnimData.iIconLeftOffset ), 
                        ( iAnimData.iCurrentRect.iTl.iY + iconYPos ) );

        iAnimData.iIconRectFadeInOld = iAnimData.iIconRectFadeIn;
        iAnimData.iIconRectFadeIn.SetRect( iconPos, bmpSize );

        oldRect = iAnimData.iIconRectFadeInOld;
        bmpRect = iAnimData.iIconRectFadeIn;
        }

    CWindowGc& gc = SystemGc();
    gc.Clear( oldRect );
    gc.BitBlt( bmpRect.iTl, bitmap );
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::DrawHighlight
// Draws the highlight rect each time Animate() is called
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::DrawHighlight() const
    {
    if( Flags() & CListItemDrawer::ESingleClickDisabledHighlight ) 
        {
        return;
        }

    CWindowGc& gc = SystemGc();

    TSize corner( KCornerSize,KCornerSize );
    // Clear the old highlight
    gc.SetPenSize( TSize( KBorderSize, KBorderSize ) );
    TLogicalRgb backgroundColour( TLogicalRgb::ESystemBackgroundColor );
    gc.SetPenColor( backgroundColour );
    gc.DrawRoundRect( iAnimData.iCurrentRectOld, corner );
    if ( !iAnimData.iScrolling )
        {
        gc.DrawRoundRect( iAnimData.iPreviousRectOld, corner );
        }
    // Draw the new highlight
    gc.SetPenSize( TSize( KBorderSize, KBorderSize ) );
    gc.SetPenColor( iAnimData.iPenFadeIn );
    gc.DrawRoundRect( iAnimData.iCurrentRect, corner );
    if ( !iAnimData.iScrolling )
        {
        gc.SetPenColor( iAnimData.iPenFadeOut );
        gc.DrawRoundRect( iAnimData.iPreviousRect, corner );
        }
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::DrawListL
// Draws the list to an off-screen bitmap
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::DrawListL()
    {
    // create an off-screen device and context
    CFbsBitGc* bitmapGc = NULL;
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL( iAnimData.iOffScreenBitmap );
    CleanupStack::PushL( bitmapDevice );
    User::LeaveIfError( bitmapDevice->CreateContext( bitmapGc ) );
    CleanupStack::PushL( bitmapGc );

    bitmapGc->SetPenStyle( CGraphicsContext::ENullPen );
    bitmapGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
    bitmapGc->DrawRect( TRect( iAnimData.iOffScreenBitmap->SizeInPixels() ) );

    TInt i;
    // only draw top 4 items in the list
    for ( i = 0; i < iNumberOfIconsToDisplay-1; i++ )
        {
        TInt position = iTopDisplayPosition + i;
        CCamSceneSettingItem* item = iSettingItemArray[position];

        // Draw the standard icon
        iDisplacedIcons[i].DrawImage( *bitmapGc, 
                const_cast<CFbsBitmap*>( item->Icon( ECamIconSizeSmall ) ), NULL );
        // Draw the header text
        iDisplacedTitles[i].DrawText( *bitmapGc, *item->Title() );
        }

    CleanupStack::PopAndDestroy( bitmapGc );
    CleanupStack::PopAndDestroy( bitmapDevice );
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::ScrollList
// Scrolls the list
// ---------------------------------------------------------------------------
//
TBool CCamSceneSettingContainer::ScrollList()
    {
    SystemGc().BitBlt( iAnimData.iOffScreenPos, iAnimData.iOffScreenBitmap );
    if ( iAnimData.iMovingDown )
        {
        iAnimData.iOffScreenPos.iY -= iAnimData.iScrollingY;
        if ( iAnimData.iOffScreenPos.iY <= 0 )
            {
            return ETrue;
            }
        }
    else
        {
        iAnimData.iOffScreenPos.iY += iAnimData.iScrollingY;
        if ( iAnimData.iOffScreenPos.iY >= iDisplacedIcons[1].Rect().iTl.iY )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCamSceneSettingContainer::MoveHighlight
// Moves the highlight to its new position, ready to be animated
// ---------------------------------------------------------------------------
//
void CCamSceneSettingContainer::MoveHighlight()
    {
    iAnimData.iPreviousRect = iAnimData.iCurrentRect;

    TInt curIndex = iCurrentArrayPosition - iTopDisplayPosition;
    TInt prevIndex = iPreviousArrayPosition - iTopDisplayPosition;

    TRect iconRect;
    TRect titleRect;

    if ( iAnimData.iMovingDown )
        {
        TInt defaultIndex = curIndex - 1;
        iconRect = iNormalIcons[defaultIndex].Rect();
        titleRect = iNormalTitles[defaultIndex].TextRect();
        }
    else
        {        
        iconRect = iDisplacedIcons[curIndex].Rect();
        titleRect = iDisplacedTitles[curIndex].TextRect();
        }

    iAnimData.iCurrentRect = iHighlightArray[curIndex].Rect();
    iAnimData.iCurrentRect.iTl.iY = iconRect.iTl.iY;
    iAnimData.iCurrentRect.iBr.iY = iAnimData.iCurrentRect.iTl.iY + iconRect.Height();

    iAnimData.iTitleFadeOut = iHighlightTitles[prevIndex].TextRect();
    iAnimData.iTitleFadeIn = titleRect;

    iAnimData.iIconRectFadeOut = iHighlightIcons[prevIndex].Rect();
    iAnimData.iIconRectFadeIn = iconRect;
    }

// End of File  
