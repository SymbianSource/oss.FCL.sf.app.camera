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

#ifndef CXUISETTINGBUTTONCONTAINER_H_
#define CXUISETTINGBUTTONCONTAINER_H_
#include <hbwidget.h>
#include <QList>
#include "cxuisettingbutton.h"

class CxuiSettingButtonContainer : public HbWidget
{
    Q_OBJECT

public:
    explicit CxuiSettingButtonContainer(QGraphicsItem *parent);


signals:
    void buttonReleased();

public slots:

    void handleButtonPress();


protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private:
    Q_DISABLE_COPY(CxuiSettingButtonContainer)

    QList<CxuiSettingButton *>mButtons;
    bool mListCreated;
    CxuiSettingButton *mPressedDownButton;
    CxuiSettingButton *mPreviouslyPressedDownButton;

};




#endif /* CXUISETTINGBUTTONCONTAINER_H_ */
