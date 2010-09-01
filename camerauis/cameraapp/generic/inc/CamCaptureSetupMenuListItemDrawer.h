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
* Description:  Draws individual list items.*
*/


#ifndef CAMCAPTURESETUPMENULISTITEMDRAWER_H
#define CAMCAPTURESETUPMENULISTITEMDRAWER_H

//  INCLUDES
#include <eiklbi.h>
#include <AknUtils.h>

// STRUCTS

// FORWARD DECLARATIONS
class CCamCaptureSetupMenuListBoxModel;
class TResourceReader;

// CLASS DECLARATION

/**
* Draws individual list items.
*/
class CCamCaptureSetupMenuListItemDrawer : public CListItemDrawer
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
		* @since 2.8
        * @param aListBoxModel Model that is used to get content information 
        * about list item being drawn.
        * @param aReader Used to populate drawer with layout information for
        * list box elements it draws.
        * @return pointer to the created list item drawer.
        */
        static CCamCaptureSetupMenuListItemDrawer* NewL(
            CCamCaptureSetupMenuListBoxModel* aListBoxModel, 
            TResourceReader& aReader );

        /**
        * Destructor.
        */
        ~CCamCaptureSetupMenuListItemDrawer();
            
    public: // New functions
        /**
        * Sets the parent control of the listbox
        * @since 2.8
        * @param aControl pointer the the listbox's parent
        */
        void SetParentControl( const CCoeControl* aControl );

    public: // Functions from base class CListItemDrawer

        /**
        * Draws currentitem and highlights if needed
		* @since 2.8
		* @param aItemIndex An index of item
		* @param aActualItemRect Rect of item
		* @param aItemIsCurrent ETrue if current item
		* @param aViewIsEmphasized ETrue if emphasized
		* @param aViewIsDimmed ETrue if dimmed
		* @param aItemIsSelected ETrue if selected
		*/
        virtual void DrawActualItem( TInt aItemIndex, const 
			TRect& aActualItemRect, TBool aItemIsCurrent, 
			TBool aViewIsEmphasized, TBool aViewIsDimmed, 
			TBool aItemIsSelected )const;



    private:    // Constructors
        /**
        * C++ default constructor.
		* @since 2.8
		* @param aListBoxModel Handle to ListBoxModel
        */
        CCamCaptureSetupMenuListItemDrawer( CCamCaptureSetupMenuListBoxModel* aListBoxModel );

		/**
        * 2nd phase constructor.
		* @since 2.8
        * @param aReader Used to populate drawer with layout information for
        * list box elements it draws.
        */
        void ConstructFromResourceL( TResourceReader& aReader );

	private:    // New functions
					 
        /**
        * Draws a highlighted rectangle for an item.
		* @since 2.8
		* @param aActualItemRect the rectangular area to be highlighted.
        */
		void DrawHighlightedItemRect( const TRect& aActualItemRect ) const;

        /**
        * Draws a rectangle for an item.
		* @since 2.8
		* @param aActualItemRect the rectangular area to be drawn.
        */
        void DrawItemRect( const TRect& aActualItemRect ) const;



    private:    // Data
        // Used to get the current value of a list item.
		CCamCaptureSetupMenuListBoxModel* iModel;

        // Specifies the layout for drawing a list item's text.
        AknLayoutUtils::SAknLayoutText iLayoutDataForText;

        // Specifies the layout for drawing a list item's icon text (if defined).
        AknLayoutUtils::SAknLayoutText iLayoutDataForIconText;

        // Specifies the layout for drawing a list item's bitmap.
        AknLayoutUtils::SAknLayoutRect iLayoutDataForBitmap;

        // Highlight offset loaded from resource
        TInt iHighlightOffset;      
        
        // The parent control of the list box
        const CCoeControl* iParentControl;
	};

#endif      // CAMCAPTURESETUPMENULISTITEMDRAWER_H
            
// End of File
