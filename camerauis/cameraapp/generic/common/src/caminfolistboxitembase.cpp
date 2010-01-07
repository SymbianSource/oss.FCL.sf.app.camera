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
* Description:  Implementations of Info Listbox Item Base class methods.
*
*/



// ===========================================================================
// Includes
#include <fbs.h>

#include "camlogging.h"
#include "caminfolistboxitembase.h"


// ===========================================================================
// Class methods

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItem destructor
// 
// ---------------------------------------------------------------------------
//
CCamInfoListboxItemBase::~CCamInfoListboxItemBase()
  {
  PRINT( _L("Camera => ~CCamInfoListboxItemBase") );
  delete iListItemText; iListItemText = NULL;
  delete iBitmap;       iBitmap       = NULL;
  delete iBitmapMask;   iBitmapMask   = NULL;
  PRINT( _L("Camera <= ~CCamInfoListboxItemBase") );
  }

// ===========================================================================
// from MCamInfoListboxItem

// ---------------------------------------------------------------------------
// CCamInfoListboxItemBase::ItemText
// Return a pointer to the item text
// ---------------------------------------------------------------------------
//
TPtrC CCamInfoListboxItemBase::ItemText() const
  {
  return *iListItemText;
  }

// ---------------------------------------------------------------------------
// CCamInfoListboxItemBase::ItemValue
// Return the id of the setting value represented by this item
// ---------------------------------------------------------------------------
//
TInt CCamInfoListboxItemBase::ItemValue() const
  {
  return iSettingItemValueId;
  }

// ---------------------------------------------------------------------------
// CCamInfoListboxItemBase::Bitmap
// Return a pointer to the bitmap
// ---------------------------------------------------------------------------
//
CFbsBitmap* CCamInfoListboxItemBase::Bitmap() const
  {
  return iBitmap;
  }

// ---------------------------------------------------------------------------
// CCamInfoListboxItemBase::BitmapMask
// Return a pointer to the bitmap mask
// ---------------------------------------------------------------------------
//
CFbsBitmap* CCamInfoListboxItemBase::BitmapMask() const
  {
  return iBitmapMask;
  }


// ===========================================================================
// new methods

// ---------------------------------------------------------------------------
// <<static>>
//
// ---------------------------------------------------------------------------
//
TBool 
CCamInfoListboxItemBase::EqualIds( const TInt* aSettingValueId, 
                                   const CCamInfoListboxItemBase& aOther )
  {
  PRINT1( _L("Camera => CCamInfoListboxItemBase::EqualIds, pointer:%d"), aSettingValueId );
  TBool match( EFalse );
  if( aSettingValueId )
    {
    PRINT2( _L("Camera <> CCamInfoListboxItemBase: %d =?= %d"), *aSettingValueId, aOther.ItemValue() );
    match = aOther.ItemValue() == *aSettingValueId;
    }
  PRINT1( _L("Camera <= CCamInfoListboxItemBase::EqualIds, return:%d"), match );
  return match;
  }


// ===========================================================================
// end of file
