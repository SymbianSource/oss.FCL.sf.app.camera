/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cxuiview.h"


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
class HbWidget;
class CxuiZoomSlider;

/**
 * Pre-capture view
 */
class CxuiPrecaptureView : public CxuiView
{
    Q_OBJECT

public:
    CxuiPrecaptureView(QGraphicsItem *parent = 0);

    virtual ~CxuiPrecaptureView();

public:

    virtual void construct(HbMainWindow *mainWindow, CxeEngine *engine,
                           CxuiDocumentLoader *documentLoader,
                           CxuiCaptureKeyHandler *keyHandler);

    /**
     * Loads widgets that are not part of the default section in layouts xml.
     * Widgets are created at the time they are first loaded.
     */
    virtual void loadWidgets() = 0;
    void prepareWindow();

public slots:

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

    void disableControlsTimeout();

    void toggleZoom();

    // Settings related
    void launchDiskFullNotification();
    void showSettingsGrid();
    void hideSettingsGrid();
    void launchSliderSetting();
    void prepareToShowDialog(HbAction *action);
    void prepareToCloseDialog(HbAction *action);

protected:
    void toggleControls();
    virtual void initializeSettingsGrid() = 0;
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    void launchSettingsDialog(QObject *action);
    bool isPostcaptureOn() const;
    void addIncreaseDecreaseButtons(CxuiZoomSlider *slider);
    QString getSettingItemIcon(const QString &key, QVariant value);
    void updateQualityIcon();
    void updateSceneIcon(const QString& sceneId);


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
    HbTransparentWindow *mViewfinder; // not own, owned by the graphics scene
    CxuiDisplayPropertyHandler *mDisplayHandler;
    HbToolBarExtension *mSettingsGrid;
    bool mWidgetsLoaded;
    CxuiSettingDialog *mSettingsDialog;
    CxuiSettingRadioButtonList *mSettingsDialogList;
    HbLabel *mQualityIcon;
    HbLabel *mFaceTrackingIcon;

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

    HbAction *mSettingsDialogOkAction;
    HbAction *mSliderSettingsDialogOkAction;
};

#endif // CXUIPRECAPTUREVIEW_H
