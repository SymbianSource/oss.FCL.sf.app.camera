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



// INCLUDE FILES
#include "CameraappTestBitmapManager.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
const TInt KMaxBitmaps = 3;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCameraappTestBitmapManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCameraappTestBitmapManager* CCameraappTestBitmapManager::NewL()
    {
    CCameraappTestBitmapManager* self = new( ELeave ) CCameraappTestBitmapManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CCameraappTestBitmapManager::~CCameraappTestBitmapManager()
    {
    iBitmaps.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCameraappTestBitmapManager::SnapshotImage
// Get post-exposure snapshot bitmap, if available.
//
// Returns: pointer to post-exposure snapshot bitmap. Does not transfer ownership.
//          May be NULL if no image available.
// 
// ---------------------------------------------------------------------------
//
CFbsBitmap& CCameraappTestBitmapManager::Bitmap( TInt aBitmapIndex ) const
    {
    if ( aBitmapIndex < iBitmaps.Count() )
        {
        return *( iBitmaps[aBitmapIndex] );
        }
    else 
        {
        return *( iBitmaps[iBitmaps.Count() -  1] );
        }
    }

CFbsBitmap& CCameraappTestBitmapManager::NextBitmap() 
    {
    IncrementBitmapIndex();
    return *(iBitmaps[iBitmapIndex]);
    }

// -----------------------------------------------------------------------------
// CCameraappTestBitmapManager::CCameraappTestBitmapManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCameraappTestBitmapManager::CCameraappTestBitmapManager()
: iBitmapIndex( -1 )
    {
    }

// -----------------------------------------------------------------------------
// CCameraappTestBitmapManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCameraappTestBitmapManager::ConstructL()
    {
    // load test bitmaps
    _LIT( bitmapFile, "z:\\system\\apps\\cameraapp\\DummyViewFinder.mbm" );
    
    for ( TInt i = 0 ; i < KMaxBitmaps; i++ )
        {
        CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap;
        CleanupStack::PushL( bitmap );

        User::LeaveIfError( bitmap->Load( bitmapFile, i ) );
        User::LeaveIfError( iBitmaps.Append( bitmap ) );
        CleanupStack::Pop( bitmap ); 
        }
    }

void CCameraappTestBitmapManager::IncrementBitmapIndex()
    {
    iBitmapIndex = ++iBitmapIndex % iBitmaps.Count();
    }

//  End of File  
