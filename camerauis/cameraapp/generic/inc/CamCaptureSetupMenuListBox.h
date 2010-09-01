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
* Description:  Encapsulates the Capture Setup Menu listbox.*
*/


#ifndef CAMCAPTURESETUPMENULISTBOX_H
#define CAMCAPTURESETUPMENULISTBOX_H

//  INCLUDES
#include <eiklbx.h>
//#include <aknlists.h>
#include <avkon.hrh>
// FORWARD DECLARATIONS
class CCamCaptureSetupMenuListBoxModel;
class CCamAppController;

// CLASS DECLARATION

/**
*  Capture setup menu listbox
*  Main class of list box
*
*/
class CCamCaptureSetupMenuListBox : public CEikListBox
	{
	public:  // Constructors and destructor
	    
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aParent the container for this list box
        * @param aReader the resource readerthat will be used to populate the list box.
        * @param aController used by the list box model to get list item values.
        * @param aSelectedItemIndex the index of the list item that should be highlighted.
        * @return pointer to the created list item.
        */
        static CCamCaptureSetupMenuListBox* NewL( const CCoeControl* aParent, TResourceReader& aReader, CCamAppController& aController, TInt aSelectedItemIndex );
    
        /**
        * Destructor.
        * @since 2.8
        */
		~CCamCaptureSetupMenuListBox();

	public: // New functions

        /**
        * Returns the settings model id that corresponds with the
        * specified list item index.
        * @since 2.8
        * @param aItemIndex the index of the list item.
        * @return the settings model id associated with the list item.
        */
        TInt ItemSettingsId( TInt aItemIndex );
	
	protected:  // Functions from base class CEikListBox
        
        /**
        * Returns instance to view
        * @since 2.8
        * @return Instance to CListBoxView
        */
        virtual CListBoxView* MakeViewClassInstanceL();

    public: // Functions from base class CEikListBox

        /**
        * Handles key events. If a control wishes to process key events, it should 
        * implement this function. The implementation must ensure that the 
        * function returns EKeyWasNotConsumed if it does not do anything in 
        * response to a key event otherwise, other controls or dialogs may be 
        * prevented from receiving the key event. If it is able to process the 
        * event it should return EKeyWasConsumed. When a key event occurs, the 
        * control framework calls this function for each control on the control 
        * stack, until one of them can process the key event 
        * (and returns EKeyWasConsumed).
        * @since 2.8
        * @param aKeyEvent The key event
        * @param aType The type of key event: EEventKey, EEventKeyUp or EEventKeyDown
        * @return Indicates whether or not the key event was used by this control
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    private:    // Constructors

        /**
        * Constructor
        * @since 2.8
        */
		CCamCaptureSetupMenuListBox();

        /**
        * 2nd phase constructor
        * @since 2.8
        * @param aParent the container for this list box
        * @param aReader the resource readerthat will be used to populate the list box.
        * @param aController used by the list box model to get list item values.
        * @param aSelectedItemIndex the index of the list item that should be highlighted.
        */
		void ConstructL( const CCoeControl* aParent, TResourceReader& aReader, 
            CCamAppController& aController, TInt aSelectedItemIndex );

    private:    // New functions

	private:    // Data
	};

#endif //CAMCAPTURESETUPMENULISTBOX_H
