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
* Description:  Active Palette Base plugin class*
*/


/**
 * @file CActivePalettePluginBase.cpp
 * Active Palette Base plugin class
 */

// INCLUDE FILES

#include "CActivePalettePluginBase.h"
#include "ActivePalette2Logger.h"

#include "ActivePalette2Utils.h"

#include <fbs.h>
#include <bitstd.h>

#include "ActivePalette2Cfg.h"


// CONSTANTS

const TInt KAnimPhase1 = 0;
const TInt KAnimPhase2 = 1;
const TInt KAnimPhaseMax = KAnimPhase2;
const TInt KAnimNofPhases = 2;

const TInt KAnimDeviatinFactorMul = 1;
const TInt KAnimDeviatinFactorDiv = 5;


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CAnimationHelper::CAnimationHelper()
// C++ constructor
// -----------------------------------------------------------------------------
//
CAnimationHelper::CAnimationHelper()
    {
	// empty
    }

// -----------------------------------------------------------------------------
// CAnimationHelper::ConstructL()
// 2nd phase construction
// -----------------------------------------------------------------------------
//
void CAnimationHelper::ConstructL(const TSize aSize, const TBool aMaskNeeded)
    {
	ActivePalette2Utils::CreateDrawingObjectsL(aSize,&iFrame,&iFrameDevice,&iFrameGc);
	if (aMaskNeeded)
	    {
		ActivePalette2Utils::CreateDrawingObjectsL(EGray256,
			aSize,&iFrameMask,&iFrameMaskDevice,&iFrameMaskGc);		
	    }
    }

// -----------------------------------------------------------------------------
// CAnimationHelper::~CAnimationHelper()
// Destructor
// -----------------------------------------------------------------------------
//
CAnimationHelper::~CAnimationHelper()
    {
    // DeleteDrawingObjects checks passed pointers to ensure they are not null
	ActivePalette2Utils::DeleteDrawingObjects(&iFrame,&iFrameDevice,&iFrameGc);
	ActivePalette2Utils::DeleteDrawingObjects(&iFrameMask,&iFrameMaskDevice,&iFrameMaskGc);
    }

// -----------------------------------------------------------------------------
// CAnimationHelper::NewL()
// Symbian 2 phase construction
// -----------------------------------------------------------------------------
//
CAnimationHelper * CAnimationHelper::NewL(const TSize aSize, const TBool aMaskNeeded)
    {
	CAnimationHelper * self = new (ELeave) CAnimationHelper();
	
	CleanupStack::PushL(self);
	self->ConstructL(aSize, aMaskNeeded);
	CleanupStack::Pop(self);
	
	return self;
    }

// -----------------------------------------------------------------------------
// NActivePalettePluginFactory::TPluginCreationParams::PluginObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C MActivePalettePluginObserver& NActivePalettePluginFactory::TPluginCreationParams::PluginObserver(void) const 
    { 
	return iObserver; 
    }

// -----------------------------------------------------------------------------
// NActivePalettePluginFactory::TPluginCreationParams::Dimensions(void)
// -----------------------------------------------------------------------------
//
EXPORT_C TSize NActivePalettePluginFactory::TPluginCreationParams::Dimensions(void) const
    {
	return iDimensions;	
    }

// -----------------------------------------------------------------------------
// NActivePalettePluginFactory::TPluginCreationParams::TPluginCreationParams()
// -----------------------------------------------------------------------------
//
EXPORT_C NActivePalettePluginFactory::TPluginCreationParams::TPluginCreationParams(MActivePalettePluginObserver & aObserver,
						  												  const TSize& aDimensions)
:   iObserver(aObserver),
    iDimensions(aDimensions)
    {
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::CActivePalettePluginBase()
// C++ constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CActivePalettePluginBase::CActivePalettePluginBase(const NActivePalettePluginFactory::TPluginCreationParams & aCreationParams)
:   iPluginObserver(aCreationParams.PluginObserver())
    {
	LOGTEXT( _L( "CActivePalettePluginBase::CActivePalettePluginBase" ) );
	PluginObserver().NotifyPluginCreated(this);
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::~CActivePalettePluginBase()
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CActivePalettePluginBase::~CActivePalettePluginBase(void)
    {
	LOGTEXT( _L( "CActivePalettePluginBase::~CActivePalettePluginBase" ) );
	CBase::Delete(iAnimationHelper);
	iAnimationHelper = NULL;
	PluginObserver().NotifyPluginDestroyed(this);
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::ConstructL()
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
EXPORT_C void CActivePalettePluginBase::ConstructL(const NActivePalettePluginFactory::TPluginCreationParams& /*aCreationParams*/, 
									               const TDesC8 & /*aCustomDataDes*/,
									               const TInt /*aCustomDataInt*/)
    {
	LOGTEXT( _L( "CActivePalettePluginBase::ConstructL" ) );
	// empty
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::Shutdown()
// -----------------------------------------------------------------------------
//
EXPORT_C void CActivePalettePluginBase::Shutdown(void)
    {
	LOGTEXT( _L( "CActivePalettePluginBase::Shutdown" ) );
	// empty
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::HandleMessageL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CActivePalettePluginBase::HandleMessageL(const TInt aMessageID,
                                                       const TDesC8& /*aDataDes*/,
                                                       const TInt /*aDataInt*/)
    {
	LOGTEXT1( _L( "CActivePalettePluginBase::HandleMessageL, message ID: %d" ), aMessageID );
	// empty
	if (aMessageID)
	    {
	    // no compiler warning
	    }
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::PrepareAniFramesL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CActivePalettePluginBase::PrepareAniFramesL(const TInt aAniType,
									  					  TPluginAnimationInfo& aAniInfo,
									  					  const TPluginAnimationInfo& aDefaultAniInfo,
				 						  	   		      const CFbsBitmap& aItemIcon,
							      		  			      const CFbsBitmap* aItemIconMask)
    {
	LOGTEXT1( _L( "CActivePalettePluginBase::PrepareAniFramesL, AniType ID: %d" ), aAniType);
	
	// use default animation info
	aAniInfo = aDefaultAniInfo; 
	
	// create data for icon and mask frames
	iAnimationHelper = CAnimationHelper::NewL(aDefaultAniInfo.iFrameSize,
											  (NULL != aItemIconMask));	

	if (aAniType)
	    {
	    // no compiler warning
	    }
	if (aItemIcon.Handle())
	    {
	    // no compiler warning
	    }
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::ReleaseAniFrames()
// -----------------------------------------------------------------------------
//
EXPORT_C void CActivePalettePluginBase::ReleaseAniFrames(const TInt aAniType)
    {
	LOGTEXT1( _L( "CActivePalettePluginBase::ReleaseAniFrames, AniType ID: %d" ), aAniType);

	CBase::Delete(iAnimationHelper);
	iAnimationHelper = NULL;

	if (aAniType) 
	    {
	    // no compiler warning
	    }
    }

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::ProduceAniFrame(
// -----------------------------------------------------------------------------
//
EXPORT_C void CActivePalettePluginBase::ProduceAniFrame(
    TInt aAniType,
    TInt aFrameNo,
    const TPluginAnimationInfo& aAniInfo,
    const CFbsBitmap& aItemIcon,
    const CFbsBitmap* aItemIconMask,
    CFbsBitmap** aFrame,
    CFbsBitmap** aFrameMask,
    TBool& aOwnershipTransferForFrame,
    TBool& aOwnershipTransferForMask)
    {
	LOGTEXT2( 
		_L( "CActivePalettePluginBase::ProduceAniFrame, AniType ID: %d, frame no : %d" ), 
		aAniType,
		aFrameNo);

	CAnimationHelper* ani = static_cast<CAnimationHelper*>(iAnimationHelper);

	aOwnershipTransferForFrame = EFalse;
	aOwnershipTransferForMask  = EFalse;		
	
	*aFrame 		= ani->iFrame;
	*aFrameMask 	= ani->iFrameMask;

	// empty mask, transparent
	if (NULL != ani->iFrameMaskGc)
	    {		
		ani->iFrameMaskGc->Reset();
		ani->iFrameMaskGc->SetBrushColor(KRgbBlack);
		ani->iFrameMaskGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
		ani->iFrameMaskGc->Clear(); 
	    }

	// empty frame	
	ani->iFrameGc->Reset();
	ani->iFrameGc->SetBrushColor(KRgbWhite);
	ani->iFrameGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	ani->iFrameGc->Clear(); 
	
	// ani frame
	TInt phase = Min(KAnimPhaseMax, 
					 KAnimNofPhases * aFrameNo / aAniInfo.iNofFrames);
	
	TInt inPhaseMax = aAniInfo.iNofFrames / KAnimNofPhases;
	
	TInt inPhaseOfs = Min(inPhaseMax, aFrameNo - (phase * KAnimPhaseMax));

	TInt lenX = aAniInfo.iFrameSize.iWidth;
	TInt lenY = aAniInfo.iFrameSize.iHeight;
	
	TInt devX = lenX * KAnimDeviatinFactorMul / KAnimDeviatinFactorDiv;
	TInt devY = lenY * KAnimDeviatinFactorMul / KAnimDeviatinFactorDiv;
	
	TInt szx(0);
	TInt szy(0);
	
	TInt addX = devX * inPhaseOfs / inPhaseMax;
	TInt addY = devY * inPhaseOfs / inPhaseMax;
	
	switch (phase)
	    {
		case KAnimPhase1:
			szx = lenX - addX;
			szy = lenY - addY;
			break;

		case KAnimPhase2:
		default:
			szx = lenX - devX + addX;
			szy = lenY - devY + addY;
			break;
	    }

	TRect frameRect(TPoint( (lenX - szx) / 2, (lenY - szy) / 2),
					TSize(szx, szy));
	TRect iconRect(TPoint(0,0), aItemIcon.SizeInPixels());
	
	if (NULL != aItemIconMask && NULL != ani->iFrameMaskGc)
	    {
		ani->iFrameMaskGc->DrawBitmap(frameRect,aItemIconMask,iconRect);
	    }
	
	ani->iFrameGc->DrawBitmap(frameRect,&aItemIcon,iconRect);		
	
	if (aAniType)
	    {
	    // no compiler warning
	    }
    }

// End of File  
