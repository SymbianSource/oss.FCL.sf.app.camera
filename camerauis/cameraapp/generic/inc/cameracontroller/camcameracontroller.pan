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
* Description:  Camera controller panics
*
*/


#ifndef CAM_CAMERACONTROLLER_PAN
#define CAM_CAMERACONTROLLER_PAN

namespace NCamCameraController
  {
	_LIT( KCamControllerName, "Camera controller" );

  /** Camera controller panic codes */
  enum TCamCameraControllerPanics 
    {
    ECamNullPointer = 100,
    ECamCameraControllerBusy,
    ECamCameraControllerCorrupt,
    ECamCameraControllerUnknownRequest,
    ECamCameraControllerUnknownEvent,
    ECamCameraControllerUnsupported,
    ECamCameraControllerCaeUnsupported,

    ECamCameraControllerUnrecovableError,

    ECamCameraControllerPanicCount
    };
  
  /** Helper method to raise panic */
  inline void Panic( TCamCameraControllerPanics aReason )
    {
    User::Panic( KCamControllerName, aReason );
    }
  }

#endif // CAM_CAMERACONTROLLER_PAN

// end of file
