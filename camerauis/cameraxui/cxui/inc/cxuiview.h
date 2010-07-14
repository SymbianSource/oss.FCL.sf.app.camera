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
* Description: This is a header file for a CxuiView class
*
*/

#ifndef CXUIVIEW_H
#define CXUIVIEW_H

#include <QObject>
#include <QTimer>
#include <hbview.h>
#include <hbmainwindow.h>
#include <hbframedrawer.h>

class CxuiDocumentLoader;
class CxuiCaptureKeyHandler;
class CxuiZoomSlider;
class CxuiCaptureKeyHandler;
class CxeEngine;
class HbWidget;

// CONSTANTS
const int CXUI_HIDE_CONTROLS_TIMEOUT     = 6000; // 6 seconds

/**
 * A base class for all views in the Camera UI
 * The class is for deriving only
 */
class CxuiView : public HbView
{
    Q_OBJECT

public:
    CxuiView(QGraphicsItem *parent = 0);
    virtual ~CxuiView();

protected:
    virtual void construct(HbMainWindow *mainWindow, CxeEngine *engine,
                           CxuiDocumentLoader *documentLoader,
                           CxuiCaptureKeyHandler * keyHandler);

public:
    virtual void updateOrientation(Qt::Orientation orientation);

protected:
    virtual bool allowShowControls() const;
    virtual void toggleControls();
    virtual void showToolbar();
    virtual void hideZoom();
    virtual void showZoom();
    virtual void hideIndicators();
    virtual void showIndicators();
    virtual void createWidgetBackgroundGraphic(HbWidget *widget,
                                       const QString &graphicName,
                                       HbFrameDrawer::FrameType frameType =
                                       HbFrameDrawer::NinePieces);

protected slots:
    virtual void launchNotSupportedNotification();
    virtual void launchPhotosApp();
    virtual void launchVideosApp();
    virtual void releaseCamera();

    virtual void launchScenesView();

    // Control visibility of all UI items at the same time: toolbar, zoom and titlepane items
    virtual void hideControls();
    virtual void showControls();

    virtual void hideToolbar();


signals:
    void viewCloseEvent();
    void showScenesView();

protected: //common data
    HbMainWindow *mMainWindow; // not own
    CxeEngine *mEngine; // not own
    CxuiDocumentLoader *mDocumentLoader; // not own
    CxuiCaptureKeyHandler* mCaptureKeyHandler;

    CxuiZoomSlider *mSlider; // zoom slider, not own, owned by the graphics scene
    HbToolBar *mToolbar; // not own, owned by the graphics scene
    HbWidget *mIndicators;

    bool   mControlsVisible;
    bool   mZoomVisible;
    QTimer mHideControlsTimeout;

};

#endif // CXUIVIEW_H
