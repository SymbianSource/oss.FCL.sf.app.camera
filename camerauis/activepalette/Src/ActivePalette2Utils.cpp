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
* Description:  Active Palette Utilities*
*/


/**
 * @file ActivePalette2Utils.cpp
 * Active Palette Utilities
 */

// INCLUDE FILES
#include "ActivePalette2Utils.h"

#include "ActivePalette2Cfg.h"

#include <fbs.h>
#include <bitstd.h>
#include <eikenv.h>
#include <e32math.h>
#include <AknUtils.h>

// LOCAL CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// ActivePalette2Utils::CreateDrawingObjectsL(
// -----------------------------------------------------------------------------
//
void ActivePalette2Utils::CreateDrawingObjectsL(const TDisplayMode aDisplayMode,
														 const TSize aSize,
														 CFbsBitmap** aBitmap, 
														 CFbsBitmapDevice** aDevice,
														 CBitmapContext** aGc)
    {
	CreateDrawingObjectsLC(aDisplayMode,aSize,aBitmap,aDevice,aGc);
	CleanupStack::Pop(*aGc);
	CleanupStack::Pop(*aDevice);
	CleanupStack::Pop(*aBitmap);
    }

// -----------------------------------------------------------------------------
// ActivePalette2Utils::CreateDrawingObjectsLC()
// -----------------------------------------------------------------------------
//
void ActivePalette2Utils::CreateDrawingObjectsLC(const TDisplayMode aDisplayMode,
                                                          const TSize aSize,
                                                          CFbsBitmap** aBitmap, 
                                                          CFbsBitmapDevice** aDevice,
                                                          CBitmapContext** aGc)
    {   
	*aBitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(*aBitmap);

	User::LeaveIfError((*aBitmap)->Create(aSize,aDisplayMode));

	*aDevice = CFbsBitmapDevice::NewL(*aBitmap);
	CleanupStack::PushL(*aDevice);

	CFbsBitGc* context;
	User::LeaveIfError((*aDevice)->CreateContext(context));
	*aGc = context;
	CleanupStack::PushL(*aGc);
    }

// -----------------------------------------------------------------------------
// ActivePalette2Utils::CreateDrawingObjectsL()
// -----------------------------------------------------------------------------
//
void ActivePalette2Utils::CreateDrawingObjectsL(const TSize aSize,
														 CFbsBitmap**       aBitmap, 
														 CFbsBitmapDevice** aDevice,
														 CBitmapContext**   aGc)
    {
    TInt color;
    TInt gray;
	TDisplayMode displayMode;
	displayMode = CEikonEnv::Static()->WsSession().GetDefModeMaxNumColors(color,gray);
	CreateDrawingObjectsL(displayMode,aSize,aBitmap,aDevice,aGc);
    }

// -----------------------------------------------------------------------------
// ActivePalette2Utils::DeleteDrawingObjects()
// -----------------------------------------------------------------------------
//
void ActivePalette2Utils::DeleteDrawingObjects( CFbsBitmap**       aBitmap, 
                                                CFbsBitmapDevice** aDevice,
                                                CBitmapContext**   aGc)
    {
	if (*aGc)	
	    {
		delete *aGc;
		*aGc = NULL;
	    }
	if (*aDevice)	
	    {
		delete *aDevice;
		*aDevice = NULL;
	    }
	if (*aBitmap)
	    {
		delete *aBitmap;
		*aBitmap = NULL;
	    }
    }
    

// -----------------------------------------------------------------------------
// ActivePalette2Utils::APDimension()
// -----------------------------------------------------------------------------
//
TInt ActivePalette2Utils::APDimension( TAPDimension aDimensionType, TInt aGeneric )
    {
    TSize ScreenSize( 0, 0 );
	AknLayoutUtils::LayoutMetricsSize( AknLayoutUtils::EScreen, ScreenSize );
	TInt baseheight = 240;
	TInt lower = 0;
	if ( ScreenSize.iWidth < ScreenSize.iHeight )
    	{
    	lower = ScreenSize.iWidth;
    	}
    else
        {
        lower = ScreenSize.iHeight;
        }
    TReal ratio = TReal(lower)/baseheight;
    TInt returnValue = 0;
    TReal roundResult;
    switch ( aDimensionType )
        {
        case EItemWidth:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KItemWidth, 0 );
            }
            break;
        case EItemHeight:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KItemHeight, 0 );
            }
            break;
        case EPaletteWidth:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KPaletteWidth, 0 );
            }
            break;
        case ETopSectionHeight:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KTopSectionHeight, 0 );
            }
            break;
        case EBottomSectionHeight:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KBottomSectionHeight, 0 );
            }
            break;
        case EGapBetweenItems:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KGapBetweenItems, 0 );
            }
            break;
        case EItemXOffset:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KItemXOffset, 0 );
            }
            break;
        case EFocusRingWidth:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KFocusRingWidth, 0 );
            }
            break;
        case EFocusRingHeight:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KFocusRingHeight, 0 );
            }
            break;
        case EFocusRingXOffset:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KFocusRingXOffset, 0 );
            }
            break;
        case EFocusRingYOffset:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KFocusRingYOffset, 0 );
            }
            break;
        case ETooltipYOffset:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KTooltipYOffset, 0 );
            }
            break;
        case ETooltipHeight:
            {
            Math::Round( roundResult, NAP2Cfg::KTooltipFactor*ratio*NAP2Cfg::KTooltipHeight, 0 );
            }
            break;
        case ETooltipWidth:
            {
            Math::Round( roundResult, NAP2Cfg::KTooltipFactor*ratio*NAP2Cfg::KTooltipWidth, 0 );
            }
            break;
        case ETooltipBaseline:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::KTooltipBaseline, 0 );
            }
            break;
        case ETooltipTail:
            {
            Math::Round( roundResult, NAP2Cfg::KTooltipFactor*ratio*NAP2Cfg::KTooltipTail, 0 );
            }
            break;
        case ETooltipNose:
            {
            Math::Round( roundResult, NAP2Cfg::KTooltipFactor*ratio*NAP2Cfg::KTooltipNose, 0 );
            }
            break;
        case E3dTooltipTextYOffset:
            {
            Math::Round( roundResult, ratio*NAP2Cfg::K3dTooltipTextYOffset, 0 );
            }
            break;
        case E3dTooltipMaxWidth:
            {
            roundResult = ScreenSize.iWidth;
            }
            break;
        case EGenericParam:
            {
            Math::Round( roundResult, ratio*aGeneric, 0 );
            }
            break;
        default:
            {
            //PRINT( _L("Camera :: CCamAppUi::HandleCommandL Panic!") )
            //CamPanic( ECamPanicUi );
            }
            break;
        }
    returnValue = TInt(roundResult);
    return returnValue;
    }


// -----------------------------------------------------------------------------
// ActivePalette2Utils::APDimensionSize()
// -----------------------------------------------------------------------------
//
TSize ActivePalette2Utils::APDimensionSize( TAPDimensionSize aDimensionSize )
    {
    TSize returnSize( 0, 0 );
    switch ( aDimensionSize )
        {
        case EItemSize:
            {
            TInt width = ActivePalette2Utils::APDimension( EItemWidth );
            TInt height = ActivePalette2Utils::APDimension( EItemHeight );
            returnSize.SetSize( width, height );
            }
            break;
        case E3dPaletteTextureSize:
            {
            TInt width = ActivePalette2Utils::APDimension( EGenericParam, K3dPaletteTextureSize.iWidth );
            TInt height = ActivePalette2Utils::APDimension( EGenericParam, K3dPaletteTextureSize.iHeight );
            returnSize.SetSize( width, height );
            }
            break;
        case E3dScrollIconSize:
            {
            TInt width = ActivePalette2Utils::APDimension( EGenericParam, K3dScrollIconSize.iWidth );
            TInt height = ActivePalette2Utils::APDimension( EGenericParam, K3dScrollIconSize.iHeight );
            returnSize.SetSize( width, height );
            }
            break;
        case E3dFocusRingSize:
            {
            TInt width = ActivePalette2Utils::APDimension( EGenericParam, K3dFocusRingSize.iWidth );
            TInt height = ActivePalette2Utils::APDimension( EGenericParam, K3dFocusRingSize.iHeight );
            returnSize.SetSize( width, height );
            }
            break;
        default:
            {
            //PRINT( _L("Camera :: CCamAppUi::HandleCommandL Panic!") )
            //CamPanic( ECamPanicUi );
            }
            break;
        }
    return returnSize;
    }

// -----------------------------------------------------------------------------
// ActivePalette2Utils::APDimensionPoint()
// -----------------------------------------------------------------------------
//
TPoint ActivePalette2Utils::APDimensionPoint( TAPDimensionPoint aDimensionPoint )
    {
    TPoint returnPoint( 0, 0 );
    switch ( aDimensionPoint )
        {
        case EDefault2dPaletteOffset:
            {
            TInt x = ActivePalette2Utils::APDimension( EGenericParam, KDefault2dPaletteOffset.iX );
            TInt y = ActivePalette2Utils::APDimension( EGenericParam, KDefault2dPaletteOffset.iY );
            returnPoint.SetXY( x, y );
            }
            break;
        case EDefault3dPaletteOffset:
            {
            TInt x = ActivePalette2Utils::APDimension( EGenericParam, KDefault3dPaletteOffset.iX );
            TInt y = ActivePalette2Utils::APDimension( EGenericParam, KDefault3dPaletteOffset.iY );
            returnPoint.SetXY( x, y );
            }
            break;
        case E3dScrollIconOffset:
            {
            TInt x = ActivePalette2Utils::APDimension( EGenericParam, K3dScrollIconOffset.iX );
            TInt y = ActivePalette2Utils::APDimension( EGenericParam, K3dScrollIconOffset.iY );
            returnPoint.SetXY( x, y );
            }
            break;
        case E3dFocusRingOffset:
            {
            TInt x = ActivePalette2Utils::APDimension( EGenericParam, K3dFocusRingOffset.iX );
            TInt y = ActivePalette2Utils::APDimension( EGenericParam, K3dFocusRingOffset.iY );
            returnPoint.SetXY( x, y );
            }
            break;
        case E3dTooltipOffset:
            {
            TInt x = -ActivePalette2Utils::APDimension( EItemXOffset );
            TInt y = ActivePalette2Utils::APDimension( EGenericParam, K3dTooltipOffset.iY );
            returnPoint.SetXY( x, y );
            }
            break;
        default:
            {
            //PRINT( _L("Camera :: CCamAppUi::HandleCommandL Panic!") )
            //CamPanic( ECamPanicUi );
            }
            break;
        }
    return returnPoint;
    }

// End of File  
