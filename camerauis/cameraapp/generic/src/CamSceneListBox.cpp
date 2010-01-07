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
* Description:  Camera scenes list-box*
*/



// INCLUDE FILES
#include "CamSceneListBox.h"
#include "CamSceneListBoxModel.h"

#include "CamCaptureSetupListItemDrawer.h"
#include "CamCaptureSetupControlHandler.h"

#include "CamUtility.h"

// ============================ MEMBER FUNCTIONS ===============================

CCamSceneListBox::~CCamSceneListBox()
  {
  
  }
        
/**
* C++ default constructor.
*/
CCamSceneListBox::CCamSceneListBox( TCamCameraMode            aMode,
                                    MCamSettingValueObserver* aObserver,
                                    CCamAppController&        aController )
  : CCamCaptureSetupListBox( aObserver, aController ), 
    iMode( aMode )
  {
  }

/**
* By default Symbian 2nd phase constructor is private.
*/
void CCamSceneListBox::ConstructL( 
        CCamAppController& aController,
        const CCoeControl* aParent,
        RPointerArray<HBufC>& aSummaryTitleTextArray,
        RPointerArray<HBufC>& aSummaryDescriptionTextArray,
        TBool aUserBaseScenes )
  {
  iBorder = TGulBorder::ENone;

  //Constructing model
  CCamSceneListBoxModel* model =
      CCamSceneListBoxModel::NewLC( aController, 
                                    iMode, 
                                    aSummaryTitleTextArray, 
                                    aSummaryDescriptionTextArray, 
                                    aUserBaseScenes,
                                    Rect() );

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
