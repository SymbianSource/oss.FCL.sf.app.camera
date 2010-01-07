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
* Description:
*
*/



// INCLUDE FILES
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamIndicatorResourceReader.h"
#include "CamIndicatorData.h"
#include "camlogging.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamIndicatorResourceReader::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamIndicatorResourceReader* CCamIndicatorResourceReader::NewLC( TResourceReader& aReader )
    {
    CCamIndicatorResourceReader* self = new ( ELeave) CCamIndicatorResourceReader();
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    return self;
    }

// Destructor
CCamIndicatorResourceReader::~CCamIndicatorResourceReader()
  {
  PRINT( _L("Camera => ~CCamIndicatorResourceReader") );
  if ( iIndicatorArray )
    {
    iIndicatorArray->ResetAndDestroy();
    delete iIndicatorArray;
    }
  PRINT( _L("Camera <= ~CCamIndicatorResourceReader") );
  }

// ---------------------------------------------------------
// CCamIndicatorResourceReader::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamIndicatorResourceReader::ConstructL( TResourceReader& aReader )
    {
    iIndicatorArray = new ( ELeave ) CArrayPtrFlat<CCamIndicatorData> ( 3 );

    TInt ii;
    TInt numindicators = aReader.ReadInt16();
    for ( ii = 0; ii < numindicators; ii++ )
        {
        CCamIndicatorData* data = CCamIndicatorData::NewLC( aReader );
        iIndicatorArray->AppendL( data );
        CleanupStack::Pop( data );
        }
    }

// ---------------------------------------------------------------------------
// CCamIndicatorResourceReader::CCamIndicatorResourceReader
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamIndicatorResourceReader::CCamIndicatorResourceReader()
    {
    }

// ---------------------------------------------------------------------------
// CCamIndicatorResourceReader::IndicatorData
// return indicator data
// ---------------------------------------------------------------------------
//
CArrayPtrFlat<CCamIndicatorData>& CCamIndicatorResourceReader::IndicatorData()
    {
    return *iIndicatorArray;
    }

// End of File  
