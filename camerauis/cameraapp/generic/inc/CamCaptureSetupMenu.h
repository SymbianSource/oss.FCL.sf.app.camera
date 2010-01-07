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
* Description:  Container for the listbox that draws the Capture Setup Menu.*
*/


#ifndef CAMCAPTURESETUPMENU_H
#define CAMCAPTURESETUPMENU_H

// INCLUDES
#include "CamContainerBase.h"
#include <AknPopupFader.h>

// FORWARD DECLARATIONS
class CCamCaptureSetupMenuListBox;        

// CLASS DECLARATION

/**
*  CCamCaptureSetupMenu  container control class.
*  
*/
class CCamCaptureSetupMenu : public CCamContainerBase, public MAknFadedComponent
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aController used by the list box model to get list item values.
        * @param aView the view that this container belongs to.
        * @param aRect the client rectangular area.
        * @param aResourceId the id of the resource that will be used to populate the list box.
        * @param aSelectedItemIndex the index of the selected item.
        * @return pointer to the created capture setup menu container.
        */
        static CCamCaptureSetupMenu* NewL( 
                                CCamAppController& aController,
                                CAknView& aView, 
                                const TRect& aRect, 
                                TInt aResourceId, 
                                TInt aSelectedItemIndex = 0 );
        /**
        * Destructor.
        * @since 2.8
        */
        ~CCamCaptureSetupMenu();

    public: // New functions
        /**
        * Returns the list index of the currently selected item.
        * @since 2.8
        * @return the list index
        */
        TInt CurrentItemIndex() const;

        /**
        * Fades everything behind this menu.
        * @since 2.8
        * @param aFade ETrue if everything is to be faded. Otherwise EFalse.
        */
        void FadeBehind( TBool aFade );

    public: // Functions from base class MAknFadedComponent

        /**
        * Returns the number of components that are not faded
        * when the fader object is active.
        * @since 2.8
        * @return the number of unfaded components.
        */
        TInt CountFadedComponents();

        /**
        * Returns a pointer to a non-faded component.
        * @since 2.8
        * @param aIndex the index of the non-faded component.
        * @return the CCoeControl pointer.
        */
        CCoeControl* FadedComponent(TInt aIndex);

    public: // Functions from base classes

        /**
        * Handles commands.
        * @since 2.8
        * @param aCommand the command id.
        */
        void HandleCommandL( TInt aCommand );


    private: // Constructors.
        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to CCamAppController instance
        * @param aView reference to the view containing this container
        */
        CCamCaptureSetupMenu( CCamAppController& aController,
                                  CAknView& aView );

        /**
        * 2nd phase constructor.
        * @since 2.8
        * @param aRect Frame rectangle for container.
        * @param aResourceId the id of the resource to populate the list box with.
        * @param aSelectedItemIndex the index of the selected item.
        */
        void ConstructL( const TRect& aRect, TInt aResourceId, TInt aSelectedItemIndex );

    private: // Functions from base classes

        /**
        * Returns number of component controls in this container.
        * @since 2.8
        * @return the number of component controls.
        */
        TInt CountComponentControls() const;

        /**
        * Returns a component control.
        * @since 2.8
        * @param aIndex the index of the desired component control.
        * @return the component control.
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * Draws the container.
        * @since 2.8
        * @param aRect the client rectangular area.
        */
        void Draw( const TRect& aRect ) const;

        /**
        * Handles key events.
        * @since 2.8
        * @param aKeyEvent the key event.
        * @param aType the type of key event.
        * @return whether the key has been consumed or not.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );



    private: // New functions

        /**
        * Creates the list box.
        * @since 2.8
        * @param aRect the client rectangular area.
        * @param aResourceId the id of the resource to populate the list.
        * @param aSelectedItemIndex Index of the item to be selected.
        */
        void CreateCaptureSetupListBoxL( TRect& aRect, TInt aResourceId, TInt aSelectedItemIndex );

        /**
        * Returns the command id for activating the current list item control.
        * @since 2.8
        * @return the command id.
        */
        TInt CommandIdForActivatingCurrentItemControl();

        /**
        * Handles the pressing of the softkey selection.
        * @since 2.8
        */
        void HandleSelectionL();

    private: //data

        // The list box.
        CCamCaptureSetupMenuListBox* iCaptureSetupListBox;

        // Fader object used to fade everything except this menu and the CBA.
        TAknPopupFader iPopupFader;

    };

#endif // CAMCAPTURESETUPMENU_H
