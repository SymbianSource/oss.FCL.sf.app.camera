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




#ifndef CAM_CAMERAREQUESTS_H
#define CAM_CAMERAREQUESTS_H

// ===========================================================================


/**
* Request identification codes
*/
enum TCamCameraRequestId
  {
  ECamRequestNone    = 0,

  // If client has issued a sequence of requests to be processed
  // by the controller, client must request cancelling the sequence,
  // if it wishes to issue other requests before ECamEngineEventSequenceEnd
  // has been notified by controller.
  ECamRequestCancelSequence,

  ECamRequestReserve,
  ECamRequestRelease,

  ECamRequestPowerOn,
  ECamRequestPowerOff,

  ECamRequestVfStart,
  ECamRequestVfStop,
  ECamRequestVfStopEcam,
  ECamRequestVfRelease,

  ECamRequestSsStart,
  ECamRequestSsStop,
  ECamRequestSsRelease,

  ECamRequestImageInit,
  ECamRequestImageCapture, 
  ECamRequestImageCancel,  // Cancel capturing image.
  ECamRequestImageRelease,

  ECamRequestVideoInit,
  ECamRequestVideoStart,
  ECamRequestVideoPause,
  ECamRequestVideoStop,
  ECamRequestVideoRelease,
  ECamRequestSetAsyncVideoStopMode, 
  
  ECamRequestStartAutofocus,
  ECamRequestCancelAutofocus,
  ECamRequestSetAfRange, 

//  ECamRequestSettingChange,

  ECamRequestLast // Just marker, keep last.
  };


#ifdef _DEBUG
static const TUint16* KCamRequestNames[] =
  {
  // ECamRequestNone used in performance measurements to mark sequence start and end.
  (const TUint16*)_S16("Sequence"), 

  (const TUint16*)_S16("Cancel sequence"),
  
  (const TUint16*)_S16("Reserve"),
  (const TUint16*)_S16("Release"),
  (const TUint16*)_S16("Power on"),
  (const TUint16*)_S16("Power off"),
  
  (const TUint16*)_S16("Start vf"),
  (const TUint16*)_S16("Stop vf"),
  (const TUint16*)_S16("Stop vf ecam"),
  (const TUint16*)_S16("Release vf"),

  (const TUint16*)_S16("Start snapshot"),
  (const TUint16*)_S16("Stop  snapshot"),
  (const TUint16*)_S16("Release snapshot"),

  (const TUint16*)_S16("Init image"),
  (const TUint16*)_S16("Capture image"),
  (const TUint16*)_S16("Cancel image"),
  (const TUint16*)_S16("Release image"),

  (const TUint16*)_S16("Init video"),
  (const TUint16*)_S16("Start video"),
  (const TUint16*)_S16("Pause video"),
  (const TUint16*)_S16("Stop video"),
  (const TUint16*)_S16("Release video"),
  (const TUint16*)_S16("Async stop video"), 
  
  (const TUint16*)_S16("Start autofocus"),
  (const TUint16*)_S16("Cancel autofocus"),
  (const TUint16*)_S16("Set Af range")
  
  
//  ,  (const TUint16*)_S16("Setting change")
  };

// Verifying there is a description for all the request ids
__ASSERT_COMPILE( (sizeof(KCamRequestNames) / sizeof(TUint16*)) == ECamRequestLast );

#endif // _DEBUG
  

// ===========================================================================
#endif // CAM_CAMERAREQUESTS_H

// end of file
