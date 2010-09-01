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


#ifndef CAMINFOLISTBOXMODEL_H
#define CAMINFOLISTBOXMODEL_H

//  INCLUDES
#include "CamCaptureSetupListBoxModel.h"
#include "CamAppController.h"

// CLASS DECLARATION

/**
*  List box model for Camera scenes
*
*  @since 3.0
*/
class CCamInfoListBoxModel : public CCamCaptureSetupListBoxModel
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCamInfoListBoxModel* NewLC( CCamAppController& aController,
        				 					 RPointerArray<HBufC>& aSummaryTitleTextArray,
        				                     RPointerArray<HBufC>& aSummaryDescriptionTextArray,
        				                     TInt aResourceId,
        				                     TRect aListBoxRect );
        
        /**
        * Destructor.
        */
        virtual ~CCamInfoListBoxModel();

  
    private:

        /**
        * C++ default constructor.
        */
        CCamInfoListBoxModel( CCamAppController& aController, 
        				 RPointerArray<HBufC>& aSummaryTitleTextArray,
        				 RPointerArray<HBufC>& aSummaryDescriptionTextArray );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TInt aResourceId, TRect aListBoxRect );

        /**
        * Get settings for all supported scenes
        * @since 3.0
        * @param aResourceId ID of resource definition for scenes
        */
        void GetListboxDataL( TInt aResourceId );
        
   
    private:    // Data
				
		// Array of title descriptions for the summary title lines 
        RPointerArray<HBufC>& iSummaryTitleTextArray;
        // Array of descriptons for the summary description lines
        RPointerArray<HBufC>& iSummaryDescriptionTextArray;
        
    };

#endif      // CAMINFOLISTBOXMODEL_H   
            
// End of File
