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
* Description:  Class for displaying the title in the Status Pane
*
*/



#ifndef CAMTITLE_H
#define CAMTITLE_H

// INCLUDES
#include <coecntrl.h>
#include "CamAppUiBase.h"             //  For TCamAppViewIds


// FORWARD DECLARATIONS
class MAknsControlContext;

// CLASS DECLARATION

/**
* Title control to show in small status pane layout
*/
class CCamTitlePane : public CCoeControl
    {
    public: // Constructors and destructor
        
        /**
        * Destructor.
        * @since 3.0
        */
        virtual ~CCamTitlePane();
        
        /**
        * Factory construction function
        * @since 3.0
        */        
        static CCamTitlePane* NewL();

    public: // New functions
		/**
		* Sets the title with the specified resource string
		* @since 3.0
		* @param aResource The resource to get the string from
		*/
        void SetFromResourceL( TInt aResource );
        
		/**
		* Sets the title with the specified string
		* @since 3.0
		* @param aText The string to set the text
		*/
        void SetTextL( const TDesC& aText );
        
    public: // Functions from base classes
        /**
        * From CCoeControl
        * @since 3.0        
        * @param aRect Rect indicating the specific area to draw
        */
        void Draw( const TRect& /*aRect*/ ) const;

        /**
        * From CCoeControl
        * @since 3.0        
        */
        void SizeChanged();
        
    protected: // Constructors

        /**
        * C++ constructor
        * @since 3.0
        */
        CCamTitlePane();
        
        /**
        * Second phase constructor
        * @since 3.0
        */
        void ConstructL();

    protected: // Data
        HBufC16* iTextHbuf;
        
        TAknLayoutText iTitleTextLayout;
        
        // Layout for the Title Pane rectangle
        TAknLayoutRect iTitlePaneLayoutRect;
    };

#endif // CAMCONTAINERBASE_H

// End of File
