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
* Description:  Active Palette Styler Factory*
*/


/**
 * @file ActivePalette2StylerFactory.h
 * Active Palette Styler Factory
 */

#ifndef _ACTIVE_PALETTE_2_STYLER_FACTORY_H
#define _ACTIVE_PALETTE_2_STYLER_FACTORY_H


#include <activepalette2factory.h>

class CActivePalette2Styler;

/**
 * Factory class to generate CActivePalette2Styler instances.
 */
class ActivePalette2StylerFactory
    {
public:
    /**
     * Creates a styler based on the supplied draw mode
     * @param aDrawMode The draw mode to implement
     * @return The instantiated styler
     */
    static CActivePalette2Styler* CreateStylerL(TActivePalette2DrawMode aDrawMode);
    };

#endif // _ACTIVE_PALETTE_2_STYLER_FACTORY_H
