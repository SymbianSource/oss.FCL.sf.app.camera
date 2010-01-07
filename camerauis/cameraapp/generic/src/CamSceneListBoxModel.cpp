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
* Description:  Scene list box model*
*/



// INCLUDE FILES
#include "CamSceneListBoxModel.h"
#include "CamCaptureSetupListItem.h"
#include "CamUtility.h"
#include <cameraapp.rsg>
#include <vgacamsettings.rsg> 

#include <barsread.h>
#include <aknlayoutscalable_apps.cdl.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamSceneListBoxModel::CCamSceneListBoxModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamSceneListBoxModel::CCamSceneListBoxModel( 
    CCamAppController& aController,
    TCamCameraMode aMode,
    RPointerArray<HBufC>& aSummaryTitleTextArray,
    RPointerArray<HBufC>& aSummaryDescriptionTextArray,
    TBool aUserBaseScenes )
  : CCamCaptureSetupListBoxModel( aController, ETrue ),
    iMode( aMode ),
    iSummaryTitleTextArray( aSummaryTitleTextArray ),
    iSummaryDescriptionTextArray( aSummaryDescriptionTextArray ),
    iUserBaseScenes( aUserBaseScenes ),
    iController( aController )
  {
  }

// -----------------------------------------------------------------------------
// CCamSceneListBoxModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamSceneListBoxModel::ConstructL( TRect aListBoxRect )
  {
  ReadLayoutData( aListBoxRect );  
  GetDataFromResourceL();
  }


// -----------------------------------------------------------------------------
// GetDataFromResourceL
// -----------------------------------------------------------------------------
//
void
CCamSceneListBoxModel::GetDataFromResourceL()
  {
  TInt resource = KErrNotFound;
  
  if( ECamControllerVideo == iMode )
    {
    resource = R_CAM_CAPTURE_SETUP_LIST_VIDEO_SHOOTING_MODE;
    }
  else
    {
    if( iUserBaseScenes )
      resource = R_CAM_CAPTURE_SETUP_LIST_USER_SCENE_SHOOTING_MODE;
    else
      resource = R_CAM_CAPTURE_SETUP_LIST_PHOTO_SHOOTING_MODE;
    }

  if( KErrNotFound != resource )
    {
    GetScenesDataL( resource );
    }
  }


// -----------------------------------------------------------------------------
// CCamSceneListBoxModel::GetScenesDataL
// Load up data and descriptions for supported 
// -----------------------------------------------------------------------------
//   
void CCamSceneListBoxModel::GetScenesDataL( TInt aArrayResourceId )
  {

  // create array from resource
  TResourceReader reader;
  CEikonEnv::Static()->CreateResourceReaderLC( reader, aArrayResourceId );
  const TInt count = reader.ReadInt16();
    
    // for each entry in the resource array, create a new list item
  TInt i;
  for ( i = 0; i < count; i++ )
    {
    CCamCaptureSetupListItem* listItem = 
      CCamCaptureSetupListItem::NewLC( reader, iIconLayoutData );
    
    TInt sceneVal = listItem->ItemValue();                      
    if( IsSupportedScene( sceneVal ) )
      {
      iItemArray.AppendL( listItem );
      CleanupStack::Pop( listItem );
      }
    else
      {
      CleanupStack::PopAndDestroy( listItem );
      }
    }    
  CleanupStack::PopAndDestroy(); // reader
  }

// -----------------------------------------------------------------------------
// CCamSceneListBoxModel::IsSupportedScene
// Check if a specific scene is supported by current product
// -----------------------------------------------------------------------------
// 
TBool CCamSceneListBoxModel::IsSupportedScene( TInt aItem )
  {
  TBool  imageMode = (ECamControllerImage == iMode);
    
  // Scene must be in supported scenes list.
  TBool supported = iController.IsSceneSupported( aItem, imageMode );
  
  // Still, user scene is never supported in user scene base list.
  TBool userException = (ECamSceneUser == aItem && iUserBaseScenes );

  if( supported && !userException )
    {
    return ETrue;
    }
  else
    {
    return EFalse;
    }
  }
  
// -----------------------------------------------------------------------------
// CCamSceneListBoxModel::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamSceneListBoxModel* 
CCamSceneListBoxModel::NewLC( 
    CCamAppController& aController,
    TCamCameraMode aMode, 
    RPointerArray<HBufC>& aSummaryTitleTextArray,
    RPointerArray<HBufC>& aSummaryDescriptionTextArray,
    TBool aUserBaseScenes,
    TRect aListBoxRect )
  {
  CCamSceneListBoxModel* self = 
      new( ELeave ) CCamSceneListBoxModel( aController, 
                                           aMode,
                                           aSummaryTitleTextArray, 
                                           aSummaryDescriptionTextArray,
                                           aUserBaseScenes );
    
  CleanupStack::PushL( self );
  self->ConstructL( aListBoxRect );
  return self;
  }

    
// Destructor
CCamSceneListBoxModel::~CCamSceneListBoxModel()
  {
  }


//  End of File  
