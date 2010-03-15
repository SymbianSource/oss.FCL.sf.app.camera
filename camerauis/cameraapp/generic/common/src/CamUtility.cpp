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



// INCLUDE FILES
#include <bldvariant.hrh> // for feature definitions

#include <aknappui.h>                  // CEikonEnv::Static()->FsSession()
#include <AknSkinsInternalCRKeys.h>
#include <aknnotewrappers.h>

#include <AknCommonDialogsDynMem.h>
#include <driveinfo.h>

#include <sysutil.h>                   // SysUtil::DiskSpaceBelowCriticalLevelL()
#include <pathinfo.h>
#include <eikapp.h>                    // CEikApplication
#include <collate.h>                   // TCollateMethod
#include <e32property.h>
#include <centralrepository.h>
#include <MmsEngineInternalCRKeys.h>   // MMS cenrep keys
#include <hwrmpowerstatesdkpskeys.h>
#include <coreapplicationuisdomainpskeys.h> // USB connection 
#include <locationtrailpskeys.h>
#include <AvkonInternalCRKeys.h>   // KAknQwertyInputModeActive

#include <layoutmetadata.cdl.h>            
#include <aknlayoutscalable_apps.cdl.h>    

#if defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)
  #include <harvesterclient.h>
#endif // defined(RD_MDS_2_5) && !defined(__WINSCW__) && !defined(__WINS__)


#include <glxcollectionmanager.h>

#ifndef CAMERAAPP_PLUGIN_BUILD
    #include <cameraapp.rsg>
    #include <vgacamsettings.rsg>  
#else
    #include <gscamerapluginrsc.rsg>
#endif //CAMERAAPP_PLUGIN_BUILD

#include "CamUtility.h"
#include "CamPanic.h"
#include "CameraappPrivateCRKeys.h"
#include "camuidconstants.h"


// CONSTANTS
const TInt KCamNhdWidth  = 640;
const TInt KCamNhdHeight = 360;   

#ifndef CAMERAAPP_PLUGIN_BUILD

#include "CamAppUi.h"
#include "CameraUiConfigManager.h"
#include "CamResourceLookup.h"

// ===========================================================================
// CONSTANTS
const TInt KMicSecInHalfSec  = 500000;
const TInt KMicSecInQuartSec = 250000;

const TInt KCaptureLongPress = 500; // value in milliseconds

// UI EV range
const TInt KEVCompMinVal       = -2;
const TInt KEVCompMaxVal       =  2;
const TInt KEVCompStepsPerUnit =  2;

// Limit for burst captured images 
const TInt KMaxBurstImages     = 18;

// Timeout value for courtesyUI
const TInt KCourtesyUiTimeoutValue  = 5;

// Self-timer constants
const TInt KMicroSecPerSec = 1000000;

#endif //CAMERAAPP_PLUGIN_BUILD


// ============================ MEMBER FUNCTIONS ===============================


// ---------------------------------------------------------------------------
// CamUtility::MemoryFree
// Get free space in selected memory
//
// Returns: free space in selected memory (in bytes) minus the disk critical space
// ---------------------------------------------------------------------------
//

TInt64 CamUtility::MemoryFree( 
    //AknCommonDialogsDynMem::TMemoryTypes aMemory,
    DriveInfo::TDefaultDrives aMemory,
    TInt aCriticalLevel )
    {   
    PRINT( _L("Camera => CamUtility::MemoryFree") ); 
    TVolumeInfo vinfo;
    TInt error = KErrNone;

    // if measuring space on memory card
    if ( aMemory ==  DriveInfo::EDefaultRemovableMassStorage )//AknCommonDialogsDynMem::EMemoryTypeMMC )
        {
        error = CEikonEnv::Static()->FsSession().Volume(
            vinfo, MemoryCardDrive() );
        if ( error || MemoryCardStatus() != ECamMemoryCardInserted )
            {
        PRINT( _L("Camera <= CamUtility::MemoryFree, failed to get volume info 1!!") ); 
            return 0;
            }            
        }
    else if ( aMemory == DriveInfo::EDefaultMassStorage )//AknCommonDialogsDynMem::EMemoryTypeInternalMassStorage )
    	{
        error = CEikonEnv::Static()->FsSession().Volume( 
            vinfo, MassMemoryDrive() );
    	}
    else // otherwise, measuring space on the phone memory
        {
        error = CEikonEnv::Static()->FsSession().Volume( 
            vinfo, PhoneMemoryDrive() );
        }

    if ( error ) // if getting the volume info failed
        {
        PRINT( _L("Camera <= CamUtility::MemoryFree, failed to get volume info 2!!") ); 
        return 0; // indicate not enough space
        }
    PRINT( _L("Camera <= CamUtility::MemoryFree") ); 
    return vinfo.iFree - aCriticalLevel; 
    }

// ---------------------------------------------------------------------------
// CamUtility::MemoryCardStatus
// Returns the current status of the MMC
// ---------------------------------------------------------------------------
//
TCamMemoryCardStatus CamUtility::MemoryCardStatus()
    {
    PRINT( _L("Camera => CamUtility::MemoryCardStatus") );    
    TDriveInfo driveInfo;

    CEikonEnv::Static()->FsSession().Drive( driveInfo, MemoryCardDrive() );

    // Check if the MMC is currently locked.
    // If mmc is locked, then driveInfo.iType is zero, therefore switch clause 
    // below does not notice new card
    // Check iMediaAtt instead, if media can be locked and is currently locked 
    // and has password, then locked MMC is inserted

    TCamMemoryCardStatus mmcStatus( ECamMemoryCardNotInserted );

    if ( ( driveInfo.iMediaAtt & ( KMediaAttLockable|KMediaAttLocked|KMediaAttHasPassword ) ) 
           == ( KMediaAttLockable|KMediaAttLocked|KMediaAttHasPassword ) )
        {
        mmcStatus = ECamMemoryCardLocked;
        }
    else
        {
        switch( driveInfo.iType )
            {
            case EMediaNotPresent:
                {
                mmcStatus = ECamMemoryCardNotInserted;
                break;
                }
            default:
                {
                mmcStatus = ECamMemoryCardInserted;
                break;
                }
            }
        }
    // Check that volume is valid (ie. MMC is not corrupted)
    TVolumeInfo volumeInfo;
    TInt error = CEikonEnv::Static()->FsSession().Volume( volumeInfo, MemoryCardDrive() );
    if ( error )
        {
        mmcStatus = ECamMemoryCardNotInserted;
        }        
    PRINT( _L("Camera <= CamUtility::MemoryCardStatus") );    
    return mmcStatus;
    }


// ---------------------------------------------------------------------------
// CamUtility::MemoryCardDrive
// Get drive number for memory card.
// ---------------------------------------------------------------------------
//
TInt CamUtility::MemoryCardDrive()
    {
    // Get the root path of the default memory card.
    TInt drive;
    TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, drive );
    if ( err != KErrNone )
    	{
    	return err;
    	}
    TFileName path;
    err = PathInfo::GetRootPath( path, drive );
    if ( err != KErrNone )
    	{
    	return err;
    	}
    TInt res = 0;
    err = RFs::CharToDrive( path[0], res );
    __ASSERT_ALWAYS( !err, CamPanic( ECamPanicFileSystemError ) );
    // return the memory card drive number
    return res;
    }

// ---------------------------------------------------------------------------
// CamUtility::PhoneMemoryDrive
// Get drive number for phone memory.
// ---------------------------------------------------------------------------
//
TInt CamUtility::PhoneMemoryDrive()
    {
    // Get the root path of the default phone memory.
    TInt drive;
    TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive );
    if ( err != KErrNone )
    	{
    	return err;
    	}
    TFileName path;
    err = PathInfo::GetRootPath( path, drive );
    if ( err != KErrNone )
    	{
    	return err;
    	}
    TInt res = 0;
    err = RFs::CharToDrive( path[0], res );
    __ASSERT_ALWAYS( !err, CamPanic( ECamPanicFileSystemError ) );
    // return the phone memory drive number
    return res;
    }

// ---------------------------------------------------------------------------
// CamUtility::MassMemoryDrive
// Get drive number for mass storage memory.
// ---------------------------------------------------------------------------
//
TInt CamUtility::MassMemoryDrive()
	{
    // Get the root path of the default mass storage memory.
    TInt drive;
    TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive );
    if ( err != KErrNone )
    	{
    	return err;
    	}
    TFileName path;
    err = PathInfo::GetRootPath( path, drive );
    if ( err != KErrNone )
    	{
    	return err;
    	}
    TInt res = 0;
    err = RFs::CharToDrive( path[0], res );
    __ASSERT_ALWAYS( !err, CamPanic( ECamPanicFileSystemError ) );
    // return the mass storage memory drive number
    return res;
	}


TInt CamUtility::GetDriveTypeFromDriveNumber( TInt aDrive )
	{
	TInt drive;
	TInt returnVal = DriveInfo::EDefaultPhoneMemory;
	DriveInfo::GetDefaultDrive( DriveInfo::EDefaultPhoneMemory, drive );
	if ( drive == aDrive )
		{
		returnVal = DriveInfo::EDefaultPhoneMemory;
		}
	DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive );
	if ( drive == aDrive )
		{
		returnVal = DriveInfo::EDefaultMassStorage;
		}
	DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, drive );
	if ( drive == aDrive )
		{
		returnVal = DriveInfo::EDefaultRemovableMassStorage;
		}
    return returnVal;
	}

// ---------------------------------------------------------------------------
// CamUtility::MapToSettingsListMemory
// Return settings list mapped value
// ---------------------------------------------------------------------------
//   

TInt CamUtility::MapToSettingsListMemory( TInt aStorage )
    {
    TInt mappedMemory;
    switch ( aStorage )
        {
        case ECamMediaStoragePhone:
            {
            mappedMemory = DriveInfo::EDefaultPhoneMemory;
            }
            break;
        case ECamMediaStorageCard:
            {
            mappedMemory = DriveInfo::EDefaultRemovableMassStorage;
            }
            break;
        case ECamMediaStorageMassStorage:
            {
            mappedMemory = DriveInfo::EDefaultMassStorage;
            }
            break;
        default:
            {
            mappedMemory = KErrNotFound;
            }
            break;
        }
    return mappedMemory;        
    }
    
// ---------------------------------------------------------------------------
// CamUtility::MapFromSettingsListMemory
// Return camera storage location mapped from settings list value
// ---------------------------------------------------------------------------
// 
TInt CamUtility::MapFromSettingsListMemory( TInt aStorage )
    {
    TInt mappedMemory;
    switch ( aStorage )
        {
        case DriveInfo::EDefaultPhoneMemory:
            {
            mappedMemory = ECamMediaStoragePhone;
            }
            break;
        case DriveInfo::EDefaultRemovableMassStorage:
            {
            mappedMemory = ECamMediaStorageCard;
            }
            break;
        case DriveInfo::EDefaultMassStorage:
            {
            mappedMemory = ECamMediaStorageMassStorage;
            }
            break;
        default:
            {
            mappedMemory = KErrNotFound;
            }
            break;
        }
    return mappedMemory;        
    }    

// ---------------------------------------------------------------------------
// CamUtility::GetUniqueNameL
// Suggest a new unique filename using the name stub provided in aFilename
// and adding a numbered postfix to ensure uniqueness
// The full filename is returned in aFilePath. The name of the file only, is
// returned in aFilename
// ---------------------------------------------------------------------------
//
void CamUtility::GetUniqueNameL( TDes& aFilePath, 
                                 TDes& aFilename, 
                                 TInt aSuggestedNumeral,
                                 const TDesC& aExt )
    {
    PRINT( _L("Camera => CamUtility::GetUniqueNameL") );    
    // Leave if there is a problem with the passed directory path.
    TEntry entry;
    RFs rfs;
    User::LeaveIfError( rfs.Connect( ) );
    CleanupClosePushL( rfs );
    User::LeaveIfError( rfs.Entry( aFilePath, entry ) );
	// check it actually is a directory
    if( !entry.IsDir() )
        {
        PRINT( _L("Camera => CamUtility::GetUniqueNameL not dir") );
        User::Leave( KErrCorrupt );
        }   

    TInt namelen = aFilename.Length();

    TFileName nameBase( aFilename.Left( namelen ) );

    TBool done = EFalse;
    TInt counter = aSuggestedNumeral;

    RFs& fs = CEikonEnv::Static()->FsSession();

    while ( !done )
        {
        // Generate new name
        FormatFileName( counter, nameBase, aFilename );

        TFileName newPath( aFilePath );
        newPath.Append( aFilename );
        newPath.Append( aExt );

        // define new instance of TEntry to prevent LINT error
        TEntry newEntry;
        TInt ferr = fs.Entry( newPath, newEntry );        
        // if there is no file in the current location with this name
        if ( ferr == KErrNotFound )
            {
            done = ETrue;
            aFilePath.Copy( newPath );
            }
        else if( ferr == KErrNone )// the filename has already been used in this location
            {
            // try the next number
            ++counter;
            }
        else
            {
            User::Leave( ferr );
            }            
        }
    CleanupStack::PopAndDestroy( &rfs );
    PRINT( _L("Camera => CamUtility::GetUniqueNameL") );    
    }

// ---------------------------------------------------------------------------
// CamUtility::FormatFileName
// Generate a file name based on base name and counter.
// ---------------------------------------------------------------------------
//
void CamUtility::FormatFileName( TUint aCounter,
                                 const TDesC& aBaseName,
                                 TDes& aFilename )
    {
    const TUint KMax3Digits = 999;
    _LIT( KCamNameFormatFirst, "%S" );      	// "Photo"
    _LIT( KCamNameFormat3Digits, "%S%03d" );    // "Photo001"
    _LIT( KCamNameFormat4Digits, "%S%d" );    // "Photo1234"
   
    // if the number is 0
    if ( aCounter == 0 )
        {
        aFilename.Format( KCamNameFormatFirst, &aBaseName );
        }
    else if ( aCounter <= KMax3Digits ) // the number is 1 - 999
        {
        aFilename.Format( KCamNameFormat3Digits, &aBaseName, aCounter );
        } 
    else // the number is > 999
        {
        aFilename.Format( KCamNameFormat4Digits, &aBaseName, aCounter );
        }
    }

// ---------------------------------------------------------------------------
// CamUtility::CheckFileNameValidityL
// Checks if given filename contains illegal characters or
// is otherwise illegal.
// ---------------------------------------------------------------------------
//
TBool CamUtility::CheckFileNameValidityL( const TDesC& aFilename ) 
    {
    // For filename validity checking
    _LIT( KSingleDot, "." );
    _LIT( KDoubleDot, ".." );
    _LIT( KBackslash, "\\" );

    // "." and ".." are not valid filenames
    if ( aFilename.Compare( KSingleDot ) == 0 ||
         aFilename.Compare( KDoubleDot ) == 0 )
        {
        CAknNoteWrapper* note = new ( ELeave ) CAknNoteWrapper();
        note->ExecuteLD( R_CAM_UNSUITABLE_FILENAME );
        return EFalse;
        }

    // No backslashes in filenames.
    // File server checks for other special characters.
    if ( aFilename.Find( KBackslash ) != KErrNotFound ||
         !CCoeEnv::Static()->FsSession().IsValidName( aFilename ) )
        {
        CAknNoteWrapper* note = new ( ELeave ) CAknNoteWrapper();
        note->ExecuteLD( R_CAM_ILLEGAL_CHARACTERS );
        return EFalse;
        }

    // File name is okay.
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CamUtility::FormatDateFileNameL
// Generate a file name based on date and counter.
// ---------------------------------------------------------------------------
//
void CamUtility::FormatDateFileNameL( TDes& aFilename )
    {
    const TInt KCamMaxDateLen = 8;

    _LIT( KCamNameFormatFirst, "%S" );      // "Image"

    TBuf<KCamMaxDateLen> date;
    GetCurrentDateForNameBaseL( date );

    aFilename.Format( KCamNameFormatFirst, &date );
    }

// ---------------------------------------------------------------------------
// CamUtility::GetCurrentDateForNameBaseL
// Format current date into given descriptor in a format
// suitable for file names.
// ---------------------------------------------------------------------------
//
void CamUtility::GetCurrentDateForNameBaseL( TDes& aBuffer )
    {
    const TInt KCamDateFormatLength = 8;
    _LIT( KCamEuropeanDateFormat, "%F%D%M%Y" );
    _LIT( KCamAmericanDateFormat, "%F%M%D%Y" );
    _LIT( KCamJapaneseDateFormat, "%F%Y%M%D" );

    TTime now;
    now.HomeTime();
    TLocale locale;
    TBuf<KCamDateFormatLength> dateFormat;

    switch ( locale.DateFormat() )
        {
        case EDateAmerican:
            {
            dateFormat = KCamAmericanDateFormat;
            break;
            }

        case EDateJapanese:
            {
            dateFormat = KCamJapaneseDateFormat;
            break;
            }

        case EDateEuropean:
        default:
            {
            dateFormat = KCamEuropeanDateFormat;
            break;
            }
        }

    now.FormatL( aBuffer, dateFormat );
    }

// ---------------------------------------------------------------------------
// CamUtility::ResourceFileName
// Return the name and path of the application MBM file.
// ---------------------------------------------------------------------------
//
void CamUtility::ResourceFileName( TDes& aResName )
    {
    _LIT(KResName, "z:\\resource\\apps\\cameraapp.mbm");

    aResName = KResName();
    }


// ---------------------------------------------------------------------------
// CamUtility::GetMaxMmsSizeL
// Get maximum MMS message size
// ---------------------------------------------------------------------------
//
TInt CamUtility::MaxMmsSizeInBytesL() 
    {    
	const TUint32 KCCorMaxMms          = 0x00000006;
    
    TInt maxSize = 0;
      
    CRepository* mmsRepository = CRepository::NewLC( KCRUidMmsEngine );
    User::LeaveIfError( mmsRepository->Get( 
        KMmsEngineMaximumSendSize, maxSize ) );

    if ( !maxSize )
        {
        CRepository *repository = CRepository::NewLC( KCRUidCamcorderFeatures );
        
        // Maximum MMS size not defined in Camcorder local variation
        // ini file, try the MMS ini file        
       	User::LeaveIfError( repository->Get(
                                KCCorMaxMms, 
                                maxSize ) ); 
                                
        CleanupStack::PopAndDestroy( repository );                        
        }
       
    const TInt KKiloByte = 1024;
    // How many bytes to allocate for other things than 
    // video in a MMS message
    const TInt KMmsSizeReserve = 5 * KKiloByte; 
    
    // Convert to kilobytes, leave some space for other things
    maxSize = Max( maxSize - KMmsSizeReserve, 0 );
    
    

    //delete mmsRepository;
    CleanupStack::PopAndDestroy( mmsRepository ); 
    return maxSize;
    }

// ---------------------------------------------------------------------------
// CcorUtility::CompareIgnoreCase
// Compare two descriptors
// ---------------------------------------------------------------------------
//
TBool CamUtility::CompareIgnoreCase( const TDesC& aFirst, 
                                      const TDesC& aSecond )
    {
    // Get the standard method
    TCollationMethod m = *Mem::CollationMethodByIndex( 0 );
    m.iFlags |= TCollationMethod::EFoldCase;
    m.iFlags |= TCollationMethod::EIgnoreNone;

    // Magic: Collation level 3 is used
    return aFirst.CompareC( aSecond, 3, &m ) == 0;
    }

// ---------------------------------------------------------------------------
// CamUtility::DeleteVideoL
// Delete a video clip.
// ---------------------------------------------------------------------------
//
void CamUtility::DeleteVideoL( const TDesC& aFilename )
    {
    TInt err = CEikonEnv::Static()->FsSession().Delete( aFilename );
    if ( err && err != KErrNotFound )
        {
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// CamUtility::DeleteStillImageL
// Delete a still image.
// ---------------------------------------------------------------------------
//
void CamUtility::DeleteStillImageL( const TDesC& aFilename )
    {
    TInt err = CEikonEnv::Static()->FsSession().Delete( aFilename );
    if ( err && err != KErrNotFound )
        {
        User::Leave( err );
        }

    TFileName thumbnailPath;
    GetThumbnailPath( aFilename, thumbnailPath );

    // ignore error
    CEikonEnv::Static()->FsSession().Delete( thumbnailPath );
    }

// ---------------------------------------------------------------------------
// CamUtility::GetThumbnailPath
// Get full path to a thumbnail file corresponding to given image file.
// ---------------------------------------------------------------------------
void CamUtility::GetThumbnailPath( const TDesC& aImagePath, 
                                    TDes& aThumbnailPath )
    {
    TParsePtrC parse( aImagePath );
    aThumbnailPath = parse.DriveAndPath();
    aThumbnailPath.Append( PathInfo::ImagesThumbnailPath() );
    aThumbnailPath.Append( parse.NameAndExt() );
    }

// ---------------------------------------------------------------------------
// CamUtility::RenameStillImageL
// Rename a still image file.
// ---------------------------------------------------------------------------
//
void CamUtility::RenameStillImageL( const TDesC& aOldPath,
                                      const TDesC& aNewName,
                                      TDes& aNewPath )
    {
    CFileMan* fileMan = CFileMan::NewL( CEikonEnv::Static()->FsSession() );
    CleanupStack::PushL( fileMan );

    TParsePtrC parse( aOldPath );
    TFileName fullNewPath;
    fullNewPath = parse.DriveAndPath();
    fullNewPath.Append( aNewName );
    fullNewPath.Append( parse.Ext() );

    // Rename image file
    User::LeaveIfError( fileMan->Rename( aOldPath, fullNewPath, 0 ) );
    // NOTE: flags = 0 --> attempting overwrite will leave

    aNewPath = fullNewPath;

    // Rename thumbnail
    TFileName oldThumbPath;
    TFileName newThumbPath;
    GetThumbnailPath( aOldPath, oldThumbPath );
    GetThumbnailPath( aNewPath, newThumbPath );

    // ignore error
    fileMan->Rename( oldThumbPath, newThumbPath, CFileMan::EOverWrite );

    CleanupStack::PopAndDestroy(); // fileMan
    }


// -----------------------------------------------------------------------------
// CamUtility::IdMatchesName
// Checks whether the window ID corresponds to the ground name supplied.
// -----------------------------------------------------------------------------
//
TBool CamUtility::IdMatchesName( TInt aWindowId, const TDesC& aWindowGroupName )
    {   
    PRINT2(_L("Camera => CamUtility::IdMatchesName ( %d, %S )"), aWindowId, &aWindowGroupName );

	const TInt KMaxWGName = 256;
	
    TBuf <KMaxWGName> name;

    // Look up the window group name from the supplied ID.
    CEikonEnv::Static()->WsSession().GetWindowGroupNameFromIdentifier( aWindowId, name );

    // We have to do some manipulation, as the window group name string
    // contains NULL seperators which will stop a normal descriptor Find 
    // operation from finding the string needed.    
    for ( TInt i=0; i<name.Length(); i++)   // Replace any "null" chars with space
        {
        if ( name[i] == NULL )
            {
            name[i] = ' ';
            }
        }
        
    PRINT1(_L("Camera    CamUtility::IdMatchesName tidied name '%S'"), &name);
                
    TInt ret = name.Find( aWindowGroupName );
    
    PRINT2(_L("Camera    CamUtility::IdMatchesName ret=%d, return(%d)"), ret, ret != KErrNotFound);
    return ( ret != KErrNotFound );
    }


// -----------------------------------------------------------------------------
// CamUtility::IsBatteryPowerOK
// Returns ETrue if the battery power is good
// -----------------------------------------------------------------------------
//
TBool CamUtility::IsBatteryPowerOK()
	{
    PRINT(_L("Camera => CamUtility::IsBatteryPowerOK"));
	TBool isBatteryOK =  EFalse;    // Default to battery low

	TInt strength = EBatteryStatusUnknown;
	TInt result = RProperty::Get( KPSUidHWRMPowerState, KHWRMBatteryStatus, strength );
    PRINT2(_L("Camera    CamUtility::IsBatteryPowerOK result=%d, strength=%d"), result, strength);
	if ( result == KErrNone && strength == EBatteryStatusOk )
		{
		isBatteryOK = ETrue;
		}

    PRINT1(_L("Camera <= CamUtility::IsBatteryPowerOK return(%d)"), isBatteryOK);
	return isBatteryOK;
	}


// -----------------------------------------------------------------------------
// CamUtility::IsBatteryPowerEmpty
// Returns ETrue if the battery power is empty
// -----------------------------------------------------------------------------
//
TBool CamUtility::IsBatteryPowerEmpty()
    {
    PRINT(_L("Camera => CamUtility::IsBatteryPowerEmpty"));
    TBool isBatteryEmpty =  EFalse;    

    TInt strength = EBatteryStatusUnknown;
    TInt result = RProperty::Get( KPSUidHWRMPowerState, KHWRMBatteryStatus, strength );
    PRINT2(_L("Camera    CamUtility::IsBatteryPowerEmpty result=%d, strength=%d"), result, strength);
    if ( result == KErrNone && strength == EBatteryStatusEmpty )
        {
        isBatteryEmpty = ETrue;
        }

    PRINT1(_L("Camera <= CamUtility::IsBatteryPowerEmpty return(%d)"), isBatteryEmpty);
    return isBatteryEmpty;
    }


// -----------------------------------------------------------------------------
// CamUtility::IsBatteryCharging
// Returns ETrue if the battery is being charged
// -----------------------------------------------------------------------------
//
TBool CamUtility::IsBatteryCharging()
	{
    PRINT(_L("Camera => CamUtility::IsBatteryCharging"));
	TBool isCharging = EFalse;      // Default to not charging

	TInt charge = EChargingStatusError;
	TInt result = RProperty::Get( KPSUidHWRMPowerState, KHWRMChargingStatus, charge );
    PRINT2(_L("Camera    CamUtility::IsBatteryCharging result=%d, charge=%d"), result, charge);
	if ( (result == KErrNone) && (charge == EChargingStatusCharging || 
								  charge == EChargingStatusAlmostComplete || 
								  charge == EChargingStatusChargingContinued || 
								  charge == EChargingStatusChargingComplete) )
		{
		isCharging = ETrue;
		}
    PRINT1(_L("Camera <= CamUtility::IsBatteryCharging return(%d)"), isCharging);

	return isCharging;
	}


// -----------------------------------------------------------------------------
// CamUtility::IsLocationAware()
// Returns ETrue if LocationTrail is started and valid GPS data is available
// -----------------------------------------------------------------------------
//
TBool CamUtility::IsLocationAware()
    {
#ifndef __WINSCW__
    TBool ret = EFalse;
    TInt keyVal;
    TInt err = RProperty::Get( KPSUidLocationTrail, KLocationTrailState, keyVal );
    if( KErrNone == err )
        {
        if( KCamLocationTrailGPSDataAvail == keyVal )
            {
            ret = ETrue;
            }    
        }
    return ret;
#else
    return ETrue;
#endif // !__WINSCW__
    }

// ---------------------------------------------------------
// CamUtility::SetAlphaL  
// ---------------------------------------------------------
//
void CamUtility::SetAlphaL( CFbsBitmap*& aBmp, CFbsBitmap*& aMask, 
                            TRgb aColor ) 
    {
    // Draw EColor16MA bitmap: 
    CFbsBitmap* bmpWithAlpha = new (ELeave) CFbsBitmap();
	User::LeaveIfError( 
	                 bmpWithAlpha->Create( aBmp->SizeInPixels(), EColor16MA) );  
	CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( bmpWithAlpha );
    CleanupStack::PushL( device );
    CFbsBitGc* gc;
    User::LeaveIfError( device->CreateContext(gc) );
    CleanupStack::PushL(gc);
	
    gc->BitBlt( TPoint(0,0), aBmp );
    delete aBmp; 
    aBmp = NULL;
    aBmp = bmpWithAlpha;
    
    CleanupStack::PopAndDestroy(gc);
    CleanupStack::PopAndDestroy(device);

    // Ensure mask colour depth
    TSize size = aMask->SizeInPixels();
    TBool inv( EFalse );
    if ( aMask->DisplayMode() != EGray256 ) 
        {
        CFbsBitmap* bmp256 = new (ELeave) CFbsBitmap();
        User::LeaveIfError( 
	                 bmp256->Create( size, EGray256) ); 
	    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( bmp256 );
        CleanupStack::PushL( device );
        CFbsBitGc* gc;
        User::LeaveIfError( device->CreateContext(gc) );
        CleanupStack::PushL(gc); 
        gc->BitBlt( TPoint(0,0), aMask );
        delete aMask; 
        aMask = NULL;
        aMask = bmp256;  
        CleanupStack::PopAndDestroy(gc);
        CleanupStack::PopAndDestroy(device); 
        inv = ETrue;          
        }
           
    // Add Alpha values:    
    TUint32 dPitch = CFbsBitmap::ScanLineLength( size.iWidth, EColor16MA ) / 4;
    TUint32 maskPitch = CFbsBitmap::ScanLineLength( size.iWidth, EGray256 );                     
    aMask->LockHeap();
    aBmp->LockHeap();

    // Component is not expected to deal with the extended bitmap. 
    TUint32* destAddr = reinterpret_cast<TUint32*>( aBmp->DataAddress() );
        
    for ( TUint32 y = 0; y < size.iHeight; y++ )
        {
        HBufC8* maskBuf = HBufC8::NewLC( maskPitch );
        TPtr8 maskPtr = maskBuf->Des();
        aMask->GetScanLine( maskPtr, TPoint(0, y), size.iWidth, EGray256 );
        for ( TUint32 x = 0; x < size.iWidth; x++ )
            {
            TUint8 value = maskPtr[x];
            if ( inv )
                {
                value = ~value;
                }
            // Round mask value to fully opaque or transparent
            if ( value > 0x7f ) 
                {
                // Opaque pixel.
                destAddr[ y * dPitch + x ] &= aColor.Internal();  
                destAddr[ y * dPitch + x ] |= 0xFF000000;
                }
            else
                {
                // Transparent pixel
                destAddr[ y * dPitch + x ] = 0x00000000;
                }
            }
        CleanupStack::PopAndDestroy(maskBuf);
        }
        
    aBmp->UnlockHeap();
    aMask->UnlockHeap();
    
    // Mask is not used for drawing
    delete aMask;
    aMask = NULL;
    }

// ---------------------------------------------------------
// CamUtility::IsUsbActive
// ---------------------------------------------------------
//        
TBool CamUtility::IsUsbActive()
    {
    TInt usbState;
    TInt err = RProperty::Get( KPSUidCoreApplicationUIs,
                               KCoreAppUIsUSBFileTransfer,
                               usbState );                                  
    return !err && usbState == ECoreAppUIsUSBFileTransferActive;
    }
    
// ---------------------------------------------------------
// CamUtility::IsNhdDevice for Layouts only
// ---------------------------------------------------------
//        
TBool CamUtility::IsNhdDevice() 
    {
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );   
    TRect nhdRectLandscape( TSize( KCamNhdWidth, KCamNhdHeight ) );
    TRect nhdRectPortrait( TSize( KCamNhdHeight, KCamNhdWidth ) ); 
 
    return screenRect == nhdRectLandscape || screenRect == nhdRectPortrait;
    }

// ---------------------------------------------------------------------------
// CamUtility::GetPsiInt
// Allows retrieval of product specific information (TInt)
// ---------------------------------------------------------------------------
//
#ifndef CAMERAAPP_PLUGIN_BUILD

TInt CamUtility::GetPsiInt( TCamPsiKey aKey, TInt& aValue )
  {
  aValue = KErrNotSupported;

  switch ( aKey )
    {
    case ECamPsiOrientation:
    case ECamPsiPrimaryCameraOrientation:
    case ECamPsiSecondaryCameraOrientation:
    case ECamPsiEditingOrientation:
         return CamUtility::GetCommonPsiInt( aKey, aValue );
    case ECamPsiOverLaySidePane:  { aValue = ETrue;         break; }
    case ECamPsiSidePanePosition: { aValue = ECamHandRight; break; }
    case ECamPsiSoftKeyPosition:  { aValue = ECamHandRight; break; } 

    case ECamPsiLeftNaviKey:     { aValue = ECamNaviKeyCaptureSetup; break; }
    case ECamPsiLeftNaviKeyCam2: { aValue = ECamNaviKeyNone;         break; }
    case ECamPsiRightNaviKey:    { aValue = ECamNaviKeyNone;         break; }
    case ECamPsiVideoZoomSupport:    { aValue = ECamZoomWhenRecord; break; }


     // Interval: return the value in units of 1000 microseconds
    case ECamPsiLongCaptureKeyPressInterval: { aValue = KCaptureLongPress;            break; }
    case ECamPsiMaxBurstCapture:             { aValue = KMaxBurstImages;              break; }
    case ECamPsiCourtesyUiTimeoutValue:      { aValue = KCourtesyUiTimeoutValue;      break; } 
    case ECamPsiAPEVCompLegend:                { aValue = R_CAM_CAPTURE_SETUP_MENU_PHOTO_EXPOSURE_ARRAY2; break; }
    default:
      {
      return KErrNotSupported;
      }
    }
  return KErrNone;
  }

// ---------------------------------------------------------------------------
// CamUtility::GetPsiIntArrayL
// Allows retrieval of product specific information (TInt array)
// ---------------------------------------------------------------------------
//
TInt CamUtility::GetPsiIntArrayL( TCamPsiKey aKey, RArray<TInt>& aArray )
  {
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  CCameraUiConfigManager* configManager = appUi->AppController().UiConfigManagerPtr();

  switch ( aKey )
    {     
    // Resource ids for User Scene setup items  
    case ECamPsiUserSceneResourceIds:
      {
      User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_SCENEBASE_DATA    ) );
      if ( configManager->IsFlashModeSupported() )
          User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_FLASH_DATA ) );
      if ( configManager->IsWhiteBalanceFeatureSupported() )
          User::LeaveIfError( aArray.Append( 
                                    R_CAM_PHOTO_USER_SCENE_SETUP_LIST_WHITEBALANCE_DATA ) );
      if ( configManager->IsEVSupported() )
          User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_EV_DATA2 ) );

      if ( configManager->IsColorToneFeatureSupported() )
          User::LeaveIfError( aArray.Append( 
                                    R_CAM_PHOTO_USER_SCENE_SETUP_LIST_COLOUREFFECT_DATA ) );
      if ( configManager->IsISOSupported() )
        {
        if ( configManager->IsExtendedLightSensitivitySupported() ) 
            {
            User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_EXTENDED_LIGHTSENSITIVITY_DATA ) );
            } 
       else {
            User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_LIGHTSENSITIVITY_DATA ) );
            }
        }
      
      if ( configManager->IsSharpnessFeatureSupported() )
          User::LeaveIfError( aArray.Append( 
                                    R_CAM_PHOTO_USER_SCENE_SETUP_LIST_IMAGESHARPNESS_DATA ) );
      
      if ( configManager->IsContrastSupported() )
          User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_CONTRAST_DATA ) );
      
      User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_SET_USER_DEFAULT ) ); 
            
      User::LeaveIfError( aArray.Append( R_CAM_PHOTO_USER_SCENE_SETUP_LIST_RESET_DATA ) );
      break;
      }
    // -----------------------------------------------------
    default:
      {
      return KErrNotSupported;
      }
    }
  return KErrNone;
  }

// ---------------------------------------------------------------------------
// CamUtility::GetPsiAnyL
// Allows retrieval of product specific information (TAny)
// ---------------------------------------------------------------------------
//
TInt CamUtility::GetPsiAnyL( TCamPsiKey aKey, TAny* aValue )
  {
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  CCameraUiConfigManager* uiConfigManager = appUi->AppController().UiConfigManagerPtr();

  RArray<TInt> zoomValues;
  CleanupClosePushL( zoomValues );
  zoomValues.Reset();
  
  typedef TPckgBuf<TCamMaxZoomSteps>          TCamZoomStepPckg;
  typedef TPckgBuf<TSelfTimerSetting>         TCamTimerSetPckg;
  typedef TPckgBuf<TCamZoomLAF>               TCamZoomLAFPckg;
  typedef TPckgBuf<TCamEvCompRange>           TCamEVCompPckg;

  switch ( aKey )
    {
    // -----------------------------------------------------
    // Self timer periods
    case ECamPsiSelfTimerPeriod1:
      {
      // Default constructor assigns KErrNotSupported to all fields.
      // See CamPSI.h.
      TSelfTimerSetting timerPeriod1; 

      // First period is at 2-seconds remaining, "always on"
      timerPeriod1.iThreshold  [0] = 2*KMicroSecPerSec;
      timerPeriod1.iFlashPeriod[0] = KMaxTInt;
      timerPeriod1.iBeepPeriod [0] = KMaxTInt;

      // There are no other periods

      // Autofocus triggered on "2-seconds remaining"
      timerPeriod1.iAutoFocus = 2*KMicroSecPerSec;

      TCamTimerSetPckg* ptr = static_cast<TCamTimerSetPckg*>( aValue );
      *ptr = timerPeriod1;
      break;
      }
    case ECamPsiSelfTimerPeriod2:
      {
      TSelfTimerSetting timerPeriod2;
       
      // First period is at 10-seconds until capture, flashing once every second.
      timerPeriod2.iThreshold  [0] = 10*KMicroSecPerSec;
      timerPeriod2.iFlashPeriod[0] = KMicSecInHalfSec;   // 500ms on/500ms off
      timerPeriod2.iBeepPeriod [0] = KMicroSecPerSec;       // Beep every second

      // Second period starts at 3-seconds until capture, flashing twice every second
      timerPeriod2.iThreshold  [1] = 5*KMicroSecPerSec;
      timerPeriod2.iFlashPeriod[1] = KMicSecInQuartSec;  // 250ms on/250ms off
      timerPeriod2.iBeepPeriod [1] = KMicSecInHalfSec;   // Beep twice every second

      // Third period is not supported
      timerPeriod2.iThreshold  [2] = 3*KMicroSecPerSec;
      timerPeriod2.iFlashPeriod[2] = KMaxTInt;
      timerPeriod2.iBeepPeriod [2] = KMaxTInt;

      // Autofocus triggered on "3-seconds remaining"
      timerPeriod2.iAutoFocus = 3*KMicroSecPerSec;          

      TCamTimerSetPckg* ptr = static_cast<TCamTimerSetPckg*>( aValue );
      *ptr = timerPeriod2;
      break;
      }
    case ECamPsiSelfTimerPeriod3:
      {
      TSelfTimerSetting timerPeriod3;

      // First period is at 20-seconds until capture, flashing once every second.
      timerPeriod3.iThreshold  [0] = 20*KMicroSecPerSec;
      timerPeriod3.iFlashPeriod[0] = KMicSecInHalfSec;   // 500ms on/500ms off
      timerPeriod3.iBeepPeriod [0] = KMicroSecPerSec;       // Beep every second

      // Second period starts at 3-seconds until capture, flashing twice every second
      timerPeriod3.iThreshold  [1] = 5*KMicroSecPerSec;
      timerPeriod3.iFlashPeriod[1] = KMicSecInQuartSec;  // 250ms on/250ms off
      timerPeriod3.iBeepPeriod [1] = KMicSecInHalfSec;   // Beep twice every second

      // Third period is not supported
      timerPeriod3.iThreshold  [2] = 3*KMicroSecPerSec;
      timerPeriod3.iFlashPeriod[2] = KMaxTInt;
      timerPeriod3.iBeepPeriod [2] = KMaxTInt;

      // Autofocus triggered on "3-seconds remaining"
      timerPeriod3.iAutoFocus = 3*KMicroSecPerSec;

      TCamTimerSetPckg* ptr = static_cast<TCamTimerSetPckg*>( aValue );
      *ptr = timerPeriod3;
      break;
      }
    // -----------------------------------------------------
    // Zoom steps.
    case ECamPsiMaxOpticalZoomSteps:
      {
      // Default constructor defined to initialize
      // all fields to KErrNotSupported. See CamPSI.h.
      TCamMaxZoomSteps optSteps;

      // No optical zoom steps supported => nothing more to set.

      TCamZoomStepPckg* ptr = static_cast<TCamZoomStepPckg*>( aValue );
      *ptr = optSteps;
      break;
      }
    case ECamPsiMaxZoomSteps:
      {
      TCamMaxZoomSteps digSteps;
      
      // the order the values read are exactly in the order defined in
      // TCamMaxZoomSteps
      
      uiConfigManager->SupportedMaxZoomStepsL( zoomValues );
      
      // all the values if they are supported or not will be run-time
      TInt count = zoomValues.Count()-1; 
      TInt i = 0;
      if ( zoomValues.Count() )   
          {
          digSteps.iMaxPhotoStepW9MP        = zoomValues[Min(count,i++)]; 
          digSteps.iMaxPhotoStepW6MP        = zoomValues[Min(count,i++)]; 
          digSteps.iMaxPhotoStep12MP        = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStep8MP         = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStep5MP         = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStep3MP         = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStep2MP         = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStep1_3MP       = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStep1MP         = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStep0_8MP       = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStepSVGA        = zoomValues[Min(count,i++)];
          digSteps.iMaxPhotoStepVGA         = zoomValues[Min(count,i++)];

          
          digSteps.iMaxVideoStepHD          = zoomValues[Min(count,i++)];
          digSteps.iMaxVideoStepWVGA        = zoomValues[Min(count,i++)];
          digSteps.iMaxVideoStepNHD         = zoomValues[Min(count,i++)];
          digSteps.iMaxVideoStepVGA         = zoomValues[Min(count,i++)];
          digSteps.iMaxVideoStepQVGA        = zoomValues[Min(count,i++)];
          digSteps.iMaxVideoStepCIF         = zoomValues[Min(count,i++)];
          digSteps.iMaxVideoStepQCIF        = zoomValues[Min(count,i++)];
          digSteps.iMaxVideoStepSQCIF       = zoomValues[Min(count,i++)];
          digSteps.iMax2ndCamPhotoStep      = zoomValues[Min(count,i++)];
          digSteps.iMax2ndCamVideoStepCIF   = zoomValues[Min(count,i++)];
          digSteps.iMax2ndCamVideoStepQCIF  = zoomValues[Min(count,i++)];
          digSteps.iMax2ndCamVideoStepSQCIF = zoomValues[Min(count,i++)];
          }
      else
          {
          User::Leave( KErrNotSupported );
          }
      TCamZoomStepPckg* ptr = static_cast<TCamZoomStepPckg*>( aValue );
      *ptr = digSteps;
      break;
      }
    case ECamPsiMaxExtendedZoomSteps:
      {
      TCamMaxZoomSteps extSteps;

      // the order the values read are exactly in the order defined in
      // TCamMaxZoomSteps

      uiConfigManager->SupportedMaxExtendedZoomStepsL( zoomValues );
      
      // TODO: some kind of check to see if there are all possible values in
      //       the array that we ask from config manager.
      TInt count = zoomValues.Count()-1; 
      TInt i = 0;
      if ( zoomValues.Count() > 0 )
          {
          extSteps.iMaxPhotoStepW9MP   = zoomValues[Min(count,i++)]; 
          extSteps.iMaxPhotoStepW6MP   = zoomValues[Min(count,i++)]; 
          extSteps.iMaxPhotoStep12MP   = zoomValues[Min(count,i++)]; //KMaxPhoto12MPExtZoomStep;  
          extSteps.iMaxPhotoStep8MP    = zoomValues[Min(count,i++)]; //KMaxPhoto8MPExtZoomStep;  
          extSteps.iMaxPhotoStep5MP    = zoomValues[Min(count,i++)]; //KMaxPhoto5MPExtZoomStep;
          extSteps.iMaxPhotoStep3MP    = zoomValues[Min(count,i++)]; //KMaxPhoto3MPExtZoomStep;
          extSteps.iMaxPhotoStep2MP    = zoomValues[Min(count,i++)]; //KMaxPhoto2MPExtZoomStep;
          extSteps.iMaxPhotoStep1_3MP  = zoomValues[Min(count,i++)]; //KMaxPhoto1_3MPExtZoomStep;
          extSteps.iMaxPhotoStep1MP    = zoomValues[Min(count,i++)]; //KMaxPhoto1MPExtZoomStep;
          extSteps.iMaxPhotoStep0_8MP  = zoomValues[Min(count,i++)]; //KMaxPhoto0_8MPExtZoomStep;
          extSteps.iMaxPhotoStepSVGA   = zoomValues[Min(count,i++)];          
          extSteps.iMaxPhotoStepVGA    = zoomValues[Min(count,i++)]; //KMaxPhotoVGAExtZoomStep;
          }
      else
          {
          User::Leave( KErrNotSupported );
          }
      TCamZoomStepPckg* ptr = static_cast<TCamZoomStepPckg*>( aValue );
      *ptr = extSteps;
      break;
      }
    // -----------------------------------------------------
    // Zoom appearance
    case ECamPsiZoomBarLAF:
      {
      const TRgb KZoomColourDig = TRgb( 255, 140, 1 );
      const TRgb KZoomColourExt = TRgb( 255, 0, 0 );

      TCamZoomLAF zoomLAF;
      uiConfigManager->SupportedLAFZoomBarL( zoomValues );
      
      if ( zoomValues.Count() > 0 )
          {
          // Time per zoom tick in milliseconds
          zoomLAF.iZoomSpeedDig  = zoomValues[0]; //KZoomSpeedDig;
          // No of zoom steps per tick
          zoomLAF.iZoomStepsDig  = zoomValues[2]; //KZoomStepsDig;

          // Time per ext zoom tick in milliseconds
          zoomLAF.iZoomSpeedExt  = zoomValues[1]; //KZoomSpeedExt;
          // No of ext zoom steps per tick  
          zoomLAF.iZoomStepsExt  = zoomValues[3]; //KZoomStepsExt;
          
          zoomLAF.iSecondCameraZoomSteps = zoomValues[4]; //K2ndCamZoomStepSize
          }
      else
          {
          User::Leave( KErrNotSupported );
          }
      
      // Colour of optical zoom in zoom bar
      zoomLAF.iZoomColourDig = KZoomColourDig;
      // Colour of optical zoom in zoom bar
      zoomLAF.iZoomColourExt = KZoomColourExt;                

      TCamZoomLAFPckg* ptr = static_cast<TCamZoomLAFPckg*>( aValue );
      *ptr = zoomLAF;
      break;
      }
    // -----------------------------------------------------
    // Exposure compensation values
    case ECamPsiEVCompSliderInfo:
      {
      TCamEvCompRange evComp;

      evComp.iMinValue     = KEVCompMinVal;
      evComp.iMaxValue     = KEVCompMaxVal;
      evComp.iStepsPerUnit = KEVCompStepsPerUnit;

      TCamEVCompPckg* ptr = static_cast<TCamEVCompPckg*>( aValue );
      *ptr = evComp;
      break;
      }
    // -----------------------------------------------------
    default:
      {
      return KErrNotSupported;        
      }
    // -----------------------------------------------------
    }
  CleanupStack::PopAndDestroy( &zoomValues );
  return KErrNone;
  }

    
// ---------------------------------------------------------------------------
// CamUtility::ResourceOrientationID
// Returns the resource for the given ID, based on the current Orientation
// ---------------------------------------------------------------------------
//
TInt 
CamUtility::ResourceOrientationID( TInt aDefaultResourceID )
  {
  TCamOrientation orientation;
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

  // Make sure the resource ID is in a valid range
  TBool inValidRange = Rng( 0, aDefaultResourceID, R_CAM_LAST_RESOURCE_ID );
  __ASSERT_DEBUG( inValidRange, CamPanic(ECamPanicInvalidResource) );

  // This should never happen, but we may be being passed the original resource number
  if( !inValidRange ) { return aDefaultResourceID; }

  // Get the current orientation
  // If we don't have the AppUI, 
  // assume the app is starting up and use the default orientation
  if( appUi ) { orientation = appUi->CamOrientation(); }
  else        { orientation = ECamOrientationDefault;  }

  __ASSERT_DEBUG( (ECamOrientationTotal > orientation), CamPanic(ECamPanicBadIndex) );
      
  return KRoidLookup[aDefaultResourceID][orientation];
  }


// ---------------------------------------------------------------------------
// MapLightSensitivity2IsoValueL
// Maps each general light sensitivity id (TCamLightSensitivityId)
// to a product specific ISO rating.
// ---------------------------------------------------------------------------
//
TInt
CamUtility::MapLightSensitivity2IsoValueL( TCamLightSensitivityId aLightSensitivityId, RArray<TInt>& aSupportedValues )
  {
    CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    CCameraUiConfigManager* UiConfigManager = appUi->AppController().UiConfigManagerPtr();
    
    aSupportedValues.Sort();
    TInt count = aSupportedValues.Count();
    if ( count == 0 )
        {
        return ECamIsoRateAuto;
        }
    if ( UiConfigManager->IsExtendedLightSensitivitySupported() )
        {
        // Make sure there is enough supported ISO values for extended light sensitivity
        // first value is 0 which means automatic, there should be 5 or more other values
        TBool enoughSupported = (count > 5);
        if ( !enoughSupported ) {
            CamPanic(ECamPanicNotEnoughISOValuesSupported);
        }
        switch ( aLightSensitivityId )
            {
            // Supports: 100, 200, 400, 800, 1600, 3200.
            case ECamLightSensitivityLow:       return aSupportedValues[1];
            case ECamLightSensitivityLowMed:    return aSupportedValues[2];
            case ECamLightSensitivityMed:       return aSupportedValues[3];
            case ECamLightSensitivityMedHigh:   return aSupportedValues[4];
            case ECamLightSensitivityHigh:      return aSupportedValues[5];
            default:                            return ECamIsoRateAuto;
            }
        }
    else 
        {
        switch ( aLightSensitivityId )
            {
            case ECamLightSensitivityLow:       return aSupportedValues[0];
            case ECamLightSensitivityMed:       return aSupportedValues[TInt(count/2)];
            case ECamLightSensitivityHigh:      return aSupportedValues[count-1];
            default:                            return ECamIsoRateAuto;
            }
        }
  }


// ---------------------------------------------------------------------------
// MapSharpnessId2SharpnessValueL
// Maps each general sharpness value (TCamImageSharpnessId)
// to a product specific value.
// ---------------------------------------------------------------------------
//
TInt 
CamUtility::MapSharpnessId2SharpnessValueL( TCamImageSharpnessId aSharpnessId )
  {
  TInt sharpnessValue = 0;
  RArray<TInt> supportedValues;
  CleanupClosePushL( supportedValues );
  
  CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
  if ( appUi && appUi->AppController().UiConfigManagerPtr() )
      {
      appUi->AppController().UiConfigManagerPtr()->SupportedSharpnessSettingsL( 
                                                               supportedValues );
      }
  
  if ( supportedValues.Count() > 0 )
      {
      switch( aSharpnessId )
        {
        case ECamImageSharpnessHard:    
            sharpnessValue = supportedValues[supportedValues.Count()-1];
            break;
        case ECamImageSharpnessSoft:
            sharpnessValue =  supportedValues[0];
            break;
        case ECamImageSharpnessNorm:    // <<fall through>>
        default:
            sharpnessValue = supportedValues[1]; // normal value = 0
            break;
      }
     }
  else
      {
      User::Leave( KErrNotSupported );
      }
   CleanupStack::PopAndDestroy( &supportedValues );

   return sharpnessValue;
  }


// ---------------------------------------------------------------------------    
// CamUtility::MapVideoQualityToViewFinderRes
// Return resource id for viewfinder resolution for primary camera
// ---------------------------------------------------------------------------
//    
TInt 
CamUtility::MapVideoQualityToViewFinderRes( TCamVideoResolution aResolution )
  {
  PRINT1( _L("Camera => CamUtility::MapVideoQualityToViewFinderRes res (%d)"), aResolution )
  switch( aResolution )
    {
    case ECamVideoResolutionVGA: return R_CAM_VIEWFINDER_RECT_QVGA_CAMCORDER;
    case ECamVideoResolutionQVGA: return R_CAM_VIEWFINDER_RECT_QVGA_CAMCORDER;
    case ECamVideoResolutionQCIF: return R_CAM_VIEWFINDER_RECT_CIF_CAMCORDER_ADJUSTED;
    default:                      return R_CAM_VIEWFINDER_RECT_QVGA_CAMCORDER;
    }
  }

#endif // CAMERAAPP_PLUGIN_BUILD
// ---------------------------------------------------------------------------
// CamUtility::GetCommonPsiInt
// Allows retrieval of product specific information (TInt)
// ---------------------------------------------------------------------------
//
TInt CamUtility::GetCommonPsiInt( TCamPsiKey aKey, TInt& aValue )
  {
  aValue = KErrNotSupported;

  switch ( aKey )
    {
    case ECamPsiOrientation:                { aValue = CAknAppUiBase::EAppUiOrientationLandscape; break; }
    case ECamPsiPrimaryCameraOrientation:   { aValue = CAknAppUiBase::EAppUiOrientationLandscape; break; }
    case ECamPsiSecondaryCameraOrientation: { aValue = CAknAppUiBase::EAppUiOrientationPortrait;  break; }
    case ECamPsiEditingOrientation:         { aValue = CAknAppUiBase::EAppUiOrientationPortrait;  break; }               

    default:
      {
      return KErrNotSupported;
      }
    }
  return KErrNone;
  }


// -----------------------------------------------------------------------------
// CamUtility::IsQwertyOpen
// -----------------------------------------------------------------------------
//
TBool CamUtility::IsQwertyOpen()
	{
    TBool retvalue = EFalse;    
    TInt status = 0;
    TInt err = RProperty::Get( KCRUidAvkon, KAknQwertyInputModeActive, status );
    PRINT2(_L("Camera <> CamUtility::IsQwertyOpen err%d status%d"),err, status);
	if ( !err && status )
        {
		retvalue = ETrue;
		}    
    return retvalue;
	}


// -----------------------------------------------------------------------------
// CamUtility::ViewfinderLayout
//
// Get resolution from CCamAppController::GetCurrentVideoResolution()
// or CCamAppController::GetCurrentImageResolution().
//
// -----------------------------------------------------------------------------
//
TRect
CamUtility::ViewfinderLayout( TCamCameraMode aMode,
                              TInt           aResolution )
  {
  PRINT1( _L("Camera => CamUtility::ViewfinderLayout - resolution[%d]"), aResolution );

  TAknWindowLineLayout l = IsNhdDevice() 
                         ? ViewfinderLayoutTouch(aMode, aResolution)
                         : ViewfinderLayoutNonTouch(aMode, aResolution);  

  TRect screenRect;
  if ( Layout_Meta_Data::IsLandscapeOrientation() || IsNhdDevice() )
      {
      AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
      }
  else
      {
      AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
      }

  TAknLayoutRect vfLayoutRect;
  vfLayoutRect.LayoutRect( screenRect, l );
  TRect vfRect( vfLayoutRect.Rect() );

  PRINT4( _L("Camera <= CamUtility::ViewfinderLayout - rect[x:%d y:%d w:%d h:%d]"),
          vfRect.iTl.iX, vfRect.iTl.iY, vfRect.Width(), vfRect.Height() );

  return vfRect;
  }


// -----------------------------------------------------------------------------
// CamUtility::ViewfinderLayoutTouch
//
// -----------------------------------------------------------------------------
//
TAknWindowLineLayout 
CamUtility::ViewfinderLayoutTouch( TCamCameraMode aMode,
                                   TInt           aResolution )
  { 
  TInt variant = Layout_Meta_Data::IsLandscapeOrientation();
  
  // Default to 4:3 full screen
  TAknWindowLineLayout l = 
      AknLayoutScalable_Apps::cam4_image_uncrop_pane( variant );

  // -------------------------------------------------------      
  // Video mode
  if ( ECamControllerVideo == aMode )
    {
    switch( aResolution )
      {
      case ECamVideoResolutionQCIF:
      case ECamVideoResolutionCIF:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutTouch - QCIF/CIF video resolution") );
        l = AknLayoutScalable_Apps::video4_image_uncrop_cif_pane( variant );
        break;
        }
      case ECamVideoResolutionNHD:
      case ECamVideoResolutionHD:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutTouch - nHD/HD video resolution") );
        l = AknLayoutScalable_Apps::video4_image_uncrop_nhd_pane( variant );
        break;
        }
      default:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutTouch - 4:3 video resolution") );
        // All other video resolutions are expected to
        // have 4:3 aspect ratio
        l = AknLayoutScalable_Apps::video4_image_uncrop_vga_pane( variant );
        break;
        }
      }
    }
  // -------------------------------------------------------      
  // Image mode
  else if ( ECamControllerImage == aMode )
    {
    switch( aResolution )
      {
      case ECamPhotoSizeW9MP:
      case ECamPhotoSizeW6MP:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutTouch - 16:9 still image resolution") );
        l = AknLayoutScalable_Apps::video4_image_uncrop_nhd_pane( variant );
        break;
        }
      default:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutTouch - 4:3 still image resolution") );
        l = AknLayoutScalable_Apps::cam4_image_uncrop_pane( variant );
        break;
        }
      }
    }
  // -------------------------------------------------------      

  return l;
  }



// ---------------------------------------------------------------------------
// CamUtility::ViewfinderLayoutNonTouch
// ---------------------------------------------------------------------------
//
TAknWindowLineLayout 
CamUtility::ViewfinderLayoutNonTouch( TCamCameraMode aMode,
                                      TInt           aResolution )
  {
  TInt variant = Layout_Meta_Data::IsLandscapeOrientation() ? 0 : 3;
  
  TAknWindowLineLayout l =
      AknLayoutScalable_Apps::cam6_image_uncrop_pane( variant );

  // -------------------------------------------------------      
  // Video mode
  if ( ECamControllerVideo == aMode )
    {
    switch( aResolution )
      {
      case ECamVideoResolutionQCIF:
      case ECamVideoResolutionCIF:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutNonTouch - QCIF/CIF video resolution") );
        l = AknLayoutScalable_Apps::vid6_image_uncrop_cif_pane( variant );
        break;
        }
      case ECamVideoResolutionNHD:
      case ECamVideoResolutionHD:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutNonTouch - nHD/HD video resolution") );
        l = AknLayoutScalable_Apps::vid6_image_uncrop_nhd_pane( variant );
        break;
        }
      case ECamVideoResolutionWVGA:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutNonTouch - WVGA video resolution") );
        l = AknLayoutScalable_Apps::vid6_image_uncrop_wvga_pane( variant );
        break;
        }
      default:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutNonTouch - 4:3 video resolution") );
        // All other video resolutions are expected to
        // have 4:3 aspect ratio
        l = AknLayoutScalable_Apps::vid6_image_uncrop_vga_pane( variant );
        break;
        }
      }
    }
  // -------------------------------------------------------      
  // Image mode
  else
    {
    switch( aResolution )
      {
      case ECamPhotoSizeW9MP:
      case ECamPhotoSizeW6MP:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutNonTouch - 16:9 still image resolution") );
        l = AknLayoutScalable_Apps::vid6_image_uncrop_nhd_pane( variant );
        }
      default:
        {
        PRINT( _L("Camera <> CamUtility::ViewfinderLayoutNonTouch - 4:3 still image resolution") );
        l = AknLayoutScalable_Apps::cam6_image_uncrop_pane( variant );
        break;
        }
      }
    }
  // -------------------------------------------------------      

  return l;
  }


//  End of File  

