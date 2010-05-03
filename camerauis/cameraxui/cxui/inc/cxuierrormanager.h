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
#ifndef CXUIERRORMANAGER_H
#define CXUIERRORMANAGER_H

#include <QObject>
#include "cxeerror.h"

class HbDialog;
class CxuiDocumentLoader;
class CxuiCaptureKeyHandler;


/*
* class which handles errors and reports
*/
class CxuiErrorManager : public QObject
{
    Q_OBJECT

public:

    CxuiErrorManager(CxuiCaptureKeyHandler &keyHandler,CxuiDocumentLoader *documentLoader);
    ~CxuiErrorManager();

signals:

    void aboutToRecoverError();
    void errorRecovered();

private slots:

    void analyze(CxeError::Id error);
    void aboutToClosePopup();
    void closeApp();

private:

    // error severity
    enum ErrorSeverity
    {
        None = 0,
        Warning, // Low severity, just warning user needed
        Severe, // when error cannot be recovered
        Critical // when error can be recovered, but needs actions e.g. camera in use
    };

    void launchPopup(QString& errorMsgTxt);
    void showHighSeverityNote(QString& errorMsgTxt);
    void showLowSeverityNote(QString& errorMsgTxt);
    QString getErrorDetails(CxeError::Id error);

private:

    //data
    CxuiCaptureKeyHandler &mKeyHandler;
    CxuiDocumentLoader *mDocumentLoader; // not own
    HbDialog* mErrorMsgPopup;
    CxuiErrorManager::ErrorSeverity mErrorSeverity;
};

#endif // CXUIVIEWMANAGER_H
