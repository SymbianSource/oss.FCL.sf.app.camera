/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Image settings container
*
*  Copyright (c) 2008 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
*
*
*/


// INCLUDE FILES
#include <eiktxlbx.h>
#include <aknlists.h>       // CAknSingleGraphicStyleListBox
#include <barsread.h>       // TResourceReader
#include <eikclbd.h>        // CColumnListBoxData

#include <aknsettingitemlist.h> // CAknSettingItemList

#include "CamAppUid.h"

#include <csxhelp/lcam.hlp.hrh>
#include "GSCamcorderSettingsContainer.h"
#include "GSCamPhotoSettingsList.h"
#include "GSCamVideoSettingsList.h"
#include "CamAppUiBase.h"
#include "MCamAppController.h"
#include "CameraUiConfigManager.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsContainer::ConstructL( CCamStaticSettingsModel& aModel, 
                                                TBool aLauchedFromGS,
                                                TBool aSecondaryCameraSettings,
                                                TBool aImageSettings )
    {
    iLauchedFromGS = aLauchedFromGS;
    iImageSettings = aImageSettings;
    if ( iImageSettings )
        {
        iSettingsList = new ( ELeave ) CGSCamPhotoSettingsList(  aModel );
        }
    else
        {
        iSettingsList = new ( ELeave ) CGSCamVideoSettingsList(  aModel );
        }
    CreateWindowL();
    
    iSettingsList->SetContainerWindowL( *this );
    if ( iImageSettings )
        {
        static_cast<CGSCamPhotoSettingsList*>( iSettingsList )->ConstructL( 
                                               aLauchedFromGS, aSecondaryCameraSettings );
        }
    else
        {
        static_cast<CGSCamVideoSettingsList*>( iSettingsList )->ConstructL( 
                                               aLauchedFromGS, aSecondaryCameraSettings );
        }

    if ( !iLauchedFromGS )
        {
        // read the product specific capture key from ui config manager
        RArray<TInt> primaryCaptureKeys;
        CleanupClosePushL( primaryCaptureKeys );
        if ( aModel.UiConfigManagerPtr() )
           {
           aModel.UiConfigManagerPtr()->SupportedPrimaryCameraCaptureKeyL( primaryCaptureKeys );        
           }
        iProductCaptureKey = primaryCaptureKeys[0];
        CleanupStack::PopAndDestroy( &primaryCaptureKeys );        
        }

    /*
    if ( !aLauchedFromGS )
        {
        // create navi pane bitmaps and add to navipane
        // only if launched from camera application.
	    CreateNaviBitmapsL();
	    }
    */	    
    }


// Destructor
CGSCamcorderSettingsContainer::~CGSCamcorderSettingsContainer()
    {
    if ( iSettingsList )
        {
        delete iSettingsList;
        iSettingsList = NULL;
        }
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::EditCurrentL
// Open setting page for currently selected setting item.
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsContainer::EditCurrentL( TBool aCalledFromMenu /*=ETrue*/ )
    {
    TInt index = iSettingsList->SettingItemArray()->ItemIndexFromVisibleIndex(
                    iSettingsList->ListBox()->CurrentItemIndex() );
    iSettingsList->EditItemL( index, aCalledFromMenu );
    }


// ---------------------------------------------------------
// CGSCamcorderSettingsContainer::GetHelpContext
// Gives the help context to be displayed
// ---------------------------------------------------------
//
void CGSCamcorderSettingsContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
#ifdef __SERIES60_HELP    
    aContext.iMajor = TUid::Uid( KCameraappUID );
    if ( iImageSettings )
        {
        aContext.iContext = KLCAM_HLP_SETTINGS_PHOTO;
        }
    else
        {
        aContext.iContext = KLCAM_HLP_SETTINGS_VIDEO;
        }
#endif    
    }
    
    
// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::CountComponentControls
// From CCoeControl return the number of controls owned
// ---------------------------------------------------------------------------
//
TInt CGSCamcorderSettingsContainer::CountComponentControls() const
    {
    return 1;
    }


// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::ComponentControl
// From CCoeControl returns a control
// ---------------------------------------------------------------------------
//
CCoeControl* CGSCamcorderSettingsContainer::ComponentControl(
    TInt /* aIndex */ ) const
    {
    return iSettingsList;
    }

// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::SettingsList
// Returns a pointer to settingslist
// ---------------------------------------------------------------------------
//
CAknSettingItemList* CGSCamcorderSettingsContainer::SettingsList() const
    {
    return iSettingsList;
    }
    

// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::OfferKeyEventL
// Key event handling
// ---------------------------------------------------------------------------
//
TKeyResponse CGSCamcorderSettingsContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( aKeyEvent.iScanCode == iProductCaptureKey && !iLauchedFromGS )
        {
        CCamAppUiBase* camAppUi = (CCamAppUiBase*)iAvkonAppUiBase;

        TBool background = camAppUi->AppInBackground( EFalse );
        
        if ( background )
            {
            // Capture key pressed and camera in background.
            // Jump to foreground.
            CEikonEnv::Static()->RootWin().SetOrdinalPosition( 0, 0 );
            return EKeyWasConsumed;
            }        
        return EKeyWasNotConsumed;
        }
    if ( aKeyEvent.iCode == EKeyLeftArrow ||
            aKeyEvent.iCode == EKeyRightArrow )
        {
        // Listbox takes all events even if it doesn't use them
        return EKeyWasNotConsumed; 
        }
    return iSettingsList->OfferKeyEventL( aKeyEvent, aType );
    }

    
   
// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::SizeChanged
// Set the size and position of component controls.
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsContainer::SizeChanged()
    {
    if ( iSettingsList )
        {
        iSettingsList->SetRect( Rect() );
        }
    }
    
// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::HandleResourceChange
// Handles resource changes, e.g. layout changes of screen.
// ---------------------------------------------------------------------------
void CGSCamcorderSettingsContainer::HandleResourceChange( TInt aType )
    {
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane,
                                           mainPaneRect);
        SetRect( mainPaneRect );
        }
    CCoeControl::HandleResourceChange( aType );
    }
    

// ---------------------------------------------------------
// CGSCamcorderSettingsContainer::FocusChanged
// Focus changed for listbox item.
// ---------------------------------------------------------
//
void CGSCamcorderSettingsContainer::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    if ( iSettingsList )
        {
        iSettingsList->SetFocus( IsFocused(), aDrawNow );
        }
    //CreateNaviBitmapsL();
    }

// ---------------------------------------------------------------------------
// CGSCamcorderSettingsContainer::CreateNaviBitmapsL()
// We ask navi counter model to draw the counter and storage icon for us
// when we are in image settings.
// ---------------------------------------------------------------------------
//
void CGSCamcorderSettingsContainer::CreateNaviBitmapsL()
    {
	if ( !iLauchedFromGS )
        {
        CCamAppUiBase* camAppUi = (CCamAppUiBase*)iAvkonAppUiBase;
        if ( camAppUi )
            {
            camAppUi->CreateNaviBitmapsL( ETrue );
            }
         }
    }

// End of File
