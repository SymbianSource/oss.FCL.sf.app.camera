/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides the flashing icon/LED & timing capability of the*
*/



// INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <eikbtgpc.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include "CamSelfTimer.h"
#include "CamPanic.h"
#include "CamAppUi.h"
#include "CamUtility.h"
#include "Cam.hrh"  // For ECamSelfTimerSoundId def
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamSelfTimerTraces.h"
#endif

// CONSTANTS


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================




// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamSelfTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamSelfTimer* CCamSelfTimer::NewL( CCamAppController& aController )
    {
    CCamSelfTimer* self = new( ELeave ) CCamSelfTimer( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CCamSelfTimer::~CCamSelfTimer()
  {
  PRINT( _L("Camera => ~CCamSelfTimer") );
  // If self-timer is active, stop it
  if ( IsActive() )
      {
      Cancel();
      }

  // close up observers array
  iObservers.Close();

  delete iPeriodicTimer;    
  PRINT( _L("Camera <= ~CCamSelfTimer") );
  }

// -----------------------------------------------------------------------------
// CCamSelfTimer::StartSelfTimer
// Will be called to start self timer.  If self timer is already running, the 
// timer is cancelled and the capture operation is run immediately.
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::StartSelfTimer()
  {
  // Check it is not already running
  if ( ECamSelfTimerStandby == iState )
    {
    ChangeState( ECamSelfTimerActive1 );
    }
  // If the timer is already running, then we must cancel the timer
  // and do the capture operation now!
  else  
    { 
    // Cancel the timer
    Cancel();

    if( ECamControllerImage == iMode )
      {
      switch ( iImageMode )
        {
        case ECamImageCaptureBurst:
        case ECamImageCaptureTimeLapse:
          {
          // Start the autofocus operation now, the capture will be queued
          // after focus completes
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP7_CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_APP_AF 1" );
          iController.StartAutoFocus();
          // Softkeys are blank/cancel if doing burst capture
          // but cancel means cancel ongoing burst, not cancel self-timer
          // replace the buttons with the correct command set
          CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
          if ( appUi )
            {
            // blank the softkeys in preparation for capture
            TRAP_IGNORE( appUi->SetSoftKeysL( R_CAM_SOFTKEYS_BLANK_STOP ) );
            }
          }
          //lint -fallthrough
        case ECamImageCaptureSingle: 
          {
          // If still Self-Timer is interrupted, take the photo NOW.
          // If we were in multi-shot, this capture is queued
          OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_APP_SHOT_TO_SNAPSHOT 1" );
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_PRI_SHOT_TO_SNAPSHOT 1" );
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_PRI_SHOT_TO_SAVE 1" );
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_PRI_SHOT_TO_SHOT 1" );
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_APP_SHOT_TO_STILL 1" );
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_APP_CAPTURE_START 1" );
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP6_CCAMSELFTIMER_STARTSELFTIMER, "e_CAM_PRI_SERIAL_SHOOTING 1" );

          iController.Capture();
          break;
          }
        default:
          break;
        }
      }
    else
      {
      // Start recording now
      TRAP_IGNORE( iController.StartVideoRecordingL() );
      }
    }
  }

// -----------------------------------------------------------------------------
// CCamSelfTimer::SetModeL
// Called when the current capture method (Video or Still) changes.
// Updates the internal state (so correct capture method is called when self
// timer expires) and if timer is already running, cancels the outstanding
// timer (as the view has changed).
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::SetModeL( TCamCameraMode         aMode, 
                             TCamImageCaptureMode   aImageMode,
                             TCamSelfTimerFunctions aFunc )
  {
  iMode      = aMode;
  iImageMode = aImageMode;
  
  // Based on the supplied selftimer function (specifying periods) load the 
  // individual data from PSI.
  TPckgBuf <TSelfTimerSetting> pckg;
  
  TInt ret = KErrNone;
  switch ( aFunc )
    {
    case ECamSelfTimerMode1:
      ret = CamUtility::GetPsiAnyL( ECamPsiSelfTimerPeriod1, &pckg );
      break;
    case ECamSelfTimerMode2:
      ret = CamUtility::GetPsiAnyL( ECamPsiSelfTimerPeriod2, &pckg );
      break;
    case ECamSelfTimerMode3:
      ret = CamUtility::GetPsiAnyL( ECamPsiSelfTimerPeriod3, &pckg );
      break;            
    case ECamSelfTimerDisabled:
      //Fall-through
    default:
      break;
    }
  // Check it is supported.  
  __ASSERT_ALWAYS( ret == KErrNone, CamPanic( ECamPanicUnhandledCommand ) );

  iSettings = pckg();
  // Set time remaining and update observers.
  iTimeRemaining = iSettings.iThreshold[ECamSelfTimerMode1];    
  NotifyObservers( ECamSelfTimerEventTimerOn );
  }

// -----------------------------------------------------------------------------
// CCamSelfTimer::Cancel
// Cancels the timer if already running, and returns the self timer to 
// self-timer standby state.
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::Cancel()
    {
    ChangeState( ECamSelfTimerStandby );
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::IsActive
// Returns whether or not the SelfTimer is currently active (counting down)
// -----------------------------------------------------------------------------
//
TBool CCamSelfTimer::IsActive()
    {
    if ( iState == ECamSelfTimerStandby )
        {
        return EFalse;
        }
    else
        {
        return ETrue;        
        }           
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::CCamSelfTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamSelfTimer::CCamSelfTimer( CCamAppController& aController ) :                                 
                                 iState( ECamSelfTimerStandby ),
                                 iController( aController )
    {                                     
    iCallback = TCallBack( TimerCallback, this );
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::ConstructL()
    {
    iPeriodicTimer = CPeriodic::NewL( CActive::EPriorityHigh );    
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::ChangeState
// Called to move between the internal self-timer states.
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::ChangeState( TCamSelfTimer aState )
    {  
    // if the timer object has not been created
    __ASSERT_DEBUG( iPeriodicTimer != NULL, CamPanic( ECamPanicNullPointer ) );

    // If the state has been changed
    iState = aState;

    switch ( iState )
        {
        case ECamSelfTimerActive1:
            iController.StopIdleTimer(); // no standby mode if time is running
            iIndication = ETrue;
            //lint -fallthrough
        case ECamSelfTimerActive2:
        case ECamSelfTimerActive3:
            iTimeRemaining = iSettings.iThreshold[iState];    
            // Use smallest time period as tick period. This assumes that
            // smallest time period is divisble into larger time period.
            iTimerTickPeriod = iSettings.iFlashPeriod[iState];    // Need half-periods
            if ( iTimerTickPeriod.Int() > iSettings.iBeepPeriod[iState] )
                {
                iTimerTickPeriod = iSettings.iBeepPeriod[iState];
                }
            
            if ( iTimerTickPeriod.Int() == KMaxTInt ) // If state is timer based (always on)
              {
              iIndication = ETrue;
              iPeriodicTimer->Cancel();
              iTimerTickPeriod = iTimeRemaining;// Force period to be time left

              // Reschedule the timer to go off when the remaining time has completed
              iPeriodicTimer->Start( iTimeRemaining, iTimeRemaining, iCallback );
              }
            else if ( iTimerTickPeriod.Int() == 0 ) // Always off
              {
              iIndication = EFalse;
              iPeriodicTimer->Cancel();
              iTimerTickPeriod = iTimeRemaining;// Force period to be time left

              // Reschedule the timer to go off when the remaining time has completed
              iPeriodicTimer->Start( iTimeRemaining, iTimeRemaining, iCallback );
              }
            else if ( iTimerTickPeriod.Int() != KErrNotSupported )    // This is a valid period
              {
              TInt period = iTimerTickPeriod.Int();
              iPeriodicTimer->Cancel();
              iPeriodicTimer->Start( period, period, iCallback );
              }
            // otherwise, do nothing
            else
              {
              // empty statement to remove Lint error.
              }
            
            // Check if we need to start autofocus
            if ( iSettings.iAutoFocus != KErrNotSupported && 
                 iTimeRemaining.Int() == iSettings.iAutoFocus )
              {
              if ( ECamControllerVideo != iMode )
                {
                // Start the AutoFocus operation
                OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSELFTIMER_CHANGESTATE, "e_CAM_APP_AF 1" );
                iController.StartAutoFocus();
                }
              }
            DoIndication( iIndication );
            break;
        
        case ECamSelfTimerStandby:           
            iPeriodicTimer->Cancel();
            if ( !iController.IsSlideInSelfTimerPreview() )
              {
              iController.SetDynamicSettingToDefault(ECamSettingItemDynamicSelfTimer);
              }
            NotifyObservers( ECamSelfTimerEventTimerOff );
            break;

        default:
            break;
        }    
    }

// -----------------------------------------------------------------------------
// TimerCallback Callback used for self timer class to be informed of periodic
// timer events
// Returns: TInt: KErrNone if successful.
// -----------------------------------------------------------------------------
//
TInt CCamSelfTimer::TimerCallback( TAny* aPtr )
    {
    CCamSelfTimer* timer = static_cast< CCamSelfTimer* >( aPtr );
    __ASSERT_DEBUG( timer != NULL, CamPanic( ECamPanicNullPointer ) ); 

    timer->Tick();        

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::Tick
// Called when the periodic timer "ticks", used to 
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::Tick()
  {
  PRINT(_L("Camera => CCamSelfTimer::Tick()"));    
  // Adjust the time remaining
  iTimeRemaining = iTimeRemaining.Int() - iTimerTickPeriod.Int();

  // Adjust the time elapsed since last beep and indicator state change.
  iTimeElapsedSinceIndication += iTimerTickPeriod.Int();

  // Adjust the indicator state if necessary.
  __ASSERT_DEBUG( iState < ECamSelfTimerStandby, CamPanic( ECamPanicUnhandledCreateSettingItem ) );
  if ( iTimeElapsedSinceIndication >= iSettings.iFlashPeriod[iState] )
    {
    iIndication = !iIndication;
    iTimeElapsedSinceIndication = 0;
    }
  
  // If we have reached the end of the timer period...
  if ( iTimeRemaining == TTimeIntervalMicroSeconds32( 0 ) )
    {
    // Cancel the timer, reset internal state.
    Cancel();

    // The Controller takes care of the synchronisation of this.
    // If AutoFocus is complete, start Capture now
    // If AutoFocus still ongoing, it will wait until autofocus is 
    // complete THEN Capture.
    if ( ECamControllerImage == iMode )
      {  
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if( appUi )
        {
        // Blank all softkeys if doing still capture
        if ( iImageMode == ECamImageCaptureSingle )
          {
          // blank the softkeys in preparation for capture
          TRAP_IGNORE( appUi->SetSoftKeysL( R_CAM_SOFTKEYS_BLANK ) );
          }
                
        // Softkeys are blank/cancel if doing burst capture
        // but cancel means cancel ongoing burst, not cancel self-timer        
        else if ( ECamImageCaptureBurst     == iImageMode 
               || ECamImageCaptureTimeLapse == iImageMode )
          {
          // blank the softkeys in preparation for capture
          TRAP_IGNORE( appUi->SetSoftKeysL( R_CAM_SOFTKEYS_BLANK_STOP ) )
          }
        }

      // Ensure everything is OFF
      DoIndication( EFalse );

      // Do capture
      OstTrace0( CAMERAAPP_PERFORMANCE, CCAMSELFTIMER_TICK, "e_CAM_APP_SHOT_TO_SNAPSHOT 1" );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMSELFTIMER_TICK, "e_CAM_PRI_SHOT_TO_SNAPSHOT 1" );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMSELFTIMER_TICK, "e_CAM_PRI_SHOT_TO_SAVE 1" );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMSELFTIMER_TICK, "e_CAM_PRI_SHOT_TO_SHOT 1" );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMSELFTIMER_TICK, "e_CAM_APP_SHOT_TO_STILL 1" );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMSELFTIMER_TICK, "e_CAM_APP_CAPTURE_START 1" );
      OstTrace0( CAMERAAPP_PERFORMANCE, DUP6_CCAMSELFTIMER_TICK, "e_CAM_PRI_SERIAL_SHOOTING 1" );

      iController.Capture();
      }
    else // video mode
      {   
      TRAP_IGNORE( iController.StartVideoRecordingL() )
      }            
    }

  // The timer is ongoing
  else 
    {
    TInt remaining = iTimeRemaining.Int();
    // Work out if we need to change period.
    if ( remaining        == iSettings.iThreshold[ECamSelfTimerActive2] 
      && KErrNotSupported != iSettings.iThreshold[ECamSelfTimerActive2] )
      {
      ChangeState( ECamSelfTimerActive2 );
      }
    else if ( remaining        == iSettings.iThreshold[ECamSelfTimerActive3] 
           && KErrNotSupported != iSettings.iThreshold[ECamSelfTimerActive3])
      {
      ChangeState( ECamSelfTimerActive3 );
      }
    else
      {
      // Update the indicators (done as part of change state as well)
      DoIndication( iIndication );
      }
  
    // Check if we need to start autofocus
    if ( KErrNotFound != iSettings.iAutoFocus 
      && remaining    == iSettings.iAutoFocus )
      {
      if ( ECamControllerImage == iMode )
        {
        // Start the AutoFocus operation
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP7_CCAMSELFTIMER_TICK, "e_CAM_APP_AF 1" );
        iController.StartAutoFocus();
        }
      }
    }
  PRINT(_L("Camera <= CCamSelfTimer::Tick()"));    
  }

// -----------------------------------------------------------------------------
// CCamSelfTimer::DoIndication
// Send event to self timer observers
// -----------------------------------------------------------------------------
//                    
void CCamSelfTimer::DoIndication( TBool aOn )
    {
    iIndication = aOn;

    if ( aOn )        
        {        
        NotifyObservers( ECamSelfTimerEventTimerOn );
        iController.PlaySound( ECamSelfTimerSoundId,  EFalse );
        }
    else
        {
        NotifyObservers( ECamSelfTimerEventTimerOff );
        }
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::AddObserverL
// Add a self timer observer 
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::AddObserverL( const MCamSelfTimerObserver* aObserver )
    {
    User::LeaveIfError( iObservers.Append( aObserver ) );
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::RemoveObserver
// Remove a self timer observer
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::RemoveObserver( const MCamSelfTimerObserver* aObserver )
    {
    TInt i;
    TInt numobservers = iObservers.Count();
    for ( i = 0; i < numobservers; i++ )
        {
        // if the observer is the one to remove
        if ( iObservers[i] == aObserver )
            {
            iObservers.Remove( i );
            return;
            }
        }
    }

// -----------------------------------------------------------------------------
// CCamSelfTimer::NotifyObservers
// Send self timer event to observers
// -----------------------------------------------------------------------------
//
void CCamSelfTimer::NotifyObservers( TCamSelfTimerEvent aEvent )
    {
    const TInt KMicroSecsPerSec = 1000000;
    TInt timeRem = iTimeRemaining.Int() / KMicroSecsPerSec;
    TInt i;
    TInt numobservers = iObservers.Count();
    for ( i = 0; i < numobservers; i++ )
        {
        iObservers[i]->HandleSelfTimerEvent( aEvent, timeRem );
        }
    }

//  End of File
