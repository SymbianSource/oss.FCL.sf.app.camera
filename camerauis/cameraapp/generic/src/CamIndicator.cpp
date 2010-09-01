/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Camera Indicator control*
*/



// INCLUDE FILES
#include <aknview.h>
#include "CamIndicator.h"
#include "CamUtility.h"
#include "cambitmapitem.h"
#include <eikenv.h>
#include <eikappui.h>	// For CCoeAppUiBase
#include <eikapp.h>		// For CEikApplication
#include <AknIconUtils.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------------------------
// CCamIndicator::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamIndicator* CCamIndicator::NewL( const TRect& aRect )
    {
    return new ( ELeave ) CCamIndicator( aRect );
    }


// Destructor
CCamIndicator::~CCamIndicator()
  {
  PRINT( _L("Camera => ~CCamIndicator") );
  iIcons.ResetAndDestroy();
  PRINT( _L("Camera <= ~CCamIndicator") );
  }

// ---------------------------------------------------------------------------
// CCamIndicator::CCamIndicator
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamIndicator::CCamIndicator( const TRect& aRect )
: iRect( aRect ),
  iDefaultRect( aRect )
    {
    }

// -----------------------------------------------------------------------------
// CCamIndicator::CCamIndicator
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CCamIndicator::CCamIndicator()
	{
	}

// ---------------------------------------------------------
// CCamIndicator::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamIndicator::Draw( CBitmapContext& aGc ) const
    {
    if ( !iClear )
        {
        iIcons[ iCurrentIcon ]->Draw( aGc );
        }
    }

// ---------------------------------------------------------
// CCamIndicator::AddIconL
// Add a bitmap icon to array of available icons
// ---------------------------------------------------------
//
void CCamIndicator::AddIconL( TInt32 aBitmapId, TInt32 aMaskId )
    {
    CCamBitmapItem* item = CCamBitmapItem::NewL( aBitmapId, aMaskId );
    CleanupStack::PushL( item );
    User::LeaveIfError( iIcons.Append( item ) );
    CleanupStack::Pop( item );
    item = NULL;

    // Unused (used to be mask). Just here to make sure the indexing
    // system remains the same
    item = CCamBitmapItem::NewL( aBitmapId, aMaskId );
    CleanupStack::PushL( item );
    User::LeaveIfError( iIcons.Append( item ) );
    CleanupStack::Pop( item );
    item = NULL;
    }

// ---------------------------------------------------------
// CCamIndicator::UpdateRect
// Update the indicator coords
// ---------------------------------------------------------
//
void CCamIndicator::UpdateRect( TRect& aRect )
    {
    iRect = aRect;
    iDefaultRect = iRect;
    }

// ---------------------------------------------------------
// CCamIndicator::SetIcon
// Set index for current icon (from array of loaded bitmaps)
// ---------------------------------------------------------
//
void CCamIndicator::SetIcon( TInt aIndex )
  {
  // Mask is used from index (aIndex + 1),
  // make sure also mask index is in bounds
  if ( aIndex + 1 < iIcons.Count() )
    {
    iClear = EFalse;
    iCurrentIcon = aIndex;
    }
  else
    {
    iClear = ETrue;
    PRINT1( _L("Camera <<warning>> CCamIndicator::SetIcon index out of bounds:%d"), aIndex );
    }
  }

// ---------------------------------------------------------
// CCamIndicator::ClearIcon
// Request for indicator to clear bitmap icon
// - indicator will clear itself on redraw
// ---------------------------------------------------------
//
void CCamIndicator::ClearIcon()
    {
    iClear = ETrue;
    }

// ---------------------------------------------------------
// CCamIndicator::DisplayIcon
// Request for indicator to draw current bitmap icon
// ---------------------------------------------------------
//
void CCamIndicator::DisplayIcon()
    {
    iClear = EFalse;
    }

// ---------------------------------------------------------
// CCamIndicator::SetFlashing
// set flashing state of indicator
// ---------------------------------------------------------
//
void CCamIndicator::SetFlashing( TBool aFlashing )
    {
    iFlashing = aFlashing;
    }

// ---------------------------------------------------------
// CCamIndicator::IsFlashing
// return flashing state of indicator
// ---------------------------------------------------------
//
TBool CCamIndicator::IsFlashing() const
    {
    return iFlashing;
    }

// ---------------------------------------------------------
// CCamIndicator::OverridePosition
// Allows a new position to be temporarialy specified
// ---------------------------------------------------------
//
void CCamIndicator::OverridePosition( const TPoint& aPos )
    {
    iRect.iTl = aPos;
    }

// ---------------------------------------------------------
// CCamIndicator::CancelOverridePosition
// Cancels the position set by OverridePosition
// ---------------------------------------------------------
//
void CCamIndicator::CancelOverridePosition()
    {
    iRect = iDefaultRect;
    }

// ---------------------------------------------------------
// CCamIndicator::SetRect
//
// ---------------------------------------------------------
//
void CCamIndicator::SetRect( const TRect& aRect )
    {
    iRect = aRect;

    for ( TInt i = 0; i < iIcons.Count(); i++ )
        {
        TRAP_IGNORE( iIcons[i]->SetLayoutL( aRect ) );
        }
    }

// ---------------------------------------------------------
// CCamIndicator::Rect
// Indicator layout rect
// ---------------------------------------------------------
//
TRect CCamIndicator::LayoutRect()  
    {
    return iRect;
    }
// End of File  
