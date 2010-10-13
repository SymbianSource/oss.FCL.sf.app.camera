/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class for file server service.
*
*  Copyright © 2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*
*/


// INCLUDE FILES
#include "CamNewFileService.h"
#include "CNewFileServiceBase.h"

#include "CamSettingsModel.h"
#include <f32file.h>
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <AiwGenericParam.h>

#include "CamAppUi.h"

#include "CamUtility.h"
#include "CamAppController.h"

// FORWARD DECLARATIONS

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CCamNewFileService::NewL
// Constructor
// ---------------------------------------------------------------------------
//
CCamNewFileService* CCamNewFileService::NewL()
    {
	return new( ELeave ) CCamNewFileService;
    }

// ---------------------------------------------------------------------------
// CCamNewFileService::~CCamNewFileService
// Destructor
// ---------------------------------------------------------------------------
//
CCamNewFileService::~CCamNewFileService()
  {
  PRINT( _L("Camera => ~CCamNewFileService") );
  if ( !iCompleted && iObserver )
    {	    
    TRAP_IGNORE( iObserver->HandleCompletedNewServiceL( EFalse ) );
    }
  CCamAppUi* appUi = static_cast< CCamAppUi* >( CEikonEnv::Static()->EikAppUi() );
  if ( appUi )
      {
      // Tell appui 'this' is not valid MCamEmbeddedObserver pointer anymore
      appUi->SetEmbeddedObserver( NULL );
      }
  PRINT( _L("Camera <= ~CCamNewFileService") );
  }
    
// ---------------------------------------------------------------------------
// CCamNewFileService::HandleNewFileL
// Receive message from service to record new files
// ---------------------------------------------------------------------------
//   
void CCamNewFileService::HandleNewFileL( MNewFileServiceObserver* /*aObserver*/,
        							  RFile& /*aFile*/,
        							  TNewServiceFileType /*aType*/ )
    {    
  
   
    }

// ---------------------------------------------------------------------------
// CCamNewFileService::HandleNewFileL
// Receive message from service to record new files
// ---------------------------------------------------------------------------
// 
void CCamNewFileService::HandleNewFileL( MNewFileServiceObserver* aObserver,
        							  CDesCArray& aFilenameArray,
        							  TNewServiceFileType aType,
        							  TBool /*aMultipleFiles*/ )
  {
  PRINT( _L("Camera: CCamNewFileService::HandleNewFileL()") );
  
  iCompleted = EFalse;
  
  if( aType != ENewFileServiceImage && aType != ENewFileServiceVideo )
		{
		aObserver->HandleCompletedNewServiceL( EFalse );
		return;
		}
		
	iObserver = aObserver;
  

  TInt mode = KErrNone;
  
  switch ( aType )
    {
    case ENewFileServiceImage:
     	mode = ECamControllerImage;
      break;
    case ENewFileServiceVideo:
    	mode = ECamControllerVideo;               
      break;
    default:
      break;
    }   
        
  iCamFilenameArray = &aFilenameArray;
  
  CAiwGenericParamList* ParamList = GenericParams();
  TInt resolutionsAvailable = ParamList->Count(EGenericParamResolution);
  // set the default requested resolution as 0*0, which is later on
  // interpreted as the default embedded quality.
  TSize requestedResolution(0,0);
  if ( resolutionsAvailable > 0 ) 
  	{
  	TInt index = 0;
  	const TAiwGenericParam* firstReso = ParamList->FindFirst( index, EGenericParamResolution );
  	if ( KErrNotFound != index )
  		{
  		TPtrC8 resData = firstReso->Value().AsData();//&firstReso->Value().AsData();
  		TBuf8<sizeof(TSize)> bufRes;
  		if ( resData.Length() > sizeof(TSize) ) User::Leave( KErrArgument );
  		bufRes.Copy(resData);
  		TDes8& rText = bufRes;
  		TPckgBuf<TSize>* ptr = 
            static_cast<TPckgBuf<TSize>*>( &rText );
        if( !ptr ) User::Leave( KErrNotSupported );
        TSize& params( (*ptr)() );
        requestedResolution.SetSize( params.iWidth, params.iHeight );
        PRINT2( _L("Camera: CCamNewFileService::HandleNewFileL() - Requested resolution: %d * %d"), requestedResolution.iWidth, requestedResolution.iHeight );
  		}
  	}

  CCamAppUi * appUi =
      static_cast< CCamAppUi* >( CEikonEnv::Static()->EikAppUi() );
  appUi->SetRequestedNewFileResolution(requestedResolution);
  appUi->StartAsServerAppL( this, mode ); // 'this' is used for MCamEmbeddedObserver pointer          
  }
    	
// ---------------------------------------------------------------------------
// CCamNewFileService::ServiceCompleteL
// Receive message from service that file service has completed
// and can be closed
// ---------------------------------------------------------------------------
// 
void CCamNewFileService::ServiceCompleteL()
	{
	PRINT( _L("Camcorder: CCamNewFileService::ServiceCompleteL()") );
	iCompleted = ETrue;	

  	CEikonEnv::Static()->EikAppUi()->HandleCommandL( EEikCmdExit );
	}
	
// ---------------------------------------------------------------------------
// CCamNewFileService::FileCompleteL
// Receive command from application that file named aName has been captured.
// (image or video)
// ---------------------------------------------------------------------------
// 
void
CCamNewFileService::FileCompleteL( const TDesC& aName )
	{
    PRINT( _L("Camcorder: CCamNewFileService::FileCompleteL()") );	
	if( iCamFilenameArray != NULL )
		{
		iCamFilenameArray->AppendL( aName );
		}

	iCompleted = ETrue;
	iObserver->HandleCompletedNewServiceL( ETrue );	
	
	
	}
// ---------------------------------------------------------------------------
// CCamNewFileService::AbortL
// Receive command from application that file service must be aborted
// ---------------------------------------------------------------------------
// 
void CCamNewFileService::AbortL()
	{
	PRINT( _L("Camcorder => CCamNewFileService::AbortL()") );
	if( !iCompleted )
		{
		iCompleted = ETrue;
		iObserver->HandleCompletedNewServiceL( EFalse );
		}
    PRINT( _L("Camcorder <= CCamNewFileService::AbortL()") );
	}
	
// End of File
