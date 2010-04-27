/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Helper class for implementing one-click upload support
*
*/


#ifndef CAMONECLICKUPLOADUTILITY_H
#define CAMONECLICKUPLOADUTILITY_H


#include <e32std.h>

#include "CamSettingsInternal.hrh"
#include "camimagedecoder.h"


class CAiwServiceHandler;
class AknToolbar;
class CGulIcon;

class CFbsBitmap;
class CCamImageDecoder;
class MCamImageDecoderObserver;

/**
 *  Helper class for implementing one-click upload support
 *
 *  @since S60 v5.0
 */
class CCamOneClickUploadUtility : public CBase,
                                  public MCamImageDecoderObserver
    {
public:
    /**
     * Two-phased constructor.
     */
    static CCamOneClickUploadUtility* NewL();

    /**
    * Destructor.
    */
    virtual ~CCamOneClickUploadUtility();

public:

    /**
    * Check if one-click upload is supported.
    * @since S60 v5.0
    * @return A boolean value indicating if one-click upload support is present.
    */
    TBool OneClickUploadSupported() const;

    /**
    * Upload a file.
    * @since S60 v5.0
    * @param aFilename Full path to the file to be uploaded.
    */
    void UploadL( const TDesC& aFilename );

    /**
    * Upload multiple files.
    * @since S60 v5.0
    * @param aFilenames Full paths to the files to be uploaded.
    */
    void UploadL( const MDesC16Array& aFilenames );

    /**
    * Get the tooltip text for one-click upload button.
    * @since S60 v5.0
    * @return Tooltip text
    */
    const TDesC& ButtonTooltipL();
    
    /**
     * Get the current one click icon
     */
    void UpdateUploadIconL( CAknToolbar *aToolbar, TCamCameraMode aMode );
    
    /**
     * Launch Share settings view 
     */
    void LaunchShareSettings();

    /**
     * From MCamImageDecoderObserver
     */
    virtual void ImageDecodedL( TInt aStatus, const CFbsBitmap* aBitmap, 
                               const CFbsBitmap* aMask );    
    
private:
    void ConstructL();
    void CheckVersionL();
    void InitializeAiwL();
    CCamOneClickUploadUtility();
    
    /**
    * Obtains the path for icon to be used. 
    * @param aMode the current camera mode
    * @param aPath descriptor containing the path of icon to be used 
    */
    void CurrentIconPathL( TCamCameraMode aMode, TDes& aPath ); 

    /**
    * Decodes / starts decoding of icon to be used.
    * @param aPath descriptor containing the path of icon to be used 
    */
    void DecodeIconL( TDesC* aPath );

private: // Data

    TBool iUploadSupported;

    // Own
    CAiwServiceHandler* iAiwServiceHandler;
    
    CFbsBitmap*    iDecodedBitmap;
    CCamImageDecoder* iDecoder;    

    TFileName iIconFileName;
    CFbsBitmap* iIconImage;
    CFbsBitmap* iIconMask;
    
    CAknToolbar* iToolbar;

    // Own
    HBufC* iTooltip;
    };

#endif // CAMONECLICKUPLOADUTILITY_H
