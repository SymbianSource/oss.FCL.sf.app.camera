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
* Description:  Container class for video quality data*
*/



#ifndef CAMVIDEOQUALITYLEVEL_H
#define CAMVIDEOQUALITYLEVEL_H

//  INCLUDES
#include <barsread.h>
#include "camconfigurationconstants.h"


// CLASS DECLARATION

/**
*  Container class to hold video quality settings 
*  Read in from resource file
*
*  @since 2.8
*/
class CCamVideoQualityLevel : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aReader resource reader for video quality level resources
        * @return pointer to instances of CCamVideoQualityLevel
        */
        static CCamVideoQualityLevel* NewL( TResourceReader& aReader );

        /**
        * Two-phased constructor.
        * @param aReader resource reader for video quality level resources
        * @param aVariantFlags The flags to specify operator variants
        * @return pointer to instances of CCamVideoQualityLevel
        */
        static CCamVideoQualityLevel* NewL( TVideoQualitySettings& aSettings );

        /**
        * Destructor.
        */
        virtual ~CCamVideoQualityLevel();

    public: // New functions

        /**
        * Return video resolution associated with this quality level
        * @since 2.8
        * @return Video resolution
        */
        TVideoResolution VideoResolution();

        /**
        * Return video frame rate associated with this quality level
        * @since 2.8
        * @return Video frame rate
        */
        TReal FrameRate();

        /**
        * Return video bitrate associated with this quality level
        * @since 2.8
        * @return Video bitrate
        */
        TInt VideoBitRate();

        /**
        * Return audio bitrate associated with this quality level identifier
        * @since 2.8
        * @return audio bitrate
        */
        TInt AudioBitRate();

        /**
        * Return mime type required associated with this video quality level
        * @since 2.8
        * @return descriptor containing mime type
        */
        TPtrC8 VideoMimeType();

        /**
        * Return preferred supplier value associated with this video quality level
        * @since 2.8
        * @return descriptor containing preferred supplier
        */
        TPtrC PreferredSupplier();

        /**
        * Return video codec associated with this video quality level
        * @since 2.8
        * @return descriptor containing codec
        */
        TPtrC8 VideoFileType();

        /**
        * Return audio codec associated with this video quality level
        * @since 2.8
        * @return descriptor containing audio codec
        */
        TPtrC8 AudioType();

        /**
        * Return file type identier associated with this video quality level
        * either Mpeg4 or 3GP
        * @since 2.8
        * @return flags
        */
        TInt VideoType();

        /**
        * Return file length associated with this video quality level
        * either max or short
        * @since 2.8
        * @return flags
        */
        TInt VideoLength();

        /**
        * Return video quality level
        * @since 3.0
        * @return quality level value
        */
        TInt VideoQuality();

    private:

        /**
        * C++ default constructor.
        */
        CCamVideoQualityLevel();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aReader resource reader for video quality level resources
        * @param aVariantFlags The flags to specify operator variants
        */
        void ConstructL( TVideoQualitySettings& aSettings );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aReader resource reader for video quality level resources
        * @param aVariantFlags The flags to specify operator variants
        */
        void ConstructFromResourceL( TResourceReader& aReader );

    private:
        /**
        * Reads an LTEXT element from resources and returns
        * it as HBufC8.
        * @since 2.8
        * @param aReader Resource reader to read descriptor from
        * @return HBufC8 containing string read from resource
        */
        HBufC8* ReadHBufC8L( TResourceReader& aReader ) const;

    private:    // Data
        // Identifer
        TInt iQualityLevel;
        // Video reoslution
        TVideoResolution iVideoResolution;
        // Video Frame rate
        TReal iVideoFrameRate;
        // Video bit rate
        TInt iVideoBitRate;
        // Audio bit rate
        TInt iAudioBitRate;
        // Video mime type
        HBufC8* iVideoMimeType;
        // Preferred supplier
        HBufC* iPreferredSupplier;
        // Video file type
        HBufC8* iVideoCodec;
        // Audio codec
        HBufC8* iAudioCodec;
        // video type identifier (MPeg4 or 3GP)
        TInt iVideoFileType;
        // video length (max or short)
        TInt iVideoLength;
    };

#endif      // CAMVIDEOQUALITYLEVEL_H  
            
// End of File
