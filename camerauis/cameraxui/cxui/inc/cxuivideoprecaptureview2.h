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

#ifndef CXUIVIDEOPRECAPTUREVIEW2_H
#define CXUIVIDEOPRECAPTUREVIEW2_H

#include <QObject>
#include "cxuivideoprecaptureview.h"

/**
 * CxuiVideoPrecaptureView2
 * 
 */
class CxuiVideoPrecaptureView2 : public CxuiVideoPrecaptureView
{
    Q_OBJECT

public:
    CxuiVideoPrecaptureView2(QGraphicsItem *parent = 0);

    virtual ~CxuiVideoPrecaptureView2();

public:

    /**
     * Loads default widgets in layouts xml.
     */
    virtual void loadDefaultWidgets();

    /**
     * Loads widgets that are not part of the default section in layouts xml.
     * Widgets are created at the time they are first loaded.
     */
    virtual void loadWidgets();

};
#endif /* CXUIVIDEOPRECAPTUREVIEW2_H */
