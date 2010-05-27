/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include "cxutils.h"
#include "cxeengine.h"
#include "cxeenginesymbian.h"


// This should be the only exported method
EXPORT_C CxeEngine* CxeEngine::createEngine()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeEngineSymbian *res = new CxeEngineSymbian();
    res->construct();
    CX_DEBUG_EXIT_FUNCTION();
    return res;
}

CxeEngine::CxeEngine()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

CxeEngine::~CxeEngine()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}
