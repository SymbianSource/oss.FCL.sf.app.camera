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
* Description:  Scenes list box model*
*/


#ifndef CAMSCENELISTBOXMODEL_H
#define CAMSCENELISTBOXMODEL_H

//  INCLUDES
#include "CamCaptureSetupListBoxModel.h"
#include "CamAppController.h"

// CLASS DECLARATION

class CCamAppController;

/**
*  List box model for Camera scenes
*
*  @since 3.0
*/
class CCamSceneListBoxModel : public CCamCaptureSetupListBoxModel
  {
  public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CCamSceneListBoxModel* NewLC( CCamAppController& aController,
                                         TCamCameraMode aMode,
                                         RPointerArray<HBufC>& aSummaryTitleTextArray,
                                         RPointerArray<HBufC>& aSummaryDescriptionTextArray,
                                         TBool aUserBaseScenes,
                                         TRect aListBoxRect );
      
    /**
    * Destructor.
    */
    virtual ~CCamSceneListBoxModel();


  private:

    /**
    * C++ default constructor.
    */
    CCamSceneListBoxModel( CCamAppController& aController, 
                           TCamCameraMode aMode,
                           RPointerArray<HBufC>& aSummaryTitleTextArray,
                           RPointerArray<HBufC>& aSummaryDescriptionTextArray,
                           TBool aUserBaseScenes );

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( TRect aListBoxRect );
        
    /**
    * Select the resource to use and load listbox data
    * from the selected resource.
    */
    void GetDataFromResourceL();
        
    /**
    * Get settings for all supported scenes
    * @since 3.0
    * @param aResourceId ID of resource definition for scenes
    */
    void GetScenesDataL( TInt aResourceId );

    /**
    * Is specified scene suported for current product
    * @since 3.0
    * @param aSceneValue Identifier for scene (i.e ECamSceneAuto)
    * @return ETrue if scene is supported by current product
    */
    TBool IsSupportedScene( TInt aSceneValue );


  private:    // Data

    // capture mode (still/video)
    TCamCameraMode iMode;

    // Array of title descriptions for the summary title lines 
    RPointerArray<HBufC>& iSummaryTitleTextArray;
    // Array of descriptons for the summary description lines
    RPointerArray<HBufC>& iSummaryDescriptionTextArray;

    // Whether we are displaying the user scene base scene list or not.
    TBool iUserBaseScenes;
    CCamAppController& iController;
  };

#endif      // CAMSCENELISTBOXMODEL_H   
            
// End of File
