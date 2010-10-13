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
* Description:  Control for containing the NaviPane
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*/


// INCLUDE FILES
#include <eiklabel.h>
#include <eikapp.h>		// For CEikApplication
#include <eikenv.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <AknConsts.h>
#include <StringLoader.h> // StringLoader
#include <e32base.h>
#include <barsread.h>    // TResourceReader
#include <AknBidiTextUtils.h>
#include <cameraapp.mbg>
#include <AknLayoutFont.h>
#include "CamApplicationPane.h"
#include "CamNaviCounter.h"
#include "CamNaviProgressBar.h"
#include "CamAppUi.h"
#include "CamUtility.h"

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

#include "CamLogger.h"

#include "AknsBasicBackgroundControlContext.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------
// CCamApplicationPane::NewL
// Factory construction function
// ---------------------------------------------------------
//
CCamApplicationPane* CCamApplicationPane::NewL( CCamAppController& aController )
    {
    CCamApplicationPane* self = new( ELeave ) CCamApplicationPane( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCamApplicationPane::~CCamApplicationPane()
  {    
  PRINT( _L("Camera => ~CCamApplicationPane") );
  iNaviCounter = NULL;        // Not owned     
  iNaviProgressBar = NULL;    // Not owned
  PRINT( _L("Camera <= ~CCamApplicationPane") );
  }


// ---------------------------------------------------------
// CCamApplicationPane::CCamApplicationPane
// C++ constructor
// ---------------------------------------------------------
//
CCamApplicationPane::CCamApplicationPane( CCamAppController& aController ) : iController( aController )
    {
    }

// ---------------------------------------------------------
// CCamApplicationPane::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamApplicationPane::ConstructL()
    {
    iAppPaneLayoutRect.LayoutRect( Rect(), ROID(R_CAM_APPLICATION_PANE_RECT_ID));                 
    SetRect( iAppPaneLayoutRect.Rect() );               
    
    iPaneInUse = EAppPaneUndefined;
    }                

// ---------------------------------------------------------
// CCamApplicationPane::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCamApplicationPane::SizeChanged()
    {    
    }

// ---------------------------------------------------------
// CCamApplicationPane::Draw
// Draw the control
// ---------------------------------------------------------
//
void CCamApplicationPane::Draw( const TRect& /* aRect */ ) const
    {
    }
    
    
// ---------------------------------------------------------
// CCamApplicationPane::CountComponentControls
// Returns the number of component controls to display
// ---------------------------------------------------------
//   
TInt CCamApplicationPane::CountComponentControls() const
	{
	// If an active Navi Pane is defined, this is one control to show
	if ( iPaneInUse != EAppPaneUndefined ) 	
	    return 1;
	else 
	    return 0;
	}

// ----------------------------------------------------
// CCamApplicationPane::ComponentControl
// Return requested control
// ----------------------------------------------------
//
CCoeControl* CCamApplicationPane::ComponentControl( TInt /*aIndex*/ ) const
	{    
	CCoeControl* con = NULL;
	switch ( iPaneInUse )
	    {
	    case EAppPaneCounter:
	        {
	        con = iNaviCounter;
	        break;	        
	        }
	        
	    case EAppPaneProgress:
	        {
	        con = iNaviProgressBar;
	        break;	    	        
	        }
	        
	    case EAppPaneUndefined:
	    default:
	        {
	        break;        
	        }	        
	    }	
	return con;	
	}
	


// ----------------------------------------------------
// CCamApplicationPane::ShowCounter
// Shows the NaviCounter control in the Application Pane
// ----------------------------------------------------
//    
void CCamApplicationPane::ShowCounter()
    {    
    iPaneInUse = EAppPaneCounter;
    iNaviCounter->MakeVisible( ETrue );
    iNaviProgressBar->MakeVisible( EFalse );        
    }


// ----------------------------------------------------
// CCamApplicationPane::ShowProgress
// Shows the NaviProgress control in the Application Pane
// ----------------------------------------------------
//       
void CCamApplicationPane::ShowProgress()
    {    
    iPaneInUse = EAppPaneProgress;    
    iNaviProgressBar->MakeVisible( ETrue );            
    iNaviCounter->MakeVisible( EFalse );
    }
    

// ----------------------------------------------------
// CCamApplicationPane::InitialisePanesL
// Sets the owned panes to a known good state (all invisible)
// ----------------------------------------------------
//      
void CCamApplicationPane::InitialisePanesL()
    {      
    iNaviCounter = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->NaviCounter();
    iNaviCounter->SetMopParent( this );
    iNaviCounter->SetContainerWindowL( *this );  
    iNaviCounter->SetRect( Rect() );            
    iNaviCounter->MakeVisible( EFalse );
    
    iNaviProgressBar = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->NaviProgressBar();
    iNaviProgressBar->SetMopParent( this );
    iNaviProgressBar->SetContainerWindowL( *this );            
    iNaviProgressBar->SetRect( Rect() );            
    iNaviProgressBar->MakeVisible( EFalse );    
    
    if ( iPaneInUse == EAppPaneCounter )
        {
        iNaviCounter->MakeVisible( ETrue );
        }
    if ( iPaneInUse == EAppPaneProgress )
        {
        iNaviProgressBar->MakeVisible( ETrue );    
        }    
    }
        
// ----------------------------------------------------
// CCamApplicationPane::SetBurstModelL
// Sets the burst model to use if app pane showing burst 
// filenames
// ----------------------------------------------------
//           
void CCamApplicationPane::SetBurstModelL( CCamBurstThumbnailGridModel* aModel ) 
    {
    iNaviCounter->SetBurstModelL( aModel );
    }


// ----------------------------------------------------
// CCamApplicationPane::SetCaptureMode
// Sets the capture mode the pane should indicate
// ----------------------------------------------------
//              
void CCamApplicationPane::SetCaptureMode( TCamCameraMode aMode )
  {
  TRAP_IGNORE( ignore, iNaviCounter->SetCaptureModeL( aMode ) );
  }
    


// End of File


