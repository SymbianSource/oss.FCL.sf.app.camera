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

TEMPLATE = lib
TARGET =
DEPENDPATH += inc inc/api src/dummyengine
INCLUDEPATH += inc \
               inc/sensor \
               inc/api \
               src/dummyengine \
               traces  \
               ../extensions/inc

VPATH += src
CONFIG += dll
MOC_DIR = tmp

symbian {
    TARGET.CAPABILITY = ALL -TCB -DRM
    TARGET.UID3 = 0x20027016
    # Fix for QMake translating INCLUDEPATH to SYSTEMINCLUDE
    # and TraceCompiler needing USERINCLUDE.
    MMP_RULES           += "USERINCLUDE traces"
}

# Libs
LIBS += -lecam
LIBS += -lecamadvsettings
LIBS += -lecamsnapshot
LIBS += -lfbscli
LIBS += -lbitgdi
LIBS += -lgdi
LIBS += -lws32
LIBS += -lcone
LIBS += -lecamdirectviewfinder
LIBS += -lmediaclientvideo
LIBS += -lefsrv
LIBS += -lsysutil
LIBS += -lbafl
LIBS += -lmediaclientaudio
LIBS += -lecom
LIBS += -lecampluginsupport
LIBS += -lmmfcontrollerframework
LIBS += -lplatformenv
LIBS += -lxqsettingsmanager
LIBS += -lsensrvclient
LIBS += -lsensrvutil
LIBS += -limagingconfigmanager
LIBS += -lharvesterclient
LIBS += -lthumbnailmanagerqt
LIBS += -lxqserviceutil
LIBS += -loommonitor
LIBS += -lhal

symbian {
   TARGET.EPOCALLOWDLLDATA = 1 // TODO: check this out??
}

# dependencies
#cxeenginecenrep.sources = cenrep/20027017.txt cenrep/20027018.txt
#cxeenginecenrep.path = /private/10202be9
# deploy it
#DEPLOYMENT += cxeenginecenrep


# Sound files
symbian {
    CXENGINE_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/autoFocus.wav    /epoc32/data/z/system/sounds/digital/autoFocus.wav"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/capture.wav      /epoc32/data/z/system/sounds/digital/capture.wav"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/videoStart.wav   /epoc32/data/z/system/sounds/digital/videoStart.wav"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/data/videoStop.wav    /epoc32/data/z/system/sounds/digital/videoStop.wav"
}

# confml, crml files
symbian {
    CXENGINE_DIR = $$section(PWD,":",1)
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>"
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/CI_camerax.confml                 APP_LAYER_CONFML(CI_camerax.confml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/camerax.confml                    APP_LAYER_CONFML(camerax.confml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/camerax_20027017.crml             APP_LAYER_CRML(camerax_20027017.crml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/cameraxvariation.confml           APP_LAYER_CONFML(cameraxvariation.confml)
    BLD_INF_RULES.prj_exports += "$$CXENGINE_DIR/conf/cameraxvariation_20027018.crml    APP_LAYER_CRML(cameraxvariation_20027018.crml)
}


# Input
HEADERS += cxecameradevicecontrol.h \
           cxecameradevicecontrolsymbian.h \
           cxeengine.h \
           cxeenginesymbian.h \
           cxeviewfindercontrol.h \
           cxeviewfindercontrolsymbian.h \
           cxestillcapturecontrol.h \
           cxestillcapturecontrolsymbian.h \
           cxevideocapturecontrol.h \
           cxevideocapturecontrolsymbian.h \
           cxesettingscontrolsymbian.h \
           cxeautofocuscontrol.h \
           cxeautofocuscontrolsymbian.h \
           cxezoomcontrol.h \
           cxezoomcontrolsymbian.h \
           cxeimagedataqueue.h \
           cxeimagedataqueuesymbian.h \
           cxeimagedataitem.h \
           cxeimagedataitemsymbian.h \
           cxefilenamegeneratorsymbian.h \
           cxecameradevice.h \
           cxutils.h \
           cxeerror.h \
           cxeerrormappingsymbian.h \
           cxesoundplayersymbian.h \
           cxenamespace.h \
           cxesettings.h \
           cxesettingsimp.h \
           cxefeaturemanager.h \
           cxefeaturemanagerimp.h \
           cxesettingsmodel.h \
           cxesettingsmodelimp.h \
           cxestatemachine.h \
           cxestate.h \
           cxestatemachinebase.h \
           cxestillimage.h \
           cxestillimagesymbian.h \
           cxevideorecorderutility.h \
           cxevideorecorderutilitysymbian.h \
           cxedummycamera.h \
           cxesettingsmappersymbian.h \
           cxefilesavethread.h \
           cxefilesavethreadsymbian.h \
           cxesensoreventhandler.h \
           cxesensoreventhandlersymbian.h \
           cxesettingscenrepstore.h \
           cxesysutil.h \
           cxevideocontainer.h \
           cxequalitydetails.h \
           cxequalitypresets.h \
           cxequalitypresetssymbian.h \
           cxeharvestercontrolsymbian.h \
           cxethumbnailmanagersymbian.h \
           cxememorymonitor.h \
           cxememorymonitorprivate.h \
           cxediskmonitor.h \
           cxediskmonitorprivate.h \
           cxesnapshotcontrol.h \
           cxesnapshotcontrolprivate.h \
           sensor/xqsensor.h \
           sensor/xqaccsensor.h \
           sensor/xqdeviceorientation.h \
           sensor/xqdeviceorientation_p.h \
           sensor/xqsensor_p.h \
           sensor/xqaccsensor_p.h \
           traces/OstTraceDefinitions.h

SOURCES += cxecameradevicecontrolsymbian.cpp \
           cxeengine.cpp \
           cxeenginesymbian.cpp \
           cxeviewfindercontrolsymbian.cpp \
           cxestillcapturecontrolsymbian.cpp \
           cxevideocapturecontrolsymbian.cpp \
           cxesettingscontrolsymbian.cpp \
           cxeautofocuscontrolsymbian.cpp \
           cxezoomcontrolsymbian.cpp \
           cxeimagedataqueuesymbian.cpp \
           cxeimagedataitemsymbian.cpp \
           cxefilenamegeneratorsymbian.cpp \
           cxecameradevice.cpp \
           cxesoundplayersymbian.cpp \
           cxesettingsmodelimp.cpp \
           cxesettingsimp.cpp \
           cxefeaturemanagerimp.cpp \
           cxestatemachine.cpp \
           cxestate.cpp \
           cxestatemachinebase.cpp \
           cxeerrormappingsymbian.cpp \
           cxestillimagesymbian.cpp \
           cxevideorecorderutilitysymbian.cpp \
           cxesettingsmappersymbian.cpp \
           cxedummycamera.cpp \
           cxefilesavethread.cpp \
           cxefilesavethreadsymbian.cpp \
           cxesensoreventhandlersymbian.cpp \
           cxesettingscenrepstore.cpp \
           cxesysutil.cpp \
           cxevideocontainer.cpp \
           cxequalitypresetssymbian.cpp \
           cxeharvestercontrolsymbian.cpp \
           cxethumbnailmanagersymbian.cpp \
           cxememorymonitor.cpp \
           cxememorymonitorprivate.cpp \
           cxediskmonitor.cpp \
           cxediskmonitorprivate.cpp \
           cxesnapshotcontrol.cpp \
           cxesnapshotcontrolprivate.cpp \
           sensor/xqsensor.cpp \
           sensor/xqaccsensor.cpp \
           sensor/xqdeviceorientation.cpp \
           sensor/xqdeviceorientation_p.cpp \
           sensor/xqsensor_p.cpp \
           sensor/xqaccsensor_p.cpp




