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
#include <QCoreApplication>

#include <hbfeedbackmanager.h>
#include "cxuisettingbutton.h"
#include "cxuisettingbuttoncontainer.h"
#include "cxutils.h"


CxuiSettingButtonContainer::CxuiSettingButtonContainer(QGraphicsItem *parent)
: HbWidget(parent), mListCreated(false), mPressedDownButton(NULL), mPreviouslyPressedDownButton(NULL)
{

}

void CxuiSettingButtonContainer::handleButtonPress()
{
    CX_DEBUG_ENTER_FUNCTION();
    grabMouse();

    if (!mListCreated){
        QList<QGraphicsItem*> buttons = this->childItems();
        CxuiSettingButton *button;
        foreach(QGraphicsItem *item, buttons){
            button = static_cast<CxuiSettingButton*>(item);
            mButtons.append(button);
        }
        mListCreated = true;
    }


    // find the pressed item
    foreach (CxuiSettingButton* button , mButtons) {

        if (button->isDown()) {
            mPressedDownButton = button;
            break;
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiSettingButtonContainer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if (boundingRect().contains(event->pos())) {
        // moving inside the tool bar
        if (!mPressedDownButton || !mPressedDownButton->geometry().contains(event->pos())) {
            if (mPressedDownButton) {
                // lift it up and try to find some other button
                mPressedDownButton->setDown(false);
                mPreviouslyPressedDownButton = mPressedDownButton;
                mPressedDownButton = 0;
            }

            // Find the pressed button
            foreach (CxuiSettingButton* button, mButtons) {
                if (button->geometry().contains(event->pos())) {
                    mPressedDownButton = button;
                    button->setDown(true);
                    button->setSetting();
                    HbFeedbackManager* feedback = HbFeedbackManager::instance();
                    CX_ASSERT_ALWAYS(feedback != NULL);
                    feedback->triggered(button, Hb::InstantDraggedOver);

                    break;
                }
            }
        }
    } else {
        // moving outside the tool bar

        // if a button is pressed down, lift it.
        if (mPressedDownButton) {
            mPressedDownButton->setDown(false);
            mPressedDownButton = 0;
            mPreviouslyPressedDownButton = mPressedDownButton;
        }
    }
}

void CxuiSettingButtonContainer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    ungrabMouse();

    if (mPressedDownButton
        && !mPreviouslyPressedDownButton) {
        // Convert event's scene position to item's coordinate system.
        event->setPos(mPressedDownButton->mapFromScene(event->scenePos()));
        QCoreApplication::sendEvent(mPressedDownButton, event);
        mPressedDownButton = 0;
        emit buttonReleased();
    } else if (mPressedDownButton) {
        mPressedDownButton->setDown(false);
        emit buttonReleased();
    } else
    mPreviouslyPressedDownButton = 0;
}


