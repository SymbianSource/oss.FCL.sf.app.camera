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
* Description:  Control for displaying remaining images/videos in Navi Pane
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
*
*/


// INCLUDE FILES
#include <AknsUtils.h>
#include <e32base.h>
#include "CamNaviProgressBarControl.h"
#include "CamNaviProgressBarModel.h"
#include "CamAppUi.h"
#include "CamUtility.h"

// CONSTANTS

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------
// CCamNaviProgressBarControl::NewL
// Factory construction function
// ---------------------------------------------------------
//
CCamNaviProgressBarControl* CCamNaviProgressBarControl::NewL( CCamNaviProgressBarModel& aModel )
    {
    CCamNaviProgressBarControl* self = new( ELeave ) CCamNaviProgressBarControl( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCamNaviProgressBarControl::~CCamNaviProgressBarControl()
    {
    iModel.DeregisterObserver( this );
    }

// ---------------------------------------------------------
// CCamNaviProgressBarControl::CCamNaviProgressBarControl
// C++ constructor
// ---------------------------------------------------------
//
CCamNaviProgressBarControl::CCamNaviProgressBarControl(CCamNaviProgressBarModel& aModel )
    : iModel ( aModel ),
    iActive ( EFalse )
    {
    }

// ---------------------------------------------------------
// CCamNaviProgressBarControl::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamNaviProgressBarControl::ConstructL()
    {
    iModel.RegisterObserverL( this );
    }                

// ---------------------------------------------------------
// CCamNaviProgressBarControl::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCamNaviProgressBarControl::SizeChanged()
    {
    // if the rectangle has width or height
    if ( Rect().Size() != TSize( 0, 0 ) && iActive )
        {
        TRAP_IGNORE(iModel.SetExtentL( Rect() ));
        TRAP_IGNORE(iModel.ReloadResourceDataL());
        }

//    AknsUtils::RegisterControlPosition( this );
    }
    
// ---------------------------------------------------------
// CCamNaviProgressBarControl::DrawNaviCtr
// Draw the control
// ---------------------------------------------------------
//
void 
CCamNaviProgressBarControl::DrawProgressBar( CBitmapContext& aGc ) const
	{
	if ( iActive )
	    {
        iModel.DrawProgressBar( aGc, this );
	    }
	}

// ---------------------------------------------------------
// CCamNaviProgressBarControl::Draw
// Draw the control
// ---------------------------------------------------------
//
void CCamNaviProgressBarControl::Draw( const TRect& /* aRect */ ) const
    {
    PRINT( _L("Camera => CCamNaviProgressBarControl::Draw" ))
    CWindowGc& gc=SystemGc();
    
    DrawProgressBar( gc );
    }
    
// ---------------------------------------------------------
// CCamNaviProgressBarControl::SetActive
// Set's whether this control is currently active or not
// ---------------------------------------------------------
//
void CCamNaviProgressBarControl::SetActiveL( TBool aActive )
    {
    iActive = aActive;
    if ( iActive )
        {
        iModel.SetExtentL( Rect() );
        }
    }

// ---------------------------------------------------------
// CCamNaviProgressBarControl::HandleObservedEvent
// Gets events from observed model
// ---------------------------------------------------------
//
void CCamNaviProgressBarControl::HandleObservedEvent(TCamObserverEvent aEvent)
    {
    if ( ECamObserverEventNaviModelUpdated == aEvent )
        {
        if ( iActive )
            {
            CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );

            if ( appUi && appUi->IsDirectViewfinderActive() )
                {
                TRAP_IGNORE(appUi->HandleCommandL( ECamCmdRedrawScreen ));
                }
            else
                {
                DrawDeferred();
                }
            }
        }
    }

// End of File


