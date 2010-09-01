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
* Description:  Declaration of Info Listbox Item Base class.
*                Adds support for comparison operator.
*
*/


#ifndef CAM_INFOLISTBOXITEMBASE_H
#define CAM_INFOLISTBOXITEMBASE_H

#include <e32base.h>
#include "mcaminfolistboxitem.h"

// ===========================================================================
// Forward declarations
class CFbsBitmap;


// ===========================================================================
// Class declarations
class CCamInfoListboxItemBase : public CBase,
                                public MCamInfoListboxItem
  {
  // =======================================================
  public:
    virtual ~CCamInfoListboxItemBase();

  // -------------------------------------------------------
  // from MCamInfoListboxItem
  public:

    /**
    * Returns a pointer to the item text
    * @since 2.8
    * @return Pointer to the item text
    */   
    virtual TPtrC ItemText() const;
    
    /**
    * Returns the item's value id
    * @since 2.8
    * @return Value id
    */  
    virtual TInt ItemValue() const;
    
    /**
    * Returns a pointer to the item's bitmap
    * @since 2.8
    * @return Pointer to the bitmap
    */  
    virtual CFbsBitmap* Bitmap() const;
    
    /**
    * Returns a pointer to the item's bitmap mask
    * @since 2.8
    * @return Pointer to the bitmap mask
    */  
    virtual CFbsBitmap* BitmapMask() const;    

  // -------------------------------------------------------
  public:

    /**
    * Comparison operator that can be given as parameter to RPointerArray::Find.
    * @param aFirstItemId  Pointer to setting value id of first item
    * @param aOtherItem    Reference to the second item
    * @return Boolean value describing if the items are equal by ids.
    *
    */
    static TBool EqualIds( const TInt*                    aFirstItemId, 
                           const CCamInfoListboxItemBase& aOtherItem );

  // =======================================================
  // Data
  protected:

    // The text to be displayed in the list controls
    HBufC* iListItemText;
    // The id of the setting value represented by this item
    TInt iSettingItemValueId;
    // The id of the bitmap 
    TInt iBitmapId; 
    // The bitmap
    CFbsBitmap* iBitmap;
    // The bitmap mask
    CFbsBitmap* iBitmapMask;

  // =======================================================
  };

#endif // CAM_INFOLISTBOXITEMBASE_H

// ===========================================================================
// end of file
