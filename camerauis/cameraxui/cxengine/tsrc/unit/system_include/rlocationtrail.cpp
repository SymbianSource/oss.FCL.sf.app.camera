/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "rlocationtrail.h"

RLocationTrail::RLocationTrail()
{
}

RLocationTrail::~RLocationTrail()
{
}

TInt RLocationTrail::Connect()
{
    return KErrNone;
}

void RLocationTrail::Close()
{
}

TInt RLocationTrail::StartLocationTrail(TTrailCaptureSetting aState)
{
    return KErrNone;
}

TInt RLocationTrail::StopLocationTrail()
{
    return KErrNone;
}

TInt RLocationTrail::GetLocationTrailState( TTrailState& aState )
{
    return KErrNone;
}

void RLocationTrail::NotifyLocationTrailStateChange( TRequestStatus& aStatus )
{
}

void RLocationTrail::CancelNotificationRequest()
{
}

TInt RLocationTrail::RetrieveLocation( const TTime& aTimeStamp,
                                       TLocationData& aLocationData,
                                       TTrailState& aState )
{
    return KErrNone;
}

void RLocationTrail::CurrentLocation( TRequestStatus& aStatus,
                                      TLocationData& aLocationData)
{
}

void RLocationTrail::CancelLocationRequest()
{
}

TInt RLocationTrail::GetTrailCaptureSetting( TTrailCaptureSetting& aCaptureSetting )
{
    return KErrNone;
}

// end of file
