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
#ifndef CXUISCENELABEL_H_
#define CXUISCENELABEL_H_

#include <hblabel.h>

class CxeEngine;

class CxuiSceneLabel : public HbLabel
{
    Q_OBJECT
    Q_PROPERTY(QString settingId READ settingId WRITE setSettingId)
    Q_PROPERTY(QStringList itemValues READ itemValues WRITE setItemValues)
    Q_PROPERTY(QStringList items READ items WRITE setItems)

public:
    explicit CxuiSceneLabel(QGraphicsItem *parent, CxeEngine *engine);

    QString settingId() const;
    void setSettingId(const QString &id);

    QStringList itemValues() const;
    void setItemValues(const QStringList &values);

    QStringList items() const;
    void setItems(const QStringList &values);

signals:

public slots:
    void changeIcon(int);
    
private:
    void setSceneIcon();
    Q_DISABLE_COPY(CxuiSceneLabel)

    QString mSettingId;
    QStringList mItemValues;
    QStringList mItems;
    CxeEngine *mEngine;
};












#endif /* CXUISCENELABEL_H_ */
