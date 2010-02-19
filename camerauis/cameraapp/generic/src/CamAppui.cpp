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
* Description:  App UI class for cameraapp
 *
*/


// This needs to be included first, because other include files
// may be variated using these flags.
#include <avkon.hrh>
#include <barsread.h>  // TResourceReader
#include <StringLoader.h>
#include <sysutil.h>
#include <eikenv.h>
#include <hlplch.h>
#include <apgwgnam.h>
#include <apgcli.h>
#include <DocumentHandler.h>
#include <apmstd.h>
#include <textresolver.h>
#include <aknnotewrappers.h>
#include <akntitle.h>  // CAknTitlePane
#include <aknclearer.h>
#include <aknnavide.h>
#include <AknWaitDialog.h>
#include <aknstaticnotedialog.h>
#include <AknCapServerDefs.h>
#include <akntoolbar.h>
#include <aknstyluspopupmenu.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <akntoolbarextension.h>
#include <eiksoftkeypostingtransparency.h>
#include <centralrepository.h>
#include <AknSgcc.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <activepalette2ui.h>
#include <AknGlobalNote.h>
#include <hal.h>
#include <hal_data.h>
#include <oommonitorsession.h>
#include <driveinfo.h>
#include <pathinfo.h>
#include <accmonitor.h>

#ifndef __WINSCW__
//#include <SFIUtilsAppInterface.h>
#endif

#include <musresourceproperties.h>
#include <cameraplatpskeys.h>

#include <AknCommonDialogsDynMem.h>
#include <CAknMemorySelectionDialogMultiDrive.h>

#include "CameraappPrivateCRKeys.h"
#include "CamAppUi.h"
#include "CamUtility.h"
#include "CamAppController.h"
#include "CamControllerObservers.h"
#include "CamTimer.h"
#include "CamLogger.h"
#include "CamStillPreCaptureView.h"
#include "CamStillPostCaptureView.h"
#include "CamVideoPreCaptureView.h"
#include "CamVideoPostCaptureView.h"
#include "CamBurstThumbnailView.h"
#include "CamSelfTimer.h"
#include "CamPanic.h"
#include "CamBurstModeObserver.h"
#include "CamStillUserSceneSetupView.h"
#include "CamNaviCounterControl.h"
#include "CamNaviCounterModel.h"
#include "CamNaviProgressBarControl.h"
#include "Cam.hrh"
#include "CamSettings.hrh"
#include "CamProductSpecificSettings.hrh"
#include "CamAppUiBase.h"
#include "CamWaitDialog.h"
#include "CamSidePane.h"
#include "CamZoomPane.h"
#include "CamAppUid.h"
#include <csxhelp/lcam.hlp.hrh>
#include "CameraVariant.hrh"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CamAppUiTraces.h"
#endif
#include "camactivepalettehandler.h"
#include "CamContainerBase.h"
#include "camuidconstants.h"
#include "CamGSInterface.h"
#include "CamFileCheckAo.h"
#include "CamLocalViewIds.h"
#include "CameraUiConfigManager.h"

#include "MCamAddToAlbumObserver.h"
#include "CamCollectionManagerAo.h"
#include "glxgallery.hrh"
#include "glxcollectionplugincamera.hrh"
#include "s32mem.h"


#include "camflashstatus.h"
#include "CamMemoryMonitor.h"



// ===========================================================================
// Constants

// Sequence mode related constants that define the amount of pictures taken
// with sequence mode.
const TInt KShortBurstCount  = 18;   // number of images captured during burst

// When asked if current memory is full in video mode
// there needs to be at least following amount of recording
// time left or memory is said to be full.
const TTimeIntervalMicroSeconds KMemoryFullVideoRemaining = 1000*1000;

// This is used to find the window group id for Eikon Server.
// The Eikon Server window group id is later used to ignore all window server
// events saying Eikon Server has gained focus.
_LIT( KEikonServer, "EikonServer" );

//const TCamMediaStorage KCamInternalStorage = ECamMediaStoragePhone;

const TUint KCameraEventInterest = ( ECamCameraEventClassBasicControl
                                   | ECamCameraEventClassImage
                                   | ECamCameraEventClassVideo
                                   | ECamCameraEventClassVfControl
                                   | ECamCameraEventClassSsControl
                                   | ECamCameraEventClassSettings );



// ===========================================================================
// Member functions

// -----------------------------------------------------------------------------
// CCamAppUi::~CCamAppUi
// Destructor
// -----------------------------------------------------------------------------
//
CCamAppUi::~CCamAppUi()
    {
    PRINT( _L("Camera => ~CCamAppUi" ))
    if ( iMemoryMonitor )
        {
        iMemoryMonitor->StopMonitoring();
        delete iMemoryMonitor;
        iMemoryMonitor = 0;
        }

    iController.SetAppUiAvailable( EFalse );

    iResourceLoaders.Close();

#ifdef CAMERAAPP_DELAYED_POSTCAPTURE_CREATION
    if( iPostCapIdle )
        {
        iPostCapIdle->Cancel();
        delete iPostCapIdle;
        }
#endif

    if ( iFileCheckAo )
        {
        iFileCheckAo->Cancel();
        delete iFileCheckAo;	
        }

    if ( iController.UiConfigManagerPtr()->IsOrientationSensorSupported() &&
         iSensorIdle )
        {
        CancelSensorIdle();
        delete iSensorIdle;
        }  

#if !defined(__WINSCW__)
    // relinquish capture keys
    UnRegisterCaptureKeys();    
#endif

    SaveCaptureMode();
    SaveCourtesyUiState();

    delete iDecoratedNaviPaneCounter;
    delete iDecoratedNaviPaneProgressBar;
#if defined( CAMERAAPP_CAPI_V2_DVF )
    if ( iSelfTimer )
        {
        iSelfTimer->RemoveObserver( this );
        }
#endif // CAMERAAPP_CAPI_V2_DVF
    delete iSelfTimer;
    delete iParentAppName;
    delete iTextResolver;

    iController.RemoveControllerObserver( this );

    // Burst mode observers are not owned, delete only the pointers to them.
    iBurstModeObservers.Close();

    if ( iWaitTimer && iWaitTimer->IsActive() )
        {
        iWaitTimer->Cancel();
        }
    delete iWaitTimer;
    delete iDocHandler;
    delete iCamSidePane;
    delete iCamZoomPane;       

    delete iRepository;

/*#ifndef __WINSCW__
    delete iSFIUtils;
#endif*/

    delete iNaviCounterControl;
    delete iNaviProgressBarControl;
    delete iNaviCounterModel;
    delete iNaviProgressBarModel;  
    delete iActivePaletteHandler;
    iActivePaletteHandler = NULL;

    if( iCourtesyTimer )
        {
        iCourtesyTimer->Cancel();
        }
    delete iCourtesyTimer;
    delete iScreenClearer;
    delete iVolumeKeyObserver;

    delete iCollectionManager;
    iCollectionManager = NULL;

    PRINT( _L("Camera <= ~CCamAppUi" ))
    }

// -----------------------------------------------------------------------------
// CCamAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamAppUi::ConstructL()
  {
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP2_CCAMAPPUI_CONSTRUCTL, "e_CCamAppUi_ConstructL 1" );
  PRINT( _L("Camera => CCamAppUi::ConstructL") )
  iRotatedKeyEvent = ETrue;
  iReturnedFromPlugin = EFalse;
  iController.SetAppUiAvailable( ETrue );

  iLockedQwertyState = E2ndCamUnknown; 
  iMemoryMonitor = CCamMemoryMonitor::NewL( this, &iController );

  TInt memError =
  iMemoryMonitor->CheckAndRequestMemoryL( iController.UiConfigManagerPtr()->CriticalLevelRamMemory(),
                                          iController.UiConfigManagerPtr()->RequiredRamMemory(),
                                          ETrue );


  TInt freeMemory = 0;
  HAL::Get( HALData::EMemoryRAMFree, freeMemory );
  if ( memError && freeMemory < iController.UiConfigManagerPtr()->CriticalLevelRamMemoryFocusGained() )
      {
      User::Leave( memError );
      }
  iUiConstructionComplete = EFalse;
  iDSASupported = iController.UiConfigManagerPtr()->IsDSAViewFinderSupported();
  
  TBool uiOrientationOverride = iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported();
  
  // Get the screenmode values used for setting  the orientation
  RArray<TInt> screenModeValues;
  if ( uiOrientationOverride )
      {
      iController.UiConfigManagerPtr()->SupportedScreenModesL( screenModeValues );
      iLandscapeScreenMode = screenModeValues[0];
      iPortraitScreenMode = screenModeValues[1];
      }
  
  // The embedded views are set after ConstructL completes
  // but the value is only of interest if the app is embedded
  iEmbeddedViewSet = !IsEmbedded();
  PRINT1( _L("Camera => CCamAppUi::ConstructL iEmbeddedViewSet:%d"), iEmbeddedViewSet );
  
  // In embedded case requesting new file without defined resolution
  // the 0*0 is interpreted as the default mms size resolution
  iRequestedNewFileResolution.SetSize( 0, 0 );

  // Camera window group id
  iMyWgId = iCoeEnv->RootWin().Identifier();

  // Get the id of Eikon Server window group
  iEikonServerWindowGroupId = iCoeEnv->WsSession().FindWindowGroupIdentifier(
    0,
    KEikonServer );
  
  CApaWindowGroupName::FindByAppUid(
    KBtNotifierServerUid,
    iCoeEnv->WsSession(),
    iBTServerWindowGroupId);

  CApaWindowGroupName::FindByAppUid(
    KPhoneAppUid,
    iCoeEnv->WsSession(),
    iPhoneAppWindowGroupId);

  PRINT( _L("Camera <> create new repository") );
  TInt crValue = 0;
  if( !iRepository )
    {
    iRepository = CRepository::NewL( KCRUidCameraappSettings ); 
    }

  // get softkey position
  CamUtility::GetPsiInt( ECamPsiSoftKeyPosition, iSoftkeyPosition );
  
  iCamOrientation = ReadCurrentDeviceOrientation();
  PRINT1( _L("Camera <> CurrentDeviceOrientation (%d)"), iCamOrientation )
  PERF_EVENT_START_L2( EPerfEventAvkonUIConstruction );  
  TInt orientation;
  if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() )
      {
      PRINT( _L("Camera <> using slider position to set orientation") );
      // Use the current slider position for the required orientation
      TInt sliderPosition;
      TInt err = RProperty::Get(
            CameraPlatPSKeys::KPSCameraPlatUid,
            CameraPlatPSKeys::KLensCoverStatus,
            sliderPosition );  
      
      // ToDo this could be removed after PCFW has released new implementation (wk25/2009)
      if( err == KErrNotFound )
          {
          PRINT( _L("Camera <> using slider position to set orientation") );
          err = RProperty::Get(
              NMusResourceApi::KCategoryUid,
              NMusResourceApi::KCameraAvailability,
              sliderPosition );  
          }
      PRINT2( _L("Camera <> slider position = %d, error = %d"),
            sliderPosition, err );
  
      // If there is an error then assume the slider is closed (as long as there is
      // more than one camera) 
      if ( err )
        {
        PRINT1( _L("Camera <> Slider status err%d - Check status later"),err )
        iController.SetCameraSwitchRequired( ESwitchToUnknown );
        }
      if ( ( ( CCamAppController::CamerasAvailable() > 1 && 
             sliderPosition == CameraPlatPSKeys::EClosed ) || 
           err != KErrNone ) && !IsQwerty2ndCamera( ETrue ) ) 
          {
          PRINT( _L("Camera <> setting orientation for secondary camera") )
          CamUtility::GetPsiInt( ECamPsiSecondaryCameraOrientation, orientation );
          }
      else
          {
          PRINT( _L("Camera <> setting orientation for primary camera") )
          CamUtility::GetPsiInt( ECamPsiPrimaryCameraOrientation, orientation );    
          } 
      }
  else
      {
      // Without lens cover support, always start to primary camera
      CamUtility::GetPsiInt( ECamPsiPrimaryCameraOrientation, orientation );
      }

  // Finish engine construction    
  if( uiOrientationOverride )
    {
    iController.CompleteConstructionL();
    }

  if ( orientation == ( TInt ) EAppUiOrientationLandscape )
    {
    PRINT( _L("Camera <> orientation is landscape ") )
    iCamOrientation = (ECamHandLeft == iSoftkeyPosition ) 
                    ? ECamOrientationCamcorderLeft 
                    : ECamOrientationCamcorder;
    if( uiOrientationOverride )
      {
      //set orientation to CCamera	
      iController.SetCameraOrientationModeL( iLandscapeScreenMode );
      // and complete the cameracontroller construction
      iController.CompleteCameraConstructionL();
      }
    PRINT( _L("Camera <> Calling BaseConstructL") )
    OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPUI_CONSTRUCTL, "e_CAM_APP_BASE_CONSTRUCT 1" ); //CCORAPP_APP_BASE_CONSTRUCT_START
    BaseConstructL( EAppOrientationLandscape | EAknEnableSkin | EAknEnableMSK | 
                    ENonStandardResourceFile | EAknSingleClickCompatible );
    OstTrace0( CAMERAAPP_PERFORMANCE, DUP1_CCAMAPPUI_CONSTRUCTL, "e_CAM_APP_BASE_CONSTRUCT 0" );    //CCORAPP_APP_BASE_CONSTRUCT_END
    PRINT( _L("Camera <> BaseConstructL Complete") )
    }
  else
    {
    PRINT( _L("Camera <> orientation is portrait") )
    iCamOrientation = ECamOrientationPortrait;
    if( uiOrientationOverride )
      {
      //set orientation to CCamera	
      iController.SetCameraOrientationModeL( iPortraitScreenMode );
      // and complete the cameracontroller construction
      iController.CompleteCameraConstructionL();
      }     
    PRINT( _L("Camera <> Calling BaseConstructL") )
    OstTrace0( CAMERAAPP_PERFORMANCE, DUP4_CCAMAPPUI_CONSTRUCTL, "e_CAM_APP_BASE_CONSTRUCT 1" );
    BaseConstructL( EAppOrientationPortrait | EAknEnableSkin | EAknEnableMSK | 
                    ENonStandardResourceFile );  
    OstTrace0( CAMERAAPP_PERFORMANCE, DUP5_CCAMAPPUI_CONSTRUCTL, "e_CAM_APP_BASE_CONSTRUCT 0" );
    PRINT( _L("Camera <> BaseConstructL Complete") )
    }

    PERF_EVENT_END_L2( EPerfEventAvkonUIConstruction );

#if !defined (__WINSCW__)
#if !( defined(__WINS__) || defined(__WINSCW__) )
  // Ensure that the macro switch keys are always captured by this
  // application.
  if ( !AppInBackground( EFalse ) )  //camera is constructed in background
     {
      RegisterCaptureKeysL();
     }
#endif // !defined(__WINS__) || defined(__WINSCW__)    
#endif

  if( !uiOrientationOverride )
    {
    // Load the settings model static settings
    PRINT( _L("Camera <> call CCamAppController::LoadStaticSettingsL..") )
    iController.LoadStaticSettingsL( IsEmbedded() );
  
    // Check the screen size if it matches the orientation we expect.
    // If not, delay the initialization until HandleScreenDeviceChangedL is called.
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP6_CCAMAPPUI_CONSTRUCTL, "e_ReadUiOrientationFromWindowServer 1" );
    TSize screenSize = iCoeEnv->ScreenDevice()->SizeInPixels();
    PRINT2( _L("<> CCamAppUi::ConstructL() iCoeEnv->ScreenDevice()->SizeInPixels():  w=%d h=%d"), screenSize.iWidth, screenSize.iHeight );
    TBool expectLandscape = (iCamOrientation != ECamOrientationPortrait);
    TBool isLandscape     = (screenSize.iWidth > screenSize.iHeight);
    if( isLandscape != expectLandscape || AppInBackground( EFalse ) )   //screen is not ready or camera app is in background
      {
      iEngineConstructionDelayed = ETrue;
      }

OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP7_CCAMAPPUI_CONSTRUCTL, "e_ReadUiOrientationFromWindowServer 0" );

    // initialise the self timer mode 
    iInSelfTimerMode = ECamSelfTimerDisabled;

    if ( !iEngineConstructionDelayed )     
      {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP8_CCAMAPPUI_CONSTRUCTL, "e_EngineConstructionDelayed 1" );
      iController.CompleteConstructionL();
      iController.StoreUserSceneSettingsL();
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP9_CCAMAPPUI_CONSTRUCTL, "e_EngineConstructionDelayed 0" );

      } 
    }
  else
    {
    iEngineConstructionDelayed = EFalse;    
    iInSelfTimerMode = ECamSelfTimerDisabled;
    }
  
  
  iEmbeddedObserver = NULL;
  
  iController.AddControllerObserverL( this );
  iController.AddCameraObserverL( this, KCameraEventInterest );
  
  if( !uiOrientationOverride )
    {
/*#ifndef __WINSCW__
    // Initialize SFIUtils
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP10_CCAMAPPUI_CONSTRUCTL, "e_InitSendFileInCall 1" );
	PRINT(_L("iSFIUtils = CSFIUtilsAppInterface::NewL -->"));
    TRAPD(errSFI,iSFIUtils = CSFIUtilsAppInterface::NewL());
	PRINT1(_L("iSFIUtils = CSFIUtilsAppInterface::NewL <-- errSFI=%d"),errSFI);
	User::LeaveIfError(errSFI);
    iSendFileInCall = EFalse;
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP11_CCAMAPPUI_CONSTRUCTL, "e_InitSendFileInCall 0" );
    PRINT( _L("iSendFileInCall = EFalse") );
#endif*/

    // Create text resolver for error note text
    iTextResolver = CTextResolver::NewL();

    ConstructPreCaptureViewsL();
  
    iSelfTimer = CCamSelfTimer::NewL( iController );
#if defined( CAMERAAPP_CAPI_V2_DVF )
    iSelfTimer->AddObserverL( this ); 
#endif // CAMERAAPP_CAPI_V2_DVF

    PRINT( _L("Camera <> creating navicounter model") );
    iNaviCounterModel = CCamNaviCounterModel::NewL( iController );
    PRINT( _L("Camera <> append navicounter to resourceloaders") );
    User::LeaveIfError( iResourceLoaders.Append(iNaviCounterModel) );
  
    PRINT( _L("Camera <> creating progress bar model") );
    iNaviProgressBarModel = CCamNaviProgressBarModel::NewL( iController );
    PRINT( _L("Camera <> append progressbar to resourceloaders") );
    User::LeaveIfError(iResourceLoaders.Append(iNaviProgressBarModel));
  
    ConstructNaviPaneL();
    }

  
  iRepository->Get( KCamCrAlwaysDrawCourtesyUi, crValue );
  iAlwaysDrawPreCaptureCourtesyUI = (crValue == KCamCrAlwaysDrawCourtesyUiIsOn);

  iCoeEnv->RootWin().EnableScreenChangeEvents();


  if( uiOrientationOverride )
    {
    // always start in still mode	
    iMode = ECamControllerImage;
    iTargetMode = ECamControllerImage;	
    }
  else
    {  
    // If the view launch needs to wait until the engine
    // is constructed or the embedded launch view is set, the starting
    // state will be standby    
    if ( !IsConstructionComplete() )
      {
      iViewState = ECamViewStateStandby;
      }

    // Get start up mode from settings.
    // If the application is embedded the new view will be set
    // when HandleNewFileL is called by the framework
    PRINT( _L("Camera <> CCamAppUi::ConstructL set default view") );
    if ( StartupMode() == ECamStillCapture )
      {
      iMode = ECamControllerImage;
      iTargetMode = ECamControllerImage;
      SetDefaultViewL( *iStillCaptureView );
      }
    else
      {
      iMode = ECamControllerVideo;
      iTargetMode = ECamControllerVideo;
      SetDefaultViewL( *iVideoCaptureView );
      }      


    if( !iEngineConstructionDelayed )
      {
      // pre-construct side-pane & zoom pane
      // get whether we overlay sidepane over view-finder
      TBool overlayViewFinder;
      User::LeaveIfError( CamUtility::GetPsiInt( ECamPsiOverLaySidePane, overlayViewFinder ) );
      PRINT( _L("Camera <> CCamAppUi::ConstructL create sidepane") );
      iCamSidePane = CCamSidePane::NewL( iController, overlayViewFinder );
      User::LeaveIfError(iResourceLoaders.Append(iCamSidePane));
    
      PRINT( _L("Camera <> CCamAppUi::ConstructL create zoom pane") );
      iCamZoomPane = CCamZoomPane::NewL( iController, overlayViewFinder );
      User::LeaveIfError(iResourceLoaders.Append(iCamZoomPane));
      }
  }  
  // create the timer used for callbacks
  iWaitTimer = CPeriodic::NewL( CActive::EPriorityHigh );

 
  // initialise the view construction states
  iPostCaptureViewsConstructed     = EFalse;
  iUserSceneSetupViewConstructed   = EFalse;
  iCustomiseToolbarViewConstructed = EFalse;
  
  if( !uiOrientationOverride )
    {  
    PRINT( _L("Camera => CCamAppUi::ConstructL create doc handler") );
    iDocHandler = CDocumentHandler::NewL( CEikonEnv::Static()->Process() );
  iController.CheckMemoryToUseL();
  
    // create navi-pane and navi-porgress bar for use in camcorder mode 
    PRINT( _L("Camera => CCamAppUi::ConstructL create navicounter control") );
    iNaviCounterControl = CCamNaviCounterControl::NewL( *iNaviCounterModel );
    iNaviCounterControl->SetExtentToWholeScreen();
  
    PRINT( _L("Camera => CCamAppUi::ConstructL create progress bar control") );
    iNaviProgressBarControl = CCamNaviProgressBarControl::NewL( *iNaviProgressBarModel );        
  
    // get max num of images for burst capture
    CamUtility::GetPsiInt( ECamPsiMaxBurstCapture, iMaxBurstCaptureNum );
  
    PRINT( _L("Camera => CCamAppUi::ConstructL create AP handler") );
    iActivePaletteHandler = CCamActivePaletteHandler::NewL( iController, EFalse );
    }
      
  iDrawPreCaptureCourtesyUI = ETrue;
  iDrawPostCaptureCourtesyUI = ETrue;
  iAlwaysDrawPostCaptureCourtesyUI = ETrue;
  iLeftSoftKeyPressed = EFalse;
  if( !uiOrientationOverride )
    {
    // get coutesy UI timeout
    TInt timeoutValue;
    CamUtility::GetPsiInt( ECamPsiCourtesyUiTimeoutValue, timeoutValue );
  
    iCourtesyTimer = CCamTimer::NewL( timeoutValue * 1000000, 
                                      TCallBack( CCamAppUi::CourtesyTimeout, this ) );

    PRINT( _L("Camera => CCamAppUi::ConstructL update navi models") );
    UpdateNaviModelsL();
    PRINT( _L("Camera => CCamAppUi::ConstructL navi counter reload resources") );
    iNaviCounterModel->ReloadResourceDataL();
    PRINT( _L("Camera => CCamAppUi::ConstructL navi progress bar reload resources") );
    iNaviProgressBarModel->ReloadResourceDataL();
    }
      
  iZoomPaneShown = EFalse;
  iFirstBoot = ETrue; // Show zoom
  iProcessingScreenRedraw = EFalse;
  iSendAsInProgress = EFalse;
  
  if( !uiOrientationOverride )
    {  
    iFileCheckAo = CCamFileCheckAo::NewL( iController, *this );  
    iCollectionManager = new (ELeave) CCamCollectionManagerAO( *this );
    }

  iCheckOperationInProgress = EFalse;
  iLostFocusToNewWindow = EFalse;
  iFirstVFStart = ETrue;

  iInternalStorage = iController.ExistMassStorage()?ECamMediaStorageMassStorage:ECamMediaStoragePhone;

  if ( !uiOrientationOverride )
    {
    iUiConstructionComplete = ETrue;
    }
  iMemoryMonitor->StartMonitoring( iController.UiConfigManagerPtr()->CriticalLevelRamMemoryFocusGained(),
                                   iController.UiConfigManagerPtr()->RequiredRamMemoryFocusGained() );
  PRINT( _L("Camera <= CCamAppUi::ConstructL") )
  OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP3_CCAMAPPUI_CONSTRUCTL, "e_CCamAppUi_ConstructL 0" );
  }
    


// -----------------------------------------------------------------------------
// IsInternalView
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::IsInternalView( TCamViewState aViewState ) const
  {
  PRINT1( _L("Camera => CCamAppUi::IsInternalView [%s]"), KCamViewStateNames[aViewState] );

  TBool internal( EFalse );

  if ( aViewState < ECamViewStateRangeInternalMax
    && aViewState > ECamViewStateRangeInternalMin )
      {
      internal = ETrue;
      }
  else if( aViewState < ECamViewStateRangeExternalMax
        && aViewState > ECamViewStateRangeExternalMin )
      {
      internal = EFalse;
      }
  else
    {
    CamPanic( ECamPanicInvalidView );
    }

  PRINT1( _L("Camera <= CCamAppUi::IsInternalView, return %d"), internal );
  return internal;
  }


 
// -----------------------------------------------------------------------------
// CCamAppUi::IsConstructionComplete
// Returns whether or not all construction has completed
// -----------------------------------------------------------------------------
//    
TBool CCamAppUi::IsConstructionComplete() const
    {     
    TBool complete = ETrue;
    complete = !iEngineConstructionDelayed;
    return complete && iEmbeddedViewSet; 
    }

    
// -----------------------------------------------------------------------------
// CCamAppUi::NaviCounter
// Returns the pointer to the NaviCounter control
// -----------------------------------------------------------------------------
//
CCamNaviCounterControl* CCamAppUi::NaviCounterControl()
    {
    return iNaviCounterControl;
    }
    
// -----------------------------------------------------------------------------
// CCamAppUi::NaviProgressBar
// Returns the pointer to the NaviProgress control
// -----------------------------------------------------------------------------
//
CCamNaviProgressBarControl* CCamAppUi::NaviProgressBarControl()
    {
    return iNaviProgressBarControl;
    }

// -----------------------------------------------------------------------------
// CCamAppUi::NaviCounterModel
// Returns the pointer to the NaviCounter model
// -----------------------------------------------------------------------------
//    
CCamNaviCounterModel* CCamAppUi::NaviCounterModel()
    {
    return iNaviCounterModel;
    }


// -----------------------------------------------------------------------------
// CCamAppUi::CCamAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamAppUi::CCamAppUi( CCamAppController& aController )                              
    : iController( aController ),
    iViewState( ECamViewStatePreCapture ),
    iTargetViewState( ECamViewStatePreCapture ), 
    iPreventActivePaletteDisplay( EFalse ),
    iInternalStorage(ECamMediaStorageNone)
    {
    }

// ----------------------------------------------------
// CCamAppUi::SetTitleL
// Set title pane text from a resource.
// ----------------------------------------------------
//
void CCamAppUi::SetTitleL( TInt aResourceId )
    {
    // Set title
    CAknTitlePane* title = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceId );
    title->SetFromResourceL( reader );
    CleanupStack::PopAndDestroy();  // resource reader
    }

// ----------------------------------------------------
// CCamAppUi::SetTitleL
// Set title pane text from a descriptor.
// ----------------------------------------------------
//
void CCamAppUi::SetTitleL( const TDesC& aText )
    {
    CAknTitlePane* title = static_cast<CAknTitlePane*>(
        StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    title->SetTextL( aText );
    }



// ----------------------------------------------------
// CCamAppUi::PushNaviPaneCounterL
// Push navi pane counter on to navi pane stack.
// ----------------------------------------------------
//
void 
CCamAppUi::PushNaviPaneCounterL()
  {
  iNaviCounterModel->SetCaptureModeL( iMode, iImageMode );
  iDecoratedNaviPaneCounter->MakeScrollButtonVisible( EFalse );
  NaviPaneL()->PushL( *iDecoratedNaviPaneCounter );
  }

// ----------------------------------------------------
// CCamAppUi::PushNaviPaneProgressBarL
// Push navi pane progress bar on to navi pane stack.
// ----------------------------------------------------
//
void CCamAppUi::PushNaviPaneProgressBarL()
    {
    iDecoratedNaviPaneProgressBar->MakeScrollButtonVisible( EFalse );
    NaviPaneL()->PushL( *iDecoratedNaviPaneProgressBar );
    }

// ----------------------------------------------------
// CCamAppUi::PushDefaultNaviPaneL
// Push default navi pane on to navi pane stack.
// ----------------------------------------------------
//
void CCamAppUi::PushDefaultNaviPaneL()
    {
    NaviPaneL()->PushDefaultL();
    }


// ----------------------------------------------------
// CCamAppUi::DisplayDeleteNoteL
// Call DisplayDeleteNoteL on the current view
// ----------------------------------------------------
//
TBool CCamAppUi::DisplayDeleteNoteL()
  {
  TBool deleted = 
          static_cast<CCamPostCaptureViewBase*>( iView )->DisplayDeleteNoteL();

  // If the image/video has been deleted, switch view
  if ( deleted )
    {
                              
    if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() && IsEmbedded() )
        {
        // if in embedded mode, then check the status of the slide
        // and set a pending camera switch if the slide has been
        // opened/closed after starting the capture
        iController.CheckSlideStatus();
        }      
                               
    // if this is burst mode and there are still images left, 
    // return to thumbnail view
    if ( ECamImageCaptureBurst == iController.CurrentImageModeSetup()
      && 0                     <  iController.CurrentItemCount() )
      {
      iTargetViewState = ECamViewStateBurstThumbnail;
      }
    // otherwise switch to pre-capture view
    else
      {
      iTargetViewState = ECamViewStatePreCapture;
      }
    TrySwitchViewL();
    }
  else
    {
    }
  return deleted;
  }

// ----------------------------------------------------
// CCamAppUi::SelfTimerEnableL
// Enable or disable SelfTimer functionality
// ----------------------------------------------------
//
void CCamAppUi::SelfTimerEnableL( TCamSelfTimerFunctions aState )
    {
    // If the requested state is the current state, do nothing.
    if ( iInSelfTimerMode == aState )
        {
        return;
        }

    __ASSERT_DEBUG( iSelfTimer, CamPanic( ECamPanicNullPointer ) );

    // Update the member variable
    iInSelfTimerMode = aState;

    if ( aState != ECamSelfTimerDisabled  )
        {     
        // Set up for current timer mode
        iSelfTimer->SetModeL( iMode, iImageMode, iInSelfTimerMode );              
        UpdateCba();
        }
    else                
        {
        // stop the timer and remove the indicator from the side pane
        iSelfTimer->Cancel();

        // In capture setup mode, toolbar and indicators are not visible
        // They will be updated when returning to precap, similarily to
        // when setting the self timer mode above
        if( iPreCaptureMode != ECamPreCapCaptureSetup )
            {
            // Re-show the active palette
            iActivePaletteHandler->UpdateActivePaletteL();
            iPreventActivePaletteDisplay = EFalse;
            SetActivePaletteVisibility( ETrue );
            
            if ( iController.IsTouchScreenSupported() )
                {
                CAknToolbar* toolbar = CurrentFixedToolbar();
                if (toolbar)
                    {
                    toolbar->SetToolbarVisibility( ETrue );
                    }
                }
            // Cancel any active focusing operation
            // this won't cancel if capture has already been requested
            iController.CancelFocusAndCapture();
                
            iStillCaptureView->UpdateToolbarIconsL();
            
            // Raise precapture UI and restart courtesy UI timer,
            // if hide icons is enabled.
            RaisePreCaptureCourtesyUI(EFalse);        
            }        
        
        UpdateCba();
        }    
    }

// -----------------------------------------------------------------------------
// CCamAppUi::SelfTimerEnabled
// Returns current self-timer state
// -----------------------------------------------------------------------------
//        
TBool CCamAppUi::SelfTimerEnabled() const
    {
    return ( iInSelfTimerMode != ECamSelfTimerDisabled );
    }

// -----------------------------------------------------------------------------
// CCamAppUi::SelfTimer
// Returns pointer to self-timer object
// -----------------------------------------------------------------------------
//        
CCamSelfTimer* CCamAppUi::SelfTimer()
    {
    return iSelfTimer;
    }

// -----------------------------------------------------------------------------
// CCamAppUi::AddBurstModeObserverL
// Register an observer for burst mode changes
// -----------------------------------------------------------------------------
// 
void 
CCamAppUi::AddBurstModeObserverL( MCamBurstModeObserver* aBurstModeObserver )
  {
  // Check for non-null and only add once.
  if( aBurstModeObserver &&
     KErrNotFound == iBurstModeObservers.Find( aBurstModeObserver ) )
    {
    iBurstModeObservers.AppendL( aBurstModeObserver );
    }
  }

// -----------------------------------------------------------------------------
// CCamAppUi::RemoveBurstModeObserver
// Remove the burst mode observer
// -----------------------------------------------------------------------------
//
void
CCamAppUi::RemoveBurstModeObserver( const MCamBurstModeObserver* aBurstModeObserver )
  {
  if( aBurstModeObserver )
    {
    TInt index = iBurstModeObservers.Find( aBurstModeObserver );
    if( KErrNotFound != index )
      {
      iBurstModeObservers.Remove( index );
      }
    }
  }

// -----------------------------------------------------------------------------
// CCamAppUi::IsBurstEnabled
// Return burst mode enabled state
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::IsBurstEnabled() const
  {
  PRINT_FRQ( _L("Camera => CCamAppUi::IsBurstEnabled" ) )
  TBool ret = EFalse;
  if ( ECamImageCaptureBurst     == iImageMode 
    || ECamImageCaptureTimeLapse == iImageMode )
    {
    ret = ETrue;
    }

  PRINT_FRQ( _L("Camera <= CCamAppUi::IsBurstEnabled" ) )
  return ret;
  }
    
// -----------------------------------------------------------------------------
// CCamAppUi::CurrentBurstMode
// Return burst mode enabled type
// -----------------------------------------------------------------------------
// 
TCamImageCaptureMode CCamAppUi::CurrentBurstMode() const
  {
  PRINT_FRQ( _L("Camera => CCamAppUi::CurrentBurstMode" ) )
  TCamImageCaptureMode mode( ECamImageCaptureNone );

  if ( ECamImageCaptureBurst     == iImageMode
    || ECamImageCaptureTimeLapse == iImageMode )
    {
    mode = iImageMode;
    }

  PRINT_FRQ( _L("Camera <= CCamAppUi::CurrentBurstMode" ) )
  return mode;
  }   


// -----------------------------------------------------------------------------
// CCamAppUi::IsSecondCameraEnabled
// Returns whether or the secondary camera has been activated
// Will return ETrue when when the second camera is active
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::IsSecondCameraEnabled() const
    {
    PRINT_FRQ( _L("Camera =><= CCamAppUi::IsSecondCameraEnabled" ))
    return iController.ActiveCamera() == ECamActiveCameraSecondary;
    }

// -----------------------------------------------------------------------------
// CCamAppUi::IsQwerty2ndCamera
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::IsQwerty2ndCamera( TBool aLock )
    {
    TBool ret = EFalse;
    if ( aLock )
        {
        iLockedQwertyState = E2ndCamUnknown;
        }

    switch( iLockedQwertyState )
        {
        case E2ndCamPortraitForced: 
            {
            ret = EFalse; 
            break;
            }
        case E2ndCamLandscapeForced:     
            {
            ret = ETrue; 
            break;
            }
        case E2ndCamUnknown:     
            {
            // Get variant setting i.e. initialize
            TInt value = 0;
            if ( iController.UiConfigManagerPtr() )
               {
            	value = iController.UiConfigManagerPtr()->IsQwerty2ndCamera();
                
                iLockedQwertyState = static_cast<T2ndCamOrientation>(value); 
                if ( iLockedQwertyState == E2ndCamAuto )
                    {
                    ret = CamUtility::IsQwertyOpen(); 
                    }
                else
                    {
                    ret = iLockedQwertyState == E2ndCamLandscapeForced;
                    }
 		}            
            break;
            }
        case E2ndCamAuto:
            {
            ret = CamUtility::IsQwertyOpen(); 
            break;
            }
        default: 
            break;
        }

    if ( aLock )
        {
        iLockedQwertyState = ret?E2ndCamLandscapeForced:E2ndCamPortraitForced;
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CCamAppUi::IsWaitDialog
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::IsWaitDialog() const
  {
  return iWaitDialog != NULL;
  }

// -----------------------------------------------------------------------------
// CCamAppUi::ActiveCamera
// Returns the active camera
// -----------------------------------------------------------------------------
//
TCamActiveCamera CCamAppUi::ActiveCamera() const
  {
  return iController.ActiveCamera();
  }
    
// -----------------------------------------------------------------------------
// CCamAppUi::HandleCommandL
// Handle user menu selections
// -----------------------------------------------------------------------------
//
void CCamAppUi::HandleCommandL( TInt aCommand )
  {
  PRINT1( _L("Camera => CCamAppUi::HandleCommandL (%d)"), aCommand )
  switch( aCommand )
    {       
    // -----------------------------------------------------
    case ECamCmdRaiseCourtesyUI:
      {            
      if ( ECamViewStatePreCapture == iViewState )
        {
        RaisePreCaptureCourtesyUI(EFalse);
        }
      else if ( ECamViewStatePostCapture == iViewState )
        {
        // iLeftSoftKeyPressed resembles the Left SoftKey(LSK) 
        // press when AP tool bar is hidden

        if ( iLeftSoftKeyPressed )
        	{
        	// LSK is pressed, we raise the AP tool bar
        	RaisePostCaptureCourtesyUI();
        	}
        else
        	{
        	// RSK is pressed, we return back to PreCaptureView
        	HandleCommandL( EAknSoftkeyBack );	
        	}
        
        }
      else
        {
        }
      }
      break;

    // -----------------------------------------------------
    case ECamCmdSceneSettingList:
      {
      iTargetViewState = ECamViewStateSceneSettingList;
      TrySwitchViewL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdGoToStandby:
      {
      CAknTitlePane* title = static_cast<CAknTitlePane*>
                             ( StatusPane()->ControlL( TUid::Uid ( EEikStatusPaneUidTitle ) ) );

      title->SetTextL( _L("") ); 
      title->MakeVisible( ETrue ); 
      title->DrawNow(); 
      SetActivePaletteVisibility( EFalse );

      if ( iView 
          && ( iStillCaptureView == iView || iVideoCaptureView == iView ) )
          {
          iProcessingScreenRedraw = ETrue;
          static_cast<CCamPreCaptureViewBase*>( iView )->Container()->DrawNow();
          iProcessingScreenRedraw = EFalse;
          }

      PRINT1( _L("CCamAppUi::HandleCommandL standby %d"), iViewState );            
      iTargetViewState = ECamViewStateStandby;
      TrySwitchViewL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdSwitchToPrecapture:
      {
      iTargetViewState = ECamViewStatePreCapture;
      TrySwitchViewL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdNewPhoto:
      {
	  if ( !iController.IsViewFinding() && iController.EngineRequestsPending() ) 
	    {	
		return;
	    }
      // check if currently preparing return
      if ( iController.CaptureModeTransitionInProgress() )
        {
        return;
        }
      #ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
      PERF_EVENT_START_L1( EPerfEventSwitchToStillMode );
      iController.SetPerformanceState( EPerfWaitingForStillMode );
      #endif    
      
      if ( iMode == ECamControllerVideo )
        {
        ZoomPane()->ResetZoomTo1x();    
        }

      // Hide the active palette before switching so that 
      // it is not visible until the preview is visible.

      SetActivePaletteVisibility( EFalse );
      if( iController.UiConfigManagerPtr() && 
          iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() && 
          iVideoCaptureView->Container() )
          {
          PRINT( _L("Camera <> CCamAppUi::HandleCommandL calling Draw") ); 
          iVideoCaptureView->Container()->DrawNow();
          } 
      // Switch to still pre capture view
      iTargetViewState = ECamViewStatePreCapture;   	
      // if coming from video mode then set single capture mode
      // otherwise stay with the current capture mode
      if ( iMode == ECamControllerVideo )
        {
        iTargetMode = ECamControllerImage;
        }
      TrySwitchViewL();
      

      }
      break;
    case EEikCmdExit: // fallthrough
    case EAknSoftkeyExit:
      {
      OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP3_CCAMAPPUI_HANDLECOMMANDL, "e_ExternalExit 1" );
      PRINT( _L("Camera CCamAppUi external exit call") );
      if ( iEikonEnv->StartedAsServerApp() )
        {
        if ( iEmbeddedObserver )
            {
            iEmbeddedObserver->AbortL();
            }
        CloseAppL(); 
        }
      else
/*#ifndef __WINS__
          {
          if( iSendFileInCall )
              {
              // bring phone app to fore  ground
              BringPhoneAppToForeGroundL();
              }
          else
              {
              // hide task icon immediately so it doesn't show during
              // closing which might take a while
              HideTaskL( ETrue );
              CloseAppL();  	
              }
          }
#else*/
          {
          HideTaskL( ETrue );
          CloseAppL();
          }
//#endif
        OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP5_CCAMAPPUI_HANDLECOMMANDL, "e_ExternalExit 0" );
      }
      break;
    // -----------------------------------------------------
    case ECamCmdInternalExit:
      {
      OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_HANDLECOMMANDL, "e_ECamCmdInternalExit 1" );
      PRINT( _L("Camera <> CCamAppUi::HandleCommandL case ECamCmdInternalExit") );            
    #ifdef CAMERAAPP_PERF_LOG_MEMORY 
      // Manually save the current memory log data. This is needed
      // in case the application just goes to background, in which
      // case the log data is not saved automatically.
      CCamPerformanceLogger::SaveAndReset();
    #endif  
              
    #ifdef CAMERAAPP_PERF_LOG_TRACES 
      // Shutdown end event cannot be logged to memory logger
      // So also log start event only to traces
      if( !iController.AlwaysOnSupported() )
        {
        PERF_EVENT_START_TRACE( EPerfEventApplicationShutdown );
        }
    #endif  
      PRINT( _L("Camera CCamAppUi::ECamCmdInternalExit") );
      InternalExitL();
      OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPUI_HANDLECOMMANDL, "e_ECamCmdInternalExit 0" );
      }
      break;  
    // -----------------------------------------------------
    case ECamCmdSlideClosedExit:
      {
      // The controller tells the AppUi that the app needs to 
      // do an internal exit because the slider has closed.
      // However the view needs to do some processing of this command
      // e.g. to ensure that the application leaves standby mode.
      // Pass the cammand on to the view. It will come back to
      // the AppUi as ECamCmdInternalExit
      OstTrace0( TRACE_PERFORMANCE_DETAIL, DUP2_CCAMAPPUI_HANDLECOMMANDL, "e_ECamCmdSlideClosedExit 1" );
      
      iLensCoverExit = ETrue;
      PRINT( _L("Camera CCamAppUi::ECamCmdSlideClosedExit") );
      if ( iView )
        {
        PRINT( _L("Camera CCamAppUi passing ECamCmdSlideClosedExit to iView") );
        iView->HandleCommandL( ECamCmdInternalExit );
        }
      else
        {
        PRINT( _L("Camera CCamAppUi handlingECamCmdSlideClosedExit") );
        HandleCommandL( ECamCmdInternalExit );
        }
      OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP4_CCAMAPPUI_HANDLECOMMANDL, "e_ECamCmdSlideClosedExit 0" );
      }
      break;               
    // -----------------------------------------------------
    case EAknSoftkeyBack:
      {
      PRINT( _L("Camera <> CCamAppUi::HandleCommandL case EAknSoftkeyBack") );            
      // If in burst mode
      if ( iImageMode == ECamImageCaptureBurst )
        {
        // If in burst post-capture view, "back" is to the thumbnail grid
        if ( iViewState == ECamViewStatePostCapture )
          {
          iTargetViewState = ECamViewStateBurstThumbnail ;
          }
        else // From any other burst view... 
          {                             
          iTargetViewState = ECamViewStatePreCapture;               
          }
        }
        // Switch to pre capture view
      else
        {
        iTargetViewState = ECamViewStatePreCapture;   	
        }
      TrySwitchViewL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdDelete:
      {
      // Request that the view displays the delete confirmation note
	  if(!iController.IsProcessingCapture())
		{
		DisplayDeleteNoteL();
		}
	  else
		{
		//Do Nothing
		}
      }
      break;
    // -----------------------------------------------------
    case ECamCmdNewVideo:
      {
      // check if currently preparing return
      if ( iController.CaptureModeTransitionInProgress() )
        {
        return;
        }
      #ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
      PERF_EVENT_START_L1( EPerfEventSwitchToVideoMode );
      iController.SetPerformanceState( EPerfWaitingForVideoMode );
      #endif
      
      if ( iMode == ECamControllerImage )
        {
        ZoomPane()->ResetZoomTo1x();    
        }
      
      // Hide the active palette before any attempt to switch off burst mode
      // otherwise the view switch to video fails when we call GetFocusWindowGroup to
      // see which application has the foreground. Use SetSuppressUIRiseOnViewfinderStart
      // otherwise the AP is raised automatically when the viewfinder starts.
            
      SetActivePaletteVisibility( EFalse );
      SetSuppressUIRiseOnViewfinderStart( ETrue );                
      // Switch burst mode off before changing to video
      if ( IsBurstEnabled() )
        {
        SwitchStillCaptureModeL( ECamImageCaptureSingle, EFalse );
        PRINT( _L("Camera => CCamAppUi::HandleCommandL ECamCmdNewVideo completed switch to single"));
        }

      // Switch to video pre capture view
      iTargetViewState = ECamViewStatePreCapture;
      iTargetMode = ECamControllerVideo;
      SetSuppressUIRiseOnViewfinderStart( EFalse );
      PRINT( _L("Camera => CCamAppUi::HandleCommandL Try switch to video mode"));
      TrySwitchViewL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdSelfTimer1: // 10-sec self-timer
      {
      SelfTimerEnableL( ECamSelfTimerMode1 );
      }             
      break;
    // -----------------------------------------------------
    case ECamCmdSelfTimer2: // 2-sec self-timer
      {
      SelfTimerEnableL( ECamSelfTimerMode2 );
      }             
      break;
    // -----------------------------------------------------
    case ECamCmdSelfTimer3: // (not supported)
      {
      SelfTimerEnableL( ECamSelfTimerMode3 );
      }             
      break;
    // -----------------------------------------------------
    case ECamCmdSelfTimerActivate:
      {
        // Self timed capture happens either from shutter press or
        // self timer activate event from softkey press. Shutter
        // press case is handled in CCamStillPreCaptureContainer.
        if( iController.UiConfigManagerPtr()->IsXenonFlashSupported() && 
            !iController.CheckFlash() )
            {
            // Flash is not ready for taking a picture
            return;            
            }        

      // In touch UI we will get two ECamCmdSelfTimerActivate commands
      // immediately then the user touches the left softkey to activate
      // self-timer. We need to ignore the second one to enable the counter
      // to count down properly.
      
      // only start countdown if there is space to save image
      if ( CheckMemoryL() )
        {
        if ( iMode != ECamControllerVideo )
          {
          InitCaptureCountL();
          }
        // Start capture operation NOW
        iSelfTimer->StartSelfTimer();            
          
        if ( iController.IsTouchScreenSupported() )
          {
          CAknToolbar* toolbar = CurrentFixedToolbar();
          if (toolbar)
            {
            toolbar->SetToolbarVisibility( EFalse );
            }
          }
            
        iPreventActivePaletteDisplay = ETrue;
        SetActivePaletteVisibility( EFalse );           
           
        // Update CBA
        UpdateCba();
        }
      }
      break;
    // -----------------------------------------------------
    case ECamCmdSelfTimerCancel:
      {
      // Called when the user clicks "Cancel" to leave self-timer mode
      SelfTimerEnableL( ECamSelfTimerDisabled );
      }
      break;
    // -----------------------------------------------------
    case ECamCmdSettings:
      {
      // Turn off the courtesy timer if its active so that the softkeys
      // will remain active
      if( iCourtesyTimer )
        {
        iCourtesyTimer->Cancel();
        }

      iReturnedFromPlugin = EFalse;
      iSettingsPluginLaunched = ETrue;
      SetActivePaletteVisibility( EFalse );
      CCamViewBase* precapView = NULL;
      if ( iView == iStillCaptureView ||
          iView == iVideoCaptureView )
        {
        precapView = static_cast<CCamViewBase*>( iView );
        __ASSERT_DEBUG(precapView, CamPanic(ECamPanicNullPointer));
		precapView->BlankSoftkeysL();
        }
      iTargetViewState = ECamViewStateSettings;
      if ( iController.IsTouchScreenSupported() )
         {
          CAknToolbar* toolbar = CurrentFixedToolbar();
          if ( toolbar )
              {
              toolbar->SetToolbarVisibility( EFalse );
              }
          }
      TrySwitchViewL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdSelect:
      {
      PRINT( _L("Camera => CCamAppUi::HandleCommandL ECamCmdSelect"));
      if ( iEikonEnv->StartedAsServerApp() )
        {
        const TDesC& name( iController.CurrentFullFileName() );
        
        PRINT1( _L("Camera => CCamAppUi::HandleCommandL ECamCmdSelect %S"), &name )
        iEmbeddedObserver->FileCompleteL( name );
        iController.EnterShutdownMode( ETrue );
        }
/*#ifndef __WINS__
      else if ( iSendFileInCall )
        {
        PRINT( _L("CCamAppUi::HandleCommandL SendMedia file") )
        // handle in-call send interaction
        
        // Sending of captured media file and to return to call view after sending        			
        BringPhoneAppToForeGroundL(); 
        DoInCallSendL();			  
        HideTaskL( ETrue ); // otherwise camera would show in "Active Applications"   		
        iPretendExit = ETrue; // we are basicly in a "pretended exit" state
        //BringPhoneAppToForeGroundL();
        }
#endif*/               
      else
        {
        // ACS What do we need to do here? Call internal exit?
        // this will exit when any current process completes
        iController.EnterShutdownMode( EFalse );
        }
      }
      break;
    // -----------------------------------------------------
    case EAknSoftkeyEmpty: // Must handle this key, but do nothing
      break;
    // -----------------------------------------------------
    case ECamCmdPhotos:
      { 
      TUid msgUidLastModified(TUid::Uid(KGlxActivationCmdShowLastModified));
      TUid msgUidCameraAlbum(TUid::Uid(KGlxActivationCameraAlbum));
      TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
      TApaTask apaTask = apaTaskList.FindApp( TUid::Uid( KGlxGalleryApplicationUid) );
        
      TBuf8<100> buffer;
      RDesWriteStream stream( buffer );
      CleanupClosePushL( stream );
      stream.Open( buffer );
      

      if ( ECamViewStatePreCapture == iViewState )
          {
          stream << msgUidCameraAlbum;          
          }
      else 
          {
          stream << msgUidLastModified;
          }
      stream.CommitL();

      //Hide fixed toolbar before calling to start photos.
      if ( iController.IsTouchScreenSupported() && 
           IsSecondCameraEnabled() && 
           iViewState == ECamViewStatePreCapture )
          {         
          CAknToolbar* fixedToolbar = CurrentFixedToolbar();
          if ( fixedToolbar )
              {
              fixedToolbar->SetToolbarVisibility( EFalse );
              }
          //Bitmap mode's VF consumes much of CPU in second camera, stop VF in advance to accelerate switching to photos.
          iController.StopViewFinder(); 
          }

      if ( apaTask.Exists() )
          {
          // sending APA task message
          apaTask.SendMessage( TUid::Uid( KGlxActivationCmdShowLastModified ), 
                                          buffer );
          }
      else // Photos Application is not running
          {
          StartAppCmdLineL( buffer );
          }
      CleanupStack::PopAndDestroy( &stream );
      }
      break;
      // -----------------------------------------------------
      // This should only be available when time lapse is not supported    
      // so the current mode should be still or multi
    case ECamCmdToggleMulti:
      {
      if ( iImageMode == ECamImageCaptureSingle )
        {
        TRAPD( ignore, SwitchStillCaptureModeL( ECamImageCaptureBurst, ETrue ) );
        if ( ignore )
            {
            // do nothing
            }
        }
      else if ( iImageMode == ECamImageCaptureBurst )   
        {
        SwitchStillCaptureModeL( ECamImageCaptureSingle, ETrue );
        }
      else
        {
        // Lint
        }
      
      // Update timelapse interval
      iController.SetTimeLapseInterval( 0 );
      
      // stop and start the viewfinder in order to update the settings   
      iController.ExitViewfinderMode( ECamControllerImage );
      iController.EnterViewfinderMode( ECamControllerImage );
      
      }
      break;
    // -----------------------------------------------------
    // Toggle facetracking setting
    case ECamCmdToggleFacetracking:
      {
      if ( iController.UiConfigManagerPtr()->IsFaceTrackingSupported() )
        {
        if ( iController.IntegerSettingValue( ECamSettingItemFaceTracking ) == ECamSettOn )
            {
            iController.SetIntegerSettingValueL( 
                                         ECamSettingItemFaceTracking,
                                         ECamSettOff );
            }
        else 
            {
            iController.SetIntegerSettingValueL( 
                                         ECamSettingItemFaceTracking,
                                         ECamSettOn );
            }
        iController.TryAFRequest( ECamRequestCancelAutofocus );
        }
      }
      break;
    // -----------------------------------------------------
    // Switch to single capture
    case ECamCmdSingleCapture:
      {
      SwitchStillCaptureModeL( ECamImageCaptureSingle, ETrue );
      }
      break;
    // -----------------------------------------------------
    // Activate burst mode
    case ECamCmdSequence:       
      {
      SwitchStillCaptureModeL( ECamImageCaptureBurst, ETrue );
      }
      break;
    // -----------------------------------------------------
    // Activate time lapse mode    
    case ECamCmdTimeLapse:
      {
      SwitchStillCaptureModeL( ECamImageCaptureTimeLapse, ETrue );
      }
      break;
    // -----------------------------------------------------
    case ECamCmdOpenPhoto:
      {
      if(iViewState == ECamViewStateBurstThumbnail )
        {
        iTargetViewState = ECamViewStatePostCapture;
        TrySwitchViewL();
        }
      }
      break;
    // -----------------------------------------------------
    case ECamCmdUserScene:
      {
      iTargetViewState = ECamViewStateUserSceneSetup;
      TrySwitchViewL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdPlay:   
      {
      TDataType dataType;
      SetEmbedding( ETrue );
      iDocHandler->OpenFileEmbeddedL( iController.CurrentFullFileName(),
                                      dataType );
      }
      break;
     // -----------------------------------------------------
     case ECamCmdSwitchCamera:
         {
         PRINT( _L("Camera => CCamAppUi::HandleCommandL ECamCmdSwitchCamera" ))
         //If modechange sequence going on ignore switching...
         if( iController.CaptureModeTransitionInProgress() ||
             iController.CaptureState() != ECamCaptureOff || 
             iController.IsSavingInProgress() )
             {
             PRINT( _L("Camera <= CCamAppUi::HandleCommandL STOP ECamCmdSwitchCamera" ))
             iController.SetCameraSwitchRequired( ESwitchDone );
             break;
             } 
         
         // Hide fixed toolbar and softkeys to prevent blinking
         if ( iController.IsTouchScreenSupported() )
             {         
             CAknToolbar* fixedToolbar = CurrentFixedToolbar();
             if ( fixedToolbar )
                 {
                 fixedToolbar->SetToolbarVisibility( EFalse );
                 }
             }
         CCamViewBase* precapView = static_cast<CCamViewBase*>( iView );
		 __ASSERT_DEBUG( precapView, CamPanic( ECamPanicNullPointer ));
         if ( precapView )
             {
             precapView->ViewCba()->MakeVisible( EFalse );
             }
           
         PRINT( _L("Camera <> CCamAppUi::HandleCommandL continue ECamCmdSwitchCamera" ))

         if ( ActiveCamera() == ECamActiveCameraPrimary && IsBurstEnabled() )
             {
             SwitchStillCaptureModeL( ECamImageCaptureSingle, EFalse );
             }

         TInt primaryOrientation   = EAppUiOrientationUnspecified;
         TInt secondaryOrientation = EAppUiOrientationUnspecified;
         TCamPsiKey orientation = IsQwerty2ndCamera( ETrue )?     
                                  ECamPsiPrimaryCameraOrientation:
                                  ECamPsiSecondaryCameraOrientation;
         if ( !CamUtility::GetPsiInt( ECamPsiPrimaryCameraOrientation, 
                                      primaryOrientation ) &&
              !CamUtility::GetPsiInt( orientation, 
                                      secondaryOrientation ) )  
             {
             if ( primaryOrientation != secondaryOrientation || IsQwerty2ndCamera() )
                 {
                 if( ( !IsSecondCameraEnabled() || 
                       iController.CameraSwitchQueued() == ESwitchSecondaryOrientation ) &&
                     !IsQwerty2ndCamera() )  
                     {
                     PRINT( _L("Camera => CCamAppUi::HandleCommandL ECamCmdSwitchCamera 2ndary portrait" ))
                     iCamOrientation = ECamOrientationPortrait;
                     SetOrientationL( static_cast<TAppUiOrientation>( secondaryOrientation ) );

                     if ( iController.IsTouchScreenSupported() )
                         {
                         iStillCaptureView->CreateAndSetToolbarL( R_CAM_STILL_PRECAPTURE_TOOLBAR_PORTRAIT );
                         iVideoCaptureView->CreateAndSetToolbarL( R_CAM_VIDEO_PRECAPTURE_TOOLBAR_PORTRAIT );
                         }

                     StatusPane()->MakeVisible( ETrue );
                     }
                 else
                     {
                     PRINT( _L("Camera => CCamAppUi::HandleCommandL ECamCmdSwitchCamera 1ary or 2nd landscape" ))
                     iCamOrientation = (iSoftkeyPosition == ECamHandLeft)
                     ? ECamOrientationCamcorderLeft
                         : ECamOrientationCamcorder;

                     SetOrientationL( 
                        static_cast<TAppUiOrientation>( primaryOrientation ) );

                     if ( iController.IsTouchScreenSupported() )
                         {
                         if ( ( !IsSecondCameraEnabled() || iController.CameraSwitchQueued() == ESwitchSecondaryOrientation ) &&
                               IsQwerty2ndCamera() )   
                            {
                            iStillCaptureView->CreateAndSetToolbarL( 
                                    R_CAM_STILL_PRECAPTURE_TOOLBAR_LANDSCAPE );
                            iVideoCaptureView->CreateAndSetToolbarL( 
                                    R_CAM_VIDEO_PRECAPTURE_TOOLBAR_LANDSCAPE );
                            }
                         else
                            { 
                            iStillCaptureView->CreateAndSetToolbarL( 
                                    R_CAM_STILL_PRECAPTURE_TOOLBAR );
                            if(iController.UiConfigManagerPtr()->IsXenonFlashSupported())
                                {
                                iVideoCaptureView->CreateAndSetToolbarL(R_CAM_VIDEO_PRECAPTURE_TOOLBAR);
                                iVideoCaptureView->UpdateToolbarIconsL();
                                }
                            else
                                {
                                iVideoCaptureView->CreateAndSetToolbarL(R_CAM_VIDEO_PRECAPTURE_TOOLBAR_VIDEOLIGHT);
                                }
                            }
                         }

                     StatusPane()->MakeVisible( EFalse );
                     }
                 UpdateNaviModelsL();

                 //Reset zoom when changing cameras
                 ZoomPane()->ResetZoomTo1x(); 

                 TInt rl( 0 );
                 for ( rl = 0; rl < iResourceLoaders.Count(); rl++ )
                     {
                     iResourceLoaders[rl]->ReloadResourceDataL();
                     }

                 iCamSidePane->UpdateLayout();          
                 iTargetViewState = ECamViewStatePreCapture;
                 delete iScreenClearer;
                 iScreenClearer = NULL;
                 iScreenClearer = CAknLocalScreenClearer::NewL( 
                              IsSecondCameraEnabled() || IsQwerty2ndCamera() );
                 TrySwitchViewL( ETrue );
                 iController.SwitchCameraL();       
                 }
             }
         }                
         break;
    // -----------------------------------------------------
    case EAknCmdHelp:
      {
      LaunchHelpL();
      }
      break;
    // -----------------------------------------------------
    case ECamCmdRedrawScreen:
      {
      if( !iProcessingScreenRedraw && iView )
        {
        iProcessingScreenRedraw = ETrue;
        iView->HandleCommandL(ECamCmdRedrawScreen);
        iProcessingScreenRedraw = EFalse;
        }
      }
      break;
    // -----------------------------------------------------
    case ECamCmdShootSetup: // fall through
    case ECamCmdFlash:      // fall through
    case ECamCmdAddAudio:   // fall through
    case ECamCmdEdit:       // fall through
    case ECamCmdPrint:      // fall through
    case ECamCmdSendToCallerMultimedia: // fall through
    case ECamCmdRenameImage:            // fall through
    case ECamCmdRenameVideo:            // fall through        
      {
      User::Leave( KErrNotSupported );
      }
      break;

    // -----------------------------------------------------
    case ECamCmdViewfinderGrid:
      {
      SelectViewFinderGridSettingL();
      break;     
      }

    // -----------------------------------------------------
    case EAknCmdTaskSwapper:
    	/*
    	* MSK: Active Applications
    	* This command is actually handled by Avkon FW
    	* Applications should not handle this
    	*/
    	break;
    //-------------------------------------------------------
    default:
      {
      PRINT( _L("Camera <> CCamAppUi::HandleCommandL does nothing") )
      } 
      break;
    }
    // -----------------------------------------------------
    PRINT( _L("Camera <= CCamAppUi::HandleCommandL") )
  } 

// ===========================================================================
// From MCamCameraObserver

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//

void
CCamAppUi::HandleCameraEventL( TInt              /*aStatus*/, 
                               TCamCameraEventId  aEventId, 
                               TAny*             /*aEventData //= NULL */
                             )
  {
  switch( aEventId )
    {        
    // -----------------------------------------------------
    case ECamCameraEventVfStart: // ECamEventViewFinderStarted
      {
      PRINT( _L( "Camera <> CCamAppUi::HandleCameraEventL ECamCameraEventVfStart" ) )
      if( iScreenClearer )
        {
        delete iScreenClearer;
        iScreenClearer = NULL;
        }
#ifdef CAMERAAPP_DELAYED_POSTCAPTURE_CREATION
        
      // Construct postcapture views when idle  
      // or latest when view switch is requested
      StartPostCapIdleL();
#else               
      TRAPD( err, ConstructPostCaptureViewsL() )
      if ( KErrNone != err )
        {
        TRAP_IGNORE( HandleCameraErrorL( err ) );

        // if embedded need to notify embeddee
        if ( iEikonEnv->StartedAsServerApp() )
          {
          iEmbeddedObserver->AbortL();
          }
        else
          {
          CloseAppL();  
          }
        }
#endif            
      // Only show the Active Palette once viewfinding has started
      iPreventActivePaletteDisplay = EFalse;     

      // For forced focus scenes, do one autofocus request once vf starts if needed
      if( iController.UiConfigManagerPtr()->IsAutoFocusSupported() )
        {
        if( iController.CurrentSceneHasForcedFocus() ) 
            {
            PRINT( _L( "Camera <> CCamAppUi::HandleCameraEventL, TryAFRequest( ECamRequestCancelAutofocus )") );        
            //Only autofocuscancel will set AFrange which is needed for forced focus cases
            iController.TryAFRequest( ECamRequestCancelAutofocus );
            }
        else // For scenes with reticule cancel the autofocus, which sets the focus correctly
            {
            if ( iController.UiConfigManagerPtr()->IsAutofocusSetInHyperfocalAtStartup() )
                {
                PRINT( _L( "Camera <> CCamAppUi::HandleCameraEventL, TryAFRequest( ECamRequestCancelAutofocus )") );
                iController.TryAFRequest( ECamRequestCancelAutofocus );
                }
            else if ( !iFirstVFStart )
                {
                iController.TryAFRequest( ECamRequestCancelAutofocus );
                }
            }
        }
      iFirstVFStart = EFalse;
      
      break;              
      }
      // -----------------------------------------------------
      case ECamCameraEventVideoAsyncStop: // Audio and video streams are stopped
        {
        PRINT( _L( "Camera <> CCamAppUi::HandleCameraEventL ECamCameraEventVideoAsyncStop" ) )
        // Stop View finder if not needed to left run
        if ( iController.IntegerSettingValue( ECamSettingItemVideoShowCapturedVideo ) == ECamSettOn ) 
          {
          iController.StopViewFinderEcam();
          }
        break;              
        }
    // -----------------------------------------------------        
    case ECamCameraEventPowerOnRequested:
    case ECamCameraEventReserveRequested:
      {
      // event only send if UIOrientationOverride feature is supported,
      // in that case UI construction is divided into two parts, call here 
      // to complete the construction	if in first startup
      if( iFirstBoot )
          {	
          PRINT( _L( "Camera <> CCamAppUi::HandleCameraEventL ECamCameraEventReserveRequested/ECamCameraEventPowerOnRequested" ) )	
          if ( iWaitTimer->IsActive() )
              {
              PRINT( _L( "Camera <> timer already active" ) )
              iWaitTimer->Cancel();
              }
          PRINT( _L( "Camera <> start the appui construct timer" ) )
          iWaitTimer->Start( 0, 0,  TCallBack( AppUIConstructCallbackL, this ) );  
          }
      else
          {
          //load settings in case they were changed via GS	
          iController.LoadStaticSettingsL( IsEmbedded() );
          // and check the availability of the memory to be used
          iController.CheckMemoryToUseL();
          }    
      break;  
      }
    default:
      {
      break;  
      }  
      
    }    

  }
// ===========================================================================

// -----------------------------------------------------------------------------
// CCamAppUi::HandleControllerEventL
// Handle controller events
// -----------------------------------------------------------------------------
//
void 
CCamAppUi::HandleControllerEventL( TCamControllerEvent aEvent, 
                                   TInt                aError )
  {
  PRINT1( _L( "Camera => CCamAppUi::HandleControllerEventL() %d" ), aEvent ); 
  switch ( aEvent )
    {
    // -----------------------------------------------------
    case ECamEventExitRequested:
      {
      PRINT( _L( "Camera <> case ECamEventExitRequested" ) )
      if ( SettingsLaunchedFromCamera() )
        {
        SettingsPluginExitedL( ECameraCompleteExit ); 
        }
      if ( iWaitTimer->IsActive() )
        {
        PRINT( _L( "Camera <> timer already active" ) )
        iWaitTimer->Cancel();
        }
      PRINT( _L( "Camera <> start the exit timer" ) )
      iWaitTimer->Start( 0, 0,  TCallBack( CallExit, this ) );  
      break;
      }
    // -----------------------------------------------------
    case ECamEventOperationStateChanged:
      {
      PRINT( _L("Camera <> case ECamEventOperationStateChanged") )

      switch ( iController.CurrentOperation() )
        {
        case ECamFocusing:
          UpdateCba();
          break;
        case ECamCapturing:
          iPreventActivePaletteDisplay = ETrue;
          SetActivePaletteVisibility( EFalse );
          //Image capture has started. If postcaptureview is enabled assume that postcapture
          //is next view. Thus if camera goes to background and comes back to foreground don't
          //start viewfinder because it is not needed in postcapture view. If image capture fails
          //or is canceled enabling viewfinder is possible.
          if ( iView && ( iStillCaptureView == iView || iVideoCaptureView == iView )
                  && ShowPostCaptureView() )
              {
              SetAssumePostCaptureView( ETrue );
              }
          break;
        case ECamStandby:
          //DismissMemoryNote();
          HandleStandbyEventL( aError );
          break;         
        case ECamNoOperation:
            {
            if( iPreventActivePaletteDisplay )
                {
                iPreventActivePaletteDisplay = EFalse;
                }            
            }
            break;
        default:
          break;
        }
      break;
      }
    // -----------------------------------------------------
    case ECamEventRecordComplete:
      {
      PRINT( _L("Camera <> case ECamEventRecordComplete") )
      // Dismiss the saving video clip wait dialog
      if ( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();
        //iWaitDialog = NULL; //ProcessFinishedL() will make iWaitDialog NULL asynchronously
        }

      // switch on active palette recording is complete
      if( ECamOrientationCamcorder     == iCamOrientation 
       || ECamOrientationCamcorderLeft == iCamOrientation 
       || ECamOrientationPortrait      == iCamOrientation )
        {
        iPreventActivePaletteDisplay = EFalse;
        if ( DrawPreCaptureCourtesyUI() )
          {
          SetActivePaletteVisibility( ETrue );
          }
        } 
      if ( iController.InCallOrRinging() &&
           iController.IntegerSettingValue( ECamSettingItemVideoShowCapturedVideo ) == ECamSettOn )
        {
        // when video recording is stopped by incoming call
        // and post capture view is turned on, post capture timer
        // should only start after some user interaction
        iDelayVideoPostCaptureTimeout = ETrue;
        }
      }            //lint -fallthrough to switch to post capture view
    // -----------------------------------------------------
    case ECamEventSnapshotReady:
      {
      PRINT( _L("Camera <> case ECamEventSnapshotReady") )

      // If burst mode is active, do nothing until the whole burst completes
      if ( IsBurstEnabled() )
        {
        // handle error if necessary
        HandleCameraErrorL( aError );
          
        // Check if there was an error, and there were no successful
        // captures.  In this case, we will be staying in pre-capture view
        // so allow the active palette to be displayed again
        if ( aError != KErrNone &&
             iController.CurrentItemCount() == 0 )
          {
          iPreventActivePaletteDisplay = EFalse;
          SetActivePaletteVisibility( ETrue );
          }

        break;
        }
      // if the MMC has been removed while recording video to it, 
      // don't go to post-capture view
      else if ( iMode == ECamControllerVideo
                && iController.IntegerSettingValueUnfiltered( ECamSettingItemVideoMediaStorage ) 
                    == ECamMediaStorageCard 
                && ( aError != KErrNone && aError != KErrDiskFull )
                && CamUtility::MemoryCardStatus() != ECamMemoryCardInserted )
        {
        iTargetViewState = ECamViewStatePreCapture;
        }
      // if USB was connected while MMC or mass storage was used as storage, 
      // don't go to postcapture
      else if ( CamUtility::IsUsbActive() && 
                ( iController.CurrentStorage() == ECamMediaStorageCard ||
                iController.CurrentStorage() == ECamMediaStorageMassStorage ) )
        {
        iTargetViewState = ECamViewStatePreCapture;
        }
      else if ( iController.IsCaptureStoppedForUsb() && 
                ( iController.CurrentStorage() == ECamMediaStorageCard ||
                iController.CurrentStorage() == ECamMediaStorageMassStorage ) )
        {
        PRINT( _L("Camera <> HandleControllerEventL ECamEventSnapshotReady: video stopped for usb" ) );
        iTargetViewState = ECamViewStatePreCapture;
        }
      // If there was an error (we have no snap), we must stay in pre-capture view
      else if ( aError )
        {
        iPreventActivePaletteDisplay = EFalse;
        SetActivePaletteVisibility( ETrue );
        iController.SetCaptureKeyPressedWhileImageSaving(EFalse);
        // To be able to continue viewfinding
        iController.FreezeViewFinder( EFalse );

        iTargetViewState = ECamViewStatePreCapture;
        }               
      // If the user has the show captured image setting turned on.
      // switch to post capture view (Except in burst mode)            
      else if ( ShowPostCaptureView() )
          {
          if( iController.CaptureKeyPressedWhileImageSaving() )
              {
              PRINT( _L("Camera <> HandleControllerEventL ECamEventSnapshotReady - Capture key pressed while saving ") )
              iController.SetCaptureKeyPressedWhileImageSaving(EFalse);
              iTargetViewState = ECamViewStatePreCapture;
              SetSuppressUIRiseOnViewfinderStart( ETrue );
              // To be able to continue viewfinding
              iController.FreezeViewFinder( EFalse );
              }
          else
              {
              if ( !CamUtility::IsBatteryPowerEmpty() )
                { 
                if ( iLensCoverExit )
                  {
                  PRINT (_L ("Camera <> set target to precapture if Battery not Empty") );
                  iTargetViewState = ECamViewStatePreCapture;
                  }
                else
                  {
                  PRINT (_L ("Camera <> set target to postcapture if Battery not Empty") );
                  iTargetViewState = ECamViewStatePostCapture;
				  
                  //We need to blank the precapture view CBA here to prevent
                  //blinking when changing back to precapture from postcapture
                  UpdateCba();
                  }
                }
              }
          }
      
      // Always switch to post capture view if self timer is enabled
      else if ( iInSelfTimerMode != ECamSelfTimerDisabled )
          {
          iTargetViewState = ECamViewStatePostCapture;
          // ensure that the view doesn't switch to precapture view when
          // ECamEventCaptureComplete is received
          iSelfTimedCapture = ETrue;
          }
      // Otherwise, 
      // if not showing captured images go straight to pre capture view 
      // (Except in burst mode)
      else               
          {
          iTargetViewState = ECamViewStatePreCapture;
          SetSuppressUIRiseOnViewfinderStart( ETrue );
          }
          
      // The app is not going to try to switch to post capture view at all
      // so the array needs to be released now.
      if ( iTargetViewState == ECamViewStatePreCapture )
          {
          iController.ReleaseArray();
          }
          
      // If no view switch is needed make sure the cba is not disabled
      // and the self timer is switched off
      if ( ViewSwitchRequired() != ECamViewSwitch )
          {                
          if ( iInSelfTimerMode != ECamSelfTimerDisabled )
              {
              SelfTimerEnableL( ECamSelfTimerDisabled );
              }
          UpdateCba();
          }
      TrySwitchViewL();

      break;
      }
    // -----------------------------------------------------
    case ECamEventCaptureComplete:
      {
      PRINT( _L("Camera <> case ECamEventCaptureComplete") )
      // check if exit is required
      if ( iController.CheckExitStatus() )
        {
        PRINT( _L("Camera <> calling internal exit after checking exit status") )
        // The exit event is replacing the view switch, 
        // so  release the array now
        iController.ReleaseArray();
        InternalExitL();
        return;
        }
      if( ECamControllerImage == iMode )
        {
        switch( iImageMode )
          {
          // -----------------------------
          // If burst mode is active, 
          // only switch view when the whole burst completes
          case ECamImageCaptureBurst:
            {
            // If there are images to show (i.e. user has not cancelled), 
            // switch to thumbnail view. "Show captured images" setting has no
            // effect in burst mode
            
            if ( iController.CurrentItemCount() != 0 && !iController.IsCaptureStoppedForUsb() )
              {
              iTargetViewState = ECamViewStateBurstThumbnail;
              
              // Normally iPreventActivePaletteDisplay is set to EFalse when
              // postcture AP is shown, but in burst case that doesn't
              // necessarily happen so do this here.
              iPreventActivePaletteDisplay = EFalse;
              }
            // Otherwise, no images to show. Go straight to pre capture view. 
            else               
              {
              iTargetViewState = ECamViewStatePreCapture;
              // The app is not going to try to switch to post capture view at 
              // all. So the array needs to be released now.
              iController.ReleaseArray();
              iController.StopViewFinder(); 
              iController.StartViewFinder();
              }
            // If no view switch is needed make sure the cba is not disabled
            // and the self timer is switched off
            if ( ViewSwitchRequired() != ECamViewSwitch )
              {
              if ( iInSelfTimerMode != ECamSelfTimerDisabled )
                {
                SelfTimerEnableL( ECamSelfTimerDisabled );
                }
                UpdateCba();
              }
    
            TrySwitchViewL();
            break;
            }
          // -----------------------------
          case ECamImageCaptureTimeLapse:
            {
            iTargetViewState = ECamViewStatePostCapture;
            TrySwitchViewL();
            break;
            }      
          // -----------------------------
          case ECamImageCaptureSingle:
            {
            PRINT( _L("Camera <> case ECamImageCaptureSingle") )
            if( aError != KErrNone )
              {
              PRINT( _L("Camera <> error received") )
              // handle error if necessary
              HandleCameraErrorL( aError );
              
              iPreventActivePaletteDisplay = EFalse;
              SetActivePaletteVisibility( ETrue );   
                    
   			  if (iCamOrientation == ECamOrientationPortrait) 
   			   {
    		   iTargetViewState = ECamViewStatePrePortraitCapture;
   		       }
  			  else
     		   {
    		   iTargetViewState = ECamViewStatePreCapture;   	
    		   }
            TrySwitchViewL();
              }       
            else
              {
              PRINT( _L("Camera <> no errors in") )
              if ( !ShowPostCaptureView() && 
                   iInSelfTimerMode == ECamSelfTimerDisabled && 
                   !IsSelfTimedCapture() )
                {
                PRINT( _L("Camera <> update cba") )
                UpdateCba();
				if( ActiveCamera() != ECamActiveCameraPrimary )
					{
	                PRINT( _L("Camera <> release array") )
	                iController.ReleaseArray();
					}
                iPreventActivePaletteDisplay = EFalse;
                SetActivePaletteVisibility( ETrue );   
                if ( iController.IsTouchScreenSupported() )
                    {
                    CAknToolbar* fixedToolbar = CurrentFixedToolbar();
                    if ( fixedToolbar )
                        {
                        fixedToolbar->SetToolbarVisibility( ETrue );
                        }
                    }
                HandleCommandL( ECamCmdRaiseCourtesyUI );
                if ( IsSecondCameraEnabled() || IsEmbedded() )
                    {
                    //For secondary or embedded camera the viewfinder
                    //was stopped. Restart it now.
                    iController.StartViewFinder(); 
                    }
                }
              else if ( !ShowPostCaptureView() && 
                       iInSelfTimerMode != ECamSelfTimerDisabled )
                {
                SelfTimerEnableL( ECamSelfTimerDisabled );
                }
              }
            break;
            }
          // -----------------------------
          default:
            {
            // do nothing
            break;
            }
          // -----------------------------
          }
        }
            // reset the selftimed capture flag for next capture
            PRINT( _L("resetting iSelfTimedCapture") )
            iSelfTimedCapture = EFalse;
      break;
      }     
    // -----------------------------
    case ECamEventControllerReady: 
        {
        if ( iView )
            {
            if ( iFirstBoot && IsSecondCameraEnabled() 
                            && IsQwerty2ndCamera()
                            && CurrentViewState() != ECamViewStateStandby )
                {
                PRINT( _L("Camera <> Booting directly to the secondary camera") )  
                // Main camera settings are loades as default. 
                // Need to be updated to secondary
                iStillCaptureView->CreateAndSetToolbarL( 
                                    R_CAM_STILL_PRECAPTURE_TOOLBAR_LANDSCAPE );
                iVideoCaptureView->CreateAndSetToolbarL( 
                                    R_CAM_VIDEO_PRECAPTURE_TOOLBAR_LANDSCAPE );
                // Add toolbar observer. Needed in first boot to secondary
                SetPreCaptureMode( ECamPreCapViewfinder ); 
                }
            iFirstBoot = EFalse;
            iView->HandleCommandL( ECamCmdPopUpMenuZoom );
            }
        //Remaining recording time counter update needed when recording has stopped and
        //show last captured video setting is off.
        if( iNaviPaneCounterControl && !ShowPostCaptureView() && iMode == ECamControllerVideo )
            {
            iNaviPaneCounterControl->ForceNaviPaneUpdate();
            }        
        break;
        }      
    // -----------------------------        
    // -----------------------------------------------------
    // The MMC is removed
    case ECamEventMemoryCardHotswap:
        {
        TBool mmcInUse = EFalse;

        TCamMediaStorage currentLocation = 
            static_cast<TCamMediaStorage>( 
            iController.IntegerSettingValueUnfiltered( 
                                          ECamSettingItemVideoMediaStorage ) );

        // if app is in back show MMC note on returning to foreground
        PRINT( _L("Camera => CCamAppUi::HandleControllerEventL ECamEventMMCHotswap") )
        if ( currentLocation == ECamMediaStorageCard )
            {
            if ( AppInBackground( ETrue ) && iReturnFromBackground )
                {
                PRINT( _L("Camera => CCamAppUi::HandleControllerEventL ECamEventMMCHotswap bckgrn") )
                //iMMCRemoveNoteRequired = ETrue;
  
                // Force use of phone memory.
                // with multiple drive support, 
                // this actually uses the internal mass memory
               iController.UsePhoneMemoryL();
               }
        else // show note on next capture attempt
            {
            PRINT( _L("Camera => CCamAppUi::HandleControllerEventL ECamEventMMCHotswap not bckgrnd") )
            iMMCRemoveNoteRequiredOnCapture = ETrue;

            // force update of navi pane
            iNaviPaneCounterControl->ForceNaviPaneUpdate();
            if ( IsDirectViewfinderActive() )
                {
                TRAP_IGNORE( HandleCommandL( ECamCmdRedrawScreen ) );
                }
            }
        }

        // if the current mode is video, and the storage location is MMC
        if ( ECamControllerVideo == iMode )
            {
            mmcInUse = (ECamMediaStorageCard == currentLocation);
            }
           // if the current mode is still, and the storage location is MMC
        else        
            {
            mmcInUse = ( ECamMediaStorageCard == 
                       iController.IntegerSettingValueUnfiltered( 
                           ECamSettingItemPhotoMediaStorage ) );
            }

        // If a post capture view is active, and the current files
        // are saved to the MMC, switch to pre capture view
        if ( mmcInUse
             && ( ECamViewStateBurstThumbnail == iViewState
             || ECamViewStatePostCapture    == iViewState )
           )
            {
            if (iCamOrientation == ECamOrientationPortrait) 
                {
                iTargetViewState = ECamViewStatePrePortraitCapture;
                }
            else
                {
                iTargetViewState = ECamViewStatePreCapture;   	
                }
            TrySwitchViewL();
            }
        break;            
        }
    // -----------------------------------------------------
    case ECamEventCounterUpdated:
        break;
    // -----------------------------------------------------
    case ECamEventVideoPauseTimeout:
        {
        PRINT( _L( "Camera <> case ECamEventVideoPauseTimeout" ) )
        EndCapture();
        break;
        }
    // -----------------------------------------------------
    case ECamEventSaveLocationChanged:
        {
        PRINT( _L( "Camera <> case ECamEventSaveLocationChanged" ) )
        if ( IsMMCRemovedNotePending() && 
           ( CamUtility::MemoryCardStatus() != ECamMemoryCardNotInserted ) )
            {
            iMMCRemoveNoteRequired = EFalse;
            iMMCRemoveNoteRequiredOnCapture = EFalse;
            }
        if ( IsDirectViewfinderActive() )
            {
            TRAP_IGNORE( HandleCommandL( ECamCmdRedrawScreen ) );
            }
        break;
        }   

    // -----------------------------------------------------
    case ECamEventSaveComplete:
        {
        PRINT( _L( "Camera <> case ECamEventSaveComplete" ) )
        if ( ECamImageCaptureSingle == iImageMode )
            {
            if( iController.CaptureKeyPressedWhileImageSaving() && 
                ShowPostCaptureView())
                {
                PRINT( _L("Camera <> HandleControllerEventL ECamEventSaveComplete - Capture key pressed while saving ") )
                iController.SetCaptureKeyPressedWhileImageSaving(EFalse);
                if (iCamOrientation == ECamOrientationPortrait)  
                    {
                    iTargetViewState = ECamViewStatePrePortraitCapture;
                    }
                else
                    {
                    iTargetViewState = ECamViewStatePreCapture;   	
                    }
               TrySwitchViewL();
               }
            }
        break;
        }
    // -----------------------------------------------------
    case ECamEventMediaFileChanged:
        {
 
        if ( iController.UiConfigManagerPtr() && 
             !iController.UiConfigManagerPtr()->IsLocationSupported() )
            {
            PRINT2( _L( "Camera <> CCamAppU, ECamEventMediaFileChanged, mode [%s], targetmode[%s]" ), 
                KCamModeNames[ iController.CurrentMode() ], 
                KCamModeNames[ iController.TargetMode() ] )
    	  
            if( iEndKeyPressed )
                {
                if ( iController.CurrentMode() == ECamControllerShutdown || 
                     iController.IsInShutdownMode() )
                    {
                    PRINT( _L( "Camera <> ECamEventMediaFileChanged, notify shutdown" ) )
                    HandleControllerEventL( ECamEventExitRequested, KErrNone );
                    }
                }
            }
 
      if ( iController.UiConfigManagerPtr() && iController.UiConfigManagerPtr()->IsLocationSupported() )
          {
          // If record location setting is on, stop location trail
          if ( ECamViewStatePreCapture != iViewState && ECamLocationOn == iController.IntegerSettingValue( ECamSettingItemRecLocation ) )
          	  {
          	  PRINT( _L("Camera: Gallery notified and non pre-capture view, stop location trail") )
          	  iController.StopLocationTrail();
          	  }
          else
          	  {
              PRINT1( _L("Camera: location trail not in use or view state (%d) is precapture"), iViewState )
          	  }
          }

        break;
        }
    // -----------------------------------------------------
    case ECamEventCameraChanged:
        {
        PRINT( _L("Camera <> case ECamEventCameraChanged"))
        // exit standby and use the new camera
        if( iView)
            {
            iView->HandleCommandL( ECamCmdExitStandby );
            }
        break;
        }
    // -----------------------------------------------------
    case ECamEventInvalidMemoryCard:
        {
        PRINT( _L( "Camera <> case ECamEventInvalidMemoryCard" ) ); 
        ShowErrorNoteL( ECamMemoryStatusCardNotInserted );
        break;
        }
    // -----------------------------------------------------
    case ECamEventRestoreCameraSettings:
        {
        PRINT( _L( "Camera <> case ECamEventRestoreCameraSettings" ) ); 
        if ( SettingsLaunchedFromCamera() )
            {
            iSettingsPluginLaunched = EFalse;
            iReturnedFromPlugin = ETrue;
            } 
        iTargetViewState = ECamViewStatePreCapture;
          
        // get start up mode from settings
        if ( StartupMode() == ECamStillCapture )
            {
            iMode       = ECamControllerImage;
            iTargetMode = ECamControllerImage;
            SetDefaultViewL( *iStillCaptureView );
            }
        else
            {
            iMode       = ECamControllerVideo;
            iTargetMode = ECamControllerVideo;
            SetDefaultViewL( *iVideoCaptureView );
            }
            
        TrySwitchViewL();
        // After restoring settings, still capture view should be active 
        // -> Update all toolbar icons for still-capture view. 
        iStillCaptureView->UpdateToolbarIconsL();
        break;
        }
    // -----------------------------------------------------
    case ECamEventCallStateChanged:
        SelfTimerEnableL( ECamSelfTimerDisabled );
        break;
    // -----------------------------------------------------
    default:
        break;
    // -----------------------------------------------------
    }
    PRINT( _L( "Camera: <= CCamAppUi::HandleControllerEventL()" ) );         
  }

// -----------------------------------------------------------------------------
// HandleStandbyEventL
// -----------------------------------------------------------------------------
//
void
CCamAppUi::HandleStandbyEventL( TInt aStatus )
  {
  PRINT( _L( "Camera => CCamAppUi::HandleStandbyEventL") );
  if( !iPretendExit &&
      !AppInBackground( EFalse )  )
    {
    if( ECamViewStateStandby != iViewState )
      {
      DismissMemoryNoteL();  
    
      if( IsInternalView( iViewState ) )
        {
         
        if ( aStatus != KErrInUse &&
             iPreCaptureMode == ECamPreCapCaptureSetup ||
             iPreCaptureMode == ECamPreCapSceneSetting )
            {
            // VF settings does not go to standby state, VF is stopped instead
            // Screen saver releases resources
            iViewFinderStopped = ETrue;
            PRINT( _L( "Camera <> CCamAppUi::HandleStandbyEventL: in one of settings view. Release camera") );
            iController.ReleaseCamera();
            //Draw black viewfinder area. Because camera is in internal viewstate view is valid.
            static_cast<CCamViewBase*>( iView )->HandleCommandL( ECamCmdRedrawScreen );
            }         
        else 
            {
            PRINT( _L( "Camera <> CCamAppUi::HandleStandbyEventL: call iView->ExitAllModesL") );
            PRINT1( _L( "Camera <> CCamAppUi::HandleStandbyEventL: iView = 0x%x"), iView );
            // Exit any special modes.
            // If AppUI construction is not finished, do it first, otherwise
            // the view has not been created yet
            if ( iController.UiConfigManagerPtr() && 
                 iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() &&
                 !iUiConstructionComplete )
              {
              CompleteAppUIConstructionL(); 
              }
            else
              {    
              static_cast<CCamViewBase*>( iView )->ExitAllModesL();
              }
             
            }
        }
      // Not InternalView
      else 
        {
        PRINT( _L( "Camera <= CCamAppUi::HandleStandbyEventL: Not internalView, standby ignored, return") );
        return;
        }
      
      // VF settings does not go to standby state, VF is stopped instead
      if ( aStatus == KErrInUse || iPreCaptureMode != ECamPreCapCaptureSetup &&
           iPreCaptureMode != ECamPreCapSceneSetting )  
           {
           PRINT( _L( "Camera <> CCamAppUi::HandleStandbyEventL: Set standby status..") );
           SetStandbyStatus( aStatus );
           if ( iController.InVideocallOrRinging() && iScreenClearer )
             {
             delete iScreenClearer;
             iScreenClearer = NULL;
             }
           
           PRINT( _L( "Camera <> CCamAppUi::HandleStandbyEventL: Send command ECamCmdGoToStandby..") );
           HandleCommandL( ECamCmdGoToStandby );
           }
       }
    else
      {
      PRINT( _L( "Camera <> CCamAppUi::HandleStandbyEventL: Already in standby, ignored!") );
      }
    }
  else
    {
    PRINT( _L( "Camera <> CCamAppUi::HandleStandbyEventL: Camera already in background, no action..") );    
    }
  PRINT( _L( "Camera <= CCamAppUi::HandleStandbyEventL") );
  } 

// -----------------------------------------------------------------------------
// SetStandbyStatus
// -----------------------------------------------------------------------------
//
void CCamAppUi::SetStandbyStatus( TInt aStatus )
  {
  PRINT1( _L( "Camera =><= CCamAppUi::SetStandbyStatus( %d )"), aStatus );
  iStandbyStatus = aStatus;
  }

// -----------------------------------------------------------------------------
// StandbyStatus
// -----------------------------------------------------------------------------
//
TInt CCamAppUi::StandbyStatus() const
  {
  return iStandbyStatus;
  }

// ---------------------------------------------------------
// CCamAppUi::IsEmbedded
// Retrun whether we are embedded or not
// ---------------------------------------------------------
//
TBool CCamAppUi::IsEmbedded() const
  {
  PRINT( _L("Camera => CCamAppUi::IsEmbedded") );
/*#ifndef __WINS__
  PRINT1( _L("CCamAppUi::IsEmbedded %d"), iSendFileInCall );
  // treat inCallSend mode as same as embedded
  if( iSendFileInCall )
    {
    PRINT( _L("CCamAppUi::IsEmbedded iCall" ) );
    return ETrue;
    }
#endif*/
  PRINT( _L("Camera <= CCamAppUi::IsEmbedded") );
  return iEikonEnv->StartedAsServerApp();
  }


// ---------------------------------------------------------
// ---------------------------------------------------------
// CCamAppUi::HandleWsEventL
// Handle a window server event. Used to handle focus loss
// ---------------------------------------------------------
//
void 
CCamAppUi::HandleWsEventL( const TWsEvent&    aEvent,
                                 CCoeControl* aDestination )
  {
    
  TInt type = aEvent.Type();
  
  PRINT1( _L("Camera => CCamAppUi::HandleWsEventL (type: %d)"), type )
  // In case we receive an enter key event, we should map it to MSK
  if ( aEvent.Type() == EEventKey && aEvent.Key()->iRepeats == 0 && 
       aEvent.Key()->iScanCode == EStdKeyEnter &&
       !( iMode == ECamControllerVideo && iViewState == ECamViewStatePreCapture && iController.IsDemandKeyRelease() ) ) 
    {
    PRINT( _L("Camera <> CCamAppUi::HandleWsEventL: mapping enter to MSK") );
    // Gets the window group id of the app in foreground
    TInt windowGroupId = iCoeEnv->WsSession().GetFocusWindowGroup();
    // if the foreground app is this app
    if ( windowGroupId == iMyWgId ) 
        {
        // enter key should be mapped to MSK.
        TWsEvent  ownEvent = aEvent;
        ownEvent.Key()->iScanCode = EStdKeyDevice3;
        ownEvent.Key()->iCode = EKeyOK;
        iCoeEnv->WsSession().SendEventToWindowGroup(windowGroupId, ownEvent);
        }
    }
  // in case we receive a long press of applications key when the self timer is 
  // enabled, we should cancel the self timer
  else if ( aEvent.Type() == EEventKey &&
            aEvent.Key()->iScanCode == EStdKeyApplication0 &&
            aEvent.Key()->iRepeats > 0 && 
            SelfTimerEnabled() )
        {
        SelfTimerEnableL( ECamSelfTimerDisabled );
        }
  // in other cases do the normal handling
  else 
    {

      switch ( type )
        { 
        // a new window group has the focus
        case EEventFocusGroupChanged:
          {
          PRINT( _L("Camera <> CCamAppUi::HandleWsEventL: case EEventFocusGroupChanged") );
                
          // When camera looses its focus after going to background
          // during the state of ViaPlayer play back mode
          // we dont perform any operation; but return directly      
          if ( iViewState == ECamViewStateViaPlayer )
            {
            // We call the base class and return
            CAknAppUi::HandleWsEventL( aEvent, aDestination );
            return;
            }
                
          // When camera looses its focus after going to background
          // we return to the same settings page that was before       
          if ( SettingsLaunchedFromCamera() )
          	{
          	// We call the base class and return
          	CAknAppUi::HandleWsEventL( aEvent, aDestination );
          	return;
          	}
          // we are going to background no need for harvester callbacks.
          iController.DeRegisterHarverterClientEvents();
    
          if ( AppInBackground( ETrue ) && iPretendExit )
            {
            //Make the current container invisible to prevent windowserver from bringing it to front
            //before the foreground event.
            TVwsViewId currentViewId;
            GetActiveViewId( currentViewId );
            switch ( currentViewId.iViewUid.iUid )
                {
                case ECamViewIdStillPreCapture:
                    {
                    if( iStillCaptureView->Container() && 
                        !APHandler()->AccessedViaAP() )
                        {
                        iStillCaptureView->Container()->MakeVisible( EFalse );
                        }
                    iLastHiddenViewId = currentViewId.iViewUid;
                    break;
                    }
                case ECamViewIdStillPostCapture:
                    {
                    CCamStillPostCaptureView* stillPostCaptureView = 
                    ((CCamStillPostCaptureView*)View( currentViewId.iViewUid ));
                    if( stillPostCaptureView && stillPostCaptureView->Container() )
                        {
                        stillPostCaptureView->Container()->MakeVisible( EFalse );
                        }
                    iLastHiddenViewId = currentViewId.iViewUid;
                    break;
                    }
                case ECamViewIdVideoPreCapture:
                    {
                    if( iVideoCaptureView->Container() && 
                        !APHandler()->AccessedViaAP() )
                        {
                        iVideoCaptureView->Container()->MakeVisible( EFalse );
                        }
                    iLastHiddenViewId = currentViewId.iViewUid;
                    break;
                    }
                case ECamViewIdVideoPostCapture:
                    {
                    CCamVideoPostCaptureView* videoPostCaptureView = 
                     ((CCamVideoPostCaptureView*)View( currentViewId.iViewUid ));
                     if( videoPostCaptureView && videoPostCaptureView->Container() )
                         {
                         videoPostCaptureView->Container()->MakeVisible( EFalse );
                         }
                    iLastHiddenViewId = currentViewId.iViewUid;
                    break;
                    }
                default:
                    break;
                }
            }

          // if the application has gone to the background
          // note this does not include eikon server windows
          // The exception to this is when the slider is closed with keypad 
          // lock active and the app grid opens with a keypad status note. 
          // We know in this situation that the app should release resources.
          if ( AppInBackground( EFalse ) || 
             ( AppInBackground( ETrue ) && iPretendExit ) )
            {
            PRINT( _L("Camera <> AppUI EEventFocusGroupChanged A"))
            // when we return to foreground we use this boolean to
            // check if we have to still exit the settings modes e.g. 
            // UserScene or SceneSettings that we are in when screen saver 
            // comes or going InternalExit
            iLostFocusToNewWindow = ETrue;
              
            // Handle losing focus to another application
            // this is used for behaviour that does not need to occur
            // when views go to the background for another view within our
            // own application. Also for when the application was behind
            // an eikon server window but another application has now taken
            // the foreground
    
            if( iView && iReturnedFromPlugin )
              {
              PRINT( _L("Camera <> AppUI EEventFocusGroupChanged B"))
              static_cast<CCamViewBase*>( iView )->HandleFocusLossL();
              }
    
            // the application knows it is in the background so we are
            // not interested in any more events
            iCoeEnv->RootWin().DisableFocusChangeEvents();
            // disable timeout to standby when going to background
            iController.StopIdleTimer();
      
            // We need to know if we gain focus from a true background event
            iReturnFromBackground = ETrue;  
            // Sent to the background, so compress the heap
            User::Heap().Compress();                
              
            // stop listening mmc dismount notifications
            iController.CancelDismountMonitoring();
      
    #if !defined (__WINSCW__)
            if ( AppInBackground( ETrue ) )     
              {
              // relinquish capture keys
              UnRegisterCaptureKeys();    
              }
    #endif
            }
    
          if( AppInBackground(EFalse) || ( !CamUtility::IsBatteryPowerOK() && 
                                           !CamUtility::IsBatteryCharging() ) )
            {
            if( ECamControllerVideo == iMode )
              {
              TCamCaptureOperation operation( iController.CurrentOperation() );
              // In case of phone app, video rec is stopped already when the call starts ringing
              if ( ( !iController.CurrentlySavingVideo() ) && 
                 ( ECamCapturing == operation 
                || ECamPaused    == operation 
                || ECamPausing   == operation 
                || ECamResuming  == operation ) )
                {
                iController.StopVideoRecording();
                }
              }
            else if( iController.SequenceCaptureInProgress() && IsBurstEnabled() )
              {
              iController.StopSequenceCaptureL();
              }
            else
              {
              if( iController.IsViewFinding() )
                {
                if(CamUtility::IsBatteryPowerOK() )         
                  {
                  iController.StopViewFinder();
                  iViewFinderStopped = ETrue;	
                  }
                // stop self timer countdown if necessary
                if( SelfTimer() && SelfTimer()->IsActive() && 
                    CamUtility::IsBatteryPowerOK() )
                  {
                  // If low battery note appears, do not cancel self timer
                  SelfTimerEnableL( ECamSelfTimerDisabled );
                  }
                }
              }
            }
          break;
          }
        // -----------------------------------------------------
        // the camera app has regained the focus
        case EEventFocusGained:
          {            
          PRINT( _L("Camera <> CCamAppUi::HandleWsEventL: case EEventFocusGained") );
          TBool uiOverride = iController.UiConfigManagerPtr() && iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported();
          
          if ( !iSendAsInProgress )
              {
              // if we've regained focus assume we're not embedding anything
              SetEmbedding( EFalse ); 
              }
          
          //Make the current container visible again. This way the camera ui doesn't flash quickly in the portait mode
          //when returning from the background. unless this is first startup and uiorientationioverride api is used
          if ( !uiOverride || ( !iFirstBoot && uiOverride ) )
              {
              switch ( iLastHiddenViewId.iUid )
                  {
                  case ECamViewIdStillPreCapture:
                      {
                      if( iStillCaptureView->Container() 
                          && !APHandler()->AccessedViaAP())
                          {
                          iStillCaptureView->Container()->MakeVisible( ETrue );
                          }
                      break;
                      }
                  case ECamViewIdStillPostCapture:
                      {
                      CCamStillPostCaptureView* stillPostCaptureView = 
                      ((CCamStillPostCaptureView*)View( iLastHiddenViewId ));
                      if( stillPostCaptureView && stillPostCaptureView->Container() )
                          {
                          stillPostCaptureView->Container()->MakeVisible( ETrue );
                          }
                      break;
                      }
                  case ECamViewIdVideoPreCapture:
                      {
                  if( iVideoCaptureView->Container() 
                      && !APHandler()->AccessedViaAP())
                          {
                          iVideoCaptureView->Container()->MakeVisible( ETrue );
                          }
                      break;
                      }
                  case ECamViewIdVideoPostCapture:
                      {
                      CCamVideoPostCaptureView* videoPostCaptureView = 
                       ((CCamVideoPostCaptureView*)View( iLastHiddenViewId ));
                       if( videoPostCaptureView && videoPostCaptureView->Container() )
                           {
                           videoPostCaptureView->Container()->MakeVisible( ETrue );
                           }
                      break;
                      }
                  default:
                      break;
                  }
              }
          // focus gained event is received while videocall is active
          if ( iController.InVideocallOrRinging() )
              {
              if ( ECamViewStateStandby == iViewState && 
                   StandbyStatus() == KErrInUse )
                  { 
                  // We call the base class and return
                  CAknAppUi::HandleWsEventL( aEvent, aDestination );
                  return;
                  }
              else if ( iViewState != ECamViewStateSettings && 
                        iViewState != ECamViewStateViaPlayer &&
                        iTargetViewState != ECamViewStatePostCapture &&
                       !iFirstBoot )
                  {
                  //Don't update task state in case of embedded camera
                  if ( !iEikonEnv->StartedAsServerApp())
                      {
                      HideTaskL( EFalse );
                      }
                  iPretendExit = EFalse; 	
                  // Go to standby with error	
                  HandleStandbyEventL( KErrInUse );
                  // We call the base class and return
                  CAknAppUi::HandleWsEventL( aEvent, aDestination );
                  return;
                  } 
              }
          // Only do startup performance traces if this is
          // a subsequent startup.
          if ( IsInPretendExit() )
              {
              OstTrace0( CAMERAAPP_PERFORMANCE, CCAMAPPUI_HANDLEWSEVENTL, "e_CAM_APP_INIT 1" ); //CCORAPP_APP_INIT_START
              }
              
          PERF_EVENT_START_L1( EPerfEventAppSubsequentStartup );
                
         // start listening mmc dismount notifications
         iController.StartDismountMonitoring();	 
               
          if ( !uiOverride || ( !iFirstBoot && uiOverride ) )
              {      
              NotifyViews( ECamAppEventFocusGained );
              if( !uiOverride )
                  {
                  // whenever we come from background, we reset the image, 
                  // video & common settings as there is a possibility that 
                  // some settings might have changed from the GS application
                  // by the user.
                  iController.LoadStaticSettingsL( IsEmbedded() );
                  }
                    
              iStillCaptureView->UpdateToolbarIconsL();
              if( !uiOverride )
                  {
                  //check for silent profile whenever we come to foreground.
                  iController.IsProfileSilent();
                  }
              }                  
              
          //Check existance of current file if in video or image in 
          //burstthumbnail view.      
          if( CurrentViewState() == ECamViewStateBurstThumbnail  )
              {
              StartFileNameCheck();
              }
          /*
          * We check if the state of the camera is in the ViaPlayer mode state
          * if so, then we just activate the player view and return after
          * coming from the background.
          */
          if ( iViewState == ECamViewStateViaPlayer )
              {
              ActivateLocalViewL ( iViaPlayerUid );
              // We call the base class and return
              CAknAppUi::HandleWsEventL( aEvent, aDestination );
              return;
              }
          else if ( iTargetViewState == ECamViewStatePostCapture  )
              {
              TrySwitchViewL( ETrue );
              // register an interest in new events
              iCoeEnv->RootWin().EnableFocusChangeEvents();   // ignore error
                
#if !defined (__WINSCW__)
              // Grab capture keys
              RegisterCaptureKeysL();    
    
#endif             
              // iPretendExit flag needs to be reset before returning, otherwise
              // views think we are still in exit state and will not reserve camera
              iPretendExit = EFalse;
              CAknAppUi::HandleWsEventL( aEvent, aDestination );
              return;
              }
          else if ( SettingsLaunchedFromCamera() )
              {
              if ( iController.InVideocallOrRinging() && 
                   iController.CurrentOperation() == ECamNoOperation )
                  {
                  iTargetViewState = ECamViewStatePreCapture;
                  }
              else
                  {    
                  CCamViewBase *precapView = static_cast<CCamViewBase*>( iView );
				  __ASSERT_DEBUG(precapView, CamPanic(ECamPanicNullPointer));
                  if( precapView->IsInStandbyMode() )
                	  {
                	  iView->HandleCommandL( ECamCmdExitStandby );
                	  }
                  iTargetViewState = ECamViewStateSettings;
                  }
              TrySwitchViewL();
              CAknAppUi::HandleWsEventL( aEvent, aDestination );
              return;
              }
          else
              {
              // we are now sure that settings plugin is not active
              iReturnedFromPlugin = ETrue;
              }
           
         
          if ( iLostFocusToNewWindow )
              {            
              if ( ECamViewStateBurstThumbnail != iTargetViewState )
                  { 
                  iLostFocusToNewWindow = EFalse;
                  }
              }
          if( !uiOverride || ( !iFirstBoot && uiOverride ) )
              {  
              StartCheckingDefaultAlbumIdL();
              }
  
          // Free the needed ram memory if not enough available
          iMemoryMonitor->CheckAndRequestMemoryL(
                           iController.UiConfigManagerPtr()->CriticalLevelRamMemoryFocusGained(),
                           iController.UiConfigManagerPtr()->RequiredRamMemoryFocusGained(),
                           EFalse );


          iController.SetEndKeyExitWaiting( EFalse );
          //Don't update task state in case of embedded camera
          if ( !iEikonEnv->StartedAsServerApp())
              {
              HideTaskL( EFalse );
              }
          // Restart the sensor idle
          if ( iController.UiConfigManagerPtr()->IsOrientationSensorSupported() )
              {
              // Handle sensor api init when we have idle time
              StartSensorIdleL();      
              }
                
          // the focus can be regained during a SendAs Bluetooth operation
          // so don't try to exit the app if SendAs is in progress - the
          // exit check will be performed once SendAs completes
          // The same applies while a sequence capture is being completed
          if ( !iSendAsInProgress && !iController.SequenceCaptureInProgress() )
              {
              PRINT( _L("Camera <> CCamAppUi::HandleWsEventL EEventFocusGained checking exit status") )
              // check if exit is required
              if ( iController.CheckExitStatus() )
                  {
                  InternalExitL();
                  PRINT( _L("Camera <= CCamAppUi::HandleWsEventL, internal exit") )
                  return;
                  }
              }
                        
          // register an interest in new events
          iCoeEnv->RootWin().EnableFocusChangeEvents();   // ignore error
                
    #if !defined (__WINSCW__)
          // Grab capture keys
          RegisterCaptureKeysL();    
    
    #endif             
    
          iReturnFromBackground = EFalse;

          // When the application gains focus
          // the memory to use should be re-checked for 
          // availability. If uioverride supported, check
          // made later since settings not loaded at this point
          if ( !uiOverride )
              {
              iController.CheckMemoryToUseL();
              }
                    
          TBool returningFromPretendExit = iPretendExit;
          iPretendExit = EFalse; 
                 
          TCamViewSwitch switchNeeded = ECamViewSwitchNone;               
                                         
          // if a view switch has been waiting for the application to get 
          // the foreground.
          if ( iPendingViewSwitch != EPendingViewSwitchNone && 
               !iSendAsInProgress )
              {
              // In case the application is exited from portrait mode 
              // and landscape should be used -> orientation switch is required 
              TAppUiOrientation appOrientation = CAknAppUiBase::Orientation();
              if ( EAppUiOrientationPortrait == appOrientation ) 
                  {
                  SwitchOrientationIfRequiredL( 
                                 CAknAppUiBase::EAppUiOrientationUnspecified );
                  }
              TBool deactivateFirst = 
                   ( iPendingViewSwitch == EPendingViewSwitchDeactivateFirst );
              switchNeeded = ViewSwitchRequired( deactivateFirst );
              iPendingViewSwitch = EPendingViewSwitchNone;
              TrySwitchViewL( deactivateFirst );
              }
          else
              {
              if( iViewFinderStopped )
                  {
                  // don't start VF if EEventFocusGroupChanged is coming later
                  if( !iLostFocusToNewWindow && 
                      iController.CurrentMode() == ECamControllerImage || 
                      iController.CurrentMode() == ECamControllerVideo ) 
                      {
                      iController.StartViewFinder(); 
                      }
                  iLostFocusToNewWindow = EFalse;
                  }           
              }    
                    
          // force side pane and active palette to update if simulating app 
          // launch this has to be done after the call to TrySwitchViewL() to 
          // ensure that IsBurstEnabled() returns the correct value.
          if ( returningFromPretendExit )
              {
            /*if ( iController.UiConfigManagerPtr() && 
                 iController.UiConfigManagerPtr()->IsLocationSupported() )
                {
                // If record location setting is on, start location trail,
                // provided, primary camera and pre-capture view
                if ( ECamLocationOn == iController.IntegerSettingValue( ECamSettingItemRecLocation ) 
                     && ECamActiveCameraPrimary == ActiveCamera() 
            	     && ECamViewStatePreCapture == iViewState 
            	   )
                    {
                  	iController.StartLocationTrailL();
                    }
                }*/ 
          
            iCamSidePane->SetCaptureMode( iMode );
            // The active palette is always switched on when simulating 
            // app launch
            iPreventActivePaletteDisplay = EFalse;                                
    
               
            if ( iView == iVideoCaptureView )
                {
                iVideoCaptureView->UpdateToolbarIconsL();
                }
            }

          if ( uiOverride && ( returningFromPretendExit || iFirstBoot ) )
              {
              SetActivePaletteVisibility( EFalse );               	
              }
          else if ( !TimeLapseSliderShown() && 
                    iViewState == ECamViewStatePreCapture && 
                    switchNeeded != ECamViewSwitch && 
                    !uiOverride )
              {
              RaisePreCaptureCourtesyUI( ETrue );
              }
          
          // If keylock is set on when recording is starting up but not yet 
          // started, display toolbar when keylock is set off since we are 
          // returning to precapture view, unless capturing is still going on.
          if ( iView == iVideoCaptureView 
              && iController.CurrentOperation() != ECamCapturing 
              && !iController.IsProcessingCapture() )
              {
             if ( iController.IsTouchScreenSupported() )
                 {
                 CAknToolbar* toolbar = CurrentFixedToolbar();
                 if ( toolbar )
                     {
                     if ( iPreCaptureMode == ECamPreCapViewfinder )
                         {
                         toolbar->SetToolbarVisibility( ETrue );
                         }
                     else
                         {
                         toolbar->SetToolbarVisibility( EFalse );
                         }
                     }
                 }
              }
          break;
          }
        // -----------------------------------------------------
        case EEventFocusLost:
          {
          PRINT( _L("Camera <> CCamAppUi::HandleWsEventL: case EEventFocusLost") );

          //When go to background from video post caputure view, we need to hide the toolbar to avoid icons overlap
          if( AppInBackground( EFalse )
              && iViewState == ECamViewStatePostCapture
              && iMode == ECamControllerVideo )
              {
              SubmergeToolbar();
              }

          // focus lost event while videocall active and camera in standby,
          // no notification to views
          if ( iController.InVideocallOrRinging()
            && ECamViewStateStandby == iViewState )
            {
            // We call the base class and return
            CAknAppUi::HandleWsEventL( aEvent, aDestination );
            return;         	
            }	
            
          NotifyViews( ECamAppEventFocusLost );
          
          if ( iController.UiConfigManagerPtr()
               && iController.UiConfigManagerPtr()->IsOrientationSensorSupported() )
          // Shut down the Sensor API object.
          iController.UpdateSensorApiL(EFalse);

          if ( iController.UiConfigManagerPtr() && 
               iController.UiConfigManagerPtr()->IsLocationSupported() )
              {
        	  // If record location setting is on, stop location trail when losing focus
        	  if ( AppInBackground( EFalse ) )
        	      {
        	      PRINT( _L("Camera: CCamAppUi::HandleWsEventL focus lost, stop location trail") )
        	      iController.StopLocationTrail();
        	      }
              }

    #if !defined (__WINSCW__)
          // relinquish capture keys
          UnRegisterCaptureKeys();    
          
          RArray <TInt> halfCaptureKeys;
          CleanupClosePushL( halfCaptureKeys );

          // now get half key press code to register
          if ( iController.UiConfigManagerPtr() )
              {
              iController.UiConfigManagerPtr()->
                          SupportedPrimaryCameraAutoFocusKeyL( halfCaptureKeys );
              }
          if ( halfCaptureKeys.Count() > 0 )
              {
              // Simulate key up event for the half-press key to make
              // sure autofocus does not get stuck when we lose focus.
              TKeyEvent key;
              key.iRepeats = 0;
              key.iCode = 0;
              key.iScanCode = halfCaptureKeys[0];
              key.iModifiers = 0;
              iEikonEnv->SimulateKeyEventL( key, EEventKeyUp );        
              }
          CleanupStack::PopAndDestroy( &halfCaptureKeys );
    #endif
          break;
          }
        // -----------------------------------------------------
        case KUidValueAknsSkinChangeEvent:
          {
          if ( iNaviProgressBarModel )
              {
              iNaviProgressBarModel->ReloadResourceDataL();
              }
          break;
          }
        case EEventPointer:
            PRINT( _L("Camera <> CCamAppUi::HandleWsEventL: EEventPointer") );
            // restart idle timer every time screen is touched
            iController.StartIdleTimer();
            break;
        // -----------------------------------------------------
        default:
          PRINT( _L("Camera <> CCamAppUi::HandleWsEventL: unhandled case") );
          break;
        // -----------------------------------------------------
        }
    
      CAknAppUi::HandleWsEventL( aEvent, aDestination );
    }
  PRINT( _L("Camera <= CCamAppUi::HandleWsEventL") );
  }


// -----------------------------------------------------------------------------
// NotifyViews
// -----------------------------------------------------------------------------
//
void
CCamAppUi::NotifyViews( const TCamAppEvent& aEvent )
  {
  PRINT( _L("Camera => CCamAppUi::NotifyViews") );
  TUid settingsPluginUid = KNullUid;
  if ( iPlugin )
     {
     settingsPluginUid = iPlugin->Id();  
     }

  for( TInt i = 0; i < iViews->Count(); i++ )
    {
    const TUid& uid( (*iViews)[i]->Id() );
    PRINT2( _L("Camera <> View[%2d] uid[%3d]"), i, uid.iUid );


       if (
             KGSCamImageSettingsViewId != uid &&
             KGSCamVideoSettingsViewId != uid &&
             settingsPluginUid != uid &&
             ECamViewIdViaPlayerPlayBack != uid.iUid ) 

              {
               static_cast<CCamViewBase*>( (*iViews)[i] )->HandleAppEvent( aEvent );
              }
      }

  PRINT( _L("Camera <= CCamAppUi::NotifyViews") );
  }

// -----------------------------------------------------------------------------
// CCamAppUi::ConstructPreCaptureViewsL
// Construct the application views
// -----------------------------------------------------------------------------
//
void CCamAppUi::ConstructPreCaptureViewsL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_CONSTRUCTPRECAPTUREVIEWSL, "e_CCamAppUi_ConstructPreCaptureViewsL 1" );
    PRINT( _L( "Camera => CCamAppUi::ConstructPreCaptureViewsL()" ) ); 
    PERF_EVENT_START_L2( EPerfEventPreCaptureViewConstruction );

    // Initialize views
    CCamStillPreCaptureView* stillView =
        CCamStillPreCaptureView::NewLC( iController );
    iStillCaptureView = stillView;

    AddViewL( stillView );
    CleanupStack::Pop( stillView );
    
    PRINT( _L( "Camera => CCamAppUi::ConstructPreCaptureViewsL() still view complete" ) ); 

    CCamVideoPreCaptureView* videoView = CCamVideoPreCaptureView::NewLC( iController );
    iVideoCaptureView = videoView;

    AddViewL( videoView );
    CleanupStack::Pop( videoView );

#if !( defined(__WINS__) || defined(__WINSCW__) )
    // Get Central Repository key indicating if product uses volume keys 
    // for zoom. Load the zoom pane orientation value from the Central 
    // Repository
    if( !iRepository )
        {
        iRepository = CRepository::NewL( KCRUidCameraappSettings );
        }

    TInt val = 0;
    TInt err = iRepository->Get( KCamCrZoomUsingVolumeKeys, val );

    // If there is an error then assume volume keys not used
    if ( err )
        {
        iZoomUsingVolumeKeys = EFalse;
        }
    else 
        {
        iZoomUsingVolumeKeys = val;
        }
        
    if( iZoomUsingVolumeKeys )
        {
        // Remote controller observe
        delete iVolumeKeyObserver;
        iVolumeKeyObserver = NULL;
        iVolumeKeyObserver = CCamRemConObserver::NewL( *this );
        }
    
#else
    iZoomUsingVolumeKeys = EFalse;   
#endif // !( defined(__WINS__) || defined(__WINSCW__) ) 

    PERF_EVENT_END_L2( EPerfEventPreCaptureViewConstruction );
    PRINT( _L( "Camera <= CCamAppUi::ConstructPreCaptureViewsL()" ) ); 
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPUI_CONSTRUCTPRECAPTUREVIEWSL, "e_CCamAppUi_ConstructPreCaptureViewsL 0" );
    }

// -----------------------------------------------------------------------------
// CCamAppUi::ConstructPostCaptureViewsL
// Construct the application views
// -----------------------------------------------------------------------------
//
void CCamAppUi::ConstructPostCaptureViewsL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_CONSTRUCTPOSTCAPTUREVIEWSL, "e_CCamAppUi_ConstructPostCaptureViewsL 1" );
    PRINT( _L( "Camera => CCamAppUi::ConstructPostCaptureViewsL()" ) ); 
    if ( iPostCaptureViewsConstructed )
        {
        PRINT( _L( "Camera <= CCamAppUi::ConstructPostCaptureViewsL(,)iPostCaptureViewsConstructed == ETrue" ) ); 
        OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP2_CCAMAPPUI_CONSTRUCTPOSTCAPTUREVIEWSL, "e_CCamAppUi_ConstructPostCaptureViewsL 0" );
        return;
        }

    PERF_EVENT_START_L2( EPerfEventPostCaptureViewConstruction );
    
    ConstructPostCaptureViewIfRequiredL( ECamViewStatePostCapture,    
                                         ECamControllerImage );
    ConstructPostCaptureViewIfRequiredL( ECamViewStateBurstThumbnail, 
                                         ECamControllerImage );
    ConstructPostCaptureViewIfRequiredL( ECamViewStatePostCapture,    
                                         ECamControllerVideo );
    
    iPostCaptureViewsConstructed = ETrue;
    PERF_EVENT_END_L2( EPerfEventPostCaptureViewConstruction );
    PRINT( _L( "Camera <= CCamAppUi::ConstructPostCaptureViewsL()" ) ); 
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPUI_CONSTRUCTPOSTCAPTUREVIEWSL, "e_CCamAppUi_ConstructPostCaptureViewsL 0" );
    }

// -----------------------------------------------------------------------------
// CCamAppUi::ConstructViewIfRequiredL
// Construct the application views
// -----------------------------------------------------------------------------
//
void CCamAppUi::ConstructViewIfRequiredL()
    {
    PRINT( _L( "Camera => CCamAppUi::ConstructViewIfRequiredL()" ) ); 

  // Initialize views
  CAknView* view( NULL );

  switch( iTargetViewState )
    {
    // -----------------------------------------------------
    // Construct one of the postcapture views if needed
    case ECamViewStateBurstThumbnail:
    case ECamViewStatePostCapture:
      {
      ConstructPostCaptureViewIfRequiredL( iTargetViewState, iMode );
      break;
      }
    // -----------------------------------------------------
    // construct settings views if needed
    case ECamViewStateSettings:
      {
      if( !iPlugin )
        {
        CCamGSInterface* plugin = CCamGSInterface::NewL( KCamGSPluginUid );
        CleanupStack::PushL( plugin );
        AddViewL( plugin ); // Transfer ownership to AppUi
        CleanupStack::Pop( plugin );
        iPlugin = plugin;        
        }
      break;
      }
    // -----------------------------------------------------
    // construct user scene setup view if needed
    case ECamViewStateUserSceneSetup:
      {
      if( !iUserSceneSetupViewConstructed )
        {           
        view = CCamStillUserSceneSetupView::NewLC( iController );
        AddViewL( view );
        CleanupStack::Pop( view );
        iUserSceneSetupViewConstructed = ETrue;
        }
      break;
      }
    // -----------------------------------------------------
 
    default:
      {
      // no action
      break;
      }
    // -----------------------------------------------------
    }
  PRINT( _L( "Camera <= CCamAppUi::ConstructViewIfRequiredL" ) ); 
  }

// -----------------------------------------------------------------------------
// CCamAppUi::ConstructNaviPaneL
// Construct the navi pane
// -----------------------------------------------------------------------------
//
void CCamAppUi::ConstructNaviPaneL()
    {
     OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_CONSTRUCTNAVIPANEL, "e_CCamAppUi_ConstructNaviPaneL 1" );
     // Navi pane indicators
    iNaviPaneCounterControl = 
                            CCamNaviCounterControl::NewL( *iNaviCounterModel );
    
    iDecoratedNaviPaneCounter = 
        CAknNavigationDecorator::NewL( NaviPaneL(), iNaviPaneCounterControl );   

    iDecoratedNaviPaneCounter->SetContainerWindowL( 
            *(StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) ) );   
    iNaviPaneCounterControl->SetContainerWindowL( *iDecoratedNaviPaneCounter );

    iNaviPaneProgressBarControl = 
                    CCamNaviProgressBarControl::NewL( *iNaviProgressBarModel );
    
    iDecoratedNaviPaneProgressBar = CAknNavigationDecorator::NewL( 
                                    NaviPaneL(), iNaviPaneProgressBarControl );   

    iDecoratedNaviPaneProgressBar->SetContainerWindowL( 
            *(StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) ) );   
    iNaviPaneProgressBarControl->SetContainerWindowL( 
                                              *iDecoratedNaviPaneProgressBar );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPUI_CONSTRUCTNAVIPANEL, "e_CCamAppUi_ConstructNaviPaneL 0" );
    }

// ----------------------------------------------------
// CCamAppUi::DoPostCaptureTimeoutL
// Implementation function for post capture view timeout
// ----------------------------------------------------
//
TInt CCamAppUi::DoPostCaptureTimeoutL()
    {
    // If burst mode is active, switch to thumbnail view
    if ( iViewState == ECamViewStatePostCapture &&
         iImageMode == ECamImageCaptureBurst )
        {
        iTargetViewState = ECamViewStateBurstThumbnail ;
        }
    // otherwise, return to pre-capture view
    else if (iCamOrientation == ECamOrientationPortrait) 
      {
      iTargetViewState = ECamViewStatePrePortraitCapture;
      }
    else
      {
      iTargetViewState = ECamViewStatePreCapture;   	
      }
    
    TrySwitchViewL();
    return EFalse;
    }

// ----------------------------------------------------
// CCamAppUi::TrySwitchViewL
// Try to switch active view
// ----------------------------------------------------
//
void 
CCamAppUi::TrySwitchViewL( TBool aDeactivateFirst )
  {
  PRINT2(_L("Camera => CCamAppUi::TrySwitchViewL, iTargetViewState = %d, iViewState = %d"), iTargetViewState, iViewState);
  PRINT2(_L("Camera <> CCamAppUi::TrySwitchViewL, iTargetMode = %d, iMode = %d"), iTargetMode, iMode);
  
  //Sometimes current view and target view are same. For example switching from main camera to secondary camera
  //always sets target mode to precaptrueview and current view could already be in precapture view. This is fine
  //but In some postcaptureview cases current view and targetview is postcapture.
  //In those cases view switching is not needed unless current mode also changes.
  if( (iViewState==iTargetViewState) && ( iViewState==ECamViewStatePostCapture) && (iMode==iTargetMode) )
      {
      PRINT(_L("Camera <> CCamAppUi::TrySwitchViewL, CALLED WITHOUT PURPOSE"));      
      return;
      }  
  
  if( iZoomUsingVolumeKeys && !iVolumeKeyObserver &&
      ECamViewStatePreCapture == iTargetViewState )
    {
    // Volumekeyobserver has been removed due to activating an external
    // view. Volume keys are only needed for zooming in precapture, so we
    // re-create the observer during next switch to precap (=now)  
    iVolumeKeyObserver = CCamRemConObserver::NewL( *this );            
    }
   
   
    //if burst capture stopped for usb inserted, iTargetViewState is ECamViewStatePreCapture, it need to stop sequence capture too.
    if ( iViewState == ECamViewStatePreCapture && 
          ( iTargetViewState == ECamViewStateBurstThumbnail || 
          ( ( iTargetViewState == ECamViewStatePreCapture ) && iController.IsCaptureStoppedForUsb() ) ) && 
       iController.SequenceCaptureInProgress() )       
	{
	iController.StopSequenceCaptureL();	
	}

  // some views are only constructed when they are first used
  TRAPD( err, ConstructViewIfRequiredL() )
  PRINT1( _L( "Camera <> view constrcuction status:%d"), err )
  // if the view construction failed
  if ( err )
    {
    iTargetViewState = ECamViewStatePreCapture;
    User::Leave( err );
    } 

  // if the application is in the background the view switch will happen 
  // when we get the foreground again. This stops the app coming back to
  // the foreground when the view switch occurs.
  // If keylock is enabled, view switching is allowed in order to avoid 
  // flickering
  
  if ( AppInBackground( ETrue ) && ECamViewStateStandby != iTargetViewState )  
    {          
    PRINT( _L("Camera <> app in background set pending view switch") )
    if ( aDeactivateFirst )
      {
      PRINT( _L("Camera <> setting iPendingViewSwitch = EPendingViewSwitchDeactivateFirst") )
      iPendingViewSwitch = EPendingViewSwitchDeactivateFirst;
      }
    else
      {
      // If a deactivate first view switch has already been requested 
      // then don't overwrite it
      if ( iPendingViewSwitch != EPendingViewSwitchDeactivateFirst )
        {
        PRINT( _L("Camera <> setting iPendingViewSwitch = EPendingViewSwitchNormal") )
        iPendingViewSwitch = EPendingViewSwitchNormal;
        }
      }
    PRINT( _L("Camera TrySwitchViewL returning without switch") );
    return;
    }
  
  if ( iController.UiConfigManagerPtr() && 
       iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() &&
       !iUiConstructionComplete )
    {
    iPendingViewSwitch = EPendingViewSwitchNormal;
    PRINT( _L("Camera <= CCamAppUi::TrySwitchViewL setting EPendingViewSwitchNormal, return") );
    return;   
    }

  TCamViewSwitch switchNeeded = ViewSwitchRequired( aDeactivateFirst );
  PRINT( _L("Camera TrySwitchViewL C") )
  if ( iTargetViewState == ECamViewStatePreCapture )
    {
    PRINT( _L("Camera TrySwitchViewL D") )
    // if going back to precapture view in timelapse mode then the array
    // of timelapse filenames can now be deleted
    if ( iImageMode == ECamImageCaptureTimeLapse )
      {
      iController.ResetTimelapseArray();
      }        
   
    // If exiting a post capture view, release the capture array
    if ( iViewState == ECamViewStatePostCapture ) 
      {
      iController.ReleaseArray();
      }
    else if( iViewState == ECamViewStateBurstThumbnail )
      {
      iController.ReleaseArray( ETrue );    
      }
    if ( iPreCaptureMode != ECamPreCapTimeLapseSlider )
      {
      iPreCaptureMode = ECamPreCapViewfinder;
      }       
    }

  if ( iTargetViewState == ECamViewStatePostCapture )
    {
    // If going to post capture, make sure the AP is displayed
    iPreventActivePaletteDisplay = EFalse;
    }

  // If no switches are required
  if ( switchNeeded == ECamViewSwitchNone )
    {
    PRINT( _L("Camera TrySwitchViewL no VS") )
    if ( IsDirectViewfinderActive() )
      {
      TRAP_IGNORE(HandleCommandL( ECamCmdRedrawScreen ));
      }
    return;
    }
  // If a full view switch is required, activate new view
  if ( switchNeeded == ECamViewSwitch )
    {
    PRINT( _L("Camera <> CCamAppUi::TrySwitchViewL: Stop VF for full view switch") )
    // this skip VF stopping if DV is active and camera is in standby mode
    // possible if screensaver delay is short and startup takes long enough
    if ( ECamViewStateStandby == iViewState && 
    // DSVF active or in bitmap mode in any active camera (no need to stop VF)
          ( IsDirectViewfinderActive() || 
            ( ECamActiveCameraSecondary == ActiveCamera() && 
              !iController.UiConfigManagerPtr()->IsDSAViewFinderSupported( EFalse ) ) ||      
            ( ECamActiveCameraPrimary == ActiveCamera() && 
              !iController.UiConfigManagerPtr()->IsDSAViewFinderSupported( ETrue ) ) ) )    
      {
      PRINT( _L("Camera <> CCamAppUi::TrySwitchViewL: !!! No stopviewfinder in Standby !!!") )	
      }
    else
      {
      SetViewFinderInTransit(ETrue);
      iController.StopViewFinder();
      if ( iView )
        {
        TUid viewId = iView->Id();
        if(viewId == TUid::Uid( ECamViewIdStillPreCapture )
           || viewId == TUid::Uid( ECamViewIdVideoPreCapture ))
           {
           CCamViewBase* view = static_cast<CCamViewBase*>(iView);
           CCamContainerBase* container = view->Container(); 
           container->DrawNow();
           }
        }
      
      
      }
    PRINT( _L("Camera TrySwitchViewL E") )
    TUid uid;
    TUid messageUid;
    TBool validView = ViewId( iTargetViewState, 
                              iTargetMode, 
                              iTargetImageMode, 
                              uid, 
                              messageUid, 
                              aDeactivateFirst );
    PRINT2(_L("Camera <> CCamAppUi::TrySwitchViewL, iTargetViewState = %d, iViewState = %d"), 
                                                    iTargetViewState, iViewState);
    PRINT2(_L("Camera <> CCamAppUi::TrySwitchViewL, uid = %d, messageUid = %d"), 
                                                    uid.iUid, messageUid.iUid);  
    
    __ASSERT_DEBUG( validView, CamPanic( ECamPanicInvalidView ) );

    if ( validView )  
        {
        PRINT( _L("Camera TrySwitchViewL f") )
        // the cba is reset when the view is activated
        iCbaIsDisabled = EFalse;
        ActivateLocalViewL( uid, messageUid, KNullDesC8 );
        // If self timer mode is enabled and we are switching views, we need to 
        // disable Self Timer
        if ( iInSelfTimerMode != ECamSelfTimerDisabled)
            {
            SelfTimerEnableL( ECamSelfTimerDisabled );
            }
        }
    }
    
  // Mode only switches are handled here

  // ...if we are switching to/from the video/photo views then
  // ...reset the dynamic settings.
  if ( (  ( iMode != ECamControllerVideo ) && 
          ( iTargetMode == ECamControllerVideo ) )
      ||
      (   ( iMode == ECamControllerVideo ) && 
          ( iTargetMode != ECamControllerVideo ) ) )
    {
#ifdef RESET_DYNAMIC_SETTINGS_WHEN_SWITCH_MODE
    iController.SetDynamicSettingsToDefaults();
#endif // RESET_DYNAMIC_SETTINGS_WHEN_SWITCH_MODE
    }

  if ( iController.UiConfigManagerPtr() && iDSASupported )
    {
      // ...check if we are moving to BurstCapture
      // 
      if ( // If entering burst mode...
          ( ( iTargetImageMode == ECamImageCaptureBurst     ||
              iTargetImageMode == ECamImageCaptureTimeLapse ) &&
              iImageMode       == ECamImageCaptureSingle )
             // ...or leaving burst mode
           || 
            ( ( iImageMode       == ECamImageCaptureBurst     || 
                iImageMode       == ECamImageCaptureTimeLapse ) &&     
                iTargetImageMode == ECamImageCaptureSingle )
           )     
        {
        // Stop the viewfinder (using the current state)
        PRINT( _L("Camera TrySwitchViewL burst stop VF") )
        iController.StopViewFinder();
        
        // Update AppUi internal state 
        iViewState = iTargetViewState;
        iMode      = iTargetMode;
        iImageMode = iTargetImageMode;
        
        }
      else        
        {
        // Not entering/leaving burst mode, so just update the state 
        iViewState = iTargetViewState;
        iMode      = iTargetMode;          
        iImageMode = iTargetImageMode;
        }
    }
  else
    {
      // ...set the current capture mode and view state to the targets.
      iViewState = iTargetViewState;
      iMode      = iTargetMode;  
      iImageMode = iTargetImageMode;
    }

  PRINT(_L("Camera <= CCamAppUi::TrySwitchViewL"));
  }

// ----------------------------------------------------
// CCamAppUi::ViewSwitchRequired
// Is a switch of the active view necessary
// ----------------------------------------------------
//

CCamAppUi::TCamViewSwitch 
CCamAppUi::ViewSwitchRequired( TBool aDeactivateFirst ) const
  {   
  CCamAppUi::TCamViewSwitch vSwitch = ECamViewSwitchNone;

  if ( aDeactivateFirst )
    {
    // View switch required
    vSwitch = ECamViewSwitch;
    }
  else
    {
    // Post <=> Pre and/or Video <=> Image always require view switch
    if ( iTargetViewState == iViewState 
      && iTargetMode      == iMode )
      {
      // single / burst / timelapse
      if( iTargetImageMode == iImageMode )
        {
        // Completely same mode already
        vSwitch = ECamViewSwitchNone;
        }
      else  
        {
        // No view switch is required for changing between these modes
        vSwitch = ECamViewSwitchModeOnly;
        }
      }
    else
      {
      // View switch required 
      vSwitch = ECamViewSwitch;
      }
    }

  return vSwitch;
  }

// ----------------------------------------------------
// CCamAppUi::ViewId
// Get uid of view with requested state and mode
// ----------------------------------------------------
//
TBool CCamAppUi::ViewId( TCamViewState        aState, 
                         TCamCameraMode       aMode, 
                         TCamImageCaptureMode aImageMode, 
                         TUid&                aUid, 
                         TUid&                aMessageUid,
                         TBool                aDeactivateFirst )
  {
  aMessageUid.iUid = 0;
  
  TBool found = EFalse;
  switch ( aState )
    {       
    // -----------------------------------------------------
    case ECamViewStateStandby:
      {
      // Custom message to pass to the view, is to initiate standby
      aMessageUid.iUid = ECamViewMessageStandby;
      }
    //lint -fallthrough 
    // -----------------------------------------------------
    case ECamViewStatePreCapture: // fallthrough
      {
      if ( aDeactivateFirst )
        {
        aMessageUid.iUid = ECamViewMessageDeactivateFirst;
        }
      if ( aMode == ECamControllerVideo )
        {
        aUid.iUid = ECamViewIdVideoPreCapture;               
        }
      else 
        {
        aUid.iUid = ECamViewIdStillPreCapture;
        }
      found = ETrue;
      break;
      }
    // -----------------------------------------------------
    case ECamViewStateSceneSettingList:
      {
      // Ensure the view starts with the capture setup menu displayed.
      aMessageUid.iUid = ECamViewMessageSceneSettingList;
      if ( aMode == ECamControllerVideo )
        {
        aUid.iUid = ECamViewIdVideoPreCapture;
        }
      else 
        {
        aUid.iUid = ECamViewIdStillPreCapture;
        }
      found = ETrue;
      break;
      }
    // -----------------------------------------------------
    case ECamViewStatePostCapture:
      {
      if ( aMode == ECamControllerVideo )
        {
        aUid.iUid = ECamViewIdVideoPostCapture;
        }
      else 
        {
        aUid.iUid = ECamViewIdStillPostCapture;
        }
      found = ETrue;
      break;
      }
    // -----------------------------------------------------
    case ECamViewStateBurstThumbnail:
      {
      if ( ECamControllerImage   == aMode
        && ECamImageCaptureBurst == aImageMode )
        {
        aUid.iUid = ECamViewIdBurstThumbnail;
        // We have view id for multi capture mode
        found = ETrue;
        }
      else
        {
        // No valid view id
        found = EFalse;
        }
      break;
      }
    // -----------------------------------------------------
    case ECamViewStateSettings:
      {
      TUid viewUid = KNullUid;
      aMessageUid = KNullUid;
 
      if ( ECamControllerVideo == aMode )
          {
          if ( IsSecondCameraEnabled() )
              {
              aMessageUid = TUid::Uid( KGSSecondaryCameraVideoSettingsView );
              }
          viewUid = KGSCamVideoSettingsViewId;
          }
      else
          { 
          if ( IsSecondCameraEnabled() )
              {
              aMessageUid = TUid::Uid( KGSSecondaryCameraPhotoSettingsView );
              }
          viewUid = KGSCamImageSettingsViewId;
          }
      
      if ( iPlugin )
          {
          aUid = viewUid;
          found = ETrue;	
          }

      break;
      }
    // -----------------------------------------------------
    case ECamViewStateUserSceneSetup:
      {
      if ( aMode != ECamControllerVideo )
        {
        aUid.iUid = ECamViewIdPhotoUserSceneSetup;
        // We have a photo user scene setup view id
        found = ETrue;
        }
      else
        {
        found = EFalse;
        }           
      break;
      }
    // -----------------------------------------------------
    default:                    
      break;
    // -----------------------------------------------------
    }

  return found;
  }

// ----------------------------------------------------
// CCamAppUi::NaviPaneL
// Return a pointer to the status pane's navi control container
// ----------------------------------------------------
//
CAknNavigationControlContainer* CCamAppUi::NaviPaneL()
    {
    return static_cast<CAknNavigationControlContainer*>(
                StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    }

// ---------------------------------------------------------------------------
// CCamAppUi::UpdateCba
// Update softkeys to reflect current state
// ---------------------------------------------------------------------------
//
void CCamAppUi::UpdateCba()
    {    
    CCamViewBase* view = static_cast<CCamViewBase*>( iView );        
    TRAP_IGNORE( view->UpdateCbaL() );             
    }

// Added back, but modified the note string resource
// ----------------------------------------------------
// CCamAppUi::DisplayWaitDialogL
// Displays the wait dialog when saving video
// ----------------------------------------------------
//
void CCamAppUi::DisplayWaitDialogL()
    {
//    __ASSERT_DEBUG( iWaitDialog == NULL, CamPanic( ECamPanicUi ) );
    if ( iWaitDialog == NULL )
       {
        // Display saving video wait note
        // The wait note has its initial delay disabled. This is because the call
        // to the engine to stop recording does not return until the video save 
        // etc. has completed. This does not give the wait dialog a chance to 
        // receive its timer call back and display itself. 
        iWaitDialog = new( ELeave ) CCamWaitDialog(
                reinterpret_cast<CEikDialog**>( &iWaitDialog ), ETrue );
        iWaitDialog->ExecuteLD( R_CAM_SAVING_VIDEO_WAIT_NOTE );
        // From base class CEikDialog to prevent S60 style CBAs from showing 
        // unnecessarily
        iWaitDialog->ButtonGroupContainer().MakeVisible(EFalse);
       }
    }


// ---------------------------------------------------------
// CCamAppUi::StartupMode
// Get startup capture mode from settings
// ---------------------------------------------------------
//
TCamStartupMode CCamAppUi::StartupMode() const
  {    
  __ASSERT_DEBUG( iRepository, CamPanic( ECamPanicNullPointer ) );

  TInt value = 0;
  TInt err = iRepository->Get( KCamCrDefaultCaptureMode, value );
  TCamStartupMode setting = static_cast<TCamStartupMode>( value );
  if ( err == KErrAbort ||
       err == KErrPermissionDenied ||
       err == KErrNotFound ||
       err == KErrArgument )
    {
    PRINT1( _L("Camera <> CCamAppUi::StartupMode err=%d"), err );
    CamPanic( ECamPanicDefaultNotFoundInIniFile );      
    }

  return setting;
  }

// ---------------------------------------------------------
// CCamAppUi::SaveCaptureMode
// Save current capture mode to settings
// ---------------------------------------------------------
//
void CCamAppUi::SaveCaptureMode()
    { 
    // Since this function can be called from the destructor, it is neccesary 
    // to check that iRepository is not null prior to use
    if ( iRepository && !IsEmbedded() )    
        {
        switch ( iMode )         
            {
            case ECamControllerImage:
                // Ignore errors
                iRepository->Set( KCamCrDefaultCaptureMode, ECamStillCapture );
                break;
            case ECamControllerVideo:
                // Ignore errors
                iRepository->Set( KCamCrDefaultCaptureMode, ECamVideoCapture );
                break;
            default:
               break;            
            }
        }
    }


// ---------------------------------------------------------
// CCamAppUi::SaveCourtesyUiState
// Save whether or not the UI should always be drawn.
// ---------------------------------------------------------
//

void CCamAppUi::SaveCourtesyUiState()
  {
  // Since this function can be called from the destructor, 
  // it is neccesary to check that iRepository is not null prior to use    
  if ( iRepository )
    {
    iRepository->Set( KCamCrAlwaysDrawCourtesyUi, 
                      iAlwaysDrawPreCaptureCourtesyUI );
    }
  }



// ---------------------------------------------------------
// CCamAppUi::InitCaptureCountL
// Sets up the number of images to be captured
// ---------------------------------------------------------
//
void CCamAppUi::InitCaptureCountL()
    {
    PRINT( _L("Camera => CCamAppUi::InitCaptureCountL" ))
    // If video mode is active, capture count is not used
    if ( iMode == ECamControllerImage )
        {
        TInt imageCount = 1;

        switch( iImageMode )
            {
            case ECamImageCaptureBurst:
                // Capture count initially set to 6 for short burst.
                // Once long burst is determined we adjust the burst count.
                // This approach keeps short burst as fast as possible.
                imageCount = KShortBurstCount;
                break;
            case ECamImageCaptureTimeLapse:
                // In timelapse mode set the limit as high as possible,
                // unless self-timer is used. In that case only 6 images
                // will be captured.
                if ( ECamSelfTimerDisabled != iInSelfTimerMode )
                    {
                    imageCount = KShortBurstCount;
                    }
                else
                    {
                    imageCount = iMaxBurstCaptureNum;
                    }
                break;
            default:
                // Single capture mode. Keep 1 as limit.
                break;
            }

        iController.SetCaptureLimitL( imageCount );
        }
    PRINT( _L("Camera <= CCamAppUi::InitCaptureCountL" ))  
    }

// ---------------------------------------------------------
// CCamAppUi::SetCaptureMode
// Set the current capture mode
// ---------------------------------------------------------
//
void 
CCamAppUi::SetCaptureMode( TCamCameraMode       aMode, 
                           TCamImageCaptureMode aImageMode /*= ECamImageCaptureNone*/)
    {
    iMode = aMode;
    iImageMode = aImageMode;
    iTargetMode = aMode;
    iTargetImageMode = aImageMode;
    }

// ---------------------------------------------------------
// CCamAppUi::SetTitleEmbeddedL
// Set title to  the name of embedding application
// ---------------------------------------------------------
//
void CCamAppUi::SetTitleEmbeddedL()
    {
    if (iParentAppName != NULL )
        {
        SetTitleL ( *iParentAppName );
        }
    }

// ---------------------------------------------------------
// CCamAppUi::CheckMemoryL
// Checks if there is sufficient space available for capture
// ---------------------------------------------------------
//
TBool 
CCamAppUi::CheckMemoryL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_CHECKMEMORYL, 
    			"e_CCamAppUi_CheckMemoryL 1" );
    PRINT( _L("Camera => CCamAppUi::CheckMemoryL") );
    TBool capture = ETrue;

   TCamMediaStorage  storeToCheck = ECamMediaStorageCurrent;

   if ( ECamControllerVideo == iController.CurrentMode() &&
        ECamMediaStorageCard == iController.IntegerSettingValue( ECamSettingItemVideoMediaStorage ) &&
        IsMemoryFullOrUnavailable( ECamMediaStorageCard ) )
    	{
    	storeToCheck =  ECamMediaStorageCard; 
    	}
    if(AllMemoriesFullOrUnavailable())
    	{
    	HBufC* text = StringLoader::LoadLC(R_NOTE_TEXT_ALLMEMORY_FULL);
    	TInt ret = ShowOOMNoteL( *text, EFalse);
    	capture = EFalse;
    	CleanupStack::PopAndDestroy(text);
    	}
    else if(IsMemoryFullOrUnavailable( storeToCheck ))
		{
		HBufC* text = StringLoader::LoadLC(R_NOTE_TEXT_MEMORY_FULL);
		CAknStaticNoteDialog* note = new( ELeave ) CAknStaticNoteDialog;
		note->PrepareLC( R_CAM_OOM_NOTE_OK_CANCEL);
		note->SetTextL( *text );
		iController.StopIdleTimer();
		TInt ret = note->RunDlgLD();
		CleanupStack::PopAndDestroy( text );
		if(EAknSoftkeyOk == ret)
			{
			//when memoryDialog showed, redraw background
			if ( IsDirectViewfinderActive() )
			  {
			  TRAP_IGNORE( HandleCommandL( ECamCmdRedrawScreen ) );
			  }
      
			TInt supportedMemTypes = 0;
			TCamMediaStorage currentLocation;
			TInt key = ( ECamControllerVideo == iMode )
			? ECamSettingItemVideoMediaStorage
					: ECamSettingItemPhotoMediaStorage;

			if ( iMMCRemoveNoteRequiredOnCapture )
				{
				iMMCRemoveNoteRequiredOnCapture = EFalse;
				currentLocation = static_cast<TCamMediaStorage>( 
									iController.
										IntegerSettingValueUnfiltered( key ) );
				}
			else
				{
				currentLocation = static_cast<TCamMediaStorage>( 
									iController.IntegerSettingValue( key ) );
				}
			if(currentLocation != ECamMediaStoragePhone && 
				!IsMemoryFullOrUnavailable(ECamMediaStoragePhone))
				{
				supportedMemTypes |= AknCommonDialogsDynMem::EMemoryTypePhone;
				}
			if(currentLocation != ECamMediaStorageCard && 
				!IsMemoryFullOrUnavailable(ECamMediaStorageCard))
				{
				supportedMemTypes |= AknCommonDialogsDynMem::EMemoryTypeMMCExternal;
				}
			if(currentLocation != ECamMediaStorageMassStorage && 
				!IsMemoryFullOrUnavailable(ECamMediaStorageMassStorage))
				{
				supportedMemTypes |= 
						AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage;
				}
			
			CAknMemorySelectionDialogMultiDrive* memoryDialog = 
							CAknMemorySelectionDialogMultiDrive::NewL(
											   ECFDDialogTypeSelect,
											   R_CAM_MEMORY_SELECT_DIALOG,
											   EFalse,
											   supportedMemTypes );
			TDriveNumber driveNumber = static_cast<TDriveNumber>(KErrNotFound);    
			CAknCommonDialogsBase::TReturnKey result = 
								memoryDialog->ExecuteL( driveNumber );
			 		 
			if ( result != CAknCommonDialogsBase::TReturnKey(
								CAknCommonDialogsBase::ERightSoftkey) )
				 {
				 DriveInfo::TDefaultDrives memVal = 
				 	static_cast<DriveInfo::TDefaultDrives>(
				 		CamUtility::GetDriveTypeFromDriveNumber(driveNumber));
				 TInt settingValue = 
				 	CamUtility::MapFromSettingsListMemory( memVal );
				 iController.SetIntegerSettingValueL(key,settingValue);
				 }
			else
				{
				//No impl.
				}
			capture = EFalse;
			}
    	else
    		{
    		capture = EFalse;
    		}
		iController.StartIdleTimer();
		}
    
    PRINT1( _L("Camera <= CCamAppUi::CheckMemoryL, capture ok: %d"), capture );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPUI_CHECKMEMORYL, 
    			"e_CCamAppUi_CheckMemoryL 0" );
    return capture;
    }

// ---------------------------------------------------------
// CCamAppUi::ShowErrorNoteL
// Display error note
// ---------------------------------------------------------
//
void 
CCamAppUi::ShowErrorNoteL( TCamMemoryStatus aStatus )
    {
    PRINT( _L("Camera => CCamAppUi::ShowErrorNoteL") );
    TInt resourceId;

    TCamMediaStorage currentLocation;
    if ( iMode == ECamControllerVideo )
        {
        currentLocation = static_cast<TCamMediaStorage> 
                    ( iController.IntegerSettingValueUnfiltered( ECamSettingItemVideoMediaStorage ) );
        }
    else // photo or burst mode
        {
        currentLocation = static_cast<TCamMediaStorage> 
                    ( iController.IntegerSettingValueUnfiltered( ECamSettingItemPhotoMediaStorage ) );
        }
    TInt NumberOfParameters = 0;
    TInt drive;
    TFileName path;
    TInt DriveError = KErrNone;

    // load text from resource
    switch ( aStatus )
        {
        case ECamMemoryStatusMassStorageFull:
            {
            // internal and card both full
            resourceId = R_NOTE_TEXT_MASSMEMORY_FULL;
            // Get the root path of the memory.
            DriveError = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive );
            DriveError = PathInfo::GetRootPath( path, drive );
            if ( DriveError == KErrNone )
                {
                // %U  parameter
                NumberOfParameters = 1;
                }
            else 
                {
                // no %U  parameter
                NumberOfParameters = 0;
                }
            }
            break;

        case ECamMemoryStatusFull:
            {
            if ( currentLocation == ECamMediaStorageMassStorage ) 
                {
                // internal mass storage full
                resourceId = R_NOTE_TEXT_MASSMEMORY_FULL;
                // Get the root path of the memory.
                DriveError = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive );
                DriveError = PathInfo::GetRootPath( path, drive );
                }
            else if ( currentLocation == ECamMediaStoragePhone )
                {
                // internal phone memory full
                resourceId = R_NOTE_TEXT_NOT_ENOUGH_DEVICE_MEMORY;
                // Get the root path of the memory.
                DriveError = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive );
                DriveError = PathInfo::GetRootPath( path, drive );
                }
            else
                {
                // memory card full
                resourceId = R_NOTE_TEXT_NOT_ENOUGH_MEMORY_CARD_DEFAULTNAME;
                // Get the root path of the memory.
                DriveError = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, drive );
                DriveError = PathInfo::GetRootPath( path, drive );
                }
            if ( DriveError == KErrNone )
                {
                // %U  parameter
                NumberOfParameters = 1;
                }
            else 
                {
                // no %U  parameter
                NumberOfParameters = 0;
                }

            }
            break;

        case ECamMemoryStatusInternalFull:
            {
            if ( currentLocation == ECamMediaStorageMassStorage ) 
                {
                // internal space full
                resourceId = R_NOTE_TEXT_MASSMEMORY_FULL;
                // Get the root path of the memory.
                DriveError = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive );
                DriveError = PathInfo::GetRootPath( path, drive );
                }
            else
                {
                // internal space full
                resourceId = R_NOTE_TEXT_NOT_ENOUGH_DEVICE_MEMORY;
                // Get the root path of the memory.
                DriveError = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive );
                DriveError = PathInfo::GetRootPath( path, drive );
                }
            if ( DriveError == KErrNone )
                {
                // %U  parameter
                NumberOfParameters = 1;
                }
            else 
                {
                // no %U  parameter
                NumberOfParameters = 0;
                }
            }
            break;

        case ECamMemoryStatusCardNotInserted: // fallthrough
        case ECamMemoryStatusCardLocked:
            {
            // card not present or corrupt or locked
            resourceId = R_NOTE_TEXT_CARD_NOT_INSERTED;
            }
            break;
    
        case ECamMemoryStatusCardReadOnly:
            {
            // card read-only
            resourceId = R_NOTE_TEXT_CARD_READ_ONLY;
            }
            break;
    
        default:
            {
            // invalid status - don't show note
            return;
            }            
        }

    // prevent timeout to standby - stop standby timer
    iController.StopIdleTimer();

    if ( iView && ( iStillCaptureView == iView || iVideoCaptureView == iView ) )
        {
        // Precapture view is visible... Enable the non-fading flag here to keep
        // viewfinder visible under the note. The non-fading flag is reset when
        // getting the EEventFocusGained event
        CCoeControl* container = static_cast<CCamViewBase*>( iView )->Container();
        if ( container )
            {
            RDrawableWindow* window = container->DrawableWindow();
            if ( window )
                {
                window->SetNonFading( ETrue );
                }
            }
        }
    if ( NumberOfParameters == 0 )
        {
        // set error note text
        HBufC* text = StringLoader::LoadLC( resourceId );
        // show error note
        CAknStaticNoteDialog* note = new( ELeave ) CAknStaticNoteDialog;
        note->PrepareLC( R_CAM_ERROR_NOTE );
        note->SetTextL( *text );
        note->RunDlgLD();
        CleanupStack::PopAndDestroy( text );
        }
    else if ( NumberOfParameters == 1 )
        {
        // set error note text
        HBufC* text = StringLoader::LoadLC( resourceId, path, iEikonEnv );
        // show error note
        CAknStaticNoteDialog* note = new( ELeave ) CAknStaticNoteDialog;
        note->PrepareLC( R_CAM_ERROR_NOTE );
        note->SetTextL( *text );
        note->RunDlgLD();
        CleanupStack::PopAndDestroy( text );
        }
  
    // restart standby timer
    iController.StartIdleTimer();

    PRINT( _L("Camera <= CCamAppUi::ShowErrorNoteL") );
    }

// ---------------------------------------------------------
// CCamAppUi::ShowOOMNoteL
// Display error note for OOM condition
// ---------------------------------------------------------
//
TInt CCamAppUi::ShowOOMNoteL( TDesC& aLabel, TBool aWithCancel )
    {
    // prevent timeout to standby - stop standby timer
    iController.StopIdleTimer();
    
    // show OOM note
    CAknStaticNoteDialog* note = new( ELeave ) CAknStaticNoteDialog;
    if(aWithCancel)
    	note->PrepareLC( R_CAM_OOM_NOTE_OK_CANCEL);
    else
    	note->PrepareLC( R_CAM_OOM_NOTE_OK_EMPTY);
    note->SetTextL( aLabel );
    TInt ret = note->RunDlgLD();

    // restart standby timer
    iController.StartIdleTimer();

    return ret;
    }

// ---------------------------------------------------------
// CCamAppUi::ShowMMCRemovalNoteIfRequiredL
// Shows the mmc removal note if it is required
// ---------------------------------------------------------
//
void 
CCamAppUi::ShowMMCRemovalNoteIfRequiredL()
    {
    PRINT( _L("Camera => CCamAppUi::ShowMMCRemovalNoteIfRequiredL") );
    // If we have detected a removal and not shown the note.
    if ( iViewState != ECamViewStateStandby && iMMCRemoveNoteRequired )
        {
        PRINT( _L("Camera <> Need to show note") );

        iMMCRemoveNoteRequired = EFalse;
        // Show the card not inserted note
        if( !iSendAsInProgress )
            {
            PRINT( _L("Camera <> showing note..") );
            ShowErrorNoteL( ECamMemoryStatusCardNotInserted );
            PRINT( _L("Camera <> ..done") );
            }
        }
    if ( IsDirectViewfinderActive() )
        {
        TRAP_IGNORE(HandleCommandL(ECamCmdRedrawScreen));
        }
    PRINT( _L("Camera <= CCamAppUi::ShowMMCRemovalNoteIfRequiredL") );
    }

// ---------------------------------------------------------
// CCamAppUi::CheckMemoryCard
// Check status of the memory card
// ---------------------------------------------------------
//
void CCamAppUi::CheckMemoryCard( TBool& aFull,
                                TCamMemoryStatus& aStatus, TCamMediaStorage aCurrentLocation )
    {
    // check if memory card is full
    aFull = MemoryFull( ECamMediaStorageCard, aCurrentLocation );

    // get status of card
    TCamMemoryCardStatus cardStatus = CamUtility::MemoryCardStatus();
    switch ( cardStatus )
        {
        case ECamMemoryCardLocked:
            {
            // card is locked
            aStatus = ECamMemoryStatusCardLocked;
            }
            break;
        case ECamMemoryCardInserted:
            {
            aStatus = ECamMemoryStatusOK;
            }
            break;
        case ECamMemoryCardNotInserted:
        default:
            {
            aStatus = ECamMemoryStatusCardNotInserted;
            }
            break;        
        }
    }

// ---------------------------------------------------------
// CCamAppUi::MemoryFull
// Check if the memory location (phone or card) is full
// ---------------------------------------------------------
//
TBool 
CCamAppUi::MemoryFull( TCamMediaStorage aStorage, 
                       TCamMediaStorage aCurrentLocation )
    {
    PRINT( _L("Camera => CCamAppUi::MemoryFull") );
    TBool cardFull = ETrue;

    if ( iMode != ECamControllerVideo )
        {
        // check there is space for image(s)
        TInt numImages = iController.ImagesRemaining( aStorage, IsBurstEnabled() );

        // at least 1 image for still capture and 2 for sequence capture
        if ( ( iImageMode == ECamImageCaptureSingle && numImages > 0 ) ||
                numImages > 1 )
            {
            cardFull = EFalse;
            }            
        else
            {
            cardFull = ETrue;
            }
        }
    else // capture mode is video
        {
        // If we want to know the remaining time at the current storage location
        // we can obtain this from the engine.
        if ( aStorage == aCurrentLocation )
            {
            TTimeIntervalMicroSeconds timeRemaining = iController.RecordTimeRemaining();

            // Check that enough rec time available
            // or that we are recording short (MMS) video
            if ( timeRemaining >= KMemoryFullVideoRemaining )
                {
                PRINT( _L("Camera <> not full 1") );
                cardFull = EFalse;
                }
            else
                {
                PRINT( _L("Camera <> full 1") );
                cardFull = ETrue;
                }
            }
        else
            {
            // We want to know how much remaining time is available on the
            // storage location not currently in use.
            // We don't have a reliable way of estimating this, so we will
            // assume that if there is more space available than on the current
            // storage location, then the memory is not full.

            TTimeIntervalMicroSeconds phoneMemoryFree = 
            CamUtility::MemoryFree( DriveInfo::EDefaultPhoneMemory );
            TTimeIntervalMicroSeconds cardMemoryFree =
            CamUtility::MemoryFree( DriveInfo::EDefaultRemovableMassStorage );
            TTimeIntervalMicroSeconds MassMemoryFree =
            CamUtility::MemoryFree( DriveInfo::EDefaultMassStorage );
            if ( aCurrentLocation == ECamMediaStoragePhone )
                {
                if ( phoneMemoryFree > cardMemoryFree
                     && phoneMemoryFree > MassMemoryFree
                )
                    {
                    PRINT( _L("Camera <> full 2") );
                    cardFull = ETrue;
                    }
                else
                    {
                    PRINT( _L("Camera <> not full 2") );
                    cardFull = EFalse;
                    }
                }
            else if ( aCurrentLocation == ECamMediaStorageMassStorage )
                {
                if ( MassMemoryFree > cardMemoryFree
                        && MassMemoryFree > phoneMemoryFree )
                    {
                    PRINT( _L("Camera <> full 3") );
                    cardFull = ETrue;
                    }
                else
                    {
                    PRINT( _L("Camera <> not full 3") );
                    cardFull = EFalse;
                    }
                }
            else
                {
                if ( cardMemoryFree > phoneMemoryFree 
                     && cardMemoryFree > MassMemoryFree )
                    {
                    PRINT( _L("Camera <> full 4") );
                    cardFull = ETrue;
                    }
                else
                    {
                    PRINT( _L("Camera <> not full 4") );
                    cardFull = EFalse;
                    }
                }
            }
        }

    PRINT1( _L("Camera <= CCamAppUi::MemoryFull, full:%d"), cardFull );
    return cardFull;
    }

// ---------------------------------------------------------
// CCamAppUi::StartCaptureL
// Starts the video/photo capture
// ---------------------------------------------------------
//
TKeyResponse 
CCamAppUi::StartCaptureL( const TKeyEvent& /*aKeyEvent*/ )
    {
    PRINT( _L("Camera => CCamAppUi::StartCaptureL") );
    iLensCoverExit = EFalse; 
    if ( iMode != ECamControllerVideo )
        {
        // Check for active viewfinder before proceeding with capture
        if ( !iController.IsViewFinding() )
          {
          // VF not active - stop capture
          PRINT( _L("Camera <= CCamAppUi::StartCaptureL, not vf") );
          return EKeyWasNotConsumed;
          }
        // If the view is in self-timer mode, pressing of the shutter
        // button initiates the timer...
        if ( iInSelfTimerMode != ECamSelfTimerDisabled )
            {
            __ASSERT_DEBUG( iSelfTimer, CamPanic( ECamPanicNullPointer ) );
            // Will start timer if not already started.  
            // If already started, forces capture NOW.
            /*iSelfTimer->StartSelfTimer();

            // Change CBA to be SK1 (Blank) SK2 (Cancel)
            UpdateCba();*/
            HandleCommandL(ECamCmdSelfTimerActivate);
            }
        else // Not in SelfTimer mode so just take the photo.
            {
            if ( iController.IsProcessingCapture() )
                {
                PRINT( _L("Camera <= CCamAppUi::StartCaptureL - already processing - ignored capture key") );
                return EKeyWasNotConsumed;
                }

            // Start capture
            iController.Capture();

            // Ensure softkeys are correct for sequence
            if ( IsBurstEnabled() )            
                {
                if( ECamImageCaptureBurst == iImageMode )
                    {
                    //Disable cancel autofocus and capture functionality from now on until burst is finished.
                    PRINT( _L("Camera <> CCamAppUi::StartCaptureL, SetNoBurstCancel( ETrue )") );
	                  iController.SetKeyUp();
                    iController.SetNoBurstCancel( ETrue );
                    }
                // Make sure cancel softkey is displayed
                iPreventActivePaletteDisplay = ETrue;
                static_cast<CCamViewBase*>( iView )->UnsetCourtesySoftKeysL();
                // Change CBA to be SK1 (Blank) SK2 (Cancel)
                UpdateCba();

                // disable timeout to standby during burst capture
                iController.StopIdleTimer();
                }            
            }
        // Shutter key - full press -> perform capture
        PRINT( _L("Camera <= CCamAppUi::StartCaptureL, image capture requested") );
        return EKeyWasConsumed;           
        }

    // video capture mode 
    else 
        {
        // check for available space before proceeding with capture
        TInt callType( EPSCTsyCallTypeUninitialized );
        RProperty::Get( KPSUidCtsyCallInformation, KCTsyCallType, callType );
        // video call or no memory available - stop capture
        if ( callType == EPSCTsyCallTypeH324Multimedia || !iController.IsViewFinding() || !CheckMemoryL() )
            {
            PRINT( _L("Camera <= CCamAppUi::StartCaptureL, video no memory") );
            return EKeyWasNotConsumed;
            }
        PERF_EVENT_END_L1( EPerfEventKeyToCapture );
        PERF_EVENT_START_L1( EPerfEventStartVideoRecording );    
        static_cast<CCamViewBase*>( iView )->UnsetCourtesySoftKeysL();
        iController.StartVideoRecordingL();
        PRINT( _L("Camera <= CCamAppUi::StartCaptureL, video capture requested") );
        return EKeyWasConsumed;
        }
    }

// ---------------------------------------------------------
// CCamAppUi::EndCapture
// Stops the capture of video
// ---------------------------------------------------------
//
void CCamAppUi::EndCapture()
    {
    PRINT( _L("Camera => CCamAppUi::EndCapture") );
    if ( ECamControllerVideo == iMode && !iController.IsDemandKeyRelease() )
        {
        TCamCaptureOperation operation = iController.CurrentOperation();
        // if in video recording or paused state
        if ( ECamCapturing == operation
                || ECamPaused    == operation )  
            {
            // if already requested to stop the recording
            // but the operation hasn't completed yet
            // then just return
            if ( iWaitTimer->IsActive() )
                {
                return;
                }

            ZoomPane()->StopZoom();
            ZoomPane()->MakeVisible( EFalse, ETrue );
            // ignore any errors (eg out of memory)
            TRAP_IGNORE( SetSoftKeysL( R_CAM_SOFTKEYS_BLANK ) );
            TRAP_IGNORE( DisplayWaitDialogL() );
            // give the wait dialog a chance to display itself
            iWaitTimer->Start( 0, 0,  TCallBack( StopVideoRecording, this ) );   
            }
        }
    PRINT( _L("Camera <= CCamAppUi::EndCapture") );
    }

// ---------------------------------------------------------
// CCamAppUi::SwitchOrientationIfRequiredL
// Change to the new orientation if necessary
// ---------------------------------------------------------
//    
void CCamAppUi::SwitchOrientationIfRequiredL( TInt aNewOrientation )
    {
    TInt error = KErrNone;
    // No orientation specified, find the default orientation
    if ( aNewOrientation == EAppUiOrientationUnspecified )
        {
        if ( iController.UiConfigManagerPtr()->IsSecondaryCameraSupported() ) 
            {
            // Default orientation is determined by current active camera
            if ( IsSecondCameraEnabled() && !IsQwerty2ndCamera() )   
                {
                error = CamUtility::GetPsiInt( ECamPsiSecondaryCameraOrientation, aNewOrientation );
                }
            else
                {
                error = CamUtility::GetPsiInt( ECamPsiPrimaryCameraOrientation, aNewOrientation );
                }
            }
        else
            {
            error = CamUtility::GetPsiInt( ECamPsiOrientation, aNewOrientation );
            }
        }    
    TAppUiOrientation currentOrientation = Orientation();        
    if ( !error && ( aNewOrientation != currentOrientation ) )
        {
        PRINT( _L("Camera => CCamAppUi::SwitchOrientationIfRequiredL set orient" ))
        SetOrientationL( static_cast<TAppUiOrientation>( aNewOrientation ) );
        }
    }

// ---------------------------------------------------------
// CCamAppUi::HandleScreenDeviceChangedL
// Handle change of orientation etc.
// ---------------------------------------------------------
//
void CCamAppUi::HandleScreenDeviceChangedL()
    {
    PRINT( _L("Camera => CCamAppUi::HandleScreenDeviceChangedL") )
    CAknAppUiBase::HandleScreenDeviceChangedL();
    if ( iEngineConstructionDelayed )
        {
        PRINT( _L("CCamAppUi::HandleScreenDeviceChangedL continuing engine construction") );  
        iEngineConstructionDelayed = EFalse;
        iController.CompleteConstructionL();
        // Don't set the target mode here. If the application is not embedded
        // the mode will already be correct. If embedded, the target mode is set
        // when the HandleNewFileL function is called.
        iTargetViewState = ECamViewStatePreCapture;

        // pre-construct side-pane & zoom pane
        // get whether we overlay sidepane over view-finder
        TBool overlayViewFinder = EFalse;
        User::LeaveIfError( CamUtility::GetPsiInt( ECamPsiOverLaySidePane, overlayViewFinder ) );

        iCamSidePane = CCamSidePane::NewL( iController, overlayViewFinder );
        User::LeaveIfError(iResourceLoaders.Append(iCamSidePane));

        iCamZoomPane = CCamZoomPane::NewL( iController, overlayViewFinder );
        User::LeaveIfError(iResourceLoaders.Append(iCamZoomPane));

        // Exit standby mode now 
        // (unless still waiting for HandleNewFileL command - embedded only)
        if ( IsConstructionComplete() )
            {   
            // make sure standby exits as this may not actually result in 
            // a view switch if the correct view is already activated
            if( iView )
                {
                iView->HandleCommandL( ECamCmdExitStandby ); 
                }

            iViewState = ECamViewStatePreCapture;      
            TrySwitchViewL();                    
            } 
        }

    if ( iController.UiConfigManagerPtr()->IsSecondaryCameraSupported() )
        {
        if ( iCameraSwitchDelayed )       
            {
            PRINT( _L("CCamAppUi::HandleScreenDeviceChangedL restarting switch camera") );  
            iCameraSwitchDelayed = EFalse;
            iController.SwitchCameraL(); 
            // The keys will have been blanked - update them
            static_cast<CCamViewBase*>( iView )->UpdateCbaL();
            }
        }
    PRINT( _L("Camera <= CCamAppUi::HandleScreenDeviceChangedL") );
    }

// ---------------------------------------------------------
// CCamAppUi::StopVideoRecording
// Stops the video recording
// ---------------------------------------------------------
//
TInt CCamAppUi::StopVideoRecording( TAny* aAny )
    {
    PRINT( _L("Camera => CCamAppUi::StopVideoRecording") );
    CCamAppUi* appui = static_cast<CCamAppUi*>( aAny );
    __ASSERT_DEBUG( appui != NULL && appui->iWaitTimer != NULL, CamPanic( ECamPanicNullPointer ) );
    appui->iWaitTimer->Cancel();
    appui->iController.StopVideoRecordingAsync();
    PRINT( _L("Camera <= CCamAppUi::StopVideoRecording") );
    return KErrNone;
    }

// ---------------------------------------------------------
// CCamAppUi::CallExit
// Exit the application
// ---------------------------------------------------------
//
TInt CCamAppUi::CallExit( TAny* aAny )
    {
    PRINT( _L("Camera => CCamAppUi::CallExit") )
    CCamAppUi* appui = static_cast<CCamAppUi*>( aAny );
    __ASSERT_DEBUG( appui != NULL && appui->iWaitTimer != NULL, CamPanic( ECamPanicNullPointer ) );
    appui->iWaitTimer->Cancel();
    appui->Exit();
    return KErrNone;
    }

// ---------------------------------------------------------
// CCamAppUi::CloseAppL
// Exit the application
// ---------------------------------------------------------
//
void CCamAppUi::CloseAppL()  
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_CLOSEAPPL, "e_CCamAppUi_CloseAppL 1" );
    
    PRINT( _L("Camera => CCamAppUi::CloseAppL") )

    // cancel the self timer if active - can be active
    // if closing the slide while counting down
    if ( iInSelfTimerMode != ECamSelfTimerDisabled )
        {
        SelfTimerEnableL( ECamSelfTimerDisabled );
        }

    // if we are exiting cam app, we dont need any updates
    // for the navipane
    if ( NULL != iNaviCounterControl && NULL != iNaviPaneCounterControl && 
         NULL != iNaviProgressBarControl && NULL != iNaviPaneProgressBarControl )
        { 
        UpdateNaviModelsL( EFalse );
        }

    if ( iEikonEnv->StartedAsServerApp() )
        {
        TCamCaptureOperation operation( iController.CurrentOperation() );

        // If recording a video, ensure exit after video recording process has completed
        if( ECamControllerVideo == iMode
                && ( ECamCapturing == operation
                  || ECamPausing   == operation
                  || ECamPaused    == operation
                  || ECamResuming  == operation ) )
            {
            iController.EnterShutdownMode( EFalse );            
            }
        // Otherwise, close application immediately.
        else
            {
            iController.EnterShutdownMode( ETrue );
            Exit();            
            }
        }
    else
        {
        // this will exit when any current process completes
        iController.EnterShutdownMode( EFalse );
        }
    PRINT( _L("Camera <= CCamAppUi::CloseAppL") )        
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP_CCAMAPPUI_CLOSEAPPL, "e_CCamAppUi_CloseAppL 0" );
    
    }

// ---------------------------------------------------------
// CCamAppUi::InternalExitL
// Exit the application from an internal call - this often means
// just going to the background and resetting the settings/modes
// ---------------------------------------------------------
//
void CCamAppUi::InternalExitL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_INTERNALEXITL, "e_CCamAppUi_InternalExitL 1" );
    
    PRINT( _L("Camera => CCamAppUi::InternalExitL") );

    iController.StoreFaceTrackingValue(); // store the current FT setting
	
    if ( iController.UiConfigManagerPtr() && 
         iController.UiConfigManagerPtr()->IsLocationSupported() )
        {
        // If record location setting is on, stop location trail
        if ( ECamLocationOn == iController.IntegerSettingValue( ECamSettingItemRecLocation ) )
          	{
          	// The parameter ETrue for also closing the session to location utility server
          	// when exiting camera.
          	iController.StopLocationTrail( ETrue );
          	}
        }
    
    if( !iController.IsSavingInProgress() )
        {
        iController.ReleaseArray(ETrue);  
        }
    
    if ( iEikonEnv->StartedAsServerApp() || !iController.AlwaysOnSupported() )
        {
        CloseAppL(); 
        }
    else if ( SettingsLaunchedFromCamera() )
        {
        //If internal exit was called from settings, we need to set the camera to
        //background before closing it to prevent view switch and that way ensuring
        //that all the camera resources are released in exit.
        SendCameraAppToBackgroundL();                
        CloseAppL();
        }
    else
        {
        // if UI orientetion override is supported or lens cover is not and 
		// exiting 2ndary camera, set cameraswitch to main camera since 
		// always starting in maincamera stillmode
        if ( iController.UiConfigManagerPtr() &&
             ( iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() || 
               !iController.UiConfigManagerPtr()->IsLensCoverSupported() ) &&
             IsSecondCameraEnabled() )
            {
            iController.SetCameraSwitchRequired( ESwitchSecondaryToPrimary );    
            }       
        else if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() )
            {
            PRINT( _L("Camera <> SetCameraSwitchRequired -> ESwitchToUnknown") );
            iController.SetCameraSwitchRequired( ESwitchToUnknown );
            }
        // cancel the self timer if active - can be active
        // if closing the slide while counting down
        if ( iInSelfTimerMode != ECamSelfTimerDisabled)
            {
            SelfTimerEnableL( ECamSelfTimerDisabled );
            }
          
        if ( iController.UiConfigManagerPtr()->IsLensCoverSupported() )
            {
            iController.SliderCloseEventActioned();
            }

        iController.SetEndKeyExitWaiting( EFalse );
        PRINT( _L("Camera CCamAppUi::InternalExitL - setting zoom reset pending") );

        iCamZoomPane->ResetToDefaultAfterPrepare( ETrue );
        SendCameraAppToBackgroundL();
        
        // if faster startup feature is supported, always start in stillmode after exit
        if ( iController.UiConfigManagerPtr() && 
             iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() &&
             iMode == ECamControllerVideo )
            {
            PRINT( _L("Camera <> InternalExitL - switching to still precapture") );
            iTargetMode = ECamControllerImage;
            iTargetViewState = ECamViewStatePreCapture;
            TrySwitchViewL( ETrue );
            }
        else if ( iView && iViewState != ECamViewStatePreCapture && iReturnedFromPlugin )
            {
            PRINT( _L("Camera: CCamAppUi::InternalExitL - switching to precapture mode") )
            iTargetViewState = ECamViewStatePreCapture;
            TrySwitchViewL( ETrue );
            }

        // If the application is already in the background but has not released
        // the resources (e.g. it was behind an eikon server window and had not yet set 
        // the iPretendExit flag) then the view needs to know that the resources should 
        // be released now.
        // This situation occurs when the application is 'pretending' to exit after a
        // slider closed event, but the keypad lock note appeared (but only if it was
        // in the middle of capturing a sequence as the exit is delayed for the sequence 
        // to complete).
        // HandleFocusLoss() releases the resources in situations where the app was
        // 'continuing in the background'. This release of resources will only occur
        // once as the iContinueInBackground flag is reset as soon as the resources are 
        // released.

        if ( iView && iReturnedFromPlugin )
            {
            PRINT( _L("Camera InternalExitL calling iView->HandleFocusLossL()") );
            static_cast<CCamViewBase*>( iView )->HandleFocusLossL();
            }
        // Reset active palette position. As this is done before pretended exit,
        // no AP update is needed when the application is returned to foreground
        if( iActivePaletteHandler && iReturnedFromPlugin )
            {            
            iActivePaletteHandler->ResetToDefaultItem();
            // Ensure AP visibility on camera startup (also for 2ndary camera)
            // unless UI orientation override is used
            if ( iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
                {
                iActivePaletteHandler->SetVisibility( EFalse );	
                }
            else    
                {
                iActivePaletteHandler->SetVisibility( ETrue );
                }
        if( ECamViewStatePreCapture == iViewState )
            {
            iDrawPreCaptureCourtesyUI = ETrue;
            if ( !iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() )
                {
                RaisePreCaptureCourtesyUI(EFalse);
                }
 
            if ( IsSecondCameraEnabled() 
                 && !IsEmbedded() )
                {
                if ( ECamControllerVideo == iMode )
                    {
                    SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__RECORD_SECONDARY );
                    }
                else
                    {
                    SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE_SECONDARY );      
                    }
                }
              }
            }

        // Cancel delayed sensor initialization, in case it's still pending when 
        // internal exit is requested.
        if ( iController.UiConfigManagerPtr()->IsOrientationSensorSupported() )
            {
            CancelSensorIdle();
            }

        if ( iController.UiConfigManagerPtr()->IsXenonFlashSupported() )
            {
            // Reset the flash error status
            iController.FlashStatus()->SetFlashError( EFalse );
            }
/*#ifndef __WINS__
        if ( iSendFileInCall )
            {
            // bring phone app to fore ground
            BringPhoneAppToForeGroundL();
            }    
#endif // __WINS__   */
        }
    
    if ( iView && ( iStillCaptureView == iView || iVideoCaptureView == iView )
            && ShowPostCaptureView() )
        {
        SetAssumePostCaptureView( EFalse );
        }

    if( iController.IsViewFinding() )
        {
        iController.StopViewFinder();
        iViewFinderStopped = ETrue;        
        } 
 
    PRINT( _L("Camera <= CCamAppUi::InternalExitL") );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP_CCAMAPPUI_INTERNALEXITL, "e_CCamAppUi_InternalExitL 0" );
    
    } 

// ---------------------------------------------------------
// CCamAppUi::CallSwitch
// Switch view
// ---------------------------------------------------------
//
TInt CCamAppUi::CallSwitch( TAny* aAny )
    {
    CCamAppUi* appui = static_cast<CCamAppUi*>( aAny );
    __ASSERT_DEBUG( appui != NULL, CamPanic( ECamPanicNullPointer ) );
    appui->iWaitTimer->Cancel();
    TRAP_IGNORE( appui->TrySwitchViewL() );
    return KErrNone;
    }


// ---------------------------------------------------------
// CCamAppUi::AppInBackground
// Whether or not the app is in the background
// ---------------------------------------------------------
//
TBool CCamAppUi::AppInBackground( TBool aIncludeNotifier ) const
    {
    PRINT1( _L("Camera => CCamAppUi::AppInBackground( %d )" ), aIncludeNotifier );
    TBool aBackground = EFalse;
    // Gets the window group id of the app coming in foreground
    TInt windowGroupId = iCoeEnv->WsSession().GetFocusWindowGroup();

    PRINT1( _L("Camera    CCamAppUi::AppInBackground windowGroupId=%d" ), windowGroupId );
    
    // Check if the window group moving to the front is the AknCapServer.
    // This often happens when OOM or out of disk space notes are shown
    TBool capServerGroup = CamUtility::IdMatchesName( windowGroupId, KAknCapServer );     

    // AknNotifyServer notifies e.g. Location related events.
    TBool nfyServerGroup = CamUtility::IdMatchesName( windowGroupId, KAknNotifyServer );     
    
	// KPbkSever - Contacts App; Assign to contacts scenario
	TBool pbkServerGroup = CamUtility::IdMatchesName( windowGroupId, KPbkServer );
    PRINT1( _L("Camera    CCamAppUi::AppInBackground iMyWgId=%d" ), iMyWgId );
    // if the foreground app is not this app
    if ( windowGroupId != iMyWgId )
        {
        // if another app has the foreground
        if ( !capServerGroup 
          && !nfyServerGroup
		  && !pbkServerGroup
          &&  windowGroupId != iEikonServerWindowGroupId 
          &&  windowGroupId != iBTServerWindowGroupId 
           )
            {
            PRINT(_L("Camera    CCamAppUi::AppInBackground non-AKN, non-EIK serv has focus"));
            aBackground = ETrue;
            }
        // if the eikon server has the foreground and we're classing that as
        // putting us in the background
        else if ( aIncludeNotifier )
            {           
            // Check whether the new foreground window is an Eikon window
            if ( windowGroupId == iEikonServerWindowGroupId )
                {
                PRINT(_L("Camera <> CCamAppUi::AppInBackground EIK SERV HAS FOCUS"));
                aBackground = ETrue;    
                }
            // Check if AknCapServer is in the foreground (includes other notes)
            else if ( capServerGroup )
                {
                PRINT(_L("Camera <> CCamAppUi::AppInBackground CAP SERV HAS FOCUS"));
                aBackground = ETrue;    
                }
            else if( nfyServerGroup )
                {
                PRINT(_L("Camera <> CCamAppUi::AppInBackground Avkon notify server has focus"));
                aBackground = ETrue;
                }
            else if ( windowGroupId == iBTServerWindowGroupId )
                {
                PRINT(_L("Camera <> CCamAppUi::AppInBackground BT SERV HAS FOCUS"));
                aBackground = ETrue;
                }    
            else // empty statement for lint
                {
                }                
            }
        else // empty statement for lint
            {
            }
        }
    PRINT1( _L("Camera <= CCamAppUi::AppInBackground return(%d)" ), aBackground );
    return aBackground;
    }


// ---------------------------------------------------------
// CCamAppUi::LaunchHelpL
// Launch the context sensitive help application
// ---------------------------------------------------------
//
void CCamAppUi::LaunchHelpL()
    {
    PRINT( _L("Camera => CCamAppUi::LaunchHelpL") );
    TCoeHelpContext helpContext;
    helpContext.iMajor = TUid::Uid( KCameraappUID );
    
    // Check which view is active to obtain the help context
    TVwsViewId currentViewId;
    TInt err = GetActiveViewId( currentViewId );
    if ( err == KErrNone )
        {
        switch ( currentViewId.iViewUid.iUid )
            {
            case ECamViewIdStillPreCapture:
                // Still precapture can have multiple modes. We must ask it
                // for the current context.
                iStillCaptureView->GetHelpContext( helpContext );
                break;            
            case ECamViewIdStillPostCapture:
                helpContext.iContext = KLCAM_HLP_POST_PHOTO;                    
                break;
            case ECamViewIdVideoPreCapture:
                // Video precapture can have multiple modes. We must ask it
                // for the current context.
                iVideoCaptureView->GetHelpContext( helpContext );
                break;
            case ECamViewIdVideoPostCapture:
                helpContext.iContext = KLCAM_HLP_POST_VIDEO;
                break;
            case ECamViewIdBurstThumbnail:
                helpContext.iContext = KLCAM_HLP_POST_SEQ;
                break;
            case ECamViewIdVideoSettings:
                helpContext.iContext = KLCAM_HLP_SETTINGS_VIDEO;
                break;
            case ECamViewIdPhotoSettings:
                helpContext.iContext = KLCAM_HLP_SETTINGS_PHOTO;
                break;
            case ECamViewIdPhotoUserSceneSetup:
                helpContext.iContext = KLCAM_HLP_SETTINGS_PHOUSER;
                break;
            default:
                break;
            }                
        }
    else if ( err == KErrNotFound && iPreCaptureMode == ECamPreCapStandby )
        {
        // This is a work-around for the problem of GetActiveViewId()
        // returning -1 when in the Standby view.
        if ( IsEmbedded() )
            {
            helpContext.iContext = KLCAM_HLP_STANDYBY_EM;
            }
        else
            {
            helpContext.iContext = KLCAM_HLP_STANDYBY_EM;
            }        
        }
    else
        {
        User::Leave( err );
        }    

    PRINT1( _L("Camera <> CCamAppUi::LaunchHelpL, help context: %S"), &(helpContext.iContext) );

    // Create the context array and append the single context item
    CArrayFix<TCoeHelpContext>* contextArray = 
        new ( ELeave ) CArrayFixFlat<TCoeHelpContext> ( 1 );

    CleanupStack::PushL( contextArray );
    contextArray->AppendL( helpContext );
    CleanupStack::Pop( contextArray );

    //Launch help - takes ownership of context array
    HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                                         contextArray );

    PRINT( _L("Camera <= CCamAppUi::LaunchHelpL") );
    }

#ifndef __WINS__
// ---------------------------------------------------------
// CCamAppUi::ProcessCommandParametersL
// Allow Camera application to respond to request for in-call send
// functionality from Phone application
// ---------------------------------------------------------
//
TBool CCamAppUi::ProcessCommandParametersL(
    TApaCommand aCommand,
    TFileName& aDocumentName,
    const TDesC8& /*aTail*/ )
    {
    PRINT( _L("CCamAppUi::ProcessCommandParametersL") );
    // Check if we should start in SFI mode
/*    if ( aCommand == EApaCommandOpen && aDocumentName == KSFITrailing )
        {
        if ( !SFIUtility() )
            {
            PRINT( _L("CCamAppUi::ProcessCommandParametersL !!") );
            return EFalse;
            }
        SFIUtility()->IsCLIValidL();
        
        iSendFileInCall = ETrue;
        PRINT( _L("CCamAppUi::ProcessCommandParametersL iSendFileInCall = ETrue") );
        // Make sure that the static settings are loaded
        iController.LoadStaticSettingsL( ETrue );
        }*/
    return EFalse;
    }

// ---------------------------------------------------------
// CCamAppUi::HandleMessageL
// Bring Camera app to foreground in response to request for in-call send
// ---------------------------------------------------------
//
MCoeMessageObserver::TMessageResponse CCamAppUi::HandleMessageL(
    TUint32 aClientHandleOfTargetWindowGroup,
    TUid aMessageUid,
    const TDesC8& aMessageParameters )
    {
    PRINT( _L("CCamAppUi::HandleMessageL") );
    // Go to SFI mode at run-time if requested by SFIUtils
    // using a window server message.
    if ( aMessageUid.iUid == 0 && aMessageParameters.Length() > 0 )
        {
        TInt lengthInCharacters = 
            ( aMessageParameters.Length() * sizeof( TUint8 ) ) /
            sizeof( TUint16 );
        TPtrC message(
            reinterpret_cast<const TUint16*>( aMessageParameters.Ptr() ),
            lengthInCharacters );

        /*if ( message == KSFITrailing )
            {
            if ( !SFIUtility() )
                {
                // Jump to foreground anyway, even if SFI mode cannot
                // be activated.
                iEikonEnv->RootWin().SetOrdinalPosition( 0, 0 ); 
                return CAknAppUi::HandleMessageL( 
                    aClientHandleOfTargetWindowGroup, 
                    aMessageUid, 
                    aMessageParameters );
                }

            SFIUtility()->IsCLIValidL();
            
            iSendFileInCall = ETrue;
            PRINT( _L("CCamAppUi::HandleMessageL iSendFileInCall = ETrue") );

            // Jump to foreground
            iEikonEnv->RootWin().SetOrdinalPosition( 0, 0 ); 
            
            // load embedded static settings
            iController.LoadStaticSettingsL( ETrue );
            }*/
        }
    return CAknAppUi::HandleMessageL( aClientHandleOfTargetWindowGroup, 
        aMessageUid, aMessageParameters);
    }

#endif // HANDLE_TRANSITIONAL_UI    

// ---------------------------------------------------------
// CCamAppUi::SidePane
// Return reference to side-pane object
// ---------------------------------------------------------
//
CCamSidePane* CCamAppUi::SidePane()
    {
    return iCamSidePane;
    }

// ---------------------------------------------------------
// CCamAppUi::ZoomPane
// Return reference to zoom-pane object
// ---------------------------------------------------------
//
CCamZoomPane* CCamAppUi::ZoomPane()
    {
    return iCamZoomPane;
    }
    
// ---------------------------------------------------------
// CCamAppUi::CurrentViewState
// Return current view state
// ---------------------------------------------------------
//        
TCamViewState CCamAppUi::CurrentViewState() const
    {
    return iViewState;
    }

// ---------------------------------------------------------
// CCamAppUi::TargetViewState
// Returns target view state
// ---------------------------------------------------------
//        
TCamViewState CCamAppUi::TargetViewState()
    {
    return iTargetViewState;
    }

// ---------------------------------------------------------
// CCamAppUi::TargetMode
// ---------------------------------------------------------
//  
TCamCameraMode CCamAppUi::TargetMode() const
    {
    return iTargetMode;
    }

// ---------------------------------------------------------
// CCamAppUi::HandleShutterKeyL
// Handles the shutter key press
// ---------------------------------------------------------
//   
void CCamAppUi::HandleShutterKeyL( TBool aPressed )
    {
    // only handle the shutter key if the app is
    // in the foreground and view finder is active
    if ( !AppInBackground( ETrue ) )
        {
        RArray<TInt> captureKeys;
        CleanupClosePushL( captureKeys );
        
        if ( iController.UiConfigManagerPtr() )
            {
            iController.UiConfigManagerPtr()->
                 SupportedPrimaryCameraCaptureKeyL( captureKeys );
            if ( captureKeys.Count() <= 0 )
                {
                User::Leave( KErrNotSupported );
                }
            }
        TKeyEvent keyEvent;
        keyEvent.iScanCode = captureKeys[0];//EProductKeyCapture
        keyEvent.iModifiers = 0;
        keyEvent.iRepeats = 0;

        CleanupStack::PopAndDestroy( &captureKeys );

        // check we have a currently active view
        if( iView )
            {
            CCoeControl* container = static_cast<CCamViewBase*>( iView )->Container();
            if ( container )
                {
                if ( aPressed )
                    {
                    container->OfferKeyEventL( keyEvent, EEventKeyDown ); 
                    }
                else
                    {
                    container->OfferKeyEventL( keyEvent, EEventKeyUp ); 
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// CCamAppUi::IsHeadsetConnected
// Return whether headset is connected
// ---------------------------------------------------------
// 
TBool CCamAppUi::IsHeadsetConnected() const
    {
    TBool connected( EFalse );
    
    TRAP_IGNORE(
        {
        CAccMonitor* monitor = CAccMonitor::NewLC();
        RConnectedAccessories accessories;
        CleanupClosePushL( accessories );
          
        TAccMonCapability device = KAccMonNoDevice;
            
        monitor->GetConnectedAccessoriesL( accessories );
        TInt count = accessories.Count();
           
        // loop through connected accessories
        for ( TInt i = 0; i != count; i++ )
            {
            device = accessories[i]->AccDeviceType();
            // headset device type
            if ( device == KAccMonHeadset )
                {
                connected = ETrue;
                break;
                }
            }
           
        CleanupStack::PopAndDestroy( &accessories );
        CleanupStack::PopAndDestroy( monitor );     
        });
                
    return connected;
    }

// ---------------------------------------------------------------------------
// CCamAppUi::HandleVolumeKeyEvent
// 
// ---------------------------------------------------------------------------
//    
void CCamAppUi::HandleVolumeKeyEvent( TRemConCoreApiOperationId aOperationId,
                                  TRemConCoreApiButtonAction aButtonAct )
    {
    PRINT2( _L("Camera => CCamAppUi::HandleVolumeKeyEvent op (%d) act (%d)"), aOperationId, aButtonAct )
    
    if ( !iZoomUsingVolumeKeys || IsHeadsetConnected() )
        {
        PRINT( _L("Camera <= CCamAppUi::HandleVolumeKeyEvent NOT zooming with volume keys") )
        return;
        }
    // only handle the shutter key if the app is
    // in the foreground and view finder is active
    if ( !AppInBackground( ETrue ) )
        {
        PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent yes"))
        // check we have a currently active view
        if( iView && ( iCamOrientation == ECamOrientationCamcorder
                || iCamOrientation == ECamOrientationCamcorderLeft) &&
                ECamViewStatePreCapture == iViewState ) // Zoom only needed in precap
            {
            CCoeControl* container = static_cast<CCamViewBase*>( iView )->Container();
            if ( container )
                {
                PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent cont"))
                TKeyEvent keyEvent;
                keyEvent.iModifiers = 0;
                keyEvent.iRepeats = 0; 

                // map up/down to appropriate zoom key
                if( aOperationId == ERemConCoreApiVolumeUp )
                    {
                    PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent up"))
                    keyEvent.iScanCode = EStdKeyIncVolume;
                    }
                else if( aOperationId == ERemConCoreApiVolumeDown )
                    {
                    PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent down"))
                    keyEvent.iScanCode = EStdKeyDecVolume;
                    }
                else
                    {
                    PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent ??"))
                    }
                // map event type
                if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent press"))
                    TRAP_IGNORE( container->OfferKeyEventL( keyEvent, EEventKeyDown ) ); 
                    }
                else if( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent release"))
                    TRAP_IGNORE( container->OfferKeyEventL( keyEvent, EEventKeyUp ) );
                    }
                else if( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent click"))
                    TRAP_IGNORE( container->OfferKeyEventL( keyEvent, EEventUser ) ); 
                    }        
                else
                    {
                    PRINT( _L("Camera <> CCamAppUi::HandleVolumeKeyEvent unhandled volume keypress"))
                    }
                }
            }
        }
    PRINT( _L("Camera <= CCamAppUi::HandleVolumeKeyEvent"))
    }

  
// ---------------------------------------------------------
// CCamAppUi::SetEmbedding
// Sets embedding flag
// ---------------------------------------------------------
//  
void CCamAppUi::SetEmbedding( TBool aEmbedding )
    {
    PRINT1( _L("Camera => CCamAppUi::SetEmbedding %d"), aEmbedding )
    iEmbedding = aEmbedding;
    PRINT( _L("Camera <= CCamAppUi::SetEmbedding") )
    }
    
// ---------------------------------------------------------
// CCamAppUi::Embedding
// Returns embedding flag
// ---------------------------------------------------------
// 
TBool CCamAppUi::Embedding() const
    {
    PRINT1( _L("Camera => CCamAppUi::Embedding %d"), iEmbedding )
    PRINT( _L("Camera <= CCamAppUi::Embedding") )
    return iEmbedding;
    }    
    
// ---------------------------------------------------------
// CCamAppUi::SetSendAsInProgressL
// Sets the SendAs flag
// ---------------------------------------------------------
// 
void CCamAppUi::SetSendAsInProgressL( TBool aSendAsInProgress )
    {
    PRINT1( _L("Camera => CCamAppUi::SetSendAsInProgressL %d"), aSendAsInProgress );
    // Check before updating embedding status.
    TBool exitStatus = iController.CheckExitStatus();

    iSendAsInProgress = aSendAsInProgress;
    SetEmbedding( aSendAsInProgress );

    if ( !iSendAsInProgress )
        {
        PRINT( _L("Camera <> CCamAppUi::SetSendAsInProgressL - checking exit status..") );
        // Exit not wanted when embedding / embedded
        // Check if camera switch is required.        
        if ( exitStatus )
            {
            InternalExitL();   
            }
        else if ( iPendingViewSwitch != EPendingViewSwitchNone )
            {
            iPendingViewSwitch = EPendingViewSwitchNone;        
            iWaitTimer->Start( 0, 0,  TCallBack( CallSwitch, this ) ); 
            }  
        else
            {
            // Do nothing
            }                   
        }
    PRINT( _L("Camera <= CCamAppUi::SetSendAsInProgressL") );
    }     

// ---------------------------------------------------------
// CCamAppUi::SendAsInProgressL
// Get the SendAs flag value
// ---------------------------------------------------------
// 
TBool CCamAppUi::SendAsInProgress() const
    {
    return iSendAsInProgress;
    }


// ---------------------------------------------------------
// CCamAppUi::IsMMCRemoveNotePending
// Return whether an MMC removal note is pending
// ---------------------------------------------------------
//
TBool CCamAppUi::IsMMCRemovedNotePending() const
    {
    if ( iMMCRemoveNoteRequired || iMMCRemoveNoteRequiredOnCapture )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CCamAppUi::HandleCameraErrorL
// Display an error note
// ---------------------------------------------------------
//
void CCamAppUi::HandleCameraErrorL( TInt aError )
    {
    if( aError == KErrNoMemory )
        {
        PRINT( _L( "Camera => CCamAppUi::HandleCameraErrorL KErrNoMemory" ) );
        HBufC* text = StringLoader::LoadLC( R_CAM_MEMLO_NOT_ENOUGH_MEMORY );
        // Display an error note 
        CAknErrorNote* dlg = new (ELeave) CAknErrorNote( ETrue );
        dlg->ExecuteLD( *text );

        CleanupStack::PopAndDestroy( text );
        }
    else if ( aError )
        {
        PRINT1( _L( "Camera => CCamAppUi::HandleCameraErrorL (%d)" ), aError );

        // Resolve the error text with no context or separator
        TPtrC buf;
        buf.Set( iTextResolver->ResolveErrorString( aError, CTextResolver::ECtxNoCtxNoSeparator ) );             
        
        // Display an error note 
        CAknErrorNote* dlg = new (ELeave) CAknErrorNote( ETrue );
        dlg->ExecuteLD( buf );

        PRINT( _L( "Camera <= CCamAppUi::HandleCameraErrorL" ) );
        }
    else
        {
        }
    }
    
// ---------------------------------------------------------
// CCamAppUi::TextResolver
// Offers a reference to appui's TextResolver instance
// ---------------------------------------------------------
// 
CTextResolver* CCamAppUi::TextResolver()
    {
    return iTextResolver;
    } 


#ifndef __WINS__
// ---------------------------------------------------------
// CCamAppUi::SFIUtility
// Return pointer to in-call send utility object
// ---------------------------------------------------------
//
/*CSFIUtilsAppInterface* CCamAppUi::SFIUtility() const
    {
    return iSFIUtils;
    }*/

// ---------------------------------------------------------
// CCamAppUi::IsInCallSend
// Return whether in in-call send mode
// ---------------------------------------------------------
//
/*TBool CCamAppUi::IsInCallSend() const
    {
    return iSendFileInCall;
    }*/

// ---------------------------------------------------------------------------
// CCamAppUi::DoInCallSendL
// Handle send to caller functionality
// ---------------------------------------------------------------------------
//
/*void CCamAppUi::DoInCallSendL() const
    {
    PRINT( _L("CCamAppUi::DoInCallSendL in") );
    if ( iSFIUtils && iSFIUtils->IsCLIValidL() )
        {
        PRINT( _L("CCamAppUi::DoInCallSendL send") );
        iSFIUtils->SendMediaFileL( iController.CurrentFullFileName() );
        }
    PRINT( _L("CCamAppUi::DoInCallSendL out") );
    }  */

// ---------------------------------------------------------------------------
// CCamAppUi::BringPhoneAppToForeGroundL
// Bring phone application to foreground
// ---------------------------------------------------------------------------
//
void CCamAppUi::BringPhoneAppToForeGroundL()
    {
    PRINT( _L("CCamAppUi::BringPhoneAppToForeGroundL") );
    TApaTaskList apaTaskList( CCoeEnv::Static()->WsSession() );
    TApaTask apaTask = apaTaskList.FindApp( KPhoneAppUid );
    if ( apaTask.Exists() )
        {
        apaTask.BringToForeground();
        }

    // load non-embedded static settings
    iController.LoadStaticSettingsL( EFalse );
    // no longer in SFI state
//    iSendFileInCall = EFalse;

    // set us back to pre-capture
    iTargetViewState = ECamViewStatePreCapture;   	
	// when camera is used again, TrySwitchViewL() will be called
    iPendingViewSwitch = EPendingViewSwitchDeactivateFirst; 

    PRINT( _L("CCamAppUi::BringPhoneAppToForeGroundL iSendFileInCall = EFalse") );
    }

#endif

// ---------------------------------------------------------
// CCamAppUi::IsInPretendExit
// Indicates whether or not the application is in a simulated exit situation
// ---------------------------------------------------------
// 
TBool CCamAppUi::IsInPretendExit() const
    {
    return iPretendExit;
    }  

// ---------------------------------------------------------------------------
// CCamAppUi::SendCameraAppToBackgroundL
// Sends the camera application to the background, to pretend we're closing
// ---------------------------------------------------------------------------
//
void CCamAppUi::SendCameraAppToBackgroundL()
    {
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, CCAMAPPUI_SENDCAMERAAPPTOBACKGROUNDL, "e_CCamAppUi_SendCameraAppToBackgroundL 1" );
    
    PRINT( _L("Camera => CCamAppUi::SendCameraAppToBackgroundL") );
    // Pretend we're closing, so reset the dynamic settings.
    iController.SetDynamicSettingsToDefaults();

    // stop listening mmc dismount notifications
    iController.CancelDismountMonitoring();
    
    // Switch sequence capture off if the app is going to pretend to close
    if ( IsBurstEnabled() )
        {
        PRINT( _L("CCamAppUi::SendCameraAppToBackgroundL calling ToggleMulti") );
        SwitchStillCaptureModeL( ECamImageCaptureSingle, EFalse );
        }

    // Send the task with the given WgId to the background 
    CAknSgcClient::MoveApp( iMyWgId, ESgcMoveAppToBackground );

    // Hide from task list/FSW and send to background
    HideTaskL( ETrue );    		
    iPretendExit = ETrue;
    iFirstVFStart = ETrue;

    PRINT( _L("Camera <= CCamAppUi::SendCameraAppToBackgroundL") );
    OstTrace0( CAMERAAPP_PERFORMANCE_DETAIL, DUP1_CCAMAPPUI_SENDCAMERAAPPTOBACKGROUNDL, "e_CCamAppUi_SendCameraAppToBackgroundL 0" );

    }

// ---------------------------------------------------------------------------
// CCamAppUi::HideTaskL
// Hides or shows the Camera app task in the tasklist
// ---------------------------------------------------------------------------
// 
void CCamAppUi::HideTaskL(TBool aHide)
    {
    // HideApplicationFromFSW also hides running status from menu grid
   	HideApplicationFromFSW( aHide );

	// The following ensures that the task is hidden even if an embedded camera app is also running    
    RWsSession session = iEikonEnv->WsSession();
    CApaWindowGroupName* wgName = CApaWindowGroupName::NewL( session, iMyWgId );
    wgName->SetHidden( aHide );
    wgName->SetWindowGroupName( iEikonEnv->RootWin() );
    delete wgName;
    }
    
// ---------------------------------------------------------
// CCamAppUi::StartAsServerAppL
// Start application as file server application
// ---------------------------------------------------------
//
void 
CCamAppUi::StartAsServerAppL( MCamEmbeddedObserver* aEmbeddedObserver, 
                              TInt                  aMode )
    {       
    PRINT( _L("Camera => CCamAppUi::StartAsServerAppL") );  

    __ASSERT_ALWAYS( aMode == ECamControllerVideo || aMode == ECamControllerImage, 
            CamPanic( ECamPanicNotSupported ) );

    // Load Embedded Settings
    iController.LoadStaticSettingsL( ETrue );
    iEmbeddedViewSet = ETrue;     
    iEmbeddedObserver = aEmbeddedObserver; 
    iTargetViewState = ECamViewStatePreCapture;


    if ( aMode == ECamControllerVideo )
        {
        iTargetMode = ECamControllerVideo;
        }
    else if ( aMode == ECamControllerImage )
        {
        iTargetMode = ECamControllerImage;
        }

    // Find the parent app's name:

    RWsSession ws = iEikonEnv->WsSession();

    // Get a list of the names and IDs of the all the window groups
    RArray<RWsSession::TWindowGroupChainInfo> windowList;
    CleanupClosePushL(windowList);

    // Get a list of window group IDs
    User::LeaveIfError(ws.WindowGroupList(&windowList));

    // Find our window group ID
    RThread thread;
    TInt cameraWindowGroup = ws.FindWindowGroupIdentifier(0, thread.Id());
    thread.Close();

    TInt parentID = 0;
    TBool found = EFalse;

    // Find our window ID in the list, to look up the parent ID
    for( TInt i = 0; !found && i < windowList.Count(); i++ )
        {
        RWsSession::TWindowGroupChainInfo windowInfo = windowList[i];
        if ( windowInfo.iId == cameraWindowGroup )
            {
            parentID = windowInfo.iParentId;
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy(&windowList);

    if ( found )
        {
        TBuf<100> windowName;
        ws.GetWindowGroupNameFromIdentifier( parentID, windowName );

        // The format of windowName is:
        // [number][zero-char][UID of app][zero-char][Title of window][zero-char]
        // We want to discard everything up to the window title:
        for ( TInt t = 0; t  < 2; t++ )
            {
            TInt zeroPos = windowName.Locate(0);

            if ( zeroPos != KErrNotFound 
                    && zeroPos < windowName.Length() )
                {
                windowName.Delete(0, zeroPos + 1);
                }
            }

        // And the final zero-char
        if ( windowName.Length() > 0 )
            {
            if ( windowName[windowName.Length() - 1] == 0 )
                {
                windowName.Delete(windowName.Length() - 1, 1);
                }
            }

        if ( windowName.Length() > 0 )
            {
            iParentAppName = HBufC::NewL(windowName.Length());
            iParentAppName->Des().Copy(windowName);

            SetTitleL(windowName);
            }
        else
            {
            // Something's wrong - act as if we haven't found it
            found = EFalse;
            }
        }

    // We were unable to find the parent app's name, so fall back on our own app title        
    if( !found )
        {
        iParentAppName = StringLoader::LoadL(R_CAM_GENERIC_TITLE_NAME);
        }

    // make sure standby exits as this may not actually result in 
    // a view switch if the correct view is already activated
    if( iView )
        {
        iView->HandleCommandL( ECamCmdExitStandby );
        }
    iViewState = ECamViewStatePreCapture;    
  iTargetViewState = ECamViewStatePreCapture;   	
    TrySwitchViewL();                                      


    PRINT( _L("Camera <= CCamAppUi::StartAsServerAppL") );
    }
           

// ---------------------------------------------------------------------------
// CCamAppUi::CamOrientation
// Returns the current orientation of the app
// ---------------------------------------------------------------------------
//
TCamOrientation CCamAppUi::CamOrientation()
    {
    return iCamOrientation;
    }

// ---------------------------------------------------------------------------
// CCamAppUi::SetCamOrientationToLandscape()
// Set the current orientation as landscape
// ---------------------------------------------------------------------------
//
void CCamAppUi::SetCamOrientationToLandscape()
    {
    iCamOrientation = (ECamHandLeft == iSoftkeyPosition ) 
                    ? ECamOrientationCamcorderLeft 
                    : ECamOrientationCamcorder;
    }

// ---------------------------------------------------------------------------
// CCamAppUi::AppUiOrientation
// Returns the AppUi orientation (e.g. Portrait or Landscape) for the given product-specific orientation
// ---------------------------------------------------------------------------
//
CAknAppUiBase::TAppUiOrientation CCamAppUi::AppUiOrientation(TCamOrientation aCamOrientation)
    {
    switch ( aCamOrientation )
        {
        case ECamOrientationCamcorder: // fallthrough
        case ECamOrientationCamcorderLeft:
            {
            return CAknAppUiBase::EAppUiOrientationLandscape;
            }
        case ECamOrientationViewMode:
            {
            return CAknAppUiBase::EAppUiOrientationLandscape;
            }
        case ECamOrientationPortrait:
            {
            return CAknAppUiBase::EAppUiOrientationPortrait;
            }
        default:
            {
            return CAknAppUiBase::EAppUiOrientationUnspecified;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCamAppUi::LayoutStateToOrientation
// Returns the camera orientation based on the layout state set by the phone
// ---------------------------------------------------------------------------
//
TCamOrientation CCamAppUi::LayoutStateToOrientation( TInt aLayoutState )
    {
    TCamOrientation orientation = ECamOrientationDefault;

#ifdef __WINS__  
    switch(aLayoutState)
        {
        // Emulator numbers
        case 2:
            orientation = ECamOrientationPortrait;
            break;
        case 6:
            if( iSoftkeyPosition == ECamHandLeft )
                {
                orientation = ECamOrientationCamcorderLeft;
                }
            else
                {
                orientation = ECamOrientationCamcorder;
                }
            break;
            }
#else // !__WINS__  
    switch(aLayoutState)
        {
        // Magic number alert!
        // These numbers come from AknPriv.rss, where they are currently hardcoded
        case 0:
            orientation = ECamOrientationPortrait;
            break;
        case 1:
            if( iSoftkeyPosition == ECamHandLeft )
                {
                orientation = ECamOrientationCamcorderLeft;
                }
            else
                {
                orientation = ECamOrientationCamcorder;
                }
            break;
        case 2:
            orientation = ECamOrientationViewMode;
            break;
        case 3:
            orientation = ECamOrientationPortrait;
            break;
        default:
            orientation = ECamOrientationDefault;
            break;
        }
#endif // __WINS__

    return orientation;
    }


// ---------------------------------------------------------------------------
// CCamAppUi::ReadCurrentDeviceOrientation
// Returns the camera orientation based on the layout state set by the phone
// ---------------------------------------------------------------------------
//
TCamOrientation CCamAppUi::ReadCurrentDeviceOrientation()
    {
    PRINT( _L("Camera <> CCamAppUi::ReadCurrentDeviceOrientation") )
    return ECamOrientationDefault;
    }

// ---------------------------------------------------------------------------
// CCamAppUi::ActivePalette
// Return handle to Active Palette
// ---------------------------------------------------------------------------
//
MActivePalette2UI* CCamAppUi::ActivePalette() const
    {
    return iActivePaletteHandler->ActivePalette();
    }

// ---------------------------------------------------------------------------
// CCamAppUi::APHandler
// Returns pointer to Active Palette handler
// ---------------------------------------------------------------------------
//   
CCamActivePaletteHandler* CCamAppUi::APHandler() const
    {
    return iActivePaletteHandler;
    }

// ---------------------------------------------------------------------------
// CCamAppUi::DrawPreCaptureCourtesyUI
// Returns whether the optional precapture UI should be currently shown
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::DrawPreCaptureCourtesyUI() const
  {
  if ( CourtesyUiApplies() )
    {
    return iDrawPreCaptureCourtesyUI;
    }
  else
    {
    // Otherwise always draw the UI
    return ETrue;
    }
  }
    
// ---------------------------------------------------------------------------
// CCamAppUi::DrawPostCaptureCourtesyUI
// Returns whether the optional postcapture UI should be currently shown
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::DrawPostCaptureCourtesyUI() const
    {
    if ( CourtesyUiApplies() )
        {
        return iDrawPostCaptureCourtesyUI;
        }
    else
        {
        // Otherwise always draw the UI
        return ETrue;
        }
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::AlwaysDrawPreCaptureCourtesyUI
// Returns whether the optional precapture UI should always be shown
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::AlwaysDrawPreCaptureCourtesyUI() const
    {
    return iAlwaysDrawPreCaptureCourtesyUI;
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::AlwaysDrawPostCaptureCourtesyUI
// Returns whether the optional postcapture UI should always be shown
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::AlwaysDrawPostCaptureCourtesyUI() const
    {
    return iAlwaysDrawPostCaptureCourtesyUI;
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::SetAlwaysDrawPreCaptureCourtesyUI
// Set whether the optional precapture UI should always be shown
// ---------------------------------------------------------------------------
//
void  CCamAppUi::SetAlwaysDrawPreCaptureCourtesyUI( TBool aNewSetting )
    {
    iAlwaysDrawPreCaptureCourtesyUI = aNewSetting;
    
    if ( aNewSetting )
        {
        RaisePreCaptureCourtesyUI(EFalse);
        }
    else
        {
        SubmergePreCaptureCourtesyUI();
        }
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::SetAlwaysDrawPostCaptureCourtesyUI
// Set whether the optional postcapture UI should always be shown
// ---------------------------------------------------------------------------
//
void  CCamAppUi::SetAlwaysDrawPostCaptureCourtesyUI(TBool aNewSetting)
    {
    iAlwaysDrawPostCaptureCourtesyUI = aNewSetting;

    if ( aNewSetting )
        {
        RaisePostCaptureCourtesyUI();
        }
    else
        {
        SubmergePostCaptureCourtesyUI();
        }
    }

// ---------------------------------------------------------------------------
// CCamAppUi::CourtesyTimeout
// Called after 15 seconds of key inactivity
// ---------------------------------------------------------------------------
//
TInt CCamAppUi::CourtesyTimeout( TAny* aPtr )
    {
    return static_cast<CCamAppUi*>( aPtr )->DoCourtesyTimeout();
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::DoCourtesyTimeoutL
// Called after 15 seconds of key inactivity
// ---------------------------------------------------------------------------
//
TInt CCamAppUi::DoCourtesyTimeout()
    {
    if ( iCourtesyTimerInPreCapture )
        {
        if ( iController.IsTouchScreenSupported() )
            {
            CAknToolbar* toolbar = CurrentFixedToolbar();
            if ( toolbar )
                {
                CAknToolbarExtension* toolbarextension =
                    toolbar->ToolbarExtension();
                if ( toolbarextension && toolbarextension->IsShown() )
                    {
                    // Do not hide courtesy UI when toolbar extension is open

                    return EFalse;
                    }
                }
            }
        iDrawPreCaptureCourtesyUI = EFalse;
        SubmergePreCaptureCourtesyUI();
        }
    
    iCourtesyTimer->Cancel();

    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::CheckCourtesyKeyEventL
// Check whether the current key event should raise the optional UI
// ---------------------------------------------------------------------------
//
TKeyResponse CCamAppUi::CheckCourtesyKeyEventL( 
            const TKeyEvent aKeyEvent, 
            TEventCode aType, 
            TBool aInPreCapture )
    {
    PRINT(_L("Camera => CheckCourtesyKeyEventL"))

    TKeyResponse retVal = EKeyWasNotConsumed;

    if ( CourtesyUiApplies() )
        {
        // Always reset the timer on any key event, if appropriate
        if(    (   (  aInPreCapture && !iAlwaysDrawPreCaptureCourtesyUI  )
               ||  ( !aInPreCapture && !iAlwaysDrawPostCaptureCourtesyUI )
               )
           &&
               (   ( aInPreCapture && iDrawPreCaptureCourtesyUI  )
               ||  (!aInPreCapture && iDrawPostCaptureCourtesyUI )
               )
          )
            {
            iCourtesyTimer->Cancel();
            iCourtesyTimer->StartTimer();
            iCourtesyTimerInPreCapture = aInPreCapture;
            }
        // Here we check if the LSK key is pressed by using the scan code
        // We do this, to show the active tool bar when LSK is pressed when 
        // the AP tool bar is hidden AND Pressing "RSK" shall have to return
        // to PreCaptureView (Image/Video mode)
        // LSK  --> Left Softkey 
        // RSK  --> Right Softkey        
        if ( aType == EEventKeyDown && 
             aKeyEvent.iScanCode == EStdKeyDevice0 )
        	{
        	iLeftSoftKeyPressed = ETrue;
        	}
        else
        	{
        	iLeftSoftKeyPressed = EFalse;
        	}
 
        if ( iController.UiConfigManagerPtr() && 
             !iController.UiConfigManagerPtr()->IsLocationSupported() )
            {
			if ( aType == EEventKey && 
           	  	aKeyEvent.iScanCode == EStdKeyNo )
        		{
        		iEndKeyPressed = ETrue;
        		}
        	else
        		{
        		iEndKeyPressed = EFalse;
        		}
            }

        const TCamCaptureOperation operation( iController.CurrentOperation() );        
        if ( aType == EEventKey )
            {
            if ( EStdKeyRightArrow == aKeyEvent.iScanCode && !iController.IsTouchScreenSupported() )
                 {
                 if( aInPreCapture )
                     { 
                     // during sequence capturing, Navi-Right is inactive to affect CourtesyUI
                     if ( !( IsBurstEnabled() && ( ECamCapturing == operation || ECamCompleting == operation ) )  )
                         {
                         iDrawPreCaptureCourtesyUI = EFalse;	
                         if ( !IsSecondCameraEnabled() )
                             {                         
                             SubmergePreCaptureCourtesyUI();
                             }
                         else if ( !IsEmbedded() )
                             {
                             SetActivePaletteVisibility( EFalse );
                             if ( ECamControllerVideo == iMode )
                                 {
                                 SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__RECORD_TXT );
                                 }
                             else
                                 {
                                 SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE_TXT );
                                 }
                             }
                         }
                     }
                 else
                     {
                     iDrawPostCaptureCourtesyUI = EFalse;	
                     if ( !IsSecondCameraEnabled() )
                         {                                              
                         SubmergePostCaptureCourtesyUI();
                         }
                     else
                         {
                         SetActivePaletteVisibility( EFalse );
                         }
                     }
                 return EKeyWasConsumed;
                 }
                 
             else if ( (  EStdKeyLeftArrow == aKeyEvent.iScanCode
                     || (!aInPreCapture && EStdKeyDevice3 == aKeyEvent.iScanCode && !IsToolBarVisible() ) ) 
                     && !iController.IsTouchScreenSupported() )
                 {
                 iRotatedKeyEvent = ETrue;
                 if( aInPreCapture )
                     {
                     // during sequence capturing, Navi-Left is inactive to affect CourtesyUI
                     if ( !( IsBurstEnabled() && ( ECamCapturing == operation || ECamCompleting == operation ) )  )
                         {
                         iDrawPreCaptureCourtesyUI = ETrue;
                         RaisePreCaptureCourtesyUI(EFalse);
                         
                         if ( /*IsSecondCameraEnabled() 
                                &&*/ !IsEmbedded() )
                             {
                             if ( ECamControllerVideo == iMode )
                                 {
                             IsSecondCameraEnabled()?SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__RECORD_SECONDARY ):
                             SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__RECORD );
                                 }
                             else
                                 {
                                  IsSecondCameraEnabled()?SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE_SECONDARY ):
                                                          SetSoftKeysL( R_CAM_SOFTKEYS_OPTIONS_EXIT__CAPTURE ); 
                                 }
                             }
                         }
                     }
                 else
                     {
                     iDrawPostCaptureCourtesyUI = ETrue;
                     RaisePostCaptureCourtesyUI();
                     }
                 return EKeyWasConsumed;
                 }
            // Only check if the user has turned the UI off
            if(   (  aInPreCapture && !iAlwaysDrawPreCaptureCourtesyUI )
               || ( !aInPreCapture && !iAlwaysDrawPostCaptureCourtesyUI ))
                {
                // Decide whether to turn the UI back on        
                if(    ( aInPreCapture && !iDrawPreCaptureCourtesyUI)
                    || (!aInPreCapture && !iDrawPostCaptureCourtesyUI))
                    {
                    if (   aKeyEvent.iScanCode == EStdKeyUpArrow 
                        || aKeyEvent.iScanCode ==  EStdKeyDownArrow
                        || aKeyEvent.iScanCode ==  EStdKeyDevice3 
                        // Navikey select
                       )
                        {
                        if ( aInPreCapture )
                            {
                            RaisePreCaptureCourtesyUI(EFalse);
                            }
                        else
                            {
                            RaisePostCaptureCourtesyUI();
                            }
                        retVal = EKeyWasConsumed;
                        }
                    }
#ifndef __WINS__
                RArray <TInt> halfCaptureKeys;
                CleanupClosePushL( halfCaptureKeys );

                // now get half key press code to register
                if ( iController.UiConfigManagerPtr() )
                    {
                    iController.UiConfigManagerPtr()->
                                SupportedPrimaryCameraAutoFocusKeyL( halfCaptureKeys );
                    }
                if ( halfCaptureKeys.Count() > 0 )
                    {
                    if ( aInPreCapture && aKeyEvent.iScanCode 
                        == halfCaptureKeys[0] )
                        {
                        SubmergePreCaptureCourtesyUI();
                        retVal = EKeyWasConsumed;
                        }                    
                    }
                CleanupStack::PopAndDestroy( &halfCaptureKeys );
#else
                if ( aInPreCapture && aKeyEvent.iScanCode 
                    == EProductKeyCaptureHalf )
                    {
                    SubmergePreCaptureCourtesyUI();
                    retVal = EKeyWasConsumed;
                    }
#endif 
                }
            }
        }
    PRINT(_L("Camera <= CheckCourtesyKeyEvent"))
    
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::RaisePreCaptureCourtesyUI
// Shows the optional precapture UI
// ---------------------------------------------------------------------------
//
void CCamAppUi::RaisePreCaptureCourtesyUI(TBool aShowZoomBar)
    {
    PRINT1(_L("Camera => CCamAppUi::RaisePreCaptureCourtesyUI \
         aShowZoomBar = %d"), aShowZoomBar);
    
    if ( CourtesyUiApplies() && iDrawPreCaptureCourtesyUI )
        {

        iDrawPreCaptureCourtesyUI = ETrue;
        
        if ( iCamZoomPane && aShowZoomBar && !iCamZoomPane->IsZoomAtMinimum() )  
            {
            PRINT(_L("Camera == CCamAppUi::RaisePreCaptureCourtesyUI \
                Showing zoom pane"));
            
            CCamViewBase* view = NULL;
            
            if ( iMode == ECamControllerVideo )
                {
                view = iVideoCaptureView;
                }
            else
                {
                view = iStillCaptureView;
                }
            
            if ( view )
                {
                CCamContainerBase* container = view->Container();
                
                if ( container )
                    {
                    container->ShowZoomPaneWithTimer();
                    }
                }
            }
        else
            {
            if ( iZoomPaneShown && iRotatedKeyEvent )
                {
                HideZoomPane(ETrue);
                iRotatedKeyEvent = EFalse;
                }
            }
        PRINT(_L("Camera == CCamAppUi::RaisePreCaptureCourtesyUI Showing AP"));
        SetActivePaletteVisibility( ETrue );
        
        TUid settingsPluginUid = KNullUid;

  		  if ( iPlugin )
    		    {
    		    settingsPluginUid = iPlugin->Id();  
    		    }

        if ( iView )
            {
            const TUid uid1( iView->Id() );
            const TInt uid( uid1.iUid );          

        if( !iController.InVideocallOrRinging() &&
          ( ECamViewIdStillPreCapture  == uid ||
            ECamViewIdStillPostCapture == uid || 
            ECamViewIdVideoPreCapture  == uid || 
            ECamViewIdVideoPostCapture == uid ||
            ECamViewIdBurstThumbnail   == uid )
          )
                {
                TRAP_IGNORE( static_cast<CCamViewBase*>
                     ( iView )->UnsetCourtesySoftKeysL() );    
                }
            }
            
        TRAP_IGNORE(HandleCommandL(ECamCmdRedrawScreen));
            
        iCourtesyTimerInPreCapture = ETrue;
        
        if ( iCourtesyTimer )
            {
            iCourtesyTimer->Cancel();
            
            if ( !iAlwaysDrawPreCaptureCourtesyUI )
                {
                iCourtesyTimer->StartTimer();
                }
            }
        }
    PRINT(_L("Camera <= CCamAppUi::RaisePreCaptureCourtesyUI"))
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::RaisePostCaptureCourtesyUI
// Shows the optional postcapture UI
// ---------------------------------------------------------------------------
//
void CCamAppUi::RaisePostCaptureCourtesyUI()
    {
    PRINT(_L("Camera => CCamAppUi::RaisePostCaptureCourtesyUI"))
    if ( CourtesyUiApplies() )
        {
        iDrawPostCaptureCourtesyUI = ETrue;

        SetActivePaletteVisibility( ETrue );

        if ( iView )
            {
            TRAP_IGNORE(static_cast<CCamViewBase*>
                 (iView)->UnsetCourtesySoftKeysL());
            }
            
        TRAP_IGNORE(HandleCommandL(ECamCmdRedrawScreen));
            
        iCourtesyTimerInPreCapture = EFalse;
        
        }
    PRINT(_L("Camera <= CCamAppUi::RaisePostCaptureCourtesyUI"))
    }

// ---------------------------------------------------------------------------
// CCamAppUi::SubmergePreCaptureCourtesyUI
// Hides the optional precapture UI
// ---------------------------------------------------------------------------
//
void CCamAppUi::SubmergePreCaptureCourtesyUI()
    {
    PRINT(_L("Camera => CCamAppUi::SubmergePreCaptureCourtesyUI"))

    const TCamCaptureOperation operation( iController.CurrentOperation() );


    
    if ( ( CourtesyUiApplies() && !iDrawPreCaptureCourtesyUI
        && !SelfTimerEnabled() 
        && ECamViewStatePreCapture == iViewState )
           // and if we are burst capturing 
           // then do not remove the cancel softkey
           && !( IsBurstEnabled() 
              && (ECamCapturing  == operation
                ||ECamCompleting == operation ) ) )
        {
        if ( FullScreenViewfinderEnabled() )
            {
            // Don't submerge if we're capturing time-lapse photos
            if ( !(ECamImageCaptureTimeLapse == CurrentBurstMode()
                && iController.SequenceCaptureInProgress()) )
                {
                
                if ( ECamControllerVideo == iMode )
                	{

	                SetActivePaletteVisibility( EFalse );
	                
	                if ( iController.IsTouchScreenSupported() )
	                    {
	                    // Enable fixed toolbar
	                    CAknToolbar* toolbar = CurrentFixedToolbar();
	                    if ( toolbar )
	                        {
	                        toolbar->SetToolbarVisibility( EFalse );
	                        }
	                    }
	
	                if ( iView )
	                    {
	                    if(!iController.IsTouchScreenSupported())
	                        {
	                    TRAP_IGNORE(static_cast<CCamViewBase*>
	                        (iView)->SetCourtesySoftKeysL());
	                        }
	                    else
	                        {
	                        TRAP_IGNORE(static_cast<CCamViewBase*>
	                            (iView)->UnsetCourtesySoftKeysL());
	                        }
	                    }

                	} // end if ( iMode == ECamControllerVideo )
		        else
		        	{
		        	// in still mode toolbar and softkeys
		        	// are not to be hidden as part of courtesy ui
		        	// imode: ECamControllerIdle, ECamControllerImage,
		        	// or ECamControllerShutdown

		        	SetActivePaletteVisibility( EFalse );
		        	
	                if ( iView )
	                    {
	                    if(!iController.IsTouchScreenSupported())
	                        {
	                        TRAP_IGNORE(static_cast<CCamViewBase*>
	                            (iView)->SetCourtesySoftKeysL());
	                        }
	                    else
	                        {
	                        TRAP_IGNORE(static_cast<CCamViewBase*>
	                             (iView)->UnsetCourtesySoftKeysL());
	                        }
	                    }
		        	
		        	}
                
                TRAP_IGNORE(HandleCommandL(ECamCmdRedrawScreen));	
                }
	            
            } // if ( FullScreenViewfinderEnabled() 
        }               
    PRINT(_L("Camera <= CCamAppUi::SubmergePreCaptureCourtesyUI"))
    }
    
    
// ---------------------------------------------------------------------------
// CCamAppUi::SubmergePostCaptureCourtesyUI
// Hides the optional postcapture UI
// ---------------------------------------------------------------------------
//
void CCamAppUi::SubmergePostCaptureCourtesyUI()
    {
    PRINT(_L("Camera => CCamAppUi::SubmergePostCaptureCourtesyUI"))
    if ( CourtesyUiApplies() 

         && ECamViewStatePostCapture == iViewState )
        {
        iDrawPostCaptureCourtesyUI = EFalse;
        
        SetActivePaletteVisibility( EFalse );

        if ( iView )
            {
            TRAP_IGNORE(static_cast<CCamViewBase*>
                 (iView)->SetCourtesySoftKeysL());
            }

    TRAP_IGNORE(HandleCommandL(ECamCmdRedrawScreen));
        }
    PRINT(_L("Camera <= CCamAppUi::SubmergePostCaptureCourtesyUI"))
    }

// ---------------------------------------------------------------------------
// CCamAppUi::ShowZoomPane
// Needed to show or update the zoom pane in direct viewfinder mode, 
// or where the active toolbar is used.
// ---------------------------------------------------------------------------
//
void CCamAppUi::ShowZoomPane(TBool aRedraw)
    {
    PRINT( _L("Camera => CCamAppUi::ShowZoomPane" ))
    iZoomPaneShown = ETrue;
    
  if ( aRedraw && IsDirectViewfinderActive() )
        {
        TRAP_IGNORE(HandleCommandL(ECamCmdRedrawScreen));
        }
  PRINT( _L("Camera <= CCamAppUi::ShowZoomPane" ))
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::HideZoomPane
// Needed to hide the zoom pane in direct viewfinder mode,
// or where the active toolbar is used.
// ---------------------------------------------------------------------------
//
void CCamAppUi::HideZoomPane( TBool aRedraw )
    {
    PRINT( _L("Camera => CCamAppUi::HideZoomPane" ))
    iZoomPaneShown = EFalse;
    
    if ( iDrawPreCaptureCourtesyUI && iController.IsViewFinding() )
        {
        if ( iViewState == ECamViewStatePreCapture )
            {
            SetActivePaletteVisibility( ETrue );
            }
        }
        
  if ( aRedraw && IsDirectViewfinderActive() )
        {
        TRAP_IGNORE( HandleCommandL(ECamCmdRedrawScreen ) );
        }
  PRINT( _L("Camera <= CCamAppUi::HideZoomPane" ))
    }

// ---------------------------------------------------------------------------
// CCamAppUi::SetActivePaletteVisibility
// Displays or hides the active palette, depending on some inbuilt restraints
// ---------------------------------------------------------------------------
//

void CCamAppUi::SetActivePaletteVisibility( TBool aShown )
  {
  PRINT1( _L("Camera => CCamAppUi::SetActivePaletteVisibility %d" ), aShown )

  if ( !iController.IsTouchScreenSupported() )
      {
      if ( ECamOrientationCamcorder     == iCamOrientation 
        || ECamOrientationCamcorderLeft == iCamOrientation
        || ECamOrientationPortrait == iCamOrientation )
        {
        TCamCaptureOperation operation = iController.CurrentOperation();
     
        // Does the current video mode prevent showing.
        //    
        TBool videoOperationPrevents = 
             ( ECamControllerVideo == iMode )
          && ( ECamCapturing == operation || ECamPaused == operation );

        TBool imageOperationPrevents = 
              ( ECamControllerImage == iMode )
           && ( ECamCapturing == operation || ECamCompleting == operation
                || ECamFocusing == operation || ECamFocused == operation
                || ECamFocusFailed == operation );

        if (  (!aShown )

           || ( aShown && !iPreventActivePaletteDisplay
                       && !TimeLapseSliderShown()
                       && ( iViewState == ECamViewStatePostCapture 
                            || FullScreenViewfinderEnabled()
                            || IsSecondCameraEnabled() )
                       && !videoOperationPrevents
                       && !imageOperationPrevents
                       && ((iDrawPreCaptureCourtesyUI && iViewState == ECamViewStatePreCapture) 
                       || (iDrawPostCaptureCourtesyUI && iViewState == ECamViewStatePostCapture) ))
           )
          {
          if ( iActivePaletteHandler )
            {
            PRINT1(_L("Precapture mode is %d"),iPreCaptureMode)                
            PRINT(_L("Setting Active Palette Visibility"))
            iActivePaletteHandler->SetVisibility( aShown );
            if ( ( !IsEmbedded() && IsSecondCameraEnabled() ) 
                    || !IsSecondCameraEnabled() )
                {
                iToolbarVisibility = aShown;
                }
            else if ( IsEmbedded() && IsSecondCameraEnabled() ) 
                {
                iToolbarVisibility = EFalse;
                }
            }
          }
        }
      }
  PRINT( _L("Camera <= CCamAppUi::SetActivePaletteVisibility" ))
  }


// ---------------------------------------------------------------------------
// CCamAppUi::IsDirectViewfinderActive
// Detects whether the direct viewfinder is in use or not
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::IsDirectViewfinderActive() 
    {
    if ( iController.UiConfigManagerPtr() && 
       ( ( iDSASupported  && iCamOrientation == ECamOrientationCamcorder  || 
           iCamOrientation == ECamOrientationCamcorderLeft ) || 
         ( iController.IsDirectScreenVFSupported( EFalse ) &&
           iCamOrientation == ECamOrientationPortrait ) ) && 
           !( IsBurstEnabled() && iController.SequenceCaptureInProgress() ) )
        {
        return ETrue;
        }
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::UpdateNaviModelsL
// Called to set the correct navicounter / naviprogress controls to "active"
// ---------------------------------------------------------------------------
//
void CCamAppUi::UpdateNaviModelsL( const TBool aUpdateNavi )
    {
    iNaviCounterControl->SetActiveL( EFalse );
    iNaviPaneCounterControl->SetActiveL( EFalse );
    iNaviProgressBarControl->SetActiveL( EFalse );
    iNaviPaneProgressBarControl->SetActiveL( EFalse );
    if ( !aUpdateNavi )
        {
        // We are immediately closing the app or in Pretend Exit
        // No need to update NaviPane
        return;
        }
    else if ( ECamOrientationCamcorder == iCamOrientation 
        || ECamOrientationCamcorderLeft == iCamOrientation
        || ECamOrientationPortrait == iCamOrientation )
        {
        iNaviCounterControl->SetExtentToWholeScreen();
        iNaviCounterControl->SetActiveL( ETrue );
        iNaviProgressBarControl->SetExtentToWholeScreen();
        iNaviProgressBarControl->SetActiveL( ETrue );
        }
    else
        {
        iNaviPaneCounterControl->SetActiveL( ETrue );
        iNaviPaneProgressBarControl->SetActiveL( ETrue );
        }
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::AppController
// Returns the app controller
// ---------------------------------------------------------------------------
//
MCamAppController& CCamAppUi::AppController()
    {
    return iController;
    }

// ---------------------------------------------------------
// CCamAppUi::ForegroundAppIsPhoneApp
// Whether or not the phone app is in the foreground
// ---------------------------------------------------------
//
TBool CCamAppUi::ForegroundAppIsPhoneApp() const
    {
    TBool phoneInForeground = EFalse;
    // Gets the window group id of the app coming in foreground
    TInt windowGroupId = iCoeEnv->WsSession().GetFocusWindowGroup();
    if ( windowGroupId == iPhoneAppWindowGroupId )
        {
        phoneInForeground = ETrue;
        }
    return phoneInForeground;         
    }    
   
// ---------------------------------------------------------------------------
// CCamAppUi::SwitchStillCaptureModeL
// Change the still capture mode
// ---------------------------------------------------------------------------
//    
void 
CCamAppUi::SwitchStillCaptureModeL( TCamImageCaptureMode aImageMode, 
                                    TBool                aReprepare,
                                    TBool                aUpdateUi /*=ETrue*/)
  {
  PRINT( _L( "Camera => CCamAppUi::SwitchStillCaptureModeL" ) )

  iTargetImageMode = aImageMode;   

  if ( iController.UiConfigManagerPtr()->IsXenonFlashSupported() )
      {
      switch( iTargetImageMode )
        {
        case ECamImageCaptureSingle:
        case ECamImageCaptureTimeLapse:
          {
          // Switching from burst mode to single/timelapse mode. 
          // Restore previous original flash setting.
          iController.RestoreFlashMode();
          break;
          }
        case ECamImageCaptureBurst:
          {
          if( iImageMode != iTargetImageMode )
            {
            // Switching from single/timelapse capture to burst mode. 
            // Save current flash mode, and disable flash.  
            iController.SaveFlashMode();
            }           
          iController.SetIntegerSettingValueL( ECamSettingItemDynamicPhotoFlash,
              ECamFlashOff );
          break;
          }
        }
    }
 
  TBool burstEnabled = ( ECamImageCaptureBurst     == aImageMode 
                      || ECamImageCaptureTimeLapse == aImageMode );
      
  if ( iController.TimeLapseSupported() )
    {
    // if burst has been disabled
    // then make sure the timelapse interval has been
    // set back to 0 in the controller.    
    if ( !burstEnabled )
      {
      iController.SetTimeLapseInterval( TInt64(0) );
      }
    }

  if ( iImageMode != iTargetImageMode )    
    {    
    // Inform any observers that burst mode has been changed
    TInt count = iBurstModeObservers.Count();
    TInt i;
    for ( i = 0; i < count; i++ )
      {
      iBurstModeObservers[i]->BurstModeActiveL( burstEnabled, aReprepare );
      }
    }
    
  if( aUpdateUi )
    {
    TrySwitchViewL(); 
    }
  
  iController.BurstModeActiveL( burstEnabled, aReprepare );
     
  if( aUpdateUi )
    {
    iNaviCounterModel->SetCaptureModeL( iMode, iImageMode );   

    if ( aReprepare )
      {
      PRINT( _L( "Camera <> CCamAppUi::SwitchStillCaptureModeL: update AP.." ))
      iActivePaletteHandler->UpdateActivePaletteL();    
      }
    }
  PRINT( _L("Camera <= CCamAppUi::SwitchStillCaptureModeL") )
  }
    
#if !defined(__WINSCW__)
// ---------------------------------------------------------------------------
// CCamAppUi::RegisterCaptureKeysL
// Register to capture half and full shutter press key events
// ---------------------------------------------------------------------------
// 
void CCamAppUi::RegisterCaptureKeysL()
  {
  RArray<TInt> captureKeys;
  CleanupClosePushL( captureKeys );
  
  if ( iController.UiConfigManagerPtr() )
      {
      iController.UiConfigManagerPtr()->
           SupportedPrimaryCameraCaptureKeyL( captureKeys );
      }

  if( iCameraKeyHandle == 0 )
    {
  	iCameraKeyHandle = iCoeEnv->RootWin().CaptureKey( EKeyCamera, 0, 0 );
    }
  
  if( iFullShutterUpDownHandle == 0 )
    {
    if ( captureKeys.Count() > 0 )
        {
        //EProductKeyCapture
        iFullShutterUpDownHandle = iCoeEnv->
            RootWin().CaptureKeyUpAndDowns( captureKeys[0] , 0, 0 );        
        }

    }
  
  if( iFullShutterPressHandle == 0 )
    {
    if ( captureKeys.Count() > 0 )
        {    
        iFullShutterPressHandle = iCoeEnv->
            RootWin().CaptureKey( captureKeys[0], 0, 0 );
        }
    }
    
  if( iHalfShutterPressHandle == 0 )
    {
    captureKeys.Reset();
    
    // now get half key press code to register
    if ( iController.UiConfigManagerPtr() )
        {
        iController.UiConfigManagerPtr()->
        SupportedPrimaryCameraAutoFocusKeyL( captureKeys );
        }
    if ( captureKeys.Count() > 0 )
        {
        iHalfShutterPressHandle = iCoeEnv->
            RootWin().CaptureKey( captureKeys[0], 0, 0 );        
        }
    }
  CleanupStack::PopAndDestroy( &captureKeys );
  }
    
// ---------------------------------------------------------------------------
// CCamAppUi::UnRegisterCaptureKeys
// Release capture of half and full shutter press key events
// ---------------------------------------------------------------------------
// 
void CCamAppUi::UnRegisterCaptureKeys()
  {
  //Since this function can be called from the destructor, we should
  //check that iCoeEnv isn't null prior to using it.
  if( iCoeEnv )
      {
      if ( iCameraKeyHandle > 0 )
            {
            iCoeEnv->RootWin().CancelCaptureKey( iCameraKeyHandle );
            iCameraKeyHandle = 0;
            }
      if ( iFullShutterPressHandle > 0 )
            {
            iCoeEnv->RootWin().CancelCaptureKey( iFullShutterPressHandle );
            iFullShutterPressHandle = 0;
            }
        if ( iHalfShutterPressHandle > 0 )
            {
            iCoeEnv->RootWin().CancelCaptureKey( iHalfShutterPressHandle );
            iHalfShutterPressHandle = 0;
            }
            
        if ( iFullShutterUpDownHandle > 0 )
            {
            iCoeEnv->RootWin().CancelCaptureKey( iFullShutterUpDownHandle );
            iFullShutterUpDownHandle = 0;
            }    
      }
  }
  
#endif

// ---------------------------------------------------------------------------
// CCamAppUi::SetSoftKeysL
// Sets the softkeys to the specified Resource ID
// ---------------------------------------------------------------------------
//
void CCamAppUi::SetSoftKeysL( TInt aResource )
  {
  PRINT1( _L("Camera => CCamAppUi::SetSoftKeysL resource:%x"), aResource );
  if ( iView )
    {
    static_cast<CCamViewBase*>(iView)->SetSoftKeysL(aResource);
    }
  PRINT( _L("Camera <= CCamAppUi::SetSoftKeysL") );
  }
    
// ---------------------------------------------------------------------------
// CCamAppUi::TimeLapseSliderShown
// Returns whether time lapse slider is currently shown
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::TimeLapseSliderShown() const
    {
    return ECamPreCapTimeLapseSlider == iPreCaptureMode;    
    }     

// ---------------------------------------------------------------------------
// CCamAppUi::FullScreenViewfinderEnabled
// Returns whether or not the full screen viewfinder is being shown
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::FullScreenViewfinderEnabled() const
    {
    TBool retVal = EFalse;

    if ( ECamViewStatePreCapture == iViewState
        && ( ECamOrientationCamcorder == iCamOrientation
            || ECamOrientationCamcorderLeft == iCamOrientation )
        && ECamPreCapViewfinder == iPreCaptureMode)
        {
        retVal = ETrue;
        }
        
    return retVal;        
    }

// ---------------------------------------------------------------------------
// CCamAppUi::SetPreCaptureModeL
// Sets the current pre-capture mode
// ---------------------------------------------------------------------------
//
void CCamAppUi::SetPreCaptureModeL(TCamPreCaptureMode aMode)  
    {
    PRINT(_L("Camera=>CCamAppUi::SetPreCaptureMode"))
    iPreCaptureMode = aMode;

    CCamViewBase* precapView = NULL;
    if ( iView == iStillCaptureView ||
         iView == iVideoCaptureView )
        {
        precapView = static_cast<CCamViewBase*>( iView );
		__ASSERT_DEBUG( precapView, CamPanic( ECamPanicNullPointer ));
		}
   
    if ( ECamPreCapViewfinder == iPreCaptureMode )
        {
        if ( !( iController.UiConfigManagerPtr() && 
                iController.UiConfigManagerPtr()->IsUIOrientationOverrideSupported() ) )
            {
            RaisePreCaptureCourtesyUI(ETrue);
            }
        if ( precapView )
            {
            if ( iView == iStillCaptureView )
	            {
	            // fixed toolbar is used only with touch devices
	            if ( iController.IsTouchScreenSupported() )
	                {
                    if ( IsSecondCameraEnabled() )
                        {
                        TInt resourceId = IsQwerty2ndCamera()? 
                                      R_CAM_STILL_PRECAPTURE_TOOLBAR_LANDSCAPE:
                                      R_CAM_STILL_PRECAPTURE_TOOLBAR_PORTRAIT; 
                        precapView->CreateAndSetToolbarL( resourceId );		
                        }
                    else
                        {
                        precapView->CreateAndSetToolbarL( R_CAM_STILL_PRECAPTURE_TOOLBAR ); 
                        }
                    CAknToolbar* fixedToolbar = CurrentFixedToolbar();
                    fixedToolbar->SetToolbarVisibility( EFalse );

                    if ( !IsSecondCameraEnabled() )
                        {
                        iStillCaptureView->UpdateToolbarIconsL();
                        }
	                
	                fixedToolbar->SetToolbarObserver( iStillCaptureView );
	                SetToolbarVisibility();

	                // Make sure toolbar extension button has no background
	                CAknButton* extensionButton = static_cast<CAknButton*>(
	                    fixedToolbar->ControlOrNull( ECamCmdToolbarExtension ) );
	                if ( extensionButton )
	                    {
	                    extensionButton->SetButtonFlags(
	                       // Set KAknButtonNoFrame flag for extension buttons      
	                       extensionButton->ButtonFlags() | KAknButtonNoFrame );
	                    }   
	                }
    	        }
            else if ( iView == iVideoCaptureView )
        	    {
        	    // fixed toolbar is used only with touch devices
        	    if ( iController.IsTouchScreenSupported() )
        	        {
                    if ( IsSecondCameraEnabled() )
                        {
                        TInt resourceId = IsQwerty2ndCamera()? 
                                      R_CAM_VIDEO_PRECAPTURE_TOOLBAR_LANDSCAPE:
                                      R_CAM_VIDEO_PRECAPTURE_TOOLBAR_PORTRAIT; 
                        precapView->CreateAndSetToolbarL( resourceId );	
                        }
                    else
                        {
                        if(iController.UiConfigManagerPtr()->IsXenonFlashSupported())
                            {
                            precapView->CreateAndSetToolbarL(R_CAM_VIDEO_PRECAPTURE_TOOLBAR);
                            }
                        else
                            {
                            precapView->CreateAndSetToolbarL(R_CAM_VIDEO_PRECAPTURE_TOOLBAR_VIDEOLIGHT);
                            }
                        }
                    CAknToolbar* fixedToolbar = CurrentFixedToolbar();
                    fixedToolbar->SetToolbarVisibility( EFalse );

                    if ( !IsSecondCameraEnabled() )
                        {
                        iVideoCaptureView->UpdateToolbarIconsL();
                        }
                    
                    fixedToolbar->SetToolbarObserver( iVideoCaptureView );
                    SetToolbarVisibility();
                       
                    // Make sure toolbar extension button has no background
                    CAknButton* extensionButton = static_cast<CAknButton*>(
                      fixedToolbar->ControlOrNull( ECamCmdToolbarExtension ) );
                    if ( extensionButton )
                        {
                        extensionButton->SetButtonFlags(
                        // Set KAknButtonNoFrame flag for extension buttons      
                        extensionButton->ButtonFlags() | KAknButtonNoFrame );
                        }
        	        }
        	    }
            	
            if( !IsSecondCameraEnabled() || IsQwerty2ndCamera() )  
                {
                EikSoftkeyPostingTransparency::MakeTransparent(
                        *precapView->ViewCba(), ETrue );
                
                if( iDrawPreCaptureCourtesyUI )
                    {
                    precapView->ViewCba()->MakeVisible( ETrue );
                    }
                }
              
            }
        }
    else
        {
        if ( precapView )
            {
            precapView->ViewCba()->MakeVisible( ETrue );
            // fixed toolbar is used only with touch devices
            if ( iController.IsTouchScreenSupported() )
                {
                precapView->CreateAndSetToolbarL( R_CAM_EMPTY_FIXED_TOOLBAR );
                }
            EikSoftkeyPostingTransparency::MakeTransparent(
                *precapView->ViewCba(), EFalse );
            }
        SetActivePaletteVisibility( EFalse );
        }

    if ( iViewState != ECamViewStateUserSceneSetup )
        {
	    if ( CamUtility::IsNhdDevice() ) 
	        {
	        StatusPane()->MakeVisible( aMode == ECamPreCapStandby || 
	                                   IsSecondCameraEnabled() && 
	                                   !IsQwerty2ndCamera() ||
	                                   SettingsLaunchedFromCamera() ); 
	        }
	    else
	        {
	        // Settings, launched via toolbar, have a visible statuspane in 
	        // Non-touch device 
	        StatusPane()->MakeVisible( !precapView ||
	                                   aMode != ECamPreCapViewfinder &&      
	                                   aMode != ECamPreCapTimeLapseSlider || 
	                                   IsSecondCameraEnabled() ||
	                                   SettingsLaunchedFromCamera() );  
	        }
        }


    PRINT(_L("Camera<=CCamAppUi::SetPreCaptureMode"))        
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::SetPreCaptureMode
// Sets the current pre-capture mode
// ---------------------------------------------------------------------------
//
void CCamAppUi::SetPreCaptureMode( TCamPreCaptureMode aMode )  
    {
    TRAPD(err, SetPreCaptureModeL( aMode ) );
    if ( err )
        {
        PRINT1(_L("Camera<=CCamAppUi::SetPreCaptureModeL Leave=%d"), err)   
        }
    }

// ---------------------------------------------------------------------------
// CCamAppUi::ShowPostCaptureView
// Whether to show the post capture view in current mode
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::ShowPostCaptureView() const
  {
  TInt settingId( ( ECamControllerVideo == iMode )
                  ? ECamSettingItemVideoShowCapturedVideo
                  : ECamSettingItemShowCapturedPhoto      );
  
  return ( ECamSettOn == iController.IntegerSettingValue( settingId ) );
  }
    

// ---------------------------------------------------------------------------
// CCamAppUi::SuppressUIRiseOnViewfinderStart
// Whether the automatic UI-rise should be suppressed on
// next viewfinder start event
// ---------------------------------------------------------------------------
//
TBool 
CCamAppUi::SuppressUIRiseOnViewfinderStart() const
  {
  return iSuppressUIRise;
  }
    

// ---------------------------------------------------------------------------
// CCamAppUi::SetSuppressUIRiseOnViewfinderStart
// Sets whether the automatic UI-rise should be suppressed on
// next viewfinder start event
// ---------------------------------------------------------------------------
//
void 
CCamAppUi::SetSuppressUIRiseOnViewfinderStart(TBool aSuppress) 
  {
  iSuppressUIRise = aSuppress;
  }


// ---------------------------------------------------------------------------
// CCamAppUi::SelectViewFinderGridSettingL
// Select the viewfinder grid setting.
// Currently only sets the grid ON or OFF.
// ---------------------------------------------------------------------------
//    
void 
CCamAppUi::SelectViewFinderGridSettingL()
  {
  PRINT( _L("Camera => CCamAppUi::SelectViewFinderGridSettingL") );
  PRINT1( _L("Camera <> ECamSettingItemViewfinderGrid == (%d)"), 
      ECamSettingItemViewfinderGrid );

  TInt current = iController.IntegerSettingValue( ECamSettingItemViewfinderGrid );
  TInt invert  = (current == ECamViewfinderGridOn) 
               ? ECamViewfinderGridOff 
               : ECamViewfinderGridOn;

  // Switch the value.
  iController.SetIntegerSettingValueL( ECamSettingItemViewfinderGrid, invert );

  // We'll get a HandleControllerEventL callback 
  // from iController when the value has been set.
  PRINT( _L("Camera <= CCamAppUi::SelectViewFinderGridSettingL") );
  }


// ---------------------------------------------------------------------------
// CCamAppUi::ConstructPostCaptureViewIfRequiredL
//
// Construct a postcapture view if not already constructed
//
// The following parameter combinations can be used:
//      * Still postcapture view: aViewState = ECamViewStatePostCapture, 
//                                aMode      = ECamControllerImage
//      * Burst thumbnail view:   aViewState = ECamViewStateBurstThumbnail
//                                aMode      = << ignored >>
//      * Video postcapture view: aViewState = ECamViewStatePostCapture, 
//                                aMode      = ECamControllerVideo
// ---------------------------------------------------------------------------
//   
void 
CCamAppUi::ConstructPostCaptureViewIfRequiredL( TCamViewState aViewState,
                                                TCamCameraMode aCaptureMode )
    {
    CAknView* view;
    
    if( ECamViewStateBurstThumbnail == aViewState )
        {
        // Burst thumbnail view        
        if( !iBurstThumbnailViewConstructed )
            {    
            PRINT( _L("Camera => CCamAppUi::\
                ConstructPostCaptureViewIfRequiredL - BurstThumbnail") );        
            view = CCamBurstThumbnailView::NewLC( iController );
            AddViewL( view );
            CleanupStack::Pop( view );
            iBurstThumbnailViewConstructed = ETrue;
            PRINT( _L("Camera <= CCamAppUi::\
                ConstructPostCaptureViewIfRequiredL - BurstThumbnail") );    
            }                        
        }
    else if( ECamViewStatePostCapture == aViewState )
        {
        if( ECamControllerVideo == aCaptureMode )
            {
            // Video postcapture
            if( !iVideoPostCaptureViewConstructed )
                {
                PRINT( _L("Camera => \
                    CCamAppUi::ConstructPostCaptureViewIfRequiredL \
                    - VideoPostCapture") );    
                view = CCamVideoPostCaptureView::NewLC( iController );
                AddViewL( view );
                CleanupStack::Pop( view );
                iVideoPostCaptureViewConstructed = ETrue;
                PRINT( _L("Camera <= \
                    CCamAppUi::ConstructPostCaptureViewIfRequiredL \
                    - VideoPostCapture") );    
                }            
            }
        else
            {
            // Still postcapture
            if( !iStillPostCaptureViewConstructed )
                {                         
                PRINT( _L("Camera => \
                    CCamAppUi::ConstructPostCaptureViewIfRequiredL \
                    - StillPostCapture") );                             
                view = CCamStillPostCaptureView::NewLC( iController );
                AddViewL( view );
                CleanupStack::Pop( view );    
                iStillPostCaptureViewConstructed = ETrue;
                PRINT( _L("Camera <= \
                    CCamAppUi::ConstructPostCaptureViewIfRequiredL \
                     - StillPostCapture") );    
                }
            }
        }
    }
    

#ifdef CAMERAAPP_DELAYED_POSTCAPTURE_CREATION

// ---------------------------------------------------------------------------
// CCamAppUi::StartPostCapIdleL
//
// Starts calls to PostCapIdleCallBack during processor idle time. The calls
// are continued until PostCapIdleCallBack is done.
//    
// ---------------------------------------------------------------------------
void CCamAppUi::StartPostCapIdleL()
    {
    if( !iPostCapIdle )
        {
        iPostCapIdle = CIdle::NewL( CActive::EPriorityIdle );
        iPostCapProgress = ECamConstructStillPostCap;
        iPostCapIdle->Start( TCallBack( PostCapIdleCallBack, this ) );
        }
    else
        {
        // iPostCapIdle already exists.
        // Do nothing - the views have already been constructed
        }        
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::PostCapIdleCallBack
//
// Handles postcapture view construction so that each call to this function
// constructs one postcapture view. Returns ETrue if there are still more views
// to construct, EFalse if all are finished and no further calls are required.
//    
// ---------------------------------------------------------------------------    
TInt CCamAppUi::PostCapIdleCallBack( TAny* aSelf )
    {
    PRINT( _L("Camera => CCamAppUi::PostCapIdleCallBack") );
    TInt ret = EFalse;
        
    CCamAppUi* self = static_cast<CCamAppUi*>( aSelf );
    if( self )    
        {  
        if( ECamConstructStillPostCap == self->iPostCapProgress )
            {
            TRAP_IGNORE( self->ConstructPostCaptureViewIfRequiredL(
                ECamViewStatePostCapture, ECamControllerImage ) );
            self->iPostCapProgress = ECamConstructBurstPostCap;    
            ret = ETrue;
            }
        else if( ECamConstructBurstPostCap == self->iPostCapProgress )
            {
            TRAP_IGNORE( self->ConstructPostCaptureViewIfRequiredL(
                ECamViewStateBurstThumbnail, ECamControllerImage ) );
            self->iPostCapProgress = ECamConstructVideoPostCap;
            ret = ETrue;
            }
        else if( ECamConstructVideoPostCap == self->iPostCapProgress )
            {
            TRAP_IGNORE( self->ConstructPostCaptureViewIfRequiredL(
                ECamViewStatePostCapture, ECamControllerVideo ) );
            self->iPostCapProgress = ECamPostCapsDone;
            // Complete
            ret = EFalse;
            }                                  
        }
        
    PRINT( _L("Camera <= CCamAppUi::PostCapIdleCallBack") );        
    return ret;
    }

#endif // CAMERAAPP_DELAYED_POSTCAPTURE_CREATION


// ---------------------------------------------------------------------------
// CCamAppUi::StartSensorIdleL
//
// Initiates a call to SensorIdleCallback during processor idle time
// ---------------------------------------------------------------------------
// 
void CCamAppUi::StartSensorIdleL()
    {
    if ( iController.UiConfigManagerPtr()->IsOrientationSensorSupported() )
    	{
	    if( iSensorIdleRunning )
	        {
	        // Delayed sensor initialization is already in progress  
	        return;  
	        }  
	      
	    if( !iSensorIdle )
	        {
	        // Instantiate new CIdle  
	        iSensorIdle = CIdle::NewL( CActive::EPriorityIdle );   
	        }
	    else
	        {
	        // Cancel existing the existing request, just in case
	        iSensorIdle->Cancel();  
	        }     
	     
	    // Initialize the idle callback   
	    iSensorIdle->Start( TCallBack( SensorIdleCallBack, this ) );                
	    iSensorIdleRunning = ETrue;
		}
    }

// ---------------------------------------------------------------------------
// CCamAppUi::CancelSensorIdle
//
// Cancels the (possibly) pending call to SensorIdleCallBack
// ---------------------------------------------------------------------------
// 
void CCamAppUi::CancelSensorIdle()
    {
		if ( iController.UiConfigManagerPtr()->IsOrientationSensorSupported() )
			{
	    if( iSensorIdle && iSensorIdleRunning )
	        {
	        iSensorIdle->Cancel();  
	        iSensorIdleRunning = EFalse;  
	        }
      }
    }
    
// ---------------------------------------------------------------------------
// CCamAppUi::SensorIdleCallBack
//
// Calls iControler.UpdateSensorApiL
// ---------------------------------------------------------------------------
//    
TInt CCamAppUi::SensorIdleCallBack( TAny* aSelf )
    {
    CCamAppUi* self = static_cast<CCamAppUi*>( aSelf );
    
    if( self ) 
        {
        TRAP_IGNORE( self->iController.UpdateSensorApiL( ETrue ) );
        self->iSensorIdleRunning = EFalse;
        }
    return EFalse; // No more calls needed
    }


// ---------------------------------------------------------------------------
// CCamAppUi::IsSelfTimedCapture
// Whether the current capture was selftimer initiated
// ---------------------------------------------------------------------------
//         
TBool CCamAppUi::IsSelfTimedCapture() const
    {
    return iSelfTimedCapture;
    }

// ---------------------------------------------------------------------------
// CCamAppUi::DismissMemoryNoteL
// Dismisses out of memory / memory card removed note, if active.
// ---------------------------------------------------------------------------
//
void
CCamAppUi::DismissMemoryNoteL()  
  {
  if( iMemoryNote )
    {         
    if( iMemoryNote->IsFocused() )
      {
      // For some reason, the note's softkeys are displayed in standby mode,
      // even though the note itself has been dismissed (=deleted)
      // HACK:
      // If the note is focused, we know that also its softkeys are in use,
      // so we can simulate key events to emulate user closing the note
      // using left softkey.
      
      TKeyEvent key;
      key.iRepeats = 0;
      key.iCode = EKeyDevice0;
      key.iModifiers = 0;
      iEikonEnv->SimulateKeyEventL( key, EEventKeyDown );
      CEikButtonGroupContainer::Current()->OfferKeyEventL( key, 
          EEventKeyDown );            
      }

    // Make sure the note closes even if it wasn't focused.
    delete iMemoryNote;
    iMemoryNote = NULL;                           
    }               
  }


// ---------------------------------------------------------------------------
// Starts Gallery Application command line
// ---------------------------------------------------------------------------
//


void
CCamAppUi::StartAppCmdLineL( const TDes8& aData )
    {
    // MM not running yet - use Command Line Tail
    RApaLsSession appArcSession;
    CleanupClosePushL( appArcSession );
    User::LeaveIfError( appArcSession.Connect() );      
    TApaAppInfo appInfo;
    User::LeaveIfError( appArcSession.GetAppInfo( appInfo,
           TUid::Uid( KGlxGalleryApplicationUid) ) );

    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetExecutableNameL( appInfo.iFullName );
    cmdLine->SetCommandL( EApaCommandRun );   
    cmdLine->SetTailEndL( aData );
   
    User::LeaveIfError( appArcSession.StartApp( *cmdLine ) );      
  
    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy( &appArcSession ); 
    }
// ---------------------------------------------------------------------------
// CCamAppUi::StartCheckingDefaultAlbumIdL()
// We start check operation to see if "Default Album exists"
// Usecase: Whenever the camera returns from background/startup we first start
// checking the default album which is set, if any, to make sure that album 
// really exists. If not, then we set back the value of setting item "Add to 
// album" to "No" in Image/Video setting image settings
// ---------------------------------------------------------------------------
//
void CCamAppUi::StartCheckingDefaultAlbumIdL()
    {
    TBool checkAlbum = ( 
            iController.IntegerSettingValue( 
                            ECamSettingItemPhotoStoreInAlbum ) == ECamSettYes ||
            iController.IntegerSettingValue( 
                            ECamSettingItemVideoStoreInAlbum ) == ECamSettYes );

    if ( !iCheckOperationInProgress && checkAlbum )
        {
        TUint32 defaultAlbumId = 
        static_cast<TUint32> ( 
                iController.IntegerSettingValue( 
                        ECamSettingItemDefaultAlbumId ) );
        if ( iCollectionManager )
            {
            iCheckOperationInProgress = ETrue;
            iCollectionManager->CheckForAlbumExistenceL( defaultAlbumId );
            }
        }

    }

// ---------------------------------------------------------------------------
// CCamAppUi::CheckAlbumIdOperationCompleteL
// Call back method, which is called once the check operation is complete
// ---------------------------------------------------------------------------
//
void CCamAppUi::CheckAlbumIdOperationCompleteL( 
    TBool aAlbumExists, const TDesC& /*aAlbumTitle*/ )
    {
    PRINT( _L("CCamAppUi::CheckAlbumIdOperationCompleteL --> ENTERS") );
    if ( iController.UiConfigManagerPtr()->IsPhotosSupported() )
        {
        if ( iCheckOperationInProgress )
            {
            PRINT( _L("CheckAlbumIdOperationCompleteL \
                iCheckOperationInProgress: ETRUE") );
            iCheckOperationInProgress = EFalse;
            if ( !aAlbumExists )
                {
                // Reset all the values to the default ones, 
                // if the default album doesnt exist, when returned from background
                iController.SetIntegerSettingValueL( 
                                     ECamSettingItemPhotoStoreInAlbum,
                                     ECamSettNo );
                iController.SetIntegerSettingValueL( 
                                     ECamSettingItemVideoStoreInAlbum,
                                     ECamSettNo );
                // reseting the album id to the default value "0"
                iController.SetIntegerSettingValueL(
                                     ECamSettingItemDefaultAlbumId,
                                     0 );
                }
            }        
        }
    PRINT( _L("CCamAppUi::CheckAlbumIdOperationCompleteL <-- EXITS") );
    }


// ---------------------------------------------------------------------------
// CCamAppUi::SettingsLaunchedFromCamera()
// We keep track of plugin event launched from camera
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::SettingsLaunchedFromCamera() const
    {
    return iSettingsPluginLaunched;
    }


// ---------------------------------------------------------------------------
// CCamAppUi::SettingsPluginExitedL
// We handle the case when settings plugin exists
// ---------------------------------------------------------------------------
//
void CCamAppUi::SettingsPluginExitedL( TCamSettingsPluginExitMessage aType )
    {
    iSettingsPluginLaunched =  EFalse;
    iTargetViewState = ECamViewStatePreCapture;
    TrySwitchViewL();

    switch ( aType )
        {
        case ECameraPluginExit:
            {
            // we exit the plugin and switch to pre-capture
            iReturnedFromPlugin = ETrue;
            break;
            }
        case ECameraInternalExit:
             {
             // Internal Exit from Plugin -> Options -> Exit
             iReturnedFromPlugin = EFalse;
             InternalExitL();
             break;
             }
        case ECameraCompleteExit:
             {
             // Plugiin -> End Key or "c" key
             iReturnedFromPlugin = EFalse;
             break;
             }
        }
    PRINT( _L("Camera <= CCamAppUi::SettingsPluginExitedL") );	    
    }

// ---------------------------------------------------------------------------
// CCamAppUi::StaticSettingsModel()
//
// Gets the handle to the settings model, which is used inturn by the
// plugin to call the interfaces for performing needed actions.
// ---------------------------------------------------------------------------
//
MCamStaticSettings& 
CCamAppUi::StaticSettingsModel()
    {
    return iController.StaticSettingsModel();
    }

// ---------------------------------------------------------------------------
// CCamAppUi::CreateNaviBitmapsL
// Draw the counter to the bitmap used in the navi pane
// ---------------------------------------------------------------------------
//
void 
CCamAppUi::CreateNaviBitmapsL( TBool aDrawStorageIconAndCounter )
    {
    if ( iNaviCounterModel )
        {
	    iNaviCounterModel->CreateNaviBitmapsL( aDrawStorageIconAndCounter );
	    }
    }

// ---------------------------------------------------------------------------
// CCamAppUi::ReadyToDraw
// Checks whether its safe to draw controls
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::ReadyToDraw() const
    {
    if ( AppInBackground( ETrue ) || !iView )
        {
        return EFalse;
        }
    else 
        {
        return ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CCamAppUi::StartFileNameCheck
// Checks whether the file name is deleted/renamed outside camera
// ---------------------------------------------------------------------------
//
void CCamAppUi::StartFileNameCheck()
    {
    PRINT( _L("Camera => CCamAppUi::StartFileNameCheck") );
    if( !iFileCheckAo->IsActive() )
        {
        iFileCheckAo->Start();
        }
    PRINT( _L("Camera <= CCamAppUi::StartFileNameCheck") );
    }


// ---------------------------------------------------------------------------
// CCamAppUi::FileCheckingComplete
// Handle a file checking complete event. Used to inform
// when filechecking active object.
// has finished checking existance of files.
// ---------------------------------------------------------------------------
//
void CCamAppUi::FileCheckingCompleteL( TInt aStatus, TInt aError )
  {
  PRINT2( _L("Camera => CCamAppUi::FileCheckingComplete(), \
      aStatus=%d, aError=%d "), aStatus, aError )

  if( aStatus <= 0 || KErrNone != aError )
      {
      PRINT( _L("Camera <> CCamAppUi::FileCheckingComplete: \
      all files deleted, trying viewswitch") );
      iTargetViewState = ECamViewStatePreCapture;      	
      TrySwitchViewL( ETrue );		  	
      }
  else if ( aStatus < iController.BurstCaptureArray()->Count() )
      {
      PRINT2( _L("Camera <> CCamAppUi::FileCheckingComplete: %d < %d,\
              some file(s) have been deleted"),
              aStatus,
              iController.BurstCaptureArray()->Count() );
      
      if ( iViewState == ECamViewStateBurstThumbnail || iViewState == ECamViewStatePostCapture )
          {
          PRINT( _L("Camera <> CCamAppUi::FileCheckingComplete() updating thumbnail view") );
          CCamBurstThumbnailView* thumbnailView =
          static_cast<CCamBurstThumbnailView*>( View( TUid::Uid( ECamViewIdBurstThumbnail ) ) );

          if ( thumbnailView )
              {
              thumbnailView->ImageFilesDeleted();
              }
          
          if ( iViewState == ECamViewStatePostCapture )
              {
              // view state is post-capture, check if the image currently viewed has been deleted
              if ( iController.BurstCaptureArray()->IsDeleted( iController.CurrentImageIndex() ) )
                  {
                  //switch back to burst thumbnail view
                  HandleCommandL( EAknSoftkeyBack );
                  }
              
              }
          }
      }

  //When return to video post capture view from background, we need to determine whether show the toolbar
  if( iReturnFromBackground
      && iViewState == ECamViewStatePostCapture
      && iMode == ECamControllerVideo )
      {
      SetToolbarVisibility();
      }

  PRINT( _L("Camera <= CCamAppUi::FileCheckingComplete()") );
  }
	
// ---------------------------------------------------------------------------
// CCamAppUi::HandleSelfTimerEvent
// Handle self-timer event
// ---------------------------------------------------------------------------
//
void CCamAppUi::HandleSelfTimerEvent( TCamSelfTimerEvent aEvent,
                                      TInt aCountDown )
    {
#if defined( CAMERAAPP_CAPI_V2_DVF )
    TRAP_IGNORE( HandleCommandL( ECamCmdRedrawScreen ) );
#else
    (void) aEvent;
    (void) aCountDown;
#endif // CAMERAAPP_CAPI_V2_DVF
    }

// ---------------------------------------------------------------------------
// CCamAppUi::RequestedNewFileResolution
// Set the requested resolution for a new file
// ---------------------------------------------------------------------------
//
void CCamAppUi::SetRequestedNewFileResolution(
    const TSize& aRequestedResolution )
    {
    iRequestedNewFileResolution.SetSize(
        aRequestedResolution.iWidth,
        aRequestedResolution.iHeight );
    }

// ---------------------------------------------------------------------------
// CCamAppUi::RequestedNewFileResolution
// Get the requested resolution for a new file
// ---------------------------------------------------------------------------
//
TSize CCamAppUi::RequestedNewFileResolution() const
    {
    // Default is TSize(0, 0) which interpreted as MMS quality resolution
    return iRequestedNewFileResolution;
    }

// ---------------------------------------------------------------------------
// CCamAppUi::SetToolbarVisibility
// Set toolbar visibility in according to whether current view and 
// mode are fit. When embedded camera starts up, view and mode may 
// not be fit.
// ---------------------------------------------------------------------------
//
void CCamAppUi::SetToolbarVisibility()
    {
    // fixed toolbar is used only with touch devices
    CAknToolbar* fixedToolbar = CurrentFixedToolbar();
    if ( iController.IsTouchScreenSupported() && fixedToolbar )  
        {
        if ( IsEmbedded() )
            {
            // Check that iMode matches iTargetMode and that the corresponding
            // view is active.
            if ( iMode == iTargetMode &&
                ( ( iMode == ECamControllerVideo && iView == iVideoCaptureView ) ||
                  ( iMode == ECamControllerImage && iView == iStillCaptureView ) ) )
                {
                fixedToolbar->SetToolbarVisibility( ETrue );
                }
            else
                {
                fixedToolbar->SetToolbarVisibility( EFalse );
                }
            }
        else
            {
            if( (iPreCaptureMode != ECamPreCapCaptureSetup) && 
                (iPreCaptureMode != ECamPreCapGenericSetting) && 
                (iPreCaptureMode != ECamPreCapSceneSetting ) )
                {
                fixedToolbar->SetToolbarVisibility( ETrue );
                }
            else
                {
                // No toolbar with the settings view
                fixedToolbar->SetToolbarVisibility( EFalse );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCamAppUi::SubmergeToolbar
// Hide toolbar if needed.
// ---------------------------------------------------------------------------
//
void CCamAppUi::SubmergeToolbar()
    {
    PRINT( _L("Camera => CCamAppUi::SubmergeToolbar()") );
    // fixed toolbar is used only with touch devices
    CAknToolbar* fixedToolbar = CurrentFixedToolbar();
    if ( iController.IsTouchScreenSupported() && fixedToolbar )  
        {
        fixedToolbar->SetToolbarVisibility( EFalse );
        }
    PRINT( _L("Camera <= CCamAppUi::SubmergeToolbar()") ); 
    }

// ---------------------------------------------------------------------------
// Check if it's applicable to use the courtesy UI
// ---------------------------------------------------------------------------
//
TBool CCamAppUi::CourtesyUiApplies() const
    {
    return ( ECamCapturing != iController.CurrentVideoOperation()
             && ECamPaused != iController.CurrentVideoOperation()
             && !SelfTimerEnabled()
             && ( ECamOrientationCamcorder == iCamOrientation ||
                  ECamOrientationCamcorderLeft == iCamOrientation ||
                  ECamOrientationPortrait == iCamOrientation ) );
    }

// -----------------------------------------------------------------------------
// CCamAppUi::IsToolBarVisible
// Returns whether or the tool bar has been activated
// Will return ETrue when when the tool bar is active
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::IsToolBarVisible() const
    {
    PRINT1( _L("Camera => CCamAppUi::IsToolBarVisible %d" ), iToolbarVisibility )
    return iToolbarVisibility;
    }


// -----------------------------------------------------------------------------
// CCamAppUi::SetAssumePostCaptureView
// Sets iAssumePostCapture flag according to aValue
// -----------------------------------------------------------------------------
//
void CCamAppUi::SetAssumePostCaptureView( TBool aValue )
    {
    if ( iView 
         && ( iStillCaptureView == iView || iVideoCaptureView == iView ) )
        {	
        static_cast<CCamPreCaptureViewBase*>( iView )->SetPostCaptureViewAsumption( aValue );	
        }
    }
    
// --------------------------------------------------------------------------- 
// CCamAppUi::SetDrawPreCaptureCourtesyUI 
// Set iDrawPreCaptureCourtesyUI flag 
// --------------------------------------------------------------------------- 
// 
void CCamAppUi::SetDrawPreCaptureCourtesyUI(TBool iDraw) 
    { 
        if( iDraw ) 
            { 
            iDrawPreCaptureCourtesyUI = ETrue; 
            } 
        else 
            { 
            iDrawPreCaptureCourtesyUI = EFalse; 
            } 
    } 

// -----------------------------------------------------------------------------
// CCamAppUi::IsMemoryFullOrUnavailable
// -----------------------------------------------------------------------------
//
TBool 
CCamAppUi::IsMemoryFullOrUnavailable(const TCamMediaStorage 
									 aStorageLocation) const
	{
	if(!iController.IsMemoryAvailable(aStorageLocation))
		return ETrue;
	
	if(iMode == ECamControllerImage)
		{
		TInt availableMemoryUnits = 0;
		availableMemoryUnits = 
			iController.ImagesRemaining(aStorageLocation,EFalse);
		return (availableMemoryUnits <= 0);
		}
	else if(iMode == ECamControllerVideo)
		{
		TTimeIntervalMicroSeconds timeLeft = 0;
		TRAPD(err,timeLeft =
					iController.
					CalculateVideoTimeRemainingL(aStorageLocation););
		if(err)
		    timeLeft = 0;
		return (timeLeft < KMemoryFullVideoRemaining);
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CCamAppUi::AllMemoriesFullOrUnavailable
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::AllMemoriesFullOrUnavailable() const
	{
	return ( IsMemoryFullOrUnavailable(ECamMediaStoragePhone) &&
		   IsMemoryFullOrUnavailable(ECamMediaStorageCard) &&
		   IsMemoryFullOrUnavailable(ECamMediaStorageMassStorage) );
	}

    
// ---------------------------------------------------------
// CCamAppUi::AppUIConstructCallbackL
// 
// ---------------------------------------------------------
//
TInt CCamAppUi::AppUIConstructCallbackL( TAny* aAny )
    {
    PRINT( _L("Camera => CCamAppUi::AppUIConstructCallbackL") )
    CCamAppUi* appui = static_cast<CCamAppUi*>( aAny );
    __ASSERT_DEBUG( appui != NULL && appui->iWaitTimer != NULL, CamPanic( ECamPanicNullPointer ) );
    appui->iWaitTimer->Cancel();
    if ( !appui->IsUiConstructionComplete() )
        {
        appui->CompleteAppUIConstructionL();
        }
    PRINT( _L("Camera <= CCamAppUi::AppUIConstructCallbackL") )
    return KErrNone;
    }    

// ---------------------------------------------------------
// CCamAppUi::CompleteAppUIConstructionL
// 
// ---------------------------------------------------------
//    
void CCamAppUi::CompleteAppUIConstructionL()
    {
    PRINT( _L("Camera => CCamAppUi::CompleteAppUIConstructionL") )
    iUiConstructionComplete = ETrue; 
    // Load the settings model static settings
    PRINT( _L("Camera <> call CCamAppController::LoadStaticSettingsL..") )
    iController.LoadStaticSettingsL( IsEmbedded() );
    // store the userscene settings
    iController.StoreUserSceneSettingsL();

/*#ifndef __WINSCW__    
        if ( !iSFIUtils )
            {
            // Initialize SFIUtils
            iSFIUtils = CSFIUtilsAppInterface::NewL();
            iSendFileInCall = EFalse;
            PRINT( _L("iSendFileInCall = EFalse") );	
            }	
#endif        */
  
    // Create text resolver for error note text
    iTextResolver = CTextResolver::NewL();
  
    ConstructPreCaptureViewsL();
  
    iSelfTimer = CCamSelfTimer::NewL( iController );
#if defined( CAMERAAPP_CAPI_V2_DVF )
    iSelfTimer->AddObserverL( this ); 
#endif // CAMERAAPP_CAPI_V2_DVF

    PRINT( _L("Camera <> creating navicounter model") );
    iNaviCounterModel = CCamNaviCounterModel::NewL( iController );
    PRINT( _L("Camera <> append navicounter to resourceloaders") );
    User::LeaveIfError( iResourceLoaders.Append(iNaviCounterModel) );
  
    PRINT( _L("Camera <> creating progress bar model") );
    iNaviProgressBarModel = CCamNaviProgressBarModel::NewL( iController );
    PRINT( _L("Camera <> append progressbar to resourceloaders") );
    User::LeaveIfError(iResourceLoaders.Append(iNaviProgressBarModel));
  
    ConstructNaviPaneL();
    
    //always start in stillmode
    SetDefaultViewL( *iStillCaptureView );
        
    // pre-construct side-pane & zoom pane
    // get whether we overlay sidepane over view-finder
    TBool overlayViewFinder;
    User::LeaveIfError( CamUtility::GetPsiInt( ECamPsiOverLaySidePane, overlayViewFinder ) );
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL create sidepane") );
    iCamSidePane = CCamSidePane::NewL( iController, overlayViewFinder );
    User::LeaveIfError(iResourceLoaders.Append(iCamSidePane));
    
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL create zoom pane") );
    iCamZoomPane = CCamZoomPane::NewL( iController, overlayViewFinder );
    User::LeaveIfError(iResourceLoaders.Append(iCamZoomPane));
  
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL create doc handler") );
    iDocHandler = CDocumentHandler::NewL( CEikonEnv::Static()->Process() );

    // Check to see if we are set to use mmc storage but the card has
    // been removed.  
    iController.CheckMemoryToUseL();

    // create navi-pane and navi-porgress bar for use in camcorder mode 
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL create navicounter control") );
    iNaviCounterControl = CCamNaviCounterControl::NewL( *iNaviCounterModel );
    iNaviCounterControl->SetExtentToWholeScreen();
  
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL create progress bar control") );
    iNaviProgressBarControl = CCamNaviProgressBarControl::NewL( *iNaviProgressBarModel );        
  
    // get max num of images for burst capture
    CamUtility::GetPsiInt( ECamPsiMaxBurstCapture, iMaxBurstCaptureNum );
  
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL create AP handler") );
    iActivePaletteHandler = CCamActivePaletteHandler::NewL( iController, EFalse );
  
    // get coutesy UI timeout
    TInt timeoutValue;
    CamUtility::GetPsiInt( ECamPsiCourtesyUiTimeoutValue, timeoutValue );
  
    iCourtesyTimer = CCamTimer::NewL( timeoutValue * 1000000, 
                                    TCallBack( CCamAppUi::CourtesyTimeout, this ) );
                                    
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL update navi models") );
    UpdateNaviModelsL();
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL navi counter reload resources") );
    iNaviCounterModel->ReloadResourceDataL();
    PRINT( _L("Camera <> CCamAppUi::CompleteAppUIConstructionL navi progress bar reload resources") );
    iNaviProgressBarModel->ReloadResourceDataL();
  
    iFileCheckAo = CCamFileCheckAo::NewL( iController, *this );  
    iCollectionManager = new (ELeave) CCamCollectionManagerAO( *this );
    
    //iStillCaptureView->Container()->MakeVisible( ETrue );
    ActivateLocalViewL( iStillCaptureView->Id() );
    
    StartCheckingDefaultAlbumIdL();
    PRINT( _L("Camera <= CCamAppUi::CompleteAppUIConstructionL") )
    }

// ---------------------------------------------------------
// CCamAppUi::IsUiConstructionComplete()
// 
// ---------------------------------------------------------
//
TBool CCamAppUi::IsUiConstructionComplete()
    {
    return iUiConstructionComplete;    
    }
	
// -----------------------------------------------------------------------------
// CCamAppUi::SetLensCoverExit 
// Sets iLensCoverExit flag according to aValue
// -----------------------------------------------------------------------------
//
void CCamAppUi::SetLensCoverExit( TBool aValue )
    {
    iLensCoverExit = aValue;
    }
	
// -----------------------------------------------------------------------------
// CCamAppUi::SetViewFinderStoppedStatus 
// set whether camera viewfinder stop or not
// @param aViewFinderStopped ETrue viewfinder is stoped,EFalse viewfinder is runing
// -----------------------------------------------------------------------------
//
void CCamAppUi::SetViewFinderStoppedStatus( TBool aViewFinderStopped )
    {
    iViewFinderStopped = aViewFinderStopped;
    }

// -----------------------------------------------------------------------------
// CCamAppUi::IsViewFinderInTransit 
// -----------------------------------------------------------------------------
//
TBool CCamAppUi::IsViewFinderInTransit()
    {
    return iViewFinderInTransit;
    }
        
// -----------------------------------------------------------------------------
// CCamAppUi::SetViewFinderInTransit 
// -----------------------------------------------------------------------------
//
void CCamAppUi::SetViewFinderInTransit(TBool aInTransit)
    {
    iViewFinderInTransit = aInTransit;
    }
//  End of File
