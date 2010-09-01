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
* Description:  Class for showing the EV slider*
*/



#ifndef __CGSCAMQUALITYSETTINGSLIDER_H
#define __CGSCAMQUALITYSETTINGSLIDER_H

//  INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>               // for TAknLayoutText
#include "CamPSI.h"                 // For Product Specific Information  
#include "CamSettingsInternal.hrh"
#include "CamSettings.hrh"  
#include "GSCamcorderPlugin.hrh"
#include "Cam.hrh"
#include "CamSettingValueObserver.h"

// CONSTANTS
// The maximum number of divisions required for the quality slider.
#define BITMAP_SHAFT_MAX_DIVISIONS 6

class CCamConfiguration;


/**
* Utility class used to group together related layout information into one
* object, and construct itself using a supplied resource reader.
*
*  @since 2.8
*/
class CCamSliderLegend : public CBase
    {
    
public:

    ~CCamSliderLegend();

    /**
    * Constructs the layout information for the object
    * @since 3.0
    * @param aLayoutRect Layout for the icon
    * @param aBitmap     Bitmap ID
    * @param aMask       Mask ID
    */
    void ConstructL( const TAknLayoutRect& aLayoutRect,
    				 TInt aBitmap, TInt aMask );

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
    };



/**
*  Slider for showing Quality level settings 
*
*  @since 2.8
*/
class CGSCamQualitySettingSlider : public CCoeControl
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CGSCamQualitySettingSlider* NewL( const CCoeControl* aParent, 
                                                 MCamSettingValueObserver* aObserver,
                                                 TCamSettingItemIds aSettingType,                                         
                                                 TInt aSteps,
                                                 CCamConfiguration* aConfiguration
                                             );
        
        /**
        * Destructor.
        */
        virtual ~CGSCamQualitySettingSlider();

    public: // New functions
        
        /**
        * Sets up the range and initial value of the slider
        * @since 2.8
        * @param aValue The value to start with on the slider.                  
        */        
        void InitializeL( TInt aValue, 
                          HBufC* aTopSubtitle,
                          HBufC* aBottomSubtitle );

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
        CGSCamQualitySettingSlider( MCamSettingValueObserver* aObserver, 
                                    TCamSettingItemIds aSettingType,
                                    TInt aSteps, 
                                    CCamConfiguration* aConfiguration ); 

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const CCoeControl* aParent );
        
        /**
        * Called to read the layout information from a resource 
        * @since 2.8
        */  
        void ReadLayoutL();

        /**
        * Read touch layout
        */
        void TouchLayoutVideo();
        
        /**
        * Read nontouch layout
        */
        void NonTouchLayoutVideo();
        
        /**
        * Read touch layout
        */
        void TouchLayoutPhoto();
        
        /**
        * Read nontouch layout
        */
        void NonTouchLayoutPhoto();

        /**
        * Appends legends to iLegendArray by Product Specific Information key 
        * @since 5.0
        * @param aKey, Product Specific Information key
        * @param aNumSliderValues, number of slider values
        */
        void AppendLegendArrayL( TCamPsiKey aKey, TInt aNumSliderValues = 0 );
           
        /**
        * Get layout for video/image quality legend icons
        * @since S60 v5.0
        * @param aParentRect  Parent rectangle
        * @param aSettingType Setting type
        * @param aIndex       Item index
        * @return             Layout rect that has the correct layout set
        */
        TAknLayoutRect GetTouchVideoLegendLayout(
             const TRect& aParentRect, 
             TInt aIndex ) const;
        
        /**
        * Get layout for video/image quality legend icons
        * @since S60 v5.0
        * @param aParentRect  Parent rectangle
        * @param aSettingType Setting type
        * @param aIndex       Item index
        * @return             Layout rect that has the correct layout set
        */
        TAknLayoutRect GetNonTouchVideoLegendLayout(
             const TRect& aParentRect, 
             TInt aIndex ) const;

        /**
        * Get layout for video/image quality legend icons
        * @since S60 v5.0
        * @param aParentRect  Parent rectangle
        * @param aSettingType Setting type
        * @param aIndex       Item index
        * @return             Layout rect that has the correct layout set
        */
        TAknLayoutRect GetTouchPhotoLegendLayout(
             const TRect& aParentRect, 
             TInt aIndex ) const;
        
        /**
        * Get layout for three image quality legend icons
        * @since S60 v5.0
        * @param aParentRect  Parent rectangle
        * @param aSettingType Setting type
        * @param aIndex       Item index
        * @return             Layout rect that has the correct layout set
        */
        TAknLayoutRect GetNonTouchPhotoLegend3Layout(
             const TRect& aParentRect, 
             TInt aIndex ) const;
        
        /**
        * Get layout for five image quality legend icons
        * @since S60 v5.0
        * @param aParentRect  Parent rectangle
        * @param aSettingType Setting type
        * @param aIndex       Item index
        * @return             Layout rect that has the correct layout set
        */
        TAknLayoutRect GetNonTouchPhotoLegend5Layout(
             const TRect& aParentRect, 
             TInt aIndex ) const;

        /**
        * Counts quality setting steps. Available layouts are for three and 
        * five steps.
        * @return  TBool, ETrue if count is three or less
        */
        TBool IsThreeTicksSlider() const; 
        
    public:     // Data
    
    protected:  // Data

    private:    // Data

        // Bitmaps for the slider "shaft"
        CFbsBitmap* iBitmapShafts[BITMAP_SHAFT_MAX_DIVISIONS+1];
        CFbsBitmap* iBitmapShaftsMasks[BITMAP_SHAFT_MAX_DIVISIONS+1];

        // Current level to be indicated by the slider
        TInt iValue;
        
        // Pointer to the observer of slider events
        MCamSettingValueObserver* iSettingObserver;        

        // The values that 
        RPointerArray <CCamSliderLegend> iLegendArray;   
 
        // Layout structure for the Shaft
        TAknLayoutRect iShaftLayout;

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

        // Configuration manager
        CCamConfiguration* iConfiguration;

        // The subtitle text at top of page
        HBufC* iTopSubtitle;
        // The subtitle text at bottom of page
        HBufC* iBottomSubtitle;
        // The layout for the subtitle text at top
        TAknLayoutText iLayoutSubtitleTop;
        // The layout for the subtitle text at bottom of page
        TAknLayoutText iLayoutSubtitleBottom;
        
    };

#endif      // __CGSCAMQUALITYSETTINGSLIDER_H   
            
// End of File
