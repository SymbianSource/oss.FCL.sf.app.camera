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

TARGET = unittest_cxesettingscenrepstore

LIBS *= -lxqsettingsmanager

SOURCES *= unittest_cxesettingscenrepstore.cpp \
           cxesettingscenrepstore.cpp

HEADERS *= unittest_cxesettingscenrepstore.h \
           cxutils.h \
           cxeerror.h \
           cxenamespace.h \
           cxecenrepkeys.h \
           cxesettingscenrepstore.h \
           cxetestutils.h
