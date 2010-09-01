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
* Description:  Camera setting preview handler interface.
*
*/


#ifndef CAM_MCAMSETTINGPREVIEWHANDLER_H
#define CAM_MCAMSETTINGPREVIEWHANDLER_H

#include "CamSettingsInternal.hrh"

/**
 *  MCamSettingPreviewHandler mixin.
 */
class MCamSettingPreviewHandler
  {
  public:

    /**
    * Activates preview for one setting.
    * Returns the preview value as setting value in ProvideCameraSettingL
    * until Commit or Cancel is called.
    * @param aSettingId    The setting for which preview is set
    * @param aSettingValue The setting value used for preview
    * @leave Any system error code.
    */
    virtual void ActivatePreviewL( const TCamSettingItemIds& aSettingId,
                                         TInt                aSettingValue ) = 0;

    /**
    * Cancels the preview for one setting. 
    * After this call ProvideCameraSettingL returns the actual setting value.
    * If the setting does not have active preview, such error is ignored.
    * @param aSettingId Id of the setting for which the preview is cancelled.
    */
    virtual void CancelPreview( const TCamSettingItemIds& aSettingId ) = 0;
    

    /**
    * Cancels the preview for a set of settings.
    * If some of the settings do not have active preview, such error is ignored.
    */
    virtual void CancelPreviews( const RArray<TCamSettingItemIds> aSettingIds ) = 0;
    
    /**
    * Cancels all previews.
    * No error reported, if no previews active.
    */
    virtual void CancelAllPreviews() = 0;
    
    /**
    * Commits the preview value for the given set of settings.
    * After this call completes, the preview is no more active for the setting.
    * If the call completes successfully, the preview is stored as the
    * actual setting value. If leave occurs, the preview is not stored.
    * @param aSettingId Id of the setting for which the preview is committed.
    * @leave KErrArgument There was no preview value for the given setting.
    * @leave Any system error code.
    */
    virtual void CommitPreviewL( const TCamSettingItemIds& aSettingId ) = 0;

    /**
    * Commits the preview for a set of settings.
    * If some of the settings do not have active preview, such error is ignored.
    */
    virtual void CommitPreviewsL( const RArray<TCamSettingItemIds>& aSettingIds ) = 0;

    /**
    * Commit previews for all settings with active preview.
    * @leave Any system error code.
    */
    virtual void CommitAllPreviewsL() = 0;
    
    /**
    * Get the set of settings with active preview.
    * Preview is active if it has been set with ActivatePreviewL
    * but not cancelled or committed after that.
    * @param aSettingIds Return the ids of the active settings in this array.
    */
    virtual void ActivePreviewsL( RArray<TCamSettingItemIds>& aSettingIds ) const = 0;

    /**
    * Virtual destructor.
    * Need to be defined in order to be able to gracefully delete
    * inherited class entities through MCamCameraPreviewHandler pointer.
    * If not declared here, User-42 panic occurs on delete.
    */
    virtual ~MCamSettingPreviewHandler() {};
  };

#endif // CAM_MCAMSETTINGPREVIEWHANDLER_H

// end of file
