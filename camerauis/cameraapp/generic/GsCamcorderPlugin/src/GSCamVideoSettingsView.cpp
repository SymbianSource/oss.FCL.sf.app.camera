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
* Description:  Video settings view
*
*/


// INCLUDE FILES

#include <avkon.hrh>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <akntitle.h>  // CAknTitlePane
#include <eikmenup.h>
#include <bldvariant.hrh>
#include <barsread.h>
#include <gscamerapluginrsc.rsg>
#include <camerasettingsplugin.mbg>
#include <gstabhelper.h>

#include "CamAppUid.h"
#include "GSCamcorderPlugin.h"
#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "GSCamVideoSettingsView.h"
#include "GSCamcorderSettingsBaseView.h"
#include "GSCamcorderPlugin.h"
#include "CamLocalViewIds.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::CGSCamVideoSettingsView
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamVideoSettingsView::CGSCamVideoSettingsView( CCamStaticSettingsModel& aModel )
: CGSCamcorderSettingsBaseView( aModel, EFalse )
    {
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::NewLC
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
//
CGSCamVideoSettingsView* CGSCamVideoSettingsView::NewLC(
    CCamStaticSettingsModel& aModel,
    CArrayPtrFlat<MGSTabbedView>* aTabViewArray )
    {
    CGSCamVideoSettingsView* self = 
        new ( ELeave ) CGSCamVideoSettingsView( aModel );
    CleanupStack::PushL( self );
    self->ConstructL( aTabViewArray );    
    return self;
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CGSCamVideoSettingsView::ConstructL( CArrayPtrFlat<MGSTabbedView>* aTabViewArray )
    {
    iTabHelper = CGSTabHelper::NewL();
    aTabViewArray->AppendL(this);
    BaseConstructL( R_CAM_SETTING_LIST_VIEW );
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::~CGSCamVideoSettingsView()
// Destructor
// ---------------------------------------------------------------------------
//
CGSCamVideoSettingsView::~CGSCamVideoSettingsView()
    {
    delete iTabHelper;
    iTabHelper = NULL;
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::Id
// From CAknView, returns Uid of View
// ---------------------------------------------------------------------------
//
TUid CGSCamVideoSettingsView::Id() const
    {
    return KGSCamVideoSettingsViewId;
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::HandleCommandL
// From MEikMenuObserver delegate commands from the menu
// ---------------------------------------------------------------------------
//
void CGSCamVideoSettingsView::HandleCommandL( TInt aCommand )
    {
    const TVwsViewId KVideoPreCaptureView( TUid::Uid( KCameraappUID ),
                                           KVideoPreCaptureId );
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
            {
            if ( LaunchedFromGS() )
            	{
            	AppUi()->ActivateLocalViewL( KGSCamcorderGSPluginUid );
            	}
            else
            	{
            	AppUi()->ActivateViewL( KVideoPreCaptureView,
            	                        TUid::Uid( aCommand ),
            	                        KNullDesC8 );
            	}
            break;
            }
            
        case ECamCmdInternalExit:
            {
            if ( LaunchedFromGS() )
                {
                AppUi()->HandleCommandL( EAknCmdExit );
                }
            else
                {
                //To ensure that camera resources are released when exiting
                //settings, we need to call internal exit here to close the
                //camera completely
                AppUi()->HandleCommandL( aCommand );
                }
            break;
            }
        default:
            {
            CGSCamcorderSettingsBaseView::HandleCommandL( aCommand );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CGSCamVideoSettingsView::DoActivateL( const TVwsViewId& aPrevViewId,
                                           TUid aCustomMessageId,
                                           const TDesC8& aCustomMessage )
    { 
    
    CGSCamcorderPlugin* parent = 
          static_cast<CGSCamcorderPlugin*> ( 
              AppUi()->View( KGSCamcorderGSPluginUid ) );
          
     if ( aCustomMessageId == TUid::Uid( KGSSecondaryCameraVideoSettingsView ) )
          {
          if( parent)
              {
              parent->SetSecondaryCameraSettings( ETrue );
              }
          }
    else
          {
          if( parent )
              {
              parent->SetSecondaryCameraSettings( EFalse );
              }
          }
     
    CGSCamcorderSettingsBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    //StatusPane()->MakeVisible( ETrue );
 
    if( parent )
        {
        if ( LaunchedFromGS() )
            {
            iTabHelper->CreateTabGroupL( Id(), 
                static_cast<CArrayPtrFlat<MGSTabbedView>*>
                    ( parent->TabbedViews() ) );	
            }
        } 
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CGSCamVideoSettingsView::DoDeactivate()
    {
    CGSCamcorderSettingsBaseView::DoDeactivate();
    if ( iTabHelper )
        {
        iTabHelper->RemoveTabGroup();
        }
    }


// ---------------------------------------------------------------------------
// CGSCamVideoSettingsView::CreateTabIconL()
// Creates icon for tab.
// ---------------------------------------------------------------------------
//    
CGulIcon* CGSCamVideoSettingsView::CreateTabIconL()
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
        KAknsIIDDefault, 
        fileName,
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_tab_video,
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_tab_video );
    
    return icon;
    }       
        
// End of File
