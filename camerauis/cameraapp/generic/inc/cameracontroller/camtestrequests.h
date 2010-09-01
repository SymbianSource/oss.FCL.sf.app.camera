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



#ifndef CAM_TEST_REQUESTS_H
#define CAM_TEST_REQUESTS_H

#include <ecam.h>
#include "camcamerarequests.h"


static const TCamCameraRequestId KCamCameraStartupSequencePreVf[] =
  {
  ECamRequestReserve,
  ECamRequestPowerOn,

  ECamRequestImageInit,

  ECamRequestVfInitBitmap,
  ECamRequestVfStart,

  ECamRequestSsInit,
  ECamRequestSsStart
  };

static const TCamCameraRequestId KCamCameraStartupSequenceVfPre[] =
  {
  ECamRequestReserve,
  ECamRequestPowerOn,

  ECamRequestVfInitBitmap,
  ECamRequestVfStart,

  ECamRequestImageInit,

  ECamRequestSsInit,
  ECamRequestSsStart
  };


#endif