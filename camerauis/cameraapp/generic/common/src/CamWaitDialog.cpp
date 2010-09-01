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
* Description:  Wait dialog that doesn't consume key events*
*/



// INCLUDE FILES
#include "CamWaitDialog.h"
#include "CamUtility.h"


// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCamWaitDialog::CCamWaitDialog
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamWaitDialog::CCamWaitDialog( CEikDialog** aSelfPtr,
                                TBool        aVisibilityDelayOff )
  : CAknWaitDialog( aSelfPtr, aVisibilityDelayOff )
  {
  }

// ---------------------------------------------------------------------------
// CCamWaitDialog::~CCamWaitDialog
// C++ destructor
// ---------------------------------------------------------------------------
//
CCamWaitDialog::~CCamWaitDialog()
  {
  iNonConsumedKeys.Close();
  }

// ---------------------------------------------------------------------------
// CCamWaitDialog::OfferKeyEventL
// Key event handling
// ---------------------------------------------------------------------------
//
TKeyResponse 
CCamWaitDialog::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                      TEventCode aType     )
  {
  PRINT( _L("Camera => CCamWaitDialog::OfferKeyEventL") );
  if ( KErrNotFound != iNonConsumedKeys.Find( aKeyEvent.iScanCode ) )
    {
    PRINT( _L("Camera <= CCamWaitDialog::OfferKeyEventL not consumed") );
	  return EKeyWasNotConsumed;	
	  }
  else
    {
    PRINT( _L("Camera <= CCamWaitDialog::OfferKeyEventL offer to CAknWaitDialog") );
    return CAknWaitDialog::OfferKeyEventL( aKeyEvent, aType );
    }
  }


// ---------------------------------------------------------------------------
// CCamWaitDialog::SetNonConsumedKeysL
// 
// ---------------------------------------------------------------------------
//
void 
CCamWaitDialog::SetNonConsumedKeysL( const RArray<TInt>& aNonConsumedKeys )
  {
  TInt n = aNonConsumedKeys.Count();
  
  iNonConsumedKeys.Reset();
  for( TInt i = 0; i < n; i++ )
    {
    User::LeaveIfError( iNonConsumedKeys.Append( aNonConsumedKeys[i] ) ); 
    }    
  }

