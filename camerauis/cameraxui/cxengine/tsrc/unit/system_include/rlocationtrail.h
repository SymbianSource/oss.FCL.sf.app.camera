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

#ifndef R_RLOCATIONTRAIL_H
#define R_RLOCATIONTRAIL_H

#include <e32base.h>
#include <etel3rdparty.h>
#include <lbsposition.h>
#include <locationdatatype.h>

typedef TPckg<TLocality> TLocalityPckg;

/**
 *  RLocationManager dummy.
 */
class RLocationTrail
    {
public:

    enum TTrailState
        {
        ETrailStopped,
        EWaitingGPSData,
        ESearchingGPS,
        ETrailStarted,
        ETrailStopping,
        ETrailStarting
        };
    enum TTrailCaptureSetting
        {
        EOff,
        ECaptureNetworkInfo,
        ECaptureAll
        };

public:
    RLocationTrail();
    ~RLocationTrail();

    TInt Connect();
    void Close();

    TInt StartLocationTrail(TTrailCaptureSetting aState);
    TInt StopLocationTrail();
    TInt GetLocationTrailState( TTrailState& aState );
    void NotifyLocationTrailStateChange( TRequestStatus& aStatus );
    void CancelNotificationRequest();
    TInt RetrieveLocation( const TTime& aTimeStamp,
                           TLocationData& aLocationData,
                           TTrailState& aState );
    void CurrentLocation( TRequestStatus& aStatus,
                          TLocationData& aLocationData);
    void CancelLocationRequest();
    TInt GetTrailCaptureSetting( TTrailCaptureSetting& aCaptureSetting );
    };

#endif // R_RLOCATIONTRAIL_H

// end of file
