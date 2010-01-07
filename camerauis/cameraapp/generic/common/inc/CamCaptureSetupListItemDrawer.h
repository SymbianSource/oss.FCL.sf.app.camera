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
* Description:  A drawer class for the capture setup list box items*
*/


#ifndef CAMCAPTURESETUPLISTITEMDRAWER_H
#define CAMCAPTURESETUPLISTITEMDRAWER_H

//  INCLUDES
#include <eiklbi.h>
#include <AknUtils.h>
#include <AknLayout2ScalableDef.h> 

// FORWARD DECLARATIONS
class MCamListboxModel;
class CAknIconArray;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
*  Item drawer for CCamCaptureSetupListBox
*
*  @since 2.8
*/
class CCamCaptureSetupListItemDrawer : public CListItemDrawer
    {
    public:  // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aListBoxModel reference to a list box model used to access item data
        * @return pointer to the newly created object
        */
        static CCamCaptureSetupListItemDrawer* NewL( MCamListboxModel& aListBoxModel );        

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamCaptureSetupListItemDrawer();

    public:
        /**
        * Sets the parent control of the listbox
        * @since 2.8
        * @param aControl pointer the the listbox's parent
        */
        void SetParentControl( const CCoeControl* aControl );
	
    private:

        /**
        * C++ default constructor.
        * @since 2.8
		* @param aListBoxModel reference to a list box model used to access item data
        */
        CCamCaptureSetupListItemDrawer( MCamListboxModel& aListBoxModel );

		/**
        * Second phase construction
        */
        void ConstructL();
		
    private: // Methods from base classes


        /**
        * From CListItemDrawer
        * Calculates the minimum size for a cell
        * @since 2.8
        * @return the minimum size of a cell
        */
        TSize MinimumCellSize() const;

        /**
        * From CListItemDrawer
        * Draws an item to the screen
        * @since 2.8
		* @param aItemIndex the item to draw
        * @param aActualItemRect the screen area occupied by the item
        * @param aItemIsCurrent whether or not the item is highlighted
        * @param aViewIsEmphasized unused
        * @param aViewIsDimmed unused
        * @param aItemIsSelected whether or not the item is selected
        */
        void DrawActualItem( TInt aItemIndex, 
                             const TRect& aActualItemRect,
                             TBool aItemIsCurrent,
                             TBool aViewIsEmphasized,
                             TBool aViewIsDimmed,
                             TBool aItemIsSelected ) const;

    private: // New methods

        /**
        * Draws the highlight rectangle
        * @since 2.8
        * @param aActualItemRect the screen area occupied by the item
        */
        void DrawHighlight( const TRect& aActualItemRect ) const;

        /**
        * Draws the radio button according to the items current state
        * @since 2.8
        * @param remainingItemRect the remaining are for the item
        * @param aItemIsSelected whether or not the item is selected
        * @param aColor The colour to draw the radio button in
        */
        void DrawRadioButton( const TRect& remainingItemRect, 
                                    TBool aItemIsSelected, const TRgb& aColor ) const;
   
        /**
        * Draws a rectangle for an item.
		* @since 2.8
		* @param the rectangular area to be drawn.
        */
        void DrawItemRect( const TRect& aActualItemRect ) const;

        /**
        * Read list layout
        */
        void ReadLayout();
        
        void CreateIconAndAddToArrayL( CArrayPtr<CGulIcon>*& aIconArray,
                                       const TAknsItemID& aId,
                                       const TInt aColorIndex,
                                       const TDesC& aBmpFile,
                                       const TInt32 aBmp,
                                       const TInt32 aBmpM );        

    private:    // Data

		// Array of icons for different radio button states.
        CArrayPtr<CGulIcon>* iRadioButtonIconArray;

        // Model class providing access to item data
        MCamListboxModel& iModel;

        // Item height read from resource
        TInt iCaptureSetupItemHeight;

        // the parent control of the listbox
        const CCoeControl* iParentControl;

        // Layouts
        TAknTextComponentLayout   iTxtLayout;   
        TAknWindowComponentLayout iBmpLayout;
        TAknTextComponentLayout   iTxtWithRbLayout;
        TAknWindowComponentLayout iBmpWithRbLayout;
        TAknWindowComponentLayout iHighlightLayout;
	};

#endif      // CAMCAPTURESETUPLISTITEMDRAWER_H
            
// End of File
