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
* Description:  Manages all settings data.
*
*/



#ifndef CAMSETTINGSMODEL_H
#define CAMSETTINGSMODEL_H

// INCLUDES
 

#include <eikenv.h>

#include "Cam.hrh"
#include "mcamsettingsmodel.h"
#include "CamBurstModeObserver.h"
#include "mcamsettingpreviewhandler.h"

#include "camconfigurationconstants.h"

// CONSTANTS

// FORWARD DECLARATIONS
class MCamSettingsModelObserver;
class CCamVideoQualityLevel;
class CCamStaticSettingsModel;
class MCamStaticSettings;
class CCamConfiguration;
class CCameraUiConfigManager;

// STRUCTS

// CLASS DECLARATIONS

/**
* Manages all static settings data.
* @since 2.8
*/
class CCamSettingsModel : public CBase,
                          public MCamSettingsModel,
                          public MCamSettingPreviewHandler,
       /** to be removed*/public MCamBurstModeObserver
  {
  // =========================================================================
  public: // Constructors and destructor
  
    /**
    * Symbian two-phased constructor.
    * @param aConfiguration
    * @since 2.8
    * engine changes.
    */
    static CCamSettingsModel* NewLC( CCamConfiguration& aConfiguration );
  
    /**
    * Symbian two-phased constructor.
    * @param aConfiguration
    * @since 2.8
    * engine changes.
    */
    static CCamSettingsModel* NewL( CCamConfiguration& aConfiguration );
  
  
    /**
    * Destructor.
    */
    ~CCamSettingsModel();
    
  public:
   /** 
    * Saves user defined FT setting
    * @since S60 5.0
    * @param None
    * @return None
    */
    void StoreFaceTrackingValue(); 
    
    
    /**
    * Returns the face tracking state as it was before the latest scene mode change
    */  
    TCamSettingsOnOff GetPreviousFaceTrack();
    
    /**
    * Sets the face tracking state as it was before the latest scene mode change
    */  
    void SetPreviousFaceTrack( TCamSettingsOnOff aPreviousFaceTrack );
    
    /**
    * Returns the scene mode that was in use before the current scene mode was selected
    */ 
    TCamSceneId GetPreviousSceneMode();

    /**
    * Sets the scene mode that was in use before the current scene mode was selected
    */  
    void SetPreviousSceneMode( TCamSceneId aPreviousSceneMode );
    

  // -------------------------------------------------------------------------
  // From base class MCamSettingsModel
  public:

    /**
    * Add a settings model observer.
    * Observers will be notified of settings changes.
    */
    virtual void AttachObserverL( const MCamSettingsModelObserver* aObserver );

    /**
    * Remove a settings model observer.
    */
    virtual void DetachObserver( const MCamSettingsModelObserver* aObserver );

    /**
    * Resets all user scene settings to their defaults.
    * @since 2.8
    */
    virtual void ResetUserSceneL();
    
    /**
    * Returns the current integer value for the specified setting
    * @return the current integer setting value
    * @param aSettingItem specifies which setting item that want the value of.
    * @since 2.8
    */
    virtual TInt IntegerSettingValue( TInt aSettingItem ) const; 
    
    /**
    * Returns the setting value for a specific field of a scene 
    * @return the setting value
    * @param aSceneId specifies which scene.
    * @param aSceneId specifies which field of the scene.
    * @since 2.8
    */
    virtual TInt SceneSettingValue( TInt aSceneId, TInt aSceneField ) const; 

    /**
    * Sets a new value for the specified integer setting
    * @param aSettingItem specifies which setting item that want to set the value of.
    * @param aSettingValue the new integer value for the specified setting item.
    * @since 2.8
    */
    virtual void SetIntegerSettingValueL( TInt aSettingItem, TInt aSettingValue ); 

    /**
    * Sets a new text value for the specified setting
    * @param aSettingItem specifies which setting item that want to set the text value of.
    * @param aSettingValue the new text value for the specified setting item.
    * @since 2.8
    */
    virtual void SetTextSettingValueL(       TInt aSettingItem, 
                                       const TDesC& aSettingValue ); 
    
    /**
    * Returns the current text value for the specified setting
    * @return the current text setting value
    * @param aSettingItem specifies which setting item that want the text value of.
    * @since 2.8
    */
    virtual TPtrC TextSettingValue( TInt aSettingItem ) const; 

    /**
    * Returns whether a particular setting value can be set or not.
    * @return ETrue if can be set. Otherwise, EFalse.
    * @param aSettingValue the value in question. 
    * @param aSettingItem the setting item in question. 
    * @since 2.8
    */
    virtual TBool SettingValueEnabled( TInt aSettingItem, 
                                       TInt aSettingValue ) const;

    /**
    * Get the default value for a setting item.
    * @param  aSettingId The id of the setting.
    * @return The default value for setting item or KErrNotFound if not found.
    */
    virtual TInt DynamicSettingDefaultValue( TInt aSettingId ) const;
    
    /**
    * Sets all dynamic settings to their defaults.
    * @since 2.8
    */       
    virtual void SetDynamicSettingsToDefaults();
    
    /**
    * Sets a single dynamic setting to its default.
    * @since 2.8
    */       
    virtual void SetDynamicSettingToDefault(TInt aSettingItem);

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
                                                   TBool aVideoScene );


    /**
    * Loads the static settings from shared data. Required to update
    * the settings whenever get foreground event, in case of external
    * changes to the settings.
    * @since 2.8
    * @param aIsEmbedded Specifies whether the application is embedded
    * or not.
    */       
    virtual void LoadStaticSettingsL( TBool aIsEmbedded );

    /**
    * Return handle to video quality level array
    * @since 2.8
    * @return reference to array of video quality levels
    */
    virtual const RPointerArray<CCamVideoQualityLevel>& VideoQualityArray();

    /**
    * Return handle to video resolution array
    * @since 2.8
    * @param return reference to array of video resolutions
    */
    virtual const RArray<TSize>& VideoResolutions();

    /** 
    * Returns the image resolution for a given resolution index.
    * @since 2.8
    * @param aIndex The index to get the resolution for
    * @returns The X and Y dimensions in pixels of the resolution.
    */
    virtual TSize ImageResolutionFromIndex( TCamPhotoSizeId aIndex );

    /**
    * Returns the current photo quality.
    * @return the quality.
    * @since 2.8
    */
    virtual TInt CurrentPhotoCompression() const;

    /**
    * Returns the current photo resolution from Settings
    * @return the resolution.
    * @since 2.8
    */
    virtual TCamPhotoSizeId CurrentPhotoResolution() const;

    /**
    * Returns the photo resolution for a particular quality
    * @return the resolution.
    * @param aQualityIndex The index of the quality
    * @since 3.0
    */
    virtual TCamPhotoSizeId PhotoResolution( TInt aQualityIndex ) const;

    /**
    * Stores the primary camera settings when changing to secondary camera
    * that they can be reapplied when changing back
    * @since 2.8
    */
    virtual void StorePrimaryCameraSettingsL();

    /**
    * Restores the primary camera settings 
    * @since 2.8
    */
    virtual void RestorePrimaryCameraSettingsL();

    /**
    * Return variant info.
    */
    virtual const TCamVariantInfo& VariantInfo();

    /*
    * Reset repository file
    * @since3.0
    */
    virtual void ResetRepository();
    
  // -------------------------------------------------------------------------
  // From base class MCamSettingPreviewHandler
  public:

    /**
    * Activates preview for one setting.
    * Returns the preview value as setting value in ProvideCameraSettingL
    * until Commit or Cancel is called.
    * @param aSettingId    The setting for which preview is set
    * @param aSettingValue The setting value used for preview
    * @leave Any system error code.
    */
    virtual void ActivatePreviewL( const TCamSettingItemIds& aSettingId,
                                   TInt                      aSettingValue );

    /**
    * Cancels the preview for one setting. 
    * After this call ProvideCameraSettingL returns the actual setting value.
    * If the setting does not have active preview, such error is ignored.
    * @param aSettingId Id of the setting for which the preview is cancelled.
    */
    virtual void CancelPreview( const TCamSettingItemIds& aSettingId );
    

    /**
    * Cancels the preview for a set of settings.
    * If some of the settings do not have active preview, such error is ignored.
    */
    virtual void CancelPreviews( const RArray<TCamSettingItemIds> aSettingIds );
    
    /**
    * Cancels all previews.
    * No error reported, if no previews active.
    */
    virtual void CancelAllPreviews();
    
    /**
    * Commits the preview value for the given set of settings.
    * After this call completes, the preview is no more active for the setting.
    * If the call completes successfully, the preview is stored as the
    * actual setting value. If leave occurs, the preview is not stored.
    * @param aSettingId Id of the setting for which the preview is committed.
    * @leave KErrArgument There was no preview value for the given setting.
    * @leave Any system error code.
    */
    virtual void CommitPreviewL( const TCamSettingItemIds& aSettingId );

    /**
    * Commits the preview for a set of settings.
    * If some of the settings do not have active preview, such error is ignored.
    */
    virtual void CommitPreviewsL( const RArray<TCamSettingItemIds>& aSettingIds );

    /**
    * Commit previews for all settings with active preview.
    * @leave Any system error code.
    */
    virtual void CommitAllPreviewsL();
    
    /**
    * Get the set of settings with active preview.
    * Preview is active if it has been set with ActivatePreviewL
    * but not cancelled or committed after that.
    * @param aSettingIds Return the ids of the active settings in this array.
    */
    virtual void ActivePreviewsL( RArray<TCamSettingItemIds>& aSettingIds ) const;


  // -------------------------------------------------------------------------
  // New methods
  public:

    /*
    * Sets user scene setting to default if user has once selected
    */
	void SetUserSceneDefault(); 
    
	/*
    * Gets the handle to the settings model, which is used inturn by the
    * plugin to call the interfaces for performing needed actions.
    */
    MCamStaticSettings& StaticSettingsModel() const;

    /**
    * Loads the static settings from shared data, or if unable to from the resource file. 
    * @since 2.8
    */       
    void LoadSettingsL();   
    
    /**
    * Saves the static settings to shared data.
    * @since 2.8
    */       
    void SaveSettingsL() ;
    
    /*
    * Is Scene supported
    */
    TBool IsImageSceneSupported( const TInt aSceneId ) const;
    
    /*
    * Is Scene supported
    */
    TBool IsVideoSceneSupported( const TInt aSceneId ) const;
    
    /*
    * Returns UiConfigManager Ptr from StaticSettingsModel
    */
    CCameraUiConfigManager* UiConfigManagerPtr();
  private:
  
    /**
    * Saves one static setting to Central Repository.
    * @param aSettingId Id of the setting to be saved. 
    * @since Camera 4.0
    */
    void SaveStaticSettingL( TCamSettingItemIds aSettingId );

    /**
    * Get the associated settings list for given setting item.
    * @param aSettingId Id of the setting for which the list is requested.
    * @since Camera 4.0
    */
    RPointerArray<TIntSetting>* 
      MapSettingItem2SettingsList( TCamSettingItemIds aSettingId );

  // -------------------------------------------------------------------------
  // Constructors
  private:

    /**
    * C++ constructor.
    * @since 2.8
    */
    CCamSettingsModel( CCamConfiguration& aConfiguration );

    /**
    * C++ 2nd phase constructor.
    * @since 2.8
    */
    void ConstructL();

  // -------------------------------------------------------------------------
  // new functions
  private: 
    
    // Notify setting model observers, that a setting item value has changed.
    void NotifyObservers( TCamSettingItemIds aSettingId, 
                          TInt               aSettingValue );

    void NotifyObservers( TCamSettingItemIds aSettingId, 
                          const TDesC&       aSettingValue );
    
    /**
    * Read variant flags from Central Repository.
    */
    void ReadVariantFlagsL();
 
    /**
    * Resets all user scene settings to their defaults without 
    * activating them.
    * @since 2.8
    */
    void ResetUserSceneWithoutActivatingChangesL();

    /**
    * Copies a user scene setting across to the capture setup settings.
    * @since 2.8
    * @param aSettingItem the user scene setting to copy.
    */
    void CopyUserSceneSettingToCaptureSettingL( TInt aSettingItem );

    /**
    * Activates the current user scene settings.
    * @since 2.8
    */
    void ActivateUserSceneSettingsL();

    /**
    * Returns whether a particular setting is an engine setting, or not.
    * @since 2.8
    * @param aSettingItem The id of the setting item.
    * @return ETrue if an engine setting. Otherwise EFalse.
    */
    TBool SettingIsForEngine( TInt aSettingItem );

    /**
    * Searches in a settings list for a particular setting item..
    * @param aSettingsList the setting list to search in.
    * @param aSettingItem the id of the setting item to look for.
    * @return the index of the setting item if found. Otherwise returns KErrNotFound .
    * @since 2.8
    */
    TInt SearchInSettingsListFor( 
            const RPointerArray<TIntSetting>& aSettingsList, 
                  TInt                        aSettingItem  ) const;


    /**
    * Finds a scene in a particular scene list.
    * @param aSceneItemId the id of the scene to search for.
    * @param aSceneList the scene list to search in.
    * @return the index to the scene in the list.
    * @since 2.8
    */
    TInt FindSceneInSceneList( 
                  TInt                       aSceneItemId, 
            const RPointerArray<TSceneData>& aSceneList   ) const;

    /**
    * Handles a change in the value for the user scene setting.
    * @param aSettingValue The new scene value.
    * @param aActivate Whether to activate the user scene changes
    * @since 2.8
    */
    void UserSceneHasChangedL( TInt aSettingValue, TBool aActivate );

    /**
    * Handles a change in the value for the photo scene setting.
    * That is, it updates the rest of the photo dynamic setings.
    * @param the new scene value.
    * @since 2.8
    */
    void PhotoSceneHasChangedL( TInt aSettingValue );
    /**
    * Handles a change in the value for the video scene setting.
    * That is, it updates the rest of the video dynamic setings.
    * @since 2.8
    * @param aSettingValue The new scene value.
    */
    void VideoSceneHasChangedL( TInt aSettingValue );

    /**
    * Returns the default value of a setting for a video scene item.
    * @param aSceneId the id of the scene item.
    * @param aSetting the id of the setting item.
    * @return the default value
    * @since 2.8
    */
    TInt DefaultSettingValueForVideoScene( TInt aSceneId, TInt aSetting ) const;


    /**
    * Returns the maximum allowed quality for a photo scene.
    * @param the scene id.
    * @return the highest allowed quality.
    * @since 2.8
    */
    TInt PhotoSceneMaximumQuality( TInt aSceneId ) const;

    /**
    * Returns the current photo quality value.
    * @return the quality.
    * @since 2.8
    */
    TInt CurrentPhotoQualityValue() const;

    /**
    * Returns the photo quality array index.
    * @Param the photo quality id for the desired quality
    * @return the array index for the specified quality.
    * @since 2.8
    */
    TInt PhotoQualityArrayIndex(TInt aPhotoQualityId ) const;
    
    /**
    * Returns the default value of a setting for a particular scene item.
    * @param aSceneId the id of the scene item.
    * @param aSetting the id of the setting item.
    * @return the default value
    * @since 2.8
    */
    TInt DefaultSettingValueForScene( TInt aSceneId, TInt aSetting ) const;

    /**
    * Loads a list of dynamic settings from the resource file and 
    * their default values.
    * @param aResourceId the id of the resource that contains the
    * settings items id's and their default values.
    * @param aSettingsList the list to populate with dynamic settings
    * from the resource file.
    * @param aDefaultsList the list to populate with default values
    * for the dynamic settings from the resource file.
    * @since 2.8
    */
    void LoadDynamicSettingsL( TInt                        aResourceId, 
                               RPointerArray<TIntSetting>& aSettingsList, 
                               RArray<TInt>&               aDefaultsList );

    /**
    * Loads a list of dynamic settings from the resource file 
    * @param aResourceId the id of the resource that contains the
    * settings items id's and their default values.
    * @param aSettingsList the list to populate with dynamic settings
    * from the resource file.
    * @since 2.8
    */
    void LoadDynamicSettingsL( TInt                        aResourceId, 
                               RPointerArray<TIntSetting>& aSettingsList );

    /**
    * Loads a list of scene settings from the resource file.
    * @param aResourceId the id of the resource in the resource file.
    * @param aSceneDataArray the array to populate with the resource.
    * @since 2.8
    */
    void LoadScenesDataL( TInt                       aResourceId, 
                          RPointerArray<TSceneData>& aSceneDataArray );
        
    /**
    * Loads a list of scene settings from product specific header file.
    * @since 3.0
    */
    void LoadSceneDataL( TBool aPhotoSceneSettings = ETrue );
    
    
    /**
    * Removes any previously loaded settings.
    * @since 2.8
    */
    void UnloadSettings();
    
    /**
    * Removes any previously loaded static settings.
    * @since 2.8
    */
    void UnloadStaticSettings();

    /**
    * Loads component resources
    */ 
    void LoadResourcesL();  
    
    /**
    * Frees component resources
    */ 
    void UnLoadResources();
    
    /**
    * Backup and Restore Settings array
    * @param aSrc The source settings array
    * @param aDest The destination settings array
    * @param aRestore ETrue to restore
    * @since 5.1
    */
    void CopySettingsL(RPointerArray<TIntSetting>& aSrc,
                 RPointerArray<TIntSetting>& aDest, 
                 TBool aRestore = EFalse);

    /**
    * Store UserScene Settings 
    * @since 5.1
    */
    void StoreUserSceneSettingsL();
    
  private: // Functions from base classes

    /**
    * From MCamBurstModeObserver
    * @since 2.8
    * @param aActive whether or not burst mode is active
    * @param aStillModeActive whether or not still capture is active
    */
    void BurstModeActiveL( TBool aActive, TBool aStillModeActive );

  // =========================================================================
  private: // data
    TCamSettingsOnOff iPreviousFaceTrack; // previous user defined FT setting
    TCamSceneId iPreviousSceneMode; // previous camera scene mode
  

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

    // Reference to observer of this settings model.
    // MCamSettingsModelObserver& iObserver;
    RPointerArray<MCamSettingsModelObserver> iObservers;
    
    // Setting previews
    RPointerArray<TIntSetting> iSettingPreviews;
    
    // List of setting item id/value pairs for static photo settings.
    RPointerArray<TIntSetting> iDynamicPhotoIntSettings;
    
    //Backup of iDynamicPhotoIntSettings
    RPointerArray<TIntSetting> iDynamicPhotoIntSettingsBackup;
    
    
    // Parallel array to "iDynamicPhotoIntSettings" containing
    // default values.
    RArray<TInt> iDynamicPhotoIntDefaults;
    
    // List of setting item id/value pairs for dynamic photo settings.
    RPointerArray<TIntSetting> iDynamicVideoIntSettings;
    
    //Backup of iDynamicVideoIntSettings
    RPointerArray<TIntSetting> iDynamicVideoIntSettingsBackup;
    
    // Parallel array to "iDynamicVideoIntSettings" containing
    // default values.
    RArray<TInt> iDynamicVideoIntDefaults;
    
    // List of setting item id/value pairs for user scene settings.
    RPointerArray<TIntSetting> iUserSceneSettings;
    
    //Backup of iUserSceneSettings
    RPointerArray<TIntSetting> iUserSceneSettingsBackup;
    
    // Lists of scene data.
    RPointerArray<TSceneData> iPhotoScenes;
    RPointerArray<TSceneData> iVideoScenes;
    
    // Specifies whether the engine should be updated with settings such as
    // White Balance after being prepared. This is device dependent and so
    // will be read from the resource file.
    TBool iUpdateAfterPrepare;

    // Specifies whether this application is embedded or not.
    TBool iEmbedded;

    // Specifies whether burst mode is enabled or not
    TBool iBurstEnabled;

    // Reference to the CEikonEnv object, since it is used a lot.
    CEikonEnv* iEnv;
    
    // array of video quality level items read in from resource file
    RPointerArray < CCamVideoQualityLevel > iQualityLevels;
    
    // array of video resolutions read from resource
    RArray< TSize > iVideoResolutions;
    
    // array of photo quality levels read from resource
    RArray< TCamPhotoQualitySetting > iPhotoQualityLevels;
    
    // array of photo resolutions read from resource
    RArray< TSize > iPhotoResolutions;
    
    // Specifies whether the engine is ready for receiving
    // a video prepare event.
    TBool iEngineReadyForVideoPrepare;
    
    // Specifies whether the settings model is waiting to
    // prepare the engine with video.
    TBool iWaitingToPrepareVideo;
    
    // The cached primary camera settings
    TCamCameraSettings iPrimaryCameraSettings;
    // The cached secondary camera settings
    TCamCameraSettings iSecondaryCameraSettings;
    
    // Variant info
    TCamVariantInfo iVariantInfo;
    TInt iResourceFileOffset;
    
    // Number of image scene settings

    CCamConfiguration& iConfiguration;
    CCamStaticSettingsModel* iStaticModel;
    CCameraUiConfigManager* iUiConfigManager; // we dont own
  };

#endif // CAMSETTINGSMODEL_H

// End of File
