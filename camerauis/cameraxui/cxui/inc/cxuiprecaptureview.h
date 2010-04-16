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
#ifndef CXUIPRECAPTUREVIEW_H
#define CXUIPRECAPTUREVIEW_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <hbview.h>
#include <hbradiobuttonlist.h>
#include <hbframedrawer.h>

#include "cxezoomcontrol.h"
#include "cxeviewfindercontrol.h"
#include "cxuidisplaypropertyhandler.h"


class HbAction;
class HbLabel;
class HbSlider;
class CxeEngine;
class HbMainWindow;
class CxuiSettingsInfo;
class HbToolBarExtension;
class CxuiDocumentLoader;
class HbTransparentWindow;
class CxuiCaptureKeyHandler;
class CxuiCaptureKeyHandler;
class CxuiSettingDialog;
class CxuiSettingRadioButtonList;
class CxuiSettingSlider;
class HbToolBarExtension;

/**
 * Pre-capture view
 */
class CxuiPrecaptureView : public HbView
{
    Q_OBJECT

public:
    CxuiPrecaptureView(QGraphicsItem *parent = 0);

    virtual ~CxuiPrecaptureView();

public:

    /**
     * Construct-method handles initialisation tasks for this class.
     * @param mainwindow
     * @param engine
     * @param documentLoader
     * @param keyHandler
     */
    virtual void construct(HbMainWindow *mainWindow, CxeEngine *engine,
                           CxuiDocumentLoader *documentLoader,
                           CxuiCaptureKeyHandler *keyHandler);

    /**
     * Loads widgets that are not part of the default section in layouts xml.
     * Widgets are created at the time they are first loaded.
     */
    virtual void loadWidgets() = 0;
    void prepareWindow();
    virtual void updateOrientation(Qt::Orientation orient) = 0;

public slots:

    void releaseCamera();
    void initCamera();
    void requestCameraSwitch();

    // whenever a setting is changed on the engine side, an icon might need updating
    // connects to the settingValueChanged signal of CxeSettings
    virtual void handleSettingValueChanged(const QString& key, QVariant newValue);

protected slots:

    // Key events
    virtual void handleAutofocusKeyPressed();

    // Camera / Engine
    void handleEngineZoomStateChange(CxeZoomControl::State newState, CxeError::Id error);
    void handleZoomLevelChange(int);
    void handleVfStateChanged(CxeViewfinderControl::State newState, CxeError::Id error);
    virtual void handleFocusGained();
    virtual void handleFocusLost() = 0;
    // UI: Zoom slider change notification
    void zoomTo(int value);

    // Control visibility of all UI items at the same time: toolbar, zoom and titlepane items
    void hideControls();
    virtual void showControls();
    void toggleControls();

    void disableControlsTimeout();

    void toggleZoom();
    void hideToolbar();

    void launchPhotosApp();
    void launchVideosApp();

    // Settings related
    void launchNotSupportedNotification();
    void showSettingsGrid();
    void hideSettingsGrid();
    void launchSliderSetting();
    void prepareToShowDialog(HbAction *action);
    void prepareToCloseDialog(HbAction *action);


protected:

   virtual void initializeSettingsGrid() = 0;

    void hideZoom();
    void showZoom();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void showToolbar();
    bool eventFilter(QObject *object, QEvent *event);
    void launchSettingsDialog(QObject *action);
    bool isPostcaptureOn() const;
    void addIncreaseDecreaseButtons(HbSlider *slider);
    QString getSettingItemIcon(const QString &key, QVariant value);
    void updateQualityIcon();
    void createWidgetBackgroundGraphic(HbWidget *widget,
                                       const QString &graphicName,
                                       HbFrameDrawer::FrameType frameType =
                                       HbFrameDrawer::NinePieces);

signals:

    // signals to switch to post/pre-capture view.
    void changeToPostcaptureView();
    void changeToPrecaptureView();

    // Switching between cameras
    void switchCamera();

    // signals to start and stop standby timer
    void startStandbyTimer();
    void stopStandbyTimer();

    // signal to report error to ErrorManager for further actions.
    void reportError(CxeError::Id errorId);

protected:
    CxeEngine *mEngine; // not own
    HbTransparentWindow *mViewfinder; // not own, owned by the graphics scene
    HbMainWindow *mMainWindow; // not own
    CxuiDocumentLoader *mDocumentLoader; // not own
    CxuiDisplayPropertyHandler *mDisplayHandler;
    bool   mControlsVisible;
    QTimer mHideControlsTimeout;
    HbSlider *mSlider; // zoom slider, not own, owned by the graphics scene
    HbToolBar *mToolBar; // not own, owned by the graphics scene
    HbToolBarExtension *mSettingsGrid;
    bool mZoomVisible;
    bool mWidgetsLoaded;
    CxuiSettingDialog *mSettingsDialog;
    CxuiSettingRadioButtonList *mSettingsDialogList;
    CxuiCaptureKeyHandler *mKeyHandler;
    HbLabel *mQualityIcon;

private:
    CxuiSettingDialog* createSettingsDialog();
    CxuiSettingDialog* createSliderSettingsDialog();
    QPointF getDialogPosition();
private:
    HbLabel *mSettingsDialogHeading;

    CxuiSettingDialog *mSliderSettingsDialog;
    HbLabel *mSliderSettingsDialogHeading;
    CxuiSettingSlider *mSettingsSlider;

    CxuiSettingsInfo *mSettingsInfo;

};

#endif // CXUIPRECAPTUREVIEW_H
