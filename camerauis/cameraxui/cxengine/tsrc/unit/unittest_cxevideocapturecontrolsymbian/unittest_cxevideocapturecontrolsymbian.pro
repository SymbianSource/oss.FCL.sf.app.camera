# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:
include(../unittest.pri)
TARGET = unittest_cxevideocapturecontrolsymbian

LIBS += -lecamadvsettings
LIBS += -lecam
LIBS += -lecampluginsupport
LIBS += -lfbscli
LIBS += -lbitgdi
LIBS += -lecamdirectviewfinder
LIBS += -lecamsnapshot

LIBS += -lmediaclientvideo
LIBS += -lmediaclientaudio
LIBS += -lmmfcontrollerframework
LIBS += -lplatformenv


DEFINES += CXE_USE_DUMMY_CAMERA
DEFINES = $$unique(DEFINES)
SOURCES += unittest_cxevideocapturecontrolsymbian.cpp \
    cxevideocapturecontrolsymbian.cpp \
    cxevideocapturecontrolsymbianunit.cpp \
    cxefakevideorecorderutility.cpp \
    cxevideorecorderutilitysymbian.cpp \
    cxefakefilenamegenerator.cpp \
    cxedummycamera.cpp \
    cxefakecameradevice.cpp \
    cxecameradevice.cpp \
    cxefakeviewfindercontrol.cpp \
    cxefakecameradevicecontrol.cpp \
    cxefakesettings.cpp \
    cxesettingsimp.cpp \
    cxefakesettingsmodel.cpp \
    cxestatemachine.cpp \
    cxestate.cpp \
    cxestatemachinebase.cpp \
    cxesettingsmappersymbian.cpp \
    cxesoundplayersymbian.cpp \
    cxeerrormappingsymbian.cpp \
    cxefakequalitypresets.cpp \
    cxesysutil.cpp

HEADERS += unittest_cxevideocapturecontrolsymbian.h \
    cxevideocapturecontrolsymbian.h \
    cxevideocapturecontrolsymbianunit.h \
    cxevideorecorderutility.h \
    cxefakevideorecorderutility.h \
    cxevideorecorderutilitysymbian.h \
    cxevideocapturecontrol.h \
    cxefakefilenamegenerator.h \
    cxedummycamera.h \
    cxefakecameradevice.h \
    cxecameradevice.h \
    cxefakeviewfindercontrol.h \
    cxeviewfindercontrol.h \
    cxefakecameradevicecontrol.h \
    cxesettingsimp.h \
    cxesettings.h \
    cxefakesettings.h \
    cxefakesettingsmodel.h \
    cxestatemachine.h \
    cxestate.h \
    cxeerrormappingsymbian.h \
    cxestatemachinebase.h \
    cxecameradevicecontrol.h \
    cxesettingsmappersymbian.h \
    cxesoundplayersymbian.h \
    cxutils.h \
    cxefakequalitypresets.h \
    cxesysutil.h
