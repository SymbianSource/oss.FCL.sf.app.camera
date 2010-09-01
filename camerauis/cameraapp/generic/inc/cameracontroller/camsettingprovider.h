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
* Description:  Camera setting provider, which queries the settings from settingmodel.
*
*/


#ifndef CAM_SETTINGPROVIDER_H
#define CAM_SETTINGPROVIDER_H

#include <e32base.h>
#include "mcamsettingprovider.h"
#include "mcamsettingsmodelobserver.h"
 

class MCamSettingsModel;
class CCamAppController;

class CCamSettingProvider : public CBase,
                            //public MCamSettingsModelObserver,
                            public MCamSettingProvider
  {
  // =========================================================================
  // Constructors and destructor
  public:
    static CCamSettingProvider* NewL ( CCamAppController& aController, MCamSettingsModel* aSettingsModel );
    static CCamSettingProvider* NewLC( CCamAppController& aController, MCamSettingsModel* aSettingsModel );
    virtual ~CCamSettingProvider();
  
  // -------------------------------------------------------------------------
  // From MCamSettingProvider
  public:
    virtual void 
      ProvideCameraParamL( 
        const NCamCameraController::TCamCameraParamId&   aParamId, 
              TAny*                                      aParamData );

    virtual void
      ProvideCameraSettingL( 
        const NCamCameraController::TCamCameraSettingId& aSettingId,
              TAny*                                      aSettingData );

    virtual void
      ProvidePendingSettingChangesL(
        RArray<NCamCameraController::TCamCameraSettingId>& aSettingIds );

    virtual TInt PendingSettingChangeCount() const;

    /**
    * Set the supported ISO rates.
    */
    virtual void SetSupportedISORatesL( const RArray<TInt>& aSupportedIsoRates );

  // -------------------------------------------------------------------------
  // New methods
  public:
  
    /**
    * Clear all pending setting changes.
    */
    void Reset();

    /**
    * Add an array of pending setting changes.
    */  
    void AddPendingSettingChangesL( 
            const RArray<TCamSettingItemIds>& aUiSettingIds );
    
    /**
    * Add one pending setting change.
    */
    void AddPendingSettingChangeL( TCamSettingItemIds aUiSettingId );

  // -------------------------------------------------------------------------
  // Private constructors
  private:
  
    void ConstructL();

    CCamSettingProvider( CCamAppController& aController, MCamSettingsModel* aSettingsModel );

  // =========================================================================
  // Data
  private:

    MCamSettingsModel* iSettingsModel;
    CCamAppController& iController;
    RArray<NCamCameraController::TCamCameraSettingId> iPendingSettingChanges;
    
    TInt iEvStepAsUiValue;
    
    RArray<TInt> iSupportedISORates;
    TBool iValidISORates;

  // =========================================================================
  };

#endif // CAM_CONSTANTSETTINGPROVIDER_H
