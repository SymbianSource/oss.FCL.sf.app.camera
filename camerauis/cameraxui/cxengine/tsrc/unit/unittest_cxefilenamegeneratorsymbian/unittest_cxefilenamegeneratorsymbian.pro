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

include(../unittest.pri)

TARGET = unittest_cxefilenamegeneratorsymbian

LIBS += -lplatformenv

SOURCES += unittest_cxefilenamegeneratorsymbian.cpp \
           cxefakesettings.cpp \
           cxeerrormappingsymbian.cpp \
           cxefilenamegeneratorsymbian.cpp \
           cxesysutil.cpp

HEADERS += unittest_cxefilenamegeneratorsymbian.h \
           cxefakesettings.h \
           cxesettings.h \
           cxesysutil.h


