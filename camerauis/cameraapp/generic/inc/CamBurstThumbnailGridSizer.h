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
* Description:  Class used to for resizing burst mode thumbnails*
*/



#ifndef CAMBURSTTHUMBNAILGRIDSIZER_H
#define CAMBURSTTHUMBNAILGRIDSIZER_H

//  INCLUDES
#include <bitmaptransforms.h>   // For CBitmapScaler

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
// Helper class
class CScaleTask : public CBase
  {   
  // =======================================================
  public:
    
    static CScaleTask* NewL( const CFbsBitmap& aBitmap, 
                             const TSize&      aSize,
                             TInt32            aInfo );
    
    virtual ~CScaleTask();

  private:

    CScaleTask( const TSize&      aSize,
                TInt32            aInfo    );

    void ConstructL( const CFbsBitmap& aBitmap );

  // -------------------------------------------------------  
  public:
  
    inline const CFbsBitmap* Bitmap() const { return iSrc;  };
    inline const TSize&      Size()   const { return iSize; };
    inline TInt32            Info()   const { return iInfo; };

  // =======================================================
  private:
  
    CFbsBitmap* iSrc; //< owned
    TSize       iSize;
    TInt32      iInfo;

  // =======================================================
  };    


// Interface definition for notification of when an image has been resized.
class MBitmapScaler
    {
public: 
    /**
    * Mixin interface to be implemented by any object wishing to receive 
    * notification of scale complete events.
    * @since 2.8
    * @param aErr KErrNone if successful, on error other Epoc codes possible
    * @param aBitmap The resized bitmap (called class takes ownership)
    * @param aInfo Optional parameter that will be passed back when scale completes
    */
    virtual void BitmapScaleCompleteL( TInt aErr, CFbsBitmap* aBitmap, TInt32 aInfo ) = 0 ;
    };

/**
*  Utility class used by the Thumbnail Grid Model for resizing/scaling bitmaps
*
*  @since 2.8
*/
class CCamThumbnailGridSizer : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aObserver The object to be notified of scaling completion
        * @param aSize The default size of the bitmap to scale TO
        * @return An instance of the sizer object
        */
        static CCamThumbnailGridSizer* NewL( MBitmapScaler* aObserver, TSize& aSize );
        
        /**
        * Destructor.
        */
        virtual ~CCamThumbnailGridSizer();
              
    public: // New functions
        
   
    	/**
    	 * Sets new size for the scaling
    	 * @param aThumbSize The new size
    	 */
    	void SetSize( const TSize aThumbSize );
        
    	/**
        * Scales from a source bitmap to a target bitmap, using the dimentions
        * of the target bitmap to define the scale
        * @since 2.8
        * @param aSrc Source bitmap, scaling from        
        */
        void StartScaleL( const CFbsBitmap& aSrc );
        
        /**
        * Scales from a source bitmap to a target bitmap, using the dimensions
        * of the target bitmap to define the scale
        * @since 2.8
        * @param aSrc Source bitmap, scaling from        
        * @param aSize The size of the bitmap to scale to
        * @param aInfo Optional parameter that will be passed back when scale completes
        */
        void StartScaleL( const CFbsBitmap& aSrc, TSize& aSize, TInt32 aInfo );
    
    public: // Functions from base classes
       
    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * From CActive
        */
       	void DoCancel();
    
        /**
        * From CActive
        */
        void RunL();    

    private:

        /**
        * C++ default constructor.
        * @param aObserver The object to be notified of scaling completion
        * @param aSize The default size of the bitmap to scale TO
        */
        CCamThumbnailGridSizer( MBitmapScaler* aObserver, TSize& aSize );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Allows specifying of specific sizing task to start
        * @param aTask The task to begin
        * @since 3.0
        */
        void StartScaleL( CScaleTask* aTask );
       
    private:    // Data
        // Bitmap scaling object, actually does the scaling
        CBitmapScaler* iScaler;        

        // The object to be notified when scaling is complete.
        MBitmapScaler* iObserver;    
        
        // Temporary bitmap used while performing the scaling operation
        CFbsBitmap* iTmpBitmap; 
        
        // The size of the bitmap to scale to
        TSize iThumbSize;                
        
        // List of outstanding scaling tasks
        RPointerArray <CScaleTask> iScaleTaskArray;        
    };

#endif      // CAMBURSTTHUMBNAILGRIDSIZER_H
            
// End of File
