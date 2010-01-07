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
* Description:  Model class of capture setup menu list box.*
*/


#ifndef CAMCAPTURESETUPMENULISTBOXMODEL_H
#define CAMCAPTURESETUPMENULISTBOXMODEL_H

//  INCLUDES
#include <eiklbm.h>
#include <badesca.h>

// CLASS DECLARATION
class CFbsBitmap;
class CCamAppController;
class CCamCaptureSetupMenuListItem;
class TResourceReader;

/**
*  Model class of capture setup menu list box
*
*/
class CCamCaptureSetupMenuListBoxModel : public CBase, public MListBoxModel
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aController used to obtain setting model values for list items.
        * @param aReader used to construct the model's array of list items from resources.
        * @return pointer to the created model.
        */
        static CCamCaptureSetupMenuListBoxModel* NewLC
            ( CCamAppController& aController, TResourceReader& aReader );

        /**
        * Destructor.
        * @since 2.8
        */
        ~CCamCaptureSetupMenuListBoxModel();

    public: // New functions
        
        /**
        * Queries whether the item has icon text.
        * @since 2.8
        * @return bool result.
        */
		TBool ItemHasIconText( TInt aItemIndex ) const;

        /**
        * Returns the bitmap to be used for a particular list item.
        * @since 2.8
        * @param aItemIndex index of the list item.
        * @param aMask ETrue to return the mask bitmap
        * @return pointer to the bitmap.
        */
		CFbsBitmap* BitmapForItem( TInt aItemIndex, TBool aMask );

        /**
        * Returns the text to be used for a particular list item.
        * @since 2.8
        * @param aItemIndex index of the list item.
        * @return Ref to the item text.
        */
        const TDesC& TextForItem( TInt aItemIndex ) const;

        /**
        * Returns the text to be used in place of an icon for a particular list item.
        * @since 2.8
        * @param aItemIndex index of the list item.
        * @return Ref to the icon text.
        */
		const TDesC& IconTextForItem( TInt aItemIndex ) const;

        /**
        * Returns the settings model id associated with a particular list item.
        * @since 2.8
        * @param aItemIndex index of the list item.
        * @return the settings model id.
        */
        TInt SettingsModelIdAssociatedWithItem( TInt aItemIndex ) const;
		
    public: // Functions from MListBoxModel

        /**
        * From MListBoxModel returns number of items in item array
        * @since 2.8
        * @return Number of items
        */
        TInt NumberOfItems() const;
		
		/**
        * Returns pointer to the MDesCArray
        * @since 2.8
        * @return Handle to MDesCArray
        */        
		const MDesCArray* MatchableTextArray() const;	

    private: // Constructors

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aController used to obtain setting model values for list items.
        */
        CCamCaptureSetupMenuListBoxModel( CCamAppController& aController );

        /**
        * 2nd phase constructor.
        * @since 2.8
        * @param aReader used to construct the model's array of list items from resources.
        */
        void ConstructFromResourceL( TResourceReader& aReader );



	private: //New functions

    private:    // Data
		
        // Reference to the controller.
        CCamAppController& iController;

		//Matchable text array
		MDesCArray* iMatchableTextArray;
		
        // Array of all the list items.
	    RPointerArray<CCamCaptureSetupMenuListItem> iListItemPtrArray;
    };

#endif      // CAMCAPTURESETUPMENULISTBOXMODEL_H   
            
// End of File
