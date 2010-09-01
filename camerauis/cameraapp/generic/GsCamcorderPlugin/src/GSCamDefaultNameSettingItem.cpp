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
* Description:  Setting item type for CAknEnumeratedItems.*
*/



// INCLUDE FILES
#include "GSCamDefaultNameSettingItem.h"
#include "GSCamdefaultnamesettingpage.h"
#include "CamSettingsInternal.hrh"
// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CGSCamDefaultNameSettingItem::CGSCamDefaultNameSettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamDefaultNameSettingItem::CGSCamDefaultNameSettingItem( 
    TDes&          aNameBase,
    TCamCameraMode aMode,
    TInt           aIdentifier,
    TInt&          aValue,
    TBool          aCamera )
: CAknEnumeratedTextPopupSettingItem( aIdentifier, aValue ),
  iNameBase( aNameBase ),
  iMode( aMode ),
  iCamera ( aCamera )
    {
    }


// ---------------------------------------------------------------------------
// CGSCamDefaultNameSettingItem::~CGSCamDefaultNameSettingItem
// Destructor.
// ---------------------------------------------------------------------------
//
CGSCamDefaultNameSettingItem::~CGSCamDefaultNameSettingItem()
    {
    }


// ---------------------------------------------------------------------------
// CGSCamDefaultNameSettingItem::CreateSettingPageL
// Framework method for constructing the launching setting page.
// ---------------------------------------------------------------------------
//
CAknSettingPage* CGSCamDefaultNameSettingItem::CreateSettingPageL()
    {
    // Create an instance of our customized setting page.
    TPtrC settingName = SettingName();
    return new( ELeave )CGSCamDefaultNameSettingPage(
        iNameBase,
        iMode,
        &settingName,
        SettingNumber(), 
        EAknCtPopupSettingList,
        SettingEditorResourceId(),
        SettingPageResourceId(),
        *QueryValue(), iCamera );
    }
