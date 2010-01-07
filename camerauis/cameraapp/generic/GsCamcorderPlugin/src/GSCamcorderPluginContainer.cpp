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
* Description:  Camcorder sub-menu in General Settings.
*
*/


// INCLUDES
#include <aknlists.h>               //for CAknSettingStyleListBox
#include <gscamerapluginrsc.rsg>
#include <camerasettingsplugin.mbg>
#include <GSListbox.h>              //for listbox clases
#include <AknIconArray.h>   // CAknIconArray
#include <eikclbd.h>        // CColumnListBoxData

#include "CamAppUid.h"
#include "GSCamcorderPluginContainer.h"
#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "GSCamcorderPlugin.h"
#include <csxhelp/lcam.hlp.hrh>

const TInt KIconArrayGranularity = 2;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::ConstructL
// Symbian OS default constructor
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::ConstructL( const TRect& aRect )
    {
    RDebug::Print( _L( "[CGSCamcorderPluginContainer]: ConstructL - Begin" ) );
    
    // Construct iListBox
    iListBox = new ( ELeave ) CAknSingleLargeStyleListBox;      
    BaseConstructL( aRect, R_GS_CAMCORDER_VIEW_TITLE, R_CAM_LBX );
    LoadIconsL();
    RDebug::Print( _L( "[CGSCamcorderPluginContainer]: ConstructL - End" ) );
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::~CGSTelPluginContainer
// Destructor
// ---------------------------------------------------------------------------
//
CGSCamcorderPluginContainer::~CGSCamcorderPluginContainer()
    {
    delete iListboxItemArray;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::UpdateListBoxL
// Update listbox item
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::UpdateListBoxL( TInt aFeatureId )
    {
    RDebug::Print( 
        _L( "[CGSCamcorderPluginContainer]: UpdateListBoxL - Begin" ) );
    switch ( aFeatureId )
        {
        case EVideoSettingsFolder:
            MakeVideoSettingsFolderItemL();
            break;
        case EImageSettingsFolder:
            MakeImageSettingsFolderItemL();
            break;
       
        default:
            break;
        }
    
    // Update the listbox
    iListBox->HandleItemAdditionL();
    RDebug::Print( 
        _L( "[CGSCamcorderPluginContainer]: UpdateListBoxL - End" ) );
    }

    
// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::ConstructListBoxL
// Update listbox item
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::ConstructListBoxL( TInt aResLbxId )
    {
    RDebug::Print( 
        _L( "[CGSCamcorderPluginContainer]: ConstructListBoxL - Begin" ) );
    iListBox->ConstructL( this, EAknListBoxSelectionList );

    iListboxItemArray = CGSListBoxItemTextArray::NewL( aResLbxId, 
        *iListBox, *iCoeEnv );
    iListBox->Model()->SetItemTextArray( iListboxItemArray );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );    
    
    CreateListBoxItemsL(); 
    RDebug::Print(
        _L( "[CGSCamcorderPluginContainer]: ConstructListBoxL - End" ) );
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::CreateListBoxItemsL
// Update listbox item
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::CreateListBoxItemsL()
    {
    RDebug::Print( 
        _L( "[CGSCamcorderPluginContainer]: CreateListBoxItemsL - Begin" ) );
    MakeVideoSettingsFolderItemL();
    MakeImageSettingsFolderItemL();
    RDebug::Print( 
        _L( "[CGSCamcorderPluginContainer]: CreateListBoxItemsL - End" ) );
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::CurrentFeatureId
// Get the currently selected feature
// ---------------------------------------------------------------------------
//
TInt CGSCamcorderPluginContainer::CurrentFeatureId() const
    {
    return iListboxItemArray->CurrentFeature();
    }


// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::MakeVideoSettingsFolderItemL
// Create Video Settings folder item
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::MakeVideoSettingsFolderItemL()
    {
    iListboxItemArray->SetItemVisibilityL( EVideoSettingsFolder, 
        CGSListBoxItemTextArray::EVisible );
    }


// ---------------------------------------------------------------------------
// CGSTelPluginContainer::MakeImageSettingsFolderItemL
// Create Image Settings folder item
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::MakeImageSettingsFolderItemL()
    {
    iListboxItemArray->SetItemVisibilityL( EImageSettingsFolder, 
        CGSListBoxItemTextArray::EVisible );
    }
   

// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::GetHelpContext
// Get help for this view
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::GetHelpContext( 
                                        TCoeHelpContext& aContext ) const
    {
#ifdef __SERIES60_HELP    
    aContext.iMajor = TUid::Uid( KCameraappUID );
	aContext.iContext = KLCAM_HLP_SETTINGS;
#endif        
    }

    
    
// ----------------------------------------------------------------------------
// CGSCamcorderPluginContainer::LoadIconsL
// Load icon bitmaps.
// ----------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::LoadIconsL()
    {
    RDebug::Print( _L( "[CGSCamcorderPluginContainer]: LoadIconsL - Begin" ) );
    CEikColumnListBox* listbox = static_cast<CEikColumnListBox*>( iListBox );

    // Delete the old icon array, if any
    delete listbox->ItemDrawer()->ColumnData()->IconArray();
    listbox->ItemDrawer()->ColumnData()->SetIconArray( NULL );

    // Construct a new CAknIconArray for iListBox
    CAknIconArray* icons = 
        new ( ELeave ) CAknIconArray( KIconArrayGranularity );
    CleanupStack::PushL( icons );
    
    TParse* fp = new(ELeave) TParse();
    CleanupStack::PushL( fp );
    TInt err = fp->Set(KDirAndFile, &KDC_APP_BITMAP_DIR, NULL); 

    if (err != KErrNone)
        {
        User::Leave(err);
        }
    TBuf<KMaxFileName> fileName = fp->FullName();
    CleanupStack::PopAndDestroy( fp );  //fp
    
    AppendIconToArrayL( 
        icons, 
        KAknsIIDQgnPropSetCamsImageSub,
        fileName,
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_folder_image,
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_folder_image_mask );

    AppendIconToArrayL(
        icons, 
        KAknsIIDQgnPropSetCamsVideoSub,
        fileName,
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_folder_video,
        EMbmCamerasettingspluginQgn_lcam_gen_sett_camera_folder_video_mask );
    
    CleanupStack::Pop( icons ); // icons

    listbox->ItemDrawer()->ColumnData()->SetIconArray( icons );
     RDebug::Print( _L( "[CGSCamcorderPluginContainer]: LoadIconsL - End" ) );
    }
  
// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::SizeChanged
// Set the size and position of component controls.
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }
    
    
// ----------------------------------------------------------------------------
// CGSCamcorderPluginContainer::FocusChanged
//Focus changed for listbox item.
// ----------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    if( iListBox )
        {
        iListBox->SetFocus( IsFocused(), aDrawNow );
        }
    }
  
// ---------------------------------------------------------------------------
// CGSCamcorderPluginContainer::AppendIconToArrayL
// Load a possibly skinned icon (with mask) and append it to an 
// icon array.
// ---------------------------------------------------------------------------
//
void CGSCamcorderPluginContainer::AppendIconToArrayL( CAknIconArray* aArray,
    const TAknsItemID& aID,
    const TDesC& aMbmFile,
    TInt aBitmapId,
    TInt aMaskId )
    {
    CGulIcon* icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), aID, aMbmFile,
        aBitmapId, aMaskId );
    
    CleanupStack::PushL( icon );
    aArray->AppendL( icon );
    CleanupStack::Pop( icon ); // icon   
    }
// End of File
