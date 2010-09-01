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
* Description:  Setting item for capture tone
*/


// INCLUDE FILES
#include <centralrepository.h>
#include "GSCamCaptureToneSettingItem.h"
#include "camuidconstants.h"
#include "CamSettings.hrh"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CGSCamCaptureToneSettingItem::CGSCamCaptureToneSettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamCaptureToneSettingItem::CGSCamCaptureToneSettingItem(TInt aIdentifier,
        TInt& aValue) :
    CAknEnumeratedTextPopupSettingItem(aIdentifier, aValue)
    {

    }

// ---------------------------------------------------------------------------
// CGSCamCaptureToneSettingItem::~CGSCamCaptureToneSettingItem
// Destructor.
// ---------------------------------------------------------------------------
//
CGSCamCaptureToneSettingItem::~CGSCamCaptureToneSettingItem()
    {
    }

// ---------------------------------------------------------------------------
// CGSCamCaptureToneSettingItem::CGSCamCaptureToneSettingItem
// 
// ---------------------------------------------------------------------------
//
void CGSCamCaptureToneSettingItem::CompleteConstructionL()
    {
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();

    // get the KCCorEnableCaptureToneOff key value from cenrep
    CRepository* repository = NULL;
    TInt shutterSoundEnabled = 0; //by default enabled
    TRAPD(ret, repository = CRepository::NewL(KCRUidCamcorderFeatures));
    if (ret == KErrNone)
        {
        const TUint32 KCCorShutterSound = 0x00000007;
        ret = repository->Get(KCCorShutterSound,
                shutterSoundEnabled);
        }
    delete repository;
    repository = NULL;

    if ( shutterSoundEnabled == 1 )
        {
        // if enabled, delete off from the list
        // if disabled, off option is shown to the user
        // under capture tone selection menu
        CArrayPtr<CAknEnumeratedText>
                * itemArray =
                        static_cast<CAknEnumeratedTextPopupSettingItem*>(this)->EnumeratedTextArray();

        TInt count = itemArray->Count();
        
        for(TInt idx = 0; idx < count; idx++)
            {
            TInt value = itemArray->At( idx )->EnumerationValue();
            if(value == ECamSettToneOff)
                {
                itemArray->Delete(idx);
                }
            }
        }
    }
