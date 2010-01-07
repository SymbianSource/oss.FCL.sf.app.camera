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
* Description:  Container for scene setting view
*
*/



#ifndef CAMSCENESETTINGCONTAINER_H
#define CAMSCENESETTINGCONTAINER_H

// INCLUDES
#include "CamContainerBase.h"
#include "CamAppController.h"
#include "CamSettings.hrh"
#include <aknutils.h>

// DATA TYPES
// Container class holding data for animated scene icons
class TCamAnimationData
    {
public:
    /**
    * Set up animation data for scene setting icon 
    * @since 2.8
    * @param aRect The initial highlighted rect
    */
    void Initialise( const TRect& aRect );

    // the current highlighted rect to fade in
    TRect iCurrentRect;
    // old rect to rub out
    TRect iCurrentRectOld;
    // the previous rect to fade out
    TRect iPreviousRect;
    // old rect to rub out
    TRect iPreviousRectOld;
    // the icon rect to fade out
    TRect iIconRectFadeOut;
    // old rect to rub out
    TRect iIconRectFadeOutOld;
    // the icon rect to fade in
    TRect iIconRectFadeIn;
    // old rect to rub out
    TRect iIconRectFadeInOld;
    // the title rect to fade out
    TRect iTitleFadeOut;
    // old rect to rub out
    TRect iTitleFadeOutOld;
    // the title rect to fade in
    TRect iTitleFadeIn;
    // old rect to rub out
    TRect iTitleFadeInOld;
    // the pen color to use fading out
    TRgb iPenFadeOut;
    // the pen color to use fading in
    TRgb iPenFadeIn;
    // icon to fade out
    const CFbsBitmap* iIconFadeOut;
    // icon to fade in
    const CFbsBitmap* iIconFadeIn;
    // moving down the list
    TBool iMovingDown;
    // amount of pixels to animate the fade in/out
    TBool iDeltaY;
    // amount of pixels to scroll the list by
    TBool iScrollingY;
    // current step in the animation
    TInt iStep;
    // amount to add to the left margin when fading icons in/out
    TReal iIconLeftInc;     
    // left margin offset for drawing the icon when fading in/out
    TReal iIconLeftOffset;  
    // off screen bitmap to assist with the scrolling of the list
    CFbsBitmap* iOffScreenBitmap;
    // is the list in scrolling mode
    TBool iScrolling;
    // the position of the off screen bitmap
    TPoint iOffScreenPos;
    };

// the size of the icons used during fading
enum TCamIconSize
    {
    ECamIconSizeLarge,
    ECamIconSizeSmall,
    ECamIconSizeOne,
    ECamIconSizeTwo,
    ECamIconSizeThree,
    ECamIconSizeFour,
    ECamIconSizeFive,
    ECamIconSizeSix
    };

// FORWARD DECLARATIONS
class CAknView;

// CLASS DECLARATION

class CCamSceneSettingItem : public CBase
    {
    public:

        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aSceneId Id of scene to create
        * @param aLargeBmpSize the size of the largest icon to use
        * @param aSmallBmpSize the size of the smallest icon to use
        */
        static CCamSceneSettingItem* NewL( TCamSceneId aSceneId,
                                           const TSize& aLargeBmpSize,
                                           const TSize& aSmallBmpSize ); 

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamSceneSettingItem();

        /**
        * Returns the scene id
        * @return the scene id for this item
        * @since 2.8
        */
        TCamSceneId SceneId() const;
        
        /**
        * returns the bitmap for the given size
        * @param aSize     The bitmap size to return
        * @return The bitmap for the size given
        * @since 2.8
        */
        const CFbsBitmap* Icon( TCamIconSize aSize ) const;

        /**
        * returns the item title
        * @return the item title
        * @since 2.8
        */
        const HBufC* Title() const;

        /**
        * returns the item description
        * @return the item description
        * @since 2.8
        */
        const HBufC* Description() const;


    private:

        /**
        * Symbian OS 2nd phase constructor.
        * @param aLargeBmpSize the size of the largest icon to use
        * @param aSmallBmpSize the size of the smallest icon to use
        * @since 2.8
        */
        void ConstructL( const TSize& aLargeBmpSize, 
                         const TSize& aSmallBmpSize );

        /**
        * C++ constructor
        * @param aSceneId Id of scene to create
        * @since 2.8
        */
        CCamSceneSettingItem( TCamSceneId aSceneId );

    private:
        // the bitmap for non-highlighted items
        CFbsBitmap* iBitmapSmall;
        // bitmap 1 in the fade in/out animation
        CFbsBitmap* iBitmap1;
        // bitmap 2 in the fade in/out animation
        CFbsBitmap* iBitmap2;
        // bitmap 3 in the fade in/out animation
        CFbsBitmap* iBitmap3;
        // bitmap 4 in the fade in/out animation
        CFbsBitmap* iBitmap4;
        // bitmap 5 in the fade in/out animation
        CFbsBitmap* iBitmap5;
        // bitmap 6 in the fade in/out animation
        CFbsBitmap* iBitmap6;
        // the bitmap to use for highlighted items
        CFbsBitmap* iBitmapLarge;
        // the title of the item
        HBufC* iTitle;
        // the description of the icon
        HBufC* iDescription;
        // the scene id of the item
        TCamSceneId iSceneId;

    };


/**
* Container for still image Post capture view
*/
class CCamSceneSettingContainer : public CCamContainerBase, 
                                  public MEikScrollBarObserver
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
        static CCamSceneSettingContainer* NewL( const TRect& aRect, 
                                                 CAknView& aView,
                                                 TCamCameraMode aMode,
                                                 CCamAppController& aController,
                                                 TBool aUserBaseScenes = EFalse );

        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamSceneSettingContainer();

    private:
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * Reads the layout info from the resource
        * @since 2.8
        * @param aResourceId The resource containing the layout info
        * @param aArray The array to add the layout info to.
        */
        void ReadLayoutL( TInt aResourceId, RArray<TAknLayoutRect>& aArray ) const;
        void ReadLayoutL( TInt aResourceId, RArray<TAknLayoutText>& aArray ) const;

        /**
        * C++ constructor
        * @since 2.8
        * @param aMode Current capture mode
        * @param aController reference to CCamAppControllerBase instance
        * @param aView Reference to the view containing this container
        * @param aUserBaseScenes Specifies whether this list should
        * display the base scenes for the user scene.
        */
        CCamSceneSettingContainer( TCamCameraMode aMode, 
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

        /**
        * From MEikScrollBarObserver
        * Called when a new thumbnail has been generated       
        * @since 2.8
        * @param aScrollBar
        * @param aEventType
        */
        void HandleScrollEventL( CEikScrollBar* aScrollBar, TEikScrollEvent aEventType );

    private: 

        /**
        * Sets up the scroll bar, used on start up and when items are deleted
        * to update the state
        * @since 2.8
        */
        void SetupScrollbarL();

        /**
        * Populates iSettingItemArray with scenes
        * and scene bitmaps from the MBM file
        * @param aResourceId resource id for scene array.
        * @since 2.8
        */
        void CreateSceneArrayL( TInt aResourceId );

        /**
        * Sets the current position of the scene array to the element
        * associated with a particular scene id.
        * @param aSceneId the id of the scene.
        * @since 2.8
        */
        void SetCurrentPositionToScene( TCamSceneId aSceneId );

        /**
        * Called to start the amination
        * @since 2.8
        */
        void StartAnimation();

        /**
        * Called when the animation timer expires
        * @since 2.8
        * @param aPtr pointer to the object that called the timer
        * @return Error code
        */
        static TInt TimerExpired( TAny* aPtr );

        /**
        * Animates the list
        * @since 2.8
        */
        void Animate();

        /**
        * Updates the highlight on each call of Animate
        * @since 2.8
        * @returns ETrue if the highlight has finished updating
        */
        TBool UpdateHighlight();

        /**
        * Updates the pen/icons on each call of Animate
        * @since 2.8
        */
        void UpdateAnimationData();

        /**
        * Draws the text on each call of Animate
        * @since 2.8
        * @param aText  The text to draw
        * @param aFont  The font to use for drawing
        * @param aBaseline  The baseline of the text
        * @param aFadeOut Whether text is fading in/out
        */
        void DrawText( const TDesC& aText, const CAknLayoutFont* aFont, 
                       TInt aBaseline, TBool aFadeOut ) const;

        /**
        * Draws the icons on each call of Animate
        * @since 2.8
        * @param aFadeOut whether the icon to draw is fading in/out
        */
        void DrawIcon( TBool aFadeOut );

        /**
        * Draws the highlight on each call of Animate
        * @since 2.8
        */
        void DrawHighlight() const;

        /**
        * Draws the list onto an off screen bitmap for
        * scrolling purposes
        * @since 2.8
        */
        void DrawListL();

        /**
        * Animates the list when scrolling occurs
        * @since 2.8
        * @return ETrue if the scrolling has completed
        */
        TBool ScrollList();

        /**
        * Moves the highlight position ready for animating
        * @since 2.8
        */
        void MoveHighlight();

    private: // data

        // Array containing all pointers to all scene items
        RPointerArray<CCamSceneSettingItem> iSettingItemArray;
        // Arrays to store the layout data for the list
        // large icon positions
        RArray<TAknLayoutRect> iHighlightIcons;
        // default icon positions
        RArray<TAknLayoutRect> iNormalIcons;
        // displaced icon positions
        RArray<TAknLayoutRect> iDisplacedIcons;
        // highlight positions
        RArray<TAknLayoutRect> iHighlightArray;
        // highlighted title positions
        RArray<TAknLayoutText> iHighlightTitles;
        // highlighted decsription positions
        RArray<TAknLayoutText> iHighlightDescriptions;
        // default title positions
        RArray<TAknLayoutText> iNormalTitles;
        // displaced title positions
        RArray<TAknLayoutText> iDisplacedTitles;
        // current highlight position
        TInt iCurrentArrayPosition;
        // previous highlight position
        TInt iPreviousArrayPosition;
        // position in array of item at the top of screen
        TInt iTopDisplayPosition;
        // number of icons to display on screen at any time
        TInt iNumberOfIconsToDisplay;
        // the capture mode
        TCamCameraMode iMode;
        // Specifies whether or not the scene list is for 
        // the user base scenes
        TBool iUserBaseScenes;
        // An animation timer object
        CPeriodic*     iAnimTimer;
        // Animation Data
        TCamAnimationData iAnimData;
        // the scrollbar frame
        CEikScrollBarFrame* iScrollFrame;
        // the model to use for the scrollbar
        TAknDoubleSpanScrollBarModel ivModel;
        // the position of the scrollbar
        TAknLayoutRect iScrollLayout;

    };

#endif // CAMSCENESETTINGCONTAINER_H

// End of File
