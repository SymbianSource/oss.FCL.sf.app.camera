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
#include <hblabel.h>
#include <hbslider.h>
#include <hbtoolbar.h>
#include <hbaction.h>

#include "cxutils.h"
#include "cxenamespace.h"
#include "cxuienums.h"
#include "cxuidocumentloader.h"
#include "cxuivideoprecaptureview.h"
#include "cxuivideoprecaptureview2.h"


using namespace Cxe;
using namespace CxUiLayout;


CxuiVideoPrecaptureView2::CxuiVideoPrecaptureView2( QGraphicsItem *parent) :
    CxuiVideoPrecaptureView(parent)
{
    CX_DEBUG_IN_FUNCTION();
}

CxuiVideoPrecaptureView2::~CxuiVideoPrecaptureView2()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxuiVideoPrecaptureView2::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    // Get pointers to ui components from the layout data
    QGraphicsWidget *widget = NULL;
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_VIEWFINDER);
    mViewfinder = qobject_cast<HbTransparentWindow *> (widget);
    CX_DEBUG_ASSERT(mViewfinder);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_ELAPSED_TIME_LABEL);
    mElapsedTimeText = qobject_cast<HbLabel *> (widget);
    CX_DEBUG_ASSERT(mElapsedTimeText);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_RECORDING_ICON);
    mRecordingIcon = qobject_cast<HbLabel *> (widget);
    CX_DEBUG_ASSERT(mRecordingIcon);

    // get needed pointers to some of the widgets
    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_ZOOM_SLIDER);
    mSlider = qobject_cast<HbSlider *> (widget);
    CX_DEBUG_ASSERT(mSlider);

    widget = mDocumentLoader->findWidget(VIDEO_PRE_CAPTURE_TOOLBAR);
    mToolBar = qobject_cast<HbToolBar *> (widget);
    CX_DEBUG_ASSERT(mToolBar);

    mWidgetsLoaded = true;

    hideControls();

    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiVideoPrecaptureView2::loadWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    // All loaded in loadDefaultWidgets()
    CX_DEBUG_EXIT_FUNCTION();
}


// end of file
