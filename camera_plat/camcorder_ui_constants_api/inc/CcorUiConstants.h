/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Constants for controlling Camcorder.
*
*/


#ifndef CCORUICONSTANTS_H
#define CCORUICONSTANTS_H

// INCLUDES
#include <e32std.h>

// CONSTANTS
const TUid KUidCamcorder = { 0x101F857A };
const TInt KCcorMsk = 3; 
const TInt KCcorLsk = 0;

const TInt KSilentProfile = 1;
const TInt KMeetingProfile = 2;

/**
* Constants for use in embedding.
*/
enum TCcorAppData
    {
    ECcorNormal = 0,
    ECcorEmbedStartUp,       // Starting embed mode
    ECcorForceStillMode,     // Force still image mode
    ECcorForceVideoMode,     // Force video mode
    ECcorForceMMSVideoMode   // Force video mode with MMS size limit for videos
    };

/**
* Image/video storage constants
*/
enum TCcorMemory
    {
    ECcorPhoneMemory,
    ECcorMemoryCard
    };

#endif

// End of File

