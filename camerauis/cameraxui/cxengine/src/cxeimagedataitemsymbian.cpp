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

#include <bautils.h> // for deleting files
#include "cxeimagedataitemsymbian.h"
#include "cxeerrormappingsymbian.h"
#include "cxesysutil.h"
#include "cxutils.h"
#include "cxestate.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxeimagedataitemsymbianTraces.h"
#endif


CxeImageDataItemSymbian::CxeImageDataItemSymbian(QByteArray data,
                                                 QString filename,
                                                 int id,
                                                 CxeImageDataItem::State state)
  : CxeStateMachine("CxeImageDataItemSymbian"),
    mError(KErrNone),
    mId(id),
    mData(data),
    mPath(filename)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeImageDataItem::State>();
    qRegisterMetaType<CxeError::Id>();
    // Init mState
    initializeStates();
    setInitialState(state);
    // Init delayer variables
    //mDelayedDelete = false;
    //mDelayedRename = false;
    //mDelayedFileName = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

CxeImageDataItemSymbian::~CxeImageDataItemSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Close file
    mFile.Close();

    // Close file system
    mFs.Close();

    CX_DEBUG_EXIT_FUNCTION();
}

/*
void CxeImageDataItemSymbian::deleteImage()
    {
    CX_DEBUG_ENTER_FUNCTION();

    int err = KErrNone;

    //! @todo: make this function return a KErrNotReady if below
    if ( mState != CxeImageDataItem::Idle )
        {
        CX_DEBUG(("Error: This data item has no data..."));
        CX_DEBUG_ASSERT(0); // panics
        return;
        }

    // do delete or delayed delete
    if (( mState == CxeImageDataItem::Waiting ) || ( mState == CxeImageDataItem::Saving ))
        {
        // we are currently saving, so we have to delete later
        CX_DEBUG(("delayed delete"));
        mDelayedDelete = true;
        }
    else
        {
        // delete now
        CX_DEBUG(("deleting now..."));
        err = KErrNotFound;


        //! @todo: this is horrible for performance... there is no need to create multiple server sessions
        RFs fs;
        TInt connectError = fs.Connect();
        BaflUtils ba;
        if( !connectError && ba.FileExists( fs, *mPath ) )
            {
            err = KErrNone;
            ba.DeleteFile( fs, *mPath );
            }
        fs.Close();
        }

    CX_DEBUG(("err: %d", err));

    CX_DEBUG_EXIT_FUNCTION();
    //return err; //! @todo
    }

void CxeImageDataItemSymbian::renameImage( const TDesC& newPath )
    {
    CX_DEBUG_ENTER_FUNCTION();

    int err = KErrNone;

    //! @todo: make this function return a KErrNotReady if below
    if ( mState != CxeImageDataItem::Idle )
        {
        CX_DEBUG(("Error: This data item has no data..."));
        CX_DEBUG_ASSERT(0); // panics
        return;
        }

    // do rename or delayed rename
    if (( mState == CxeImageDataItem::Waiting ) || ( mState == CxeImageDataItem::Saving ))
        {
        // we are currently saving, so we have to rename later
        CX_DEBUG(("delayed rename"));
        mDelayedRename = true;

        TRAP( err,
            mDelayedFileName = HBufC::NewL( newPath.Length() );
            mDelayedFileName->Des().Append( newPath );
            );
        }
    else
        {
        // rename now
        CX_DEBUG(("delayed rename"));
        err = KErrNotFound;
        RFs fs;
        TInt connectError = fs.Connect();
        BaflUtils ba;
        if( !connectError && ba.FileExists( fs, *mPath ) )
            {
            err = KErrNone;
            ba.RenameFile( fs, *mPath, newPath );
            }
        fs.Close();
        }

    CX_DEBUG(("err: %d", err));

    CX_DEBUG_EXIT_FUNCTION();
    //return err; //! @todo
    }
*/

CxeError::Id CxeImageDataItemSymbian::save()
{
    CX_DEBUG_ENTER_FUNCTION();

    mError = KErrNone;

    CX_DEBUG(( "Starting to save %s", mPath.toAscii().constData() ));

    if (mPath.isEmpty()) {
        CX_DEBUG(("Filename not set !"));
        mError = KErrArgument;
    }

    TPtrC16 filename;

    if (!mError) {
        filename.Set(reinterpret_cast<const TUint16*>(mPath.utf16()));
        // Init
        mError = mFs.Connect();
        CX_DEBUG(("mFsSession.Connect mError=%d", mError));
    }

    // Get drive number
    TInt drive = 0;
    if (!mError) {
        mError = RFs::CharToDrive(filename[0], drive);
        CX_DEBUG(("CharToDrive mError=%d", mError));
    }

    // Check disk space
    if (!mError) {
        TBool fullDisk = EFalse;
        fullDisk = checkDiskSpace(&mFs, mData.size(), drive);
        if (fullDisk) {
            CX_DEBUG(("SysUtil::FullDisk"));
            mError = KErrDiskFull;
        }
    }

    // Attempt to create the file
    if (!mError) {
        // Note: In sake of MDS not starting harvesting here,
        // do not use RFile::Replace. If harvesting is started now,
        // our later call to harvest may be ignored and
        // file may be missing from "Captured" album.
        mError = mFile.Create(mFs, filename, EFileWrite);
        CX_DEBUG(("file.Create mError=%d", mError));
    }

    // Write the file
    if (!mError) {
        // Update state
        setState(CxeImageDataItem::Saving);

        CX_DEBUG(("about to write to file"));
        TPtrC8 data(reinterpret_cast<const TUint8*> (mData.constData()), mData.size());
        mError = mFile.Write(data); // synchronous
        saveCleanup();
        CX_DEBUG(("file write completed"));
    }

    mFile.Close(); //~400us
    mFs.Close();   //~450us
    OstTrace0(camerax_performance, CXEIMAGEDATAIMTEMSYMBIAN_SAVED, "msg: e_CX_SHOT_TO_SAVE 0");

    if (mError == KErrNone) {
        setState(CxeImageDataItem::Saved);
    } else {
        setState(CxeImageDataItem::SaveFailed);
    }
    emit imageSaved(CxeErrorHandlingSymbian::map(mError), mPath, mId);

    CX_DEBUG(("mError: %d", mError));
    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(mError);
}

/**
* Get the id number of this data item.
*/
int CxeImageDataItemSymbian::id() const
{
    return mId;
}

/**
* Get the path of this data item.
*/
QString CxeImageDataItemSymbian::path() const
{
    return mPath;
}


int CxeImageDataItemSymbian::checkDiskSpace(RFs* aFs,
        TInt aBytesToWrite,
        TInt aDrive)
{
    CX_DEBUG_ENTER_FUNCTION();
    int value = CxeSysUtil::DiskSpaceBelowCriticalLevel(
            aFs,
            aBytesToWrite,
            aDrive );
    return value;
}

void CxeImageDataItemSymbian::saveCleanup()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_ASSERT( state() == CxeImageDataItem::Saving );

    // Flush file.
    if (!mError) {
        CX_DEBUG(("flushing..."));
        mError = mFile.Flush();
        CX_DEBUG(("flushed"));
    }


    /*
    // Delayed rename, if needed
    if (( !mError ) && ( mDelayedRename ))
        {
        CX_DEBUG(("doing delayed rename..."));
        mError = KErrNotFound;
        BaflUtils ba;
        if( ba.FileExists( mFs, *mPath ) )
            {
            mError = KErrNone;
            TPtrC newPath = *mDelayedFileName;
            ba.RenameFile( mFs, *mPath, newPath );
            }
        mDelayedRename = false;
        CX_DEBUG(("rename done, mError: %d", mError));
        }

    // Delayed delete, if needed
    if (( !mError ) && ( mDelayedDelete ))
        {
        CX_DEBUG(("doing delayed delete..."));
        mError = KErrNotFound;
        BaflUtils ba;
        if( ba.FileExists( mFs, *mPath ) )
            {
            mError = KErrNone;
            ba.DeleteFile( mFs, *mPath );
            }
        mDelayedDelete = false;
        CX_DEBUG(("delete done, mError: %d", mError));
        }*/

    CX_DEBUG_EXIT_FUNCTION();
}

CxeImageDataItem::State CxeImageDataItemSymbian::state() const
{
    return static_cast<CxeImageDataItem::State> (stateId());
}

void CxeImageDataItemSymbian::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State> (newStateId), error);
}

void CxeImageDataItemSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState(SavePending, "SavePending", Saving | SaveFailed));
    addState(new CxeState(Saving, "Saving", Saved | SaveFailed));
    addState(new CxeState(Saved, "Saved", 0));
    addState(new CxeState(SaveFailed, "SaveFailed", 0));
}
