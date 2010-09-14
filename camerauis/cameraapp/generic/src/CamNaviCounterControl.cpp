/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
*/


// INCLUDE FILES
#include <AknsUtils.h>
#include <e32base.h>
#include "CamNaviCounterControl.h"
#include "CamNaviCounterModel.h"
#include "CamAppUi.h"
#include "CamUtility.h"

// CONSTANTS

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------
// CCamNaviCounterControl::NewL
// Factory construction function
// ---------------------------------------------------------
//
CCamNaviCounterControl* CCamNaviCounterControl::NewL( CCamNaviCounterModel& aModel )
    {
    CCamNaviCounterControl* self = new( ELeave ) CCamNaviCounterControl( aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CCamNaviCounterControl::~CCamNaviCounterControl()
    {
    iModel.DeregisterObserver( this );
    }

// ---------------------------------------------------------
// CCamNaviCounterControl::CCamNaviCounterControl
// C++ constructor
// ---------------------------------------------------------
//
CCamNaviCounterControl::CCamNaviCounterControl(CCamNaviCounterModel& aModel )
    : iModel ( aModel )
    {
    }

// ---------------------------------------------------------
// CCamNaviCounterControl::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------
//
void CCamNaviCounterControl::ConstructL()
    {
    iModel.RegisterObserverL( this );
    }                

// ---------------------------------------------------------
// CCamNaviCounterControl::SizeChanged
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CCamNaviCounterControl::SizeChanged()
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
// CCamNaviCounterControl::DrawNaviCtr
// Draw the control
// ---------------------------------------------------------
//
void CCamNaviCounterControl::DrawNaviCtr( CBitmapContext& aGc ) const
	{
	if ( iActive )
	    {
        iModel.DrawNaviCtr( aGc, this );
	    }
	}

// ---------------------------------------------------------
// CCamNaviCounterControl::Draw
// Draw the control
// ---------------------------------------------------------
//
void CCamNaviCounterControl::Draw( const TRect& /* aRect */ ) const
    {
    PRINT( _L("Camera => CCamNaviCounterControl::Draw" ))
    
       CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
    if ( appUi->CurrentViewState() != ECamViewStatePostCapture ) 
	{
    		CWindowGc& gc=SystemGc();    
    		DrawNaviCtr( gc );
    	}

    PRINT( _L("Camera <= CCamNaviCounterControl::Draw" ))
    }
    
// ---------------------------------------------------------
// CCamNaviCounterControl::ForceNaviPaneUpdate
// Force update of navi-pane (i.e after dismissal of MMC removed error note)
// ---------------------------------------------------------
//
void CCamNaviCounterControl::ForceNaviPaneUpdate()
    {
    iModel.ForceNaviPaneUpdate();
    
    // redraw
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

// ---------------------------------------------------------
// CCamNaviCounterControl::SetActiveL
// Set's whether this control is currently active or not
// ---------------------------------------------------------
//
void CCamNaviCounterControl::SetActiveL( TBool aActive )
    {
    iActive = aActive;
    if ( iActive )
        {
        ActivateL();
        iModel.SetExtentL( Rect() );
        }
    }

// ---------------------------------------------------------
// CCamNaviCounterControl::HandleObservedEvent
// Gets events from observed model
// ---------------------------------------------------------
//
void CCamNaviCounterControl::HandleObservedEvent(TCamObserverEvent aEvent)
    {
    PRINT( _L("Camera => CCamNaviCounterControl::HandleObservedEvent" ));
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
#ifndef __WINS__ 
              if( appUi && appUi->ReadyToDraw() && appUi->IsBurstEnabled()
                  && !appUi->SettingsLaunchedFromCamera() )
                    {
                    PRINT( _L("Camera <> CCamNaviCounterControl::HandleObservedEvent - draw deferred") );
                    DrawDeferred();
                    }
               
#endif
                }
            }
        }
    PRINT( _L("Camera <= CCamNaviCounterControl::HandleObservedEvent" ));
    }

// End of File


