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
* Description:  Model for CamInfoListBox*
*/



// INCLUDE FILES
#include <barsread.h>
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
#include "CamInfoListBoxModel.h"
#include "CamCaptureSetupListItem.h"
#include "camlogging.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamInfoListBoxModel::CCamInfoListBoxModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamInfoListBoxModel::CCamInfoListBoxModel( CCamAppController& aController,
        				 RPointerArray<HBufC>& aSummaryTitleTextArray,
        				 RPointerArray<HBufC>& aSummaryDescriptionTextArray )
: CCamCaptureSetupListBoxModel( aController, ETrue ),  
  iSummaryTitleTextArray( aSummaryTitleTextArray ),
  iSummaryDescriptionTextArray( aSummaryDescriptionTextArray )
    {
    }

// -----------------------------------------------------------------------------
// CCamInfoListBoxModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamInfoListBoxModel::ConstructL( TInt aResourceId, TRect aListBoxRect )
    {
    // read layout data
    ReadLayoutData( aListBoxRect ); 
    GetListboxDataL( aResourceId );    	
    }
 
// -----------------------------------------------------------------------------
// CCamInfoListBoxModel::GetListboxDataL
// -----------------------------------------------------------------------------
//   
void CCamInfoListBoxModel::GetListboxDataL( TInt aArrayResourceId )
 	{
 	
    
    // create array from resource
    TResourceReader reader;
    CEikonEnv::Static()->CreateResourceReaderLC( reader, aArrayResourceId );
    const TInt count = reader.ReadInt16();
    
    // for each entry in the resource array, create a new list item
	TInt i;
    for ( i = 0; i < count; i++ )
        {
        CCamCaptureSetupListItem* listItem = CCamCaptureSetupListItem::
                               NewLC( reader, iIconLayoutData );
        
        iItemArray.AppendL( listItem );
        CleanupStack::Pop( listItem );        
        }    
    CleanupStack::PopAndDestroy(); // reader
 	}


	
// -----------------------------------------------------------------------------
// CCamInfoListBoxModel::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamInfoListBoxModel* CCamInfoListBoxModel::NewLC( CCamAppController& aController,
        				 RPointerArray<HBufC>& aSummaryTitleTextArray,
        				 RPointerArray<HBufC>& aSummaryDescriptionTextArray,
        				 TInt aResourceId,
        				 TRect aListBoxRect )
    {
    CCamInfoListBoxModel* self = new( ELeave ) CCamInfoListBoxModel( aController, 
    													aSummaryTitleTextArray, 
    													aSummaryDescriptionTextArray );
    
    CleanupStack::PushL( self );
    self->ConstructL( aResourceId, aListBoxRect );
    return self;
    }

    
// Destructor
CCamInfoListBoxModel::~CCamInfoListBoxModel()
    {
    }

//  End of File  
