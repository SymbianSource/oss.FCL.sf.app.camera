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
* Description:  Listbox used by CamInfoListBoxContainer*
*/



// INCLUDE FILES
#include "CamInfoListBox.h"
#include "CamInfoListBoxModel.h"

#include "CamCaptureSetupListItemDrawer.h"
#include "CamCaptureSetupControlHandler.h"

#include "CamUtility.h"

// ============================ MEMBER FUNCTIONS ===============================

CCamInfoListBox::~CCamInfoListBox()
	{
	
	}
        
/**
* C++ default constructor.
*/
CCamInfoListBox::CCamInfoListBox( MCamSettingValueObserver* aObserver,
									CCamAppController& aController )
:CCamCaptureSetupListBox( aObserver, aController )
	{
	
	}

/**
* By default Symbian 2nd phase constructor is private.
*/
void CCamInfoListBox::ConstructL( CCamAppController& aController,
								   const CCoeControl* aParent,
        						RPointerArray<HBufC>& aSummaryTitleTextArray,
        				 		RPointerArray<HBufC>& aSummaryDescriptionTextArray,
        				 		TInt aResourceId )
	{
	iBorder = TGulBorder::ENone;

	//Constructing model
    CCamInfoListBoxModel* model =
             CCamInfoListBoxModel::NewLC( aController, aSummaryTitleTextArray, 
             									aSummaryDescriptionTextArray, aResourceId, Rect() );

    CCamCaptureSetupListItemDrawer* itemDrawer = 
                                CCamCaptureSetupListItemDrawer::NewL( *model );

                                
    itemDrawer->SetParentControl( aParent );
    
    // CEikListBox takes ownership before anything can leave.
    // Do not need the model or drawer on the stack when call ConstructL,
    // because CEikListBox assigns objects as member variables before
    // ConstructL calls any leaving functions.
	CleanupStack::Pop( model ); 
	CEikListBox::ConstructL( model, itemDrawer, aParent, 0/*aFlags*/ );
    // The number of items is fixed after the list box has been constructed
    const TInt KMaxListItems = 5; // This value should come from the LAF
    // Find the number of items to be displayed in the list box
    iRequiredHeightInNumOfItems = Min( Model()->NumberOfItems(), KMaxListItems );
    SetItemHeightL( iItemDrawer->MinimumCellSize().iHeight );
	}

//  End of File  
