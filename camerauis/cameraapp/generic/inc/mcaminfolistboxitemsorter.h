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
* Description:  Declaration of Item Sorter Interface for Info Listboxes.
*                Provides a standard interface for externalising the
*                sorting of Info Listbox items.
*
*/


#ifndef M_CAM_INFOLISTBOXITEMSORTER_H
#define M_CAM_INFOLISTBOXITEMSORTER_H

// ===========================================================================
// Forward declarations
class CCamInfoListboxItemBase;

// ===========================================================================
// Class declarations
class MCamInfoListboxItemSorter
  {
  public:
    virtual void SortItems( RPointerArray<CCamInfoListboxItemBase>& aItems ) const = 0;
  };

#endif // M_CAM_INFOLISTBOXITEMSORTER_H

// ===========================================================================
// end of file
