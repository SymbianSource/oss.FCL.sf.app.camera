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
* Description:  Encapsulates the data for a particular Capture Setup Menu item.*
*/



#ifndef CAMCAPTURESETUPMENULISTITEM_H
#define CAMCAPTURESETUPMENULISTITEM_H

//  INCLUDES
#include <e32base.h>
#include <e32des16.h>
 

// FORWARD DECLARATIONS
class CFbsBitmap;
class TResourceReader;

// CONSTANTS
const TInt KListItemMaximumTextLength = 40;
const TInt KListItemMaximumIconTextLength = 5;

typedef TBuf<KListItemMaximumIconTextLength> TIconText;


// CLASS DECLARATION

/**
*  Encapsulates the data for a particular Capture Setup Menu item.
*  @since 2.8
*/
class CCamCaptureSetupMenuListItem: public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aReader reader that is used to construct the data for this list item
        * @return pointer to the created list item.
        */
        static CCamCaptureSetupMenuListItem* NewL( TResourceReader& aReader );
        
        /**
        * Destructor.
        */
        ~CCamCaptureSetupMenuListItem();

    public: // New functions
        
        /**
        * Returns the bitmap that represents the current settings model 
        * value for this list item.
        * @since 2.8
        * @return A pointer to the bitmap.
        */
        CFbsBitmap* Bitmap() const;

        /**
        * Returns the mask that represents the current settings model 
        * value for this list item.
        * @since 2.8
        * @return A pointer to the bitmap.
        */
        CFbsBitmap* Mask() const;

        /**
        * Returns text used in place of an icon for this list item.
        * @since 2.8
        * @param aSettingVal The setting value used to return the text for
        * @return A reference to the icon text.
        */
        const TDesC& IconText( TInt aSettingVal ) const;

        /**
        * Returns text that describes the setting represented by this list item. 
        * @since 2.8
        * @return A reference to the text.
        */
        const TDesC& Text() const;

        /**
        * Returns the cached settings model value that the current bitmap represents. 
        * @since 2.8
        * @return the cached settings model value.
        */
        TInt CurrentValue() const;

        /**
        * Returns the id of the settings model item that this list item represents. 
        * @since 2.8
        * @return the settings model item id.
        */
        TInt SettingsModelItemId() const;

        /**
        * Changes the cached settings model value and the bitmap that represents it. 
        * @since 2.8
        * @param aNewValueId the new settings model value id.
        */
        void SetValueTo( TInt aNewValueId );

    private: // Constructors
 
        /**
        * C++ default constructor.
        * @since 2.8
        */
        CCamCaptureSetupMenuListItem();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 2.8
        * @param aReader reader that is used to construct the data for this list item
        * from resources
        */
        void ConstructFromResourceL( TResourceReader& aReader );

    private: // New functions
        /**
        * Returns the array index for the brightness/contrast setting value
        * @since 2.8
        * @param aValue  in:the setting value to map to an array index
        *                out: the array index
        */
        void GetBrightnessContrastArrayIndex( TInt& aValue ) const;

    private:    // Data
        // Provides a list of all the bitmaps that are associated with
        // a particular setting item value id.
        RPointerArray<CFbsBitmap> iBitmaps;
        RPointerArray<CFbsBitmap> iMasks;

        // Text to be used in place of an icon for this list item.
        CArrayFixFlat<TIconText>* iIconTexts;

        // id of the setting item that is associated with this list item.
        TInt iSettingsModelItemId;

        // The text description for this list item.
        TBuf<KListItemMaximumTextLength> iText;

        // The settings model id of the currently set value for this list item.
        TInt iCurrentSettingItemValueId;
    };

#endif      // CAMCAPTURESETUPMENULISTITEM_H   
            
// End of File
