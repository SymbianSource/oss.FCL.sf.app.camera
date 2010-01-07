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


#include <AiwServiceHandler.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <centralrepository.h>

#include "camoneclickuploadutility.h"
#include "camlogging.h"
#include "CamPanic.h"

const TUid KShareOnlineCrUid = { 0x2000BB53 };
const TUint32 KShareCrApplicationVersion = 0x01010020;
const TInt KCamShareOnlineVersionBufLen = 12;
const TVersion KShareOnlineMinimumVersion( 4, 3, 0 );
const TUid KOpenModeOneClick = { 2 };
const TUid KCmdGetOneClickToolTip = { 15 };


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CCamOneClickUploadUtility* CCamOneClickUploadUtility::NewL()
    {
    CCamOneClickUploadUtility* self =
        new ( ELeave ) CCamOneClickUploadUtility();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamOneClickUploadUtility::CCamOneClickUploadUtility()
    {
    // No implementation needed
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::ConstructL()
    {
    PRINT( _L( "Camera => CCamOneClickUploadUtility::ConstructL()" ) );
#ifndef __WINSCW__
    // iUploadSupported is zeroed by CBase constructor.
    // If a leave occurs here, it is left as EFalse.
    // If InitializeAiwL() succeeds, it is set to ETrue.
    TRAP_IGNORE(
        {
        CheckVersionL();
        InitializeAiwL();
        iUploadSupported = ETrue;
        } );
#else
    iUploadSupported = EFalse;
#endif // __WINSCW__    
    PRINT1( _L( "Camera <> iUploadSupported = %d" ), iUploadSupported );
    PRINT( _L( "Camera <= CCamOneClickUploadUtility::ConstructL()" ) );
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamOneClickUploadUtility::~CCamOneClickUploadUtility()
    {
    delete iAiwServiceHandler;
    delete iTooltip;
    }


// ---------------------------------------------------------------------------
// Check if one-click upload is supported
// ---------------------------------------------------------------------------
//
TBool CCamOneClickUploadUtility::OneClickUploadSupported() const
    {
    return iUploadSupported;
    }


// ---------------------------------------------------------------------------
// Upload a file
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::UploadL( const TDesC& aFilename )
    {
    PRINT( _L( "Camera => CCamOneClickUploadUtility::UploadL(TDesC&)" ) );

    CDesC16ArrayFlat* array = new ( ELeave ) CDesC16ArrayFlat( 1 );
    CleanupStack::PushL( array );

    array->AppendL( aFilename );

    UploadL( *array );

    CleanupStack::PopAndDestroy( array );

    PRINT( _L( "Camera <= CCamOneClickUploadUtility::UploadL(TDesC&)" ) );
    }

// ---------------------------------------------------------------------------
// Upload multiple files
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::UploadL( const MDesC16Array& aFilenames )
    {
    PRINT( _L( "Camera => CCamOneClickUploadUtility::UploadL(MDesC16Array&)" ) );

    __ASSERT_ALWAYS( iAiwServiceHandler, CamPanic( ECamPanicNullPointer ) );

    CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
    paramList.Reset();

    TAiwVariant openVariant( KOpenModeOneClick );
    TAiwGenericParam openParam( EGenericParamModeActivation, openVariant );
    paramList.AppendL( openParam );

    for ( TInt i = 0; i < aFilenames.MdcaCount(); i++ )
        {
        TPtrC filename = aFilenames.MdcaPoint( i );
        PRINT1( _L( "Camera <> adding file %S" ), &filename );
        TAiwVariant variant( filename );
        TAiwGenericParam param( EGenericParamFile, variant );
        paramList.AppendL( param );
        }

    iAiwServiceHandler->ExecuteServiceCmdL(
            KAiwCmdUpload,
            paramList,
            iAiwServiceHandler->OutParamListL() );

    PRINT( _L( "Camera <= CCamOneClickUploadUtility::UploadL(MDesC16Array&)" ) );
    }

// ---------------------------------------------------------------------------
// Get the tooltip text for one-click upload button.
// ---------------------------------------------------------------------------
//
const TDesC& CCamOneClickUploadUtility::ButtonTooltipL()
    {
    __ASSERT_ALWAYS( iAiwServiceHandler, CamPanic( ECamPanicNullPointer ) );

    CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
    paramList.Reset();

    TAiwVariant openVariant( KCmdGetOneClickToolTip );
    TAiwGenericParam openParam( EGenericParamModeActivation, openVariant );
    paramList.AppendL( openParam );

    CAiwGenericParamList& outParamList = iAiwServiceHandler->OutParamListL();
    outParamList.Reset();
    iAiwServiceHandler->ExecuteServiceCmdL(
            KAiwCmdUpload, paramList, outParamList );

    TInt index = 0;
    const TAiwGenericParam* res = outParamList.FindFirst(
                index,
                EGenericParamNoteItem,
                EVariantTypeDesC );

    if ( !res )
        {
        User::Leave( KErrArgument );
        }

    delete iTooltip;
    iTooltip = NULL;
    iTooltip = res->Value().AsDes().AllocL();
    return *iTooltip;
    }


// ---------------------------------------------------------------------------
// Check Share Online version
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::CheckVersionL()
    {
    PRINT( _L( "Camera => CCamOneClickUploadUtility::CheckVersionL()" ) );

    CRepository* rep = CRepository::NewLC( KShareOnlineCrUid );
    TBuf<KCamShareOnlineVersionBufLen> versionBuf;
    User::LeaveIfError( rep->Get( KShareCrApplicationVersion, versionBuf ) );
    TVersion version( 0, 0, 0 );
    TLex lex( versionBuf );
    User::LeaveIfError( lex.Val( version.iMajor ) );
    if ( lex.Get() != TChar('.') )
        {
        User::Leave( KErrCorrupt );
        }
    User::LeaveIfError( lex.Val( version.iMinor ) );
    if ( lex.Get() != TChar('.') )
        {
        User::Leave( KErrCorrupt );
        }
    User::LeaveIfError( lex.Val( version.iBuild ) );

    PRINT3( _L( "Camera <> CCamOneClickUploadUtility::CheckVersionL() - share online %d.%d.%d" ), version.iMajor, version.iMinor, version.iBuild );

    // Compare version number and leave if the detected
    // version is less than KShareOnlineMinimumVersion.
    if ( version.iMajor < KShareOnlineMinimumVersion.iMajor )
        {
        User::LeaveIfError( KErrNotSupported );
        }
    else if ( version.iMajor == KShareOnlineMinimumVersion.iMajor )
        {
        if ( version.iMinor < KShareOnlineMinimumVersion.iMinor )
            {
            User::LeaveIfError( KErrNotSupported );
            }
        else if ( version.iMinor == KShareOnlineMinimumVersion.iMinor )
            {
            if ( version.iBuild < KShareOnlineMinimumVersion.iBuild )
                {
                User::LeaveIfError( KErrNotSupported );
                }
            else
                {
                // Version is supported, fall through
                }
            }
        else
            {
            // Version is supported, fall through
            }
        }
    else
        {
        // Version is supported, fall through
        }

    CleanupStack::PopAndDestroy( rep );

    PRINT( _L( "Camera <= CCamOneClickUploadUtility::CheckVersionL()" ) );
    }


// ---------------------------------------------------------------------------
// Initialize AIW framework
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::InitializeAiwL()
    {
    __ASSERT_DEBUG( !iAiwServiceHandler, CamPanic( ECamPanicResourceLeak ) );

    iAiwServiceHandler = CAiwServiceHandler::NewL();
    iAiwServiceHandler->AttachL( R_CAM_ONE_CLICK_UPLOAD_INTEREST );
    }
