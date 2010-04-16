#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

include(../camerax.pri)
include(../rom/camerax_rom.pri)

TEMPLATE = app
TARGET =
symbian {
    TARGET.CAPABILITY   = ALL -Tcb -DRM
    TARGET.UID3         = 0x101f857a
    TARGET.VID          = VID_DEFAULT
    TARGET.EPOCHEAPSIZE = 0x020000 0x1000000
    ICON                = ./icons/cxui.svg
    RSS_RULES           = "group_name=\"Qt Camera\";"
}

DEPENDPATH += ../cxengine/inc/api
VPATH += src \
    inc
INCLUDEPATH += inc \
               ../traces

LIBS += -lecam  \
    -lecamsnapshot \
    -lws32 \
    -lbitgdi \
    -lfbscli \
    -lcxengine \
    -lapmime \
    -lcommonui \
    -lxqsettingsmanager \
    -lxqutils \
    -lalfclient \
    -lxqservice \
    -lxqserviceutil \
    -lshareui

CONFIG += hb
CONFIG += service
QT += xml
SERVICE.FILE = cxui_service_conf.xml
SERVICE.OPTIONS = embeddable

HB += hbcore hbwidgets hbutils hbtools hbfeedback

# Input
HEADERS += cxuiprecaptureview.h \
    cxuistillprecaptureview.h \
    cxuistillprecaptureview2.h \
    cxuivideoprecaptureview.h \
    cxuivideoprecaptureview2.h \
    cxuipostcaptureview.h \
    cxestillcapturecontrol.h \
    cxuicapturekeyhandler.h \
    cxuiviewmanager.h \
    ../traces/OstTraceDefinitions.h \
    cxuidocumentloader.h \
    cxuidisplaypropertyhandler.h \
    cxuisettingdialog.h \
    cxuisettingbutton.h \
    cxuisettingslider.h \
    cxuiselftimer.h \
    cxuisettingradiobuttonlist.h \
    cxuiscenelabel.h \
    cxuisettingbuttoncontainer.h \
    cxuierrormanager.h \
    cxuistandby.h \
    cxuisettingradiobuttonlistmodel.h \
    cxuisettingsinfo.h \
    cxuisettingxmlreader.h \
    cxuiserviceprovider.h

SOURCES += main.cpp \
    cxuiprecaptureview.cpp \
    cxuivideoprecaptureview.cpp \
    cxuivideoprecaptureview2.cpp \
    cxuistillprecaptureview.cpp \
    cxuistillprecaptureview2.cpp \
    cxuipostcaptureview.cpp \
    cxuicapturekeyhandler.cpp \
    cxuiviewmanager.cpp \
    cxuidocumentloader.cpp \
    cxuidisplaypropertyhandler.cpp \
    cxuisettingdialog.cpp \
    cxuisettingbutton.cpp \
    cxuisettingslider.cpp \
    cxuiselftimer.cpp \
    cxuisettingradiobuttonlist.cpp \
    cxuiscenelabel.cpp \
    cxuisettingbuttoncontainer.cpp \
    cxuierrormanager.cpp \
    cxuistandby.cpp \
    cxuisettingradiobuttonlistmodel.cpp \
    cxuisettingsinfo.cpp \
    cxuisettingxmlreader.cpp \
    cxuiserviceprovider.cpp


RESOURCES += cxui.qrc

TRANSLATIONS = camera.ts
