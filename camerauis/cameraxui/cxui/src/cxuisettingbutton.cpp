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

#include <QGraphicsSceneMouseEvent>
#include <hbdeviceprofile.h> //HbDeviceProfile

#include <hbeffect.h>
#include <hbdialog.h>

#include "cxuisettingbutton.h"
#include "cxutils.h"
#include "cxuidocumentloader.h"
#include "cxeengine.h"
#include "cxesettings.h"


CxuiSettingButton::CxuiSettingButton(QGraphicsItem *parent, CxeEngine *engine)
: HbPushButton(parent), mSettingId(), mSettingValue(0), mSettingPopupXml(), mEngine(engine)
{
    connect(this, SIGNAL(pressed()), this, SLOT(handleClicked()));

}

CxuiSettingButton::~CxuiSettingButton()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiSettingButton::updateIcon(const QString& key, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    // we're only interested in the setting changes that match our id
    if (key == mSettingId){
        int value = newValue.toInt();
        // so we wont overindex
        if (value < mIcons.size()){
            // for this to work the icons need to be in the correct order in the XML
            QString newIcon = mIcons.at(value);

            // to make sure that for some reason there isn't an empty string
            if (!newIcon.isEmpty()){
                CX_DEBUG(("setting icon to: %s",newIcon.toAscii().constData()));
                setIcon(HbIcon(newIcon));
                setBackground(HbIcon(newIcon));
            }else{
                CX_DEBUG(("empty string"));
            }

        }else{
            CX_DEBUG((("index %d is out of bounds"), value));
        }


    }
    CX_DEBUG_EXIT_FUNCTION();
}

QString CxuiSettingButton::settingId() const
{
    return mSettingId;
}

void CxuiSettingButton::setSettingId(const QString &id)
{
    mSettingId = id;
}

QString CxuiSettingButton::scenemodeId() const
{
    return mScenemode;
}

void CxuiSettingButton::setScenemodeId(const QString &scenemode)
{
    mScenemode = scenemode;
}


int CxuiSettingButton::settingValue() const
{
    return mSettingValue;
}

void CxuiSettingButton::setSettingValue(int value)
{
    mSettingValue = value;
}

void CxuiSettingButton::setIconList(const QString &iconList)
{
    CX_DEBUG_ENTER_FUNCTION();
    mIconList = iconList;
    CX_DEBUG(("%s", mIconList.toAscii().data()));
    mIcons = mIconList.split("+");
    CX_DEBUG((("%d"), mIcons.size()));

    // we connect the signal to update the button
    // this is done here so that only those buttons that have an iconlist are connected
    // to the signal
    QObject::connect(&(mEngine->settings()), SIGNAL(settingValueChanged(const QString&,QVariant)),
                this, SLOT(updateIcon(const QString&, QVariant)));


    CX_DEBUG_EXIT_FUNCTION();
}

QString CxuiSettingButton::iconList() const
{
    return mIconList;
}

QString CxuiSettingButton::settingPopupXml() const
{
    return mSettingPopupXml;
}

void CxuiSettingButton::setSettingPopupXml(const QString &popupXml)
{
    mSettingPopupXml = popupXml;
}

void CxuiSettingButton::handleClicked()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mSettingPopupXml.isEmpty()) {
        // If a settings popup XML reference is given, launch popup

        CX_DEBUG(("CxuiSettingButton::handleClicked() - launching popup %s", mSettingPopupXml.toAscii().constData()));

        CxuiDocumentLoader* documentLoader = new CxuiDocumentLoader(mEngine);
        bool ok = false;

        // Use document loader to create popup
        QObjectList objects = documentLoader->load(mSettingPopupXml, &ok);

        CX_DEBUG(("load ok=%d", ok));

        // Set parameters for all loaded popups (almost always only one match, though)
        foreach(QObject* o, objects) {
            HbDialog* popup = qobject_cast<HbDialog*>(o);
            if (popup) {
                popup->setTimeout(HbDialog::NoTimeout);
                popup->setBackgroundFaded(false);
                popup->setAttribute(Qt::WA_DeleteOnClose, true);

                // set the popup place to be at the bottom of the screen
                QRectF popupRect = popup->boundingRect();
                QSizeF screenSize = HbDeviceProfile::profile(popup).logicalSize();
                QRectF screenRect = QRectF(QPointF(0,0), screenSize);

                popupRect.moveBottom(screenRect.bottom());
                popup->setPos(popupRect.topLeft());

                popup->show();

            }
        }

        delete documentLoader;
        documentLoader = NULL;
        TInt trash;
        TInt heapSize;
        User::Heap().Compress();
        heapSize = User::Available( trash  );
        RDebug::Print( _L( "showHeapSize <=> heapSize: %d" ),heapSize);

    } else {
        // Set the setting value directly to the engine
        CxeSettings *settings = NULL;
        if (mEngine) {
            settings = &mEngine->settings();
        }

        CX_DEBUG_ASSERT(settings);
        CX_DEBUG_ASSERT(!mSettingId.isEmpty());

        if (settings && !mSettingId.isEmpty()) {
            CX_DEBUG(("CxuiSettingButton::handleClicked() - setting %s to %d", mSettingId.toAscii().constData(), mSettingValue));
            settings->set(mSettingId, mSettingValue);
        }
    }
    emit clickHandled();


    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiSettingButton::setSetting()
{
    CxeSettings *settings = NULL;
        if (mEngine) {
            settings = &mEngine->settings();
        }

        CX_DEBUG_ASSERT(settings);
        CX_DEBUG_ASSERT(!mSettingId.isEmpty());

        if (settings && !mSettingId.isEmpty()) {
            CX_DEBUG(("CxuiSettingButton::handleClicked() - setting %s to %d", mSettingId.toAscii().constData(), mSettingValue));
            settings->set(mSettingId, mSettingValue);
        }

}



/*
void CxuiSettingButton::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxuiSettingButton::mouseMoveEvent() -  %s to %d", mSettingId.toAscii().constData(), mSettingValue));
    QPointF pos = event->pos();
    QPoint pos2 = pos.toPoint();
    CX_DEBUG(("itemPos: %d %d", pos2.x(), pos2.y()));
    if (!hitButton(pos)){
        CX_DEBUG(("Left the button area"));
        this->clearFocus();

    }



    CX_DEBUG_EXIT_FUNCTION();
}


*/
