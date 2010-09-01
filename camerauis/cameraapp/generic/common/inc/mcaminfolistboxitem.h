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
* Description:  Declaration of Info Listbox Item.
*                Provides a standard interface for Info Listbox items.
*
*/


#ifndef M_CAM_INFOLISTBOXITEM_H
#define M_CAM_INFOLISTBOXITEM_H


// ===========================================================================
// Forward declarations
class CFbsBitmap;


// ===========================================================================
// Class declarations
class MCamInfoListboxItem
  {
  // =======================================================
  public:

    /**
    * Returns a pointer to the item text
    * @since 2.8
    * @return Pointer to the item text
    */   
    virtual TPtrC ItemText() const = 0;
    
    /**
    * Returns the item's value id
    * @since 2.8
    * @return Value id
    */  
    virtual TInt ItemValue() const = 0;
    
    /**
    * Returns a pointer to the item's bitmap
    * @since 2.8
    * @return Pointer to the bitmap
    */  
    virtual CFbsBitmap* Bitmap() const = 0;
    
    /**
    * Returns a pointer to the item's bitmap mask
    * @since 2.8
    * @return Pointer to the bitmap mask
    */  
    virtual CFbsBitmap* BitmapMask() const = 0;

  // =======================================================
  };

#endif // M_CAM_INFOLISTBOXITEM_H

// ===========================================================================
// end of file
