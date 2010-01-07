/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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

class CAiwServiceHandler;


/**
 *  Helper class for implementing one-click upload support
 *
 *  @since S60 v5.0
 */
class CCamOneClickUploadUtility : public CBase
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

private:
    void ConstructL();
    void CheckVersionL();
    void InitializeAiwL();
    CCamOneClickUploadUtility();

private: // Data

    TBool iUploadSupported;

    // Own
    CAiwServiceHandler* iAiwServiceHandler;

    // Own
    HBufC* iTooltip;
    };


#endif // CAMONECLICKUPLOADUTILITY_H

