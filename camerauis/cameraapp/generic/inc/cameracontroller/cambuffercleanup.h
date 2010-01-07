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

#ifndef CAM_CAMERABUFFERCLEANUP_H
#define CAM_CAMERABUFFERCLEANUP_H

#include <ecam.h> // MCameraBuffer

void 
CameraBufferCleanup( TAny* aCameraBuffer )
  {
  MCameraBuffer* buffer = static_cast<MCameraBuffer*>( aCameraBuffer );
  buffer->Release();
  }

#endif
