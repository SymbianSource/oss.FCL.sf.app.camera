/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QCoreApplication>
#include <HbDialog>
#include <HbMessageBox>
#include <HbLabel>
#include <HbPushButton>

#include "cxutils.h"
#include "cxeerror.h"
#include "cxeengine.h"
#include "cxuienums.h"
#include "cxuierrormanager.h"
#include "cxuidocumentloader.h"
#include "cxecameradevicecontrol.h"


/*!
* Constructor
*/
CxuiErrorManager::CxuiErrorManager(CxuiCaptureKeyHandler &keyHandler,CxuiDocumentLoader *documentLoader) :
    mKeyHandler(keyHandler),
    mDocumentLoader(documentLoader),
    mErrorMsgPopup(NULL),
    mErrorSeverity(CxuiErrorManager::None)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor
*/
CxuiErrorManager::~CxuiErrorManager()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Analyze the error code and act accordingly.
* @param error Error code.
*/
void CxuiErrorManager::analyze(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    mErrorMsgPopup = NULL;
    mErrorSeverity = CxuiErrorManager::None;

    if (error != CxeError::None) {
        // start evaluating the error.
        QString errorMsgTxt = getErrorDetails(error);

        if(mErrorSeverity != CxuiErrorManager::None) {
            // show the error note to the user.
            launchPopup(errorMsgTxt);
        } else {
            // ignore
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Slot that gets called when error note is closed.
*/
void CxuiErrorManager::aboutToClosePopup()
{
    CX_DEBUG_ENTER_FUNCTION();
    // handle any use-cases when the error can be recovered
    emit errorRecovered();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Helper method for closing the application.
*/
void CxuiErrorManager::closeApp()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::quit();
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Helper method to get the error message to use for showing note to user,
* and set the severity level, based on given error code.
* @param error Error code to be analyzed.
*/
QString CxuiErrorManager::getErrorDetails(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    QString msg("No Error");
    switch(error) {
        case CxeError::Died:
        case CxeError::InitializationFailed:
        case CxeError::HwNotAvailable:
        case CxeError::NotReady:
            mErrorSeverity = CxuiErrorManager::Severe;
            msg = hbTrId("txt_cam_info_error");
            break;
        case CxeError::InUse:
            mErrorSeverity = CxuiErrorManager::Severe;
            msg = hbTrId("txt_cam_info_camera_already_in_use");
            break;
        case CxeError::DiskFull:
            mErrorSeverity = CxuiErrorManager::Warning;
            msg = hbTrId("txt_cam_info_memory_full");
        default:
            break;
    }
    CX_DEBUG_EXIT_FUNCTION();

    return msg;
}

/*!
*
*/
void CxuiErrorManager::launchPopup(QString& errorMsgTxt)
{
    CX_DEBUG_ENTER_FUNCTION();

    switch (mErrorSeverity) {
    case CxuiErrorManager::Warning:
        showLowSeverityNote(errorMsgTxt);
        break;
    case CxuiErrorManager::Severe:
    case CxuiErrorManager::Critical:
        showHighSeverityNote(errorMsgTxt);
        break;
    default:
        break;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Show error note for high severity error.
*/
void CxuiErrorManager::showHighSeverityNote(QString& errorMsgTxt)
{
    // we always prepare the popup for the new message and hence we load the
    // popup everytime from document loader

    CX_ASSERT_ALWAYS(mDocumentLoader);
    bool ok = false;

    // Use document loader to create popup
    mDocumentLoader->load(CxUiLayout::ERROR_POPUP_XML, &ok);
    CX_DEBUG(("mErrorMsgPopup load ok=%d", ok));

    mErrorMsgPopup = qobject_cast<HbDialog*>(mDocumentLoader->findWidget(CxUiLayout::ERROR_POPUP));
    CX_ASSERT_ALWAYS(mErrorMsgPopup);

    // HbDialog's default background item is replaced with black rectangle
    QGraphicsRectItem *backgroundItem = new QGraphicsRectItem();
    QBrush blackBrush = QBrush(Qt::black);
    backgroundItem->setBrush(blackBrush);
    QGraphicsItem *origBgItem = mErrorMsgPopup->backgroundItem();
    backgroundItem->setRect(origBgItem->boundingRect());
    mErrorMsgPopup->setBackgroundItem(backgroundItem);

    mErrorMsgPopup->setTimeout(HbDialog::NoTimeout);
    mErrorMsgPopup->setBackgroundFaded(false);

    // color of standby text is set in the code. It cannot be done in docml
    HbLabel* label = qobject_cast<HbLabel*>(mDocumentLoader->findWidget(CxUiLayout::ERROR_TEXT_WIDGET));
    label->setTextColor(Qt::white);
    label->setPlainText(errorMsgTxt);

    HbPushButton *exitButton = qobject_cast<HbPushButton*>(mDocumentLoader->findWidget(CxUiLayout::ERROR_BUTTON_WIDGET));

    if(mErrorSeverity == CxuiErrorManager::Severe) {
        // inform ui about error recovery
        emit aboutToRecoverError();
        QObject::connect(mErrorMsgPopup, SIGNAL(aboutToClose()), this, SLOT(closeApp()));
        exitButton->show();
    } else {
        // TODO handle other severity cases here.
    }

    mErrorMsgPopup->show();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Show error note for low severity error.
*/
void CxuiErrorManager::showLowSeverityNote(QString& errorMsgTxt)
{
    CX_DEBUG_ENTER_FUNCTION();
    HbMessageBox::warning(errorMsgTxt);
    CX_DEBUG_EXIT_FUNCTION();
}
