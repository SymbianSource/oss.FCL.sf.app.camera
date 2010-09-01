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
* Description:  ID's for all Camera Plugin Views
*
*/


#ifndef  CAMLOCALVIEWIDS_H
#define  CAMLOCALVIEWIDS_H

#include "CamAppUiBase.h"

//
// Constants used from Camera Settings Plugin
//
const TUid KGSCamImageSettingsViewId = {50};
const TUid KGSCamVideoSettingsViewId = {51};

// Uid to actually launch back the appropriate views from settings application
// and these are used only when camera plugin is launched from camera.
// the values are obtained from "CamAppUiBase.h"

const TUid KImagePreCaptureId = { ECamViewIdStillPreCapture };
const TUid KVideoPreCaptureId = { ECamViewIdVideoPreCapture };

// Following are the message uids which are used to communicate to the camera application
// to handle specific case seperately.

// Message Uid meant for activating appropriate Image/Video Precapture view. 
const TUid KMsgActivatePreCaptureView = {0};

// message uid meant for exiting camera from plugin
const TUid KMsgExitCameraFromPlugin = {1};

// message uid meant for resetting camera settings
const TUid KMsgRestoreSettingsUid = {2};

/*
* Id's to activate the local plugin views; used from both camera and setting plugin
*/
const TInt KGSActivatePhotoSettingsView = 10;
const TInt KGSActivateVideoSettingsView = 20;
const TInt KGSSecondaryCameraPhotoSettingsView = 30;
const TInt KGSSecondaryCameraVideoSettingsView = 40;

#endif //CamLocalViewIds_H
// End of File
