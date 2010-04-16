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

include(../../camerax.pri)

TEMPLATE = app
TARGET = cxuitestbat
TARGET.UID3 = 0xE0115615
TARGET.EPOCHEAPSIZE = 0x020000 \
    0x1000000
DEPENDPATH += .

VPATH += ../src \
         ../inc

INCLUDEPATH += . \
               ../../cxengine/inc/api \
               ../../cxengine/inc \
               ../inc \
               ./cxui \
               ../traces

QT += testlib
CONFIG += qtestlib hb symbian_test
HB += hbcore hbwidgets hbtools hbfeedback hbutils

LIBS += -lcxengine \
        -lcommonui \
        -lapmime \
        -lws32 \
        -lbitgdi \
        -lfbscli \
        -lxqsettingsmanager \
        -lxqutils

TARGET.CAPABILITY = ALL -TCB -DRM

# Input
SOURCES += cxuitestbat.cpp \
    cxuiautofocusreticule.cpp \
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
    cxuisettingslider.cpp \
    cxuisettingradiobuttonlist.cpp \
    cxuisettingscenemoderadiobuttonlist.cpp \
    cxuiscenelabel.cpp \
    cxuisettingbutton.cpp \
    cxuisettingbuttoncontainer.cpp \
    cxuiselftimer.cpp \
	cxuierrormanager.cpp \
    cxuistandby.cpp

HEADERS += cxuitestbat.h \
    cxuiautofocusreticule.h \
    cxuiprecaptureview.h \
    cxuistillprecaptureview.h \
    cxuistillprecaptureview2.h \
    cxuivideoprecaptureview.h \
    cxuivideoprecaptureview2.h \
    cxuipostcaptureview.h \
    cxuicapturekeyhandler.h \
    cxuiviewmanager.h \
    cxuienums.h \
    cxuidocumentloader.h \
    cxuidisplaypropertyhandler.h \
	cxuisettingbutton.h \
	cxuisettingslider.h \
    cxuisettingradiobuttonlist.h \
    cxuisettingscenemoderadiobuttonlist.h \
    cxuiscenelabel.h \
	cxuisettingbuttoncontainer.h \
    cxuiselftimer.h \
	cxuierrormanager.h \
    cxuistandby.h \
    ../traces/OstTraceDefinitions.h

RESOURCES += ../cxui.qrc

symbian {
    RSS_RULES = "group_name=\"Qt Camera\";"
}