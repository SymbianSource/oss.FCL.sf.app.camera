#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
    # Fix for QMake translating INCLUDEPATH to SYSTEMINCLUDE
    # and TraceCompiler needing USERINCLUDE.
    MMP_RULES           += "USERINCLUDE traces"
}

# export sound file
symbian {
    CXUI_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$$CXUI_DIR/data/selftimer.wav    /epoc32/data/z/system/sounds/digital/selftimer.wav"
}

DEPENDPATH += ../cxengine/inc/api
VPATH += src \
    inc
INCLUDEPATH += inc \
               traces

LIBS += -lecam  \
    -lecamsnapshot \
    -lws32 \
    -lbitgdi \
    -lfbscli \
    -lcxengine \
    -lapmime \
    -lcommonui \
    -lxqutils \
    -lxqservice \
    -lxqserviceutil \
    -lshareui \
    -lcone \
    -lefsrv \
    -lws32 \
    -lgdi \
    -lapgrfx

CONFIG += hb
CONFIG += service
QT += xml
SERVICE.FILE = cxui_service_conf.xml
SERVICE.OPTIONS = embeddable

HB += hbcore hbwidgets hbfeedback hbutils

# Input
HEADERS += cxuiapplication.h \
    cxuiapplicationframeworkmonitor.h \
    cxuiapplicationframeworkmonitorprivate.h \
    cxuiview.h \
    cxuiprecaptureview.h \
    cxuistillprecaptureview.h \
    cxuivideoprecaptureview.h \
    cxuipostcaptureview.h \
    cxestillcapturecontrol.h \
    cxuicapturekeyhandler.h \
    cxuiviewmanager.h \
    cxuidocumentloader.h \
    cxuidisplaypropertyhandler.h \
    cxuisettingdialog.h \
    cxuisettingslider.h \
    cxuiselftimer.h \
    cxuisettingradiobuttonlist.h \
    cxuiscenelabel.h \
    cxuierrormanager.h \
    cxuistandby.h \
    cxuisettingradiobuttonlistmodel.h \
    cxuisettingsinfo.h \
    cxuisettingxmlreader.h \
    cxuiserviceprovider.h \
    cxuiscenemodeview.h \
    cxuizoomslider.h \
    traces/OstTraceDefinitions.h

SOURCES += main.cpp \
    cxuiapplication.cpp \
    cxuiapplicationframeworkmonitor.cpp \
    cxuiapplicationframeworkmonitorprivate.cpp \
    cxuiview.cpp \
    cxuiprecaptureview.cpp \
    cxuivideoprecaptureview.cpp \
    cxuistillprecaptureview.cpp \
    cxuipostcaptureview.cpp \
    cxuicapturekeyhandler.cpp \
    cxuiviewmanager.cpp \
    cxuidocumentloader.cpp \
    cxuidisplaypropertyhandler.cpp \
    cxuisettingdialog.cpp \
    cxuisettingslider.cpp \
    cxuiselftimer.cpp \
    cxuisettingradiobuttonlist.cpp \
    cxuiscenelabel.cpp \
    cxuierrormanager.cpp \
    cxuistandby.cpp \
    cxuisettingradiobuttonlistmodel.cpp \
    cxuisettingsinfo.cpp \
    cxuisettingxmlreader.cpp \
    cxuiserviceprovider.cpp \
    cxuiscenemodeview.cpp \
    cxuizoomslider.cpp

RESOURCES += cxui.qrc

# Variating internal and external icons for scene selection view
SCENEICONDIR = ../internal/icons
exists($$SCENEICONDIR) {
    RESOURCES += cxuiinternalsceneimages.qrc
}
else {
    RESOURCES += cxuiexternalsceneimages.qrc
}

TRANSLATIONS = camera.ts
