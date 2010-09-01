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
* Description:  Handler for Active Palette in Camera application
*
*/


#ifndef CAMACTIVEPALETTEHANDLER_H
#define CAMACTIVEPALETTEHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <eikenv.h>
 
#include <activepalette2observer.h>


// FORWARD DECLARATIONS
class CCamAppController;
class CCamViewBase;
class TActivePalette2EventData;
class MActivePalette2UI;

// CLASS DECLARATION

class TCamActivePaletteItem
    {
public:
    TInt iItemId;
    TInt iCmdId;
    TInt iPosition;
    TInt iIsVisible;
    TInt iType;
    };

class TCamAPPreCaptureItem
    {
public:
    TInt    iSettingValue;
    TInt    iBitMapId;   
    };

// struct holding setting item and value   
class TCamAPSettingsItem
    {
public:
    TInt iSettingItem;
    TInt iSettingVal;
    };

/**
 *  Handler for Active Palette in Camera application
 *
 *  @since 3.0
 */
class CCamActivePaletteHandler : public CBase, public MActivePalette2Observer
    {
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCamActivePaletteHandler* NewL( CCamAppController& aController, TBool aCreatePalette );

    /**
     * Destructor.
     */
    virtual ~CCamActivePaletteHandler();

public: // New functions

    /**
     * Return handle to Active Palette
     * @since 3.0
     * @return Reference to Active Palette
     */
    MActivePalette2UI* ActivePalette() const;        

    /**
     * Install Active Palette items from resource
     * @since 3.0
     * @param aResId resource id for Active Palette items
     */
    void InstallAPItemsL( TInt aResId );

    /**
     * Inform active palette handler that returned from pretend exit
     * so that it can deal with it.
     * @since 3.0
     */
    void ReturnedFromPretendExitL();

    /**
     * Resets highlight to the first AP item
     * @since 3.1
     */
    void ResetToFirstItem();

    /**
     * Resets highlight to the default AP item
     * @since 3.1
     */
    void ResetToDefaultItem();


    /**
     * Force Active Palette update
     * @since 3.0
     */
    void UpdateActivePaletteL();

    /**
     * Set current view for Active Palette
     * @since 3.0
     * @param aView - pointer to current view
     */
    void SetView( CCamViewBase* aView );

    /**
     * Set visibility of Active Palette
     * @since 3.0
     * @param aVisibility - ETrue to set visible, EFalse to set invisible
     */
    void SetVisibility( TBool aVisibility );

    /**
     * Visibility of Active Palette
     * @since 3.0
     * @return ETrue if AP is visible
     */
    TBool IsVisible() const;

    /**
     * Whether current item was accessed via Active Palette
     * @since 3.0
     * @return ETrue if AP launched current item
     */
    TBool AccessedViaAP() const;

    /**
     * Inform AP handler AP launched item has completed
     * @since 3.0
     */
    void APOperationComplete();

    /**
     * Pass keys events to Active Palette
     * @since 3.0
     * @param aKeyEvent - key event
     * @param aType - key type
     */
    void HandleAPKeyEventL( const TKeyEvent& aKeyEvent,
            TEventCode aType);

    /**
     * Create the Active Palette
     * @since 3.0
     */
    void CreateActivePaletteL(const CCoeControl &aContainerWindow,
            MObjectProvider *aMopParent);

    /**
     * Destroys the Active Palette
     * @since 3.0
     */
    void DestroyActivePalette();

    /**
     * Load post-capture view resources
     * @since 3.0
     * @param aItem id of resource to load
     */
    void LoadPostCaptureResourcesL( TInt aItem );

    /**
     * Get the resource id for the current setting
     * @since 3.0
     * @param aSettingItem  Setting we need resource for
     * @return Id of array resource containing icons for this setting
     */
    TInt PreCaptureArrayId( TInt aSettingItem );

    /**
     * Get bitmap for specfic value of settings item and sets its tooltip
     * @since 3.0
     * @param aItem the setting value to map to a bitmap id
     * @return bitmap id for specified settings val
     */
    TInt GetBitmapIdFromValueL( TCamAPSettingsItem& aItem );

    /**
     * Get resource for for specific post-capture item
     * @since 3.0
     * @param aCommandId the command ID value to map to an item ID
     * @return resource ID for post capture item 
     */
    TInt PostCaptureItemId( TInt aCommandId );

    /**
     * Sets an icons visability
     * @since 3.0
     * @param aCommandId the command ID for the icon
     * @param aIsVisible the visability of the icon
     */
    TInt SetItemVisible( const TInt aCommandId, const TBool aIsVisible );

    /**
     * Disables/enables active palette updates. 
     * This can be used to avoid redrawing and re-initalizing active object
     * contents multiple times, when several settings are updated to the engine
     * at once.
     * @since 3.0
     * @param aUpdatesDisabled
     */
    void DisableUpdates( TBool aUpdatesDisabled );

    /**
     * Update tooltip text for one-click upload item. This method needs to be
     * called before InstallAPItemsL is called.
     *
     * @since S60 v5.1
     * @param aToolTip   Tooltip text for one-click upload item 
     */
    void SetOneClickUploadTooltipL( const TDesC& aToolTip );

public: // Functions from base classes

    /**
     * Notification of AP item focus change
     * @since 3.0
     * @param aPrevItem index of item we are moving from
     * @param aFocusedItem index of item we are moving to
     */
    void NotifyItemFocused(TInt aPrevItem, TInt aFocusedItem);

    /**
     * Notification of AP item selection
     * @since 3.0
     * @param aEvent Event data from active palette
     */
    void NotifyItemSelected(const TActivePalette2EventData& aEvent);       

    /**
     * Notification of active palette item completion
     * @since 3.0
     * @param aEvent Event data from active palette
     * @param aDataDes - descriptor containing packaged data
     * @param aDataInt - data from AP event
     */
    void NotifyItemComplete(const TActivePalette2EventData& aEvent,
            const TDesC8& aDataDes, TInt aDataInt);

    /**
     * Notification message received from Active Palette
     * @since 3.0
     * @param aMessageID - id of message received from Active Palette
     * @param aEvent Event data from active palette
     * @param aDataDes - descriptor containing packaged data
     * @param aDataInt - data from AP event
     */
    void NotifyMessage(const TActivePalette2EventData& aEvent, 
            TInt aMessageID, const TDesC8& aDataDes, TInt aDataInt);

    /**
     * Requests that the camera application redraws the supplied area
     * @since 3.0
     * @param aArea the area to redraw
     */
    void Redraw(const TRect& aArea);

    /**
     * Informs us that the Active Palette has been updated
     * @since 3.0
     */
    void ActivePaletteUpdated(void);


private:

    /**
     * C++ default constructor.
     */
    CCamActivePaletteHandler( CCamAppController& aController );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( TBool aCreatePalette );

    /**
     * Determine Non-Setting State of a Pre-Capture Non-Setting item
     * @since 3.0
     * @param aItem - The activepalette item to initialise
     * @return the current "setting value" of the item
     */
    TInt DetermineNonSettingState(const TCamActivePaletteItem& aItem) const;

    /**
     * Set the tooltip of a pre-capture non-setting item
     * @since 3.0
     * @param aItem - The activepalette item to initialise
     */
    void SetNonSettingItemTooltipL(const TCamActivePaletteItem& aItem);

    /**
     * Clears items from the Active Palette Control
     * @since 3.0
     */        
    void ClearActivePalette();


    void InstallOneAPItemL( const TCamActivePaletteItem& aItem,
            const TUid&                  aPluginUid,
            const TDesC&                 aToolTipText,
            const TDesC&                 aIconFileName,
            TInt                   aIconId,
            TInt                   aMaskId );

    void CheckFlashItemVisibilityL();

    /**
     * Read toolbar items resource content to aItems 
     * @param aResId Toolbar item resource id
     * @param aItems Item array
     */
    void ReadToolbarItemsFromResourceL( TInt   aResId, 
            RArray<TCamActivePaletteItem>& aItems );


    /**
     * Calculate the active palette dimension according to resolution
     * param aLength the length to be converted
     */
    TPoint APScreenLocation( TPoint aQvgaPoint );

    /*
     *  To check if a specific feature is supported, to append or show in AP
     */
    TBool IsAPItemFeatureSupportedL( const TInt aItemId ) const;

private:    
    CEikonEnv* 			iEnv;

    MActivePalette2UI*   iActivePalette;       

    CCamAppController& iController;

    CCamViewBase* 		iView;

    // array of items for active palette
    RArray<TCamActivePaletteItem> iItemArray;

    // AP visibility flag
    TBool 				iVisible;

public: 
    // flag indicating whether AP has launched an item
    TBool 				iItemAccessedViaAP;

private: 
    // array of pre-capture items
    RArray<TCamAPPreCaptureItem> iPreCaptureItems;

    // The current tooltip text
    HBufC*              iToolTipText;

    // Icon file name.
    HBufC*              iIconFileName;

    // Current icon id.
    TInt				iIconId;

    // store resoure id for EV cmp
    TInt				iEvCompRes;

    TBool       iUpdatesDisabled;

    TBool 		iStarted;

    // Set to true, if items for the toolbar were initialised dynamically
    TBool     iDynamicToolbar;

    // Tooltip text for one-click upload item.
    // Own.
    HBufC* iOneClickUploadTooltip;
    
    RArray<TCamAPSettingsItem> iCurrentValSettingsItem;
    };

#endif      // CAMACTIVEPALETTEHANDLER_H  

// End of File
