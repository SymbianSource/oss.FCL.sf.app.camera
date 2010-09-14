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
* Description:  Container for burst thumbnail view
*/



#ifndef CAMBURSTTHUMBNAILCONTAINER_H
#define CAMBURSTTHUMBNAILCONTAINER_H

// INCLUDES
#include "CamContainerBase.h"

// FORWARD DECLARATIONS
class CCamAppController;
class CAknView;
class CCamBurstThumbnailGrid;
class CCamBurstThumbnailGridModel;
class MTouchFeedback;

// CLASS DECLARATION

/**
* Container for burst thumbnail view
*/
class CCamBurstThumbnailContainer : public CCamContainerBase
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aController reference to CCamAppController instance
        * @param aView reference to the view containing this container
        * @param aRec Frame rectangle for container.        
        * @param aGridModel Model representing the grid contents
        * @return pointer to the created CCamBurstThumbnailContainer object
        */
        static CCamBurstThumbnailContainer* NewL( CCamAppController& aController,
                                                  CAknView& aView,
                                                  const TRect& aRec,
                                                  CCamBurstThumbnailGridModel& aGridModel );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamBurstThumbnailContainer();

        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to CCamAppController instance
        * @param aView reference to the view containing this container
        * @param aGridModel Model representing the grid contents
        */
        CCamBurstThumbnailContainer( CCamAppController& aController,
                                     CAknView& aView,
                                     CCamBurstThumbnailGridModel& aGridModel );

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
         * From CoeControl
         */
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );


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
        * @return the requested control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;


        /**
        * From CCoeControl
        * @since 2.8
        * @param aRect area where to draw
        */
        void Draw( const TRect& aRect ) const;

    private: // Data
        // Pointer to the CCoeControl for the thumbnail grid
        CCamBurstThumbnailGrid* iGridControl;   

        // The model describing the contents of the grid control
        CCamBurstThumbnailGridModel& iGridModel;
        
        // touch feedback interface for providing tactile feedback
        MTouchFeedback* iFeedback; 
    };

#endif // CAMBURSTTHUMBNAILCONTAINER_H

// End of File
