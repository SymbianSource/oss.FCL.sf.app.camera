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

#include <coemain.h>
#include <QGraphicsSceneEvent>
#include <QProcess>
#include <QApplication>
#include <hbmainwindow.h>
#include <hbslider.h>
#include <hbtoolbar.h>
#include <hblabel.h>
#include <hbtoolbarextension.h>
#include <hbtransparentwindow.h>
#include <hbmenu.h>
#include <hbdialog.h>
#include <hbdeviceprofile.h>
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <hbstyle.h>
#include <hbframeitem.h>
#include <hbwidget.h>

#include "cxeengine.h"
#include "cxeviewfindercontrol.h"
#include "cxezoomcontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxenamespace.h"
#include "cxefeaturemanager.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxesettings.h"
#include "cxutils.h"
#include "cxuienums.h"
#include "cxuidocumentloader.h"
#include "cxuidisplaypropertyhandler.h"
#include "cxuicapturekeyhandler.h"
#include "cxuiprecaptureview.h"
#include "cxuisettingdialog.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuisettingslider.h"
#include "cxuisettingsinfo.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuiprecaptureviewTraces.h"
#endif
#include "cxuiserviceprovider.h"
#include "cxuizoomslider.h"


// CONSTANTS
const int CXUI_HIDE_CONTROLS_TIMEOUT     = 6000; // 6 seconds
const QString PhotosAppExe = "photos.exe";
const QString VideosAppExe = "videoplayer.exe";

using namespace CxUiLayout;
using namespace CxUiSettings;
using namespace CxUiInternal;



// ---------------------------------------------------------------------------
// CxuiPrecaptureView::CxuiPrecaptureView
// Constructor
// ---------------------------------------------------------------------------
//
CxuiPrecaptureView::CxuiPrecaptureView(QGraphicsItem *parent) :
    HbView(parent),
    mEngine(0),
    mViewfinder(0),
    mMainWindow(0),
    mDocumentLoader(0),
    mDisplayHandler(0),
    mControlsVisible(false),
    mHideControlsTimeout(this),
    mSlider(0),
    mToolBar(0),
    mSettingsGrid(0),
    mZoomVisible(false),
    mWidgetsLoaded(false),
    mSettingsDialog(NULL),
    mSettingsDialogList(NULL),
    mKeyHandler(NULL),
    mQualityIcon(NULL),
    mIndicators(NULL),
    mSettingsDialogHeading(NULL),
    mSliderSettingsDialog(NULL),
    mSliderSettingsDialogHeading(NULL),
    mSettingsSlider(NULL),
    mSettingsInfo(NULL),
    mSettingsDialogOkAction(NULL),
    mSliderSettingsDialogOkAction(NULL)
{
    CX_DEBUG_ENTER_FUNCTION();
    mDisplayHandler = new CxuiDisplayPropertyHandler();
    CX_DEBUG_EXIT_FUNCTION();
}

CxuiPrecaptureView::~CxuiPrecaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::instance()->removeEventFilter(this);
    delete mSettingsGrid;
    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiPrecaptureView::construct(HbMainWindow *mainWindow, CxeEngine *engine,
                                   CxuiDocumentLoader *documentLoader,
                                   CxuiCaptureKeyHandler * keyHandler)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUIPRECAPTUREVIEW_CONSTRUCT, "msg: e_CX_PRECAPVIEW_CONST 1" );

    CX_ASSERT_ALWAYS(mainWindow);
    CX_ASSERT_ALWAYS(engine);
    CX_ASSERT_ALWAYS(documentLoader);

    mMainWindow = mainWindow;
    mEngine = engine;
    mDocumentLoader = documentLoader;
    mKeyHandler = keyHandler;

    mSettingsInfo = new CxuiSettingsInfo(engine);
    CX_DEBUG_ASSERT(mSettingsInfo);

    setContentFullScreen(true);

    connect(&mEngine->zoomControl(), SIGNAL(stateChanged(CxeZoomControl::State, CxeError::Id)), this,
            SLOT(handleEngineZoomStateChange(CxeZoomControl::State, CxeError::Id)));

    connect(&mEngine->zoomControl(), SIGNAL(zoomLevelChanged(int)), this,
            SLOT(handleZoomLevelChange(int)));


    connect(&mEngine->viewfinderControl(), SIGNAL(stateChanged(CxeViewfinderControl::State, CxeError::Id)),
            this, SLOT(handleVfStateChanged(CxeViewfinderControl::State, CxeError::Id)));

    connect(&(mEngine->settings()), SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&, QVariant)));

    // adjust the timer, and connect it to correct slot
    connect(&mHideControlsTimeout, SIGNAL(timeout()), this, SLOT(hideControls()));

    mHideControlsTimeout.setSingleShot(true);
    mHideControlsTimeout.setInterval(CXUI_HIDE_CONTROLS_TIMEOUT);

    hideControls();

    if (CxuiServiceProvider::isCameraEmbedded()) {
        // override exit with action that completes the request before exiting
        HbAction* exitAction = new HbAction(Hb::QuitNaviAction, this);
        connect(exitAction, SIGNAL(triggered()), CxuiServiceProvider::instance(), SLOT(sendFilenameToClientAndExit()));
        setNavigationAction(exitAction);
    }
    OstTrace0( camerax_performance, DUP1_CXUIPRECAPTUREVIEW_CONSTRUCT, "msg: e_CX_PRECAPVIEW_CONST 0" );

    QCoreApplication::instance()->installEventFilter(this);
    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiPrecaptureView::prepareWindow
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::prepareWindow()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mMainWindow) {
        mEngine->viewfinderControl().setWindow(mMainWindow->effectiveWinId());
        setFlag(QGraphicsItem::ItemIsFocusable);
        setFocusPolicy(Qt::StrongFocus);
        setFocus();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::handleEngineZoomStateChange
// Slot for the initModeComplete signal from device control.
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::handleEngineZoomStateChange(CxeZoomControl::State newState, CxeError::Id /*error*/ )
{
    CX_DEBUG_ENTER_FUNCTION();

    if (newState == CxeZoomControl::Ready) {
        // Set the zoom range (if more than zero!)
        // NOTE: This is & should be done after ECam resolution has been set.
        int min = mEngine->zoomControl().min();
        int max = mEngine->zoomControl().max();

        // only change values if they are acceptable and have changed
        if (mSlider && (max - min > 0) && ((mSlider->maximum() != max)
                || (mSlider->minimum()!= min))) {
            mSlider->setRange(min, max);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::zoomTo
// Responds to valueChanged notification from zoom bar UI
// and asks the engine to set the appropriate zoom level.
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::zoomTo(int value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(( "Slider value: %d", value ));
    mEngine->zoomControl().zoomTo(value);

    // restarts the timer, if ongoing
    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::hideControls
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::hideControls()
{
    CX_DEBUG_ENTER_FUNCTION();

    hideItems(Hb::AllItems);
    hideZoom();
    mControlsVisible = false;
    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.stop();
    }
    hideToolbar();

    // show indicators when controls are hidden
    showIndicators();

    // give the keyboard focus back to the view
    // for the view to receive key events
    setFocus();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::hideToolbar
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::hideToolbar()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mToolBar) {
        mToolBar->hide();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::hideZoom
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::hideZoom()
{
    if (mSlider) {
        mSlider->hide();
    }
    mZoomVisible = false;
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::showZoom
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::showZoom()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mSlider) {

        // if maxVal has not been set yet, ask for new parameters from engine
        if (mSlider->maximum() <= 0) {

            // get the zoom range
            int min = mEngine->zoomControl().min();
            int max = mEngine->zoomControl().max();

            // only change values if they are acceptable and have changed
            if ((max - min > 0) && ((mSlider->maximum() != max) || (mSlider->minimum() != min))) {
                mSlider->setRange(min, max);
            }
        }

        // show zoom only if the slider has acceptable value
        if (mSlider->maximum() > 0) {
            mSlider->show();
        }
    }

    mZoomVisible = true;
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::toggleZoom
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::toggleZoom()
{
    if (mZoomVisible) {
        hideZoom();
    } else {
        showZoom();
    }
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::hideIndicators
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::hideIndicators()
{
    if (mIndicators) {
        mIndicators->hide();
    }
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::showIndicators
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::showIndicators()
{
    if (mIndicators) {
        mIndicators->show();
    }
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::showControls
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::showControls()
{
    if (mEngine) {
        bool videoCases = (mEngine->videoCaptureControl().state() == CxeVideoCaptureControl::Recording ||
                           mEngine->videoCaptureControl().state() == CxeVideoCaptureControl::Paused);

        if (mEngine->isEngineReady() || videoCases) {
            // show toolbar
            showToolbar();
            // show zoom
            showZoom();
            // show titlepane
            showItems(Hb::AllItems);

            // hide indicators when controls are shown
            hideIndicators();

            mHideControlsTimeout.start();
            mControlsVisible = true;
        }
    }
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::showToolbar
// shows toolbar
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::showToolbar()
{

    CX_DEBUG_ENTER_FUNCTION();

    // toolbar pointer is missing if widgets for the view have not been loaded
    // from the DocML
    if (mToolBar) {
        mToolBar->show();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*
 *  Event filter which filters tool bar mouse events and
 *  application wide mouse events.
 */

bool CxuiPrecaptureView::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object)

    bool eventWasConsumed = false;

    switch (event->type())
    {
    case QEvent::GraphicsSceneMouseRelease:
        mHideControlsTimeout.start();
        break;
    case QEvent::GraphicsSceneMousePress:
        mHideControlsTimeout.stop();
        break;
    default:
        break;
    }

    return eventWasConsumed;
}


// ---------------------------------------------------------------------------
// CxuiPrecaptureView::toggleControls
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::toggleControls()
{
    // This method is put here to demonstrate that widget creation can
    // be postponed using sections in layout xml
    if (!mWidgetsLoaded) {
        // call load widgets to load app DocML and get the pointers to needed widgets
        loadWidgets();
    }

    if (mControlsVisible) {
        hideControls();
    } else {
        showControls();
    }
}


// ---------------------------------------------------------------------------
// CxuiPrecaptureView::mousePressEvent
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //! @todo temporary workaround for title bar mouse event handling bug
    if (event->type() == QEvent::GraphicsSceneMousePress && event->scenePos().y() > 70) {
        toggleControls();
        event->accept();
    }
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::handleZoomLevelChange
// Slot to handle zoom level change signal from zoom control.
// -------------------------------------------------------------------------
//
void CxuiPrecaptureView::handleZoomLevelChange(int zoomValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mSlider && zoomValue != mSlider->value()) {
        CX_DEBUG(( "Zoom level is: %d", zoomValue ));
        mSlider->setValue(zoomValue);
    }

    // reset the timer if it is ongoing
    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::showEvent
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::showEvent(QShowEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();

    QGraphicsWidget::showEvent(event);

    if (event->type() == QEvent::Show) {
        QCoreApplication::instance()->installEventFilter(this);
        event->accept();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::hideEvent
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::hideEvent(QHideEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();

    QGraphicsWidget::hideEvent(event);

    if (event->type() == QEvent::Hide) {
        QCoreApplication::instance()->removeEventFilter(this);
        hideZoom();
        event->accept();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::requestCameraSwitch
// Slot to switch camera. Needed for options menu signals (in DocML).
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::requestCameraSwitch() {
    emit switchCamera();
}

/**
* Handle pressing auto focus key.
*/
void CxuiPrecaptureView::handleAutofocusKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mSettingsDialog) {
        // Prevent us from trying to show grid when dialog closes.
        mSettingsDialog->setStarterAction(NULL);
        mSettingsDialog->close();
    }
    if (mSliderSettingsDialog) {
        mSliderSettingsDialog->setStarterAction(NULL);
        mSliderSettingsDialog->close();
    }

    menu()->close();
    hideControls();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::disableControlsTimeout
// Slot
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::disableControlsTimeout()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.stop();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::releaseCamera
// ---------------------------------------------------------------------------
void CxuiPrecaptureView::releaseCamera()
{
    CX_DEBUG_ENTER_FUNCTION();
    mEngine->cameraDeviceControl().release();
    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiPrecaptureView::initCamera
// ---------------------------------------------------------------------------
void CxuiPrecaptureView::initCamera()
{
    CX_DEBUG_ENTER_FUNCTION();
    mEngine->initMode(mEngine->mode());
    CX_DEBUG_EXIT_FUNCTION();
}


/*
 * viewfinder state call backs
 */
void CxuiPrecaptureView::handleVfStateChanged(
    CxeViewfinderControl::State newState, CxeError::Id /*error*/)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (newState == CxeViewfinderControl::Running) {
        // switch off screen saver and turn on backlight
        if(mDisplayHandler) {
            mDisplayHandler->setDisplayAlwaysVisible(true);
        }
        // start the standby timer
        emit startStandbyTimer();
    } else {
        // switch on screen saver and turn off backlight
        if(mDisplayHandler) {
            mDisplayHandler->setDisplayAlwaysVisible(false);
        }
        // stop the standby timer when vf is not running.
        emit stopStandbyTimer();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*
 * CxuiPrecaptureView::handleFocusGained
 */
void CxuiPrecaptureView::handleFocusGained()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Set the window size and handle again.
    prepareWindow();
    initCamera();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle setting dialog showing up.
* If the setting dialog was launched from the setting grid, hide grid now.
* @param action Action that launched the dialog.
*/
void CxuiPrecaptureView::prepareToShowDialog(HbAction *action)
{
    if (!action) {
        return;
    }

    // Check if the dialog was started from grid, and hide the grid now if needed.
    QString fromGrid = action->property(PROPERTY_KEY_SETTING_GRID).toString();
    if (fromGrid.compare(QString(PROPERTY_KEY_TRUE)) == 0 ) {
        hideSettingsGrid();
    }
}

/*!
* Handle setting dialog closing down.
* If the setting dialog was launched from the setting grid, show grid again now.
* @param action Action that launched the dialog.
*/
void CxuiPrecaptureView::prepareToCloseDialog(HbAction *action)
{
    if (!action) {
        return;
    }

    // Check if the dialog was started from grid, and show the grid now if needed.
    // Autofocus key-press will clear the action to disable showing the grid.
    QString fromGrid = action->property(PROPERTY_KEY_SETTING_GRID).toString();
    if (fromGrid.compare(QString(PROPERTY_KEY_TRUE)) == 0 ) {
        showSettingsGrid();
    }

    // Clear the starter actions to be sure they are not reused.
    if (mSettingsDialog) {
        mSettingsDialog->setStarterAction(NULL);
    }
    if (mSliderSettingsDialog) {
        mSliderSettingsDialog->setStarterAction(NULL);
    }
}



/*!
*Helper method to create setting dialog
*/
CxuiSettingDialog* CxuiPrecaptureView::createSettingsDialog()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Loading popup DocML"));
    CxuiDocumentLoader *documentLoader = new CxuiDocumentLoader(mEngine);

    // Use document loader to create popup
    bool ok = false;
    documentLoader->load(SETTINGS_DIALOG_XML, &ok);
    CX_DEBUG(("load ok=%d", ok));

    CxuiSettingDialog *dialog =
        qobject_cast<CxuiSettingDialog*>(documentLoader->findWidget(SETTINGS_DIALOG_WIDGET));
    CX_ASSERT_ALWAYS(dialog);

    dialog->setTimeout(HbDialog::NoTimeout);
    dialog->setBackgroundFaded(false);

    mSettingsDialogHeading = qobject_cast<HbLabel *>(
                                    documentLoader->findWidget(SETTINGS_DIALOG_HEADING_WIDGET));
    CX_ASSERT_ALWAYS(mSettingsDialogHeading);

    mSettingsDialogList = qobject_cast<CxuiSettingRadioButtonList *>(
                                    documentLoader->findWidget(SETTINGS_DIALOG_CONTENT_WIDGET));
    CX_ASSERT_ALWAYS(mSettingsDialogList);

    QObject *object = documentLoader->findObject(SETTINGS_DIALOG_OK_ACTION);
    mSettingsDialogOkAction = qobject_cast<HbAction *>(object);
    CX_DEBUG_ASSERT(mSettingsDialogOkAction);

    CX_DEBUG_EXIT_FUNCTION();

    delete documentLoader;
    documentLoader = NULL;

    return dialog;
}



/*!
*Helper method to create setting dialog
*/
void CxuiPrecaptureView::launchSettingsDialog(QObject* action)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!action) {
        return;
    }

    hideControls();

    QString key = action->property(PROPERTY_KEY_SETTING_ID).toString();
    CX_DEBUG(("settingsKey=%s", key.toAscii().constData()));

    RadioButtonListParams data;

    // Load data and check that the setting is supported.
    if (mSettingsInfo && mSettingsInfo->getSettingsContent(key, data)) {

        if (!mSettingsDialog) {
            mSettingsDialog = createSettingsDialog();

            connect(mSettingsDialog, SIGNAL(dialogAboutToShow(HbAction *)),
                    this, SLOT(prepareToShowDialog(HbAction *)));
            connect(mSettingsDialog, SIGNAL(dialogAboutToClose(HbAction *)),
                    this, SLOT(prepareToCloseDialog(HbAction *)));
        }

        if (mSettingsDialogHeading) {
            mSettingsDialogHeading->setPlainText(data.mHeading);
        }

        if (mSettingsDialogList) {
            mSettingsDialogList->init(&data);
        }

        if (mSettingsDialog) {
            // Store action that starts the dialog.
            // Dialog passes this along with it's signals,
            // so we can easily decide if we show grid again or not.
            mSettingsDialog->setStarterAction(qobject_cast<HbAction *>(action));

            if (mSettingsDialogOkAction) {
                // disconnect ok action from dialog so that dialog won't be closed
                // automatically when action is triggered. handleSelectionAccepted
                // is called instead
                mSettingsDialogOkAction->disconnect(mSettingsDialog);
                connect(mSettingsDialogOkAction, SIGNAL(triggered()), mSettingsDialogList, SLOT(handleSelectionAccepted()));

                // Close the dialog when new setting value is committed
                connect(mSettingsDialogList, SIGNAL(selectionCommitted()), mSettingsDialog, SLOT(close()));

                connect(mSettingsDialog, SIGNAL(aboutToClose()), mSettingsDialogList, SLOT(handleClose()));
            }

            // Adjust position and show the dialog.
            mSettingsDialog->setPreferredPos(getDialogPosition(), HbPopup::BottomRightCorner);
            mSettingsDialog->show();
        }
    } else {
        launchNotSupportedNotification();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
*Helper method to create slider setting dialog
*/
CxuiSettingDialog* CxuiPrecaptureView::createSliderSettingsDialog()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Loading dialog DocML"));
    CxuiDocumentLoader *documentLoader = new CxuiDocumentLoader(mEngine);

    // Use document loader to create popup
    bool ok = false;
    documentLoader->load(SETTINGS_SLIDER_DIALOG_XML, &ok);
    CX_DEBUG(("load ok=%d", ok));

    CxuiSettingDialog *dialog =
        qobject_cast<CxuiSettingDialog *>(documentLoader->findWidget(SETTINGS_SLIDER_DIALOG_WIDGET));
    CX_ASSERT_ALWAYS(dialog);

    dialog->setTimeout(HbDialog::NoTimeout);
    dialog->setBackgroundFaded(false);

    mSliderSettingsDialogHeading = qobject_cast<HbLabel *>(
                                    documentLoader->findWidget(SETTINGS_SLIDER_DIALOG_HEADING_WIDGET));
    CX_ASSERT_ALWAYS(mSliderSettingsDialogHeading);

    mSettingsSlider = qobject_cast<CxuiSettingSlider *>(
                                    documentLoader->findWidget(SETTINGS_SLIDER_DIALOG_CONTENT_WIDGET));
    CX_ASSERT_ALWAYS(mSettingsSlider);

    QObject *object = documentLoader->findObject(SETTINGS_DIALOG_OK_ACTION);
    mSliderSettingsDialogOkAction = qobject_cast<HbAction *>(object);
    CX_DEBUG_ASSERT(mSliderSettingsDialogOkAction);

    CX_DEBUG_EXIT_FUNCTION();

    delete documentLoader;
    documentLoader = NULL;

    return dialog;

}


void CxuiPrecaptureView::launchSliderSetting()
{
    CX_DEBUG_ENTER_FUNCTION();

    QObject *action = sender();
    if (!action) {
         return;
    }

    hideControls();

    QString key = action->property(PROPERTY_KEY_SETTING_ID).toString();
    CX_DEBUG(("settingsKey=%s", key.toAscii().constData()));

    SliderParams data;

    // Load data and check that the setting is supported.
    if (mSettingsInfo && mSettingsInfo->getSettingsContent(key, data)) {

        if (!mSliderSettingsDialog) {
            mSliderSettingsDialog = createSliderSettingsDialog();

            connect(mSliderSettingsDialog, SIGNAL(dialogAboutToShow(HbAction *)),
                    this, SLOT(prepareToShowDialog(HbAction *)));
            connect(mSliderSettingsDialog, SIGNAL(dialogAboutToClose(HbAction *)),
                    this, SLOT(prepareToCloseDialog(HbAction *)));
        }

        if (mSliderSettingsDialogHeading) {
            mSliderSettingsDialogHeading->setPlainText(data.mHeading);
        }

        if (mSettingsSlider) {
            mSettingsSlider->init(&data);
        }

        if(mSliderSettingsDialog && mSettingsSlider) {
            // Store action that starts the dialog.
            // Dialog passes this along with it's signals,
            // so we can easily decide if we show grid again or not.
            mSliderSettingsDialog->setStarterAction(qobject_cast<HbAction *>(action));

            if (mSliderSettingsDialogOkAction) {
                // disconnect ok action from dialog so that dialog won't be closed
                // automatically when action is triggered. handleSelectionAccepted
                // is called instead
                mSliderSettingsDialogOkAction->disconnect(mSliderSettingsDialog);
                connect(mSliderSettingsDialogOkAction, SIGNAL(triggered()), mSettingsSlider, SLOT(handleSelectionAccepted()));

                // Close the dialog when new setting value is committed
                connect(mSettingsSlider, SIGNAL(selectionCommitted()), mSliderSettingsDialog, SLOT(close()));

                connect(mSliderSettingsDialog, SIGNAL(aboutToClose()), mSettingsSlider, SLOT(handleClose()));
            }

            // Adjust position and show the dialog.
            mSliderSettingsDialog->setFrameType(HbPopup::Weak);
            mSliderSettingsDialog->setPreferredPos(getDialogPosition(), HbPopup::BottomRightCorner);
            mSliderSettingsDialog->show();
        }
    } else {
        launchNotSupportedNotification();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiPrecaptureView::launchPhotosApp()
{
    QProcess::startDetached(PhotosAppExe);
}

/*!
 * Launching Videos application as a separate process
 */
void CxuiPrecaptureView::launchVideosApp()
{
    //Releasing cameda device in order to free
    //graphical memory
    releaseCamera();
    QProcess::startDetached(VideosAppExe);
}

/*!
* Show "not supported" notification.
*/
void CxuiPrecaptureView::launchNotSupportedNotification()
{
    CX_DEBUG_ENTER_FUNCTION();
    HbNotificationDialog::launchDialog("Notification", "Not supported yet");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Show "Disk full" notification.
*/
void CxuiPrecaptureView::launchDiskFullNotification()
{
    CX_DEBUG_ENTER_FUNCTION();
    HbMessageBox::warning(hbTrId("txt_cam_info_memory_full"));
    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Show settings grid.
*/
void CxuiPrecaptureView::showSettingsGrid()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Create grid if it's not shown before.
    initializeSettingsGrid();

    // Show grid modally.
    // This call will still not block until grid is dismissed.
    if(mSettingsGrid) {
        mSettingsGrid->setModal(true);
        mSettingsGrid->show();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Hide the settings grid dialog.
*/
void CxuiPrecaptureView::hideSettingsGrid()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mSettingsGrid) {
        mSettingsGrid->hide();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Get if postcapture view should be shown or not.
* Postcapture view may be shown for a predefined time or
* until user dismisses it, or it may be completely disabled.
*/
bool CxuiPrecaptureView::isPostcaptureOn() const
{
    CX_DEBUG_ENTER_FUNCTION();
    if (CxuiServiceProvider::isCameraEmbedded()) {
        // always show post capture in embedded mode
        return true;
    }

    // Read the value from settings. Ignoring reading error.
    // On error (missing settings) default to "postcapture on".
    int showPostcapture(-1);
    QString key;
    if(mEngine) {
        if (mEngine->mode() == Cxe::ImageMode) {
            key = CxeSettingIds::STILL_SHOWCAPTURED;
        } else {
            key = CxeSettingIds::VIDEO_SHOWCAPTURED;
        }
        mEngine->settings().get(key, showPostcapture);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return showPostcapture != 0; // 0 == no postcapture
}

/*!
* Adding zoom buttons to the slider
* \param slider Pointer to the slider object, where the buttons will be added
*/
void CxuiPrecaptureView::addIncreaseDecreaseButtons(CxuiZoomSlider *slider)
{
    // get current slider elements
    QList<QVariant> elements = slider->sliderElements();

    // add increase and decrease elements to the slider
    elements << HbSlider::IncreaseElement << HbSlider::DecreaseElement;
    slider->setSliderElements(elements);

    // set icons for the increase and decrease element
    slider->setElementIcon(HbSlider::DecreaseElement , HbIcon("qtg_mono_minus"));
    slider->setElementIcon(HbSlider::IncreaseElement , HbIcon("qtg_mono_plus"));
}


/*!
* Function can be used to create a graphics item and setting it as a background
* item for HbWidget. graphicName refers to system wide graphic name. Given graphic
* can consist of one, three or nine pieces. Nine piece graphics are used by default.
* See HbFrameDrawer documentation for graphic naming.
*/
void CxuiPrecaptureView::createWidgetBackgroundGraphic(HbWidget *widget,
                                                       const QString &graphicName,
                                                       HbFrameDrawer::FrameType frameType)
{
    if (widget) {
        HbFrameDrawer *drawer = new HbFrameDrawer(graphicName, frameType);

        if (drawer) {
            HbFrameItem *backgroundItem = new HbFrameItem(drawer, widget);
            if (backgroundItem) {
                // set item to fill the whole widget
                backgroundItem->setGeometry(QRectF(QPointF(0, 0), widget->size()));
                backgroundItem->setZValue(0);
                widget->setBackgroundItem(backgroundItem);
            }
        }
    }
}

/*!
* Returns dialogs bottom right corner
*/
QPointF CxuiPrecaptureView::getDialogPosition()
{
    HbStyle *style = new HbStyle();
    qreal bottomMargin;
    qreal rightMargin;
    bool ok = style->parameter("hb-param-margin-gene-bottom", bottomMargin);
    CX_ASSERT_ALWAYS(ok);
    ok = style->parameter("hb-param-margin-gene-right", rightMargin);
    CX_ASSERT_ALWAYS(ok);

    // calculate dialog's bottom right position
    QSize deviceRes = mEngine->viewfinderControl().deviceDisplayResolution();
    QPointF point(deviceRes.width() - rightMargin, deviceRes.height() - bottomMargin);

    delete style;

    return point;
}


/*!
    Get setting item icon for given value.
    \param key Setting key (CxeSettingIds)
    \param value Setting value
    \return Returns string containing icon locigal name
*/
QString CxuiPrecaptureView::getSettingItemIcon(const QString &key, QVariant value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxUiSettings::RadioButtonListParams data;
    QString icon = "";
    if (mSettingsInfo && mSettingsInfo->getSettingsContent(key, data)) {
        CxUiSettings::SettingItem setting;
        foreach (setting, data.mSettingPairList) {
            if (setting.mValue == value) {
                icon = setting.mIcon;
                break;
            }
        }
    }

    CX_DEBUG((("Setting icon name [%s]"), icon.toAscii().constData()));
    CX_DEBUG_EXIT_FUNCTION();
    return icon;
}

/*!
    Update the quality indicator
*/
void CxuiPrecaptureView::updateQualityIcon()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mQualityIcon && mEngine) {
        QString key = "";
        QString icon = "";
        int currentValue = -1;

        if (mEngine->mode() == Cxe::VideoMode) {
            key = CxeSettingIds::VIDEO_QUALITY;
        } else {
            key = CxeSettingIds::IMAGE_QUALITY;
        }

        mEngine->settings().get(key, currentValue);
        icon = getSettingItemIcon(key, currentValue);

        mQualityIcon->setIcon(HbIcon(icon));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiPrecaptureView::handleSettingValueChanged(const QString& key, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(key);
    Q_UNUSED(newValue);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update the scene mode icon.
* @param sceneId The new scene id.
*/
void CxuiPrecaptureView::updateSceneIcon(const QString& sceneId)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxuiPrecaptureView - scene: %s", sceneId.toAscii().constData()));

    // No need to update icon, if widgets are not even loaded yet.
    // We'll update the icon once the widgets are loaded.
    if (mWidgetsLoaded) {

        QString key;
        QString iconObjectName;
        if (mEngine->mode() == Cxe::VideoMode) {
            key = CxeSettingIds::VIDEO_SCENE;
            iconObjectName = VIDEO_PRE_CAPTURE_SCENE_MODE_ACTION;
        } else {
            key = CxeSettingIds::IMAGE_SCENE;
            iconObjectName = STILL_PRE_CAPTURE_SCENE_MODE_ACTION;
        }

        QString icon = getSettingItemIcon(key, sceneId);
        CX_DEBUG(("CxuiPrecaptureView - icon: %s", icon.toAscii().constData()));

        if (mDocumentLoader) {
            QObject *obj = mDocumentLoader->findObject(iconObjectName);
            CX_DEBUG_ASSERT(obj);
            qobject_cast<HbAction *>(obj)->setIcon(HbIcon(icon));
        }
    } else {
        CX_DEBUG(("CxuiPrecaptureView - widgets not loaded yet, ignored!"));
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
