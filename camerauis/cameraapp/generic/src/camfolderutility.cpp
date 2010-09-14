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
* Description:  
*
*/


// ===========================================================================
// Included headers


#include <bldvariant.hrh> // for feature definitions

#include <f32file.h>
#include <centralrepository.h>
#include <pathinfo.h>
#include <StringLoader.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamPanic.h"
#include "CamUtility.h"       // PRINT macros
#include "CameraappPrivateCRKeys.h"  
#include "camfolderutility.h"


// ===========================================================================
// Local constants

static const TInt KMultipleFolderNumberChars = 2;
static const TInt KMaxMonthFolders           = 260;
static const TInt KBase10                    = 10;
static const TInt KCamMonthFolderNameLength = 8;


_LIT( KCamMonthFolderFormat, "%F%Y%M" );  
_LIT( KBackslash,            "\\"     );
_LIT( KCharacterOffset,      "A"      );
_LIT( KDigitOffset,          "0"      );
_LIT( KMaxCharacter,         "Z"      );
_LIT( KWildCardCharacter,    "?"      );



// ===========================================================================
// CCamFolderUtility implementation

// ---------------------------------------------------------------------------
// GetBasePathL
// Generates the path where new images/videos are to be saved to.
// ---------------------------------------------------------------------------
//
void
CCamFolderUtility::GetBasePathL( TInt*                 aMonthCounters,
                                 TInt                  aStorage, // TCamMediaStorage
                                 TDes&                 aPath, 
                                 TCamCameraMode aCaptureMode, 
                                 TBool                 aCreateAll,
                                 TInt                  aRequiredFileCount,
                                 TTime                 aTime )
  {
  PRINT( _L("Camera => CCamFolderUtility::GetBasePathL"))

  TInt folderTypeIndex = 0;
  
    if ( ( aStorage == ECamMediaStoragePhone ) ) // Saving to phone memory
       { 
       // phone folders are in the odd indexes
       folderTypeIndex ++;
       PRINT( _L("Camera GetBasePathL saving to phone memory"))
       aPath.Copy( PathInfo::PhoneMemoryRootPath() );
       }
       // with multiple drives, mass storage is the default, like phone memory used to be.
    else if ( aStorage == ECamMediaStorageMassStorage ) // Saving to mass storage memory
       {
       PRINT( _L("Camera GetBasePathL saving to mass storage memory"))
       // Get the root path of the mass storage drive.
       TInt drive;
       TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive );
       TFileName path;
       err = PathInfo::GetRootPath( path, drive );
       aPath.Copy(path);
       }
    else  // Saving to MMC
       {
       PRINT( _L("Camera GetBasePathL saving to memory card"))
       // Get the root path of the mmc.
       TInt drive;
       TInt err = DriveInfo::GetDefaultDrive( DriveInfo::EDefaultRemovableMassStorage, drive );
       TFileName path;
       err = PathInfo::GetRootPath( path, drive );
       aPath.Copy(path);
       }

     // Append the folder to the path
  if ( ECamControllerVideo == aCaptureMode )
      {
      aPath.Append( PathInfo::VideosPath() );
      // video folders are offset to ECamFolderTypeVideoMMC
      folderTypeIndex += ECamFolderTypeVideoMMC;
      }
  else 
      {
      aPath.Append( PathInfo::ImagesPath() );  
      }             
    // Copied from below
    TBuf<KMaxNameBaseLength> cameraFolder; // "Camera"
    StringLoader::Load( cameraFolder, R_CAM_CAMERA_SUBFOLDER );
    
    RFs rfs;
    User::LeaveIfError( rfs.Connect() );
    CleanupClosePushL( rfs );     
 
    TFileName fn( aPath );
    fn.Append( cameraFolder );
    TEntry entry;
    TInt err2 = rfs.Entry(fn, entry );

    if( KErrNone == err2 )
      {
       if (!entry.IsDir( ))
          {
           RBuf newName;
           CleanupClosePushL( newName);  
           newName.CreateL( fn.Length() + 4 );
           newName.Copy( fn );
           newName.Append(_L(".bak"));
           TInt error = rfs.Rename( fn, newName );
           if ( error != KErrNone )
               {
                User::LeaveIfError( rfs.Delete( fn ) );
               }
           CleanupStack::PopAndDestroy( &newName );
          }
       }
  // Add a folder for the current month
  // Use the specified time to determine the year and month.
  // If this is 0 then use the current time.
  TTime now = aTime;
  if ( now.Int64() == TInt64( 0 ) )
      {
      now.HomeTime();
      }
  TBuf<KCamMonthFolderNameLength> monthFolder;
  now.FormatL( monthFolder, KCamMonthFolderFormat );
  // If the month folder name is different to the last used month folder name
  // this indicates that a new month has been started. All the counters will
  // need to be reinitialised
  
  // Get last used folder name
  TBuf<KCamMonthFolderNameLength> previousMonthFolder;
  CRepository* cr = CRepository::NewLC( KCRUidCameraappSettings );
  cr->Get( KCamCrLastUsedMonthFolder, previousMonthFolder );
  
  // Compare to current folder name, if different then reset all counters
  if ( monthFolder.Compare( previousMonthFolder) != 0 )
      {
      ResetCounters( aMonthCounters );
      // Write month folder name to shared data 
      cr->Set( KCamCrLastUsedMonthFolder, monthFolder );
      }  
  CleanupStack::PopAndDestroy( cr );    
//  TBuf<KMaxNameBaseLength> cameraFolder;
//  StringLoader::Load( cameraFolder, R_CAM_CAMERA_SUBFOLDER );
  aPath.Append( cameraFolder );   
  aPath.Append( KBackslash );          
  aPath.Append( monthFolder );  
  aPath.Append( KBackslash ); 
  
  // Keep track of the month folder (YYYYMM) name length
  // This may be returned e.g.if the month counter destination folder  (YYYYMMXX) is
  // not created
  TInt monthFolderLength = aPath.Length();

  // ensure the path exists
  TInt err = rfs.MkDirAll( aPath );
 
  // If the folder is newly created then set the counter to 0  
  if      ( KErrNone          == err ) aMonthCounters[folderTypeIndex] = 0;
  else if ( KErrAlreadyExists == err ) err = KErrNone;
  else                                 User::Leave( err );

          
  // If the month counter is uninitialised it needs to be set up
  if ( aMonthCounters[folderTypeIndex] < 0 )
      {
      User::LeaveIfError( InitialiseMonthCounter( aMonthCounters,
                                                  aPath,
                                                  monthFolder,
                                                  rfs,
                                                  folderTypeIndex ) );
      }
  aPath.Append( monthFolder ); 
  // Only ensure the folder exists (and has space) if the aCreateAll flag is set
  if ( aCreateAll )
      {
      PRINT( _L("Camera GetBasePathL creating month counter folder") )
      // This adds on the correct counter if completing without error
      User::LeaveIfError( CreateDestinationFolder( aMonthCounters,
                                                   aPath, 
                                                   rfs, 
                                                   folderTypeIndex, 
                                                   aRequiredFileCount ) );
      }
  else
      {
      TInt monthCounter = aMonthCounters[folderTypeIndex];
      aPath.Append( KCharacterOffset()[0] + monthCounter/KBase10 );
      aPath.Append( KDigitOffset()[0] + monthCounter%KBase10 );
      aPath.Append( KBackslash );
      // If the folder does not exist then remove the final folder name from the path
      //TEntry entry;
      
      if ( rfs.Entry( aPath, entry ) == KErrNotFound )
          {
          aPath.SetLength( monthFolderLength );
          }
      }        
  CleanupStack::PopAndDestroy( &rfs );

  PRINT( _L("Camera <= CCamAppController::GetBasePathL returning") )
  }


// ---------------------------------------------------------------------------
// ResetCounters
// ---------------------------------------------------------------------------
//
void 
CCamFolderUtility::ResetCounters( TInt* aMonthCounters,
                                  TInt  aFrom,
                                  TInt  aTo )
  {
  PRINT( _L("Camera => CCamFolderUtility::ResetCounters") )
    {
    for( TInt i = aFrom; i <= aTo; i++ )
      {
      aMonthCounters[i] = -1;
      }
    }
  }



// ---------------------------------------------------------------------------
// InitialiseMonthCounter
// Sets the value of the folder counter for the current month/media store/mode
// ---------------------------------------------------------------------------
//  
TInt 
CCamFolderUtility::InitialiseMonthCounter( TInt*  aMonthCounters,
                                           TDes&  aPath,
                                           TDesC& aMonthFolder,
                                           RFs&   aFs,
                                           TInt   aFolderType )
  {
  PRINT( _L("Camera => CCamFolderUtility::InitialiseMonthCounter") )

  // start by initialising the appropriate folder counter to 0
  aMonthCounters[aFolderType] = 0; 
      
  TInt monthFolderLength = aPath.Length();
  // The month counter folder starts with the same characters as the parent month folder
  aPath.Append( aMonthFolder );  
  TInt charCount;
  // Add on '??' wildcard characters to get a list of all folders with this 
  // month's format
  for ( charCount = 0; charCount < KMultipleFolderNumberChars; charCount++ )
      {
      aPath.Append( KWildCardCharacter );
      }
  // Get a list of folders for this month, sorted in descending alphabetical order
  // the first entry should be the latest used folder
  CDir* dirList;
  TInt err = KErrNone;
  err =  aFs.GetDir( aPath,
                     KEntryAttMatchExclusive|KEntryAttDir,
                     ESortByName|EDescending,
                     dirList );
  // Prune back to the parent folder path                           
  aPath.SetLength( monthFolderLength );                           
  if ( err == KErrNone )                           
    {
    TInt monthFolderCount = dirList->Count();                
    TInt index = 0;
    TBool done = EFalse;
    // Look through the list of folders in the month for the highest numbered folder
    // with the format YYYYMMAX Where YYYY is the year MM is the month A is an alphabetical
    // character in the range a-z or A-Z and X is a digit 0-9
    while ( index < monthFolderCount && !done )
      {
      done = ETrue;
      // The list is sorted in descending order. Get the last 2 characters from
      // the first directory in the list these indicate the highest folder number
      TPtrC name = ( *dirList )[index].iName; 
      TInt nameLength = name.Length();
      // Check the first character is in the range a-z or A-Z
      TChar firstChar = name[nameLength - KMultipleFolderNumberChars];
      firstChar.UpperCase();
      // If the character is not in the range then disregard this folder
      if ( firstChar < KCharacterOffset()[0] ||
           firstChar > KMaxCharacter()[0] )
        {
        done = EFalse;
        }
      // Check the second character is in the range 0-9                
      TChar secondChar = name[nameLength - 1];
      TInt secondCharVal = secondChar.GetNumericValue();
      if ( secondCharVal < 0 ||
           secondCharVal > KBase10 - 1 )
        {
        done = EFalse;
        }   
      if ( done )                             
        {
        TUint folderNumber = firstChar;
        // 10's part of folder number is represented by characters A-Z
        // convert the character into a decimal value
        folderNumber -= KCharacterOffset()[0];
        folderNumber *= KBase10; 
        // Now add on the units
        folderNumber += secondCharVal;
        aMonthCounters[aFolderType] = folderNumber;
        }
      // TUint folderNumber = name[nameLength - 2];       
      index++;   
      } 
    }
  delete dirList;  
  dirList = NULL;     

  PRINT1( _L("Camera <= CCamAppController::InitialiseMonthCounter returning %d"), err)
  return err;      
  }
  
// ---------------------------------------------------------------------------
// CreateDestinationFolder
// Creates the folder where new images/videos are to be saved to.
// ---------------------------------------------------------------------------
//
TInt 
CCamFolderUtility::CreateDestinationFolder( TInt* aMonthCounters,
                                            TDes& aPath,
                                            RFs&  aFs, 
                                            TInt  aFolderType,
                                            TInt  aRequiredFileCount )    
  {
  PRINT( _L("Camera => CCamFolderUtility::CreateDestinationFolder ") ) 
  __ASSERT_DEBUG( aFolderType < ECamFolderTypeLast, CamPanic( ECamPanicBadIndex ) );

  TInt folderCreated = EFalse;
  // This error value will only be retained if the counter is outside
  // the allowed range 
  TInt err = KErrArgument; 
  TInt monthCounter = aMonthCounters[aFolderType];
  while ( !folderCreated && monthCounter < KMaxMonthFolders )
    {
    aMonthCounters[aFolderType] = monthCounter;
    err = KErrNone;   
    // Add on the new counter
    aPath.Append( KCharacterOffset()[0] + monthCounter/KBase10 );
    aPath.Append( KDigitOffset()[0] + monthCounter%KBase10 );
    aPath.Append( KBackslash );
    err = aFs.MkDirAll( aPath );
    PRINT1( _L("Camera <> MkDirAll returned %d "), err ) 
    // If the folder already exists then check there is enough space for the required file count
    if ( err == KErrAlreadyExists )
      {
      PRINT( _L("Camera <> MkDirAll KErrALreadyExists ") ) 
      // if this is the final folder (Z9) there is no need to check for available space
      // this folder will be used anyway
      if ( monthCounter >= KMaxMonthFolders - 1 ) 
        {
        PRINT( _L("Camera <> MkDirAll KErrALreadyExists Z9") ) 
        folderCreated = ETrue;
        // sanity check to ensure the counter is not too high
        aMonthCounters[aFolderType] = KMaxMonthFolders - 1;
        err = KErrNone;
        }
      // if this is not the final folder (Z9) check for space and try the next one if necessary
      else
        {  
        PRINT( _L("Camera <> MkDirAll KErrALreadyExists not Z9, retry") )                        
        CDir* fileList;  
        TInt dirErr = ( aFs.GetDir( aPath,
                                    KEntryAttMaskSupported,
                                    ESortNone,
                                    fileList ) );  
        TBool spaceRemaining = EFalse;    
        if ( !dirErr )
          {
          spaceRemaining = ( fileList->Count() + aRequiredFileCount <= KMaxFilesPerFolder );   
          }
        delete fileList;  
        fileList = NULL;                      
        if ( dirErr )        
          {
          PRINT1( _L("Camera <= CCamFolderUtility::DoCreateDestinationFolderL returning %d"), err)   
          return dirErr;    
          }             
                
        // If there is insufficient space then try the next folder                                       
        if ( !spaceRemaining )  
          {
          monthCounter++;
          // Remove the previous counter characters and the trailing backslash then try again                                        
          aPath.SetLength( aPath.Length() - ( KMultipleFolderNumberChars + 1 ) );
          }
        else // This folder has enough space for the capture
          {
          folderCreated = ETrue;
          err = KErrNone;
          }     
        }
      }
    // There is a problem creating folders - report error         
    else if ( err )   
      {
      PRINT1( _L("Camera <= CCamFolderUtility::DoCreateDestinationFolderL returning %d"), err)
      return err;    
      }
    // A new folder has been created. There is no need to check the space
    else 
      {
      folderCreated = ETrue;
      }            
    }

  PRINT1( _L("Camera <= CCamFolderUtility::DoCreateDestinationFolderL returning %d"), err)
  return err;        
  }

// ===========================================================================

// end of file
