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
* Description:  Defines abstract interface observing add to album operation completion*
*/



#ifndef MCAMSTATICSETTINGS_H
#define MCAMSTATICSETTING_H

class CCamConfiguration;

/**
*  Interface for static settings model.
*/
class MCamStaticSettings
  {
  public:

    /**
    * Returns the current integer value for the specified setting
    * @return the current integer setting value
    * @param aSettingItem specifies which setting item that want the value of.
    * @since 2.8
    */
    virtual TInt IntegerSettingValue( TInt aSettingItem ) const = 0;
    
    /**
    * Sets a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want to set the value of.
    * @param aSettingValue the new integer value for the specified setting item.
    * @since 2.8
    */
    virtual void SetIntegerSettingValueL( TInt aSettingItem, TInt aSettingValue ) = 0;
    
    /**
    * Returns the current text value for the specified setting
    * @return the current text setting value
    * @param aSettingItem specifies which setting item that want the text value of.
    * @since 2.8
    */
    virtual TPtrC TextSettingValue( TInt aSettingItem ) const = 0;
    
    /**
    * Sets a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want to set the value of.
    * @param aSettingValue the new integer value for the specified setting item.
    * @since 2.8
    */
    virtual void SetTextSettingValueL(       TInt aSettingItem, 
                                       const TDesC& aSettingValue ) = 0; 
    
    /**
    * Loads the image/common static settings from shared data. Required to update
    * the settings whenever get foreground event, in case of external
    * changes to the settings.
    * @since 2.8
    */       
    virtual void LoadPhotoStaticSettingsL( const TBool aResetFromPlugin ) = 0;
    
    /**
    * Loads the video/common static settings from shared data. Required to update
    * the settings whenever get foreground event, in case of external
    * changes to the settings.
    */       
    virtual void LoadVideoStaticSettingsL( const TBool aResetFromPlugin ) = 0;
    
    /** 
    * Returns reference to CCamConfiguration object describing
    * the device's camera configuration.
    * @return configuration
    */
    virtual CCamConfiguration& Configuration() const = 0;
    };

#endif // MCAMSTATICSETTINGS_H
// End of File
