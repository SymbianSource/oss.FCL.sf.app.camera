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
* Description:  Header file for CCamBatteryPaneDrawer class.
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/


#ifndef CAMBATTERYPANEDRAWER_H
#define CAMBATTERYPANEDRAWER_H

// INCLUDES

#include <e32base.h>
#include "cambatterypanecontroller.h" // Battery level constants

// FORWARD DECLARATIONS

class CFbsBitmap;
class CBitmapContext;
class CWindowGc;
class CCamBitmapItem;

// CLASS DECLARATION

class CCamBatteryPaneDrawer: public CBase
    {
    public: // Construction and destruction
        /**
        * Destructor
        */  
        virtual ~CCamBatteryPaneDrawer();     
        
        /**
        * Two-phased constructor
        */
        static CCamBatteryPaneDrawer* NewL();
    
    public: // New functions            
        
        /**
        * Set battery strength level used in drawing
        * @param aBatteryStrength Battery Strength )
        */
        void SetBatteryStrength( TInt aBatteryStrength );
        
        /**
        * Set battery pane location used in drawing
        * @param aLocation
        */
        void SetLocation( const TPoint &aLocation );
        
        /**
        * Get battery pane's rectangle
        * @return rectangle
        */
        TRect Rect() const;

        /**
        * Draw the battery pane
        * @param aGc Graphics context
        */
        // void Draw( CWindowGc& aGc ) const;
        void Draw( CBitmapContext& aGc ) const;                       
        
        /**
        * Clear the battery pane indicator
        * @param aGc Graphics context
        */
        void ClearBattery( CBitmapContext& aGc ) const;          
        
        /**
        * Handle resource change event
        * @param aType Type of the resource change
        */
        void HandleResourceChange( TInt aType );         
        
    protected:
        /**
        * Second phase constructor
        */
        void ConstructL();
    
    private:
        /**
        * Constructor
        */
        CCamBatteryPaneDrawer();
        
        /**
        * Load layouts for elements in the battery pane
        */
        void LoadLayoutsL();
        
        /**
        * Load icons needed for drawing the battery pane
        */
        void LoadIconsL();

        /**
        * Delete loaded icons
        */
        void DeleteIcons();
        
        /**
        * Read nontouch layout
        */
        void NonTouchLayoutL();

        /**
        * Read touch layout
        */
        void TouchLayoutL(); 


    private: // Data
        // Battery strength    
        TInt        iBatteryStrength;

        
        RPointerArray<CCamBitmapItem> iBatteryIcons;
        
        // Rectangle of the battery pane
        TRect       iRect;

    };

#endif // CAMBATTERYPANEDRAWER_H    
