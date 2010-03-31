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
* Description:  A list box that displays settings items with radio buttons*
*/


// INCLUDE FILES
#include "CamCaptureSetupListBox.h"
#include "CamCaptureSetupListBoxModel.h"
#include "CamCaptureSetupListItemDrawer.h"
#include "CamSettingValueObserver.h"
#include <uikon.hrh>

#include "CamUtility.h"

#include <aknlayoutscalable_apps.cdl.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBox::CCamCaptureSetupListBox
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamCaptureSetupListBox::CCamCaptureSetupListBox( MCamSettingValueObserver* aObserver
#ifndef CAMERAAPP_PLUGIN_BUILD
        						                  ,CCamAppController& aController 
#else
                                                  ,MCamAppController& aController
#endif //CAMERAAPP_PLUGIN_BUILD
                                                  ,TBool aSkinnedBackGround
                                                 )
: iSettingObserver( aObserver ), iController( aController ), iSkinnedBackGround( aSkinnedBackGround )
    {
    iBorder = TGulBorder::ENone;
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBox::ConstructL
// 2nd phase construction
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListBox::ConstructL( const CCoeControl* aParent, 
                                          TInt aArrayResourceId, 
                                          TInt aSettingItem,
                                          TInt aFlags,
                                          TBool aShootingModeListBox,
                                          TBool aLocationSettingListBox
                                          )
	{

	//Constructing model
    CCamCaptureSetupListBoxModel* model =
             CCamCaptureSetupListBoxModel::NewLC( iController,
             									  aArrayResourceId, 
             									  aSettingItem,
             								      aShootingModeListBox,
                                                  aLocationSettingListBox,
                                                  Rect()
             								      );
    CCamCaptureSetupListItemDrawer* itemDrawer = 
        CCamCaptureSetupListItemDrawer::NewL( *model, iSkinnedBackGround || aLocationSettingListBox );
                                
    itemDrawer->SetParentControl( aParent );
    // CEikListBox takes ownership before anything can leave.
    // Do not need the model or drawer on the stack when call ConstructL,
    // because CEikListBox assigns objects as member variables before
    // ConstructL calls any leaving functions.
	CleanupStack::Pop( model ); 
	CEikListBox::ConstructL( model, itemDrawer, aParent, aFlags );
    // The number of items is fixed after the list box has been constructed
    const TInt KMaxListItems = 5; // This value should come from the LAF
    // Find the number of items to be displayed in the list box
    iRequiredHeightInNumOfItems = Min( Model()->NumberOfItems(), KMaxListItems );
    SetItemHeightL( iItemDrawer->MinimumCellSize().iHeight );
	}

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBox destructor
// 
// ---------------------------------------------------------------------------
//
CCamCaptureSetupListBox::~CCamCaptureSetupListBox()
	{
  PRINT( _L("Camera => ~CCamCaptureSetupListBox") );
  PRINT( _L("Camera <= ~CCamCaptureSetupListBox") );
	}

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBox::InitializeL
// Initialize the current and selected item in the view
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListBox::InitializeL( TInt aCurrentValueId )
    {
    TInt selectedItemIndex = 
      static_cast<MCamListboxModel*>( iModel )->ValueIndex( aCurrentValueId );

    // if the item does not exist select the topmost item
    selectedItemIndex = Max( selectedItemIndex, 0 );
    if ( selectedItemIndex < iModel->NumberOfItems() )
        {
        iView->SetCurrentItemIndex( selectedItemIndex );
        iView->SetDisableRedraw( ETrue );
        iView->SelectItemL( selectedItemIndex );
        iView->SetDisableRedraw( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListBox::OfferKeyEventL
// Handle key presses
// ---------------------------------------------------------------------------
//
TKeyResponse CCamCaptureSetupListBox::OfferKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType )
    {
    PRINT( _L("Camera => CCamCaptureSetupListBox::OfferKeyEventL" ))
    // If the Select or navi key select button is pressed, select the current item
    if ( ( aKeyEvent.iCode == EKeyOK && aKeyEvent.iRepeats == 0 && aType == EEventKey )
        || aKeyEvent.iScanCode == EStdKeyDevice0 ) 
        {
        iView->ClearSelection();
	    iView->SelectItemL( iView->CurrentItemIndex() );
        }
    //TInt oldCurrentItem = CurrentItemIndex();
    //TKeyResponse response = CEikListBox::OfferKeyEventL( aKeyEvent, aType );
    TKeyResponse response = EKeyWasConsumed;
    /*TInt newCurrentItem = CurrentItemIndex();
    if ( newCurrentItem != oldCurrentItem )
        {
        PRINT( _L("Camera => CCamCaptureSetupListBox::OfferKeyEventL set obs" ))
        MCamListboxModel* model = static_cast<MCamListboxModel*>( iModel );
        iSettingObserver->HandleSettingValueUpdateL( model->ItemValue( newCurrentItem ) );
        }*/
    return response;
    }






// ---------------------------------------------------------------------------
// CCamCaptureSetupListBox::HandlePointerEventL
// Handle pointer events
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {   
    /*TInt oldCurrentItem = CurrentItemIndex();
    TInt newCurrentItem = CurrentItemIndex();
    
    // listbox does default handling
    CEikListBox::HandlePointerEventL(aPointerEvent);
    
    newCurrentItem = CurrentItemIndex();
      
    // handle settings update when focus changes  
	if ( newCurrentItem != oldCurrentItem )
	    {
	    MCamListboxModel* model = static_cast<MCamListboxModel*>( iModel );
	    iSettingObserver->HandleSettingValueUpdateL( model->ItemValue( newCurrentItem ) );

	    }*/
    iController.StartIdleTimer(); // standby
    CEikListBox::HandlePointerEventL(aPointerEvent);
    }


//End of file

