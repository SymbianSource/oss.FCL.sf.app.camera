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
* Description:  Dummy controller class for Cameraapp Unit Tests*
*/




#ifndef CAMERAAPPTESTBITMAPMANAGER_H
#define CAMERAAPPTESTBITMAPMANAGER_H

//  INCLUDES
#include <fbs.h>

// CONSTANTS
_LIT( KTestData, "This is the test data" );

// MACROS
//#define ?macro ?macro_def

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* Handle intercommunication between camcorder UI and engine.
*
*  @since 2.8
*/
class CCameraappTestBitmapManager : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CCameraappTestBitmapManager* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CCameraappTestBitmapManager();
 
    public: // New functions

        CFbsBitmap& Bitmap( TInt aBitmapIndex ) const;
        CFbsBitmap& NextBitmap();

    protected:  // New functions

    private:

        /**
        * C++ default constructor.
        */
        CCameraappTestBitmapManager();

        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.1
        */
        void ConstructL();

        void IncrementBitmapIndex();

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        // Test bitmaps
        RPointerArray<CFbsBitmap> iBitmaps;
        // Index into test bitmap array
        TInt iBitmapIndex;
    };

#endif      // CAMERAAPPTESTBITMAPMANAGER_H
            
// End of File
