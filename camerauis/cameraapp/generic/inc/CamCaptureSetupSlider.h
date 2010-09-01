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
* Description:  Class for showing the EV slider*
*/



#ifndef __CCAMCAPTURESETUPSLIDER_H
#define __CCAMCAPTURESETUPSLIDER_H

//  INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>                       // for TAknLayoutText
 
#include "CamSettingsInternal.hrh"
#include "CamSettings.hrh"  
#include "Cam.hrh"
#include "CamSettingValueObserver.h"


/**
* Utility class used to group together related layout information into one
* object, and construct itself using a supplied resource reader.
*
*  @since 2.8
*/
class CCamSliderLegend : public CBase
    {
public:
    virtual ~CCamSliderLegend();

    CCamSliderLegend( TBool aFullySkinned=EFalse );
    /**
    * Constructs the layout information for a text object
    * @since 5.0
    * @param aRect The parent rect for the layout entries
    * @param aLayout Layout for the text
    */
    void ConstructTextL(
            TResourceReader& aReader,
            const TRect& aRect,
            const TAknTextComponentLayout& aLayout );
 
    /**
    * Constructs the layout information for an icon object
    * @since 5.0
    * @param aRect The parent rect for the layout entries
    * @param aLayout Layout for the icon
    */
    void ConstructIconL(
            TResourceReader& aReader,
            const TRect& aRect,
            const TAknWindowComponentLayout& aLayout );

    /**
    * Draws the legend
    * @since 2.8
    * @param aGc The window contest to use for the drawing 
    */
    void Draw( CWindowGc& aGc ) const;

    /**
    * Returns the rect for the legend
    * @since 2.8
    * @return The rect for the legend
    */
    TRect Rect() const;
    
private:
    TCamSliderLegendItem iItem;
    TAknLayoutText iPosition; 
    HBufC* iText;
	TAknLayoutRect iIconRect;
	CFbsBitmap* iBitmap;
	CFbsBitmap* iMask;
    TBool iFullySkinned;
    };



/**
*  Slider for showing Exposure Value in capture settings 
*
*  @since 2.8
*/
class CCamCaptureSetupSlider : public CCoeControl
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCamCaptureSetupSlider* NewL( const CCoeControl* aParent, 
                                             MCamSettingValueObserver* aObserver,
                                             TCamSettingItemIds aSettingType,
                                             TInt aSteps,
                                             TBool aFullySkinned=EFalse );
        
        /**
        * Destructor.
        */
        virtual ~CCamCaptureSetupSlider();

    public: // New functions
        
        /**
        * Sets up the range and initial value of the slider
        * @since 2.8
        * @param aValue The value to start with on the slider.                  
        */        
        void InitializeL( TInt aValue );

        /**
        * Returns the minium slider value
        * @since 2.8
        * @return The minimum slider value                  
        */  
        TInt MinValue() const;

        /**
        * Returns the maximum slider value
        * @since 2.8
        * @return The maximum slider value                  
        */  
        TInt MaxValue() const;

                
        /**
        * Sets the minimum and maximum values of the slider control
        * @since 3.0
        * @param aMin The minimum slider value
        * @param aMax The maximum slider value
        */              
        void SetRange( TInt aMin, TInt aMax );

    public: // Functions from base classes

        /**
        * From CCoeControl
        * @since 2.8
        * @param aRect Rect indicating the specific area to draw
        */        
        void Draw( const TRect& aRect ) const;

        /**
        * From CCoeControl
        * @since 2.8
        * @param aKeyEvent Details of the key event
        * @param aType The keypress type
        * @return TKeyResponse whether the key event was handled or ignored
        */     
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType ) ;

        /**
        * From CCoeControl
        * @since 2.8
        * @return The minimum size required for the control to display
        */     
        TSize MinimumSize();   

        /**
        * From CCoeControl
        * @since 2.8
        */    
        void SizeChanged();
        
        /**
        * From CCoeControl
        * @
        */   
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CCamCaptureSetupSlider( MCamSettingValueObserver* aObserver, 
                                TCamSettingItemIds aSettingType,
                                TInt aSteps, TBool aFullySkinned=EFalse );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CCoeControl* aParent );
        
        /**
        * Called to read the layout information from a resource 
        * @since 2.8
        */  
        void ReadLayoutL();

        /*
        *  Read touch layout
        */
        void TouchLayoutL();

        /*
        *  Read non-touch layout
        */        
        void NonTouchLayoutL();

        /*
        *  Read touch contrast layout
        */        
        void TouchContrastLayoutL( const TRect& aParentRect );

        /*
        *  Read non-touch contrast layout
        */        
        void NonTouchContrastLayoutL( const TRect& aParentRect );

        /*
        *  Read touch exposure value layout
        */        
        void TouchEVLayoutL( const TRect& aParentRect );

        /*
        *  Read non-touch exposure value layout
        */
        void NonTouchEVLayoutL( const TRect& aParentRect );

        /*
        *  Initialzing slider values
        */
        void InitializeSliderValuesL();
        
        
    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Bitmap and mask for slider "shaft"
        CFbsBitmap* iBitmapShaft;
        CFbsBitmap* iBitmapShaftMask;

        // Bitmap and mask for slider "thumb"
        CFbsBitmap* iBitmapThumb;        
        CFbsBitmap* iBitmapThumbMask;

        // Bitmap and mask for pressed slider "thumb"
        CFbsBitmap* iBitmapThumbSelected;        
        CFbsBitmap* iBitmapThumbSelectedMask;

        // Bitmap and mask for slider "thumb" currently in use
        CFbsBitmap* iBitmapThumbCurrent;        
        CFbsBitmap* iBitmapThumbCurrentMask;
        
        // Current level to be indicated by the thumb
        TInt iValue;
        
        // Pointer to the observer of slider events
        MCamSettingValueObserver* iSettingObserver;        

        // Point of where the top-left of the thumb bitmap will be drawn
        TPoint iThumbPoint;
                              
        // The values that 
        RPointerArray <CCamSliderLegend> iLegendArray;   
 
        // Layout structure for the Shaft
        TAknLayoutRect iShaftLayout;

        // Layout structure for the Thumb
        TAknLayoutRect iThumbLayout;

        // Main parent 
        TRect iLayoutAreaRect;  

        // The minimum size the control requires to be drawn
        TSize iMinimumSize;

        // The parent control of the slider
        const CCoeControl* iParentControl;

        // The maximum slider value
        TInt iMaxSliderValue;

        // The minium slider value
        TInt iMinSliderValue;

        // the number of slider values
        TInt iNumSliderValues;

        // Identifies which setting is being edited
        TCamSettingItemIds iSettingType;
        
        // number of slider steps
        TInt iSteps;
        // Drawing mode
        TBool iFullySkinned;
    };

#endif      // __CCAMCAPTURESETUPSLIDER_H   
            
// End of File
