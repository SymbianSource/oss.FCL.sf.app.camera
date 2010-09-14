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
* Description:  Control for displaying remaining images in navi pane.
*/


#ifndef CAMNAVICOUNTERMODEL_H
#define CAMNAVICOUNTERMODEL_H

// INCLUDES
#include <bldvariant.hrh>
#include <coecntrl.h>
#include <AknUtils.h>
#include "CamAppController.h"
#include "CamSettings.hrh"
#include "CamResourceLoader.h"
#include "CamObservable.h"
#include "CamObserverHandler.h"

#include "CamSelfTimerObserver.h"

#include "CamBurstModeObserver.h"

#include "CamAppUiBase.h"

// CONSTANTS
const TInt KMaxTextLength = 40;

// FORWARD DECLARATIONS
class MAknsSkinInstance;
class CCamDecorator;
class CCamTextItem;
class CAknNavigationDecorator;
class CFbsBitmap;
// CLASS DECLARATION


/**
* Control for displaying recording time info in navi pane.
* @since 2.8
*/
class CCamNaviCounterModel : public CBase,
                                public MCamControllerObserver,
                                public MCamObservable,
                                public MCamResourceLoader, 
                                public MCamSelfTimerObserver,
                                public MCamBurstModeObserver

    {
    public: // Constructors and destructor
		/**
		*	Static factory construction function
        *   @since 2.8
		*	@param aController Reference to CCamAppController
		*	@return pointer to constructed instance of CCamNaviCounterModel
		*/
		static CCamNaviCounterModel* NewL( CCamAppController& aController );

        /**
        * Destructor.
        */
        ~CCamNaviCounterModel();

    private: // New functions
        /**
        * Update remaining image counter
        * @since 2.8
        */
        void UpdateCounter();

        /**
        * Update remaining time till next timelapse capture
        * @since 3.0
        */
        void UpdateTimeLapseCountdownL();
        
        /**
        * Update captured and remaining image count during burst and timelapse capture
        * @since 3.0
        */
        void UpdateSequenceImageCount();

        /**
        * Update remaining record time
        * @since 2.8
        */
        void UpdateRecordTimeAvailableL();

    public: // From MCamResourceLoader
        /**
        * From MCamResourceLoader
        * @since 3.0
        */
        void LoadResourceDataL();

        /**
        * From MCamResourceLoader
        * @since 3.0
        */
        void UnloadResourceData();

        /**
        * From MCamResourceLoader
        * @since 3.0
        */
        void ReloadResourceDataL();
    
    public: // From MCamObservable
        /**
        * From MCamObservable
        * @param aObserver Observer to register
        * @since 3.0
        */
        void RegisterObserverL(MCamObserver* aObserver);

        /**
        * From MCamObservable
        * @param aObserver Observer to deregister
        * @since 3.0
        */
        void DeregisterObserver(MCamObserver* aObserver);

        /**
        * From MCamObservable
        * @param aEvent Event code to broadcast
        * @since 3.0
        */
        void BroadcastEvent(TCamObserverEvent aEvent);
    
    public: // New functions
        /*
        * Draws counter and storage icon in the navi pane.
        */
        void CreateNaviBitmapsL( TBool aFlag = ETrue );
        
        /**
        * Set capture mode (defines which counter to use)
        * @since 2.8
        * @param aCaptureMode Specifies the current capture mode in use
        */
        void SetCaptureModeL( TCamCameraMode       aCaptureMode,
                              TCamImageCaptureMode aImageMode    );
        
        /**
        * Draw the storage icon
        * @since 2.8
        * @param aGc The graphics context to use
        */                     
        void DrawStorageIcon( CBitmapContext& aGc ) const;

        /**
        * Draw the video file type icon
        * @since 2.8
        * @param aGc The graphics context to use
        */                     
        void DrawVideoFileTypeIndicator( CBitmapContext& aGc ) const;
               
        /**
        * Force update of navi-pane (i.e after dismissal of MMC removed error note)
        * @since 2.8
        */
        void ForceNaviPaneUpdate();

        /**
        * Sets a new extent to render into
        * @param aExtent New extent
        */
        void SetExtentL( TRect aExtent );
        
    public: // Functions from base class MCamSelfTimerObserver
        /**
        * Handle an event from CCamSelfTimer.
        * @since 2.8
        * @param aEvent the self timer event
        * @param aCountDown self timer countdown duration
        */
        void HandleSelfTimerEvent( TCamSelfTimerEvent aEvent, 
            TInt aCountDown );

    public: // Functions from base class MCamBurstModeObserver
        /**
        * Handle sequence state change
        * @since 2.8
        * @param aActive ETrue if sequence now active, else EFalse
        */
        void BurstModeActiveL( TBool aActive, TBool aStillModeActive );

        /**
        * Draw the counter to the bitmap used in the navi pane
        * @since 3.0
        * @param aBmpGc The graphics context for the bitmap
        * @param aBmpMaskGc The graphics context for the bitmap mask
        */
	    void DrawCounterToBitmaps( CFbsBitGc& aBmpGc, CFbsBitGc& aBmpMaskGc ) const;	

		/**
		* Draw Navi-Counter
		* @since 3.0
		* @param aGc Handle to graphics context
		* @param aControl Pointer to the control
		*/
		void DrawNaviCtr( CBitmapContext& aGc, const CCoeControl* aControl ) const;

	protected:
		/**
        * C++ constructor.
        * @since 2.8
        * @param aController Reference to CCamAppController
        */
        CCamNaviCounterModel( CCamAppController& aController );
        
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        */
        void ConstructL();

    private: // Functions from base classes

        /**
        * Draw the navi-counter
        * @since 3.0
        * @param aGc Handle to graphics context
        * @param aControl Pointer to the control
        */
        void DrawCounter( CBitmapContext& aGc, const CCoeControl* aControl ) const;
        
        /**
        * Draw the storage icon to the bitmap used in the navi pane
        * @since 3.0
        * @param aBmpGc The graphics context for the bitmap
        * @param aBmpMaskGc The graphics context for the bitmap mask
        */
        void DrawStorageIconToBitmap( CFbsBitGc& aBmpGc, CFbsBitGc& aBmpMaskGc  ) const;

        /**
        * Draw the text 
        * @since 2.8
        * @param aSkin the current skin
        * @param aText A Reference to the text
        */
        void DrawText(       MAknsSkinInstance* aSkin, 
                       const TDesC&             aText, 
                             CBitmapContext&    aGc   ) const;

        /**
        * Draw the countdown text (timelapse mode)
        * @since 3.0
        * @param aSkin the current skin
        * @param aGc the graphics context
        */                       
        void DrawTimeLapseCountdown( MAknsSkinInstance* aSkin,
                                     CBitmapContext&    aGc   ) const;  
                                     
        /**
        * Draw the captured and remaining image text (timelapse and burst mode)
        * @since 3.0
        * @param aSkin the current skin
        * @param aGc the graphics context
        */  
        void DrawSequenceImageText( MAknsSkinInstance* aSkin,
                                    CBitmapContext&    aGc   ) const;    
             
        /**
        * Draw the count of captured images (timelapse mode - postcapture)
        * @since 3.0
        * @param aSkin the current skin
        * @param aGc the graphics context
        */                                       
        void DrawImagesCapturedTextL( MAknsSkinInstance* aSkin,
                                      CBitmapContext&    aGc   ) const;  

        /**
        * Draw the current file size
        * @since 3.0
        * @param aSkin the current skin
        */
        void DrawCurrentFileSizeL( MAknsSkinInstance* aSkin, 
                                   CBitmapContext&    aGc   ) const;

        /**
        * Construct the member variables for selftimer icon
        * in the navi-pane
        * @since 2.8
        * @param aResname The resource file name
        */
        void ConstructNaviSelfTimerL( TPtrC& aResname );
        
        /**
        * Draw the selftimer icon and text
        * @since 2.8
        * @param aGc The graphics context to use
        * @param aSkin The current skin
        */        
        void DrawNaviSelfTimer( CBitmapContext&    aGc, 
                                MAknsSkinInstance* aSkin ) const;           

        /**
        * Construct the member variables for sequence icon 
        * in the navi-pane
        * @since 2.8
        * @param aResname The resource file name
        */
        void ConstructNaviSequenceL( TPtrC& aResname );

        /**
        * Draw the sequence icon
        * @since 2.8
        * @param aGc The graphics context to use
        */                
        void DrawNaviSequence( CBitmapContext& aGc ) const;

#ifdef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
        /**
        * Construct the member variables for general setup icon 
        * in the navi-pane
        * @since 2.8
        * @param aResname The resource file name
        */
        void ConstructNaviGenericSetupL( TPtrC& aResname );
        
        /**
        * Draw the generic setup icon 
        * @since 2.8
        * @param aGc The graphics context to use
        */                
        void DrawNaviGenericSetup( CBitmapContext& aGc ) const;
#endif // PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR

        /**
        * Construct the member variables for audio mute icon 
        * in the navi-pane
        * @since 2.8
        * @param aResname The resource file name
        */
        void ConstructNaviAudioMuteL( TPtrC& aResname );
        
        /**
        * Draw the audio mute icon 
        * @since 2.8
        * @param aGc The graphics context to use
        */                
        void DrawNaviAudioMute( CBitmapContext& aGc ) const;


#ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE
        /**
        * Construct the member variables for mode subtitle 
        * in the navi-pane
        * @since 2.8
        * @param aResname The resource file name
        */
        void ConstructNaviModeSubtitleL( TPtrC& aResname );
        
        /**
        * Draw the mode subtitle icon and text
        * @since 2.8
        * @param aGc The graphics context to use
        * @param aSkin The current skin
        */                
        void DrawNaviModeSubtitle( CBitmapContext& aGc, MAknsSkinInstance* aSkin ) const;
#endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE

        /**
        * From MCamControllerObserver
        * Handle an event from CCamAppController.
        * @since 2.8
        * @param aEvent the type of event
        * @param aError error code
        */
        void HandleControllerEventL( TCamControllerEvent aEvent,
                                    TInt aError );
        
        /**
        * Read nontouch layout for primary camera
        */
        void NonTouchLayoutL();
        
        /**
        * Read nontouch layout for primary camera
        */
        void NonTouchLayoutSecondaryL();

        /**
        * Read touch layout
        */
        void TouchLayoutL(); 

    protected: //data
       TInt iOriginalValueForEachBurst;  
       TBool iCounterNeedUpdate;  
        CCamAppController& iController; // camera app controller

        // text array - used to store remaining images as text
        TBuf<KMaxTextLength> iCounterText;
        
        // string used to store remaining time till next 
        // timelapse capture 
        HBufC* iCountDownText;
        
        // text array - used to store captured and remaining image
        // count in timelapse and burst modes
        TBuf<KMaxTextLength> iSequenceImageText;

        // text array - used to store remaining record time as text
        TBuf<KMaxTextLength> iRemainingTimeText;

        // Layout information for drawing the counter text.
        TAknLayoutText iPhotoTextLayout;
        TAknLayoutText iVideoTextLayout;

        // own
        CCamTextItem* iPhotoTextItem;

        // own
        CCamTextItem* iVideoTextItem;

        // Text items for drawing timelapse and burst text.
        // own
        CCamTextItem* iSequenceImageTextItem;

        // own
        CCamTextItem* iSequenceCapturedTextItem;

        // own
        CCamTextItem* iTimeLapseCountdownTextItem;

#ifdef PRODUCT_SUPPORTS_NAVIPANE_FILENAME        
        // Layout information for drawing the current filename text.
        TAknLayoutText iPhotoNameLayout;
        TAknLayoutText iVideoNameLayout;
#endif // PRODUCT_SUPPORTS_NAVIPANE_FILENAME    

        // current capture mode info
        TCamCameraMode       iMode;
        TCamImageCaptureMode iImageMode;

        // storage location bitmaps
        CFbsBitmap* iPhoneIcon;
        CFbsBitmap* iPhoneIconMask;
        CFbsBitmap* iMMCIcon;
        CFbsBitmap* iMMCIconMask;
        CFbsBitmap* iMassStorageIcon;
        CFbsBitmap* iMassStorageIconMask;
        
        CFbsBitmap* iMpeg4Icon;
        CFbsBitmap* iMpeg4IconMask;
        CFbsBitmap* i3GPIcon;
        CFbsBitmap* i3GPIconMask;

        // current storage location (phone or card)
        TCamMediaStorage iStorageLocation;

        // set to ETrue when engine has initialised video recorder
        TBool iVideoInitialised;

        // Layout information for the video storage icon (in postcapture state).
        TAknLayoutRect iVidPostStorageIconRect;
        
        // Layout information for the video storage icon (in precapture state).
        TAknLayoutRect iVidPreStorageIconRect;
        
        // Layout information for the image storage icon.
        TAknLayoutRect iImgStorageIconRect;

        // Layout information for the video file type icon.
        TAknLayoutRect iVideoFileTypeIconRect;
                
        // Layout information for drawing the self timer icon.
        TAknLayoutRect iSelfTimerIconRect;

        // Self timer bitmaps.
        CFbsBitmap* iSelfTimerIcon;
        CFbsBitmap* iSelfTimerMask;

        // Specifies whether the self timer should be drawn or not.
        TBool iDrawSelfTimer;

        // Layout information for drawing the self timer text.
        TAknLayoutText iSelfTimerTextLayout;

        // text array - used to store self timer countdown as text
        TBuf<KMaxTextLength> iSelfTimerText;

#ifdef PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE
        // Specifies whether the sequence should be drawn or not.
        TBool iDrawSequence;        
#endif // PRODUCT_SUPPORTS_NAVIPANE_SEQUENCE

        // Layout information for drawing the sequence icon.
        TAknLayoutRect iSequenceIconRect;

        // Sequence bitmap and mask
        CFbsBitmap* iSequenceIcon;
        CFbsBitmap* iSequenceMask;

#ifdef PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR
        // Layout information for drawing the generic setup icon.
        TAknLayoutRect iGenericIconRect;

        // Generic setup bitmap and mask
        CFbsBitmap* iGenericIcon;
        CFbsBitmap* iGenericMask;
#endif // PRODUCT_SUPPORTS_NAVIPANE_GENERIC_SETUP_INDICATOR

        // Layout information for drawing the audio mute icon.
        TAknLayoutRect iAudioMuteIconRect;

        // Audio mute bitmap and mask
        CFbsBitmap* iAudioMuteIcon;
        CFbsBitmap* iAudioMuteMask;

#ifdef PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE   
        // Layout information for the subtitles text
        TAknLayoutText iSubtitlePhotoTextLayout;
        TAknLayoutText iSubtitleVideoTextLayout;

        // Text strings for photo/video subtitles
        HBufC16* iSubtitlePhotoText;
        HBufC16* iSubtitleVideoText;                

        // Layout information for the subtitle icons (video and photo mode)
        TAknLayoutRect iSubtitlePhotoRect;
        TAknLayoutRect iSubtitleVideoRect;                

        // Photo and video mode bitmaps and masks
        CFbsBitmap* iSubtitlePhotoIcon;
        CFbsBitmap* iSubtitlePhotoMask;
        CFbsBitmap* iSubtitleVideoIcon;
        CFbsBitmap* iSubtitleVideoMask;        
#endif // PRODUCT_SUPPORTS_NAVIPANE_MODE_SUBTITLE

        // Layout information for Photo size text
        TAknLayoutText iPhotoSizeLayout;

        // Layout information for Video size text
        TAknLayoutText iVideoSizeLayout;
        
        // Resource string used to format the time text
        HBufC* iTimeFormat;

        // set to ETrue if burst (sequence) mode enabled
        TBool iBurstActive;
        
        // Current orientation
        TCamOrientation iCamOrientation;
        
        // Decorators for various layouts
        CCamDecorator* iPhotoPrecapDecorator;
        CCamDecorator* iVideoPrecapDecorator;
        CCamDecorator* iSequenceInCaptureDecorator;
        CCamDecorator* iPhotoPostcapDecorator;
        CCamDecorator* iVideoPostcapDecorator;
        CCamDecorator* iTimeLapsePostCaptureDecorator;
        
        // Where the counter should be drawn
        TRect iExtent;
        
        // To help handle the observers
        CCamObserverHandler* iObserverHandler;
        
        TBool iLocationIconVisible;
        
        // Used to display items in the navi pane
        CAknNavigationDecorator* iNaviDec;        
        CFbsBitmap* iNaviBitmap;
        CFbsBitmap* iNaviBitmapMask;
    };

#endif // CAMNAVICOUNTERMODEL_H

// End of File
