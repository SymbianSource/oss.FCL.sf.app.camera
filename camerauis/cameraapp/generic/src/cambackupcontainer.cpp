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
* Description:  Base class for all camera application containers*
*/

#include "cambackupcontainer.h"

// -----------------------------------------------------------------------------
// CCamBackupContainer::CCamBackupContainer
// -----------------------------------------------------------------------------
//
CCamBackupContainer::CCamBackupContainer( CCamAppController& aController,
                       CAknView& aView)
: CCamContainerBase(aController, aView)
    {
    
    }

// -----------------------------------------------------------------------------
// CCamBackupContainer::ConstructL
// -----------------------------------------------------------------------------
//
void CCamBackupContainer::ConstructL(const CCoeControl& aContainer)
    {
    SetContainerWindowL(aContainer);
    DrawableWindow()->SetOrdinalPosition(-1);
    }

// -----------------------------------------------------------------------------
// CCamBackupContainer::~CCamBackupContainer
// -----------------------------------------------------------------------------
//
CCamBackupContainer::~CCamBackupContainer()
    {
    //Do nothing
    }


// -----------------------------------------------------------------------------
// CCamBackupContainer::NewLC
// -----------------------------------------------------------------------------
//
CCamBackupContainer* CCamBackupContainer::NewLC(CCamAppController& aController,
                            CAknView& aView,
                            const CCoeControl& aContainer)
    {
    CCamBackupContainer* self = new(ELeave) CCamBackupContainer(aController,aView);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer);
    return self;
    }

// -----------------------------------------------------------------------------
// CCamBackupContainer::NewL
// -----------------------------------------------------------------------------
//
CCamBackupContainer* CCamBackupContainer::NewL(CCamAppController& aController,
                            CAknView& aView,
                            const CCoeControl& aContainer)
    {
    CCamBackupContainer* self = CCamBackupContainer::NewLC(aController,aView,aContainer);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCamBackupContainer::Draw
// -----------------------------------------------------------------------------
//
void CCamBackupContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetBrushColor(KRgbBlack);
    gc.Clear();
    }

