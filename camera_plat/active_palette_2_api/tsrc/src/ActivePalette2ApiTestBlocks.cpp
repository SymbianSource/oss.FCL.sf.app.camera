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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32cmn.h>
#include <e32svr.h>
#include <coeaui.h>

#include <bitstd.h> // CFbsBitGc
#include <fbs.h>    // CFbsBitmap
#include <bitdev.h> // CFbsBitmapDevice

#include <StifParser.h>
#include <Stiftestinterface.h>

#include <activepalette2ui.h>
#include <activepalette2factory.h>
#include <activepalette2genericpluginint.h>
#include <activepalette2eventdata.h>
#include <activepalette2itemvisible.h>
#include <activepalette2navigationkeys.h>

#include <ActivePalette2ApiTest.mbg>

#include "ActivePalette2ApiTest.h"
#include "ActivePaletteContainer.h"



// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define LOG(text) iLog->Log(_L(text))
#define LOG1(text, param) iLog->Log(_L(text), param)
#define LOG2(text, param1, param2) iLog->Log(_L(text), param1, param2)
#define LOG3(text, param1, param2, param3) iLog->Log(_L(text), param1, param2, param3)

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

namespace {
    static const TInt KNaviPrevious = EKeyUpArrow;
    static const TInt KNaviNext     = EKeyDownArrow;
    static const TInt KNaviSelect   = EKeyEnter;

    _LIT( KTrueString,  "true"  );
    _LIT( KFalseString, "false" );
    
    _LIT( KIconFileName, "\\resource\\apps\\ActivePalette2ApiTest.mif" );
        
    static const TInt KIconIds[] = {
            EMbmActivepalette2apitestZeroth,
            EMbmActivepalette2apitestFirst,
            EMbmActivepalette2apitestSecond,
            EMbmActivepalette2apitestThird,
            EMbmActivepalette2apitestFourth,
            EMbmActivepalette2apitestFifth,
            EMbmActivepalette2apitestSixth,
            EMbmActivepalette2apitestSeventh,
            EMbmActivepalette2apitestEighth,
            EMbmActivepalette2apitestNinth
    };
    
    static const TInt KMaskIds[] = {            
            EMbmActivepalette2apitestZeroth_mask,
            EMbmActivepalette2apitestFirst_mask,
            EMbmActivepalette2apitestSecond_mask,
            EMbmActivepalette2apitestThird_mask,
            EMbmActivepalette2apitestFourth_mask,
            EMbmActivepalette2apitestFifth_mask,
            EMbmActivepalette2apitestSixth_mask,
            EMbmActivepalette2apitestSeventh_mask,
            EMbmActivepalette2apitestEighth_mask,
            EMbmActivepalette2apitestNinth_mask
    };

    static const TInt KIconCount = sizeof(KIconIds)/sizeof(TInt);
}

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CActivePalette2ApiTest::Delete() 
    {
    // Make sure this is released, although DisposeUi is supposed to be
    // called within the test case.
    delete iContainer;
    iContainer = NULL;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CActivePalette2ApiTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateUi", CActivePalette2ApiTest::CreateUiL ),
        ENTRY( "DisposeUi", CActivePalette2ApiTest::DisposeUiL ),
        ENTRY( "SetNavigationKeys", CActivePalette2ApiTest::SetNavigationKeysL ),
        ENTRY( "ListPlugins", CActivePalette2ApiTest::ListPluginsL ),
        ENTRY( "SetPaletteVisibility", CActivePalette2ApiTest::SetPaletteVisibilityL ),
        ENTRY( "MovePalette", CActivePalette2ApiTest::MovePaletteL ),
        ENTRY( "AddItems", CActivePalette2ApiTest::AddItemsL ),
        ENTRY( "RemoveItems", CActivePalette2ApiTest::RemoveItemsL ),
        ENTRY( "ShowItems", CActivePalette2ApiTest::ShowItemsL ),
        ENTRY( "HideItems", CActivePalette2ApiTest::HideItemsL ),
        ENTRY( "SetCurrentItem", CActivePalette2ApiTest::SetCurrentItemL ),
        ENTRY( "CheckCurrentItem", CActivePalette2ApiTest::CheckCurrentItemL ),
        ENTRY( "CheckItemList", CActivePalette2ApiTest::CheckItemListL ),
        ENTRY( "CheckItemVisibilities", CActivePalette2ApiTest::CheckItemVisibilitiesL ),
        
        ENTRY( "TestOffscreenDrawing", CActivePalette2ApiTest::TestOffscreenDrawingL ),

        ENTRY( "TestKeyPrevious", CActivePalette2ApiTest::TestKeyPreviousL ),
        ENTRY( "TestKeyNext", CActivePalette2ApiTest::TestKeyNextL ),
        ENTRY( "TestKeySelect", CActivePalette2ApiTest::TestKeySelectL ),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::Log
// Get logger instance.
// -----------------------------------------------------------------------------
//
CStifLogger* 
CActivePalette2ApiTest::Log() const
    {
    return iLog;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::CreateUiL
// Create Container instance, add it to the view stack and ask Container 
// to create the Active Palette instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::CreateUiL( CStifItemParser& /*aItem*/ )
    {
    LOG("=> CActivePalette2ApiTest::CreateUiL");
    delete iContainer;
    iContainer = NULL;
    iContainer = new (ELeave) CActivePaletteContainer();
    LOG("<> CActivePalette2ApiTest::CreateUiL 1");
    iContainer->ConstructL(this);
    LOG("<> CActivePalette2ApiTest::CreateUiL 2");
    CCoeEnv::Static()->AppUi()->RegisterViewL(*iContainer);
    LOG("<> CActivePalette2ApiTest::CreateUiL 3");
    CCoeEnv::Static()->AppUi()->AddToViewStackL(*iContainer, iContainer);
    LOG("<> CActivePalette2ApiTest::CreateUiL 4");
    CCoeEnv::Static()->AppUi()->ActivateViewL(iContainer->ViewId());
    LOG("<= CActivePalette2ApiTest::CreateUiL, status");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::DisposeUiL
// Ask Container to dispose the Active Palette and remove the container from
// view stack.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::DisposeUiL( CStifItemParser& /*aItem*/ )
    {
    LOG("=> CActivePalette2ApiTest::DisposeUiL");
    if( iContainer != NULL )
        {
        CCoeEnv::Static()->AppUi()->DeregisterView(*iContainer);
        CCoeEnv::Static()->AppUi()->RemoveFromViewStack(*iContainer, iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    LOG("<= CActivePalette2ApiTest::DisposeUiL");    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::SetNavigationKeysL
// Set the Active Palette navigation keys.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt
CActivePalette2ApiTest::SetNavigationKeysL( CStifItemParser& /*aItem*/ ) 
    {
    LOG3("=> CActivePalette2ApiTest::SetNavigationKeysL - prev[%d] next[%d] select[%d]",
         KNaviPrevious, KNaviNext, KNaviSelect);
    
    // Cover also the TActivePalette2NavigationKeys default constructor.
    TActivePalette2NavigationKeys keys; 
    keys = TActivePalette2NavigationKeys(KNaviPrevious,KNaviNext,KNaviSelect);
    
    // Set navigation keys. 
    iContainer->ActivePalette()->SetNavigationKeys( keys );

    LOG("<= CActivePalette2ApiTest::SetNavigationKeysL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::ListPluginsL
// Ask the available plugins from Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::ListPluginsL( CStifItemParser& /*aItem*/ )
    {
    LOG("=> CActivePalette2ApiTest::ListPluginsL");
    RArray<TUid> list;
    CleanupClosePushL(list);
    
    User::LeaveIfError( iContainer->ActivePalette()->GetAvailablePlugins(list) );
    for( TInt i = 0; i < list.Count(); i++ )
        {
        LOG1("<> CActivePalette2ApiTest::ListPluginsL - available plugin uid[0x%08X]", list[i].iUid );
        }
    CleanupStack::PopAndDestroy(&list);
    LOG("<= CActivePalette2ApiTest::ListPluginsL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::SetPaletteVisibilityL
// Show or hide the whole Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::SetPaletteVisibilityL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <visible> <delay> <animate> <duration>"
    // where
    //     <visible>  = "true" or "false" for whether to show or hide palette.
    //     <delay>    = integer, defining the delay for showing or hiding in [ms]
    //     <animate>  = "true" or "false" for whether to use animation effect
    //     <duration> = integer, defining the animation (if any) duration in [ms]
    LOG("=> CActivePalette2ApiTest::SetPaletteVisibilityL");

    MActivePalette2UI* ap = iContainer->ActivePalette();
    
    TBool visible( false );
    TBool animate( false );
    TInt  delay( 0 );
    TInt  duration( 0 );
    TInt  test( 0 );

    LOG("<> CActivePalette2ApiTest::SetPaletteVisibilityL - read visibility parameter..");
    User::LeaveIfError( GetNextBooleanFromString(aItem, visible) );
    LOG("<> CActivePalette2ApiTest::SetPaletteVisibilityL - read delay parameter..");
    User::LeaveIfError( aItem.GetNextInt(delay) );
    LOG("<> CActivePalette2ApiTest::SetPaletteVisibilityL - read animation parameter..");
    User::LeaveIfError( GetNextBooleanFromString(aItem, animate) );
    LOG("<> CActivePalette2ApiTest::SetPaletteVisibilityL - read duration parameter..");
    User::LeaveIfError( aItem.GetNextInt(duration) );

    LOG("<> CActivePalette2ApiTest::SetPaletteVisibilityL - set animation duration..");
    User::LeaveIfError( ap->SetPaletteVisibilityAnimationDuration(duration) );
    User::LeaveIfError( ap->GetPaletteVisibilityAnimationDuration(test) );
    
    if( test != duration )
        {
        User::Leave(KErrGeneral);
        }

    LOG("<> CActivePalette2ApiTest::SetPaletteVisibilityL - set visibility..");
    User::LeaveIfError( ap->SetPaletteVisibility(visible, animate, delay) );

    LOG("<= CActivePalette2ApiTest::SetPaletteVisibilityL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::MovePaletteL
// Move the Active Palette to given location.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::MovePaletteL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <x> <y>"
    // where
    //     <x> = integer, defining the x coordinate for target location
    //     <y> = integer, defining the y coordinate for target location
    LOG("=> CActivePalette2ApiTest::MovePaletteL");

    MActivePalette2UI* ap = iContainer->ActivePalette();
    
    TInt x;
    TInt y;
    
    LOG("<> CActivePalette2ApiTest::MovePaletteL - read x coordinate..");
    User::LeaveIfError( aItem.GetNextInt(x) );
    LOG("<> CActivePalette2ApiTest::MovePaletteL - read y coordinate..");
    User::LeaveIfError( aItem.GetNextInt(y) );

    LOG2("<> CActivePalette2ApiTest::MovePaletteL - set location to[%d,%d]", x, y);
    ap->LocateTo( TPoint(x,y) );
    iContainer->DrawNow();
    LOG("<> CActivePalette2ApiTest::MovePaletteL - check location..");
    TPoint moved = ap->Location();
    if( moved.iX != x || moved.iY != y )
        {
        User::Leave(KErrGeneral);
        }
    
    LOG("<= CActivePalette2ApiTest::MovePaletteL");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::AddItemsL
// Add items to the Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt
CActivePalette2ApiTest::AddItemsL( CStifItemParser& aItem ) 
    {
    // Define test case as
    //     "instance teststepname <a> <b> <c> ..."
    // where
    //     <a>, <b>, <c> ... = integers, id for an item to be added.
    LOG("=> CActivePalette2ApiTest::AddItemsL");
    MActivePalette2UI* ap = iContainer->ActivePalette();

    _LIT(KTooltipFormat, "Tooltip: %d");
    TBuf<20> buffer;
    int id;
        
    while( aItem.GetNextInt(id) == KErrNone )
        {
        LOG1("<> CActivePalette2ApiTest::AddItemsL - got id[%d]", id);
        buffer.Format(KTooltipFormat, id);

        TAP2GenericPluginParams params( 
                TAP2GenericPluginParams::EGenericCustomResFile,
                KIconFileName,           // Icon file name
                KIconIds[id%KIconCount], // Icon id
                KMaskIds[id%KIconCount], // Mask id
                buffer,                  // Tooltip text
                id,                      // msg id
                0);                      // custom param
        TPckg<TAP2GenericPluginParams> params_buf(params);

        const TBool visibility = true;
        
        TActivePalette2ItemVisible itemVisible(id, visibility);

        if( itemVisible.ItemId()  != id && 
            itemVisible.Visible() != visibility )
            {
            LOG("<> CActivePalette2ApiTest::AddItemsL - id or visibility setting failed!");
            User::Leave(KErrGeneral);
            }
        
        LOG("<> CActivePalette2ApiTest::AddItemsL - install item..");
        ap->InstallItemL(itemVisible,
                         TUid::Uid(KActivePalette2GenericPluginUID),
                         params_buf);

        LOG("<> CActivePalette2ApiTest::AddItemsL - test sending message, for updating icon..");
        TAP2GenericPluginIcon icon(KIconIds[id%KIconCount], KMaskIds[id%KIconCount]);
        TPckgC<TAP2GenericPluginIcon> update_buf(icon);
        LOG("<> CActivePalette2ApiTest::AddItemsL - send the message..");
        User::LeaveIfError( ap->SendMessage( id, EAPGenericPluginChangeIcon, update_buf) );
        }

    // This constructor is exported, although we have no use for it.
    // Try this method also.
    LOG("<> CActivePalette2ApiTest::AddItemsL - test TAP2GenericPluginIcon..");
    TAP2GenericPluginIcon tmp( 101, 102 );
    if( tmp.iIconId != 101 && tmp.iMaskId != 102 )
        {
        LOG("<> CActivePalette2ApiTest::AddItemsL - TAP2GenericPluginIcon constructor error!");
        User::Leave( KErrGeneral);
        }


    LOG("<= CActivePalette2ApiTest::AddItemsL");
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::RemoveItemsL
// Remove given items from the Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::RemoveItemsL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <a> <b> <c> ..."
    // where
    //     <a>, <b>, <c> ... = integers, id for an item to be removed.
    LOG("=> CActivePalette2ApiTest::RemoveItemsL");
    MActivePalette2UI* ap = iContainer->ActivePalette();
    TInt               id = 0;
    while ( aItem.GetNextInt(id) == KErrNone )
        {
        LOG1("<> CActivePalette2ApiTest::RemoveItemsL - removing id[%d]", id );
        User::LeaveIfError( ap->RemoveItem(id) );
        }        
    LOG("<= CActivePalette2ApiTest::RemoveItemsL");
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::ShowItemsL
// Show given Active Palette items. 
// Items must already be added to Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::ShowItemsL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <a> <b> <c> ..."
    // where
    //     <a>, <b>, <c> ... = integers, id for an item to be shown.
    //
    LOG("=> CActivePalette2ApiTest::ShowItemsL");
    MActivePalette2UI* ap = iContainer->ActivePalette();
    TInt               id = 0;
    while ( aItem.GetNextInt(id) == KErrNone )
        {
        LOG1("<> CActivePalette2ApiTest::ShowItemsL - showing id[%d]", id );
        User::LeaveIfError( ap->SetItemVisibility(id, true) );
        }        
    LOG("<= CActivePalette2ApiTest::ShowItemsL");
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::HideItemsL
// Hide given Active Palette items. 
// Items must already be added to Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::HideItemsL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <a> <b> <c> ..."
    // where
    //     <a>, <b>, <c> ... = integers, id for an item to be hidden.
    //
    LOG("=> CActivePalette2ApiTest::HideItemsL");
    MActivePalette2UI* ap = iContainer->ActivePalette();
    TInt               id = 0;
    while ( aItem.GetNextInt(id) == KErrNone )
        {
        LOG1("<> CActivePalette2ApiTest::HideItemsL - hiding id[%d]", id );
        User::LeaveIfError( ap->SetItemVisibility(id, false) );
        }        
    LOG("<= CActivePalette2ApiTest::HideItemsL");
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::SetCurrentItemL
// Set the currently focused Active Palette item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::SetCurrentItemL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <id>"
    // where
    //     <id> = integer, id for an item to be set focused.
    //
    LOG("=> CActivePalette2ApiTest::SetCurrentItemL");
    TInt id = 0;    
    User::LeaveIfError( aItem.GetNextInt(id) );
    LOG1("<> CActivePalette2ApiTest::SetCurrentItemL - focusing id[%d]", id );
    User::LeaveIfError( iContainer->ActivePalette()->SetCurrentItem(id) );
    LOG("<= CActivePalette2ApiTest::SetCurrentItemL");
    return KErrNone;        
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::CheckCurrentItemL
// Check the currently focused Active Palette item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::CheckCurrentItemL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <id>"
    // where
    //     <id> = integer, id of the item that is expected to be focused.
    //
    LOG("=> CActivePalette2ApiTest::CheckCurrentItemL");
    TInt expected = 0;
    TInt current  = 0;
    
    User::LeaveIfError( aItem.GetNextInt(expected) );
    LOG1("<> CActivePalette2ApiTest::CheckCurrentItemL - expecting focused id[%d]", expected );    
    
    User::LeaveIfError( iContainer->ActivePalette()->GetCurrentItem(current) );
    LOG1("<> CActivePalette2ApiTest::CheckCurrentItemL - actually focused id[%d]", current );    

    const TInt status = expected == current ? KErrNone : KErrGeneral;
    LOG1("<= CActivePalette2ApiTest::CheckCurrentItemL - return %d", status);
    return status;
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::CheckItemListL
// Check the items currently contained in Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::CheckItemListL( CStifItemParser& aItem )
    {
    // Define test case as
    //     "instance teststepname <a> <b> <c> ..."
    // where
    //     <a>, <b>, <c> ... = integers, ids of the items that, and only that,
    //                         are expected to be contained in Active Palette.
    //
    LOG("=> CActivePalette2ApiTest::CheckItemListL");
    RArray<TInt> expected;
    CleanupClosePushL(expected);

    LOG("<> CActivePalette2ApiTest::CheckItemListL - get expected items..");
    TInt id = 0;
    while ( aItem.GetNextInt(id) == KErrNone )
        {
        expected.Append(id);
        }

    LOG("<> CActivePalette2ApiTest::CheckItemListL - get current items..");
    RArray<TActivePalette2ItemVisible> current;
    CleanupClosePushL(current);
    User::LeaveIfError( iContainer->ActivePalette()->GetItemList(current) );

    if( current.Count() != expected.Count() )
        {
        LOG2("<> CActivePalette2ApiTest::CheckItemListL - counts do not match, currently[%d] expected[%d]!",
            current.Count(), expected.Count());
        User::Leave(KErrGeneral);
        }
    for( int i = 0; i < current.Count(); i++ ) 
        { 
        if( expected[i] != current[i].ItemId() )
            {
            LOG3("<> CActivePalette2ApiTest::CheckItemListL - item at[%d] has id[%d] expected id[%d] !",
                 i, current[i].ItemId(), expected[i]);
            User::Leave(KErrGeneral);
            }
        }

    LOG("<> CActivePalette2ApiTest::CheckItemListL - set current items again..");
    User::LeaveIfError( iContainer->ActivePalette()->SetItemList(current) );

    CleanupStack::PopAndDestroy(&current);    
    CleanupStack::PopAndDestroy(&expected);
    
    LOG("<= CActivePalette2ApiTest::CheckItemListL"); 
    return KErrNone;   
    }

// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::CheckItemVisibilitiesL
// Check the items currently visible in Active Palette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::CheckItemVisibilitiesL( CStifItemParser& aItem )
    {
    // In test case definition, use the following format
    // "instance teststepname <id1> <visibility> <id2> <visibility> ..."
    // Where 
    //     <id1>, <id2> ... = integer, defining the id for item to be checked.
    //     <visibility>     = "true" or "false", defines the expected visibility 
    //                        of item with the id number given just before.
    //
    // Note: All items currently contained in Active Palette must be mentioned.
    //
    LOG("=> CActivePalette2ApiTest::CheckItemVisibilitiesL");

    MActivePalette2UI* ap = iContainer->ActivePalette();

    LOG("<> CActivePalette2ApiTest::CheckItemVisibilitiesL - get expected items..");
    TInt     count   = 0;
    TInt     id      = 0;
    TBool    expectVisible = false;
    TBool    isVisible     = false;

    
    while( aItem.GetNextInt(id) == KErrNone )
        {
        LOG1("<> CActivePalette2ApiTest::CheckItemVisibilitiesL - get expected visibility for id[%d]", id);
        User::LeaveIfError( GetNextBooleanFromString(aItem, expectVisible) );        

        LOG1("<> CActivePalette2ApiTest::CheckItemVisibilitiesL - get current visibility for id[%d]", id);
        User::LeaveIfError( ap->GetItemVisibility(id, isVisible) );
        if( isVisible != expectVisible )
            {
            LOG("<> CActivePalette2ApiTest::CheckItemVisibilitiesL - visibilities do not match!");
            User::Leave(KErrGeneral);
            }

        // Count the items so we can make sure all are checked.
        count++;
        }

    // Check now that all items were checked.
    LOG("<> CActivePalette2ApiTest::CheckItemVisibilitiesL - get current items..");
    RArray<TActivePalette2ItemVisible> current;
    CleanupClosePushL(current);
    User::LeaveIfError( iContainer->ActivePalette()->GetItemList(current) );
    if( current.Count() != count ) 
        {
        LOG2("<> CActivePalette2ApiTest::CheckItemVisibilitiesL - counts do not match, currently[%d] expected[%d]!",
            current.Count(), count);
        User::Leave(KErrGeneral);        
        }
    CleanupStack::PopAndDestroy(&current);        

    LOG("<= CActivePalette2ApiTest::CheckItemVisibilitiesL");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::TestOffscreenDrawingL
// Test the use of offscreen buffer for Active Palette rendering.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::TestOffscreenDrawingL( CStifItemParser& /*aItem*/ )
    {
    LOG("=> CActivePalette2ApiTest::TestOffscreenDrawingL");
    MActivePalette2UI* ap = iContainer->ActivePalette();

    LOG("<> CActivePalette2ApiTest::TestOffscreenDrawingL - create bitmap..");
    CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap();
    CleanupStack::PushL( bitmap );
    User::LeaveIfError( bitmap->Create( iContainer->Size(), EColor16MU ) );
    
    LOG("<> CActivePalette2ApiTest::TestOffscreenDrawingL - create device..");
    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( bitmap );
    CleanupStack::PushL( device );

    LOG("<> CActivePalette2ApiTest::TestOffscreenDrawingL - create context..");
    CFbsBitGc* context = NULL;
    User::LeaveIfError( device->CreateContext( context ) );
    CleanupStack::PushL( context );
    
    LOG("<> CActivePalette2ApiTest::TestOffscreenDrawingL - test rendering..");
    ap->SetGc( context );
    ap->RenderActivePalette( TRect( TPoint(0,0), bitmap->SizeInPixels() ) );
    ap->SetGc();
    
    CleanupStack::PopAndDestroy( context );
    CleanupStack::PopAndDestroy( device  );
    CleanupStack::PopAndDestroy( bitmap  );
    
    LOG("<= CActivePalette2ApiTest::TestOffscreenDrawingL");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::TestKeyPreviousL
// Test handling key event for moving to previous item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::TestKeyPreviousL( CStifItemParser& /*aItem*/ ) 
    {
    LOG("=> CActivePalette2ApiTest::TestKeyPreviousL");
    TKeyEvent event;
    event.iCode     = KNaviPrevious;
    event.iScanCode = KNaviPrevious;
    
    TKeyResponse response = iContainer->OfferKeyEventL(event, EEventKey);
    LOG("<= CActivePalette2ApiTest::TestKeyPreviousL");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::TestKeyNextL
// Test handling key event for moving to next item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::TestKeyNextL( CStifItemParser& /*aItem*/ ) 
    {
    LOG("=> CActivePalette2ApiTest::TestKeyNextL");
    TKeyEvent event;
    event.iCode     = KNaviNext;
    event.iScanCode = KNaviNext;
    
    TKeyResponse response = iContainer->OfferKeyEventL(event, EEventKey);
    LOG("<= CActivePalette2ApiTest::TestKeyNextL");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::TestKeySelectL
// Test handling key event for selecting item.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePalette2ApiTest::TestKeySelectL( CStifItemParser& /*aItem*/ ) 
    {
    LOG("=> CActivePalette2ApiTest::TestKeySelectL");
    TKeyEvent event;
    event.iCode     = KNaviSelect;
    event.iScanCode = KNaviSelect;
    event.iRepeats  = 0;
    
    TKeyResponse response = iContainer->OfferKeyEventL(event, EEventKey);


    LOG("<= CActivePalette2ApiTest::TestKeySelectL");
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::GetNextBooleanFromString
// Helper method for reading a string with expected values of
// "true" and "false" and converting them to a TBool value.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt
CActivePalette2ApiTest::GetNextBooleanFromString( CStifItemParser& aItem, TBool& aValue ) 
    {
    LOG("=> CActivePalette2ApiTest::GetNextBooleanFromString");

    TPtrC content;    
    TInt status = aItem.GetNextString(content);
        
    if( status == KErrNone ) 
        {
        LOG("<> CActivePalette2ApiTest::GetNextBooleanFromString - about to trace string..");
        LOG1("<> CActivePalette2ApiTest::GetNextBooleanFromString - got [%S]", &content );
        if( content.Compare(KTrueString) == 0 )
            {
            aValue = ETrue;
            }
        else if( content.Compare(KFalseString) == 0 )
            {
            aValue = EFalse;
            }
        else
            {
            User::Panic(_L("Test case not valid!"), 1);
            }
        }
    
    LOG("<= CActivePalette2ApiTest::GetNextBooleanFromString");
    return status;
    }


// -----------------------------------------------------------------------------
// CActivePalette2ApiTest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CActivePalette2ApiTest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
