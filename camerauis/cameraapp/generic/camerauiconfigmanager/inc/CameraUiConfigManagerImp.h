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
* Description:  Helper class which handles reading and storing the cr keys 
 *                and its values.
 *
*/


#ifndef CAMERAUICONFIGMANAGERIMP_H
#define CAMERAUICONFIGMANAGERIMP_H

// INCLUDES

#include <eikenv.h>
#include "CameraUiConfigManager.h"
#include "CameraDynamicSettings.hrh"

// FORWARD DECLARATIONS
class TDynamicConfigSettingsData;
class CRepository;


// Class declaration
class TDynamicSettingsData
    {
    public:
        TBuf<256> iText;
        TInt iItemId; // enum value converted to TInt
    };


class TSupportedSettingsData
    {
    public:
        TInt    iSettingId;
        TInt    iSupportedValue;
        RArray<TInt> iSupportedSettingItems;
    };

/**
* Helper class which handles the Cr keys
*/
class CCameraUiConfigManagerImp : public CCameraUiConfigManager
  {
  // =========================================================================
  public: // Constructors and destructor
  
        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aConfiguration    
        * engine changes.
        */
        static CCameraUiConfigManagerImp* NewLC();
      
        /**
        * Symbian two-phased constructor.
        * @since 2.8
        * @param aConfiguration    
        * engine changes.
        */
        static CCameraUiConfigManagerImp* NewL();
      
        /**
        * Destructor.
        */
        ~CCameraUiConfigManagerImp();
    
 public:
        
        /*
        * To check if a particular feature is supported and/or in somecase
        * used to return the configured setting for e.g. camera capture key
        */
        TInt IsFeatureSupported( const TCamDynamicSettings aSettingId ) const;
        
        /*
        *  To get all supported color tone setting items
        */
        void SupportedSettingItemsL( const TCamDynamicSettings aSettingId,
                                     RArray<TInt>& aSupportedValues );
        
        /*
        * Organizing CenRep data to a definite structure for scene modes.
        */
        void OrganiseSceneSettings( RArray<TSceneSettings>& aTargetArray,
                                    RArray<TInt>& aSceneSettingsArray,
                                    const TBool aPhotoScene );
        
 private:
        
        /*
        * To get the setting item id, based on string comparison
        */
        TInt SettingItemId( RArray<TDynamicSettingsData>& aPossibleSettings,
                            const TDesC& aItemString ) const;

        /*
        * Gets the string value from Cr key
        */
        HBufC* GetStringDataL( const TUint32 aCrKey );
        
        /*
        * Gets all supported items by mapping the item id's based on the configured
        * items in the cr key
        */
        void GetSupportedSettingIdsL( RArray<TDynamicSettingsData>& aAllItems,
                                      RArray<TInt>& aSupportedValues, 
                                      const TUint32 crKey );
        
        /*
        * Load all default setting items supported from the UI defined in the resource
        */
        void LoadDefaultSettingsDataL( const TInt aSettingItem,
                                       RArray<TDynamicSettingsData>& aDefaultItems );
        
        /*
        * Load all the dynamic setting values from cenrep
        */
        void LoadAllDynamicSettingsL();
        
        /*
        * Append all the dynamic settings to the array
        */
        void AppendToSettingsArrayL( const TInt aItemId,
                                     const TInt aFeatureSupported,
                                     RArray<TDynamicSettingsData>& aAllItems,
                                     TUint32 aCrItemsKey = 0,
                                     TBool loadSettingItems = 0 );
        
        /*
        * Search for setting item dynamic settings array
        */                             
        TInt SearchInSettingsListFor( 
                        const RPointerArray<TSupportedSettingsData>& aSettingsList, 
                        TInt                        aSettingItem  ) const;
        
        /*
        * Map setting item to corresponding "Feature" Cr key 
        */
        TUint32 MapSettingItemToCrFeatureKeyL( const TInt aSettingItem ) const;
        
        
        /*
        * Map setting item to configured setting "values" Cr key
        */
        TUint32 MapSettingItemToCrItemsKeyL( const TInt aSettingItem ) const;
        
        /*
        * Load all configured settings from CenRep
        */
        void LoadConfiguredSettingItemValuesFromCrL(
                               const TInt aSettingId,
                               const TInt aFeatureSupported,
                               RArray<TDynamicSettingsData>& aSettingItemDefaultData );
        
        /*
        * LoadCrForFeatureSupport
        */
        TBool LoadCrForFeatureSupport( const TInt aSettingId ) const;
        
        /*
        * Load corresponding setting item enumeration array
        */
        const TInt* LoadSettingEnumerationArray( const TInt aSettingItem );
        
        /*
        * Load corresponding setting item string array
        */
        const TUint16* const* LoadSettingStringArray( const TInt aSettingItem );
      
 private:

    /**
     * Actual constructor where the reading from the Central Repository
     * happens
     */
    void ConstructL();
          
 private:
    // we own
    CRepository* iRepository;
    RPointerArray<TSupportedSettingsData> iSupportedSettings;
};

#endif // CameraUiConfigManagerImp_H

// End of File
