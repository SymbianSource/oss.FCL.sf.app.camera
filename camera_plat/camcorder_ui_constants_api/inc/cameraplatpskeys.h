/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  P&S keys defined in Camcorder area.
*
*/

#ifndef CAMERAPLATPSKEYS_H
#define CAMERAPLATPSKEYS_H

/**
 *  A namespace container for Camera related P&S keys.
 *  
 *  
 *  In a practical usecase a product has camera slider and it must configure
 *  cenrep value of CameraPlatPSKeys::KPSCameraPlatUid. 
 *  
 *  When camera slider is opened , It must call
 *    
 *             RProperty::Set( CameraPlatPSKeys::KPSCameraPlatUid,
 *                             CameraPlatPSKeys::KLenscoverStatus,
 *                             ( TInt ) CcorUiPropertiesApi::EOpen );
 *
 *  When camera slider is closed , It must call
 *  
 *             RProperty::Set( CameraPlatPSKeys::KPSCameraPlatUid,
 *                             CameraPlatPSKeys::KLenscoverStatus,
 *                             ( TInt ) CcorUiPropertiesApi::EClosed );
 *     
 */
namespace CameraPlatPSKeys
    {
    /**  The category of this P&S keys. */
    const TUid KPSCameraPlatUid = { 0x101F857A }; // UID3 of Cameraapp
    
    /**  Contains the availability state of camera resource.
     *  It can have one of the values of TStatus.
     */
    const TUint32 KLensCoverStatus = 0x00000001;
    
    /**
     * Indicates the status of KLenscoverStatus
     *
     * Possible values are:
     *
     * 1: Lens cover is open and camera is usable
     * 2: Lens cover is closed and camera cannot be used
     *
     */
    enum TStatus
        {        
        EOpen = 1,        
        EClosed
        };
    }

#endif // CAMERAPLATPSKEYS_H
