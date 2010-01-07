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
* Description:  Utility class to rotate bitmaps*
*/





#ifndef __CamSyncRotatorAo_H
#define __CamSyncRotatorAo_H

//  INCLUDES
#include <bitmaptransforms.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class MBitmapRotationObserver
    {
public: 
    /**
    * Mixin interface to be implemented by any object wishing to receive 
    * notification of rotation complete events.
    * @since 3.0
    * @param aErr KErrNone if successful, on error other Epoc codes possible
    */
    virtual void RotationCompleteL( TInt aErr ) = 0 ;
    };


/**
*  Utility class to help in the rotating of bitmaps (for post-capture snapshot)
*
*  @since 3.0
*/
class CCamSyncRotatorAo : public CActive
    {
    private:
        // This structure stores the details of a rotation task.
        // Used only internally, the bitmap is *not* owned by this
        // class, and will be overwritten by the rotation operation.        
        class CRotateTask : public CBase
            {        
        public:
            // The source bitmap to be rotated (and overwritten)
            CFbsBitmap* iBitmap;

            // The angle to rotate the bitmap by
            CBitmapRotator::TRotationAngle iAngle;
            };

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCamSyncRotatorAo* NewL(MBitmapRotationObserver& aObserver);
        
        /**
        * Destructor.
        */
        virtual ~CCamSyncRotatorAo();

    public: // New functions        
        /**
        * Rotates the specified bitmap by the supplied angle.  
        * Note that the original bitmap will be overwritten
        * @since 3.0
        * @param aBitmap The bitmap to rotate
        * @param aRotation The angle to rotate the bitmap        
        */
        void RotateL( CFbsBitmap* aBitmap, CBitmapRotator::TRotationAngle aRotation );        

    public: // Functions from base classes
        /**
        * Called when a rotate operation has completed
        * @since 3.0        
        */
        void RunL();
           

        /**
        * Called to cancel an outstanding operation (eg when class is being destroyed)
        * @since 3.0       
        */
        void DoCancel();
        
    private:

        /**
        * C++ default constructor.
        * @since 3.0
        */
        CCamSyncRotatorAo(MBitmapRotationObserver& aObserver);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 3.0
        */
        void ConstructL();

    private:    // Data
    
        // ICL utility class that actually does the bitmap rotation
        CBitmapRotator* iRotator;        
        
        MBitmapRotationObserver& iObserver;
    };

#endif      // __CamSyncRotatorAo_H
            
// End of File
