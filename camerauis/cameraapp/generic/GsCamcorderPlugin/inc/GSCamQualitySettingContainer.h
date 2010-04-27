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
* Description:  Implements the quality slider for image and video settings*
*/


#ifndef GSCAMQUALITYSETTINGCONTAINER_H
#define GSCAMQUALITYSETTINGCONTAINER_H

//  INCLUDES
#include <bldvariant.hrh>
#include <coecntrl.h>
#include <AknUtils.h>
#include "CamSettingValueObserver.h"
#include "GSCamQualitySettingSlider.h"
#include "CamControllerObservers.h"

// CONSTANTS
const TInt KMaxTextLength = 40;

// FORWARD DECLARATIONS
class CCamCaptureSetupSlider;
class MAknQueryValue;

class CCamConfiguration;
class MTouchFeedback;

enum TCamCameraStateFlags
  {
  ECamIdle      =    0,
  ECamReserved  = 1<<0,
  ECamPowerOn   = 1<<1,
  ECamVFOn      = 1<<2,
  ECamImageOn   = 1<<3,
  ECamVideoOn   = 1<<4
//  ECamCaptureOn = 1<<5
  };
/**
*  Implements the quality slider control and descriptive texts
*
*  @since 2.8
*/
class CGSCamQualitySettingContainer : public CCoeControl,
                                      public MCamSettingValueObserver,
                                      public MCamControllerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aController reference to the app controller
        * @param aMode the current capture mode
        * @param aParent this control's parent
        * @return pointer to a new quality container
        */
        static CGSCamQualitySettingContainer* NewL( MCamAppController& aController,
                                                    TCamCameraMode aMode,
                                                    CCoeControl* aParent );
        
        /**
        * Destructor.
        */
        virtual ~CGSCamQualitySettingContainer();

    public: // New functions
        
        /**
        * Returns the selected quality
        * @since 2.8
        * @return the selected quality
        */
        TInt SelectedQuality() const;

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

    public: // From MCamControllerObserver
        /**
        * From MCamControllerObserver
        * Handles an event from the controller
        * @since 2.8
        * @param aEvent the event that the controller sends out
        * @param aError KErrNone if the operation that cause the event was successful
        */ 
        void HandleControllerEventL( TCamControllerEvent aEvent, TInt aError );
        
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
        CGSCamQualitySettingContainer( MCamAppController& aController,                                     
                                       TCamCameraMode aMode,
                                       CCoeControl* aParent );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 2.8
        */
        void ConstructL();

        /**
        * Reads the layout from a resource file
        * @since 2.8
        */
        void ReadLayoutL();

        /**
        * Draws the quality description texts for image/video
        * @since 2.8
        * @param aGc The graphics context to draw to
        */
        void DrawQualityTexts( CWindowGc& aGc, TRgb aColor ) const;

        /**
        * Draws the storage icon for image/video
        * @since 2.8
        * @param aGc The graphics context to draw to
        */
        void DrawStorageIcon( CWindowGc& aGc ) const;

        /**
        * Draws the remaining image count/video time remaining
        * @since 2.8
        * @param aGc The graphics context to draw to
        */
        void DrawRemainingText( CWindowGc& aGc ) const;

        /**
        * Queries the controller to update the remaining time text
        * @since 2.8
        */
        void UpdateRemainingTextL();

        /**
        * Resets the remaing time/images text.
        */
        void ResetRemainingTextL();
        
        /**
        * Populates a text array from the resource definition of available
        * quality texts using the supported quality values
        * @since 3.0
        * @param aReader the resource reader with the array of available texts
        * @param aArray the array to populate
        */
        void ReadInTextArrayL( TResourceReader& aReader, CDesCArrayFlat& aArray );
                
    public:     // Data
    
    protected:  // Data

    private:    // Data
        // the app controller interface
        MCamAppController& iController;
        
        // the parent control        
        CCoeControl* iParent;
        // the selected quality settings
        TInt iSelectedQuality;
        // phone storage location bitmap
        CFbsBitmap* iPhoneIcon;
        // phone storage location mask
        CFbsBitmap* iPhoneIconMask;
        // MMC storage location bitmap
        CFbsBitmap* iMMCIcon;
        // MMC storage location mask
        CFbsBitmap* iMMCIconMask;
        // mass storage storage location bitmap
        CFbsBitmap* iMassStorageIcon;
        // mass storage storage location mask
        CFbsBitmap* iMassStorageIconMask;
        // The text in the title pane
        HBufC* iTextTitle;
        // The subtitle text at top of page
        HBufC* iTextTopSubtitle;
        // The subtitle text at bottom of page
        HBufC* iTextBottomSubtitle;
        // The title for the remaining time text
        HBufC* iTextRemainingTitle;
        // The remaining count/time text
        TBuf<KMaxTextLength> iTextRemaining;
        // Format string for remaining time
        HBufC* iTimeFormat;

        // Array of all quality titles
        CDesCArrayFlat* iTitleArray;
        // Array of all quality descriptions
        CDesCArrayFlat* iDescArray;
        // Array of the enumeration mapping to the titles
        RArray<TInt> iEnumArray; 
        // Array of quality description lines
        CArrayFixFlat<TPtrC>* iCurrentDescLineArray;
        // The layout of the quality title
        TAknLayoutText iLayoutSettingTitle;
        // the layout for the rect containing all description text
        TAknLayoutRect iLayoutDescRect;
        // Array of layouts for each of the lines in the quality decription
        RArray<TAknLayoutText> iLayoutSettingDescLines;
        // The layout for the remaining title text
        TAknLayoutText iLayoutRemainingTitle;
        // The layout for the remaining images/video time
        TAknLayoutText iLayoutRemainingText;
        // The layout for the storage icon
        TAknLayoutRect iLayoutIcon;
        // The slider control
        CGSCamQualitySettingSlider* iSlider;
        // The current capture mode (video/still)
        TCamCameraMode iMode;
        
        // Array of supported qualities
        RArray<TInt> iSupportedQualitiesArray;
        
        // ETrue if remaining video recording time for current video quality
        // is known, so remaining time and storage icon can be drawn.
        TBool iRemainingVideoTimeKnown;
        
        // Configuration Manager        
        CCamConfiguration* iConfiguration;
        
        MTouchFeedback* iFeedback; // not own
    };

#endif      // GSCAMQUALITYSETTINGCONTAINER_H   
            
// End of File
