/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manages all static settings data.
*
*/



#ifndef CAMSTATICSETTINGSMODEL_H
#define CAMSTATICSETTINGSMODEL_H

// INCLUDES
  // Internal build flags, keep first.

#include <eikenv.h>
#include <centralrepository.h>
#include "Cam.hrh"
#include "mcamsettingsmodel.h"
#include "MCamStaticSettings.h"
#include "CamSettingsInternal.hrh"

// FORWARD DECLARATIONS

class CCamConfiguration;
class CCameraUiConfigManager;

/**
* Manages all static settings data.
* @since 2.8
*/
class CCamStaticSettingsModel : public CBase,
                                public MCamStaticSettings
  {
  // =========================================================================
  public: // Constructors and destructor
  
    /**
    * Symbian two-phased constructor.
    * @since 2.8
    * @param aConfiguration    
    * engine changes.
    */
    static CCamStaticSettingsModel* NewLC( CCamConfiguration& aConfiguration );
  
    /**
    * Symbian two-phased constructor.
    * @since 2.8
    * @param aConfiguration    
    * engine changes.
    */
    static CCamStaticSettingsModel* NewL( CCamConfiguration& aConfiguration );
  
    /**
    * Destructor.
    */
    ~CCamStaticSettingsModel();

  public: // New Fuctions

    /**
    * Returns the current integer value for the specified setting
    * @return the current integer setting value
    * @param aSettingItem specifies which setting item that want the value of.
    * @since 2.8
    */
    TInt IntegerSettingValue( TInt aSettingItem ) const;
    
    /**
    * Sets a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want to set the value of.
    * @param aSettingValue the new integer value for the specified setting item.
    * @since 2.8
    */
    void SetIntegerSettingValueL( TInt aSettingItem, TInt aSettingValue );

    /**
    * Sets a new text value for the specified setting
    * @param aSettingItem specifies which setting item that want to set the text value of.
    * @param aSettingValue the new text value for the specified setting item.
    * @since 2.8
    */
    void SetTextSettingValueL( TInt aSettingItem,
                               const TDesC& aSettingValue );
    
    /**
    * Returns the current text value for the specified setting
    * @return the current text setting value
    * @param aSettingItem specifies which setting item that want the text value of.
    * @since 2.8
    */
    TPtrC TextSettingValue( TInt aSettingItem ) const; 

#ifndef CAMERAAPP_PLUGIN_BUILD
    /**
    * Loads the static settings from shared data. Required to update
    * the settings whenever get foreground event, in case of external
    * changes to the settings.
    * @since 2.8
    * @param aIsEmbedded Specifies whether the application is embedded
    * or not.
    */       
    void LoadStaticSettingsL( TBool aIsEmbedded );

#endif //CAMERAAPP_PLUGIN_BUILD

    /**
    * Reads the specified key from the Central Repository, returns the 
    * content in HBufC
    *
    * @since 5.0
    * @param aCenRepKeyId   The Central Repository key id to read
    */ 
    HBufC* ReadCenRepStringL( TInt aCenRepKeyId );
    /*
    * Reset repository file
    * @since3.0
    */
    void ResetRepository();
    
    /**
    * Loads the image/common static settings from shared data. Required to update
    * the settings whenever get foreground event, in case of external
    * changes to the settings.
    */       
    void LoadPhotoStaticSettingsL( const TBool aResetFromPlugin );
    
    /**
    * Loads the video/common static settings from shared data. Required to update
    * the settings whenever get foreground event, in case of external
    * changes to the settings.
    */       
    void LoadVideoStaticSettingsL( const TBool aResetFromPlugin );
    
  // New methods
  public:
    
    /**
    * Reads the specified setting from the Central Repository, and 
    * stores the new setting object in the array.
    * @since 3.0
    * @param TCamSettingItemIds the settings range for static settings from CamSettingsInternals.hrh
    * @param TCamSettingItemIds the settings range for static settings from CamSettingsInternals.hrh
    * @param aArray The array to add the setting to
    */
    void ReadFromCenRepL( const TInt aMinRange, 
                          const TInt aMaxRange,
                          RPointerArray <TIntSetting>& aArray );    
    
    /**
    * Reads the specified setting from the Central Repository, and 
    * stores the new setting object in the array.
    * @since 3.0
    * @param aUiId The Settings Model ID to be loaded to the array
    * @param aCenRepID The Central Repository setting ID to read
    * @param aArray The array to add the setting to
    */ 
    void ReadCenRepIntL( TCamSettingItemIds aUiId, 
                         TInt aCenRepID, 
                         RPointerArray <TIntSetting>& aArray );
                         
                         
    /**
    * Saves the setting item from the array to the Central Repository.
    * May leave with KErrNotFound if the item id 'aItemId' is not found
    * in the array 'aArray'.
    * @since 3.0
    * @param aItemId The Settings Model ID to be saved 
    * @param aCenRepID The Central Repository setting ID to saved to
    * @param aArray The array to read the setting from
    */ 
    void SaveCenRepItemL( TCamSettingItemIds aItemId,
                          TInt aCenRepId,
                          const RPointerArray<TIntSetting>& aArray );

    /**
    * Saves one static setting to Central Repository.
    * @param aSettingId Id of the setting to be saved. 
    * @since Camera 4.0
    */
    void SaveStaticSettingL( TCamSettingItemIds aSettingId );
    
    /**
    * Stores the primary camera settings when changing to secondary camera
    * that they can be reapplied when changing back
    * @since 2.8
    */
    void StorePrimaryCameraSettingsL();

    /**
    * Restores the primary camera settings
    * @since 2.8
    */
    void RestorePrimaryCameraSettingsL();

    /*
    * Resets a particular setting item value
    */
    void ResetSettingItem( const TInt aSettingItem );
    
    /**
    * Get the associated Central Repository Key for given setting item.
    * @param aSettingId Id of the setting for which the key is requested.
    * @param aEmbedded  Is request made for embedded mode setting or not.
    * @since Camera 4.0
    */
    static TUint32 MapSettingItem2CRKey( TCamSettingItemIds aSettingId,
                                  TBool              aEmbedded );

    /**
    * Get the associated settings list for given setting item.
    * @param aSettingId Id of the setting for which the list is requested.
    * @since Camera 4.0
    */
    RPointerArray<TIntSetting>*
      MapSettingItem2SettingsList( TCamSettingItemIds aSettingId );

    /**
    * Removes any previously loaded static settings.
    * @since 2.8
    */
    void UnloadStaticSettings();
    
    /**
    * Loads a list of dynamic settings from the resource file 
    * @param aResourceId the id of the resource that contains the
    * settings items id's and their default values.
    * @param aSettingsList the list to populate with dynamic settings
    * from the resource file.
    * @since 2.8
    */
    void LoadSettingsFromResourceL( TInt                        aResourceId, 
                                    RPointerArray<TIntSetting>& aSettingsList );

    /** 
    * Returns reference to CCamConfiguration object describing
    * the device's camera configuration.
    * @return configuration
    */
    CCamConfiguration& Configuration() const;
    
    /*
    *  Handle to Camera Ui Config Manager
    */
   CCameraUiConfigManager* UiConfigManagerPtr();

  private:
    
    //
    // CCamStaticSettingsModel::LoadCommonStaticSettingsL
    //
    void LoadCommonStaticSettingsL( const TBool aResetFromPlugin );
    
    /*
    * Searches for a setting item in the list of available list of items
    */
    TInt SearchInSettingsListFor(
                                const RPointerArray<TIntSetting>& aSettingsList,
                                TInt aSettingItem ) const;

    /**
    * Saves the static settings to shared data.
    * @since 2.8
    */       
    void SaveSettingsL();
    
    /*
    * Load embedded camera specific settings
    */
    void LoadEmbeddedSettingsL();

  // Constructors
  private:

    /**
    * C++ constructor.
    * @since 2.8
    * @param aConfiguration    
    */
    CCamStaticSettingsModel( CCamConfiguration& aConfiguration );            

    /**
    * C++ 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();

    /**
    * Loads photo base name
    * @since 2.8
    */
    void LoadPhotoBaseNameL();

    /**
    * Loads video base name
    * @since 2.8
    */
    void LoadVideoBaseNameL();

   /**
    * Saves photo base name
    * @since 2.8
    */
    void SavePhotoBaseNameL();

    /**
    * Saves video base name
    * @since 2.8
    */
    void SaveVideoBaseNameL();
    
    /**
    * Saves name of the DefaultAlbum
    */
    void SaveDefaultAlbumNameL();
    
    /**
    * Loads name of the DefaultAlbum
    */
    void LoadDefaultAlbumNameL();
    
  private: // data

    /**
    * Stores information for front/back camera settings
    * @since 2.8
    */
    class TCamCameraSettings
      {
      public:
        TInt    iPhotoQuality;
        TInt    iVideoQuality;
      };

    // Commom static settings.
    RPointerArray<TIntSetting> iStaticCommonIntSettings;

    // List of setting item id/value pairs for static photo settings.
    RPointerArray<TIntSetting> iStaticPhotoIntSettings;
    
    // List of setting item id/value pairs for static video settings.
    RPointerArray<TIntSetting> iStaticVideoIntSettings;
    
    // Video file base name.
    TBuf<KMaxNameBaseLength> iVideoBaseName;
    
    // Photo file base name.
    TBuf<KMaxNameBaseLength> iPhotoBaseName;
    
    TBuf<KMaxAlbumNameLength> iDefaultAlbumName;

    // Specifies whether this application is embedded or not.
    TBool iEmbedded;
    
    // Handle to actual setting storage object
    CRepository* iRepository;                
    
    // The cached primary camera settings
    TCamCameraSettings iPrimaryCameraSettings;
    // The cached secondary camera settings
    TCamCameraSettings iSecondaryCameraSettings;
    CCamConfiguration& iConfiguration;
    CCameraUiConfigManager* iConfigManager;
  };

#endif // CAMSETTINGSMODEL_H

// End of File
