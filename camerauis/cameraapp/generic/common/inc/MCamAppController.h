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
* Description:  Defines abstract API's for the controller observers*
*/


#ifndef MCAMAPPCONTROLLER_H
#define MCAMAPPCONTROLLER_H

#include "CamSettings.hrh"

#include "CamPSI.h" // TCamEvCompRange

class MCamControllerObserver;
class CCameraUiConfigManager;
/**
* Abstract API for controller observer from the settings plugin. 
* 
*/
class MCamAppController
    {
    public:
        
        /**
        * Add a controller observer.
        * @since 2.8
        * @param aObserver Pointer to an object implementing 
        *                   MCamControllerObserver
        * @return ?description
        */
        virtual void AddControllerObserverL( 
                     const MCamControllerObserver* aObserver ) = 0;

        /**
        * Remove a controller observer
        * @since 2.8
        * @param aObserver Pointer to the observer to remove. The object
        *                  does not have to be added as an observer.
        */
        virtual void RemoveControllerObserver
                     ( const MCamControllerObserver* aObserver ) = 0;
        
        /**
        * Return number of images that can still be captured
        * @since 2.8
        * @param aStorage storage location - defaults to current loation
        * @param aBurstActive - set to ETrue if burst mode is active
        * @return the number of images
        */
        virtual TInt ImagesRemaining( TCamMediaStorage aStorage,
                              TBool            aBurstActive, 
                              TInt             aQualityIndex ) = 0;
        
        /*
        * Return amount of video that can still be captured
        * @since 2.8
        * @return the amount of video in secs
        */
        virtual TTimeIntervalMicroSeconds RecordTimeRemaining() = 0;

        /**
        * Previews a new value for the specified integer setting
        * @param aSettingItem specifies which setting item that want
        * to preview.
        * @param aSettingValue the new integer value for the specified 
        * setting item to be previewed.
        * @since 2.8
        */
        virtual void PreviewSettingChangeL( TInt aSettingItem,
                                            TInt aSettingValue ) = 0;
        
        
        /**
        * Cancels all preview changes, since last commit/cancel.
        * @since 2.8
        */
        virtual void CancelPreviewChangesL() = 0;

        /**
        * Commits last preview change.
        * @since 2.8
        */
        virtual void CommitPreviewChanges() = 0;
        
        /**
        * Restore Camera settings to default
        * @since 3.0
        * @param aIsEmbedded if app is embedded
        */
        virtual void RestoreFactorySettingsL( TBool aIsEmbedded ) = 0;
        
        
        /**
        * Returns the current integer value for the specified setting
        * without the filtering usually performed on storage location.
        * This is neccesary to allow the video/photo settings list to show
        * the selected storage location rather than the forced storage location.
        * @return the current integer setting value
        * @param aSettingItem specifies which setting item that want the value of.
        * @since 2.8
        */
        virtual TInt IntegerSettingValueUnfiltered( TInt aSettingItem ) const = 0;
        
        /**
        * Returns the current integer value for the specified setting
        * @return the current integer setting value
        * @param aSettingItem specifies which setting item that want the value of.
        * @since 2.8
        */
        virtual TInt IntegerSettingValue( TInt aSettingItem ) const = 0;
    
        /**
        * Sets a new value for the specified integer setting
        * @param aSettingItem specifies which setting item that want to set the value of.
        * @param aSettingValue the new integer value for the specified setting item.
        * @since 2.8
        */
        virtual void SetIntegerSettingValueL( TInt aSettingItem, TInt aSettingValue ) = 0;
    
        /**
        * Returns the current text value for the specified setting
        * @return the current text setting value
        * @param aSettingItem specifies which setting item that want the text value of.
        * @since 2.8
        */
        virtual TPtrC TextSettingValue( TInt aSettingItem ) const = 0;
    
        /**
        * Sets a new value for the specified integer setting
        * @param aSettingItem specifies which setting item that want to set the value of.
        * @param aSettingValue the new integer value for the specified setting item.
        * @since 2.8
        */
        virtual void SetTextSettingValueL( TInt aSettingItem,
                                           const TDesC& aSettingValue ) = 0;
        
        /**
        * Increments the engine usage count
        * @since 2.8
        */
        virtual void IncCameraUsers() = 0;

        /**
        * Decrements the engine usage count If the engine is no longer required
        * then it is released. If it is still processing, the engine count will 
        * be checked when the current process completes and the engine will be 
        * released if it is no longer needed.
        * @since 2.8
        */
        virtual void DecCameraUsers() = 0;

        /**
         * Return range of EV value supported for current product
         * @since 3.0
         * @return TEvRange struct
         */
        virtual TCamEvCompRange EvRange() const = 0;

        /**
        * Restart the idle timer
        * @since 2.8
        */
        virtual void StartIdleTimer() = 0;
        
        /*
         * Get handle to CameraUiConfigManager
         */ 
        virtual CCameraUiConfigManager* UiConfigManagerPtr() = 0;
    };

#endif //
        