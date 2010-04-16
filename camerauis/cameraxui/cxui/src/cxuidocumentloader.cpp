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


#include <hbdocumentloader.h>

//#include <hbeffect.h>

#include "cxuidocumentloader.h"
#include "cxuistillprecaptureview.h"
#include "cxuipostcaptureview.h"
#include "cxuistillprecaptureview2.h"
#include "cxuivideoprecaptureview.h"
#include "cxuivideoprecaptureview2.h"
#include "cxutils.h" //CX_DEBUG_ASSERT
#include "cxuisettingdialog.h"
#include "cxuisettingbutton.h"
#include "cxuisettingslider.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuiscenelabel.h"
#include "cxuisettingbuttoncontainer.h"
#include "cxeengine.h"
#include "cxuienums.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuidocumentloaderTraces.h"
#endif


CxuiDocumentLoader::CxuiDocumentLoader(CxeEngine *engine) : HbDocumentLoader(), mEngine(engine)
{
}

QObject *CxuiDocumentLoader::createObject(const QString& type, const QString &name)
{
    OstTrace0( camerax_performance, CXUIDOCUMENTLOADER_CREATEOBJECT, "msg: e_CX_DOCLOADER_CREATEOBJECT 1" );
    QObject *object = NULL;

    if (type == "HbView") {
        if (name == CxUiLayout::STILL_PRE_CAPTURE_VIEW) {
            object = new CxuiStillPrecaptureView();
        } else if (name == CxUiLayout::VIDEO_PRE_CAPTURE_VIEW) {
            object = new CxuiVideoPrecaptureView();
        } else if (name == CxUiLayout::POSTCAPTURE_VIEW) {
            object = new CxuiPostcaptureView();
        }
    } else if (type == "CxuiStillPrecaptureView2") {
        object = new CxuiStillPrecaptureView2();

    } else if (type == "CxuiVideoPrecaptureView2") {
        object = new CxuiVideoPrecaptureView2;

    } else if (type == "HbDialog") {
        if (name == CxUiLayout::SETTINGS_DIALOG_WIDGET
         || name == CxUiLayout::SETTINGS_SLIDER_DIALOG_WIDGET) {
            object = new CxuiSettingDialog();
        }
    } else if (type == "CxuiSettingButton") {
        object = new CxuiSettingButton(NULL, mEngine);

    } else if (type == "CxuiSettingSlider") {
        object = new CxuiSettingSlider(NULL, mEngine);

    } else if (type == "CxuiSettingRadioButtonList") {
        object = new CxuiSettingRadioButtonList(NULL, mEngine);

    } else if (type == "CxuiSceneLabel") {
        object = new CxuiSceneLabel(NULL, mEngine);

    } else if (type == "CxuiSettingButtonContainer") {
        object = new CxuiSettingButtonContainer(NULL);

    }

    if (object) {
        object->setObjectName(name);
    } else {
        object = HbDocumentLoader::createObject(type, name);
    }

    OstTrace0( camerax_performance, DUP1_CXUIDOCUMENTLOADER_CREATEOBJECT, "msg: e_CX_DOCLOADER_CREATEOBJECT 0" );
    return object;
}





