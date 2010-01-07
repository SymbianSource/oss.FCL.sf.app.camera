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
* Description:  Setting page for Image/Video quality.*
*/


#ifndef CAMSHOOTINGMODECONTAINER_H
#define CAMSHOOTINGMODECONTAINER_H

//  INCLUDES
#include "CamContainerBase.h"
#include "CamAppController.h"
#include "CamSettingValueObserver.h"
#include "CamSettings.hrh"
#include <AknUtils.h>
#include <AknInfoPopupNoteController.h> 
#include <eiklbo.h>


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MAknQueryValue;
class CCamSceneListBox;
class CAknInfoPopupNoteController;
class MTouchFeedback;

// CLASS DECLARATION

/**
*  Implements the Shooting Mode Setting control and descriptive texts
*
*  @since 2.8
*/


class CCamShootingModeContainer : public CCamContainerBase, 
                                  public MCamSettingValueObserver,
                                  public MEikListBoxObserver
            {
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aRect Frame rectangle for container.
        * @param aView Reference to the view containing this container
        * @param aMode Current capture mode
        * @param aController reference to CCamAppControllerBase instance
        * @param aUserBaseScenes Specifies whether this list should
        * display the base scenes for the user scene.
        */
        static CCamShootingModeContainer* NewL( const TRect& aRect, 
                                                 CAknView& aView,
                                                 TCamCameraMode aMode,
                                                 CCamAppController& aController,
                                                 TBool aUserBaseScenes = EFalse );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamShootingModeContainer();

    private:
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );
        
        /**
        * C++ constructor
        * @since 2.8
        * @param aMode Current capture mode
        * @param aController reference to CCamAppControllerBase instance
        * @param aView Reference to the view containing this container
        * @param aUserBaseScenes Specifies whether this list should
        * display the base scenes for the user scene.
        */
        CCamShootingModeContainer( TCamCameraMode aMode, 
                                    CCamAppController& aController,
                                    CAknView& aView,
                                    TBool aUserBaseScenes );

    public: // New functions
        /**
        * Is the user scene currently selected
        * @since 2.8
        * @return ETrue if user scene highlighted
        */
        TBool UserSceneHighlighted();

        /** 
        * Saves the selected scene in dynamic settings
        * @since 2.8
        * @return ETrue if the scene setting was changed successfully.
        */
        TBool SaveSceneSettingL();

        /** 
        * Saves the selected scene in dynamic settings if the scene
        * mode has changed from current value.
        * @since S60 v5.0
        * @return ETrue if the scene setting was changed successfully.
        */
        TBool SaveSceneSettingIfChangedL();

        /**
        * Gets the settings item value ID of the current item
        * @since 2.8
        * @return the settings item value of the currently highlighted item
        */
        TInt CurrentSettingItemValue() const;
        
        /**
        * Gets the index in the item array that has the specified settings 
        * item value ID of the current item
        * @since 3.0
        * @return the index of the item with the specified value id
        */
        TInt IndexForValueId( TInt aValueId ) const;
        
        /**
         * Hilights the given item on scene listbox
         * @return returns false if item cannot be selected
         */
        TBool SelectItem( TInt aItemIndex );
        
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
      
      
      /**
      * Select the resource for summary texts.
      * @return Resource id of the summaries.
      */
      TInt SelectSummaryResourceL();

      /**
      * Get if the given scene is supported.
      */
      TBool IsSupportedScene( TInt aSceneId ) const;

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

    public:
     
     /**
     * From CCoeControl
     */
     void HandlePointerEventL( const TPointerEvent& aPointerEvent );
     
     /**
      * From MEikListBoxObserver
      */
     void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
     /**
     * Displays tooltip for the selected listbox item
     */
     void ShowTooltipL();
     
     /**
     * Hides tooltip for the selected listbox item, if one is visible
     */
     void HideCurrentTooltipL();
     
     /**
     * Reads the title text from given resource
     * @param aResourceId The resource that defines the summary pane details
     */
     void ConstructContainerTitleFromResourceL( TInt aResourceId );
     
        
    private: // data
        
      // the shooting mode list box
      CCamSceneListBox* iListBox;

      // the capture mode (video/still)
      TCamCameraMode iMode;
      // Specifies whether or not the scene list is for 
      // the user base scenes
      TBool iUserBaseScenes;
      // Array of bitmaps for the large summary icon
      RPointerArray<CFbsBitmap> iSummaryBitmapArray;        
      // Array of all shooting mode titles
      RPointerArray<HBufC> iTitleArray;
      // Array of all shooting mode descriptions
      RPointerArray<HBufC> iDescArray;
       
      
      // Array of supported scenes
      RArray<TInt> iSupportedScenes;

        // The text shown as the title of the container
      HBufC* iTitleText;

      // Layout rect for title text
      TAknLayoutText iTitleTextRectLayout;  
      TAknLayoutRect iListboxLayoutRect; 

      TRect iLayoutAreaRect;  // Scene setting
      TRect iExplanationRect; 
      TInt iExplLineCount;  // Number of available layouts
      TAknLayoutRect iExplIconLayout; 
      TAknLayoutText iExplTitleLayout;
      RArray<TAknLayoutText> iExplTxtLinesLayout;
      CArrayFixFlat<TPtrC>* iCurrentDescLineArray; // own
        
      CAknInfoPopupNoteController* iTooltipController;
      TBool iShowTooltip;
      HBufC16* iListboxTitle;
      TInt iTooltipIndex;
        
      TBool iActivateOnTouchRelease;
      MTouchFeedback* iFeedback; // not own 

    };
    
#endif //CAMSHOOTINGMODECONTAINER_H
