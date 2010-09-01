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
* Description:  Player Observer interface
*
*/


#ifndef MCAM_PLAYEROBSERVER_H
#define MCAM_PLAYEROBSERVER_H

// ===========================================================================
// Includes


// ===========================================================================
// Forward declarations


// ===========================================================================
// Class definitions

/**
* Player Observer interface
*
*/
class MCamPlayerObserver
  {
  public:
    virtual void PlayComplete( TInt aStatus, TInt aSoundId ) = 0;
  };

#endif MCAM_PLAYEROBSERVER_H

// ===========================================================================
// end of file
