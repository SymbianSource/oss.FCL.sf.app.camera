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
#include <QApplication>
#include <QGraphicsProxyWidget>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <coemain.h>
#include <eikenv.h>

#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxuistillprecaptureview.h"
#include "cxuivideoprecaptureview.h"
#include "cxuicapturekeyhandler.h"
#include "cxuiviewmanager.h"
#include "cxutils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif

// needed for localization
#include <QTranslator>
#include <QLocale>

#include "cxuiserviceprovider.h"
#include <xqserviceutil.h>

using namespace Cxe;

int main(int argc, char *argv[])
{
    CX_DEBUG(("CxUI: entering main()"));
    OstTrace0( camerax_performance, _MAIN, "msg: e_CX_STARTUP 1" );

    Q_INIT_RESOURCE(cxui);

    OstTrace0( camerax_performance, DUP7__MAIN, "msg: e_CX_HBAPP_CREATION 1" );
    HbApplication app(argc, argv);
    OstTrace0( camerax_performance, DUP8__MAIN, "msg: e_CX_HBAPP_CREATION 0" );

    // Load the language specific localization files: application + common
    QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";

    CX_DEBUG(("CxUI: loading translation"));
    bool ret = false;
    ret = translator.load(path + "camera_" + lang);
    CX_DEBUG(("load ok=%d", ret));
    app.installTranslator( &translator );

    QTranslator commonTranslator;
    commonTranslator.load(path + "common_" + lang);
    app.installTranslator(&commonTranslator);

    OstTrace0( camerax_performance, DUP11__MAIN, "msg: e_CX_HBMAINWINDOW_CREATION 1" );
    HbMainWindow mainWindow(0, Hb::WindowFlagTransparent |
                               Hb::WindowFlagNoBackground);
    mainWindow.setAttribute(Qt::WA_NoBackground);
    OstTrace0( camerax_performance, DUP12__MAIN, "msg: e_CX_HBMAINWINDOW_CREATION 0" );

    // Creating and initializing engine as early as possible.
    // Reserve and power on can then proceed in parallel with
    // ui construction.
    OstTrace0( camerax_performance, DUP1__MAIN, "msg: e_CX_CREATE_ENGINE 1" );
    CxeEngine *eng = CxeEngine::createEngine();
    OstTrace0( camerax_performance, DUP2__MAIN, "msg: e_CX_CREATE_ENGINE 0" );

    if (XQServiceUtil::isService()) {
        // Embedded mode.  Engine is inited to correct mode
        // by service provider when request arrives
        CX_DEBUG(("CxUI: creating serviceprovider"));
        CxuiServiceProvider::create(eng);
        CX_DEBUG(("CxUI: done"));
    } else {
        // Normal mode. Init engine now.
        OstTrace0( camerax_performance, DUP5__MAIN, "msg: e_CX_INIT_ENGINE 1" );
        eng->initMode(Cxe::ImageMode);
        OstTrace0( camerax_performance, DUP6__MAIN, "msg: e_CX_INIT_ENGINE 0" );
    }

    // If the parent of the engine is set to be the
    // HbApplication, then for some reason the engine won't be deleted
    // on shutdown (or at least there will be no traces visible of it)
    //eng->setParent(&app); // HbApplication will now own the engine

    CxuiCaptureKeyHandler keyHandler(*eng);

    OstTrace0( camerax_performance, DUP3__MAIN, "msg: e_CX_CREATE_VIEW_MANAGER 1" );
    CxuiViewManager viewManager(mainWindow, *eng, keyHandler);
    OstTrace0( camerax_performance, DUP4__MAIN, "msg: e_CX_CREATE_VIEW_MANAGER 0" );

    // Setting the viewmanager as the parent of the engine fixes the deletion issue
    eng->setParent(&viewManager);

    OstTrace0( camerax_performance, DUP17__MAIN, "msg: e_CX_HBMAINWINDOWORIENT 1" );
    mainWindow.setOrientation(Qt::Horizontal);
    OstTrace0( camerax_performance, DUP18__MAIN, "msg: e_CX_HBMAINWINDOWORIENT 0" );

    OstTrace0( camerax_performance, DUP13__MAIN, "msg: e_CX_MAINWINDOW_FULLSCREEN 1" );
    mainWindow.showFullScreen();
    OstTrace0( camerax_performance, DUP14__MAIN, "msg: e_CX_MAINWINDOW_FULLSCREEN 0" );

    OstTrace0( camerax_performance, DUP15__MAIN, "msg: e_CX_PREPAREWINDOW 1" );
    viewManager.prepareWindow();

    OstTrace0( camerax_performance, DUP16__MAIN, "msg: e_CX_PREPAREWINDOW 0" );

    int returnValue = app.exec();

    // delete service provider instance
    CxuiServiceProvider::destroy();

    return returnValue;
}
