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
* Description:  Setting item type for Image/Video quality.*
*/



// INCLUDE FILES
#include "GSCamQualitySettingItem.h"
#include "GSCamQualitySettingPage.h"
#include "CamUtility.h"
#include "CamStaticSettingsModel.h"

#include "camconfiguration.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------------------------
// CGSCamQualitySettingItem::CGSCamQualitySettingItem
// C++ constructor
// ---------------------------------------------------------------------------
//
CGSCamQualitySettingItem::CGSCamQualitySettingItem(
    TCamCameraMode aMode, 
    TInt aIdentifier, 
    TInt& aValue,
    CCamStaticSettingsModel& aModel )
  : CAknEnumeratedTextPopupSettingItem( aIdentifier, aValue ),
    iMode( aMode ),
    iModel( aModel )
  {
  }


// ---------------------------------------------------------------------------
// CGSCamQualitySettingItem::~CGSCamQualitySettingItem
// Destructor.
// ---------------------------------------------------------------------------
//
CGSCamQualitySettingItem::~CGSCamQualitySettingItem()
  {
  PRINT( _L("Camera => ~CGSCamQualitySettingItem") );
  iSupportedQualitiesArray.Reset();
  PRINT( _L("Camera <= ~CGSCamQualitySettingItem") );
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingItem::CreateSettingPageL
// Framework method for constructing the launching setting page.
// ---------------------------------------------------------------------------
//
CAknSettingPage* CGSCamQualitySettingItem::CreateSettingPageL()
  {
  PRINT( _L("Camera => CGSCamQualitySettingItem::CreateSettingPageL") );
  // Create an instance of our customized setting page.
  TPtrC settingName = SettingName();
  CAknSettingPage* page =  
      new( ELeave ) CGSCamQualitySettingPage( iMode,
                                              settingName,
                                              SettingNumber(), 
                                              EAknCtPopupSettingList,
                                              SettingEditorResourceId(),
                                              SettingPageResourceId(),
                                              *QueryValue() );
  PRINT( _L("Camera <= CGSCamQualitySettingItem::CreateSettingPageL") );
  return page;
  }

// ---------------------------------------------------------------------------
// CGSCamQualitySettingItem::CompleteConstructionL
// 
// ---------------------------------------------------------------------------
//
void CGSCamQualitySettingItem::CompleteConstructionL()
  {
  PRINT( _L("Camera => CGSCamQualitySettingItem::CompleteConstructionL") );
  CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
  
  // make sure we only have the supported items in our text array
  CArrayPtr<CAknEnumeratedText>* array = EnumeratedTextArray();
  
  TBool isVideoMode = (ECamControllerVideo == iMode);

  TCamPsiKey key = isVideoMode
                 ? ECamPsiSupportedVideoQualities
                 : ECamPsiSupportedStillQualities;
                 
  iModel.Configuration().GetPsiIntArrayL( key, iSupportedQualitiesArray );

  iSupportedQualitiesArray.Sort();
  
  // amend list of supported settings
  TInt sCount = iSupportedQualitiesArray.Count();
  
  TInt cnt = array->Count();
  TInt j = --sCount;
  TInt i;
   
  for ( i = cnt; --i >= 0; )
    {
    if( j < 0 ||
        i != iModel.Configuration().MapSupportedQualityToQualityDescriptor(         
                      isVideoMode, 
                      iSupportedQualitiesArray[j] )
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

  PRINT( _L("Camera <= CGSCamQualitySettingItem::CompleteConstructionL") );
  }

//  End of file
