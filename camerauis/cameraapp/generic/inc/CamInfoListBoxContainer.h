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
* Description:  Setting page for Image/Video quality.*
*/


#ifndef CAMINFOLISTBOXCONTAINER_H
#define CAMINFOLISTBOXCONTAINER_H

//  INCLUDES
#include "CamContainerBase.h"
#include "CamAppController.h"
#include "CamSettingValueObserver.h"
#include "CamSettings.hrh"
#include <AknUtils.h>
#include <eiklbo.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAknQueryValue;
class CCamInfoListBox;
class CAknInfoPopupNoteController;
class MTouchFeedback;

// CLASS DECLARATION

/**
* Implements a general setting control, with a listbox with choices 
* in the left and a icon and highlighted item description in the right.
*
*  @since 2.8
*/

class CCamInfoListBoxContainer : public CCamContainerBase, 
                                 public MCamSettingValueObserver,
                                 public MEikListBoxObserver
            {
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aRect Frame rectangle for container.
        * @param aView Reference to the view containing this container
        * @param aController reference to CCamAppControllerBase instance
        * @param aListBoxResource Listbox resource data
        * @param aSummaryResource Summary resource data
        * @param aSettingValue Initial setting value
        */
        static CCamInfoListBoxContainer* NewL( const TRect& aRect, 
                                                 CAknView& aView,
                                                 CCamAppController& aController,
                                                 TInt aListBoxResource,
                                                 TInt aSummaryResource,
                                                 TInt aSettingValue, 
                                                 TInt aTitleResource,
                                                 TBool aSkinnedBackGround = ETrue );
        

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamInfoListBoxContainer();

    private:
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aRect Frame rectangle for container.
        * @param aListBoxResource
        * @param aSummaryResource
        */
        void ConstructL( const TRect& aRect, TInt aListBoxResource, TInt aSummaryResource, TInt aTitleResource  );
        
        /**
        * C++ constructor
        * @since 2.8
        * @param aController reference to CCamAppControllerBase instance
        * @param aView Reference to the view containing this container
        * @param aSettingValue Initial setting value
        * display the base scenes for the user scene.
        */
        CCamInfoListBoxContainer(   CCamAppController& aController,
                                    CAknView& aView,
                                    TInt aSettingValue, TBool aSkinnedBackGround = EFalse );

    public: 
        
        /**
        * Gets the settings item value ID of the current item
        * @since 2.8
        * @return the settings item value of the currently highlighted item
        */
        TInt CurrentSettingItemValue() const;
        
        /**
        * Returns ETrue if the setting item value has changed
        * @since 2.8
        * @return setting value change status
        */        
        TBool SettingValueChanged() const;
        
        /**
        * Gets the index in the item array that has the specified settings 
        * item value ID of the current item
        * @since 3.0
        * @return the index of the item with the specified value id
        */
        TInt IndexForValueId( TInt aValueId ) const;
        
   public: //Functions from base classes
        /**
        * From MCamSettingValueObserver
        * Handles a change in the slider value
        * @since 2.8
        * @param aNewValue the new value of the setting
        */ 
        void HandleSettingValueUpdateL( TInt aNewValue );
        
 
    protected: // Functions from base classes
        /**
        * From CoeControl
        * @since 2.8
        * @return number of contained controls
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl
        * @since 2.8
        * @param aIndex The index of the required control
        * @return Returns the requested control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl
        * @since 2.8
        * @param aRect area where to draw
        */
        void Draw( const TRect& aRect ) const;

        /**
        * From CamContainerBase 
        * @since 2.8
        * @param aKeyEvent the key event
        * @param aType the type of the event
        * @return TKeyResponse key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

    private: 
        /**
        * Sets up the summary pane detail arrays
        * @since 2.8
        * @param aResourceId The resource that defines the summary pane details
        */
        void ConstructSummaryDetailsFromResourceL(TInt aResourceId);

        
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );
        /**
         * From MEikListBoxObserver
         */
        void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
        void ShowTooltipL();
        void ConstructContainerTitleFromResourceL( TInt aResourceId );
        
        /**
        * Draws the summary title and description text
        * @param aGc The graphics context to draw to
        */
        void DrawSummaryTextL( CWindowGc& aGc ) const;
        
        /**
        * Read touch layout
        * @return TRect, listbox rectangle
        */
        TRect TouchLayout();

        /**
        * Read non-touch layout
        * @return TRect, listbox rectangle
        */
        TRect NonTouchLayout();
        
    private: // data
        
        // The listbox itself
				CCamInfoListBox* iListBox;

        // Array of bitmaps for the large summary icon
        RPointerArray<CFbsBitmap> iSummaryBitmapArray;        
        // Array of all shooting mode titles
        RPointerArray<HBufC> iTitleArray;
        // Array of all shooting mode descriptions
        RPointerArray<HBufC> iDescArray;
             
        TInt iSettingValue;
        
        		
		// The text shown as the title of the container
        HBufC16* iListboxTitle;
        
        // info tooltip
        CAknInfoPopupNoteController* iTooltipController;
        TBool iShowTooltip;
        TInt iTooltipIndex;
        
        // Layout rect for title text
        TAknLayoutText iTitleTextRectLayout;  
        TAknLayoutRect iListboxLayoutRect; 
        TRect iLayoutAreaRect;  
        TRect iExplanationRect; 
        TInt iExplLineCount; // Number of available layouts
        TAknLayoutRect iExplIconLayout;  
        TAknLayoutText iExplTitleLayout;
        RArray<TAknLayoutText> iExplTxtLinesLayout;
        CArrayFixFlat<TPtrC>* iCurrentDescLineArray; // own
        
        TBool iActivateOnTouchRelease;

        TBool iSkinnedBackGround;        
        
    };
    
#endif //CAMINFOLISTBOXCONTAINER_H
