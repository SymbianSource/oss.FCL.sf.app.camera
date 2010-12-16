/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class to rotate bitmaps*
*/





#ifndef __CAMSNAPSHOTROTATOR_H
#define __CAMSNAPSHOTROTATOR_H

//  INCLUDES
#include <bitmaptransforms.h>

#include "CamSyncRotatorAo.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CCamAppController;

/**
*  Utility class to help in the rotating of bitmaps (for post-capture snapshot)
*
*  @since 5.1
*/
class CCamSnapShotRotator : public CBase, public MBitmapRotationObserver
    {
 
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCamSnapShotRotator* NewL( CCamAppController& aController,
                CCamSyncRotatorAo& aRotatorAo );
        
        /**
        * Destructor.
        */
        virtual ~CCamSnapShotRotator();

    public: // New functions      

        /**
        * C++ default constructor.
        * @since 5.1
        * @param aBitmap The bitmap to rotate
        */
        void RotateL( CFbsBitmap* aBitmap );

        /**
        * Indicates rotation process state.
        * @since 5.1
        * @return ETrue if image rotation is in progress
        */
        TBool IsActive();
            
        /**
        * From MBitmapRotationObserver
        * Notification of bitmap rotation complete events
        * @param aErr KErrNone if successful
        */
        void RotationCompleteL( TInt aErr ); 

    private:

        /**
        * C++ default constructor.
        * @since 5.1
        */
        CCamSnapShotRotator( CCamAppController& aController,
                CCamSyncRotatorAo& aRotatorAo );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 5.1
        */
        void ConstructL();

    private:    // Data
  
        CCamAppController& iController; // not owned
        CFbsBitmap* iRotatedSnapshot;   // owned
        CCamSyncRotatorAo& iRotatorAo;  // not owned
        
    };

#endif      // __CAMSNAPSHOTROTATOR_H
            
// End of File
