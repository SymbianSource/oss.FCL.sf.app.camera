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





#ifndef __CAMBMPROTATORAO_H
#define __CAMBMPROTATORAO_H

//  INCLUDES
#include <bitmaptransforms.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Utility class to help in the rotating of bitmaps (for Zoom Pane)
*
*  @since 3.0
*/
class CCamBmpRotatorAo : public CActive
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
        static CCamBmpRotatorAo* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CCamBmpRotatorAo();

    public: // New functions        
        /**
        * Rotates the specified bitmap by the supplied angle.  
        * Note that the original bitmap will be overwritten
        * @since 3.0
        * @param aBitmap The bitmap to rotate
        * @param aRotation The angle to rotate the bitmap        
        */
        void AddToQueueL( CFbsBitmap* aBitmap, CBitmapRotator::TRotationAngle aRotation );        

        /**
        * Starts the next rotation task in the queue (if there is one)
        * @since 3.0
        */
        void StartNextRotate(); 

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
        CCamBmpRotatorAo();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since 3.0
        */
        void ConstructL();

    private:    // Data
        // List of outstanding rotation tasks to perform
        RPointerArray <CRotateTask> iQueue;    

        // ICL utility class that actually does the bitmap rotation
        CBitmapRotator* iRotator;         
    };

#endif      // __CAMBMPROTATORAO_H
            
// End of File
