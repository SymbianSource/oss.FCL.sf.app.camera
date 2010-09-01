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
* Description:  Panic function for the Camera Application*
*/


#ifndef CAMPANIC_H
#define CAMPANIC_H

//  INCLUDES
#include <e32base.h>

// DATA TYPES

/** Camera application panic codes */
enum TCameraPanic 
    {
    ECamPanicUi = 1,
    ECamPanicEVUnexpectedNumerator,
    ECamPanicResourceLeak,
    ECamPanicInvalidState,
    ECamPanicInvalidView,
    ECamPanicFileSystemError,
    ECamPanicNullImage,
    ECamPanicNullPointer,
    ECamPanicUnhandledCreateSettingItem,
    ECamPanicUnknownSettingItem,
    ECamPanicInvalidZoomValue,
    ECamPanicCaptureSetupMenuUnknownItem,
    ECamPanicUnhandledCommand,
    ECamPanicUSSPairNotFound,
    ECamPanicDefaultNotFoundInIniFile,
    ECamPanicBadIndex,
  	ECamPanicMultipleSettingItemPreviews,
    ECamPanicUnexpectedBitmapSize,
    // add further panics here
    ECamPanicInvalidResource,
    ECamPanicInvalidActivePaletteItemType,
    ECamPanicNonSettingActivePaletteTypeExpected,
    ECamPanicNoSuitableIsoSettingFound,    
    ECamPanicHistogramError,
    ECamPanicSecondaryQualityMissing,
    ECamPanicMMSQualityMissing,
    ECamPanicNotEnoughISOValuesSupported,
        
    ECamPanicNotSupported = 99,
    };

GLREF_C void CamPanic( TCameraPanic aReason );

#endif      // CAMPANIC_H 
            
// End of File
