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
* Description: 
*
*/


#ifndef GS_CAMCORDERGSPLUGIN_H
#define GS_CAMCORDERGSPLUGIN_H

// Includes
#include <aknview.h>
#include <eikclb.h>
#include <ConeResLoader.h>
#include <GSPluginInterface.h> 
#include <gsbaseview.h>
#include "CamStaticSettingsModel.h"

// Classes referenced
class CGSCamcorderPluginContainer;
class CAknViewAppUi;
class CGSBaseView;
class MGSTabbedView;
class CCamStaticSettingsModel;
class CCamConfiguration;


// Constants
_LIT(KDirAndFile,"z:\\resource\\apps\\camerasettingsplugin.mif");

/** 
* UPDATE THIS:
*
* This UID is used for both the view UID and the ECOM plugin implementation 
* UID.
*/
const TUid KGSCamcorderGSPluginUid = { 0x2000F8E0 };

_LIT( KGSCamcorderPluginResourceFileName, "z:\\resource\\gscamerapluginrsc.rsc" );
_LIT( KGSCamcorderPluginResourceFileNamev2, "z:\\resource\\gscamerapluginrscv2.rsc" );
  
// CLASS DECLARATION

/**
*  CGSCamcorderPlugin view class.
*
* This is an CamcorderGS of the minimal GS plugin.CGSPluginInterface
*/
class CGSCamcorderPlugin : public CGSBaseView
    {
    
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @return 
        * @since 3.1
        */
        static CGSCamcorderPlugin* NewL( TAny* aInitParams );
            
        /**
        * Destructor.
        */
        ~CGSCamcorderPlugin();       
        

    public: // Functions from base classes
        
        /**
        * From CAknView, returns the views id.
        * @return Returns id of the view.
        * @since 3.1
        */       
        TUid Id() const;
        
         /**
        * From MEikCommandObserver, handles the menu based commands.
        * @param aCommand identifies the given command.
        * @since 3.1
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * Updates specific value to container 
        * @param aListItem item index to update
        * @param aNewValue new value for the setting
        * @since 3.1
        */
        void UpdateListBoxL( const TInt& aListItem, TInt aNewValue );
        
     public: // From CGSPluginInterface 
		
        /*
        * Check whether this view been lauched from General Settings.
        */
        TBool LaunchedFromGS() const;

        /**
        * Method for getting caption of this plugin. This should be the
        * localized name of the settings view to be shown in parent view.
        * @param aCaption pointer to Caption variable
        * @since 3.1
        */
        void GetCaptionL( TDes& aCaption ) const;
        
        /**
        * Creates a new icon of desired type. Override this to provide custom
        * icons. Othervise default icon is used. Ownership of the created icon
        * is transferred to the caller.
        *
        * Icon type UIDs (use these defined constants):
        * KGSIconTypeLbxItem  -   ListBox item icon.
        * KGSIconTypeTab      -   Tab icon.
        *
        * @param aIconType UID Icon type UID of the icon to be created.
        * @return Pointer of the icon. NOTE: Ownership of this icon is
        *         transferred to the caller.
        * @since 3.1
        */
        CGulIcon* CreateIconL( const TUid aIconType );
        
         /**
        * Method for reading the ID of the plugin provider category. See
        * TGSPluginProviderCategory. PluginProviderCategory can be used for
        * sorting plugins.
        *
        * Default value is EGSPluginProvider3rdParty. Override this function
        * to change the category.
        *
        * @return Plugin provider category ID defined by
        *         TGSPluginProviderCategory
        * @since 3.1
        */
        TInt PluginProviderCategory() const;
        
        /**
        * Method for checking, if plugin should be visible and used in GS FW.
        * (for example shown in listbox of the parent view).
        *
        * On default plugin is visible. Overwrite this function to enable or
        * disable your plugin dynamically.
        *
        * @return ETrue if plugin should be visible in GS.
        * @return EFalse if plugin should not be visible in GS.
        * @since 3.1
        */
        TBool Visible() const;
        
        /**
        * Reserved for future use/plugin's custom functionality. This can be
        * overwritten if plugin needs to have custom functionality which cannot
        * be fulfilled otherwise.
        * @since 3.1
        */
        TAny* CustomOperationL( TAny* aParam1, TAny* aParam2 );
        
        /**
        * From CEikAppUi
        * Handle skin change event.
        * @since 3.1
        */
        void HandleClientRectChange();
        
        /**
        * From CAknView Activate this view
        * @param aPrevViewId 
        * @param aCustomMessageId 
        * @param aCustomMessage 
        * @since 3.1
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
                          
        /**
        * From CAknView Deactivate this view
        * @since 3.1
        */
        void DoDeactivate();
        
        /**
        * Activate local views within this plug-in
        * @param aFeatureId ID of the view to be activated
        * @since 3.1
        */
        void ActivateInternalViewL( TInt aFeatureId );
        
        /**
        * Creates local views within this plug-in
        * @since 3.1
        */
        void CreateLocalViewL();            
        
        /**
        * Get CamcorderGSPlugin's ccontainer.
        * @since 3.1
        */
        CGSCamcorderPluginContainer* Container();
        
        /**
        * From MGSTabbedView
        * @return Reference to owned tabbed views which should be included in 
        *         the sub-view's tab group.
        * @since 3.1
        */
        CArrayPtrFlat<MGSTabbedView>* TabbedViews();
        
        /**
        *  Method used to check if the the Settings are
        *  are for Secondary camera.
        */
        TBool IsSecondaryCameraSettings();
        
        /*
        *  Fetch the integer value from appropriate settings model
        */
        TInt IntegerSettingValue( const TInt aSettingItem );
        
        /*
        * Fetch the text value from appropriate settings model
        */
        TPtrC TextSettingValue( const TInt aSettingItem );
        
        /*
        * Set text value of a setting item using right settings model
        */
        void SetTextSettingValueL( TInt aSettingItem,
                                   const TDesC& aSettingValue );
        
        /*
        * Set int value of a setting item using right settings model
        */
        void SetIntegerSettingValueL( TInt aSettingItem,
                                      TInt aSettingValue );
        
        /*
        * Loads all video static settings from CenRep
        */
        void LoadVideoStaticSettingsL( TBool aResetFromGS );
        
        /*
        * Loads all image static settings from CenRep
        */
        void LoadPhotoStaticSettingsL( TBool aResetFromGS );
        
        /*
         * Display the reset factory settings dialog
         */
         TInt DisplayRestoreSettingsDlgL(); 
         
         void SetSecondaryCameraSettings( TBool aSecondarySettings );
         
       
    protected:

        /**
        * C++ default constructor.
        */
        CGSCamcorderPlugin();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();
        
        /**
        * Handles OK key press.
        * @since 3.1
        */
        void HandleListBoxSelectionL();
        
        /**
        * From MEikMenuObserver Dynamically customize menu items
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * Creates new container.
        * @since 3.1
        */
        void NewContainerL();
        
    protected: //Data        
       
        RConeResourceLoader iResources;
        // Previous view to return when exiting CamcorderGSPlugin.
        TVwsViewId iParentView;
        
    private:
        void RemoveLocalViews();
                    
    private:
        CArrayPtrFlat<MGSTabbedView>* iViewArray; 
        TBool iSecondaryCameraSettings;
        CCamStaticSettingsModel* iSettingsModel;
        CCamConfiguration*       iConfiguration;
        TBool iReloadSettings;
    };

#endif // GS_CamcorderGSPLUGIN_H
