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

#include <QString>

#include "cxutils.h"
#include "thumbnailmanager_qt.h"




/*!
* ThumbnailManager::ThumbnailManager
*/
ThumbnailManager::ThumbnailManager(QObject* parentPtr)
{
    Q_UNUSED(parentPtr);

    mCurrentThumbnailId = 0;
    mThumbnailManagerIds.clear();
}


/*!
* ThumbnailManager::~ThumbnailManager
*/
ThumbnailManager::~ThumbnailManager()
{
}



/*!
* create thumbnail for the given image/video file name and data.
*/
int ThumbnailManager::getThumbnail(const QString& filename, void * clientData, int priority)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(priority);
        
    int status = KErrNone;

    if (filename.isNull() || filename.isEmpty()) {
        status = KErrNotFound;
    } else {
        // generate thumbnail id
        mThumbnailManagerIds.append(mCurrentThumbnailId);
    }

    emit thumbnailReady(QPixmap(), clientData, mCurrentThumbnailId, status);

    mCurrentThumbnailId++;
    
    CX_DEBUG_EXIT_FUNCTION();

    return status;
}



/*!
* start canceling creating thumbnail operation
*/
bool ThumbnailManager::cancelRequest(int id)
{
    return mThumbnailManagerIds.contains(id);
}
