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


#include <gulicon.h>
#include <akntoolbar.h>
#include <aknbutton.h> 

#include <AiwServiceHandler.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include <centralrepository.h>


#include "camoneclickuploadutility.h"
#include "camlogging.h"
#include "CamPanic.h"
#include "Cam.hrh"

#include <cameraapp.mbg>
#include "CamUtility.h"

const TUid KShareOnlineCrUid = { 0x2002CC1F };
const TUint32 KShareCrApplicationVersion = 0x01010020;
const TInt KCamShareOnlineVersionBufLen = 12;
const TVersion KShareOnlineMinimumVersion( 5, 0, 0 );
const TUid KOpenModeOneClick = { 2 };
const TUid KCmdGetOneClickToolTip = { 15 };
const TUid KOpenModeShareSettings  = { 11 };

const TUint32 KShareImageServiceIconFileName = 0x00000002;
const TUint32 KShareVideoServiceIconFileName = 0x00000003;
const TUint32 KShareCommonServiceIconFileName = 0x00000004;


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
        ButtonTooltipL();
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
    
    if ( iDecoder )
        delete iDecoder;
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

    if ( iTooltip )
        {
        return *iTooltip;
        }

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
// Launch Share settings view
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::LaunchShareSettings()
    {
    PRINT( _L("Camera => CCamOneClickUploadUtility::LaunchShareSettings") );
    
    CAiwGenericParamList& paramList = iAiwServiceHandler->InParamListL();
    paramList.Reset();
    
    TAiwVariant openVariant( KOpenModeShareSettings );
    TAiwGenericParam openParam( EGenericParamModeActivation, openVariant );
    paramList.AppendL( openParam );
    iAiwServiceHandler->ExecuteServiceCmdL( KAiwCmdView, 
            paramList, iAiwServiceHandler->OutParamListL() );
    
    PRINT( _L("Camera <= CCamOneClickUploadUtility::LaunchShareSettings") );
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
    iAiwServiceHandler->AttachL( R_CAM_AIW_VIEW_INTEREST );
    PRINT( _L("Camera <> CCamOneClickUploadUtility::InitializeAiwL - initialized") );
    }

// ---------------------------------------------------------------------------
// Get the icon for Share AP item
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::CurrentIconPathL( TCamCameraMode aMode, TDes& aPath )
    {
    PRINT1( _L("Camera => CCamOneClickUploadUtility::CurrentIconPathL mode:%d"), aMode );
    TUint32 serviceIconId = KShareCommonServiceIconFileName;

    if ( ECamControllerVideo == aMode )
        {
        serviceIconId = KShareVideoServiceIconFileName;
        }
    else // image
        {
        serviceIconId = KShareImageServiceIconFileName;
        }

    CRepository* rep = CRepository::NewLC( KShareOnlineCrUid );
    User::LeaveIfError( rep->Get( serviceIconId, aPath ) );
    CleanupStack::PopAndDestroy( rep );
    PRINT1( _L("Camera <= CCamOneClickUploadUtility::CurrentIconPathL: %S"), &aPath );
    }

// -----------------------------------------------------------------------------
// CCamOneClickUploadUtility::UpdateUploadIconL
// -----------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::UpdateUploadIconL( CAknToolbar *aToolbar,
                                                  TCamCameraMode aMode )
    {
    PRINT( _L("Camera => CCamOneClickUploadUtility::UpdateUploadIconL") );

    TFileName currIcon;
    CurrentIconPathL( aMode, currIcon );

    // If the icons are different then load the icon
    PRINT1( _L("Camera <> current icon: %S"), &iIconFileName );
    if ( currIcon.Compare(iIconFileName) != 0 )
        {
        PRINT( _L("Camera <> Decoding icon") );
        iToolbar = aToolbar;
        TRAPD( err, DecodeIconL( &currIcon ) );
        if (err)
            {
            PRINT1( _L("Camera <> CamOneClickUploadUtility - Icon decoding failed: %d"), err );
            }
        }
    else
        {
        if ( aToolbar && iIconImage )
            {
            PRINT( _L("Camera <> Copying icon") );
            CAknButton* button = dynamic_cast<CAknButton*>(
                aToolbar->ControlOrNull( ECamCmdOneClickUpload ) );
            if ( button )
                {
                CAknButtonState* state = button->State();

                CGulIcon *icon = CGulIcon::NewL( iIconImage, iIconMask );
                state->SetIcon( icon );
                icon->SetBitmapsOwnedExternally( ETrue );

                button->SetButtonFlags( KAknButtonNoFrame | KAknButtonPressedDownFrame );
                aToolbar->DrawNow();
                }
            }
        }
    PRINT( _L("Camera <= CCamOneClickUploadUtility::UpdateUploadIconL") );
    }

// -----------------------------------------------------------------------------
// CCamOneClickUploadUtility::DecodeIconL()
// -----------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::DecodeIconL( TDesC* aPath )
    {
    PRINT1( _L("Camera => CCamOneClickUploadUtility::DecodeIconL: %S"), aPath );

    if ( !iDecoder )
        {
        iDecoder = CCamImageDecoder::NewL( *this );
        }
    iDecoder->StartIconConversionL( aPath );

    // Mark the given file as the icon in use when decoding has started
    iIconFileName.Copy( *aPath );

    PRINT( _L("Camera <= CCamOneClickUploadUtility::DecodeIconL") );
    }

// ---------------------------------------------------------------------------
// Image decoding complete notificationL
// ---------------------------------------------------------------------------
//
void CCamOneClickUploadUtility::ImageDecodedL( TInt aStatus, const CFbsBitmap* aBitmap, const CFbsBitmap* aMask )
    {
    PRINT( _L("Camera => CCamOneClickUploadUtility::ImageDecodedL") );

    if ( aStatus == KErrNone )
        {
        delete iIconImage;
        delete iIconMask;
        iIconImage = NULL;
        iIconMask = NULL;
        
        iIconImage = new (ELeave) CFbsBitmap;
        iIconImage->Duplicate( aBitmap->Handle() );

        if ( aMask )
            {
            iIconMask = new (ELeave) CFbsBitmap;
            iIconMask->Duplicate( aMask->Handle() );
            }
    
        if ( iToolbar )
            {
            PRINT( _L("Displaying icon") );
            
            CAknButton* uploadButton =
                        static_cast<CAknButton*> (iToolbar->ControlOrNull(ECamCmdOneClickUpload));
            
            CAknButtonState* currentState(NULL);
            if ( uploadButton )
                {
                currentState = uploadButton->State();
                }
            CGulIcon *icon(NULL);
            if ( iIconMask )
                {
                icon =  CGulIcon::NewL( iIconImage, iIconMask );
                }
            else
                {
                icon =  CGulIcon::NewL( iIconImage ); //Prevent freed pointer warning
                }
            icon->SetBitmapsOwnedExternally( ETrue );
            if ( currentState )
                {
                currentState->SetIcon( icon );
                }
            if ( uploadButton )
                {
                uploadButton->SetButtonFlags( KAknButtonNoFrame | KAknButtonPressedDownFrame );
                }
            iToolbar->DrawNow();
            }
        }
    else
        {
        PRINT1( _L("Camera <> CCamOneClickUploadUtility::ImageDecoded - err:%d"), aStatus );
        }

    PRINT( _L("Camera <= CCamOneClickUploadUtility::ImageDecoded") );
    }
