/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 
#include <aknViewAppUi.h>
#include <gscamerapluginrsc.rsg>
#include <AknQueryDialog.h>
#include <StringLoader.h>
#include "CamSettingsInternal.hrh"
#include "GSCamdefaultnamesettingpage.h"
#include "CamUtility.h"
// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CGSCamDefaultNameSettingPage::CGSCamDefaultNameSettingPage
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamDefaultNameSettingPage::CGSCamDefaultNameSettingPage(
    TDes&           aNameBase,
    TCamCameraMode  aMode,
    const TDesC*    aSettingTitleText, 
    TInt            aSettingNumber, 
    TInt            aControlType,
    TInt            aEditorResourceId, 
    TInt            aSettingPageResourceId,
    MAknQueryValue& aQueryValue,
    TBool           aCamera )
    : CAknPopupSettingPage( aSettingTitleText, aSettingNumber,
                            aControlType, aEditorResourceId,
                            aSettingPageResourceId,
                            aQueryValue ),
      iNameBase( aNameBase ),
      iMode( aMode ),
      iCamera ( aCamera )
    {
    iNameSelected  = QueryValue()->CurrentValueIndex();
    }
    

// ---------------------------------------------------------------------------
// CGSCamDefaultNameSettingPage::~CGSCamDefaultNameSettingPage
// Destructor.
// ---------------------------------------------------------------------------
//
CGSCamDefaultNameSettingPage::~CGSCamDefaultNameSettingPage()
    {
    }


// ---------------------------------------------------------------------------
// CGSCamDefaultNameSettingPage::OkToExitL
// Check if the name base value is ok, so the user can
// exit the setting page.
// ---------------------------------------------------------------------------
//
TBool CGSCamDefaultNameSettingPage::OkToExitL( TBool aAccept )
    {
    if ( aAccept )
        {
        iTempNameBase = iNameBase;
        
        if ( QueryValue()->CurrentValueIndex() == ECamNameBaseText )
            {
            // Ensure application is in editing orientation
            CAknAppUiBase* appUi =  
            static_cast<CAknAppUiBase*>( CCoeEnv::Static()->AppUi() );

            // Show the editor dialog                
            TBool editorRet = EFalse;  
            // Trap this to make sure the orientation is reverted back to normal   
            TRAPD( leaveErr, editorRet = ShowEditorDialogL() )
            if ( !editorRet )
                {
                // redraw listbox to update radio button when cancel is pressed 
                ListBoxControl()->DrawNow();
                }
            if ( !iCamera )
                { 
                appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape );
                }
            else
                {
                //appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationPortrait );
                }
            User::LeaveIfError( leaveErr ); 
            return editorRet;
            }
           
        }
    else  
        {
        // Cancelled
        QueryValue()->SetCurrentValueIndex( iNameSelected );
        }
    return ETrue;
    }
 
// ---------------------------------------------------------------------------
// CGSCamDefaultNameSettingPage::ShowEditorDialogL
// Display the text editor dialog
// ---------------------------------------------------------------------------
//
TBool CGSCamDefaultNameSettingPage::ShowEditorDialogL()    
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
