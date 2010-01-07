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
* Description:  Implements the array of items for the user scene setup list.*
*/


// INCLUDE FILES
#include "CamUserSceneSetupItemArray.h"
#include <aknlists.h>

// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CCamUserSceneSetupItemArray::CCamUserSceneSetupItemArray
// C++ Constructor
// ---------------------------------------------------------
//
CCamUserSceneSetupItemArray::CCamUserSceneSetupItemArray( TInt aGranularity )
: CArrayPtrFlat<CCamUserSceneSetupListItem>( aGranularity )
    {
    }

// ---------------------------------------------------------
// CCamUserSceneSetupItemArray::ConstructL
// 2nd phase Constructor
// ---------------------------------------------------------
//
void CCamUserSceneSetupItemArray::ConstructL()
    {
    }

// ---------------------------------------------------------
// CCamUserSceneSetupItemArray::~CCamUserSceneSetupItemArray
// Destructor
// ---------------------------------------------------------
//
CCamUserSceneSetupItemArray::~CCamUserSceneSetupItemArray()
  {
  ResetAndDestroy();
  }


// ---------------------------------------------------------
// CCamUserSceneSetupItemArray::MdcaCount
// Returns the number of items in the array
// ---------------------------------------------------------
//
TInt CCamUserSceneSetupItemArray::MdcaCount() const
    {
    return Count();
    }

// ---------------------------------------------------------
// CCamUserSceneSetupItemArray::MdcaPoint
// Returns the text for the aIndex-th item in the array.
// ---------------------------------------------------------
//
TPtrC16 CCamUserSceneSetupItemArray::MdcaPoint( TInt aIndex ) const
    {
    return At( aIndex )->ListBoxText();
	}


// End of File  
