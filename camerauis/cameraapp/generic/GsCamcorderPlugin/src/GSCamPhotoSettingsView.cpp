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
* Description:  Image settings view
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
#include <GSTabHelper.h>
#include "CamAppUid.h"
#include "GSCamcorderPlugin.h"
#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "GSCamPhotoSettingsView.h"
#include "GSCamcorderPlugin.h"
#include "CamLocalViewIds.h"
#include "GSCamcorderSettingsBaseView.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::CGSCamPhotoSettingsView
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamPhotoSettingsView::CGSCamPhotoSettingsView( CCamStaticSettingsModel& aModel )
: CGSCamcorderSettingsBaseView( aModel, ETrue )
    {
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::NewLC
// Symbian OS two-phased constructor.
// ---------------------------------------------------------------------------
//
CGSCamPhotoSettingsView* CGSCamPhotoSettingsView::NewLC(
    CCamStaticSettingsModel& aModel, 
    CArrayPtrFlat<MGSTabbedView>* aTabViewArray )
    {
    CGSCamPhotoSettingsView* self = 
        new ( ELeave ) CGSCamPhotoSettingsView( aModel );
    CleanupStack::PushL( self );
    self->ConstructL( aTabViewArray );    
    return self;
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CGSCamPhotoSettingsView::ConstructL( CArrayPtrFlat<MGSTabbedView>* aTabViewArray )
    {
    iTabHelper = CGSTabHelper::NewL();
    aTabViewArray->AppendL(this);
    BaseConstructL( R_CAM_SETTING_LIST_VIEW );
    }



// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::~CGSCamPhotoSettingsView
// Destructor
// ---------------------------------------------------------------------------
//
CGSCamPhotoSettingsView::~CGSCamPhotoSettingsView()
    {
    delete iTabHelper;
    iTabHelper = NULL;
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::Id
// From CAknView, returns Uid of View
// ---------------------------------------------------------------------------
//
TUid CGSCamPhotoSettingsView::Id() const
    {
    return KGSCamImageSettingsViewId;
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::HandleCommandL
// ---------------------------------------------------------------------------
//
void CGSCamPhotoSettingsView::HandleCommandL( TInt aCommand )
    {
    const TVwsViewId KImagePreCaptureView ( TUid::Uid( KCameraappUID ), KImagePreCaptureId );
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
            	AppUi()->ActivateViewL( KImagePreCaptureView,
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
            }
            break;
        default:
            {
            CGSCamcorderSettingsBaseView::HandleCommandL( aCommand );
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::DoActivateL
// Activate this view
// ---------------------------------------------------------------------------
//
void CGSCamPhotoSettingsView::DoActivateL( const TVwsViewId& aPrevViewId,
                                          TUid aCustomMessageId,
                                          const TDesC8& aCustomMessage )
    {
    CGSCamcorderPlugin* parent = 
         static_cast<CGSCamcorderPlugin*> ( 
             AppUi()->View( KGSCamcorderGSPluginUid ) );
         
    if ( aCustomMessageId == TUid::Uid( KGSSecondaryCameraPhotoSettingsView ) )
         {
         RDebug::Print(_L("[CGSCamcorderPlugin] DoActivateL()1" )); 
         if( parent)
             {
             parent->SetSecondaryCameraSettings( ETrue );
             }
         }
   else
         {
         parent->SetSecondaryCameraSettings( EFalse );
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
// CGSCamPhotoSettingsView::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CGSCamPhotoSettingsView::DoDeactivate()
    {
    CGSCamcorderSettingsBaseView::DoDeactivate();
    iTabHelper->RemoveTabGroup();
    }


// ---------------------------------------------------------------------------
// CGSCamPhotoSettingsView::CreateTabIconL()
// Creates icon for tab.
// ---------------------------------------------------------------------------
//    
CGulIcon* CGSCamPhotoSettingsView::CreateTabIconL()
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
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_tab_image,
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_tab_image );
    
    return icon;
    }       

// End of File
