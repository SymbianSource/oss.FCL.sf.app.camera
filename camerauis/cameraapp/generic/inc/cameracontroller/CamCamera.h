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


#ifndef CAM_CAMERA_H
#define CAM_CAMERA_H

#include <ecam/ecamplugin.h>
#include <e32base.h>

#include "CamCameraCallback.h"
class MCameraObserver;
class MCameraObserver2;
class CCamCameraCallback; //CCamCameraCallBack

class CCamCamera : public CCameraPlugin
  {
  public:

    /*
    * NewL method; creating an instance based on MCameraObserver2
    */
    static CCamCamera* NewL( MCameraObserver2& aObserver, 
                             TInt              aCameraIndex, 
                             TInt              aPriority );

    /**
    * New2L method
    */
    static CCamCamera* New2L( MCameraObserver2& aObserver,
                              TInt              aCameraIndex,
                              TInt              aPriority );

    /*
    * NewL method returns CCamera object; created based on
    * MCameraObserver's observer
    */                                      
    static CCamCamera* NewL( MCameraObserver& aObserver, 
                             TInt             aCameraIndex );

    /**
    * New2L method
    */
    static CCamCamera* New2L( MCameraObserver& aObserver,
                              TInt             aCameraIndex );

    /*
    * NewDuplicateL
    */
    static CCamCamera* NewDuplicateL( MCameraObserver2& aObserver,
                                      TInt              aCameraHandle );
   
    /*
    * NewDuplicate2L
    */
    static CCamCamera* NewDuplicate2L( MCameraObserver2& aObserver,
                                       TInt              aCameraHandle );     
    
    /*
    * NewDuplicateL
    */  	                                 
    static CCamCamera* NewDuplicateL( MCameraObserver& aObserver, 
                                      TInt             aCameraHandle );
  	 
    /*
    * NewDuplicate2L
    */
    static CCamCamera* NewDuplicate2L( MCameraObserver& aObserver,
                                       TInt             aCameraHandle );
	 
    /*
    * Destructor
    */
    ~CCamCamera();


    // Needed for compatibility with Symbian CCameraPlugin. Not implemented.
    void Construct2L(MCameraObserver& aObserver, TInt aCameraIndex);
    void Construct2DupL(MCameraObserver& aObserver, TInt aCameraHandle);
    void Construct2L(MCameraObserver2& aObserver, TInt aCameraIndex,TInt aPriority);
    void Construct2DupL(MCameraObserver2& aObserver, TInt aCameraHandle);
    
  public: // From CCamera documentation found from symbian

    /*
    * Returns the number of camera's available
    * For now, its 2
    */
    static TInt CamerasAvailable();

    /*
    * All the default information is set using this method for the camera
    * Refer to TCameraInfo for further details
    */      
    virtual void CameraInfo(TCameraInfo& aInfo) const;
    
    /*
    * Interface to reserve the camera, before actually using it
    */
    virtual void Reserve();
    
    /*
    * Release the camera, if not needed
    */
    virtual void Release();
    
    /*
    * Interface to switch to PowerOn state of the camera
    */
    virtual void PowerOn();
    
    /*
    * Interface to switch to PowerOff state
    */
    virtual void PowerOff();
    
    /*
    * Interface to return the Handle ID for the camera
    */
    virtual TInt Handle();
    
    /*
    * Interface to set the zoom factor for the camera; by default its 0
    */
    virtual void SetZoomFactorL(TInt aZoomFactor = 0);
    
    /*
    * Interface to get the zoom factor
    */
    virtual TInt ZoomFactor() const;
     
    /*
    * Interface to set the digital zoom factor for the camera; by default its 0
    */
    virtual void SetDigitalZoomFactorL(TInt aDigitalZoomFactor = 0);
    
    /*
    * Interface to get the digital zoom factor for the camera; by default its 0
    */
    virtual TInt DigitalZoomFactor() const;
    
    /*
    * Interface to set the contrast for the camera
    */
    virtual void SetContrastL(TInt aContrast);
    
    /*
    * Interface to get the contrast for the camera
    */
    virtual TInt Contrast() const;
     
    /*
    * Interface to set the brightness
    */
    virtual void SetBrightnessL(TInt aBrightness);
    
    /*
    * Interface to get the brightness of camera
    */
    virtual TInt Brightness() const;
    
    /*
    * Interface to start the direct view finder
    */
    virtual void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect);
    
    /*
    * Interface to start the direct view finder
    */
    virtual void StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect);
     
    /*
    * Interface to start the view finder bitmaps
    */
    virtual void StartViewFinderBitmapsL(TSize& aSize);
    
    /*
    * Interface to start the view finder bitmaps
    */
    virtual void StartViewFinderBitmapsL(TSize& aSize,TRect& aClipRect);
    
    /*
    * Interface to start the View finder frame
    */
    virtual void StartViewFinderL(TFormat aImageFormat,TSize& aSize);
    
    /*
    * Interface to start the view finder
    */
    virtual void StartViewFinderL(TFormat aImageFormat,TSize& aSize,TRect& aClipRect);
     
    /*
    * Interface to check if the view finder is active
    */
    virtual TBool ViewFinderActive() const;
    
    /*
    *
    */
    virtual void SetViewFinderMirrorL(TBool aMirror);
    
    /*
    *
    */
    virtual TBool ViewFinderMirror() const;
    
    /*
    * Interface to stop the view finder
    */
    virtual void StopViewFinder();
    
    /*
    * Interface to start Prepare Image Capture
    */
    virtual void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex);
     
    /*
    *  Interface to start Prepare Image Capture
    */
    virtual void PrepareImageCaptureL(TFormat aImageFormat,TInt aSizeIndex,const TRect& aClipRect);
    
    /*
    * Interface to Capture Image
    */
    virtual void CaptureImage();
    
    /*
    * Interface to Cancel Capture Image
    */
    virtual void CancelCaptureImage();
    
    /*
    *
    */
    virtual void EnumerateCaptureSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const;
     
    /*
    * Interfaces to start prepare video capture
    */
    virtual void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer);
    
    /*
    * Interfaces to start prepare video capture
    */
    virtual void PrepareVideoCaptureL(TFormat aFormat,TInt aSizeIndex,TInt aRateIndex,TInt aBuffersToUse,TInt aFramesPerBuffer,const TRect& aClipRect);
    
    /*
    * Interface to start the video capture
    */
    virtual void StartVideoCapture();
    
    /*
    * Interface to stop the video capture
    */
    virtual void StopVideoCapture();
    
    /*
    * Interface to check if the video capture is active
    */
    virtual TBool VideoCaptureActive() const;
    
    /*
    *
    */
    virtual void EnumerateVideoFrameSizes(TSize& aSize,TInt aSizeIndex,TFormat aFormat) const;
    
    /*
    *
    */
    virtual void EnumerateVideoFrameRates(TReal32& aRate,TInt aRateIndex,TFormat aFormat,TInt aSizeIndex,TExposure aExposure = EExposureAuto) const;
    
    /*
    * Interface to fetch the frame size
    */
    virtual void GetFrameSize(TSize& aSize) const;
     
    /*
    * Interface to get the Frame Rate
    */
    virtual TReal32 FrameRate() const;
    
    /*
    * Interface to get the number of buffers in use
    */
    virtual TInt BuffersInUse() const;
    
    /*
    * Interface to get the number of frames in a buffer
    */
    virtual TInt FramesPerBuffer() const;
    
    /*
    * Interface to set the JPEG quality
    */
    virtual void SetJpegQuality(TInt aQuality);
    
    /*
    * Return the JPEG quality
    */
    virtual TInt JpegQuality() const;
     
    /*
    *
    */
    virtual TAny* CustomInterface(TUid aInterface);
    /*
    *
    */
    virtual void SetFlashL(TFlash aFlash = EFlashNone);
    /*
    *
    */
    virtual TFlash Flash() const;
    /*
    *
    */
    virtual void SetExposureL(TExposure aExposure = EExposureAuto);
    /*
    *
    */
    virtual TExposure Exposure() const;
    /*
    *
    */
    virtual void SetWhiteBalanceL(TWhiteBalance aWhiteBalance = EWBAuto);
    /*
    *
    */
    virtual TWhiteBalance WhiteBalance() const;
    /*
    *
    */
     

  // Private methods
  private:

    /*
    *
    */
    void InitPhotoResolutionsL();  

    /*
    *
    */
    static TInt ViewfinderCallBack( TAny* aSelf );   


  // Constructors
  private:

    /*
    * ConstructL
    */
    void ConstructL();
    
    /*
    * Constructor
    */
    CCamCamera( MCameraObserver& aObserver, TInt aCameraIndex );
    
    /*
    * Constructor
    */
    CCamCamera( MCameraObserver2& aObserver, TInt aCameraIndex, TInt aPriority ); 
     

  private:
    // Observers defined from CamCameraController
    MCameraObserver* iObserver;
    MCameraObserver2* iObserver2;
    
    // Call Back Related members
    CCamCameraCallBack* iCallBackActive;
    
    // Keeping track of the camera states (TCamCameraStateFlags ORed).
    // @see   TCamCameraStateFlags
    TUint         iState;
    
    TInt          iCameraIndex;
    TInt          iPriority;

    TCameraInfo   iInfo;

    TInt          iBrightness;
    TInt          iContrast;

    TWhiteBalance iWhiteBalance;
    TExposure     iExposure;
    TFlash        iFlash;

    TInt          iDigitalZoom;
    TInt          iZoomFactor;

    TSize         iViewfinderSize;
    TBool         iViewFinderMirror;

    TFormat       iFormat;
    TInt          iSizeIndex;
    TInt          iQuality;
    
    RArray<TSize> iPhotoResolutions;  
    CCameraappTestBitmapManager* iBitmapManager;  
    CPeriodic *iVfPeriodic;    
	};

#endif // CAM_CAMERA_H



