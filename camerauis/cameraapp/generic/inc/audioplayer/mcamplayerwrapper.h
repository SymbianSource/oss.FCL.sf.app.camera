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
* Description:  Player Wrapper interface and base class
*
*/



#ifndef MCAM_PLAYERWRAPPER_H
#define MCAM_PLAYERWRAPPER_H

// ===========================================================================
// Includes
#include <e32base.h>

// ===========================================================================
// Forward declarations


// ===========================================================================
// Class definitions

/**
* Mixin class for Player Wrappers.
*
*/
class MCamPlayerWrapper
  {
  public: 

    /**
    * Player id
    */
    virtual TInt Id() const = 0;

    /**
    * Play the sound of this player
    */
    virtual void Play( TBool aCallback ) = 0;

    /**
    * Cancel any pending or ongoing playing.
    */
    virtual void CancelPlay() = 0;
  };

// ===========================================================================

/**
* Base class for Player Wrappers.
*
*/
class CCamPlayerWrapperBase : public CBase,
                              public MCamPlayerWrapper
  {
  public:

    /**
    * Destructor.
    * Needed to be able to use this base class pointer for delete.
    */
    virtual ~CCamPlayerWrapperBase() {};

  public: // from MCamPlayerWrapper

    virtual TInt Id() const = 0;

    /**
    * Comparison function to tell if this player's sound
    * is equal to the sound which id is given. There might
    * be a situation where the sound ids are as such different,
    * but still the sound is the same. In that case, the
    * same player could be used for both sounds.
    */
    virtual TBool IsEqualSound( TInt aSoundId ) const = 0;
    
    virtual void Play( TBool aCallback ) = 0;
    virtual void CancelPlay() = 0;
  };


#endif // MCAM_PLAYERWRAPPER_H

// ===========================================================================
// end of file
