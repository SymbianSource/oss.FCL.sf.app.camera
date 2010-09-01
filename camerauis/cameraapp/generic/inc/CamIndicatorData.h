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
* Description:  Encapsulates the data specific to a single indicator.*
*/


#ifndef CAMINDICATORDATA_H
#define CAMINDICATORDATA_H

// INCLUDES
 

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* 
*/
class CCamIndicatorData : public CBase
    {
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @since 2.8
        * @param aReader reference to resource reader object
        */
        static CCamIndicatorData* NewLC( TResourceReader& aReader );

        /**
        * Destructor.
        */
        virtual ~CCamIndicatorData();

    private:
        /**
        * Symbian OS 2nd phase constructor.
        * @since 2.8
        * @param aReader reference to resource reader object
        */
        void ConstructL( TResourceReader& aReader );

        /**
        * C++ constructor
        * @since 2.8
        */
        CCamIndicatorData();

    public: // New functions
        /**
        * returns the indicator identifier
        * @since 2.8
        * @return indicator id
        */
        TInt IndicatorId() const;

        /**
        * returns the indicator rectangle
        * @since 2.8
        * @return indicator rectangle
        */
        TRect IndicatorRect() const;
        
        /**
        * returns the number of available bitmaps for the indicator
        * @since 2.8
        * @return number of bitmaps
        */
        TInt IndicatorBitmapCount() const;

        /**
        * returns a bitmap identifier
        * @since 2.8
        * @param aIndex indx into array of available bitmaps
        * @return bitmap id at specified index
        */
        TInt32 IndicatorBitmapId( TInt aIndex ) const;

    private: // Data
        TInt iIndicatorId;
        TRect iRect;
        TInt iIndicatorBitmapCount;
        RArray<TInt32> iBitmapIdArray;
    };

#endif

// End of File
