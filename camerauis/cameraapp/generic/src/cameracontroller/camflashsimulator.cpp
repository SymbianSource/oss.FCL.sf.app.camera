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
* Description:  Simulator for flash recharging
*
*/



// ===========================================================================
// Includes
#include <ECam.h>            // TECAMEvent
#include <ecamadvsettings.h> // ECam event ids

#include "camlogging.h"
#include "camcameracontroller.h"
#include "camflashsimulator.h"


// ===========================================================================
// Local constants
static const TUint KEventInterest = ECamCameraEventClassNone
                                  | ECamCameraEventClassImage;


// ===========================================================================
// Methods

// ---------------------------------------------------------------------------
// CCamFlashSimulator::CCamFlashSimulator
// ---------------------------------------------------------------------------
//
CCamFlashSimulator::CCamFlashSimulator( CCamCameraController& aController )
  : iController   ( aController ),
    iError        ( KErrNone    ),
    iFlashRequired( EFalse      ),
    iFlashReady   ( ETrue       )
  {
  }

// ---------------------------------------------------------------------------
// CCamFlashSimulator::~CCamFlashSimulator
// ---------------------------------------------------------------------------
//    
CCamFlashSimulator::~CCamFlashSimulator()
  {
  iController.DetachObserver( this );

  if( iRechargeTimer )
    {
    iRechargeTimer->Cancel();
    delete iRechargeTimer;
    }
  // DeleteProperty();
  }
    

// ---------------------------------------------------------------------------
// CCamFlashSimulator::NewL
// ---------------------------------------------------------------------------
// 
CCamFlashSimulator*
CCamFlashSimulator::NewL( CCamCameraController& aController )
  {
  CCamFlashSimulator* self = 
      new (ELeave) CCamFlashSimulator( aController );
  CleanupStack::PushL( self );
  self->ConstructL();
  CleanupStack::Pop(); // self
  return self;
  }

// ---------------------------------------------------------------------------
// CCamFlashSimulator::ConstructL
// ---------------------------------------------------------------------------
//
void
CCamFlashSimulator::ConstructL()
  {
  iRechargeTimer = CPeriodic::NewL( EPriorityNormal );

  iController.AttachObserverL( this, KEventInterest );
  }
    

// ---------------------------------------------------------------------------
// HandleCameraEventL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamFlashSimulator::HandleCameraEventL( TInt              aStatus, 
                                        TCamCameraEventId aEventId, 
                                        TAny*             aEventData /*=NULL*/ )
  {
  switch( aEventId )
    {
    case ECamCameraEventImageInit :
      {
      StartRecharging();
      break;
      }
    
    default :
      {
      break;
      }
    }
  }


// ---------------------------------------------------------------------------
// CCamFlashSimulator::SetFlashRequired
// ---------------------------------------------------------------------------
//
void
CCamFlashSimulator::SetFlashRequired( TBool aFlashRequired )
  {
  iFlashRequired = aFlashRequired;
  }

// ---------------------------------------------------------------------------
// CCamFlashSimulator::SetFlashError
// ---------------------------------------------------------------------------
//    
void
CCamFlashSimulator::SetFlashError( TBool aFlashError )
  {
  iError = aFlashError;
  }
    

// ---------------------------------------------------------------------------
// CCamFlashSimulator::StartRecharging
// ---------------------------------------------------------------------------
//    
void
CCamFlashSimulator::StartRecharging( TBool aError )
  {
  if( iRechargeTimer )
    {
    // Empty the flash, and publish the property value
    iFlashReady = EFalse;  
    
    // The possible error shall be published when the recharging is done
    iError = aError;
    
    // Start recharging timer
    iRechargeTimer->Cancel();
    iRechargeTimer->Start( KSimulatedChargingTime,
                           KMaxTInt32,
                           TCallBack( RechargeTimerCallback, this ) );    

    // Send "flash not ready" event to Camera Controller
    NotifyController();
    }
  }


// ---------------------------------------------------------------------------
// NotifyController
// ---------------------------------------------------------------------------
//
void 
CCamFlashSimulator::NotifyController()
  {
  PRINT2( _L("Camera => CCamFlashSimulator::NotifyController, flash ready:%d, status:%d"), iFlashReady, iError )

  const TUid eventId( iFlashReady 
                    ? KUidECamEventFlashReady 
                    : KUidECamEventFlashNotReady );   
  const TECAMEvent event( eventId, iError );

  iController.HandleEvent( event );

  PRINT ( _L("Camera <= CCamFlashSimulator::NotifyController") );
  }

// ---------------------------------------------------------------------------
// CCamFlashSimulator::RechargeTimerCallback
// ---------------------------------------------------------------------------
//
TInt
CCamFlashSimulator::RechargeTimerCallback( TAny* aSelf )
  {
  CCamFlashSimulator* self = static_cast<CCamFlashSimulator*>( aSelf );
  if( self )
    {
    self->iRechargeTimer->Cancel();
    // Set the flash to ready state and publish value
    self->iFlashReady = ETrue;

    // Send "flash ready" event to Camera Controller
    self->NotifyController();
    }
  return KErrNone;    
  }
