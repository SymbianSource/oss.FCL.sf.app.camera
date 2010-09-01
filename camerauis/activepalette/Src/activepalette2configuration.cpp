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
* Description:
*
*/


#include <activepalette2factory.h>

#include "ActivePalette2Cfg.h"
#include "activepalette2configuration.h"


CActivePalette2Configuration* 
CActivePalette2Configuration::NewL( TActivePalette2DrawMode aDrawMode )
  {
  CActivePalette2Configuration* self = 
      new (ELeave) CActivePalette2Configuration( aDrawMode );

  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop( self );

  return self;
  }

CActivePalette2Configuration
::CActivePalette2Configuration( TActivePalette2DrawMode aDrawMode )
  : iDrawMode( aDrawMode )
  {
  }

void
CActivePalette2Configuration::ConstructL()
  {
  }

CActivePalette2Configuration::~CActivePalette2Configuration()
  {
  }


void 
CActivePalette2Configuration
::TooltipConfiguration( TActivePalette2TooltipConfig& aTooltipTimes )
  {
  switch( iDrawMode )
    {
    case EAP2DrawModeSemiTransparentTooltipModeFading:

      aTooltipTimes.iPreshowDelay     = NAP2Cfg::KTooltipPreshowDelayFading;
      aTooltipTimes.iFullshowPeriod   = NAP2Cfg::KTooltipFullshowPeriodFading;
      aTooltipTimes.iTimerTickLength  = NAP2Cfg::KTooltipTimerTickLengthFading;
      aTooltipTimes.iTransitionSpeed  = KErrNotSupported;
      aTooltipTimes.iTransitionFrames = NAP2Cfg::KTooltipFadingSteps;

      break;

    case EAP2DrawModeSemiTransparent:
    case EAP2DrawModeCheckerboard:
    case EAP2DrawModeOpaque:
    default:

      aTooltipTimes.iPreshowDelay     = NAP2Cfg::KTooltipPreshowDelay;
      aTooltipTimes.iFullshowPeriod   = NAP2Cfg::KTooltipFullshowPeriod;
      aTooltipTimes.iTimerTickLength  = NAP2Cfg::KTooltipTimerTickLength;
      aTooltipTimes.iTransitionSpeed  = NAP2Cfg::KTooltipShowSpeed;
      aTooltipTimes.iTransitionFrames = KErrNotSupported;

      break;
    }
  }
