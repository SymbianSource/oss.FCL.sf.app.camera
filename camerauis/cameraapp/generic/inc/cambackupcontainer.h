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
* Description:  Base class for all camera application containers
*
*  Copyright © 2007-2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
*
*/




#ifndef CAMBACKUPCONTAINER_H_
#define CAMBACKUPCONTAINER_H_

#include "CamContainerBase.h"

class CCamBackupContainer : public CCamContainerBase
    {
private:
    CCamBackupContainer();
    
protected: 

    /**
    * C++ constructor
    * @since 5.1
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    */
    CCamBackupContainer( CCamAppController& aController,
                       CAknView& aView );
    
    /**
    * Perform the second phase construction of a CCamBackupContainer object.
    * @since 5.1
    * @param aContainer Parent container
    */
    void ConstructL(const CCoeControl& aContainer);

public:
    /**
    * Destructor.
    */
    virtual ~CCamBackupContainer();
    
    /**
    * Two-phased factory constructor
    * @since 5.1
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    * @param aContainer Parent container
    * @return pointer to the created CCamBackupContainer object already pushed on the Cleanupstack
    */
    static CCamBackupContainer* NewLC(CCamAppController& aController,
                            CAknView& aView,
                            const CCoeControl& aContainer);
    
    /**
    * Two-phased factory constructor.
    * @since 5.1
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    * @param aContainer Parent container
    * @return pointer to the created CCamBackupContainer object
    */
    static CCamBackupContainer* NewL(CCamAppController& aController,
                            CAknView& aView,
                            const CCoeControl& aContainer);
    
public:
    /**
    * Performs the actual draw on the container 
    * @since 5.1
    * @param aRect area where to draw
    */
    virtual void Draw( const TRect& aRect ) const;
    
    };

#endif /* CAMBACKUPCONTAINER_H_ */
