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
* Description:  Location Setting item type for Image settings*
*/



// INCLUDE FILES
#include "GSCustomQualitySettingItem.h"
#include "CamLocationSettingPage.h"
#include "CamUtility.h"
#include "GSCamcorderPlugin.h"
#include "CamAppUiBase.h"

#include "camconfiguration.h"


// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CGSCustomQualitySettingItem::CGSCustomQualitySettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCustomQualitySettingItem::CGSCustomQualitySettingItem( 
    TInt                 aIdentifier, 
    TInt&                aValue,
    CCamStaticSettingsModel& aModel,
    TCamCameraMode       aMode )
  : CAknEnumeratedTextPopupSettingItem( aIdentifier, aValue ),
    iModel( aModel ),
    iMode( aMode )
  {
  }


// ---------------------------------------------------------------------------
// CGSCustomQualitySettingItem::~CGSCustomQualitySettingItem
// Destructor.
// ---------------------------------------------------------------------------
//
CGSCustomQualitySettingItem::~CGSCustomQualitySettingItem()
    {
    iSupportedQualityLevels.Close();
    }

// ---------------------------------------------------------------------------
// CGSCustomQualitySettingItem::CompleteConstructionL
// Gets the correct text for a currently selected album
// ---------------------------------------------------------------------------
//	
void CGSCustomQualitySettingItem::CompleteConstructionL()
    {
    PRINT( _L("Camera => CGSCustomQualitySettingItem::CompleteConstructionL") );
    
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
  
  // make sure we only have the supported items in our text array
  CArrayPtr<CAknEnumeratedText>* array = EnumeratedTextArray();
  
  TBool isVideoMode = (ECamControllerVideo == iMode);

  TCamPsiKey key = isVideoMode
                 ? ECamPsiSupportedVideoQualities
                 : ECamPsiSupportedStillQualities;
                 
  iModel.Configuration().GetPsiIntArrayL( key, iSupportedQualityLevels );

  iSupportedQualityLevels.Sort();
  
  // amend list of supported settings
  TInt sCount = iSupportedQualityLevels.Count();
  
  TInt cnt = array->Count();
  TInt j = --sCount;
  TInt i;
   
  for ( i = cnt; --i >= 0; )
    {
    if( j < 0 ||
        i != iModel.Configuration().MapSupportedQualityToQualityDescriptor(         
                      isVideoMode, 
                      iSupportedQualityLevels[j] )
      )
      {
      delete ( array->At( i ) );
      array->Delete( i );
      }
    else
      {
      j--;
      }
    }
  
  // Reset the enum values in the resource, so that the updated list has the
  // new values starting from 0
  for( i = 0; i < array->Count(); i++ )
    {
    array->At(i)->SetEnumerationValue( i );
    }

#ifdef _DEBUG
  PRINT( _L("Camera <> ===============================================") );
  PRINT( _L("Camera <> Quality setting item texts left:") );
  for( i = 0; i < array->Count(); i++ )
    {
    PRINT2( _L("Camera <> Quality (%d): %S"), 
            i, 
            array->At(i)->Value() );
    }
  PRINT( _L("Camera <> ===============================================") );
#endif

    
    PRINT( _L("Camera <= CGSCustomQualitySettingItem::CompleteConstructionL") );
    }



//  End of file
