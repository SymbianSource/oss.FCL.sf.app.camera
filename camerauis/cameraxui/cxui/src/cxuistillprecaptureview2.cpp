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
* Description:
*
*/

#include <hbtransparentwindow.h>
#include <hbslider.h>
#include <hbtoolbar.h>
#include <hbaction.h>

#include "cxutils.h"
#include "cxenamespace.h"
#include "cxuienums.h"
#include "cxuidocumentloader.h"
#include "cxuistillprecaptureview.h"
#include "cxuistillprecaptureview2.h"


using namespace Cxe;
using namespace CxUiLayout;


CxuiStillPrecaptureView2::CxuiStillPrecaptureView2( QGraphicsItem *parent) :
    CxuiStillPrecaptureView(parent)
{
    CX_DEBUG_IN_FUNCTION();
}

CxuiStillPrecaptureView2::~CxuiStillPrecaptureView2()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxuiStillPrecaptureView2::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    // get pointer to the viewfinder
    QGraphicsWidget *widget = 0;
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_VIEWFINDER);
    mViewfinder = qobject_cast<HbTransparentWindow *>(widget);
    CX_DEBUG_ASSERT(mViewfinder);


    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_ZOOM_SLIDER);
    mSlider = qobject_cast<HbSlider *>(widget);
    CX_DEBUG_ASSERT(mSlider);
    Q_ASSERT_X((mSlider != 0), "camerax ui", "invalid xml file");

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_TOOLBAR);
    mToolBar = qobject_cast<HbToolBar *>(widget);
    CX_DEBUG_ASSERT(mToolBar);

    QObject *object = 0;
    object = mDocumentLoader->findObject(STILL_PRE_CAPTURE_FLASH_ACTION);
    mFlashSetting = qobject_cast<HbAction *>(object);
    CX_DEBUG_ASSERT(mFlashSetting);

    mWidgetsLoaded = true;

    hideControls();

    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiStillPrecaptureView2::loadWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


// end of file
