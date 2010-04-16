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
 * cxesettings.h
 *
 *  Created on: Dec 30, 2008
 *      
 */
#ifndef CXESETTINGS_H_
#define CXESETTINGS_H_

#include <QObject>
#include <QString>
#include <QVariant>

#include "cxenamespace.h"
#include "cxeerror.h"


// forward declaration
class CxeSettingsModel;



/*
* Class to access all kind of Camera Settings
*/
class CxeSettings : public QObject
{
    
    Q_OBJECT

    public:
        
        /*
        * returns the current integer setting value for the given key
        */
        virtual CxeError::Id get(const QString &key, int &value) const = 0;

        /*
        * returns the current real setting value for the given key
        */
        virtual CxeError::Id get(const QString &key, qreal &value) const = 0;

        /*
        * returns the current string setting value for the given key
        */
        virtual CxeError::Id get(const QString &key, QString &stringValue) const = 0;
        
        /*
        * Returns value of external setting item which is not owned by camera
        */
        virtual void get(long int uid,
                         unsigned long int key,
                         Cxe::SettingKeyType type,
                         QVariant &value) const = 0;
        
        /**
         * Get a value of a certain setting. A convenience method with a template
         * parameter to be used with enumerations.
         *
         * An example:
         * \code
         *  Cxe::Whitebalance wb = Cxe::WhitebalanceAutomatic;
         *  if (settings.get<Cxe::Whitebalance>(CxeSettingIds::WHITE_BALANCE, wb)) {
         *      CX_DEBUG(("Error getting white balance - using default"));
         *  }
         * \endcode
         * @param settingId  Setting key
         * @param value      Reference to a variable where to put the setting value
         * @return           Error code
         */
        template<typename T>
        inline CxeError::Id get(const QString &key, T &value) const {
            int intValue = value; // This will not compile if T cannot be converted to an int
            CxeError::Id err = get(key, intValue);
            value = static_cast<T>(intValue); // Convert to enum
            return err;
        }
        
        /*
        * Set new values for the given key
        */
        virtual CxeError::Id set(const QString &key, int newValue) = 0;

        /*
        * Set new values for the given key
        */
        virtual CxeError::Id set(const QString &key, qreal newValue) = 0;

        /*
        * Set new values for the given key
        */
        virtual CxeError::Id set(const QString &key, const QString &newValue) = 0;

        /*
        * Resets only virtual settings( persistent settings )
        */
        virtual void reset() = 0;
        
    signals:
        /*
        * to notify engine and ui components for a change in a setting value
        */
        void settingValueChanged(const QString &key, QVariant newValue);

        /*
        * to notify engine and ui components for a change in a setting value
        */
        void settingValueChanged(long int uid, unsigned long int key, QVariant value);

        /*
        * to update engine and ui components of new image scene
        */
        void sceneChanged(CxeScene &scene);

    protected:
        CxeSettings() {} 
        
    private:
        Q_DISABLE_COPY( CxeSettings )
};

#endif /*CXESETTINGS_H_*/

// end  of file
