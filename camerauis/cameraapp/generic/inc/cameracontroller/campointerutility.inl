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
* Description:  Inline utility methods for pointer handling
*
*/


#ifndef CAM_POINTERUTILITY_INL
#define CAM_POINTERUTILITY_INL

// ===========================================================================
namespace NCamCameraController
  {
  inline void CheckNonNullL( const TAny* aPtr, TInt aLeave )
    {
    if( !aPtr ) User::Leave( aLeave );
    };
  }
// ===========================================================================
#endif // CAM_POINTERUTILITY_INL

// end of file
