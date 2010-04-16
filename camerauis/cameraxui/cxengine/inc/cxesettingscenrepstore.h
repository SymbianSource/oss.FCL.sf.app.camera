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
#ifndef  CXESETTINGSCENREPSTORE_H_
#define  CXESETTINGSCENREPSTORE_H_

//  Include Files
#include <QHash>
#include <QVariant>
#include <QMetaType>

#include "cxeerror.h"
#include "cxenamespace.h"
#include "xqsettingsmanager.h"

// forward declarations
class XQSettingsManager;
class XQSettingsKey;


/*
* Settings store intrerface.
*/
class CxeSettingsStore
{
public:

    /**
    * This needs to be here to be able to delete an object
    * of inherited class through mixin pointer.
    * If this is not defined, deleting through the mixin pointer
    * results in User-42 panic!
    */
    virtual ~CxeSettingsStore() {};

    /*
    * resets the cenrep store
    */
    virtual void reset() = 0;
    
    /*
    * Reads a value from cenrep
    * @param "key"   - setting key
    * @param "value" - setting value read from cenrep
    */
	virtual CxeError::Id get(const QString& key, QVariant &value) = 0;

    /*
    * Reads a value from cenrep
    * @param "uid"   - UID of the component that own setting key
    * @param "key"   - setting key id
    * @param "type"  - type of setting key
    * @param "value" - setting value read from cenrep
    */
	virtual void get(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value) = 0;
	
	/*
    * Sets a new value to cenrep
    * @param "key"   - setting key
    * @param "newValue" - new value set to the key in cenrep
    */
	virtual CxeError::Id set(const QString& key,const QVariant newValue) = 0;
	    
	/*
    * Reads/loads all run-time settings values from cenrep
    * @param QList<QString> contains list of all runtime key ids which we use to load values from cenrep.
    * returns: QHash container, "contains" values associated with each key that are read from cenrep
    * NOTE: loading runtime settings should be done only ONCE at start-up. Its an overhead to read runtime keys
    *       unnecessarily multiple times as the values of the runtime keys are not changed.
    *       Runtime keys are only used to configure camerax application.
    */
	virtual QHash<QString, QVariantList> loadRuntimeSettings(QList<QString>& settingKeys) = 0;
};



/*
* CxeSettingsCenRepStore class implements CxeSettingsStore.
* This class uses CenRep key mechanism for storing and retrieving settings information.
*/
class CxeSettingsCenRepStore : public QObject,
                               public CxeSettingsStore
{

    Q_OBJECT

public:

    CxeSettingsCenRepStore();
	~CxeSettingsCenRepStore();

public: // from base class
    /*
    * resets the cenrep store
    */
    void reset();
    
    /*
    * Reads a value from cenrep
    * @param "key"   - setting key
    * @param "value" - setting value read from cenrep
    */
	CxeError::Id get(const QString& key, QVariant &value);

    /*
    * Reads a value from cenrep
    * @param "uid"   - UID of the component that own setting key
    * @param "key"   - setting key id
    * @param "type"  - type of setting key
    * @param "value" - setting value read from cenrep
    */
    void get(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value);
	
	/*
    * Sets a new value to cenrep
    * @param "key"   - setting key
    * @param "newValue" - new value set to the key in cenrep
    */
	CxeError::Id set(const QString& key,const QVariant newValue);
	    
	/*
    * Reads/loads all run-time settings values from cenrep
    * @param QList<QString> contains list of all runtime key ids which we use to load values from cenrep.
    * returns: QHash container, "contains" values associated with each key that are read from cenrep
    * NOTE: loading runtime settings should be done only ONCE at start-up. Its an overhead to read runtime keys
    *       unnecessarily multiple times as the values of the runtime keys are not changed.
    *       Runtime keys are only used to configure camerax application.
    */
	QHash<QString, QVariantList> loadRuntimeSettings(QList<QString>& settingKeys);


signals:

    void settingValueChanged(long int uid, unsigned long int key, QVariant value);

private slots:

    void handleValueChanged(XQSettingsKey key, QVariant value);

private:

    class CxeCenRepDefinition
    {
    public:
        long int mRepositoryUid;            //! UID of the all the setting keys
        unsigned long int mKeyId;           //! key cenrep id
        bool mReadOnly;                     //! if the key holds data that is read-only
        XQSettingsManager::Type mDataType;  //! type of data that cenrep key holds
        
    };

private:
    
    /*
     *  add's key mapping to the hash container.
     */
    void addKeyMapping(QString key,
                       unsigned long int keyid,
                       XQSettingsManager::Type type,
                       bool readOnly = false);
    /*
    * Generates XQSettingsKey from given setting/runtime key
    */
    XQSettingsKey generateXQSettingsKey(const QString& key,CxeError::Id& error);
    
    /*
    * maps "string" type setting key ids to cenrep key ids that XQSettingsManager understands
    */
	void mapKeys();
	
protected:
    const QHash<QString, CxeCenRepDefinition>& keyMapping() const;

private: // data

    XQSettingsManager* mSettingsManager;
	QHash<QString, CxeCenRepDefinition> mKeyMapping;
};


/*
 * Settings store that reads key values from cenrep and keeps
 * cached copies of them in memory. Doesn't write anything back
 * to cenrep.
 */
class CxeSettingsLocalStore : public CxeSettingsCenRepStore
{
  Q_OBJECT

public:
     CxeSettingsLocalStore();
     ~CxeSettingsLocalStore();

     /*
     * Reads a value from local store.
     * @param "key"   - setting key
     * @param "value" - setting value read from cenrep
     */
     CxeError::Id get(const QString& key, QVariant &value);

     /*
     * Sets a new value to local store
     * @param "key"   - setting key
     * @param "newValue" - new value set to the key in cenrep
     */
     CxeError::Id set(const QString& key, const QVariant newValue);
private:

     bool useValueFromCenrep(const QString &key) const;

     QHash<QString, QVariant> mSettings;
};

#endif  // CXESETTINGSCENREPSTORE_H_

