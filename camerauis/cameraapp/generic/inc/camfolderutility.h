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
*
*
*/




#ifndef CAM_FOLDERUTILITY_H
#define CAM_FOLDERUTILITY_H

// ===========================================================================
// Included headers
#include <e32base.h>

#include "CamSettingsInternal.hrh"
#include "CamSettings.hrh"

// ===========================================================================

/**
* Utility class for folder naming
*/
class CCamFolderUtility : public CBase
  {
  // =======================================================
  // Types and constants
  public:

    static const TInt KMaxGuaranteedBurstGroup   = 20;
    static const TInt KMaxFilesPerFolder         = 100;
    
    enum TCamFolderType
        {
        ECamFolderTypeImageMMC,
        ECamFolderTypeImagePhone,
        ECamFolderTypeVideoMMC,
        ECamFolderTypeVideoPhone,
        ECamFolderTypeLast
        };

  // =======================================================
  public:

    /**
    * Get the base path to be used for greating the next filename
    * @since 2.8
    * @param aMonthCounters Array with counter for every storage type folders.
    *        Must have ECamFolderTypeLast number of items (or more).
    * @param aStorage Whether storing to Phone or MMC
    * @param aPath On return will contain the new path, if the final destination
    * (month counter) folder does not exist then this will contain the parent
    * folder (month folder)
    * @param aCaptureMode image or video mode (used to determine folder path/name)
    * @param aCreateAll whether or not the destination folder should be created
    * if it does not already exist. The folders at higher levels than this are 
    * always ensured to exist (unless the function returns an error, when existence is 
    * undetermined). If aCreateAll is ETrue the remaining space (max 100 files) in the
    * destination folder is checked, 
    * @param aRequiredFileCount Folders will contain a maximum of 100 files. If there is 
    * not enough space for the specified count then a new folder will be created.
    * @param aTime provides the month and year used to determine the folder name (ensures
    * that all images in a sequence are placed in the same month parent folder). If aTime 
    * is 0 GetBasePathL uses the current system time.
    */
    static void GetBasePathL( TInt*          aMonthCounters,
                              TInt           aStorage, // TCamMediaStorage
                              TDes&          aPath, 
                              TCamCameraMode aCaptureMode, 
                              TBool          aCreateAll,
                              TInt           aRequiredFileCount = 1,
                              TTime          aTime              = TInt64( 0 ) );

    /**
    * Reset certain month folder counters.
    * @param The array containing month folder counters for every storage type.
    *        Must have ECamFolderTypeLast number of items (or more).
    * @param aFrom The index of first counter which is resetted.
    * @param aTo   The index of last counter which is resetted.
    */
    static void ResetCounters( TInt* aMonthCounters,
                               TInt  aFrom = ECamFolderTypeImageMMC,
                               TInt  aTo   = ECamFolderTypeLast - 1 );

  private:
    
    /**
    * Set up the counters used to create mulitple month folders
    * @since 3.0
    * @param aMonthCounters Array with counter for every storage type folders.
    *        Must have ECamFolderTypeLast number of items (or more).
    * @param aPath the current path.
    * @param aMonthFolder the name of the month folder parent directory
    * @param aFs file server session
    * @param aFolderType Index into the array of folder counters (image/video, mmc/phone)                
    * @return KErrNone or another system wide error code
    */                        
    static TInt InitialiseMonthCounter( TInt*  aMonthCounters,
                                        TDes&  aPath,
                                        TDesC& aMonthFolder,
                                        RFs&   aFs,
                                        TInt   aFolderType );   
                                            
    /**
    * Create the destination folder for the next image or video. 
    * @since 3.0
    * @param aMonthCounters Array with counter for every storage type folders.
    *        Must have ECamFolderTypeLast number of items (or more).
    * @param aPath If returning without error this will contain the new path.
    * @param aFs file server session
    * @param aFolderType Index into the array of folder counters (image/video, mmc/phone)
    * @param aRequiredFileCount Folders will contain a maximum of 100 files. If there is 
    * not enough space for the specified count then a new folder will be created. When the
    * highest possible folder number is reached (Z9) the 100 file limit is ignored
    * @return KErrNone or another system wide error code
    */                           
    static TInt CreateDestinationFolder( TInt* aMonthCounters,
                                         TDes& aPath, 
                                         RFs&  aFs,
                                         TInt  aFolderType, // TCamMediaStorage
                                         TInt  aRequiredFileCount );

    
  // =======================================================
  };


#endif // CAM_FOLDERUTILITY_H

// ===========================================================================
// end of file
