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
* Description:  Array of user scene setup items.*
*/


#ifndef CAMUSERSCENESETUPITEMARRAY_H
#define CAMUSERSCENESETUPITEMARRAY_H

// INCLUDES
#include <bamdesca.h>
#include "CamUserSceneSetupListItem.h"
   
// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CCamUserSceneSetupItemArray  container control class.
*  
*/
class CCamUserSceneSetupItemArray : public CArrayPtrFlat<CCamUserSceneSetupListItem>,
                                    public MDesCArray //, public MEikListBoxObserver, public MCoeControlObserver
    {
    public: // Constructors and destructor

	    /**
	    * C++ Constructor
	    * @param	Number of items to initialise array size with.
        * @since 2.8
	    */
        CCamUserSceneSetupItemArray( TInt aGranularity );

	    /**
	    * 2nd phase Constructor
        * @since 2.8
	    */
        void ConstructL();
        /**
        * Destructor.
        * @since 2.8
        */
        ~CCamUserSceneSetupItemArray();

    public: // New functions

    public: // Functions from base class MDesC16Array
	    /**
	    * Method by which the CAknSetStyleListBox accesses the number of items in the array
	    * @return	number of items in the array
        * @since 2.8
	    */ 
	    TInt MdcaCount() const;

	    /**
	    * Method by which the setting item list accesses the text for the aIndex-th item in the 
	    * array.
	    *
	    * @param	aIndex	index at which to access the listbox text
	    * @return	Descriptor for this item to be used by the listbox.
        * @since 2.8
	    */
	    TPtrC16 MdcaPoint( TInt aIndex ) const;

    private: // Constructors

     
    private: //data
    };

#endif // CAMUSERSCENESETUPITEMARRAY_H

// End of File
