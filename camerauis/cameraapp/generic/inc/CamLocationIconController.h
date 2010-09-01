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
* Description:  Header file for CCamLocationIconController class.
*
*  Copyright © 2007 Nokia.  All rights reserved.
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


#ifndef CAMLocationIconCONTROLLER_H
#define CAMLocationIconCONTROLLER_H

// INCLUDES

#include <e32base.h>
#include "CamPropertyObserver.h" // MPropertyObserver
#include "CamControllerObservers.h"

// CONSTANTS

const TInt KLocationSettingStateOff = 0;
const TInt KLocationSettingStateOn = 1;

// FORWARD DECLARATIONS

class CCamAppController;
class CCamIndicator;
class CCamPropertyWatcher;
class CWindowGc;
class CBitmapContext;
class CEikonEnv;

// CLASS DECLARATION

/**
* Abstract API for location icon observer.
* Derived classes, which have subscribed to CCamLocationIconController
* receive notification when the location icon contents change
*/
class MCamLocationIconObserver
    {
    public:    
        /**
        * The location icon contents have changed
        */
        virtual void LocationIconUpdated() = 0;
    };   

/**
* Watches changes in location state properties.
* Notifies observer of changes in the location indocator.
* Handles drawing requests by passing them to owned CCamLocationIconDrawer
* object.
*/
class CCamLocationIconController: public CBase, public MPropertyObserver, public MCamControllerObserver
    {
    
    public: // Construction and destruction
        /**
        * Descructor
        */
        ~CCamLocationIconController();
        /**
        * Two-phased constructor
        * @param aObserver location indicator observer
        * @param aCallbackActive Callback to the observer active
        * @return Pointer to the created 
        */
        static CCamLocationIconController* NewL( CCamAppController& aController, 
                                                 MCamLocationIconObserver& aObserver, 
                                                 TBool aCallbackActive, TRect& aRect );
    
    
    public: // From MPropertyObserver
        /**
        * The value of a watched property has changed
        * @param aCategory The category of the property
        * @param aKey the Identifier of the property
        */    
        void HandlePropertyChangedL( const TUid& aCategory, const TUint aKey );
        
               
    public: // New functions
        /**
        * Activate or deactivate callbacks to observer when there
        * are changes in the location indicator.
        * @param aActive Callback to observer active
        */          
        void SetCallbackActive( TBool aActive );
        
        /**
        * Set location state.
        * @param aState location state
        */
        void SetLocationState( TInt aState );
                
        /**
        * location indicator's rectangle
        * @return rectangle
        */
        TRect Rect() const; 
        
        /**
        * Draw the location icon
        * @param aGc Graphics context
        */                 
        void Draw( CBitmapContext& aGc ) const;
        //void Draw( CWindowGc& aGc ) const;
        
        /**
        * From MCamControllerObserver
        * @since 2.8
        * @param aEvent The enumerated code for the event received
        * @param aError The error code associated with the event
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                            TInt aError );

    protected:
        /**
        * Second phase constructor
        */
        void ConstructL( TRect& aRect );
    
    private:
        /**
        * Constructor.
        * @param aObserver location indocator observer
        * @param aCallbackActive Callback to the observer active
        */
        CCamLocationIconController( CCamAppController& aController, 
                                    MCamLocationIconObserver& aObserver, 
                                    TBool aCallbackActive );
        
        void CreateLocationIndicatorL();
        void UpdateRect( TBool aSecondCameraEnabled );
        void UpdateRect();
        //TRect Rect() const;
        
        /**
        * Notifies observer of changes in the location indicator,
        * if iCallbackActive is set
        */
        void NotifyObserver();
        
        /**
        * Read current status of the properties. Set location state accordingly
        */
        void ReadCurrentState();
        
    private:
        CCamAppController&           iController; 
        // Array of pointer to the location indicators
        RPointerArray<CCamIndicator> iLocationIndicators;
        // The current location indicator
        TInt                         iLocationState;
        const CEikonEnv *            iEnv;
        // Observer of the location indicator changes
        MCamLocationIconObserver&    iObserver;
        TBool                        iCallbackActive;
        TRect                        iRect;

        // Property watchers for location state changes
        CCamPropertyWatcher*        iLocationStateWatcher;
    };

#endif
