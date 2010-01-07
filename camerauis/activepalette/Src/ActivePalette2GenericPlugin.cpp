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
* Description:  Active Palette 2 Generic Plugin*
*/


/**
 * @file ActivePalette2GenericPlugin.cpp
 * Active Palette 2 Generic Plugin
 */

// INCLUDE FILES
#include "ActivePalette2GenericPlugin.h"
#include "ActivePalette2Const.h"
#include "ActivePalette2Cfg.h"

#include <gulicon.h>
#include <AknIconUtils.h>

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TAPGenericPluginParams::TAPGenericPluginParams()
// C++ Constructor
// -----------------------------------------------------------------------------
//
TAP2GenericPluginParams::TAP2GenericPluginParams() :
	iResFileType(EGenericStdResFile),
	iGraphicsFileName(KNullDesC),
	iIconId(0),
	iMaskId(0),
	iTooltipText(KNullDesC),
	iMessageId(KActivePaletteNoDataInt),
	iMsgCustomParam(KActivePaletteNoDataInt)	
	{
	}

// -----------------------------------------------------------------------------
// TAPGenericPluginParams::TAPGenericPluginParams()
// C++ Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TAP2GenericPluginParams::TAP2GenericPluginParams(const TInt aResFileType,
														const TDesC& aGraphicsFileName,
														const TInt aIconId,
														const TInt aMaskId,
														const TDesC& aTooltipText,
														const TInt aMessageId,
														const TInt aMsgCustomParam):
	iResFileType(aResFileType),
	iGraphicsFileName(aGraphicsFileName),
	iIconId(aIconId),
	iMaskId(aMaskId),
	iTooltipText(aTooltipText),
	iMessageId(aMessageId),
	iMsgCustomParam(aMsgCustomParam)
	{
	}

// -----------------------------------------------------------------------------
// TAPGenericPluginIcon::TAPGenericPluginIcon()
// C++ Constructor
// -----------------------------------------------------------------------------
//
TAP2GenericPluginIcon::TAP2GenericPluginIcon() :
	iIconId(0),
	iMaskId(0)
	{
	}

// -----------------------------------------------------------------------------
// TAPGenericPluginIcon::TAPGenericPluginIcon()
// C++ Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TAP2GenericPluginIcon::TAP2GenericPluginIcon(const TInt aIconId,
													const TInt aMaskId):
	iIconId(aIconId),
	iMaskId(aMaskId)
	{
	}

// -----------------------------------------------------------------------------
// CAP2GenericPlugin::CAP2GenericPlugin()
// C++ Constructor
// -----------------------------------------------------------------------------
//
CAP2GenericPlugin::CAP2GenericPlugin(const NActivePalettePluginFactory::TPluginCreationParams& aCreationParams):
CActivePalettePluginBase(aCreationParams)
	{
	}

// -----------------------------------------------------------------------------
// CAP2GenericPlugin::CAP2GenericPlugin()
// Destructor
// -----------------------------------------------------------------------------
//
CAP2GenericPlugin::~CAP2GenericPlugin()
	{
	}

// -----------------------------------------------------------------------------
// CAP2GenericPlugin::HandleItemSelectedL()
// -----------------------------------------------------------------------------
//
void CAP2GenericPlugin::HandleItemSelectedL(void)
	{
	PluginObserver().SendMessage(iMessageId,iMsgCustomParam);	
	PluginObserver().NotifyItemComplete(KActivePaletteNoDataInt);
	}

// -----------------------------------------------------------------------------
// CAP2GenericPlugin::ConstructL()
// -----------------------------------------------------------------------------
//
void CAP2GenericPlugin::ConstructL(const NActivePalettePluginFactory::TPluginCreationParams& aCreationParams, 
								  const TDesC8& aCustomDataDes, const TInt aCustomDataInt)
	{
	CActivePalettePluginBase::ConstructL(aCreationParams, aCustomDataDes, aCustomDataInt);
	
	iDimensions = aCreationParams.Dimensions();

	// unpack creation params
	TAP2GenericPluginParams params;
	TPckg<TAP2GenericPluginParams> paramsPckg(params);
	if (aCustomDataDes.Length())
		{
		paramsPckg.Copy(aCustomDataDes);
		SetIconAndTooltipL(params);
		}
	}

// -----------------------------------------------------------------------------
// CAP2GenericPlugin::SetIconAndTooltipL()
// -----------------------------------------------------------------------------
//
void CAP2GenericPlugin::SetIconAndTooltipL(TAP2GenericPluginParams& aParams)
	{
	iMessageId 		= aParams.iMessageId;
	iMsgCustomParam = aParams.iMsgCustomParam;

	iGraphicsFileName = aParams.iGraphicsFileName;		

	CGulIcon* icon = CreateIconL(aParams.iIconId, aParams.iMaskId);
	CleanupStack::PushL(icon);
	
	// Don't need to worry about the CGulIcon object from now on
	// as we are transferring ownership of it to the active palette.
	PluginObserver().SetItemIconAndTooltipL(icon, ETrue, aParams.iTooltipText);	
	
	CleanupStack::Pop(icon);
	}
	
// -----------------------------------------------------------------------------
// CAP2GenericPlugin::CreateIconL()
// -----------------------------------------------------------------------------
//
CGulIcon* CAP2GenericPlugin::CreateIconL(TInt aIconId, TInt aMaskId)
	{
	CFbsBitmap* picture;
	CFbsBitmap* mask;

	AknIconUtils::CreateIconLC(picture, mask, iGraphicsFileName, aIconId, aMaskId);
	AknIconUtils::SetSize(picture, iDimensions);
	AknIconUtils::SetSize(mask, iDimensions);
	
	CGulIcon* icon = CGulIcon::NewL(picture,mask);

	// Can now take the bitmaps off the cleanup stack as we have
	// transferred ownership to the CGulIcon object
	CleanupStack::Pop(mask);
	CleanupStack::Pop(picture);
	
	return icon;
	}

// -----------------------------------------------------------------------------
// CAP2GenericPlugin::HandleMessageL()
// -----------------------------------------------------------------------------
//
void CAP2GenericPlugin::HandleMessageL(const TInt aMessageID, const TDesC8& aDataDes, const TInt /* aDataInt */)
	{
	switch (aMessageID)
		{
		case EAPGenericPluginChangeIcon:
			{
			TAP2GenericPluginIcon iconParams;
			TPckg<TAP2GenericPluginIcon> iconParamsPckg(iconParams);
			if (aDataDes.Length())
				{
				iconParamsPckg.Copy(aDataDes);
				CGulIcon* icon = CreateIconL(iconParams.iIconId, iconParams.iMaskId);
				// Ownership of icon is transferred to the Active Palette
				PluginObserver().SetItemIcon(icon, ETrue);
				}
			}
			break;
			
		case EAPGenericPluginChangeParams:
			{
			TAP2GenericPluginParams params;
			TPckg<TAP2GenericPluginParams> paramsPckg(params);
			if (aDataDes.Length())
				{
				paramsPckg.Copy(aDataDes);
				SetIconAndTooltipL(params);
				}
			break;
			}

		default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// InstantiateGenericPluginL()
// -----------------------------------------------------------------------------
//
CActivePalettePluginBase* InstantiateGenericPluginL(
    const TUid& aPluginUid, 
    const NActivePalettePluginFactory::TPluginCreationParams& aCreationParams)
	{
	if (TUid::Uid(KActivePalette2GenericPluginUID) != aPluginUid)
		{
		User::Leave(KErrNotSupported);
		}
	
	CActivePalettePluginBase* self = new (ELeave) CAP2GenericPlugin(aCreationParams);
	return self;
	}

// End of File  
