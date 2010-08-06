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
* Description:
*
*/

#include <HbMainWindow>
#include <HbView>
#include <HbLabel>
#include <QGraphicsLinearLayout>
#include "cxeviewfindercontroldesktop.h"
#include "cxecameradevicedesktop.h"
#include "cxeviewfinderwidgetdesktop.h"
#include "cxutils.h"

const int KResWidth = 640;
const int KResHeight = 360;

CxeViewfinderControlDesktop::CxeViewfinderControlDesktop(CxeCameraDeviceDesktop &cameraDevice) :
        mState(Uninitialized),
        mResolution(KResWidth, KResHeight),
        mCameraDevice(cameraDevice),
        mViewfinderWidget(0)
{
    CX_DEBUG_IN_FUNCTION();
}

CxeViewfinderControlDesktop::~CxeViewfinderControlDesktop()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Set Window ID
*/
void CxeViewfinderControlDesktop::setWindow(WId windowId)
{
    CX_DEBUG_ENTER_FUNCTION();
    mWindowId = windowId;

    if (state() != Running) {
        mState = Ready;
        emit stateChanged(mState, CxeError::None);
    CX_DEBUG_EXIT_FUNCTION();
    }


    HbMainWindow* mainWindow = qobject_cast<HbMainWindow*>(QWidget::find(mWindowId));

    if (mainWindow) {
        if( !mViewfinderWidget) {
            mViewfinderWidget = new CxeViewfinderWidgetDesktop();
            connect(&mCameraDevice, SIGNAL(imageChanged(QPixmap)), mViewfinderWidget, SLOT(handleImageChange(QPixmap)));
        }
        HbView* view = mainWindow->currentView();
        view->scene()->addItem(mViewfinderWidget);
        mViewfinderWidget->setZValue(-1.0);
    }
}

/*!
* Start viewfinder
* @return CxeEngine specific error code
*/
CxeError::Id CxeViewfinderControlDesktop::start()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mState != Running) {
        mState = Running;
        emit stateChanged(mState, CxeError::None);
        mCameraDevice.start();
    }
    CX_DEBUG_EXIT_FUNCTION();
    return CxeError::None;
}

/*!
* Stop viewfinder
*/
void CxeViewfinderControlDesktop::stop()
{
    CX_DEBUG_ENTER_FUNCTION();
    mState = Ready;
    emit stateChanged(mState, CxeError::None);
    mCameraDevice.stop();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Current viewfinder state
*/
CxeViewfinderControl::State CxeViewfinderControlDesktop::state() const
{
    return mState;
}

/*!
* Returns Device's Display resolution
*/
QSize CxeViewfinderControlDesktop::deviceDisplayResolution() const
{
    return mResolution;
}

// end of file
