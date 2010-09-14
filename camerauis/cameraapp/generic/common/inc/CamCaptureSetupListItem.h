/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A list box item with text, a bitmap and a value id
*/


#ifndef CAMCAPTURESETUPLISTITEM_H
#define CAMCAPTURESETUPLISTITEM_H

//  INCLUDES
#include <AknUtils.h>
#include "caminfolistboxitembase.h"


// CONSTANTS


// FORWARD DECLARATIONS
class CFbsBitmap;
class TResourceReader;

/**
*  A list item for CCamCaptureSetupListBox
*/
class CCamCaptureSetupListItem : public CCamInfoListboxItemBase
	{
    public:

        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aReader Interpreter for the resource used to populate the item
        * @param aLayoutRect layout for icon displayed as part of list item
        * @return pointer to the created CCamCaptureSetupListItem object
        */
        static CCamCaptureSetupListItem* NewLC( TResourceReader& aReader, 
                                                TAknLayoutRect& aIconLayout ); 

        /**
        * Two-phased constructor.
        * For construction with dynamic values, not resource.
        */
        static CCamCaptureSetupListItem* NewLC( 
                  const TDesC&                         aItemText,
                        TInt                           aSettingId,
                        TInt                           aBitmapId,
                        TAknLayoutRect aIconLayout );
        /**
        * Destructor.
        * @since 2.8
        */
        virtual ~CCamCaptureSetupListItem();

    private:

        /**
        * C++ default constructor.
        * @since 2.8
        */     
        CCamCaptureSetupListItem( TAknLayoutRect& aLayoutRect );

        /**
        * Construct the item using the supplied resource data
        * @since 2.8
        * @param aReader Resource interpreter intialised with resource data
        */   
        void ConstructFromResourceL( TResourceReader& aReader );

        /**
        * Construct the item using the supplied values
        */
        void ConstructFromValuesL( const TDesC& aItemText,
                                         TInt   aSettingId,
                                         TInt   aBitmapId );

        /**
        * Second phase construction
        */
        void ConstructL();

    private:

      // Layout for icon in list-box
        TAknLayoutRect iIconLayout;
	};

#endif      // CAMCAPTURESETUPLISTITEM_H   
            
// End of File 
