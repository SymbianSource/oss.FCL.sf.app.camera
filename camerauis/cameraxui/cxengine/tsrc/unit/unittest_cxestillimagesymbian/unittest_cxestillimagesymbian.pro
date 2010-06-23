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

TARGET = unittest_cxestillimagesymbian

LIBS *= -lplatformenv
LIBS *= -lsysutil

SOURCES *= unittest_cxestillimagesymbian.cpp \
    cxeimagedataitemsymbian.cpp \
    cxestatemachine.cpp \
    cxestatemachinebase.cpp \
    cxestate.cpp \
    cxeerrormappingsymbian.cpp \
    cxestillimagesymbian.cpp \
    cxesysutil.cpp

HEADERS *= unittest_cxestillimagesymbian.h \
    cxestillimagesymbian.h \
    cxeimagedataitemsymbian.h \
    cxeimagedataitem.h \
    cxestatemachine.h \
    cxestatemachinebase.h \
    cxestate.h \
    cxeerrormappingsymbian.h \
    cxutils.h \
    cxesysutil.h
