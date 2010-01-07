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
* Description:  A list box that displays settings items with radio buttons*
*/


#ifndef CAMCAPTURESETUPLISTBOX_H
#define CAMCAPTURESETUPLISTBOX_H

//  INCLUDES
 
#include <eiklbx.h>
#include <AknUtils.h>

#ifndef CAMERAAPP_PLUGIN_BUILD
  #include "CamAppController.h"
#else
  #include "MCamAppController.h"
#endif //CAMERAAPP_PLUGIN_BUILD


// FORWARD DECLARATIONS
class MCamSettingValueObserver;

// CLASS DECLARATION

/**
*  Main class of capture setup radio button list box.
*
*  @since 2.8
*/
class CCamCaptureSetupListBox : public CEikListBox
	{
	public:  // Constructors and destructor

        /**
		 * C++ default constructor.
		 */
        CCamCaptureSetupListBox( MCamSettingValueObserver* aObserver
#ifndef CAMERAAPP_PLUGIN_BUILD
        						 ,CCamAppController& aController 
#else
                                 ,MCamAppController& aController
#endif //CAMERAAPP_PLUGIN_BUILD
                                );

        /**
		 * Second phase constructor
		 * @since 2.8
		 * @param aParent Parent control
		 * @param aArrayResourceId Listbox's item array
		 * @param aSettingItem setting item id
		 * @param aFlags Flags. By default 0
		 * @param aShootingModeListBox ETrue if the listbox is shooting mode setting style
		 */
		void ConstructL( const CCoeControl* aParent,
                         TInt aArrayResourceId,
                         TInt aSettingItem,
						 TInt aFlags = 0,						
						 TBool aShootingModeListBox = ETrue,
						 TBool aLocationSettingListBox = EFalse 
						 );
	       
		/**
		 * Destructor.
		 */
		virtual ~CCamCaptureSetupListBox();

    public:     // New methods

        /**
		 * Initialize the list box
		 * @since 2.8
		 * @param aCurrentValueId the current value for the setting being edited
		 */
        void InitializeL( TInt aCurrentValueId );

    public:     // methods from base classes

        /**
        * From CCoeControl. 
        * Handles key events.
        * @since 2.8
        * @param aKeyEvent The key event.
        * @param aType The type of the event.
        * @return Indicates whether or not the key event was used by this control.
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                    TEventCode aType );
                                    
        /**
        * From CCoeControl. 
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);


	private:    // Data

        // observer that handles the control's update events
        MCamSettingValueObserver* iSettingObserver;

        // Layout for icon in list-box
        TAknLayoutRect iIconLayout;       
#ifndef CAMERAAPP_PLUGIN_BUILD
        CCamAppController& iController;
#else
        MCamAppController& iController;
#endif //CAMERAAPP_PLUGIN_BUILD		
	};

#endif // CAMCAPTURESETUPLISTBOX_H
