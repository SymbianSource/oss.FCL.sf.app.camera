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
* Description:  Container for standby mode*
*/



#ifndef CAMSTANDBYCONTAINER_H
#define CAMSTANDBYCONTAINER_H

// INCLUDES
#include <AknUtils.h>
#include "CamContainerBase.h"
#include "CamAppController.h"

// FORWARD DECLARATIONS
class CAknView;
class CCamTimerBase;

// CLASS DECLARATION

/**
* Container for standby mode
*/
class CCamStandbyContainer : public CCamContainerBase
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aRect Frame rectangle for container.
        * @param aView Reference to the view containing this container
        * @param aController reference to CCamAppControllerBase instance
        * @param aError associated error code
        */
        static CCamStandbyContainer* NewL( const TRect& aRect, 
                                                 CAknView& aView,
                                                 CCamAppController& aController,
                                                 TInt aError );
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamStandbyContainer();
    
    public: // Functions from base classes

        /**
        * From CamContainerBase 
        * @since 2.8
        * @param aKeyEvent the key event
        * @param aType the type of the event
        * @return TKeyResponse key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

        /**
  	    * From CCamContainerBase
        * Handles the foreground/background event
        * @since 2.8
        * @param aForeground ETrue if this coming to foreground, else EFalse.
        */
        void HandleForegroundEventL( TBool aForeground );

        /**
         * From CCoeControl
         * Handles pointer events
         * @since S60 5.0
         * @param aPointerEvent containing pointer event information
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        
    public: // New functions
    
        /**
        * Gets the current error
        * @since 2.8
        */
        TInt GetStandbyError() const;

    private:
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aRect Frame rectangle for container.
        * @param aError associated error code
        */
        void ConstructL( const TRect& aRect, TInt aError );

        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to CCamAppControllerBase instance
        * @param aView Reference to the view containing this container
        * @param aError associated error code
        */
        CCamStandbyContainer( CCamAppController& aController,
                                    CAknView& aView, TInt aError );

    protected: // Functions from base classes
        /**
        * From CoeControl
        * @since 2.8
        * @return number of contained controls
        */
        TInt CountComponentControls() const;

        /**
        * From CoeControl
        * @since 2.8
        * @param aIndex The index of the control required
        * @return The requested control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl
        * @since 2.8
        * @param aRect area where to draw
        */
        void Draw( const TRect& aRect ) const;

        /**
        * Static callback function for foreground timer timeout.
        * @since 2.8
        * @param aPtr Pointer to an instance of the CCamStandbyContainer
        * @return TInt to indicate if function should be called again
        */
        static TInt ForegroundTimeout( TAny* aPtr );

        /**
        * Foreground timeout expiry - invoke exit from standby mode
        * after foreground event has occurred
        * @since 2.8
        * @return TInt to indicate if function should be called again
        */
        TInt DoForegroundTimeout();

        /**
        * Create background context (iBgContext).
        * @since S60 v5.0
        */
        void CreateBackgroundContextL();

    private: // data
        // text to display in container - loaded from resource
        CEikLabel *iText;

        // associated error code
        TInt iStandbyError;

        // timer used after gaining foreground
        CCamTimerBase* iForegroundTimer;

        // label text, wrapped over two lines if required
        HBufC* iWrappedLabelText; // owned

    };

#endif      // CAMSTANDBYCONTAINER_H

// End of File
