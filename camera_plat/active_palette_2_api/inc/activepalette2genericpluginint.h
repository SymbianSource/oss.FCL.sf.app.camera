/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active Palette generic plugin public interface
*
*/


/**
 * @file ActivePalette2GenericPluginInt.h
 * Active Palette generic plugin public interface
 */

#ifndef ACTIVE_PALETTE_2_GENERIC_PLUGIN_INT_H
#define ACTIVE_PALETTE_2_GENERIC_PLUGIN_INT_H

// INCLUDES
#include <e32std.h>

/// UID for generic plugin
#define KActivePalette2GenericPluginUID			    0x10208A6D

// CONSTANTS
/// Maximum tooltip length, in characters
const TInt KMaxTooltipLen = 256;

// FORWARD DECLARATIONS

/**
 * Generic plugin parameters
 */
class TAP2GenericPluginParams
	{
public:
    /**
     * Use to signify the resource file type
     */
    enum TAP2GenericPluginResFileType
        {
        EGenericStdResFile 			= 1, ///< Use the inbuilt icons
    	EGenericCustomResFile	    = 2  ///< Use a custom graphics file
    	};
    	
    /**
     * The IDs for the inbuilt icons
     */
    enum TAP2GenericPluginIconId
        {
        // No built-in graphics supported
	    };

public:

    /**
    * Constructor
    */
	TAP2GenericPluginParams();

    /**
    * Constructor
    * @param aResFileType Resource file type
    * @param aGraphicsFileName Graphics file name
    * @param aIconId Bitmap UD for the item's icon
    * @param aMaskId Bitmap UD for the item's mask
    * @param aTooltipText Tooltip text
    * @param aMessageId Message ID
    * @param aMsgCustomParam Message custom parameter
    */
	IMPORT_C	TAP2GenericPluginParams(const TInt aResFileType,
									   const TDesC& aGraphicsFileName,
									   const TInt aIconId,
									   const TInt aMaskId,
									   const TDesC& aTooltipText,
									   const TInt aMessageId,
									   const TInt aMsgCustomParam);	

    /// Current graphics file type in use
	TInt					iResFileType;
	/// Current graphics file in use
	TFileName 				iGraphicsFileName;		
	/// The icon ID
	TInt      				iIconId;
	/// The mask ID
	TInt      				iMaskId;
	/// Tooltip text for the item
	TBuf<KMaxTooltipLen> 	iTooltipText;
	/// Message ID
	TInt					iMessageId;
	/// Custom parameter
	TInt					iMsgCustomParam;
	};


/**
 * Message IDs used by generic plugin
 */
enum TAP2GenericPluginMessageIDs
	{
	EAPGenericPluginChangeIcon,  ///< Change the current icon
	EAPGenericPluginChangeParams ///< Change the general parameters
	};

/**
 * Describes an icon and its mask, for the generic plugin
 */
class TAP2GenericPluginIcon
	{
public:

    /**
    * Constructor
    */
	TAP2GenericPluginIcon();
	
    /**
    * Constructor
    * @param aIconId Bitmap ID for the icon
    * @param aMaskId Bitmap ID for the mask
    */
	IMPORT_C	TAP2GenericPluginIcon(const TInt aIconId,
									 const TInt aMaskId);	

    /// The icon ID
	TInt      	iIconId;
	/// The mask ID
	TInt      	iMaskId;
	};

#endif // ACTIVE_PALETTE_2_GENERIC_PLUGIN_INT_H

// End of File
