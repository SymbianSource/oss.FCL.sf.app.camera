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

#include <hbdialog.h>
#include <hblabel.h>
#include <hbpushbutton.h>
#include <QCoreApplication>

#include "cxutils.h"
#include "cxeerror.h"
#include "cxeengine.h"
#include "cxuienums.h"
#include "cxuierrormanager.h"
#include "cxuidocumentloader.h"
#include "cxecameradevicecontrol.h"


/*
* CxuiErrorManager::CxuiErrorManager
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

// ---------------------------------------------------------------------------
// CxuiViewManager::~CxuiViewManager
//
// ---------------------------------------------------------------------------
//
CxuiErrorManager::~CxuiErrorManager()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiErrorManager::reportError
//
// ---------------------------------------------------------------------------
//
void CxuiErrorManager::analyze(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    mErrorMsgPopup = NULL;
    mErrorSeverity = CxuiErrorManager::None;

    // start evaluating the error.
    QString errorMsgTxt = getErrorDetails(error);

    if(mErrorSeverity != CxuiErrorManager::None) {
        // show the error note to the user.
        launchPopup(errorMsgTxt);
    } else {
        // ignore
    }

    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiErrorManager::aboutToClosePopup
//
// ---------------------------------------------------------------------------
//
void CxuiErrorManager::aboutToClosePopup()
{
    CX_DEBUG_ENTER_FUNCTION();
    // handle any use-cases when the error can be recovered
    emit errorRecovered();
    CX_DEBUG_EXIT_FUNCTION();
}



// ---------------------------------------------------------------------------
// CxuiErrorManager::closeApp
//
// ---------------------------------------------------------------------------
//
void CxuiErrorManager::closeApp()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::quit();
    CX_DEBUG_EXIT_FUNCTION();
}



// ---------------------------------------------------------------------------
// CxuiErrorManager::getErrorDetails
// evaluates error for error severity and error note
// ---------------------------------------------------------------------------
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
        default:
            break;
    }
    CX_DEBUG_EXIT_FUNCTION();

    return msg;
}


// ---------------------------------------------------------------------------
// CxuiErrorManager::launchPopup
// ---------------------------------------------------------------------------
void CxuiErrorManager::launchPopup(QString& errorMsgTxt)
{
    CX_DEBUG_ENTER_FUNCTION();

    // we always prepare the popup for the new message and hence we load the
    // popup everytime from document loader

    CX_ASSERT_ALWAYS(mDocumentLoader);
    bool ok = false;

    // Use document loader to create popup
    mDocumentLoader->load(CxUiLayout::ERROR_POPUP_XML, &ok);
    CX_DEBUG(("mErrorMsgPopup load ok=%d", ok));

    mErrorMsgPopup = qobject_cast<HbDialog*>(mDocumentLoader->findWidget(CxUiLayout::ERROR_POPUP));

    CX_ASSERT_ALWAYS(mErrorMsgPopup);

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
