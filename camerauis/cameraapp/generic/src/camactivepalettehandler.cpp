/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/



// INCLUDE FILES
#include <activepalette2factory.h>
#include <activepalette2genericpluginint.h>
#include <activepalette2itemvisible.h>
#include <activepalette2eventdata.h>
#include <activepalette2navigationkeys.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <barsc.h>
#include <barsread.h>
#include <bautils.h>
#include <AknIconUtils.h>
#include <aknnotewrappers.h>
#include <AknUtils.h>
#include <e32math.h>
#include <gulicon.h>
#include <cameraapp.mbg>
#include <utf.h>
#include <textresolver.h>
#include <apgcli.h>
#include <apaid.h>

#include "CamUtility.h"
#include "CamAppController.h"
#include "camactivepalettehandler.h"
#include "CamAppUid.h"
#include "CamCaptureSetupViewBase.h"
#include "CamAppUiBase.h"
#include "CamPanic.h"
#include "CamAppUi.h"
#include "CamUtility.h"
#include "CamTimeLapseUtility.h"
#include "CamPSI.h"
#include "CameraUiConfigManager.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "camactivepalettehandlerTraces.h"
#endif


typedef TAP2GenericPluginIcon   TCamAPGenericPluginIcon;
typedef TAP2GenericPluginParams TCamAPGenericPluginParams;

// ============================ CONSTANTS ======================================

// ActivePalette2 settings
const TActivePalette2DrawMode KActivePaletteDrawMode = EAP2DrawModeSemiTransparentTooltipModeFading;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::CCamActivePaletteHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamActivePaletteHandler::CCamActivePaletteHandler( CCamAppController& aController )
:   iController( aController )
    {
    }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::ConstructL( TBool aCreatePalette )
  {
  if ( !iController.IsTouchScreenSupported() )
      {
      iEnv = CEikonEnv::Static();
      
      if( aCreatePalette )
        {
        iActivePalette = 
          ActivePalette2Factory::CreateActivePaletteUIL( KActivePaletteDrawMode );
        iActivePalette->SetObserver( this );
        }
           
      // Get the icon file name
      delete iIconFileName;
      iIconFileName = NULL;
      iIconFileName = HBufC::NewL( KMaxFileName );
      TPtr  ptr = iIconFileName->Des();
      CamUtility::ResourceFileName( ptr );


      // get res id for EV comp from psi
      CamUtility::GetPsiInt( ECamPsiAPEVCompLegend, iEvCompRes );
      if( iEvCompRes == KErrNotSupported )
        {
        iEvCompRes = R_CAM_CAPTURE_SETUP_MENU_PHOTO_EXPOSURE_ARRAY;
        }
      }
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamActivePaletteHandler* CCamActivePaletteHandler::NewL( CCamAppController& aController, TBool aCreatePalette )
  {
  CCamActivePaletteHandler* self = new( ELeave ) CCamActivePaletteHandler( aController );
  
  CleanupStack::PushL( self );
  self->ConstructL( aCreatePalette );
  CleanupStack::Pop();

  return self;
  }

    
// Destructor
CCamActivePaletteHandler::~CCamActivePaletteHandler()
  {
  PRINT( _L("Camera => ~CCamActivePaletteHandler") );
  delete iIconFileName;
  iIconFileName = NULL;

  iItemArray.Close();

  iCurrentValSettingsItem.Close();

  iPreCaptureItems.Close();
  delete iToolTipText;

  delete iOneClickUploadTooltip;

  DestroyActivePalette();
  PRINT( _L("Camera <= ~CCamActivePaletteHandler") );
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ActivePalette()
// Return handle to Active Palette
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MActivePalette2UI* CCamActivePaletteHandler::ActivePalette() const
  {
  return iActivePalette;
  }    


/*
 *  To check if a specific feature is supported, to append or show in AP
 */
TBool CCamActivePaletteHandler:: IsAPItemFeatureSupportedL( const TInt aItemId ) const
  {
  TBool featureSupported = ETrue; // by default we feature supported
  CCameraUiConfigManager* configMgr = iController.UiConfigManagerPtr();
  switch( aItemId )
      {
      case ECamSettingItemFaceTracking:
          {
          featureSupported = configMgr->IsFaceTrackingSupported();
          break;
          }
      case ECamSettingItemGotoPhotos:
          {
          featureSupported = configMgr->IsPhotosSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoFlash:
          {
          featureSupported = configMgr->IsFlashModeSupported();
          break;
          }   
      case ECamSettingItemDynamicVideoFlash:
          {
          featureSupported = configMgr->IsVideoLightSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoColourFilter:
      case ECamSettingItemDynamicVideoColourFilter:
          {
          featureSupported = configMgr->IsColorToneFeatureSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoWhiteBalance:
      case ECamSettingItemDynamicVideoWhiteBalance:    
          {
          featureSupported = configMgr->IsWhiteBalanceFeatureSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoLightSensitivity:
          {    
          featureSupported = configMgr->IsISOSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoExposure:
          {
          featureSupported = configMgr->IsEVSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoImageSharpness:
          {
          featureSupported = configMgr->IsSharpnessFeatureSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoContrast:
      case ECamSettingItemDynamicVideoContrast:
          {
          featureSupported = configMgr->IsContrastSupported();
          break;
          }
      case ECamSettingItemDynamicPhotoBrightness:
      case ECamSettingItemDynamicVideoBrightness:
          {
          featureSupported = configMgr->IsBrightnessSupported();
          break; 
          }
      default:
          {
          break;      
          }    
      }
  return featureSupported;
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::InstallAPItemsL
// Install ap items from resource
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::InstallAPItemsL( TInt aResId )
  {  
  PRINT( _L( "Camera => CCamActivePaletteHandler::InstallAPItemsL" ) );  

  if ( !iController.IsTouchScreenSupported() )
      {
      // Clear existing items first	  
      iItemArray.Reset();

      if ( iController.UiConfigManagerPtr() &&
           iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
          {
          iCurrentValSettingsItem.Reset();
          }

      
      // Array for new toolbar items
      RArray<TCamActivePaletteItem> newItems;
      CleanupClosePushL( newItems );  

      // Static toolbar content - load items from resource
      PRINT( _L( "Camera <> CCamActivePaletteHandler::InstallAPItemsL - Static toolbar items from resource" ) );        
      ReadToolbarItemsFromResourceL( aResId, newItems );                  
      iDynamicToolbar = EFalse;

      TInt nItems = newItems.Count();
      
      for( TInt i=0; i<nItems; i++ )
        {
        TCamActivePaletteItem item = newItems[i];
        TBool append ( ETrue );
        if ( ECamActivePaletteItemTypePostCapture != item.iType )
            {
            append = IsAPItemFeatureSupportedL( item.iItemId ); 
            }
        else
            {
            // if postcapture items, then check if add to album and properties ae supported
            if ( ECamCmdAddToAlbum == item.iCmdId ||
                 ECamCmdProperties == item.iCmdId ||
                 ECamCmdPhotos == item.iCmdId )
                 {
                 append = iController.UiConfigManagerPtr()->IsPhotosSupported();
                 }
            else
                {
                }
            }

        if ( append )
            {
            iItemArray.AppendL( item ); 
            }
        }

      TInt items = iItemArray.Count();
        
      for( TInt i=0; i<items; i++ )
        {
        TCamAPSettingsItem settingItem;
        settingItem.iSettingItem = iItemArray[i].iItemId;
        
        TUid uid( TUid::Uid( KActivePalette2GenericPluginUID ) );
          
        switch( iItemArray[i].iType )
          {
          case ECamActivePaletteItemTypeCommand:
            {
            PRINT( _L("Camera <> CCamActivePaletteHandler: case ECamActivePaletteItemTypeCommand") );
            // Command id is used to identify the icon.
            // Only one icon is used for command type items.
            settingItem.iSettingVal = iItemArray[i].iCmdId;
            iIconId = GetBitmapIdFromValueL( settingItem );
            break;
            }
          case ECamActivePaletteItemTypePreCapture:
            {
            PRINT( _L("Camera <> CCamActivePaletteHandler: case ECamActivePaletteItemTypePreCapture") );
            // Get initial value & associated icon
            settingItem.iSettingVal = iController.IntegerSettingValue( iItemArray[i].iItemId );
            iIconId = GetBitmapIdFromValueL( settingItem );
            break;
            }
          case ECamActivePaletteItemTypePreCaptureNonSetting:
            {                
            PRINT( _L("Camera <> CCamActivePaletteHandler: case ECamActivePaletteItemTypePreCaptureNonSetting") );
            settingItem.iSettingVal = DetermineNonSettingState( iItemArray[i] );
            iIconId = GetBitmapIdFromValueL( settingItem );
            if ( iIconId != KErrNotFound )
              {
              SetNonSettingItemTooltipL( iItemArray[i] );
              }
            break;
            }
          case ECamActivePaletteItemTypePostCapture:
            {
            PRINT( _L("Camera <> CCamActivePaletteHandler: case ECamActivePaletteItemTypePostCapture") );
            // Setting up the AP post-capture is simpler as the icons
            // don't need to change depending on any settings.
            LoadPostCaptureResourcesL( PostCaptureItemId( iItemArray[i].iCmdId ) );
            break;
            }
          default:
            {
            PRINT( _L("Camera <> CCamActivePaletteHandler: default case => PANIC!!!") );
             __ASSERT_DEBUG( EFalse, CamPanic( ECamPanicInvalidActivePaletteItemType ) );
            break;
            }
          } // switch
           
        PRINT( _L("Camera <> CCamActivePaletteHandler::InstallAPItemsL About to install item"));    

        // Override tooltip for one-click upload
        if ( iOneClickUploadTooltip &&
             iItemArray[i].iCmdId == ECamCmdOneClickUpload )
            {
            delete iToolTipText;
            iToolTipText = NULL;
            iToolTipText = iOneClickUploadTooltip->AllocL();
            }
			
        // cache the current values of the AP items so that we don't need to check later
        // specifically for the items that there values are persisted
        if ( iController.UiConfigManagerPtr() &&
             iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
            {
            TCamAPSettingsItem tempSettingItem;
            tempSettingItem.iSettingItem   = iItemArray[i].iItemId;
            // set initial value to some invalid number using -1
            tempSettingItem.iSettingVal    = -1;
            iCurrentValSettingsItem.AppendL( tempSettingItem );
            }
            
        InstallOneAPItemL( iItemArray[i], uid, *iToolTipText, *iIconFileName, iIconId, iIconId );

        PRINT( _L("Camera <> CCamActivePaletteHandler::InstallAPItemsL Item installed"));    
        } // for
       
    if ( iController.UiConfigManagerPtr()->IsXenonFlashSupported() )
        {
        CheckFlashItemVisibilityL();
        }
                          
      CleanupStack::PopAndDestroy(); // newItems
      }
  PRINT( _L( "Camera <= CCamActivePaletteHandler::InstallAPItemsL" ) );
  }




// -----------------------------------------------------------------------------
// InstallOneAPItemL
//
// Helper method to install one AP item.
// -----------------------------------------------------------------------------
//
void 
CCamActivePaletteHandler
::InstallOneAPItemL( const TCamActivePaletteItem& aItem,
                     const TUid&                  aPluginUid,
                     const TDesC&                 aToolTipText,
                     const TDesC&                 aIconFileName,
                           TInt                   aIconId,
                           TInt                   aMaskId )
  {
  // only add items that actually exist
  if( iIconId != KErrNotFound )
      {
      TCamAPGenericPluginParams params( 
              TCamAPGenericPluginParams::EGenericCustomResFile,
              aIconFileName,
              aIconId,
              aMaskId,
              aToolTipText,
              aItem.iCmdId,   // message ID
              0 );            // Custom param            
      TPckg<TCamAPGenericPluginParams> paramsPckg( params );

      TActivePalette2ItemVisible itemVisible( aItem.iCmdId,aItem.iIsVisible );
      iActivePalette->InstallItemL( itemVisible,
              aPluginUid, 
              paramsPckg );
      }
  }


// -----------------------------------------------------------------------------
// CheckFlashItemVisibilityL
//
// Hide flash AP item if needed.
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::CheckFlashItemVisibilityL()
  {
  if ( !iController.IsTouchScreenSupported() && 
       iController.UiConfigManagerPtr()->IsXenonFlashSupported() )
      {
      // Make flash item invisible, in case updating AP for burst mode
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if( appUi &&  ECamImageCaptureBurst == appUi->CurrentBurstMode() || 
         
          // ECamCaptureModeTimeLapse == appUi->CurrentBurstMode() || 
          ECamSceneSports == iController.IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) )
          {
          // Flash item is disabled in burst mode and for sport scene   
          iActivePalette->SetItemVisibility( ECamCmdCaptureSetupFlashStill, EFalse );
          }
       else
          {
          iActivePalette->SetItemVisibility( ECamCmdCaptureSetupFlashStill, ETrue );    
          }
      }
  }

 
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ReturnedFromPretendExitL
// Resets current item position and updates the active palette.
// -----------------------------------------------------------------------------
//   
 void CCamActivePaletteHandler::ReturnedFromPretendExitL()
    {
    PRINT( _L("Camera => CCamActivePaletteHandler::ReturnedFromPretendExitL"));
    if ( !iController.IsTouchScreenSupported() )
        {
        if ( iActivePalette ) 
            {       
             ResetToDefaultItem();
            }
            
        UpdateActivePaletteL();
        }
    PRINT( _L("Camera <= CCamActivePaletteHandler::ReturnedFromPretendExitL"));    
    }



// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ResetToFirstItem
// Resets highlight to the first AP item
// -----------------------------------------------------------------------------
//   
void CCamActivePaletteHandler::ResetToFirstItem()
    {
    PRINT( _L("Camera => CCamActivePaletteHandler::ResetToFirstItem"));
    if ( !iController.IsTouchScreenSupported() )
        {
        if( iActivePalette && iItemArray.Count() > 0 )
            {
            iActivePalette->SetCurrentItem( iItemArray[0].iCmdId );            
            } 
        }
    PRINT( _L("Camera <= CCamActivePaletteHandler::ResetToFirstItem"));                          
    }     



// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ResetToDefaultItem
// Resets highlight to the default AP item
// -----------------------------------------------------------------------------
//   
void CCamActivePaletteHandler::ResetToDefaultItem()
    {
    PRINT( _L("Camera => CCamActivePaletteHandler::ResetToDefaultItem"));
    if ( !iController.IsTouchScreenSupported() )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        if( !appUi->IsEmbedded() 
/*#ifndef __WINS__
            || appUi->IsInCallSend()           
#endif*/
          )
            {          
            if( iActivePalette && iItemArray.Count() > 1 )
                {
                // In static toolbar, always reset to second item 
                // First item selected to adjust the visible items
                iActivePalette->SetCurrentItem( iItemArray[0].iCmdId );            
                iActivePalette->SetCurrentItem( iItemArray[1].iCmdId );            
                } 
            }
        else
            {
            ResetToFirstItem();
            }
        }
    PRINT( _L("Camera <= CCamActivePaletteHandler::ResetToDefaultItem"));  
    }     
  

 
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::UpdateActivePaletteL
// Force Active palette update
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
 void CCamActivePaletteHandler::UpdateActivePaletteL()
  {
  PRINT( _L("Camera => CCamActivePaletteHandler::UpdateActivePaletteL"))
  if ( !iController.IsTouchScreenSupported() )
      {
      TBool orientation = iController.UiConfigManagerPtr() && 
                          iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported();
      if( iUpdatesDisabled )
        {
        PRINT( _L("Camera <= CCamActivePaletteHandler::UpdateActivePaletteL (updates disabled)"))    
        return;
        }
      
      if( !iActivePalette )
        {
        PRINT( _L("Camera <= CCamActivePaletteHandler::UpdateActivePaletteL no palette"))
        return;
        }

      HBufC8* package = NULL;
      TInt messageId = -1;      
      TInt count = iItemArray.Count();
      for( TInt i = 0; i < count; i++ )
        {
        TCamAPSettingsItem settingItem;
        
        TCamActivePaletteItem& item = iItemArray[i];
           
        settingItem.iSettingItem = item.iItemId;

        switch (item.iType)
          {
          case ECamActivePaletteItemTypeCommand:
            {
            // Tooltip or icon will now change for command type item
            break;
            }
          case ECamActivePaletteItemTypePreCapture:
          case ECamActivePaletteItemTypePostCapture:
            {
            if ( item.iItemId >= 0 )
              {
              // attempt to get actual setting value
              // get handle to controller
              settingItem.iSettingVal = iController.IntegerSettingValue( item.iItemId );

              if( !orientation || 
                ( orientation && 
                  settingItem.iSettingVal != iCurrentValSettingsItem[i].iSettingVal ) )
                  {
                  TPckgBuf<TCamAPSettingsItem> buf( settingItem );
                  TInt iconId = GetBitmapIdFromValueL( settingItem );

                  if ( iconId != KErrNotFound )
                    {
                    TCamAPGenericPluginIcon icon( iconId, iconId );
                    TPckgC<TCamAPGenericPluginIcon> iconPckg( icon );
                    package = iconPckg.AllocLC();
                    messageId = EAPGenericPluginChangeIcon;
                    }
                  }
              }
            break;
            }
          case ECamActivePaletteItemTypePreCaptureNonSetting:
            {
            settingItem.iSettingVal = DetermineNonSettingState( item );

            if( !orientation || 
              ( orientation &&
                settingItem.iSettingVal !=  iCurrentValSettingsItem[i].iSettingVal ) )
              {
              iIconId = GetBitmapIdFromValueL( settingItem );
              PRINT1( _L("Camera <> CCamActivePaletteHandler::UpdateActivePaletteL icon id %d"), iIconId);
              if( iIconId != KErrNotFound )
                {
                SetNonSettingItemTooltipL( item );
                TCamAPGenericPluginParams params (
                        TCamAPGenericPluginParams::EGenericCustomResFile,
                        *iIconFileName,      // bitmap filename
                        iIconId,           // icon
                        iIconId,           // mask
                        *iToolTipText, // text
                        item.iCmdId,       // message ID
                        0 );               // Custom param
                TPckg<TCamAPGenericPluginParams> paramsPckg( params );
                package = paramsPckg.AllocLC();
                messageId = EAPGenericPluginChangeParams;
                }
              }
            break;
            }
          default:
            {
            PRINT( _L("Camera <> CCamActivePaletteHandler::UpdateActivePaletteL: Unknown item, PANIC!!!") );
            __ASSERT_DEBUG( EFalse, CamPanic( ECamPanicInvalidActivePaletteItemType ) );
            }                                            
          } // switch
        if ( package )
          {
          // if the value of the AP item has changed update the local settings value cache
          if ( orientation ) 
            {
            iCurrentValSettingsItem[i].iSettingVal = settingItem.iSettingVal;
            }

          // Change item by sending message to the generic plugin
          iActivePalette->SendMessage( item.iCmdId, messageId, *package) ;
          CleanupStack::PopAndDestroy( package );
          package = NULL;
          }
        } // for

    if ( iController.UiConfigManagerPtr()->IsXenonFlashSupported() )
        {
        CheckFlashItemVisibilityL();
        }
      
      }
   
  PRINT( _L("Camera <= CCamActivePaletteHandler::UpdateActivePaletteL"))
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::SetView
// Set current view
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CCamActivePaletteHandler::SetView( CCamViewBase* aView )
  {
  iView = aView;
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::SetVisibility
// Set Active palette visiblity
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void CCamActivePaletteHandler::SetVisibility( TBool aVisibility )
  {
  PRINT2(_L("Camera => CCamActivePaletteHandler::SetVisibility from %d to %d"), iVisible, aVisibility)
  
  if ( !iController.IsTouchScreenSupported() )
      {
      // Only animate if we're moving to a different state
      if ( iVisible != aVisibility && iActivePalette )
        {
        iVisible = aVisibility;
        if ( ECamCapturing == iController.CurrentOperation() || 
           (!iStarted && 
            !iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() ) || 
            !aVisibility )
            {
            iActivePalette->SetPaletteVisibility(iVisible, EFalse);
            iStarted = ETrue;
            PRINT(_L("AP NO animation"));
            }
        else
            {
            iActivePalette->SetPaletteVisibility(iVisible,ETrue,100);
            PRINT(_L("AP animation"));
            }
                   
        PERF_MESSAGE_L2( EPerfMessageActivePaletteAnimationStarting );
        PRINT(_L("AP animation started"));
        }
        
      if ( iController.IsTouchScreenSupported() )
          {
          if ( iActivePalette )
              {
              iActivePalette->SetPaletteVisibility( EFalse, EFalse );
              }
          }
      }
  
  PRINT(_L("Camera <= CCamActivePaletteHandler::SetVisibility"))
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::IsVisible
// Return Active palette visiblity
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool CCamActivePaletteHandler::IsVisible() const
  {
  return iVisible;
  }
    
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::SetVisibility
// Whether AP has requested item
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TBool CCamActivePaletteHandler::AccessedViaAP() const
  {
  return iItemAccessedViaAP;
  }
        
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::APOperationComplete
// Inform AP handler AP launched item has completed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::APOperationComplete()
  {
  iItemAccessedViaAP = EFalse;
  }
     
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::HandleAPKeyEventL
// Pass keys events to Active Palette
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
void 
CCamActivePaletteHandler::HandleAPKeyEventL( const TKeyEvent& /* aKeyEvent */, 
                                                   TEventCode /* aType */ )
  {
  }
        
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::CreateActivePaletteL
// Create the Active Palette
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//         
                                    
void 
CCamActivePaletteHandler::CreateActivePaletteL(const CCoeControl &aContainerWindow,
                                                     MObjectProvider *aMopParent)
  {
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMACTIVEPALETTEHANDLER_CREATEACTIVEPALETTEL, "e_CCamActivePaletteHandler_CreateActivePaletteL 1" );
  PRINT(_L("Camera => CCamActivePaletteHandler::CreateActivePaletteL"))
  PERF_EVENT_START_L2( EPerfEventActivePaletteConstruction );    
   
  if ( !iController.IsTouchScreenSupported() )
      {
      if(!iActivePalette)
        {
        // Variate this according to usage of Direct Screen viewfinder
        if ( iController.IsDirectScreenVFSupported() )
            {
            iActivePalette = ActivePalette2Factory::CreateActivePaletteUIL(EAP2DrawModeCheckerboard);
            }
        else
            {
            iActivePalette = ActivePalette2Factory::CreateActivePaletteUIL(KActivePaletteDrawMode);
            }
        iActivePalette->SetObserver(this);
        }
      else
        {
        ClearActivePalette();
        }        
      iActivePalette->CoeControl()->SetContainerWindowL( aContainerWindow ); 
      iActivePalette->CoeControl()->SetMopParent( aMopParent );
      iActivePalette->LocateTo(APScreenLocation( TPoint(280, 30) ));
      iActivePalette->SetPaletteVisibility(EFalse,EFalse);
      iVisible=EFalse;
      }
   
  PERF_EVENT_END_L2( EPerfEventActivePaletteConstruction );                            
  PRINT(_L("Camera <= CCamActivePaletteHandler::CreateActivePaletteL"))
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMACTIVEPALETTEHANDLER_CREATEACTIVEPALETTEL, "e_CCamActivePaletteHandler_CreateActivePaletteL 0" );
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::DestroyActivePalette
// Deletes the Active Palette
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::DestroyActivePalette()
  {
  if ( !iController.IsTouchScreenSupported() )
      {
      delete iActivePalette;
      iActivePalette = NULL;
      }
  }

//
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::NotifyItemFocused
// Handle a change of item focus in the active palette
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::NotifyItemFocused(TInt /*aPrevItem*/, TInt /*aFocusedItem*/)
    {
    // We don't care about changes in item focus.
    }


// 
//
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::NotifyItemSelected
// Pass keys events to Active Palette
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CCamActivePaletteHandler::NotifyItemSelected(const TActivePalette2EventData & aEvent)
  {
  PRINT1( _L("Camera => CCamActivePaletteHandler::NotifyItemSelected item (%d)"), aEvent.CurItem() );
  
  if ( !iController.IsTouchScreenSupported() )
      {
      if( iView && iVisible )
        {       
        TRAPD(ignore, iView->HandleCommandL( aEvent.CurItem()) );
        
        // Handling a command implies a container switch, so we need to switch the activepalette back
        // to the system Gc.
        iActivePalette->SetGc();
          
        // Error -14 (KErrInUse) must be handled here or it's ignored and there's no error note   
        if( ignore == KErrInUse )
          {
          PRINT( _L( "Camera <> CCamActivePaletteHandler::NotifyItemSelected KErrInUse" ) );
          
          // Get the application caption (name)
          RApaLsSession ls;
          TApaAppInfo appInfo;
          TRAPD(err1, ls.Connect() );
          if( err1 != KErrNone)
              {
              ls.Close();
              return;
              }
          TRAPD(err2, ls.GetAppInfo(appInfo, TUid::Uid(KCameraappUID)) );
          if( err2 != KErrNone)
              {
              ls.Close();
              return;
              }
          TApaAppCaption  appCaption = appInfo.iCaption;
          ls.Close();
          
          // Get the correct error description
          CTextResolver* iTextResolver = NULL;
          
          TRAPD(err3, iTextResolver = CTextResolver::NewL());
          if( err3 != KErrNone)
              {
              return;
              }
          TPtrC buf;
          buf.Set( iTextResolver->ResolveErrorString( ignore, CTextResolver::ECtxNoCtxNoSeparator ) );
          delete iTextResolver;
          iTextResolver = NULL;
          
          // Compose the full error message
          TBuf<100> fullnote;
          fullnote.Append(appCaption);
          fullnote.Append(':');
          fullnote.Append('\n');
          fullnote.Append(buf);
          
          // Display the error note 
          CAknErrorNote* dlg = new CAknErrorNote( ETrue );
          if(dlg==NULL)
              {
              return;
              }
          TRAPD(err4, dlg->ExecuteLD( fullnote ));
          if( err4 != KErrNone)
              {
              return;
              }
          }
        iItemAccessedViaAP = ETrue;
        }
      }
  PRINT( _L("Camera <= CCamActivePaletteHandler::NotifyItemSelected") );
  }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::NotifyItemComplete
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CCamActivePaletteHandler::NotifyItemComplete(const TActivePalette2EventData& /*aEvent*/,
                                                  const TDesC8& /*aDataDes*/, 
                                                  TInt /*aDataInt*/)        
    {
    }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::NotifyMessage
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//                          
void 
CCamActivePaletteHandler::NotifyMessage(
    const TActivePalette2EventData& /*aEvent*/,
    TInt /*aMessageID*/,
    const TDesC8& /*aDataDes*/,
    TInt /*aDataInt*/ )   
  {
  }
    
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::Redraw
//
// (other items were commented in a header)
// -----------------------------------------------------------------------------
void CCamActivePaletteHandler::Redraw(const TRect& aArea)
  {
  if ( iView )
    {
    iView->RedrawAPArea( aArea );
    }
  }
    
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ActivePaletteUpdated
//
// (other items were commented in a header)
// -----------------------------------------------------------------------------
void CCamActivePaletteHandler::ActivePaletteUpdated()
  {
  }         
    
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::LoadPostCaptureResourcesL
// Load resource for post-capture item
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::LoadPostCaptureResourcesL( TInt aResourceId )
  {
  if ( !iController.IsTouchScreenSupported() )
      {
      TResourceReader reader;
      iEnv->CreateResourceReaderLC(reader, aResourceId);
      
      // Store tooltip text and icon ID in member variables
      TPtrC ptr = reader.ReadTPtrC();
      delete iToolTipText;
      iToolTipText = NULL;
      iToolTipText = ptr.AllocL();
      
      iIconId = reader.ReadInt16();

      CleanupStack::PopAndDestroy();  // reader
      }
  }
    
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::PreCaptureArrayId
// Map settings item to CAM_CAPTURE_SETUP_MENU_ITEM_ARRAY  type resource id 
// -----------------------------------------------------------------------------
//
TInt CCamActivePaletteHandler::PreCaptureArrayId( TInt aSettingItem )
  {
  PRINT( _L("Camera => CCamActivePaletteHandler::PreCaptureArrayId") );
  TBool extLightSensitivitySupported = EFalse;
  TInt resId = KErrNotFound;
  if ( !iController.IsTouchScreenSupported() )
      {
      switch( aSettingItem )
        {
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoScene:
          resId = R_CAM_CAPTURE_SETUP_MENU_VIDEO_SCENE_ARRAY;
          break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoWhiteBalance:
          resId = R_CAM_CAPTURE_SETUP_MENU_VIDEO_WHITE_BALANCE_ARRAY;
          break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoExposure:
          break;
        // -----------------------------------------------------
    // Replaced by ECamSettingItemDynamicVideoLightSensitivity
    //    case ECamSettingItemDynamicVideoISO:
    //      break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoColourFilter:
          resId = R_CAM_CAPTURE_SETUP_MENU_VIDEO_COLOURFILTER_ARRAY;
          break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoBrightness:
          break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoContrast:
          break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoColourSaturation:
          break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoScene:
          resId = R_CAM_CAPTURE_SETUP_MENU_PHOTO_SCENE_ARRAY;
          break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoWhiteBalance:
          resId = R_CAM_CAPTURE_SETUP_MENU_PHOTO_WHITE_BALANCE_ARRAY;
          break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoExposure:
          resId = iEvCompRes;
          break;       
        // -----------------------------------------------------
    // Replaced by ECamSettingItemDynamicPhotoLightSensitivity
    //    case ECamSettingItemDynamicPhotoISO:
    //      break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoColourFilter:
          resId = R_CAM_CAPTURE_SETUP_MENU_PHOTO_COLOURFILTER_ARRAY;
          break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoFlash:
          resId = R_CAM_CAPTURE_SETUP_MENU_PHOTO_FLASH_ARRAY;
          break;       
        // -----------------------------------------------------  
        case ECamSettingItemDynamicVideoFlash:
          resId = R_CAM_CAPTURE_SETUP_MENU_VIDEO_FLASH_ARRAY;
          break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoBrightness:
          resId = R_CAM_CAPTURE_SETUP_MENU_PHOTO_BRIGHTNESS_ARRAY;
          break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoContrast:
          resId = R_CAM_CAPTURE_SETUP_MENU_PHOTO_CONTRAST_ARRAY;
          break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoImageSharpness:
          resId = R_CAM_CAPTURE_SETUP_MENU_PHOTO_IMAGESHARPNESS_ARRAY;
          break;       
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoColourSaturation:
          break;
        // -----------------------------------------------------  
        case ECamSettingItemFaceTracking:
          resId = R_CAM_ACTIVE_PALETTE_PRECAPTURE_FACETRACKING_ARRAY;
          break;
        // -----------------------------------------------------
        case ECamSettingItemSequenceMode:
          resId = R_CAM_ACTIVE_PALETTE_PRECAPTURE_SEQUENCE_MODE_ARRAY;
          break;         
        // -----------------------------------------------------
        case ECamSettingItemVideoModeSelect:
          resId = R_CAM_ACTIVE_PALETTE_PRECAPTURE_VIDEO_MODE_SELECT;
          break;
        // -----------------------------------------------------
        case ECamSettingItemImageModeSelect:
          resId = R_CAM_ACTIVE_PALETTE_PRECAPTURE_IMAGE_MODE_SELECT;
          break;
        case ECamSettingItemSwitchCamera:
            resId = R_CAM_ACTIVE_PALETTE_PRECAPTURE_SWITCH_CAMERA;
            break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicSelfTimer:
          resId = R_CAM_ACTIVE_PALETTE_SELF_TIMER_SELECT;
          break;
        // -----------------------------------------------------
        case ECamSettingItemDynamicPhotoLightSensitivity:
        case ECamSettingItemDynamicVideoLightSensitivity:
          extLightSensitivitySupported = 
             iController.UiConfigManagerPtr()->IsExtendedLightSensitivitySupported();
          if ( extLightSensitivitySupported ) 
              {
              resId = R_CAM_ACTIVE_PALETTE_EXTENDED_LIGHT_SENSITIVITY_SELECT;
              } 
          else 
              {
              resId = R_CAM_ACTIVE_PALETTE_LIGHT_SENSITIVITY_SELECT;
              }
          break;
        // -----------------------------------------------------
        case ECamSettingItemViewfinderGrid:
          resId = R_CAM_ACTIVE_PALETTE_VF_GRID_SET_ARRAY;
          break;
        // -----------------------------------------------------
        case ECamSettingItemGotoPhotos:
            resId = R_CAM_ACTIVE_PALETTE_GOTO_PHOTOS_SET_ARRAY;
            break;
        // -----------------------------------------------------          
        default:
          break;
        // -----------------------------------------------------
        } // switch
      }

  PRINT1( _L("Camera <= CCamActivePaletteHandler::PreCaptureArrayId, return:%d" ), resId )
  return resId;
  }
 
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::GetBitmapIdFromValueL
// Find bitmap id for specific settings value
// -----------------------------------------------------------------------------
//  
TInt CCamActivePaletteHandler::GetBitmapIdFromValueL( TCamAPSettingsItem& aItem )
  {
  PRINT( _L("Camera => CCamActivePaletteHandler::GetBitmapIdFromValueL") );

  if ( !iController.IsTouchScreenSupported() )
      {
      TInt arrayId = PreCaptureArrayId( aItem.iSettingItem );
      
      if ( arrayId == KErrNotFound )
        {
        return KErrNotFound;
        }

      TInt bitmapId = KErrNotFound;

      PRINT( _L("Camera <> CCamActivePaletteHandler: Looking for tooltip...") );

      TResourceReader reader;
      iEnv->CreateResourceReaderLC( reader, arrayId );
      
      // Settings model ID
      reader.ReadInt16();
      
      // Tooltip text
      TPtrC ptr = reader.ReadTPtrC();

      delete iToolTipText;
      iToolTipText = NULL;
      iToolTipText = ptr.AllocL();

      TInt tmpVal = aItem.iSettingVal;

      if ( iController.UiConfigManagerPtr()->IsContrastSupported() )
          {
          // For contrast setting map the value to bitmap value
          if ( aItem.iSettingItem == ECamSettingItemDynamicPhotoContrast ||
                    aItem.iSettingItem == ECamSettingItemDynamicVideoContrast )
              {
              if ( tmpVal > 0 )
                  {
                  tmpVal = ECamContrastPlus;
                  }
              else if ( tmpVal < 0 )
                  {
                  tmpVal = ECamContrastNeg;
                  }
              else
                  {
                  tmpVal = ECamContrastNorm;
                  }
              }          
          }

      if ( iController.UiConfigManagerPtr()->IsBrightnessSupported() )
          {
          // For brightness setting map the value to bitmap value
          if ( aItem.iSettingItem == ECamSettingItemDynamicPhotoBrightness ||
                    aItem.iSettingItem == ECamSettingItemDynamicVideoBrightness )
              {
              if ( tmpVal > 0 )
                  {
                  tmpVal = ECamBrightnessPlus;
                  }
              else if ( tmpVal < 0 )
                  {
                  tmpVal = ECamBrightnessNeg;
                  }
              else
                  {
                  tmpVal = ECamBrightnessNorm;
                  }
              }          
          }
            
      PRINT( _L("Camera <> CCamActivePaletteHandler: Looking for bitmap...") );
      const TInt count = reader.ReadInt16();
          
      for( TInt i = 0; i < count; i++ )
        {
            if (reader.ReadInt16() == tmpVal )
          {
          bitmapId = reader.ReadInt16();
          break;
          }
                
        // Skip these values if this isn't the one we're interested in.
        reader.ReadInt16();   // Bitmap ID
        reader.ReadTPtrC();     // Icon text
        }
       
      CleanupStack::PopAndDestroy();  // reader

      PRINT1( _L("Camera <= CCamActivePaletteHandler::GetBitmapIdFromValueL, return id:%d"), bitmapId );
      return bitmapId;
      }
  else
      {
      return KErrNotFound;
      }
  }
    
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::PostCaptureItemId
// Find resource ID for post capture item from command ID
// -----------------------------------------------------------------------------
//    
TInt CCamActivePaletteHandler::PostCaptureItemId( TInt aCommandId )
  {
  PRINT( _L("Camera => CCamActivePaletteHandler::PostCaptureItemId") );
  TInt resId = KErrNotFound;

  if ( !iController.IsTouchScreenSupported() )
      {
      switch( aCommandId )
        {
        case ECamCmdPhotos:
          resId = R_CAM_AP_PHOTOS;
          break;           
        case ECamCmdDelete:
          resId = R_CAM_AP_DELETE;
          break;
        case ECamCmdPlay:
          resId = R_CAM_AP_PLAY;
          break;
        case ECamCmdSend:
          resId = R_CAM_AP_SEND;
          break;
        case ECamCmdSendToCallerMultimedia:
          resId = R_CAM_AP_SEND_MULTIMEDIA;
          break;
        case ECamCmdOneClickUpload:
          resId = R_CAM_AP_ONE_CLICK_UPLOAD;
          break;
        case ECamCmdAddToAlbum:
          resId = R_CAM_AP_ADD_TO_ALBUM;
          break;        
        case ECamCmdProperties: 
          resId = R_CAM_AP_PROPERTIES;
          break; 
        default:
          break;
        }
      }
  PRINT( _L("Camera <= CCamActivePaletteHandler::PostCaptureItemId") );
  return resId; 
  }
  
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::DetermineNonSettingState
// Current state of a Non-Setting Item
// -----------------------------------------------------------------------------
//   
TInt 
CCamActivePaletteHandler
::DetermineNonSettingState( const TCamActivePaletteItem& aItem )  const
  {
  __ASSERT_DEBUG( aItem.iType == ECamActivePaletteItemTypePreCaptureNonSetting,
                  CamPanic( ECamPanicNonSettingActivePaletteTypeExpected ) );    
  
  TInt state = KErrNotFound;

  if ( !iController.IsTouchScreenSupported() )
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>(iEnv->AppUi());
      
           
      switch( aItem.iItemId )
        {
        // -----------------------------------------------------
        case ECamSettingItemSequenceMode:
          {
          TBool burst = appUi->IsBurstEnabled();
          state = CamTimeLapseUtility::IntervalToEnum( iController.TimeLapseInterval(), burst );
          PRINT1( _L("Camera <> CCamActivePaletteHandler::DetermineNonSettingState Sequence Mode %d"), appUi->IsBurstEnabled());  
          break;
          }
        // -----------------------------------------------------
        case ECamSettingItemImageModeSelect:
        case ECamSettingItemVideoModeSelect:
        case ECamSettingItemSwitchCamera:
          {
          state = 0;
          break;
          }
        // -----------------------------------------------------
        case ECamSettingItemViewfinderGrid:
          {
          state = iController.IntegerSettingValue( ECamSettingItemViewfinderGrid );
          break;
          }   
        // -----------------------------------------------------
        case ECamSettingItemFaceTracking:
          {
          state = iController.IntegerSettingValue( ECamSettingItemFaceTracking );
          break;
          }   
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoFlash:
          {
          state = iController.IntegerSettingValue( ECamSettingItemDynamicVideoFlash );
          break;
          }   
        // -----------------------------------------------------
        default:
          {
          __ASSERT_DEBUG( EFalse, CamPanic( ECamPanicInvalidActivePaletteItemType ) );
          break;
          }            
        // -----------------------------------------------------
        }
      }

  return state;
  }
    
// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::SetNonSettingItemTooltipL
// Change a Non-Setting Item's tooltip
// -----------------------------------------------------------------------------
//   
void CCamActivePaletteHandler::SetNonSettingItemTooltipL(const TCamActivePaletteItem& aItem)    
  {
  if ( !iController.IsTouchScreenSupported() )
      {
      __ASSERT_DEBUG( aItem.iType == ECamActivePaletteItemTypePreCaptureNonSetting,
          CamPanic( ECamPanicNonSettingActivePaletteTypeExpected ) );    
      
      TInt resource = 0;
      switch ( aItem.iItemId )
        {
        // -----------------------------------------------------
        case ECamSettingItemSequenceMode:
          {
          resource = ( iController.TimeLapseSupported() )
                   ? R_CAM_ACTIVE_PALETTE_PRECAPTURE_ADVANCED_SEQUENCE_MODE_TOOLTIP_ARRAY
                   : R_CAM_ACTIVE_PALETTE_PRECAPTURE_SEQUENCE_MODE_TOOLTIP_ARRAY;
          break;
          }
        // -----------------------------------------------------
        case ECamSettingItemViewfinderGrid:
          {
          resource = R_CAM_ACTIVE_PALETTE_VF_GRID_TOOLTIP_ARRAY;
          break;
          }
        // -----------------------------------------------------
        case ECamSettingItemDynamicVideoFlash:
          {
          resource = R_CAM_ACTIVE_PALETTE_VIDEO_FLASH_TOOLTIP_ARRAY;
          break;
          }
        // -----------------------------------------------------
        case ECamSettingItemFaceTracking:
          {
          resource = R_CAM_ACTIVE_PALETTE_FACETRACKING_TOOLTIP_ARRAY;
          break;
          }
        // -----------------------------------------------------
        default:
          {
          break;
          }    
        // -----------------------------------------------------
        } // switch

      if ( resource )
        {
        TResourceReader reader;
        iEnv->CreateResourceReaderLC(reader, resource);
      
        // Current Item State
        TInt state = DetermineNonSettingState(aItem);
        
        const TInt count = reader.ReadInt16();
        
        for( TInt i = 0; i < count; i++ )
          {
          if (i == state)
            {
            TPtrC ptr = reader.ReadTPtrC();
            delete iToolTipText;
            iToolTipText = NULL;
            iToolTipText = ptr.AllocL();
            break;
            }
          // Skip these values if this isn't the one we're interested in.
          reader.ReadTPtrC();     // Tooltip text
          }            
        CleanupStack::PopAndDestroy();  // reader        
        }  
      }
  }


// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::SetItemVisible
// Sets an icons visability
// -----------------------------------------------------------------------------
//    
TInt CCamActivePaletteHandler::SetItemVisible( const TInt aCommandId, const TBool aIsVisible )
  {
	PRINT( _L("Camera => CCamActivePaletteHandler::SetItemVisible")); 
	TInt ret = 0;
	if ( !iController.IsTouchScreenSupported() )
	    {
	    if ( iActivePalette )
	        {
	        return iActivePalette->SetItemVisibility( aCommandId, aIsVisible );
	        }     
	    else
	        {
	        return ret;
	        }
	    }
	else
	    {
	    return ret;
	    }
  }


// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::DisableUpdates
// Disables / enables activepalette updates. 
// When updates are disabled, UpdateActivePaletteL does nothing.
// -----------------------------------------------------------------------------
//  
void CCamActivePaletteHandler::DisableUpdates( TBool aDisableUpdates )
    {  
    PRINT1( _L("Camera => CCamActivePaletteHandler::DisableUpdates(%d)" ), aDisableUpdates );
      
    iUpdatesDisabled = aDisableUpdates;
        
    PRINT( _L("Camera <= CCamActivePaletteHandler::DisableUpdates" ) );        
    }        

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::SetOneClickUploadTooltipL
// -----------------------------------------------------------------------------
//  
void CCamActivePaletteHandler::SetOneClickUploadTooltipL(
        const TDesC& aToolTip )
    {
    delete iOneClickUploadTooltip;
    iOneClickUploadTooltip = NULL;
    iOneClickUploadTooltip = aToolTip.AllocL();
    }

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ClearActivePalette
// Deletes all items from the active palette without deleting the active palette
// object itself, as this would cause problems for the containers that have
// pointers to 
// -----------------------------------------------------------------------------
//
void CCamActivePaletteHandler::ClearActivePalette()
  {
  if ( !iController.IsTouchScreenSupported() )
      {
      RArray<TActivePalette2ItemVisible> itemArray;
      iActivePalette->GetItemList(itemArray);
      for(TInt i=0;i<itemArray.Count();i++)
        {
        iActivePalette->RemoveItem(itemArray[i].ItemId());
        }
      itemArray.Close();
      }
  }
  

// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::ReadToolbarItemsFromResourceL
// Load toolbar item resource content to argument array
// -----------------------------------------------------------------------------
//
void
CCamActivePaletteHandler::ReadToolbarItemsFromResourceL( TInt   aResId, 
                                 RArray<TCamActivePaletteItem>& aItems )
  {
  PRINT( _L("Camera => CCamActivePaletteHandler::ReadToolbarItemsFromResourceL" ) );  
  if ( !iController.IsTouchScreenSupported() )
      {
      // Create resource reader  
      TResourceReader apItemReader;
      iEnv->CreateResourceReaderLC( apItemReader, aResId );
      
      // Read number of items
      TInt numItems = apItemReader.ReadInt16();
      PRINT1( _L("Camera <> CCamActivePaletteHandler::ReadToolbarItemsFromResourceL %d items"), numItems );
      
      TInt currentPosition = 1;
      for( TInt i = 0; i < numItems; i++ )
        {
        TCamActivePaletteItem item;
        item.iItemId = apItemReader.ReadInt32();
        item.iCmdId  = apItemReader.ReadInt32();

        // NOTE:
        //   !! Current AP2 implementation ignores the iPosition value
        //      and the order is set by installing order !!!
        // 
        // There are predefined "slots" for
        // each used item. The order in resource need not to be
        // the same as the order in Active Palette.
        //    item.iPosition = apItemReader.ReadInt8();

        apItemReader.ReadInt8(); // Read unused position value
        item.iPosition = currentPosition;
        currentPosition++;

        item.iIsVisible = apItemReader.ReadInt8();
        item.iType      =    apItemReader.ReadInt8();
          
        // Append to array of items
        aItems.AppendL( item );    
        }
        
      CleanupStack::PopAndDestroy(); // apItemReader
      PRINT( _L("Camera <= CCamActivePaletteHandler::ReadToolbarItemsFromResourceL" ) ); 
      }
  } 


// -----------------------------------------------------------------------------
// CCamActivePaletteHandler::APDimensionConvertion()
// -----------------------------------------------------------------------------
//
TPoint CCamActivePaletteHandler::APScreenLocation( TPoint aQvgaPoint )
    {
    TSize ScreenSize( 0, 0 );
	AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, ScreenSize );
	TInt baseheight = 240;
	TInt lower = 0;
	if ( ScreenSize.iWidth < ScreenSize.iHeight )
    	{
    	lower = ScreenSize.iWidth;
    	}
    else
        {
        lower = ScreenSize.iHeight;
        }
    TReal ratio = TReal(lower)/baseheight;
    TInt APHeightQvga = 240 - 2*aQvgaPoint.iY;
    TReal APHeightReal;
    Math::Round( APHeightReal, ratio*APHeightQvga, 0 );
    TInt APHeight = TInt(APHeightReal);
    TInt YValue = (ScreenSize.iHeight/2) - (APHeight/2);
    
    TReal APXValueReal;
    TInt XValue;

    // With full screen viewfinder, use this!
    TInt APXLengthQvga = 320 - aQvgaPoint.iX;
    Math::Round( APXValueReal, ratio*APXLengthQvga, 0 );
    TInt APXLength = TInt(APXValueReal);
    XValue = ScreenSize.iWidth - APXLength;

    return TPoint( XValue, YValue );
    }

    
//  End of File  
