# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

# This unit test doesn't want to include system include path of real xqsettingsmanager api
CONFIG *= cxeunit_no_system_paths

include(../unittest.pri)

INCLUDEPATH *= ../system_include
DEPENDPATH  *= ../system_include

TARGET = unittest_cxefilesavethreadsymbian

LIBS *= -lsysutil
LIBS *= -lplatformenv
LIBS *= -lefsrv
LIBS *= -lcone

SOURCES *= unittest_cxefilesavethreadsymbian.cpp \
           thumbnailmanager_qt.cpp \
           harvesterclient.cpp \
           cxefilesavethread.cpp \
           cxefilesavethreadsymbian.cpp \
           cxefakeimagedataitem.cpp \
           cxeimagedataitemsymbian.cpp \
           cxeharvestercontrolsymbian.cpp \
           cxethumbnailmanagersymbian.cpp \
           cxesysutil.cpp \
           cxestatemachinebase.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxeerrormappingsymbian.cpp

HEADERS *= unittest_cxefilesavethreadsymbian.h \
           thumbnailmanager_qt.h \
           harvesterclient.h \
           cxefilesavethread.h \
           cxefilesavethreadsymbian.h \
           cxefakeimagedataitem.h \
           cxeimagedataitemsymbian.h \
           cxeimagedataitem.h \
           cxeerror.h \
           cxeharvestercontrolsymbian.h \
           cxethumbnailmanagersymbian.h \
           cxesysutil.h \
           cxestatemachinebase.h \
           cxestatemachine.h \
           cxestate.h \
           cxeerrormappingsymbian.h

