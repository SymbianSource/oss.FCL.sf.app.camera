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
* Description:  Reference counting wrapper for MCameraBuffer.
*
*/


#ifndef CAM_BUFFERSHARE_H
#define CAM_BUFFERSHARE_H

#include <e32base.h>


class MCameraBuffer;

class CCamBufferShare : public CBase
  {
  // =======================================================
  // Methods

  // -------------------------------------------------------
  // Constructor and destructor
  public:

    /**
    * This is just a wrapper class => c++ constructor
    */
    CCamBufferShare( MCameraBuffer* aSharedBuffer );

  private:    

    /**
    * Reference counting used, so destructor is private.
    */
    ~CCamBufferShare();
    
  // -------------------------------------------------------
  // Other methods
  public:

    /**
    * Get the shared buffer. Ownership not given to caller.
    */
    MCameraBuffer* SharedBuffer();

    /**
    * Reserve a reference to this share.
    */  
    void Reserve();

    /**
    * Release a reference to this share.
    */
    void Release();

#ifdef _DEBUG
    /**
    * Get the amount of used buffers in total.
    */
    static TInt TotalBufferShareCount();
#endif
 
  // =======================================================
  // Data 
  protected:

#ifdef _DEBUG
    static TInt    isTotalBufferCount;
#endif

  private:

    MCameraBuffer* iSharedBuffer;
    TInt           iReferenceCount;
    
  // =======================================================
  };

#endif

// end of file

