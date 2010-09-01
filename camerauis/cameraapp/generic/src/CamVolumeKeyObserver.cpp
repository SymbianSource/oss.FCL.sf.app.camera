/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Receives buton presses from volume keys*
*/


// INCLUDE FILES

#include "CamVolumeKeyObserver.h"
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include "CamUtility.h"
#include "CamAppUi.h"
#include "CamZoomPane.h"
#include <AknDef.h>
#include <aknconsts.h>



/**
*  Helper class for sending response back to 
*  Remote Controller Framework
*
*  @since 3.1
*/
class CCamRemConKeyResponse : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * aparam Remote Controller instance
        * @return new instance.
        */
        static CCamRemConKeyResponse* NewL( CRemConCoreApiTarget& 
        											aRemConCoreApiTarget );
            
        /**
        * Destructor.
        */
        virtual ~CCamRemConKeyResponse();
       
    public: // new function

		/**
        * Send the any key response back to Remcon server
        * @since 3.1
        * @param aOperationId Remcon operation
        */
        void CompleteAnyKeyL( TRemConCoreApiOperationId aOperationId );
        
    private: //from base class
    
        /**
        * From CActive
        * Called on completion of a request
        * @since 3.1
        */
        void RunL();
        
        /**
        * From CActive
        * Cancels an outstanding request
        * @since 3.1
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        * @since 3.1
        * aparam Remote Controller instance
        */
        CCamRemConKeyResponse( CRemConCoreApiTarget& aRemConCoreApiTarget );
   
    private:
    	
    	// Response array
    	RArray<TRemConCoreApiOperationId> iResponseArray;
    	
    	// Remote controller
    	CRemConCoreApiTarget& iRemConCoreApiTarget;
    };


// -----------------------------------------------------------------------------
// CCamRemConKeyResponse::CCamRemConKeyResponse
// default C++ constructor
// -----------------------------------------------------------------------------
//
CCamRemConKeyResponse::CCamRemConKeyResponse( CRemConCoreApiTarget& 
													aRemConCoreApiTarget )
      : CActive ( EPriorityNormal ),
      	iRemConCoreApiTarget ( aRemConCoreApiTarget )

    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CCamRemConKeyResponse::NewL
// -----------------------------------------------------------------------------
//
CCamRemConKeyResponse* CCamRemConKeyResponse::NewL(CRemConCoreApiTarget& 
														aRemConCoreApiTarget)
    {
    CCamRemConKeyResponse* self = 
        new (ELeave) CCamRemConKeyResponse( aRemConCoreApiTarget );

    return self;
    }

// -----------------------------------------------------------------------------
// CCamRemConKeyResponse::~CCamRemConKeyResponse
// -----------------------------------------------------------------------------
//
CCamRemConKeyResponse::~CCamRemConKeyResponse()
  {
  PRINT( _L("Camera => ~CCamRemConKeyResponse::~CCamRemConKeyResponse"))
  Cancel();
  PRINT( _L("Camera => ~CCamRemConKeyResponse::~CCamRemConKeyResponse B"))
  iResponseArray.Reset();
  PRINT( _L("Camera => ~CCamRemConKeyResponse::~CCamRemConKeyResponse C"))
  iResponseArray.Close();
  PRINT( _L("Camera <= ~CCamRemConKeyResponse::~CCamRemConKeyResponse"))
  }
    
// -----------------------------------------------------------------------------
// CCamRemConKeyResponse::DoCancel
// -----------------------------------------------------------------------------
//
void CCamRemConKeyResponse::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CCamRemConKeyResponse::RunL
// -----------------------------------------------------------------------------
//
void CCamRemConKeyResponse::RunL()
    {
    // if any existing -> Send response
    if ( iResponseArray.Count() )
        {
        CompleteAnyKeyL( iResponseArray[0] );
        // Remove already completed key
        iResponseArray.Remove(0);
        iResponseArray.Compress();
        }
    }

// -----------------------------------------------------------------------------
// CCamConKeyResponse::CompleteAnyKeyL
// -----------------------------------------------------------------------------
//
void CCamRemConKeyResponse::CompleteAnyKeyL( TRemConCoreApiOperationId 
																aOperationId )
  {
  PRINT(_L("Camera => CCamRemConKeyResponse::CompleteAnyKeyL"))
  if ( !IsActive() )
    {
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp:
            {
            PRINT( _L("CCamRemConKeyResponse::CompleteAnyKeyL operation volume up") );
            iRemConCoreApiTarget.VolumeUpResponse( iStatus, KErrNone );
            SetActive();
            }
            break;
        case ERemConCoreApiVolumeDown:
            {
            PRINT( _L("CCamRemConKeyResponse::CompleteAnyKeyL operation volume down") );
            iRemConCoreApiTarget.VolumeDownResponse( iStatus, KErrNone );	
            SetActive();
            }
            break;
        default:
            {
            PRINT( _L("CCamRemConKeyResponse::CompleteAnyKeyL operation - other operation") );
            // Send general response for 'other' keys
            iRemConCoreApiTarget.SendResponse( iStatus, aOperationId, KErrNone );
            SetActive();
            }
            break;
        }
    }
  // already active. Append to array and complete later.
  else
    {
        User::LeaveIfError( iResponseArray.Append( aOperationId ) );
    }
  }


// -----------------------------------------------------------------------------
// CCamRemConObserver::NewL
// -----------------------------------------------------------------------------
//
CCamRemConObserver* CCamRemConObserver::NewL( MCamVolumeKeyObserver &aObserver )
    {
    CCamRemConObserver* self = new ( ELeave ) CCamRemConObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneRemConObserver::~CPhoneRemConObserver
// destructor
// -----------------------------------------------------------------------------
//
CCamRemConObserver::~CCamRemConObserver()
    {
    PRINT( _L("Camera => CCamRemConObserver::~CCamRemConObserver"))
    delete iActiveRemCon;
    
    delete iInterfaceSelector;
    
    PRINT( _L("Camera <= CCamRemConObserver::~CCamRemConObserver"))
    }
 
// -----------------------------------------------------------------------------
// CPhoneRemConObserver::CPhoneRemConObserver
// C++ default constructor
// -----------------------------------------------------------------------------
//
CCamRemConObserver::CCamRemConObserver( MCamVolumeKeyObserver& aObserver ):
	iObserver ( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneRemConObserver::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CCamRemConObserver::ConstructL()
    {
    iInterfaceSelector = CRemConInterfaceSelector::NewL();

	// owned by CRemConInterfaceSelector instance  
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    
    iActiveRemCon = CCamRemConKeyResponse::NewL( *iCoreTarget );
    
    iInterfaceSelector->OpenTargetL();  
    }
  
// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoCommand
// A command has been received. 
// -----------------------------------------------------------------------------
//
void CCamRemConObserver::MrccatoCommand(
        TRemConCoreApiOperationId aOperationId, 
        TRemConCoreApiButtonAction aButtonAct )
    {
    PRINT2( _L("Camera => CCamRemConObserver::MrccatoCommand op (%d) act (%d)"), aOperationId, aButtonAct )
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp:
            {
            // send the response back to Remcon server
			TRAP_IGNORE( iActiveRemCon->CompleteAnyKeyL( aOperationId ) );
			
			// send button press to zoom pane
			iObserver.HandleVolumeKeyEvent( aOperationId, aButtonAct );
            }
            break;
        case ERemConCoreApiVolumeDown:
           	{
			TRAP_IGNORE( iActiveRemCon->CompleteAnyKeyL( aOperationId ) );
			iObserver.HandleVolumeKeyEvent( aOperationId, aButtonAct );
           	}
            break;
        default:
        	{
            // Complete any other operation id
            TRAP_IGNORE( iActiveRemCon->CompleteAnyKeyL( aOperationId ) );
        	}
            break;
        }
    }

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoPlay
// not used
// -----------------------------------------------------------------------------
//
void CCamRemConObserver::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/, 
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
	// not used
 	}

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoTuneFunction
// not used
// -----------------------------------------------------------------------------
//
void CCamRemConObserver::MrccatoTuneFunction( TBool /*aTwoPart*/, 
		TUint /*aMajorChannel*/, 
		TUint /*aMinorChannel*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
    // not used
 	}

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoSelectDiskFunction
// not used
// -----------------------------------------------------------------------------
//
void CCamRemConObserver::MrccatoSelectDiskFunction( TUint /*aDisk*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
	// not used
 	}
	
// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoSelectAvInputFunction
// not used
// -----------------------------------------------------------------------------
//
void CCamRemConObserver::MrccatoSelectAvInputFunction( TUint8 /*aAvInputSignalNumber*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
    // not used
 	}

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoSelectAudioInputFunction
// not used
// -----------------------------------------------------------------------------
//
void CCamRemConObserver::MrccatoSelectAudioInputFunction( TUint8 /*aAudioInputSignalNumber*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
	// not used
 	}

//  End of File
