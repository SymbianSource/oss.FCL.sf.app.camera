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
* Description:  Location Setting item type for Image settings*
*/



// INCLUDE FILES
#include "CamLocationSettingItem.h"
#include "CamLocationSettingPage.h"
#include "CamUtility.h"
#include "CamAppUiBase.h"
#include "CamStaticSettingsModel.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CCamLocationSettingItem::CCamLocationSettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamLocationSettingItem::CCamLocationSettingItem(
    TCamCameraMode     aMode, 
    TInt               aIdentifier, 
    TInt&              aValue )
  : CAknEnumeratedTextPopupSettingItem( aIdentifier, aValue ),
    iMode( aMode )
  {
  }


// ---------------------------------------------------------------------------
// CCamLocationSettingItem::~CCamLocationSettingItem
// Destructor.
// ---------------------------------------------------------------------------
//
CCamLocationSettingItem::~CCamLocationSettingItem()
    {
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingItem::CreateSettingPageL
// Framework method for constructing the launching setting page.
// ---------------------------------------------------------------------------
//
CAknSettingPage* CCamLocationSettingItem::CreateSettingPageL()
    {
    PRINT( _L("Camera => CCamLocationSettingItem::CreateSettingPageL") );
    // Create an instance of our customized setting page.
    TPtrC settingName = SettingName();
    CAknSettingPage* page =  
        new( ELeave ) CCamLocationSettingPage( iMode,
                                               settingName,
                                               SettingNumber(), 
                                               EAknCtPopupSettingList,
                                               SettingEditorResourceId(),
                                               SettingPageResourceId(),
                                               *QueryValue(),
                                               static_cast<CCamAppUiBase*>( CCoeEnv::Static()->AppUi() )
                                               ->AppController() );
    PRINT( _L("Camera <= CCamLocationSettingItem::CreateSettingPageL") );
    return page;
    }

// ---------------------------------------------------------------------------
// CCamLocationSettingItem::CompleteConstructionL
// 
// ---------------------------------------------------------------------------
//
void CCamLocationSettingItem::CompleteConstructionL()
    {
    PRINT( _L("Camera => CCamLocationSettingItem::CompleteConstructionL") );
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
    // make sure we only have the support items in our test array
    CArrayPtr<CAknEnumeratedText>* array = EnumeratedTextArray();
    PRINT( _L("Camera <= CCamLocationSettingItem::CompleteConstructionL") );
    }

//  End of file
