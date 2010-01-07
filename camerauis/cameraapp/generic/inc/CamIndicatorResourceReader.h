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
* Description:  Camera Indicator Resource Reader class*
*/


#ifndef CAMINDICATORRESOURCEREADER_H
#define CAMINDICATORRESOURCEREADER_H

// INCLUDES
#include <barsread.h>

// FORWARD DECLARATIONS
class CCamIndicatorData;

// CLASS DECLARATION

/**
* Side Pane control
*/
class CCamIndicatorResourceReader : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aReader reference to resource reader
        */
        static CCamIndicatorResourceReader* NewLC( TResourceReader& aReader );

        /**
        * Destructor.
        */
        virtual ~CCamIndicatorResourceReader();

    private:
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aReader reference to resource reader
        */
        void ConstructL( TResourceReader& aReader );

        /**
        * C++ constructor
        * @since 2.8
        */
        CCamIndicatorResourceReader();

    public:
        /**
        * return indicator data
        * @since 2.8
        * @return array of indicator data
        */

        CArrayPtrFlat<CCamIndicatorData>& IndicatorData();

    private: // Data
        CArrayPtrFlat<CCamIndicatorData>* iIndicatorArray;
    };

#endif

// End of File
