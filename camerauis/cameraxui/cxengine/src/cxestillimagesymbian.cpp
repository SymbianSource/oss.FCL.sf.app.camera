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

#include "cxestillimagesymbian.h"
#include "cxutils.h"

int CxeStillImageSymbian::mNextId = CxeStillImage::INVALID_ID + 1;

CxeStillImageSymbian::CxeStillImageSymbian()
  : mSnapshot(),
    mDataItem(NULL),
    mSaved(false),
    mId(++mNextId)
{
}

CxeStillImageSymbian::~CxeStillImageSymbian()
{
    // Not owned.
    mDataItem = NULL;
}

CxeImageDataItem* CxeStillImageSymbian::dataItem()
{
    return mDataItem;
}

QString CxeStillImageSymbian::filename() const
{
    return mFilename;
}

QPixmap CxeStillImageSymbian::snapshot() const
{
    return mSnapshot;
}

bool CxeStillImageSymbian::saved() const
{
    return mSaved;
}

int CxeStillImageSymbian::id() const
{
    return mId;
}

void CxeStillImageSymbian::setSnapshot( QPixmap pixmap )
{
    mSnapshot = pixmap;
}

void CxeStillImageSymbian::setFilename( const QString& filename )
{
    mFilename = filename;
}

void CxeStillImageSymbian::setSaved( bool saved )
{
    mSaved = saved;
}

void CxeStillImageSymbian::setDataItem( CxeImageDataItem* dataItem )
{
    mDataItem = dataItem;
}
