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

#include <coemain.h>
#include <eikenv.h>
#include <QApplication>
#include <QGraphicsProxyWidget>
// needed for localization
#include <QTranslator>
#include <QLocale>
#include <hbmainwindow.h>
#include <xqserviceutil.h>

#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxuiapplication.h"
#include "cxuiviewmanager.h"
#include "cxutils.h"
#include "cxuiserviceprovider.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif

using namespace Cxe;

// CONSTANTS
const QString TRANSLATIONS_PATH = "z:/resource/qt/translations/";
const QString TRANSLATIONS_FILE_NAME = "camera_";
const QString COMMON_TRANSLATIONS_FILE_NAME = "common_";

int main(int argc, char *argv[])
{
    CX_DEBUG(("CxUI: entering main()"));
    OstTrace0( camerax_performance, _MAIN, "msg: e_CX_STARTUP 1" );

    Q_INIT_RESOURCE(cxui);

    OstTrace0( camerax_performance, DUP1__MAIN, "msg: e_CX_HBAPP_CREATION 1" );
    CxuiApplication app(argc, argv);
    OstTrace0( camerax_performance, DUP2__MAIN, "msg: e_CX_HBAPP_CREATION 0" );

    // Creating and initializing engine as early as possible.
    // Reserve and power on can then proceed in parallel with
    // ui construction.
    OstTrace0( camerax_performance, DUP7__MAIN, "msg: e_CX_CREATE_ENGINE 1" );
    CxeEngine *eng = CxeEngine::createEngine();
    OstTrace0( camerax_performance, DUP8__MAIN, "msg: e_CX_CREATE_ENGINE 0" );

    if (XQServiceUtil::isService()) {
        // Embedded mode.  Engine is inited to correct mode
        // by service provider when request arrives
        CX_DEBUG(("CxUI: creating serviceprovider"));
        CxuiServiceProvider::create(eng);
        CX_DEBUG(("CxUI: done"));
    } else {
        // Normal mode. Init engine now.
        OstTrace0( camerax_performance, DUP9__MAIN, "msg: e_CX_INIT_ENGINE 1" );
		 //! @todo temporarily commented as part of a hack to change the startup sequence
         // to avoid GOOM issues
        //eng->initMode(Cxe::ImageMode);
        OstTrace0( camerax_performance, DUP10__MAIN, "msg: e_CX_INIT_ENGINE 0" );
    }

    // Load the language specific localization files: application + common
    OstTrace0( camerax_performance, DUP3__MAIN, "msg: e_CX_LOAD_TRANSLATIONS 1" );
    QTranslator translator;
    QString lang = QLocale::system().name();

    CX_DEBUG(("CxUI: loading translation"));
    bool ret = false;
    ret = translator.load(TRANSLATIONS_PATH + TRANSLATIONS_FILE_NAME + lang);
    CX_DEBUG(("load ok=%d", ret));
    app.installTranslator( &translator );

    QTranslator commonTranslator;
    CX_DEBUG(("CxUI: loading common translation"));
    ret = false;
    ret = commonTranslator.load(TRANSLATIONS_PATH + COMMON_TRANSLATIONS_FILE_NAME + lang);
    CX_DEBUG(("load ok=%d", ret));
    app.installTranslator(&commonTranslator);
    OstTrace0( camerax_performance, DUP4__MAIN, "msg: e_CX_LOAD_TRANSLATIONS 0" );

    OstTrace0( camerax_performance, DUP5__MAIN, "msg: e_CX_MAINWINDOW_CREATION 1" );
    HbMainWindow *mainWindow = new HbMainWindow(0, Hb::WindowFlagTransparent |
                                                   Hb::WindowFlagNoBackground);
    mainWindow->setAttribute(Qt::WA_NoBackground);
    OstTrace0( camerax_performance, DUP6__MAIN, "msg: e_CX_MAINWINDOW_CREATION 0" );

    OstTrace0( camerax_performance, DUP11__MAIN, "msg: e_CX_CREATE_VIEW_MANAGER 1" );
    CxuiViewManager *viewManager = new CxuiViewManager(app, *mainWindow, *eng);
    OstTrace0( camerax_performance, DUP12__MAIN, "msg: e_CX_CREATE_VIEW_MANAGER 0" );

    OstTrace0( camerax_performance, DUP13__MAIN, "msg: e_CX_MAINWINDOW_SETORIENTATION 1" );
    mainWindow->setOrientation(Qt::Horizontal);
    OstTrace0( camerax_performance, DUP14__MAIN, "msg: e_CX_MAINWINDOW_SETORIENTATION 0" );

    OstTrace0( camerax_performance, DUP15__MAIN, "msg: e_CX_MAINWINDOW_FULLSCREEN 1" );
    mainWindow->showFullScreen();
    OstTrace0( camerax_performance, DUP16__MAIN, "msg: e_CX_MAINWINDOW_FULLSCREEN 0" );

    OstTrace0( camerax_performance, DUP17__MAIN, "msg: e_CX_PREPAREWINDOW 1" );
    viewManager->prepareWindow();
    OstTrace0( camerax_performance, DUP18__MAIN, "msg: e_CX_PREPAREWINDOW 0" );
    //! @todo initMode call added here as a temporary hack to change the startup sequence
	// in order to avoid GOOM issues
	  User::After(2000000);
    eng->initMode(Cxe::ImageMode);
    int returnValue = app.exec();

    // delete service provider instance
    CxuiServiceProvider::destroy();

    delete viewManager;
    delete mainWindow;
    delete eng;

    return returnValue;
}
