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
#include <hbframeitem.h>

#include "cxenamespace.h"
#include "cxesettings.h"
#include "cxuiscenemodeview.h"
#include "cxuienums.h"
#include "cxutils.h"
#include "cxuicapturekeyhandler.h"
#include "cxuisettingsinfo.h"
#include "cxeviewfindercontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxuidocumentloader.h"

using namespace Cxe;
using namespace CxUi;
using namespace CxUiLayout;
using namespace CxUiSettings;
using namespace CxeSettingIds;

const int CXUI_SCENES_TRANSITION_TIME = 350; // 350 milliseconds
const int CXUI_SCENES_CAMERA_TIMEOUT = 60000; // 60 seconds for the camera hw release timeout
const QString CXUI_SCENES_AUTOMATIC_IMAGE=":/camerax/scene_automatic.png";
const QString CXUI_SCENES_LANDSCAPE_IMAGE=":/camerax/scene_landscape.png";
const QString CXUI_SCENES_CLOSEUP_IMAGE=":/camerax/scene_close-up.png";
const QString CXUI_SCENES_PORTRAIT_IMAGE=":/camerax/scene_portrait.png";
const QString CXUI_SCENES_SPORT_IMAGE=":/camerax/scene_sport.png";
const QString CXUI_SCENES_NIGHT_IMAGE=":/camerax/scene_night.png";
const QString CXUI_SCENES_NIGHT_PORTRAIT_IMAGE=":/camerax/scene_nightportrait.png";
const QString CXUI_SCENES_LOW_LIGHT_IMAGE=":/camerax/scene_lowlight.png";


/*!
* Constructor
*/
CxuiSceneModeView::CxuiSceneModeView(QGraphicsItem *parent) :
    HbView(parent),
    mMainWindow(NULL),
    mSettingsInfo(NULL),
    mEngine(NULL),
    mDocumentLoader(NULL),
    mCaptureKeyHandler(NULL),
	mScenesBackground(NULL)
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Destructor
*/
CxuiSceneModeView::~CxuiSceneModeView()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Construct-method handles initialisation tasks for this class. Needs to be called
* before the instance of this class is used.
* @param mainwindow
* @param engine
* @param documentLoader
* @param keyHandler
*/
void CxuiSceneModeView::construct(HbMainWindow *mainwindow,
                               CxeEngine *engine,
                               CxuiDocumentLoader *documentLoader,
                               CxuiCaptureKeyHandler *keyHandler)
{
    CX_DEBUG_ENTER_FUNCTION();

    mMainWindow = mainwindow;
    mDocumentLoader = documentLoader;
    mCaptureKeyHandler = keyHandler;
    mEngine = engine;
    mSettingsInfo = new CxuiSettingsInfo(mEngine);
    setContentFullScreen(true);
    loadDefaultWidgets();

    mCameraReleaseTimer.setInterval(CXUI_SCENES_CAMERA_TIMEOUT);    
    connect(&mCameraReleaseTimer, SIGNAL(timeout()), this, SLOT(releaseCameraHw()), Qt::UniqueConnection);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Load widgets from DocML.
*/
void CxuiSceneModeView::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    QGraphicsWidget *widget = NULL;

    widget = mDocumentLoader->findWidget(SCENE_VIEW_CONTAINER);
    mScenesContainer = qobject_cast<HbWidget *> (widget);

    //Now let's retreive the pointer to icon widget
    widget = mDocumentLoader->findWidget(SCENE_VIEW_BG_IMAGE);
    mScenesBackground = qobject_cast<HbLabel *> (widget);

    widget = mDocumentLoader->findWidget(SCENE_VIEW_BG_IMAGE2);
    mScenesBackground2 = qobject_cast<HbLabel *> (widget);

    //Assuming that the automatic scene mode is always the default one
    CX_DEBUG(("CxuiSceneModeView::loadDefaultWidgets -> Now setting default image"));
    mScenesBackground->setIcon(HbIcon(CXUI_SCENES_AUTOMATIC_IMAGE));

    widget = mDocumentLoader->findWidget(SCENE_VIEW_RADIOBUTTONS);
    mScenesList = qobject_cast<CxuiSettingRadioButtonList *> (widget);

    mTransitionAnimation = new QPropertyAnimation(mScenesBackground2, "opacity");
    mTransitionAnimation->setDuration(CXUI_SCENES_TRANSITION_TIME);

    createWidgetBackgroundGraphic(mScenesContainer, TRANSPARENT_BACKGROUND_GRAPHIC);

    connectSignals();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper function to connect signals needed in this class.
*/
void CxuiSceneModeView::connectSignals()
{
    connect(mScenesList, SIGNAL(itemSelected(int)), this, SLOT(handleSceneRadiobuttonPress(int)));
    connect(mTransitionAnimation, SIGNAL(finished()), this, SLOT(finishScenesTransition()));
}

/*!
* Function can be used to create a graphics item and setting it as a background
* item for HbWidget. graphicName refers to system wide graphic name. Given graphic
* can consist of one, three or nine pieces. Nine piece graphics are used by default.
* See HbFrameDrawer documentation for graphic naming.
*/
void CxuiSceneModeView::createWidgetBackgroundGraphic(HbWidget *widget,
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
* This public method assumes that the view is already properly constructed
*/
void CxuiSceneModeView::loadBackgroundImages()
{
     CX_DEBUG_ENTER_FUNCTION();
    RadioButtonListParams data;
    if (mEngine->mode() == Cxe::ImageMode) {
        mSettingsInfo->getSettingsContent(CxeSettingIds::IMAGE_SCENE, data);
    } else {
        mSettingsInfo->getSettingsContent(CxeSettingIds::VIDEO_SCENE, data);
    }
    mSettingPairList = data.mSettingPairList;
    mScenesList->init(&data);

    if (mScenesBackground) {
        QString sceneId;
        mEngine->settings().get(data.mSettingId, sceneId);
        mScenesBackground->setIcon(HbIcon(backgroundForScene(sceneId)));
    } else {
        //First time displaying a list
        //Assuming that the automatic scene mode is always the default one and is on top of the list
        mScenesList->setSelected(0);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle selecting value in scene list.
*/
void CxuiSceneModeView::handleSceneRadiobuttonPress(int index)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxUiSettings::SettingItem item = mSettingPairList.at(index);
    QString sceneId = item.mValue.toString();
    mScenesBackground2->setIcon(HbIcon(backgroundForScene(sceneId)));
    startBackgroundTransition();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Helper method for getting the background graphic name
* for each of the scenes.
*/
QString CxuiSceneModeView::backgroundForScene(const QString& sceneId)
{
    //!@todo: This mapping should be added to the setting xml.
    if (sceneId == CxeSettingIds::IMAGE_SCENE_AUTO) {
        return CXUI_SCENES_AUTOMATIC_IMAGE;
    } else if (sceneId == CxeSettingIds::IMAGE_SCENE_PORTRAIT) {
        return CXUI_SCENES_PORTRAIT_IMAGE;
    } else if (sceneId == CxeSettingIds::IMAGE_SCENE_SCENERY) {
        return CXUI_SCENES_LANDSCAPE_IMAGE;
    } else if (sceneId == CxeSettingIds::IMAGE_SCENE_MACRO) {
        return CXUI_SCENES_CLOSEUP_IMAGE;
    } else if (sceneId == CxeSettingIds::IMAGE_SCENE_SPORTS) {
        return CXUI_SCENES_SPORT_IMAGE;
    } else if (sceneId == CxeSettingIds::IMAGE_SCENE_NIGHT) {
        return CXUI_SCENES_NIGHT_IMAGE;
    } else if (sceneId == CxeSettingIds::IMAGE_SCENE_NIGHTPORTRAIT) {
        return CXUI_SCENES_NIGHT_PORTRAIT_IMAGE;
    } else if (sceneId == CxeSettingIds::VIDEO_SCENE_AUTO) {
        return CXUI_SCENES_AUTOMATIC_IMAGE;
    } else if (sceneId == CxeSettingIds::VIDEO_SCENE_LOWLIGHT) {
        return CXUI_SCENES_LOW_LIGHT_IMAGE;
    } else if (sceneId == CxeSettingIds::VIDEO_SCENE_NIGHT) {
        return CXUI_SCENES_NIGHT_IMAGE;
    } else {
        return "";
    }

}

/*!
* Start animation for changing the scene background graphic.
*/
void CxuiSceneModeView::startBackgroundTransition()
{
    mTransitionAnimation->setStartValue(0.0);
    mTransitionAnimation->setEndValue(1.0);
    mTransitionAnimation->start();

}

/*!
* Handle scene background graphic animation finishing.
*/
void CxuiSceneModeView::finishScenesTransition()
{
    mScenesBackground->setIcon(mScenesBackground2->icon());
    mScenesBackground2->setOpacity(0);
    mScenesBackground2->setIcon(HbIcon());
}

/*!
* Handle press of ok button.
*/
void CxuiSceneModeView::handleOkButtonPress()
{
    CX_DEBUG_ENTER_FUNCTION();
    mScenesList->handleSelectionAccepted();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle press of cancel button.
*/
void CxuiSceneModeView::handleCancelButtonPress()
{
    CX_DEBUG_ENTER_FUNCTION();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* This view is about to be shown.
* Update the view to match currently selected scene.
*/
void CxuiSceneModeView::showEvent(QShowEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();
    mScenesList->scrollTo(mScenesList->currentIndex(), HbAbstractItemView::PositionAtTop);

    mEngine->viewfinderControl().stop();
    mCameraReleaseTimer.start();
    QGraphicsWidget::showEvent(event);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot to handle capture key full press.
*/
void CxuiSceneModeView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot to handle capture key half press / autofocus key press.
*/
void CxuiSceneModeView::handleAutofocusKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Closing the view and reactivating camera hw if needed
*/
void CxuiSceneModeView::closeView()
{
    CX_DEBUG_ENTER_FUNCTION();
    mScenesList->handleClose();
    mScenesBackground->setIcon(HbIcon());
    // Make sure engine prepares for new image/video if necessary
    mEngine->initMode(mEngine->mode());
    emit viewCloseEvent();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot to handle camera hw release timeout
*/
void CxuiSceneModeView::releaseCameraHw()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCameraReleaseTimer.stop();
    mEngine->cameraDeviceControl().release();
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file

