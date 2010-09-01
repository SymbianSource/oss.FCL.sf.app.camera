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
* Description:
*
*/


#ifndef CAMLOCATIONSETTINGCONTAINER_H
#define CAMLOCATIONSETTINGCONTAINER_H

//  INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>
#include <eiklbo.h>

#include "MCamAppController.h"
#include "CamSettingValueObserver.h"
#include "CamStaticSettingsModel.h"
// CONSTANTS
const TInt KMaxTextLength = 40;

// FORWARD DECLARATIONS
class CCamCaptureSetupListBox;
class MAknQueryValue;
class MTouchFeedback;

// CLASS DECLARATION

/**
*  Implements the location listbox control and descriptive texts
*
*  @since 2.8
*/
class CCamLocationSettingContainer : public CCoeControl,
                                     public MCamSettingValueObserver,
                                     public MEikListBoxObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aController reference to the app controller
        * @param aCaptureMode the current capture mode
        * @param aParent this control's parent
        * @return pointer to a new quality container
        */
        static CCamLocationSettingContainer* NewL( MCamAppController& aController,
                                                  TCamCameraMode aCaptureMode,
                                                  CCoeControl* aParent );
        
        /**
        * Destructor.
        */
        virtual ~CCamLocationSettingContainer();

    public: // New functions
        
        /**
        * Returns the selected quality
        * @since 2.8
        * @return the selected quality
        */
        TInt SelectedValue() const;

    public: // Functions from base classes

        /**
        * From CCoeControl 
        * Constructs the control from a resource
        * @since 2.8
        * @param aReader the resource reader
        */
        void ConstructFromResourceL( TResourceReader& aReader );

        /**
        * From CCoeControl
        * Standard CCoeControl routine to return the number of component controls
        * @since 2.8
        * @return the number of component controls
        */
	    TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * Standard CCoeControl routine to return the control at a given index
        * @since 2.8
        * @param anIndex index at which to return control
        * @return the control at the given index
        */ 
        CCoeControl* ComponentControl( TInt anIndex ) const;

        /**
        * From CCoeControl
        * Draws the controls
        * @since 2.8
        * @param aRect	the invlaid rect to draw
        */ 
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl
        * Called when the size of the control changes
        * @since 2.8
        */ 
        void SizeChanged();

        /**
        * From CCoeControl
        * Offers a key event to the control
        * @since 2.8
        * @param aKeyEvent the key event offered
        * @param aType the type key event
        * @return whether the key was handled
        */ 
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    
        /**
        * From MCamSettingValueObserver
        * Handles a change in the slider value
        * @since 2.8
        * @param aNewValue the new value of the setting
        */ 
        void HandleSettingValueUpdateL( TInt aNewValue );


    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aController reference to the app controller
        * @param aMode the current capture mode
        * @param aParent this control's parent
        */
        CCamLocationSettingContainer( MCamAppController& aController,
                                     TCamCameraMode aMode,
                                     CCoeControl* aParent );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 2.8
        */
        void ConstructL();
       
        /**
        * Reads the listbox layout from a resource file
        */
        void ReadListboxLayoutL( TRect& aRect );

        /**
	      * Sets up the summary pane detail arrays
	      * @since 2.8
	      * @param aResourceId The resource that defines the summary pane details
	      */
	      void ConstructSummaryDetailsFromResourceL(TInt aResourceId);
        
	      /**
	      * Draws the summary title and description text
	      * @since 2.8
	      * @param aGc The graphics context to draw to
	      */
	      void DrawSummaryText( CWindowGc& aGc ) const;
	       /**
	      * Splits the summary title and description text into multiple lines that fit the status layouts
	      * @since 2.8
	      */
	      void SplitCurrentTextL();

          void ConstructContainerTitleFromResourceL( TInt aResourceId );
          void HandlePointerEventL( const TPointerEvent& aPointerEvent );
          /**
           * From MEikListBoxObserver
           */
          void HandleListBoxEventL(CEikListBox* aListBox, 
                                      TListBoxEvent aEventType );
	      
    private:    // Data
        // the parent control        
        CCoeControl* iParent;
        // the app controller
        MCamAppController& iController;
        
        // the location mode list box
        CCamCaptureSetupListBox* iListBox;
        // the capture mode (video/still)
        TCamCameraMode iMode;

         // Layout of summary outer region (box)
        TAknLayoutRect iSummaryRect;
        // Array of all descriptions texts
        RPointerArray<HBufC> iDescArray;
        // Array of layouts for description lines 
        RArray<TAknLayoutText> iDescLayoutArray;
        // Array of split description lines
        CArrayFixFlat<TPtrC>* iCurrentDescLineArray;
	      
        // The text shown as the title of the container
        HBufC16* iTextTitle;
 
        TBool iActivateOnTouchRelease;
 
    };

#endif      // CAMLOCATIONSETTINGCONTAINER_H   
            
// End of File
