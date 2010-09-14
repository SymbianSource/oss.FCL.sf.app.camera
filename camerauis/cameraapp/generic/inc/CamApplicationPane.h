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
* Description:  Control for containing the NaviPane 
*/


#ifndef CAMAPPLICATIONPANE_H
#define CAMAPPLICATIONPANE_H

// INCLUDES
#include <coecntrl.h>
#include <aknutils.h>
#include "CamSettings.hrh"
#include "CamAppController.h"

// FORWARD DECLARATIONS
class MAknsSkinInstance;
class CAknsBasicBackgroundControlContext;
class CCamNaviCounter;
class CCamNaviProgressBar;	
class CCamBurstThumbnailGridModel;
class CCamTitlePane;

// CLASS DECLARATION

/**
* Control for containing the NaviPane
* @since 2.8
*/
class CCamApplicationPane : public CCoeControl
    {
    private:    
        // Enumeration used to indicate the pane currently visible
        enum TAppPane
            {
            EAppPaneUndefined,
            EAppPaneCounter,
            EAppPaneProgress
            };
        
    public: // Constructors and destructor
		/**
		*	Static factory construction function
        *   @since 3.0
		*	@param aController Reference to CCamAppController
		*	@return pointer to constructed instance of CCamApplicationPane
		*/
		static CCamApplicationPane* NewL( CCamAppController& aController );

        /**
        * Destructor.
        */
        ~CCamApplicationPane();
        
    public: // New functions    
        /**
        * Shows the NaviProgress control in the Application Pane
        * @since 3.0
        */    
        void ShowProgress();
        
        /**
        * Shows the NaviCounter control in the Application Pane
        * @since 3.0
        */    
        void ShowCounter();
        
        /**
        * Sets the capture mode the pane should indicate
        * @since 3.0
        * @param aCaptureMode The capture mode to use
        */            
        void SetCaptureMode( TCamCameraMode aMode );
        
        /**
        * Sets the burst model to use if app pane showing burst filenames
        * @since 3.0
        * @param aModel The grid model to get filenames from
        */            
        void SetBurstModelL( CCamBurstThumbnailGridModel* aModel );
        
        /**
        * Sets the owned panes to a known good state (all invisible)
        * @since 3.0        
        */
        void InitialisePanesL();

    public: // Functions from base classes        
        /**
        * Returns the number of component controls
        * @since 3.0
        * @return The number of owned controls to display
        */       
        TInt CountComponentControls() const;
        
        /**
        * Returns the specified component control
        * @since 3.0
        * @param aIndex The index of the control to return
        * @return Pointer to the specified control
        */               
        CCoeControl* ComponentControl( TInt aIndex ) const;
                
	protected:
		/**
        * C++ constructor.
        * @since 3.0
        * @param aController Reference to CCamAppController
        */
        CCamApplicationPane( CCamAppController& aController );
        
        /**
        * Symbian OS 2nd phase constructor.
        * @since 3.0
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * From CCoeControl
        * @since 3.0
        * Set the size and position of its component controls.
        */
        void SizeChanged();

        /**
        * From CCoeControl Draw the view
        * @since 3.0
        * @param aRect area where to draw
        */
        void Draw( const TRect& aRect ) const;      
        

    protected: //data     
        // The current pane displayed in the Application Pane
        TAppPane iPaneInUse;
        
        // Instance of Navi Counter control
        CCamNaviCounter* iNaviCounter;
        
        // Instance of Navi Progress control
        CCamNaviProgressBar* iNaviProgressBar;	
        
        // Controller used to initialise the Navi controls.  Not owned
        CCamAppController& iController;
                
        // Instance of the Title Pane control
        CCamTitlePane* iTitlePane;
        
        // Layout for the Application Pane rectangle
        TAknLayoutRect iAppPaneLayoutRect;
    };

#endif // CAMAPPLICATIONPANE_H

// End of File
