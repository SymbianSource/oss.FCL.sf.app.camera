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
* Description:  Control for displaying remaining images in navi pane.
*/


#ifndef CAMNAVICOUNTERCONTROL_H
#define CAMNAVICOUNTERCONTROL_H

// INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>
#include "CamAppController.h"
#include "CamSettings.hrh"
#include "CamResourceLoader.h"
#include "CamNaviCounterModel.h"
#include "CamObserver.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Control for displaying recording time info in navi pane.
* @since 2.8
*/
class CCamNaviCounterControl : public CCoeControl, 
                               public MCamObserver
    {
    public: // Constructors and destructor
		/**
		* Static factory construction function
        * @since 2.8
		* @param aModel Counter model reference
		* @return pointer to constructed instance of CCamNaviCounterControl
		*/
		static CCamNaviCounterControl* NewL( CCamNaviCounterModel& aModel );

        /**
        * Destructor.
        */
        ~CCamNaviCounterControl();

    public: // New functions
               
        /**
        * Force update of navi-pane (i.e after dismissal of MMC removed error note)
        * @since 2.8
        */
        void ForceNaviPaneUpdate();

		/**
		* Draw Navi-Counter
		* @since 3.0
		* @param aGc Handle to graphics context
		*/
		void DrawNaviCtr( CBitmapContext& aGc ) const;

        /**
        * Sets whether this control is active. If not, it cannot affect the model, or use it to render.
        * @param aActive Whether to set it active or not
        */
        void SetActiveL( TBool aActive );
        
    public: // From MCamObserver
    
        /**
        * From MCamObserver  
        * @param aEvent Observed object's event code
        */
        void HandleObservedEvent(TCamObserverEvent aEvent);

	protected:
		/**
        * C++ constructor.
        * @since 2.8
        * @param aModel Counter model reference
        */
        CCamNaviCounterControl( CCamNaviCounterModel& aModel );
        
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * From CCoeControl
        * @since 2.8
        * Set the size and position of its component controls.
        */
        void SizeChanged();

        /**
        * From CCoeControl Draw the view
        * @since 2.8
        * @param aRect area where to draw
        */
        void Draw( const TRect& aRect ) const;
        
    protected: //data
        CCamNaviCounterModel& iModel;

    private: //data
        TBool iActive;    
    
    };

#endif // CAMNAVICOUNTERCONTROL_H

// End of File
