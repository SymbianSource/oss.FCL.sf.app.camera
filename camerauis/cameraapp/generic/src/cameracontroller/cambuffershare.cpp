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
* Description:  Implementation of CCamBufferShare class.
*                Wrapper around to store and handle sharing of buffer
*                implementing MCameraBuffer interface. Counts references
*                and frees the stored buffer, when no client uses it.
*                
*
*/



// ===========================================================================
// Includes
#include <ecam.h> // MCameraBuffer
#include "camlogging.h"
#include "cambuffershare.h"

// ===========================================================================
// Static data initialisation
#ifdef _DEBUG
  TInt CCamBufferShare::isTotalBufferCount = 0;
#endif // _DEBUG


// ===========================================================================
// Class methods

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCamBufferShare::CCamBufferShare( MCameraBuffer* aSharedBuffer )
  : iSharedBuffer( aSharedBuffer ),
    iReferenceCount( 0 )
  {
#ifdef _DEBUG
  ++isTotalBufferCount;
#endif // _DEBUG
  }

// ---------------------------------------------------------------------------
// Destructor
//   Private, Release is to be used by clients.
// ---------------------------------------------------------------------------
//
CCamBufferShare::~CCamBufferShare()
  {
  PRINT( _L("Camera => ~CCamBufferShare") );  
  if( iSharedBuffer )
    {
    iSharedBuffer->Release();
    iSharedBuffer = NULL;
    }
#ifdef _DEBUG
  --isTotalBufferCount;
#endif
  PRINT( _L("Camera <= ~CCamBufferShare") );  
  }

// ---------------------------------------------------------------------------
// Return the shared buffer.
// Ownership never transferred to caller.
// ---------------------------------------------------------------------------
//
MCameraBuffer* 
CCamBufferShare::SharedBuffer()
  {
  return iSharedBuffer;
  }
  
// ---------------------------------------------------------------------------
// Signal, that new client uses this buffer.
// ---------------------------------------------------------------------------
//
void 
CCamBufferShare::Reserve()
  {
  ++iReferenceCount;
  }

// ---------------------------------------------------------------------------
// Signal that one client has stopped using this buffer.
// ---------------------------------------------------------------------------
//
void 
CCamBufferShare::Release()
  {
  --iReferenceCount;

  if( iReferenceCount <= 0 )
    {
    delete this;
    }
  }

#ifdef _DEBUG
// ---------------------------------------------------------------------------
// TotalBufferShareCount <<static>>
//
// Debug method to follow the total amount of shared buffers in use.
// ---------------------------------------------------------------------------
//
TInt 
CCamBufferShare::TotalBufferShareCount()
  {
  return isTotalBufferCount;
  }
#endif // _DEBUG


// end of file

