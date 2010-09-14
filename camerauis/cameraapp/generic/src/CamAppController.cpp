/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for managing intercommunication between Camera UI*
*/



// ===========================================================================
// INCLUDE FILES

#include <bldvariant.hrh> // for feature definitions

#include <e32property.h>
#include <apparc.h>
#include <fbs.h>
#include <eikenv.h>

#include <hal.h>
#include <hal_data.h>
#include <pathinfo.h>
#include <barsread.h>
#include <AknUtils.h>
#include <akntoolbar.h>
#include <akntoolbarextension.h>
#include <centralrepository.h>

#include <ctsydomainpskeys.h>
#include <ProfileEngineSDKCRKeys.h>
#include <sysutildomaincrkeys.h>
#include <ScreensaverInternalPSKeys.h>
#include <musresourceproperties.h>
#include <cameraplatpskeys.h>
#include <bitmaptransforms.h>
#include <sensrvorientationsensor.h>
#include <sensrvchannel.h>
#include <sensrvchannelfinder.h>

#include <UsbWatcherInternalPSKeys.h>
#include <usbpersonalityids.h>

#include <UikonInternalPSKeys.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include "CamSettingsInternal.hrh"
#include "CamProductSpecificSettings.hrh"
#include "CameraappPrivateCRKeys.h"
#include "CamAppController.h"
#include "CamUtility.h"
#include "CamLogger.h"
#include "CamSettings.hrh"
#include "CamSettingsModel.h"
#include "CamPanic.h"
#include "CamBurstCaptureArray.h"
#include "CamTimer.h"
#include "CamImageSaveActive.h"
#include "CamAppUi.h"
#include "CamObserverHandler.h"
#include "CamSelfTimer.h"
#include "camflashstatus.h"

#include "CamPerformance.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamAppControllerTraces.h"
#endif

#include "CamVideoQualityLevel.h"
#include "CamCallStateAo.h"    
#include "CamPropertyWatcher.h"
#include <avkondomainpskeys.h>
#include <ProfileEngineInternalPSKeys.h>
#include <ProfileEnginePrivatePSKeys.h>

#ifndef __WINSCW__
  #include "rlocationtrail.h"
  #include "locationtrailpskeys.h"
#endif
//#include "camconstantsettingprovider.h"
#include "camsettingprovider.h"
#include "camsettingconversion.h"
#include "camcamerarequests.h"
#include "camcameraevents.h"
#include "camcameracontrollertypes.h"
#include "camcameracontroller.h"
#include "cambuffershare.h"
#include "cambuffersharecleanup.h"
#include "camaudioplayercontroller.h"
#include "camuidconstants.h"
#include "camconfiguration.h"
#include "CamPreCaptureViewBase.h"
#include "CamPostCaptureViewBase.h"

#include <cfclient.h>
#include <cfcontextobject.h>
#include <cfcontextquery.h>

#include "camvideotime.h"
#include "CamGSInterface.h"
#include "CameraUiConfigManager.h"
#include "camsnapshotrotator.h"
#include "CamVideoPreCaptureView.h"

#include <bitmaptransforms.h> 

#ifdef _DEBUG
#ifdef _AO_TRACKING
struct CFakeActiveScheduler : public CActiveScheduler {
  virtual TInt Extension_( TUint, TAny *&, TAny* ) { return 0; }
};
#endif // _AO_TRACKING
#endif // _DEBUG

// ===========================================================================
// Local constants

// Sequence mode related constants that define the amount of pictures taken
// with sequence mode.
const TInt KShortBurstCount  = 18;   // number of images captured during burst
const TInt KMinBurstCount    = 2;    // minimum of images captured
const TInt KBurstEstimate    = 10;   // Correction of underestimated file size  

const TUint32 KCamLatestFilePath      = 0x00000001;


// temporary constants until image naming is implemented
_LIT( KImageExtension, ".jpg" );
_LIT( KVideo3GPExtension, ".3gp" );
#ifndef __WINS__
_LIT( KVideoMP4Extension, ".mp4" );
#endif

const TInt KMaxExtension = 4;
//const TInt64 KBurstInterval = 0;

const TInt KIdleTimeout     = 1000000 * 60; // 60 seconds
const TInt KDeepSleepTimeout = KIdleTimeout*2; // Measuring time is two minutes  
const TInt KVideoArrayUsers = 1;
const TInt KImageArrayUsers = 2;

const TInt KVideoNameRetryCount = 1;    // Maximum number of video naming retries

// UID for central repository file
const TInt KCRCamShutterSound = 0x7; // key for shutter sound

const TInt KMicroSecsInMilliSec = 1000;
const TInt KSecondInMicSec = 1000000;
#if defined(RD_MDS_2_5) && !defined(__WINSCW__)
const TInt KLocTrailCloseRetryTime = 5 * KSecondInMicSec;
#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__)

// Estimated time needed to complete autofocus
const TInt KFocusingDurationSeconds = 2;

const TInt KCCorFocused = 0x00000002;

const TInt KCamMaxDateLen = 8;

static const TInt KTimelapseArrayGranularity = 6;

// Backlight / Inactivity timer related
//   Callback interval is set to 4s. 
//   Smallest value that the user can select from Phone Settings is 5s.
static const TInt KBacklighTimerPriority = CActive::EPriorityHigh;
static const TInt KBacklighTimerInterval = 4*1000*1000; 

// Camera Controller events interest.
//   Want to receive all events.
//   Vf frames needed for correct zoom handling (should be left out otherwise).
static const TUint KCamEventInterest = (  ECamCameraEventClassAll );

static const TInt KLensCoverDelay = 500*1000;

_LIT( KCamContextSource, "Application" );
_LIT( KCamContextType, "Camera.Zoom" );
_LIT( KCamContextValueEnable, "Enabled" );
_LIT( KCamContextValueDisable, "Disabled" );
_LIT_SECURITY_POLICY_PASS( KCamContextSecurity );

static const TInt KCriticalMemoryLevel = 5*1000*1000;
// ===========================================================================
// Local namespace
namespace NCamAppController
  {
  // -------------------------------------------------------
  // Request sequences 
//  static const TCamCameraRequestId KCamSequencePowerOffOn[] = 
//    { 
//    ECamRequestPowerOff, 
//    ECamRequestPowerOn
//    };
  
  // -------------------------------------------------------
  // Local methods

  inline TInt ResetBitmap( CFbsBitmap*& aBitmap )
    {
    if( aBitmap )
      {
      aBitmap->Reset();
      return KErrNone;
      }
    else
      {
      TRAPD( create, aBitmap = new (ELeave) CFbsBitmap() );
      return create;
      }
    };

  inline void ClearSequenceBusyFlag( TAny* aBusyFlags )
    {
    TUint* flags = static_cast<TUint*>( aBusyFlags );
    if( flags )
      {
      *flags &= ~EBusySequence;
      }
    };

  inline void ClearRequestBusyFlag( TAny* aBusyFlags )
    {
    TUint* flags = static_cast<TUint*>( aBusyFlags );
    if( flags )
      {
      *flags &= ~EBusyRequest;
      }
    };
  }

#include "camflagutility.inl"
#include "campointerutility.inl"

using namespace NCamAppController;
using namespace NCamCameraController;
// ===========================================================================


// ===========================================================================
// Member functions

// ---------------------------------------------------------------------------
// CCamAppController::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCamAppController* CCamAppController::NewL()
    {
    CCamAppController* self = new( ELeave ) CCamAppController();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CCamAppController::CompleteConstructionL
// The camera engine cannot be instantiated until the application orientation has 
// been set by the CCamAppUi. This does not exist when the CCamAppController is 
// constructed. CompleteConstructionL must be called in CCamAppUi::ConstructL()
// ---------------------------------------------------------------------------
//    
void CCamAppController::CompleteConstructionL()
  {
  PRINT( _L("Camera => CCamAppController::CompleteConstructionL" ))

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  
  // Memory critical levels reading moved to the point when
  // those values are actually needed for the first time.   

  TInt index = 0;
  iInfo.iActiveCamera = ECamActiveCameraPrimary;
  PRINT1( _L("Camera <> Cameras available: %d"), CamerasAvailable() )
  
  PRINT( _L("Camera <> Store primary camera settings"))
  iSettingsModel->StorePrimaryCameraSettingsL();


  PRINT1( _L("Camera <> CCamAppController::CompleteConstructionL iSlideState initial value = %d" ), iSlideState)
  if ( iConfigManager->IsLensCoverSupported() )
      {
      // Request notification of slide state changes
      iSlideStateWatcher->Subscribe();

      // Read the current slider status - use the front camera (if there is one) as default if
      // there are any errors.
      TInt slideErr = iSlideStateWatcher->Get( iSlideState );
      PRINT1( _L("Camera <> CCamAppController::CompleteConstructionL setting iSlideState to %d" ), iSlideState)
      TInt requiredOrientation;
      if ( ( appUi->CamOrientation() == ECamOrientationCamcorderLeft && iSlideState == CameraPlatPSKeys::EClosed ) ||
            ( appUi->CamOrientation() == ECamOrientationCamcorder && iSlideState == CameraPlatPSKeys::EClosed ) )
        {
        if ( appUi->IsQwerty2ndCamera() )
            {  
            PRINT( _L("Camera <> Do not exit. Reload landscape 2nd camera settings") )
            index = 1;
            iInfo.iActiveCamera = ECamActiveCameraSecondary;
            CCamAppUi* appUi = static_cast<CCamAppUi*>( 
                               CEikonEnv::Static()->AppUi() ); 
            // We may have primary camera settings loaded 
            LoadStaticSettingsL( appUi->IsEmbedded() );   
            CamUtility::GetPsiInt( ECamPsiSecondaryCameraOrientation, 
                                   requiredOrientation );
            }
        else
            {
            PRINT( _L("Camera <> Lens cover has been closed during camera construction. Exit camera.") )
            appUi->HandleCommandL( EEikCmdExit);
            }
        }
      else  
        {  
        // if the slide is closed or there is an error, then use the front camera
        // check that there is more than 1 camera and that the current orientation is correct
        if ( ( iSlideState == CameraPlatPSKeys::EClosed               
              || slideErr    != KErrNone )
              && CamerasAvailable() > 1 )
          {
          PRINT( _L("Camera <> CCamAppController::CompleteConstructionL slider is not open" ))
          index = 1;
          iInfo.iActiveCamera = ECamActiveCameraSecondary;
          CamUtility::GetPsiInt( ECamPsiSecondaryCameraOrientation, requiredOrientation );
          }
        else // slide is open or using primary camera as default
          {
          PRINT( _L("Camera <> CCamAppController::CompleteConstructionL slider is open- primary camera in use" ))
          index = 0;
          iInfo.iActiveCamera = ECamActiveCameraPrimary;

          //when active camera is primary camera,orientation must be landscape,
          //so need to fix orientation
          if ( ECamOrientationPortrait == appUi->CamOrientation() )
            {
            TInt primaryOrientation;
            CamUtility::GetPsiInt( ECamPsiPrimaryCameraOrientation, 
                                      primaryOrientation );
            appUi->SetOrientationL( ( CAknAppUiBase::TAppUiOrientation ) primaryOrientation );		
            appUi->SetCamOrientationToLandscape();
            }
          }   
        }
      }

  ConstructCameraControllerL( index );
  
  // Send startup sequence ?

  /**
   * Removed all custom interfaces requested from engine
   */
  iAccSensorListening = EFalse;

  iCallStateAo = CCamCallStateAo::NewL( this );             
  
  // Use backlight timer instead of every Nth vf frame event
  // to reset inactivity timer. This is to avoid adding load
  // when higher VF frame rates are used (and overall load is
  // already higher).
  iBacklightTimer = CPeriodic::NewL( KBacklighTimerPriority );

#ifdef _DEBUG
#ifdef _AO_TRACKING
  TAny *iAoBacklightTimer = (TAny *)iBacklightTimer;
  PRINT2( _L("Camera <> CCamAppController: BacklightTimer=%x, %d"),iBacklightTimer, iAoBacklightTimer );
  CActiveScheduler *pAS = CActiveScheduler::Current();
  CFakeActiveScheduler *pFAS = static_cast<CFakeActiveScheduler*>(pAS);
  pFAS->Extension_( 0, iAoBacklightTimer, (TAny *)("iBacklightTimer") );
#endif // _AO_TRACKING
#endif // _DEBUG

#ifdef _DEBUG
#ifdef _AO_TRACKING
  TAny *iAoDeepSleepTimer = (TAny *)iDeepSleepTimer;
  PRINT2( _L("Camera <> CCamAppController: DeepSleepTimer=%x, %d"),iDeepSleepTimer, iAoDeepSleepTimer );
  CActiveScheduler *pAS2 = CActiveScheduler::Current();
  CFakeActiveScheduler *pFAS2 = static_cast<CFakeActiveScheduler*>(pAS2);
  pFAS2->Extension_( 0, iAoDeepSleepTimer, (TAny *)("iDeepSleepTimer") );
#endif // _AO_TRACKING
#endif // _DEBUG


  if ( UiConfigManagerPtr()->IsXenonFlashSupported() )
      {
      PRINT( _L("Camera <> CCamAppController: Create flash status observer..") );
      iFlashStatus = CCamFlashStatus::NewL( *this );
      }

  PRINT( _L("Camera <= CCamAppController::CompleteConstructionL" ))
  }    


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::GetCriticalMemoryLevelL( const TCamMediaStorage& aStorage )
  {
  PRINT( _L("Camera => CCamAppController::GetCriticalMemoryLevelL" ) )

  // If this is the first call here, get the critical levels from
  // Central repository. Later the cached values will be used.
  if( KErrNotFound == iRamDiskCriticalLevel
   || KErrNotFound == iDiskCriticalLevel   )
    {
    CRepository* repository = CRepository::NewLC( KCRUidDiskLevel );
    TInt err;
    err = repository->Get( KDiskCriticalThreshold , iDiskCriticalLevel );
    if ( KErrNone != err )
      {
      CamPanic( ECamPanicDefaultNotFoundInIniFile );      
      }
    err = repository->Get( KRamDiskCriticalLevel , iRamDiskCriticalLevel ); 
    if ( KErrNone != err )
      {
      CamPanic( ECamPanicDefaultNotFoundInIniFile ); 
      } 
    CleanupStack::PopAndDestroy( repository );
    }
  
  TInt level = ( aStorage == ECamMediaStoragePhone ) 
             ? iRamDiskCriticalLevel
             : iDiskCriticalLevel;

  PRINT1( _L("Camera <= CCamAppController::GetCriticalMemoryLevelL, level:%d" ), level )
  return level;
  }

// ---------------------------------------------------------------------------
// CCamAppController::~CCamAppController
// Destructor
// ---------------------------------------------------------------------------
//
CCamAppController::~CCamAppController()
  { 
  PRINT( _L("Camera => ~CCamAppController") );
  // As a precaution, make sure the screen saver is never left in a disabled state
  EnableScreenSaver( ETrue );

  RProperty::Set( KPSUidCamcorderNotifier, KCCorFocused, 0 );
  
  if ( iFlashStatus )
    {
    delete iFlashStatus;
    }

  delete iSoundPlayer;

  PRINT( _L("Camera <> close observers array..") );
  __ASSERT_DEBUG( iControllerObservers.Count() == 0, CamPanic( ECamPanicResourceLeak ) );
  iControllerObservers.Close();

  delete iCallStateAo;    

  delete iBurstTimer;

// Using timer also on bitmap mode.
  if( iBacklightTimer )
    {
    iBacklightTimer->Cancel();
    delete iBacklightTimer;
    }

    if( iConfigManager && iConfigManager->IsOrientationSensorSupported() && 
    		iAccSensorListening )
        {
        iAccSensorChannel->StopDataListening();
        iAccSensorChannel->CloseChannel();
        }
    delete iAccSensorChannel;

  if ( iSlideStateWatcher )
    {
    iSlideStateWatcher->Cancel();
    delete iSlideStateWatcher;
    }
  
  if ( iSliderCallBack )
    { 
    iSliderCallBack->Cancel();
    delete iSliderCallBack;
    iSliderCallBack = NULL;
    }

  if ( iKeyLockStatusWatcher && iConfigManager && iConfigManager->IsKeyLockWatcherSupported() )
    {
    iKeyLockStatusWatcher->Cancel();
    delete iKeyLockStatusWatcher;
    }
  
  if ( iProfileStatusWatcher )
      {
      iProfileStatusWatcher->Cancel();
      delete iProfileStatusWatcher;
      }
  
  if ( iConfigManager  && iConfigManager->IsPublishZoomStateSupported() )
      {
      TRAP_IGNORE ( PublishZoomStateL( EFalse ) );
      delete iContextFwClient;      
      }

  if ( iConfigManager  && iConfigManager->IsLocationSupported() )
    {
    if( iLocationTrailTimer )
      {
      iLocationTrailTimer->Cancel();
      delete iLocationTrailTimer;
      iLocationTrailTimer = NULL;
      }
    }
      
  if ( iConfigManager  && iConfigManager->IsLocationSupported() )
    {
    // stop trail and close session
    StopLocationTrail( ETrue );
    }

  if ( iImageSaveActive )  
    {
      iImageSaveActive->ForceCancel();
    delete iImageSaveActive;
    }

  PRINT( _L("Camera <> delete settingsmodel..") );
  delete iSettingsModel;  // Must be before iEngine deleted
  iSettingsPreviewHandler = NULL; // Currently the Settings Model object.
  iPreviewRollbacks.Close();


  delete iCaptureArray;
  delete iRotationArray;
  if ( iSequenceFilenameArray )
    {
    iSequenceFilenameArray->Reset();
    delete iSequenceFilenameArray;
    }    

  if( iDeepSleepTimer )
    {
    iDeepSleepTimer->Cancel();
    delete iDeepSleepTimer;
    }

  if( iIdleTimer )
    {
    iIdleTimer->Cancel();
    delete iIdleTimer;
    }

  if ( iTimeLapseTimer )
    {
    iTimeLapseTimer->Cancel();
    delete iTimeLapseTimer;
    }

  if( iCaptureToneDelayTimer )
    {
    iCaptureToneDelayTimer->Cancel();
    delete iCaptureToneDelayTimer;
    }

  delete iCaptureCompletionObserverHandler;      
  delete iSnapShotCopy; 
    
  iJpegDataSizes.Close();
  iSequencePostProcessDataSizes.Close();
  
  // Close the arrays
  iPendingObserversArray.Close();
  iCameraEventInterested.Close();

  // Controller handles releasing CCamera if needed.
  PRINT( _L("Camera <> delete camera controller..") );
  delete iCameraController;
  PRINT( _L("Camera <> delete setting provider..") );
  delete iSettingProvider;
  PRINT( _L("Camera <= ~CCamAppController") );
  
  PRINT( _L("Camera <> delete Configuration Manager..") );
  delete iConfiguration;
  iConfiguration = NULL;  

  RProperty::Delete( KPSUidCamcorderNotifier, KCCorFocused );  
    if( iPlugin )
        {
        // Destroy Ecom plugin
        iPlugin->DestroyPlugin();
        }
  iPlugin = NULL;
  delete iDriveChangeNotifier;
  iFs.Close();
  
  if( iRotatorAo )
      {
      delete iRotatorAo;
      }

  delete iSnapShotRotator;
  
  if( iTvAccessoryMonitor )
      {
      delete iTvAccessoryMonitor;
      iTvAccessoryMonitor = NULL;
      }
  
  PRINT( _L("Camera <= ~CCamAppController") );
  }


// ---------------------------------------------------------------------------
// CCamAppController::SavedCurrentImage
// Returns whether the last requested captured image has been saved or not.
// ---------------------------------------------------------------------------
//
TBool CCamAppController::SavedCurrentImage() const
  {
  PRINT( _L("Camera => CCamAppController::SavedCurrentImage") );
  TBool saved( ETrue );

  // If we are waiting for a save request then image has not been saved.
  if( iImageSaveRequestPending )
    {
    PRINT( _L("Camera <> CCamAppController::SavedCurrentImage: iImageSaveRequestPending") )
    saved = EFalse;
    }
  else if( CurrentFullFileName() == KNullDesC )
    {
    saved = EFalse;
    PRINT( _L("Camera <> CCamAppController::SavedCurrentImage: filename not yet reserved") ) 
    }
  else if( !iCaptureArray->AlreadySavedFile( CurrentFullFileName() ) ||
            iCaptureArray->CurrentlySavingFile( CurrentFullFileName() ) )
    {
    PRINT( _L("Camera <> CCamAppController::SavedCurrentImage: capture array not saved file or currently saving file") )
    saved = EFalse;
    }
  else if ( iInfo.iOperation == ECamCapturing || iInfo.iOperation == ECamCompleting )
    {
   	PRINT(_L("Camera CCamAppController::SavedCurrentImage returning false, capturing/completing") )
   	saved = EFalse;    
    }
  else
    {
    // empty
    }

  PRINT1( _L("Camera <= CCamAppController::SavedCurrentImage, return %d"), saved );
  return saved;
  }

// ---------------------------------------------------------------------------
// CCamAppController::RenameCurrentFileL
// Rename image/video.
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::RenameCurrentFileL( const TDesC&          aNewName, 
                                       const TCamCameraMode& /*aMode*/ )
  {
  TBool status = EFalse;

  // If file to rename has already been saved then remove from album,
  // rename the file and add the renamed file to the album.
  if ( BurstCaptureArray()->AlreadySavedFile( CurrentFullFileName() ) )
    {                
    // Rename the file.
    TFileName filePath = CurrentFullFileName();
    CamUtility::RenameStillImageL( filePath, aNewName, filePath );
    
    // Update capture array with new name and path.
    BurstCaptureArray()->SetNameL( filePath, aNewName, iCurrentImageIndex );
    
    status = ETrue;
    }
  // Otherwise, if the file is not currently being saved, modify the names 
  // in the capture array before it is saved.
  else if ( !BurstCaptureArray()->CurrentlySavingFile( CurrentFullFileName() ) )
    {
    // Update capture array with new name and path.
    // ...determine new path.
    TParsePtrC parse( CurrentFullFileName() );
    TFileName fullNewPath;
    fullNewPath = parse.DriveAndPath();
    fullNewPath.Append( aNewName );
    fullNewPath.Append( parse.Ext() );

    BurstCaptureArray()->SetNameL( fullNewPath, aNewName, iCurrentImageIndex );

    status = ETrue;
    }
  // Otherwise, the file is in the process of being saved, and 
  // so that renaming must wait until completed.
  else
    {
    // Leave status to EFalse
    }
    
  return status;
  }


// ---------------------------------------------------------------------------
// CCamAppController::AddControllerObserverL
// Add a controller observer.
// ---------------------------------------------------------------------------
//
void CCamAppController::AddControllerObserverL( const MCamControllerObserver* aObserver )
  {
  if( aObserver &&
      KErrNotFound == iControllerObservers.Find( aObserver ) )
    {
    User::LeaveIfError( iControllerObservers.Append( aObserver ) );
    }
  }

// ---------------------------------------------------------------------------
// CCamAppController::RemoveControllerObserver
// Remove a controller observer.
// ---------------------------------------------------------------------------
//
void CCamAppController::RemoveControllerObserver( const MCamControllerObserver* aObserver )
  {
  // Errors ignored
  if( aObserver )
    {
    TInt index = iControllerObservers.Find( aObserver );
    if( KErrNotFound != index )
      {
      iControllerObservers.Remove( index );
      }
    }
  }

// ---------------------------------------------------------------------------
// AddCameraObserverL
// ---------------------------------------------------------------------------
//
void 
CCamAppController
::AddCameraObserverL( const MCamCameraObserver* aObserver, 
                      const TUint&              aEventInterest )
  {
  PRINT1( _L("Camera => CCamAppController::AddCameraObserverL aObserver=%d"), aObserver );

  // First check that observer is not in array already
  if( KErrNotFound == iPendingObserversArray.Find( aObserver ) )
      {  
      PRINT(_L("Camera <> CCamAppController::AddCameraObserverL add Observer") );

      iPendingObserversArray.AppendL( aObserver );
      iCameraEventInterested.AppendL( aEventInterest );
      }

  if( iCameraController )
      {
      PRINT(_L("Camera <> CCamAppController::AddCameraObserverL camera controller available") );
      while ( iPendingObserversArray.Count() > 0 &&
              iCameraEventInterested.Count() > 0 )
          {
          const MCamCameraObserver* camEventObserver = iPendingObserversArray[0];
          TUint eventInterest = iCameraEventInterested[0];
          iCameraController->AttachObserverL( camEventObserver, eventInterest );
          iPendingObserversArray.Remove( 0 );
          iCameraEventInterested.Remove( 0 );
          } 
      }
  PRINT(_L("Camera <= CCamAppController::AddCameraObserverL") );
  }

// ---------------------------------------------------------------------------
// RemoveCameraObserver
// ---------------------------------------------------------------------------
//
void 
CCamAppController
::RemoveCameraObserver( const MCamCameraObserver* aObserver )
  {
  if( iCameraController )
    iCameraController->DetachObserver( aObserver );
  }

// ---------------------------------------------------------------------------
// AddSettingsObserverL
// ---------------------------------------------------------------------------
//
void
CCamAppController
::AddSettingsObserverL( const MCamSettingsModelObserver* aObserver )
  {
  if( iSettingsModel )
    iSettingsModel->AttachObserverL( aObserver );
  else
    User::Leave( KErrNotReady );
  }

// ---------------------------------------------------------------------------
// RemoveCameraObserver
// ---------------------------------------------------------------------------
//
void 
CCamAppController
::RemoveSettingsObserver( const MCamSettingsModelObserver* aObserver )
  {
  if( iSettingsModel )
    iSettingsModel->DetachObserver( aObserver );
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamAppController::ConstructCameraControllerL( TInt aCameraIndex )
  {
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPCONTROLLER_CONSTRUCTCAMERACONTROLLERL, "e_CCamAppController_ConstructCameraControllerL 1" );
  PRINT( _L("Camera => CCamAppController::ConstructCameraControllerL") )
  PERF_EVENT_START_L2( EPerfEventCAEConstruction );

  if( !iCameraController )
    {
    iCameraController = CCamCameraController::NewL( *iSettingProvider, *this,
                                                    aCameraIndex ); 
    //If uiorientationoverride feature is not supported, the camera switch has
    // to be finished here                                                    
    if( !( iConfigManager && iConfigManager->IsUIOrientationOverrideSupported() ) )
      {
      iCameraController->CompleteSwitchCameraL();
      }
    }

  // Attach as Camera Controller observer to get events
  PRINT1( _L("Camera <> Attaching as camera observer with interest:%032b"), KCamEventInterest );
  iCameraController->AttachObserverL( this, KCamEventInterest );


  PERF_EVENT_END_L2( EPerfEventCAEConstruction );
  PRINT( _L("Camera <= CCamAppController::ConstructCameraControllerL") )
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPCONTROLLER_CONSTRUCTCAMERACONTROLLERL, "e_CCamAppController_ConstructCameraControllerL 0" );
  }

// #################################################################################################

// ---------------------------------------------------------------------------
// Returns the current camera controller state
// (Bitfield of type TCamCameraStateFlags )
// ---------------------------------------------------------------------------
//
TUint
CCamAppController::CameraControllerState() const
  {
  TUint state( ECamIdle );
  if( iCameraController )
    {
    state = iCameraController->State();
    }
  PRINT1( _L("Camera =><= CCamAppController::CameraControllerState(): %032b"), state );
  return state;
  }
  
// ---------------------------------------------------------------------------
// Returns the current camera state
// (TCamCameraState)
// ---------------------------------------------------------------------------
//
TCamCameraState
CCamAppController::CameraState() const
  {
  TCamCameraState state( ECamCameraIdle );  
  
  TUint controllerState = CameraControllerState();
 
  // Convert controller state to TCamCameraState
  // Use the state represented by the "most significant"
  // bit in the bitfield. Ignore VF state.
  if( IsFlagOn( controllerState, ECamImageOn ) )
    {
    state = ECamCameraPreparedImage;
    }
  else if( IsFlagOn( controllerState, ECamVideoOn ) )
    {
    state = ECamCameraPreparedVideo;  
    }
  else if( IsFlagOn( controllerState, ECamPowerOn ) )  
    {
    state = ECamCameraPowerOn;  
    }
  else if( IsFlagOn( controllerState, ECamReserved ) )  
    {
    state = ECamCameraReserved;  
    }
    
  PRINT1( _L("Camera =><= CCamAppController::CameraState(): %d"), state );
  return state;
  }  

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCameraMode
CCamAppController::CurrentMode() const
  {
  PRINT1( _L("Camera <> CCamAppController::CurrentMode:%d"), iInfo.iMode );
  return iInfo.iMode;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCameraMode
CCamAppController::TargetMode() const
  {
  return iInfo.iTargetMode;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
// CCamAppController::CurrentCaptureModeSetup()
TCamImageCaptureMode
CCamAppController::CurrentImageModeSetup() const
  {
  TCamImageCaptureMode captureMode = ECamImageCaptureSingle; 
  if( IsAppUiAvailable() ) 
    { 
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
    if ( appUi->IsBurstEnabled() ) 
      { 
      captureMode = appUi->CurrentBurstMode(); 
      } 
    } 
  return captureMode; 
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCaptureOperation
CCamAppController::CurrentOperation() const
  {
  return iInfo.iOperation;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCaptureOperation
CCamAppController::CurrentImageOperation() const
  {
  if( ECamControllerImage == iInfo.iMode )
    return iInfo.iOperation;
  else
    return ECamNoOperation;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCaptureOperation  
CCamAppController::CurrentVideoOperation() const
  {
  if( ECamControllerVideo == iInfo.iMode )
    return iInfo.iOperation;
  else
    return ECamNoOperation;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamImageCaptureMode  CCamAppController::CurrentImageMode() const
    {
    return iInfo.iImageMode;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TCamCameraTriState CCamAppController::ViewfinderTargetState() const
  {
  return iInfo.iTargetVfState;
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetMode( const TCamCameraMode& aNewMode )
  {
  PRINT3( _L("Camera => CCamAppController::SetMode old[%s] new[%s] target[%s]"), 
          KCamModeNames[iInfo.iMode], 
          KCamModeNames[aNewMode],
          KCamModeNames[iInfo.iTargetMode] );

  if( aNewMode != iInfo.iMode )
    {
    // In shutdown mode will not accept leaving ECamControllerShutdown state.
    TBool newModeNotAccepted = ( IsInShutdownMode() 
                              && ECamControllerShutdown == iInfo.iMode 
                              && ECamControllerShutdown != aNewMode );
    if( !newModeNotAccepted )
      {
      iInfo.iMode = aNewMode;
      NotifyControllerObservers( ECamEventEngineStateChanged );
      }
    else
      {
      PRINT( _L("Camera <> this mode change not acceptable in shutdown mode!") );
      }      
    }
  PRINT( _L("Camera <= CCamAppController::SetMode") );
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetTargetMode( const TCamCameraMode& aNewMode )
  {
  PRINT2( _L("Camera =><= CCamAppController::SetTargetMode [%s] -> [%s]"), 
          KCamModeNames[iInfo.iTargetMode],
          KCamModeNames[aNewMode         ] );

  iInfo.iTargetMode = aNewMode;
  }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetImageMode( const TCamImageCaptureMode& aNewMode )
  {
  PRINT2( _L("Camera =><= CCamAppController::SetImageMode [%s] -> [%s]"), 
          KCamImageModeNames[iInfo.iImageMode],
          KCamImageModeNames[aNewMode        ] );

  iInfo.iImageMode = aNewMode;
  // Notify ??
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetTargetImageMode( const TCamImageCaptureMode& aNewMode )
  {
  PRINT2( _L("Camera =><= CCamAppController::SetTargetImageMode [%s] -> [%s]"), 
          KCamImageModeNames[iInfo.iTargetImageMode],
          KCamImageModeNames[aNewMode              ] );

  iInfo.iTargetImageMode = aNewMode;
  }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCamAppController::SetOperation( TCamCaptureOperation aNewOperation, 
                                      TInt                 aError /*= KErrNone*/ )
  {
  PRINT2( _L("Camera => CCamAppController::SetOperation: [%s] -> [%s]"), 
          KCamCaptureOperationNames[iInfo.iOperation], 
          KCamCaptureOperationNames[aNewOperation   ] );
    PERF_OPERATION_STATE_CHANGE( aNewOperation ); 
    
    if( iInfo.iOperation != aNewOperation 
     || aError           != KErrNone ) 
      {      
      iInfo.iOperation = aNewOperation;
      NotifyControllerObservers( ECamEventOperationStateChanged, aError );
      if( aNewOperation == ECamStandby )
          {
          ClearSequenceBusyFlag( &iBusyFlags );
          TCamControllerInfo& info = const_cast<TCamControllerInfo&>( iCameraController->ControllerInfo() );
          ClearFlags( info.iBusy , ECamBusySequence );
          }
      }

  PRINT( _L("Camera <= CCamAppController::SetOperation") );
  }



// ---------------------------------------------------------------------------
// CCamAppController::ImagesRemaining
// Return the number of images that can still be saved
// ---------------------------------------------------------------------------
//
TInt CCamAppController::ImagesRemaining( TCamMediaStorage aStorage,
                                         TBool            aBurstActive )
  {
  TCamPhotoSizeId size = static_cast<TCamPhotoSizeId>
        ( iSettingsModel->IntegerSettingValue( ECamSettingItemPhotoSize ) ); 

  return ImagesRemaining( aStorage, aBurstActive, size );
  }

// ---------------------------------------------------------------------------
// CCamAppController::ImagesRemaining
// Return the number of images that can still be saved
// ---------------------------------------------------------------------------
//
TInt CCamAppController::ImagesRemaining( TCamMediaStorage aStorage,
                                         TBool            aBurstActive, 
                                         TInt             aQualityIndex )
  {
  TCamPhotoSizeId size = static_cast<TCamPhotoSizeId>
        ( iSettingsModel->PhotoResolution( aQualityIndex ) );

  return ImagesRemaining( aStorage, aBurstActive, size );
  }

// ---------------------------------------------------------------------------
// CCamAppController::ImagesRemaining
// Return the number of images that can still be saved
// ---------------------------------------------------------------------------
//
TInt CCamAppController::ImagesRemaining( TCamMediaStorage aStorage,
                                         TBool            aBurstActive, 
                                         TCamPhotoSizeId  aSize        )
  {
  PRINT( _L("Camera => CCamAppController::ImagesRemaining" ))
  if ( ECamMediaStorageCurrent == aStorage )
    {
    TCamMediaStorage store_unfiltered = static_cast<TCamMediaStorage>
        ( IntegerSettingValueUnfiltered( ECamSettingItemPhotoMediaStorage ) );    
   
    // check to see if the MMC has been removed and we are waiting on a storage switch note
    // before we start returning the values for phone memory
    if( ECamMediaStorageCard == store_unfiltered
     && static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->IsMMCRemovedNotePending() 
      )
      {
      PRINT( _L("Camera <= CCamAppController::ImagesRemaining mmc removed - returning 0" ))
      return 0;// UI Behaves as if MMC is still memory in use, thus we return zero images remaining as MMC is removed
      }                       

    // use current storage location
    aStorage = static_cast<TCamMediaStorage>
        ( IntegerSettingValue( ECamSettingItemPhotoMediaStorage ) ); 
    }
  else
    {
    // use specified location
    }
        

  TInt remaining     = 0;
  TInt criticalLevel = 0;

  PRINT( _L("Camera <> Get critical memory level.." ))
  TRAPD( err, criticalLevel = GetCriticalMemoryLevelL( aStorage ) );
  if( !err )
    remaining = iConfiguration->ImagesRemaining( aStorage, aSize, criticalLevel, aBurstActive  );
  
  // There is a factor in cenrep that defined a correction factor in percent.
  // This feature makes possible to define separate file size estimates in 
  // burst capture mode.  100 = 100%, 50 = half of single capture size etc.
  if ( aBurstActive ) 
     {
     TInt corrFactor = iConfigManager->BurstFileSizeEstimateFactor();
     PRINT1( _L("Camera <> CCamAppController::ImagesRemaining, factor: %d"), corrFactor )
     remaining = TInt( remaining * corrFactor / 100 );
     }

  PRINT1( _L("Camera <= CCamAppController::ImagesRemaining, return:%d"), remaining )
  return remaining;
  }

// ---------------------------------------------------------------------------
// CCamAppController::RecordTimeElapsed
// Return the length of video that has been recorded
// ---------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CCamAppController::RecordTimeElapsed() const
    {
    return iVideoTimeElapsed;
    }

// ---------------------------------------------------------------------------
// CCamAppController::RecordTimeElapsed
// Return the length of video that has been recorded
// ---------------------------------------------------------------------------
//
void CCamAppController::RecordTimeElapsed(TTimeIntervalMicroSeconds aElapsed )
    {
    iVideoTimeElapsed=aElapsed;
    }

// ---------------------------------------------------------------------------
// CCamAppController::RecordTimeRemaining
// Return the length of video that can still be saved
// ---------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds 
CCamAppController::RecordTimeRemaining() 
    {
    PRINT( _L("Camera => CCamAppController::RecordTimeRemaining" ));
    TTimeIntervalMicroSeconds remain( 0 );
  
    // All the time we get this information from the CaeEngine which is thru camera controller.

    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if( appUi->SettingsLaunchedFromCamera() || iDismountPending )
        {
        // In case settings plugin is being run or mmc dismount is pending
        // due to usb activation, we calculate the remaining time here, 
        // instead of repreparing the engine and getting it from there  
        TRAPD( err, iVideoTimeRemaining = CalculateVideoTimeRemainingL( static_cast < TCamMediaStorage >(CurrentVideoStorageLocation()) ) );
        if( KErrNone != err )
            {
            iVideoTimeRemaining = 0;
            }
        }
    else
        {    
        if( iInfo.iOperation == ECamCapturing    ||
            iInfo.iOperation == ECamPausing      ||
            iInfo.iOperation == ECamPaused       ||
            iInfo.iOperation == ECamResuming     ||
            iInfo.iOperation == ECamCompleting )
            {    
            iVideoTimeRemaining = RemainingVideoRecordingTime();  
            }
        else 
            {
            PRINT( _L("Camera <> CCamAppController::RecordTimeRemaining - video mode not yet initialized" ));
            TRAPD( err, iVideoTimeRemaining = CalculateVideoTimeRemainingL( static_cast < TCamMediaStorage >(CurrentVideoStorageLocation()) ) );
            if( KErrNone != err )
                {
                iVideoTimeRemaining = 0;
                }
            }
        }
   if ( ECamControllerVideo == CurrentMode() &&
            iInfo.iOperation == ECamNoOperation && 
   	    ECamMediaStorageCard == IntegerSettingValue( ECamSettingItemVideoMediaStorage ) &&
   	    appUi->IsMemoryFullOrUnavailable( ECamMediaStorageCard ) )
        {
        iVideoTimeRemaining =  0; 
        }
  
    // if the storage location is MMC but the MMC is not accessible then
    // return 0 time remaining
    if ( appUi->IsMMCRemovedNotePending() )
        {
        // Return remaining time of zero, it will be reset when
        // a card is reinserted or storage location is changed
        PRINT( _L("Camera <> MMC note pending, return 0") );
        }
    else
        {
        // If the time we have is greater than the maximum allowed, return the
        // maximum
        TTimeIntervalMicroSeconds maxRecordingLength( static_cast<TInt64>(KMaxRecordingLength) );
    
        if ( iVideoTimeRemaining > maxRecordingLength )
            {
            remain = maxRecordingLength;
            }
        else
            {
            remain = iVideoTimeRemaining;
            }
        }

    PRINT1( _L("Camera <= CCamAppController::RecordTimeRemaining, %Ld" ), remain.Int64() );
    return remain;
    }

// ---------------------------------------------------------------------------
// SetNoBurstCancel
//
//Set iNoBurstCancel flag which is used to prevent stopping burst
// when actual burst capture has started after focusing. Flag is
// used in CancelFocusAndCapture method.
// ---------------------------------------------------------------------------
//
void
CCamAppController::SetNoBurstCancel(TBool aValue )
  {
  PRINT1 ( _L("Camera =><= CCamAppController::SetNoBurstCancel, aValue=%d"),aValue );
  	iNoBurstCancel=aValue;
  }


// ---------------------------------------------------------------------------
// SoftStopBurstL
//
// Stop burst as soon as possible given the following constrains:
//   - if quick burst is ongoing, it's continued to the end, unless
//     aFastStop is true. Then the minimum of 2 images is allowed.
//     For "immediate" stop, StopSequenceCapture should be used.
//   - if press and hold burst is ongoing, stop after next image.
// ---------------------------------------------------------------------------
//
void
CCamAppController::SoftStopBurstL( TBool aFastStop /*=EFalse*/ )
  {
  PRINT ( _L("Camera => CCamAppController::SoftStopBurstL") );
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

  // Still in short burst mode, if
  // a) no burst timer at all (long burst not supported)
  // b) burst timer still ticking
  // c) in self-timer initiated burst, which is always short burst.
  TBool shortBurst = ( !iBurstTimer 
                    ||  iBurstTimer->IsActive() 
                    ||  appUi->SelfTimerEnabled() );

  PRINT1( _L("Camera <> CCamAppController::SoftStopBurstL .. is short burst: %d"), shortBurst );

  StopBurstTimer();

  if( iSequenceCaptureInProgress )
    {
    PRINT ( _L("Camera <> CCamAppController::SoftStopBurstL .. burst ongoing") );
    if( aFastStop || !shortBurst )
      {
      // Camera controller takes care of setting "high enough" limit.
      // SetCaptureLimitL takes care of setting right count to 
      // stop as early as possible.
      SetCaptureLimitL( 0 );
      }
    }
  else if( ECamFocusing != iInfo.iOperation &&
       !iAutoFocusRequested )
    {
    PRINT ( _L("Camera <> CCamAppController::SoftStopBurstL .. burst NOT ongoing, just cancel pending..") );
    iCaptureRequested = EFalse;
    }
  PRINT ( _L("Camera <= CCamAppController::SoftStopBurstL") );
  }

// ---------------------------------------------------------------------------
// StartBurstTimerL
// ---------------------------------------------------------------------------
//
void
CCamAppController::StartBurstTimerL()
  {
  PRINT ( _L("Camera => CCamAppController::StartBurstTimerL") );
  if( iBurstTimer )
    {
    StopBurstTimer();
    }
  else
    {
    TInt timeout( 0 );
    User::LeaveIfError( CamUtility::GetPsiInt( ECamPsiLongCaptureKeyPressInterval, timeout ) );

    PRINT1( _L("Camera <> CCamAppController::StartBurstTimerL .. Creating timer with timeout value of [%d ms]"), timeout );

    // Multiply timeout by 1000 to get microseconds
    TCallBack cb( CCamAppController::ShortBurstTimeout, this );
    iBurstTimer = CCamTimer::NewL( timeout*1000, cb );
    }

  PRINT ( _L("Camera <> CCamAppController::StartBurstTimerL .. Starting timer..") );
  iBurstTimer->StartTimer();

  PRINT ( _L("Camera <= CCamAppController::StartBurstTimerL") );
  }

// ---------------------------------------------------------------------------
// StopBurstTimer
// ---------------------------------------------------------------------------
//
void
CCamAppController::StopBurstTimer()
  {
  PRINT( _L("Camera => CCamAppController::StopBurstTimer") );
  if( iBurstTimer )
    {
    iBurstTimer->Cancel();
    }
  PRINT( _L("Camera <= CCamAppController::StopBurstTimer") );
  }

// ---------------------------------------------------------------------------
// ShortBurstTimeout
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::ShortBurstTimeout( TAny* aController )
  {
  PRINT( _L("Camera => CCamAppController::ShortBurstTimeout") );  

  CCamAppController* self = static_cast<CCamAppController*>( aController );
  TRAP_IGNORE( self->DoShortBurstTimeoutL() );
  
  PRINT( _L("Camera <= CCamAppController::ShortBurstTimeout") );
  return KErrNone; // no more callbacks
  }


// ---------------------------------------------------------------------------
// DoShortBurstTimeoutL
// ---------------------------------------------------------------------------
//
void
CCamAppController::DoShortBurstTimeoutL()
  {
  PRINT( _L("Camera => CCamAppController::DoShortBurstTimeoutL") );
  PRINT2( _L("Camera <> CCamAppController::DoShortBurstTimeoutL iInfo.iImageMode: %d, iSequenceCaptureInProgress: %d"), iInfo.iImageMode, iSequenceCaptureInProgress );
  if( ECamImageCaptureBurst == iInfo.iImageMode 
   && iSequenceCaptureInProgress )
    {
    TInt longBurstLimit( 0 );
    CamUtility::GetPsiInt( ECamPsiMaxBurstCapture, longBurstLimit );

    PRINT1( _L("Camera <> CCamAppController::DoShortBurstTimeoutL .. product long burst limit: %d"), longBurstLimit );  

    SetCaptureLimitL( longBurstLimit );
    }

  PRINT( _L("Camera <= CCamAppController::DoShortBurstTimeoutL") );
  }

// ---------------------------------------------------------------------------
// Actual capture limit value.
// ---------------------------------------------------------------------------
//
TInt
CCamAppController::CaptureLimit() const
  {
  TInt limit( 1 );

  if( iCameraController && ECamImageCaptureBurst == iInfo.iImageMode )
    {
    limit = iCameraController->ControllerInfo().iCaptureLimit;
    }

  return limit;
  }

// ---------------------------------------------------------------------------
// CaptureLimitSetting
//
// Stored for Setting Provider to give to Camera Controller.
// ---------------------------------------------------------------------------
//
TInt
CCamAppController::CaptureLimitSetting() const
  {
  TInt limit( 1 );

  if( ECamImageCaptureBurst == iInfo.iTargetImageMode )
    {
    limit = iRequestedCaptureCount;
    }

  return limit;  
  }

// ---------------------------------------------------------------------------
// SetCaptureLimitL
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetCaptureLimitL( TInt aLimit )
  {
  PRINT1( _L("Camera => CCamAppController::SetCaptureLimitL .. requested limit: %d"), aLimit );

  if( ECamImageCaptureBurst == iInfo.iTargetImageMode )
    {
    PRINT ( _L("Camera <> CCamAppController::SetCaptureLimitL .. burst mode active") );
    // We give the total amount of images during burst, not the remaining ones.
    // Need to subtract captures that have already been taken,
    // before comparing to the disk limit.
    if( iSequenceCaptureInProgress )
      {
      //TInt captured  = iCameraController->ControllerInfo().iCaptureCount;
      TInt captured  = iCameraController->ControllerInfo().iSnapshotCount;
      PRINT1( _L("Camera <> CCamAppController::SetCaptureLimitL .. captured already: %d"), captured );

      if( aLimit > captured )
        {
        PRINT ( _L("Camera <> CCamAppController::SetCaptureLimitL .. some more captures requested..") );

        // Check which is more restrictive - disk space or given limit.
        TInt diskLimit = ImagesRemaining( ECamMediaStorageCurrent, ETrue );
        TInt remaining = Min( aLimit - captured, diskLimit );

        PRINT1( _L("Camera <> CCamAppController::SetCaptureLimitL .. disk limit: %d"), diskLimit );

        iRequestedCaptureCount = Max( KMinBurstCount, captured + remaining );
        if ( diskLimit - KMinBurstCount < iRequestedCaptureCount ) 
            {
            // Estimated file size is not worst case estimate and long burst 
            // sequence accumulates error, thus critical disk space limit may
            // be reached. Requested capture count is decreased here if needed. 
            iRequestedCaptureCount -= 
                        (iRequestedCaptureCount+KMinBurstCount)/KBurstEstimate;
            PRINT( _L("Camera <> CCamAppController::SetCaptureLimitL .. Near to critical level, adjust iRequestedCaptureCount"));
            }
        }
      else
        {
        PRINT ( _L("Camera <> CCamAppController::SetCaptureLimitL .. capturing should be stopped as soon as possible..") );
        // Stop as fast as possible requested.
        // Still need to request minimum of 2.
        iRequestedCaptureCount = Max( KMinBurstCount, captured + 1 );
        }
      }
    // Not capturing - adjust freely
    else
      {
      TInt diskLimit = ImagesRemaining( ECamMediaStorageCurrent, ETrue );
      PRINT1( _L("Camera <> CCamAppController::SetCaptureLimitL .. disk limit: %d"), diskLimit );

      iRequestedCaptureCount = Max( KMinBurstCount, Min( aLimit, diskLimit ) );
      }

    PRINT1( _L("Camera <> CCamAppController::SetCaptureLimitL .. setting the request limit to: %d"), iRequestedCaptureCount );

    // In burst mode camera needs to be updated with the limit.
    iCameraController->DirectSettingsChangeL( ECameraSettingCaptureLimit );
    }
  else
    {
    PRINT ( _L("Camera <> CCamAppController::SetCaptureLimitL .. single / timelapse mode active") );
    TInt diskLimit = ImagesRemaining( ECamMediaStorageCurrent, EFalse );
    PRINT1( _L("Camera <> CCamAppController::SetCaptureLimitL .. disk limit: %d"), diskLimit );

    iRequestedCaptureCount = Min( aLimit, diskLimit );

    PRINT1( _L("Camera <> CCamAppController::SetCaptureLimitL .. setting the request limit to: %d"), iRequestedCaptureCount );
    }
  PRINT ( _L("Camera <= CCamAppController::SetCaptureLimitL") );
  }
// ---------------------------------------------------------------------------
// CCamAppController::SetTimeLapseInterval
// Updates the interval used in next TimeLapse capture
// ---------------------------------------------------------------------------
//
void CCamAppController::SetTimeLapseInterval( TTimeIntervalMicroSeconds aInterval )
  {
  iTimeLapseInterval = aInterval;
  }

// ---------------------------------------------------------------------------
// CCamAppController::TimeLapseInterval
// Returns the interval used in next TimeLapse capture
// ---------------------------------------------------------------------------
//   
TTimeIntervalMicroSeconds CCamAppController::TimeLapseInterval()
  {    
  return iTimeLapseInterval;
  } 
    
// ---------------------------------------------------------------------------
// CCamAppController::TimeLapseCountdown
// Returns the remaining time lapse interval until the next capture
// ---------------------------------------------------------------------------
// 
TTimeIntervalMicroSeconds CCamAppController::TimeLapseCountdown()
  {
  TInt64 remainingTime( 0 );    // Assume uninitialised start time
  
  // If the start time is uninitialised then the first capture is about to be initiated
  if ( iTimeLapseStartTime == remainingTime )
    {
    return remainingTime;
    }
  // Otherwise, work out how much time is left before the next capture
  TTime now;
  now.HomeTime();
  TTimeIntervalMicroSeconds elapsedTime = now.MicroSecondsFrom( iTimeLapseStartTime );
  remainingTime = Max ( remainingTime, iTimeLapseInterval.Int64() - elapsedTime.Int64() );   
  return remainingTime;            
  } 

// ---------------------------------------------------------------------------
// CCamAppController::StartAutoFocus
// Start the autofocus procedure
// ---------------------------------------------------------------------------
//
void CCamAppController::StartAutoFocus()
    {
    PRINT (_L("Camera => CCamAppController::StartAutoFocus"));
    PRINT1(_L("Camera <> CCamAppController::StartAutoFocus - CurrentOperation() = [%s]"), KCamCaptureOperationNames[iInfo.iOperation] );
    PRINT1(_L("Camera <> CCamAppController::StartAutoFocus - IsViewFinding()    = [%d]"), IsViewFinding() );

    if( iConfigManager 
     && iConfigManager->IsAutoFocusSupported()
     && IsViewFinding() 
     && !iAFCancelInProgress 
     && ECamNoOperation == iInfo.iOperation )
        {

        PRINT(_L("Camera <> Calling TryAutoFocus" ) );  
        TryAutoFocus();        
        }
    PRINT(_L("Camera <= CCamAppController::StartAutoFocus"));
    }

// ---------------------------------------------------------------------------
// CCamAppController::Capture
// Begin the capture procedure
// ---------------------------------------------------------------------------
//
void CCamAppController::Capture()
  {
  PRINT2( _L("Camera => CCamAppController::Capture(), operation[%s], iSaving:%d"), 
          KCamCaptureOperationNames[iInfo.iOperation], 
          iSaving );    

  // If the camera orientation changed during capture and not in burst mode, 
  // set the new orientation
  if ( iConfigManager && iConfigManager->IsOrientationSensorSupported()
       && iOrientationChangeOccured
       && iInfo.iImageMode != ECamImageCaptureBurst )
    {
    iOrientationChangeOccured = EFalse;
    TRAP_IGNORE( SetImageOrientationL() );
    }
    if( iConfigManager && iConfigManager->IsOrientationSensorSupported() && !iAccSensorListening )
        {
        // Delayed sensor initialization has not yet happened, but shutter key has been pressed.
        // Force initialization now, causing an addition to shutter lag - this is a very
        // rare case, as there should always be enough idle time to run the delayed
        // initialization.  
        TRAP_IGNORE( UpdateSensorApiL( ETrue ) );            
        }

  PERF_EVENT_END_L1( EPerfEventKeyToCapture );   

  // Set to EFalse for burst 
  iAllSnapshotsReceived = EFalse;
  
  if ( ECamFocusing == iInfo.iOperation ||
       iAFCancelInProgress ||
       iAutoFocusRequested || IsAfNeeded() )
    {
    PRINT( _L("Camera <> Focusing going on, cannot start capture - setting iCaptureRequested" ) );
    iCaptureRequested = ETrue;
    if( !IsAfNeeded() )
        {
        CancelAFNow();
        }        
    } 
  // -------------------------------------------------------
  //Quick pressed capture key after backing to precapture from postcapture in burst mode
  //Quick pressed capture key during cancelling autofocus(eg. backing to precapture from setting view ) 
  else if ( ECamNoOperation == iInfo.iOperation
    && ( ( ECamImageCaptureTimeLapse == iInfo.iImageMode ) 
      || ( ECamImageCaptureBurst == iInfo.iImageMode )
      || ( ECamImageCaptureSingle == iInfo.iImageMode ) )      
    && iAFCancelInProgress )
    {
    PRINT(_L("Camera <> Cancelling auto focus going on, cannot start capture - setting iCaptureRequested")); 
    // delay the start of capture until cancelling autofocus has finished
    iCaptureRequested = ETrue;
    }    
  // -------------------------------------------------------
  // Quick pressed after capture, during image processing 
  else if ( ECamCompleting         == iInfo.iOperation
    && ECamImageCaptureSingle == iInfo.iImageMode )
    {
    PRINT(_L("Camera <> operation state is capture completing setting iCaptureRequested")); 
     // delay the start of capture until current capture has finished
    iCaptureRequested = ETrue;
    }
  // -------------------------------------------------------
  // Ready for new shot
  else if( ECamNoOperation == iInfo.iOperation 
        || ECamFocused     == iInfo.iOperation
        || ECamFocusFailed == iInfo.iOperation )
    {
    PRINT(_L("Camera <> operation state is no operation")); 
    
    // Update current capture mode
//    iInfo.iImageMode = CurrentImageModeSetup();

    if ( iConfigManager && iConfigManager->IsAutoFocusSupported() )
        {
        // If AF sound hasn't finished playing yet, stop it now
        // to make sure it does not block capture sound playing.
        iSoundPlayer->CancelPlaying( ECamAutoFocusComplete );
        }
    
#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
    if( ECamImageCaptureBurst != iInfo.iImageMode )
      {       
      // Do not log the single image start events in burst mode, as the end
      // events would arrive for all images at once after all images have been taken
      PERF_EVENT_START_L1( EPerfEventShotToSnapshot );  
      PERF_EVENT_START_L1( EPerfEventShotToStillImageReady );  
      PERF_EVENT_START_L1( EPerfEventShotToSave );
      }
    else
      {
      PERF_EVENT_START_L1( EPerfEventSequenceCapture ); 
      }
#endif // CAMERAAPP_PERFORMANCE_MEASUREMENT

    if( ECamImageCaptureTimeLapse == iInfo.iImageMode &&
        !iSequenceCaptureInProgress )
      {
      // First image of timelapse, reset capture count  
      iTimeLapseCaptureCount = 0;
      }  

    if( iInfo.iImageMode == ECamImageCaptureNone )  
        {
        PRINT(_L("Camera <= CCamAppController::Capture() - request ignored")); 
        return;
        }

    // iCaptureRequested is reset in DoCaptureL
    TBool capture = EFalse;
    TInt err = KErrNone;
    iFilenameReserved = EFalse;
    TRAP( err, capture = DoCaptureL() );
    if ( KErrNone != err )
       {
       // Sequence capture is not in progress as capture failed 
       iSequenceCaptureInProgress = EFalse; 
       }

    PRINT1( _L("Camera <> Tried to start capture, status:%d"), capture ); 
    }
  // -------------------------------------------------------
  // Not ready for a capture
  else
    {
    // request ignored
    }  
  // -------------------------------------------------------
  PRINT(_L("Camera <= CCamAppController::Capture()"));    
  }


// ---------------------------------------------------------------------------
// CCamAppController::StopSequenceCaptureL
// Stop and complete the burst capture operation
// ---------------------------------------------------------------------------
//
void CCamAppController::StopSequenceCaptureL()
  {
  PRINT( _L("Camera => CCamAppController::StopSequenceCaptureL()") );

  iSequenceCaptureInProgress = EFalse;
  iNoBurstCancel = EFalse;

  // Re-enable screen saver
  EnableScreenSaver( ETrue );

  TCamImageCaptureMode captureModeSetup = CurrentImageModeSetup();
  // Test that this method was not called in single capture mode
  __ASSERT_DEBUG( captureModeSetup != ECamImageCaptureSingle, CamPanic( ECamPanicInvalidState ) );

  // If a burst sequence is currently active or has been requested
  if ( iInfo.iImageMode == ECamImageCaptureBurst 
    || ( captureModeSetup == ECamImageCaptureBurst && iCaptureRequested ) )
    {
    IssueDirectRequestL( ECamRequestImageCancel );
    if ( iInfo.iOperation != ECamCapturing )
      {
      CompleteBurstOperation();
      }
    
    }
  // If a timelapse sequence is currently active or has been requested        
  else if ( iInfo.iImageMode == ECamImageCaptureTimeLapse ||
            ( captureModeSetup == ECamImageCaptureTimeLapse &&
              iCaptureRequested ) )
    {
    if ( TimeLapseSupported() )
      {
      if ( iTimeLapseTimer )
        {
        iTimeLapseTimer->Cancel();
        delete iTimeLapseTimer;
        iTimeLapseTimer = NULL;
        }

      // The current state is in between captures the next one has now been cancelled
      // so just ensure that the view switch occurs           
      // if ( iInfo.iOperation != ECamCapturing && !iCaptureRequested )
      if ( iInfo.iOperation != ECamCapturing )
        {
        CompleteTimeLapseOperation();
        } 
      // Either the first capture is queued or a capture is still in stages of
      // completion. Any future capture has been cancelled so all that remains is to ensure the
      // completion code is run when the capture finally completes    
      else
        {
        iCompleteTimeLapsePending = ETrue;
        }
      }
    }
  else // Otherwise there is no active or pending burst
    {
    // Do nothing
    }

  PRINT( _L("Camera <= CCamAppController::StopSequenceCaptureL()") );
  }


// ---------------------------------------------------------------------------
// CCamAppController::CapturePending
// Whether or not a capture is pending
// ---------------------------------------------------------------------------
//
TBool CCamAppController::CapturePending() const
    {
    // iOperation can be ECamFocusing but a 
    // request to start still capture has been made and is waiting for the
    // focus operation to complete
    return iCaptureRequested;
    }

// ---------------------------------------------------------------------------
// CCamAppController::VideoRecordPending
// Whether or not video recording is pending
// ---------------------------------------------------------------------------
//
TBool CCamAppController::VideoRecordPending() const
    {
    // iOperation can be ECamFocusing but a 
    // request to start video has been made and is waiting for the
    // focus operation to complete
    return iVideoRequested;
    }

// ---------------------------------------------------------------------------
// CCamAppController::SequenceCaptureInProgress
// Whether or not sequence capture is active
// ---------------------------------------------------------------------------
//
TBool CCamAppController::SequenceCaptureInProgress() const
  {
  return iSequenceCaptureInProgress;  
  }

// -----------------------------------------------------------------------------
// CCamAppController::StartVideoRecordingL
// Begin the video recording procedure
// -----------------------------------------------------------------------------
//
void CCamAppController::StartVideoRecordingL()
  {
  PRINT( _L("Camera => CCamAppController::StartVideoRecordingL") );    
  __ASSERT_DEBUG( iCameraController, CamPanic( ECamPanicInvalidState ) );
  if ( ECamControllerVideo == iInfo.iMode )
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );  
      if ( IntegerSettingValue( ECamSettingItemStopRecordingInHdmiMode) &&
               IsHdmiCableConnected() )
          {
          appUi->HandleHdmiEventL( ECamHdmiCableConnectedBeforeRecording );
          return;
          }
      if ( !iVideoRequested
              &&  ECamNoOperation == iInfo.iOperation )
          {
          iVideoRequested = ETrue;     
      if( !iSilentProfile || iShutterSndAlwaysOn  )
          {
          // Load (and play) the start video sound	
          PlaySound( ECamVideoStartSoundId , ETrue );  
          }   
  
          // initialise the array accessing values
          iArrayUsageCount   = KVideoArrayUsers;
          iCurrentImageIndex = 0;
  
          if ( ECamMediaStorageCard == 
                   IntegerSettingValue( ECamSettingItemVideoMediaStorage ) )
              {
              TRAPD( err, ReserveFileNamesL( iInfo.iMode, ECamImageCaptureNone ) );
              if ( err )
                  {
                  PRINT( _L("Camera <> invalid MMC") );        
                  NotifyControllerObservers( ECamEventInvalidMemoryCard );
  
                  // If we have got here, we can't access MMC. Switch to phone memory
                  TRAP_IGNORE( ForceUsePhoneMemoryL() ); //with multiple drive support, 
                                                         //this actually uses the internal mass memory
                  // Ignore for now, let fail when recording.
                  TRAP_IGNORE( ReserveFileNamesL( iInfo.iMode, ECamImageCaptureNone, ETrue ) );
                  }
              }
          else
              {
              // Ignore for now, let fail when recording.
              TRAP_IGNORE( ReserveFileNamesL( iInfo.iMode, ECamImageCaptureNone ) );
              }

          if( iSilentProfile && !iShutterSndAlwaysOn )
              {
              PlaySoundComplete();
              }
          // Remember where are we recording
          iInitialVideoStorageLocation = static_cast<TCamMediaStorage>( 
               IntegerSettingValue( ECamSettingItemVideoMediaStorage ) );
          }
      else
          {
          // Video already requested or other operation busy.
          // Request ignored.
          }
      }
  // Note: The code to actually START video recording has been moved
  // to the PlaySoundComplete function so as to only start when
  // sound playback has completed.
  }

// ---------------------------------------------------------------------------
// CCamAppController::StopVideoRecording
// End the video recording procedure
// ---------------------------------------------------------------------------
//
void 
CCamAppController::StopVideoRecording()
  {
  PRINT( _L("Camera => CCamAppController::StopVideoRecording") );
  // if a video capture operation is pending
  if ( iVideoRequested )
      {
      iVideoRequested = EFalse;
      }
  // otherwise, if a video recording operation is in progress or is paused
  /*  else */
  if ( ECamCapturing == CurrentVideoOperation() 
         || ECamPaused    == CurrentVideoOperation() )
    {
    //  TRAPD( err, iAutoFocus->StopContinuousFocusL() );
    //  iFocusLocked = EFalse;
    // Keep track of the fact we are now in "saving" state
    iSaving = ETrue;
    TRAP_IGNORE( 
      {
      // IssueDirectRequestL( ECamRequestVfStop );
      StopViewFinder(); 
      IssueRequestL( ECamRequestVideoStop ); // Rest handled when event comes.
      });
    
    StartIdleTimer();
    }
  else
    {
    // otherwise there is no current recording operation
    }
  PRINT( _L("Camera <= CCamAppController::StopVideoRecording") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::StopVideoRecordingAsync
// End the video recording procedure asynchronously
// ---------------------------------------------------------------------------
//
void 
CCamAppController::StopVideoRecordingAsync()
  {
  PRINT( _L("Camera => CCamAppController::StopVideoRecordingAsync") );
  
  // Change CAE and MMF mode to async
  TRAP_IGNORE( IssueRequestL( ECamRequestSetAsyncVideoStopMode ) ); 
  StopVideoRecording();  // Call normal stop, mode is set to sync in call-backs

  PRINT( _L("Camera <= CCamAppController::StopVideoRecordingAsync") );
  }

// ---------------------------------------------------------------------------
// HandleVideoAsyncStopEvent
// ---------------------------------------------------------------------------
//
void
CCamAppController::HandleVideoAsyncStopEvent( TInt aStatus )
  {
  PRINT1( _L( "Camera => CCamAppController::HandleVideoAsyncStopEvent, status:%d" ), aStatus );
  (void)aStatus; // remove compiler warning
  
  // We DO NOT play the stop sound when recording stopped due to
  // an incoming call.  UI Spec 1.0, 4.4.1.
  if ( !iInCallOrRinging 
    && !iVideoStoppedForPhoneApp )
    {
    PlaySound( ECamVideoStopSoundId, EFalse ); // No callback
    }

  // Recording completed, so we can re-enable key sounds
  iSoundPlayer->EnableAllKeySounds();
    
  PRINT( _L( "Camera <= CCamAppController::HandleVideoAsyncStopEvent" ) );
  }

// ---------------------------------------------------------------------------
// HandleVideoStopEvent
// ---------------------------------------------------------------------------
//
void
CCamAppController::HandleVideoStopEvent( TInt aStatus )
  {
  PRINT1( _L( "Camera => CCamAppController::HandleVideoStopEvent, status:%d" ), aStatus );
    
  PERF_EVENT_END_L1( EPerfEventVideoStopToSave );        
        
  if ( KErrDiskFull   == aStatus 
    || KErrCompletion == aStatus)
    {
    // if recording ends due to disk full condition or end of
    // specified recording period do not propogate the error
    aStatus = KErrNone;
    }
    // Play sound here if async mode is not supported
    // If it is then next playing is copyed to HandleVideoAsyncStopEvent
    if ( iCameraController && !iCameraController->AsyncVideoStopModeSupported() )
        {
        // We DO NOT play the stop sound when recording stopped due to
        // an incoming call.  UI Spec 1.0, 4.4.1.
        if ( !iInCallOrRinging 
            && !iVideoStoppedForPhoneApp )
            {
            PlaySound( ECamVideoStopSoundId, EFalse ); // No callback
            }

        // Recording completed, so we can re-enable key sounds
        iSoundPlayer->EnableAllKeySounds();
        }
  // Keep track of the fact we are now leaving saving state
  iSaving = EFalse;
  
  // try closing video record to free up resources
  // Test - <eo> commented out, no such direct request supported
  // TRAP_IGNORE( IssueDirectRequestL( ECamRequestVideoRelease ) );
  
  // if video post capture is off then force re-prepare so
  // that remaining record time is updated
  // REMOVED

  // if using direct viewfinding pause viewfinder
  // REMOVED

  // report to LifeBlog
  RProperty::Set( KPSUidCamcorderNotifier, KCamLatestFilePath, iSuggestedVideoPath ); 
  // Add to album if this is enabled for videos
  TUint32 defaultAlbumId = static_cast<TUint32>( 
              IntegerSettingValue( ECamSettingItemDefaultAlbumId ));       
  if ( iSettingsModel->IntegerSettingValue
                     ( ECamSettingItemVideoStoreInAlbum ) == ECamSettYes )
    {
    iImageSaveActive->AddToAlbum( iSuggestedVideoPath, ETrue, defaultAlbumId );
    }
  else // Harvest the file but dont add to the default album
    {
    iImageSaveActive->AddToAlbum( iSuggestedVideoPath, EFalse, defaultAlbumId );
    }

  //create thumbnail
  if( iConfigManager && iConfigManager->IsThumbnailManagerAPISupported() )
      {
      TRAP_IGNORE( iImageSaveActive->CreateThumbnailsL( *BurstCaptureArray() ) );
      }
  NotifyControllerObservers( ECamEventRecordComplete,   aStatus );
  SetOperation( ECamNoOperation );
  PRINT( _L( "Camera <> calling HandleCaptureCompletion.." ) )        
  HandleCaptureCompletion();
  NotifyControllerObservers( ECamEventMediaFileChanged, aStatus );
  PRINT( _L( "Camera <= CCamAppController::HandleVideoStopEvent" ) );
  }

    
// ---------------------------------------------------------------------------
// CCamAppController::SetVideoStoppedForPhoneApp
// Store the cause of the video stopping
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetVideoStoppedForPhoneApp( TBool aIsPhoneApp )
  {
  iVideoStoppedForPhoneApp = aIsPhoneApp;
  }
    

// ---------------------------------------------------------------------------
// CCamAppController::PauseVideoRecording
// Pause the video recording procedure
// ---------------------------------------------------------------------------
//
void CCamAppController::PauseVideoRecording()
    {
    PRINT( _L("Camera => CCamAppController::PauseVideoRecording") );
    if( ECamCapturing == CurrentVideoOperation() )
        {
        TRAPD( error, IssueRequestL( ECamRequestVideoPause ) );
        if( KErrNone != error )
            {
            PRINT( _L("Camera <> PAUSING FAILED!!") );
            }
        else
            {
            // start video pause timeout
            StartIdleTimer();
            }
        }  
    PRINT( _L("Camera <= CCamAppController::PauseVideoRecording") );
    }


// ---------------------------------------------------------------------------
// CCamAppController::ContinueVideoRecording
// Continue the video recording procedure
// ---------------------------------------------------------------------------
//
void CCamAppController::ContinueVideoRecording()
  {
  PRINT( _L("Camera => CCamAppController::ContinueVideoRecording") );
  if ( ECamPaused == CurrentVideoOperation() )
    {
    SetOperation( ECamResuming );
    // Restart video when sound played
    PlaySound( ECamVideoResumeSoundId, ETrue );        
    }
  PRINT( _L("Camera <= CCamAppController::ContinueVideoRecording") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::TryAFRequest
// issue AF request if there are no pending AF requests currently
// ---------------------------------------------------------------------------
//
TBool CCamAppController::TryAFRequest( TInt aAFRequest ) 
  {
  PRINT( _L( "Camera => TryAFRequest") );
  PRINT3( _L( "iPendingAFRequest=%d aAFRequest=%d ,iCurrentAFRequest=%d"), iPendingAFRequest,
    aAFRequest, iCurrentAFRequest );
  switch( aAFRequest )
    {    
    case  ECamRequestCancelAutofocus:
    case ECamRequestStartAutofocus:
        {
        if( iPendingAFRequest == 0 ||
          ( !iAFCancelInProgress &&
            iPendingAFRequest == ECamRequestCancelAutofocus &&
            iCurrentAFRequest == ECamRequestCancelAutofocus ) )
            {
            if ( !( UiConfigManagerPtr()->IsContinuosAutofocusSupported()&& ECamControllerVideo == CurrentMode() ) ) 
                {
                iPendingAFRequest=aAFRequest;
                TRAPD( err, IssueDirectRequestL( TCamCameraRequestId(aAFRequest) ) );
                //TRAPD( err, iCameraController->DirectRequestL( aAFRequest ) );            
                if ( err != KErrNone )
                    {
                    // There is an error, we leave without focusing
                    iPendingAFRequest=0;
                    return EFalse; // Not doing any autofocus request.
                    //This is only case where AFrequest cause EFalse to be returned.
                    }
                }
            } 
        else
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
            if ( appUi->AlwaysDrawPreCaptureCourtesyUI()
                && !CurrentSceneHasForcedFocus() )
                {
                appUi->SetActivePaletteVisibility( EFalse );
                }
            iPendingAFRequest=aAFRequest;
            }
        }
        break;
    default:
        {
        //Not type of autofocus request. Ignoring.
        }
        break;    
    }
  PRINT( _L( "Camera <= TryAFRequest") );
  return ETrue;              
  }

// ---------------------------------------------------------------------------
// CCamAppController::CancelAFNow
// Cancel AutoFocus 
// ---------------------------------------------------------------------------
//
void CCamAppController::CancelAFNow()
  {    
  PRINT( _L("Camera => CCamAppController::CancelAfNow()") );
  
  if( !iAFCancelInProgress && ECamFocusing == CurrentOperation() ) 
    {
    TRAPD( err, IssueDirectRequestL( ECamRequestCancelAutofocus ) );          
    if( KErrNone == err ) 
      {
      iAFCancelInProgress = ETrue;  
      iCurrentAFRequest = ECamRequestCancelAutofocus;      
      iPendingAFRequest = ECamRequestCancelAutofocus;
      }
    }
  else 
    {
    PRINT( _L("AF Cancel already in progress or not focusing") );
    }   
    
  PRINT( _L("Camera <= CCamAppController::CancelAfNow()") );
  }
// CCamAppController::CancelFocusAndCapture
// Cancel the focus and capture procedures
// ---------------------------------------------------------------------------
//
void CCamAppController::CancelFocusAndCapture()
  {
  PRINT( _L( "Camera => CCamAppController::CancelFocusAndCapture") );            
  PRINT1( _L("Camera <> CCamAppController::CancelFocusAndCapture - CurrentOperation() = [%s]"), KCamCaptureOperationNames[iInfo.iOperation] );
  PRINT1( _L("Camera <> CCamAppController::CancelFocusAndCapture - IsViewFinding()    = [%d]"), IsViewFinding() );
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
  // Only cancels autofocus if needed, burst capture is no longer
  // stopped if capture key pressed during burst
  
  if( !iNoBurstCancel 
   && ECamNoOperation != CurrentOperation()
   && ECamCompleting  != CurrentOperation() )
    {  
    // If we are currently focused, cancel autofocus
    if ( IsViewFinding() && CurrentOperation() != ECamCapturing &&  // Cannot do AF operations if VF not on. AF is anyway cancelled on VF start event.
        iCurrentAFRequest != ECamRequestCancelAutofocus ) // Don't cancel twice
      {
      if( ECamFocusing == CurrentOperation() )
        {
        CancelAFNow();
        if ( IsTouchScreenSupported() )
            {
            CAknToolbar* fixedToolbar = appUi->CurrentFixedToolbar();
            if ( fixedToolbar )
              {
              fixedToolbar->SetToolbarVisibility( ETrue );
              }        
            }
        }   
      else 
        {
        TryAFRequest( ECamRequestCancelAutofocus );
        }
      }
    iCaptureRequested = EFalse;
    }
  PRINT( _L( "Camera <= CCamAppController::CancelFocusAndCapture") );            
  }


// ---------------------------------------------------------------------------
// CCamAppController::SnapshotImage
// Get post-exposure snapshot bitmap, if available.
//
// Returns: pointer to post-exposure snapshot bitmap. Does not transfer ownership.
//          May be NULL if no image available.
// 
// ---------------------------------------------------------------------------
//
const CFbsBitmap* 
CCamAppController::SnapshotImage()
  {
  PRINT ( _L("Camera => CCamAppController::SnapshotImage") ); 
  PRINT1( _L("Camera <> CCamAppController::SnapshotImage .. current image index: %d"), iCurrentImageIndex ); 
  PRINT1( _L("Camera => CCamAppController::SnapshotImage .. saved current image: %d"), SavedCurrentImage() ); 

  PRINT1( _L("Camera <> CCamAppController::SnapshotImage - iSnapshotRedrawNeeded:%d"), iSnapshotRedrawNeeded );
  if ( iSnapShotRotator->IsActive() )
    {
    // Avoid flickering. Do not show original snapshot, if it needs to be rotated
    // Snapshot might need a separate draw if rotation takes long time 
    iSnapshotRedrawNeeded = ETrue;
    PRINT( _L("Camera <= CCamAppController::SnapshotImage - return null") );
    return NULL;
    }
  iSnapshotRedrawNeeded = EFalse;

  //Sometime burst capture array includes more than one image in single capture mode, 
  //so just display the latest image here.
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
  if ( BurstCaptureArray()->Count() > 1 && !appUi->IsBurstEnabled() )
      {
      TInt currentImageIndex = BurstCaptureArray()->Count() - 1;
      const CFbsBitmap* ss = BurstCaptureArray()->Snapshot( currentImageIndex );
      PRINT( _L("Camera <= CCamAppController::SnapshotImage") ); 
      return ss;
      }

  const CFbsBitmap* ss = BurstCaptureArray()->Snapshot( iCurrentImageIndex );
  PRINT( _L("Camera <= CCamAppController::SnapshotImage") ); 
  return ss;
  }


// ---------------------------------------------------------------------------
// CCamAppController::SetAsCurrentImage
// Sets the specified index as the "current image", as will be used by the 
// post capture view
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetAsCurrentImage( TInt aIndex )
  {
  iCurrentImageIndex = aIndex;
  }

// ---------------------------------------------------------------------------
// CCamAppController::CurrentImageName
// Get the user-visible name for the current image 
// used in post-capture views.
// ---------------------------------------------------------------------------
//
const TDesC& 
CCamAppController::CurrentImageName() const
  {
  return BurstCaptureArray()->ImageName( iCurrentImageIndex );
  }

// ---------------------------------------------------------------------------
// CCamAppController::BurstCaptureArray
// Return a pointer to the array representing a burst capture
// ---------------------------------------------------------------------------
//
CCamBurstCaptureArray* 
CCamAppController::BurstCaptureArray() const
  {
  return iCaptureArray;
  }        

// ---------------------------------------------------------------------------
// CCamAppController::CurrentItemCount
// Return the count of non-deleted items in the burst array
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::CurrentItemCount()
  {
  if ( !BurstCaptureArray() )
    {
    return 0;
    }
  return BurstCaptureArray()->ImagesRemaining();
  }

// ---------------------------------------------------------------------------
// CCamAppController::CurrentCapturedCount
// Return the count of burst capture moments that have taken
// place in a current burst
// ---------------------------------------------------------------------------
//    
TInt 
CCamAppController::CurrentCapturedCount()
  {
  //return iActualBurstCount;
  TInt count( 0 );
  if( iCameraController )
    {
    // Using snapshot as count, because UI updates counter
    // on snapshot events.
    count = iCameraController->ControllerInfo().iSnapshotCount;
    }
  return count;  
  }

// ---------------------------------------------------------------------------
// CCamAppController::TimeLapseImageCount
// Return the count of items in the timelapse sequence array
// ---------------------------------------------------------------------------
//    
TInt 
CCamAppController::TimeLapseImageCount()
  {
  return iTimeLapseCaptureCount;
  }

// ---------------------------------------------------------------------------
// CCamAppController::ResetTimelapseArray
// Resets the timelapse sequence array
// ---------------------------------------------------------------------------
//    
void CCamAppController::ResetTimelapseArray()
  {
  iSequenceFilenameArray->Reset();
  }

// ---------------------------------------------------------------------------
// CCamAppController::PlaySound
// Play a sound with given id.
// ---------------------------------------------------------------------------
//
void CCamAppController::PlaySound( TCamSoundId aSoundId, TBool aEnableCallback )
    {
    PRINT1( _L("Camera => CCamAppController::PlaySound %d"), aSoundId );
    if ( ToneShouldBeSilent( aSoundId ) &&
         !iShutterSndAlwaysOn && 
         iSilentProfile )
        {
        PRINT( _L("Camera <> Profile silent, do not play sound") );  
        // Don't play shutter sound if we are following
        // current profile setting and profile is silent.
        // Notify the observer right away. E.g. video start
        // depends on the notification
        if( aEnableCallback )
          {
          PlayComplete( aSoundId, KErrNone );
          }
        /* OLD
        if ( aSoundId == ECamVideoStartSoundId ||
             aSoundId == ECamVideoPauseSoundId ||
             aSoundId == ECamVideoResumeSoundId )
            {
            // Force the callback to start recording without sound.
            PlaySoundComplete();
            }
        */
        }
    else
        {
        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_PLAYSOUND, "e_CAM_APP_CAPTURE_SOUND_PLAY 1" );   //CCORAPP_CAPTURE_SOUND_PLAY_START
        iSoundPlayer->CancelAllPlaying();
        iSoundPlayer->PlaySound( aSoundId, aEnableCallback ); 

        }
        
    // If in timelapse mode the remaining time counter should be updated
    // as soon as the capture tone has played, but as there is no callback
    // this is the closest we can get            
    if ( ECamImageCaptureTimeLapse == iInfo.iImageMode )  
        {
        NotifyControllerObservers( ECamEventCounterUpdated );   
        }       
        
    PRINT( _L("Camera <= CCamAppController::PlaySound") );                 
    }


// ---------------------------------------------------------------------------
// CCamAppController::AllOptionsVisibleForSettingItem
// Returns true if all the settings list options for a particular setting
// item are visible.
// ---------------------------------------------------------------------------
//
TBool CCamAppController::AllOptionsVisibleForSettingItem( TInt aSettingItem, 
                                                    TInt& aRevisedResourceId )
  {
  if ( ( aSettingItem == ECamSettingItemVideoQuality ) && 
       ( iInfo.iActiveCamera == ECamActiveCameraSecondary ) )
    {
    aRevisedResourceId = ROID(R_CAM_FRONT_CAMERA_VIDEO_QUALITY_TEXT_ARRAY);
    return EFalse;
    }
  // Otherwise, all options are visible for the setting item
  return ETrue;
  }

// ----------------------------------------------------
// CCamAppController::SwitchCameraL
// Switch the camera
// ----------------------------------------------------
//
void CCamAppController::SwitchCameraL()
  {
  PRINT( _L("Camera => CCamAppController::SwitchCameraL") );

  TCamAppControllerInfo oldInfo      = iInfo;
  TCamActiveCamera      targetCamera = ECamActiveCameraNone;
  // If something goes wrong when changing from 2nd to 1st camera,
  // we need to make sure, that 2nd cam settings are not stored 
  // to CR by accident.
  iInfo = TCamAppControllerInfo(); // reset
  
  // reset zoom waiting flag. this flag is only used with bitmap viewfinder
  // and it might cause problems if one camera uses bitmap viewfinder and the
  // other one uses direct viewfinder
  iZoomWaitingForCamera = EFalse;

  CancelPreviewChangesL();

  if( ECamActiveCameraPrimary == oldInfo.iActiveCamera )
    {
    // Currently using primary camera, switch to secondary
    PRINT( _L("Camera <> switch to 2nd cam") );
 
#ifdef _DEBUG
    PRINT ( _L("Camera <> ======================================") );
    PRINT ( _L("Camera <> Photo quality index:                  ") );
    PRINT1( _L("Camera <> Before loading 2nd cam settings: %d   "), IntegerSettingValue( ECamSettingItemPhotoQuality ) );
#endif
    
    // Scene mode is forced to Automatic while secondary camera is in use.
    iSceneModeForcedBySecondaryCamera = ETrue;

    LoadSecondaryCameraSettingsL();
  
#ifdef _DEBUG
    PRINT1( _L("Camera <> After loading 2nd cam settings:  %d   "), IntegerSettingValue( ECamSettingItemPhotoQuality ) );
    PRINT ( _L("Camera <> ======================================") );
#endif

    targetCamera = ECamActiveCameraSecondary;
    }
  else
    {
    // Currently using secondary camera, switch to primary
    // or landscape secondary camera  
    PRINT( _L("Camera <> switch to 1st camera or change 2nd camera mode") )
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
    if( appUi->IsEmbedded() )
        {
        LoadStaticSettingsL( appUi->IsEmbedded() );
        }
    else if ( CameraSwitchQueued() != ESwitchSecondaryOrientation )
        {
        iSettingsModel->RestorePrimaryCameraSettingsL();
        }

    if ( CameraSwitchQueued() != ESwitchSecondaryOrientation )
        {
        PRINT( _L("Camera <> switch to 1st cam") )
        targetCamera = ECamActiveCameraPrimary;
        
        // Primary camera will use its previously selected scene mode after camera switch.
        iSceneModeForcedBySecondaryCamera = EFalse;
        }
    else
        {
        PRINT( _L("Camera <> change 2nd camera mode") )
        targetCamera = ECamActiveCameraSecondary;
        }
    }	            
  SetCameraSwitchRequired( ESwitchDone );  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  iCameraController->SwitchCameraL( (ECamActiveCameraPrimary == targetCamera) ? 0 : 1 );
  if( UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
      {
      RArray<TInt> screenModeValues;
      CleanupClosePushL( screenModeValues );
      UiConfigManagerPtr()->SupportedScreenModesL( screenModeValues );
      TInt landscapeScreenMode = screenModeValues[0];
      SetCameraOrientationModeL( landscapeScreenMode );
      CleanupStack::PopAndDestroy( &screenModeValues );
      }
  iCameraController->CompleteSwitchCameraL();
  // Force to get a sensor data after switch camera from primary to secondary 
  // when always holding in camera with portrait mode. 
  if( iConfigManager 
      && iConfigManager->IsOrientationSensorSupported() )
    {
    if( iAccSensorListening )
      {
      iAccSensorListening = EFalse;
      }
    if( iAccSensorChannel )
      {
      delete iAccSensorChannel;
      iAccSensorChannel = NULL;
      }        
    TRAP_IGNORE( UpdateSensorApiL( ETrue ) );            
    }

  // Camera switched.
  // a) Set current camera index to the new one.
  // b) Set target mode (video/image) to the one before mode switch.
  // c) Image mode is single shot after camera switch
  // d) Ensure vf start
  iInfo.iActiveCamera    = targetCamera; // a)
  iInfo.iTargetMode      = appUi->TargetMode(); // b)
  iInfo.iTargetImageMode = ( ECamControllerVideo==iInfo.iTargetMode )
                         ? ECamImageCaptureNone : ECamImageCaptureSingle; // c)
  iInfo.iTargetVfState   = ECamTriActive; // d)
  if( oldInfo.iMode != ECamControllerShutdown )
  	{
  	IssueModeChangeSequenceL();
  	}
  	
  appUi->SetDrawPreCaptureCourtesyUI( ETrue );

  NotifyControllerObservers( ECamEventCameraChanged );

  PRINT( _L("Camera <= CCamAppController::SwitchCameraL") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::ActiveCamera
// Returns the active camera
// ---------------------------------------------------------------------------
//
TCamActiveCamera 
CCamAppController::ActiveCamera() const
  {
  return iInfo.iActiveCamera;
  }

// ---------------------------------------------------------------------------
// CCamAppController::CameraSwitchQueued
// Returns whether a camera switch is pending
// ---------------------------------------------------------------------------
//
TCameraSwitchRequired 
CCamAppController::CameraSwitchQueued() const   
  {
  PRINT1( _L("Camera =><= CCamAppController::CameraSwitchQueued %d"), iCameraSwitchRequired )
  return iCameraSwitchRequired;
  }

// ---------------------------------------------------------------------------
// CCamAppController::CheckExitStatus
// Check whether exit is required
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::CheckExitStatus()
  {
  PRINT( _L("Camera => CCamAppController::CheckExitStatus" ) )
  TBool exit = EFalse;
  if ( iEndKeyWaiting )
      {
      exit = ETrue;
      }   
  else if ( iConfigManager  && iConfigManager->IsLensCoverSupported() )
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

      if ( iSliderCloseEvent && !appUi->IsEmbedded() && !appUi->Embedding() )
          {
          PRINT( _L("Camera <> CCamAppController::CheckExitStatus - slider closed, not embedded/embedding" ) )
          exit = ETrue;
          }
      else
          {
          PRINT( _L("Camera <> CCamAppController::CheckExitStatus - embedded/embedding, not exiting" ) )
          exit = EFalse;
          }
      }
  else
      {
      exit = EFalse;
      }

  PRINT1( _L("Camera <= CCamAppController::CheckExitStatus - return %d" ), exit )
  return exit;
  }
    
// ---------------------------------------------------------------------------
// CCamAppController::SetEndKeyExitWaiting
// Update whether exit is required
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetEndKeyExitWaiting( TBool aEndKeyWaiting )
  {
  iEndKeyWaiting = aEndKeyWaiting;
  }    

// ---------------------------------------------------------------------------
// CCamAppController::CameraSlideState
// Returns the status of the camera slide
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::CameraSlideState() const
  {
  return iSlideState;
  }

// ---------------------------------------------------------------------------
// CCamAppController::SliderCloseEventActioned
// Update whether exit is required
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SliderCloseEventActioned()
  {
  iSliderCloseEvent = EFalse;
  }   

// ---------------------------------------------------------------------------
// CCamAppController::SetCameraSwitchRequired
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetCameraSwitchRequired( TCameraSwitchRequired aSwitchRequired )  
  {
  iCameraSwitchRequired = aSwitchRequired;
  }   


// ---------------------------------------------------------------------------
// CCamAppController::CheckSlideStatus
// Check the slide state and schedule a camera switch if needed
// ---------------------------------------------------------------------------
//
void CCamAppController::CheckSlideStatus()
  {
  PRINT( _L("Camera => CCamAppController::CheckSlideStatus") );
  // if only 1 camera available then can't switch camera
  if ( CamerasAvailable() <= 1 )
    {
    PRINT( _L("Camera <> Only one camera, no action") );
    }
  else
    {
    RefreshSlideStatus();
    // if the slide is closed then schedule a camera switch
    // if the back camera is active
    if ( CameraSlideState() == CameraPlatPSKeys::EClosed  || 
         CameraSlideState() == KErrNone )
      {
      if ( iInfo.iActiveCamera == ECamActiveCameraPrimary )
        {
        PRINT( _L("Camera <> slide closed or uninitialized -> switch to secondary") );
        iCameraSwitchRequired = ESwitchPrimaryToSecondary;
        }
      else
        {
        PRINT( _L("Camera <> no camera switch needed") );
        iCameraSwitchRequired = ESwitchDone;
        }
      }
    // if the slide is open then schedule a camera switch
    // if the front camera is active
    else if ( CameraSlideState() == CameraPlatPSKeys::EOpen )
      {
      if ( iInfo.iActiveCamera == ECamActiveCameraSecondary )
        {
        PRINT( _L("Camera <> slide open -> switch to primary") );
        iCameraSwitchRequired = ESwitchSecondaryToPrimary;
        }
      else
        {
        PRINT( _L("Camera <> no camera switch needed") );
        iCameraSwitchRequired = ESwitchDone;
        }
      }
    else
      {
      // do nothing
      PRINT( _L("Camera <> not recognized slider state -> no action") );
      }
    }
  PRINT( _L("Camera <= CCamAppController::CheckSlideStatus") );
  }


// ----------------------------------------------------
// CCamAppController::EvRange
// Range of EV value supported by current product
// ----------------------------------------------------
//
TCamEvCompRange CCamAppController::EvRange() const
  {
  PRINT( _L("Camera =><= CCamAppController::EvRange") );
  return iEvRange;
  }            


// ----------------------------------------------------
// CCamAppController::CallStateChanged
// Callback for when the current phone call state changes
// ----------------------------------------------------
//
void 
CCamAppController::CallStateChanged( TPSCTsyCallState aState, 
                                     TInt                  /* aError */ )
  {
  PRINT1( _L("Camera => CCamAppController::CallStateChanged aState: %d"), aState);

  TBool oldState = iInCallOrRinging;

  if ( aState == EPSCTsyCallStateAlerting ||
       aState == EPSCTsyCallStateRinging ||
       aState == EPSCTsyCallStateDialling ||
       aState == EPSCTsyCallStateAnswering ||
       aState == EPSCTsyCallStateDisconnecting ||
       aState == EPSCTsyCallStateConnected ||
       aState == EPSCTsyCallStateHold )
    {
    // If not already in a call/ringing state, and the new state
    // indicates this, stop recording
    if ( !iInCallOrRinging )
      {
      // Set the flag before handle call, as this may need to 
      // know the call state for stopping video.
      iInCallOrRinging = ETrue;
      PRINT( _L("Camera <> CCamAppController::CallStateChanged: calling handleincomingcallL") );
      HandleIncomingCall();                                     
      }
    }
  else
    {
    iInCallOrRinging = EFalse;
    }
  // If the state has changed from in a call to not, notify the
  // observers
  // Also notify again if the state has reached connected so that
  // the observer can reconsider its actions now that connection is complete
  if ( iInCallOrRinging               != oldState ||
       EPSCTsyCallStateConnected == aState )
    {
    PRINT( _L("Camera <> CCamAppController::CallStateChanged: telling call observers") );
    NotifyControllerObservers( ECamEventCallStateChanged, KErrNone );        
    }

  PRINT1( _L("Camera <= CCamAppController::CallStateChanged(%d, %d)"), aState );
  }






// ---------------------------------------------------------------------------
// ReleaseCamera
//
// Releases the camera hardware for other apps to use
// ---------------------------------------------------------------------------
//
void CCamAppController::ReleaseCamera()   
    {    
    PRINT( _L("Camera => CCamAppController::ReleaseCamera") );

    if ( IsAppUiAvailable())  
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if ( appUi )
        {
        appUi->SetAssumePostCaptureView( EFalse ); 
        } 
      }
                                                                                         
    if ( Busy() )
      {
      PRINT( _L("Camera <> CCamAppController::ReleaseCamera: set release pending") );
      iPendingRelease = ETrue;
      }
    else if ( iCameraController && ECamCameraIdle != CameraState() )
      {      
      // No leaving code here. Clear the flag before any operation
      // to prevent calling this from the event callbacks caused
      // by the following requests.
      iPendingRelease = EFalse;

      if( iInfo.iImageMode == ECamImageCaptureBurst &&
          SequenceCaptureInProgress() )
        {
        // Cancel ongoing burst capture
        // Burst capture is normally cancelled by setting iRequestedBurstCount,
        // and actual cancelling is done when next imagedata arrives.
        // However, in this case camera is going to be released, and there won't
        // be next image data, so give ECamRequestImageCancel and capture
        // completion event here to avoid problems
        PRINT( _L("Camera <> CCamAppController::ReleaseCamera: Going to bacgkround in the middle of a burst capture") ); 
        TRAP_IGNORE ( IssueDirectRequestL( ECamRequestImageCancel ) );
        NotifyControllerObservers( ECamEventCaptureComplete, KErrCancel );          
        }
      //In SetOperation(), it notify observer with ECamEventOperationStateChanged, 
      //the clear work for flash indicator flag can be done with ECamEventOperationStateChanged in some abnormal cases.
      if ( ECamControllerImage == CurrentMode() && iInfo.iActiveCamera == ECamActiveCameraPrimary )
        {             
        SetOperation( ECamNoOperation );
        }

        
      //iInfo.iMode            = ECamControllerShutdown;
      //iInfo.iTargetMode      = ECamControllerIdle;
      iInfo.iTargetMode      = ECamControllerIdle;
      iInfo.iImageMode       = ECamImageCaptureNone;
      iInfo.iTargetImageMode = ECamImageCaptureNone; 
      iInfo.iOperation       = ECamNoOperation;

      iCaptureModeTransitionInProgress = EFalse;

      // Last change to stop view finder if we were "busy" previously in 
      // ExitViewfinderMode method. View finder must be stopped in case where 
      // orientation is changed, so Posting API display control doesn't try to 
      // draw outside existing area. Otherwise KErrTooBig error is encountered. 
      StopViewFinder();

      PRINT( _L("Camera <> CCamAppController::ReleaseCamera: requesting release directly..") );
      // iInfo.iMode set when complete
      TRAP_IGNORE( IssueDirectRequestL( ECamRequestRelease ) );
      //iPendingRelease = EFalse;
      //shutdown the orientation channel
      if( iConfigManager && iConfigManager->IsOrientationSensorSupported() && 
      		iAccSensorListening )
        {
        TRAP_IGNORE( UpdateSensorApiL( EFalse ) );
        }
      }
    else
      {
      // No action needed.
      PRINT( _L("Camera <> CCamAppController::ReleaseCamera: camera already idle!!") );
      if(ECamControllerIdle != iInfo.iMode )
        {
        PRINT( _L("Camera <> CCamAppController::ReleaseCamera: camera already idle correcting state!!") );
        iInfo.iMode = ECamControllerIdle;
        }
      }
    PRINT( _L("Camera <= CCamAppController::ReleaseCamera") );
    }


// ---------------------------------------------------------------------------
// IncCameraUsers
//
// Increments the camera usage count
// ---------------------------------------------------------------------------
//
void CCamAppController::IncCameraUsers()
  {
  PRINT ( _L("Camera => CCamAppController::IncCameraUsers") );
  iCameraReferences++;
  PRINT1( _L("Camera <= CCamAppController::IncCameraUsers, count now %d"), iCameraReferences );
  }


// ---------------------------------------------------------------------------
// DecCameraUsers
//
// Decrements the camera usage count
// ---------------------------------------------------------------------------
//
void CCamAppController::DecCameraUsers()
  {
  PRINT ( _L("Camera => CCamAppController::DecCameraUsers") );

  __ASSERT_DEBUG( iCameraReferences > 0, CamPanic( ECamPanicInvalidState ) );
  iCameraReferences--;

  PRINT1( _L("Camera <> CCamAppController::DecCameraUsers: count now %d"), iCameraReferences );
  // If the engine is no longer required then release it.
  if ( iCameraReferences <= 0 )
    {
    ReleaseCamera();
    }       
  PRINT ( _L("Camera <= CCamAppController::DecCameraUsers") );
  }


    

// ----------------------------------------------------
// CCamAppController::InCallOrRinging
// Whether we are in call or ringing
// ----------------------------------------------------
//
TBool 
CCamAppController::InCallOrRinging() const
  {
  return iInCallOrRinging;
  }

// ----------------------------------------------------
// CCamAppController::InVideocallOrRinging
// Whether we are in videocall or ringing
// ----------------------------------------------------
//
TBool 
CCamAppController::InVideocallOrRinging()
    {
    if ( InCallOrRinging() )
        {
        TInt callType( EPSCTsyCallTypeUninitialized );
        TInt err = RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType, callType );
        if ( err == KErrNone && callType == EPSCTsyCallTypeH324Multimedia )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        return EFalse;
        }
    
    }
  
// ----------------------------------------------------
// CCamAppController::CurrentlySavingVideo
// Whether we are currently saving a video clip or not
// ----------------------------------------------------
//
TBool 
CCamAppController::CurrentlySavingVideo() const
  {
  return iSaving;
  }
        


// ---------------------------------------------------------------------------
// IssueModeChangeSequenceL
// ---------------------------------------------------------------------------
//
void
CCamAppController::IssueModeChangeSequenceL( TBool aStartup )
  {
  PRINT( _L( "Camera => CCamAppController::IssueModeChangeSequenceL" ) );
  // Generate the request sequence and issue to Camera Controller.
  RCamRequestArray sequence;
  CleanupClosePushL( sequence );

  if( !aStartup )
    {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    TVwsViewId activeView;
    (void) appUi->GetActiveViewId( activeView ); // ignore error
    
    if(IntegerSettingValue(ECamSettingItemRemovePhoneMemoryUsage) &&
            !IsMemoryAvailable(ECamMediaStorageMassStorage) &&
            !IsMemoryAvailable(ECamMediaStorageCard) &&
            ( appUi->PreCaptureMode() == ECamPreCapViewfinder ||
              appUi->PreCaptureMode() == ECamPreCapGenericSetting ) &&
              activeView.iViewUid.iUid != ECamViewIdPhotoUserSceneSetup )
        {
        TBool usbPersonality = 0;
        #ifndef __WINSCW__
        User::LeaveIfError ( RProperty::Get (KPSUidUsbWatcher, 
                          KUsbWatcherSelectedPersonality, usbPersonality));
        #endif // __WINSCW__     
        TInt mmcInserted = 0;
        User::LeaveIfError( RProperty::Get( KPSUidUikon, KUikMMCInserted, mmcInserted ) );

        if( KUsbPersonalityIdMS == usbPersonality )
            {
            SwitchToStandbyL( ECamErrMassStorageMode );
            }
        else if( !mmcInserted )
            {
            SwitchToStandbyL( ECamErrMemoryCardNotInserted );
            }
        else
            {
            // Memory card and mass storage not accessible. Storage may be locked or corrupted.
            PRINT1( _L("Camera <> Storage is locked/corrupted. USB personality:%d"), usbPersonality );
            SwitchToStandbyL( ECamErrMassStorageMode );
            }
        
        iIssueModeChangeSequenceSucceeded = EFalse;
        ClearSequenceBusyFlag( &iBusyFlags );
        }
    else
        {
        GenerateModeChangeSequenceL( sequence );
        
        if( iCameraReferences ) 
            {
            SetFlags( iBusyFlags, EBusySequence );
            TCleanupItem clearBusy( ClearSequenceBusyFlag, &iBusyFlags );
            CleanupStack::PushL( clearBusy );
            iCameraController->RequestSequenceL( sequence );
            CleanupStack::Pop();
            
            iCaptureModeTransitionInProgress = ETrue;
            
            }
        else 
            {
            PRINT( _L("Camera <> No camera users, not requesting sequence.") );
            }
        iIssueModeChangeSequenceSucceeded = ETrue;
        }  
    }
  else
    {
    GenerateStartupSequenceL( sequence );
	
	SetFlags( iBusyFlags, EBusySequence );
    TCleanupItem clearBusy( ClearSequenceBusyFlag, &iBusyFlags );
    CleanupStack::PushL( clearBusy );
    iCameraController->RequestSequenceL( sequence );
    CleanupStack::Pop();
    iIssueModeChangeSequenceSucceeded = ETrue;
    }
  
  CleanupStack::PopAndDestroy(); // sequence.Close()
  PRINT( _L( "Camera <= CCamAppController::IssueModeChangeSequenceL" ) );
  }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void
CCamAppController::GenerateModeChangeSequenceL( RCamRequestArray& aSequence )
  {
  PRINT( _L( "Camera => CCamAppController::GenerateModeChangeSequenceL" ) );
  TBool vfStartDone = EFalse;
  //Have to empty autofocusrequest values here because modechange could override
  //currently processed autofocus request.
  iPendingAFRequest=0;
  iCurrentAFRequest=0;
  iNoBurstCancel = EFalse;
  iKeyUP = EFalse;
  iAFCancelInProgress = EFalse;
  iTouchCapture = EFalse;

  // -------------------------------------------------------
  if( ECamTriIdle != iCameraController->ViewfinderState() )
    {
    // If VF is not stopped, frames might come when we are not
    // in either image or video mode.
    if( ECamControllerIdle != iInfo.iMode 
     && iInfo.iTargetMode  != iInfo.iMode )    
      {
      aSequence.Append( ECamRequestVfRelease );    
      } 
    else  
      
    if( ECamTriIdle == iInfo.iTargetVfState )
      {
      aSequence.Append( ECamRequestVfRelease );      
      }
    else if( ECamTriInactive == iInfo.iTargetVfState )
      {
      aSequence.Append( ECamRequestVfStop );      
      }
    else
      {
      // Nothing needed here
      }

    }

  switch( iInfo.iMode )
    {
    // ---------------------------------
    case ECamControllerImage:
      {
      if( ECamControllerImage != iInfo.iTargetMode )
        {
        aSequence.Append( ECamRequestImageRelease );
        }
      break;
      }
    // ---------------------------------
    case ECamControllerVideo:
      {
      if( ECamControllerVideo != iInfo.iTargetMode )
        {
        aSequence.Append( ECamRequestVideoRelease );
        }
      break;
      }
    // ---------------------------------
    default:
      {
      // If we need to get to video or image mode
      // make sure camera has power on before further requests.
      if( ECamControllerImage == iInfo.iTargetMode
       || ECamControllerVideo == iInfo.iTargetMode )
        {
        TUint cameraState( iCameraController->State() );

        if( !(ECamPowerOn&cameraState) )
          {
          if( !(ECamReserved&cameraState) )
            {
            aSequence.Append( ECamRequestReserve );
            }
          aSequence.Append( ECamRequestPowerOn );
          }
        }
      break;
      }
    // ---------------------------------
    }

  // -------------------------------------------------------
  // Append prepare operations to achieve target camera mode
  switch( iInfo.iTargetMode )
    {
    // ---------------------------------
    case ECamControllerImage:
      {
      if( ECamImageCaptureNone != iInfo.iTargetImageMode )
        {
        aSequence.Append( ECamRequestImageInit );      

        // Snapshot may have to be released and restarted because the
        // snapshot size may change.
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        if( appUi )
            {
            if( appUi->ShowPostCaptureView() || iInfo.iTargetImageMode == ECamImageCaptureBurst )
                {
                // If vf needs to be (re)started.
                // Camera Controller checks from Setting Provider if
                // viewfinder mode (direct/bitmap) has changed, and
                // releases old type first if needed.
                if( ECamTriActive == iInfo.iTargetVfState )
                  {
                  PRINT( _L( "Camera <> CCamAppController::GenerateModeChangeSequenceL: VF start needed" ) );
                  aSequence.Append( ECamRequestVfStart );
                  vfStartDone = ETrue;
                  }
                PRINT( _L( "Camera <> CCamAppController::GenerateModeChangeSequenceL: Snapshot on" ) );
                aSequence.Append( ECamRequestSsRelease );
                aSequence.Append( ECamRequestSsStart );
                }
            else
                {
                PRINT( _L( "Camera <> CCamAppController::GenerateModeChangeSequenceL: Snapshot on" ) );
                aSequence.Append( ECamRequestSsRelease );
                aSequence.Append( ECamRequestSsStart ); // SS is requested and delivered to tn-manager, but not shown on display
                }
            }
        }

      break;
      }
    // ---------------------------------
    case ECamControllerVideo:
      {
      // Video recorder needs to be initialized and prepared
      // if we are not already in video mode.
      if ( iInfo.iMode != ECamControllerVideo )
          {
          iVideoInitNeeded = ETrue;
          }
       aSequence.Append( ECamRequestVideoInit );
       
      // Snapshot has to be released and restarted because the
      // snapshot size may change.
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if( appUi )
          {
          if( appUi->ShowPostCaptureView() || iInfo.iTargetImageMode == ECamImageCaptureBurst )
              {
              // If vf needs to be (re)started.
              // Camera Controller checks from Setting Provider if
              // viewfinder mode (direct/bitmap) has changed, and
              // releases old type first if needed.
              if( ECamTriActive == iInfo.iTargetVfState )
                {
                PRINT( _L( "Camera <> CCamAppController::GenerateModeChangeSequenceL: VF start needed" ) );
                aSequence.Append( ECamRequestVfStart );
                vfStartDone = ETrue;
                }
              PRINT( _L( "Camera <> CCamAppController::GenerateModeChangeSequenceL: Snapshot on" ) );
              aSequence.Append( ECamRequestSsRelease );
              aSequence.Append( ECamRequestSsStart );
              }
          else
              {
              PRINT( _L( "Camera <> CCamAppController::GenerateModeChangeSequenceL: Snapshot on" ) );
              aSequence.Append( ECamRequestSsRelease );
              aSequence.Append( ECamRequestSsStart );
              }
          }
      break;
      }
    // ---------------------------------
    default:
      break;
    // ---------------------------------
    }

  // If vf needs to be (re)started.
  // Camera Controller checks from Setting Provider if
  // viewfinder mode (direct/bitmap) has changed, and
  // releases old type first if needed.
  if( !vfStartDone && ECamTriActive == iInfo.iTargetVfState )
    {
    PRINT( _L( "Camera <> CCamAppController::GenerateModeChangeSequenceL: VF start needed" ) );
    aSequence.Append( ECamRequestVfStart );
    }


  // -------------------------------------------------------
#ifdef _DEBUG
  PRINT( _L( "Camera <> Generated sequence: " ) );      
  for( TInt i = 0; i < aSequence.Count(); i++ )
    {
    PRINT2( _L( "Camera <> %d) %s" ), i, KCamRequestNames[aSequence[i]] );
    }
#endif  
  PRINT( _L( "Camera <= CCamAppController::GenerateModeChangeSequenceL" ) );
  }


// ---------------------------------------------------------------------------
// CCamAppController::SettingsPhotoResolution
// Returns the photo resolution stored by the Settings Model
// ---------------------------------------------------------------------------
//
TCamPhotoSizeId  CCamAppController::SettingsPhotoResolution() const
    {
    return iSettingsModel->CurrentPhotoResolution();
    }

// ---------------------------------------------------------------------------
// CCamAppController::GetCurrentImageDimensions
// ---------------------------------------------------------------------------
//
TSize 
CCamAppController::GetCurrentImageDimensions() const
  {
  TCamPhotoSizeId index = iSettingsModel->CurrentPhotoResolution();
  TSize           size  = iSettingsModel->ImageResolutionFromIndex( index );
  return size;
  }

// ---------------------------------------------------------------------------
// CCamAppController::GetCurrentImageResolution
// Gets the current image resolution used.  May not be the same at that stored in Settings
// if Zoom state is not 1x
// ---------------------------------------------------------------------------
//
TCamPhotoSizeId CCamAppController::GetCurrentImageResolution() const
    {   
    TCamPhotoSizeId size = iSettingsModel->CurrentPhotoResolution();
    return size;    
    }
    
// ---------------------------------------------------------------------------
// CCamAppController::GetCurrentImageCompression
// Returns the photo compression level from the Settings Model
// ---------------------------------------------------------------------------
//
TInt  CCamAppController::GetCurrentImageCompression() const
    {
    return iSettingsModel->CurrentPhotoCompression();
    }

// ---------------------------------------------------------------------------
// CCamAppController::GetCurrentVideoResolution
// Gets the current video resolution used.  
// ---------------------------------------------------------------------------
//
TCamVideoResolution CCamAppController::GetCurrentVideoResolution() const
    {   
    TInt videoQuality = iSettingsModel->IntegerSettingValue( ECamSettingItemVideoQuality );
    CCamVideoQualityLevel& level = *( iSettingsModel->VideoQualityArray() )[videoQuality];        
    return static_cast<TCamVideoResolution>( level.VideoResolution() );
    }

// ---------------------------------------------------------------------------
// CCamAppController::UsePhoneMemoryL
// Force use of phone memory storage.  
// ---------------------------------------------------------------------------
//
void CCamAppController::UsePhoneMemoryL() const
    {
#ifdef PRODUCT_SUPPORTS_FORCE_MEDIA_STORAGE_VALUE   
    // This function is used to directly force the value of media storage
    // Bypasses any updates the controller does for change to these values

    if( ExistMassStorage() )
        {
        iSettingsModel->SetIntegerSettingValueL( ECamSettingItemPhotoMediaStorage, 
                                             ECamMediaStorageMassStorage );
        
        iSettingsModel->SetIntegerSettingValueL( ECamSettingItemVideoMediaStorage, 
                                             ECamMediaStorageMassStorage );
        }
    else
        {        
        TCamMediaStorage storage = iSettingModel->
                                    IntegerSettingValue( ECamSettingItemRemovePhoneMemoryUsage )?
                                    ECamMediaStorageNone:
                                    ECamMediaStoragePhone;

        iSettingsModel->SetIntegerSettingValueL( ECamSettingItemPhotoMediaStorage, 
                                                    storage );
        
        iSettingsModel->SetIntegerSettingValueL( ECamSettingItemVideoMediaStorage, 
                                                    storage );
        }

#endif // PRODUCT_SUPPORTS_FORCE_MEDIA_STORAGE_VALUE
    }


// ===========================================================================
// From MCamSettingsModelObserver

// ---------------------------------------------------------------------------
// CCamAppController::IntSettingChangedL
//
// ---------------------------------------------------------------------------
//
void 
CCamAppController::IntSettingChangedL( TCamSettingItemIds aSettingItem, 
                                       TInt               aSettingValue )
  {
  PRINT( _L("Camera => CCamAppController::IntSettingChangedL") );  
  switch( aSettingItem )
    {
    case ECamSettingItemDynamicSelfTimer:
      {
      TCamSelfTimerFunctions function( ECamSelfTimerDisabled );
      switch( aSettingValue )
        {
        case ECamSelfTimer2:  function = ECamSelfTimerMode1; break;
        case ECamSelfTimer10: function = ECamSelfTimerMode2; break;
        case ECamSelfTimer20: function = ECamSelfTimerMode3; break;            
        default:                                             break;
        }
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if( appUi ) 
        {
        appUi->SelfTimerEnableL( function );
        }
      break;
      }
    case ECamSettingItemPhotoQuality:
        {
        HandleImageQualitySettingChangeL();
        break;
        }
    case ECamSettingItemVideoQuality:
       NotifyControllerObservers( ECamEventVideoQualityChanged, KErrNone );
       break;
    case ECamSettingItemVideoAudioRec:
      {
      NotifyControllerObservers( ECamEventAudioMuteStateChanged );
      break;
      }
    case ECamSettingItemContinuousAutofocus:  
      {
      NotifyControllerObservers( ECamEventContAFStateChanged );
      break;
      }
      
    case ECamSettingItemFaceTracking:
      {
      PRINT( _L("Camera => ECamSettingItemFaceTracking") );
      TCamCameraSettingId cameraId( 
        CCamSettingConversion::Map2CameraControllerSettingId( aSettingItem ) );
      iCameraController->DirectSettingsChangeL( cameraId );
      NotifyControllerObservers( ECamEventFaceTrackingStateChanged );
      
      // If Face tracking was turned off by user (not forced off by a scene mode)
      // update iPreviousFaceTrack to the current state as we are not going to
      // reset to the previous state automatically at any point.
      TCamSceneId scene = static_cast< TCamSceneId > 
          ( IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) );
      
      if ( scene != ECamSceneMacro &&
           scene != ECamSceneScenery &&
           scene != ECamSceneSports && 
           !iSceneModeForcedBySecondaryCamera )
          {
          PRINT1( _L("Camera MK: Changing face tracking state -> update iPreviousFaceTrack to %d"), aSettingValue );
          iSettingsModel->SetPreviousFaceTrack( static_cast<TCamSettingsOnOff>( aSettingValue ) );
          }
      
      break;
      }  
    case ECamSettingItemDynamicVideoFlash:  
      {
      PRINT( _L("Camera => ECamSettingItemDynamicVideoFlash") );
      TCamCameraSettingId cameraId( 
        CCamSettingConversion::Map2CameraControllerSettingId( aSettingItem ) );
      iCameraController->DirectSettingsChangeL( cameraId );
      break;
      }  
    default:
      {
      break;
      }
    }
  PRINT( _L("Camera <= CCamAppController::IntSettingChangedL") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::TextSettingChangedL
//
// ---------------------------------------------------------------------------
//
void 
CCamAppController::TextSettingChangedL( TCamSettingItemIds /*aSettingItem*/, 
                                        const TDesC&       /*aSettingValue*/ )
  {
  }

// ---------------------------------------------------------------------------
// HandleImageQualitySettingChangeL
// ---------------------------------------------------------------------------
//
void 
CCamAppController::HandleImageQualitySettingChangeL() 
  {
  PRINT( _L("Camera => CCamAppController::HandleImageQualitySettingChangeL") );
  if( IsViewFinding() ) 
    {
    PRINT( _L("Camera <> CCamAppController - viewfinder on, stop vf / re-prepare / start vf..") );
    TRAPD( status, 
      {
      IssueDirectRequestL( ECamRequestVfStop    );
      IssueDirectRequestL( ECamRequestSsRelease );
      IssueDirectRequestL( ECamRequestImageInit );
      IssueDirectRequestL( ECamRequestVfStart   );
      IssueDirectRequestL( ECamRequestSsStart   );
      });
      
    if( KErrNone != status )
      {
      SetOperation( ECamStandby, status );
      }
    }

  NotifyControllerObservers( ECamEventImageQualityChanged, KErrNone );      
  PRINT( _L("Camera <= CCamAppController::HandleImageQualitySettingChangeL") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::BurstModeActive
//
// ---------------------------------------------------------------------------
//
void
CCamAppController::BurstModeActiveL( TBool aActive, TBool /*aStillModeActive*/ )
  { 
  PRINT1( _L("Camera => CCamAppController::BurstModeActiveL %d"), aActive );  
  if( ECamControllerImage == iInfo.iMode )  
    {
    //TCamImageCaptureMode oldImageMode = iInfo.iImageMode; 
    // We are in still capture mode     
    if( aActive )
      {
      PRINT( _L("Camera <> CCamAppController .. iInfo.iImageMode = appUi->CurrentBurstMode() ") );  
      // Burst mode active. Query current mode from AppUi  
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      // iInfo.iImageMode = appUi->CurrentBurstMode();
      SetImageMode( appUi->CurrentBurstMode() );
      // reset image orientation
      iImageOrientation = ECamOrientation0;
      TRAP_IGNORE( SetImageOrientationL() );
      
      }
    else
      {
      // Burst mode not active  
      PRINT( _L("Camera <> CCamAppController .. iInfo.iImageMode = ECamImageCaptureSingle ") );
      // iInfo.iImageMode = ECamImageCaptureSingle;
      SetImageMode( ECamImageCaptureSingle );
      }  
    iRequestedCaptureCount = ( ECamImageCaptureBurst == iInfo.iImageMode )
                             ? KShortBurstCount
                             : 1;
    iCameraController->DirectSettingsChangeL( ECameraSettingCaptureLimit );
    // set image orientation in case it was changed during burst mode
    if( iOrientationChangeOccured )
        {
        iOrientationChangeOccured = EFalse;	
        TRAP_IGNORE( SetImageOrientationL() );
        }

    }
  else
    {
    PRINT( _L("Camera <> CCamAppController .. [WARNING] Not in image mode, set iInfo.iImageMode = ECamImageCaptureNone") );  
    // iInfo.iImageMode = ECamImageCaptureNone;  
    SetImageMode( ECamImageCaptureNone );
    } 
  PRINT( _L("Camera <= CCamAppController::BurstModeActiveL %d") );    
  }


// ---------------------------------------------------------------------------
// CCamAppController::SetVideoLengthL
// Set video length (max or short)
// ---------------------------------------------------------------------------
//
void CCamAppController::SetVideoLengthL( TInt /*aLength*/ )
    {
    PRINT( _L( "Camera => CCamAppController::SetVideoLengthL" ) );
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------------------------
// CCamAppController::CurrentPhotoStorageLocation
// Returns the current photo storage location filtered by MMC presence
// ---------------------------------------------------------------------------
TInt CCamAppController::CurrentPhotoStorageLocation() const
  {
  TInt storageLocation = iSettingsModel->IntegerSettingValue( ECamSettingItemPhotoMediaStorage );
  
  // added iForceUseOfPhoneMemory part to treat current location 
  // as Mass Storage / Phone, also if it is forced.
  if (  iForceUseOfPhoneMemory ||  
        ( ( storageLocation == ECamMediaStorageCard ) &&
          ( CamUtility::MemoryCardStatus() != ECamMemoryCardInserted ) ) )
    {
    if ( ExistMassStorage() )
      {
      storageLocation = ECamMediaStorageMassStorage;
      }
    else
      {
      storageLocation = IntegerSettingValue( ECamSettingItemRemovePhoneMemoryUsage)? 
                          ECamMediaStorageNone:
                          ECamMediaStoragePhone;
      }
    }
  return storageLocation;
  }

// ---------------------------------------------------------------------------
// CCamAppController::CurrentVideoStorageLocation
// Returns the current video storage location filtered by MMC presence
// ---------------------------------------------------------------------------
TInt CCamAppController::CurrentVideoStorageLocation() const
	{
	TInt storageLocation = iSettingsModel->IntegerSettingValue( ECamSettingItemVideoMediaStorage );        

	// added iForceUseOfPhoneMemory part to treat current location 
	// as Mass Storage / Phone, also if it is forced.
	
  if ( iForceUseOfPhoneMemory )
    {
    storageLocation = iForcedStorageLocation;
    }
  return storageLocation;
  }



// ---------------------------------------------------------------------------
// CCamAppController::ReleaseArray
// Record that the view has finished with the array
// ---------------------------------------------------------------------------
//
void CCamAppController::ReleaseArray( TBool aForcedRelease )
  {
  PRINT( _L("Camera => CCamAppController::ReleaseArray") );
  // Otherwise  we were using the real capture array, so reset it
  if( iArrayUsageCount > 0 )
      {
      iArrayUsageCount--;
      }

  if ( iArrayUsageCount == 0 || aForcedRelease )
      {
      PRINT( _L("Camere <> CCamAppController::ReleaseArray .. usage to 0") );
      iCaptureArray->Reset();
      iCurrentImageIndex = 0;
      // May have released a large amount of memory, so compress the heap
      User::Heap().Compress();
      }        

  PRINT( _L("Camera <= CCamAppController::ReleaseArray") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::RestoreFactorySettingsL
// Delete 
// ---------------------------------------------------------------------------
//    
void CCamAppController::RestoreFactorySettingsL( TBool aIsEmbedded )
  {          
  PRINT( _L("Camera => CCamAppController::RestoreFactorySettingsL") );
  iSettingsModel->ResetRepository();
    
  // reload static settings
  LoadStaticSettingsL( aIsEmbedded );
  
  // reload dynamic settings
  SetDynamicSettingsToDefaults();
  
  // Restoring camera settings didn't set the capture mode to single capture, 
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  appUi->SwitchStillCaptureModeL( ECamImageCaptureSingle, EFalse, EFalse );
  //
  SetMode( ECamControllerIdle );  
      
  if( iLocationTrailStarted &&
      ECamLocationOn != IntegerSettingValue( ECamSettingItemRecLocation ) )
    {    
    PRINT( _L("Camera <> CCamAppController::RestoreFactorySettingsL - Stop location trail") );
    // Location trail has been started, but setting off after restore -> stop trail
    // Starting (if needed after restore) happens when still precap view is activated
    StopLocationTrail();
    }
        
  //
  NotifyControllerObservers( ECamEventRestoreCameraSettings );
  PRINT( _L("Camera <= CCamAppController::RestoreFactorySettingsL") );
  }




// ---------------------------------------------------------------------------
// PlayComplete
//
// Called from when sound playing completes.
// ---------------------------------------------------------------------------
//
void 
CCamAppController::PlayComplete( TInt /*aStatus*/, TInt /*aSoundId*/ )
  {
  PlaySoundComplete();
  }

// ---------------------------------------------------------------------------
// CCamAppController::PlaySoundComplete
// Called when playback of a sound has completed.  Typically used in conjunction
// with Video recording (must only start recording AFTER sound has finished
// to avoid recording the sound in the recorded clip).
// ---------------------------------------------------------------------------
//
void CCamAppController::PlaySoundComplete()
  {
  PRINT( _L("Camera => CCamAppController::PlaySoundComplete") );
  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_PLAYSOUNDCOMPLETE, "e_CAM_APP_CAPTURE_SOUND_PLAY 0" );    //CCORAPP_CAPTURE_SOUND_PLAY_END
  
  __ASSERT_DEBUG( iCameraController, CamPanic( ECamPanicInvalidState ) );
  if( ECamControllerVideo == iInfo.iMode && !IsKeyLockOn() )
    {
    switch( CurrentVideoOperation() )
      {
      case ECamResuming:
        {
        // Restarting video
        DoRecord();
        break;
        }
      case ECamPausing:
        {
        // Notifies observers, so paused icon and softkeys can be updated.
        SetOperation( ECamPaused );
        break;
        }
      case ECamNoOperation:
        {
        // Starting the video capture
        // If the next file path has not changed, start capturing
        if ( BurstCaptureArray()->FileName( 0 ).Compare( iSuggestedVideoPath ) == 0 )
          {
          PRINT( _L("Camera <> starting to record..") );
          DoRecord();
          }
        // File path changed. Prepare the engine with the new video file path.
        // Start video recording when filename changed (event)
        else 
          {
          iVideoRequested = ETrue;
          iSuggestedVideoPath.Copy( BurstCaptureArray()->FileName( 0 ) );
          PRINT1( _L("Camera <> Requesting Camera controller to set the video name, iSuggestedVideoPath = [%S] "), &iSuggestedVideoPath );          
          TRAP_IGNORE(
            {
            // Set video name change pending and request setting it to camera controller.
            iSettingProvider->AddPendingSettingChangeL( ECamSettingItemVideoNameBase );
            IssueSettingsChangeRequestL();
            });
          iVideoNameRetries = KVideoNameRetryCount;
          }      
        }
      default:
        {
        // No operation. 
        // Should be stopping video
        }
      }
    }
  else
    {
    // Image mode or keylock on, no action
    }
  PRINT( _L("Camera <= CCamAppController::PlaySoundComplete") );
  }
// ---------------------------------------------------------------------------
// CCamAppController::MediaFileChangedNotification
//Helper function for making notification about deleted files
// ---------------------------------------------------------------------------
//
void  
CCamAppController::MediaFileChangedNotification()
  {
  PRINT( _L("Camera => CCamAppController::MediaFileChangedNotification") );
  NotifyControllerObservers( ECamEventMediaFileChanged );
  PRINT( _L("Camera <= CCamAppController::MediaFileChangedNotification") );
  }


// ---------------------------------------------------------------------------
// CCamAppController::DeleteCurrentFile
// Delete the current image/video file
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::DeleteCurrentFile( TBool aNotify /*=ETrue*/ )
  {
  PRINT( _L("Camera => CCamAppController::DeleteCurrentFile") );
  iImageSaveRequestPending = EFalse;

  TInt err( KErrNone );
  if( BurstCaptureArray()->Count() > 0 )
    {
    err = BurstCaptureArray()->SetDeleted( iCurrentImageIndex, ETrue );
    if ( aNotify && err == KErrNone )
      {
      NotifyControllerObservers( ECamEventMediaFileChanged );
      }
    }

  PRINT( _L("Camera <= CCamAppController::DeleteCurrentFile") );
  return err;
  }

// ---------------------------------------------------------------------------
// CCamAppController::DeleteTimeLapseFiles
// Delete the current timelapse sequence
// ---------------------------------------------------------------------------
//
void 
CCamAppController::DeleteTimeLapseFiles()    
  {
  // Count() is inline
  for ( TInt i = 0; i < iSequenceFilenameArray->Count(); i++ )
    {
    TPtrC filename( iSequenceFilenameArray->MdcaPoint( i ) );
    // Error ignored
    (void)iImageSaveActive->DeleteFile( filename, ETrue );
    }
  iSequenceFilenameArray->Reset();        
  }
 
// ---------------------------------------------------------------------------
// CCamAppController::IsProcessingCapture
// Indicate whether or not the controller is actively engaged in still
// or video capture
// ---------------------------------------------------------------------------
//   

TBool CCamAppController::IsProcessingCapture()
    {
    return ( iCaptureRequested  // if a still capture is pending
          || iVideoRequested    // if a video capture is pending                            
          || EngineProcessingCapture() // if recording or capture is active
          || ECamImageCaptureTimeLapse == iInfo.iImageMode  // if capturing time lapse
          || CurrentlySavingVideo() );        // if a video save is in progress  
    }
  
// ---------------------------------------------------------------------------
// CCamAppController::CompleteCaptureAndNotifyL
// Request that any current processes complete, then notify observers
// ---------------------------------------------------------------------------
//   
void 
CCamAppController::CompleteCaptureAndNotifyL( MCamObserver* aObserver, 
                                              TBool         aSafeCompletion )    
  {
  PRINT( _L( "Camera => CCamAppController::CompleteCaptureAndNotifyL" ) )
  
  RProperty::Set( KPSUidCamcorderNotifier, KCCorFocused, 0 );
    
  // Add the observer to the observer handler for capture completion.
  if ( !iCaptureCompletionObserverHandler )
    {
    iCaptureCompletionObserverHandler = CCamObserverHandler::NewL();
    }
  iCaptureCompletionObserverHandler->RegisterObserverL( aObserver );
    
  // if the capture has not yet begun, all that is required is to switch off
  // the pending capture
  iCaptureRequested = EFalse;
  iVideoRequested   = EFalse;
               
  // if no capture is actively in progress
  if ( !IsProcessingCapture() )
    {
    PRINT( _L( "Camera <> not capturing calling HandleCaptureCompletion" ) )
    HandleCaptureCompletion();
    }

  // if recording is active
  else if ( ECamControllerVideo == iInfo.iMode )
    {
    if( ECamCapturing == iInfo.iOperation
     || ECamPausing   == iInfo.iOperation
     || ECamPaused    == iInfo.iOperation
     || ECamResuming  == iInfo.iOperation )
      {
      PRINT( _L( "Camera <> currently recording calling StopVideoRecording()" ) )
      StopVideoRecording();
      // iCaptureCompletionObserverHandler will be notified when the record completes
      // in McaeoVideoRecordingComplete
      }
    }        
  else if ( ECamImageCaptureTimeLapse == iInfo.iImageMode )
    {
    PRINT( _L( "Camera <> Calling StopSequenceCapture" ) )
    StopSequenceCaptureL();
    // iCaptureCompletionObserverHandler will be notified when the 
    // next image saving completes
    }
            
  // if a burst capture is in early stages
  else if ( ECamImageCaptureBurst == iInfo.iImageMode )
    {
    if( ECamCapturing == iInfo.iOperation )
      {
      if ( !aSafeCompletion )
        {
        //iEngine->CancelCaptureStill();
        PRINT( _L( "Camera <> Safe completion requested, currently capturing burst, issue ECamRequestImageCancel" ) )
        IssueDirectRequestL( ECamRequestImageCancel );
        CompleteBurstOperation();
        }
      else
        {
        //iEngine->StopStillBurstCapture();
        PRINT( _L( "Camera <> Not safe completion requested, currently capturing burst, issue ECamRequestImageCancel" ) )
        IssueDirectRequestL( ECamRequestImageCancel );
        // iCaptureCompletionObserverHandler will be notified when the 
        // image saving completes
        }
      }
    }
  else // do nothing
    {
    PRINT( _L( "Camera <> currently waiting for capture completion" ) )
    // e.g. iOperation == ECamCaptureCompleting
    // iCaptureCompletionObserverHandler will be notified when the 
    // image saving completes
    
    // e.g. CurrentlySavingVideo()
    // iCaptureCompletionObserverHandler will be notified when the 
    // video saving completes in McaeoVideoRecordingComplete
    
    // e.g. iOperation == ECamCaptureInProgress (single capture)
    // iCaptureCompletionObserverHandler will be notified when the 
    // image saving completes
    }        
  PRINT( _L( "Camera <= CCamAppController::CompleteCaptureAndNotifyL" ) )
  }

// ---------------------------------------------------------------------------
// CCamAppController::EnterShutdownMode
// Enter shutdown mode as the application is closing
// ---------------------------------------------------------------------------
//
void 
CCamAppController::EnterShutdownMode( TBool aImmediateShutdown )
  {
  PRINT1( _L( "Camera => CCamAppController::EnterShutdownMode( aImmediateShutdown=%d )"), aImmediateShutdown );
  
  DeRegisterHarverterClientEvents();  

  if ( !IsInShutdownMode() )
    {
    PRINT ( _L( "Camera <> CCamAppController::EnterShutdownMode: set target mode to ECamControllerShutdown..") );
    SetTargetMode( ECamControllerShutdown );
    }

  // if not close down straightaway (i.e. for embedded )
  if ( !aImmediateShutdown )
    {
    // cancel any outstanding sound requests
    // iSoundPlayer->CancelTonePlayer();
    iSoundPlayer->CancelAllPlaying();
    
    TRAPD( err, CompleteCaptureAndNotifyL( this, EFalse ) )
    if ( err )
      {
      NotifyControllerObservers( ECamEventExitRequested, KErrNone );
      }
    }
  else
    {
    // This checks for any ongoing operation.
    ReleaseCamera();
    }

  PRINT ( _L( "Camera <= CCamAppController::EnterShutdownMode") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::IsInShutdownMode
// Whether or not the controller is in shutdown mode
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsInShutdownMode() const
    {
    return ( ECamControllerShutdown == iInfo.iTargetMode );
    }


// ---------------------------------------------------------------------------
// CCamAppController::IsAppUiAvailable
// Returns true if AppUi has been constructed, and not yet destructed
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsAppUiAvailable() const
    {
    PRINT1(_L("Camera <> CCamAppController::IsAppUiAvailable=%d"),iAppUiAvailable);
    return iAppUiAvailable;
    }       
    
// ---------------------------------------------------------------------------
// CCamAppController::SetAppUiAvailable
// Sets AppUi availablility (see IsAppUiAvailable)
// ---------------------------------------------------------------------------
//    
void CCamAppController::SetAppUiAvailable( TBool aAvailable )
    {
    iAppUiAvailable = aAvailable;  
    }


// ---------------------------------------------------------------------------
// CCamAppController::GetZoomRange
// Returns the minimum and maximum supported zoom values
// ---------------------------------------------------------------------------
//
void 
CCamAppController::GetZoomRange( TInt& aMinZoom, TInt& aMaxZoom ) const
  {
  __ASSERT_ALWAYS( iCameraController, CamPanic( ECamPanicNullPointer ) );
  aMinZoom = 0;
  aMaxZoom = iCameraController->CameraInfo().iMaxDigitalZoom;
  }

// ---------------------------------------------------------------------------
// CCamAppController::ZoomValue
// Returns the current zoom value
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::ZoomValue() const
  {       
  return iDesiredZoomVal;
  }

// ---------------------------------------------------------------------------
// CCamAppController::SetZoomValue
// Sets a new zoom value
// ---------------------------------------------------------------------------
//
void CCamAppController::SetZoomValue( TInt aValue )
  {
  PRINT1( _L("Camera => CCamAppController::SetZoomValue( %d )"), aValue );
  __ASSERT_ALWAYS( iCameraController, CamPanic( ECamPanicNullPointer ) );

  // Check the requested value for validity
  __ASSERT_DEBUG( Rng( 0, aValue, iCameraController->CameraInfo().iMaxDigitalZoom ), 
                  CamPanic( ECamPanicInvalidZoomValue ) );
  
  if ( ECamControllerImage == iInfo.iMode
    || ECamControllerVideo == iInfo.iMode )
    {
    // Setting provider asks for the zoom value from us
    iDesiredZoomVal = aValue;

    if ( !iZoomWaitingForCamera )
      {
      PRINT( _L("Camera <> CCamAppController: calling Camera Controller..") );
      
      TRAPD( error,
        {
        iCameraController->DirectSettingsChangeL( ECameraSettingDigitalZoom );
        });

      // If no leave occurred, notification is done in HandleSettingEventL
      if( KErrNone != error )
        {
        NotifyControllerObservers( ECamEventZoomStateChanged, error );
        }

      // iZoomWaitingForCamera is set to EFalse when vf frame arrives.
      // With direct vf, this never happens, so do not set to ETrue.
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if ( appUi && !appUi->IsDirectViewfinderActive() )
        {
        iZoomWaitingForCamera = ETrue;
        }
      iNewZoomPending = EFalse;
      }
    else
      {
      PRINT( _L("Camera <> CCamAppController: zoom is waiting for VF update") );
      iNewZoomPending = ETrue;
      }
    }
  PRINT( _L("Camera <= CCamAppController::SetZoomValue") );
  }



// ---------------------------------------------------------------------------
// CCamAppController::ResetUserSceneL
// Resets all the user scene settings to their defaults.
// ---------------------------------------------------------------------------
//
void CCamAppController::ResetUserSceneL()
    {
    iSettingsModel->ResetUserSceneL();
    }

// ---------------------------------------------------------------------------
// CCamAppController::PreviewSettingChangeL
// 
// Previews a new value for the specified integer setting.
// If method leaves, preview is not activated.
// ---------------------------------------------------------------------------
//
void 
CCamAppController::PreviewSettingChangeL( TInt aSettingItem, 
                                          TInt aSettingValue )
  {
  PRINT( _L("Camera => CCamAppController::PreviewSettingChangeL") );
  __ASSERT_DEBUG( iSettingsPreviewHandler, CamPanic( ECamPanicNullPointer ) );
  
  TCamSettingItemIds settingId( 
      static_cast<TCamSettingItemIds>(aSettingItem) );

  PRINT1( _L("Camera <> Setting to be previewed [%s]"), KCamSettingItemNames[settingId] );

  if ( settingId == ECamSettingItemDynamicSelfTimer )
      {
      SetSlideInSelfTimerPreview(ETrue);
      }

  // Activate the preview as a first thing.
  // If requests are issued to Camera Controller,
  // it asks for the setting value through Setting Provider and
  // the setting needs to be already in preview at that point.
  // If this fails, there's nothing we need to roll back.
  iSettingsPreviewHandler->ActivatePreviewL( settingId, aSettingValue );


  // Check which setting is previewed.
  // Some need special attention, or should/need not be set to Camera at this point.
  TInt status( KErrNone );
  switch( settingId )
    {
    // ---------------------------------
    // Image quality
    case ECamSettingItemPhotoQuality:
      // Still image quality does not require re-prepare,
      // as even the remining images counting is done 
      // based on local size esimates.
      // Need to do notification to update counters.
      NotifyControllerObservers( ECamEventImageQualityChanged );
      break;
    // ---------------------------------
    // Video quality
    case ECamSettingItemVideoQuality:
      // Video needs to be re-prepared to be able to update
      // remaining time counters.
      NotifyControllerObservers( ECamEventVideoQualityChanged );
      break;
    // ---------------------------------
    // Self timer
    case ECamSettingItemDynamicSelfTimer:
      // Just set the preview. Once committed, set via AppUi.
      break;
    // ---------------------------------
    // Others - set to Camera to see the effect.
    default:
      {
      TCamCameraSettingId cameraId( 
        CCamSettingConversion::Map2CameraControllerSettingId( settingId ) );

      //PRINT1( _L("Camera <> CCamAppController: Setting the preview to Camera, id[%s]"), KCameraSettingNames[settingId] );

      // Assertation to notice if some setting is not properly handled.
      __ASSERT_DEBUG( ECameraSettingNone != cameraId, CamPanic( ECamPanicNotSupported ) );
      
      if( ECameraSettingNone != cameraId )
        {
        // Preview is active.
        // Camera controller will ask the value during DirectSettingsChangeL.
        // Need trap to be able to cancel on failure.
        TRAP( status, iCameraController->DirectSettingsChangeL( cameraId ) );
        }      
      break;
      }
    // ---------------------------------
    }

  if( KErrNone != status )
    {
    PRINT( _L("Camera <> CCamAppController: Problem in preview, cancelling!!!") );     

    // Invalid id does not cause problem here.
    iSettingsPreviewHandler->CancelPreview( settingId );
    User::Leave( status );
    }
  PRINT( _L("Camera <= CCamAppController::PreviewSettingChangeL") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::CancelPreviewChangesL
// Cancels all preview changes, since last commit/cancel.
// ---------------------------------------------------------------------------
//
void
CCamAppController::CancelPreviewChangesL()
  {
  PRINT( _L("Camera => CCamAppController::CancelPreviewChangesL") );

  // Just check non-null, but no panic. 
  if ( iSettingsPreviewHandler )
    {
    RArray<TCamSettingItemIds> previews;
    CleanupClosePushL( previews );

    // Get all active previews
    iSettingsPreviewHandler->ActivePreviewsL( previews );
    // Reset all previews so the Setting Provider does
    // not get those values anymore.
    iSettingsPreviewHandler->CancelAllPreviews();

    // Reset the original setting values to Camera
    TInt                errorOne( KErrNone           );
    TInt                errorAll( KErrNone           );    
    TCamCameraSettingId cameraId( ECameraSettingNone );
    for( TInt i = previews.Count() - 1; i >= 0; i-- )
      {
      if( ECamSettingItemVideoQuality == previews[i] )
        {
        TRAP( errorOne, RePrepareVideoL() );
        }
      else
        {
        cameraId =
          CCamSettingConversion::Map2CameraControllerSettingId( previews[i] );
      
        if( ECameraSettingNone != cameraId )
          {
          TRAP( errorOne, iCameraController->DirectSettingsChangeL( cameraId ) );
          }
        /* All previews are not set to Camera.
        else
          {
          errorOne = KErrNotSupported;
          }
        */
        }

      if( KErrNone == errorAll 
       && KErrNone != errorOne )
        {
        errorAll = errorOne;
        }
      }
    User::LeaveIfError( errorAll );

    CleanupStack::PopAndDestroy(); // previews
    }
  PRINT( _L("Camera <= CCamAppController::CancelPreviewChangesL") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::CommitPreviewChanges
// Commits last preview change.
// ---------------------------------------------------------------------------
//
void 
CCamAppController::CommitPreviewChanges()
  {
  PRINT( _L("Camera => CCamAppController::CommitPreviewChanges") );
  __ASSERT_DEBUG( iSettingsPreviewHandler, CamPanic( ECamPanicNullPointer ) );

  // All previews have been set to Camera already.
  // Only need to request the model to store new values.
  // Currently ignore error, if settings model cannot save the preview.
  TRAPD( error, iSettingsPreviewHandler->CommitAllPreviewsL() );
  if( KErrNone != error )
    {
    // Ignored.
    PRINT( _L("Camera <> Settings model could not save the preview!!!") );
    }
  PRINT( _L("Camera <= CCamAppController::CommitPreviewChanges") );
  }


// ---------------------------------------------------------------------------
// RePrepareVideoL
//
// Helper method for re-preparing video if e.g. a setting change
// requires it. If Camera Controller is currently processing
// some request, set the flag to issue the request later.
//
// Used atleast in PreviewSettingChangeL and CancelPreviewChangesL.
// ---------------------------------------------------------------------------
//
void
CCamAppController::RePrepareVideoL()
  {
  PRINT( _L("Camera => CCamAppController::RePrepareVideoL") );

  GenerateNextValidVideoPathL();
  if( !Busy() )
    {
    iVideoPrepareNeeded = EFalse;
    IssueRequestL( ECamRequestVideoInit );
    }
  else
    {
    iVideoPrepareNeeded = ETrue;
    }
  PRINT( _L("Camera <= CCamAppController::RePrepareVideoL") );
  }



// ---------------------------------------------------------------------------
// CCamAppController::IntegerSettingValue
// Returns the current integer value for the specified setting
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::IntegerSettingValue( TInt aSettingItem ) const
  {
  // If the requested item is the photo storage location, this must be filtered
  // based on the availability of the MMC.
  if ( aSettingItem == ECamSettingItemPhotoMediaStorage )
    {
    return CurrentPhotoStorageLocation();
    }

  // If the requested item is the video storage location, this must be filtered
  // based on the availability of the MMC.
  if ( aSettingItem == ECamSettingItemVideoMediaStorage )
    {
    return CurrentVideoStorageLocation();
    }

  // Not a setting we sometimes need to filter
  return IntegerSettingValueUnfiltered( aSettingItem );
  }

// ---------------------------------------------------------------------------
// CCamAppController::IntegerSettingValueUnfiltered
// Returns the current integer value for the specified setting
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::IntegerSettingValueUnfiltered( TInt aSettingItem ) const
  {
  if( aSettingItem == ECamSettingItemVideoFileType )
    {
    TInt videoQuality = iSettingsModel->IntegerSettingValue( ECamSettingItemVideoQuality );
  
    CCamVideoQualityLevel& level = *( iSettingsModel->VideoQualityArray() )[videoQuality];
  
    return level.VideoType();
    }
  // Query the relevant settings model.
  return iSettingsModel->IntegerSettingValue( aSettingItem );
  }

// ---------------------------------------------------------------------------
// CCamAppController::SetIntegerSettingValueL
// Sets a new value for the specified integer setting
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetIntegerSettingValueL( TInt aSettingItem, 
                                            TInt aSettingValue )
  {
  PRINT( _L("Camera => CCamAppController::SetIntegerSettingValueL"))
  // Update the settings model.
  iSettingsModel->SetIntegerSettingValueL( aSettingItem, aSettingValue );

  // Update observers with any changes that do not directly affect the
  // engine. That is, they need to be updated now because the settings model
  // will not pass these to this observer.
  switch( aSettingItem )
    {
    case ECamSettingItemPhotoMediaStorage:
    case ECamSettingItemVideoMediaStorage:
      {
      PRINT1(_L("Camera SetIntegerSettingValueL calling SetPathnamesToNewStorageL %d" ), aSettingValue);
      // reset iForceUseOfPhoneMemory flag when user chooses 
      // from settings.
      iForceUseOfPhoneMemory = EFalse;
      SetPathnamesToNewStorageL(static_cast < TCamMediaStorage > ( aSettingValue ) );
      InitCaptureLimitL();
      break;
      }
    case ECamSettingItemDynamicVideoScene:
      {
      iVideoInitNeeded = ETrue;
      NotifyControllerObservers( ECamEventSceneStateChanged );
      break;
      }
    case ECamSettingItemDynamicPhotoScene:
    case ECamSettingItemUserSceneBasedOnScene:
      {
      if( iConfigManager && iConfigManager->IsAutoFocusSupported() )
        {
        // Check for macro or landscape scene/based on scene and
        // set up AF accordingly. For other scenes, the AF mode
        // is setup at capture
        CheckAfModeForScene();
        }
      NotifyControllerObservers( ECamEventSceneStateChanged );
      break;
      }
    case ECamSettingItemVideoAudioRec:
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if ( appUi->IsEmbedded() )
        {
        iCameraController->DirectSettingsChangeL( ECameraSettingAudioMute );
        }
      NotifyControllerObservers( ECamEventAudioMuteStateChanged );
      break;
      }
    case ECamSettingItemContinuousAutofocus:
      {
      iSettingProvider->AddPendingSettingChangeL( ECamSettingItemContinuousAutofocus );
      IssueSettingsChangeRequestL();
      break;
      }
    case ECamSettingItemPhotoStoreInAlbum:
    case ECamSettingItemVideoStoreInAlbum:
      {
      TUint32 defaultAlbumId = static_cast<TUint32>( 
              IntegerSettingValue( ECamSettingItemDefaultAlbumId ));

      iImageSaveActive->SetAddImageToAlbum( aSettingValue == ECamSettYes,
                                            defaultAlbumId );
      break;
      }
    case ECamSettingItemRecLocation:
      {
      // Set the location utility active object
      if ( ECamLocationOff == aSettingValue )
          {
          StopLocationTrail();
          }
      else if ( ECamLocationOn == aSettingValue )
          {
          StartLocationTrailL();
          }
      
      // Notify observers
      NotifyControllerObservers( ECamEventLocationSettingChanged );
      break;  
      }
    // Need to support more zoom settings.
    // Digital/extended setting combined to 
    // ECamSettingItemPhotoDigitalZoom and ECamSettingItemVideoDigitalZoom
    case ECamSettingItemPhotoDigitalZoom:
    case ECamSettingItemVideoDigitalZoom:
      {
      // Notify observers
      NotifyControllerObservers( ECamEventZoomSetupChanged );
      break;
      }

    default:
      {
      // no action for others here
      break;
      }
    }

  PRINT( _L("Camera <= CCamAppController::SetIntegerSettingValueL"))        
  }



// ---------------------------------------------------------------------------
// CCamAppController::SetPathnamesToNewStorageL
// Sets pathnames for video and image files to point to new storage
// ---------------------------------------------------------------------------
void CCamAppController::SetPathnamesToNewStorageL( TCamMediaStorage aMediaStorage )
  {    
  PRINT(_L("Camera => CCamAppController::SetPathnamesToNewStorageL" ))
  
  iSetNewPathnamesPending = ECamMediaStorageNone;
  TFileName path;
  
  // If the engine is prepared for image capture then check that the required 
  // image folder exists
  if ( IsFlagOn( CameraControllerState(), ECamImageOn ) )
    {
    CCamFolderUtility::GetBasePathL( iMonthFolderCounters, 
                                     aMediaStorage, 
                                     path, 
                                     ECamControllerImage, 
                                     EFalse ); 
    iCurrentStorage = aMediaStorage;
    }
   
  // If the engine is prepared for video capture then check that the required 
  // month folder exists and set up the engine with the new video path, this
  // will be adjusted with the appropriate month counter folder when video
  // capture begins
  else if ( IsFlagOn( CameraControllerState(), ECamVideoOn ) )
    {
    CCamFolderUtility::GetBasePathL( iMonthFolderCounters, 
                                     aMediaStorage, 
                                     path, 
                                     ECamControllerVideo, 
                                     EFalse ); 
     if( iInfo.iOperation != ECamCapturing    &&
          iInfo.iOperation != ECamPausing      &&
          iInfo.iOperation != ECamPaused       &&
          iInfo.iOperation != ECamResuming   &&
          iInfo.iOperation != ECamCompleting )
      {
       GenerateNextValidVideoPathL();
      }
    iCurrentStorage = aMediaStorage;
    
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );  
    
    if( !appUi->SettingsLaunchedFromCamera() )
      {
      // Set video name change pending and request setting it to camera controller.
      if( iInfo.iOperation != ECamCapturing    &&
          iInfo.iOperation != ECamPausing      &&
          iInfo.iOperation != ECamPaused       &&
          iInfo.iOperation != ECamResuming   &&
          iInfo.iOperation != ECamCompleting )
    	{
        iSettingProvider->AddPendingSettingChangeL( ECamSettingItemVideoNameBase );
        // Can't set the new pathname, if camera controller is busy.
        if ( CameraControllerBusy() )
          {
          PRINT( _L("Camera <> Camera controller busy. New pathname not set. Set iSetNewPathnamesPending.") )
          iSetNewPathnamesPending = aMediaStorage;
          }
        else
          {
          IssueSettingsChangeRequestL();
          }
    	}
        
      iVideoNameRetries   = KVideoNameRetryCount;    
      //iVideoTimeRemaining = KMaxRecordingLength;
      if( iInfo.iOperation == ECamCapturing    ||
          iInfo.iOperation == ECamPausing      ||
          iInfo.iOperation == ECamPaused       ||
          iInfo.iOperation == ECamResuming     ||
          iInfo.iOperation == ECamCompleting )
        {          
        iVideoTimeRemaining = RemainingVideoRecordingTime();  
        }
      else 
        {
        PRINT( _L("Camera <> CCamAppController::SetPathnamesToNewStorageL - video mode not yet initialized" ));
        TRAPD( err, iVideoTimeRemaining = CalculateVideoTimeRemainingL() );
        if( KErrNone != err )
          {
          iVideoTimeRemaining = 0;
          }
        }
      }
    }
    
  // The engine is not yet prepared, the folder will be checked when the engine preparation
  // takes place. The controller also checks the state of iSetNewPathnamesPending when 
  // preparation completes, just in case preparation is currently in progress.
  else
    {
    iSetNewPathnamesPending = aMediaStorage;
    }  
              
  NotifyControllerObservers( ECamEventSaveLocationChanged );
  PRINT(_L("Camera <= CCamAppController::SetPathnamesToNewStorageL" ))
  }

// ---------------------------------------------------------------------------
// CCamAppController::SetTextSettingValueL
// Sets a new text value for the specified setting
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetTextSettingValueL(       TInt   aSettingItem, 
                                         const TDesC& aSettingValue )
  {
  // Update the relevant settings model.
  iSettingsModel->SetTextSettingValueL( aSettingItem, aSettingValue );
  }


// ---------------------------------------------------------------------------
// CCamAppController::TextSettingValue
// Returns the current text value for the specified setting
// ---------------------------------------------------------------------------
//
TPtrC 
CCamAppController::TextSettingValue( TInt aSettingItem ) const
  {
  // Query the relevant settings model.
  return iSettingsModel->TextSettingValue( aSettingItem );
  }

// ---------------------------------------------------------------------------
// CCamAppController::SettingValueEnabled
// Returns whether a particular setting value can be set or not.
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::SettingValueEnabled( TInt aSettingItem, 
                                        TInt aSettingValue ) const 
  {
  return iSettingsModel->SettingValueEnabled( aSettingItem, aSettingValue );
  }

// ---------------------------------------------------------------------------
// CCamAppController::LoadStaticSettingsL
// Loads the static settings from shared data
// ---------------------------------------------------------------------------
//
void 
CCamAppController::LoadStaticSettingsL( TBool aIsEmbedded )
  {
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPCONTROLLER_LOADSTATICSETTINGSL, "e_CCamAppController_LoadStaticSettingsL 1" );
  PRINT(_L("Camera => CCamAppController::LoadStaticSettingsL" ))
  iSettingsModel->LoadStaticSettingsL( aIsEmbedded );
  
  // Cache the image 'add to album' setting in the saving active object
  // it will change infrequently
  TInt value = iSettingsModel->IntegerSettingValue( ECamSettingItemPhotoStoreInAlbum );
  
  TUint32 defaultAlbumId = static_cast<TUint32>( 
              IntegerSettingValue( ECamSettingItemDefaultAlbumId ));
  iImageSaveActive->SetAddImageToAlbum( value == ECamSettYes,
                                        defaultAlbumId );

  // Ensure that the navipane is upto date with the current save location.
  if ( !aIsEmbedded )
    {
    NotifyControllerObservers( ECamEventAudioMuteStateChanged );
    NotifyControllerObservers( ECamEventVideoQualityChanged );
    }
    NotifyControllerObservers( ECamEventSaveLocationChanged );
    NotifyControllerObservers( ECamEventVideoStabChanged );
    if ( iConfigManager  && iConfigManager->IsLocationSupported() )
        {
        NotifyControllerObservers( ECamEventLocationSettingChanged );
        }
    NotifyControllerObservers( ECamEventVideoFileTypeChanged );
  NotifyControllerObservers( ECamEventImageQualityChanged );
  NotifyControllerObservers( ECamEventContAFStateChanged ); 
  PRINT(_L("Camera <= CCamAppController::LoadStaticSettingsL" ))
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPCONTROLLER_LOADSTATICSETTINGSL, "e_CCamAppController_LoadStaticSettingsL 0" );
  }


// ---------------------------------------------------------------------------
// DynamicSettingDefaultValue
// Get the setting default value.
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::DynamicSettingDefaultValue( TInt aSettingId ) const
  {
  return iSettingsModel->DynamicSettingDefaultValue( aSettingId );
  }

// ---------------------------------------------------------------------------
// CCamAppController::SetDynamicSettingsToDefaults
// Sets all dynamic settings to their defaults.
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetDynamicSettingsToDefaults()
  {
  iSettingsModel->SetDynamicSettingsToDefaults();
  }

// ---------------------------------------------------------------------------
// CCamAppController::SetDynamicSettingsToDefaults
// Sets a dynamic setting to its default.
// ---------------------------------------------------------------------------
//
void 
CCamAppController::SetDynamicSettingToDefault(TInt aSettingItem)
  {
  iSettingsModel->SetDynamicSettingToDefault(aSettingItem);
  }

// ---------------------------------------------------------------------------
// SetDynamicSettingToSceneDefaultL
// 
// Set the dynamic setting identified by aSettingId to scene default.
// Because video and photo scenes can have sama identifiers, aVideoScene
// is needed to decide which ones to look for.
// ---------------------------------------------------------------------------
//
TInt
CCamAppController::SetDynamicSettingToSceneDefaultL( TInt  aSceneId,
                                                     TInt  aSceneField,
                                                     TInt  aSettingId,
                                                     TBool aVideoScene )
  {
  return iSettingsModel->SetDynamicSettingToSceneDefaultL( aSceneId, 
                                                           aSceneField, 
                                                           aSettingId,
                                                           aVideoScene );
  }




#ifdef PRODUCT_USES_GENERIC_SETUP_INDICATOR
// ---------------------------------------------------------------------------
// CCamAppController::VideoSceneDefaultsAreSet
// Returns whether the video setup settings are set to the defaults
// for its current scene.
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::VideoSceneDefaultsAreSet()
    {
    return iSettingsModel->VideoSceneDefaultsAreSet();
    }

// ---------------------------------------------------------------------------
// CCamAppController::PhotoSceneDefaultsAreSet
// Returns whether the photo setup settings are set to the defaults
// for its current scene.
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::PhotoSceneDefaultsAreSet()
    {
    return iSettingsModel->PhotoSceneDefaultsAreSet();
    }

#endif // PRODUCT_USES_GENERIC_SETUP_INDICATOR


// ---------------------------------------------------------------------------
// CCamAppController::CurrentFullFileName
// ---------------------------------------------------------------------------
//
const TDesC& 
CCamAppController::CurrentFullFileName() const
  {
  PRINT1( _L("Camera => CCamAppController::CurrentFullFileName, index:%d"), iCurrentImageIndex );
  const TDesC& name( BurstCaptureArray()->FileName( iCurrentImageIndex ) );

  if( name != KNullDesC )
    {
    PRINT1( _L("Camera <= CCamAppController::CurrentFullFileName, return:[%S]"), &name );
    }
  else
    {
    PRINT( _L("Camera <= CCamAppController::CurrentFullFileName, name not set.") ); 
    }   
  return name;
  }

const TDesC& 
CCamAppController::CurrentVideoFileName() const
  {
  return iSuggestedVideoPath;
  }

// ---------------------------------------------------------------------------
// CCamAppController::CCamAppController
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CCamAppController::CCamAppController()
  : iBusyFlags( EBusyNone )
  , iDiskCriticalLevel   ( KErrNotFound )
  , iRamDiskCriticalLevel( KErrNotFound )
  , iImageOrientation( ECamOrientation0 )
  , iLastImageOrientation( ECamOrientation0 )
  , iPendingHdmiEvent( ECamHdmiNoEvent )
  {
  }

// ---------------------------------------------------------------------------
// CCamAppController::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CCamAppController::ConstructL()
  {
  PRINT( _L("Camera => CCamAppController::ConstructL"));
  iPendingAFRequest=0;
  iCurrentAFRequest=0;
  iNoBurstCancel=EFalse;

  PRINT( _L("Camera <> CCamAppController::ConstructL - constructing Configuration Manager"));
  iConfiguration = CCamConfiguration::NewL();      
  
 
  CCamFolderUtility::ResetCounters( iMonthFolderCounters );
  iCaptureKeyPressedWhileImageSaving = EFalse;

  iCaptureModeTransitionInProgress = ETrue;

  iSettingsModel = CCamSettingsModel::NewL( *iConfiguration );  
  iSettingsModel->AttachObserverL( this );
  
  // Get the config pointer from settings model
  iConfigManager = static_cast<CCamSettingsModel*>(iSettingsModel)->UiConfigManagerPtr();
  
  RArray<TInt> supportedValues;
  CleanupClosePushL( supportedValues );
    
  // Get the EV slider info
  // these values should be read before camsettingsprovided is constructd
  // as the EV step value is needed during the construction of camsettingsprovider.
  iConfigManager->SupportedEVSettingsL( supportedValues );

  if ( supportedValues.Count() > 0 )
      {
      iEvRange.iMinValue     = supportedValues[0];
      iEvRange.iMaxValue     = supportedValues[1];
      iEvRange.iStepsPerUnit = supportedValues[2];      
      }

  CleanupStack::PopAndDestroy( &supportedValues );
    
  // The Settings Model handles the previews at the moment
  iSettingsPreviewHandler = static_cast<CCamSettingsModel*>( iSettingsModel ); 

  //iSettingProvider = CCamConstantSettingProvider::NewL( *this );
  iSettingProvider = CCamSettingProvider::NewL( *this, iSettingsModel );
 
  iDeepSleepTimer = CCamTimer::NewL( KDeepSleepTimeout, 
                    TCallBack( CCamAppController::DeepSleepTimeout, this ) );
  
  iIdleTimer             = CCamTimer::NewL( KIdleTimeout, 
                                            TCallBack( CCamAppController::IdleTimeoutL, this ) );
  iImageSaveActive       = CCamImageSaveActive::NewL( *this , *this );        
  iCaptureArray          = CCamBurstCaptureArray::NewL( *iImageSaveActive );
  iRotationArray         = CCamBurstCaptureArray::NewL( *iImageSaveActive );
  iSnapShotRotator       = CCamSnapShotRotator::NewL( *this );
  iSequenceFilenameArray = new( ELeave ) CDesCArraySeg( KTimelapseArrayGranularity );
  iSoundPlayer           = CCamAudioPlayerController::NewL( *this, *this );

  if ( iConfigManager && iConfigManager->IsLensCoverSupported() )
       {
       // create property watcher for slide state
       PRINT( _L("Camera <> CCamAppController: Define Camera slider P&S key..") );
       
       // Whole slider position reading could be removed after PCFW has implemented
       // creation for KLensCoverStatus key
       TInt sliderPosition;
       TInt err = RProperty::Get(
                   CameraPlatPSKeys::KPSCameraPlatUid,
                   CameraPlatPSKeys::KLensCoverStatus,
                   sliderPosition );  
  
 
       if( err != KErrNone )
           {
           iSlideStateWatcher     = CCamPropertyWatcher::NewL( *this, 
                   NMusResourceApi::KCategoryUid, 
                   NMusResourceApi::KCameraAvailability );
           }
       else
           {
           iSlideStateWatcher     = CCamPropertyWatcher::NewL( *this, 
                   CameraPlatPSKeys::KPSCameraPlatUid, 
                   CameraPlatPSKeys::KLensCoverStatus );
           }
       }
  
  if ( iConfigManager && iConfigManager->IsKeyLockWatcherSupported() )
      {
      iKeyLockStatusWatcher  = CCamPropertyWatcher::NewL( *this,
                                                          KPSUidAvkonDomain,
                                                          KAknKeyguardStatus );
      // request notifications about key lock status
      iKeyLockStatusWatcher->Subscribe();     
      }
  
  iProfileStatusWatcher = CCamPropertyWatcher::NewL( *this,
                                                     KPSUidProfileEngine,
                                                     KProEngActiveProfileChanged );
  // request notifications about profile status
  iProfileStatusWatcher->Subscribe();    
  IsProfileSilent();
  
  // read central repository value indicating whether camera shutter sound
  // should be played always or depending on the current profile setting
  CRepository* cr = CRepository::NewLC( KCRUidCamcorderFeatures );
  TInt value;
  TInt err;
  User::LeaveIfError( cr->Get( KCRCamShutterSound, value ) );
  CleanupStack::PopAndDestroy( cr );
  iShutterSndAlwaysOn = value;
        
  // get data for monitoring free RAM during sequence capture
  CamUtility::GetPsiInt( ECamPsiSnapshotSize, iSnapshotDataSize );

  iConfiguration->GetPsiIntArrayL( ECamPsiJpegDataSizes, iJpegDataSizes );
  iConfiguration->GetPsiIntArrayL( ECamPsiSequencePostProcessDataSizes,
                                   iSequencePostProcessDataSizes );
                               
  if ( iConfigManager && iConfigManager->IsCaptureToneDelaySupported() )
    {
    TInt captureToneDelay = iConfigManager->CaptureToneDelay();
    TCallBack captureToneCb( CCamAppController::CaptureToneDelayTimeout, this );
    iCaptureToneDelayTimer = 
        CCamTimer::NewL( captureToneDelay * KMicroSecsInMilliSec, 
                         captureToneCb );
    }

  PRINT( _L("Camera <> CCamAppController: Define Camera focused P&S key..") );
  err = RProperty::Define( KPSUidCamcorderNotifier, 
                                 KCCorFocused,
                                 RProperty::EInt );
  if ( KErrNone != err )
    {
    // Error handling. Remove warning.
    }
    
  if ( iConfigManager  && iConfigManager->IsPublishZoomStateSupported() )
      { // Publish zoom state
      iContextFwClient = CCFClient::NewL( *this );
      
      // Defining the context using the Context Framework.
      err = iContextFwClient->DefineContext(
                                            KCamContextSource,
                                            KCamContextType,
                                            KCamContextSecurity );
      if( KErrNone == err )
        {
        iDefineContextSuccessful = ETrue;
        PublishZoomStateL( EFalse );
        }
      else
        {
        // We make sure that we dont proceed with publishing the zoom state
        // if the context definition fails.
        iDefineContextSuccessful = EFalse;
        }      
      } // Publish zoom state

  iLocationTrailStarted = EFalse;

  CamUtility::GetPsiInt( ECamPsiMaxBurstCapture, iLongSequenceLimit );

  User::LeaveIfError( iFs.Connect() ); 
  iDriveChangeNotifier = CCamDriveChangeNotifier::NewL( iFs, *this ); 
  
  iTvAccessoryMonitor = CCamTvAccessoryMonitor::NewL( this );
  iTvAccessoryMonitor->StartListeningL();
  
  PRINT( _L("Camera <= CCamAppController::ConstructL"));
  }



/*
* CCamAppController::PublishZoomStateL
*/
void CCamAppController::PublishZoomStateL( const TBool aEnabled )
  {
  // We publish the zoom state only when context definition is
  // successful. the value of the zoom state is controlled by the
  // boolean flag "aEnabled".
  if ( iConfigManager  && iConfigManager->IsPublishZoomStateSupported() ) 
      {
      if ( iDefineContextSuccessful )
        {
        CCFContextQuery* query = CCFContextQuery::NewLC();
        query->SetSourceL( KCamContextSource );
        query->SetTypeL( KCamContextType );
        RContextObjectArray result;
        TInt err = iContextFwClient->RequestContext( *query, result );
        if( err == KErrNone  && result.Count() > 0 )
            {
            if ( aEnabled )
                {
                result[0]->SetValueL( KCamContextValueEnable );
                }
            else
                {
                result[0]->SetValueL( KCamContextValueDisable );
                }
            TInt err = iContextFwClient->PublishContext( *result[0] );
            if ( err != KErrNone )
                {
                // Define how to handle the error case.
                }
            }
        else
            {
            // ignoring the error case.
            }
            result.ResetAndDestroy();
            CleanupStack::PopAndDestroy( query );
        }      
      }
  }


   
// ---------------------------------------------------------------------------
// CCamAppController::IsViewFinding
// Check if viewfinder is active
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsViewFinding() const
  {
  __ASSERT_DEBUG( iCameraController, CamPanic( ECamPanicInvalidState ) ); 
  return ( iCameraController
        && iCameraController->ViewfinderState() == ECamTriActive );
  }
    


// ---------------------------------------------------------------------------
// CCamAppController::NotifyControllerObservers
// Notify the controller observers that a new controller event has occurred
// ---------------------------------------------------------------------------
//
void 
CCamAppController
::NotifyControllerObservers( TCamControllerEvent aEvent,
                             TInt                aError ) const
  {
  PRINT( _L("Camera => CCamAppController::NotifyControllerObservers"));
  // We might need to stop notification for some time.
  if( !iMuteNotifications )
    {
    for ( TInt i = 0; i < iControllerObservers.Count(); i++ )
      {
      // NON-NULL checked when adding observers.
      // If the notification leaves, continue for the other observers
      TRAP_IGNORE( iControllerObservers[i]->HandleControllerEventL( aEvent, aError ) )
      }
    }
  PRINT( _L("Camera <= CCamAppController::NotifyControllerObservers"));
  }



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void 
CCamAppController
::EnterViewfinderMode( const TCamCameraMode& aMode )
  {
  PRINT( _L("Camera => CCamAppController::EnterViewfinderMode"));

  TInt error( KErrNone );
  iPendingRelease = EFalse;

  // start monitoring mmc dismount notifications
  StartDismountMonitoring();
  
  if ( iConfigManager && iConfigManager->IsPublishZoomStateSupported() )
      {
      TRAP_IGNORE ( PublishZoomStateL( ETrue ) );
      }

  // Clear these just in case some video prepare is pending.
  iVideoPrepareNeeded = EFalse;

  
  if( ECamControllerVideo == aMode )
    {
    // Filename generation needs to be done in all case, not only
    // when the current path is null.  
    // if( KNullDesC() == iSuggestedVideoPath )
      {
      TRAP_IGNORE( GenerateNextValidVideoPathL() );
      }

    // Clear this to be sure it doesn't block recording
    iVideoRequested = EFalse;
    SetTargetMode     ( ECamControllerVideo  );
    SetTargetImageMode( ECamImageCaptureNone );
    // shutdown the orientation channel when going to videomode
    if ( iConfigManager && iConfigManager->IsOrientationSensorSupported() )
        {
	    TRAP_IGNORE( UpdateSensorApiL( EFalse ) );
		}
    }
  else if( ECamControllerImage == aMode )
    {
    SetTargetMode     ( ECamControllerImage    );
//    SetTargetImageMode( ECamImageCaptureSingle );
    SetTargetImageMode( CurrentImageModeSetup() );
    }
  else
    {
    SetOperation( ECamStandby, KErrGeneral );
    return;
    }

  iInfo.iTargetVfState = ECamTriActive;
  if( !Busy() )
    {
    TRAP( error, IssueModeChangeSequenceL() );
    if ( KErrNone    != error
      && ECamStandby != iInfo.iOperation )
      {
      SetOperation( ECamStandby, error );
      }
    }
  else
    {
    // operation will continue when busy sequence or request completes
    }
  PRINT( _L("Camera <= CCamAppController::EnterViewfinderMode"));
  }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void 
CCamAppController::ExitViewfinderMode( const TCamCameraMode& aMode )
  {
  PRINT( _L("Camera => CCamAppController::ExitViewfinderMode"));
  // The check here is to ensure that the viewfinder isn't stopped by the still precapture
  // view when it has just lost the foreground to the video precapture view and vice versa.
  // In that case the mode in parameter would not match the target mode.
  if( iInfo.iOperation != ECamStandby )
    {
    if ( aMode == iInfo.iTargetMode )
      {
      iInfo.iTargetVfState = ECamTriIdle;
      SetTargetImageMode( ECamImageCaptureNone );
  
      if( !Busy() )
        {
        if ( iConfigManager && iConfigManager->IsPublishZoomStateSupported() )
            {
            TRAP_IGNORE ( PublishZoomStateL( EFalse ) );            
            }
        TRAPD( error, IssueDirectRequestL( ECamRequestVfRelease ) );
        if ( KErrNone             != error
          && ECamStandby != iInfo.iOperation )
          {
          SetOperation( ECamStandby, error );
          }
        }
      else
        {
        // operation will continue when busy sequence or request completes
        }
      }
    }
  PRINT( _L("Camera <= CCamAppController::ExitViewfinderMode"));
  }


// ---------------------------------------------------------------------------
// CCamAppController::StartViewFinder();
// Non-leaving version of StartViewFinderL() 
// ---------------------------------------------------------------------------
//
void 
CCamAppController::StartViewFinder()
  {
  PRINT( _L("Camera => CCamAppController::StartViewFinder" ))
  iInfo.iTargetVfState = ECamTriActive; 

  if( !Busy() )
    {
    OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_STARTVIEWFINDER, "e_CAM_APP_VF_INIT 0" );   //CCORAPP_APP_VF_INIT_END
    
    TRAPD( error, IssueDirectRequestL( ECamRequestVfStart ) );
    if ( KErrNone    != error
      && ECamStandby != iInfo.iOperation )
      {
      PRINT( _L("Camera <> CCamAppController::StartViewFinder - [ERROR] VF start failed, go to standby" ))
      SetOperation( ECamStandby, error );
      }
    }
  else
    {
    // Vf started at earliest convenient time when ongoing requests finish.
    }
  PRINT(_L("Camera <= CCamAppController::StartViewFinder") );
  }

// ---------------------------------------------------------------------------
// StopViewFinder
// Stop the viewfinder immediately
// ---------------------------------------------------------------------------
//
void 
CCamAppController::StopViewFinder()
  {
  PRINT( _L("Camera => CCamAppController::StopViewFinderNow" ) )
  __ASSERT_DEBUG( iCameraController, CamPanic( ECamPanicInvalidState ) ); 
  
  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_STOPVIEWFINDER, "e_CAM_APP_STOP_VF 1" );  //CCORAPP_STOP_VF_START

  if ( iBacklightTimer && iBacklightTimer->IsActive() )
    {
    iBacklightTimer->Cancel();
    }
  
  // Only stop if active.
  // Idle and inactive need no action at this point.
  if( ECamTriActive == iCameraController->ViewfinderState() )
    {
    iInfo.iTargetVfState = ECamTriInactive;
    
    // Can only fail to OOM when state is checked.
    TRAPD( error, iCameraController->DirectRequestL( ECamRequestVfStop ) );
    if( KErrNone             != error 
     && ECamStandby != iInfo.iOperation )
      {
      SetOperation( ECamStandby, error );
      }
    }
    
  OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMAPPCONTROLLER_STOPVIEWFINDER, "e_CAM_APP_STOP_VF 0" ); //CCORAPP_STOP_VF_END
    
  PRINT( _L("Camera <= CCamAppController::StopViewFinderNow" ) )
  }

// ---------------------------------------------------------------------------
// StopViewFinderEcam
// Stop the viewfinder immediately without changing states
// ---------------------------------------------------------------------------
//
void 
CCamAppController::StopViewFinderEcam()
  {
  PRINT( _L("Camera => CCamAppController::StopViewFinderEcam" ) )
  __ASSERT_DEBUG( iCameraController, CamPanic( ECamPanicInvalidState ) ); 
  
  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_STOPVIEWFINDERECAM, "e_CAM_APP_STOP_VF 1" );  //CCORAPP_STOP_VF_START
  
  // Trap and ignore error
  TRAP_IGNORE( iCameraController->DirectRequestL( ECamRequestVfStopEcam ) );
  
  OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMAPPCONTROLLER_STOPVIEWFINDERECAM, "e_CAM_APP_STOP_VF 0" ); //CCORAPP_STOP_VF_END
    
  PRINT( _L("Camera <= CCamAppController::StopViewFinderEcam" ) )
  }

// ---------------------------------------------------------------------------
// CCamAppController::FreezeViewFinder
// Copy the next viewfinder frame then stop the viewfinder
// ---------------------------------------------------------------------------
//
void CCamAppController::FreezeViewFinder( TBool aFreeze )
  {
  // If the viewfinder needs to freeze
  if ( aFreeze )
    {
    PERF_MESSAGE_L2( EPerfMessagePausingViewFinder );
    iFreezeViewFinder  = ETrue;
    iUpdateFrozenFrame = ETrue;
    }
  // Restarting the viewfinder
  else 
    {
    iFreezeViewFinder  = EFalse;
    iUpdateFrozenFrame = EFalse;
    if ( ECamTriActive != iCameraController->ControllerInfo().iVfState )
      {
      StartViewFinder();
      }
    }
  }


// ---------------------------------------------------------------------------
// CCamAppController::SetKeyUp
// Set  iKeyUP value which is used to determine if burst
// timer should be activated or not
// ---------------------------------------------------------------------------
//
void CCamAppController::SetKeyUp( TBool aValue /*= EFalse*/ )
    {
    PRINT1(_L("Camera => CCamAppController::SetKeyUp, iKeyUP=%d"), aValue );
    iKeyUP=aValue; 
    }

// ---------------------------------------------------------------------------
// CCamAppController::DoCaptureL
// Initiate image capture
// ---------------------------------------------------------------------------
//
TBool CCamAppController::DoCaptureL()
  {
  PRINT( _L("Camera => CCamAppController::DoCaptureL()"));
  PRINT2(_L("Camera <> mode[%s] imagemode[%s]"), KCamModeNames[iInfo.iMode], KCamImageModeNames[iInfo.iImageMode] );

  // ReleaseArray();
  if( iImageSaveActive->Count() <= 0 )
      {
      ReleaseArray( ETrue );
      }
  iCurrentImageIndex = 0;
  iArrayUsageCount   = KImageArrayUsers;

  iCameraWoken = EFalse;
    
  TBool continueWithCapture = ETrue;
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );  
  
  // Cancel capture if there are settings pending
  if ( IsFlagOn( iBusyFlags, EBusySetting ) )
      {
      continueWithCapture = EFalse;
      }

  switch( iInfo.iImageMode )
    {
    // -----------------------------------------------------
    // Burst capture
    case ECamImageCaptureBurst:
      {
      PRINT( _L("Camera <> CCamAppController::DoCaptureL .. case ECamImageCaptureBurst") );
      /*
      TCamPhotoSizeId sizeId( GetCurrentImageResolution() );

      TInt memoryNeed( iJpegDataSizes[sizeId]
                     + iSequencePostProcessDataSizes[sizeId] 
                     + iSnapshotDataSize );  
      TInt memoryFree( 0 );
      HAL::Get( HALData::EMemoryRAMFree, memoryFree ); // error ignored

      if( memoryFree < memoryNeed )
        {      
        PRINT( _L( "Camera <> Not enough memory for more burst captures - complete capture" ) );  
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        // Force re-prepare of engine
        appUi->HandleCommandL( ECamCmdSequence );
                
        // Display out of memory note
        NotifyControllerObservers( ECamEventSnapshotReady, KErrNoMemory );
                
        // Return app to burst precapture state from burst in-capture
        SetOperation( ECamNoOperation );
        NotifyControllerObservers( ECamEventCaptureComplete );  
                
        // Force update of UI furniture
        NotifyControllerObservers( ECamEventCounterUpdated );
        appUi->UpdateCba();

        iCaptureRequested = EFalse;
        HandleCaptureCompletion();
        continueWithCapture = EFalse;
        }
      else
        {
        iSequenceCaptureInProgress = ETrue;  
        iActualBurstCount = 0;
        FreezeViewFinder( ETrue );
        // Stop the screensaver from coming on when burst mode is active
        EnableScreenSaver( EFalse );
        // Make navi counter update itself
        NotifyControllerObservers( ECamEventCounterUpdated );
        }
      */

      if( !appUi->SelfTimerEnabled() )
        {
        // Start timer to change burst mode to press and hold
        // if not determined to be short burst before that.
        
        //One such case is if capturekeyup event has occured before
        //DocaptureL method
        if( !iKeyUP && !iNoBurstCancel )
            {
            StartBurstTimerL();            
            }
        }
      iSequenceCaptureInProgress = ETrue;  
       iValueIsStored = EFalse;
      FreezeViewFinder( ETrue );
      EnableScreenSaver( EFalse );
	  if(  iBacklightTimer 
       && !iBacklightTimer->IsActive() )
        {
        iBacklightTimer->Start( 0,
                                KBacklighTimerInterval, 
                                TCallBack( ResetInactivityTimer, this ) );
        }
      NotifyControllerObservers( ECamEventCounterUpdated );

      break;
      }
    // -----------------------------------------------------
    // Timelapse capture
    case ECamImageCaptureTimeLapse:
      {
      PRINT( _L("Camera <> CCamAppController::DoCaptureL .. case ECamImageCaptureTimeLapse") );
      iSequenceCaptureInProgress = ETrue;  
      // Stop the screensaver from coming on when burst mode is active
      EnableScreenSaver( EFalse );
            
      // Get the remaining image count and only start the timer for the next capture if there is enough
      // space, taking into account that the current capture will also use up one image     
      TCamMediaStorage storage( (TCamMediaStorage)IntegerSettingValue( ECamSettingItemPhotoMediaStorage ) );
      TCamPhotoSizeId  sizeId ( GetCurrentImageResolution() );

      TInt remainingCount = ImagesRemaining( storage, ETrue, sizeId );

      // Check if the advanced sequence is limited (by starting from selftimer)
      if( remainingCount  >= 1
       && iRequestedCaptureCount <= TimeLapseImageCount()+1 )
        {
        iCompleteTimeLapsePending = ETrue;
        }
            
      // if there isn't space for this capture (e.g. some memory has been used since the timer was initiated
      // or there isn't enough space to initiate a further capture
      else if ( remainingCount <= 1 )
        {
        PRINT( _L("Camera <> CCamAppController::DoCaptureL .. only space for one capture, stop timer") );

        // The timer is no longer needed
        if ( iTimeLapseTimer )
          {
          iTimeLapseTimer->Cancel();
          delete iTimeLapseTimer;
          iTimeLapseTimer = NULL;
          }
        // if there isn't even enough space for this capture
        if ( remainingCount < 1 ) 
          {
          // complete the timelapse capture now (prevent the current capture from starting)
          continueWithCapture = EFalse;
          iArrayUsageCount = 0;
          CompleteTimeLapseOperation();
          }
        else // if there isn't enough space to initiate a further capture
          {
          // ensure that the time lapse capture completes after the current capture
          iCompleteTimeLapsePending = ETrue;
          }                   
        }
            
      else // there is space to initiate a further capture
        {                
        PRINT( _L("Camera <> CCamAppController::DoCaptureL .. space for more than this capture, timer set on..") );
        if ( !iTimeLapseTimer )
          {
          RestartTimeLapseTimerL();
                
          //Products which support autofocus needs to reduce time for second picture
          //about the time needed by autofocus.
          if( iConfigManager && iConfigManager->IsAutoFocusSupported() 
              && !CurrentSceneHasForcedFocus() )
            {
            iTimeLapseStartTime = iTimeLapseStartTime - TTimeIntervalSeconds( KFocusingDurationSeconds );
            }
          }
        }
      break;
      }
    // -----------------------------------------------------
    // Single capture
    case ECamImageCaptureSingle:
      PRINT( _L("Camera <> CCamAppController::DoCaptureL .. case ECamImageCaptureSingle") );
      // Should not take so long to get the image captured and saved,
      // but 
      EnableScreenSaver( EFalse );
      SetCaptureKeyPressedWhileImageSaving(EFalse);
      break;
    // -----------------------------------------------------
    default:
      PRINT(_L("Camera <> Not in any image capture mode, LEAVE!!"));
      User::Leave( KErrGeneral );
      break;
    // -----------------------------------------------------
    }

  if ( continueWithCapture )
    {
    IssueRequestL( ECamRequestImageCapture );
    // Operation in progress is updated in HandleCameraEventL
    // store the orientation at capture time, needed in thumbnail creation
    iCaptureOrientation = iLastImageOrientation;
    }

  iCaptureStoppedForUsb = EFalse; 
  PRINT( _L("Camera <= CCamAppController::DoCaptureL()"));
  return continueWithCapture;
  }


// ---------------------------------------------------------------------------
// CCamAppController::DoRecord
// Initiate video recording
// ---------------------------------------------------------------------------
//
void CCamAppController::DoRecord()
  {
  PRINT( _L("Camera => CCamAppController::DoRecord") );
  __ASSERT_DEBUG( iCameraController, CamPanic( ECamPanicInvalidState ) );

  StopIdleTimer();

  // No key sounds when recording
  TRAP_IGNORE( iSoundPlayer->DisableAllKeySoundsL() );

  PRINT( _L("Camera <> Requesting video capture start..") );
  TRAPD( error, IssueRequestL( ECamRequestVideoStart ) );
  PRINT1( _L("Camera <> ..request issued, status:%d"), error );

  if( KErrNone != error ) SetOperation( ECamStandby, error );    
  else                    SetOperation( ECamCapturing );

  // Request is handled
  iVideoRequested = EFalse;

  iCaptureStoppedForUsb = EFalse;
  PRINT( _L("Camera <= CCamAppController::DoRecord") );
  }


// ---------------------------------------------------------------------------
// CCamAppController::ReserveFileNamesL
// Reserve a set of filenames for a burst capture
// ---------------------------------------------------------------------------
//
void 
CCamAppController::ReserveFileNamesL( 
    const TCamCameraMode&        aCaptureMode,
    const TCamImageCaptureMode&  aImageMode,
          TBool                  aForcePhoneMem /*= EFalse*/ )
  {
  PRINT( _L("Camera => CCamAppController::ReserveFileNamesL") )
  TInt captureCount = 1;
  TInt store        = ECamMediaStorageNone; 
  TBuf<KMaxExtension> extension; 

  // If the next file will be a photo
  if ( ECamControllerImage == aCaptureMode )
    {         
    extension = KImageExtension;
    }
  // Otherwise the next file will be a video
  else
    {
#ifndef __WINS__
    TCamVideoFileType fileType = static_cast<TCamVideoFileType> 
        ( IntegerSettingValue( ECamSettingItemVideoFileType ) );
        
    extension = ( ECamVideoMpeg4 == fileType )
                ? KVideoMP4Extension
                : KVideo3GPExtension;
#else
    extension = KVideo3GPExtension;
#endif
    }

  store = ( ECamControllerImage == aCaptureMode ) ? 
                 CurrentPhotoStorageLocation() : CurrentVideoStorageLocation();

  TPath basePath;

  // Take a snapshot of the date - to ensure a sequence on a date boundary isn't split into
  // different months
  TTime now;
  now.HomeTime();
  // Need to check that there is enough space for the files. If this is a sequence of 20 images or 
  // less then ensure there is enough space in the current folder, otherwise only one space is needed
  TInt guaranteedGroup = 1;
  if ( captureCount < CCamFolderUtility::KMaxGuaranteedBurstGroup )
    {
    guaranteedGroup = captureCount;
    }
  
  // Leaves if errors generating the path
  CCamFolderUtility::GetBasePathL( iMonthFolderCounters, 
                                   store, 
                                   basePath, 
                                   aCaptureMode, 
                                   ETrue, 
                                   guaranteedGroup, 
                                   now ); 
     
  iCurrentStorage = static_cast<TCamMediaStorage>( store );  
//  TInt numberSettingId( KErrNotFound );
  TInt typeSettingId( KErrNotFound );
  TInt baseSettingId( KErrNotFound );
  TInt counter( 0 );

  if( ECamControllerVideo == aCaptureMode )
    {
//    numberSettingId = ECamSettingItemVideoNumber;
    counter         = IntegerSettingValue( ECamSettingItemVideoNumber );
    typeSettingId   = ECamSettingItemVideoNameBaseType;
    baseSettingId   = ECamSettingItemVideoNameBase;
    }
  else
    {
//    numberSettingId = ECamSettingItemPhotoNumber;
    counter         = iImageNumberCache;
    typeSettingId   = ECamSettingItemPhotoNameBaseType;
    baseSettingId   = ECamSettingItemPhotoNameBase;
    }

//  TInt counter( IntegerSettingValue( numberSettingId ) );
  TBuf<KCamMaxDateLen> dateNameBase;
  TPtrC nameBase;
  if ( ECamNameBaseDate == iSettingsModel->IntegerSettingValue( typeSettingId ) )
      {       
      CamUtility::FormatDateFileNameL( dateNameBase );
      nameBase.Set( dateNameBase );
      }
    else
      {
      nameBase.Set( TextSettingValue( baseSettingId ) );
      }

  RFs rfs;
  User::LeaveIfError( rfs.Connect() );
  CleanupClosePushL( rfs );        
        
  TEntry entry;

  TInt namedImages = 0;
  while ( namedImages < captureCount )
    {
    // Generate new name
    TFileName newFileName;

    CamUtility::FormatFileName( counter, nameBase, newFileName );

    TFileName newPath( basePath );
    newPath.Append( newFileName );
    newPath.Append( extension );

    // if there is no file in the current location with this name
    TEntry entry;  // Prevent LINT error
    TInt ferr = rfs.Entry( newPath, entry );
    if ( KErrNotFound == ferr )
      {
      // A file with that name was not found, so the filename is okay.
      PRINT( _L("Camera <> Filename OK.") )
      namedImages++;
      StoreNextFileNameL( newPath );
          
      // If this is time lapse capture then the full set of file names
      // will be needed for the 'delete sequence' option. The burst capture array
      // in timelapse only holds data for one item at a time so a separate array
      // is used to keep track of all the images file names. The redundancy in filename
      // storage is limited to a single file name.             
      if ( ECamImageCaptureTimeLapse == aImageMode )         
        {                                     
        TRAP_IGNORE( iSequenceFilenameArray->AppendL( newPath ) );
        // Update the images captured count
        NotifyControllerObservers( ECamEventCounterUpdated );
        }
      counter++;
      }
    else if ( KErrNone == ferr )
      {
      // A file with that name exists already. Increment the counter and try again.
      PRINT( _L("Camera <> Filename already in use!") ) 
      counter++;   // Try the next index number
      }
    else
      {
      // There was some other error. Can not save file -> Leave.
      PRINT1( _L("Camera <> File server returned error: %d"), ferr );
      User::Leave( ferr );
      }
      
    }
  CleanupStack::PopAndDestroy( &rfs );

  // SetIntegerSettingValueL( numberSettingId, counter );
  if( ECamControllerVideo == aCaptureMode )
    {
    SetIntegerSettingValueL( ECamSettingItemVideoNumber, counter );
    }
  else
    {
    // SetIntegerSettingValueL called when capture ends. 
    iImageNumberCache = counter;
    }

  PRINT( _L("Camera <= CCamAppController::ReserveFileNamesL") )
  }

    
// ---------------------------------------------------------------------------
// CCamAppController::CreateDummyFileL
// Create dummy file ready for image storage
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::CreateDummyFileL( RFs& aFs, const TDesC& aFullPath )
  {
  PRINT1( _L("Camera => CCamAppController::CreateDummyFileL [%S]"), &aFullPath );  
  RFile file;
  CleanupClosePushL( file );
  
  TInt err = file.Create( aFs, aFullPath, EFileWrite );
  if ( err == KErrNone )
    {
    // Still image files are made writable and non-hidden
    // just before the save operation
    // file is set to hidden so that media gallery does not detect it
    file.SetAtt( KEntryAttReadOnly|KEntryAttHidden, 0 );
    }
  
  CleanupStack::PopAndDestroy( &file );
  
  PRINT1( _L("Camera <= CCamAppController::CreateDummyFileL, status:%d"), err );  
  return err;
  }
  
  
// ---------------------------------------------------------------------------
// CCamAppController::StoreNextFileNameL
// Add newly generated filename to array of names
// ---------------------------------------------------------------------------
//
void CCamAppController::StoreNextFileNameL( const TDesC& aFullPath )
    {
    PRINT( _L("Camera => CCamAppController::StoreNextFileNameL") );
    TParse parse;
    parse.Set( aFullPath, NULL, NULL );
    
    // Store the path + filename and plain filename for future reference
    if( ECamControllerVideo == iInfo.iMode && BurstCaptureArray()->Count() > 0 )
      {
       BurstCaptureArray()->SetNameL( aFullPath, parse.Name(), 0 );	
      }
    else
      {
      BurstCaptureArray()->SetNextNameL( aFullPath, parse.Name() );		
      }
    
    PRINT( _L("Camera <= CCamAppController::StoreNextFileNameL") );
    }

// ---------------------------------------------------------------------------
// CCamAppController::GenerateNextVideoFilePathL
// Generate a new file name and full path to save the next
// video 
// ---------------------------------------------------------------------------
//
void CCamAppController::GenerateNextVideoFilePathL( TBool aForcePhoneMem )
  {
  PRINT1( _L( "Camera => CCamAppController::GenerateNextVideoFilePathL, force phone mem:%d" ), aForcePhoneMem )    
  if( aForcePhoneMem )
    {
    ForceUsePhoneMemoryL( ETrue );
    }
  TInt store = IntegerSettingValue( ECamSettingItemVideoMediaStorage ); 
  
  TBuf<KMaxExtension> extension;        

#ifdef __WINS__
  extension = KVideo3GPExtension;
#else
  // If the video file type is MP3, then display this icon.
  TCamVideoFileType fileType = 
    static_cast<TCamVideoFileType>( 
        IntegerSettingValue( ECamSettingItemVideoFileType ) );

  extension = (fileType == ECamVideoMpeg4)
            ? KVideoMP4Extension
            : KVideo3GPExtension;
#endif
  
  TBuf<KCamMaxDateLen> dateNameBase;
  TPtrC baseVideoName;
          
  if ( iSettingsModel->IntegerSettingValue( 
                        ECamSettingItemVideoNameBaseType ) == ECamNameBaseDate )
    {       
    CamUtility::FormatDateFileNameL( dateNameBase );
    baseVideoName.Set( dateNameBase );
    }
  else
    {
    baseVideoName.Set( TextSettingValue( ECamSettingItemVideoNameBase ) );
    }

  TFileName nextFileName( baseVideoName );

  store = ( ECamControllerImage == iInfo.iMode ) ? 
                CurrentPhotoStorageLocation() : CurrentVideoStorageLocation();  

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

  if ( IsAppUiAvailable() &&
       appUi &&
       appUi->IsMemoryFullOrUnavailable( ECamMediaStorageCard ) &&
       ECamMediaStorageCard == store )
    {
    if ( ExistMassStorage() )
      {
      // with multiple drives, the mass memory is the default
      PRINT( _L("Camera <> Force ECamMediaStorageMassStorage") );
      store = ECamMediaStorageMassStorage;
      }
    else
      {
      PRINT( _L("Camera <> Force ECamMediaStoragePhone") );
      store = IntegerSettingValue(ECamSettingItemRemovePhoneMemoryUsage)?
                  ECamMediaStorageNone:
                  ECamMediaStoragePhone;
      }
    }

  TInt fileNameCounter = IntegerSettingValue( ECamSettingItemVideoNumber );
 
  // Update contents of iSuggestedVideoPath (drive and folders only)
  CCamFolderUtility::GetBasePathL( iMonthFolderCounters, 
                                   store,
                                   iSuggestedVideoPath,
                                   ECamControllerVideo,
                                   ETrue );
                                   
  iCurrentStorage = static_cast<TCamMediaStorage>( store );   
  // Update contents of iSuggestedVideoPath (appending filename and extension)
  CamUtility::GetUniqueNameL( iSuggestedVideoPath, nextFileName, fileNameCounter, extension );
  PRINT( _L( "Camera <= CamAppController::GenerateNextVideoFilePathL" ) )
  }

// ---------------------------------------------------------------------------
// CCamAppController::GenerateNextValidVideoPathL
// Generate a new file name and full path to save the next video.
// If the generated file (using settings to suggest the drive) can not be opened
// try again, forcing the file to be on C: instead.
// ---------------------------------------------------------------------------
//
void CCamAppController::GenerateNextValidVideoPathL()
  {
  PRINT( _L("Camera => CCamAppController::GenerateNextValidVideoPathL()"));    
  
  // If USB is active, use phone memory
  if( CamUtility::IsUsbActive() )
      {
      GenerateNextVideoFilePathL( ETrue );	
      }
  else
      {    	
      // Generate a file path using the settings
      TRAPD( err, GenerateNextVideoFilePathL() );
      
      // If left (possibly due to corrupt MMC), try again with phone memory
      if ( err )
          {
          // If we have got here, we can't access MMC. 
          // Don't switch to phone memory, but prepare the engine with a video
          // name in phone memory. This will be reset if needed when the user actually
          // starts recording
          PRINT( _L( "Could not generate next file path!  Corrupt MMC?" ) );
          GenerateNextVideoFilePathL( ETrue );    
          }
      }

  PRINT( _L("Camera <= CCamAppController::GenerateNextValidVideoPathL()"));        
  }

// ---------------------------------------------------------------------------
// CCamAppController::GenerateNextValidImagesPathL
// Generate a new path to save the next image.
// ---------------------------------------------------------------------------
//
void CCamAppController::GenerateNextValidImagesPathL()
  {
  PRINT( _L("Camera => CCamAppController::GenerateNextValidImagesPathL()"));    
  // Generate a file path using the settings
  // Ensure the image folder exists
  TFileName path;
  TInt storage = IntegerSettingValue( ECamSettingItemPhotoMediaStorage );
  CCamFolderUtility::GetBasePathL( iMonthFolderCounters,
                                   storage,
                                   path,
                                   ECamControllerImage,
                                   EFalse ); 
                                   
  iCurrentStorage = static_cast<TCamMediaStorage>( storage );                                 
  PRINT( _L("Camera <= CCamAppController::GenerateNextValidImagesPathL()"));        
  }    


// ---------------------------------------------------------------------------
// CCamAppController::CaptureModeTranstionInProgress
// Report whather a capture mode is currently being prepared
// ---------------------------------------------------------------------------
//
TBool CCamAppController::CaptureModeTransitionInProgress()
  {
  PRINT1( _L("Camera => CCamAppController::CaptureModeTransitionInProgress (%d)"), iCaptureModeTransitionInProgress )
  return iCaptureModeTransitionInProgress;
  }

// ---------------------------------------------------------------------------
// CCamAppController::HandleSaveEvent
// From MCamImageSaveObserver.
// Handle completion of image saving
// ---------------------------------------------------------------------------
//
void 
CCamAppController::HandleSaveEvent( TCamSaveEvent aEvent )
  {
  PRINT3( _L( "Camera => CCamAppController::HandleSaveEvent .. event[%s] imagemode[%s] operation[%s]" ),
          KCamSaveEventNames       [aEvent          ],
          KCamImageModeNames       [iInfo.iImageMode], 
          KCamCaptureOperationNames[iInfo.iOperation] )
   
  TBool mediaFileChanged = EFalse;     
  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  if( ( iCaptureArray->ImagesRemaining() <= 0 ) 
          && appUi 
          && appUi->IsInPretendExit() )
      {
      ReleaseArray( ETrue );
      }
  
  if( ECamSaveEventCriticalError == aEvent )
    {
    PRINT( _L( "Camera <> CCamAppController::HandleSaveEvent: critical error, going to standby.." ) )
    SetOperation( ECamStandby, KErrGeneral );
    PRINT( _L( "Camera <= CCamAppController::HandleSaveEvent" ) )
    return;
    }

  if( aEvent == ECamSaveEventSaveError ) 
    {
    PRINT( _L("Camera <> CCamAppController::HandleSaveEvent - ECamSaveEventSaveError!") );
    if( iInfo.iImageMode == ECamImageCaptureBurst &&
        SequenceCaptureInProgress() )
        {
        // Cancel ongoing burst capture
        // Burst capture is normally cancelled by setting iRequestedBurstCount,
        // and actual cancelling is done when next imagedata arrives.
        // However, in this case camera is going to be released, and there won't
        // be next image data, so give ECamRequestImageCancel and capture
        // completion event here to avoid problems
        PRINT( _L("Camera <> CCamAppController::HandleSaveEvent .. Cancel burst due to error") ); 
        TRAP_IGNORE ( IssueDirectRequestL( ECamRequestImageCancel ) );
        NotifyControllerObservers( ECamEventCaptureComplete, KErrCancel );   
        }
    else
        {
        PRINT( _L( "Camera <> CCamAppController::HandleSaveEvent: critical error, going to standby.." ) )
        SetOperation( ECamStandby, KErrGeneral );
        }   
    PRINT( _L("Camera <= CCamAppController::HandleSaveEvent - ECamSaveEventSaveError!") );
    return;
    }

  // if a save completes 
  if ( aEvent == ECamSaveEventComplete )
    {
    PRINT( _L("Camera <> CCamAppController::HandleSaveEvent .. ECamSaveEventComplete") );

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
    if( ECamImageCaptureBurst != iInfo.iImageMode )
      {
      // Do not log in burst mode
      PERF_EVENT_END_L1( EPerfEventShotToSave );      
      }
#endif

    OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_HANDLESAVEEVENT, "e_CAM_PRI_SHOT_TO_SAVE 0" );  //CCORAPP_PRI_SHOT_TO_SAVE_END

    // Check that this is the completion of the last pending 
    // single image save.
    // Note burst capture completion is handled separately
    TBool savedAll   = ( iImageSaveActive->Count()== 0                );
    TBool completing = ( ECamCompleting           == iInfo.iOperation );

    switch( iInfo.iImageMode )
      {
      case ECamImageCaptureNone: // In postcapture, waiting for saving
      case ECamImageCaptureSingle:
        PRINT( _L("Camera <> CCamAppController::HandleSaveEvent .. case ECamImageCaptureSingle/None") );
        if( savedAll )
          {
          OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMAPPCONTROLLER_HANDLESAVEEVENT, "e_CAM_PRI_SERIAL_SHOOTING 0" );    //CCORAPP_PRI_SERIAL_SHOOTING_END
    
          PRINT( _L("Camera <> CCamAppController::HandleSaveEvent .. calling HandleCaptureCompletion()") );
          if( !iSaveStarted )
              {
              HandleCaptureCompletion();
              NotifyControllerObservers( ECamEventCaptureComplete, KErrNone );
              }
          else
              {
              iSaveStarted = EFalse;
              }
          
          // If we have returned from postcapture to precapture before 
          // the filename has been sent to the gallery (i.e. pressing back
          // key during image processing), iArrayUsageCount will be reduced 
          // to zero and capture array will be released. In that case, we must
          // send the filename to gallery before it happens.
          if ( iArrayUsageCount == 1 )
              {
              NotifyControllerObservers( ECamEventMediaFileChanged );   
              mediaFileChanged = ETrue;
              }

          CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
          if( iImageSaveActive && IsAppUiAvailable() )
              {
              if( iImageSaveActive->Count() <= 0 && ECamViewStatePostCapture != appUi->CurrentViewState() )
                  {
                  ReleaseArray();
                  }
              }
          }
        break;
      case ECamImageCaptureTimeLapse:
        PRINT( _L("Camera <> CCamAppController::HandleSaveEvent .. case ECamImageCaptureTimeLapse") );
        // If image capture has finished, but waiting for file to be saved.
        if( ECamNoOperation == iInfo.iOperation || ECamCompleting == iInfo.iOperation )
          {
          PRINT( _L("Camera <> CCamAppController .. current file saved, complete this capture") );
          SetOperation( ECamNoOperation );
          ReleaseArray();
          }
        break;        
      case ECamImageCaptureBurst:
        PRINT( _L("Camera <> CCamAppController::HandleSaveEvent .. case ECamImageCaptureBurst") );
        // After the whole burst has stopped, we get Image Stop event from Camera Controller,
        // and enter completing state.
        if( completing && savedAll )
          {
          // Modify this event to "burst complete" one.
          aEvent = ECamSaveEventBurstComplete;
          }
        break;
      default:
        break;
      } 

    NotifyControllerObservers( ECamEventSaveComplete );
    }
  
  if ( aEvent == ECamSaveEventStarted )
     {
     if( iInfo.iImageMode == ECamImageCaptureSingle )
         {
         PRINT( _L("Camera <> CCamAppController::HandleSaveEvent .. Saving started") );
         OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMAPPCONTROLLER_HANDLESAVEEVENT, "e_CAM_APP_PREP_FOR_NEXT_SHOT 1" ); //CCORAPP_PREP_FOR_NEXT_SHOT_START      
         HandleCaptureCompletion();
         NotifyControllerObservers( ECamEventCaptureComplete, KErrNone );
         iSaveStarted = ETrue;
         NotifyControllerObservers( ECamEventSaveComplete );
         iCaptureRequested = EFalse;
         OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMAPPCONTROLLER_HANDLESAVEEVENT, "e_CAM_APP_PREP_FOR_NEXT_SHOT 0" ); //CCORAPP_PREP_FOR_NEXT_SHOT_END
         OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMAPPCONTROLLER_HANDLESAVEEVENT, "e_CAM_PRI_SHOT_TO_SHOT 0" ); //CCORAPP_PRI_SHOT_TO_SHOT_END   
         }
     }
     
  // If the user cancelled a save
  if ( aEvent == ECamSaveEventUserCancelled )
    {
    PRINT( _L( "Camera <> CCamAppController::HandleSaveEvent .. ECamSaveEventUserCancelled" ) )
    //iInfo.iImageMode = ECamImageModeNone;
    // Make sure that any further engine callbacks are ignored until next capture
    SetOperation( ECamNoOperation );

    // Delete dummy files for unsaved images
    TInt nextSavingImage = BurstCaptureArray()->NextFileIndex();
    TInt lastImage = BurstCaptureArray()->Count();
    TInt i;
    // delete the next and following files
    for ( i = nextSavingImage; i < lastImage; i++ )
      {
      SetAsCurrentImage( i );
      DeleteCurrentFile();
      }
    SetAsCurrentImage( 0 );
    // Empty out the array - this ensures the thumbnail view doesn't open
    BurstCaptureArray()->Reset();

    NotifyControllerObservers( ECamEventSaveCancelled );
    }
    
  // if a burst capture has completed
  if ( aEvent == ECamSaveEventBurstComplete )
    {
    CompleteBurstOperation();

    if( ECamCompleting == iInfo.iOperation )
      {
      PRINT( _L( "Camera <> CCamAppController::HandleSaveEvent .. completing burst capture" ) )
      SetOperation( ECamNoOperation );
      NotifyControllerObservers( ECamEventCaptureComplete, KErrNone );
      ReleaseArray();
      }

    OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMAPPCONTROLLER_HANDLESAVEEVENT, "e_CAM_PRI_SERIAL_SHOOTING 0" );  //CCORAPP_PRI_SERIAL_SHOOTING_END
    }
  else
    {
    if ( iImageSaveActive->Count() == 0 || aEvent == ECamSaveEventStarted )
      {
      iSaving = EFalse;
      }        
    }
  if ( !mediaFileChanged )
      {
      NotifyControllerObservers( ECamEventMediaFileChanged );    
      }

  PRINT( _L( "Camera <= CCamAppController::HandleSaveEvent" ) )
  }


// ---------------------------------------------------------------------------
// HandleViewfinderFrame
// ---------------------------------------------------------------------------
//
void 
CCamAppController::HandleViewfinderFrame( TInt        /*aStatus*/, 
                                          CFbsBitmap* /*aFrame*/ )
  {
  PRINT_FRQ( _L( "Camera => CCamAppController::HandleViewfinderFrame" ) );   

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
  // This function is called when a viewfinder bitmap arrives
  if( EPerfWaitingForStartup == iPerformanceState )
    {
    iPerformanceState = EPerfIdle;
    PERF_EVENT_END_L1( EPerfEventApplicationStartup );
    }   
  else if( EPerfWaitingForStillMode == iPerformanceState )
    {
    iPerformanceState = EPerfIdle;
    PERF_EVENT_END_L1( EPerfEventSwitchToStillMode );
    }
  else if( EPerfWaitingForVideoMode == iPerformanceState )
    {
    iPerformanceState = EPerfIdle;
    PERF_EVENT_END_L1( EPerfEventSwitchToVideoMode );
    }
  else if( EPerfWaitingForBurstFrame == iPerformanceState )
    {
    iPerformanceState = EPerfIdle;
    PERF_EVENT_END_L1( EPerfEventBurstCaptureMomentToViewfinderFrame );
    }
#endif

  if( ECamControllerVideo == iInfo.iMode )
    {
    // removed first vf frame usage as snapshot.
    // snapshot received now from Camera 
    }
  else if( ECamControllerImage == iInfo.iMode )
    {
    // No need to duplicate here.
    }
  else
    {
    // Not valid state for vf frame.
    __ASSERT_DEBUG( EFalse, CamPanic( ECamPanicInvalidState ) );
    }

  if ( iZoomWaitingForCamera )
    {
    iZoomWaitingForCamera = EFalse;
    if ( iNewZoomPending )
      {
      SetZoomValue( iDesiredZoomVal );
      }
    }
  PRINT_FRQ( _L( "Camera <= CCamAppController::HandleViewfinderFrame" ) );
  }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleSnapshotEvent( TInt aStatus,
                                             CFbsBitmap* aBitmap )
  {
  PRINT1( _L( "Camera => CCamAppController::HandleSnapshotEvent, status:%d" ), aStatus );

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
  if( ECamImageCaptureBurst != iInfo.iImageMode )
    {
    // Do not log in burst mode
    PERF_EVENT_END_L1( EPerfEventShotToSnapshot );      
    }
  else
    {
    // In burst mode    
    PERF_MESSAGE_L2( EPerfMessageBurstSnapshotReady );
    }
#endif   

  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_HANDLESNAPSHOTEVENT, "e_CAM_APP_SHOT_TO_SNAPSHOT 0" );    //CCORAPP_SHOT_TO_SNAPSHOT_END

// ---------------------------------------------------------------------------
// Video snapshot handling
  if( ECamControllerVideo == iInfo.iMode )
    {
    PRINT( _L("Camera <> Snapshot in video mode") ); 
    if( KErrNone == aStatus )
      {
      // Make a copy of the snapshot and set it as current video ss.        
      CopySnapshotIfNeeded( *aBitmap, aStatus );
      if( iSnapShotCopy )
        {
        if (BurstCaptureArray()->Count() > 0 )
          {
          const CFbsBitmap* tempSnapShotCopy = iSnapShotCopy;
          TRAP_IGNORE( BurstCaptureArray()->ReplaceSnapshot( tempSnapShotCopy, 0 ) );	
          } 
        else
          {
          TRAP_IGNORE( BurstCaptureArray()->SetNextSnapshotL( *iSnapShotCopy ) );	
          }
        }        
      }
      
      // If video recording has already stopped, give snapshot event,
      // which causes snapshot to be updated in postcapture view.
      // 
      // If recording is still in progress, the event would cause
      // too early switch to pastcapture.
      if( ECamNoOperation == iInfo.iOperation ||
          ECamCompleting == iInfo.iOperation )
        {  
        NotifyControllerObservers( ECamEventSnapshotReady, aStatus );  
        }        
      
    return;
    }

// ---------------------------------------------------------------------------
// Still snapshot handling

  PRINT( _L("Camera <> Snapshot in still mode") ); 

  if( ECamImageCaptureTimeLapse == iInfo.iImageMode )
    {
    // Update time lapse count during snapshot, so the update counter value
    // appears at the same time as snapshot.  
    iTimeLapseCaptureCount++;
    NotifyControllerObservers( ECamEventCounterUpdated );  
    }        
  else if ( ECamImageCaptureBurst == iInfo.iImageMode )
    {
    // Re-enable the screen saver if burst capture is completing. 
    // This has to be done when all captures complete for timelapse.
    // EnableScreenSaver( ETrue );
    
    NotifyControllerObservers( ECamEventCounterUpdated );  
    iUpdateFrozenFrame = ETrue;    
        
    }

  // -------------------------------------------------------
  // if there is an error with the still capture operation
  if ( KErrNone != aStatus )
    {
    PRINT( _L("Camera <> CCamAppController: error in, cancel saving etc..") );
    // Store this now, as it's cleared by TidyCaptureArray
    // but needed a little later
    TBool wasCapturingBurst = (ECamImageCaptureBurst == iInfo.iImageMode);

    iImageSaveActive->Cancel();

    TidyCaptureArray( BurstCaptureArray()->NextFileIndex(), aStatus );
    PRINT( _L("Camera <> cancel further capturing") );
    TRAP_IGNORE( iCameraController->DirectRequestL( ECamRequestImageCancel ) );

    NotifyControllerObservers( ECamEventSnapshotReady, aStatus );

    if ( wasCapturingBurst )
      {
      // Need this here to allow the AppUi to update the state correctly
      // following the error condition.  This needs to be *after* the
      // change to OperationNone (called in TidyCaptureArray) so when the 
      // AppUi forces CBA update we're not still in burst capture state.
      NotifyControllerObservers( ECamEventCaptureComplete, aStatus );
      }

    PRINT( _L( "Camera <> calling HandleCaptureCompletion()" ) )
    HandleCaptureCompletion();
    }
  // -------------------------------------------------------
  // No error reported
  else
    {
    PRINT( _L("Camera <> CCamAppController: status in KErrNone..") );
    __ASSERT_ALWAYS( aBitmap, CamPanic( ECamPanicNullPointer ) );
    
    // The secondary camera postcapture snapshot is rotated here. 
    // Due to the viewfinder mirroring along the landscape axis the 
    // portrait (or upside down portrait) postcapture snapshot would 
    // otherwise be upside down.
    // This affects the snapshot seen in postcapture view.
   
    if( iInfo.iActiveCamera == ECamActiveCameraSecondary &&
        iCaptureOrientation == ECamOrientation180 &&
        ECamSettOn == IntegerSettingValue( ECamSettingItemShowCapturedPhoto ) )
        {
        PRINT( _L( "Camera <> Rotate portrait secondary camera snapshot image 180 degrees" ) )
        TRAP_IGNORE( iSnapShotRotator->RotateL( aBitmap ) );
        }
     
    CopySnapshotIfNeeded( *aBitmap, aStatus );
     
    // Removed filename reservation when snapshot arrives.
    // From now on, it's always done in HandleImageCaptureEvent,
    // even if snapshot comes first.
       
    // This is being ignored, but there is nothing to display in the app
    TRAP_IGNORE( BurstCaptureArray()->SetNextSnapshotL( *aBitmap ) )

    NotifyControllerObservers( ECamEventSnapshotReady );
      
    // If this is not a burst capture and snapshot came before image
    // data, then the operation is now entering the completion phase.
    // In case iFilenameReserved is set, imagedata has already
    // arrived and we don't need to change state here.
    if ( ECamCompleting != iInfo.iOperation && 
         ECamImageCaptureBurst != iInfo.iImageMode && 
         !iFilenameReserved )    
      {  
      SetOperation( ECamCompleting );      
      }
       
    if( CurrentCapturedCount() < CaptureLimit() )
        {
        // Play capture sound for the next burst image
        PlaySound( CaptureToneId(), EFalse );
        }

    }
  // -------------------------------------------------------

  PRINT( _L( "Camera <= CCamAppController::HandleSnapshotEvent" ) );    
  }


// ---------------------------------------------------------------------------
// HandleImageCaptureEventL
// ---------------------------------------------------------------------------
//
void
CCamAppController::HandleImageCaptureEventL( TInt             aStatus, 
                                             CCamBufferShare* aShare  )
  {
  PRINT1( _L("Camera => CCamAppController::HandleImageCaptureEventL, status:%d"), aStatus ); 

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
  if( ECamImageCaptureBurst != iInfo.iImageMode )
    {
    // Do not log in burst mode
    PERF_EVENT_END_L1( EPerfEventShotToStillImageReady );     
    }
  else
    {
    PERF_MESSAGE_L2( EPerfMessageBurstStillimageReady );
    }
#endif // CAMERAAPP_PERFORMANCE_MEASUREMENT

  OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_HANDLEIMAGECAPTUREEVENTL, "e_CAM_APP_SHOT_TO_STILL 0" );  //CCORAPP_SHOT_TO_STILL_END
/*
  // Check if we need to stop the burst capture.        
  if ( ECamImageCaptureBurst == iInfo.iImageMode )
    {    
    if( CurrentCapturedCount() < CaptureLimit() )
      {
      // Play capture sound for the next burst image
      PlaySound( CaptureToneId(), EFalse );
      }
    }
*/
   // If we have all the needed snapshots set the flag
   // so the processing image text is shown.
   // Do it here instead of handlesnapshotevent so that
   // the counter has time to show the last image as well 
   if( CurrentCapturedCount() == CaptureLimit() )  
       {
       iAllSnapshotsReceived = ETrue;	
       }
        
  if( KErrNone == aStatus )
    {
    PRINT( _L("Camera <> CCamAppController: image capture status KErrNone") );
    aShare->Reserve();
    CleanupStack::PushL( TCleanupItem( CamBufferShareCleanup, aShare ) );

    // Reserve filename for saving the image. In case there's problem
    // with current media, switch to internal memory
    TRAPD(err, ReserveFileNamesL( iInfo.iMode, iInfo.iImageMode ) );
    if( KErrNone != err )
     {
     if( ECamMediaStorageCard == IntegerSettingValue( ECamSettingItemPhotoMediaStorage ) )
       {
       PRINT( _L("Camera <> Memory card access failed.") )
       NotifyControllerObservers( ECamEventInvalidMemoryCard );
       }            
       // If we have got here, we can't access MMC. Switch to phone memory
        
     TRAP_IGNORE( ForceUsePhoneMemoryL() ); //with multiple drive support, 
                                              //this actually uses the internal mass memory 
     TRAPD(err, ReserveFileNamesL( iInfo.iMode, iInfo.iImageMode ) ); 
     if( KErrNone != err )
      {
      PRINT( _L("Camera <> Mass memory or phone memory access also failed.") )
      TRAP_IGNORE( ForceUsePhoneMemoryL() );
      TRAP_IGNORE( ReserveFileNamesL( iInfo.iMode, iInfo.iImageMode, ETrue ) );
      }                 
    }
    
    TBool markedForDelete = BurstCaptureArray()->IsNextImageDeleted();
    PRINT1( _L("Camera <> Next image delete mark:%d"), markedForDelete );
    if ( !markedForDelete )
      {
      PRINT( _L("Camera <> setting iImageSaveRequestPending to false"))
      iImageSaveRequestPending = EFalse;
      const TDesC& nextName( BurstCaptureArray()->NextFileName() );
      PRINT( _L("Camera <> BurstCaptureArray()->NextFileName() returned") );
      PRINT1( _L("Camera <> Set image to be saved, name:[%S]"), &nextName );
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if( ( iInfo.iImageMode == ECamImageCaptureSingle ) 
              && ECamSettOff == IntegerSettingValue( ECamSettingItemShowCapturedPhoto )
              && iInfo.iActiveCamera == ECamActiveCameraPrimary
              && !appUi->IsEmbedded() )
          {
          HandleSaveEvent( ECamSaveEventStarted );
          }
      TBool added = iImageSaveActive->AddToSave( nextName, aShare ); 
      
      if ( !added )
        {
        PRINT( _L("Camera <> Image saving start unsuccessful!") );
        // The system state is consistent if this fails so ignore the error
        }
      else
        {
        PRINT( _L("Camera <> Image saving started ok.") );
        // Ownership of the data has now been passed to CCamImageSaveActive
        if ( ECamImageCaptureBurst == iInfo.iImageMode )
          {          
          NotifyControllerObservers( ECamEventImageData );  
          }
        PRINT1( _L("Camera <> CCamAppController::HandleImageCaptureEventL array count:%d"), BurstCaptureArray()->Count() );  

        // Snapshots are rotated here if necessary, so that the thumbnails created by 
        // ThumbNailManager accurately represent the actual images.
        // This affects the thumbnails seen in Photos.
        
        // check if snapshot bitmap needs to be rotated before creating a thumbnail from it
        TBool rotate( ( ECamSettOn == iSettingsModel->IntegerSettingValue( ECamSettingItemImageRotation ) || 
                      iInfo.iActiveCamera == ECamActiveCameraSecondary ) &&
                      iCaptureOrientation != ECamOrientation0 );

        if ( ECamImageCaptureBurst != iInfo.iImageMode )
          {
          //create thumbnail or rotate first if needed
          if ( iConfigManager && iConfigManager->IsThumbnailManagerAPISupported() )
              {
              if ( rotate )
                  {
                  RotateSnapshotL();
                  }
              else
                  {       
                  TRAP_IGNORE( iImageSaveActive->CreateThumbnailsL( *BurstCaptureArray() ) );
                  }
              } 
          }
        }
      }
    CleanupStack::PopAndDestroy(); // aShare->Release()
    aShare = NULL;
    }
  else
    {
    PRINT( _L("Camera <> CCamAppController: capture status *not* KErrNone, mark current file for delete") );
    DeleteCurrentFile();
    }


  iFilenameReserved = ETrue;

  PRINT( _L( "Camera <= CCamAppController::HandleImageCaptureEventL" ) );        
  }



// ---------------------------------------------------------------------------
// HandleImageStopEventL
// ---------------------------------------------------------------------------
//
void
CCamAppController::HandleImageStopEventL( TInt aStatus, TInt aFullCaptureCount )
  {
  PRINT2( _L("Camera => CCamAppController::HandleImageStopEventL, status:%d, full captures:%d"), aStatus, aFullCaptureCount );

  TBool saved = ( iImageSaveActive->Count()== 0 );

  switch( iInfo.iImageMode )
    {
    // -----------------------------------------------------
    case ECamImageCaptureBurst:
      {
      PRINT( _L( "Camera <> CCamAppController .. case ECamImageCaptureBurst" ) );
      // CompleteBurstOperation();
     
      // Re-enable screen saver
      EnableScreenSaver( ETrue );
      if( iBacklightTimer )
        {
        iBacklightTimer->Cancel();
        }
      
      TInt started( iCameraController->ControllerInfo().iSnapshotCount );
      if ( aFullCaptureCount < started )
        {
        PRINT ( _L("Camera <> CCamAppController .. [WARNING] Incomplete captures detected, cleaning up.." ) );
        PRINT1( _L("Camera <> CCamAppController .. [WARNING] snapshot     count: %d"), started );
        PRINT1( _L("Camera <> CCamAppController .. [WARNING] full capture count: %d"), aFullCaptureCount            );
        PRINT1( _L("Camera <> CCamAppController .. [WARNING] burst array  count: %d"), BurstCaptureArray()->Count() );
        TInt stopAt = Min( started, BurstCaptureArray()->Count() );
        for ( TInt index = aFullCaptureCount; index < stopAt; index++ )
          {
          PRINT1( _L("Camera <> CCamAppController .. [WARNING] deleting temp file from index %d.."), index );
          SetAsCurrentImage( index );
          // Parameter: notify about file system change only on last delete.
          DeleteCurrentFile( stopAt-1 == index );
          }
        PRINT1( _L("Camera <> CCamAppController .. [WARNING] setting as current image %d.."), aFullCaptureCount );
        SetAsCurrentImage( aFullCaptureCount );
        PRINT ( _L("Camera <> CCamAppController .. [WARNING] resetting burst array to required count..") );
        BurstCaptureArray()->Reset( aFullCaptureCount );
        }

      SetOperation( ECamCompleting );

      // We may have deleted all the images, so check the status here.
      if( iImageSaveActive->Count()== 0 )
        {
        PRINT( _L( "Camera <> CCamAppController .. images already saved, completing burst capture" ) )
        SetOperation( ECamNoOperation );
        NotifyControllerObservers( ECamEventCaptureComplete, aStatus );
        ReleaseArray();
        }
      // if no images were taken, clear AssumePostCaptureView flag  
      if ( started == 0 )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        if ( appUi )
          {
          appUi->SetAssumePostCaptureView( EFalse );	
          }	
        }  
      break;
      }
    // -----------------------------------------------------
    case ECamImageCaptureSingle:
      {
      PRINT( _L( "Camera <> CCamAppController .. case ECamImageCaptureSingle" ) );

      // In secondary camera we can get snapshot as last event
      // so we might still be in capturing state.
      if( ECamCapturing == iInfo.iOperation )
        {
        PRINT( _L( "Camera <> CCamAppController .. still in capturing phase, set to completing first.." ) );
        SetOperation( ECamCompleting );  
        }

  	  // In case of secondary camera, image may be saved before Stop event is received.
  	  // Notify capture complete event to the observers and other completion handling here.
      if( saved && ECamCompleting == iInfo.iOperation )
        {
        PRINT( _L( "Camera <> CCamAppController .. file saved, complete capture" ) );

		// Operation mode is also set in HandleCaptureCompletion
		HandleCaptureCompletion();
		NotifyControllerObservers( ECamEventCaptureComplete, aStatus );
        ReleaseArray();
        }
      
      break;
      }
    // -----------------------------------------------------
    case ECamImageCaptureTimeLapse:
      {
      PRINT( _L( "Camera <> CCamAppController .. case ECamImageCaptureTimeLapse" ) );
      if( saved )
        {
        PRINT( _L( "Camera <> CCamAppController .. current file saved, complete this capture" ) );
        SetOperation( ECamNoOperation );
        ReleaseArray();
        }

      if ( iCompleteTimeLapsePending )
        {
        CompleteTimeLapseOperation();
        }
      else
        {
        // If the camera orientation changed during capture, set the new orientation in the engine
        // ready for the next timelapse capture
        if ( iConfigManager && iConfigManager->IsOrientationSensorSupported()
         		 && iOrientationChangeOccured )  
          {
          iOrientationChangeOccured = EFalse;
          TRAP_IGNORE( SetImageOrientationL() );
          }
        }   
      break;
      }
    // -----------------------------------------------------
    default:
      break;
    // -----------------------------------------------------
    }
  PRINT( _L( "Camera <= CCamAppController::HandleImageStopEventL" ) );
  }


// ---------------------------------------------------------------------------
// CCamAppController::IdleTimeoutL
// ---------------------------------------------------------------------------
//
TInt CCamAppController::IdleTimeoutL( TAny* aPtr )
    {
    return static_cast<CCamAppController*>( aPtr )->DoIdleTimeoutL();
    }

// ---------------------------------------------------------------------------
// CCamAppController::DoIdleTimeoutL
// ---------------------------------------------------------------------------
//
TInt CCamAppController::DoIdleTimeoutL()
  {
  // if a video recording has been paused for 60 seconds without key presses
  if ( ECamPaused == CurrentVideoOperation() )
    {
    NotifyControllerObservers( ECamEventVideoPauseTimeout, KErrNone );
    }

  // notify switch to standby mode and stop timer
  else if( ECamStandby != iInfo.iOperation && ECamTriActive == iCameraController->ViewfinderState())
    {
    SetOperation( ECamStandby );
    }
  else
    {
    // For Lint
    }

  iIdleTimer->Cancel();

// Use backlight timer also with bitmap vf
  if( iBacklightTimer )
    {
	if( ECamTriActive == iCameraController->ViewfinderState() )
	  {
	  User::ResetInactivityTime();
      }
    iBacklightTimer->Cancel();
    }

  return EFalse;
  }

// ---------------------------------------------------------------------------
// CCamAppController::StartIdleTimer
// ---------------------------------------------------------------------------
//
void CCamAppController::StartIdleTimer()
  {
  PRINT( _L( "Camera => CCamAppController::StartIdleTimer" ) ); 
  // if in standby mode notify of switch back to pre-capture
  if ( ECamStandby == iInfo.iOperation )
    {
    PRINT( _L( "Camera <> CCamAppController::StartIdleTimer ECamStandby" ) );
    SetOperation( ECamNoOperation );
    }
  // if recording is in progress don't go to standby
  else if ( ECamCapturing == CurrentVideoOperation() )             
    {
    PRINT( _L( "Camera <> CCamAppController::StartIdleTimer no restart" ) );
    // don't restart if recording operation in progress
    return;
    }
  // if recording is paused, use the idle timer to stop recording after 60 secs
  else
    {
    PRINT( _L( "Camera <> CCamAppController::StartIdleTimer else part" ) );
    // empty else statement to remove LINT error
    }

  // restart timer
  iIdleTimer->Cancel();

  // don't restart if capturing burst, setting time lapse or in a pretend exit state
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  if ( !SequenceCaptureInProgress() && appUi && !appUi->IsInPretendExit() 
       && !appUi->TimeLapseSliderShown())
    {
    // restart timer
    PRINT( _L( "Camera <> CCamAppController::StartIdleTimer starting timer" ) );
    iIdleTimer->StartTimer();
    }

  if ( iDeepSleepTimer )
    {
    if ( iDeepSleepTimer->IsActive() )
        {
        PRINT( _L( "Camera <> CCamAppController::StartIdleTimer - restart deep sleep timer" ) );
        DeepSleepTimerCancel();
        DeepSleepTimerStart();
        }
    else if ( iDeepSleepTimerExpired )
        {
        iDeepSleepTimerExpired = EFalse; // clear flag
        TVwsViewId activeView;
        CCamAppUi* appUi = 
            static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        (void) appUi->GetActiveViewId( activeView ); // ignore error
        
        if ( !IsViewFinding() && !InVideocallOrRinging() &&
            activeView.iViewUid.iUid != ECamViewIdVideoPostCapture && 
            activeView.iViewUid.iUid != ECamViewIdStillPostCapture &&
            activeView.iViewUid.iUid != ECamViewIdBurstThumbnail )
            {
            PRINT( _L( "Camera <> CCamAppController::StartIdleTimer - Deep sleep timer expired. Restart VF" ) );
            TCamCameraMode mode = CurrentMode() == ECamControllerVideo?
                                  ECamControllerVideo : ECamControllerImage;
            EnterViewfinderMode( mode ); 
            DeepSleepTimerStart();
            }
        }
    }

  PRINT( _L( "Camera <= CCamAppController::StartIdleTimer" ) );
  }

// ---------------------------------------------------------------------------
// CCamAppController::StopIdleTimer
// ---------------------------------------------------------------------------
//
void CCamAppController::StopIdleTimer()
    {
    PRINT( _L( "Camera => CCamAppController::StopIdleTimer" ) );
    if( iIdleTimer )
        {
        iIdleTimer->Cancel();
        }
    PRINT( _L( "Camera <= CCamAppController::StopIdleTimer" ) );
    }

// ---------------------------------------------------------------------------
// CCamAppController::TimeLapseTimeoutL
// ---------------------------------------------------------------------------
//
TInt CCamAppController::TimeLapseTimeoutL( TAny* aPtr )
    {
    return static_cast<CCamAppController*>( aPtr )->DoTimeLapseTimeoutL();
    }    
    
// ---------------------------------------------------------------------------
// CCamAppController::DoTimeLapseTimeoutL
// ---------------------------------------------------------------------------
//
TInt CCamAppController::DoTimeLapseTimeoutL()
  {
  PRINT( _L("Camera => CCamAppController::DoTimeLapseTimeoutL" ))      
  // Work out how much time is left before the next capture
  TTime now;
  now.HomeTime();
  TTimeIntervalMicroSeconds elapsedTime = now.MicroSecondsFrom( iTimeLapseStartTime );
  TInt64 remainingTime = iTimeLapseInterval.Int64() - elapsedTime.Int64();              
  // update the remaining time countdown 
  NotifyControllerObservers( ECamEventCounterUpdated );
  
  //if the total timelapse period has now completed
  if ( remainingTime <= 0 )
    {
        // Workaround for DoTimeLapseTimeoutL firing while in inappropriate state
    	// cause capture to be delayed by 1 sec 
    	if( ECamNoOperation != iInfo.iOperation ) 
    		{
    		PRINT( _L("Camera <> CCamAppController::DoTimeLapseTimeoutL workaround" ))      
			//remainingTime = TInt64( KSecondInMicSec );    	
		    iTimeLapseTimer->StartTimer();
    		}    	
	else
		{
	    TInt remainingCount = ImagesRemaining( static_cast<TCamMediaStorage>( IntegerSettingValue( ECamSettingItemPhotoMediaStorage ) ), ETrue );
	    if ( remainingCount > 0 )
	      	{
	      PRINT( _L("Camera => CCamAppController::DoTimeLapseTimeoutL about to do next capture" ))   
	      // About to do the next capture so decrement the view's usage count on the array
	      ReleaseArray();
            if ( iConfigManager && iConfigManager->IsAutoFocusSupported() )
                {
                if( !CurrentSceneHasForcedFocus() )
                    {
                    if( !IsViewFinding() )
                        {
                        StartViewFinder();
                        }
                    StartAutoFocus();
                    iCaptureRequested = ETrue; //Has to be set before focused/not focused event.
                    }
                else
                    {
                    PRINT( _L("Camera <> CCamAppController::DoTimeLapseTimeoutL capture with forcedfocus scene" ))   
                    Capture();            
                    }
                }
            else 
                {
            	Capture();            
                }
			RestartTimeLapseTimerL();
      	}
	    else
	     {
	     		PRINT( _L("Camera => CCamAppController::DoTimeLapseTimeoutL capture is complete" ))   
	      	CompleteTimeLapseOperation();
	     }
	   }              
    return EFalse;
    }
  // Otherwise restart the timer for the next period    
  else
    {
    if ( remainingTime < TInt64( KSecondInMicSec ) )
      {
      PRINT( _L("Camera => CCamAppController::DoTimeLapseTimeoutL shortening timer" ))   
      iTimeLapseTimer->SetTimeout( (TInt)remainingTime );
      }
     else if(  ( remainingTime < TInt64( 5 *KSecondInMicSec )) && !iCameraWoken )
     	{
		iCameraWoken = ETrue;
		if( !IsViewFinding() )
		{
			StartViewFinder();
		}
		User::ResetInactivityTime();     		
     	}
     
    PRINT( _L("Camera => CCamAppController::DoTimeLapseTimeoutL restarting timer" ))               
    iTimeLapseTimer->StartTimer();
    }
  return EFalse;        
  }

// ---------------------------------------------------------------------------
// CCamAppController::SupportedFlashModes
// Return flash modes supported engine
// ---------------------------------------------------------------------------
//
TUint32 CCamAppController::SupportedFlashModes()
  {
  if( iCameraController )
    return iCameraController->CameraInfo().iFlashModesSupported;
  else
    return 0;
  //iSupportedFlashModes;
  }
  
// ---------------------------------------------------------------------------
// CCamAppController::SupportedWBModes
// Return white balance modes supported engine
// ---------------------------------------------------------------------------
//  
TUint32 CCamAppController::SupportedWBModes()
  {
  if( iCameraController )
    return iCameraController->CameraInfo().iWhiteBalanceModesSupported;
  else
    return 0;
  //iSupportedWBModes;
  }
  
// ---------------------------------------------------------------------------
// CCamAppController::SupportedEVModes
// Return EV modes supported engine
// ---------------------------------------------------------------------------
//  
TUint32 CCamAppController::SupportedEVModes()
  {
  if( iCameraController )
    return iCameraController->CameraInfo().iExposureModesSupported;
  else
    return 0;
  //iSupportedEVModes;
  } 

// ---------------------------------------------------------------------------
// CCamAppController::CaptureState
// ---------------------------------------------------------------------------
//  
TInt CCamAppController::CaptureState()  
  {
  if( iCameraController )
    {
    return iCameraController->ControllerInfo().iCaptureState;
    }
  else
    {
    return KErrNotFound;
    }
  } 



// -----------------------------------------------------------------------------
// CCamAppController::FlashStatus()
// Returns pointer to FlashStatus object
// -----------------------------------------------------------------------------
//
CCamFlashStatus* 
CCamAppController::FlashStatus() const
  {
  return iFlashStatus;    
  }
       

// -----------------------------------------------------------------------------
// CCamAppController::FlashRequired()
// 
// -----------------------------------------------------------------------------
//
TBool 
CCamAppController::FlashRequired() const
  {
  if( ECamSceneSports == IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) )
    {
    // Flash is not required in sport scene, even if the flash mode
    // is forced or redeye.
    return EFalse;
    }
    
  // Make decision based on current flash mode    
  TInt flashMode = IntegerSettingValue( ECamSettingItemDynamicPhotoFlash );            
  switch( flashMode )
    {
    case ECamFlashOff:
      {
      return EFalse;    
      }
    case ECamFlashAntiRedEye: // Flowthrough
    case ECamFlashForced:
      {
      return ETrue;    
      }
    default:
      {
      // Just return true in the default case.
      // Automatic mode will decide if flash is used or not.
      return ETrue;
      }
    } 
  }

// -----------------------------------------------------------------------------
// CCamAppController::CheckFlash()
// 
// -----------------------------------------------------------------------------
//     
TBool 
CCamAppController::CheckFlash() const
  {            
  PRINT ( _L("Camera => CCamAppController::CheckFlash") );
  PRINT1( _L("Camera <> CCamAppController:              flash required :%d  "), FlashRequired() );
  PRINT1( _L("Camera <> CCamAppController: FlashStatus: flash ready    :%d  "), iFlashStatus->FlashReady() );
  PRINT1( _L("Camera <> CCamAppController: CameraCtrl:  flash status   :[%s]"),
          KCamCameraReadyStateNames[iCameraController->FlashState()] );

  TBool ok( !FlashRequired() || iFlashStatus->FlashReady() );

  PRINT1( _L("Camera <= CCamAppController::CheckFlash, return:%d"), ok );
#ifdef __WINSCW__
  return ETrue;
#else  
  return ok;
#endif  
  }


// -----------------------------------------------------------------------------
// CCamAppController::SaveFlashMode()
// 
// -----------------------------------------------------------------------------
//    
void CCamAppController::SaveFlashMode()
  {    
  iFlashModeSaved  = ETrue;
  iSavedFlashMode  = static_cast<TCamFlashId>( IntegerSettingValue( ECamSettingItemDynamicPhotoFlash ) );
  iSavedFlashScene = static_cast<TCamSceneId>( IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) );
  }
    
// -----------------------------------------------------------------------------
// CCamAppController::RestoreFlashMode
// 
// -----------------------------------------------------------------------------
//    
void CCamAppController::RestoreFlashMode()
  {                
  if( iFlashModeSaved )
    {
    if( iSavedFlashScene == IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) )
      {
      // Set saved flash mode    
      TRAP_IGNORE( SetIntegerSettingValueL( ECamSettingItemDynamicPhotoFlash, iSavedFlashMode ) );
      }
    else
      {
      // Set default flash mode for this scene   
      // SetDynamicSettingToDefault( ECamSettingItemDynamicPhotoFlash );          
      TInt scene = IntegerSettingValue( ECamSettingItemDynamicPhotoScene );
      TInt sceneFlashMode = iSettingsModel->SceneSettingValue( scene, ECamSettingItemSceneFlashMode );
      TRAP_IGNORE( SetIntegerSettingValueL( ECamSettingItemDynamicPhotoFlash, sceneFlashMode ) );
      
      }
    }
  iFlashModeSaved = EFalse;   
  }


// ---------------------------------------------------------------------------
// CCamAppController::ForceUsePhoneMemoryL
// ---------------------------------------------------------------------------
//
void CCamAppController::ForceUsePhoneMemoryL( TBool aEnable )
	{
	PRINT( _L("Camera => CCamAppController::ForceUsePhoneMemoryL") );

	if( aEnable )
		{
		iForceUseOfPhoneMemory = ETrue;
    if( ExistMassStorage() )
      {
      // with multiple drives, the mass memory is the default
      PRINT( _L("Camera <> Force ECamMediaStorageMassStorage") );
      iForcedStorageLocation = ECamMediaStorageMassStorage;
      SetPathnamesToNewStorageL( ECamMediaStorageMassStorage );
      }
    else
      {
      PRINT( _L("Camera <> Force ECamMediaStoragePhone") );
      TCamMediaStorage storage = IntegerSettingValue(ECamSettingItemRemovePhoneMemoryUsage)?
                                  ECamMediaStorageNone:
                                  ECamMediaStoragePhone;
      iForcedStorageLocation = storage;
      SetPathnamesToNewStorageL( storage );
      }
		}
	// Revert back to memory card, if aEnable is EFalse
	else
		{
		iForceUseOfPhoneMemory = EFalse;
    iForcedStorageLocation = ECamMediaStorageNone;
    SetPathnamesToNewStorageL( iPreferredStorageLocation );
		}

	NotifyControllerObservers( ECamEventSaveLocationChanged );
	PRINT( _L("Camera <= CCamAppController::ForceUsePhoneMemoryL") );
	}



// ---------------------------------------------------------------------------
// CCamAppController::IsPhoneMemoryForced
// Check whether the use of phone memory is forced, because of unavailibility 
// or problems encountered in using MMC.
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsPhoneMemoryForced()
	{
	return iForceUseOfPhoneMemory;
	}

// ---------------------------------------------------------------------------
// CCamAppController::CheckMemoryToUseL
// Set the appropriate memory to use. Use Phone memory if MMC not available.
// ---------------------------------------------------------------------------
//
TBool CCamAppController::CheckMemoryToUseL()
	{
  TInt key = ( ECamControllerImage == CurrentMode() )
               ? ECamSettingItemPhotoMediaStorage
               : ECamSettingItemVideoMediaStorage;

  TCamMediaStorage storageLocation = static_cast<TCamMediaStorage> ( IntegerSettingValueUnfiltered(key) );
  TBool mmcAvailable = IsMemoryAvailable ( ECamMediaStorageCard );
    
  PRINT1( _L("Camera :: CCamAppController::CheckMemoryToUse preferred storage:%d"), 
              storageLocation );
    
  // If MMC is preferred storage location but is not available 
	if ( storageLocation == ECamMediaStorageCard && !mmcAvailable )
		{
		PRINT( _L("Camera :: CCamAppController::CheckMemoryToUse - use phone memory / mass storage") )
		ForceUsePhoneMemoryL( ETrue );
		return ETrue;
		}
	// Preferred location is MMC and its available now -> stop forcing phone memory 
	else if( storageLocation == ECamMediaStorageCard && mmcAvailable && 
	         iForceUseOfPhoneMemory ) 
		{
		PRINT( _L("Camera :: CCamAppController::CheckMemoryToUse - use MMC") )
		ForceUsePhoneMemoryL( EFalse );
		return ETrue;
		}
  if ( storageLocation == ECamMediaStorageMassStorage && !ExistMassStorage() )
		{
		PRINT( _L("Camera :: CCamAppController::CheckMemoryToUse - no mass storage, use phone memory") )
		ForceUsePhoneMemoryL( ETrue );
		return ETrue;
		}
	// Mass storage is available now -> stop forcing phone memory
	else if( storageLocation == ECamMediaStorageMassStorage && ExistMassStorage() && iForceUseOfPhoneMemory ) 
		{
		PRINT( _L("Camera :: CCamAppController::CheckMemoryToUse - use mass memory") )
		ForceUsePhoneMemoryL( EFalse );
		return ETrue;
		}
	else
	  {
	  SetPathnamesToNewStorageL( storageLocation );
	  }

  return EFalse;
	}



// ---------------------------------------------------------------------------
// CCamAppController::CompleteBurstOperation
// Complete the burst operation
// ---------------------------------------------------------------------------
//
void CCamAppController::CompleteBurstOperation()
  {
  PRINT( _L( "Camera => CCamAppController::CompleteBurstOperation" ) );
  iSequenceCaptureInProgress = EFalse;
  iNoBurstCancel=EFalse;
  
  if( ECamImageCaptureBurst == iInfo.iImageMode )
      {
      PRINT1( _L("Camera <> CCamAppController::CompleteBurstOperation array count:%d"), BurstCaptureArray()->Count() );    
      // Create thumbnails
      if( iConfigManager && iConfigManager->IsThumbnailManagerAPISupported() )
          {
          TRAP_IGNORE( iImageSaveActive->CreateThumbnailsL( *BurstCaptureArray() ) );
          }
      	
      // Re-enable screen saver
      EnableScreenSaver( ETrue );
	  if( iBacklightTimer )
		{
		iBacklightTimer->Cancel();
		}
      SetOperation( ECamNoOperation );
  
      NotifyControllerObservers( ECamEventCaptureComplete, KErrNone );
      HandleCaptureCompletion();
      }
  PRINT( _L( "Camera <= CCamAppController::CompleteBurstOperation" ) );          
  }

// ---------------------------------------------------------------------------
// CCamAppController::CompleteTimeLapseOperation
// Complete the time lapse operation
// ---------------------------------------------------------------------------
//
void CCamAppController::CompleteTimeLapseOperation()    
    {
    PRINT( _L( "Camera => CCamAppController::CompleteTimeLapseOperation" ) );

    iSequenceCaptureInProgress = EFalse;

    if ( iTimeLapseTimer )
      {
      iTimeLapseTimer->Cancel();
      delete iTimeLapseTimer;
      iTimeLapseTimer = NULL;
      }

    if( ECamImageCaptureTimeLapse == iInfo.iImageMode )
      {
      // Re-enable screen saver
      EnableScreenSaver( ETrue );
      iCompleteTimeLapsePending = EFalse;
  
//      SetImageMode( ECamImageCaptureNone );   
      SetOperation( ECamNoOperation      );
  
      NotifyControllerObservers( ECamEventCaptureComplete, KErrNone );   
      HandleCaptureCompletion();
      iTimeLapseStartTime = TInt64( 0 );
      }

    PRINT( _L( "Camera <= CCamAppController::CompleteTimeLapseOperation" ) );   
    }


// ---------------------------------------------------------------------------
// CCamAppController::EngineProcessingCapture
// Return whether or not the engine is carrying out a capture operation
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::EngineProcessingCapture() const
  {
  TBool engineIsProcessing = EFalse;

  if ( ECamCapturing  == iInfo.iOperation
    || ECamPausing    == iInfo.iOperation
    || ECamPaused     == iInfo.iOperation
    || ECamResuming   == iInfo.iOperation
    || ECamCompleting == iInfo.iOperation 
     )
    {
    engineIsProcessing = ETrue;
    }

  return engineIsProcessing;
  }

// ---------------------------------------------------------------------------
// CCamAppController::HandleIncomingCall
// Stop video recording (or burst capture in early stages)
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleIncomingCall()
  {
  PRINT2( _L("Camera => CCamAppController::HandleIncomingCallL .. mode[%s] operation[%s]"), 
          KCamModeNames[iInfo.iMode], 
          KCamCaptureOperationNames[iInfo.iOperation]
        );

  if( ECamControllerVideo == iInfo.iMode )
    {
    // if video is recording or paused
    if ( !iSaving
      && ( ECamCapturing == iInfo.iOperation
        || ECamPaused    == iInfo.iOperation ) )
      {
      StopVideoRecording();  
      }
    }
  else if( ECamControllerImage == iInfo.iMode )
    {
    iCaptureRequested = EFalse;
    switch( iInfo.iImageMode ) 
      {
      // if a timelapse operation is in progress
      case ECamImageCaptureTimeLapse:
        {
        TRAP_IGNORE( StopSequenceCaptureL() );
        break;
        }
      case ECamImageCaptureBurst:
        {
        TRAP_IGNORE( SoftStopBurstL( ETrue ) );
        /*
        if ( ECamCompleting == iInfo.iOperation )
          {
          // if some of the image captures failed
          // release the reserved filenames and tidy the array
          TInt captured( iCameraController->ControllerInfo().iCaptureCount );
          if ( BurstCaptureArray()->NextFileIndex() < captured )
            {
            TInt index;
            TInt stopAt = Min( captured, BurstCaptureArray()->Count() );
            for ( index = BurstCaptureArray()->NextFileIndex(); index < stopAt; index++ )
              {
              SetAsCurrentImage( index );
              DeleteCurrentFile();
              }
            SetAsCurrentImage( 0 );
      
            BurstCaptureArray()->Reset( BurstCaptureArray()->NextFileIndex() );
            // Finished with this now
            }
//          SetImageMode( ECamImageCaptureNone );
          if( iImageSaveActive )
            iImageSaveActive->DismissProgressNote();
          StartIdleTimer();
          }
        */
        break;
        }
      default:
        break;
      }
    }
  else
    {
    // no action needed
    }
  PRINT( _L( "Camera <= CCamAppController::HandleIncomingCallL" ) ); 
  }

    
// ---------------------------------------------------------------------------
// CCamAppController::FileSize
// Returns the file size of the specified captured item, or KErrNotFound if doesn't exist
// ---------------------------------------------------------------------------
//
TInt CCamAppController::FileSize( TDesC& aFilename ) const
  {       
  return iImageSaveActive->FileSize( aFilename );
  }

// ---------------------------------------------------------------------------
// CCamAppController::CancelStillCaptureNow
// Immediately cancels an ongoing still capture
// ---------------------------------------------------------------------------
//
void CCamAppController::CancelStillCaptureNow()
  {
  PRINT( _L( "Camera => CCamAppController::CancelStillCaptureNow" ) )
  // Re-enable screen saver
  EnableScreenSaver( ETrue ); 
//    iEngine->CancelCaptureStill();
  TRAP_IGNORE( iCameraController->DirectRequestL( ECamRequestImageCancel ) );

  // ensure that any outstanding images are discarded
  SetOperation( ECamCompleting  );
  SetOperation( ECamNoOperation );

  // If sequence mode, remove items from the array
  // this prevents the post capture view from showing
  if ( ECamImageCaptureBurst == iInfo.iImageMode )
    {
    BurstCaptureArray()->Reset( 0 );
    }   
  else // single capture, the array is no longer needed
    {
    PRINT( _L( "Camera <> single capture so releasing array and starting vf" ) )
    ReleaseArray();
    FreezeViewFinder( EFalse );
    }
  // Cancel any pending saves, this will call CompleteBurstOperation for sequence
  PRINT( _L( "Camera <> calling iImageSaveArray->Cancel" ) )
  iImageSaveActive->Cancel(); 
  PRINT( _L( "Camera <= CCamAppController::CancelStillCaptureNow" ) )
  }
    


// ---------------------------------------------------------------------------
// CCamAppController::TidyCaptureArray
// Tidy up capture array in event of burst capture failure
// ---------------------------------------------------------------------------
//
void
CCamAppController::TidyCaptureArray( TInt aImageCountDelivered, 
                                     TInt /*aError*/               )
  {
  PRINT1( _L("Camera => CCamAppController::TidyCaptureArray img count %d"), aImageCountDelivered )
  iSoundPlayer->EnableAllKeySounds();

  // if some of the image captures failed
  // release the reserved filenames and tidy the array

  TInt captured( iCameraController->ControllerInfo().iSnapshotCount );
  if ( aImageCountDelivered < captured )
    {
    TInt index;
    TInt stopAt = Min( captured, BurstCaptureArray()->Count() );
    for ( index = aImageCountDelivered; index < stopAt; index++ )
      {
      PRINT1( _L("Camera <> TidyCaptureArray delete current %d"),index )
      SetAsCurrentImage( index );
      DeleteCurrentFile();
      }
    SetAsCurrentImage( 0 );
    iCaptureArray->Reset( aImageCountDelivered );
    // Finished with this now
    }

  SetImageMode( ECamImageCaptureNone );
  StartIdleTimer();

  // Need to release the array
  ReleaseArray();


  // Unfreeze the viewfinder
  FreezeViewFinder( EFalse );

  // Change op state to none
  SetOperation( ECamNoOperation );

  PRINT( _L("Camera <= CCamAppController::TidyCaptureArray") )
  }


// ---------------------------------------------------------------------------
// CCamAppController::CaptureToneId
// Returns the current capture tone
//
// ---------------------------------------------------------------------------
//
TCamSoundId CCamAppController::CaptureToneId( )
  {
  TCamSoundId toneId = ECamStillCaptureSoundId1;

  switch(iSettingsModel->IntegerSettingValue( ECamSettingItemPhotoCaptureTone ) )
    {
    case ECamSettTone1:
      if ( ECamImageCaptureBurst == iInfo.iImageMode ) 
        {
        toneId = ECamBurstCaptureSoundId1;
        }
      else 
        {
        toneId = ECamStillCaptureSoundId1;
        }
      break;
    case ECamSettTone2:
      if ( ECamImageCaptureBurst == iInfo.iImageMode ) 
        {
        toneId = ECamBurstCaptureSoundId2;
        }
      else 
        {
        toneId = ECamStillCaptureSoundId2;
        }
      break;
    case ECamSettTone3:
      if ( ECamImageCaptureBurst == iInfo.iImageMode ) 
        {
        toneId = ECamBurstCaptureSoundId3;
        }
      else 
        {
        toneId = ECamStillCaptureSoundId3;
        }
      break;
    case ECamSettTone4:
      if ( ECamImageCaptureBurst == iInfo.iImageMode ) 
        {
        toneId = ECamBurstCaptureSoundId4;
        }
      else 
        {
        toneId = ECamStillCaptureSoundId4;
        }
      break;
    default:
      break;
    }

  return toneId;
  }

// ---------------------------------------------------------------------------
// CCamAppController::LoadSecondaryCameraSettingsL()
// Update settings for secondary camera
// ---------------------------------------------------------------------------
//  
void CCamAppController::LoadSecondaryCameraSettingsL()
  {
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPCONTROLLER_LOADSECONDARYCAMERASETTINGSL, "e_CCamAppController_LoadSecondaryCameraSettingsL 1" );
  PRINT( _L("Camera => CCamAppController::LoadSecondaryCameraSettingsL" ))
  iSettingsModel->StorePrimaryCameraSettingsL();
  iSettingsModel->StoreUserSceneSettingsL();

  PRINT( _L("Camera <> CCamAppController::LoadSecondaryCameraSettingsL B" ))
  
  SetIntegerSettingValueL( ECamSettingItemPhotoQuality, 
                           iConfiguration->SecondaryCameraImageQuality() );
  PRINT(  _L("Camera <> CCamAppController::LoadSecondaryCameraSettingsL C" ))
  SetIntegerSettingValueL( ECamSettingItemVideoQuality,
                           iConfiguration->SecondaryCameraVideoQuality() );  


  // Remember the previous state of face tracking,
  // current state of face tracking and
  // the previous scene mode
  TCamSettingsOnOff previousFaceTrack = iSettingsModel->GetPreviousFaceTrack();
  TCamSettingsOnOff faceTracking = static_cast<TCamSettingsOnOff>( IntegerSettingValue( ECamSettingItemFaceTracking ) );
  TCamSceneId previousSceneMode = iSettingsModel->GetPreviousSceneMode();
  
  PRINT( _L("Camera <> CCamAppController::LoadSecondaryCameraSettingsL E" ))
  SetIntegerSettingValueL( ECamSettingItemDynamicPhotoScene, ECamSceneAuto );
  SetIntegerSettingValueL( ECamSettingItemDynamicVideoScene, ECamSceneNormal );
  

  // Restore the previous state of face tracking,
  // current state of face tracking and
  // the previous scene mode
  iSettingsModel->SetPreviousFaceTrack( previousFaceTrack );
  SetIntegerSettingValueL( ECamSettingItemFaceTracking, faceTracking );
  iSettingsModel->SetPreviousSceneMode( previousSceneMode );
  
  PRINT( _L("Camera <> CCamAppController::LoadSecondaryCameraSettingsL F" ))

  PRINT( _L("Camera <= CCamAppController::LoadSecondaryCameraSettingsL" ))
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPCONTROLLER_LOADSECONDARYCAMERASETTINGSL, "e_CCamAppController_LoadSecondaryCameraSettingsL 0" );
  }

// ---------------------------------------------------------------------------
// ImageOrientation <<public>>
//
// Return the current image orientation based on data from orientation sensor.
// If "image rotation" setting is OFF, the value has been overriden with 
// "no rotation" value (ECamOrientation0).
// 
// Provided (primarily) for Camera Controller even if orientation
// sensor is not supported. In that case the returned
// value is always ECamOrientation0.
// ---------------------------------------------------------------------------
//
TCamImageOrientation 
CCamAppController::ImageOrientation() const
  {
  return iImageOrientation;
  }


// ---------------------------------------------------------------------------
// Indicates data is available in the receiving buffer. A client can read 
// the data through GetData()-function in the related channel object. Data 
// is valid until the data received notification occurs again.
//
// @param[in] aChannel Reference to the related channel object
// @param[in] aCount Data object count in receiving buffer. Not used in this implementation.
// @param[in] aDataLost Number of lost data items. Not used in this implementation. It does not
//     matter if some data event is lost.
// ---------------------------------------------------------------------------
//
void CCamAppController::DataReceived( CSensrvChannel& aChannel, 
                                      TInt aCount, 
                                      TInt aDataLost )
    {
    PRINT3( _L("Camera => CCamAppController::DataReceived aChannel %d aCount %d aDataLost %d"), 
                aChannel.GetChannelInfo().iChannelType, aCount, aDataLost );

    if ( KSensrvChannelTypeIdOrientationData == aChannel.GetChannelInfo().iChannelType )
        {
        // Rotation is used in primary camera whenever the feature is enabled by user.
        // A rotation is always set for secondary camera in portrait and upside down
        // portrait orientations due to the the viewfinder mirroring along the landscape axis.
        // This affects the final JPEG file.
        TBool rotate( ECamSettOn == iSettingsModel->IntegerSettingValue( ECamSettingItemImageRotation ) 
                      || iInfo.iActiveCamera == ECamActiveCameraSecondary );

        if( rotate )
            {
            TSensrvOrientationData orientationData;
            TPckg<TSensrvOrientationData> orientationPackage( orientationData );
            aChannel.GetData( orientationPackage );

            iImageOrientation =
                MapSensorOrientatio2CamOrientation( orientationData.iDeviceOrientation, 
                                                    iLastImageOrientation, 
                                                    iInfo.iActiveCamera );
            }
        else
            {
            // If "rotate images" setting is OFF, set "no rotation" value
            iImageOrientation = ECamOrientation0;
            }
        
        // rotate image if we are in still capture mode but not currently
        // capturing image, no rotation in burst mode or if orientation has stayed the same
        if( IsFlagOn( CameraControllerState(), ECamImageOn ) && 
            rotate && 
            iLastImageOrientation != iImageOrientation &&
            iImageOrientation != ECamOrientationIgnore)
            {
            if( ECamCapturing   != CurrentOperation() &&
                ECamFocusing    != CurrentOperation() &&
                ECamFocused     != CurrentOperation() &&
                ECamFocusFailed != CurrentOperation() &&
                ECamCompleting  != CurrentOperation() &&
                ECamImageCaptureBurst != iInfo.iImageMode &&
                ECamImageCaptureBurst != iInfo.iTargetImageMode )
                {
                PRINT( _L("Camera <> CCamAppController:DataReceived calling SetImageOrientationL()") );	
                TRAP_IGNORE( SetImageOrientationL() );
                }
            else
                {
                // queue a request to re-prepare still capture
                PRINT( _L("Camera <> CCamAppController: Queueing an orientation change event") );	
                iOrientationChangeOccured = ETrue;
                }
            }
        else
            {
            iOrientationChangeOccured = EFalse;
            }
        
        if( iImageOrientation != ECamOrientationIgnore )    
            {
            iLastImageOrientation = iImageOrientation; 	           
            }
        }

    PRINT ( _L("Camera <= CCamAppController::DataReceived") );
    }

// ---------------------------------------------------------------------------
// Data listening failed. 
// If error was fatal, channel has also been closed and sensor server session 
// terminated. If error was minor, some data has potentially been lost.
// iAccSensorChannel needs to be closed in fatal error case. A new channel will
// be created the next time we call UpdateSensorApiL().
//
// @param[in] aChannel Reference to the related channel object
// @param[in] aError Error code.
// ---------------------------------------------------------------------------
//
void CCamAppController::DataError( CSensrvChannel& aChannel, 
                                   TSensrvErrorSeverity aError )
    {
    PRINT2( _L("Camera => CCamAppController::DataError aChannel %d aError %d"), aChannel.GetChannelInfo().iChannelType, aError );	
    if ( ESensrvErrorSeverityFatal == aError ) 
        {
        // Delete sensor api object
        delete iAccSensorChannel;
        iAccSensorChannel = NULL;
        iAccSensorListening = EFalse;

        // Set orientation back to default if not already there.
        iImageOrientation   = ECamOrientation0;
        }
    }

// ---------------------------------------------------------------------------
// Returns a pointer to a specified interface 
//
// @since S60 5.0
// @param aInterfaceUid Identifier of the interface to be retrieved
// @param aInterface A reference to a pointer that retrieves the specified interface.
// ---------------------------------------------------------------------------
//
void CCamAppController::GetDataListenerInterfaceL( TUid aInterfaceUid, 
                                                   TAny*& aInterface )
    {
    aInterface = NULL;
    }                                
                                        
// ---------------------------------------------------------------------------
// CCamAppController::SetImageOrientationL()
// Setup image rotation parameters
//
// ---------------------------------------------------------------------------
//  
void CCamAppController::SetImageOrientationL()
    {
    PRINT( _L("Camera => CCamAppController::SetImageOrientationL") );
    if( iConfigManager && iConfigManager->IsOrientationSensorSupported()
    		&& iCameraController )
        {
        // Camera controller asks for the current orientation through
        // our ImageOrientation(). Value for that was updated in 
        // DataReceived() callback.
        iCameraController->DirectSettingsChangeL( ECameraSettingOrientation );
        }
    PRINT( _L("Camera <= CCamAppController::SetImageOrientationL"))   
    }


// ---------------------------------------------------------------------------
// MapSensorOrientatio2CamOrientation
// ---------------------------------------------------------------------------
//
TCamImageOrientation 
CCamAppController::MapSensorOrientatio2CamOrientation( 
    const TSensrvOrientationData::TSensrvDeviceOrientation& aSensorOrientation, TCamImageOrientation aLastImageOrientation,
    TCamActiveCamera aActiveCamera )
    {
    PRINT1 ( _L("Camera => CCamAppController::MapSensorOrientatio2CamOrientation aSensorOrientation: %d"), aSensorOrientation );   

    TCamImageOrientation cameraOrientation( ECamOrientation0 );

    // Primary camera rotation
    if ( aActiveCamera == ECamActiveCameraPrimary ) 
      {
      switch( aSensorOrientation )
        {        
        case TSensrvOrientationData::EOrientationDisplayUpwards:
            // If coming from upside down portrait...
            if ( ECamOrientation270 == aLastImageOrientation )
              {
            	// Switch from upside down portrait to normal portrait.") )
              cameraOrientation = ECamOrientation90; // Set normal portrait
              }
            // If coming from upside down landscape...
            else if ( ECamOrientation180 == aLastImageOrientation )
              {
            	// Switch from upside down landscape to normal landscape...") )
              cameraOrientation = ECamOrientation0; // Set normal lanscape
              }
            // If neither one, keep the current image orientation.
            else
              {
              cameraOrientation = ECamOrientationIgnore;
              }
            break;

        case TSensrvOrientationData::EOrientationDisplayDownwards:
        	  // Camera is pointing up now. Check if it was upside down previously.
            // If coming from upside down portrait...
            if ( ECamOrientation270 == aLastImageOrientation )
              {
            	// Switch from upside down portrait to normal portrait...") )
              cameraOrientation = ECamOrientation90; // Set normal portrait
              }
            // If coming from upside down landscape...
            else if ( ECamOrientation180 == aLastImageOrientation )
              {
            	// Switch from upside down landscape to normal landscape...") )
              cameraOrientation = ECamOrientation0; // Set normal lanscape
              }
            // If neither one, keep the current image orientation.
            else
              {
              cameraOrientation = ECamOrientationIgnore;
              }
        	  break;
        case TSensrvOrientationData::EOrientationDisplayRightUp:            
        case TSensrvOrientationData::EOrientationUndefined:
            cameraOrientation = ECamOrientation0;     
            break;
        case TSensrvOrientationData::EOrientationDisplayUp:
            cameraOrientation = ECamOrientation90;
            break;
        case TSensrvOrientationData::EOrientationDisplayLeftUp:
            cameraOrientation = ECamOrientation180;
            break;
        case TSensrvOrientationData::EOrientationDisplayDown:
            cameraOrientation = ECamOrientation270;
            break;
        default:
            PRINT( _L("Camera <> Unexpected orientation value") );
            break;
        }
      }
    // Secondary camera rotations
    else if ( aActiveCamera == ECamActiveCameraSecondary )
      {
      if ( aSensorOrientation == TSensrvOrientationData::EOrientationDisplayUp ) // Portrait
        {
        cameraOrientation = ECamOrientation180; 
        }
      else if ( aSensorOrientation == TSensrvOrientationData::EOrientationDisplayDown )  // Upside down portrait
        {
        cameraOrientation = ECamOrientation180;
        }
      else
        {
        cameraOrientation = ECamOrientation0;
        }
      }
      
    PRINT1( _L("Camera <= CCamAppController::MapSensorOrientatio2CamOrientation, return [%s]"), 
            KCamOrientationNames[cameraOrientation] );

    return cameraOrientation;
    }

// ---------------------------------------------------------------------------
// MapCamOrientation2RotationAngle
// ---------------------------------------------------------------------------
//
CBitmapRotator::TRotationAngle 
CCamAppController::MapCamOrientation2RotationAngle( 
                   const TCamImageOrientation aOrientation )
    {
    PRINT1 ( _L("Camera => CCamAppController::MapCamOrientation2RotationAngle aOrientation: %d"), aOrientation );   

    CBitmapRotator::TRotationAngle angle;
    
    switch( aOrientation )
        {        
        case ECamOrientation90:
            angle = CBitmapRotator::ERotation90DegreesClockwise;
            break;
        case ECamOrientation180:
            angle = CBitmapRotator::ERotation180DegreesClockwise;
            break;
        case ECamOrientation270:
            angle = CBitmapRotator::ERotation270DegreesClockwise;
            break;
        default:
            PRINT( _L("Camera <> Unexpected orientation value") );
            // using a value to avoid compiler warning
            angle = CBitmapRotator::ERotation90DegreesClockwise;
            break;
        }
    PRINT1( _L("Camera <= CCamAppController::MapCamOrientation2RotationAngle, return %d"), angle );
    return angle;    
    }
        
        
// ---------------------------------------------------------------------------
// CCamAppController::UpdateSensorApiL() <<public>>
// Updates the sensorApi object when the app focus changes
//
// ---------------------------------------------------------------------------
//  

void CCamAppController::UpdateSensorApiL(TBool aStartupApi) 
    {
    PRINT(_L("Camera => CCamAppController::UpdateSensorApiL"));
    if( iConfigManager && iConfigManager->IsOrientationSensorSupported() )
    	{
	    if(aStartupApi)
	        {
	        // only activate the orientation channel for image mode	
	        if( ECamControllerImage == CurrentMode() )
	          {
	        // This if-statement intentionally not a condition of the first if.
	        // If the iAccSensorChannel is already active, we don't want to recreate it
	        // and leak memory, but at the same time, we don't want to destroy it either.
	        
	        if( !iAccSensorChannel )
	            {
	            PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - Starting new sensor channel api - info not yet initialized"));
	            //Construct a channel finder.
	            CSensrvChannelFinder* channelFinder;
	            PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - calling CSensrvChannelFinder::NewL()"));
	            //CSensorChannelFinder* channelFinder;
	            channelFinder = CSensrvChannelFinder::NewL();
	            //channelFinder = CSensorChannelFinder::NewL();
	            PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - CSensrvChannelFinder::NewL() returned without a leave"));
	            CleanupStack::PushL( channelFinder );    
	            
	            //List of found channels.
	            RSensrvChannelInfoList channelInfoList;
	            CleanupClosePushL( channelInfoList );
	  
	            //Create and fill channel search criteria.
	            TSensrvChannelInfo channelInfo;
	            channelInfo.iChannelType = KSensrvChannelTypeIdOrientationData;
	        
	            PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - calling channelFinder->FindChannelsL"));
	            //Find the orientation channel
	            channelFinder->FindChannelsL( channelInfoList, channelInfo );
	           
	            if( channelInfoList.Count() != 1 )
	                {
	                //The device doesn’t support orientation data channel or
	                //there are several orientation channels.
	                PRINT1(_L("Camera <=> CCamAppController::UpdateSensorApiL - The device doesn’t support orientation data channel or there are several orientation channels: %d channels found"),channelInfoList.Count());
	                User::Leave( KErrNotSupported );
	                }
	            else
	                {
	                //orientation channel found 
	                PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - orientation channel found"));
	                }
	            //Open the orientation channel.
	            //When the channel object is created the channel info object 
	            //must be an object returned by CSensrvChannelFinder::FindChannelsL().
	            PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - calling CSensrvChannel::NewL"));
	            iAccSensorChannel = CSensrvChannel::NewL( channelInfoList[ 0 ] );
	            //iAccSensorChannel = CSensorChannel::NewL( channelInfoList[ 0 ] );
	            //CleanupStack::PushL( iAccSensorChannel ); 
	            CleanupStack::PopAndDestroy( &channelInfoList ); //Close() is being called on "channelInfoList"
	            CleanupStack::PopAndDestroy( channelFinder );
	          }
	        
	        if( !iAccSensorListening )
	            {
	            PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - calling iAccSensorChannel->OpenChannelL()"));
	            TRAPD(channelerror, iAccSensorChannel->OpenChannelL() );
	            if (channelerror!=KErrNone)
	                {
	                PRINT1(_L("CCamAppController::UpdateSensorApiL - iAccSensorChannel->OpenChannelL() failed. Error code: %d"),channelerror);
	                User::Leave( channelerror );
	                }
	            //iAccSensorChannel->OpenChannelL();
	            //orientation channel is now open.
	            
	            // start listening
	            PRINT(_L("Camera <=> CCamAppController::UpdateSensorApiL - calling iAccSensorChannel->StartDataListeningL"));
	            iAccSensorChannel->StartDataListeningL( this, //this object is data listener for this channel
	                                                   1, //aDesiredCount is one, i.e. each orientation change is notified separately
	                                                   1, //aMaximumCount is one, i.e. object count in receiving data buffer is one
	                                                   0 );//buffering period is not used
	            iAccSensorListening = ETrue;
	            iLastImageOrientation = ECamOrientation0;
	            }
	          }
	        }
	    else // shut down the sensorApi object
	        {
	        PRINT(_L("Camera <=> UpdateSensorApiL shutting down iAccSensor"))
	        if(iAccSensorListening)
	            {
	            // Stop listening to the events
	            iAccSensorChannel->StopDataListening();
	            iAccSensorListening = EFalse;
	            }
	            
	        // Delete sensor api object
	        delete iAccSensorChannel;
	        iAccSensorChannel = NULL;
	        }
      }
    PRINT(_L("Camera <= CCamAppController::UpdateSensorApiL"));
    }



// ---------------------------------------------------------------------------
// CCamAppController::HandlePropertyChangedL()
// Handle changes in specified property
//
// ---------------------------------------------------------------------------
//  
void CCamAppController::HandlePropertyChangedL( const TUid& aCategory, const TUint aKey )
  {
  PRINT( _L("Camera => CCamAppController::HandlePropertyChangedL"))
#if !( defined(__WINS__) || defined(__WINSCW__) )  
  
  if ( iConfigManager && iConfigManager->IsLensCoverSupported() )
      { 
      // First if condition could be removed after PCFW has released 
      // creation for KLensCoverStatus key
      if ( ( aCategory == NMusResourceApi::KCategoryUid && 
           aKey == NMusResourceApi::KCameraAvailability ) ||              
           ( aCategory == CameraPlatPSKeys::KPSCameraPlatUid && 
           aKey == CameraPlatPSKeys::KLensCoverStatus ) )        
          {    

          TInt err = iSlideStateWatcher->Get( iNewSlideState );

          PRINT2( _L( "Camera HandlePropertyChangedL getting slider state = %d, err = %d" ), iNewSlideState, err )
          
          if ( iNewSlideState != iSlideState  )
              {
              delete iSliderCallBack;
              iSliderCallBack = NULL;
              iSliderCallBack = CPeriodic::NewL( CActive::EPriorityIdle );
              iSliderCallBack->Start( KLensCoverDelay, KLensCoverDelay, TCallBack( LensCoverUpdateL, this ) );
              }
          }
      else
          {
          (void)aCategory;
          (void)aKey;
          }
      }
      
      // if its key lock state changed
      if ( aCategory == KPSUidAvkonDomain && aKey == KAknKeyguardStatus )
          {
          PRINT( _L("Camera <> aCategory == KPSUidAvkonDomain && aKey == KAknKeyguardStatus") );
          if ( !IsKeyLockOn() )
              {
              PRINT( _L("Camera <> !IsKeyLockOn()") );
              // if in standby view and the slide is opened then exit standby view
              CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
              TVwsViewId activeView;
              appUi->GetActiveViewId( activeView );
              CCamPreCaptureViewBase* view = static_cast<CCamPreCaptureViewBase*>( appUi->View( activeView.iViewUid ) );
              if ( iInfo.iOperation == ECamStandby ||
                   ( view && view->IsInStandbyMode() ) && appUi->IsRecoverableStatus() )
                  {
                   PRINT( _L("Camera HandleSlideOpenedL => Exit Standby view") );
                   view->HandleCommandL( ECamCmdExitStandby );
                  }
              }
          }
      
      if ( aCategory == KPSUidProfileEngine && aKey == KProEngActiveProfileChanged )
          {
          PRINT( _L("Camera <> aCategory == KCRUidProfileEngine && aKey == KProEngActiveWarningTones") );
          IsProfileSilent();
          }

#endif // !( defined(__WINS__) || defined(__WINSCW__        

  PRINT( _L("Camera <= CCamAppController::HandlePropertyChangedL"))

  }                             

// ---------------------------------------------------------------------------
// CCamAppController::LensCoverUpdate()
// Callback function that is called when lens cover state changes.
// Used to implement delayed handling of lens cover events.
// ---------------------------------------------------------------------------
//  
TInt CCamAppController::LensCoverUpdateL( TAny* aPtr ) 
    {
    PRINT( _L( "Camera => CCamAppController::LensCoverUpdateL()" ) )
    CCamAppController* self = static_cast<CCamAppController*>( aPtr );
    if ( self->iNewSlideState != self->iSlideState ||
         self->iNewSlideState == CameraPlatPSKeys::EClosed // always handle closing
       )
        {

        self->iSlideState = self->iNewSlideState;

        if ( self->iSlideState == CameraPlatPSKeys::EClosed )
            {
            PRINT( _L( "Camera => CCamAppController::LensCoverUpdateL() SLIDE CLOSED" ) )
            self->HandleSlideClosedL();
            }
        else if ( self->iSlideState == CameraPlatPSKeys::EOpen )
            {
            PRINT( _L( "Camera => CCamAppController::LensCoverUpdateL() SLIDE OPENED" ) )
            self->HandleSlideOpenedL();
            }
        }
    self->iSliderCallBack->Cancel();
    delete self->iSliderCallBack;
    self->iSliderCallBack = 0;
            
    PRINT( _L( "Camera <= CCamAppController::LensCoverUpdateL()" ) )
    return KErrNone;

    }
// ---------------------------------------------------------------------------
// CCamAppController::RefreshSlideState
// Force a refresh of the slide status
// ---------------------------------------------------------------------------
//
void CCamAppController::RefreshSlideStatus()
    {
    PRINT( _L("Camera => CCamAppController::RefreshSlideStatus") );  
    // read the slider status from P & S key
    if ( iSlideStateWatcher->Get( iSlideState ) != KErrNone )
           {
           iSlideState = KErrNone;
           }   
    }
      
// ---------------------------------------------------------------------------
// CCamAppController::HandleSlideClosedL
// Handle the slide closed event
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleSlideClosedL()
    {
    PRINT( _L("Camera => CCamAppController::HandleSlideClosedL") );

    NotifyControllerObservers( ECamEventSliderClosed );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    
    if ( appUi->IsInPretendExit() )
        {
        PRINT( _L("Camera <= CCamAppController::HandleSlideClosedL already in pretend exit") );        
        return;
        }
    SetCameraSwitchRequired( ESwitchDone );
    // shutdown the camera on slide closing if EDGE variant/standalone app
    // don't close app if embedded or embedding
    TInt camerasAvailable = CamerasAvailable();
    TVwsViewId activeView;
    TInt viewErr = appUi->GetActiveViewId( activeView );

    TBool embedded = appUi->IsEmbedded();
    TBool embedding = appUi->Embedding();
    if ( camerasAvailable == 1 ) // EDGE variant
        {
        PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - camerasAvailable == 1") );
        // if embedding another app then don't exit just yet
        if ( embedding )
            {
            PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - Embedding mode") );
            // remember that the slider has been closed
            iSliderCloseEvent = ETrue;
            }
        // Embedded post capture view
        else if ( embedded && viewErr == KErrNone && 
                ( activeView.iViewUid.iUid == ECamViewIdStillPostCapture ||
                  activeView.iViewUid.iUid == ECamViewIdVideoPostCapture ) )
            {
            PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - Embedded post capture") );
            // remember that the slider has been closed
            iSliderCloseEvent = ETrue;
            }
        // if standalone app performing a burst capture, 
        // don't exit till complete
        else if ( ECamImageCaptureBurst == iInfo.iImageMode ||
                ECamImageCaptureTimeLapse == iInfo.iImageMode )
            {
            // remember that the slider has been closed
            iSliderCloseEvent = ETrue;
            StopSequenceCaptureL();
            }
        else // standalone or embedded pre capture view
            {
            PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - emit ECamCmdSlideClosedExit") );
            appUi->HandleCommandL( ECamCmdSlideClosedExit );
            }
        }
    else // variants with > 1 camera
        {
        PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - camerasAvailable > 1") );
        if ( !embedding && !embedded )
            {
            // if standalone app performing a burst capture, 
            // don't exit till complete
            if ( ECamImageCaptureBurst == iInfo.iImageMode ||
                 ECamImageCaptureTimeLapse == iInfo.iImageMode )
                {
                // remember that the slider has been closed
                iSliderCloseEvent = ETrue;
                StopSequenceCaptureL();
                }
            else
                {
                PRINT( _L("Camera Normal mode-handle ECamCmdSlideClosedExit") );
                appUi->SetLensCoverExit( ETrue ); 
                if ( ECamViewIdVideoPreCapture == activeView.iViewUid.iUid ) 
                   {
                   // stop recording and save the video
                   StopVideoRecording();                          
                   }
                PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - emit ECamCmdSlideClosedExit") );
                appUi->HandleCommandL( ECamCmdSlideClosedExit );
                
				// 2ndary camera, slider closed -> application to be closed, 
				// not in embedded mode
                if ( iInfo.iActiveCamera == ECamActiveCameraSecondary )
                   {
                   PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - 2ndary camera slider closed, app closing...") );
                   appUi->CloseAppL(); 
                   }
                }
            }
        else // we are embedded in an app or embedding another app
            {
            // remember that the slider has been closed
            PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - Embedded/Embedding mode") );
            
            if ( viewErr == KErrNone )
                {
                iSliderCloseEvent = ETrue;

                PRINT1( _L("Camera <> CCamAppController::HandleSlideClosedL - active view %d"), activeView.iViewUid.iUid );
                PRINT1( _L("Camera <> CCamAppController::HandleSlideClosedL - iInfo.iOperation %d"), iInfo.iOperation );
    
                // switch the camera if slide is closed and primary camera
                // is active in embedded mode
                switch ( activeView.iViewUid.iUid )
                    {
                    case ECamViewIdStillPreCapture:
                    case ECamViewIdVideoPreCapture:
                        {
                        // if in pre capture view and slide is closed
                        if ( iInfo.iActiveCamera == ECamActiveCameraPrimary )
                            {
                            if ( iInfo.iOperation != ECamPaused &&
                                 iInfo.iOperation != ECamCapturing &&
                                 iInfo.iOperation != ECamCompleting && 
                                 iInfo.iOperation != ECamPausing &&
                                 iInfo.iOperation != ECamResuming &&
                                 !iVideoRequested )
                                {
                                appUi->HandleCommandL( ECamCmdSwitchCamera );
                                }
                            else if ( iInfo.iOperation == ECamCapturing ||
                                      iInfo.iOperation == ECamPaused )
                                {
                                StopVideoRecording();                                
                                }
                            else if ( iVideoRequested )
                                {
                                StopVideoRecording();
                                // cancel any outstanding sound requests
                                iSoundPlayer->CancelAllPlaying();

                                appUi->HandleCommandL( ECamCmdSwitchCamera );
                                }
                            else // lint
                                {
                                }
                            }
                         else 
                            {
                            // Embedded camera not closed even if lens cover is closed.
                            PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - 2ndary camera, no action..") );
                            }
                        }
                        break;
                    case ECamViewIdStillPostCapture:
                    case ECamViewIdVideoPostCapture:
                        {
                        // Embedded camera not closed even if lens cover is closed.
                        }
                        break;
                    default:
                        break;
                    }
                }
               else if ( viewErr == KErrNotFound  )
                {
                PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - view not found") );
                if ( ECamActiveCameraPrimary == iInfo.iActiveCamera ) 
                    {              
                    PRINT( _L("Camera <> CCamAppController::HandleSlideClosedL - need switch to 2ndary") );
                    SetCameraSwitchRequired( ESwitchPrimaryToSecondary );
                    if ( !appUi->AppInBackground( ETrue ) )
                        {
                        appUi->HandleCommandL( ECamCmdSwitchCamera );
                        }
                    }
                }
            }
        }

    PRINT( _L("Camera <= CCamAppController::HandleSlideClosedL") );
    }

// ---------------------------------------------------------------------------
// CCamAppController::HandleSlideOpenedL
// Handles the slide opened event
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleSlideOpenedL()
    {
    PRINT( _L("Camera => CCamAppController::HandleSlideOpenedL") );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    // reset slider event flag
    if ( iSliderCloseEvent )
        {
        iSliderCloseEvent = EFalse;
        }

    TVwsViewId activeView;
    TInt viewErr = appUi->GetActiveViewId( activeView );
    if ( viewErr == KErrNone )
        {
        PRINT1( _L("Camera HandleSlideOpenedL => active view %d"), 
                activeView.iViewUid.iUid );

        // For EDGE variant 
        TInt camerasAvailable = CamerasAvailable();
        if ( camerasAvailable == 1 )
            {
            // if in standby view and the slide is opened then exit standby view
            if ( iInfo.iOperation == ECamStandby )
                {
                PRINT( _L("Camera HandleSlideOpenedL => Exit Standby view") );
                appUi->View( activeView.iViewUid )->HandleCommandL( 
                        ECamCmdExitStandby );
                }
            }
        else // variants with > 1 camera
            {
            PRINT1( _L("Camera HandleSlideOpenedL => iInfo.iOperation %d"), 
                    iInfo.iOperation );
            PRINT1( _L("Camera HandleSlideOpenedL => CameraState() %d"), 
                    CameraState());
            switch ( activeView.iViewUid.iUid )
                {
                // if we are in the photo/video capture view and the
                // second camera is enabled then switch to the main camera
                // so long as video/photo capture is not in progress
                case ECamViewIdStillPreCapture:
                case ECamViewIdVideoPreCapture:
                    {
                    if ( iInfo.iActiveCamera == ECamActiveCameraSecondary &&
                         iInfo.iOperation != ECamCapturing &&
                         iInfo.iOperation != ECamPaused &&
                         iInfo.iOperation != ECamPausing &&
                         iInfo.iOperation != ECamResuming &&
                         iInfo.iOperation != ECamCompleting && 
                         (CameraState() == ECamCameraPreparedImage ||
                         CameraState() == ECamCameraPreparedVideo
                         ||
                         CameraState() == ECamCameraReserved ||
                         CameraState() == ECamCameraPowerOn
                         ) &&
                         !VideoRecordPending() )
                        {
                        PRINT( _L("Camera precapture 2nd camera - switching now") ); 
                        SetCameraSwitchRequired( ESwitchSecondaryToPrimary );
                        if ( !appUi->AppInBackground( ETrue ) )
                            {
                            appUi->HandleCommandL( ECamCmdSwitchCamera );
                            }
                        }
                    else 
                        {
                        PRINT( _L("Camera capturing, paused etc. in precapview") );  
                        
                        if ( ECamViewIdVideoPreCapture == activeView.iViewUid.iUid && 
                           ( ECamCapturing == iInfo.iOperation || 
                             ECamPaused == iInfo.iOperation ) ) 
                            {
                            StopVideoRecording(); // stop recording and save the video                         
                            }
                        if ( ECamActiveCameraSecondary == iInfo.iActiveCamera ) 
                            {              
                            PRINT( _L("Camera HandleSlideOpenedL, switchCamera") );
                            SetCameraSwitchRequired( ESwitchSecondaryToPrimary );
                            if ( !appUi->AppInBackground( ETrue ) )
                                {
                                appUi->HandleCommandL( ECamCmdSwitchCamera );
                                }
                            }
                        }
                    }    
                    break;
                case ECamViewIdStillPostCapture:
                case ECamViewIdVideoPostCapture:
                    {
                    if ( ECamActiveCameraSecondary == iInfo.iActiveCamera )
                       {                    
                       SetCameraSwitchRequired( ESwitchSecondaryToPrimary );
                       if ( !appUi->AppInBackground( ETrue ) )
                           {
                           appUi->HandleCommandL( ECamCmdSwitchCamera );
                           }
                       }
                    }
                    break;
                default:
                    break;
                }
            }       
        }

    else if ( viewErr == KErrNotFound && 
                iInfo.iActiveCamera == ECamActiveCameraSecondary )
          {
          // If no view is found it means that camera is in background.
          // We don't want to do camera switch in embdedded/embedding mode
          // post-capture when in background
          if ( ( !appUi->IsEmbedded() &&
                 !appUi->Embedding() ) ||
               appUi->CurrentViewState() != ECamViewStatePostCapture )
              {
              PRINT( _L("Camera HandleSlideOpenedL => view not found, 2nd active") );
              iCameraSwitchRequired = ESwitchToUnknown;
              }
          }
    else
        {
        }
    NotifyControllerObservers( ECamEventSliderOpen );
    PRINT( _L("Camera <= CamAppController::HandleSlideOpenedL") );
    }


// ---------------------------------------------------------------------------
// CCamAppController::VideoViewFinderResourceId()
// Resource Id for video viewfinder layout
//
// ---------------------------------------------------------------------------
//    
TInt CCamAppController::VideoViewFinderResourceId( TCamVideoResolution aResolution )
  {
  PRINT( _L( "Camera => CCamAppController::VideoViewFinderResourceIdL"))
  TInt vidVFRes;
  
    TCamOrientation orient = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->CamOrientation();   
    if( orient == ECamOrientationCamcorder || orient == ECamOrientationCamcorderLeft )
      {
      vidVFRes = CamUtility::MapVideoQualityToViewFinderRes( aResolution );
      }
  else
    {
    TInt resId;
    TInt err = CamUtility::GetPsiInt( ECamPsiVideoVFResSecondaryCamera, resId );
    if( err )
      {
      vidVFRes = ROID( R_CAM_VIEWFINDER_RECT_CIF_ID );
      }
    else
      {
      vidVFRes = ROID( resId );
      }
    }
    
  PRINT1( _L( "Camera => CCamAppController::VideoViewFinderResourceId res id "), vidVFRes )
  return vidVFRes;
  }

// ---------------------------------------------------------------------------
// CCamAppController::HandleObservedEvent()
// Receives event codes from observables
//
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleObservedEvent( TCamObserverEvent aEvent )
    {
    switch( aEvent )
        {
        case ECamObserverEventCaptureProcessComplete:
            {
            NotifyControllerObservers( ECamEventExitRequested, KErrNone );
            }
            break;
        default:
            break;            
        }
    }

// ---------------------------------------------------------------------------
// CCamAppController::IsProfileSilent
// Return whether current profile is silent or not, uses warning
// tones setting to determine whether silent or not
// ---------------------------------------------------------------------------
//  
TBool CCamAppController::IsProfileSilent()
    {
    if ( !iShutterSndAlwaysOn )
        {
        TRAPD( ignore, iSilentProfile = IsProfileSilentL() );
        if ( ignore )
            {
            // If reading the warning tone (= camera tones) value fails
            // we set tones off by default
            iSilentProfile = ETrue;
            }
        return iSilentProfile;
        }
    else
        {
        return EFalse;
        }
    }


// ---------------------------------------------------------------------------
// CCamAppController::IsProfileSilentL
// Return whether current profile is silent or not, uses warning
// tones setting to determine whether silent or not
// ---------------------------------------------------------------------------
//  
TBool CCamAppController::IsProfileSilentL()
    {
    // If camera tones have been set off from settings, return true here 
    if ( iSettingsModel->IntegerSettingValue( ECamSettingItemPhotoCaptureTone )
                == ECamSettToneOff )
        {
        return ETrue;
        }
    
    // Get current keypad volume as an indication of whether
    // or not we have a silent profile
    CRepository* cr = CRepository::NewLC( KCRUidProfileEngine );
    TInt value;
    User::LeaveIfError( cr->Get( KProEngActiveWarningTones, value ) );
    PRINT1(_L("Camera <> CCamAppController::IsProfileSilentL() value = %d"), value)
    CleanupStack::PopAndDestroy( cr );
    
    return ( value == 0 );
    }


// ---------------------------------------------------------------------------
// ResetInactivityTimer <<static>>
// ---------------------------------------------------------------------------
//  
TInt 
CCamAppController::ResetInactivityTimer( TAny* /*aPtr*/ )
  {
	PRINT( _L("Camera =><= CCamAppController::ResetInactivityTimer") );

  User::ResetInactivityTime();

	return KErrNone;
  }
    
// ---------------------------------------------------------------------------
// CCamAppController::ToneShouldBeSilent
// Returns ETrue if the supplied toneId should be silent if the profile is 
// set to silent. Does *not* take into account network variant forcing 
// sounds on.
// ---------------------------------------------------------------------------
//      
TBool CCamAppController::ToneShouldBeSilent( TCamSoundId aSoundId )
    {    
    if ( aSoundId == CaptureToneId() ||
         aSoundId == ECamVideoStartSoundId ||
         aSoundId == ECamVideoStopSoundId ||
         aSoundId == ECamVideoPauseSoundId ||
         aSoundId == ECamVideoResumeSoundId ||
         aSoundId == ECamAutoFocusComplete ||
         aSoundId == ECamSelfTimerSoundId )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }         
    }
    
// ---------------------------------------------------------------------------
// CCamAppController::HandleCaptureCompletion
// Inform any observers that are waiting for a capture completion event
// ---------------------------------------------------------------------------
//  
void CCamAppController::HandleCaptureCompletion()
  {
  PRINT( _L( "Camera => CCamAppController::HandleCaptureCompletion()" ) )

  if ( iInfo.iOperation != ECamFocused && iInfo.iOperation != ECamFocusing )
      {
      SetOperation( ECamNoOperation );
      }
  
  // Re-enable screensaver
  EnableScreenSaver( ETrue );

  // If mmc dismount notification came while saving, allow the dismount now
  if( iDismountPending )
    {
    iDismountPending = EFalse;
    iDriveChangeNotifier->SendAllowDismount();	
    }
  
  // If any observers are waiting for a capture completion event
  if ( iCaptureCompletionObserverHandler )
    {
    iCaptureCompletionObserverHandler->BroadcastEvent( ECamObserverEventCaptureProcessComplete );
    delete iCaptureCompletionObserverHandler;
    iCaptureCompletionObserverHandler = NULL;
    }

  // If the camera orientation changed during capture and not in burst mode, 
  // set the new orientation
  if ( iConfigManager && iConfigManager->IsOrientationSensorSupported()
       && iOrientationChangeOccured
       && iInfo.iImageMode != ECamImageCaptureBurst )
    {
    iOrientationChangeOccured = EFalse;
    TRAP_IGNORE( SetImageOrientationL() );
    }

  if ( iPendingRelease )
    {
       PRINT( _L( "Camera <> CCamAppController::HandleCaptureCompletion(), DATAMAKE2" ) )        	
       ReleaseCamera();
    }
  PRINT( _L( "Camera <= CCamAppController::HandleCaptureCompletion()" ) )        
  }    



// ---------------------------------------------------------------------------
// CCamAppController::CheckAfModeForScene
// Checks the current scene/based on scene to see if AF mode needs setting
// ---------------------------------------------------------------------------
//           
void CCamAppController::CheckAfModeForScene( TBool aForceAFReset )
    {
    PRINT(_L("Camera => CCamAppController::CheckAfModeForScene"));

    if ( iConfigManager && iConfigManager->IsAutoFocusSupported() )
        {
        TRAP_IGNORE( iCameraController->DirectRequestL( ECamRequestSetAfRange ) );
        }

    PRINT(_L("Camera <= CCamAppController::CheckAfModeForScene"));
    }


// ---------------------------------------------------------------------------
// CCamAppController::TryAutoFocus
// Start an AF operation, if needed
// ---------------------------------------------------------------------------
//      
TBool CCamAppController::TryAutoFocus()   
    {  
    PRINT( _L("Camera => CCamAppController::TryAutoFocus()") );  
    OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_TRYAUTOFOCUS, "e_CAM_APP_AF_LOCK 1" );  //CCORAPP_AF_LOCK_START
    
    if ( iConfigManager && iConfigManager->IsAutoFocusSupported() )
        {
        if( ECamActiveCameraPrimary != iInfo.iActiveCamera ||
            ECamControllerImage != iInfo.iMode )
          {
          PRINT( _L("Returning EFalse - Not using primary camera in still mode") );  
          // Not using primary camera in still mode      
          return EFalse; // Not focusing
          }
                
        PRINT( _L("Calling IssueDirectRequestL( ECamRequestStartAutofocus )") );       
        return TryAFRequest( ECamRequestStartAutofocus );
        }
    else 
        {
        return ETrue;
        }
    }


// ---------------------------------------------------------
// CCamAppController::CurrentSceneHasForcedFocus
// Returns whether the current scene has focus forced to 
// a particular value ( eg Macro/Landscape modes may have
// focus fixed to Macro/Infinite )
// ---------------------------------------------------------
//  
TBool CCamAppController::CurrentSceneHasForcedFocus() const
    {
    if ( iConfigManager && iConfigManager->IsAutoFocusSupported() )
        {
        // Work out what the current scene is (or if current scene is user
        // scene, what it's based on).
        TInt currentScene = IntegerSettingValue( ECamSettingItemDynamicPhotoScene );
        if ( currentScene  == ECamSceneUser )
            {
            currentScene = IntegerSettingValue( ECamSettingItemUserSceneBasedOnScene );
            }

        TBool isFaceTrackingOn = (iConfigManager &&
                    iConfigManager->IsFaceTrackingSupported() &&
                    (iSettingsModel->IntegerSettingValue( 
                    ECamSettingItemFaceTracking ) == ECamSettOn ) );
    
        // Check if the current (or based on) scene is one that should *not* have 
        // a reticule.
        if ( ( currentScene == ECamSceneScenery 
          || currentScene == ECamSceneNightScenery 
          || currentScene == ECamSceneSports )
          && !isFaceTrackingOn )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        return ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CCamAppController::CaptureToneDelayTimeout
// Timeout from capture tone delay timer
// ---------------------------------------------------------------------------
//
TInt CCamAppController::CaptureToneDelayTimeout( TAny* aPtr )
    {
    PRINT( _L("Camera => CCamAppController::CaptureToneDelayTimeoutL") )
    static_cast<CCamAppController*>( aPtr )->PlayDelayedCaptureTone();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCamAppController::PlayDelayedCaptureTone
// Play capture tone
// ---------------------------------------------------------------------------
//  
void CCamAppController::PlayDelayedCaptureTone()
    {
    PlaySound( CaptureToneId(), EFalse );
    }

// ---------------------------------------------------------------------------
// CCamAppController::InitAutoFocusL
// Initalises the AutoFocus interface, if needed
// ---------------------------------------------------------------------------
//
void CCamAppController::InitAutoFocusL()
    {
    PRINT( _L("Camera => CCamAppController::InitAutoFocusL") )
    PRINT(_L("Camera !! DISABLED"));
    PRINT( _L("Camera <= CCamAppController::InitAutoFocusL") )
    }

// ---------------------------------------------------------------------------
// CCamAppController::CopySnapshotIfNeededL
// Helper function for McaeoSnapImageReady that makes a copy of the snapshot
// if needed to fake a viewfinder or for image rotation.
// ---------------------------------------------------------------------------
//

void 
CCamAppController::CopySnapshotIfNeeded( const CFbsBitmap& aSnapshot, TInt aError )
  { 
  if ( ECamImageCaptureTimeLapse == iInfo.iImageMode 
    || ECamControllerVideo == iInfo.iMode   
    || ( iConfigManager && iConfigManager->IsOrientationSensorSupported() &&
        ( iSettingsModel->IntegerSettingValue( ECamSettingItemImageRotation ) == ECamSettOn 
          || iInfo.iActiveCamera == ECamActiveCameraSecondary ) )
     )
    {
    StopIdleTimer();
    delete iSnapShotCopy;
    iSnapShotCopy = NULL;
    TRAP_IGNORE ( ( iSnapShotCopy = new(ELeave) CFbsBitmap() ) );
    
    if ( iSnapShotCopy )
      {
      TInt dupeError = iSnapShotCopy->Duplicate( aSnapshot.Handle() );
      
      if ( dupeError )
        {
        iSnapShotCopy->Reset();
        delete iSnapShotCopy;
        iSnapShotCopy = NULL;
        }
      else if ( iConfigManager && !iConfigManager->IsOrientationSensorSupported() )
        {
        if( ECamImageCaptureTimeLapse == iInfo.iImageMode )
          {
          // simulate the receipt of a viewfinder frame using the snapshot image
          // McaeoViewFinderFrameReady( *iSnapShotCopy, aError );  
          HandleViewfinderFrame( aError, iSnapShotCopy );
          }
        }                
      }        
    }
  }



// ---------------------------------------------------------------------------
// CCamAppController::EnableScreenSaver
// Turns the screen saver on or off for timelapse capture
//
// ---------------------------------------------------------------------------
//  
void 
CCamAppController::EnableScreenSaver( TBool aEnable )  
  {
  if ( !aEnable ) 
    {
    // Stop the screensaver from appearing
    RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 1 );        
    }
  else
    {
    // Allow the screensaver to appear
    RProperty::Set( KPSUidScreenSaver, KScreenSaverAllowScreenSaver, 0 );
    User::ResetInactivityTime();      
    }        
  }


#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
// ---------------------------------------------------------------------------
// CCamAppController::SetPerformanceState
//
// ---------------------------------------------------------------------------
//  
void CCamAppController::SetPerformanceState( TCamPerformanceState aState )  
  {
  iPerformanceState = aState;
  }
#endif

// ---------------------------------------------------------------------------
// CCamAppController::StartLocationTrailL()
// Connects and starts the locationtrail
// ---------------------------------------------------------------------------
//
void CCamAppController::StartLocationTrailL()
  {
  PRINT( _L("Camera => CCamAppController::StartLocationTrailL") );
  CCamAppUi* appUI = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

  if( !iRepository )
    {
    iRepository = CRepository::NewL( KCRUidCameraappSettings ); 
    }

    TInt ftuValue=-1;
    TInt retErr=0;
    retErr = iRepository->Get( KCamCrFtuMessageFlag, ftuValue );
	if( ftuValue == 0 )
		{
		return;
		}

	if( iRepository )
		{
		delete  iRepository;
		iRepository = NULL;
		}


  // If a stop request is pending but the trail is being restarted, stop 
  // and delete the timer
  if( iLocationTrailTimer )
  	{
  	iLocationTrailTimer->Cancel();
   	delete iLocationTrailTimer;
   	iLocationTrailTimer = NULL;
   	}
  
  TInt err = KErrNone;
#if defined(RD_MDS_2_5) && !defined(__WINSCW__) 
  if ( !iLocationTrailStarted )
	  {
	  if ( !iLocationUtilityConnected ) 
	    {
	    PRINT( _L("Camera => CCamAppController::StartLocationTrailL - iLocationUtility.Connect()") );
	    TVwsViewId activeView;
	    if ( appUI->GetActiveViewId( activeView ) == KErrNone )
	      {
	      if( ECamViewIdStillPreCapture == activeView.iViewUid.iUid  )
	          {
	          CCamPreCaptureViewBase* view = static_cast<CCamPreCaptureViewBase*>( appUI->View( activeView.iViewUid ) );
	          view->SetLocationTrailConnecting();
	          }
	      }
	    User::LeaveIfError( iLocationUtility.Connect() );
	    iLocationUtilityConnected = ETrue;
	    }
	  err = iLocationUtility.StartLocationTrail( RLocationTrail::ECaptureAll );
	  }
#endif
  // If start fails, change the setting to off 
  if( KErrNone != err )
    {
    PRINT1( _L("Camera :: CCamAppController::StartLocationTrailL error:%d encountered while starting location trail"), err )
    
#if defined(RD_MDS_2_5) && !defined(__WINSCW__)
    // Disconnect from the location utility
    PRINT( _L("Camera => CCamAppController::StartLocationTrailL - Starting of LocationTrail failed -> iLocationUtility.Close()") );
    iLocationUtility.Close();
    iLocationUtilityConnected = EFalse;
    SetIntegerSettingValueL( ECamSettingItemRecLocation, ECamLocationOff );
#endif
    
    // Going to standby mode, location trail disabled
	  /*
    TVwsViewId activeView;
    if ( appUI->GetActiveViewId( activeView ) == KErrNone )
      {
      CCamViewBase* view = static_cast<CCamViewBase*>( appUI->View( activeView.iViewUid ) );
      view->SwitchToStandbyModeL( TCamAppViewIds(activeView.iViewUid.iUid), err );
      }
      */
    appUI->SetStandbyStatus( err );
    appUI->HandleCommandL( ECamCmdGoToStandby );
    }
  else
  	{
  	iLocationTrailStarted = ETrue;
  	}
  PRINT( _L("Camera <= CCamAppController::StartLocationTrailL") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::StopLocationTrail( TBool aCloseSession )
// Stops the locationtrail
// ---------------------------------------------------------------------------
//    
void CCamAppController::StopLocationTrail( TBool aCloseSession )
	  {
	  PRINT1( _L("Camera => CCamAppController::StopLocationTrail operation mode:%d"), iInfo.iOperation );
#if defined(RD_MDS_2_5) && !defined(__WINSCW__)
	  if( iLocationTrailStarted )
		{
		// if not ECamCapturing, ECamPausing, ECamPaused, ECamResuming or ECamCompleting
		if( (iInfo.iOperation < ECamCapturing || iInfo.iOperation > ECamCompleting ) && 
		    ( CurrentFullFileName() == KNullDesC || SavedCurrentImage() )
		  )
			{
			PRINT( _L("Camera :: CCamAppController::StopLocationTrail - stopping location trail") )
			iLocationUtility.StopLocationTrail();
			if ( aCloseSession ) 
			    {
			    PRINT( _L("Camera => CCamAppController::StartLocationTrailL - iLocationUtility.Close()") );
			    iLocationUtility.Close();
			    iLocationUtilityConnected = EFalse;
			    }
			iLocationTrailStarted = EFalse;
			
			// If a timer was used to stop the location trail, then cancel and delete the timer
	        if( iLocationTrailTimer )
	        	{
	        	iLocationTrailTimer->Cancel();
	        	delete iLocationTrailTimer;
	        	iLocationTrailTimer = NULL;
	        	}		
			}
		else
			{
			// Use a timer to close the trail in a few seconds
	    	PRINT( _L("Camera :: CCamAppController::StopLocationTrail - location trail in use") )
	    	if( !iLocationTrailTimer )
	    		{
	    		TRAP_IGNORE( iLocationTrailTimer = CCamTimer::NewL( KLocTrailCloseRetryTime, 
	    		  TCallBack( CCamAppController::RetryStopLocationTrail, this ) ) );
	    		}
	    	else // restart the timer
	    		{
				iLocationTrailTimer->Cancel();
				iLocationTrailTimer->SetTimeout( KLocTrailCloseRetryTime );
	    		}
	    	iLocationTrailTimer->StartTimer();
			}
		}
      else if ( aCloseSession ) 
        {
		PRINT( _L("Camera => CCamAppController::StartLocationTrailL - iLocationUtility.Close()") );
        // location trail already stopped, but we are exiting app, so close the utility
		iLocationUtility.Close();
		iLocationUtilityConnected = EFalse;
		}

	#endif
	  PRINT( _L("Camera <= CCamAppController::StopLocationTrail") );
	  }

// ---------------------------------------------------------------------------
// CCamAppController::RetryStopLocationTrail()
// Timer callback function to attempt to remove the location trail periodically
// ---------------------------------------------------------------------------
//    
TInt CCamAppController::RetryStopLocationTrail( TAny* aPtr )
	{
	PRINT( _L("Camera => CCamAppController::RetryStopLocationTrail") )
	static_cast<CCamAppController*>( aPtr )->StopLocationTrail();
	PRINT( _L("Camera <= CCamAppController::RetryStopLocationTrail") )
	return KErrNone;
	}
  
// -----------------------------------------------------------------------------
// CCamAppController::CaptureKeyPressedWhileImageSaving
// Whether or not a capture key pressed during image saving
// -----------------------------------------------------------------------------
//
TBool CCamAppController::CaptureKeyPressedWhileImageSaving() const
	{
	return iCaptureKeyPressedWhileImageSaving;
	}
	
// -----------------------------------------------------------------------------
// CCamAppController::SetCaptureKeyPressedWhileImageSaving
// -----------------------------------------------------------------------------
//
void CCamAppController::SetCaptureKeyPressedWhileImageSaving(TBool aCaptureKeyPressed)
	{
	iCaptureKeyPressedWhileImageSaving = aCaptureKeyPressed;
	}
  


// ---------------------------------------------------------------------------
// CamerasAvailable <<static>>
//
// Commonly needed in UI classes, so added also support to get this info
// through this class. This way we avoid unnecessary binding of 
// CCameraController and UI classes.
// ---------------------------------------------------------------------------
//
TInt 
CCamAppController::CamerasAvailable()
  {
  return CCamCameraController::CamerasAvailable();
  }

TBool 
CCamAppController::TimeLapseSupported()
  {
  return (ECamSupportOn == iSettingsModel->VariantInfo().iTimeLapseSupport);
  }
    
TBool 
CCamAppController::AlwaysOnSupported()
  {
  return (ECamSupportOn == iSettingsModel->VariantInfo().iAlwaysOnSupport);
  }

const TVersion& 
CCamAppController::Version()
  {
  return iSettingsModel->VariantInfo().iVersion;
  }


// <<public>>
TBool 
CCamAppController::Busy() const
  {
  PRINT1(_L("Camera <> CCamAppController::Busy:%d"), iBusyFlags );
  return (EBusyNone != iBusyFlags || CameraControllerBusy() );
  }

TBool 
CCamAppController::CameraControllerBusy() const
  {
  PRINT1(_L("Camera <> CCamAppController::CameraControllerBusy:%d"), iCameraController->ControllerInfo().iBusy );  
  return (iCameraController && ECamBusyOff != iCameraController->ControllerInfo().iBusy );
  }

// ===========================================================================
// From MCamCameraObserver

// ---------------------------------------------------------------------------
// HandleCameraEventL <<virtual>>
// ---------------------------------------------------------------------------
//
void 
CCamAppController
::HandleCameraEventL( TInt              aStatus, 
                      TCamCameraEventId aEventId, 
                      TAny*             aEventData /*= NULL*/ )
  {
#ifdef _DEBUG    
  // Log viewfinder events with PRINT_FRQ
  // #ifdef _DEBUG is used to avoid extra if in release builds
  // where logging is not used anyway.
  if( ECamCameraEventVfFrameReady == aEventId )  
    {
    PRINT_FRQ1( _L( "Camera => CCamAppController::HandleCameraEventL, event[%s]" ),
          KCamCameraEventNames[aEventId] );    
    }
  else
#endif // _DEBUG    
    {
    PRINT1( _L( "Camera => CCamAppController::HandleCameraEventL, event[%s]" ),
            KCamCameraEventNames[aEventId] );
    }
 
  // in case we get poweron event before appui is completely constructed
  // finish it here
  if ( aEventId == ECamCameraEventPowerOn )
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
      if ( iConfigManager && 
           iConfigManager->IsUIOrientationOverrideSupported() && 
           !appUi->IsUiConstructionComplete() )
          {
          appUi->AppUIConstructCallbackL( appUi );  
          }
      }       

  TInt handleError( KErrNone );

  if( ECamCameraEventVfFrameReady != aEventId &&
      KErrNone == aStatus || KErrDiskFull == aStatus || KErrCompletion == aStatus ) 
    {
    // Called for all events, except viewfinder frame ready event.    
    SetStateFromEvent( aEventId );
    }
  
  if( ECamCameraEventImageInit == aEventId ||
      ECamCameraEventVideoInit == aEventId )
    {  
    iSettingsRestoreNeeded = ETrue;  
    }


  // NOTE: Consider that the ones needing these events
  //       receive them directly from Camera Controller?
  switch( aEventId )
    {
    case ECamCameraEventVfStart:
      {
      CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
      // For forced focus scenes, do one autofocus request once vf starts if needed
      if( UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        if( CurrentSceneHasForcedFocus() || UiConfigManagerPtr()->IsAutofocusSetInHyperfocalAtStartup() ) 
            {
            PRINT( _L( "Camera => CCamAppController::HandleCameraEventL, TryAFRequest( ECamRequestCancelAutofocus )") );
            TryAFRequest( ECamRequestCancelAutofocus ); 
            }
        }
      if( iSettingsRestoreNeeded )
        {
        iSettingsRestoreNeeded = EFalse;  
        TRAP_IGNORE( iCameraController->DirectSettingsChangeL( ECameraSettingFacetracking ) );
        TRAP( aStatus, RestoreSettingsToCameraL() );  
        }
      // fixed toolbar is used only with touch devices
      if ( IsTouchScreenSupported() && appUi )
          {
          CAknToolbar* toolbar = appUi->CurrentFixedToolbar();
          if ( toolbar )
              {
              CAknToolbarExtension* toolbarextension = toolbar->ToolbarExtension();
              if ( toolbarextension && toolbarextension->IsShown() )
                  {
                  appUi->SetToolbarVisibility();// avoid flickering in settings view
                  }
              else
                  {
                  appUi->SubmergeToolbar();
                  }
              }
          PRINT( _L( "Camera <> SetToolbarVisibility done ") );
          }
      // else use AP    
      else
          {
          if( iConfigManager && iConfigManager->IsUIOrientationOverrideSupported() )
              {
              NotifyControllerObservers( ECamEventInitReady );
              }
          }

      if ( UiConfigManagerPtr()->IsLocationSupported() &&
              ECamLocationOn == IntegerSettingValue( ECamSettingItemRecLocation ) )
        {
        StartLocationTrailL();
        }

      //Check if profile is silent and/or camera tones are set off
      IsProfileSilent();

      break;
      }
    case ECamCameraEventVfFrameReady:
      {      
      HandleViewfinderFrame( aStatus, static_cast<CFbsBitmap*>( aEventData ) );
      
      if( KErrNone == aStatus && iSettingsRestoreNeeded )
        {
        iSettingsRestoreNeeded = EFalse;  
        TRAP( aStatus, RestoreSettingsToCameraL() );  
        }
      return;
      }
    case ECamCameraEventSsReady:
      {
      TRAP_IGNORE( HandleSnapshotEvent( aStatus, static_cast<CFbsBitmap*>( aEventData ) ) );
      
      if ( ECamImageCaptureBurst == iInfo.iImageMode &&
             CurrentCapturedCount() < CaptureLimit() )
            {
            // check memory level
            TInt memoryLeft = 0;
            HAL::Get( HALData::EMemoryRAMFree, memoryLeft );
                                          
            if ( memoryLeft < KCriticalMemoryLevel )
                {
                PRINT( _L("Camera <> RAM is below critical level #1") )
                // memory below critical level, need to stop capture
                SoftStopBurstL( ETrue );
                EventHandlingErrorRecovery( KErrNoMemory );
                }
            }
      
      break;
      }
    case ECamCameraEventImageData:
      {
//      TRAP_IGNORE( HandleImageCaptureEventL( aStatus, static_cast<HBufC8*>( aEventData ) ) ); 
      TRAP_IGNORE( HandleImageCaptureEventL( aStatus, static_cast<CCamBufferShare*>( aEventData ) ) );
      if ( ECamImageCaptureBurst == iInfo.iImageMode &&
             CurrentCapturedCount() < CaptureLimit() )
            {
            // check memory level
            TInt memoryLeft = 0;
            HAL::Get( HALData::EMemoryRAMFree, memoryLeft );
            if ( memoryLeft < KCriticalMemoryLevel )
                {
                PRINT( _L("Camera <> RAM is below critical level #2") )
                // memory below critical level, need to stop capture
                TRAP_IGNORE( iCameraController->DirectRequestL( ECamRequestImageCancel ) );
                EventHandlingErrorRecovery( KErrNoMemory );
                }
            }
      break;
      }
    case ECamCameraEventImageStop:
      {
      TRAP( aStatus, HandleImageStopEventL( aStatus, aEventData ? *static_cast<TInt*>( aEventData ) : 0 ) );
      break;
      }

    case ECamCameraEventAutofocusSuccessful: // Flowthrough
    case ECamCameraEventAutofocusFailed:
      {        
      // Autofocusing is done, do possible pending capture  
      SetAfNeeded( EFalse );
      if( iCaptureRequested && !EngineProcessingCapture() )
        {
        iCaptureRequested = EFalse;
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP7_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_APP_AF 0" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP8_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_PRI_AF_LOCK 0" );
        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_APP_SHOT_TO_SNAPSHOT 1" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_PRI_SHOT_TO_SNAPSHOT 1" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP2_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_PRI_SHOT_TO_SAVE 1" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP3_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_PRI_SHOT_TO_SHOT 1" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_APP_SHOT_TO_STILL 1" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_APP_CAPTURE_START 1" );
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP6_CCAMAPPCONTROLLER_HANDLECAMERAEVENTL, "e_CAM_PRI_SERIAL_SHOOTING 1" );
        
        // When image capture is started with touch capture button, a short
        // delay is added after AF completes, but before capturing starts to 
        // avoid blurry screen sometimes when VF is frozen and before the 
        // snapshot is drawn. Delay also helps to see focus reticule. 
        if( iTouchCapture )
            {
            User::After( 50000 );
            iTouchCapture = EFalse;
            }
        Capture();  
        }           
      break;  
      }
      
    case ECamCameraEventSettingsSingle:
      {
      TRAP_IGNORE( HandleSettingEventL( aStatus, aEventId, aEventData ) );
      return;
      }
    case ECamCameraEventVideoAsyncStop: 
    case ECamCameraEventVideoStop:
      {
      // we always want to handle these events. set the flag because
      // there is a possibility that some other event has come before
      // and caused the flag to be cleared
      SetFlags(iBusyFlags, EBusyRequest);
      if ( !IsMemoryAvailable( ECamMediaStorageCard, EFalse ) 
	     && CurrentStorage() == ECamMediaStorageCard )
      	{
        aStatus = KErrNone; 
      	}
      if( aStatus != KErrNone && aStatus != KErrDiskFull && aStatus != KErrCompletion 
              && !( aStatus == KErrPathNotFound && IsCaptureStoppedForUsb() ) )
          {
          iSaving = EFalse;
          SetOperation( ECamStandby, KErrGeneral );
          }
      else
          {
          // In case of no error, we are already in appropriate state
          }
	  break;
      }
    case ECamCameraEventIveRecover:  
      {
      if( !Busy() )
        {
        PRINT( _L( "Camera => CCamAppController::HandleCameraEventL - Issue new recovery sequence" ) );
        SetStateFromEvent( ECamCameraEventReserveLose );  
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        // Find target mode from appUi and set this controller target mode 
        if( appUi->TargetMode() ==  ECamControllerVideo )
            {
            PRINT( _L( "Camera => CCamAppController::HandleCameraEventL - recover to video mode" ) );
            SetTargetMode( ECamControllerVideo  );
            SetTargetImageMode( ECamImageCaptureNone );
            }
        else
            {
            PRINT( _L( "Camera => CCamAppController::HandleCameraEventL - recover to still mode" ) );
            SetTargetMode( ECamControllerImage );
            SetTargetImageMode( CurrentImageModeSetup() );
            }
        IssueModeChangeSequenceL();
        }
      else
        {
        PRINT( _L( "Camera => CCamAppController::HandleCameraEventL - Recovery sequence is active" ) );
        }
      break;
      }
    case ECamCameraEventSequenceEnd:
      {
      // only for image mode	
      if( iConfigManager && iConfigManager->IsOrientationSensorSupported()
          && ECamControllerImage == CurrentMode() )
        {	
        // If the camera orientation changed during initialization sequence and not in burst mode, 
        // set the new orientation
        if ( iOrientationChangeOccured &&
             iInfo.iImageMode != ECamImageCaptureBurst )  
          {
          iOrientationChangeOccured = EFalse;
          TRAP_IGNORE( SetImageOrientationL() );
          }
        if( !iAccSensorListening )
          {
          TRAP_IGNORE( UpdateSensorApiL( ETrue ) );            
          }
        }
        if ( aStatus == KErrCancel )
            {
            // sequence canceled, no need to event further
            return;
            }
        else if( aStatus == KErrNone )
            {
            //Reset standbystatus. Needed if recovered from error.
            if( IsAppUiAvailable() ) 
              {
              if ( !InVideocallOrRinging() )
                  {
                  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
                  appUi->SetStandbyStatus( aStatus );               
                  }
              } 
            }
      break;
      }
    //Image capture event  
    case ECamCameraEventImageCaptureEvent:
      {
      PlaySound( CaptureToneId(), EFalse );
      }
      break;  
    default:
      break;
    }


  // -------------------------------------------------------
  // We have requested a series of operations.
  // The sequences have ids, which we use to determine
  // the right procedure. React when sequence ends.
  if( IsFlagOn( iBusyFlags, EBusySequence ) )
    {
    PRINT( _L( "Camera <> Sequence of requests pending" ) );
    TRAP( handleError, HandleSequenceEventL( aStatus, aEventId, aEventData ) );
    }
  // -------------------------------------------------------
  // We have single request pending.
  else if( IsFlagOn( iBusyFlags, EBusyRequest ) )
    {
    PRINT( _L( "Camera <> Single request pending" ) );
    TRAP( handleError, HandleRequestEventL( aStatus, aEventId, aEventData ) );
    }
  else if( IsFlagOn( iBusyFlags, EBusySetting ) )
    {
    PRINT( _L( "Camera <> Settings finished" ) );
    TRAP( handleError, HandleSettingEventL( aStatus, aEventId, aEventData ) );
    }
  // -------------------------------------------------------
  // No request pending for us.
  // Direct request issued to Camera controller.
  else
    {
    PRINT( _L( "Camera <> No requests pending!!" ) );
    // handle these ..
    }

  // -------------------------------------------------------
  if( KErrNone != handleError )
    {
    EventHandlingErrorRecovery( handleError );
    }

  // Reserve lost event is always handled.
  if( ECamCameraEventReserveLose == aEventId && KErrNone != aStatus )
    {
    SetOperation( ECamStandby, aStatus );
    }
    
  if( iPendingRelease && !Busy() &&
      !IsSavingInProgress() && iInfo.iOperation != ECamCapturing ) 
    {  
    // Finally, if camera release is pending, and we are no longer busy,
    // call ReleaseCamera to get it done.
    // ReleaseCamera is used instead of simply setting iInfo.iTargetMode,
    // because it also handles stopping ongoing burst captures.
    PRINT( _L( "Camera <> CCamAppController::HandleCameraEventL, release pending, calling ReleaseCamera..") );
    ReleaseCamera();  
    }    

#ifdef _DEBUG    
  // Log viewfinder events with PRINT_FRQ
  if( ECamCameraEventVfFrameReady == aEventId )  
    {
    PRINT_FRQ( _L( "Camera <= CCamAppController::HandleCameraEventL" ) );  
    }
  else  
#endif // _DEBUG    
    {
    PRINT( _L( "Camera <= CCamAppController::HandleCameraEventL" ) );
    }
  }

// ===========================================================================


// ---------------------------------------------------------------------------
// EventHandlingErrorRecovery
// ---------------------------------------------------------------------------
//
void
CCamAppController::EventHandlingErrorRecovery( TInt aError )
  {
  PRINT1( _L( "Camera => CCamAppController::EventHandlingErrorRecovery, error:%d" ), aError );

  __ASSERT_ALWAYS( aError != KErrNone, CamPanic( ECamPanicInvalidState ) );

  iCaptureModeTransitionInProgress = EFalse;

  // Reset the saving flag to ensure that camera does not get stuck
  iSaving = EFalse;  

  iMuteNotifications = ETrue;

  // Cancel ongoing sequence if any
  iCameraController->CancelSequence();

  // Clear busy flags
  iBusyFlags           = EBusyNone;
  iInfo.iBusyRequestId = ECamRequestNone;

  iMuteNotifications = EFalse;

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  //Don't change to Stand by mode if -4 error gotten during burst (somehow expected)
  if(SequenceCaptureInProgress() && appUi->CurrentBurstMode() == ECamImageCaptureBurst && KErrNoMemory == aError )
    {
    TRAP_IGNORE( appUi->HandleCameraErrorL(aError) );
    }
  else
    {
    SetOperation( ECamStandby, aError );
    }

  NotifyControllerObservers( ECamEventControllerReady, aError );

  PRINT( _L( "Camera <= CCamAppController::EventHandlingErrorRecovery" ) );
  }

// ---------------------------------------------------------------------------
// HandleSettingEventL
// ---------------------------------------------------------------------------
//
void
CCamAppController::HandleSettingEventL( TInt              aStatus, 
                                        TCamCameraEventId aEventId, 
                                        TAny*             aEventData )
  {
  PRINT( _L( "Camera => CCamAppController::HandleSettingEventL" ) );
  // -------------------------------------------------------
  if( ECamCameraEventSettingsDone == aEventId )
    {
    ClearFlags( iBusyFlags, EBusySetting );

    // Need to clear any pending settings.
    iSettingProvider->Reset();

    if( IsInShutdownMode() && iInfo.iMode != ECamControllerIdle && iInfo.iMode != ECamControllerShutdown )
      {
      PRINT( _L( "Camera <> CCamAppController: Releasing camera.." ) );
      IssueRequestL( ECamRequestRelease );
      }
    else
      {
      PRINT( _L( "Camera <> CCamAppController: Got setting end event.." ) );      
      if( KErrNone != aStatus )
        {
        TCamCameraSettingId* cameraSetting =
            static_cast<TCamCameraSettingId*>( aEventData );

        // Leave with the incoming error, if setting id not provided.
        CheckNonNullL( cameraSetting, aStatus );

        if( ECameraSettingFileName == *cameraSetting )
          {
          DoVideoNameRetryL( aStatus );
          }
        else
          {
          User::Leave( aStatus );
          }
        }
      else
        {
        ProceedPendingOrNotifyReadyL();
        }
      }
    }
  // -------------------------------------------------------
  else if( ECamCameraEventSettingsSingle == aEventId )
    {
    TCamCameraSettingId* cameraSetting =
        static_cast<TCamCameraSettingId*>( aEventData );

    CheckNonNullL( cameraSetting, KErrNotFound );
    switch( *cameraSetting )
      {
      case ECameraSettingDigitalZoom:
      case ECameraSettingOpticalZoom:
        {
        PRINT( _L( "Camera <> CCamAppController: zoom event" ) );
        NotifyControllerObservers( ECamEventZoomStateChanged, aStatus );
        // Pending zooming done in HandleViewfinderFrame
        break;
        }
      default:
        PRINT( _L( "Camera <> CCamAppController: unhandled setting event" ) );
        break;
      }
    }
  // -------------------------------------------------------
  else
    {
    // Ignored
    }
  // -------------------------------------------------------
  PRINT( _L( "Camera <= CCamAppController::HandleSettingEventL" ) );
  }

// ---------------------------------------------------------------------------
// DoVideoNameRetryL
// ---------------------------------------------------------------------------
//
void
CCamAppController::DoVideoNameRetryL( TInt aStatus )
  {
  PRINT( _L( "Camera => CCamAppController::DoVideoNameRetryL" ) );
  if ( IsFlagOn( iCameraController->State(), ECamVideoOn )
    && iVideoNameRetries > 0 )        
    {
    PRINT1( _L( "Camera <> video rename failed retries = %d retrying rename" ), iVideoNameRetries );

    // Try again
    --iVideoNameRetries;
    TRAP_IGNORE( GenerateNextValidVideoPathL() );
    if ( ECamCapturing != iInfo.iOperation && ECamPaused != iInfo.iOperation )
      {
      iSettingProvider->AddPendingSettingChangeL( ECamSettingItemVideoNameBase );    
      }
    IssueSettingsChangeRequestL();
    }
  else if ( KErrDiskFull         == aStatus 
         && ECamMediaStorageCard == IntegerSettingValue( ECamSettingItemVideoMediaStorage ) )
    {
    PRINT( _L( "Camera McaeoVideoPrepareComplete disk full and set to memory card " ) );
    NotifyControllerObservers( ECamEventInvalidMemoryCard );

    // ForceUsePhoneMemory
    if( ExistMassStorage() )
        {
        TRAP_IGNORE( 
            {
            iSettingsModel->SetIntegerSettingValueL( ECamSettingItemVideoMediaStorage, 
            									     ECamMediaStorageMassStorage );
            GenerateNextValidVideoPathL();
            });
        }
    else
        {
        TCamMediaStorage storage = IntegerSettingValue(ECamSettingItemRemovePhoneMemoryUsage)?
                                ECamMediaStorageNone:
                                ECamMediaStoragePhone;
        TRAP_IGNORE( 
           {
           iSettingsModel->SetIntegerSettingValueL( ECamSettingItemVideoMediaStorage, 
                                                       storage );
           GenerateNextValidVideoPathL();
           });       
        
        }
    iSettingProvider->AddPendingSettingChangeL( ECamSettingItemVideoMediaStorage );        
    IssueSettingsChangeRequestL();
    }
  // No retries left
  else 
    {
    PRINT1( _L( "Camera <> video prep failed retries = %d removing iVideoRequested" ), iVideoNameRetries );
    // remove pending request, so that new attempts to record are not blocked
    if ( iVideoRequested )
      iVideoRequested = EFalse;

    User::LeaveIfError( aStatus );
    }
  PRINT( _L( "Camera <= CCamAppController::DoVideoNameRetryL" ) );
  }

// ---------------------------------------------------------------------------
// HandleCameraSequenceComplete
// ---------------------------------------------------------------------------
//
void
CCamAppController::HandleSequenceEventL( TInt              aStatus, 
                                         TCamCameraEventId aEventId, 
                                         TAny*             /*aEventData*/ )
  {
  PRINT( _L( "Camera => CCamAppController::HandleSequenceEventL" ) );

  if( iDismountPending &&
    ( ECamCameraEventImageInit == aEventId ||
      ECamCameraEventVideoInit == aEventId ||
      ECamCameraEventSequenceEnd == aEventId ) )
    {
    iDismountPending = EFalse;
    iDriveChangeNotifier->SendAllowDismount();	
    }
  if( ECamCameraEventSequenceEnd == aEventId )
    {
    ClearFlags( iBusyFlags, EBusySequence );

    if( IsInShutdownMode() && iInfo.iMode != ECamControllerIdle && iInfo.iMode != ECamControllerShutdown )
      {
      PRINT( _L( "Camera <> In shutdown mode!!" ) );
      IssueRequestL( ECamRequestRelease );
      }
    else
      {
      User::LeaveIfError( aStatus );
      if( iCameraController && !iCameraController->IsWaitingIveResources() )
        {
        PRINT( _L( "Camera <> sequence end, proceed with pending.." ) );
        ProceedPendingOrNotifyReadyL();  
        }
      }
    }

  PRINT( _L( "Camera <= CCamAppController::HandleSequenceEventL" ) );
  }

// ---------------------------------------------------------------------------
// HandleRequestEventL
// ---------------------------------------------------------------------------
//
void
CCamAppController::HandleRequestEventL( TInt              aStatus, 
                                        TCamCameraEventId aEventId, 
                                        TAny*             /*aEventData*/ )
  {
  PRINT( _L( "Camera => CCamAppController::HandleRequestEventL" ) );

  // Check if pending operations that need immediate attention.
  if( IsInShutdownMode() && iInfo.iMode != ECamControllerIdle && 
      iInfo.iMode != ECamControllerShutdown && 
      !iSaving && iInfo.iOperation != ECamCapturing && 
      iInfo.iOperation != ECamCompleting ) 
    {
    PRINT( _L( "Camera <> in shutdown mode, release camera.." ) );
    IssueDirectRequestL( ECamRequestRelease );
    }
  else
    {
    if( ECamCameraEventVideoStop == aEventId )
      {
      ClearFlags( iBusyFlags, EBusyRequest );
      HandleVideoStopEvent( aStatus );
      //If postcapture view is set to off notify controller ready event
      //for updating counter.
      if(IntegerSettingValue(ECamSettingItemVideoShowCapturedVideo)!=ECamSettOn )
          {
          NotifyControllerObservers( ECamEventControllerReady, KErrNone );
          }
      }
    else if( ECamCameraEventVideoAsyncStop == aEventId )
      {
      HandleVideoAsyncStopEvent( aStatus );
      }
    else
      {
      // Error handling in one place
      User::LeaveIfError( aStatus );
  
      // If our request is fully processed.
      if( !CameraControllerBusy() )
        {
        PRINT( _L( "Camera <> camera not busy, proceed or notify ready.." ) );
        ClearFlags( iBusyFlags, EBusyRequest );
        ProceedPendingOrNotifyReadyL();
        }
      }
    }
  PRINT( _L( "Camera <= CCamAppController::HandleRequestEventL" ) );
  }


// ---------------------------------------------------------------------------
// IssueRequestL
// ---------------------------------------------------------------------------
//
void
CCamAppController
::IssueRequestL( const TCamCameraRequestId& aId )
  {
  PRINT1( _L("Camera => CCamAppController::IssueRequestL request[%s]"), KCamRequestNames[aId] );

  SetFlags( iBusyFlags, EBusyRequest );
  iInfo.iBusyRequestId = aId;
  
  TRAPD( error, iCameraController->RequestL( iInfo.iBusyRequestId ) );
  if( error != KErrNone )
    {
    ClearFlags( iBusyFlags, EBusyRequest );
    iInfo.iBusyRequestId = ECamRequestNone;
    User::Leave( error );
    }

  PRINT( _L("Camera <= CCamAppController::IssueRequestL") );
  }

// ---------------------------------------------------------------------------
// IssueDirectRequestL
// ---------------------------------------------------------------------------
//
void
CCamAppController
::IssueDirectRequestL( const TCamCameraRequestId& aId )
  {
  PRINT( _L("Camera => CCamAppController::IssueDirectRequestL") );
  iCameraController->DirectRequestL( aId );
  PRINT( _L("Camera <= CCamAppController::IssueDirectRequestL") );
  }


// ---------------------------------------------------------------------------
// IssueSettingsChangeRequestL
// ---------------------------------------------------------------------------
//
void
CCamAppController::IssueSettingsChangeRequestL()
  {
  PRINT( _L("Camera => CCamAppController::IssueSettingsChangeRequestL") );
  SetFlags( iBusyFlags, EBusySetting );

  TRAPD( error, iCameraController->RequestSettingsChangeL() );
  if( KErrNone != error )
    {
    ClearFlags( iBusyFlags, EBusySetting );
    User::Leave( error );
    }
  PRINT( _L("Camera <= CCamAppController::IssueSettingsChangeRequestL") );
  }

// ---------------------------------------------------------------------------
// ProceedPendingOrNotifyReadyL
// ---------------------------------------------------------------------------
//
void 
CCamAppController::ProceedPendingOrNotifyReadyL()
  {
  PRINT( _L("Camera => CCamAppController::ProceedPendingOrNotifyReadyL") );
  PRINT4( _L("Camera <> iMode[%s] iTargetMode[%s] iImageMode[%s] iTargetImageMode[%s]"), 
         KCamModeNames[iInfo.iMode], 
         KCamModeNames[iInfo.iTargetMode], 
         KCamImageModeNames[iInfo.iImageMode], 
         KCamImageModeNames[iInfo.iTargetImageMode] );

  __ASSERT_DEBUG( iCameraController, CamPanic( ECamPanicNullPointer ) );

  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

  if( iInfo.iMode != iInfo.iTargetMode )
    {
    if ( !IsInShutdownMode() && !iSaving && iInfo.iOperation != ECamCapturing && iInfo.iOperation != ECamStandby ) 
        {
        PRINT1( _L("Camera <> CCamAppController: not in target mode, need to issue requests iInfo.iOperation=%d"), iInfo.iOperation );
        IssueModeChangeSequenceL();
        }
    }
  else if( iCameraController->ViewfinderState() != iInfo.iTargetVfState &&
           IsAppUiAvailable () &&
           !appUi->AppInBackground( ETrue ) &&
           !( iConfigManager && iConfigManager->IsUIOrientationOverrideSupported() &&
             iPendingRelease ) )
    {
    PRINT( _L("Camera <> CCamAppController: need to start/stop viewfinder..") );
    IssueModeChangeSequenceL();
    }
  else if( iSettingProvider 
        && iSettingProvider->PendingSettingChangeCount() > 0 )
    {
    PRINT( _L("Camera <> CCamAppController: settings pending, proceed now..") );
    IssueSettingsChangeRequestL();
    }
  else if( iVideoPrepareNeeded )
    {
    PRINT( _L("Camera <> CCamAppController: video prepare pending, proceed now..") );
    iVideoPrepareNeeded = EFalse;
    IssueRequestL( ECamRequestVideoInit );
    }
  else
    {
    PRINT( _L("Camera <> CCamAppController: Target mode achieved") );
    if( iCaptureModeTransitionInProgress )
      {
      PRINT( _L("Camera <> CCamAppController: capture mode transition complete") );
      iCaptureModeTransitionInProgress = EFalse;
      SetImageMode( iInfo.iTargetImageMode );
      }

    if ( iSetNewPathnamesPending > ECamMediaStorageNone )
      {
      TRAP_IGNORE( SetPathnamesToNewStorageL( iSetNewPathnamesPending ) );
      }

    if( iVideoRequested )
      {
      PRINT( _L("Camera <> starting recording..") );
      iVideoRequested = EFalse;
      IssueRequestL( ECamRequestVideoStart );
      PRINT( _L("Camera <> ..done") );
      }


    NotifyControllerObservers( ECamEventEngineStateChanged, KErrNone ); 
    NotifyControllerObservers( ECamEventControllerReady,    KErrNone ); 
    }
  PRINT( _L("Camera <= CCamAppController::ProceedPendingOrNotifyReadyL") );
  }

// ---------------------------------------------------------------------------
// SetStateFromEvent
// ---------------------------------------------------------------------------
//
void
CCamAppController::SetStateFromEvent( TCamCameraEventId aEventId )
  {   
  PRINT3( _L("Camera => CCamAppController::SetStateFromEvent mode[%s] operation[%s] event[%s]"), 
          KCamModeNames[iInfo.iMode], 
          KCamCaptureOperationNames[iInfo.iOperation],
          KCamCameraEventNames[aEventId]
        );
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() ); 
  switch( aEventId )
    {
    // =================================
    case ECamCameraEventReserveLose:
      // SetMode( ECamControllerShutdown );
      SetMode( ECamControllerIdle );
      SetOperation( ECamNoOperation );
      // we dont need location trail anymore.
      StopLocationTrail();
      break;
    // ---------------------------------
    case ECamCameraEventPowerOn:
      PRINT( _L("Camera <> Init key sound system..") );
      iSoundPlayer->InitKeySoundSystem();
      PRINT( _L("Camera <> ..Init key sound system done") );      
      break;
    // ---------------------------------
    case ECamCameraEventPowerOff:
      // Reset the flash error status
      if( iFlashStatus ) iFlashStatus->SetFlashError( EFalse );
      SetMode( ECamControllerIdle );
      SetOperation( ECamNoOperation );
      break;
    // =================================
    // VF events
    case ECamCameraEventVfStart:
      if(  iBacklightTimer 
       && !iBacklightTimer->IsActive() )
        {
        //iBacklightTimer->Start( KBacklighTimerInterval,
        //                        KBacklighTimerInterval, 
        //                        TCallBack( ResetInactivityTimer, this ) );        
        iBacklightTimer->Start( 0,
                                KBacklighTimerInterval, 
                                TCallBack( ResetInactivityTimer, this ) );
        }
		iAFCancelInProgress = EFalse;
      break;
    // ---------------------------------
    case ECamCameraEventVfStop:
      if( iBacklightTimer )
        {
        iBacklightTimer->Cancel();
        }
      break;
    // =================================
    case ECamCameraEventImageInit:
      PRINT ( _L("Camera <> CCamAppController::SetStateFromEvent .. case ECamCameraEventImageInit") );      
      SetMode( ECamControllerImage );

      // Set the capture count now, so we don't need to set it when starting capture.
      TRAP_IGNORE( InitCaptureLimitL() );
      SetOperation( ECamNoOperation );
      NotifyControllerObservers( ECamEventImageQualityChanged );

      break;
    // ---------------------------------
    case ECamCameraEventImageStart: // Capture started, not finished.
      PRINT ( _L("Camera <> CCamAppController::SetStateFromEvent .. case ECamCameraEventImageStart") );      
      SetOperation( ECamCapturing );

      // Make sure our cached value for image filename number is up to date.      
      iImageNumberCache = IntegerSettingValue( ECamSettingItemPhotoNumber );
      break;
    // ---------------------------------
    case ECamCameraEventImageStop: // Capture finished, unless in burst    
      PRINT ( _L("Camera <> CCamAppController::SetStateFromEvent .. case ECamCameraEventImageStop") );      
      // Store image counter now.
      TRAP_IGNORE( SetIntegerSettingValueL( ECamSettingItemPhotoNumber, iImageNumberCache ) );

      if ( ECamImageCaptureBurst != iInfo.iImageMode )
        {
        
        //If canceled autofocusing previously then have to set
        //autofocus range to get autofocusing work again.

        if(iAFCancelInProgress )
        {
           iAFCancelInProgress = EFalse; 
           TRAP_IGNORE( iCameraController->DirectRequestL( ECamRequestSetAfRange ) );                                                                  

        }

        
// No operation change here.
// Set to Completing in SS event, and ECamNoOperation when image saved.
//        SetOperation( ECamNoOperation );
        }
      break;
    // ---------------------------------
    case ECamCameraEventImageRelease:
      SetMode( ECamControllerIdle );
      SetOperation( ECamNoOperation );
      break;
    // =================================
    case ECamCameraEventVideoInit:
      SetMode( ECamControllerVideo );
      SetOperation( ECamNoOperation );
      break;
    // ---------------------------------
    case ECamCameraEventVideoStart:
      iVideoRequested = EFalse;
      SetOperation( ECamCapturing );
      break;
    // ---------------------------------
     case ECamCameraEventVideoPause:
       PlaySound( ECamVideoPauseSoundId, EFalse );
       SetOperation( ECamPaused );
       break;            
    // ---------------------------------
    case ECamCameraEventVideoStop:
      SetOperation( ECamCompleting );
      break;
    // ---------------------------------
    case ECamCameraEventVideoRelease:
      SetMode( ECamControllerIdle );
      SetOperation( ECamNoOperation );
      break;
    // =================================        
    case ECamCameraEventStartAutofocus:

      if ( !CurrentSceneHasForcedFocus() &&
           !iAFCancelInProgress )
        {       
        PRINT( _L("ECamCameraEventStartAutofocus -> Set operation state to ECamFocusing") );
        iCurrentAFRequest=ECamRequestStartAutofocus;
        SetOperation( ECamFocusing );
        // fixed toolbar is used only with touch devices
        if ( IsTouchScreenSupported() )
            {
            CAknToolbar* fixedToolbar = appUi->CurrentFixedToolbar();
            if ( fixedToolbar )
                {
                CAknToolbarExtension* extension = fixedToolbar->ToolbarExtension();
                if ( extension )
                    {
                    // Keep toolbar extension hidden after the half press key is released 
                    extension->SetShown( EFalse );
                    }
                fixedToolbar->SetToolbarVisibility( EFalse );
                } 
            }
        }    
      break;       
    // ---------------------------------
    case ECamCameraEventAutofocusSuccessful:
    case ECamCameraEventAutofocusFailed:
        {
        OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPCONTROLLER_SETSTATEFROMEVENT, "e_CAM_APP_AF_LOCK 0" ); //CCORAPP_AF_LOCK_END
        OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMAPPCONTROLLER_SETSTATEFROMEVENT, "e_CAM_APP_OVERLAY_UPD 1" );    //CCORAPP_OVERLAY_UPD_START
        PRINT2( _L("Camera <> CCamAppController::SetStateFromEvent - iPendingAFRequest[%s] iCurrentAFRequest[%s]"), 
                KCamRequestNames[iPendingAFRequest], 
                KCamRequestNames[iCurrentAFRequest] );
   	    
   	    if( iAFCancelInProgress && ECamRequestCancelAutofocus == iCurrentAFRequest &&
   	         ECamRequestCancelAutofocus == iPendingAFRequest )
   		    {
            // Cancelling done, camera lens is in hyperfocal position.
            // Return AF range back to normal, so that next autofocus request
            // will use the right range for this scene.
            iAFCancelInProgress = EFalse;  
            TRAP_IGNORE( iCameraController->DirectRequestL( ECamRequestSetAfRange ) );	    
   		    iPendingAFRequest=0;
   		    iCurrentAFRequest=0;
   		    NotifyControllerObservers( ECamEventFocusCancelled );
   		    
            if ( !iCaptureRequested )
                {
                SetOperation( ECamNoOperation );
                }
            else
                {
                // do not notify observers if this happens after capture
                // has been requested as that would result in invalid ui state
                iInfo.iOperation = ECamNoOperation;
                if( IsAfNeeded() )
                    {
                    SetAfNeeded( EFalse );
                    StartAutoFocus();
                    }
                }
   		    }
   		 else if( iCurrentAFRequest==iPendingAFRequest &&
   		        iPendingAFRequest==ECamRequestStartAutofocus
                && !CurrentSceneHasForcedFocus()
             )
             {
	         if( aEventId==ECamCameraEventAutofocusSuccessful )
	            {

                PRINT( _L("ECamCameraEventAutofocusSuccessful -> Set operation state to ECamFocused") );
          
        
                if( iConfigManager && iConfigManager->IsAutoFocusSupported() &&
                    !iCaptureRequested && appUi &&	appUi->SelfTimer() && 
                    !appUi->SelfTimer()->IsActive())
                    {
                    if ( !iSilentProfile || iShutterSndAlwaysOn  )
                        {
                        // Play only if camera tones are not set off
                        PlaySound(ECamAutoFocusComplete, EFalse);
                        }
                    }
                SetOperation( ECamFocused );
	            }    
	        else if(  aEventId==ECamCameraEventAutofocusFailed )
	    	    {
                PRINT( _L("ECamCameraEventAutofocusFailed -> Set operation state to ECamFocusFailed") );
                if ( iConfigManager && iConfigManager->IsAutoFocusSupported() )
                    {
                    PlaySound( ECamAutoFocusFailed, EFalse );
                    }
                SetOperation( ECamFocusFailed );
                }
            iPendingAFRequest=0;
            iCurrentAFRequest=0;	        
            }
        else if( iCurrentAFRequest != iPendingAFRequest && iPendingAFRequest !=0 
                 && !CurrentSceneHasForcedFocus()
            )
            {
            iCurrentAFRequest=iPendingAFRequest;
            iPendingAFRequest=0;
            TryAFRequest( iCurrentAFRequest ); 
            }
        else if( CurrentSceneHasForcedFocus() )
            {
            // Autofocus flags has to be reset in forced focus cases
            iPendingAFRequest=0;
            iCurrentAFRequest=0;
            }
        }
      break;
    // ---------------------------------      
    case ECamCameraEventCancelAutofocus:
        {  
        // Set iAFCancelInProgress to true so that we don't change the 
        // reticule for the next focus successful/focus failed event.
        iAFCancelInProgress = ETrue;
        iCurrentAFRequest = ECamRequestCancelAutofocus;
        // fixed toolbar is used only with touch devices
        if ( !iCaptureRequested && IsTouchScreenSupported() )
            {
            appUi->SetToolbarVisibility();  // avoid flickering in settings view
            }
        }
      break;             
    // =================================        
    default:
      // No change to state with other events
      break;
    // =================================        
    }
  PRINT2( _L("Camera <= CCamAppController::SetStateFromEvent mode[%s] operation[%s]"), 
          KCamModeNames[iInfo.iMode], 
          KCamCaptureOperationNames[iInfo.iOperation] );
  }

void 
CCamAppController::InitCaptureLimitL()
  {
  // Set the capture count now, so we don't need to set it when starting capture.
  switch( iInfo.iTargetImageMode )
    {
    case ECamImageCaptureBurst:
      PRINT1( _L("Camera <> CCamAppController .. burst mode as target, set capture limit to %d"), KShortBurstCount );      
      SetCaptureLimitL( KShortBurstCount );
      break;
    case ECamImageCaptureTimeLapse:
      PRINT1( _L("Camera <> CCamAppController .. timelapse mode as target, set capture limit locally to max %d"), iLongSequenceLimit );      
      SetCaptureLimitL( iLongSequenceLimit );
      break;
    default:
      PRINT( _L("Camera <> CCamAppController .. single mode as target, set capture limit locally to 1") );      
      SetCaptureLimitL( 1 );
      break;
    }
  }


// ===========================================================================
// 
TCamAppControllerInfo::TCamAppControllerInfo()
  :  iActiveCamera         ( ECamActiveCameraNone ),
     iMode                 ( ECamControllerIdle   ),
     iTargetMode           ( ECamControllerIdle   ),
     iImageMode            ( ECamImageCaptureNone ),
     iTargetImageMode      ( ECamImageCaptureNone ),     
     iOperation            ( ECamNoOperation      ),
     iTargetVfState        ( ECamTriIdle          ),
     iViewfinderMirror     ( EFalse               ),
     iTargetImageResolution( 0                    ),
     iTargetImageQuality   ( 0                    ),
     iBusyRequestId        ( ECamRequestNone      )
  {
  }

 
// ===========================================================================


//
//
//
void CCamAppController::HandleFileHarvestingComplete()
    {
    PRINT( _L("Camera => CCamAppController::HandleFileHarvestingComplete" ) );
    // If release required but not done in postcapture not shown mode this is the
	// last time to do it. Otherwise problems occurs in 2ndary camera if opened
    if ( iPendingRelease && ECamSettOff == IntegerSettingValue( ECamSettingItemShowCapturedPhoto ) )
       {
       PRINT( _L("Camera <> CCamAppController::HandleFileHarvestingComplete, releasing" ) );	
       ReleaseCamera();	
       }
    PRINT( _L("Camera <= CCamAppController::HandleFileHarvestingComplete" ) );
    }


//
//CCamAppController::DeRegisterHarverterClientEvents
//
void CCamAppController::DeRegisterHarverterClientEvents()
    {
    if ( iImageSaveActive )
        {
#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
        iImageSaveActive->DeRegisterHarverterClientEvents();
#endif //defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
        }
    }


// ---------------------------------------------------------------------------
// CCamAppController::Configuration
//
// ---------------------------------------------------------------------------
//
CCamConfiguration* 
CCamAppController::Configuration() const
    {
    return iConfiguration;  
    }


// ---------------------------------------------------------------------------
// CCamAppController::RestoreSettingsToCameraL
// Calls RestoreSettingIfNecessaryL for all supported camera settings.
// In case there were settings needing restoring, and camera controller
// is not busy, request camera controller to start setting changes.
// ---------------------------------------------------------------------------
// 
void
CCamAppController::RestoreSettingsToCameraL()
    {
    PRINT( _L("Camera => CCamAppController::RestoreSettingsToCameraL" ) );  
    const TCamCameraMode mode = CurrentMode(); 
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( IsAppUiAvailable() && appUi->CurrentViewState()==ECamViewStateUserSceneSetup )
        {
        //restore user scence setting
        DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneWhitebalance );
        DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneExposure );
        DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneFlash ); 
        if ( UiConfigManagerPtr()->IsColorToneFeatureSupported() )
          DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneColourFilter );        
        if ( UiConfigManagerPtr()->IsBrightnessSupported() )
          DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneBrightness ); 
        if ( UiConfigManagerPtr()->IsContrastSupported() )
           DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneContrast );   
        if ( UiConfigManagerPtr()->IsISOSupported() || UiConfigManagerPtr()->IsExtendedLightSensitivitySupported() )	
           DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneLightSensitivity );
        if ( ECamControllerImage == mode )
          {
          if ( UiConfigManagerPtr()->IsSharpnessFeatureSupported() )
            DirectRestoreSettingIfNecessaryL( ECamSettingItemUserSceneImageSharpness );
          DirectRestoreSettingIfNecessaryL( ECamSettingItemPhotoDigitalZoom );
          }
        else if ( ECamControllerVideo == mode )
          {
          DirectRestoreSettingIfNecessaryL( ECamSettingItemVideoDigitalZoom );
          DirectRestoreSettingIfNecessaryL( ECamSettingItemVideoStab);
          }
        }
    else if(iInfo.iActiveCamera == ECamActiveCameraPrimary)
	    {
        if ( ECamControllerImage == mode )
	        {
	        // Add rest of the pending settings where needed
	        RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoWhiteBalance );
	        RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoExposure );

	        if ( UiConfigManagerPtr()->IsColorToneFeatureSupported() )
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoColourFilter );        
	        
	        if ( UiConfigManagerPtr()->IsBrightnessSupported() )
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoBrightness );

	        if ( UiConfigManagerPtr()->IsContrastSupported() )
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoContrast );

	        if ( UiConfigManagerPtr()->IsSharpnessFeatureSupported() )
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoImageSharpness );
	      
	        if ( UiConfigManagerPtr()->IsISOSupported() || UiConfigManagerPtr()->IsExtendedLightSensitivitySupported() )
	            {
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoLightSensitivity );
	            }
	        
	        RestoreSettingIfNecessaryL( ECamSettingItemDynamicPhotoFlash );
	        
	        RestoreSettingIfNecessaryL( ECamSettingItemPhotoDigitalZoom );
	        }    
	    else if ( ECamControllerVideo == mode )
	        {
	        // Video settings in the toolbar
	        RestoreSettingIfNecessaryL( ECamSettingItemDynamicVideoWhiteBalance );
	        RestoreSettingIfNecessaryL( ECamSettingItemDynamicVideoExposure );

	        if ( UiConfigManagerPtr()->IsColorToneFeatureSupported() )
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicVideoColourFilter );
	        
	        // The following settings are not supported in video mode toolbar
	        // However, the values may need to be reset, in case the setting was
	        // changed in image mode
	        if ( UiConfigManagerPtr()->IsBrightnessSupported() )
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicVideoBrightness );

	        if ( UiConfigManagerPtr()->IsContrastSupported() )
	            RestoreSettingIfNecessaryL( ECamSettingItemDynamicVideoContrast );

	        if ( UiConfigManagerPtr()->IsVideoStabilizationSupported() )
	            {
	            RestoreSettingIfNecessaryL( ECamSettingItemVideoStab );      
	            }
	        RestoreSettingIfNecessaryL( ECamSettingItemVideoDigitalZoom );
	        
	        RestoreSettingIfNecessaryL( ECamSettingItemDynamicVideoFlash );
	        
	        RestoreSettingIfNecessaryL( ECamSettingItemContinuousAutofocus ); 
	        }
	    }
    else if(iInfo.iActiveCamera == ECamActiveCameraSecondary)
        {
        if ( ECamControllerImage == mode )
                {
                // Not needed
                }    
            else if ( ECamControllerVideo == mode )
                {
                // Not needed
                }
        }
    else
        {
        // Do nothing
        }
    
    if( iSettingProvider->PendingSettingChangeCount() > 0 &&
        !iCameraController->ControllerInfo().iBusy )
        {
        PRINT( _L("Camera <> CCamAppController::RestoreSettingsToCameraL - Issue settings change request") );
        IssueSettingsChangeRequestL( );    
        }
        
    PRINT( _L("Camera <= CCamAppController::RestoreSettingsToCameraL" ) );          
    }
 
// ---------------------------------------------------------------------------
// CCamAppController::RestoreSettingIfNecessaryL
// Checks from camera controller, whether the value of the setting given
// as argument has same value in camera hw and settings model (UI).
// If value is not the same, add the setting id to settingsmodel's list
// of pending changes.
// ---------------------------------------------------------------------------
//     
void
CCamAppController::RestoreSettingIfNecessaryL( TCamSettingItemIds aSettingItem )
    {    
    // Convert UI setting ID to camera controller setting ID  
    TCamCameraSettingId camSettingId = 
      CCamSettingConversion::Map2CameraControllerSettingId( aSettingItem );
    
    if( !iCameraController->SettingValueUpToDateL( camSettingId ) )
        {
        PRINT1( _L("RestoreSettingIfNecessaryL - Restoring value for setting %s"), KCamSettingItemNames[aSettingItem] );   
        iSettingProvider->AddPendingSettingChangeL( aSettingItem );  
        }
    else
        {
        if( aSettingItem == ECamSettingItemDynamicVideoFlash )
            {
            TCamCameraSettingId cameraId( 
                CCamSettingConversion::Map2CameraControllerSettingId( aSettingItem ) );
            iCameraController->DirectSettingsChangeL( cameraId );
            }
        PRINT1( _L("RestoreSettingIfNecessaryL - Setting %s has up-to-date value, no need to restore"), KCamSettingItemNames[aSettingItem] );      
        } 
    }            


// ---------------------------------------------------------------------------
// CCamAppController::DirectRestoreSettingIfNecessaryL
// Checks if the setting needs restoring (UI setting value doesn't match
// the actual setting value in camera).  In that case, function 
// directly set settting to camera
// ---------------------------------------------------------------------------
// 
void
CCamAppController::DirectRestoreSettingIfNecessaryL( TCamSettingItemIds aSettingItem )
    {

    TCamCameraSettingId cameraId( 
                CCamSettingConversion::Map2CameraControllerSettingId( aSettingItem ) );
    if ( !iCameraController->SettingValueUpToDateL( cameraId ) )
      {
      PRINT1( _L("RestoreSettingIfNecessaryL - Restoring value for setting %s"), KCamSettingItemNames[aSettingItem] );   
      iCameraController->DirectSettingsChangeL( cameraId );  
      }
    }
// -----------------------------------------------------------------------------
// CCamAppController::RestartTimeLapseTimerL()
// 
// -----------------------------------------------------------------------------
//   
void CCamAppController::RestartTimeLapseTimerL()
	{
	PRINT( _L("Camera => CCamAppController::RestartTimeLapseTimerL inside this function" ))  
   	if ( iTimeLapseTimer )
		{
		iTimeLapseTimer->Cancel();		
		iTimeLapseTimer->SetTimeout( KSecondInMicSec );
		}
    else
        {
        iTimeLapseTimer = CCamTimer::NewL( KSecondInMicSec, TCallBack( CCamAppController::TimeLapseTimeoutL, this ) );
        }

    NotifyControllerObservers( ECamEventCounterUpdated );
    PRINT( _L("Camera <> CCamAppController::RestartTimeLapseTimerL starting timelapse timer" ));
    
    iTimeLapseStartTime.HomeTime(); 
    iTimeLapseTimer->StartTimer();
	
	// Following trace may be useful to see timing related handling in TimeLapse mode 
	// TDateTime stTime = iTimeLapseStartTime.DateTime();
	// PRINT3( _L("Start time: %d:%d:%d"), stTime.Minute(), stTime.Second(), stTime.MicroSecond() );
	
	PRINT( _L("Camera <= CCamAppController::RestartTimeLapseTimerL" ))  
	}
 
     
// ---------------------------------------------------------------------------
// SetSettingsRestoreNeeded
// ---------------------------------------------------------------------------
//     
void  
CCamAppController::SetSettingsRestoreNeeded()
  {
  iSettingsRestoreNeeded = ETrue;
  }

// ---------------------------------------------------------------------------
// IsSettingsRestoreNeeded
// ---------------------------------------------------------------------------
//     
TBool 
CCamAppController::IsSettingsRestoreNeeded() const
  {
  return iSettingsRestoreNeeded;
  }    

// ---------------------------------------------------------------------------
// StaticSettingsModel
//
// Gets the handle to the settings model, which is used inturn by the
// plugin to call the interfaces for performing needed actions.
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::IsSceneSupported( const TInt aSceneId, TBool aPhotoScene ) const
  {
  if ( aPhotoScene )
      return static_cast<CCamSettingsModel*>(iSettingsModel)->IsImageSceneSupported( aSceneId );
  else
      return static_cast<CCamSettingsModel*>(iSettingsModel)->IsVideoSceneSupported( aSceneId );
  }

// ---------------------------------------------------------------------------
// StaticSettingsModel
//
// Gets the handle to the settings model, which is used inturn by the
// plugin to call the interfaces for performing needed actions.
// ---------------------------------------------------------------------------
//
MCamStaticSettings& 
CCamAppController::StaticSettingsModel()
  {
  return static_cast<CCamSettingsModel*>(iSettingsModel)->StaticSettingsModel();
  }


// ---------------------------------------------------------------------------
// EngineRequestsPending
// ---------------------------------------------------------------------------
//     
TBool CCamAppController::EngineRequestsPending() const
   {
   return IsFlagOn( iBusyFlags, EBusyRequest );
   }  
 
// ---------------------------------------------------------------------------
// CalculateVideoTimeRemainingL
//
// ---------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds
CCamAppController::CalculateVideoTimeRemainingL( TCamMediaStorage aStorage )
  {
  RFs& fs = CEikonEnv::Static()->FsSession();
  TInt drive = 0;
  // Check if setting the new video path is pending. The path may not have been 
  // set if camera controller was busy or video mode not initialized. This 
  // happens e.g. while in Settings menu.
  // If yes, use the storage media that is going to be used next.
  if( iSetNewPathnamesPending != ECamMediaStorageNone )
    {
    aStorage = iSetNewPathnamesPending;
    }
  if(aStorage == ECamMediaStorageCurrent)
	{
  const TDesC& filename = CurrentVideoFileName();
  TParse fp;
  User::LeaveIfError(fs.Parse(filename, fp));
  TPtrC driveletter = fp.Drive();
  TChar drl = driveletter[0];
  User::LeaveIfError(fs.CharToDrive(drl, drive));  
	}
  else
	{
	DriveInfo::TDefaultDrives driveInfo = static_cast<DriveInfo::TDefaultDrives>(KErrNotFound);
	switch(aStorage)
		{
		case ECamMediaStoragePhone:
			driveInfo = DriveInfo::EDefaultPhoneMemory;
			break; 
		case ECamMediaStorageCard:
			driveInfo = DriveInfo::EDefaultRemovableMassStorage;
			break;
		case ECamMediaStorageMassStorage:
			driveInfo = DriveInfo::EDefaultMassStorage;
			break;
		default:
			break;
		}
	User::LeaveIfError(DriveInfo::GetDefaultDrive( driveInfo, drive ));
	}
    
  // Get critical level for this drive type
  TDriveInfo driveInfo;
  fs.Drive(driveInfo, drive);
    
  TInt criticalDiskVal = 0;
  if( driveInfo.iType == EMediaRam ) //RAM drives have diff critical levels
	{
	CRepository* repository = CRepository::NewLC( KCRUidDiskLevel );
	User::LeaveIfError( 
		repository->Get( KRamDiskCriticalLevel, criticalDiskVal ) );
	CleanupStack::PopAndDestroy( repository );			
	}
  else // Some other media type
	{
	CRepository* repository = CRepository::NewLC( KCRUidDiskLevel );
	User::LeaveIfError( 
		repository->Get( KDiskCriticalThreshold, criticalDiskVal ) );
	CleanupStack::PopAndDestroy( repository );			
	}   
 
  // Get the available space in volumeinfo.iFree
  TVolumeInfo volumeinfo; 
  User::LeaveIfError(fs.Volume(volumeinfo, drive));   
   
  // Get the current video quality data
  TInt videoQuality = iSettingsModel->IntegerSettingValue( 
  										ECamSettingItemVideoQuality );
  CCamVideoQualityLevel& level = *( iSettingsModel->
  									VideoQualityArray() )[videoQuality];     
  
  // Audio recording on/muted
  TInt audioRec = iSettingsModel->IntegerSettingValue( 
  									ECamSettingItemVideoAudioRec );
  TBool audioMute = ECamSettOff == audioRec;
  
  TInt64 videoLimit =  KMaxTUint32; // 4G limit
  // Video file size limit (MMS case)
  if( ECamVideoClipShort == level.VideoLength() )
    {
    // Short video, needs to fit into MMS message
    videoLimit = CamUtility::MaxMmsSizeInBytesL();  
    }
  
  TCamVideoTime videoTime( volumeinfo.iFree,            // Free space
                           criticalDiskVal,             // Critical space
                           //level.VideoFileType(),       // Video codec
                           level.VideoBitRate(),        // Video bitrate
                           //level.AudioType(),           // Audio FourCC
                           level.AudioBitRate(),        // Audio bitrate
                           videoLimit,                  // File size limit
                           audioMute,                   // Mute audio?
                           iConfiguration->CMRAvgVideoBitRateScaler() );
                                                
  return videoTime.GetRemainingTimeL();    
  }


// ---------------------------------------------------------------------------
// SetRemainingImageStored
// ---------------------------------------------------------------------------
//     
void  
CCamAppController::SetRemainingImageStored()
  {
  iValueIsStored = ETrue;
  }
// ---------------------------------------------------------------------------
// IsRemainingImageStored
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::IsRemainingImageStored() const
  {
  return iValueIsStored;
  }

// ---------------------------------------------------------------------------
// IsSavingInProgress
// ---------------------------------------------------------------------------
//
TBool 
CCamAppController::IsSavingInProgress() const
  {
  return ( iImageSaveActive->Count() > 0 );
  }

// ---------------------------------------------------------------------------
// CapturedImages
// ---------------------------------------------------------------------------
//
 TInt CCamAppController::CapturedImages() const
  {
  return iCameraController->ControllerInfo().iCaptureCount;
  }
        
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCamAppController::SetSettingsPlugin( CCamGSInterface* aPlugin )
	{
	iPlugin = aPlugin;
	}

// ---------------------------------------------------------------------------
// CCamAppController::SetViewfinderWindowHandle
// ---------------------------------------------------------------------------
//
void CCamAppController::SetViewfinderWindowHandle( RWindowBase* aWindow )
    {
    __ASSERT_ALWAYS( iCameraController, CamPanic( ECamPanicInvalidState ) );

    iCameraController->SetViewfinderWindowHandle( aWindow );
    }

// ---------------------------------------------------------------------------
// CCamAppController::ViewfinderWindowDeleted
// ---------------------------------------------------------------------------
//
void CCamAppController::ViewfinderWindowDeleted( RWindowBase* aWindow )
    {
    __ASSERT_ALWAYS( iCameraController, CamPanic( ECamPanicInvalidState ) );

    iCameraController->ViewfinderWindowDeleted( aWindow );
    }


// ---------------------------------------------------------------------------
// CCamAppController::ExistMassStorage()
// ---------------------------------------------------------------------------
//
TBool CCamAppController::ExistMassStorage() const
    {
    const TUint KMassStorageBits = DriveInfo::EDriveInternal |
                                   DriveInfo::EDriveExternallyMountable;    
    TInt drive;
    TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive );
    
    if ( err != KErrNone )
        {
        return EFalse;
        }
    
    TUint driveStatus(0);    
    err = DriveInfo::GetDriveStatus( CCoeEnv::Static()->FsSession(), 
                                     drive, 
                                     driveStatus );

    TInt usbPersonality;
    TInt ret = RProperty::Get(KPSUidUsbWatcher, 
    						  KUsbWatcherSelectedPersonality,
    						  usbPersonality);
    
    if ( err != KErrNone )
        {
        return EFalse;
        }
        
    if ( (driveStatus & KMassStorageBits) == KMassStorageBits &&
		 !(driveStatus & DriveInfo::EDriveCorrupt) &&
    	(  KErrNotFound == ret || // USB watcher is not loaded
    	        ( (KErrNone == ret) && (KUsbPersonalityIdMS != usbPersonality) ) ) )
        {
        return ETrue;
        }
        
    return EFalse;
    }


// ------------------------------------------------------------------------
// CCamAppController::IsMemoryAvailable()
// ------------------------------------------------------------------------
//
TBool 
CCamAppController::IsMemoryAvailable(const TCamMediaStorage aStorage, TBool aIgnoreUsbPersonality ) const
	{
	TInt currentStorage = aStorage;
	if(aStorage == ECamMediaStorageCurrent)
		{
		TCamCameraMode mode = CurrentMode();

		if((ECamControllerImage != mode) && (ECamControllerVideo != mode))
            {
            //This is needed for case where current mode is not yet set.
            mode = TargetMode();
            }
		
		if(ECamControllerImage == mode)
			{
			currentStorage = static_cast<TCamMediaStorage>( 
								IntegerSettingValue( 
									ECamSettingItemPhotoMediaStorage ) );
			}
		else if(ECamControllerVideo == mode)
			{
			currentStorage = static_cast<TCamMediaStorage>( 
								IntegerSettingValue( 
									ECamSettingItemVideoMediaStorage ) );
			}
		else
			{
			//No impl
			}
		}
	
	TInt usbPersonality;
	TInt ret = RProperty::Get(KPSUidUsbWatcher, 
							  KUsbWatcherSelectedPersonality,
							  usbPersonality);
  if ( !aIgnoreUsbPersonality )
    {
    if ( ( currentStorage != ECamMediaStoragePhone ) && ( KErrNone == ret ) && 
         ( KUsbPersonalityIdMS == usbPersonality ) )
      {
      return EFalse;
      }
    }
		
	DriveInfo::TDefaultDrives driveInfo = static_cast<DriveInfo::TDefaultDrives>(KErrNotFound);
	switch(currentStorage)
		{
		case ECamMediaStoragePhone:
			driveInfo = DriveInfo::EDefaultPhoneMemory;
			break; 
		case ECamMediaStorageCard:
			driveInfo = DriveInfo::EDefaultRemovableMassStorage;
			break;
		case ECamMediaStorageMassStorage:
			driveInfo = DriveInfo::EDefaultMassStorage;
			break;
		default:
			break;
		}
	
	
	TInt drive;
	TInt err = DriveInfo::GetDefaultDrive( driveInfo, drive );
	if(err)
		{
		return EFalse;
		}
    TUint driveStatus(0);    
    err = DriveInfo::GetDriveStatus( CCoeEnv::Static()->FsSession(), 
                                     drive, 
                                     driveStatus );
    if(err)
    	{
    	return EFalse;
    	}
    
    return ( (driveStatus & DriveInfo::EDrivePresent) && 
		   !(driveStatus & DriveInfo::EDriveCorrupt) &&
	       (driveStatus & DriveInfo::EDriveUserVisible) &&
	       !(driveStatus & DriveInfo::EDriveInUse) &&
	       !(driveStatus & DriveInfo::EDriveReadOnly) );
	}
 
	
// ---------------------------------------------------------------------------
// CCamAppController::IsDemandKeyRelease()
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsDemandKeyRelease()
    {
    return iDemandKeyRelease;
    }

// ---------------------------------------------------------------------------
// CCamAppController::SetDemandKeyRelease()
// ---------------------------------------------------------------------------
//
void CCamAppController::SetDemandKeyRelease( TBool aDemand )
    {
    iDemandKeyRelease = aDemand;
    }


// ---------------------------------------------------------------------------
// CCamAppController::IsKeyLockOn()
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsKeyLockOn()
    {
    if ( iKeyLockStatusWatcher && iConfigManager && iConfigManager->IsKeyLockWatcherSupported() )
        {
        TInt iKeyLockStatus = 0;
        iKeyLockStatusWatcher->Get( iKeyLockStatus );
        if ( iKeyLockStatus == EKeyguardLocked )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }        
        }
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CCamAppController::DriveChangeL
// ---------------------------------------------------------------------------
//
TInt CCamAppController::DriveChangeL( const TCamDriveChangeType aType )
  {
  PRINT1( _L("Camera => CCamAppController::DriveChangeL aType: %d" ), aType );
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  appUi->CheckMemoryAvailableForCapturing();
  
    
  TCamCameraMode mode = CurrentMode();
  TCamMediaStorage currentStorage;
		
  // Photos and videos are saved onto the same media, so no need to 
  // differentiate between still/video 
  currentStorage = static_cast<TCamMediaStorage>( 
          IntegerSettingValueUnfiltered( ECamSettingItemPhotoMediaStorage ) );
    
  PRINT1( _L("Camera <> iPreferredStorageLocation is now %d"), 
                                                  iPreferredStorageLocation );
  PRINT1( _L("Camera <> Update iPreferredStorageLocation to %d"), 
                                                            currentStorage );
    
  iPreferredStorageLocation = currentStorage; 
    
  if ( ECamMediaStoragePhone == iPreferredStorageLocation )
    {
    PRINT( _L("Camera <> Phone memory is the preferred storage location. Nothing to be done here. Return KErrNone.") )
    PRINT( _L("Camera <= CCamAppController::DriveChangeL" ) );
    return KErrNone;
    }
   
  if  ( ( !IsMemoryAvailable( ECamMediaStorageCard, EFalse ) || 
          !IsMemoryAvailable( ECamMediaStorageMassStorage, EFalse ) ) ||
        !IsMemoryAvailable( iPreferredStorageLocation, ETrue ) )
    {  
    PRINT( _L("Camera <> USB personality is 'Mass storage' or iPreferredStorageLocation (MMC or mass memory) is unavailable") )

    // Stop if recording a video to mass memory or MMC
    if ( CurrentVideoOperation() == ECamCapturing ||
         CurrentVideoOperation() == ECamPaused )
      {
      PRINT( _L("Camera <> Video capture in progress...") )
        
      if ( !IsMemoryAvailable( iInitialVideoStorageLocation ) ) 
        {
        PRINT( _L("Camera <> ...Stop  video recording.") )
        iCaptureStoppedForUsb = ETrue;	
        StopVideoRecording();
        }
      else
        {
        PRINT( _L("Camera <> ...Phone memory in use. No need to stop.") )
        }
                       
      }
    // Cancel if pending recording	
    else if( iVideoRequested )
      {
      iVideoRequested = EFalse;
      }
    
    if ( appUi->SelfTimerEnabled() && appUi->AllMemoriesFullOrUnavailable() )
      {
      PRINT( _L("Camera <> All storages full or unavailable. Cancel selftimer.") )
      appUi->HandleCommandL( ECamCmdSelfTimerCancel );
      }
    
    if ( SequenceCaptureInProgress() )
      {
      PRINT( _L("Camera <> Sequence capture in progress. -> Stop and skip postcapture.") )
            
      iCaptureStoppedForUsb = ETrue;
      SoftStopBurstL( ETrue );
      }
    // If in the middle of init sequence or saving a file, set the dismount
    // pending so that the AllowDismount call will be made later
    if( IsFlagOn( iBusyFlags, EBusySequence ) || iSaving )
      {
      PRINT( _L("Camera <> CCamAppController::DriveChangeL sequence or saving ongoing" ) );  		
      
      iDismountPending = ETrue;    
      // Mass memory may be the forced storage location. Then it's necessary 
      // to switch to (forced) phone memory
      TRAP_IGNORE( ForceUsePhoneMemoryL( ETrue ) );
      PRINT( _L("Camera <= CCamAppController::DriveChangeL dismount pending" ) );  	

      }
    else
      {    	           
      // Mass memory may be the forced storage location. Then it's necessary 
      // to switch to (forced) phone memory
      TRAP_IGNORE( ForceUsePhoneMemoryL( ETrue ) );
      PRINT( _L("Camera <= CCamAppController::DriveChangeL dismount ok" ) );  	    

      }        
    }
  else if ( IsMemoryAvailable( iPreferredStorageLocation, ETrue ) &&
            ( ECamMediaStorageCard == iPreferredStorageLocation ||
              ECamMediaStorageMassStorage  == iPreferredStorageLocation ) )
    {
    PRINT( _L("Camera <> iPreferredStorageLocation (MMC or mass memory) is now available.") )
 
    // if phone memory forced, then revert back to user selected location
    if ( iForceUseOfPhoneMemory )
      {
      PRINT( _L("Camera <> Phone memory is forced, switch back to user selected storage location.") )
      TRAP_IGNORE( ForceUsePhoneMemoryL( EFalse ) );
      }
    else
      {
      PRINT( _L("Camera <> !IsPhoneMemoryForced() -> No action.") )
      }
    }
  
  if( IntegerSettingValue(ECamSettingItemRemovePhoneMemoryUsage) )
        {
        if( aType == EDriveMount &&
            !appUi->IsRecoverableStatus() )
              {
              SwitchToStandbyL( KErrNone );
              }
          else if( aType == EDriveDismount &&
                  appUi->IsRecoverableStatus() )
              {
              TInt mmcInserted = 0;
              TInt usbPersonality = 0;
              User::LeaveIfError( RProperty::Get( KPSUidUikon, KUikMMCInserted, mmcInserted ) );
              User::LeaveIfError(RProperty::Get(KPSUidUsbWatcher, 
                                            KUsbWatcherSelectedPersonality,
                                            usbPersonality) );
              if( !IsMemoryAvailable(ECamMediaStorageMassStorage) &&
                   !IsMemoryAvailable(ECamMediaStorageCard) )
                  {
                  if( KUsbPersonalityIdMS == usbPersonality )
                      {
                      SwitchToStandbyL( ECamErrMassStorageMode );
                      }
                  else if ( !mmcInserted )
                      {
                      SwitchToStandbyL( ECamErrMemoryCardNotInserted );
                      }
                  }
              }
          else if ( aType == EDriveUSBMassStorageModeOn &&
                  appUi->IsRecoverableStatus() )
              {
              SwitchToStandbyL(ECamErrMassStorageMode);
              }
          else if ( aType == EDriveUSBMassStorageModeOff &&
                  !appUi->IsRecoverableStatus() )
              {
              SwitchToStandbyL( KErrNone );
              }
        }
    
  if( appUi->CurrentViewState() == ECamViewStatePostCapture  )
      {
      if( aType == EDriveDismount && !IsMemoryAvailable( currentStorage, EFalse ))
          {
          TVwsViewId activeView;
          if ( appUi->GetActiveViewId( activeView ) == KErrNone )
              {
              if( ECamViewIdStillPostCapture == activeView.iViewUid.iUid  ||
                  ECamViewIdVideoPostCapture == activeView.iViewUid.iUid   )
                  {
                  CCamPostCaptureViewBase* view = 
                          static_cast<CCamPostCaptureViewBase*>( appUi->View( activeView.iViewUid ) );
                  if( view )
                      {
                      view->HandleCommandL( EAknSoftkeyBack );
                      }
                  }
              }
          }
      }

  PRINT( _L("Camera <= CCamAppController::DriveChangeL" ) );  	
  return KErrNone;
  }

// ---------------------------------------------------------------------------
// CCamAppController::CurrentStorage
// ---------------------------------------------------------------------------
//    
TCamMediaStorage CCamAppController::CurrentStorage()   
    {
    return iCurrentStorage;
    }	 

// ---------------------------------------------------------------------------
// CCamAppController::IsAfNeeded
// ---------------------------------------------------------------------------
// 
TBool CCamAppController::IsAfNeeded()
    {
    return iAfNeeded;
    }

// ---------------------------------------------------------------------------
// CCamAppController::SetAfNeeded
// ---------------------------------------------------------------------------
// 
void CCamAppController::SetAfNeeded( TBool aAfNeeded )
    {
    iAfNeeded = aAfNeeded;
    }
	
// ---------------------------------------------------------------------------
// CCamAppController::IsCaptureStoppedForUsb
// ---------------------------------------------------------------------------
//    
TBool CCamAppController::IsCaptureStoppedForUsb()
    {
    return iCaptureStoppedForUsb;	
    }

// ---------------------------------------------------------------------------
// CCamAppController::SetCaptureStoppedForUsb
// ---------------------------------------------------------------------------
//    
void CCamAppController::SetCaptureStoppedForUsb( TBool aCaptureStoppedForUsb )
    {
    iCaptureStoppedForUsb = aCaptureStoppedForUsb;	
    }
    
// ---------------------------------------------------------------------------
// CCamAppController::CancelDismountMonitoring
// ---------------------------------------------------------------------------
//    
void CCamAppController::CancelDismountMonitoring()
    {
    if( iDriveChangeNotifier )
        {
        iDriveChangeNotifier->CancelMonitoring();		
        }
    }


// ---------------------------------------------------------------------------
// CCamAppController::StartDismountMonitoring
// ---------------------------------------------------------------------------
//    
void CCamAppController::StartDismountMonitoring()
    {
    if( iDriveChangeNotifier )
        {
        iDriveChangeNotifier->StartMonitoring();		
        }
    }

// ---------------------------------------------------------------------------
// CCamAppController::DeepSleepTimeout
// ---------------------------------------------------------------------------
//
TInt CCamAppController::DeepSleepTimeout( TAny* aPtr )
    {
    static_cast<CCamAppController*>( aPtr )->DoDeepSleepTimeout();
    return EFalse; // dummy
    }

// ---------------------------------------------------------------------------
// CCamAppController::DoDeepSleepTimeout
// ---------------------------------------------------------------------------
//
void CCamAppController::DoDeepSleepTimeout()
    {
    PRINT( _L("Camera => CCamAppController::DoDeepSleepTimeout") );

    ReleaseCamera();
    iDeepSleepTimerExpired = ETrue;
    iDeepSleepTimer->Cancel();

    PRINT( _L("Camera <= CCamAppController::DoDeepSleepTimeout") );
    }

// ---------------------------------------------------------------------------
// CCamAppController::DeepSleepTimerStart
// ---------------------------------------------------------------------------
//
void CCamAppController::DeepSleepTimerStart()
    {
    PRINT( _L("Camera : DeepSleepTimerStart") );
    iDeepSleepTimerExpired = EFalse;
    if ( iDeepSleepTimer && !iDeepSleepTimer->IsActive() )
        {
        iDeepSleepTimer->StartTimer();
        }
    }

// ---------------------------------------------------------------------------
// CCamAppController::DeepSleepTimerCancel
// ---------------------------------------------------------------------------
//
void CCamAppController::DeepSleepTimerCancel()
    {
    PRINT( _L("Camera : DeepSleepTimerCancel") );
    if ( iDeepSleepTimer )
        {
        iDeepSleepTimer->Cancel();
        }
    }

// ---------------------------------------------------------------------------
// CCamAppController::AllSnapshotsReceived
// ---------------------------------------------------------------------------
//    
TBool CCamAppController::AllSnapshotsReceived()
    {
    PRINT1( _L("Camera <> AllSnapshotsReceived %d"), iAllSnapshotsReceived );	
    return iAllSnapshotsReceived;	
    }

// ---------------------------------------------------------------------------
// CCamAppController::PendingAFRequest
// ---------------------------------------------------------------------------
// 
TInt CCamAppController::PendingAFRequest()
    {
    return iPendingAFRequest;
    }



// CCamAppController::IsTouchScreenSupported
// Return whether the device supports touch screen
// ---------------------------------------------------------------------------
//  
TBool CCamAppController::IsTouchScreenSupported()
    {
    return AknLayoutUtils::PenEnabled();
    }

// ---------------------------------------------------------------------------
// CCamAppController::IsTouchScreenSupported
// Return whether the device supports direct screen viewfinder
// ---------------------------------------------------------------------------
//  
TBool CCamAppController::IsDirectScreenVFSupported( TBool aPrimaryCamera )
    {
    // test for integration
    return iConfigManager->IsDSAViewFinderSupported( aPrimaryCamera );
    }

// ---------------------------------------------------------------------------
// CCamAppController::UiConfigManagerPtr
// handle  to CameraUiConfigManager
// ---------------------------------------------------------------------------
//  
CCameraUiConfigManager* CCamAppController::UiConfigManagerPtr()
    {
	__ASSERT_DEBUG(iConfigManager, CamPanic(ECamPanicNullPointer));
    return iConfigManager;
    }

	
// ---------------------------------------------------------------------------
// CCamAppController::SetCameraOrientationModeL
// 
// ---------------------------------------------------------------------------
//  
void CCamAppController::SetCameraOrientationModeL( TInt aOrientation )
    {
    iCameraController->SetOrientationModeL( aOrientation );
    }
    
// ---------------------------------------------------------------------------
// CCamAppController::CompleteCameraConstruction
// 
// ---------------------------------------------------------------------------
//  
void CCamAppController::CompleteCameraConstructionL()
    {
    PRINT( _L( "Camera => CCamAppController::CompleteCameraConstructionL" ) );    	
    iCameraController->CompleteSwitchCameraL();
    // start reserve and poweron already here unless embedded mode used
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if( appUi && !appUi->IsEmbedded() )
        {
        IssueModeChangeSequenceL( ETrue );
        }
    PRINT( _L( "Camera <= CCamAppController::CompleteCameraConstructionL" ) );    	    
    }

// ---------------------------------------------------------------------------
// CCamAppController::CurrentImageIndex
// 
// ---------------------------------------------------------------------------
//  
TInt CCamAppController::CurrentImageIndex()
    {
    return iCurrentImageIndex;
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void
CCamAppController::GenerateStartupSequenceL( RCamRequestArray& aSequence )
  {
  PRINT( _L( "Camera => CCamAppController::GenerateStartupSequenceL" ) );

  //Have to empty autofocusrequest values here because modechange could override
  //currently processed autofocus request.
  iPendingAFRequest=0;
  iCurrentAFRequest=0;
  iNoBurstCancel = EFalse;
  iKeyUP = EFalse;

  aSequence.Append( ECamRequestReserve );
  aSequence.Append( ECamRequestPowerOn );

  // -------------------------------------------------------
#ifdef _DEBUG
  PRINT( _L( "Camera <> Generated sequence: " ) );      
  for( TInt i = 0; i < aSequence.Count(); i++ )
    {
    PRINT2( _L( "Camera <> %d) %s" ), i, KCamRequestNames[aSequence[i]] );
    }
#endif  
  PRINT( _L( "Camera <= CCamAppController::GenerateStartupSequenceL" ) );
  }    
  
// ---------------------------------------------------------------------------
// CCamAppController::RotateSnapshotL
// Copy and rotate snapshot bitmap before it is used to
// create a thumbnail
// ---------------------------------------------------------------------------
//  
void CCamAppController::RotateSnapshotL()
    {
    PRINT( _L( "Camera => CCamAppController::RotateSnapshotL" ) );
    // empty the array
    if( iRotationArray->Count() > 0 )
        {
        iRotationArray->Reset();
        }
    if( BurstCaptureArray()->Snapshot( iCurrentImageIndex ) ) 
        {
    // copy snapshot to preserve the original snapshot bitmap
    // first get the handle for the original snapshot bitmap
    CFbsBitmap* snapshot = new (ELeave)CFbsBitmap();
    CleanupStack::PushL( snapshot );
    snapshot->Duplicate( BurstCaptureArray()->Snapshot( iCurrentImageIndex )->Handle() );
    //create a new bitmap with the same dimensions as the original snapshot
    iRotatedSnapshot = new (ELeave)CFbsBitmap();
    iRotatedSnapshot->Create( snapshot->SizeInPixels(), snapshot->DisplayMode() );
    TRect tmpRect( TPoint( 0, 0 ), snapshot->SizeInPixels() );
    TInt tmpLen = tmpRect.Width() * tmpRect.Height() * 4;
    //copy the snapshot data
    iRotatedSnapshot->BeginDataAccess();
    snapshot->BeginDataAccess();
    Mem::Copy( iRotatedSnapshot->DataAddress(), snapshot->DataAddress(), tmpLen );
    iRotatedSnapshot->EndDataAccess();
    snapshot->EndDataAccess();
    // copy the filename 
    iRotationArray->SetNextNameL( BurstCaptureArray()->FileName( iCurrentImageIndex ), BurstCaptureArray()->ImageName( iCurrentImageIndex ));
    // rotate the copied snapshot 
    if ( !iRotatorAo )
        {
        iRotatorAo = CCamSyncRotatorAo::NewL( *this );
        }    
    iRotatorAo->RotateL( iRotatedSnapshot, MapCamOrientation2RotationAngle( iCaptureOrientation ) );
        
    CleanupStack::PopAndDestroy(snapshot);
        } 
    PRINT( _L( "Camera <= CCamAppController::RotateSnapshotL" ) );    
    } 
    
// ---------------------------------------------------------------------------
// CCamAppController::RotationCompleteL
// Notification of bitmap rotation complete events
// ---------------------------------------------------------------------------
//  
void CCamAppController::RotationCompleteL( TInt aErr )
    {
    PRINT1( _L( "Camera => CCamAppController::RotationCompleteL aErr:%d" ), aErr );
    // rotation completed, use the rotated bitmap to create a thumbnail    
    // in case of error, don't use the snapshot, thumbnail is created from 
    // the imagefile instead
    if ( !aErr )
        {
        iRotationArray->ReplaceSnapshot( iRotatedSnapshot, 0 );
        }
    TRAP_IGNORE( iImageSaveActive->CreateThumbnailsL( *iRotationArray ) );
            
    delete iRotatedSnapshot; 
    iRotatedSnapshot = NULL;
    PRINT( _L( "Camera <= CCamAppController::RotationCompleteL" ) );         
    }     
      
// ---------------------------------------------------------------------------
// CCamAppController::StoreFaceTrackingValue
// Calls CamSettingModel to save user defined FT setting
// ---------------------------------------------------------------------------
//  
void CCamAppController::StoreFaceTrackingValue()
   {
   iSettingsModel->StoreFaceTrackingValue();	
   }

// ---------------------------------------------------------------------------
// CCamAppController::SetVideoInitNeeded
// Sets the value defining the need for CAE video init and prepare
// ---------------------------------------------------------------------------
//  
void CCamAppController::SetVideoInitNeeded( TBool aVideoInitNeeded )
    {
    iVideoInitNeeded = aVideoInitNeeded;  
    }

// ---------------------------------------------------------------------------
// CCamAppController::VideoInitNeeded
// Returns the value defining the need for CAE video init and prepare
// ---------------------------------------------------------------------------
//  
TBool CCamAppController::VideoInitNeeded()
    {
    return iVideoInitNeeded;  
    }
    
// ---------------------------------------------------------------------------
// StoreUserSceneSettingsL
// Stores the UserScene settings
// ---------------------------------------------------------------------------
//  
void CCamAppController::StoreUserSceneSettingsL()
    {
    iSettingsModel->StoreUserSceneSettingsL();	    
    }      

// ---------------------------------------------------------------------------
// HandleVideoQualitySettingChangeL
// ---------------------------------------------------------------------------
//
void 
CCamAppController::HandleVideoQualitySettingChangeL() 
  {
  PRINT( _L("Camera => CCamAppController::HandleVideoQualitySettingChangeL") );
  if( IsViewFinding() && !Busy() ) 
    {
    PRINT( _L("Camera <> CCamAppController - viewfinder on, stop vf / re-prepare / start vf..") );
    TRAPD( status, 
      {
      // Generate the request sequence and issue to Camera Controller.
      RCamRequestArray sequence;
      CleanupClosePushL( sequence );

      // Asynchronously init video. No need to reconstruct video names etc.
      sequence.Append( ECamRequestVfStop );
      sequence.Append( ECamRequestSsRelease );
      
      SetVideoInitNeeded( ETrue );

      sequence.Append( ECamRequestVideoRelease );
      sequence.Append( ECamRequestVideoInit );

      sequence.Append( ECamRequestSsStart );
      sequence.Append( ECamRequestVfStart );

      // Set busy flags to indicate sequence in progress and execute the seq 
      SetFlags( iBusyFlags, EBusySequence );
      TCleanupItem clearBusy( ClearSequenceBusyFlag, &iBusyFlags );
      CleanupStack::PushL( clearBusy );
      iCameraController->RequestSequenceL( sequence );
      CleanupStack::Pop();
   
      iCaptureModeTransitionInProgress = ETrue;
      CleanupStack::PopAndDestroy(); // sequence.Close()
      });

    if( KErrNone != status )
      {
      PRINT1( _L("Camera <> CCamAppController::HandleVideoQualitySettingChangeL, error:%d"), status );
      SetOperation( ECamStandby, status );
      }
    }

  PRINT( _L("Camera <= CCamAppController::HandleVideoQualitySettingChangeL") );
  }

// ---------------------------------------------------------------------------
// CCamAppController::ToggleWideScreenQuality
// Toggles between wide-screen (16:9) and VGA (4:3) screen resolutions.
// Applicable only for highest quality settings for 16:9 & 4:3.
// ---------------------------------------------------------------------------
//
TBool CCamAppController::ToggleWideScreenQuality( TBool aWide )
    {
    TBool qualityChanged = EFalse;
    
    if ( ECamControllerImage == iInfo.iMode && ECamActiveCameraPrimary == iInfo.iActiveCamera )
        {
        PRINT( _L("Camera => CCamAppController::ToggleWideScreenQuality - image mode") );
        TInt qualityIndex = IntegerSettingValue( ECamSettingItemPhotoQuality );
        if ( qualityIndex <= EImageQualityPrint )
            {
            if ( ( qualityIndex == EImageQualityPrintHigh &&  aWide )  
                 || ( qualityIndex == EImageQualityPrint && !aWide ) )
                {
                qualityIndex = (EImageQualityPrint==qualityIndex) ? EImageQualityPrintHigh 
                                                                  : EImageQualityPrint;

                // Ensure that the setting value is enabled/usable.
                if ( iSettingsModel->SettingValueEnabled( ECamSettingItemPhotoQuality, qualityIndex ) )
                    {
                    SetIntegerSettingValueL( ECamSettingItemPhotoQuality, qualityIndex );
                    qualityChanged = ETrue;
                    }
                }
            else
                ; // skip
            }
        PRINT( _L("Camera <= CCamAppController::ToggleWideScreenQuality") );
        }
    else if ( ECamControllerVideo == iInfo.iMode && ECamActiveCameraPrimary == iInfo.iActiveCamera ) 
        {
        PRINT( _L("Camera => CCamAppController::ToggleWideScreenQuality - video mode") );        
        TInt qualityIndex = IntegerSettingValue( ECamSettingItemVideoQuality );
        if ( qualityIndex <= ECamVideoQualityNormalHigh )
            {
            TVideoQualitySettings videoQuality = iConfiguration->VideoQuality( qualityIndex );
            qualityIndex = (ECamVideoQualityNormalHigh==qualityIndex) ? ECamVideoQualityHigh
                                                                      : ECamVideoQualityNormalHigh;

            if ( ( videoQuality.iVideoResolution == ECamVideoResolutionVGA &&  aWide ) || 
                 ( videoQuality.iVideoResolution == ECamVideoResolutionHD  && !aWide ) )
                {
                // Ensure that the setting value is enabled/usable.
                if ( iSettingsModel->SettingValueEnabled( ECamSettingItemVideoQuality, qualityIndex ) )
                    {
                    SetIntegerSettingValueL( ECamSettingItemVideoQuality, qualityIndex );
                    HandleVideoQualitySettingChangeL();
                    qualityChanged = ETrue;
                    }
                }
            }
        PRINT( _L("Camera <= CCamAppController::ToggleWideScreenQuality") );
        }
    else // avoids LINT warning.
        PRINT( _L("Camera =><= CCamAppController::ToggleWideScreenQuality - current quality level NOT high") );

    return qualityChanged;
    }

// ---------------------------------------------------------------------------
// CCamAppController::IsSaveStarted
// 
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsSaveStarted()
    {
    return iSaveStarted;    
    }

// ---------------------------------------------------------------------------
// CCamAppController::SwitchToStandby
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::SwitchToStandbyL( TInt aStatus )
    {
    PRINT( _L("Camera => CCamAppController::SwitchToStandbyL") );
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    __ASSERT_DEBUG( appUi, CamPanic( ECamPanicNullPointer ) );
    if( !appUi->ChangeStandbyStatusL(aStatus) )
        {
        SetOperation( ECamStandby, aStatus );
        iIdleTimer->Cancel();

        // Use backlight timer also with bitmap vf
        if( iBacklightTimer )
            {
            if( ECamTriActive == iCameraController->ViewfinderState() )
                {
                User::ResetInactivityTime();
                }
            iBacklightTimer->Cancel();
            }
        }
    switch( aStatus )
        {
        case ECamErrMassStorageMode:
        case ECamErrMemoryCardNotInserted:
            {
            iCurrentStorage = static_cast<TCamMediaStorage>(IntegerSettingValueUnfiltered( ECamSettingItemPhotoMediaStorage ));
            }
            break;
        default:
            {
            SetIntegerSettingValueL( ECamSettingItemPhotoMediaStorage, iCurrentStorage );
            SetIntegerSettingValueL( ECamSettingItemVideoMediaStorage, iCurrentStorage );
            }
            break;
        }
    PRINT( _L("Camera <= CCamAppController::SwitchToStandbyL") );
    }	

// ---------------------------------------------------------------------------
// CCamAppController::IssueModeChangeSequenceSucceeded
// 
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IssueModeChangeSequenceSucceeded()
    {
    return iIssueModeChangeSequenceSucceeded;
    }
	
// ---------------------------------------------------------------------------
// CCamAppController::EmbeddedStartupSequence
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::EmbeddedStartupSequence()
    {
    // If startup sequence fails at this point, it will be tried again later
    TRAP_IGNORE( IssueModeChangeSequenceL( ETrue ) );
    }

// ---------------------------------------------------------------------------
// CCamAppController::SnapshotRotationComplete
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::SnapshotRotationComplete()
    {
    PRINT( _L( "Camera => CCamAppController::SnapshotRotationComplete" ) );          
    // If snapshot rotation takes too long, it might not be drawn
    // unless specifically requested
    if( iSnapshotRedrawNeeded )
        {
        iSnapshotRedrawNeeded = EFalse;
        NotifyControllerObservers( ECamEventSnapshotRotated );    
        }
    PRINT( _L( "Camera <= CCamAppController::SnapshotRotationComplete" ) );    
    }

// ---------------------------------------------------------------------------
// CCamAppController::HandleTvAccessoryConnectedL
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleTvAccessoryConnectedL()
    {
    PRINT(_L("Camera => CCamAppController::HandleTvAccessoryConnectedL "));
    if( IntegerSettingValue( ECamSettingItemStopRecordingInHdmiMode) 
            && IsHdmiCableConnected() )
        {
        if( ECamCapturing == iInfo.iOperation || 
            ECamPaused == iInfo.iOperation )
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
            iHdmiCableConnectedDuringRecording = ETrue;
            TVwsViewId activeView;
            if ( appUi->GetActiveViewId( activeView ) == KErrNone )
              {
              if( ECamViewIdVideoPreCapture == activeView.iViewUid.iUid  )
                  {
                  CCamVideoPreCaptureView* view = static_cast<CCamVideoPreCaptureView*>( appUi->View( activeView.iViewUid ) );
                  view->HandleCommandL( ECamCmdStop );
                  }
              }
            }
        }
    PRINT(_L("Camera <= CCamAppController::HandleTvAccessoryConnectedL "));
    }

// ---------------------------------------------------------------------------
// CCamAppController::HandleTvAccessoryConnectedL
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleTvAccessoryDisconnectedL()
    {
    
    }

// ---------------------------------------------------------------------------
// CCamAppController::IsHdmiCableConnected
// 
// ---------------------------------------------------------------------------
//
TBool CCamAppController::IsHdmiCableConnected()
    {
    return iTvAccessoryMonitor->IsHdmiCableConnected();
    }
//  End of File  


// ---------------------------------------------------------------------------
// CCamAppController::RemainingVideoRecordingTime
// 
// ---------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds CCamAppController::RemainingVideoRecordingTime()
    {
    return iCameraController->RemainingVideoRecordingTime();
    }

// ---------------------------------------------------------------------------
// CCamAppController::HandlePostHdmiConnectDuringRecordingEventL
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::HandlePostHdmiConnectDuringRecordingEventL()
    {
    if( iHdmiCableConnectedDuringRecording == TBool(ETrue) )
        {
        iHdmiCableConnectedDuringRecording = EFalse;
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        appUi->HandleHdmiEventL( ECamHdmiCableConnectedDuringRecording );
        }  
    }


// ---------------------------------------------------------------------------
// CCamAppController::SetPendingHdmiEvent
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::SetPendingHdmiEvent( TCamHdmiEvent aPendingHdmiEvent )
    {
    iPendingHdmiEvent = aPendingHdmiEvent;
    }


// ---------------------------------------------------------------------------
// CCamAppController::HandlePendingHdmiEvent
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::HandlePendingHdmiEvent()
    {
    if( iPendingHdmiEvent != ECamHdmiNoEvent )
        {
        CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
        appUi->HandleHdmiEventL( iPendingHdmiEvent );
        iPendingHdmiEvent = ECamHdmiNoEvent;
        }
    }

    
// ---------------------------------------------------------------------------
// CCamAppController::HandleSecondaryCameraExit
// 
// Place here any extra things that need to be done when
// exiting camera app. in secondary camera mode
// ---------------------------------------------------------------------------
//
void CCamAppController::HandleSecondaryCameraExitL()
    {
    PRINT( _L( "Camera => CCamAppController::HandleSecondaryCameraExit" ) );          
    
    // Scene mode and face tracking issues --->
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );  
    // Do a switch to primary camera.
    appUi->HandleCommandL( ECamCmdSwitchCamera );
    // Set correct settings for primary camera.
    SetDynamicSettingsToDefaults();
    // Check if "User" scene mode should be on.
    iSettingsModel->SetUserSceneDefault();
    // PhotoSceneHasChangedL() needs to be called to 
    // get also face tracking to the correct state.
    iSettingsModel->PhotoSceneHasChangedL( IntegerSettingValue( ECamSettingItemDynamicPhotoScene ) );
    // StoreFaceTrackingValue() does nothing in 2ndary camera mode.
    // (Because scene mode is forced to Auto while in 2ndary camera.)
    // -> Always save face tracking state when exiting from 2ndary cam.
    SetIntegerSettingValueL( ECamSettingItemFaceTracking, iSettingsModel->GetPreviousFaceTrack() );   
    // <--- Scene mode and face tracking issues
    
    PRINT( _L( "Camera <= CCamAppController::HandleSecondaryCameraExit" ) );    
    }    

// ---------------------------------------------------------------------------
// CCamAppController::SceneModeForcedBySecondaryCamera
// 
// ---------------------------------------------------------------------------
//    
TBool CCamAppController::SceneModeForcedBySecondaryCamera()
    {
    return iSceneModeForcedBySecondaryCamera;
    }

// ---------------------------------------------------------------------------
// CCamAppController::SetTouchCapture
// 
// ---------------------------------------------------------------------------
//
void CCamAppController::SetTouchCapture( TBool aTouchCapture )
    {
    iTouchCapture = aTouchCapture;
    }
//  End of File  

