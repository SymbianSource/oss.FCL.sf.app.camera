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
* Description:  Active Palette utilities*
*/


/**
 * @file ActivePalette2Utils.h
 * Active Palette utilities
 */

#ifndef _ACTIVE_PALETTE_2_UTILS_H
#define _ACTIVE_PALETTE_2_UTILS_H

//  INCLUDES
#include <e32std.h>
#include <gdi.h>

// FORWARD DECLARATIONS
class CFbsBitmap;
class CFbsBitmapDevice;
class CBitmapContext;


// CLASS DECLARATIONS

/**
 * General utility class for Active Palette
 */
class ActivePalette2Utils
    {
public:
    // Enum declarations
    // General AP dimensions
    enum TAPDimension
        {
        EItemWidth,
        EItemHeight,
        EPaletteWidth,
        ETopSectionHeight,
        EBottomSectionHeight,
        EGapBetweenItems,
        EItemXOffset,
        EFocusRingWidth,
        EFocusRingHeight,
        EFocusRingXOffset,
        EFocusRingYOffset,
        ETooltipYOffset,
        ETooltipHeight,
        ETooltipWidth,
        ETooltipBaseline,
        ETooltipTail,
        ETooltipNose,
        E3dTooltipTextYOffset,
        E3dTooltipMaxWidth,
        EGenericParam
        };    

    // Size AP dimensions
    enum TAPDimensionSize
        {
        EItemSize,
        E3dPaletteTextureSize,
        E3dScrollIconSize,
        E3dFocusRingSize
        };    

    // Point AP dimensions
    enum TAPDimensionPoint
        {
        EDefault2dPaletteOffset,
        EDefault3dPaletteOffset,
        E3dScrollIconOffset,
        E3dFocusRingOffset,
        E3dTooltipOffset
        };

public:
    /*
     * Create drawing objects
     * @param aSize Bitmap size
     * @param aBitmap Pointer in which to store created CFbsBitmap
     * @param aDevice Pointer in which to store created aDevice
     * @param aGc Pointer in which to store created aGc
     */
	static void CreateDrawingObjectsL(const TSize aSize,
                                               CFbsBitmap** aBitmap, 
                                               CFbsBitmapDevice** aDevice,
                                               CBitmapContext** aGc);
								 
    /*
     * Create drawing objects
     * @param aDisplayMode The colour depth to use
     * @param aSize Bitmap size
     * @param aBitmap Pointer in which to store created CFbsBitmap
     * @param aDevice Pointer in which to store created aDevice
     * @param aGc Pointer in which to store created aGc
     */
	static void CreateDrawingObjectsL(const TDisplayMode aDisplayMode,
                                               const TSize aSize,
                                               CFbsBitmap** aBitmap, 
                                               CFbsBitmapDevice** aDevice,
                                               CBitmapContext** aGc);
									 
    /*
     * Create drawing objects and push created items onto the cleanup stack
     * @param aSize Bitmap size
     * @param aBitmap Pointer in which to store created CFbsBitmap
     * @param aDevice Pointer in which to store created aDevice
     * @param aGc Pointer in which to store created aGc
     */
	static void CreateDrawingObjectsLC(const TDisplayMode aDisplayMode,
                                                const TSize aSize,
                                                CFbsBitmap** aBitmap, 
                                                CFbsBitmapDevice** aDevice,
                                                CBitmapContext** aGc);

    /*
     * Delete drawing objects
     * @param aBitmap Pointer to CFbsBitmap to delete
     * @param aDevice Pointer to aDevice to delete
     * @param aGc Pointer to aGc to delete
     */
	static void DeleteDrawingObjects(CFbsBitmap** aBitmap, 
											  CFbsBitmapDevice** aDevice,
											  CBitmapContext** aGc);

    /*
     * Return the dimension of a requested type of active palette
     * part. Dimensions scale according to device resolution.
     * @param aDimensionType a type which determines which dimension to return
     * @param aGeneric a parameter to convert any TInt to the correct size
     */
    static TInt APDimension( TAPDimension aDimensionType, TInt aGeneric = 0 );

    /*
     * Return the dimension size of a requested type of active palette
     * part. Dimensions scale according to device resolution.
     * @param aDimensionSizeType type which determines which dimension to return
     */
    static TSize APDimensionSize( TAPDimensionSize aDimensionSize );

    /*
     * Return the dimension point of a requested type of active palette
     * part. Dimensions scale according to device resolution.
     * @param aDimensionPoint type which determines which point to return
     */
    static TPoint APDimensionPoint( TAPDimensionPoint aDimensionPoint );


    };

#endif // _ACTIVE_PALETTE_2_UTILS_H

// End of File
