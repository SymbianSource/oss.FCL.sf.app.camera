/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: STIF testclass declaration
*
*/

#ifndef ACTIVEPALETTE2APITEST_H
#define ACTIVEPALETTE2APITEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KActivePalette2ApiTestLogPath, "\\logs\\testframework\\ActivePalette2ApiTest\\" ); 
// Log file
_LIT( KActivePalette2ApiTestLogFile, "ActivePalette2ApiTest.txt" ); 
_LIT( KActivePalette2ApiTestLogFileWithTitle, "ActivePalette2ApiTest_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CActivePalette2ApiTest;
class CActivePaletteContainer;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CActivePalette2ApiTest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CActivePalette2ApiTest) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CActivePalette2ApiTest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CActivePalette2ApiTest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        CStifLogger* Log() const;

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CActivePalette2ApiTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */
        virtual TInt CreateUiL( CStifItemParser& aItem );
        virtual TInt DisposeUiL( CStifItemParser& aItem );
        virtual TInt SetNavigationKeysL( CStifItemParser& aItem );
        virtual TInt ListPluginsL( CStifItemParser& aItem );
        
        virtual TInt SetPaletteVisibilityL( CStifItemParser& aItem );
        virtual TInt MovePaletteL( CStifItemParser& aItem );
        
        virtual TInt AddItemsL( CStifItemParser& aItem );
        virtual TInt RemoveItemsL( CStifItemParser& aItem );
        virtual TInt ShowItemsL( CStifItemParser& aItem );
        virtual TInt HideItemsL( CStifItemParser& aItem );
        virtual TInt SetCurrentItemL( CStifItemParser& aItem );
        virtual TInt CheckCurrentItemL( CStifItemParser& aItem );
        virtual TInt CheckItemListL( CStifItemParser& aItem );
        virtual TInt CheckItemVisibilitiesL( CStifItemParser& aItem );
        
        virtual TInt TestOffscreenDrawingL( CStifItemParser& aItem );
        
        virtual TInt TestKeyPreviousL( CStifItemParser& aItem );
        virtual TInt TestKeyNextL( CStifItemParser& aItem );
        virtual TInt TestKeySelectL( CStifItemParser& aItem );
        
        TBool GetNextBooleanFromString( CStifItemParser& aItem, TBool& aValue );         

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;
        CActivePaletteContainer* iContainer;

        // Reserved pointer for future extension
        //TAny* iReserved;
        
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
    };

#endif      // ACTIVEPALETTE2APITEST_H

// End of File
