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
* Description:  Active Palette Items*
*/


/**
 * @file ActivePalette2Item.cpp
 * Active Palette Items
 */

// DEFINITIONS
#define ACTIVEPALETTE_SUPPORT_PLUGIN_CAM

// INCLUDE FILES
#include <activepalette2genericpluginint.h>

#include "ActivePalette2Item.h"
#include "ActivePalette2Model.h"
#include "ActivePalette2Logger.h"
#include "ActivePalette2Cfg.h"
#include "ActivePalette2GenericPlugin.h"
#include "CActivePalettePluginBase.h"

#include <gulicon.h>
#include <fbs.h>
#include <bitstd.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CActivePalette2IDHolder::CActivePalette2IDHolder()
// -----------------------------------------------------------------------------
//
CActivePalette2IDHolder::CActivePalette2IDHolder()
    {
	// no implementation required
    }

// -----------------------------------------------------------------------------
// CActivePalette2IDHolder::CActivePalette2IDHolder()
// -----------------------------------------------------------------------------
//
CActivePalette2IDHolder::CActivePalette2IDHolder(TInt aItemId)
:   iItemId(aItemId)
    {
	// no implementation required
    }

// -----------------------------------------------------------------------------
// CActivePalette2IDHolder::SetItemId()
// -----------------------------------------------------------------------------
//
void CActivePalette2IDHolder::SetItemId(TInt aItemId)
    {
	iItemId = aItemId;
    }


// -----------------------------------------------------------------------------
// CActivePalette2Item::NewLC()
// -----------------------------------------------------------------------------
//
CActivePalette2Item * CActivePalette2Item::NewLC(TInt aItemId, 
									  		   const TUid& aPluginUid,
									  		   CActivePalette2Model& aModel)
    {
	CActivePalette2Item * self = new (ELeave) CActivePalette2Item(
	    aItemId, aPluginUid, aModel
	);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::CActivePalette2Item()
// -----------------------------------------------------------------------------
//
CActivePalette2Item::CActivePalette2Item(
    TInt aItemId, 
    const TUid& aPluginUid, 
    CActivePalette2Model& aModel
)
:   CActivePalette2IDHolder(aItemId),
    iPluginUid(aPluginUid),
    iModel(aModel)
    {
	LOGTEXT( _L( "CActivePalette2Item::CActivePalette2Item" ));
    }


// -----------------------------------------------------------------------------
// CActivePalette2Item::ConstructL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::ConstructL(void)
    {
	NActivePalettePluginFactory::TInstantiatePluginLFunc 
		instantiatePluginL = FindPluginFactoryL(iPluginUid);
	
	// instantiate plugin
	CActivePalettePluginBase* pluginObj = instantiatePluginL(iPluginUid, GetPluginCreationParams());
	if (NULL == iPlugin || pluginObj != iPlugin)
	    {	
	    // illegal plugin, plugin was supposed to callback via NotifyPluginCreated()
		User::Leave(KErrGeneral); 
	    }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::~CActivePalette2Item()
// -----------------------------------------------------------------------------
//
CActivePalette2Item::~CActivePalette2Item()
	{
	LOGTEXT1( _L( "CActivePalette2Item::~CActivePalette2Item entered, item ID : %d" ), ItemId());

	if (iPlugin)
		{
		LOGTEXT( _L( "... calling plugin's shutdown" ) );
		iPlugin->Shutdown();
		LOGTEXT( _L( "... plugin's shutdown returned" ) );
		}
	LOGTEXT( _L( "... deleting plugin's object" ) );
	CBase::Delete(iPlugin);
	LOGTEXT( _L( "... plugin's object deleted" ) );
	iPlugin = NULL;

	if (iOwnsIcon)
		{
		delete iIcon;
		}
	iIcon = NULL;

	delete iTooltip;
	iTooltip = NULL;

	iDLL.Close(); 

	LOGTEXT( _L( "CActivePalette2Item::~CActivePalette2Item left" ) );
	}

// -----------------------------------------------------------------------------
// CActivePalette2Item::FindPluginFactoryL()
// -----------------------------------------------------------------------------
//
NActivePalettePluginFactory::TInstantiatePluginLFunc CActivePalette2Item::FindPluginFactoryL(const TUid aUid) 
    {
	LOGTEXT1( _L( "CActivePalette2Item::FindPluginFactoryL entered, Uid : %d" ), 
			 aUid.iUid);

	NActivePalettePluginFactory::TInstantiatePluginLFunc factory = NULL;

	if (aUid == TUid::Uid(KActivePalette2GenericPluginUID))
	    {
		factory = &InstantiateGenericPluginL;
	    }
	else
	    {
		// lookup DLL file name
		TFileName dllFileName;
		TBool dllNameAssigned = EFalse;
			// todo: implement lookup in plugin table

		if (TUid::Uid(KPrintPluginUid) == aUid)
		    {
			dllFileName = KPrintPluginDLLName;
			dllNameAssigned = ETrue;
		    }
		else if (TUid::Uid(KPrintIntentPluginUid) == aUid)
		    {
			dllFileName = KPrintIntentPluginDLLName;
			dllNameAssigned = ETrue;
		    }
		else if( TUid::Uid( KOnlineSharingPluginUid ) == aUid )
		    {
			dllFileName = KOnlineSharingPluginDLLName;
			dllNameAssigned = ETrue;
		    } 
#ifdef ACTIVEPALETTE_SUPPORT_PLUGIN_CAM
        else if (TUid::Uid( KCamAPPluginUid ) == aUid)
		    {
			dllFileName = KCamAPluginDLLName;
			dllNameAssigned = ETrue;
		    }
		else
			{
			// lint
			}
#endif // ACTIVEPALETTE_SUPPORT_PLUGIN_CAM

		LOGTEXT1( _L( "... DLL file name found?: %d" ), dllNameAssigned);
		if (EFalse == dllNameAssigned)
		    {
			User::Leave(KErrNotSupported);	
		    }
		
		// load plug-in DLL
		LOGTEXT( _L( "... loading DLL" ));
		User::LeaveIfError(iDLL.Load(dllFileName));
		
		// get factory function
		LOGTEXT( _L( "... factory entry point lookup" ));
		factory = (NActivePalettePluginFactory::TInstantiatePluginLFunc) 
				iDLL.Lookup(NActivePalettePluginFactory::KFactoryEntryOrdinal);
				
		if(!factory)
			{
			User::Leave(KErrGeneral);	
			}
	    }

	LOGTEXT1( _L( "CActivePalette2Item::FindPluginFactoryL left, factory : %p" ), 
			 factory);

	return factory;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::GetPluginCreationParams()
// -----------------------------------------------------------------------------
//
NActivePalettePluginFactory::TPluginCreationParams CActivePalette2Item::GetPluginCreationParams(void) 
    {
	return NActivePalettePluginFactory::TPluginCreationParams(
					*this,
					iModel.ItemSize());
    }


// -----------------------------------------------------------------------------
// CActivePalette2Item::NotifyPluginCreated()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::NotifyPluginCreated(CActivePalettePluginBase* aPlugin)
    {
	iPlugin = aPlugin;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::NotifyPluginDestroyed()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::NotifyPluginDestroyed(CActivePalettePluginBase* aPlugin)
    {
	if (iPlugin == aPlugin)
	    {
		iPlugin = NULL;
	    }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::PluginUid()
// -----------------------------------------------------------------------------
//
TUid CActivePalette2Item::PluginUid(void) const
    {
	return iPluginUid;	
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::DoSetItemIcon()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::DoSetItemIcon(CGulIcon* aIcon, TBool aOnwershipTransfer)
    {
	if (iOwnsIcon)
		{
		delete iIcon;
		iIcon = NULL;
		}
		
	iIcon = aIcon;
	iOwnsIcon = aOnwershipTransfer;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::SetItemIcon()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::SetItemIcon(CGulIcon* aIcon, TBool aOnwershipTransfer)
    {
	DoSetItemIcon(aIcon,aOnwershipTransfer);
	// notify UI
	iModel.NotifyGraphicsChanged(*this,ETrue,EFalse);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::SetItemTooltipL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::SetItemTooltipL(const TDesC& aTooltipText)
    {
	// delete old
	delete iTooltip;
	iTooltip = NULL;

	// copy text
	iTooltip = aTooltipText.AllocL();
	// notify UI
	iModel.NotifyGraphicsChanged(*this,EFalse,ETrue);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::SetItemIconAndTooltipL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::SetItemIconAndTooltipL(
    CGulIcon* aIcon, 
    TBool aOnwershipTransfer, 
    const TDesC & aTooltipText
)
    {
	// delete old
	delete iTooltip;
	iTooltip = NULL;
	
	// copy icon and tooltip
	DoSetItemIcon(aIcon, aOnwershipTransfer);
	iTooltip = aTooltipText.AllocL();

	
	// notify UI
	iModel.NotifyGraphicsChanged(*this,ETrue,ETrue);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::NotifyItemComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::NotifyItemComplete(const TDesC8& aDataDes)
    {
	// notify model' user
	iModel.NotifyItemComplete(*this, KErrNone, aDataDes, KActivePaletteNoDataInt);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::NotifyItemComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::NotifyItemComplete(TInt aDataInt)
    {
	// notify model' user
	iModel.NotifyItemComplete(*this, KErrNone, KNullDesC8, aDataInt);
    }
	
// -----------------------------------------------------------------------------
// CActivePalette2Item::SendMessage()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::SendMessage(TInt aMessageID, const TDesC8& aDataDes)
    {
	// notify model' user
	iModel.NotifyMessage(*this, KErrNone, aMessageID, aDataDes, KActivePaletteNoDataInt);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::SendMessage()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::SendMessage(TInt aMessageID, TInt aDataInt)
    {
	// notify model' user
	iModel.NotifyMessage(*this, KErrNone, aMessageID, KNullDesC8, aDataInt);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::Icon()
// -----------------------------------------------------------------------------
//
CFbsBitmap & CActivePalette2Item::Icon(void) const
    { 	// icon is always valid
	return *iIcon->Bitmap();
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::Mask()
// -----------------------------------------------------------------------------
//
CFbsBitmap * CActivePalette2Item::Mask(void) const
    {
	return (NULL == iIcon) ? NULL : iIcon->Mask();
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::Tooltip()
// -----------------------------------------------------------------------------
//
HBufC*	CActivePalette2Item::Tooltip(void) const
    {
	return iTooltip;
    }


// -----------------------------------------------------------------------------
// CActivePalette2Item::InitPluginL()
// -----------------------------------------------------------------------------
//
void CActivePalette2Item::InitPluginL(const TDesC8& aCustomDataDes, TInt aCustomDataInt)
    {
	// do construct
	iPlugin->ConstructL(GetPluginCreationParams(),aCustomDataDes,aCustomDataInt);
	// validate icon
	if (NULL == iIcon || NULL == iIcon->Bitmap())
	    {
		User::Leave(KErrGeneral); // plugin MUST produce an icon during its ConstructL()
	    }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Item::Plugin()
// -----------------------------------------------------------------------------
//
CActivePalettePluginBase& CActivePalette2Item::Plugin(void) const
    {
	return *iPlugin;
    }


// End of File  
