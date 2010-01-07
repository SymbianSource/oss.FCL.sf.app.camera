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
* Description:  Main application server class.*
*/


// INCLUDE FILES
#include "CamServer.h"
#include <e32cmn.h>
#include <barsread2.h>

#include <NewFileService.hrh>
#include <NewFileServiceDefs.h>
#include "CamNewFileService.h"

#include "CamUtility.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCamAppServer::CreateServiceL
// Creates new file service object.
// ---------------------------------------------------------------------------
//
CApaAppServiceBase* CCamAppServer::CreateServiceL(TUid aServiceType) const
	{
	if ( aServiceType == TUid::Uid( NEWSERVICEUID ) )// Service UID
		{
		PRINT( _L( "Camera: CCamAppServer:CreateServiceL()" ) );
		CCamNewFileService* service = CCamNewFileService::NewL();
		iNewFileServiceObserver = service;
		return service;
		}
	else
		{
		return CAknAppServer::CreateServiceL(aServiceType);
		}
	}

// ---------------------------------------------------------------------------
// CCamAppServer::NewFileServiceObserver
// Returns pointer to new file sevice object.
// ---------------------------------------------------------------------------
//
MNewFileServiceObserver* CCamAppServer::NewFileServiceObserver() const
	{
	return iNewFileServiceObserver;
	}

// End of File  

