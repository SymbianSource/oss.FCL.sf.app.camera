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
* Description:  A user scene setup list item.*
*/


#ifndef CAMUSERSCENESETUPLISTITEM_H
#define CAMUSERSCENESETUPLISTITEM_H

// INCLUDES
#include <e32def.h>
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
class TResourceReader;

// CONSTANTS
const TInt KListItemMaximumSubTextLength = 50;
const TInt KListItemMaximumFullTextLength = 120;


/**
*  A user scene setup list item.  
*  
*/
class CCamUserSceneSetupListItem: public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Symbian two-phased constructor.
        * @param resource Id for current item
        * @return pointer to the created CCamUserSceneSetupListItem object.
        * @since 2.8
        */
        static CCamUserSceneSetupListItem* NewLC( TInt aResId );
        /**
        * Symbian two-phased constructor.
        * @param resource Id for current item
        * @return pointer to the created CCamUserSceneSetupListItem object.
        * @since 2.8
        */
        static CCamUserSceneSetupListItem* NewL( TInt aResId );

        /**
        * Destructor.
        * @since 2.8
        */
        ~CCamUserSceneSetupListItem();

    public: // New functions
        /**
        * Returns the formatted text of this item to be used in the list box
        * @since 2.8
        * @return pointer to the formatted text
        */
        TPtrC16 ListBoxText();
        /**
        * Returns the settings model item id that is associated with this
        * list item.
        * @since 2.8
        * @return the settings model item id.
        */
        TInt SettingsModelTypeId();
        /**
        * Sets the current value of this list item
        * @since 2.8
        * @param the new value.
        */
        void SetCurrentValue( TInt aNewValue );


    private: // Constructors
        /**
        * C++ Constructor
        * @since 2.8
        */
        CCamUserSceneSetupListItem();
        /**
        * 2nd phase Constructor
        * @param resource Id for current item.
        * @since 2.8
        */
        void ConstructL( TInt aResId );
     
    private: // New functions
        /**
        * Returns the name of the setting type represented by this
        * list item.
        * @return the name text.
        * @since 2.8
        */
        TPtrC SettingName() const;
        /**
        * Returns the text for the current setting value of this item
        * @return the setting text.
        * @since 2.8
        */
        TPtrC SettingText();


    private: //data
        // type that couples a value id with a textual description.
        class CValueTextPair: public CBase
            {
            public: // Data.
                // The settings model id of the value.
                TInt iSettingsModelId;    
                // The textual description of the value.
                TBuf<KListItemMaximumSubTextLength> iText;
            };

        // A list of all possible values for this list item, and
        // their associated text.
        RPointerArray<CValueTextPair> iValues;
        // The pair index that is associated with the current settings
        // model value for this list item.
        TInt iCurrentPairIndex;

        // The setting model id for this list item type.
        TInt iSettingsModelItemId;
        // The name of this list item type, e.g. "White balance"
        TBuf<KListItemMaximumSubTextLength> iName;
        // The text for the current value of this item.
        TBuf<KListItemMaximumSubTextLength> iValueText;
        // The full formatted text for this list item.
        TBufC<KListItemMaximumFullTextLength> iListBoxText;
    };

#endif // CAMUSERSCENESETUPLISTITEM_H

// End of File
