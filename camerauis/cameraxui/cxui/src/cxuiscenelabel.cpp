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

#include "cxuiscenelabel.h"
#include "cxeengine.h"
#include "cxesettings.h"
#include "cxutils.h"

CxuiSceneLabel::CxuiSceneLabel(QGraphicsItem *parent, CxeEngine *engine)
: HbLabel(parent), mSettingId(), mItemValues(), mItems(), mEngine(engine)
{
    CX_ASSERT_ALWAYS(engine);
}

QStringList CxuiSceneLabel::itemValues() const
{
    return mItemValues;
}

void CxuiSceneLabel::setItemValues(const QStringList &values)
{
    CX_DEBUG_ENTER_FUNCTION();
    mItemValues = values;
    CX_DEBUG_EXIT_FUNCTION();
}

QStringList CxuiSceneLabel::items() const
{
    return mItems;
}

void CxuiSceneLabel::setItems(const QStringList &values)
{
    CX_DEBUG_ENTER_FUNCTION();
    mItems = values;
    CX_DEBUG_EXIT_FUNCTION();
}

QString CxuiSceneLabel::settingId() const
{
    return mSettingId;
}

void CxuiSceneLabel::setSettingId(const QString &id)
{
    CX_DEBUG_ENTER_FUNCTION();
    mSettingId = id;
    setSceneIcon();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiSceneLabel::setSceneIcon()
{
    CX_DEBUG_ENTER_FUNCTION();

    QString scenevalue;
    if (!mSettingId.isEmpty() && !mItems.isEmpty() && !mItemValues.isEmpty()) {
        int err = mEngine->settings().get(mSettingId, scenevalue);
        int index = mItemValues.indexOf(scenevalue);
        CX_DEBUG((("err: %d index: %d"),err,index));
        setIcon(HbIcon(mItems.at(index)));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiSceneLabel::changeIcon(int index)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (!mItems.isEmpty()) {
        setIcon(HbIcon(mItems.at(index)));
    }
    CX_DEBUG_EXIT_FUNCTION();
}

