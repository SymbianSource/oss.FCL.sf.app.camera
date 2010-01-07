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
* Description:  Active Palette generic plugin*
*/


/**
 * @file ActivePalette2GenericPlugin.h
 * Active Palette generic plugin
 */

#ifndef _ACTIVE_PALETTE_2_GENERIC_PLUGIN_H
#define _ACTIVE_PALETTE_2_GENERIC_PLUGIN_H

//  INCLUDES
#include <activepalette2genericpluginint.h>
#include <gdi.h>

#include "CActivePalettePluginBase.h"


/**
 * General-purpose, generic plugin
 */
class CAP2GenericPlugin : public CActivePalettePluginBase
    {
public:     
    
    /** 
     * Constructor
     * @param aCreationParams Creation parameters
     */
	CAP2GenericPlugin(const NActivePalettePluginFactory::TPluginCreationParams & aCreationParams);

private:
    
    /**
    * Destructor
    */
	virtual ~CAP2GenericPlugin();
	
/// @name From CActivePalettePluginBase
//@{
	void ConstructL(const NActivePalettePluginFactory::TPluginCreationParams& aCreationParams, 
                    const TDesC8& aCustomDataDes,
                    const TInt aCustomDataInt);
	void HandleItemSelectedL(void);
	void HandleMessageL(const TInt aMessageID, const TDesC8& aDataDes, const TInt aDataInt);
//@}

	/**
    * Sets the icon and tooltip text for this item
    * @param aParams Reference to the parameters structure
    */
	void SetIconAndTooltipL(TAP2GenericPluginParams& aParams);
	
	/**
    * Creates an icon from bitmap IDs for the icon and mask
    * @param aIconId Bitmap ID for the icon
    * @param aMaskId Bitmap ID for the mask
    * @return A pointer to the created icon
    */
	CGulIcon* CreateIconL(TInt aIconId, TInt aMaskId);

    private:    // Member variables
        /// Current message ID being serviced
    	TInt	  iMessageId;
    	/// The current message data
    	TInt	  iMsgCustomParam;
    	/// The size of icons to provide
    	TSize	  iDimensions;
    	/// The file containing the icon graphics
    	TFileName iGraphicsFileName;
    };

/**
* Instantiaites the plugin
* @param aPluginUid The UID
* @param aCreationParams Reference to the parameters structure
* @return A pointer to the plugin
*/
extern CActivePalettePluginBase* InstantiateGenericPluginL(const TUid& aPluginUid, 
                                                           const NActivePalettePluginFactory::TPluginCreationParams& aCreationParams);

#endif // _ACTIVE_PALETTE_2_GENERIC_PLUGIN_H

// End of File
