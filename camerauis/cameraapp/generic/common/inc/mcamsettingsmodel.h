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
* Description:  Interface to object that manages all settings data.
*
*/



#ifndef MCAMSETTINGSMODEL_H
#define MCAMSETTINGSMODEL_H

// ===========================================================================
// INCLUDES
#include <aknview.h>
 
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"

// ===========================================================================
// CONSTANTS

// ===========================================================================
// FORWARD DECLARATIONS
class CCamVideoQualityLevel;
class MCamSettingsModelObserver;


// ===========================================================================
// STRUCTURES
class TIntSetting
  {
  public:
    TInt iItemId;
    TInt iValueId;
  };

class TSceneData 
  {
  public: // Destructor
    ~TSceneData ();
  
  public: // Data
    TInt iSceneId;
    RPointerArray<TIntSetting> iSettings;
  };

class TCamVariantInfo
  {
  public:
    TCamVariantInfo();

  public:
    
    TInt            iFlags;
    TCamSupportFlag iTimeLapseSupport;
    TCamSupportFlag iAlwaysOnSupport;
    TVersion        iVersion;
  };

// ===========================================================================
// CLASS DECLARATION

/**
* Interface to object that manages all settings data.
* @since 2.8
*/
class MCamSettingsModel
  {
  public: // Interface

    /**
    * This needs to be here to be able to greacefully delete
    * an object of inherited class through mixin pointer.
    * If this is not defined, deleting through the mixin pointer
    * results in User-42 panic!
    */
    virtual ~MCamSettingsModel() {};
    
    /**
    * Add a settings model observer.
    * Observers will be notified of settings changes.
    */
    virtual void AttachObserverL( const MCamSettingsModelObserver* aObserver ) = 0;

    /**
    * Remove a settings model observer.
    */
    virtual void DetachObserver( const MCamSettingsModelObserver* aObserver ) = 0;
   
    /**
    * Resets all user scene settings to their defaults.
    * @since 2.8
    */
    virtual void ResetUserSceneL() = 0;
        
    /**
    * Previews a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want to preview the value of.
    * @param aSettingValue the new integer value for the specified setting item to be previewed.
    * @since 2.8
    */
//    virtual void PreviewSettingChangeL( TInt aSettingItem, TInt aSettingValue ) = 0;
    
    /**
    * Cancels all preview changes, since last commit/cancel.
    * @since 2.8
    */
//    virtual void CancelPreviewChangesL() = 0;
    
    /**
    * Commits last preview change.
    * @since 2.8
    */
//    virtual void CommitPreviewChanges() = 0;
    
    /**
    * Returns the current integer value for the specified setting
    * @return the current integer setting value
    * @param aSettingItem specifies which setting item that want the value of.
    * @since 2.8
    */
    virtual TInt IntegerSettingValue( TInt aSettingItem ) const = 0; 
    
    /**
    * Returns the setting value for a specific field of a scene 
    * @return the setting value
    * @param aSceneId specifies which scene.
    * @param aSceneId specifies which field of the scene.
    * @since 2.8
    */
    virtual TInt SceneSettingValue( TInt aSceneId, TInt aSceneField ) const = 0; 

    /**
    * Sets a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want to set the value of.
    * @param aSettingValue the new integer value for the specified setting item.
    * @since 2.8
    */
    virtual void SetIntegerSettingValueL( TInt aSettingItem, TInt aSettingValue ) = 0; 

    /**
    * Sets a new text value for the specified setting
    * @param aSettingItem specifies which setting item that want to set the text value of.
    * @param aSettingValue the new text value for the specified setting item.
    * @since 2.8
    */
    virtual void SetTextSettingValueL(       TInt aSettingItem, 
                                       const TDesC& aSettingValue ) = 0; 
    
    /**
    * Returns the current text value for the specified setting
    * @return the current text setting value
    * @param aSettingItem specifies which setting item that want the text value of.
    * @since 2.8
    */
    virtual TPtrC TextSettingValue( TInt aSettingItem ) const = 0; 

    /**
    * Returns whether a particular setting value can be set or not.
    * @return ETrue if can be set. Otherwise, EFalse.
    * @param aSettingValue the value in question. 
    * @param aSettingItem the setting item in question. 
    * @since 2.8
    */
    virtual TBool SettingValueEnabled( TInt aSettingItem, 
                                       TInt aSettingValue ) const = 0;

    /**
    * Get the default value for a setting item.
    * @param  aSettingId The id of the setting.
    * @return The default value for setting item or KErrNotFound if not found.
    */
    virtual TInt DynamicSettingDefaultValue( TInt aSettingId ) const = 0;
    
    /**
    * Sets all dynamic settings to their defaults.
    * @since 2.8
    */       
    virtual void SetDynamicSettingsToDefaults() = 0;
    
    /**
    * Sets a single dynamic setting to its default.
    * @since 2.8
    */       
    virtual void SetDynamicSettingToDefault(TInt aSettingItem) = 0;

    /**
    * Sets a single dynamic setting to scene default value.
    * @param  aSceneId    Scene which the default value is taken from.
    * @param  aSceneField Id of the setting in scene data.
    * @param  aSettingId  Id of the setting in dynamic settings.
    * @param  aVideoScene Is the scene a video scene.
    * @return KErrNotFound, if setting could not be set.
    */
    virtual TInt SetDynamicSettingToSceneDefaultL( TInt  aSceneId,
                                                   TInt  aSceneField,
                                                   TInt  aSettingId,
                                                   TBool aVideoScene ) = 0;

    /**
    * Loads the static settings from shared data. Required to update
    * the settings whenever get foreground event, in case of external
    * changes to the settings.
    * @since 2.8
    * @param aIsEmbedded Specifies whether the application is embedded
    * or not.
    */       
    virtual void LoadStaticSettingsL( TBool aIsEmbedded ) = 0;

    /**
    * Return handle to video quality level array
    * @since 2.8
    * @return reference to array of video quality levels
    */
    virtual const RPointerArray<CCamVideoQualityLevel>& VideoQualityArray() = 0;

    /**
    * Return handle to video resolution array
    * @since 2.8
    * @param return reference to array of video resolutions
    */
    virtual const RArray<TSize>& VideoResolutions() = 0;

    /** 
    * Returns the image resolution for a given resolution index.
    * @since 2.8
    * @param aIndex The index to get the resolution for
    * @returns The X and Y dimensions in pixels of the resolution.
    */
    virtual TSize ImageResolutionFromIndex( TCamPhotoSizeId aIndex ) = 0;

    /**
    * Returns the current photo quality.
    * @return the quality.
    * @since 2.8
    */
    virtual TInt CurrentPhotoCompression() const = 0;

    /**
    * Returns the current photo resolution from Settings
    * @return the resolution.
    * @since 2.8
    */
    virtual TCamPhotoSizeId CurrentPhotoResolution() const = 0;

    /**
    * Returns the photo resolution for a particular quality
    * @return the resolution.
    * @param aQualityIndex The index of the quality
    * @since 3.0
    */
    virtual TCamPhotoSizeId PhotoResolution( TInt aQualityIndex ) const = 0;

    /**
    * Stores the primary camera settings when changing to secondary camera
    * that they can be reapplied when changing back
    * @since 2.8
    */
    virtual void StorePrimaryCameraSettingsL() = 0;

    /**
    * Restores the primary camera settings 
    * @since 2.8
    */
    virtual void RestorePrimaryCameraSettingsL() = 0;

    /**
    * Return variant info.
    */
    virtual const TCamVariantInfo& VariantInfo() = 0;

    /*
    * Reset repository file
    * @since3.0
    */
    virtual void ResetRepository() = 0;
    
	/**
    * Store FT user defined setting
    * @since 5.0
    */
    virtual void StoreFaceTrackingValue() = 0;
    
    /**
    * Returns the face tracking state as it was before the latest scene mode change
    */     
    virtual TCamSettingsOnOff GetPreviousFaceTrack() = 0;
    
    /**
    * Sets the face tracking state as it was before the latest scene mode change
    */       
    virtual void SetPreviousFaceTrack( TCamSettingsOnOff aPreviousFaceTrack ) = 0;
  
    /**
    * Returns the scene mode that was in use before the current scene mode was selected
    */       
    virtual TCamSceneId GetPreviousSceneMode() = 0;
    /**
    * Sets the scene mode that was in use before the current scene mode was selected
    */         
    virtual void SetPreviousSceneMode( TCamSceneId aPreviousSceneMode ) = 0;
    
    /**
    * Store UserScene settings
    * @since 5.1
    */
    virtual void StoreUserSceneSettingsL() = 0;
  };
// ===========================================================================

#endif // MCAMSETTINGSMODEL_H

// End of File
