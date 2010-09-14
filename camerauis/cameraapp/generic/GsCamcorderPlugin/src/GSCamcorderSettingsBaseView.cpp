/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Image settings view
*/


// INCLUDE FILES

#include <avkon.hrh>
#include <aknViewAppUi.h>
#include <eikapp.h>
#include <apparc.h>
#include <akntabgrp.h>
#include <akntitle.h>  // CAknTitlePane
#include <featmgr.h>
#include <eikmenup.h>
#include <bldvariant.hrh>
#include <barsread.h>
#include <gscamerapluginrsc.rsg>
#include <camerasettingsplugin.mbg>
#include <hlplch.h>             // For HlpLauncher

#include <aknsettingitemlist.h> // CAknSettingItemList

#include "CamAppUid.h"

#include "GSCamcorderPlugin.h"
#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "GSCamcorderSettingsBaseView.h"
#include "GSCamcorderSettingsContainer.h"
#include "GSCamcorderPlugin.h"
#include "MCamAppController.h"
#include "CamAppUiBase.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::CGSCamcorderSettingsBaseView
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamcorderSettingsBaseView::CGSCamcorderSettingsBaseView( CCamStaticSettingsModel& aModel,
                                                            TBool aPhotoSettings )
: iModel( aModel ),
  iSecondaryCameraSettings( EFalse )
    {
    iPhotoSettings = aPhotoSettings;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::~CGSCamcorderSettingsBaseView
// Destructor
// ---------------------------------------------------------------------------
//
CGSCamcorderSettingsBaseView::~CGSCamcorderSettingsBaseView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    }

// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::LaunchedFromGS
// For variating the construction of the setting items
// ---------------------------------------------------------------------------
//
TBool CGSCamcorderSettingsBaseView::LaunchedFromGS() const
    {
    return AppUi()->Application()->AppDllUid().iUid != KCameraappUID;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::HandleCommandL
// From MEikMenuObserver delegate commands from the menu
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case ECamMSKCmdAppChange:
        	 static_cast<CGSCamcorderSettingsContainer*> ( iContainer )->EditCurrentL( EFalse );
        	 break;
        case EAknCmdHelp:
#ifdef __SERIES60_HELP
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), AppUi()->AppHelpContextL() );
                }
#endif
            break;
        case EAknCmdOpen: // MSK change 
            { 
            TKeyEvent key;
            key.iRepeats = 0;
            key.iCode = EKeyEnter;
            key.iModifiers = 0;
            iEikonEnv->SimulateKeyEventL( key, EEventKey );
            break;
            }
        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::DoActivateL( const TVwsViewId& /* aPrevViewId */,
                                             TUid /*aCustomMessageId*/,
                                             const TDesC8& /*aCustomMessage*/ )
    {
    SetNaviPaneL();
    StatusPane()->MakeVisible( ETrue );	 
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    CGSCamcorderPlugin* parent = 
        static_cast<CGSCamcorderPlugin*> ( 
            AppUi()->View( KGSCamcorderGSPluginUid ) );
    iSecondaryCameraSettings = parent->IsSecondaryCameraSettings();
    CreateContainerL();
    AppUi()->AddToViewStackL( *this, iContainer );

    iContainer->SetRect( ClientRect() );   
    iContainer->ActivateL();
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::DoDeactivate()
    {
    if ( iContainer )
        {        
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::Container
// Returns network container item
// ---------------------------------------------------------------------------
//
CGSCamcorderSettingsContainer* CGSCamcorderSettingsBaseView::Container()
    {
    return static_cast <CGSCamcorderSettingsContainer*> ( iContainer );
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::DynInitMenuPaneL
// Dynamically customize menu items
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::DynInitMenuPaneL( TInt aResourceId,
                                               CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_GS_CAM_SETTINGS_MENU )
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
// CGSCamcorderSettingsBaseView::NewContainerL()
// Creates new iContainer.
// ---------------------------------------------------------------------------
void CGSCamcorderSettingsBaseView::NewContainerL()
    {
    iContainer = new (ELeave) CGSCamcorderSettingsContainer;
    }


// -----------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::HandleClientRectChange
// Handle screen size change.
// -----------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        } 
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::HandleListBoxEventL
// 
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::HandleListBoxEventL( CEikListBox* /*aListBox*/, 
                                            TListBoxEvent aEventType )
   {
    switch ( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            if( iContainer )
                {
                iContainer->EditCurrentL();
                }
            break;
        default:
           break;
        }
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::CreateContainerL()
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::CreateContainerL()
    {        
    NewContainerL();
    iContainer->SetMopParent( this );

    TRAPD( error, iContainer->ConstructL( iModel, 
                                          LaunchedFromGS(),
                                          iSecondaryCameraSettings,
                                          iPhotoSettings ) );

    if ( error )
        {
        delete iContainer;
        iContainer = NULL;
        User::Leave( error );
        }
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsBaseView::SetNaviPaneL
// Sets Navipane texts.
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsBaseView::SetNaviPaneL()
    {
    CEikStatusPane* sp = AppUi()->StatusPane();
    CAknTitlePane* title = static_cast<CAknTitlePane*> 
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    
    TInt resId = R_CAM_PHOTO_SETTINGS_TITLE_NAME;
    if ( !iPhotoSettings )
        {
        resId = R_CAM_VIDEO_SETTINGS_TITLE_NAME;
        }
    // Set view title from resource:
    TResourceReader rReader;
    iCoeEnv->CreateResourceReaderLC( rReader, resId );
    title->SetFromResourceL( rReader );
    CleanupStack::PopAndDestroy(); //rReader 
    }
    
// End of File
