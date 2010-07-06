/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXEIMAGEDATAITEMUNIT_H
#define CXEIMAGEDATAITEMUNIT_H

//  Include Files

#include "cxeimagedataitemsymbian.h"


class CxeImageDataItemSymbian;
class RFs;


/**
 * Camera device control class for unit testing. Identical to
 * CxeCameraDeviceControlSymbian except for that CxeFakeCameraDevice is used
 * instead of the real implementation.
 */
class CxeImageDataItemUnit : public CxeImageDataItemSymbian 
{
	Q_OBJECT	
public:

    CxeImageDataItemUnit(int index, QByteArray data, QString filename, bool addLocation);
    virtual ~CxeImageDataItemUnit();
    int checkDiskSpace(RFs* aFs,
                TInt aBytesToWrite,
                TInt aDrive);

protected: // from CxeImageDataItemSymbian
    //int save();
};

#endif  // CXEFAKECAMERADEVICECONTROL_H
