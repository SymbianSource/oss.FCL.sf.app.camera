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

#include <QObject>

#ifdef Q_OS_SYMBIAN
#include <e32cmn.h>
#include <w32std.h>
#include <apgwgnam.h>
#include <apgtask.h>
#include <eikenv.h>
#include <avkondomainpskeys.h>  // keyguard state
#include <hwrmpowerstatesdkpskeys.h> // battery status

#include <QMetaEnum>
#include <QString>
#include <QVariant>
#include <qsymbianevent.h>
#endif // Q_OS_SYMBIAN

#include "cxutils.h"
#include "cxuiapplication.h"
#include "cxesettings.h"
#include "cxuiapplicationframeworkmonitorprivate.h"


#ifdef Q_OS_SYMBIAN
namespace
{
    inline QString convertTDesC2QString(const TDesC& aDescriptor)
    {
        #ifdef QT_NO_UNICODE
            return QString::fromLocal8Bit(aDescriptor.Ptr(), aDescriptor.Length());
        #else
            return QString::fromUtf16(aDescriptor.Ptr(), aDescriptor.Length());
        #endif
    }

    inline QString windowGroupName(RWsSession& ws, int id)
    {
        TBuf<CApaWindowGroupName::EMaxLength> name;
        ws.GetWindowGroupNameFromIdentifier(id, name);
        // Window group name contains "null" characters,
        // which are considered end-of-string if not replaced.
        for (int i=0; i < name.Length(); i++) {
            if (name[i] == NULL) {
                name[i] = ' ';
            }
        }
        return convertTDesC2QString(name);
    }

    //!@todo: Avkon UIDs not needed once device dialogs fully implemented in Orbit.

    // AknCapServer
    static const unsigned int UID_AKNCAPSERVER    = 0x10207218;

    // Phone ui
    static const unsigned int UID_PHONEUI         = 0x100058B3;
    // Task switcher
    static const unsigned int UID_TASKSWITCHER    = 0x2002677D;
    // Dialog server
    static const unsigned int UID_DIALOGAPPSERVER = 0x20022FC5;
}
#endif // Q_OS_SYMBIAN


/*!
* Constructor
*/
CxuiApplicationFrameworkMonitorPrivate::CxuiApplicationFrameworkMonitorPrivate(CxuiApplication &application, CxeSettings& settings)
    :  mApplication(application),
       mSettings(settings),
#ifdef Q_OS_SYMBIAN
       mWsSession(CCoeEnv::Static()->WsSession()),
       mWindowGroup(CCoeEnv::Static()->RootWin()),
       mWindowGroupId(mWindowGroup.Identifier()),
       mWindowGroupName(),
       mKeyLockState(EKeyguardNotActive),
       mBatteryStatus(EBatteryStatusUnknown),
#endif // Q_OS_SYMBIAN
       mState(CxuiApplicationFrameworkMonitor::ForegroundOwned)
{
    CX_DEBUG_ENTER_FUNCTION();

#ifdef Q_OS_SYMBIAN
    mWindowGroup.EnableFocusChangeEvents();
    mWindowGroupName = windowGroupName(mWsSession, mWindowGroupId);
    init();
#endif // Q_OS_SYMBIAN

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxuiApplicationFrameworkMonitorPrivate::~CxuiApplicationFrameworkMonitorPrivate()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Current foreground owning state of this application.
* @return Foreground owning state.
*/
CxuiApplicationFrameworkMonitor::ForegroundState CxuiApplicationFrameworkMonitorPrivate::foregroundState() const
{
    return mState;
}

#ifdef Q_OS_SYMBIAN
/*!
* Slot to handle Symbian event.
* @param event Symbian event to be handled. (Ownership not taken.)
*/
void CxuiApplicationFrameworkMonitorPrivate::handleEvent(const QSymbianEvent *event)
{
    // We receive tons of these events, so function start and end traces
    // are intentionally left out.

    if (event) {
        switch (event->type()) {
        case QSymbianEvent::WindowServerEvent:
            handleWindowServerEvent(event);
            break;
        }
    }
}

/*!
* Handle changes in RProperty values of keylock state and battery status.
* @param uid Category uid of the changed property.
* @param key Integer key of the changed property.
* @param value New value of the changed property.
*/
void CxuiApplicationFrameworkMonitorPrivate::handlePropertyEvent(long int uid, unsigned long int key, QVariant value)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (uid == KPSUidAvkonDomain.iUid && key == KAknKeyguardStatus) {
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - keylock status changed: %d -> %d", value.toInt(), mKeyLockState));

        // Check if the keylock value has actually changed
        const int newKeyLockState = value.toInt();
        if (newKeyLockState != mKeyLockState) {
            mKeyLockState = newKeyLockState;
            // Set foreground state based on keylock status and focused application info.
            setState(getCurrentState());
        }
    } else if (uid == KPSUidHWRMPowerState.iUid && key == KHWRMBatteryStatus ) {
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - battery status changed: %d -> %d", value.toInt(), mBatteryStatus));

        // If status changed, check if battery is going empty.
        const int newBatteryStatus = value.toInt();
        if (newBatteryStatus != mBatteryStatus) {
            mBatteryStatus = newBatteryStatus;

            // Notify that battery is almost empty,
            // need to stop any recordings etc.
            if( mBatteryStatus == EBatteryStatusEmpty ) {
                emit batteryEmpty();
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set initial values.
*/
void CxuiApplicationFrameworkMonitorPrivate::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Connect to application (window server) events.
    connect(&mApplication, SIGNAL(symbianEvent(const QSymbianEvent *)), this, SLOT(handleEvent(const QSymbianEvent *)));

    QVariant value;

    // Get initial battery status.
    mSettings.get(KPSUidHWRMPowerState.iUid, KHWRMBatteryStatus, Cxe::PublishAndSubscribe, value);
    mBatteryStatus = value.toInt();

    // Get initial keylock status.
    mSettings.get(KPSUidAvkonDomain.iUid, KAknKeyguardStatus, Cxe::PublishAndSubscribe, value);
    mKeyLockState = value.toInt();

    bool ok = connect(&mSettings, SIGNAL(settingValueChanged(long int, unsigned long int, QVariant)),
                      this, SLOT(handlePropertyEvent(long int, unsigned long int, QVariant)));
    CX_DEBUG_ASSERT(ok);

    // Get foreground state. Depends on keyguard status, so that needs to be read first.
    mState = getCurrentState();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method to handle Symbian event that specificly is of type QSymbianEvent::WindowServerEvent.
* @param event Symbian event to be handled. (Ownership not taken.)
*/
bool CxuiApplicationFrameworkMonitorPrivate::handleWindowServerEvent(const QSymbianEvent *event)
    {
    // We receive tons of these events, so function start and end traces
    // are intentionally left out.

    const TWsEvent *wsEvent = event->windowServerEvent();
    if (wsEvent) {
        switch (wsEvent->Type()) {
        case EEventFocusGroupChanged: {
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - EEventFocusGroupChanged event"));
            setState(getCurrentState());
            break;
        }
        case EEventFocusGained: {
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - EEventFocusGained event"));
            setState(getCurrentState());
            break;
        }
        case EEventFocusLost: {
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - EEventFocusLost event"));
            setState(getCurrentState());
            break;
        }
        case EEventWindowVisibilityChanged: {
            const TWsVisibilityChangedEvent *visibilityEvent = wsEvent->VisibilityChanged();
            if (visibilityEvent) {
                CX_DEBUG(("CxuiApplicationFrameworkMonitor - EFullyVisible: bits[%s]",
                    QString::number(TWsVisibilityChangedEvent::EFullyVisible, 2).toAscii().constData() ));
                CX_DEBUG(("CxuiApplicationFrameworkMonitor - EPartiallyVisible: bits[%s]",
                    QString::number(TWsVisibilityChangedEvent::EPartiallyVisible, 2).toAscii().constData() ));
                CX_DEBUG(("CxuiApplicationFrameworkMonitor - ENotVisible: bits[%s]",
                    QString::number(TWsVisibilityChangedEvent::ENotVisible, 2).toAscii().constData() ));
                CX_DEBUG(("CxuiApplicationFrameworkMonitor - event:       bits[%s]",
                    QString::number(visibilityEvent->iFlags, 2).toAscii().constData() ));
            }
            break;
        }
        default:
            break;
        }
    }

    return false;
}

/*!
* Set state and emit signal if state really changes.
* @param state New state.
*/
void CxuiApplicationFrameworkMonitorPrivate::setState(CxuiApplicationFrameworkMonitor::ForegroundState state)
{
    if (mState != state) {
        const CxuiApplicationFrameworkMonitor::ForegroundState original(mState);

        // Check if state transition is acceptable in current state.
        switch (mState) {
        case CxuiApplicationFrameworkMonitor::ForegroundOwned:
        case CxuiApplicationFrameworkMonitor::ForegroundPartiallyLost:
            // All changes accepted.
            mState = state;
            break;
        case CxuiApplicationFrameworkMonitor::ForegroundFullyLost:
            // If foreground application is changed to note when we are already
            // fully in background, cannot accept state change to "partial foreground".
            if (state != CxuiApplicationFrameworkMonitor::ForegroundPartiallyLost) {
                mState = state;
            } else {
                CX_DEBUG(("CxuiApplicationFrameworkMonitor - state change full bg -> partial bg ignored"));
            }
        }

        if (mState != original) {
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - state change [%s] -> [%s]",
                CxuiApplicationFrameworkMonitor::staticMetaObject.enumerator(
                    CxuiApplicationFrameworkMonitor::staticMetaObject.indexOfEnumerator("ForegroundState")).valueToKey(original),
                CxuiApplicationFrameworkMonitor::staticMetaObject.enumerator(
                    CxuiApplicationFrameworkMonitor::staticMetaObject.indexOfEnumerator("ForegroundState")).valueToKey(mState) ));

            // If state was changed, signal it to listeners.
            emit foregroundStateChanged(mState);
        }
    }
}

/*!
* Get the current foreground state.
* @return Current state for foreground ownership.
*/
CxuiApplicationFrameworkMonitor::ForegroundState CxuiApplicationFrameworkMonitorPrivate::getCurrentState()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxuiApplicationFrameworkMonitor::ForegroundState state(CxuiApplicationFrameworkMonitor::ForegroundOwned);
    int focusWindowGroupId(mWsSession.GetFocusWindowGroup());

    if (mKeyLockState != EKeyguardNotActive) {
        // Keylock enabled is the same as if other application is in foreground.
        state = CxuiApplicationFrameworkMonitor::ForegroundFullyLost;
    } else if (focusWindowGroupId == mWindowGroupId) {
        // If our window group has focus, we clearly are the foreground owning application.
        CX_DEBUG(("CxuiApplicationFrameworkMonitor - Foreground window group matches ours."));
        state = CxuiApplicationFrameworkMonitor::ForegroundOwned;

    } else {
        // Need to check if foreground is owned by known apps.
        unsigned int uid(focusedApplicationUid());

        // Check the app uid.
        switch (uid) {
        case UID_AKNCAPSERVER:
        case UID_TASKSWITCHER:
        case UID_DIALOGAPPSERVER:
            // Note or task switcher in foreground.
            state = CxuiApplicationFrameworkMonitor::ForegroundPartiallyLost;
            break;
        case UID_PHONEUI:
        default:
            // Foreground owned by other app.
            state = CxuiApplicationFrameworkMonitor::ForegroundFullyLost;
            break;
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return state;
}

/*!
* Get the uid of application in foreground.
* @return Application uid for the foreground application.
*/
unsigned int CxuiApplicationFrameworkMonitorPrivate::focusedApplicationUid()
{
    unsigned int uid(0);
    int focusWgId(mWsSession.GetFocusWindowGroup());

    TRAP_IGNORE({
        CApaWindowGroupName* wgn = CApaWindowGroupName::NewLC(mWsSession, focusWgId);
        uid = wgn->AppUid().iUid;
        CleanupStack::PopAndDestroy(wgn);
    });

    // If the window group identifier does not have the application uid set,
    // get it via thread secure id.
    if (uid == 0) {
        TApaTask task(mWsSession);
        task.SetWgId(focusWgId);

        RThread t;
        int err = t.Open(task.ThreadId());
        if (err == KErrNone) {
            uid = t.SecureId().iId;
            CX_DEBUG(("CxuiApplicationFrameworkMonitor - uid resolved from thread"));
        }
        t.Close();
    }

#ifdef CX_DEBUG
    QString name(windowGroupName(mWsSession, focusWgId));

    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Own window group id:     0x%08x", mWindowGroupId));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Focused window group id: 0x%08x", focusWgId));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Own window group name:        [%s]", mWindowGroupName.toAscii().constData()));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Focused window group name:    [%s]", name.toAscii().constData()));
    CX_DEBUG(("CxuiApplicationFrameworkMonitor - Focused application uid: 0x%08x", uid));
#endif

    return uid;
}

#endif // Q_OS_SYMBIAN

// end of file
