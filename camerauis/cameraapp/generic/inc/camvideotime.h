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
* Description:  Class for calculating remaining video time
*                
*  Copyright © 2008 Nokia.  All rights reserved.
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


#ifndef CAMVIDEOTIME_H
#define CAMVIDEOTIME_H

#include <e32base.h>

class TCamVideoTime
  {
  public:
    
    /**
    * Constructor
    * @param aDiskSpace     Remaining disk space
    * @param aCriticalLimit Diskspace critical limit
    * @param aVideoBitrate  Video bitrate
    * @param aAudioBitrate  Audio bitrate
    * @param aSizeLimit     File size limit (MMS message size)
    * @param aMuteAudio     Is audio muted
    * @param aBitrateScaler Video bitrate scaler
    */      
    TCamVideoTime( TInt64 aDiskSpace,
                   TInt64 aCriticalLimit,
                   TInt   aVideoBitRate,
                   TInt   aAudioBitRate,
                   TUint  aSizeLimit,
                   TBool  aMuteAudio,
                   TReal  aBitrateScaler);    
  
  public: 
  
    /**
    * Return the estimated remaining time for the recording in microseconds.
    * @param None
    * @return Remaining time
    */  
    TTimeIntervalMicroSeconds GetRemainingTimeL();
        
  private:
    // Average bitrate of video
    TInt        iVideoAverageBitRate;
    
    // Average bitrate of audio
    TInt        iAudioAverageBitRate;    
    
    // Available space on disk at start of recording
    TInt64      iAvailableSpaceAtStart;

    // Size limit of the file in bytes
    TUint       iSizeLimit;    
  };
  
#endif // CAMVIDEOTIME_H

// End of file  
