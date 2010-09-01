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
* Description:  Class for displaying the title in the Status Pane*
*/



// INCLUDE FILES
#include <AknsControlContext.h>
#include <Barsread.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknsSkinInstance.h>
#include <AknsBasicBackgroundControlContext.h>

#include "CamPostCaptureInfoDisplay.h"
#include "CamTitlePane.h"

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TAknsItemID KNaviPaneMajorColour = KAknsIIDQsnTextColors;
const TInt KNaviPaneMinorColour = EAknsCIQsnTextColorsCG1;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamTitlePane::CCamTitlePane
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamTitlePane::CCamTitlePane()
    {
    }

// -----------------------------------------------------------------------------
// CCamTitlePane::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamTitlePane::ConstructL()
    {
    // Read the resource to get pane position information    
    iTitlePaneLayoutRect.LayoutRect( Rect(), ROID(R_CAM_TITLE_PANE_RECT_ID));                  
    SetRect( iTitlePaneLayoutRect.Rect() );
    }

// -----------------------------------------------------------------------------
// CCamTitlePane::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamTitlePane* CCamTitlePane::NewL()
    {
    CCamTitlePane* self = new( ELeave ) CCamTitlePane;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CCamTitlePane::~CCamTitlePane()
  {
  PRINT( _L("Camera => ~CCamTitlePane" ))  
  delete iTextHbuf;
  PRINT( _L("Camera <= ~CCamTitlePane" ))  
  }
    

// -----------------------------------------------------------------------------
// CCamTitlePane::SetFromResourceL
// Sets the title with the specified resource string
// -----------------------------------------------------------------------------
//
void CCamTitlePane::SetFromResourceL( TInt aResource )
    {
    TResourceReader reader; 
    iEikonEnv->CreateResourceReaderLC( reader, aResource );                
    
    if ( iTextHbuf != NULL )
        {
        delete iTextHbuf;
        iTextHbuf = NULL;
        }
    iTextHbuf = reader.ReadHBufC16L();
    CleanupStack::PopAndDestroy();  // resource reader
    DrawDeferred();
    }



// -----------------------------------------------------------------------------
// CCamTitlePane::SetTextL
// Sets the title with the specified string
// -----------------------------------------------------------------------------
//
void CCamTitlePane::SetTextL( const TDesC& aText )
    {
    if ( iTextHbuf != NULL )
        {
        delete iTextHbuf;
        iTextHbuf = NULL;
        }
    iTextHbuf = aText.AllocL();
    DrawDeferred();
    }
    
    
// ---------------------------------------------------------
// CCamTitlePane::Draw
// Draw control
// ---------------------------------------------------------
//
void CCamTitlePane::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc=SystemGc();

    // Draw the background:
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skin, cc, this, gc, Rect() );
    
    // Set drawing colour from layout information
    TRgb color = iTitleTextLayout.Color();
    
    // ...Color is not updated if it not found from the skin
    AknsUtils::GetCachedColor( skin, color,
        KNaviPaneMajorColour, KNaviPaneMinorColour );

    if ( iTextHbuf )
        {
        iTitleTextLayout.DrawText( gc, iTextHbuf->Des(), ETrue, color );             
        }    
    }
    

// ---------------------------------------------------------
// CCamTitlePane::SizeChanged
// Called by framework when the component size is changed
// ---------------------------------------------------------
//
void CCamTitlePane::SizeChanged()
    {
    if ( Rect().Size() != TSize( 0, 0 ) )
        {        
        if ( !AknLayoutUtils::LayoutMirrored() )
            {
            iTitleTextLayout.LayoutText( Rect(), ROID(R_CAM_TITLE_TEXT_LAYOUT_ID));
            }
        else
            {
            iTitleTextLayout.LayoutText( Rect(), ROID(R_CAM_TITLE_TEXT_LAYOUT_AH_ID));
            }
        }    
    }
    







//  End of File  
