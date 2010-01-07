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
* Description:  Defines abstract interface for CCamSettingsModel observers*
*/



#ifndef CAMSETTINGSMODELOBSERVER_H
#define CAMSETTINGSMODELOBSERVER_H


//  INCLUDES

// #include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CCamVideoQualityLevel;
// CLASS DECLARATION

/**
* Abstract API for settings model observer. 
* Derived classes may register as observers to get related
* notifications about events from CCamSettingsModel.
*
*  @since 2.8
*/
class MCamSettingsModelObserver
  {
  public:

    /**
    * Notification for change in integer setting item value.
    *
    */
    virtual void IntSettingChangedL( TCamSettingItemIds aSettingItem, TInt aSettingValue ) = 0;

    /**
    * Notification for change in text setting item value.
    *
    */
    virtual void TextSettingChangedL( TCamSettingItemIds aSettingItem, const TDesC& aSettingValue ) = 0;

  };

#endif      // CAMSETTINGSMODELOBSERVER_H
            
// End of File
