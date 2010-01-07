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


#ifndef CAMINFOLISTBOX_H
#define CAMINFOLISTBOX_H

//  INCLUDES
#include "CamCaptureSetupListBox.h"
#include "CamAppController.h"

// CLASS DECLARATION

/**
*  List box for camera scenes.
*  @since 3.0
*/
class CCamInfoListBox : public CCamCaptureSetupListBox
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CCamInfoListBox();
        
        /**
        * C++ default constructor.
        */
        CCamInfoListBox( MCamSettingValueObserver* aObserver,
        					CCamAppController& aController);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aParent Parent control (i.e. Shooting mode container)
        * @param aSummaryTitleTextArray - array of shooting mode (scene) titles
        * @param aSummaryDescriptionTextArray - array of shooting mode (scene) descriptions
        */
        void ConstructL( CCamAppController& aController,
        				 const CCoeControl* aParent,
        				 RPointerArray<HBufC>& aSummaryTitleTextArray,
        				 RPointerArray<HBufC>& aSummaryDescriptionTextArray,
        				 TInt aResourceId );
    
    private:
        
		// Scenes supported by product
		RArray<TInt> iSupportedScenes;
		
		// camera mode (still/video)
		TCamCameraMode iMode;
    };

#endif      // CAMINFOLISTBOX_H   
            
// End of File
