/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CCoeControl to show a logo at camera application startup
*
*/

#ifndef CAMSTARTUPLOGO_H
#define CAMSTARTUPLOGO_H

#include <e32std.h>
#include <coecntrl.h>

#include "camstartuplogocontroller.h"

class CFbsBitmap;
class CPeriodic;


// Class declaration
/**
 *  CCoeControl to show a logo at camera application startup
 */
class CCamStartupLogo : public CCoeControl,
                        public MCamStartupLogoControllerObserver
{
public:
    static CCamStartupLogo* NewL(CCamStartupLogoController& aController,
                                 const TRect& aRect);
    virtual ~CCamStartupLogo();

private:
    CCamStartupLogo(CCamStartupLogoController& aController,
                    const TRect& aRect);
    void ConstructL();

    void ShowL();
    void Hide();

    void Draw(const TRect& aRect) const;

    void StartupLogoControllerStateChanged(TStartupLogoState aNewState);

private: //data
    CFbsBitmap* iLogo;     // own
    CFbsBitmap* iLogoMask; // own

    TRect iRect;

    TBool iWindowCreated;
    
    CCamStartupLogoController& iController; 
};

#endif // CAMSTARTUPLOGO_H
