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
* Description:  Dummy Camera Application Engine*
*/



#ifndef CAMERAAPPDUMMYENGINE_H
#define CAMERAAPPDUMMYENGINE_H

//  INCLUDES
#include <CaeEngine.h>

// FORWARD DECLARATIONS
class CCameraappDummyCallbackActive;

// CLASS DECLARATION  
    
/**
* Dummy Camera Application Engine interface 
*
*/
class CCameraappDummyEngine : public CCaeEngine
    {
    public: // Creation

        /**
        * A static (factory) function to create the class instance.
        * @since 2.1
        * @return Pointer to a fully constructed CCaeEngine object. 
        * Ownership is passed to the caller.
        */
        static CCaeEngine* NewL();
    };


class CCameraappDummyEngineImp : public CCameraappDummyEngine
    {
    public: // Creation
        static CCameraappDummyEngineImp* NewL();

        /**
        * Sets the Camera Application Engine observer.
        * @since 2.1
        * @param aObserver Reference to Camera Application Engine observer.
        * @return void
        */
        void SetCamAppEngineObserver( 
            MCamAppEngineObserver& aObserver );

        ~CCameraappDummyEngineImp();

    public: // General methods

        /**
        * Gets information about the supported Camera Application Engine and 
        * Camera features.
        * @since 2.1
        * @param aInfo Info about supported features.
        * @return void
        */
        void GetInfo( TCamAppEngineInfo& /*aInfo*/ ) const;

        /**
        * Initializes Camera Application Engine for still image capturing. 
        * Reserves the camera and switches the camera power on. 
        * Intialization must be done before all other operations, except 
        * SetCamAppEngineObserver and GetInfo. 
        * Calls observer method McaeoInitComplete().
        * @since 2.1
        * @param aCreateSnapImage Boolean indicating if "snap-images" 
        * should be created, leading to calls of McaeoSnapImageReady.
        * @return void
        */
        void InitL( TBool aCreateSnapImage = ETrue );

        /**
        * Initialize the engine for still image capturing. 
        * Switches also camera power on. 
        * Intialization must be done before all other operations, except 
        * SetCamAppEngineObserver and GetInfo. 
        * Calls observer method McaeoInitComplete().
        * @since 2.1
        * @param aSdUidStillQltyLevels Still capturing quality levels Shared Data initialization file UID.
        * @param aCreateSnapImage Boolean indicating if "snap-images" should be created 
        * leading to calls of McaeoSnapImageReady.
        * @return void
        */
        void InitL( TUid /*aSdUidStillQltyLevels*/, 
                            TBool /*aCreateSnapImage = ETrue*/ ){};

        /**
        * Initializes video recording. 
        * The initialization must be done before video recording operations.
        * @since 2.1
        * @return void
        */
        void InitVideoRecorderL(){};

        /**
        * Initializes video recording. 
        * The initialization must be done before video recording operations.
        * @since 2.1
        * @param aSdUidVideoQltyLevels Video recording quality levels Shared Data initialization file UID.
        * @return void
        */
        void InitVideoRecorderL( TUid /*aSdUidVideoQltyLevels*/ ){};

        /**
        * Reserves the camera into use and switches the camera power on. 
        * Calls observer method McaeoInitComplete().
        * @since 2.1
        * @return void
        */
        void Reserve();
        
        /**
        * Switches the camera power off and releases the camera.
        * @since 2.1
        * @return void
        */
        void Release(){};

        /**
        * Switches the camera power on. 
        * Calls observer method McaeoInitComplete().
        * @since 2.1
        * @return void
        */
        void PowerOn(){};
        
        /**
        * Switches the camera power off.
        * @since 2.1
        * @return void
        */
        void PowerOff(){};

    public: // Camera settings

        /**
        * Sets the zoom mode.
        * @since 2.1
        * @param aZoomMode Zoom mode to set.
        * @return void
        */
        void SetZoomModeL( TZoomMode /*aZoomMode = EZoomModeDigital*/ );

        /**
        * Gets the current zoom mode.
        * @since 2.1
        * @return The current zoom mode.
        */
        TZoomMode ZoomMode() const{ return EZoomModeDigital; };

        /**
        * Sets the zoom value.
        * @since 2.1
        * @param aZoomValue Index of the zoom factor to set.
        * @return void
        */
        void SetZoomValueL( TInt /*aZoomValue = 0*/ );

        /**
        * Gets the current zoom value.
        * @since 2.1
        * @return Index of the current zoom factor.
        */
        TInt ZoomValue() const;

        /**
        * Sets the brightness.
        * @since 2.1
        * @param aBrightness Brightness value in the range ECam.h SetBrightnessL allows.
        * @return void
        */
        void SetBrightnessL( TInt /*aBrightness = 0*/ ){}; 

        /**
        * Gets the current brightness setting.
        * @since 2.1
        * @return Brightness value.
        */
        TInt Brightness() const{ return 0; };

        /**
        * Sets the contrast.
        * @since 2.1
        * @param aContrast Contrast value in the range ECam.h SetContrastL allows.
        * @return void
        */
        void SetContrastL( TInt/* aContrast= 0*/ ){}; 
        
        /**
        * Gets the current contrast setting.
        * @since 2.1
        * @return Contrast value.
        */
        TInt Contrast() const{ return 0; };

        /**
        * Sets the exposure mode.
        * @since 2.1
        * @param aExposureMode Exposure mode in the range ECam.h SetExposureL allows.
        * @return void
        */
        void SetExposureModeL( 
            CCamera::TExposure /*aExposureMode = CCamera::EExposureAuto*/ ){};

        /**
        * Gets the current exposure mode.
        * @since 2.1
        * @return Exposure mode.
        */
        CCamera::TExposure ExposureMode() const{ return CCamera::EExposureAuto; };

        /**
        * Sets the white balance mode.
        * @since 2.1
        * @param aWhiteBalanceMode White balance mode in the range ECam.h SetWhiteBalanceL allows.
        * @return void
        */
        void SetWhiteBalanceModeL( 
            CCamera::TWhiteBalance /*aWhiteBalanceMode = CCamera::EWBAuto*/ ){};

        /**
        * Gets the current white balance mode.
        * @since 2.1
        * @return White balance mode.
        */
        CCamera::TWhiteBalance WhiteBalanceMode() const{ return CCamera::EWBAuto; };

        /**
        * Sets the flash mode.
        * @since 2.1
        * @param aFlashMode Flash mode in the range ECam.h SetFlashL allows.
        * @return void
        */
        void SetFlashModeL( 
            CCamera::TFlash /*aFlashMode = CCamera::EFlashNone*/ ){};

        /**
        * Gets the current flash mode.
        * @since 2.1
        * @return Flash mode.
        */
        CCamera::TFlash FlashMode() const{ return CCamera::EFlashNone; };

        /**
        * Resets to the default setting values.
        * Resets the following settings: exposure mode, white balance mode, 
        * zoom mode, zoom value, flash mode, brightness, and contrast. 
        * @since 2.1
        * @return void
        */
        void ResetToDefaultsL(){};
    
    public: // View finder

        /**
        * Sets/resets view finder image mirroring.
        * @since 2.1
        * @param aMirror Boolean indicating whether view finder image should be mirrored or not.
        * @return void
        */
        void SetViewFinderMirrorL( TBool /*aMirror = EFalse*/ ){};

        /**
        * Queries if view finder image mirroring is on.
        * @since 2.1
        * @return Boolean indicating whether view finder image is currently mirrored or not.
        */
        TBool ViewFinderMirror() const{ return EFalse; };

        /**
        * Starts the bitmap-based view finder.
        * The engine starts to call observer method McaeoViewFinderFrameReady() repeatedly.
        * @since 2.1
        * @param aSize Desired view finder size.
        * @return void
        */
        void StartViewFinderBitmapsL( TSize& aSize );

        /**
        * Starts the bitmap-based view finder.
        * The engine starts to call observer method McaeoViewFinderFrameReady() repeatedly.
        * @since 2.1
        * @param aSize Desired view finder size.
        * @param aCropRect Desired cropping/clipping rectangle.
        * @return void
        */
        void StartViewFinderBitmapsL(
            TSize& /*aSize*/, 
            TRect& /*aCropRect*/ ){};

        /**
        * Starts the direct screen access view finder.
        * @since 2.1
        * @param aWs Window server session.
        * @param aScreenDevice Software device screen.
        * @param aWindow Client-side handle to a server-side window.
        * @param aScreenRect Portion of screen in screen co-ordinates.
        * @return void
        */
        void StartViewFinderDirectL( 
            RWsSession& /*aWs*/, 
            CWsScreenDevice& /*aScreenDevice*/, 
            RWindowBase& /*aWindow*/, 
            TRect& /*aScreenRect*/ ){};
        
        /**
        * Starts the direct screen access view finder.
        * @since 2.1
        * @param aWs Window server session.
        * @param aScreenDevice Software device screen.
        * @param aWindow Client-side handle to a server-side window.
        * @param aScreenRect Portion of screen in screen co-ordinates.
        * @param aCropRect Desired cropping/clipping rectangle.
        * @return void
        */
        void StartViewFinderDirectL(
            RWsSession& /*aWs*/, 
            CWsScreenDevice& /*aScreenDevice*/, 
            RWindowBase& /*aWindow*/, 
            TRect& /*aScreenRect*/, 
            TRect& /*aCropRect*/ ){};

        /**
        * Stops the view finder.
        * @since 2.1
        * @return void
        */
        void StopViewFinder();

        /**
        * Queries if view finder is running.
        * @since 2.1
        * @return Boolean indicating whether view finder is running or not.
        */
        TBool IsViewFinding() const;

        /**
        * Gets the current view finder size.
        * @since 2.1
        * @param aSize View finder size.
        * @return void
        */
        void GetViewFinderSize( TSize& /*aSize*/ ){};

    public: // Still image capturing

        /**
        * Prepares Camera Application Engine for still image capture on 
        * specified quality level.
        * Calls observer method McaeoStillPrepareComplete().
        * @since 2.1
        * @param aStillQualityIndex Still image quality level index.
        * Imaging parameters linked to quality levels are defined by Shared Data ini-file. 
        * @return void
        */
        void PrepareStillCaptureL( TInt /*aStillQualityIndex*/ ){};

        /**
        * Prepares Camera Application Engine for still image capture on 
        * specified quality level.
        * Calls observer method McaeoStillPrepareComplete().
        * @since 2.1
        * @param aStillQualityIndex Still image quality level index. 
        * Can have values 0 ... TCamAppEngineInfo::iNumStillQualityLevelsSupported - 1
        * @param aCropRect Desired cropping/clipping rectangle.
        * @return void
        */
        void PrepareStillCaptureL( 
            TInt /*aStillQualityIndex*/, 
            const TRect& /*aCropRect*/ ){};

        /**
        * Prepares Camera Application Engine for still image capture with 
        * specified image size and format.
        * Calls observer method McaeoStillPrepareComplete().
        * @since 2.1
        * @param aSize Still image size.
        * @param aFormat Still image format.
        * @param aCompressionQuality Still image compression quality [0...100].
        * @return void
        */
        void PrepareStillCaptureL(
            const TSize& /*aSize*/, 
            CCamera::TFormat /*aFormat*/,  
            TInt /*aCompressionQuality*/ ){};

        /**
        * Prepares Camera Application Engine for still image capture with specified image size and format.
        * Calls observer method McaeoStillPrepareComplete().
        * @since 2.1
        * @param aSize Still image size.
        * @param aFormat Still image format.
        * @param aCompressionQuality Still image compression quality [0...100].
        * @param aCropRect Desired cropping/clipping rectangle.
        * @return void
        */
        void PrepareStillCaptureL(
            const TSize& /*aSize*/, 
            CCamera::TFormat /*aFormat*/,  
            TInt /*aCompressionQuality*/, 
            const TRect& /*aCropRect*/ ){};

        /**
        * Gets the current prepared still quality level index.
        * @since 2.1
        * @return Still quality level index.
        */
        TInt StillQualityIndex() const{ return 0; };

        /**
        * Gets the still image frame size for the specified quality level.
        * @since 2.1
        * @return void
        */
        void GetStillFrameSize(
            TInt /*aStillQualityIndex*/, 
            TSize& /*aSize*/ ) const{};

        /**
        * Gets the estimated still image size in bytes for the 
        * specified quality level.
        * @since 2.1
        * @return Estimated still capturing image size in bytes.
        */
        TInt EstimatedStillSizeInBytes( 
            TInt /*aStillQualityIndex*/ ) const{ return 0; };

        /**
        * Captures a still image or a still image burst.
        * Calls first observer method McaeoSnapImageReady() and then 
        * McaeoStillImageReady().
        * McaeoSnapImageReady() is not called if engine initialization 
        * has been done 
        * with parameter aCreateSnapImage == EFalse.
        * @since 2.1
        * @return void
        */
        void CaptureStill();

        /**
        * Cancels the on-going still image (single/burst) capture request.
        * @since 2.1
        * @return void
        */
        void CancelCaptureStill(){};

        /** 
        * Sets the size of the snap image bitmap.
        * @since 3.0
        * @param aSize The size of the snap bitmap. Corrected size is returned. 
        * @return void
        */
        void SetSnapImageSizeL(
            TSize& ){};
         /** 
        * Sets the color mode of the snap image bitmap
        * @since 3.0
        * @param aMode The color mode of the snap bitmap.
        * @return void
        */
        void SetSnapImageColorMode(
        TDisplayMode ){};

    public: // Video recording

        /**
        * Sets the file name for the video clip to be recorded.
        * @since 2.1
        * @param aVideoClipFileName File name.
        * @return void
        */
        void SetVideoRecordingFileNameL( 
            const TDesC& aVideoClipFileName );

        /**
        * Prepares Camera Application Engine for video recording on 
        * specified quality level.
        * Calls observer method McaeoVideoPrepareComplete().
        * @since 2.1
        * Can have values 0 ... TCamAppEngineInfo::iNumVideoQualityLevelsSupported - 1
        * @return void
        */
        void PrepareVideoRecordingL( TInt /*aVideoQualityIndex*/ ){};

        /**
        * Prepares Camera Application Engine for video recording with 
        * specified recording parameters.
        * Calls observer method McaeoVideoPrepareComplete().
        * @since 2.1
        * @return void
        */
        void PrepareVideoRecordingL( 
            const TSize& /*aFrameSize*/, 
            TReal32 /*aFrameRate*/, 
            TInt /*aBitRate*/, 
            TBool /*aAudioEnabled*/,
            const TDesC8& /*aMimeType*/, 
            const TDesC& /*aPreferredSupplier*/, 
            const TDesC8& /*aVideoType = KNullDesC8*/, 
            const TDesC8& /*aAudioType = KNullDesC8*/ ){};

         /**
        * Closes (prepared) video recording to free resources.
        * @since 2.1
        * @return void
        */
        void CloseVideoRecording(){};

        /**
        * Gets the current prepared video quality level index.
        * @since 2.1
        * @return Video quality level index.
        */
        TInt VideoQualityIndex() const{ return 0; };
        
        /**
        * Gets the video frame size for the specified video quality level.
        * @since 2.1
        * @return Video frame size.
        */
        void GetVideoFrameSize(
            TInt /*aVideoQualityIndex*/, 
            TSize& /*aSize*/ ) const{};

        /**
        * Gets the video frame rate for the specified video quality level.
        * @since 2.1
        * @return Video frame rate.
        */
        TReal32 VideoFrameRate( TInt /*aVideoQualityIndex*/ ) const{ return 0; };

        /**
        * Gets the estimated video recording bit rate including audio bit rate 
        * for the specified video quality level.
        * @since 2.1
        * @return Video recording bit rate rate including audio bit rate.
        */
        TInt EstimatedVideoRecordingBitRateL( 
            TInt /*aVideoQualityIndex*/ ) const{ return 0; };

        /**
        * Sets the maximum size in bytes for a video clip.
        * @since 2.1
        * If set to 0, then no limit but the available storage space.
        * @return void
        */
        void SetVideoClipMaxSizeL( TInt /*aMaxClipSizeInBytes = 0*/ ){};
        
        /**
        * Gets the current maximum size in bytes for a video clip.
        * @since 2.1
        * @return Video clip maximum size.
        */
        TInt VideoClipMaxSize() const{ return 0; };

        /**
        * Sets audio enabled or disabled for video recording.
        * @since 2.1
        * @return void
        */
        void SetVideoAudioL( TBool /*aAudioEnabled*/ ){};
        
        /**
        * Queries if audio is enabled or disabled for video recording.
        * @since 2.1
        * @return Boolean indicating whether audio is enabled or disabled for video recording.
        */
        TBool VideoAudio() const{ return EFalse; };

        /**
        * Sets the time interval for video recording time info.
        * @since 2.1
        * @return void
        */
        void SetVideoTimesIntervalL( 
            TTimeIntervalMicroSeconds /*aInterval*/ ){};

        /**
        * Gets the current time interval for video recording time info.
        * @since 2.1
        * @return Time interval in microseconds for video recording time info.
        */
        TTimeIntervalMicroSeconds VideoTimesInterval() const{ return TTimeIntervalMicroSeconds( 0 ); };

        /**
        * Starts video recording.
        * Calls observer method McaeoVideoRecordingOn().
        * @since 2.1
        * @return void
        */
        void StartVideoRecording();

        /**
        * Stops video recording.
        * Calls observer method McaeoVideoRecordingComplete().
        * @since 2.1
        * @return void
        */
        void StopVideoRecording();

        /**
        * Pauses video recording.
        * Calls observer method McaeoVideoRecordingPaused().
        * @since 2.1
        * @return void
        */
        void PauseVideoRecording();

        /**
        * Resumes video recording after pause.
        * Calls observer method McaeoVideoRecordingOn().
        * @since 2.1
        * @return void
        */
        void ResumeVideoRecording();

        /**
        * Gets the remaining video recording time before recording has started.
        * @since 2.1
        * @return Remaining video recording time.
        */
        TTimeIntervalMicroSeconds RemainingVideoRecordingTime()  
            const{ return TTimeIntervalMicroSeconds( 99999999 ); };

        /**
        * Queries if video recording is running.
        * @since 2.1
        * @return Boolean indicating whether video recording is running or not.
        */
        TBool IsVideoRecording() const{ return EFalse; };

    public: // New overloading method for preparing video recording

        /**
        * Prepares Camera Application Engine for video recording with specified recording parameters.
        * Calls observer method McaeoVideoPrepareComplete().
        * @since 2.1
        * @return void
        */
        void PrepareVideoRecordingL(
            const TSize&  /*aFrameSize*/, 
            TReal32       /*aFrameRate*/, 
            TInt          /*aBitRate*/, 
            TBool         /*aAudioEnabled*/,
            TInt          /*aAudioBitRate*/, 
            const TDesC8& /*aMimeType*/, 
            const TDesC&  /*aPreferredSupplier*/, 
            const TDesC8& /*aVideoType*/, 
            const TDesC8& /*aAudioType*/ );

    public: // Still image burst capturing

        /**
        * Sets the Camera Application Engine still image burst capturing 
        * observer.
        * @since 2.1
        * @param aObserver Camera Application Engine still burst observer.
        * @return void
        */
        void SetCaeStillBurstObserver( MCaeStillBurstObserver& aObserver );

        /**
        * Sets the image count for still image (burst) capturing.
        * Note: Inputting value 1 switches back to normal still capturing.
        * @since 2.1
        * @param aImageCount The desired count of images to capture (in a burst). 
        * @return The count of images to be captured (in a burst).
        */
        TInt SetStillCaptureImageCountL( TInt aImageCount );

        /**
        * Gets the image count for still image (burst) capturing.
        * @since 2.1
        * @return Count of images to capture (in a burst).
        */
        TInt StillCaptureImageCount() const{ return 0; };

        /**
        * Sets the time interval between single captures in still burst.
        * @since 2.1
        * @return void
        */
        void SetStillBurstCaptureIntervalL( 
            TTimeIntervalMicroSeconds /*aInterval*/ ){};

        /**
        * Gets the time interval between single captures in still burst.
        * @since 2.1
        * @return Time interval in microseconds.
        */
        TTimeIntervalMicroSeconds StillBurstCaptureInterval() const{ return TTimeIntervalMicroSeconds( 0 ); };

        /**
        * Stops still image burst capturing. 
        * Delivers the burst images captured before call. 
        * This is different from CancelCaptureStill that cancels the operation and 
        * prevents delivering the captured image.
        * @since 2.1
        * @return void
        */
        void StopStillBurstCapture();

    public: 

        /**
        * Prepare Camera Application Engine for still image capture on 
        * specified quality level with desired snap image size. 
        * The returned snap image size is always equal or bigger in both 
        * dimensions than the given size. The returned size is stepwise 
        * downscaled from the captured image size. The parameter aSnapSize 
        * is ignored if the image format does not support it in the engine.
        * Calls observer method McaeoStillPrepareComplete().
        * @since 2.1
        * @return void
        */
        void PrepareStillCaptureL(
            TInt /*aStillQualityIndex*/, 
            TSize& /*aSnapSize*/ ){};

        /**
        * Prepare Camera Application Engine for still image capture with 
        * specified image size, format, and desired snap image size. 
        * The returned snap image size is always equal or bigger in both 
        * dimensions than the given size. The returned size is stepwise 
        * downscaled  from the captured image size. The parameter aSnapSize 
        * is ignored if the image format does not support it in the engine.
        * Calls observer method McaeoStillPrepareComplete().
        * @since 2.1
        * @param aSize Still image size.
        * @param aFormat Still image format.
        * @param aCompressionQuality Still image compression quality [0...100].
        * @param aCropRect Desired cropping/clipping rectangle. The size (0,0) means that this parameter is ignored.
        * @param aSnapSize Desired snap image size. Returns the real snap image size.
        * @return void
        */
        void PrepareStillCaptureL(
            const TSize& aSize, 
            CCamera::TFormat aFormat,  
            TInt aCompressionQuality, 
            const TRect& aCropRect,
            TSize& aSnapSize );

        /**
        * Sets the exposure value (EV) compensation index.
        * @since 2.1
        * @return void
        */
        void SetEvCompensationL( TInt /*aEvIndex = 0*/ ){};

        /**
        * Gets the current exposure value (EV) compensation index.
        * @since 2.1
        * @return Exposure value compensation index.
        */
        TInt EvCompensation() const{ return 0; };
                
        /**
        * Sets snap image creation on/off.
        * @since 2.1
        * @return void
        */
        void SetSnapImageCreation( 
            TBool /*aCreateSnapImage = ETrue*/ ){};
        
        /** 
        * A static (factory) function to create the class instance.
        * @since 2.6
        * @return Pointer to a fully constructed CCaeEngine object. 
        * Ownership is passed to the caller.
        */
        IMPORT_C static CCaeEngine* NewL( 
            TInt /*aCameraIndex*/ );
        
        /** 
        * Gets the number of cameras on the device.
        * @since 2.6
        * @return Count of cameras present on the device. 
        */
        IMPORT_C static TInt CamerasAvailable();
        
        /** 
        * Enumerates through the available image capture sizes. 
        * Returns the image size based on the specified size index and format. 
        * The largest image resolution is returned for index 0, the smallest 
        * for index TCamAppEngineInfo::iNumImageSizesSupported-1.
        * The size index must be in the range 0 to 
        * TCamAppEngineInfo::iNumImageSizesSupported-1 inclusive.
        * @since 2.6
        * @return void
        */
        void EnumerateStillCaptureSizes(
            TSize& /*aSize*/, 
            TInt /*aSizeIndex*/, 
            CCamera::TFormat /*aFormat*/ ) const{};
        
        /** 
        * Sets the quality value to use with JPEG and Exif image formats. 
        * @since 2.6
        * @return void
        */
        void SetJpegQuality(
            TInt /*aQuality*/ ){};
        
        /** 
        * Gets the currently set JPEG quality value.
        * Returns 0 if not previously prepared or set.
        * @since 2.6
        * @return The currently set JPEG quality value. 
        */
        TInt JpegQuality() const{ return 0; };

	public: // New method to allow extension mechanism.
        TAny* CustomInterface(TUid /*aId*/) { return NULL; };

        /**
        * Gets array of features per interfaces. Each feature is ordered in
        * the current calling order e.g. when certain image prosessing 
        * operation is applied to the captured image. If SetOrder() has not yet
        * been called, the returned order is the default order.
        * @since 2.8
        * @param aOrderedFeatureArray Feature array sorted by the current calling order in an interface.
        * @return  void
        */
        void GetOrderL( RCaeOrderedFeatureList& aOrderedFeatureList );

        /**
        * Sets array of features per interfaces. Each feature should be ordered
        * according to the wanted calling order e.g. to define when certain image 
        * prosessing operation is applied to the captured image. If this method
        * leaves, the internal array has not been changed.
        * @since 2.8
        * @param aOrderedFeatureArray Feature array sorted by the wanted calling order in an interface.
        * @return void. 
        */
        void SetOrderL( const RCaeOrderedFeatureList& aOrderedFeatureList );

        /** 
        * Sets the source of the snap image bitmap.
        * @since 2.8
        * @param aSnapImageSource The source of the snap image bitmap.
        * @return void
        */
        void SetSnapImageSourceL( TSnapImageSource aSnapImageSource );
        
        /** 
        * Sets the specific image codec implementation to be used in decoding and encoding.
        * @since 3.0
        * @param aDecoderUid The UID of the specific image decoder to be used. KNullUid means the default decoder.
        * @param aEncoderUid The UID of the specific image encoder to be used. KNullUid means the default encoder.
        * @return void
        */  
        void SetImageCodecsL( TUid aDecoderUid, TUid aEncoderUid );

        /** 
        * Enumerate Video Frame Size
        * @since 3.2
        * @see CCaeEngine
        */
        TInt EnumerateVideoFrameSizeL( const TSize& /*aSize*/ );
        
        /** 
        * Gets the device-unique handle of camera object owned by the CCaeEngine.
        * @since 3.2
        * @see CCaeEngine
        */
        TInt CCameraHandle() const;

        /**
        * Set CamAppEngine internal camera state to reserved and power state on. 
        * @since 3.2
        * @see CCaeEngine
        */
        void EnableVideoRecording();
        
        /**
        * Set CamAppEngine internal camera state to released and power state off.
        * @since 3.2
        * @see CCaeEngine
        */
        void DisableVideoRecording();
        
        /** 
        * Sets CAE to Extension processing mode.
        * @since 3.2
        * @param aExtModeActive Boolean indicating if extension mode is enabled or disabled.
        * @param aCreateSnapImage Boolean indicating if "snap-images" should be created by CAE.
        * @return void
        */
        void SetCaeExtensionModeL( TBool aExtModeActive, TBool aCreateSnapImage );

        /** 
        * Process extension for view finder.
        * @since 3.2
        * @param aFrame The view finder frame bitmap that is processed.
        * @return void
        */
        void ProcessExtViewFinderFrameReadyL( CFbsBitmap& aFrame );

        /** 
        * Process extension for snap image.
        * @since 3.2
        * @param aSnapImage The bitmap of snap image. Does not transfer ownership.
        * @return void
        */
        void ProcessExtSnapImageL( CFbsBitmap& aSnapImage );
        
        /** 
        * Process extension for captured image.
        * @since 3.2
        * @param aBitmap The captured bitmap image. Does not transfer ownership.
        * @param aLastImage Set when last image of the burst is completed.
        * @return void
        */
        void ProcessExtCapturedImageL( CFbsBitmap& aBitmap, TBool aLastImage );

        /** 
        * Process extension for captured image.
        * @since 3.2
        * @param aImageData The captured image, if it is a formatted image and type of HBufC8. Ownership is transferred.
        * @param aLastImage Set when last image of the burst is completed.
        * @return void
        */
        void ProcessExtCapturedImageL( HBufC8* aImageData, TBool aLastImage );

        /** 
        * Process extension for captured image.
        * @since 3.2
        * @param aImageDataDes The captured image, if it is a formatted image and type of TDesC8. Does not transfer ownership.
        * @param aLastImage Set when last image of the burst is completed.
        * @return void
        */
        void ProcessExtCapturedImageL( TDesC8& aImageDataDes, TBool aLastImage );

        /** 
        * Cancels extensions processing
        * @since 3.2
        * @return void
        */
        void ProcessExtCancel();

        /** 
        * Get list of skipped extensions. Application may define which extensions are skipped during image processing. 
        * @since 3.2
        * @param aSkippedExtensions The list of skipped extensions TUid. 
        * @return void
        */
        void SkippedExtensionListL( RCaeOrderedFeatureList& aSkippedExtensions );

        /** 
        * Set list of skipped extensions. Application may define which extensions are skipped during image processing.
        * The GetOrderL() returns the list of all installed extensions.
        * @since 3.2
        * @param aSkippedExtensions The list of installed and skipped extensions TUid. 
        * @return void
        */
        void SetSkippedExtensionList( RCaeOrderedFeatureList& aSkippedExtensions );

        TInt SetAsyncVideoStopMode( TBool aAsyncVideoStopEnabled );

    private:
        CCameraappDummyEngineImp();
        void ConstructL();

    protected: // data
        MCamAppEngineObserver* iObserver;
        CCameraappDummyCallbackActive* iCallBackActive;
        TBool iIsViewFinding;
        TInt iZoomValue;
        TInt iImageCount;
        TBool iVideoPrepared;
        CCaeEngine::TZoomMode iZoomMode;        
    };


#endif // CAMERAAPPDUMMYENGINE_H
