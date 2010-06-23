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
#ifndef CXUISTILLPRECAPTUREVIEW_H
#define CXUISTILLPRECAPTUREVIEW_H

#include <QMetaType>

#include "cxuiprecaptureview.h"
#include "cxeautofocuscontrol.h"
#include "cxeviewfindercontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxeerror.h"
#include "cxenamespace.h"
class HbListWidgetItem;
class HbListWidget;
class CxuiCaptureKeyHandler;
class CxuiDocumentLoader;
class HbAction;
class HbDialog;
class CxuiSelfTimer;
class HbPushButton;
class HbLabel;
class CxuiSelfTimerRadioButtonList;
class HbToolBarExtension;
class HbWidget;
/**
 * Pre-capture view for still image mode
 */


class CxuiStillPrecaptureView : public CxuiPrecaptureView
{
    Q_OBJECT

public:
    CxuiStillPrecaptureView(QGraphicsItem *parent = 0);

    virtual ~CxuiStillPrecaptureView();

public:

    /**
     * Construct-method handles initialisation tasks for this class. Needs to be called
     * before the instance of this class is used.
     * @param mainwindow
     * @param engine
     * @param documentLoader
     * @param keyHandler
     */
    virtual void construct(HbMainWindow *mainwindow, CxeEngine *engine,
                   CxuiDocumentLoader *documentLoader, CxuiCaptureKeyHandler *keyHandler = NULL);

    /**
     * Loads default widgets in layouts xml.
     */
    virtual void loadDefaultWidgets();

    /**
     * Loads widgets that are not part of the default section in layouts xml.
     * Widgets are created at the time they are first loaded.
     */
    virtual void loadWidgets();

protected:

    void showEvent(QShowEvent *event);
    virtual bool allowShowControls() const;

public slots:

    // from CxuiPrecaptureView
    virtual void handleSettingValueChanged(const QString& key, QVariant newValue);

    // whenever a scene setting is changed on the engine side, an icon might need updating on the toolbar
    // connects to the sceneChanged signal of CxeSettings
    void handleSceneChanged(CxeScene &scene);

protected slots:
    void focusAndCapture();
    void capture();
    void setCapturePending();
    void goToVideo();


    // Key events
    void handleCaptureKeyPressed();
    void handleAutofocusKeyPressed();
    void handleAutofocusKeyReleased();

    // Engine signals
    void handleAutoFocusStateChanged(CxeAutoFocusControl::State newState, CxeError::Id error);
    void handleViewfinderStateChanged(CxeViewfinderControl::State newState, CxeError::Id error);
    void handleStillCaptureStateChanged(CxeStillCaptureControl::State newState, CxeError::Id error);
    void handleSnapshot(CxeError::Id error);

    /**
     * Signal used to reset mCapturePending after a short timeout. If the image
     * cannot be captured within a given time of the key press, it is better to cancel
     * the whole operation.
     */
    void resetCapturePendingFlag();

    /**
     * Application focus slots are called if focus state is changed.
     * Camera is released or reserved according to new state.
     */
    void handleFocusLost();
    void launchSetting();
    void updateImagesLeftLabel();

protected:
    void initializeSettingsGrid();
    void closeDialogs();
    void updateFaceTrackingIcon();

protected:
    CxuiSelfTimer *mSelfTimer;
    HbAction *mFlashSetting;
    HbDialog *mStillSettingsPopup;
    HbDialog *mSceneModePopup;

    /**
     * Capture key was pressed while engine was not ready... Capture when possible.
     * This flag is reset after a short delay at resetCapturePendingFlag().
     */
    bool mCapturePending;
    HbLabel *mImagesLeft;
    HbWidget *mImagesLeftContainer;

private:
    bool mPendingAfCanceling;
};

Q_DECLARE_METATYPE(QList<int>)

#endif // CXUISTILLPRECAPTUREVIEW_H
