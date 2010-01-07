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
* Description:  Declaration of Data Source Interface for Info Listboxes.
*                Provides a standard interface for getting all the
*                data Info Listbox needs to show.
*
*/


#ifndef M_CAM_INFOLISTBOXDATASOURCE
#define M_CAM_INFOLISTBOXDATASOURCE

// ===========================================================================
// Class declarations
class MCamInfoListboxDataSource
  {
  public:
    virtual TInt  ItemIdCount( TInt aType ) const = 0;
    /**
    * Get item id array of type set earlier by SetItemIdArrayType.
    */
    virtual void  ItemIdsL( RArray<TInt>& aItems ) = 0;

    /**
    * Get item id array of given type.
    */
    virtual void  ItemIdsL( RArray<TInt>& aItems,
                            TInt          aType  ) = 0;

    virtual void  SetItemIdArrayType( TInt aType ) = 0;
    virtual TInt  ItemIdArrayType() const = 0;

    virtual TInt  ListboxResourceId( TInt aItemId ) = 0;  
    virtual TInt  InfoResourceId   ( TInt aItemId ) = 0;
    
    virtual TPtrC ListboxTitle     ( TInt aItemId ) = 0;
    virtual TInt  ListboxIconId    ( TInt aItemId ) = 0;
  
    virtual TPtrC InfoTitle        ( TInt aItemId ) = 0;
    virtual TPtrC InfoDescription  ( TInt aItemId ) = 0;
    virtual TInt  InfoIconId       ( TInt aItemId ) = 0;

  };

#endif // M_CAM_INFOLISTBOXDATASOURCE

// ===========================================================================
// end of file
