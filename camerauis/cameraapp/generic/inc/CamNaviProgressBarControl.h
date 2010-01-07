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
* Description:  Control for displaying remaining images in navi pane.
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/


#ifndef CAMNAVIPROGRESSBARCONTROL_H
#define CAMNAVIPROGRESSBARCONTROL_H

// INCLUDES
#include <coecntrl.h>
#include <AknUtils.h>
#include "CamAppController.h"
#include "CamSettings.hrh"
#include "CamResourceLoader.h"
#include "CamNaviProgressBarModel.h"
#include "CamObserver.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Control for displaying recording time info in navi pane.
* @since 2.8
*/
class CCamNaviProgressBarControl : public CCoeControl, 
                                   public MCamObserver
  {
  public: // Constructors and destructor
    /**
    * Static factory construction function
    * @since 2.8
    * @param aModel Reference to the progress bar model
    * @return pointer to constructed instance of CCamNaviProgressBarControl
    */
    static CCamNaviProgressBarControl* NewL( CCamNaviProgressBarModel& aModel );

    /**
    * Destructor.
    */
    ~CCamNaviProgressBarControl();

  public: // New functions
               
    /**
    * Draw Navi-Counter
    * @since 3.0
    * @param aGc Handle to graphics context
    */
    void DrawProgressBar( CBitmapContext& aGc ) const;

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
    * @param aModel Reference to the progress bar model
    */
    CCamNaviProgressBarControl( CCamNaviProgressBarModel& aModel );
    
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

        CCamNaviProgressBarModel& iModel;

  private: //data

      TBool iActive;    
    
  };

#endif // CAMNAVIPROGRESSBARCONTROL_H

// End of File
