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
* Description:  Inline implementations of CCamSnapshotProvider class
*
*/


#include "camsnapshot.h"

// ===========================================================================
// Methods

// ===========================================================================
// From MCameraSnapshot

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TUint32
CCamSnapshotProvider::SupportedFormats()
  {
  return ( iSs1
         ? iSs1->SupportedFormats()
         : iSs2->SupportedFormats() );
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSnapshotProvider::PrepareSnapshotL(       CCamera::TFormat aFormat, 
                                        const TPoint&          aPosition, 
                                        const TSize&           aSize, 
                                        const TRgb&            aBgColor, 
                                              TBool            aMaintainAspectRatio )
  {
  ( iSs1 ? iSs1->PrepareSnapshotL( aFormat, aPosition, aSize, aBgColor, aMaintainAspectRatio )
         : iSs2->PrepareSnapshotL( aFormat, aPosition, aSize, aBgColor, aMaintainAspectRatio ) );
  }
                                        

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSnapshotProvider::PrepareSnapshotL(       CCamera::TFormat aFormat, 
                                        const TSize&           aSize, 
                                              TBool            aMaintainAspectRatio )
  {
  ( iSs1 ? iSs1->PrepareSnapshotL( aFormat, aSize, aMaintainAspectRatio )
         : iSs2->PrepareSnapshotL( aFormat, aSize, aMaintainAspectRatio ) );
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSnapshotProvider::SetBgColorL( const TRgb& aBgColor )
  {
  ( iSs1 ? iSs1->SetBgColorL( aBgColor )
         : iSs2->SetBgColorL( aBgColor ) );
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSnapshotProvider::SetPositionL( const TPoint& aPosition )
  {
  ( iSs1 ? iSs1->SetPositionL( aPosition )
         : iSs2->SetPositionL( aPosition ) );
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool
CCamSnapshotProvider::IsSnapshotActive() const
  {
  return ( iSs1
         ? iSs1->IsSnapshotActive()
         : iSs2->IsSnapshotActive() );  
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSnapshotProvider::StartSnapshot()
  {
  ( iSs1 ? iSs1->StartSnapshot()
         : iSs2->StartSnapshot() );
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSnapshotProvider::StopSnapshot()
  {
  ( iSs1 ? iSs1->StopSnapshot()
         : iSs2->StopSnapshot() );
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MCameraBuffer& 
CCamSnapshotProvider::SnapshotDataL( RArray<TInt>& aFrameIndexOrder )
  {
  return ( iSs1
         ? iSs1->SnapshotDataL( aFrameIndexOrder )
         : iSs2->SnapshotDataL( aFrameIndexOrder ) );  
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void
CCamSnapshotProvider::Release()
  {
  // empty
  }

// ===========================================================================
// end of file .inl