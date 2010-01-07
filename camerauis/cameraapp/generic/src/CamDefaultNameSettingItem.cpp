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
#include "CamDefaultNameSettingItem.h"
#include "Camdefaultnamesettingpage.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CCamDefaultNameSettingItem::CCamDefaultNameSettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamDefaultNameSettingItem::CCamDefaultNameSettingItem( 
    TDes&          aNameBase,
    TCamCameraMode aMode,
    TInt           aIdentifier,
    TInt&          aValue )
: CAknEnumeratedTextPopupSettingItem( aIdentifier, aValue ),
  iNameBase( aNameBase ),
  iMode( aMode )
    {
    }


// ---------------------------------------------------------------------------
// CCamDefaultNameSettingItem::~CCamDefaultNameSettingItem
// Destructor.
// ---------------------------------------------------------------------------
//
CCamDefaultNameSettingItem::~CCamDefaultNameSettingItem()
    {
    }


// ---------------------------------------------------------------------------
// CCamDefaultNameSettingItem::CreateSettingPageL
// Framework method for constructing the launching setting page.
// ---------------------------------------------------------------------------
//
CAknSettingPage* CCamDefaultNameSettingItem::CreateSettingPageL()
    {
    // Create an instance of our customized setting page.

    TPtrC settingName = SettingName();
    return new( ELeave )CCamDefaultNameSettingPage(
        iNameBase,
        iMode,
        &settingName,
        SettingNumber(), 
        EAknCtPopupSettingList,
        SettingEditorResourceId(),
        SettingPageResourceId(),
        *QueryValue() );
    }
