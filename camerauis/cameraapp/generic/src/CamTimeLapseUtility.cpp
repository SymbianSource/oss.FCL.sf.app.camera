/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Utility class for time lapse setting*
*/
  
  
#include <e32base.h>
#include "Cam.hrh"  // ECamCmd...
#include "CamTimeLapseUtility.h"

// CONSTANTS

/* Commented out to avoid compilation warning. Olcay */
/* const TInt KDivisorFactor = 1024; // Avoids using TReal maths, use factor of 2    */
const TInt KSecondInMicSec = 1000000;
    
const TInt KInterval5secs  = 5 * KSecondInMicSec;
const TInt KInterval10secs = 10 * KSecondInMicSec;
const TInt KInterval30secs = 30 * KSecondInMicSec;
const TInt KInterval1min   = 60 * KSecondInMicSec;
const TInt KInterval5min   = 300 * KSecondInMicSec;
const TInt KInterval10min  = 600 * KSecondInMicSec;
const TInt KInterval15min  = 900 * KSecondInMicSec;
const TInt KInterval30min  = 1800 * KSecondInMicSec;

// -----------------------------------------------------------------------------
// CamTimeLapseUtility::IntervalToEnum
// Convert from Interval to Enum
// -----------------------------------------------------------------------------
//   
TCamTimeLapse CamTimeLapseUtility::IntervalToEnum( TTimeIntervalMicroSeconds  aInterval, TBool aBurst )
    {
    switch( aInterval.Int64() )
        {
        case KInterval5secs:
            return ECamTimeLapse5sec;
        case KInterval10secs:
            return ECamTimeLapse10sec;
        case KInterval30secs:
            return ECamTimeLapse30sec;            
        case KInterval1min:
            return ECamTimeLapse1min;
        case KInterval5min:
            return ECamTimeLapse5min;
        case KInterval10min:
            return ECamTimeLapse10min;
        case KInterval15min:
            return ECamTimeLapse15min;
        case KInterval30min:
        		return ECamTimeLapse30min;
        case 0:
            {
              
            // if capturing burst with 0 interval then this is normal sequence mode  
            if ( aBurst )
                {
                return ECamTimeLapseMin;
                }           
            else // otherwise burst is off
                {
                return ECamTimeLapseOff;
                }                
            }
        default: 
            return ECamTimeLapseOff;
        }
    // Should never get here
    }

// -----------------------------------------------------------------------------
// CamTimeLapseUtility::EnumToInterval
// Convert from Enum to Interval
// -----------------------------------------------------------------------------
//   
TTimeIntervalMicroSeconds CamTimeLapseUtility::EnumToInterval( TCamTimeLapse aEnum )
    {
	TInt64 interval = 0;

    switch( aEnum )
        {      
        case ECamTimeLapse5sec:
            interval = KInterval5secs;
            break;
        case ECamTimeLapse10sec:
            interval = KInterval10secs;
            break;
        case ECamTimeLapse30sec:
            interval = KInterval30secs;
            break;            
        case ECamTimeLapse1min:
            interval = KInterval1min;
            break;
        case ECamTimeLapse5min:
            interval = KInterval5min;
            break;
        case ECamTimeLapse10min:
            interval = KInterval10min;
            break;
        case ECamTimeLapse15min:
            interval =  KInterval15min;
            break;
        case ECamTimeLapse30min:
            interval =  KInterval30min;
            break;            
        // 0 means capture as quick as possible (burst mode) but
        // is also used as a default value in single capture            
        case ECamTimeLapseOff:
        case ECamTimeLapseMin:                    
        default:
        	interval = 0;
            break;
        }
    return interval;
    }
    
// -----------------------------------------------------------------------------
// CamTimeLapseUtility::EnumToCommand
// Convert from Enum to capture mode command
// -----------------------------------------------------------------------------
//   
TInt CamTimeLapseUtility::EnumToCommand( TCamTimeLapse aEnum )
    {
    switch( aEnum )
        { 
        case ECamTimeLapseOff:
            return ECamCmdSingleCapture;
        case ECamTimeLapseMin:    
            return ECamCmdSequence;        
        case ECamTimeLapse5sec:
        case ECamTimeLapse10sec:
        case ECamTimeLapse30sec:
        case ECamTimeLapse1min:
        case ECamTimeLapse5min:
        case ECamTimeLapse10min:
        case ECamTimeLapse15min:
        case ECamTimeLapse30min:
            return ECamCmdTimeLapse;         
            
        default: 
            return 0;             
        }
    }

    
//  End of File  

