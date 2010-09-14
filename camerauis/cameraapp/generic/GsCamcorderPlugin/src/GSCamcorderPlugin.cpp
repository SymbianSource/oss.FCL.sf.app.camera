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
* Description:  Main settings view class
*
*/


// INCLUDE FILES

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <vwsdefpartner.h>
#endif
#include <avkon.hrh>
#include <aknViewAppUi.h>
#include <eikapp.h>
#include <akntitle.h>  // cakntitlepane
#include <featmgr.h>
#include <barsread.h>
#include <aknnavide.h>
#include <bldvariant.hrh>
#include <gscamerapluginrsc.rsg>
#include <StringLoader.h>     //for stringloader
#include <gsprivatepluginproviderids.h>
#include <bautils.h>
#include <gsfwviewuids.h>     //for kgsmainviewuid
#include <aknnotewrappers.h>
#include <hlplch.h>             // for hlplauncher

#include <camerasettingsplugin.mbg>
#include "GSCamcorderPlugin.h"
#include "GSCamcorderPluginContainer.h"
#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "GSCamPhotoSettingsView.h"
#include "GSCamVideoSettingsView.h"
#include "CamLocalViewIds.h"
#include "MCamAppController.h"
#include "CamUtility.h" 
#include "CamAppUid.h" 

#include "camconfiguration.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::CGSCamcorderPlugin
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamcorderPlugin::CGSCamcorderPlugin()
    : iResources( *iCoeEnv  ),
      iSecondaryCameraSettings( EFalse ), iReloadSettings( EFalse )
    {
    
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CGSCamcorderPlugin* CGSCamcorderPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CGSCamcorderPlugin* self = new(ELeave) CGSCamcorderPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::ConstructL()
    {
    RDebug::Print(_L("[CGSCamcorderPlugin] ConstructL()" ));
    
    iViewArray = new (ELeave) CArrayPtrFlat<MGSTabbedView>( 2 );    
    
    iConfiguration = CCamConfiguration::NewL();
    iSettingsModel = CCamStaticSettingsModel::NewL( *iConfiguration );

    RDebug::Print( _L( "[CGSCamcorderPlugin] Loading resource from :" ) );
    if ( AknLayoutUtils::PenEnabled() )
        {
        RDebug::Print( KGSCamcorderPluginResourceFileName );
        OpenLocalizedResourceFileL( KGSCamcorderPluginResourceFileName, 
                                    iResources );
        }
    else
        {
        RDebug::Print( KGSCamcorderPluginResourceFileNamev2 );
        OpenLocalizedResourceFileL( KGSCamcorderPluginResourceFileNamev2, 
                                    iResources );   
        }

    FeatureManager::InitializeLibL();
    // Create needed Local views    
    CreateLocalViewL();
    BaseConstructL( R_GS_CAM_SETTING_LIST_VIEW );
    }



// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::~CGSCamcorderPlugin
// Destructor
// ---------------------------------------------------------------------------
//
CGSCamcorderPlugin::~CGSCamcorderPlugin()
    {
    RDebug::Print( _L( " ==> [CGSCamcorderPlugin] ~CGSCamcorderPlugin()" ) );
    FeatureManager::UnInitializeLib();

    if ( iViewArray )
        {
        iViewArray->Reset();
        delete iViewArray;
        }
    
    if ( iSettingsModel )
        {
        delete iSettingsModel;
        iSettingsModel = NULL;
        }
        
    delete iConfiguration;
    iConfiguration = NULL;
        
    iResources.Close();
    
    RDebug::Print( _L( " <==[CGSCamcorderPlugin] ~CGSCamcorderPlugin()" ) );
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::Id
// Return UID of view
// ---------------------------------------------------------------------------
//
TUid CGSCamcorderPlugin::Id() const
    {
    return KGSCamcorderGSPluginUid;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::HandleCommandL
// Handle commands
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::HandleCommandL( TInt aCommand )
    {
   
    switch ( aCommand )
        {
        case ECamMSKCmdAppChange:
        case EAknCmdOpen:
            HandleListBoxSelectionL();
            break;
                  
        case EAknSoftkeyBack:
            iAppUi->ActivateLocalViewL( iParentView.iViewUid );
            iParentView = KNullViewId;
            break; 
        case EAknCmdHelp:
#ifdef __SERIES60_HELP
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), iAppUi->AppHelpContextL() );
                }
#endif
            break;
        case ECamCmdInternalExit:
            if ( LaunchedFromGS() )
                {
                aCommand = EAknCmdExit;
                }
        default:
            iAppUi->HandleCommandL( aCommand );
            break;
        } 
    }
    

// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::DoActivate
// Activate this view
// ---------------------------------------------------------------------------
//
    
void CGSCamcorderPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid aCustomMessageId,
                                  const TDesC8& aCustomMessage )
    {
    RDebug::Print(_L("[CGSCamcorderPlugin] DoActivateL()" ));


    if ( !iReloadSettings )
        {
        if ( aCustomMessageId == TUid::Uid( KGSActivatePhotoSettingsView ) ||
            aCustomMessageId == TUid::Uid( KGSSecondaryCameraPhotoSettingsView ) )
            {
            RDebug::Print(_L("[CGSCamcorderPlugin] DoActivateL()1" ));	
            iParentView = aPrevViewId;
            iAppUi->ActivateLocalViewL( KGSCamImageSettingsViewId );
            return;
            }
        else if ( aCustomMessageId == TUid::Uid( KGSActivateVideoSettingsView ) ||
                 aCustomMessageId == TUid::Uid( KGSSecondaryCameraVideoSettingsView ) )
            {
            RDebug::Print(_L("[CGSCamcorderPlugin] DoActivateL()2" ));		
            iParentView = aPrevViewId;
            iAppUi->ActivateLocalViewL( KGSCamVideoSettingsViewId );
            return;
            }
        }
    
    // activate baseview only if not transfered straight to localview
    CGSBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );

    TInt highlight = KGSCamSettingsListImageIndex;
    if ( iPrevViewId.iViewUid == KGSCamVideoSettingsViewId )
        {
        highlight = KGSCamSettingsListVideoIndex;
        }
    if( iParentView == KNullViewId )
        { 
        // Parent view could be Camcorder internal view or KGSAppsPluginUid
        iParentView = aPrevViewId;
        if ( aCustomMessageId.iUid ) 
            {
            highlight = KGSCamSettingsListVideoIndex;
            }
        }

    iContainer->SetSelectedItem( highlight );
    if ( iReloadSettings )
        {
        iReloadSettings = EFalse;
        HandleListBoxSelectionL();
        }
      		
      		
        
    }
    


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::DoDeactivate()
    {
    RDebug::Print(_L("[CGSCamcorderPlugin] DoDeactivate()" ));

    CGSBaseView::DoDeactivate();
    if( iContainer )
        {
        iAppUi->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        } 
    }
    

// -----------------------------------------------------------------------------
// CGSCamcorderPlugin::HandleClientRectChange
// Handle screen size change.
// -----------------------------------------------------------------------------
//
void CGSCamcorderPlugin::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        } 
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::Container
// Return handle to container class.
// ---------------------------------------------------------------------------
//
CGSCamcorderPluginContainer* CGSCamcorderPlugin::Container()
    {
    return static_cast<CGSCamcorderPluginContainer*>( iContainer );
    }
    
    
// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::HandleListBoxSelectionL
// Handle any user actions while in the list view.
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::HandleListBoxSelectionL() 
    {
    const TInt currentFeatureId = Container()->CurrentFeatureId();

    switch ( currentFeatureId )
        {
        case EImageSettingsFolder:
        case EVideoSettingsFolder:
            ActivateInternalViewL( currentFeatureId );
            break;
        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::GetCaptionL
// Caption text for view to be displayed in NaviPane.
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::GetCaptionL( TDes& aCaption ) const
    {
    RDebug::Print(_L("[CGSCamcorderPlugin] GetCaption() - BEGIN" ));
     
    // the resource file is already opened.
    HBufC* result = StringLoader::LoadL( R_CAM_GS_SETTINGS_TITLE_NAME );
    
    aCaption.Copy( *result );
    delete result;
  
    RDebug::Print(_L("[CGSCamcorderPlugin] GetCaption() - END" ));
    }
    
    
// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::NewContainerL
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::NewContainerL()
    {
    iContainer = new( ELeave ) CGSCamcorderPluginContainer;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::ActivateInternalViewL
// Activate local views within this plug-in.
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::ActivateInternalViewL( TInt aFeatureId )
    {
    switch ( aFeatureId )
        {
        case EVideoSettingsFolder:
            iAppUi->ActivateLocalViewL( KGSCamVideoSettingsViewId );
            break;
        case EImageSettingsFolder:
            iAppUi->ActivateLocalViewL( KGSCamImageSettingsViewId );
            break;
        default:
            break;
        }
    }
   

// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::CreateLocalViewL
// Initializes local views within this plug-in based on the view ID.
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::CreateLocalViewL()
    {
    CAknView* view;
    
    view = CGSCamPhotoSettingsView::NewLC( *iSettingsModel, iViewArray );
    iAppUi->AddViewL( view );
    CleanupStack::Pop( view);
            
    view = CGSCamVideoSettingsView::NewLC( *iSettingsModel, iViewArray );
    iAppUi->AddViewL( view );
    CleanupStack::Pop(view);
    }
     

// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::CreateIconL
// Return the icon, if has one.
// ---------------------------------------------------------------------------
//
CGulIcon* CGSCamcorderPlugin::CreateIconL( const TUid /*aIconType*/ )
    {
    TParse* fp = new(ELeave) TParse();
    CleanupStack::PushL( fp );
    TInt err = fp->Set(KDirAndFile, &KDC_APP_BITMAP_DIR, NULL); 

    if (err != KErrNone)
        {
        User::Leave(err);
        }

    TBuf<KMaxFileName> fileName = fp->FullName();
    CleanupStack::PopAndDestroy( fp );  //fp

    CGulIcon* icon;
    
    icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropSetAppsCams,
        fileName,
        EMbmCamerasettingspluginQgn_prop_set_apps_cams,
        EMbmCamerasettingspluginQgn_prop_set_apps_cams_mask );
    
    return icon;
    }
    
    
// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::PluginProviderCategory
// A means to identify the location of this plug-in in the framework.
// ---------------------------------------------------------------------------
//
TInt CGSCamcorderPlugin::PluginProviderCategory() const
    {
    //To identify internal plug-ins.
    return KGSPluginProviderInternal;
    }
  
  
// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::TabbedViews()
// Returns pointer to tabbed views of application.
// ---------------------------------------------------------------------------
//
CArrayPtrFlat<MGSTabbedView>* CGSCamcorderPlugin::TabbedViews()
    {
    return iViewArray;
    }
    
    
// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::Visible()
// Method for checking, if plugin should be visible and used in GS.
// ---------------------------------------------------------------------------
//    
TBool CGSCamcorderPlugin::Visible() const
    {
    return ETrue;
    }
    
    
// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::CustomOperationL()
// Reserved for later use.
// ---------------------------------------------------------------------------
//        
TAny* CGSCamcorderPlugin::CustomOperationL( TAny* aParam1, TAny* /*aParam2*/ )
    {
    return aParam1;    
    }
    

// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::DynInitMenuPaneL
// Dynamically customize menu items
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::DynInitMenuPaneL( TInt aResourceId,
                                           CEikMenuPane* aMenuPane )
    {
	if ( aResourceId == R_CAM_SETTINGS_LIST_MENU )
        {
#ifdef __SERIES60_HELP
        if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
            {
            aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
            }
#endif
        }
	}


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::LaunchedFromGS
// For variating the construction of the setting items
// ---------------------------------------------------------------------------
//
TBool CGSCamcorderPlugin::LaunchedFromGS() const
    {
    return iAppUi->Application()->AppDllUid().iUid != KCameraappUID;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::IsSecondaryCameraSettings()
// Returns if we have to lauch secondary camera settings or main camera settings
// ---------------------------------------------------------------------------
//
TBool CGSCamcorderPlugin::IsSecondaryCameraSettings()
	{
	return iSecondaryCameraSettings;
	}

// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::SetSecondaryCameraSettings()
// Sets if camera has lauched secondary camera settings or main camera settings
// ---------------------------------------------------------------------------
//
void CGSCamcorderPlugin::SetSecondaryCameraSettings( TBool aSecondarySettings )
    {
    iSecondaryCameraSettings = aSecondarySettings;
    }


//
// CGSCamcorderPlugin::IntegerSettingValue
//
TInt CGSCamcorderPlugin::IntegerSettingValue( const TInt aSettingItem )
    {
    if ( LaunchedFromGS() )
        {
        TInt settingValue = iSettingsModel->IntegerSettingValue( aSettingItem );

        if (  ( ECamSettingItemPhotoMediaStorage == aSettingItem || 
                ECamSettingItemVideoMediaStorage == aSettingItem ) && 
              ( settingValue == ECamMediaStorageCard ) &&
              ( CamUtility::MemoryCardStatus() != ECamMemoryCardInserted ) )
          {
          if ( CamUtility::MassMemoryDrive() > 0 )
              {
              settingValue = ECamMediaStorageMassStorage;
              }
          else
              {
              settingValue = iSettingsModel->IntegerSettingValue(ECamSettingItemRemovePhoneMemoryUsage)?
                              ECamMediaStorageNone:
                              ECamMediaStoragePhone;
              }
          }
        return settingValue;
        }
    else
        {
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() );
        MCamAppController& controller = appUi->AppController();
        return controller.IntegerSettingValue( aSettingItem );
        }
    }

//
// CGSCamcorderPlugin::TextValueFromSettingsModel
//
TPtrC CGSCamcorderPlugin::TextSettingValue( const TInt aSettingItem )
    {
    if ( LaunchedFromGS() )
        {
        return iSettingsModel->TextSettingValue( aSettingItem );
        }
    else
        {
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() );
        MCamAppController& controller = appUi->AppController();
        return controller.TextSettingValue( aSettingItem );
        }
    }


//
// CGSCamcorderPlugin::SetTextSettingValueL
//
void CGSCamcorderPlugin::SetTextSettingValueL( TInt aSettingItem,
                                               const TDesC& aSettingValue )
    {
    if ( !LaunchedFromGS() )
        {
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() );
        MCamAppController& controller = appUi->AppController();
        controller.SetTextSettingValueL( aSettingItem, aSettingValue );
        }
    else
        {
        iSettingsModel->SetTextSettingValueL( aSettingItem, aSettingValue );
        }
    }

//
// CGSCamcorderPlugin::SetIntegerSettingValueL
//
void CGSCamcorderPlugin::SetIntegerSettingValueL( TInt aSettingItem,
                                                  TInt aSettingValue )
    {
    if ( !LaunchedFromGS() )
        {
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() );
        MCamAppController& controller = appUi->AppController();
        controller.SetIntegerSettingValueL( aSettingItem, aSettingValue );
        }
    else
        {
        iSettingsModel->SetIntegerSettingValueL( aSettingItem, aSettingValue );
        }
    }


//
// CGSCamcorderPlugin::LoadVideoStaticSettingsL
//
void CGSCamcorderPlugin::LoadVideoStaticSettingsL( TBool aResetFromGS )
    {
    if ( LaunchedFromGS() )
        {
        iSettingsModel->LoadVideoStaticSettingsL( aResetFromGS );
        }
    else
        {
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() );
        MCamStaticSettings& model = appUi->StaticSettingsModel();
        model.LoadVideoStaticSettingsL( aResetFromGS );
        }
    }



//
// CGSCamcorderPlugin::LoadPhotoStaticSettingsL
//
void CGSCamcorderPlugin::LoadPhotoStaticSettingsL( TBool aResetFromGS )
    {
    if ( LaunchedFromGS() )
        {
        iSettingsModel->LoadPhotoStaticSettingsL( aResetFromGS );
        }
    else
        {
        CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() );
        MCamStaticSettings& model = appUi->StaticSettingsModel();
        model.LoadPhotoStaticSettingsL( aResetFromGS );
        }
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPlugin::DisplayRestoreSettingsDlgL
// Display restore Camera setting confirmation dialog
// ---------------------------------------------------------------------------
//
TInt CGSCamcorderPlugin::DisplayRestoreSettingsDlgL()
  {
  HBufC* confirmationText;
    confirmationText = StringLoader::LoadLC( R_CAM_RESTORE_SETTINGS_NOTE_TEXT );
    CAknQueryDialog* confirmationDialog = new( ELeave )CAknQueryDialog( *confirmationText );
    CleanupStack::PopAndDestroy( confirmationText );
    TInt response = confirmationDialog->ExecuteLD( R_CAM_RESTORE_SETTINGS_NOTE ); 
    if ( response ) 
        {
        // since restore camera setting item is visible only when
        // launched from camera, we inform the specific view to handle
        // this from the camera side, instead doing it here in the plugin
        if ( !LaunchedFromGS() )
            {
            MCamAppController& controller = 
            static_cast<CCamAppUiBase*>(
                    CCoeEnv::Static()->AppUi() )->AppController();
            controller.RestoreFactorySettingsL( EFalse );
            }
        else
            {
            iSettingsModel->ResetRepository();
            iReloadSettings = ETrue;
            }
        } 
  return response;
  }

// End of file
