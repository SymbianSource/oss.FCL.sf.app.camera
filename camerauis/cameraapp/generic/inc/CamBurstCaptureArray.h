/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A class that represents a set of capture items
*/


#ifndef CAMBURSTCAPTUREARRAY_H
#define CAMBURSTCAPTUREARRAY_H

//  INCLUDES
 
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CFbsBitmap;
class CCamImageSaveActive;

/**
* A capture burst array, maintains a set of CCamBurstCaptureItem objects
*/
class CCamBurstCaptureArray : public CBase
	{
    public:

        /**
        * Two-phased constructor.
        * @since 2.8
        * @param aImageSaveActive class that manages image saving,
        *        informed if a save request was previously made
        *        for a file now being marked for deletion.
        * @return pointer to the created CCamBurstCaptureArray object
        */
        static CCamBurstCaptureArray* NewL( CCamImageSaveActive& aImageSaveActive ); 

        /**
        * Destructor.
        * @since 2.8
        */
        ~CCamBurstCaptureArray();

    public: // New methods

        /**
        * Sets an item's full file path and image name
        * @since 2.8
        * @param aFullFileName the full file path and image name for the item
        * @param aImageName the name of the image displayed in the UI
        * @param aIndex the index of the item to name.
        */   
        void SetNameL( const TDesC& aFullFileName, const TDesC& aImageName,
            TInt aIndex );

        /**
        * Returns whether a particular file has already been saved.
        * @since 2.8
        * @param aFilename the name of the file to check.
        * @return ETrue if the file has been saved.
        */   
        TBool AlreadySavedFile( const TDesC& aFilename );

        /**
        * Returns whether a particular file is in the process of being saved.
        * @since 2.8
        * @param aFilename the name of the file to check.
        * @return ETrue if the file is in the process of being saved.
        */   
        TBool CurrentlySavingFile( const TDesC& aFilename );

        /**
        * Returns the number of items in the array
        * @since 2.8
        * @return count of items in the array
        */   
        TInt Count() const;

        /**
        * Returns the number of un-deleted items in the array
        * @since 2.8
        * @return count of items in the array that are not marked for deletion
        */   
        inline TInt ImagesRemaining() const;

        /**
        * Delete unwanted items from the array, releasing memory
        * @since 2.8
        * @param aRequiredCount the number of items required
        */   
        void Reset( TInt aRequiredCount = 0 );

        /**
        * Sets the item's full file path and image name
        * @since 2.8
        * @param aFullFileName the full file path and image name for the item
        * @param aImageName the name of the image displayed in the UI
        */   
        void SetNextNameL( const TDesC& aFullFileName, const TDesC& aImageName );

        /**
        * Stores the snapshot bitmap in the item
        * @since 2.8
        * @param aSnapshot the snapshot image returned by the camera engine
        */   
        void SetNextSnapshotL( const CFbsBitmap& aSnapshot );

        /**
        * Sets the item's deletion state
        * @since 2.8
        * @param aItemIndex Index of the item
        * @param aDeleted ETrue if the item has been selected for deletion
        * @return system wide error code or KErrNone
        */   
        TInt SetDeleted( TInt aItemIndex, TBool aDeleted );

        /**
        * Skips the next image if marked for deletion
        * @since 2.8
        * @return Whether or not the image was skipped
        */  
        TBool IsNextImageDeleted();

        /**
        * Returns a pointer to the next full file path and name in the array
        * since the last one requested
        * @since 2.8
        * @return Pointer to the filename 
        */   
        const TDesC& NextFileName();

        /**
        * Returns the index of the next full file path and name in the array
        * since the last one requested by NextFileName()
        * This can be used to determine any unsaved images
        * @since 2.8
        * @return Index of the next filename
        */   
        TInt NextFileIndex() const;

        /**
        * Returns a pointer to the full file path and name
        * @since 2.8
        * @param aItemIndex Index of the item
        * @return Pointer to the filename 
        */   
        const TDesC& FileName( TInt aItemIndex ) const;

        /**
        * Returns a pointer to the image name
        * @since 2.8
        * @param aItemIndex Index of the item
        * @return Pointer to the image name 
        */   
        const TDesC& ImageName( TInt aItemIndex ) const;

        /**
        * Returns a pointer to the snapshot image
        * @since 2.8
        * @param aItemIndex Index of the item
        * @return the snapshot image
        */  
        const CFbsBitmap* Snapshot( TInt aItemIndex ) const;

        /**
        * Indicates whether the item has been selected for deletion
        * @since 2.8
        * @param aItemIndex Index of the item
        * @return the item's deletion state
        */  
        TBool IsDeleted( TInt aItemIndex ) const;
         
        /**
        * Replaces the bitmap in the given index.
        * @since 3.0
        * @param aBitmap The new bitmap to replace current
        * @param aItemIndex Index of the replaced item
        * @return Success of the replacement
        */          
        TBool ReplaceSnapshot( const CFbsBitmap* aBitmap, TInt aItemIndex );    

    private:

        /**
        * A capture burst item, holds the full file path and name, the snapshot image and
        * indication of whether or not the item has been selected for deletion
        */
        class CCamBurstCaptureItem : public CBase
	        {
            public:

                /**
                * Two-phased constructor.
                * @since 2.8
                * 
                * @return pointer to the created CCamBurstCaptureItem object
                */
                static CCamBurstCaptureItem* NewLC(); 

                /**
                * Destructor.
                * @since 2.8
                */
                ~CCamBurstCaptureItem();

                public: // New methods

                /**
                * Sets the item's full file path and image name
                * @since 2.8
                * @param aFullFileName the full file path and image name for the item
                * @param aImageName the name of theimage displayed in the UI
                */   
                void SetNameL( const TDesC& aFullFileName, const TDesC& aImageName );

                /**
                * Stores the snapshot bitmap in the item
                * @since 2.8
                * @param aSnapshot the snapshot image returned by the camera engine
                */   
                void SetSnapshotL( const CFbsBitmap& aSnapshot );

                /**
                * Sets the item's deletion state
                * @since 2.8
                * @param aDeleted ETrue if the item has been selected for deletion
                */   
                void SetDeleted( TBool aDeleted );

                /**
                * Returns a pointer to the full file path and name
                * @since 2.8
                * @return Pointer to the filename 
                */   
                const TDesC& FileName() const;

                /**
                * Returns a pointer to the image name
                * @since 2.8
                * @return Pointer to the image name 
                */   
                const TDesC& ImageName() const;

                /**
                * Returns a reference to the snapshot image
                * @since 2.8
                * @return the snapshot image
                */  
                const CFbsBitmap* Snapshot() const;

                /**
                * Indicates whether the item has been selected for deletion
                * @since 2.8
                * @return the item's deletion state
                */  
                TBool IsDeleted() const;
                   
                /**
                * Replaces the bitmap.
                * @since 3.0
                * @param aBitmap The new bitmap to replace current
                * @return Success of the replacement
                */          
                TBool ReplaceSnapshot( const CFbsBitmap* aBitmap );

            private:

                /**
                * C++ default constructor.
                * @since 2.8
                */     
                CCamBurstCaptureItem();

                /**
                * Second phase construction
                */
                void ConstructL();

            private:
                // The full path and filename for the image
	            HBufC* iFileName;
                // The image name
                HBufC* iImageName;
                // The snapshot image
                CFbsBitmap* iSnapshot;
                // The item's deletion state
                TBool iIsDeleted; 
	        };

    private:

        /**
        * C++ default constructor.
        * @since 2.8
        * @param aImageSaveActive class that manages image saving,
        *        informed if a save request was previously made
        *        for a file now being marked for deletion.
        */     
        CCamBurstCaptureArray( CCamImageSaveActive& aImageSaveActive );

        /**
        * Second phase construction
        */
        void ConstructL();

        /**
        * Create a new item in the array if necessary
        * @since 2.8
        * @param aRequiredIndex the expected index for an item
        */
        void CheckArraySizeL( TInt aRequiredIndex );

    private:

        // Array of items each representing an image in a burst capture
        RPointerArray<CCamBurstCaptureItem> iBurstItems;
        // Active object that manages file saving and deletion
        CCamImageSaveActive& iImageSaveActive;
        // Index used to manage storage of file names and image names
        TInt iNextSetName;
        // Index used to manage retrieval of file names
        TInt iNextGetName;
        // Index used to manage storage of snapshot images
        TInt iNextSnapshot;
        // Counter for the number of undeleted items
        TInt iImagesRemaining;
	};

#include "CamBurstCaptureArray.inl"


#endif      // CAMBURSTCAPTUREARRAY_H   
            
// End of File 
