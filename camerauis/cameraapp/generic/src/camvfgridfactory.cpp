/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implemantation for CCamVfGridFactory class.
*
* Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/


#include "camvfgridinterface.h"

#include "camlinevfgriddrawer.h" // CCamLineVfGridDrawer

//static 
MCamVfGridDrawer* 
CCamVfGridFactory::CreateVfGridDrawerL( TUid aUid )
  {
  switch( aUid.iUid )
    {
    case KCamLineDrawVfGridUid:
      return CCamLineVfGridDrawer::NewL();
    default:
      return NULL;
    }    
  }


