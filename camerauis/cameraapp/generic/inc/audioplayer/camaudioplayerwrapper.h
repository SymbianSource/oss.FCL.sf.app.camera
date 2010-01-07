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
* Description:  Wrapper for Audio Player Utility
*
*/


#ifndef CAM_AUDIOPLAYERWRAPPER_H
#define CAM_AUDIOPLAYERWRAPPER_H

// ===========================================================================
// Includes
#include <mdaaudiosampleplayer.h> // for MMdaAudioPlayerCallback
#include "mcamplayerwrapper.h"    // CCamPlayerWrapperBase

// ===========================================================================
// Forward declarations
class MCamPlayerObserver;

// ===========================================================================
// Class definitions

/**
* Audio Player Wrapper class
*
*/
class CCamAudioPlayerWrapper : public CCamPlayerWrapperBase,
                               public MMdaAudioPlayerCallback
  {
  // =======================================================
  // methods

  public: // constructors and destructor

    static CCamAudioPlayerWrapper* NewL( MCamPlayerObserver& aObserver, 
                                         TInt                aSoundId );
    virtual ~CCamAudioPlayerWrapper();

  public: // from CCamPlayerWrapperBase

    virtual TInt  Id          (                 ) const;
    virtual TBool IsEqualSound( TInt  aSoundId  ) const;
    virtual void  Play        ( TBool aCallback );
    virtual void  CancelPlay  (                 );

  public: // from MMdaAudioPlayerCallback
  
    virtual void MapcInitComplete( TInt aStatus, const TTimeIntervalMicroSeconds &aDuration );
    virtual void MapcPlayComplete( TInt aStatus );
  
  public: // new static public
  
    static TPtrC MapSoundId2FilenameL( TInt aSoundId );

  private: // new private

    void InitL();
    void NotifyPlayComplete( TInt aStatus );

  private: // private constructors

    void ConstructL();

    CCamAudioPlayerWrapper( MCamPlayerObserver& aObserver, 
                            TInt                aSoundId );

  // =======================================================
  // data
  private: 

    enum TCamAudioPlayerWrapperState
      {
      EIdle         = 0,
      EInitializing = 1,
      EReady        = 2,
      EPlaying      = 3,
      ECorrupt      = 4
      };

    MCamPlayerObserver&         iObserver;
    TInt                        iSoundId;
    CMdaAudioPlayerUtility*     iPlayer;
    TCamAudioPlayerWrapperState iState;

    TBool                       iPlayRequested;
    TBool                       iCallback;

  // =======================================================
  };

#ifdef _DEBUG
    static const TUint16* KCamAudioPlayerWrapperStateNames[] = 
      {
      (const TUint16*)_S16("EIdle"),
      (const TUint16*)_S16("EInitializing"),
      (const TUint16*)_S16("EReady"),
      (const TUint16*)_S16("EPlaying"),
      (const TUint16*)_S16("ECorrupt")
      };
#endif // _DEBUG

#endif // CAM_AUDIOPLAYERWRAPPER_H

// ===========================================================================
// end of file

