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
* Description:  Inline utility methods for flag variables
*
*/


#ifndef CAM_FLAGUTILITY_INL
#define CAM_FLAGUTILITY_INL


// ===========================================================================
namespace NCamCameraController
  {
  inline TBool IsFlagOn( TUint aState, TUint aFlag )
    {
    return (aState & aFlag);
    };

  inline TBool IsFlagOff( TUint aState, TUint aFlag )
    {
    return !IsFlagOn( aState, aFlag );
    };

  inline void CheckFlagOnL( TUint aState, TUint aFlag, TInt aLeave )
    {
    if( IsFlagOff( aState, aFlag ) )
      {
      User::Leave( aLeave );
      }
    }
  inline void CheckFlagOffL( TUint aState, TUint aFlag, TInt aLeave )
    {
    if( IsFlagOn( aState , aFlag ) )
      {
      User::Leave( aLeave );
      }
    }

  inline void CheckEqualsL( TUint aState, TUint aCheck, TInt aLeave )
    {
    if( aState != aCheck )
      {
      User::Leave( aLeave );
      }
    }
  
  // SetFlags and ClearFlags defined to aid readability.
  // "ClearFlags( flags, someflags )" should be more readable than "flags &= ~someFlags".
  // Also using these methods is less error prone. 
  // Consider "flags &= ~someFlags" changed to "flags &= someFlags" by mistake..
  inline void SetFlags( TUint& aOldFlags, TUint aSetFlags )
    {
    aOldFlags |= aSetFlags;
    }
  
  inline void ClearFlags( TUint& aOldFlags, TUint aClearFlags )
    {
    aOldFlags &= ~aClearFlags;
    }
  }

// ===========================================================================
#endif // CAM_FLAGUTILITY_INL

// end of file
