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
* Description:  Active Palette 2 Configuration details
*
*/


#ifndef ACTIVE_PALETTE_2_CONFIGURATION_H
#define ACTIVE_PALETTE_2_CONFIGURATION_H

// ===========================================================================
// Includes
#include <e32base.h>
#include <activepalette2factory.h> // for TActivePalette2DrawMode


// ===========================================================================
// Declarations
struct TActivePalette2TooltipConfig
  {
  TActivePalette2TooltipConfig()
    : iPreshowDelay    ( -1 )
    , iFullshowPeriod  ( -1 )
    , iTimerTickLength ( -1 )
    , iTransitionSpeed ( -1 )
    , iTransitionFrames( -1 ) 
    {};

  TInt iPreshowDelay;
  TInt iFullshowPeriod;
  TInt iTimerTickLength;
  TInt iTransitionSpeed;  // For scrolling tooltip
  TInt iTransitionFrames; // For fading tooltip
  };

class CActivePalette2Configuration : public CBase
  {
  // =======================================================
  // Methods
  public:

    static CActivePalette2Configuration*  NewL( TActivePalette2DrawMode aDrawMode );
    virtual ~CActivePalette2Configuration();

  protected:

    void ConstructL();
    CActivePalette2Configuration( TActivePalette2DrawMode aDrawMode );

  public:

    /**
    * Return the Tooltip details.
    * @param aTooltipTimes Structure where the details are filled.
    */
    void TooltipConfiguration( TActivePalette2TooltipConfig& aTooltipTimes );
 
  // =======================================================
  // Data
  protected:

    TActivePalette2DrawMode iDrawMode;

  // =======================================================
  };
#endif // ACTIVE_PALETTE_2_CONFIGURATION_H

// ===========================================================================
// end of file
