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
* Description: CCActivePaletteContainer implementation.
*
*/

#include <w32std.h>
#include <coeaui.h>
#include <apgwgnam.h>
//#include <eikenv.h>
//#include <eikapp.h>
//#include <eikappui.h>


#include <StifLogger.h>

#include <activepalette2ui.h>
#include <activepalette2factory.h>
#include <activepalette2genericpluginint.h>
#include <activepalette2eventdata.h>
#include <activepalette2itemvisible.h>
#include <activepalette2navigationkeys.h>

#include <ActivePalette2ApiTest.mbg>

#include "activepalette2apitest.h"
#include "ActivePaletteContainer.h"


#define LOG(text) iTest->Log()->Log(_L(text))
#define LOG1(text, param) iTest->Log()->Log(_L(text), param)
#define LOG2(text, param1, param2) iTest->Log()->Log(_L(text), param1, param2)
#define LOG3(text, param1, param2, param3) iTest->Log()->Log(_L(text), param1, param2, param3)

namespace 
    {
    _LIT( KTrueString,  "true"  );
    _LIT( KFalseString, "false" );
    }    


// -----------------------------------------------------------------------------
// CActivePaletteContainer::~CActivePaletteContainer
// Destructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CActivePaletteContainer::~CActivePaletteContainer()
    {
    if( iActivePalette != NULL )
        {
        iActivePalette->SetPaletteVisibility(ETrue, EFalse);
        delete iActivePalette;
        iActivePalette = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::ConstructL
// Second phase constructor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::ConstructL(CActivePalette2ApiTest* aTest)
    {
    iTest = aTest;

    LOG("=> CActivePaletteContainer::ConstructL");
    CreateWindowL();    

    TVwsViewId viewId;
    CCoeEnv::Static()->AppUi()->GetDefaultViewId(viewId);    
    iAppUid = viewId.iAppUid;
    LOG1("<> CActivePalette2ApiTest::ConstructL: GetDefaultViewId() appuid: %d", iAppUid.iUid );
    LOG("<> CActivePaletteContainer::ConstructL 1");
    SetRect(CCoeEnv::Static()->ScreenDevice()->SizeInPixels());
    LOG("<> CActivePaletteContainer::ConstructL 2");
    CreateActivePaletteL();
    LOG("<> CActivePaletteContainer::ConstructL 3");
    ActivateL();
    LOG("<= CActivePaletteContainer::ConstructL");
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::CountComponentControls
// From CCoeControl.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt 
CActivePaletteContainer::CountComponentControls() const
    {
    if( iActivePalette != NULL )
        return 1;
    else
        return 0;
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::ComponentControl
// From CCoeControl.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CCoeControl* 
CActivePaletteContainer::ComponentControl(TInt aIndex) const
    {
    if( aIndex == 0 )
        return iActivePalette != NULL ? iActivePalette->CoeControl() : NULL;
    else
        return NULL;            
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::Draw
// From CCoeControl.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::Draw(const TRect &aRect) const 
    {
    LOG("=> CActivePaletteContainer::Draw");
    
    CWindowGc& gc = SystemGc();
    gc.Reset();
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(TRgb(0xE0,0xE0,0xFF));
    gc.Clear(aRect);    

    LOG("<> CActivePaletteContainer::Draw 1");
    
    //iActivePalette->RenderActivePalette( TRect(iPosition,iSize) );

    LOG("<= CActivePaletteContainer::Draw");
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::OfferKeyEventL
// From CCoeControl.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse 
CActivePaletteContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType) 
    {
    LOG3("=> CActivePaletteContainer::OfferKeyEventL - keycode[%d] scancode[%d] type[%d]", 
          aKeyEvent.iCode, aKeyEvent.iScanCode, aType);
    
    TKeyResponse response = EKeyWasNotConsumed; 
    if( iActivePalette 
     && iActivePalette->CoeControl() )
        {
        response = iActivePalette->CoeControl()->OfferKeyEventL(aKeyEvent,aType);
        LOG1("<> CActivePaletteContainer::OfferKeyEventL - Active Palette consumed ? %S", 
             response == EKeyWasConsumed ? &KTrueString : &KFalseString );
        }
    if( EKeyWasNotConsumed == response )
        {
        response = CCoeControl::OfferKeyEventL(aKeyEvent, aType);        
        }

    LOG("<= CActivePaletteContainer::OfferKeyEventL");
    return response;
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::SizeChanged
// From CCoeControl.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::SizeChanged()
    {
    LOG("=> CActivePaletteContainer::SizeChanged");
    CCoeControl::SizeChanged();    
    LOG("<= CActivePaletteContainer::SizeChanged");
    }


// -----------------------------------------------------------------------------
// CActivePaletteContainer::ViewId
// From MCoeView.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TVwsViewId 
CActivePaletteContainer::ViewId() const
    {
    LOG("=> CActivePaletteContainer::ViewId");    
    TVwsViewId id;
    id.iAppUid  = iAppUid;
    id.iViewUid = TUid::Uid(1234567890);
    LOG("<= CActivePaletteContainer::ViewId");
    return id;
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::ViewActivatedL
// From MCoeView.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::ViewActivatedL(
        const TVwsViewId& /*aPrevViewId*/, 
              TUid        /*aCustomMessageId*/, 
        const TDesC8&     /*aCustomMessage*/ )
    {
    LOG("=><= CActivePaletteContainer::ViewActivatedL");    
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::ViewDeactivated
// From MCoeView.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::ViewDeactivated()
    {
    LOG("=><= CActivePaletteContainer::ViewDeactivated");    
    }


// -----------------------------------------------------------------------------
// CActivePaletteContainer::NotifyItemFocused
// From MActivePalette2Observer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::NotifyItemFocused(TInt /*aPrevItem*/, TInt /*aFocusedItem*/)
    {    
    LOG("=><= CActivePaletteContainer::NotifyItemFocused");    
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::NotifyItemSelected
// From MActivePalette2Observer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::NotifyItemSelected(const TActivePalette2EventData& aEvent)
    {
    LOG("=> CActivePaletteContainer::NotifyItemSelected");
    CheckActivePaletteEvent(aEvent);
    LOG("<= CActivePaletteContainer::NotifyItemSelected");
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::NotifyItemComplete
// From MActivePalette2Observer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::NotifyItemComplete(const TActivePalette2EventData& aEvent,
                                            const TDesC8& aDataDes,
                                            TInt aDataInt)
    {
    LOG ("=> CActivePaletteContainer::NotifyItemComplete");
    CheckActivePaletteEvent(aEvent);
    LOG1("<> CActivePaletteContainer::NotifyItemComplete - data descr: %S", &aDataDes);
    LOG1("<> CActivePaletteContainer::NotifyItemComplete - data int: %d", aDataInt);
    LOG ("<= CActivePaletteContainer::NotifyItemComplete");
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::NotifyMessage
// From MActivePalette2Observer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::NotifyMessage(const TActivePalette2EventData& aEvent,
                                       TInt aMessageID,
                                       const TDesC8& aDataDes,
                                       TInt aDataInt)
    {
    LOG ("=> CActivePaletteContainer::NotifyMessage");
    CheckActivePaletteEvent(aEvent);
    LOG1("<> CActivePaletteContainer::NotifyMessage - message id: %d", aMessageID);
    LOG1("<> CActivePaletteContainer::NotifyMessage - data descr: %S", &aDataDes);
    LOG1("<> CActivePaletteContainer::NotifyMessage - data int: %d", aDataInt);
    LOG ("<= CActivePaletteContainer::NotifyMessage");
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::Redraw
// From MActivePalette2Observer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::Redraw(const TRect& aArea)
    {
    LOG("=> CActivePaletteContainer::Redraw");    
    /*
    CWindowGc& gc = SystemGc();
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(TRgb(0x00,0x00,0xFF));
    gc.Clear(aArea);
    */
    Draw(aArea);
    LOG("<= CActivePaletteContainer::Redraw");    
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::ActivePaletteUpdated
// From MActivePalette2Observer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::ActivePaletteUpdated(void)
    {
    LOG("=> CActivePaletteContainer::ActivePaletteUpdated");    
    DrawDeferred();    
    LOG("<= CActivePaletteContainer::ActivePaletteUpdated");    
    }



// -----------------------------------------------------------------------------
// CActivePaletteContainer::CreateActivePaletteL
// Helper method for creating ActivePalette.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void
CActivePaletteContainer::CreateActivePaletteL()
    {
    delete iActivePalette;
    iActivePalette = NULL;

    iActivePalette = ActivePalette2Factory::CreateActivePaletteUIL( EAP2DrawModeSemiTransparent );

    iActivePalette->SetObserver( this );
    iActivePalette->CoeControl()->SetContainerWindowL( *this );
    iActivePalette->CoeControl()->SetMopParent( this );  
    iActivePalette->SetPaletteVisibility(ETrue, EFalse);
    TRect rect( Rect() );
    iActivePalette->LocateTo( TPoint(rect.Width()/4, rect.Height()/4) );
    }

// -----------------------------------------------------------------------------
// CActivePaletteContainer::ActivePalette
// Get the Active Palette instance.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
MActivePalette2UI* 
CActivePaletteContainer::ActivePalette() const 
    {
    return iActivePalette;
    }


// -----------------------------------------------------------------------------
// CActivePaletteContainer::CheckActivePaletteEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CActivePaletteContainer::CheckActivePaletteEvent( const TActivePalette2EventData& aEvent )
    {
    TInt resultCode( aEvent.Result() );
    TInt currentItem( aEvent.CurItem() );
    TInt previousItem( aEvent.PrevItem() );
    TInt servingItem( aEvent.ServingItem() );
    TUid servingPluginUid( aEvent.ServingPlugin() );
    
    iTest->Log()->Log( _L("=><= CActivePaletteContainer::CheckActivePaletteEvent - got: code[%d] items[current:%d previous:%d serving:%d] plugin[%d]"),
                       resultCode, currentItem, previousItem, servingItem, servingPluginUid.iUid);
    }



