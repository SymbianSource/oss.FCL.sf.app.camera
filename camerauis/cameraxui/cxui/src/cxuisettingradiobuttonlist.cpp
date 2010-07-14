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

#include "cxutils.h"
#include "cxeengine.h"
#include "cxuisettingsinfo.h"
#include "cxesettings.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuisettingradiobuttonlistmodel.h"

CxuiSettingRadioButtonList::CxuiSettingRadioButtonList(QGraphicsItem *parent, CxeEngine *engine)
: HbRadioButtonList(parent),
  mEngine(engine),
  mSettingId(),
  mSettingValues(),
  mOriginalIndex(0)
{
    CX_ASSERT_ALWAYS(mEngine);

    mListModel = new CxuiSettingRadioButtonListModel();
    setModel(mListModel);

    // connect the signals again
    connect(this, SIGNAL(itemSelected(int)), this, SLOT(handleItemSelected(int)));
}


void CxuiSettingRadioButtonList::init(CxUiSettings::RadioButtonListParams *data)
{
    // first we reset the model and clear any previous data
    mSettingValues.clear();
    mListModel->resetModel();

    disconnect(SIGNAL(valueSelected(int)));

    if (data) {
        QStringList settingStrings;
        mSettingValues.clear();

        CxUiSettings::SettingItem setting;
        foreach (setting, data->mSettingPairList) {
            CX_DEBUG(("CxuiSettingRadioButtonList - appending setting value: %s", setting.mValue.toString().toAscii().data()));
            settingStrings.append(setting.mItem); // setting string
            mSettingValues.append(setting.mValue); // engine value for setting
        }

        // Set the setting strings to the model.
        setItems(settingStrings);
        // Set the preview mode.
        // Note: We implement preview ourselves, not with HbRadioButtonList preview mode.
        setPreviewMode(HbRadioButtonList::NoPreview);
        mPreview = data->mPreview;

        setSettingId(data->mSettingId);
        setListBoxType(data->mListboxType);

        initOriginalSelectedItem();
        // ensure that currently selected item is visible
        scrollTo(currentIndex());
    }

}

/*!
 * Sets the original selection of list by value. Can be used to override value read from
 * CxeSettings or used to remember previously selected value in case of setting that is
 * not read from CxeSettings (e.g. selftimer)
 */
void CxuiSettingRadioButtonList::setOriginalSelectedItemByValue(const QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();

    int index = mSettingValues.indexOf(QVariant(value));
    if (index >= 0) {
        mOriginalIndex = index;
        setSelected(index);
        // ensure that currently selected item is visible
        scrollTo(currentIndex());
    } else {
        CX_DEBUG(("Value %s not found", value.toString().toAscii().data()));
    }
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiSettingRadioButtonList::setItems(const QStringList &values)
{
    mListModel->setItems(values);
}

void CxuiSettingRadioButtonList::setListBoxType(int type)
{
    mListModel->setListBoxType(type);
}


void CxuiSettingRadioButtonList::setSettingId(const QString &id)
{
    // Selected item is updated, when this list is shown.
    mSettingId = id;
}



void CxuiSettingRadioButtonList::handleItemSelected(int index)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mPreview) {
        commit(index);
    } else {
        // no action needed
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
*  Get the value currently active in settings.
*/
void CxuiSettingRadioButtonList::initOriginalSelectedItem()
{
    CX_DEBUG_ENTER_FUNCTION();

    QString value;
    int err = mEngine->settings().get(mSettingId, value);
    CX_DEBUG(("CxuiSettingRadioButtonList - original value: [%s]", value.toAscii().data()));

    int index = 0;

    if (err == CxeError::None) {
        index = mSettingValues.indexOf(QVariant(value));
        CX_DEBUG(("CxuiSettingRadioButtonList - got original index of: %d", index));
    }

    mOriginalIndex = index;
    setSelected(mOriginalIndex);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    This slot can be used to set the selection accepted.
*/
void CxuiSettingRadioButtonList::handleSelectionAccepted()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ok button pressed. Update originally selected item,
    // which is the one that always get's set when closing.
    mOriginalIndex = selected();

    emit selectionCommitted();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiSettingRadioButtonList::handleClose()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mSettingId.isEmpty()) {
        // If handleSelectionAccepted was called, this now contains the new index,
        // otherwise the original one is selected.
        commit(mOriginalIndex);
    }

    // clear settings id so setting value doesn't get updated by
    // accident when updating ui appearance
    mSettingId.clear();

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
  Commits value to the cenrep store.
*/
void CxuiSettingRadioButtonList::commit(int index)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("id: %s", mSettingId.toAscii().data()));

    if (!mSettingId.isEmpty() && !mSettingValues.isEmpty()) {
        QVariant value = mSettingValues.at(index);
        if (value.type() == QVariant::Int) {
            CX_DEBUG(("index:%d value:%d", index, value.toInt()));

            // Don't set the value again, if it is the current value.
            // For e.g. video quality it would result in re-preparation etc.
            int current(0);
            CxeError::Id status(mEngine->settings().get(mSettingId, current));

            if (status != CxeError::None || current != value.toInt()) {
                mEngine->settings().set(mSettingId, value.toInt());
            }
            // inform interested clients about value changed event
            emit valueSelected(value.toInt());

        } else if (value.type() == QVariant::String) {
            CX_DEBUG(("index:%d value:[%s]", index, value.toString().toAscii().constData()));

            QString current;
            CxeError::Id status(mEngine->settings().get(mSettingId, current));
            CX_DEBUG(("settings model value:[%s]", current.toAscii().constData()));

            if (status != CxeError::None || current != value.toString()) {
                mEngine->settings().set(mSettingId, value.toString());
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
