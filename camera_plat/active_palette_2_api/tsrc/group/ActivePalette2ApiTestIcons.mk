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

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
    ZDIR = $(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
    ZDIR = $(EPOCROOT)epoc32\data\z
endif

# ---------------------------------------
# Source and target folders
#
SOURCEDIR     = ..\data
TARGETDIR     = $(ZDIR)\Resource\Apps
HEADERDIR     = $(EPOCROOT)epoc32\include
TRG_FILENAME  = ActivePalette2ApiTest.mif
HDR_FILENAME  = ActivePalette2ApiTest.mbg
OPTIONS       = /c8,1



do_nothing :
	@rem do_nothing
MAKMAKE : do_nothing
BLD : do_nothing
CLEAN :
	if exist $(HEADERDIR)\$(HDR_FILENAME) @del $(HEADERDIR)\$(HDR_FILENAME)
	if exist $(TARGETDIR)\$(TRG_FILENAME) @del $(TARGETDIR)\$(TRG_FILENAME)
LIB : do_nothing
CLEANLIB : do_nothing
RESOURCE :

	mifconv $(TARGETDIR)\$(TRG_FILENAME) -H$(HEADERDIR)\$(HDR_FILENAME) \
	$(OPTIONS) $(SOURCEDIR)\zeroth.svg \
	$(OPTIONS) $(SOURCEDIR)\first.svg \
	$(OPTIONS) $(SOURCEDIR)\second.svg \
	$(OPTIONS) $(SOURCEDIR)\third.svg \
	$(OPTIONS) $(SOURCEDIR)\fourth.svg \
	$(OPTIONS) $(SOURCEDIR)\fifth.svg \
	$(OPTIONS) $(SOURCEDIR)\sixth.svg \
	$(OPTIONS) $(SOURCEDIR)\seventh.svg \
	$(OPTIONS) $(SOURCEDIR)\eighth.svg \
	$(OPTIONS) $(SOURCEDIR)\ninth.svg

FREEZE : do_nothing
SAVESPACE : do_nothing
RELEASABLES :
	@echo $(HEADERDIR)\$(HDR_FILENAME)   && \
	@echo $(TARGETDIR)\$(TRG_FILENAME)
FINAL : do_nothing
