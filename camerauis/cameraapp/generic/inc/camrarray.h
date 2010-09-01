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


#ifndef CAM_RARRAY_H
#define CAM_RARRAY_H

class CCamRArray : public CBase
  {
  public:
  
    CCamRArray( TUint32* aArray, TInt aSize )
      : iReferenced    ( NULL  ),
        iReferenceCount( 0     ),
        iArray( aArray ),
        iSize ( aSize  )
      {};
    
    CCamRArray( CCamRArray& aOther )
      {
      CCamRArray::MakeCopy( aOther, *this );
      };

  public:
        
    void Attach() 
      { 
      ++iReferenceCount; 
      };

    void Detach()
      {
      if( --iReferenceCount <= 0 )
        delete this;
      }

    inline TUint32& operator[]( TInt aIndex )
      {
      return iArray[aIndex];
      };

    inline const TUint32& operator[]( TInt aIndex ) const
      {
      return iArray[aIndex];
      };
    
    inline TInt Size() const 
      { 
      return iSize;
      };
  
  private:
  
    static void MakeCopy( CCamRArray& aFrom, CCamRArray& aTo )
      {
      aTo.iArray = aFrom.iArray;
      aTo.iSize  = aFrom.iSize;
      
      for( TInt i = 1; i < aTo.Size(); i++)
        {
        TInt tempp  = aTo[i];
        TInt tempp2 = aFrom[i];
        }
      
      aFrom.Attach();         // aTo is one reference more.
      aTo.iReferenced = &aFrom; // Used to detach aTo from aFrom
      }
  
    ~CCamRArray() 
      {
      if( iReferenced )
        iReferenced->Detach();
      else
        delete [] iArray; 

      iArray = NULL;
      iSize  = 0; 
     };
  
  private:

    TInt        iReferenceCount;
    CCamRArray* iReferenced;

    TUint32* iArray;
    TInt     iSize;
  };

#endif
