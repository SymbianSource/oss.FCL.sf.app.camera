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



#ifndef CAMSCENELISTBOX_H
#define CAMSCENELISTBOX_H

//  INCLUDES
#include "CamCaptureSetupListBox.h"
#include "CamAppController.h"


// CLASS DECLARATION

/**
*  List box for camera scenes.
*  @since 3.0
*/
class CCamSceneListBox : public CCamCaptureSetupListBox
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CCamSceneListBox();
        
        /**
        * C++ default constructor.
        */
        CCamSceneListBox( TCamCameraMode aMode, MCamSettingValueObserver* aObserver,
        					CCamAppController& aController);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aParent Parent control (i.e. Shooting mode container)
        * @param aSummaryTitleTextArray - array of shooting mode (scene) titles
        * @param aSummaryDescriptionTextArray - array of shooting mode (scene) descriptions
        * @param aUserBaseScenes - Whether we wish to display the User Scene Base Scene List
        */
        void ConstructL( CCamAppController& aController,
        				 const CCoeControl* aParent,
        				 RPointerArray<HBufC>& aSummaryTitleTextArray,
        				 RPointerArray<HBufC>& aSummaryDescriptionTextArray,
        				 TBool aUserBaseScenes );
    
    private:
        
		// Scenes supported by product
		RArray<TInt> iSupportedScenes;
		
		// capture mode (still/video)
		TCamCameraMode iMode;
    };

#endif      // CAMSCENELISTBOX_H   
            
// End of File
