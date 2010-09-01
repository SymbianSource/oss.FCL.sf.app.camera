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

#ifndef CAMVIEWFINDERGRID_H
#define CAMVIEWFINDERGRID_H

#include <e32base.h>

class CBitmapContext;
/**
* Interface for viewfinder grid usage.
*/
class MCamVfGridDrawer
  {
  public:
    virtual void  InitL( TAny* aParam ) = 0;

    virtual void  Draw( CBitmapContext& aGc ) const = 0;

    virtual void  Draw( const TRect&          aRect,
                              CBitmapContext& aGc    ) const = 0;

    virtual void  SetVisible( TBool aVisible ) = 0;

    virtual TBool IsVisible() const = 0;
    
    // Essential to be able to destroy via mixin pointer.
    // Otherwise delete causes USER 42 panic.
    virtual ~MCamVfGridDrawer() 
      {}; 
  };



/**
* Factory class to instantiate viewfinder grid.
*/
class CCamVfGridFactory : public CBase
  {
  public:
    /**
    * CreateVfGridDrawer
    * @param  aUid The implementation id of grid drawer.
    * @return      Pointer to object of the implementing class
    *              or NULL if uid not supported.
    */
    static MCamVfGridDrawer* CreateVfGridDrawerL( TUid aUid );
  };


#endif
