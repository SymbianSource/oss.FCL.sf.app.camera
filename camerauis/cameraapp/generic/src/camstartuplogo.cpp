/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CCoeControl to show a logo at camera application startup
*
*/

#include <fbs.h>
#include <e32math.h>
#include <e32debug.h>
#include <AknIconUtils.h>
#include <cameraapp.mbg>
#include "camlogging.h"
#include "camstartuplogo.h"

_LIT(KCamBitmapFile, "z:\\resource\\apps\\cameraapp.mif");
const TInt KStartupLogoOrdinalPriority( 2 );

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CCamStartupLogo* CCamStartupLogo::NewL(CCamStartupLogoController& aController,
                                       const TRect& aRect)
    {
    PRINT(_L("Camera => CCamStartupLogo::NewL"))
    CCamStartupLogo* self = new (ELeave) CCamStartupLogo(aController, aRect);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT(_L("Camera <= CCamStartupLogo::NewL"))
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CCamStartupLogo::CCamStartupLogo(CCamStartupLogoController& aController,
                                 const TRect& aRect)
    : iRect(aRect), iController(aController)
    {
    // No implementation needed
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCamStartupLogo::~CCamStartupLogo()
    {
    iController.RemoveObserver(this);

    delete iLogo;
    delete iLogoMask;
    }

// ---------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CCamStartupLogo::ConstructL()
    {
    PRINT(_L("Camera => CCamStartupLogo::ConstructL"))

    iController.AddObserver(this);

    if (iController.State() == EStartupLogoVisible)
        {
        ShowL();
        }

    PRINT(_L("Camera <= CCamStartupLogo::ConstructL"))
    }

// ---------------------------------------------------------------------------
// CCamStartupLogo::StartupLogoControllerStateChanged
// ---------------------------------------------------------------------------
//
void CCamStartupLogo::StartupLogoControllerStateChanged(
    TStartupLogoState aNewState)
    {
    PRINT(_L("Camera => CCamStartupLogo::StartupLogoControllerStateChanged"))

    if (aNewState == EStartupLogoNotVisible)
        {
        Hide();
        }
    else if (aNewState == EStartupLogoVisible)
        {
        TRAP_IGNORE(ShowL()); 
        }
    else
        {
        PRINT(_L("Camera <> StartupLogoControllerStateChanged - unknown state"))
        }

    PRINT(_L("Camera <= CCamStartupLogo::StartupLogoControllerStateChanged"))
    }

// ---------------------------------------------------------------------------
// CCamStartupLogo::ShowL
// ---------------------------------------------------------------------------
//
void CCamStartupLogo::ShowL()
    {
    PRINT(_L("Camera => CCamStartupLogo::ShowL"))

    if (!iLogo || !iLogoMask)
        {
        delete iLogo;
        iLogo = NULL;
        delete iLogoMask;
        iLogoMask = NULL;

        TTime t0;
        t0.UniversalTime();

        AknIconUtils::CreateIconL(iLogo,
                                  iLogoMask,
                                  KCamBitmapFile,
                                  EMbmCameraappQgn_menu_cams,
                                  EMbmCameraappQgn_menu_cams_mask);
        
        TSize logoSize(iRect.Size().iWidth/2, iRect.Size().iHeight/2);
        AknIconUtils::SetSize(iLogo, logoSize);

        TTime t1;
        t1.UniversalTime();

        PRINT1(_L("Camera <> Startup icon load took %d us"), I64LOW(t1.MicroSecondsFrom(t0).Int64()))
        }

    if (!iWindowCreated)
        {
        CreateWindowL();
        iWindowCreated = ETrue;
        }

    if (!IsActivated())
        {
        ActivateL();
        }

    SetExtentToWholeScreen();

    MakeVisible(ETrue);
    RDrawableWindow* window = DrawableWindow();
    if (window)
        {
        window->SetOrdinalPosition(0, KStartupLogoOrdinalPriority);
        }

    PRINT(_L("Camera <= CCamStartupLogo::ShowL"))
    }

// ---------------------------------------------------------------------------
// CCamStartupLogo::Hide
// ---------------------------------------------------------------------------
//
void CCamStartupLogo::Hide()
    {
    PRINT(_L("Camera => CCamStartupLogo::Hide"))

    MakeVisible(EFalse);

    if (iWindowCreated)
        {
        CloseWindow();
        iWindowCreated = EFalse;
        }

    delete iLogo;
    iLogo = NULL;
    delete iLogoMask;
    iLogoMask = NULL;

    PRINT(_L("Camera <= CCamStartupLogo::Hide"))
    }

// ---------------------------------------------------------------------------
// CCamStartupLogo::Draw
// ---------------------------------------------------------------------------
//
void CCamStartupLogo::Draw(const TRect& /*aRect*/) const
    {
    PRINT(_L("Camera <> CCamStartupLogo::Draw"))

    CWindowGc& gc = SystemGc();

    gc.SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    gc.SetBrushColor(TRgb::Color16MA(0xFF000000)); // opaque black
    gc.SetPenStyle(CGraphicsContext::ENullPen);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.DrawRect(Rect());

    if (iLogo && iLogoMask)
        {
        TSize logoSize(iLogo->SizeInPixels());
        TPoint logoTl(iRect.Center().iX - logoSize.iWidth / 2,
                      iRect.Center().iY - logoSize.iHeight / 2);
        gc.BitBltMasked(logoTl, iLogo, TRect(logoSize), iLogoMask, EFalse);
        }
    }

