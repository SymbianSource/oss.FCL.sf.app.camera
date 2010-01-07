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
* Description:  Camera setting provider, which gives constant settings.
*                Used as test version before real setting are queried from
*                settings model.
*
*/


#ifndef CAM_CONSTANTSETTINGPROVIDER_H
#define CAM_CONSTANTSETTINGPROVIDER_H

#include <e32base.h>
#include "mcamsettingprovider.h"
#include "mcamsettingsmodelobserver.h"


class CCamAppController;


class CCamConstantSettingProvider : public CBase,
//                                    public MCamSettingsModelObserver,
                                    public MCamSettingProvider
  {
  // =========================================================================
  // Constructors and destructor
  public:
    static CCamConstantSettingProvider* NewL ( CCamAppController& aController );
    static CCamConstantSettingProvider* NewLC( CCamAppController& aController );
    virtual ~CCamConstantSettingProvider();
  
  // -------------------------------------------------------------------------
  // From MCamSettingProvider
  public:

    virtual void 
      ProvideCameraParamL( 
        const NCamCameraController::TCamCameraParamId& aParamId,
              TAny*                                    aParamData );

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
  // From MCamSettingsModelObserver
  public:
/*
    virtual void IntegerSettingChangedL( TCamSettingItemIds aSettingItem, 
                                         TInt               aSettingValue );

    virtual void TextSettingChangedL( TCamSettingItemIds aSettingItem, 
                                      const TDesC& aSettingValue );
*/
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

    CCamConstantSettingProvider( CCamAppController& aController );

  // =========================================================================
  // Data
  private:

    CCamAppController& iController;
    RArray<NCamCameraController::TCamCameraSettingId> iPendingSettingChanges;

    RArray<TInt> iSupportedISORates;
    TBool iValidISORates;

  // =========================================================================
  };

#endif // CAM_CONSTANTSETTINGPROVIDER_H
