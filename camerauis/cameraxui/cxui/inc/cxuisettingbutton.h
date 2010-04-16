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
#ifndef CXUISETTINGBUTTON_H
#define CXUISETTINGBUTTON_H


#include <hbpushbutton.h>

class CxeEngine;

class CxuiSettingButton : public HbPushButton
{
    Q_OBJECT
    Q_PROPERTY(QString settingId READ settingId WRITE setSettingId)
    Q_PROPERTY(int settingValue READ settingValue WRITE setSettingValue)
    Q_PROPERTY(QString settingPopupXml READ settingPopupXml WRITE setSettingPopupXml)
    Q_PROPERTY(QString iconList READ iconList WRITE setIconList)
    Q_PROPERTY(QString scenemodeId READ scenemodeId WRITE setScenemodeId)
public:
    explicit CxuiSettingButton(QGraphicsItem *parent, CxeEngine *engine);
    virtual ~CxuiSettingButton();

    QString settingId() const;
    void setSettingId(const QString &id);
    int settingValue() const;
    QString iconList() const;
    void setIconList(const QString &iconList);
    void setSettingValue(int value);
    QString settingPopupXml() const;
    void setSettingPopupXml(const QString &popupXml);
    QString scenemodeId() const;
    void setScenemodeId(const QString &scenemode);
    void setSetting();


signals:
    void clickHandled();

public slots:
    void handleClicked();

    void updateIcon(const QString& key, QVariant newValue);
private:
    Q_DISABLE_COPY(CxuiSettingButton)

    QString mSettingId;
    int mSettingValue;
    QString mSettingPopupXml;
    CxeEngine *mEngine;
    QString mIconList;
    QStringList mIcons;
    QString mScenemode;
};

#endif // CXUISETTINGBUTTON_H
