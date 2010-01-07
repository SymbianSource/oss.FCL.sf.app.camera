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
* Description:  Setting item page for default name (text/date).*
*/



// INCLUDE FILES
#include "Camdefaultnamesettingpage.h"
#include "CamUtility.h"
#include "CamAppUi.h"

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include <AknQueryDialog.h>
#include <StringLoader.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCamDefaultNameSettingPage::CCamDefaultNameSettingPage
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamDefaultNameSettingPage::CCamDefaultNameSettingPage(
    TDes&           aNameBase,
    TCamCameraMode  aMode,
    const TDesC*    aSettingTitleText, 
    TInt            aSettingNumber, 
    TInt            aControlType,
    TInt            aEditorResourceId, 
    TInt            aSettingPageResourceId,
    MAknQueryValue& aQueryValue )
    : CAknPopupSettingPage( aSettingTitleText, aSettingNumber,
                            aControlType, aEditorResourceId,
                            aSettingPageResourceId,
                            aQueryValue ),
      iNameBase( aNameBase ),
      iMode( aMode )
    {
    }
    

// ---------------------------------------------------------------------------
// CCamDefaultNameSettingPage::~CCamDefaultNameSettingPage
// Destructor.
// ---------------------------------------------------------------------------
//
CCamDefaultNameSettingPage::~CCamDefaultNameSettingPage()
    {
    }


// ---------------------------------------------------------------------------
// CCamDefaultNameSettingPage::OkToExitL
// Check if the name base value is ok, so the user can
// exit the setting page.
// ---------------------------------------------------------------------------
//
TBool CCamDefaultNameSettingPage::OkToExitL( TBool aAccept )
    {
    if ( aAccept )
        {
        iTempNameBase = iNameBase;
        
        if ( QueryValue()->CurrentValueIndex() == ECamNameBaseText )
            {
            // Ensure application is in editing orientation
            CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
            TInt editingOrientation = CAknAppUiBase::EAppUiOrientationUnspecified;
            TInt error = CamUtility::GetPsiInt( ECamPsiEditingOrientation, editingOrientation );
            if ( !error )
                {
                appUi->SwitchOrientationIfRequiredL( editingOrientation );
                }  
            // Show the editor dialog                
            TBool editorRet = EFalse;  
            // Trap this to make sure the orientation is reverted back to normal   
            TRAPD( leaveErr, editorRet = ShowEditorDialogL() )
            appUi->SwitchOrientationIfRequiredL( CAknAppUiBase::EAppUiOrientationUnspecified );
            User::LeaveIfError( leaveErr ); 
            return editorRet;
            }
        }
    return ETrue;
    }
 
// ---------------------------------------------------------------------------
// CCamDefaultNameSettingPage::ShowEditorDialogL
// Display the text editor dialog
// ---------------------------------------------------------------------------
//
TBool CCamDefaultNameSettingPage::ShowEditorDialogL()    
    {
    TBool done = EFalse;
    while ( !done )
        {
        // Show data query
        HBufC* prompt = NULL;
        if ( ECamControllerVideo == iMode )
            {
            prompt = StringLoader::LoadLC( R_CAM_QUERY_DEFAULT_VIDEO_NAME );
            }
        else
            {
            prompt = StringLoader::LoadLC( R_CAM_QUERY_DEFAULT_IMAGE_NAME );
            }

        CAknTextQueryDialog* nameQuery = 
                    CAknTextQueryDialog::NewL( iTempNameBase );

        nameQuery->SetMaxLength( KMaxNameBaseLength );
        nameQuery->PrepareLC( R_CAM_RENAME_QUERY );
        nameQuery->SetPromptL( *prompt );

        TBool res = ( nameQuery->RunLD() == EAknSoftkeyOk );

        CleanupStack::PopAndDestroy(); // prompt

        if ( !res )
            {
            // User cancelled the query, return to setting page
            return EFalse;
            }

        // Check base name validity
        res = CamUtility::CheckFileNameValidityL( iTempNameBase );

        if ( res )
            {
            // New name base was valid, exit setting page
            iNameBase = iTempNameBase;
            return res;
            }

        // Show data query again
        }
    // should never get here        
    return EFalse;
    }
// end of file
