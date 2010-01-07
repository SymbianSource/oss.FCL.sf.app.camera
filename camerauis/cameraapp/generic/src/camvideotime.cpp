/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of Dynamic Configuration
*
*/


#include "camvideotime.h"

const TReal   KMetaDataCoeff         = 1.03;      // Coefficient to estimate metadata amount
const TUint   KCamCMaxClipDurationInSecs = 5400;  // Maximun video clip duration in seconds
const TReal   KCMRAvgVideoBitRateScaler = 0.9;
const TUint   KDiskSafetyLimit       = 400000;    // Amount of free disk space to leave unused

// -----------------------------------------------------------------------------
// TCamVideoTime::TCamVideoTime
//
// Constructor
// -----------------------------------------------------------------------------
//
TCamVideoTime::TCamVideoTime( TInt64 aDiskSpace,
                              TInt64 aCriticalLimit,             
                              TInt   aVideoBitRate,
                              TInt   aAudioBitRate,
                              TUint  aSizeLimit,
                              TBool  aMuteAudio,
                              TReal  aBitrateScaler )
    {

    // Video bitrate scaler - if not given, use default
    TReal scaler = KCMRAvgVideoBitRateScaler;
    if( aBitrateScaler != 0 )
      {
      scaler = aBitrateScaler;
      }
     
    // Init bitrates    
    iVideoAverageBitRate = static_cast<TInt>( aVideoBitRate * scaler );
    iAudioAverageBitRate = aAudioBitRate;  
          
    if( aMuteAudio )
      {  
      iAudioAverageBitRate = 0;
      }      
        
    // Calculate available disk space, taking into account safety limit
    // and disk critical level limit
    iAvailableSpaceAtStart = aDiskSpace - KDiskSafetyLimit - aCriticalLimit;
    if( iAvailableSpaceAtStart <= 0 )
      {
      iAvailableSpaceAtStart = 0;
      }
    
    // File size limit
    iSizeLimit = aSizeLimit;           
    }               



// -----------------------------------------------------------------------------
// TCamVideoTime::GetRemainingTimeL - from CamC3GPDataSinkImp.cpp 
//
// Return the estimated remaining time for the recording in microseconds.
// This method takes into account the file size and disk full restrictions.
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds TCamVideoTime::GetRemainingTimeL()
    {
    TTimeIntervalMicroSeconds  remaining;
    TInt64 availableSpace;
    TBool remainingFromSizeLimit = EFalse;                   

    if (iSizeLimit && ( (TInt64)iSizeLimit < iAvailableSpaceAtStart ) )
        {
        // use sizelimit as available space.
        remainingFromSizeLimit = ETrue;
        }

    // Use average audio/video bitrates to estimate remaining time
    TUint  averageBitRate;
    TUint  averageByteRate;

    averageBitRate = (TUint)((iVideoAverageBitRate + iAudioAverageBitRate) * KMetaDataCoeff);
    averageByteRate = averageBitRate / 8;

    if (remainingFromSizeLimit)
        {
        availableSpace = iSizeLimit;
        }
    else
        {
        availableSpace = iAvailableSpaceAtStart;
        }

    if (availableSpace <= 0)
        {
        remaining = 0;
        }
    else
        {
        remaining = availableSpace * 1000000 / averageByteRate; // 1000000 is for conversion between microseconds and seconds

        if ( (remaining.Int64()) > (TInt64(KCamCMaxClipDurationInSecs)*1000000) )
            {
            remaining = (TInt64(KCamCMaxClipDurationInSecs)*1000000);
            }
        }

    if ( remaining <= TInt64(0) )
        {
        remaining = 0;
        }
    return remaining;
    }

// End of file  
