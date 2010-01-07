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
* Description:  Utility functions for the Camera Application*
*/




#ifndef CAMUTILITY_H
#define CAMUTILITY_H

//  INCLUDES
#include <e32base.h>
#include <CAknMemorySelectionSettingPage.h>
 
#include <bldvariant.hrh>
#include "CamPSI.h"	// Product Specific Information
#include "CamSettings.hrh"  // for TCamMediaStorage
#include "CamSettingsInternal.hrh"
#include "camlogging.h"
#include "camconfigurationtypes.h"

#ifndef CAMERAAPP_PLUGIN_BUILD
#include "CamResourceIDs.h"
#endif //CAMERAAPP_PLUGIN_BUILD

#include <driveinfo.h>

// CONSTANTS
/* estimated average image sizes */
const TInt KCamImageSize3MP = 750000; 
const TInt KCamImageSize2MP = 475000;
const TInt KCamImageSize1_3MP = 325000;
const TInt KCamImageSize1MP = 90000;
const TInt KCamImageSizeVGA = 56500;

const TInt KCamLocationTrailStopped = 0;
const TInt KCamLocationTrailNoValidData = 1;
const TInt KCamLocationTrailSearchingDevice = 2;
const TInt KCamLocationTrailGPSDataAvail = 3;

// A multiple of the image size is kept as a buffer to ensure that a
// complex last image can be captured
const TInt KCamImageDataBufferMultiplier = 1;

// Name of the server that takes foreground when charger is inserted
_LIT( KAknCapServer, "akncapserver" );
// Location data notification windows use this server.
_LIT( KAknNotifyServer, "aknnfysrv"    );

// Phonebook server
_LIT( KPbkServer, "Pbk2ServerApp" );

// DATA TYPES
/**
* Memory card status, as returned by MemoryCardStatus()
*/
enum TCamMemoryCardStatus
    {
    ECamMemoryCardNotInserted,
    ECamMemoryCardLocked,
    ECamMemoryCardInserted
    };

#ifndef CAMERAAPP_PLUGIN_BUILD
#define ROID(resource_id)               (CamUtility::ResourceOrientationID(resource_id))

// Capture tone resources
_LIT( KCamCaptureTone1,      "z:\\system\\sounds\\digital\\cameraappCapture1.wav"  );
_LIT( KCamCaptureTone2,      "z:\\system\\sounds\\digital\\cameraappCapture2.wav"  );
_LIT( KCamCaptureTone3,      "z:\\system\\sounds\\digital\\cameraappCapture3.wav"  );
_LIT( KCamCaptureTone4,      "z:\\system\\sounds\\digital\\cameraappCapture4.wav"  );
_LIT( KCamBurstCaptureTone1,      "z:\\system\\sounds\\digital\\cameraappCaptureShort1.wav"  );
_LIT( KCamBurstCaptureTone2,      "z:\\system\\sounds\\digital\\cameraappCaptureShort2.wav"  );
_LIT( KCamBurstCaptureTone3,      "z:\\system\\sounds\\digital\\cameraappCaptureShort3.wav"  );
_LIT( KCamBurstCaptureTone4,      "z:\\system\\sounds\\digital\\cameraappCaptureShort4.wav"  );
_LIT( KCamVideoStartTone,    "z:\\system\\sounds\\digital\\cameraappStart.wav"     );
_LIT( KCamVideoPauseTone,    "z:\\system\\sounds\\digital\\cameraappStop.wav"      );
_LIT( KCamVideoStopTone,     "z:\\system\\sounds\\digital\\cameraappStop.wav"      );
_LIT( KCamAutoFocusComplete, "z:\\system\\sounds\\digital\\cameraappFocusSucc.wav" );

#endif //CAMERAAPP_PLUGIN_BUILD

// CLASS DECLARATION

/**
* Application wide utility functions
*
*  @since 2.8
*/

class CamUtility
    {
    public : // New functions

        /**
        * Get free space in selected memory
        * @since 2.8
        * @param aMemory memory card or internal memory
        * @param aCriticalLevel The critical threshold on the specified drive 
        * @return free space in selected memory (in bytes)
        */
        static TInt64 MemoryFree( 
                    DriveInfo::TDefaultDrives aMemory,
                    TInt aCriticalLevel = 0 );

        /**
        * Returns the memory card status.
        * @since 2.8
        * @return the status.
        */
        static TCamMemoryCardStatus MemoryCardStatus();

        /**
        * Get drive number for memory card.
        * @since 2.8
        * @return drive number for memory card
        */
        static TInt MemoryCardDrive();

        /**
        * Get drive number for phone memory.
        * @since 2.8
        * @return drive number for phone memory
        */
        static TInt PhoneMemoryDrive();


        /**
        * Get drive number for mass storage memory.
        * @since 3.2
        * @return drive number for mass storage memory
        */
        static TInt MassMemoryDrive();
    
        /**
        * Get drive type for a given drive number.
        * @since 3.2
        * @return DriveInfo::TDefaultDrives type for the drive number
        */
    	static TInt GetDriveTypeFromDriveNumber( TInt aDrive );

        /**
        * Map camera storage location to memory selection dialog storage location
        * @param aStorage The camera storage location to map 
        * @since 3.0
        * @return the memory selection dialog's mapped memory storage location value
        */
        static TInt MapToSettingsListMemory( TInt aStorage );
        
        /**
        * Map memory selection dialog storage location to camera storage location 
        * @param aStorage The settings list memory location to map 
        * @since 3.0
        * @return the camera storage location mapped from the memory selection dialog's location value
        */
        static TInt MapFromSettingsListMemory( TInt aStorage );

        /**
        * Suggest a new unique filename 
        * @since 2.8
        * @param aFilePath (in/out) drive and path for file name 
        *                     (must include trailing '\')
        *                     On return contains the full path and
        *                     generated file name with extension
        * @param aFilename    (in/out) filename (without path or extension)
        * @param aSuggestedNumeral number initially appended to the name to ensure
        *                     uniqueness, the actual number used may be higher
        * @param aExt         file extension (".jpg" or ".3gp")
        */
        static void GetUniqueNameL( TDes& aFilePath, 
                         TDes& aFilename, 
                         TInt aSuggestedNumeral,
                         const TDesC& aExt );

        /**
        * Generate a image file name based on base name and counter.
        * @since 2.8
        * @param aCounter image counter [1...]
        * @param aBaseName name base 
        * @param aFilename descriptor to hold the generated file name.
        *                  Does not include path or extension. Example:
        *                  "Image(001)"
        */
        static void FormatFileName( TUint aCounter,
                             const TDesC& aBaseName,
                             TDes& aFilename );

        /** 
        * Checks if given filename contains illegal characters or
        * is otherwise illegal.
        * @since 2.8
        * @param aFilename descriptor that holds file name to check.
        */
        static TBool CheckFileNameValidityL( const TDesC& aFilename );

        /** 
        * Generate a file name based on date and counter.
        * @since 2.8
        * @param aFilename descriptor to hold the generated file name.
        *                  Does not include path or extension. Example:
        *                  "240305(001)"
        */
        static void FormatDateFileNameL( TDes& aFilename );

        /** 
        * Format current date into given descriptor in a format
        * suitable for file names.        
        * @since 2.8
        * @param aBuffer the buffer to receive the formatted date.
        */
        static void GetCurrentDateForNameBaseL( TDes& aBuffer );


        /** 
        * Return the path and filename of the application MBM file
        * @since 2.8
        * @param aResName Populated with path and filename of MBM file
        */
        static void ResourceFileName( TDes& aResName );

		/**
		* Return Product Specific Information in the form of a TInt
		* @since 2.8
		* @param aKey The enumeration key identifying the data required
		* @param aValue A reference to TInt that will hold the data
		* @return KErrNone if successful, KErrNotSupported if no valid data.
		*/		
		static TInt GetPsiInt( TCamPsiKey aKey, TInt& aValue );

		/**
		* Return Product Specific Information in the form of a TInt array
		* @since 3.0
		* @param aKey The enumeration key identifying the data required
		* @param aValue A reference to a TInt array that will hold the data
		* @return KErrNone if successful, KErrNotSupported if no valid data.
		*/		
		static TInt GetPsiIntArrayL( TCamPsiKey aKey, RArray<TInt>& aValue );
        
        /**
		* Return Product Specific Information in the form of a TInt
		* @since 2.8
		* @param aKey The enumeration key identifying the data required
		* @param aValue A reference to TInt that will hold the data
		* @return KErrNone if successful, KErrNotSupported if no valid data.
		*/		
		static TInt GetCommonPsiInt( TCamPsiKey aKey, TInt& aValue );

		/**
		* Return Product Specific Information in the form of a TAny*
		* @since 2.8
		* @param aKey The enumeration key identifying the data required
		* @param aValue A pointer to where the data should be copied to.
		*        note, data is Mem::Copied, so sufficient space must be
		*        available in the buffer passed in to "aValue"
		* @return KErrNone if successful, KErrNotSupported if no valid data.
		*/
        static TInt GetPsiAnyL( TCamPsiKey aKey, TAny* aValue );

		/**
		* Return maximum size for an MMS message
		* @since 2.8
		* @return The maximum size
		*/
        static TInt MaxMmsSizeInBytesL();

		/**
		* Compares two strings.
		* @since 2.8
		* @param aFirst The first string.
		* @param aSecond The second string
        * @return ETrue if the strings are equivalent.
		*/
        static TBool CompareIgnoreCase( const TDesC& aFirst, 
                                      const TDesC& aSecond );

		/**
		* Deletes a video clip.
		* @since 2.8
		* @param aFilename The filename of the video clip
		*/
        static void DeleteVideoL( const TDesC& aFilename );

		/**
		* Deletes an image.
		* @since 2.8
		* @param aFilename The filename of the image
		*/
        static void DeleteStillImageL( const TDesC& aFilename );

		/**
		* Returns the thumbnail path for an image.
		* @since 2.8
		* @param aImagePath The path of the image
		* @param aThumbnailPath The path of the thumbnail image
		*/
        static void GetThumbnailPath( const TDesC& aImagePath, 
                                            TDes& aThumbnailPath );

		/**
		* Renames a still image.
		* @since 2.8
		* @param aOldPath The old path of the image
		* @param aNewName The new name of the image
		* @param aNewPath Receives the new path of the image.
		*/
        static void RenameStillImageL( const TDesC& aOldPath,
                                              const TDesC& aNewName,
                                              TDes& aNewPath );

		/**
		* Takes a window group ID and a group name.  Checks to see if the
        * window group referred to by the ID has a name containing the 
        * window group name string.
		* @since 3.0
		* @param aWindowId ID of the window group
		* @param aWindowGroupName Name to check for
        * @return ETrue if ID matches name
		*/
        static TBool IdMatchesName( TInt aWindowId, const TDesC& aWindowGroupName );
            
		/**
		* Returns the correct resource ID for the current orientation
		* @since 3.0
		* @param aDefaultResourceID The resource ID for the default orientation
		* @return The new resource ID
		*/
		
        static TInt ResourceOrientationID( TInt aDefaultResourceID );
        
        /**
        * Map light sensitivity id to ISO rating value.
        * 
        */
        static TInt MapLightSensitivity2IsoValueL( TCamLightSensitivityId aLightSensitivityId, RArray<TInt>& aSupportedValues );

        /**
        * Map UI sharpness setting id to Camera sharpness setting value.
        *
        */
        static TInt MapSharpnessId2SharpnessValueL( TCamImageSharpnessId aSharpnessId );

        /**
        * Obtain resource id for specific video reoslution 
        * (for primary camera)
        * @since 3.0
        * @param aResolution video reoslution
        * @return resource id
        */
        static TInt MapVideoQualityToViewFinderRes( TCamVideoResolution aResolution );
                                                   
		/**
		* Determines if the battery power is good
		* @since 3.0
		* @return ETrue if the battery is OK, and EFalse if it is 
		*         low or almost empty
		*/
		static TBool IsBatteryPowerOK();

		/**
		* Determines if the battery power is empty
		* @since 3.0
		* @return ETrue if the battery is Empty 
		*/
		static TBool IsBatteryPowerEmpty();
		        
		/**
		* Determines if the battery is being charged
		* @since 3.0
		* @return ETrue if the battery is charging
		*/
		static TBool IsBatteryCharging();

        /**
        * Determines if LocationTrail is started
        * @return ETrue if LocationTrail is started
        */
        static TBool IsLocationAware();

        /**
        * Replaces original bitmap by bitmap with alpha channel.
        * @since S60 5.0
        * @param aBmp, bitmap 
        * @param aMask, mask 
        * @param aColor, Color used instead of white.  
        */                                    
        static void SetAlphaL( CFbsBitmap*& aBmp, CFbsBitmap*& aMask, 
                               TRgb aColor = KRgbWhite );
        
    /**
    * Determines if USB is active
    * @return ETrue if USB is active
    */
    static TBool IsUsbActive();
    
    /**
    * Determines if the device has a NHD display.
    * This method is meant to be used with the layouts only
    * @return ETrue if NHD (640 x 360) display
    */
    static TBool IsNhdDevice();

    /**
    * Determines device qwerty-mode state.
    * This method is meant to be used with secondary camera.
    * @return ETrue if qwerty keyboard is open
    */
    static TBool IsQwertyOpen(); 

    /**
    * Get layout rectangle for given resolution in given mode.
    * @param aMode ECamControllerVideo or ECamControllerImage.
    * @param aResolution Resolution enumeration from 
    * CCamAppController::GetCurrentVideoResolution or
    * CCamAppController::GetCurrentImageResolution depending 
    * on the mode.
    *                    
    */
    static TRect ViewfinderLayout( TCamCameraMode aMode,
                                   TInt           aResolution );

  private:
                                   
    /**
    * Helper method for ViewfinderLayout.
    */
    static TAknWindowLineLayout ViewfinderLayoutTouch( TCamCameraMode aMode,
                                                       TInt           aResolution );

    /**
    * Helper method for ViewfinderLayout.
    */
    static TAknWindowLineLayout ViewfinderLayoutNonTouch( TCamCameraMode aMode,
                                                          TInt           aResolution );


    };

#endif      // CAMUTILITY_H 
            
// End of File
