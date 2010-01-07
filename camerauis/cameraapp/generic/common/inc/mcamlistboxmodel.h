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
* Description:  Listbox interface
*
*/


#ifndef M_CAM_LISTBOXMODEL_H
#define M_CAM_LISTBOXMODEL_H



#include <eiklbm.h>

class MCamListboxModel: public MListBoxModel
  {
  public : // from MListBoxModel
    
    /**
    * virtual empty destructor
    */
    virtual ~MCamListboxModel() {};

    /**
    * Returns the number of items.
    * @since 2.8
    * Returns number of items in item array
    * @return Number of items
    */
    virtual TInt NumberOfItems() const = 0;
		
    /**
    * Returns pointer to the MDesCArray
    * @since 2.8
    * @return Handle to MDesCArray
    */        
    virtual const MDesCArray* MatchableTextArray() const = 0;
  
  public :

    /**
    * Returns pointer to the item text
    * @param aItemIndex index of the item 
    * @return pointer to the item text
    */   
    virtual TPtrC ItemText( TInt aItemIndex ) const = 0;
    
    /**
    * Returns the value id of the specified item
    * @param aItemIndex index of the item 
    * @return the item's value id
    */   
    virtual TInt ItemValue( TInt aItemIndex ) const = 0;
    
    /**
    * Returns the bitmap for the specified item
    * @param aItemIndex index of the item 
    * @return pointer to the icon bitmap
    */   
    virtual CFbsBitmap* Bitmap( TInt aItemIndex ) const = 0;
    
    /**
    * Returns the bitmap mask for the specified item
    * @param aItemIndex index of the item 
    * @return pointer to the icon mask
    */  
    virtual CFbsBitmap* BitmapMask( TInt aItemIndex ) const = 0;
    
    /**
    * Returns the index of the item with the specified value id
    * @param aValueId the value to find
    * @return index of the matching item
    */  
    virtual TInt ValueIndex( TInt aValueId ) const = 0;
    
    
    /**
    * Returns whether the model represents a shooting mode selection listbox
    * @since 2.8
    * @return TBool ETrue if the model is representing a shooting mode listbox instead of a general settings listbox
    */
    virtual TBool ShootingModeListBox() const = 0;
    
    virtual TAknLayoutRect IconLayoutData() const = 0;
    virtual TAknLayoutRect RadioButtonLayoutData() const = 0;
    virtual TAknLayoutText TextLayoutData() const = 0;
    virtual TAknLayoutRect ListItemLayoutData() const = 0;

  };

#endif // M_CAM_LISTBOXMODEL_H
