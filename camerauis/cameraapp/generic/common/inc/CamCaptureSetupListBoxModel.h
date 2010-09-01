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
* Description:  A model class for CCamCaptureSetupListBox*
*/


#ifndef CAMCAPTURESETUPISTBOXMODEL_H
#define CAMCAPTURESETUPISTBOXMODEL_H

//  INCLUDES
 
#include <AknUtils.h>

#ifndef CAMERAAPP_PLUGIN_BUILD
  #include "CamAppController.h"
#else
  #include "MCamAppController.h"
#endif //CAMERAAPP_PLUGIN_BUILD
#include "mcamlistboxmodel.h"


// FORWARD DECLARATIONS
class CCamInfoListboxItemBase;
class CFbsBitmap;
class TAknLayoutRect;

// CLASS DECLARATION

/**
*  Model class of CCamCaptureSetupListBox
*
*  @since 2.8
*/
class CCamCaptureSetupListBoxModel : public CBase, 
                                     public MCamListboxModel
  {
  public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aController Controller reference
        * @param aArrayResourceId Id of resource used to populate the list
		* @param aSettingItem setting item
        * @param aShootingModeListBox ETrue if the model represents a shooting mode capture setup listbox
        * @return pointer to the created CCamCaptureSetupListBoxModel object
        */
        static CCamCaptureSetupListBoxModel* NewLC( 
#ifndef CAMERAAPP_PLUGIN_BUILD
                                                    CCamAppController& aController,
#else // location setting will be moved to the plugin
                                                    MCamAppController& aController,
#endif
        											TInt aArrayResourceId,
        											TInt aSettingItem,
                                                    TBool aShootingModeListBox,
                                                    TBool aLocationSettingListBox,
                                                    TRect aListBoxRect
                                                     );

    /**
    * Destructor.
    */
    virtual ~CCamCaptureSetupListBoxModel();
     
			
  public: // from MCamListboxModel

    /**
    * @since 2.8
    * Returns number of items in item array
    * @return Number of items
    */
    virtual TInt NumberOfItems() const;
    
    /**
    * Returns pointer to the MDesCArray
    * @since 2.8
    * @return Handle to MDesCArray
    */        
    virtual const MDesCArray* MatchableTextArray() const;
    
    /**
    * Returns pointer to the item text
    * @param aItemIndex index of the item 
    * @return pointer to the item text
    */   
    virtual TPtrC ItemText( TInt aItemIndex ) const;
    
    /**
    * Returns the value id of the specified item
    * @param aItemIndex index of the item 
    * @return the item's value id
    */   
    virtual TInt ItemValue( TInt aItemIndex ) const;
    
    /**
    * Returns the bitmap for the specified item
    * @param aItemIndex index of the item 
    * @return pointer to the icon bitmap
    */   
    virtual CFbsBitmap* Bitmap( TInt aItemIndex ) const;
    
    /**
    * Returns the bitmap mask for the specified item
    * @param aItemIndex index of the item 
    * @return pointer to the icon mask
    */  
    virtual CFbsBitmap* BitmapMask( TInt aItemIndex ) const;
    
    /**
    * Returns the index of the item with the specified
    *         value id
    * @param aValueId the value to find
    * @return index of the matching item
    */          
    virtual TInt ValueIndex( TInt aValueId ) const;
    

    /**
    * Returns whether the model represents a shooting mode selection listbox
    * @since 2.8
    * @return TBool ETrue if the model is representing a shooting mode listbox instead of a general settings listbox
    */
    virtual TBool ShootingModeListBox() const;
    
    virtual TAknLayoutRect IconLayoutData() const;
    virtual TAknLayoutRect RadioButtonLayoutData() const;
    virtual TAknLayoutText TextLayoutData() const;
    virtual TAknLayoutRect ListItemLayoutData() const;


	protected:

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aController Controller reference
        * @param aShootingModeListBox is ETrue if the model represents a shooting mode settings listbox
        */
        CCamCaptureSetupListBoxModel( 
#ifndef CAMERAAPP_PLUGIN_BUILD
                                     CCamAppController& aController,
#else // location setting will be moved to the plugin
                                     MCamAppController& aController,
#endif //CAMERAAPP_PLUGIN_BUILD
        							  TBool aShootingModeListBox,
        							  TBool aLocationSettingListBox = EFalse 
        							  );

    private:    // Methods

   
        /**
		* Second phase constructor
		* @since 2.8
		* @param aArrayResourceId Resource used to populate the list
		* @param aSettingItem setting item
        */
        void ConstructL( TInt aArrayResourceId, TInt aSettingItem, TRect aListBoxRect );

	protected:
        /**
        * Read in layout data from resource
        * @since 2.8
        */
        void ReadLayoutData( TRect aListBoxRect );  
        
        /**
        *
        *
        */
        TBool SettingValueSupportedL( CCamInfoListboxItemBase* aItem, TInt aSettingItem );

    protected:    // Data
		
		// Array of items in the listbox
        RPointerArray<CCamInfoListboxItemBase> iItemArray;

        
        							  
        // layout data for listbox item icon
        TAknLayoutRect iIconLayoutData;

        // layout data for listbox item radio button
        TAknLayoutRect iRadioButtonLayoutData;

        // layout data for listbox item text
        TAknLayoutText iTextLayoutData;
        
        TAknLayoutRect iListItemLayoutData;
        
        
        
#ifndef CAMERAAPP_PLUGIN_BUILD
        CCamAppController& iController;
#else
        MCamAppController& iController;
#endif //CAMERAAPP_PLUGIN_BUILD
        // Flag as to whether the model is for a shooting mode listbox
        TBool iShootingModeListBox;
        // Flag as to whether the model is for a location setting listbox
        TBool iLocationSettingListBox;
        
        TUint32 iSupportedFlashModes;
        
        TUint32 iSupportedWBModes;

    };

#endif      // CAMCAPTURESETUPISTBOXMODEL_H   
            
// End of File
