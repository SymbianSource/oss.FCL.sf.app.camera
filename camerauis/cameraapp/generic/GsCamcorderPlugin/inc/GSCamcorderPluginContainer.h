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
* Description:  Container class for the camera general settings plugin
*
*/


#ifndef CAMCODERGSPLUGINCONTAINER_H
#define CAMCODERGSPLUGINCONTAINER_H

// INCLUDES
// System includes
#include <GSBaseContainer.h>

// CONSTANTS
const TInt KGSCamSettingsListImageIndex = 0;
const TInt KGSCamSettingsListVideoIndex = 1;

// FORWARD DECLARATIONS
class CGSListBoxItemTextArray;
class CAknIconArray;
class TAknsItemID;
// CLASS DECLARATION
/**
*  CGSTelPluginContainer class 
*/
class CGSCamcorderPluginContainer  : public CGSBaseContainer
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS default constructor.
        *
        * @param aRect gives the correct TRect for construction.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Destructor.
        */
        ~CGSCamcorderPluginContainer();

    public:

        /**
        * Updates list box
        *
        * @param aFeatureId is a updated list box item
        */
        virtual void UpdateListBoxL( TInt aFeatureId );

        /**
        * Creates list box
        *
        * @param aResLbxId is resource number to create.
        */  
        void ConstructListBoxL( TInt aResLbxId );

        /**
        * Retrieves the currently selected listbox feature id
        * @return feature id.
        * @since 3.1
        */
        TInt CurrentFeatureId() const;
        
        /**
        * Load icon bitmaps.
        * @since 3.1
        */
        void LoadIconsL();
        
        /**
        * From CCoeControl changes the size of the list box
        */
        void SizeChanged();
        
    protected: //new

        /**
        * Creates list box items
        */  
        virtual void CreateListBoxItemsL();
       
        /**
        * Creates Image setting list box item.
        */
        void MakeImageSettingsFolderItemL();

        /**
        * Creates Video setting list box item.
        */
        void MakeVideoSettingsFolderItemL();
        
        /**
        * Required for help.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

        /** 
        * From CCoeControl handles focus changes for listbox item
        */
        void FocusChanged( TDrawNow aDrawNow );
        
    protected:        
        CGSListBoxItemTextArray* iListboxItemArray;
    private:
    void AppendIconToArrayL( CAknIconArray* aArray,
            const TAknsItemID& aID,
            const TDesC& aMbmFile,
            TInt aBitmapId,
            TInt aMaskId );
    };

#endif // CamCODERGSPLUGINCONTAINER_H

// End of File
