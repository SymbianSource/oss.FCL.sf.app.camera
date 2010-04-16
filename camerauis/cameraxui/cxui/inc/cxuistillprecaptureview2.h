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

#ifndef CXUISTILLPRECAPTUREVIEW2_H
#define CXUISTILLPRECAPTUREVIEW2_H

#include <QMetaType>
#include "cxuistillprecaptureview.h"

/**
 * CxuiPrecaptureView2
 * 
 */
class CxuiStillPrecaptureView2 : public CxuiStillPrecaptureView
{
    Q_OBJECT

public:
    CxuiStillPrecaptureView2(QGraphicsItem *parent = 0);

    virtual ~CxuiStillPrecaptureView2();

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

#endif /* CXUISTILLPRECAPTUREVIEW2_H */
