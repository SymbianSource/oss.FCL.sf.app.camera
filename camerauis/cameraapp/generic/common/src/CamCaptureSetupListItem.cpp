/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A list box item with text and a bitmap
*/


// INCLUDE FILES
#include "CamCaptureSetupListItem.h"
#include "CamUtility.h"
#include "camlogging.h"
#include <eikappui.h>	// For CCoeAppUiBase
#include <eikapp.h>		// For CEikApplication
#include <eikenv.h>
#include <barsread.h>
#include <cameraapp.mbg>
#include <AknIconUtils.h>
// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCamCaptureSetupListItem::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamCaptureSetupListItem* 
CCamCaptureSetupListItem::NewLC( TResourceReader&                aReader, 
                                 TAknLayoutRect& aIconLayout )
  {
  CCamCaptureSetupListItem* self = 
      new( ELeave ) CCamCaptureSetupListItem( aIconLayout );
  CleanupStack::PushL( self );
  self->ConstructFromResourceL( aReader );
  return self;
  }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CCamCaptureSetupListItem* 
CCamCaptureSetupListItem::NewLC( 
                  const TDesC&                         aItemText,
                        TInt                           aSettingId,
                        TInt                           aBitmapId,
                        TAknLayoutRect aIconLayout )
  {
  CCamCaptureSetupListItem* self = 
      new( ELeave ) CCamCaptureSetupListItem( aIconLayout );
  CleanupStack::PushL( self );
  self->ConstructFromValuesL( aItemText, aSettingId, aBitmapId );
  return self;
  }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItem::CCamCaptureSetupListItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamCaptureSetupListItem::CCamCaptureSetupListItem( TAknLayoutRect& aLayoutRect )
: iIconLayout( aLayoutRect )
    {
    }

// ---------------------------------------------------------------------------
// CCamCaptureSetupListItem::ConstructFromResourceL
// Extract the data from aReader to populate data members
// ---------------------------------------------------------------------------
//
void 
CCamCaptureSetupListItem::ConstructFromResourceL( TResourceReader& aReader )
  {
  // iListItemText = aReader.ReadTPtrC();
  iListItemText       = aReader.ReadTPtrC().AllocL();
  iSettingItemValueId = aReader.ReadInt16();
  iBitmapId           = aReader.ReadInt32();

  ConstructL();
  }


// ---------------------------------------------------------------------------
// Construct the item from given data
// ---------------------------------------------------------------------------
//
void 
CCamCaptureSetupListItem::ConstructFromValuesL( const TDesC& aItemText,
                                                      TInt   aSettingId,
                                                      TInt   aBitmapId )
  {
  iListItemText       = aItemText.AllocL();
  iSettingItemValueId = aSettingId;
  iBitmapId           = aBitmapId;

  ConstructL();
  }
// ---------------------------------------------------------------------------
// CCamCaptureSetupListItem::ConstructL
// Second phase construction
// ---------------------------------------------------------------------------
//
void CCamCaptureSetupListItem::ConstructL()
  {
  if ( iBitmapId != KErrNotFound )
    {
    TFileName resFileName;
    CamUtility::ResourceFileName( resFileName );
    TPtrC resname = resFileName;
  
    AknIconUtils::CreateIconL( iBitmap, iBitmapMask, resname, iBitmapId, iBitmapId );
    AknIconUtils::SetSize( iBitmap, TSize( iIconLayout.Rect().Width(), iIconLayout.Rect().Height() ) );
    }
  }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamCaptureSetupListItem::~CCamCaptureSetupListItem()
  {
  }

//  End of File 
