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
* Description:  Active Palette 2 Model*
*/


/**
 * @file ActivePalette2Model.cpp
 * Active Palette 2 Model
 */

// INCLUDE FILES
#include <activepalette2eventdata.h>
#include <activepalette2observer.h>
#include <activepalette2itemvisible.h>
#include <activepalette2genericpluginint.h>

#include "ActivePalette2Item.h"
#include "ActivePalette2Model.h"
#include "ActivePalette2ModelObserver.h"
#include "ActivePalette2Logger.h"
#include "ActivePalette2Cfg.h"
#include "activepalette2configuration.h"
#include "ActivePalette2Utils.h"


// -----------------------------------------------------------------------------
// TRendererCallBacks::TRendererCallBacks()
// -----------------------------------------------------------------------------
//
TRendererCallBacks::TRendererCallBacks()
:   iTooltipTimer(NULL),
    iFocusTimer(NULL),
    iItemScrollTimer(NULL),
    iAppearanceTimer(NULL),
    iTooltipDescriptive(NULL),
    iFocusDescriptive(NULL),
    iItemScrollDescriptive(NULL),
    iAppearanceDescriptive(NULL)
    {
    // no implementation needed
    }

                            


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CActivePalette2Model::NewL()
// 2 phase construction
// -----------------------------------------------------------------------------
//
CActivePalette2Model* CActivePalette2Model::NewL(
    MActivePalette2ModelObserver& aModelObserver,
    TSize                         aItemSize, 
    const TRendererCallBacks&     aCallbacks,
		CActivePalette2Configuration* aConfiguration /*=NULL*/ )
  {
	CActivePalette2Model * self = 
	    new (ELeave) CActivePalette2Model( aModelObserver, 
	                                       aItemSize, 
	                                       aCallbacks, 
	                                       aConfiguration );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // this
	return self;
  }

// -----------------------------------------------------------------------------
// CActivePalette2Model::CActivePalette2Model()
// C++ Constuctor
// -----------------------------------------------------------------------------
//
CActivePalette2Model::CActivePalette2Model( 
  MActivePalette2ModelObserver& aModelObserver,
  TSize                         aItemSize, 
  const TRendererCallBacks&     aCallbacks,
  CActivePalette2Configuration* aConfiguration )
  : iPreviousItemID(KInvalidId),
    iFocusedItemID(KInvalidId),
    iPaletteMoveCurrentOffset(0),
    iPaletteMoveOffsetTotalFrames(NAP2Cfg::KPaletteAppearFrames),
    iPaletteMoveAnimationDuration(NAP2Cfg::KPaletteAppearTotalTime / KMicrosecondsPerMillisecond),
    iAnimState(EPaletteAnimIdle),
    iCallbacks(aCallbacks),
    iModelObserver(aModelObserver),
    iItemSize(aItemSize),
    iTooltipConfig()
  {
  LOGTEXT( _L( "CActivePalette2Model::CActivePalette2Model entered" ));	
  if( aConfiguration )
    {
    aConfiguration->TooltipConfiguration( iTooltipConfig );
    }
  else
    {
    iTooltipConfig.iPreshowDelay     = NAP2Cfg::KTooltipPreshowDelay;
    iTooltipConfig.iFullshowPeriod   = NAP2Cfg::KTooltipFullshowPeriod;
    iTooltipConfig.iTimerTickLength  = NAP2Cfg::KTooltipTimerTickLength;
    iTooltipConfig.iTransitionSpeed  = NAP2Cfg::KTooltipShowSpeed;
    iTooltipConfig.iTransitionFrames = KErrNotSupported;
    }
  LOGTEXT( _L( "CActivePalette2Model::CActivePalette2Model left" ));	
  }
    
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::ConstructL(void)
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::ConstructL(void)
    {
	iTooltipTimer = CPeriodic::NewL(CActive::EPriorityHigh); // high priority for smooth animation
	iFocusChangeTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    iItemScrollTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    iItemAnimTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    iPaletteMoveTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::~CActivePalette2Model()
// Destructor
// -----------------------------------------------------------------------------
//
CActivePalette2Model::~CActivePalette2Model()
    {
	LOGTEXT( _L( "CActivePalette2Model::~CActivePalette2Model entered" ));

	if (iTooltipTimer) 
	    {
		iTooltipTimer->Cancel();	
		delete iTooltipTimer;
	    }
	
	if (iFocusChangeTimer) 
	    {
		iFocusChangeTimer->Cancel();	
		delete iFocusChangeTimer;
	    }

    if (iItemScrollTimer)
        {
		iItemScrollTimer->Cancel();	
		delete iItemScrollTimer;
        }

	if (iItemAnimTimer) 
	    {
		iItemAnimTimer->Cancel();	
		delete iItemAnimTimer;
	    }

	if (iPaletteMoveTimer) 
	    {
		iPaletteMoveTimer->Cancel();	
		delete iPaletteMoveTimer;
	    }

	iItemsArray.ResetAndDestroy();
	iItemsArray.Close();
	iVisibilityArray.Reset();
	iVisibilityArray.Close();
	
	LOGTEXT( _L( "CActivePalette2Model::~CActivePalette2Model left" ));
    }


// -----------------------------------------------------------------------------
// CActivePalette2Model::FindItem()
// item data management
// -----------------------------------------------------------------------------
//
CActivePalette2Item* CActivePalette2Model::FindItem(const TInt aItemId) const
    {
	TInt index = ItemIndexFromId(aItemId);
	
	if ( ValidItemIndex(index) ) 
	    {
	    return static_cast<CActivePalette2Item *>(iItemsArray[index]);
	    }
    else
        {
    	return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::DoInstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::DoInstallItemL(TInt aItemId, 
                                          TBool aVisible,
										  const TUid& aPluginUid, 
										  const TDesC8& aCustomDataDes, 
										  TInt aCustomDataInt)
    {
	// check if item ID is unique
	if ( ItemIndexFromId(aItemId) != KInvalidItemIndex )
	    {
		return KErrArgument;
	    }

	// item and plugin instantiate
	CActivePalette2Item* item = CActivePalette2Item::NewLC(aItemId,aPluginUid, *this);

	item->InitPluginL(aCustomDataDes, aCustomDataInt);

    User::LeaveIfError(iItemsArray.Append(item));
    User::LeaveIfError(iVisibilityArray.Append(aVisible));

	CleanupStack::Pop(item);

    if ( aVisible )
        {
        EnforceConstraints();
        } 
   
	return KErrNone;	
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::RemoveItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::RemoveItem(const TInt aItemId)
    {
	LOGTEXT1( _L( "CActivePalette2Model::RemoveItem entered, Item ID: %d" ), 
			 aItemId);

	TInt res(KErrNone);
	
	CActivePalette2Item* item = FindItem(aItemId);
	TInt index = ItemIndexFromId(aItemId);
	
	if (item)    // valid item
	    {
		iItemsArray.Remove(index); // unregister
		iVisibilityArray.Remove(index);
		
		// destroy item	
		CBase::Delete(item);     // implies plugin shutodown and DLL unload
		
        EnforceConstraints();
	    }
	else
	    {
		res = KErrArgument;
	    }
	
	LOGTEXT1( _L( "CActivePalette2Model::RemoveItem left, res %d" ), res);

	return res;	
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::HandleItemSelected()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::HandleItemSelected(const TInt aItemIndex)
    {
	LOGTEXT1( _L( "CActivePalette2Model::HandleItemSelected entered, Item ID: %d" ), 
			 aItemIndex);

    if ( ValidItemIndex(aItemIndex) )
        {
    	CActivePalette2Item* item = iItemsArray[aItemIndex];
    	if (item)
        	{
    		TInt err;

    		LOGTEXT( _L( "... calling plugin's HandleItemSelectedL" ));

    		TRAP(err, item->Plugin().HandleItemSelectedL());

    		LOGTEXT1( _L( "... res : %d" ), err);

    		if (KErrNone != err)
    		    {
    		    // leave during item selection handling, invoke callback now
    			NotifyItemComplete(*item, err, KNullDesC8, KActivePaletteNoDataInt);
    		    }
    	    } 
    	else
    	    {
    		LOGTEXT( _L( "... INTERNAL FAULT: item object not found" ));	
    	    }
        }
        	
	LOGTEXT( _L( "CActivePalette2Model::HandleItemSelected left" ));
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::DispatchMessage()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::DispatchMessage(const TInt aItemId, const TInt aMessageID, const TDesC8& aDataDes, const TInt aDataInt)
    {
	LOGTEXT2( _L( "CActivePalette2Model::DispatchMessgage entered, Item ID: %d, Message ID : %d" ), 
			 aItemId, aMessageID);

	TInt res;
	CActivePalette2Item* item = FindItem(aItemId);
	if (item)
	    {
		TInt err;

		LOGTEXT( _L( "...calling plugin's HandleMessageL" ));

		TRAP(err, item->Plugin().HandleMessageL(aMessageID, aDataDes, aDataInt));

		LOGTEXT1( _L( "... res : %d" ), err);

		res = err; // retransmit error code
	    }
	else
	    {
		res = KErrArgument;
	    }

	LOGTEXT1( _L( "CActivePalette2Model::DispatchMessgage left, res %d" ), res);

	return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::Observer()
// -----------------------------------------------------------------------------
//
MActivePalette2ModelObserver& CActivePalette2Model::Observer(void) const
    {
	return iModelObserver;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemIndexFromId()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemIndexFromId(const TInt aItemId) const
    {
    TInt res = KInvalidItemIndex;
    TInt currentItemIndex = 0;
    
    while ( res == KInvalidItemIndex && ValidItemIndex(currentItemIndex) )
        {
        if ( iItemsArray[currentItemIndex]->ItemId() == aItemId )
            {
            res = currentItemIndex;
            }
        else
            {
            currentItemIndex++;
            }
        }

    return res;    
    }





// -----------------------------------------------------------------------------
// CActivePalette2Model::TooltipText()
// -----------------------------------------------------------------------------
//
TDesC* CActivePalette2Model::TooltipText()
    {
    if ( ValidItemIndex(iFocusedItem) )
        {
        CActivePalette2Item& api = static_cast<CActivePalette2Item &> (*iItemsArray[iFocusedItem]);
        return api.Tooltip();
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ShowTooltip()
// -----------------------------------------------------------------------------
//
TBool CActivePalette2Model::ShowTooltip()
    {
    return iShowTooltip;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::TooltipCurrentFrame()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::TooltipCurrentFrame()
    {
    return iTooltipCurrentFrame;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::TooltipTotalFrames()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::TooltipTotalFrames()
    {
    return iTooltipTotalFrames;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::TooltipScreenPosition()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::TooltipScreenPosition()
    {
    return FocusedItem();
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::CompelTooltip()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::CompelTooltip()
    {
    TInt tooltip_width = iModelObserver.TooltipUpdated();

    if( iTooltipConfig.iTransitionFrames > 0 )
      iTooltipTotalFrames = iTooltipConfig.iTransitionFrames;
    else
      iTooltipTotalFrames = tooltip_width / iTooltipConfig.iTransitionSpeed;

    TooltipSetState( ETooltipRise );
    }

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::RemoveTooltip(void)
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::RemoveTooltip(void)
	{
	TooltipSetState(ETooltipNone);
	}

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipSteadyAniAction()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::TooltipSteadyAniAction(void)
	{
	// start shrinking
    TooltipSetState(ETooltipSet);
	return KCallbackFinished;	
	}

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipRiseAniAction()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::TooltipRiseAniAction(void)
	{
	TInt res = KCallbackFinished;
	
	iShowTooltip = ETrue;
	
	if ( iCallbacks.iTooltipTimer )
    	{
    	if (iTooltipCurrentFrame >= iTooltipTotalFrames)
    		{
    		TooltipSetState(ETooltipSteady);
    		}
    	else
    		{
    		res = KCallbackCallAgain;
    		iTooltipCurrentFrame++;
    		}
    		
        iCallbacks.iTooltipTimer->TooltipAnimated();
        }
    else if ( iCallbacks.iTooltipDescriptive )
        {
        TInt duration( (iTooltipTotalFrames*iTooltipConfig.iTimerTickLength) / KMicrosecondsPerMillisecond );
        iCallbacks.iTooltipDescriptive->AnimateTooltip( ETrue, duration );
        iTooltipTimer->Cancel();
        }
    else
        {
        // just for Lint.
        }
        
	return res;
	}
	
// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipDisappearAniAction()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::TooltipDisappearAniAction(void)
	{
	TInt res = KCallbackFinished;
	
	if ( iCallbacks.iTooltipTimer )
    	{
    	if (iTooltipCurrentFrame <= 0)
    		{
    		TooltipSetState(ETooltipNone);
    		}
    	else
    		{
    		res = KCallbackCallAgain;
    		iTooltipCurrentFrame--;
    		}
    		
        if ( iCallbacks.iTooltipTimer )
            {		
            iCallbacks.iTooltipTimer->TooltipAnimated();
            }
        }
    else if ( iCallbacks.iTooltipDescriptive )
        {
        TInt duration( (iTooltipTotalFrames*iTooltipConfig.iTimerTickLength) / KMicrosecondsPerMillisecond );
        iCallbacks.iTooltipDescriptive->AnimateTooltip( EFalse, duration );
        iTooltipTimer->Cancel();
        }
    else
        {
        // just for Lint.
        }
        
	return res;
	}

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::TooltipTimerCallback(TAny* aPtr)
	{
	CActivePalette2Model* self = (CActivePalette2Model*) aPtr;
	TInt res = KCallbackFinished;

	switch (self->iTooltipState)
		{
		case ETooltipSteady:
			res = self->TooltipSteadyAniAction();
			break;

		case ETooltipRise:
			res = self->TooltipRiseAniAction();
			break;
			
		case ETooltipSet:
			res = self->TooltipDisappearAniAction();
			break;

		default:
			break;
		}
		
	return res;
	}

// -----------------------------------------------------------------------------
// CActivePalette2Tooltip::TooltipSetState()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::TooltipSetState(TTooltipState aState)
	{
	iTooltipTimer->Cancel();
	TBool isOk = ETrue;
	
	switch (aState)
		{
		case ETooltipNone:
			iTooltipTimer->Cancel();
		    if ( iShowTooltip || iTooltipCurrentFrame > 0 )
		        {
    			iShowTooltip = EFalse;
    			iTooltipCurrentFrame = 0;
                iModelObserver.ShowTooltipUpdated();
    		    }
			break;

		case ETooltipSteady:
			iTooltipTimer->Start(
  			iTooltipConfig.iFullshowPeriod, 
  			iTooltipConfig.iFullshowPeriod, 
  			TCallBack(&CActivePalette2Model::TooltipTimerCallback,(TAny *)this));
			break;
			
		case ETooltipRise:
			iTooltipCurrentFrame = 0;			
			iTooltipTimer->Start(
  			iTooltipConfig.iPreshowDelay,
  			iTooltipConfig.iTimerTickLength,
  			TCallBack(&CActivePalette2Model::TooltipTimerCallback,(TAny *)this));
			break;
		
		case ETooltipSet:
//			iTooltipCurrentFrame = 0;			
			iTooltipTimer->Start(
  			iTooltipConfig.iTimerTickLength,
  			iTooltipConfig.iTimerTickLength,
				TCallBack(&CActivePalette2Model::TooltipTimerCallback,(TAny *)this));
			break;
			
		default:
			isOk = EFalse;
			break;
		}
		
	if (isOk)
		{
		iTooltipState = aState;
		}
	}

// -----------------------------------------------------------------------------
// CActivePalette2Model::FocusChangeComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::AnimateTooltipRendererComplete()
    {
	switch (iTooltipState)
		{
		case ETooltipRise:
		    TooltipSetState(ETooltipSteady);
			break;
		
		case ETooltipSet:
		    TooltipSetState(ETooltipNone);
			break;
			
		default:
		case ETooltipNone:
		case ETooltipSteady:
			break;
		}
    }


// -----------------------------------------------------------------------------
// CActivePalette2Model::CountItemsOnScreen()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::CountItemsOnScreen(void)
    {
    return iNoItemsOnScreen;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ShowTopScrollIndicator()
// -----------------------------------------------------------------------------
//
TBool CActivePalette2Model::ShowTopScrollIndicator(void)
    {
    return iShowTopScrollArrow;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ShowBottomScrollIndicator()
// -----------------------------------------------------------------------------
//
TBool CActivePalette2Model::ShowBottomScrollIndicator(void)
    {
    return iShowBottomScrollArrow;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::InitialiseConstraints()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::InitialiseConstraints(void)
    {
    iTopItemOnScreen = FindVisibleItem(0);
    
    iFocusedItem = iTopItemOnScreen;

    iNoVisibleItems = CountVisibleItems(0);

    iNoItemsOnScreen = Min(NAP2Cfg::KMaxNofItemsInView, iNoVisibleItems);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::EnforceConstraints()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::EnforceConstraints(void)
    {
    CompleteAnyItemBasedAnimations();

    iNoVisibleItems = CountVisibleItems(0);

    iNoItemsOnScreen = Min(NAP2Cfg::KMaxNofItemsInView, iNoVisibleItems);
    
    iTopItemOnScreen = FindVisibleItem(iTopItemOnScreen);
    
    if(CountVisibleItems(iTopItemOnScreen) < iNoItemsOnScreen)
        {
        iTopItemOnScreen = FindVisibleItem(0);
        
        if ( iNoVisibleItems > iNoItemsOnScreen )
            {
            while ( CountVisibleItems(iTopItemOnScreen) < iNoItemsOnScreen )
                {
                iTopItemOnScreen = FindVisibleItem(iTopItemOnScreen + 1);
                }
            }
        }

    if ( iNoVisibleItems > 0 )
        {
        if ( iFocusedItem < 0 )
            {
            iFocusedItem = FindVisibleItem(0);
            }

        if ( iFocusedItem >= iItemsArray.Count() )
            {
            iFocusedItem = PreviousVisibleItem(iItemsArray.Count() - 1);
            }

        if ( ValidItemIndex(iFocusedItem) )
            {
            if ( !iVisibilityArray[iFocusedItem] )
                {
                TInt previousFocusedItem = iFocusedItem;
                
                iFocusedItem = FindVisibleItem(iFocusedItem);
                
                if ( iFocusedItem == KInvalidItemIndex )
                    {
                    iFocusedItem = PreviousVisibleItem(previousFocusedItem);
                    }
                    
                if ( iFocusedItem == KInvalidItemIndex )
                    {
                    iFocusedItem = FindVisibleItem(0);
                    }
                }
            }

        if ( iFocusedItem < iTopItemOnScreen )
            {
            iTopItemOnScreen = iFocusedItem;
            }
        else
            {        
            while ( iFocusedItem > BottomItemOnScreen() )
                {
                iTopItemOnScreen = FindVisibleItem(iTopItemOnScreen + 1);
                }
            }
        }
    else
        {
        iFocusedItem = KInvalidItemIndex;
        }

    ItemsUpdated();
    FocusedItemUpdated();
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::CompleteAnyItemBasedAnimations()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::CompleteAnyItemBasedAnimations()
    {
    switch (iAnimState)
        {
        case EPaletteAnimItemScroll:
        case EPaletteAnimFocusChange:
        case EPaletteAnimItemAnim:
            SetAnimState(EPaletteAnimIdle);
            break;

        default:
            // do nothing
            break;            
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::BottomItemOnScreen()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::BottomItemOnScreen(void)
    {
    TInt bottomItemOnScreen = iTopItemOnScreen;

    for(TInt i = 1; i < iNoItemsOnScreen; i++)
        {
        bottomItemOnScreen = FindVisibleItem(bottomItemOnScreen + 1);
        }
        
    return bottomItemOnScreen;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::FindVisibleItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::FindVisibleItem(TInt aStartingItem)
    {
    TInt result = KInvalidItemIndex;
    TInt currentItem = aStartingItem;
    TBool found = EFalse;
    TInt arraySize = iVisibilityArray.Count();
    
    if ( aStartingItem != KInvalidItemIndex )
        {
        while(!found && currentItem < arraySize)
            {
            if(iVisibilityArray[currentItem])
                {
                result = currentItem;
                found = ETrue;
                }
            else
                {
                currentItem++;
                }
            }
        }
                
    return result;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::PreviousVisibleItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::PreviousVisibleItem(TInt aStartingItem)
    {
    TInt result = KInvalidItemIndex;
    TInt currentItem = aStartingItem;
    TBool found = EFalse;
    
    if ( aStartingItem != KInvalidItemIndex )
        {
        while(!found && currentItem >= 0)
            {
            if(iVisibilityArray[currentItem])
                {
                result = currentItem;
                found = ETrue;
                }
            else
                {
                currentItem--;
                }
            }
        }
                
    return result;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::CountVisibleItems()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::CountVisibleItems(TInt aStartingItem)
    {
    TInt result = 0;
    TInt currentItem = aStartingItem;
    TInt arraySize = iVisibilityArray.Count();
    
    if ( aStartingItem != KInvalidItemIndex )
        {
        while(currentItem < arraySize)
            {
            if(iVisibilityArray[currentItem])
                {
                result++;
                }
            currentItem++;
            }
        }
    else
        {
        result = KInvalidItemIndex;
        }
                    
    return result;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemIndexFromScreenPos()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemIndexFromScreenPos(TInt aScreenPos)
    {
    TInt result = KInvalidItemIndex;
    
    if ( aScreenPos >= 0 && iNoItemsOnScreen > 0 && aScreenPos < NAP2Cfg::KMaxNofItemsInView + 1 )
        {
        result = iOnScreenItemIndex[aScreenPos];
        }
            
    return result;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemFromScreenPos()
// -----------------------------------------------------------------------------
//
CActivePalette2Item* CActivePalette2Model::ItemFromScreenPos(TInt aScreenPos)
    {
    CActivePalette2Item* result = NULL;
    
    TInt itemIndex = ItemIndexFromScreenPos(aScreenPos);
    
    if ( itemIndex != KInvalidItemIndex )
        {
        result = iItemsArray[itemIndex];
        }
        
    return result;        
    }


// -----------------------------------------------------------------------------
// CActivePalette2Model::ScreenPosFromItemIndex()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ScreenPosFromItemIndex(TInt aItem)
    {
    TInt result = KInvalidScreenPos;
    TInt currentItem = iTopItemOnScreen;
    TInt currentScreenPos = 0;
    TBool found = EFalse;
    
    if ( aItem >= iItemsArray.Count() )
        {
        result = iNoItemsOnScreen;
        }
    else if ( aItem < 0)
        {
        result = NAP2Cfg::KScreenPosAboveTop;
        }
    else
        {
        while ( !found && currentScreenPos < iNoItemsOnScreen )
            {
            if ( aItem == currentItem )
                {
                found = ETrue;
                }
            else
                {
                currentItem = FindVisibleItem(currentItem + 1);
                currentScreenPos++;
                }
            }
        
        if ( found )
            {
            result = currentScreenPos;
            }
        }
                
    return result;
    }


// -----------------------------------------------------------------------------
// CActivePalette2Model::FocusedItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::FocusedItem()
    {
    return ScreenPosFromItemIndex(iFocusedItem);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::FocusCurrentOffset()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::FocusCurrentOffset()
    {
    return iFocusedCurrentOffset;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::FocusCurrentTotalOffsetFrames()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::FocusCurrentTotalOffsetFrames()
    {
    return NAP2Cfg::KFocusChangeFrames;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::PaletteAppearingCurrentFrame()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::PaletteAppearingCurrentFrame()
    {
    return iPaletteMoveCurrentOffset;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::PaletteAppearingTotalFrames()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::PaletteAppearingTotalFrames()
    {
    return iPaletteMoveOffsetTotalFrames;
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::ProcessInputEvent()
// -----------------------------------------------------------------------------
//
TBool CActivePalette2Model::ProcessInputEvent(TActivePalette2InputEvent aEvent)
    {
    TBool eventConsumed = EFalse;

    if ( PaletteActive() )
        {
        SetAnimState(EPaletteAnimIdle);
        
        switch(aEvent)
            {
            case EAP2InputUp:
                RemoveTooltip();

                iFocusChangeTargetItem = PreviousVisibleItem(iFocusedItem - 1);
                
                if ( iFocusChangeTargetItem != KInvalidItemIndex && iFocusChangeTargetItem != iFocusedItem )
                    {
                    if ( ScreenPosFromItemIndex(iFocusedItem) == 0 )
                        {
                        // Item scroll
                        iTopItemOnScreen = iFocusChangeTargetItem;
                        iFocusedItem = iFocusChangeTargetItem;
                        
                        iItemScrollCurrentOffset = NAP2Cfg::KItemScrollFrames;
                        
                        ItemsUpdated();
                        
                        ScrollItems(-1);
                        }
                    else
                        {
                        // Focus change
                        ChangeFocus();

                        eventConsumed = ETrue;
              			}
        			}
        	    else
        	        {
        	        iFocusChangeTargetItem = PreviousVisibleItem(iItemsArray.Count() - 1);
        	        
                    if ( iFocusChangeTargetItem != KInvalidItemIndex && iFocusChangeTargetItem != iFocusedItem )
                        {
            	        // Focus wrap
            	        TInt screenPos = iNoItemsOnScreen - 1;
            	        iTopItemOnScreen = iFocusChangeTargetItem;
            	        
            	        while ( screenPos > 0 )
                	        {
                	        iTopItemOnScreen = PreviousVisibleItem(iTopItemOnScreen-1);
                	        screenPos--;
                	        }
                        
                        ItemsUpdated();
                        
                        iFocusedItem = iItemsArray.Count();
                        
                        ChangeFocus();

                        eventConsumed = ETrue;
            	        }
            	    }
                break;
            case EAP2InputDown:
                RemoveTooltip();

                iFocusChangeTargetItem = FindVisibleItem(iFocusedItem + 1);

                if ( iFocusChangeTargetItem != KInvalidItemIndex && iFocusChangeTargetItem != iFocusedItem )
                    {
                    if ( ScreenPosFromItemIndex(iFocusedItem) == (iNoItemsOnScreen - 1))
                        {
                        // Item scroll
                        iItemScrollCurrentOffset = 0;

                        ScrollItems(+1);                        
                        }
                    else
                        {
                        // Focus change
                        ChangeFocus();
            			}
            			
                    eventConsumed = ETrue;
        			}
        		else
        		    {
        		    // wrap focus to top
                    iFocusChangeTargetItem = FindVisibleItem(0);
                    
                    if ( iFocusChangeTargetItem != KInvalidItemIndex && iFocusChangeTargetItem != iFocusedItem )
                        {
                        iTopItemOnScreen = iFocusChangeTargetItem;
                        ItemsUpdated();
                        iFocusedItem = -1;
                        
                        ChangeFocus();
            		    }
        		    }
        			
                break;

            case EAP2InputSelect:
                RemoveTooltip();
                
                if ( iFocusedItem >= 0 && iFocusedItem < iItemsArray.Count())
                    {
                    if ( iVisibilityArray[iFocusedItem] )
                        {
                    	TActivePalette2EventData res(PrepareEventData(*iItemsArray[iFocusedItem], KErrNone));
            		    
                        if ( iModelObserver.APObserver() )
                            {
                			iModelObserver.APObserver()->NotifyItemSelected(res);
                            }

            		    HandleItemSelected(iFocusedItem);
                    
                        eventConsumed = ETrue;
                        }
                    }
                break;
            }
        }

    return eventConsumed;
    }
    

// -----------------------------------------------------------------------------
// CActivePalette2Model::ScrollItems()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::ScrollItems(TInt aDirection)
    {
    SetAnimState(EPaletteAnimItemScroll);
        
    if ( iCallbacks.iItemScrollTimer )
        {
        iItemScrollDirection = aDirection;
        
    	iItemScrollTimer->Start(
    		NAP2Cfg::KItemScrollTimerTickLength,
    		NAP2Cfg::KItemScrollTimerTickLength,
    		TCallBack(&CActivePalette2Model::ItemScrollTimerCallback,(TAny *)this));
        }
    else if ( iCallbacks.iItemScrollDescriptive )
        {
        iCallbacks.iItemScrollDescriptive->AnimateItemScroll(aDirection, NAP2Cfg::KItemScrollTotalTime / KMicrosecondsPerMillisecond);
        }
    else
        {
        // just for Lint.
        }
    }
           
// -----------------------------------------------------------------------------
// CActivePalette2Model::ChangeFocus()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::ChangeFocus()
    {
    if ( iCallbacks.iFocusTimer )
        {
        if ( iFocusedItem < iFocusChangeTargetItem )
            {
            iFocusedCurrentOffset = 0;
            iFocusChangeDirection = +1;
            }
        else
            {
            iFocusedCurrentOffset = NAP2Cfg::KFocusChangeFrames;
            iFocusChangeDirection = -1;
            iFocusedItem = iFocusChangeTargetItem;
            }
        
        SetAnimState(EPaletteAnimFocusChange);
        iFocusChangeTimer->Start(
    	    NAP2Cfg::KFocusChangeTimerTickLength,
    	    NAP2Cfg::KFocusChangeTimerTickLength,
    	    TCallBack(&CActivePalette2Model::FocusChangeTimerCallback,(TAny *)this));
        }
    else if ( iCallbacks.iFocusDescriptive )
        {
        SetAnimState(EPaletteAnimFocusChange);
        iCallbacks.iFocusDescriptive->AnimateFocus(ScreenPosFromItemIndex(iFocusedItem), ScreenPosFromItemIndex(iFocusChangeTargetItem), NAP2Cfg::KFocusChangeTotalTime / KMicrosecondsPerMillisecond);
        }
    else
        {
        // lint
        }
    }





// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemAnimInfo()
// -----------------------------------------------------------------------------
//
CActivePalettePluginBase::TPluginAnimationInfo CActivePalette2Model::ItemAnimInfo()
    {
    return iAniItemInfo;
    }


// -----------------------------------------------------------------------------
// CActivePalette2Model::SetPaletteVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::SetPaletteVisibility(TBool aVisible, TBool aAnimated, TInt aDelayedStartMilliseconds)
    {
    // Check if we're already in an appearance animation
    if ( iAnimState == EPaletteAnimAppearing
         && iCallbacks.iAppearanceDescriptive )
        {
        // If so, update state
        iPaletteMoveCurrentOffset = iCallbacks.iAppearanceDescriptive->GuessCurrentAppearanceFrame();
        
        if ( iPaletteMoveCurrentOffset  < 0 || iPaletteMoveCurrentOffset  > iPaletteMoveOffsetTotalFrames )
            {
            iPaletteMoveOffsetTotalFrames = iPaletteMoveOffsetTotalFrames;
            }
        }
        
        
    if ( aAnimated || aDelayedStartMilliseconds > 0)
        {
        if ( aVisible )
            {
            if ( iPaletteMoveOffsetTotalFrames > iPaletteMoveCurrentOffset )
                {
                RemoveTooltip();
                SetAnimState(EPaletteAnimAppearing);
                iModelObserver.PaletteAppearingUpdated();

                if ( aAnimated )
                    {
                    iPaletteMoveAnimationDirection = +1;
                    }
                else
                    {
                    iPaletteMoveAnimationDirection = +iPaletteMoveOffsetTotalFrames;
                    }
                
                iPaletteMoveTimer->Cancel();
            	iPaletteMoveTimer->Start(Max(1, aDelayedStartMilliseconds * KMicrosecondsPerMillisecond), 
            	                        NAP2Cfg::KPaletteAppearTimerTickLength, 
            			                TCallBack(&CActivePalette2Model::PaletteAnimTimerCallback,(TAny*)this));
                }
            else if ( aAnimated && 0 >= iPaletteMoveAnimationDirection )
               {
               iPaletteMoveAnimationDirection = +1;
               }    
            }
        else
            {
            if ( iPaletteMoveCurrentOffset > 0 )
                {
                RemoveTooltip();
                SetAnimState(EPaletteAnimAppearing);
                iModelObserver.PaletteAppearingUpdated();
                
                if ( aAnimated )
                    {
                    iPaletteMoveAnimationDirection = -1;
                    }
                else
                    {
                    iPaletteMoveAnimationDirection = -iPaletteMoveOffsetTotalFrames;
                    }
                
                
                iPaletteMoveTimer->Cancel();
            	iPaletteMoveTimer->Start(Max(1, aDelayedStartMilliseconds * KMicrosecondsPerMillisecond), 
            	                        NAP2Cfg::KPaletteAppearTimerTickLength, 
            			                TCallBack(&CActivePalette2Model::PaletteAnimTimerCallback,(TAny*)this));
                }
                //Start fix for ERCK-77PCZJ
                else
                {
                iPaletteMoveAnimationDirection = -iPaletteMoveOffsetTotalFrames;
                SetAnimState(EPaletteAnimIdle);                              
                }
                //End fix ERCK-77PCZJ
            }
        }
    else
        {
        if ( aVisible )
            {
            if ( iPaletteMoveOffsetTotalFrames != iPaletteMoveCurrentOffset )
                {
                RemoveTooltip();
                SetAnimState(EPaletteAnimIdle);
                iPaletteMoveCurrentOffset = iPaletteMoveOffsetTotalFrames;
                iModelObserver.PaletteAppearingUpdated();
                }
            }
        else
            {
            if ( iPaletteMoveCurrentOffset != 0 )
                {
                RemoveTooltip();
                SetAnimState(EPaletteAnimIdle);
                iPaletteMoveCurrentOffset = 0;
                iModelObserver.PaletteAppearingUpdated();
                }
                //Start fix for ERCK-77PCZJ
                else
                {
                iPaletteMoveAnimationDirection = -iPaletteMoveOffsetTotalFrames;
                SetAnimState(EPaletteAnimIdle);                              
                }
                //End fix ERCK-77PCZJ
            }
        }
        
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CActivePalette2Model::NotifyItemComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::NotifyItemComplete(const CActivePalette2Item& aItem,
                            TInt aResult,
                            const TDesC8& aDataDes,
                            TInt aDataInt)
    {
	if ( iModelObserver.APObserver() )
    	{
        TActivePalette2EventData evt(PrepareEventData(aItem, aResult));
        iModelObserver.APObserver()->NotifyItemComplete(evt, aDataDes, aDataInt);
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::NotifyMessage()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::NotifyMessage(const CActivePalette2Item& aItem,
                       const TInt aResult,
                       const TInt aMessageID,
                       const TDesC8& aDataDes,
                       TInt aDataInt)
    {
	if ( iModelObserver.APObserver() )
    	{
	    TActivePalette2EventData eventData = PrepareEventData(aItem, aResult);
    	iModelObserver.APObserver()->NotifyMessage(eventData, aMessageID, aDataDes, aDataInt);
    	}
    }


// -----------------------------------------------------------------------------
// CActivePalette2Model::NotifyGraphicsChanged()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::NotifyGraphicsChanged(const CActivePalette2Item& /*aItem*/,
                               const TBool aIsIconChanged,
                               const TBool aIsTooltipChanged)
    {
    if ( aIsTooltipChanged )
        {
        RemoveTooltip();
        }

    if ( aIsIconChanged )
        {
        ItemsUpdated();
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::PrepareEventData()
// -----------------------------------------------------------------------------
//
TActivePalette2EventData CActivePalette2Model::PrepareEventData(const CActivePalette2Item& aItem,
                            TInt aResult)
    {
	TActivePalette2EventData evt
		(
		aResult,
		aItem.ItemId(),
		iPreviousItemID,
		aItem.ItemId(),
		aItem.PluginUid()
		);
		
    return evt;		
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetItemVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::SetItemVisibility(const TInt aItemId, const TBool aIsVisible)
    {
	TInt index = ItemIndexFromId(aItemId);
    TInt res = KErrNotFound;
    
    if ( ValidItemIndex(index) )
        {
        if ( iVisibilityArray[index] != aIsVisible )
            {
            iVisibilityArray[index] = aIsVisible;
            EnforceConstraints();
            }
        res = KErrNone;
        }
    
    return res;   
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::GetItemVisibility()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::GetItemVisibility(const TInt aItemId, TBool &aIsVisible) const
    {
	TInt index = ItemIndexFromId(aItemId);
    TInt res = KErrNone;

    if ( ValidItemIndex(index) )
        {
        aIsVisible = iVisibilityArray[index];
        }
    else
        {
        res = KErrArgument;
        }
        
    return res;        
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::GetAvailablePlugins()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::GetAvailablePlugins(RArray<TUid>& aPluginList) const
    {
    aPluginList.Reset();

    // Generic plugin always available    
    aPluginList.Append(TUid::Uid(KActivePalette2GenericPluginUID));
    
    AppendPluginIdIfAvailable(aPluginList, TUid::Uid(KPrintPluginUid),       TFileName(KPrintPluginDLLName));
    AppendPluginIdIfAvailable(aPluginList, TUid::Uid(KPrintIntentPluginUid), TFileName(KPrintIntentPluginDLLName));
    AppendPluginIdIfAvailable(aPluginList, TUid::Uid(KOnlineSharingPluginUid), TFileName(KOnlineSharingPluginDLLName));
#ifdef ACTIVEPALETTE_SUPPORT_PLUGIN_CAM
    AppendPluginIdIfAvailable(aPluginList, TUid::Uid(KCamAPPluginUid),       TFileName(KCamAPluginDLLName));
#endif
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::AppendPluginIdIfAvailable()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::AppendPluginIdIfAvailable(RArray<TUid>& aPluginList, const TUid aDllUid, const TFileName aDllFileName) const
    {
	RLibrary dll;
	
    TInt err = dll.Load(aDllFileName);
	
	if ( err == KErrNone )
    	{
    	aPluginList.Append(aDllUid);
    	}
	
    dll.Close();
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetPaletteVisibilityAnimationDuration()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::SetPaletteVisibilityAnimationDuration(TInt aTimeInMilliSeconds)
    {
    iPaletteMoveAnimationDuration = aTimeInMilliSeconds;
    
    TInt targetTotalFrames = (aTimeInMilliSeconds * KMicrosecondsPerMillisecond) / NAP2Cfg::KPaletteAppearTimerTickLength;
    
    if ( targetTotalFrames < 1 )
        {
        targetTotalFrames = 1;
        }

    // update current frame
    if ( iPaletteMoveCurrentOffset == iPaletteMoveOffsetTotalFrames )
        {
        iPaletteMoveCurrentOffset = targetTotalFrames;
        }
    else if ( iPaletteMoveCurrentOffset > 0 && iPaletteMoveOffsetTotalFrames > 0)
        {
        iPaletteMoveCurrentOffset = (iPaletteMoveCurrentOffset * targetTotalFrames) / iPaletteMoveOffsetTotalFrames;
        }
    else
        {
        iPaletteMoveCurrentOffset = 0;
        }

    iPaletteMoveOffsetTotalFrames = targetTotalFrames;
    
    // double check we're in range
    if ( iPaletteMoveCurrentOffset < 0 )
        {
        iPaletteMoveCurrentOffset = 0;
        }

    if ( iPaletteMoveCurrentOffset > iPaletteMoveOffsetTotalFrames )
        {
        iPaletteMoveCurrentOffset = iPaletteMoveOffsetTotalFrames;
        }
        
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::GetItemList()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::GetItemList(RArray<TActivePalette2ItemVisible>& aItemVisibleList) const
    {
    aItemVisibleList.Reset();

    TInt currentItem = 0;
    TInt totalItems = iItemsArray.Count();
    
    while ( currentItem < totalItems )
        {
        aItemVisibleList.Append(
            TActivePalette2ItemVisible(
                iItemsArray[currentItem]->ItemId(), 
                iVisibilityArray[currentItem]));
        currentItem++;
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetItemList()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::SetItemList(const RArray<TActivePalette2ItemVisible>& aItemVisibleList)
    {
    TInt res = KErrNone;
    TInt currentItem = 0;
    TInt totalItems = iItemsArray.Count();
    TInt totalInputItems = aItemVisibleList.Count();
    TInt destIndex = 0;

    TInt focusedItemId = 0;
    
    // Backup the current focused item
    if ( ValidItemIndex(iFocusedItem) )
        {
        focusedItemId = iItemsArray[iFocusedItem]->ItemId();
        }

    // Check that there aren't any invalid IDs in the list
    while ( currentItem < totalInputItems && res == KErrNone )
        {
        TInt itemId = aItemVisibleList[currentItem].ItemId();
        TInt foundIndex = ItemIndexFromId(itemId);
        
        if ( foundIndex == KInvalidItemIndex )
            {
            res = KErrNotFound;
            }

        currentItem++;            
        }

    // Rearrange the item & visibility lists
    currentItem = 0;
    while ( currentItem < totalInputItems && destIndex < totalItems && res == KErrNone )
        {
        TInt itemId = aItemVisibleList[currentItem].ItemId();
        TBool visible = aItemVisibleList[currentItem].Visible();
        
        TInt foundIndex = ItemIndexFromId(itemId);
        
        if ( foundIndex >= currentItem )
            {
            iItemsArray.Insert(iItemsArray[foundIndex], destIndex);
            iItemsArray.Remove(foundIndex + 1);

            iVisibilityArray.Insert(visible, destIndex);
            iVisibilityArray.Remove(foundIndex + 1);

            destIndex++;
            }

        currentItem++;            
        }

    // Update the focus
    if ( res == KErrNone )
        {
        iFocusedItem = ItemIndexFromId(focusedItemId);

        EnforceConstraints();
        }

    return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::GetPaletteVisibilityAnimationDuration()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::GetPaletteVisibilityAnimationDuration(TInt& aTimeInMilliseconds) const
    {
    aTimeInMilliseconds = iPaletteMoveAnimationDuration;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetCurrentItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::SetCurrentItem(const TInt aItemId)
    {
    TInt err = KErrNotFound;
	TInt index = ItemIndexFromId(aItemId);
	
    if ( ValidItemIndex(index) )
        {
        err = KErrGeneral;

        if ( iVisibilityArray[index] )
            {
            iFocusedItem = index;
            
            EnforceConstraints();
            
            err = KErrNone;
            }
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::GetCurrentItem()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::GetCurrentItem(TInt& aItemId) const
    {
    TInt err = KErrGeneral;
    
    if ( ValidItemIndex(iFocusedItem) )
        {
        if ( iItemsArray[iFocusedItem] )
            {
            aItemId = iItemsArray[iFocusedItem]->ItemId();
            
            err = KErrNone;
            }
        }
    return err;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemsUpdated()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::ItemsUpdated()
    {
    RemoveTooltip();

    // Update cached data
    TInt onScreenPos = 0;
    TInt onScreenIndex = iTopItemOnScreen;

    while ( onScreenIndex != KInvalidItemIndex && onScreenPos < (NAP2Cfg::KMaxNofItemsInView + 1))
        {
        iOnScreenItemIndex[onScreenPos] = onScreenIndex;
        onScreenIndex = FindVisibleItem(onScreenIndex + 1);
        onScreenPos++;
        }

    while (onScreenPos < (NAP2Cfg::KMaxNofItemsInView + 1))
        {
        iOnScreenItemIndex[onScreenPos] = KInvalidItemIndex;
        onScreenPos++;
        }
        
    iShowTopScrollArrow = iTopItemOnScreen > FindVisibleItem(0);
    iShowBottomScrollArrow = CountVisibleItems(iTopItemOnScreen) > iNoItemsOnScreen;
    
    iModelObserver.ItemsUpdated();
    }



// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemScrollOffset()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemScrollOffset()
    {
    return iItemScrollCurrentOffset;
    }


// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemScrollTotalFrames()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemScrollTotalFrames()
    {
    return NAP2Cfg::KItemScrollFrames;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetAnimState()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::SetAnimState(TPaletteAnimState aNewState)
    {
    if ( iAnimState != aNewState )
        {
        // Exit old state
        switch ( iAnimState )
            {
            case EPaletteAnimItemScroll:
                ItemScrollComplete();
                break;
                
            case EPaletteAnimFocusChange:
                FocusChangeComplete();
                break;
                
            case EPaletteAnimItemAnim:
                ItemAnimationComplete();
                break;
                
            case EPaletteAnimAppearing:
                PaletteAnimComplete();
                break;
                
            case EPaletteAnimIdle:
            default:
                // do nothing;
                break;
            }

        // Enter new state
        switch ( aNewState )
            {
            case EPaletteAnimItemScroll:
                break;
                
            case EPaletteAnimFocusChange:
                break;
                
            case EPaletteAnimItemAnim:
                StartItemAnimation();
                break;
                
            case EPaletteAnimAppearing:
                break;
                
            case EPaletteAnimIdle:
            default:
                // do nothing;
                break;
            }
            
        iAnimState = aNewState;
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::PaletteActive()
// -----------------------------------------------------------------------------
//
TBool CActivePalette2Model::PaletteActive()
    {
    return ( iAnimState != EPaletteAnimAppearing && iPaletteMoveCurrentOffset == iPaletteMoveOffsetTotalFrames);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ValidItemIndex()
// -----------------------------------------------------------------------------
//
TBool CActivePalette2Model::ValidItemIndex(TInt aItemIndex) const
    {
    return (aItemIndex >= 0 && aItemIndex < iItemsArray.Count());
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::SendMessage()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::SendMessage(TInt aItemId, TInt aMessageId, const TDesC8& aDataDes)
    {
    return DispatchMessage(aItemId,aMessageId,aDataDes,KActivePaletteNoDataInt);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::SendMessage()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::SendMessage(TInt aItemId, TInt aMessageId, TInt aDataInt)
    {
    return DispatchMessage(aItemId,aMessageId,KNullDesC8,aDataInt);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::InstallItemL(const TActivePalette2ItemVisible& aItemVisible, 
					const TUid& aPluginUid, 
					const TDesC8& aCustomDataDes)
    {
    return DoInstallItemL(aItemVisible.ItemId(),
	                      aItemVisible.Visible(),
	                      aPluginUid,aCustomDataDes,
	                      KActivePaletteNoDataInt);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
					const TUid& aPluginUid, 
					TInt aCustomDataInt)
	{
	return DoInstallItemL(aItemVisible.ItemId(),
	                      aItemVisible.Visible(),
	                      aPluginUid,
	                      KNullDesC8,
	                      aCustomDataInt);
	}
	
// -----------------------------------------------------------------------------
// CActivePalette2Model::InstallItemL()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::InstallItemL(const TActivePalette2ItemVisible& aItemVisible,
					const TUid& aPluginUid, 
					TInt aCustomDataInt, 
					const TDesC8& aCustomDataDes)
    {
    return DoInstallItemL(aItemVisible.ItemId(),
	                      aItemVisible.Visible(),
	                      aPluginUid,
	                      aCustomDataDes,
	                      aCustomDataInt);
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemSize()
// -----------------------------------------------------------------------------
//
TSize CActivePalette2Model::ItemSize()
    {
    return iItemSize;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::LocateTo()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::LocateTo(const TPoint& /*aTopLeft*/)
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::Location()
// -----------------------------------------------------------------------------
//
TPoint CActivePalette2Model::Location() const
    {
    // unused
    return TPoint(0,0);
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::CoeControl()
// -----------------------------------------------------------------------------
//
CCoeControl* CActivePalette2Model::CoeControl()
    {
    // unused
    return NULL;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::HuiControl()
// -----------------------------------------------------------------------------
//
CHuiControl* CActivePalette2Model::HuiControl()
    {
    // unused
    return NULL;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetGc()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::SetGc(CBitmapContext* /*aGc*/)
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::RenderActivePalette()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::RenderActivePalette(const TRect& /*aRect*/) const
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetObserver()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::SetObserver(MActivePalette2Observer* /*aObserver*/)
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::SetNavigationKeys()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::SetNavigationKeys(const TActivePalette2NavigationKeys& /*aNavigationKeys*/)
    {
    // unused
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::AnimateAppearanceRendererComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::AnimateAppearanceRendererComplete()
    {
    if ( iAnimState == EPaletteAnimAppearing )
        {
        SetAnimState(EPaletteAnimIdle);
        iModelObserver.PaletteAppearingUpdated();
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::AnimateItemScrollRendererComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::AnimateItemScrollRendererComplete()
    {
    if ( iAnimState == EPaletteAnimItemScroll )
        {
        SetAnimState(EPaletteAnimItemAnim);
        }
    }
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::SetFocusedItem()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::FocusedItemUpdated()
    {
    if ( ValidItemIndex(iFocusedItem) )
        {
        TInt currentID = iItemsArray[iFocusedItem]->ItemId();

        iPreviousItemID = iFocusedItemID;
        iFocusedItemID = currentID;
        
        if ( iModelObserver.APObserver() )
            {
            iModelObserver.APObserver()->NotifyItemFocused(iPreviousItemID, iFocusedItemID);
            }

        if ( iCallbacks.iFocusTimer )
            {
            iCallbacks.iFocusTimer->FocusUpdated();
            }
        }
    }
    







// -----------------------------------------------------------------------------
// CActivePalette2Model::StartItemAnimation()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::StartItemAnimation()
    {
    CompelTooltip();

    CActivePalettePluginBase::TPluginAnimationInfo animConfig;
    
    animConfig.iFrameSize = ActivePalette2Utils::APDimensionSize( ActivePalette2Utils::EItemSize );
	animConfig.iNofFrames = NAP2Cfg::KItemAnimNoFrames;
	animConfig.iFrameTimeGapMs = NAP2Cfg::KItemAnimFrameDuration;

	iAniItem = iItemsArray[iFocusedItem];
    iAniItemScreenPos = ScreenPosFromItemIndex(iFocusedItem);

    iAnimationFrame = 0;
	
	TRAP_IGNORE(iAniItem->Plugin().PrepareAniFramesL(
		CActivePalettePluginBase::EAniFocused,
		iAniItemInfo,
		animConfig,
		iAniItem->Icon(),
		iAniItem->Mask()));
		
	iItemAnimTimer->Start(iAniItemInfo.iFrameTimeGapMs, iAniItemInfo.iFrameTimeGapMs, 
			  TCallBack(&CActivePalette2Model::ItemAnimTimerCallback,(TAny*)this));

    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemAnimTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemAnimTimerCallback(TAny* aPtr)
	{
	CActivePalette2Model* self = (CActivePalette2Model*) aPtr;

    if ( self )
        {
        return self->ItemAnimAction();
        }
    else
        {
    	return KCallbackFinished;
    	}
	}

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemAnimAction()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemAnimAction()
    {
    TInt res = KCallbackCallAgain;

    if ( iAnimationFrame > iAniItemInfo.iNofFrames)
        {
        res = KCallbackFinished;
        SetAnimState(EPaletteAnimIdle);
        }
    else
        {
        iModelObserver.ItemAnimated(iAniItemScreenPos, iAniItem, iAnimationFrame);
        iAnimationFrame++;
        }
    
    return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemAnimationComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::ItemAnimationComplete()
    {
    iItemAnimTimer->Cancel();
    iModelObserver.ItemAnimationComplete(iAniItemScreenPos, iAniItem);
    iAniItem = NULL;   
    iAniItemScreenPos = KInvalidScreenPos;
    }






// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemScrollTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemScrollTimerCallback(TAny* aPtr)
	{
	CActivePalette2Model* self = (CActivePalette2Model*) aPtr;
	
	if ( self )
	    {
        return self->ItemScrollAction();
        }
    else
        {
    	return KCallbackFinished;
    	}
	}

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemScrollAction()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::ItemScrollAction()
    {
    TInt res = KCallbackCallAgain;
    
    iItemScrollCurrentOffset += iItemScrollDirection;
    
    if ( iItemScrollCurrentOffset <= 0 || iItemScrollCurrentOffset >= NAP2Cfg::KItemScrollFrames )
        {
        res = KCallbackFinished;
        SetAnimState(EPaletteAnimItemAnim);
        }
    else
        {
        if ( iCallbacks.iItemScrollTimer )
            {
            iCallbacks.iItemScrollTimer->ItemsScrolled();
            }
        }
            
    return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::ItemScrollComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::ItemScrollComplete()
    {
    iItemScrollTimer->Cancel();

    iItemScrollCurrentOffset = 0;
    
    if ( iFocusedItem != iFocusChangeTargetItem )
        {
        iFocusedItem = iFocusChangeTargetItem;
        iTopItemOnScreen = FindVisibleItem(iTopItemOnScreen + 1);
        ItemsUpdated();
        }
    else
        {
        if ( iCallbacks.iItemScrollTimer )
            {
            iCallbacks.iItemScrollTimer->ItemsScrolled();
            }
        }        
    }


    
    
    
// -----------------------------------------------------------------------------
// CActivePalette2Model::PaletteAnimTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::PaletteAnimTimerCallback(TAny* aPtr)
    {
	CActivePalette2Model* self = (CActivePalette2Model*) aPtr;
	
	if ( self )
	    {
	    return self->PaletteAnimAction();
	    }
	else
	    {
    	return KCallbackFinished;
    	}
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::PaletteAnimAction()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::PaletteAnimAction()
    {
    TInt res = KCallbackCallAgain;

    iPaletteMoveCurrentOffset += iPaletteMoveAnimationDirection;

    if ( (iPaletteMoveCurrentOffset <= 0) || (iPaletteMoveCurrentOffset >= iPaletteMoveOffsetTotalFrames))
        {
        res = KCallbackFinished;
        SetAnimState(EPaletteAnimIdle);
        iModelObserver.PaletteAppearingUpdated();
        }
    else
        {
        iModelObserver.PaletteAppearingUpdated();
        
        if ( iCallbacks.iAppearanceDescriptive )
            {
            iPaletteMoveTimer->Cancel();

            if ( iPaletteMoveAnimationDirection > 0 )
                {
                // Palette is appearing
                iCallbacks.iAppearanceDescriptive->AnimateAppearance(ETrue, 
                    (iPaletteMoveAnimationDuration * (iPaletteMoveOffsetTotalFrames - iPaletteMoveCurrentOffset)) 
                    / iPaletteMoveOffsetTotalFrames);
                }
            else
                {
                // Palette is disappearing
                iCallbacks.iAppearanceDescriptive->AnimateAppearance(EFalse, 
                    (iPaletteMoveAnimationDuration * (iPaletteMoveCurrentOffset)) 
                    / iPaletteMoveOffsetTotalFrames);
                }
                
            res = KCallbackFinished;
            }
        }
        
    return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::PaletteAnimComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::PaletteAnimComplete()
    {
    iPaletteMoveTimer->Cancel();
    
    if ( iPaletteMoveAnimationDirection > 0 )
        {
        iPaletteMoveCurrentOffset = iPaletteMoveOffsetTotalFrames;
        }
    else
        {
        iPaletteMoveCurrentOffset = 0;
        }    
    }
    







// -----------------------------------------------------------------------------
// CActivePalette2Model::AnimateFocusRendererComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::AnimateFocusRendererComplete()
    {
    // Check we're expecting call
    if ( iAnimState == EPaletteAnimFocusChange )
        {
        SetAnimState(EPaletteAnimItemAnim);
        }
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::FocusChangeTimerCallback()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::FocusChangeTimerCallback(TAny* aPtr)
	{
	CActivePalette2Model* self = (CActivePalette2Model*) aPtr;
	
	if ( self )
    	{
        return self->FocusChangeAction();
        }
    else
        {
        return KCallbackFinished;
        }
	}

// -----------------------------------------------------------------------------
// CActivePalette2Model::FocusChangeAction()
// -----------------------------------------------------------------------------
//
TInt CActivePalette2Model::FocusChangeAction()
    {
    TInt res = KCallbackCallAgain;
    
    iFocusedCurrentOffset += iFocusChangeDirection;
    
    if ( iFocusedCurrentOffset <= 0 || iFocusedCurrentOffset >= NAP2Cfg::KFocusChangeFrames )
        {
        res = KCallbackFinished;
        SetAnimState(EPaletteAnimItemAnim);
        }
    else
        {
        if ( iCallbacks.iFocusTimer )
            {
            iCallbacks.iFocusTimer->FocusAnimated();
            }
        }
    
    return res;
    }

// -----------------------------------------------------------------------------
// CActivePalette2Model::FocusChangeComplete()
// -----------------------------------------------------------------------------
//
void CActivePalette2Model::FocusChangeComplete()
    {
    iFocusChangeTimer->Cancel();
    
    iFocusedItem = iFocusChangeTargetItem;
    iFocusedCurrentOffset = 0;
    
    FocusedItemUpdated();
    }


// End of File  
