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


#ifndef ACTIVE_PALETTE_2_SEMI_TRANSPARENT_FADING_STYLER_H
#define ACTIVE_PALETTE_2_SEMI_TRANSPARENT_FADING_STYLER_H


// ===========================================================================
// Includes
#include "ActivePalette2SemiTransparentStyler.h"


// ===========================================================================
// Forward declarations
class CFbsBitmap;
class CFbsBitmapDevice;
class CBitmapContext;


// ===========================================================================
// Class declarations
class CActivePalette2SemiTransparentFadingStyler 
  : public CActivePalette2SemiTransparentStyler
  {
  public:    

    static CActivePalette2SemiTransparentFadingStyler* NewL();

    virtual ~CActivePalette2SemiTransparentFadingStyler();

  public:

    virtual void BlitTooltip( CBitmapContext* aGc, 
                              const TPoint&   aPos, 
                              TInt            aCurrentPhase, 
                              TInt            aTotalPhases );

  protected:    

    virtual TInt TooltipNoseMaskId();
    virtual TInt TooltipBodyMaskId();

    virtual void AllocateTooltipBufL();
    virtual void ReleaseTooltipBuf();

  private:
  
  	CFbsBitmap*       iTooltipAlphaBitmap;
  	CFbsBitmapDevice* iTooltipAlphaDevice;
  	CBitmapContext*   iTooltipAlphaContext;
  
  	CFbsBitmap*       iTooltipBlendedMaskBitmap;
  	CFbsBitmapDevice* iTooltipBlendedMaskDevice;
  	CBitmapContext*   iTooltipBlendedMaskContext;
  };
#endif // ACTIVE_PALETTE_2_SEMI_TRANSPARENT_FADING_STYLER_H

// ===========================================================================
// end of file
