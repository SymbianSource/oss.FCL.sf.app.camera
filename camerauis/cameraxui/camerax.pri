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


# Auto-detect path for S60 Qt Mobile extensions
exists(/qt/extensions/include)              { INCLUDEPATH += /qt/extensions/include }
exists(/qtextensions/include)               { INCLUDEPATH += /qtextensions/include }
exists(/s60/mw/qtextensions/include)        { INCLUDEPATH += /s60/mw/qtextensions/include }
exists(/sf/mw/qtextensions/include)         { INCLUDEPATH += /sf/mw/qtextensions/include }
exists(/ext/mw/qtextensions/include)        { INCLUDEPATH += /ext/mw/qtextensions/include }
exists(/epoc32/include/oem)                 { INCLUDEPATH += /epoc32/include/oem }
exists(/epoc32/include/middleware)          { INCLUDEPATH += /epoc32/include/middleware }
exists(/epoc32/include/domain/middleware)   { INCLUDEPATH += /epoc32/include/domain/middleware }
exists(/epoc32/include/osextensions)        { INCLUDEPATH += /epoc32/include/osextensions }
exists(/epoc32/include/domain/osextensions) { INCLUDEPATH += /epoc32/include/domain/osextensions }
exists(/epoc32/include/applications)        { INCLUDEPATH += /epoc32/include/applications }
exists(/epoc32/include/domain/applications) { INCLUDEPATH += /epoc32/include/domain/applications }
exists(/epoc32/include/mw)                  { INCLUDEPATH += /epoc32/include/mw }
exists(/epoc32/include/platform/mw)         { INCLUDEPATH += /epoc32/include/platform/mw }
exists(/epoc32/include/platform)            { INCLUDEPATH += /epoc32/include/platform }
exists(/epoc32/include/app)                 { INCLUDEPATH += /epoc32/include/app }
exists(/epoc32/include/platform/app)        { INCLUDEPATH += /epoc32/include/platform/app }
exists(/epoc32/include/ecam)                { INCLUDEPATH += /epoc32/include/ecam }
exists(/epoc32/include/mw/QtTest)           { INCLUDEPATH += /epoc32/include/mw/QtTest }


# FORCE_SECONDARY_CAMERA
# Configuration flag for forcing secondary camera hw to be used also for primary camera ui.
# To be removed
#DEFINES += FORCE_SECONDARY_CAMERA
#DEFINES = $$unique(DEFINES)