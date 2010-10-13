/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file for CCamBatteryPaneController class.
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


#ifndef CAMBATTERYPANECONTROLLER_H
#define CAMBATTERYPANECONTROLLER_H

// INCLUDES

#include <e32base.h>
#include "CamPropertyObserver.h" // MPropertyObserver

// CONSTANTS

// Battery strength and animation timer constants.
const TInt KMinBatteryStrength      = 0;
const TInt KMaxBatteryStrength      = 7;
const TInt KBatteryRechargeTick     = 500000;

// FORWARD DECLARATIONS

class CCamBatteryPaneDrawer;
class CCamPropertyWatcher;
class CWindowGc;
class CBitmapContext;

// CLASS DECLARATION

/**
* Abstract API for battery pane observer.
* Derived classes, which have subscribed to CCamBatteryPaneController
* receive notification when the battery pane contents change
*/
class MCamBatteryPaneObserver
    {
    public:    
        /**
        * The battery pane contents have changed
        */
        virtual void BatteryPaneUpdated() = 0;
    };   

/**
* Watches changes in battery strength and charge properties and provides
* animation of recharging. Notifies observer of changes in the battery pane.
* Handles drawing requests by passing them to owned CCamBatteryPaneDrawer
* object.
*/
class CCamBatteryPaneController: public CBase, public MPropertyObserver
    {
    
    public: // Construction and destruction
        /**
        * Descructor
        */
        ~CCamBatteryPaneController();
        /**
        * Two-phased constructor
        * @param aObserver              Battery pane observer
        * @param aCallbackActive        Callback to the observer active
        * @return Pointer to the created 
        */
        static CCamBatteryPaneController* NewL(
                MCamBatteryPaneObserver& aObserver,
                TBool aCallbackActive );
    
    
    public: // From MPropertyObserver
        /**
        * The value of a watched property has changed
        * @param aCategory The category of the property
        * @param aKey the Identifier of the property
        */    
        void HandlePropertyChangedL( const TUid& aCategory, const TUint aKey );
        
               
    public: // New functions
        /**
        * Pause battery pane controller. With value ETrue, stops listening
        * to battery pane events, with EFalse reinitializes and starts
        * listening to the events again.
        */
        void Pause( TBool aPause );        
    
        /**
        * Activate or deactivate callbacks to observer when there
        * are changes in the battery pane.
        * @param aActive Callback to observer active
        */          
        void SetCallbackActive( TBool aActive );
        
        /**
        * Set location in the screen, where the battery pane is
        * to be drawn.
        * @param aLocation Drawing location
        */        
        void SetLocation( TPoint aLocation );
        
        /**
        * Set battery strength.
        * @param aStrength Battery strength
        */
        void SetBatteryStrength( TInt aStrength );
                
        /**
        * Start the recharging animation
        */        
        void StartRecharging();        
        
        /**
        * Stop the recharging animation
        */  
        void StopRecharging();
        
        /**
        * Get battery pane's rectangle
        * @return rectangle
        */
        TRect Rect() const; 
        
        /**
        * Get rectangle of the animated part of battery pane
        * @return rectangle
        */
        TRect AnimationRect() const;                 

        /**
        * Draw the battery pane
        * @param aGc Graphics context
        */                 
        void Draw( CBitmapContext& aGc ) const;
        //void Draw( CWindowGc& aGc ) const;
        
        /**
        * Handle resource change event
        * @param aType Type of the resource change
        */
        void HandleResourceChange( TInt aType );        

        /**
         * Checks if battery level is low.
         * @return ETrue if battery level is low and
         * charger is not connected
         */
        TBool IsBatteryLow();
        
    protected:
        /**
        * Second phase constructor
        */
        void ConstructL();
    
    private:
        /**
        * Constructor.
        * @param aObserver Battery pane observer
        * @param aCallbackActive Callback to the observer active
        */
        CCamBatteryPaneController(
            MCamBatteryPaneObserver& aObserver,
            TBool aCallbackActive );

        /**
        * 
        * @param aThis Pointer to the object owning the ticker
        */
        static TInt TickerCallback( TAny* aThis );        
        
        /**
        * Changes the (charging) battery strenght value as needed to show the next
        * frame of the animation.
        */
        void UpdateRechargeBatteryStrength();
        
        /**
        * Notifies observer of changes in the battery pane,
        * if iCallbackActive is set
        */
        void NotifyObserver();
        
        /**
        * Read current status of the properties. Set battery level
        * and start/stop battery recharging animation accordingly.
        */
        void ReadCurrentState();
        
        /**
        * Start/stop the charging animation, depending on aStatus value
        * @param aStatus Value of the charging status property
        */
        void HandleChargingStatusChange( TInt aStatus );
        
    private:            
        // Current battery strength
        TInt                        iBatteryStrength;
    
        // Recharging animation status
        TBool                       iRecharging;
        TInt                        iRechargeBatteryStrength;
    
        // Object that handles the actual drawing
        CCamBatteryPaneDrawer*      iDrawer;
        
        // Recharge animation timer
        CPeriodic*                  iTicker;
   
        // Observer of the battery pane changes
        MCamBatteryPaneObserver&    iObserver;
        TBool                       iCallbackActive;
    
        // Property watchers for battery charging and strength
        CCamPropertyWatcher*        iBatteryStrengthWatcher;
        CCamPropertyWatcher*        iBatteryChargingWatcher;
        
        TBool                       iPaused;
    };
        
#endif
