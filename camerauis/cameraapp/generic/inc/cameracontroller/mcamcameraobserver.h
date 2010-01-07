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
* Description:  Camera observer interface.
*
*/


#ifndef CAM_CAMERAOBSERVER_H
#define CAM_CAMERAOBSERVER_H


#include "camcameraevents.h"


/**
 *  MCamCameraObserver mixin.
 *
 */
class MCamCameraObserver
  {
  public:
    
    /**
    * Handle camera event.
    *
    * @param aStatus    Status code describing the event status.
    * @param aEventId   Identifier for the event.
    * @param aEventData Event associated data. If such data is provided with the event,
    *                   it is mentioned in the documentation of particular event id.
    *                   Ownership not transferred to observer.
    */
    virtual void HandleCameraEventL( TInt              aStatus, 
                                     TCamCameraEventId aEventId, 
                                     TAny*             aEventData = NULL ) = 0;
  };


/**
* MCamCameraObservable mixin.
*/
class MCamCameraObservable
  {
  public:
  
    /**
    * Attach a new observer.
    * @param aObserver Observer to attach. If given observer is already attached,
    *                  duplicate is not added and no error is reported.
    * @param aInterest Observer event interest info. 
    *                  TCamCameraEventClassId values ORed.
    *                  If observer needs to change the interest info, it should
    *                  first detach itself and then attach with new info.
    */    
    virtual void AttachObserverL( const MCamCameraObserver* aObserver,
                                  const TUint&              aInterest ) = 0;
    
    
    /**
    * Detach an observer.
    * @param aObserver Observer to detach. If given observer is not found 
    *                  among attached observers, such an is simply ignored.
    */
    virtual void DetachObserver( const MCamCameraObserver* aObserver ) = 0;
  };


#endif // M_CAM_ENGINEOBSERVER_H

// end of file
