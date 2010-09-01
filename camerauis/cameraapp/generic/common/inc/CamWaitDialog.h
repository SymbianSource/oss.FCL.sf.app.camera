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
* Description:  Wait dialog that doesn't consume key events*
*/


#ifndef CAMWAITDIALOG_H
#define CAMWAITDIALOG_H

//  INCLUDES
#include <AknWaitDialog.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS   

// CLASS DECLARATION


/**
* Wait Dialog without key event handling
* @since 2.8
*/
class CCamWaitDialog : public CAknWaitDialog
  {
  // =======================================================
 	public: // Constructors and destructor

    /**
    * A class constructor
    *
    * @param    aSelfPtr            Pointer to itself. The pointer needs to be
    *                               valid when the dialog is dismissed and must not
    *                               be on the stack.
    * @param    aVisibilityDelayOff If set ETrue the dialog will be visible
    *                               immediality. Use only when the length of
    *                               the process is ALWAYS over 1.5 seconds.
    */
    CCamWaitDialog( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff );
    
    /**
    * Destructor
    */
    ~CCamWaitDialog();

  public: // From base class
 
    /**
    * From CAknWaitDialog
    * @since 2.8
    * @param aKeyEvent the key event
    * @param aType the type of the event
    * @return TKeyResponse key event was used by this control or not
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                       TEventCode aType );
    
  public: // New methods    

    /**
    * Set keyevents, which the component should not consume. 
    * @since 2.8
    * @param aNonConsumedKeys
    */    
    void SetNonConsumedKeysL( const RArray<TInt>& aNonConsumedKeys );

  // =======================================================
  private: // Data
                                                                                                                                
	  RArray<TInt> iNonConsumedKeys;

  // =======================================================
  };
#endif
// end of file
