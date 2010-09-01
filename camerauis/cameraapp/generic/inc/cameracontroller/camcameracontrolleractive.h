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
* Description:  CCamCameraControllerActive class declaration.
*                This class presents an active object used to call back 
*                CCamCameraController methods. This is done to keep clients
*                not blocked when issuing a request or sequence of requests.
*                
*
*/


#ifndef CAM_CAMERACONTROLLERACTIVE_H
#define CAM_CAMERACONTROLLERACTIVE_H

// ===========================================================================
// Included headers

#include <e32base.h>


// ===========================================================================
// Forward declarations

class CCamCameraController;


// ===========================================================================
// Classes

/**
* Camera controller active object.
*
* Used to assist on asynchronising CCamCameraController requests.
*
*/
class CCamCameraControllerActive : public CActive
  {
  // =========================================================================
  // Methods
 
  // -------------------------------------------------------------------------
  // Constructor(s) and destructor
  public:

    /**
    * Static 2 phase constructor.
    * @param aController Camera controller, which callbacks will be called.
    * @param aPriority   This active object's priority.
    */
    static CCamCameraControllerActive* NewL( CCamCameraController& aController, TInt aPriority );
    
    /**
    * Destructor.
    */
    virtual ~CCamCameraControllerActive();

  private:
  
    /**
    * Standard Symbian 2nd phase constructor.
    */
    void ConstructL();
    
    /**
    * Standard C++ constructor.
    */
    CCamCameraControllerActive( CCamCameraController& aController, 
                                TInt                  aPriority   );

  // -------------------------------------------------------------------------
  // from CActive
  protected:
  
    /**
    * Reacts to CActive::Cancel, if this AO was active.
    * @see See CActive for more info.
    */
  	virtual void DoCancel();

    /**
    * Calls CCamCameraController::ProcessNextRequestL. If leave occurs there,
    * RunError will be called by CActiveScheduler. Checks the return value of
    * CCamCameraController::ProcessNextRequestL and if it is not zero, sets this
    * AO ready for new iteration.
    * @see See CActive for more info.
    */
  	virtual void RunL();

    /**
    * Called if a leave occurs in RunL.
    * @param aError Leave code from RunL.
    * @return Always KErrNone.
    * @see See CActive for more info.
    */
  	virtual TInt RunError( TInt aError );

  // -------------------------------------------------------------------------
  // New methods.
  public:

    /**
    * Sets this Active Object to ActiveScheduler's queue.
    * RunL calls back CCamCameraController::ProcessNextRequestL.
    * If the return value is not zero, this AO will be scheduled to run again.
    * Otherwise new call to IssueRequest is needed.
    * If leave occurs in RunL, RunError calls CCameraController::EndSequence.
    */
    void IssueRequest();

  private:

    /**
    * Stop the sequence handling on error.
    * Calls CCameraController::EndSequence.
    */
    void HandleError( TInt aStatus );

  // =========================================================================
  // Data
  private: 

    /**
    * Camera controller, which callbacks will be called.
    */
    CCamCameraController& iController;
    
  // =========================================================================
  };
  
#endif // CAM_CAMERACONTROLLERACTIVE_H

// end of file
