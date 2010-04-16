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

    /*
	* slot which handles reporting of errors to the client
	*/
    void analyze(CxeError::Id error);

    /*
     * slot which handles cases to be considered when we are about to close
     * the pop-up
     */
    void aboutToClosePopup();
    
    /*
     * slot to close camera app
     */
    void closeApp();

private:

    // error severity
    enum ErrorSeverity
    {
        Severe     = 0x01, // when error cannot be recovered
        Critical   = 0x02, // when error can be recovered, but needs actions e.g. camera in use
        None       = 0x08, // we can ignore the error
    };

    /*
     * Activates popup which displays the error messages based on the
     * error severity
     */
    void launchPopup(QString& errorMsgTxt);
    
    /*
     *  check the error severity and get the error msg
     */
    QString getErrorDetails(CxeError::Id error);

private:
    //data
    CxuiCaptureKeyHandler &mKeyHandler;
    CxuiDocumentLoader *mDocumentLoader; // not own
    HbDialog* mErrorMsgPopup;
    CxuiErrorManager::ErrorSeverity mErrorSeverity;
};

#endif // CXUIVIEWMANAGER_H
