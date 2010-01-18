/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Camera engine controller event ids. 
*                Used in MCamEngineObserver::HandleCameraEventL callbacks.
*
*/



#ifndef CAM_CAMERAEVENTS_H
#define CAM_CAMERAEVENTS_H


/**
* Event ids
*/
enum TCamCameraEventId
  {
  ECamCameraEventNone, // Not an event id, but a marker.

  // -------------------------------------------------------
  // Events of class ECamCameraEventClassBasicControl

  // If controller is given a sequence of requests,
  // this event notifies that the full sequence is completed,
  // and client may issue more requests.
  ECamCameraEventSequenceEnd,

  ECamCameraEventReserveGain,
  ECamCameraEventReserveLose,
  ECamCameraEventReserveRequested,

  ECamCameraEventPowerOn,
  ECamCameraEventPowerOff,      
  ECamCameraEventIveRecover,
  ECamCameraEventPowerOnRequested,
  
  // -------------------------------------------------------
  // Events of class ECamCameraEventClassViewfinder
  ECamCameraEventVfStart,
  ECamCameraEventVfStop,
  ECamCameraEventVfRelease,
  // -------------------------------------------------------
  // Event of class ECamCameraEventClassVfData
  // Associated event data for ECamCameraEventViewfinderFrameReady is the VF frame in CFbsBitmap.
  ECamCameraEventVfFrameReady, 
  // -------------------------------------------------------
  // Events of class ECamCameraEventClassSnapshot
  ECamCameraEventSsStart,
  ECamCameraEventSsStop,
  ECamCameraEventSsRelease,
  // -------------------------------------------------------
  // Event of class ECamCameraEventClassSsData
  // Associated event data for ECamCameraEventSnapshotReady is the snapshot in CFbsBitmap.
  ECamCameraEventSsReady,      
  // -------------------------------------------------------
  // Events of class ECamCameraEventClassStillCapture
  ECamCameraEventImageInit,
  ECamCameraEventImageStart, // Capture started
  ECamCameraEventImageStop,  // Capture stopped (normally or by cancel request)
  ECamCameraEventImageRelease,

  // Associated event data for ECamCameraEventImageData is CCamBufferShare pointer.
  // May be null, if errors occurred.
  ECamCameraEventImageData,

  // -------------------------------------------------------
  // Events of class ECamCameraEventClassVideo
  ECamCameraEventVideoInit,
  ECamCameraEventVideoStart,
  ECamCameraEventVideoPause,
  ECamCameraEventVideoAsyncStop,
  ECamCameraEventVideoStop,
  ECamCameraEventVideoRelease,
  // -------------------------------------------------------
  // Events of class ECamCameraEventClassVideoTimes
  ECamCameraEventVideoTimes,

  // -------------------------------------------------------
  // Events of class ECamCameraEventClassSettings

  // Associated event data is TCamCameraSettingId of the 
  // setting that was processed.
  ECamCameraEventSettingsSingle,
  // Associated event data is TCamCameraSettingId of the 
  // last setting that was processed.
  ECamCameraEventSettingsDone,

  // -------------------------------------------------------
  // Events of class ECamCameraEventClassAutofocus
  ECamCameraEventStartAutofocus,
  ECamCameraEventCancelAutofocus,
  ECamCameraEventAutofocusSuccessful,
  ECamCameraEventAutofocusFailed,
  ECamCameraEventSetAfRange,

  // -------------------------------------------------------
  // Events of class ECamCameraEventClassOther
  ECamCameraEventFlashReady,
  ECamCameraEventFlashNotReady,

  // -------------------------------------------------------  
  // Not an event id, but a marker.
  ECamCameraEventCount 
  };


/**
* Camera event classes.
*/
enum TCamCameraEventClassId
  {
  ECamCameraEventClassNone         =  0,

  ECamCameraEventClassBasicControl = 1<<0,

  ECamCameraEventClassImage        = 1<<1,

  ECamCameraEventClassVideo        = 1<<2,
  ECamCameraEventClassVideoTimes   = 1<<3,

  ECamCameraEventClassVfControl    = 1<<7,
  ECamCameraEventClassVfData       = 1<<8,

  ECamCameraEventClassSsControl    = 1<<10,
  ECamCameraEventClassSsData       = 1<<11,
  
  ECamCameraEventClassSettings     = 1<<12,
  
  ECamCameraEventClassAutofocus    = 1<<13,  

  ECamCameraEventClassFlashStatus  = 1<<14,

  ECamCameraEventClassOther        = 1<<30,

  ECamCameraEventClassAll          = ~ECamCameraEventClassNone
  };

#ifdef _DEBUG

static const TUint16* KCamCameraEventNames[] =
  {
  (const TUint16*)_S16("ECamCameraEventNone"),

  (const TUint16*)_S16("ECamCameraEventSequenceEnd"),

  (const TUint16*)_S16("ECamCameraEventReserveGain"),
  (const TUint16*)_S16("ECamCameraEventReserveLose"),
  (const TUint16*)_S16("ECamCameraEventReserveRequested"),
  (const TUint16*)_S16("ECamCameraEventPowerOn"),
  (const TUint16*)_S16("ECamCameraEventPowerOff"),      
  (const TUint16*)_S16("ECamCameraEventIveRecover"),
  (const TUint16*)_S16("ECamCameraEventPowerOnRequested"),

  (const TUint16*)_S16("ECamCameraEventVfStart"),
  (const TUint16*)_S16("ECamCameraEventVfStop"),
  (const TUint16*)_S16("ECamCameraEventVfRelease"),
  (const TUint16*)_S16("ECamCameraEventVfFrameReady"), 

  (const TUint16*)_S16("ECamCameraEventSsStart"),
  (const TUint16*)_S16("ECamCameraEventSsStop"),
  (const TUint16*)_S16("ECamCameraEventSsRelease"),
  (const TUint16*)_S16("ECamCameraEventSsReady"),      

  (const TUint16*)_S16("ECamCameraEventImageInit"),
  (const TUint16*)_S16("ECamCameraEventImageStart"),
  (const TUint16*)_S16("ECamCameraEventImageStop"),
  (const TUint16*)_S16("ECamCameraEventImageRelease"),
  (const TUint16*)_S16("ECamCameraEventImageData"),

  (const TUint16*)_S16("ECamCameraEventVideoInit"),
  (const TUint16*)_S16("ECamCameraEventVideoStart"),
  (const TUint16*)_S16("ECamCameraEventVideoPause"),
  (const TUint16*)_S16("ECamCameraEventVideoAsyncStop"),
  (const TUint16*)_S16("ECamCameraEventVideoStop"),
  (const TUint16*)_S16("ECamCameraEventVideoRelease"),
  (const TUint16*)_S16("ECamCameraEventVideoTimes"),

  (const TUint16*)_S16("ECamCameraEventSettingsSingle"),
  (const TUint16*)_S16("ECamCameraEventSettingsDone"),
  
  (const TUint16*)_S16("ECamCameraEventStartAutofocus"),
  (const TUint16*)_S16("ECamCameraEventCancelAutofocus"),
  (const TUint16*)_S16("ECamCameraEventAutofocusSuccessful"),
  (const TUint16*)_S16("ECamCameraEventAutofocusFailed"),
  (const TUint16*)_S16("ECamCameraEventSetAfRange"),
  
  (const TUint16*)_S16("ECamCameraEventFlashReady"),
  (const TUint16*)_S16("ECamCameraEventFlashNotReady")
  };

__ASSERT_COMPILE( (sizeof(KCamCameraEventNames) / sizeof(TUint16*)) == ECamCameraEventCount );
#endif



#endif // CAM_CAMERAEVENTS_H

// end of file
