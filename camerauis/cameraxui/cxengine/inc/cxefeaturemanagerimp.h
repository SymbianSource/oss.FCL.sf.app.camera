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
* Description:
*
*/
/*
 * cxefeaturemanager.h
 *
 *  Created on: Dec 30, 2008
 *      
 */
#ifndef CXEFEATUREMANAGERIMP_H_
#define CXEFEATUREMANAGERIMP_H_

#include <QMetaType>
#include "cxefeaturemanager.h"
#include "cxeerror.h"

// forward declarations
class CxeSettingsModel;



/*
* Handling and accessing configured run-time values for specific features
* Handles ICM and CxUi run-time features
*/
class CxeFeatureManagerImp : public CxeFeatureManager
{

public:
    
    CxeFeatureManagerImp(CxeSettingsModel& settingsModel);
    virtual ~CxeFeatureManagerImp();

    /*
    * returns if a feature is supported or not
    * true -> supported
    * false -> not supported
    */
    CxeError::Id isFeatureSupported(const QString& key, bool& value) const;
    
    
    /*
    * We retrieve all the configured values for the given key
    */
    CxeError::Id configuredValues(const QString& key,QList<int>& values);

private: // not owned
    CxeSettingsModel& mSettingsModel;

};

#endif /*CXEFEATUREMANAGERIMP_H_*/

// end  of file
