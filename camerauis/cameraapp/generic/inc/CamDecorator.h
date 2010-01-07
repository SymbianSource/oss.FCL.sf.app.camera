/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Camera UI decorator*
*/



#ifndef CAMDECORATOR_H
#define CAMDECORATOR_H

#include <e32base.h>
 

class TResourceReader;
class CBlendedBackground;
class CBitmapContext;
class TRect;
class CCamAppController;
class CCamRecordingIconDecoration;
class MCamObserver;

class CCamDecorator : public CBase
    {
public:
    /**
    * Symbian OS two-phased constructor
    * @since 2.8
    * @param aResourceId The resource that describes the decorations required. If 0, a blank decorator is created (i.e. Draw() will have no effect)
    * @param aObserver If creator is interested, will be registered with observable decorations
    * @return New instance of class
    */
	static CCamDecorator* NewL( CCamAppController& aController,
	                            TInt               aResourceId, 
	                            MCamObserver*      aObserver = NULL );

    /**
    * Destructor
    */
    ~CCamDecorator();

protected:
    /**
    * Constructor
    */
    CCamDecorator( CCamAppController& aController );

    /**
    * Second-phase constructor
    * @param aResourceId The resource that describes the decorations required. If 0, a blank decorator is created (i.e. Draw() will have no effect)
    * @param aObserver If creator is interested, will be registered with observable decorations
    */
    void ConstructL( TInt aResourceId, MCamObserver* aObserver );

public: // New functions
    /**
    * Draws all decorations
    * @param aGc The context to use while drawing
    * @param aDrawRect Rectangle passed on to decorations - meaning varies depending on decoration
    * @param aForceBlankers Forces the display of the blanking components
    */
    void Draw( CBitmapContext& aGc, const TRect& aDrawRect, const TBool aForceBlankers = EFalse );

private: // New functions
    /**
    * Reads in the details of a blended background, and stores an object based on this
    * @param aResourceReader The resource reader to use
    */
    void CreateBlendedBackgroundL(TResourceReader& aResourceReader);

    /**
    * Reads in the details of a blanker, and stores an object based on this
    * @param aResourceReader The resource reader to use
    */
    void CreateDVFBlankerL(TResourceReader& aResourceReader);

    /**
    * Reads in the details of a recording icon, and stores an object based on this
    * @param aResourceReader The resource reader to use
    */
    void CreateRecordingIconL(TResourceReader& aResourceReader);

private: // Data
    // Lists of all decorator elements
    RArray<TRect> iDVFBlankers;
    RPointerArray<CCamRecordingIconDecoration> iRecordingIcons;
    
    // Controller
    CCamAppController& iController;

    // An observer to add to observable decorations, if wanted. May be NULL.    
    MCamObserver* iPossibleObserver;
    
    // If true, nothing will be drawn on call to Draw. Optimization.
    TBool iDrawNothing;
    };
    
#endif // CAMDECORATOR_H

// End of File
