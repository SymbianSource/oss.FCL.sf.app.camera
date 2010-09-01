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
* Description:  Wrapper for Tone Player Utility
*
*/


#ifndef CAM_TONEPLAYERWRAPPER_H
#define CAM_TONEPLAYERWRAPPER_H

// ===========================================================================
// Includes
#include <mdaaudiotoneplayer.h>   // for MMdaAudioToneObserver


// ===========================================================================
// Forward declarations
class MCamPlayerObserver;

// ===========================================================================
// Class definitions

/**
* Tone Player Wrapper class
*
*/
class CCamTonePlayerWrapper : public CBase,
                              public MMdaAudioToneObserver
  {
  // =======================================================
  // methods

  // -------------------------------------------------------
  // constructors and destructor
  public: 

    static CCamTonePlayerWrapper* NewL( MCamPlayerObserver& aObserver );
    virtual ~CCamTonePlayerWrapper();

  // -------------------------------------------------------
  // from MMdaAudioToneObserver
  public: 
  
    virtual void MatoPrepareComplete( TInt aStatus );
    virtual void MatoPlayComplete   ( TInt aStatus );
  
  // -------------------------------------------------------
  // new public
  public: 

    void PlayTone( TInt    aToneInHz, 
                   TInt    aLengthInUs, 
                   TReal32 aVolumeInPercent,
                   TBool   aCallback );

    void CancelPlay();

  // -------------------------------------------------------
  // new private
  private: 

    void InitL();
    void NotifyPlayComplete( TInt aStatus );

  // -------------------------------------------------------
  // private constructors
  private: 

    void ConstructL();
    CCamTonePlayerWrapper( MCamPlayerObserver& aObserver );

  // =======================================================
  // data
  private: 

    enum TCamTonePlayerWrapperState
      {
      EIdle         = 0,
      EInitializing = 1,
      EPlaying      = 2
      };

    MCamPlayerObserver&        iObserver;
    CMdaAudioToneUtility*      iTone;
    TCamTonePlayerWrapperState iState;

    TBool                      iCallback;

  // =======================================================
  };

#ifdef _DEBUG
    static const TUint16* KCamTonePlayerWrapperStateNames[] = 
      {
      (const TUint16*)_S16("EIdle"),
      (const TUint16*)_S16("EInitializing"),
      (const TUint16*)_S16("EPlaying")
      };
#endif // _DEBUG


#endif // CAM_TONEPLAYERWRAPPER_H

// ===========================================================================
// end of file
