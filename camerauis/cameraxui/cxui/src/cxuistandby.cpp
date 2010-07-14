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

#include <QTimer>
#include <QApplication>
#include <QGraphicsSceneEvent>
#include <hblabel.h>
#include <hbdialog.h>
#include <hbinstantfeedback.h>
#include <QGraphicsRectItem>
#include <QColor>

#include "cxutils.h"
#include "cxeengine.h"
#include "cxuienums.h"
#include "cxuistandby.h"
#include "cxuidocumentloader.h"
#include "cxeviewfindercontrol.h"
#include "cxuicapturekeyhandler.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"



/*!
 * Constructor
 */
CxuiStandby::CxuiStandby(CxuiCaptureKeyHandler &keyHandler,CxuiDocumentLoader *documentLoader, CxeEngine *engine)
: mKeyHandler(keyHandler),
  mDocumentLoader(documentLoader),
  mEngine(engine),
  mStandbyPopup(NULL),
  mStandbyDialogVisible(false),
  mAllowDismiss(true)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(engine);

    // initialize standby timer
    mStandbyTimer = new QTimer(this);

    // install event filter for application wide events
    QCoreApplication::instance()->installEventFilter(this);

    CX_ASSERT_ALWAYS(mStandbyTimer);
    connect(mStandbyTimer, SIGNAL(timeout()), this, SLOT(enterStandby()));
    mStandbyTimer->setSingleShot(true);

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
 * Destructor
 */
CxuiStandby::~CxuiStandby()
{
    CX_DEBUG_IN_FUNCTION();
    // remove the event filter
    QCoreApplication::instance()->removeEventFilter(this);
    // stop standby timer
    stopTimer();
}


/*!
* Allow dismissing standby mode with AF or capture key?
* @param allow Is dismissing allowed.
*/
void CxuiStandby::allowDismiss(bool allow)
{
    CX_DEBUG_ENTER_FUNCTION();
    mAllowDismiss = allow;
    if (allow) {
        if (mStandbyDialogVisible) {
            // Reconnect the close signals if dialog is visible
            connect(&mKeyHandler, SIGNAL(autofocusKeyPressed()), mStandbyPopup, SLOT(close()), Qt::UniqueConnection);
            connect(&mKeyHandler, SIGNAL(captureKeyPressed()),   mStandbyPopup, SLOT(close()), Qt::UniqueConnection);
        }
    } else {
        disconnect(&mKeyHandler, SIGNAL(autofocusKeyPressed()), mStandbyPopup, SLOT(close()));
        disconnect(&mKeyHandler, SIGNAL(captureKeyPressed()),   mStandbyPopup, SLOT(close()));
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Is standby mode active?
* @return True, if standby mode is active, false if not.
*/
bool CxuiStandby::isActive() const
{
    return mStandbyDialogVisible;
}

/*!
 * stops standby timer
 */
void CxuiStandby::stopTimer()
{
    if(mStandbyTimer) {
        mStandbyTimer->stop();
    }
}

/*!
 * starts standby timer
 */
void CxuiStandby::startTimer()
{
    if(mStandbyTimer) {
        mStandbyTimer->start(CXUI_STANDBY_CAMERA_TIMEOUT);
    }
}

/*!
 * Handles mouse press events
 * returns if mouse key press is consumed.
 * \param event event to be handled
 * \return boolean to indicate whether the event was handled or not
 */
bool CxuiStandby::handleMouseEvent(QEvent *event)
{
    bool keyHandled = false;

    // close the dialog if it's visible
    if (mStandbyDialogVisible && mStandbyPopup) {
        HbInstantFeedback feedback(HbFeedback::BasicItem);
        switch (event->type()) {
            case QEvent::GraphicsSceneMousePress:
                feedback.play();
                break;
            case QEvent::GraphicsSceneMouseRelease:
                if (mAllowDismiss) {
                    CX_DEBUG(( "closing the popup mStandbyDialogVisible = : %d", mStandbyDialogVisible ));
                    // todo: sound disabling doesn't work in orbit yet so don't do feedback on release
                    // needs to be enabled when orbit support is done
                    //feedback.setModalities(HbFeedback::Tactile);
                    //feedback.play();
                    exitStandby();
                }
                break;
            default:
                break;
        }
        // eat all mouse events when standby is active
        keyHandled = true;
    } else if (mStandbyTimer && mStandbyTimer->isActive()) {
        // restart the timer only if it's running
        startTimer();
    }

    return keyHandled;
}


/*!
 * switching to standby.
 */
void CxuiStandby::enterStandby()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (proceedToStandy()) {

        // signal for ui classes to prepare for standby
        emit aboutToEnterStandby();

        mStandbyDialogVisible = true;

        if (mStandbyPopup == NULL) {
            CX_DEBUG(("Loading standby DocML"));
            bool ok = false;
            // Use document loader to create popup
            mDocumentLoader->load(CxUiLayout::STANDBY_POPUP_XML, &ok);
            CX_DEBUG(("standby load ok=%d", ok));
            mStandbyPopup = qobject_cast<HbDialog*>(mDocumentLoader->findWidget(CxUiLayout::STANDBY_POPUP));
            CX_ASSERT_ALWAYS(mStandbyPopup);
            mStandbyPopup->setTimeout(HbDialog::NoTimeout);
            mStandbyPopup->setBackgroundFaded(false);
            mStandbyPopup->setPreferredPos(QPointF(0,0));
            // color of standby text is set in the code. It cannot be done in docml
            HbLabel* label = qobject_cast<HbLabel*>(mDocumentLoader->findWidget(CxUiLayout::STANDBY_TEXT_WIDGET));
            label->setTextColor(Qt::white);

            // connecting "abouttoclose" signal to dismissStandby
            connect(mStandbyPopup, SIGNAL(aboutToClose()), this, SLOT(dismissStandby()));

            // HbDialog's default background item is replaced with black rectangle
            QGraphicsRectItem *backgroundItem = new QGraphicsRectItem();
            QBrush blackBrush = QBrush(Qt::black);
            backgroundItem->setBrush(blackBrush);
            QGraphicsItem *origBgItem = mStandbyPopup->backgroundItem();
            backgroundItem->setRect(origBgItem->boundingRect());
            mStandbyPopup->setBackgroundItem(backgroundItem);

        }

        CX_ASSERT_ALWAYS(mStandbyPopup);

        mStandbyPopup->show();
        // connecting half press or full press key signal to dismiss standby
        connect(&mKeyHandler, SIGNAL(autofocusKeyPressed()), mStandbyPopup, SLOT(close()), Qt::UniqueConnection);
        connect(&mKeyHandler, SIGNAL(captureKeyPressed()),   mStandbyPopup, SLOT(close()), Qt::UniqueConnection);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Close the standby dialog.
*/
void CxuiStandby::exitStandby()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mAllowDismiss && mStandbyDialogVisible && mStandbyPopup) {
        mStandbyPopup->close();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
 * dismisses standby
 */
void CxuiStandby::dismissStandby()
{
    CX_DEBUG_ENTER_FUNCTION();

    if(mStandbyDialogVisible) {
        // stop the standby timer and close the pop-up
        mStandbyDialogVisible = false;
        //restart timer
        startTimer();
        // signal for ui classes to prepare for standby exit
        emit aboutToExitStandby();
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
 * checks if we can switch to standby
 */
bool CxuiStandby::proceedToStandy()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mEngine);

    bool ok = false;
    if (!mStandbyDialogVisible) {
        CX_DEBUG(("show standby dialog"));
        ok = true;
    }

    CX_DEBUG(( "CxuiStandby::proceedToStandy proceedToStandy: %d", ok ));

    return ok;
}



/*!
 *  Event filter which filters application wide mouse events.
 */
bool CxuiStandby::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);

    bool eventWasConsumed = false;
    switch (event->type()) {
        case QEvent::GraphicsSceneMouseMove:
        case QEvent::GraphicsSceneMousePress:
        case QEvent::GraphicsSceneMouseRelease:
            eventWasConsumed = handleMouseEvent(event);
            break;
        default:
            break;
    }
    return eventWasConsumed;
}

// end of file
