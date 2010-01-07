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
* Description:  Camera Indicator control*
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
// CCamIndicatorData::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CCamIndicatorData* CCamIndicatorData::NewLC( TResourceReader& aReader )
    {
    CCamIndicatorData* self = new( ELeave ) CCamIndicatorData();
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    return self;
    }

// Destructor
CCamIndicatorData::~CCamIndicatorData()
  {
  PRINT( _L("Camera => ~CCamIndicatorData") );
  iBitmapIdArray.Reset();
  iBitmapIdArray.Close();
  PRINT( _L("Camera <= ~CCamIndicatorData") );
  }

// ---------------------------------------------------------
// CCamIndicatorData::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamIndicatorData::ConstructL( TResourceReader& aReader )
    {
    // first integer (TInt16) represent indicator id
    iIndicatorId = aReader.ReadInt16();

    // next 4 integers (TInt16) represent TRect cordinates
    iRect.iTl.iX = aReader.ReadInt16();
    iRect.iTl.iY = aReader.ReadInt16();
    iRect.iBr.iX = aReader.ReadInt16();
    iRect.iBr.iY = aReader.ReadInt16();

    // next integer (TInt16) represents the number of bitmaps
    iIndicatorBitmapCount = aReader.ReadInt16();

    // bitmap ids are TInt32
    TInt32 bmpid;
	TInt ii;
    for ( ii = 0; ii < iIndicatorBitmapCount; ii++ )
        {
        bmpid=aReader.ReadInt32();
        User::LeaveIfError( iBitmapIdArray.Append( bmpid ) );
        }
    }

// ---------------------------------------------------------------------------
// CCamIndicatorData::CCamIndicatorData
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamIndicatorData::CCamIndicatorData()
    {
    }

// ---------------------------------------------------------------------------
// CCamIndicatorData::IndicatorId
// returns the camera indicator identifier
// ---------------------------------------------------------------------------
//
TInt CCamIndicatorData::IndicatorId() const
    {
    return iIndicatorId;
    }

// ---------------------------------------------------------------------------
// CCamIndicatorData::IndicatorRect
// returns the indicator rectangle cooords
// ---------------------------------------------------------------------------
//
TRect CCamIndicatorData::IndicatorRect() const
    {
    return iRect;
    }

// ---------------------------------------------------------------------------
// CCamIndicatorData::IndicatorBitmapCount
// returns the number of available bitmaps for the indicator
// ---------------------------------------------------------------------------
//
TInt CCamIndicatorData::IndicatorBitmapCount() const
    {
    return iIndicatorBitmapCount;
    }

// ---------------------------------------------------------------------------
// CCamIndicatorData::IndicatorBitmapId
// returns the bitmap id for the specified index
// ---------------------------------------------------------------------------
//
TInt32 CCamIndicatorData::IndicatorBitmapId( TInt aIndex ) const
    {
    return iBitmapIdArray[ aIndex ];
    }

// End of File
