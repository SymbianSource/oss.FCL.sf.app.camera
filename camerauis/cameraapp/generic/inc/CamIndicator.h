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
* Description:  Camera Indicator class*
*/


#ifndef CAMINDICATOR_H
#define CAMINDICATOR_H

// INCLUDES
 
#include <e32base.h>

// FORWARD DECLARATIONS
class CCamBitmapItem;

// CLASS DECLARATION
/**
* Camera Indicator class for drawing indicators in the viewfinder
*/
class CCamIndicator : public CBase
    {
    public: // Constructors and destructor

        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aRect frame rectangle for control
        */
        static CCamIndicator* NewL( const TRect& aRect );

        /**
        * Destructor.
        */
        virtual ~CCamIndicator();

    private:

        /**
        * C++ constructor
        * @since 2.8
        * @param aRect frame rectangle for control
        */
        CCamIndicator( const TRect& aRect );

        /**
        * C++ default constructor.
        * @since 
        */
        CCamIndicator();

    public: // New functions
        /**
        * Called to add a bitmap to array of available
        * icons for the camera indicator
        * @since 2.8
        * @param aBitmapId bitmap identifier
        * @param aMaskId mask identifier
        */
        void AddIconL( TInt32 aBitmapId, TInt32 aMaskId );

        // This is needed because location icon is from svg file
        void AddSvgIconL( TInt32 aBitmapId, TInt32 aMaskId, const TSize& aSize );
        
        // update the rect according to new coordinates
        void UpdateRect( TRect& aRect );
        
        /**
        * Called to set the icon (no redraw request)
        * @since 2.8
        * @param aIndex index into available icons for this indicator
        */
        void SetIcon( TInt aIndex );

        /**
        * Called to clear the icon
        * @since 2.8
        */
        void ClearIcon();

        /**
        * Called to show the icon
        * @since 2.8
        */
        void DisplayIcon();

        /**
        * Called to check if indicator is currently flashing
        * @since 2.8
        * @return ETrue if indicator is in flashing state
        */
        TBool IsFlashing() const;

        /**
        * Called to set indicator flashing state
        * @since 2.8
        * @param aFlashing set to ETrue to set flashing state on
        */
        void SetFlashing( TBool aFlashing );

        /**
         * Allows the position to be overriden
         * @since 3.0
         * @param aPos The new position
         */
        void OverridePosition( const TPoint& aPos );
        
        /**
         * Resets the position set by OverridePosition and causes the 
         * indicator to be displayed in the originally speficied position.
         * @since 3.0
         */
        void CancelOverridePosition();

        /**
        * @since 2.8
        * @param aGc The graphics context to use for drawing
        */
        void Draw( CBitmapContext& aGc ) const;
        
        /**
        * Returns indicators layout rect  
        * @since 5.0
        */
        TRect LayoutRect();

        /**
         * Sets new size for all icons 
         * @since S60 5.0
         * @param aRect the new size for the icons
         */
        void SetRect( const TRect& aRect );

private:

    private: // Data
        RPointerArray<CCamBitmapItem> iIcons; // array of available icons
        TInt iCurrentIcon; // index into icon array
        TBool iClear; // set to ETrue to clear the indicator

        TBool iFlashing; // set to ETrue for a flashing indicator

        // rect of the indicator in the viewfinder
        TRect iRect;
        
        // stores the originally specified (ie not overridden) rect
        TRect iDefaultRect;

    };

#endif

// End of File
