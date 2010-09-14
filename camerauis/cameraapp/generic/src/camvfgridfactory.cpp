/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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

