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
* Description:  Help saving buffer of data (e.g. image content) to a file
*                with one function call. Used for testing purposes.
*
*/


#ifndef CAM_FILESAVEUTILITY_H
#define CAM_FILESAVEUTILITY_H


#include <f32file.h>
#include <pathinfo.h>

#include "camlogging.h"

namespace NCamCameraController
  {
  inline void SaveImageDataToFileL( const TDesC8& aData, const TDesC& aFilename )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    
    RFile file;
    CleanupClosePushL( file );
  
    TFileName filename;
    filename.Append( PathInfo::PhoneMemoryRootPath() );
    filename.Append( PathInfo::ImagesPath() );
    filename.Append( aFilename );
  
    PRINT1( _L("CamTest: SaveImageDataToFileL file(%S)"), &filename );
    
    User::LeaveIfError( file.Replace( fs, filename, EFileShareExclusive|EFileWrite ) );
    User::LeaveIfError( file.Write( aData ) );
    
    CleanupStack::PopAndDestroy( 2 ); // file, fs: Close()
    };
  }
#endif // CAM_FILESAVEUTILITY_H

