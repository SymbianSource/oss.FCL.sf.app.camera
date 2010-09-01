/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef CAM_DEBUG_UTILITIES_H
#define CAM_DEBUG_UTILITIES_H

#include <aknmessagequerydialog.h>
#include "camtest.rsg"


class TCamDebugUtilities
  {
  public:
  
    static void ShowMessageDialogL( const TDesC& aTitle, const TDesC& aMessage )
      {
      TBuf<1> tmp; // CAknMessageQueryDialog needs some initial value  
      CAknMessageQueryDialog* dlg = CAknMessageQueryDialog::NewL( tmp );
      dlg->PrepareLC( R_CAM_DEBUG_MESSAGE_DLG ); 
      dlg->QueryHeading()->SetTextL( aTitle );
      dlg->SetMessageText( aMessage );
      dlg->RunLD();
      };
  };

#endif