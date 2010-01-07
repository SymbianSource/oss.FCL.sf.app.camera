/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Stores video qulaity leve linformation read from resource file*
*/



// INCLUDE FILES

#include <utf.h>         // CnvUtfConverter
#include "CameraVariant.hrh"

 
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "CamVideoQualityLevel.h"
#include "camlogging.h"
#include "CamUtility.h"


const TInt KQualityMaxStringLength = 64;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::CCamVideoQualityLevel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamVideoQualityLevel::CCamVideoQualityLevel()
    {
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::ConstructFromResourceL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamVideoQualityLevel::ConstructFromResourceL( TResourceReader& aReader )
    {
    // read identifier for this quality level
    iQualityLevel = aReader.ReadInt16();
    // read resolution
    iVideoResolution = TVideoResolution(aReader.ReadInt16());
    // read frame rate
    iVideoFrameRate = aReader.ReadReal64();
    // read video bitrate
    iVideoBitRate = aReader.ReadInt32();
    // read audio bitrate
    iAudioBitRate = aReader.ReadInt32();
    // read mime type
    iVideoMimeType = ReadHBufC8L( aReader );
    // read preferred supplier
    iPreferredSupplier = aReader.ReadHBufC16L();
    // read video codec
    iVideoCodec = ReadHBufC8L( aReader );
    // read audio codec
    iAudioCodec = ReadHBufC8L( aReader );
    // read file type
    iVideoFileType = aReader.ReadInt16();
    // read length
    iVideoLength = aReader.ReadInt16();
    
    // if sharing quality - check the flags
/*    if ( iQualityLevel == ECamVideoQualitySharing )
        {
        // sharing should be subqcif
        if ( aVariantFlags & KCamCrQualityVideoSharingIsSubQCIF )
            {
            iVideoResolution = ECamVideoResolutionSubQCIF;
            }
        }  */
    }
    
// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamVideoQualityLevel::ConstructL( TVideoQualitySettings& aSettings )
    {
    // read identifier for this quality level
    iQualityLevel = aSettings.iQualityLevel;
    // read resolution
    iVideoResolution = TVideoResolution(aSettings.iVideoResolution);
    // read frame rate
    iVideoFrameRate = aSettings.iVideoFrameRate;
    // read video bitrate
    iVideoBitRate = aSettings.iVideoBitRate;
    // read audio bitrate
    iAudioBitRate = aSettings.iAudioBitRate;

    // read mime type    
    iVideoMimeType = HBufC8::NewL( KQualityMaxStringLength );
    iVideoMimeType->Des().Copy(aSettings.iVideoMimeType);

    // read preferred supplier
    iPreferredSupplier = HBufC::NewL( KQualityMaxStringLength ) ;
    iPreferredSupplier->Des().Copy( aSettings.iPreferredSupplier); 

    // read video codec
    iVideoCodec = HBufC8::NewL( KQualityMaxStringLength );
    iVideoCodec->Des().Copy( aSettings.iVideoCodec );
    // read audio codec
    iAudioCodec = HBufC8::NewL( KQualityMaxStringLength );
    iAudioCodec->Des().Copy( aSettings.iAudioCodec );

    // read file type*
    iVideoFileType = aSettings.iVideoFileType;
    // read length
    iVideoLength = aSettings.iVideoLength;

    // if sharing quality - check the flags
/*    if ( iQualityLevel == ECamVideoQualitySharing )
        {
        // sharing should be subqcif
        if ( aVariantFlags & KCamCrQualityVideoSharingIsSubQCIF )
            {
            iVideoResolution = ECamVideoResolutionSubQCIF;
            }
        }*/
    }


// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamVideoQualityLevel* CCamVideoQualityLevel::NewL( TResourceReader& aReader )
    {
    CCamVideoQualityLevel* self = new( ELeave ) CCamVideoQualityLevel;
    
    CleanupStack::PushL( self );
    self->ConstructFromResourceL( aReader );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamVideoQualityLevel* CCamVideoQualityLevel::NewL( TVideoQualitySettings& aSettings )
    {
    CCamVideoQualityLevel* self = new( ELeave ) CCamVideoQualityLevel;
    CleanupStack::PushL( self );
    self->ConstructL( aSettings );
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CCamVideoQualityLevel::~CCamVideoQualityLevel()
  {
  PRINT( _L("Camera => ~CCamVideoQualityLevel" ));
  delete iVideoMimeType;
  delete iPreferredSupplier;
  delete iVideoCodec;
  delete iAudioCodec;
  PRINT( _L("Camera <= ~CCamVideoQualityLevel" )); 
  }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::VideoResolution
// Return video resolution associated with this quality level
// -----------------------------------------------------------------------------
//
TVideoResolution CCamVideoQualityLevel::VideoResolution()
    {
    return iVideoResolution;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::FrameRate
// Return video frame rate associated with this quality level
// -----------------------------------------------------------------------------
//
TReal CCamVideoQualityLevel::FrameRate()
    {
    return iVideoFrameRate;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::VideoBitRate
// Return video bitrate associated with this quality level
// -----------------------------------------------------------------------------
//
TInt CCamVideoQualityLevel::VideoBitRate()
    {
    return iVideoBitRate;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::AudioBitRate
// Return audio bitrate associated with this quality level identifier
// -----------------------------------------------------------------------------
//
TInt CCamVideoQualityLevel::AudioBitRate()
    {
    return iAudioBitRate;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::VideoMimeType
// Return mime type required associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC8 CCamVideoQualityLevel::VideoMimeType()
    {
    return *iVideoMimeType;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::PreferredSupplier
// Return preferred supplier value associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC CCamVideoQualityLevel::PreferredSupplier()
    {
    return *iPreferredSupplier;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::VideoFileType
// Return video codec associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC8 CCamVideoQualityLevel::VideoFileType()
    {
    return *iVideoCodec;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::AudioType
// Return audio codec associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC8 CCamVideoQualityLevel::AudioType()
    {
    return *iAudioCodec;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::VideoType
// Return file type identier associated with this video quality level
// either Mpeg4 or 3GP
// -----------------------------------------------------------------------------
//
TInt CCamVideoQualityLevel::VideoType()
    {
    return iVideoFileType;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::VideoLength
// Return length associated with this video quality level
// either max or short
// -----------------------------------------------------------------------------
//
TInt CCamVideoQualityLevel::VideoLength()
    {
    return iVideoLength;
    }

// -----------------------------------------------------------------------------
// CCamVideoQualityLevel::VideoQuality
// Return video quality level
// -----------------------------------------------------------------------------
//
TInt CCamVideoQualityLevel::VideoQuality()
    {
    return iQualityLevel;
    }
    
// ---------------------------------------------------------------------------
// CCamVideoQualityLevel::ReadHBufC8L
// Reads an LTEXT element from resources and returns
// it as HBufC8.
// ---------------------------------------------------------------------------
//
HBufC8* CCamVideoQualityLevel::ReadHBufC8L( TResourceReader& aReader ) const
    {
    HBufC* tempBuf = NULL;
    tempBuf = aReader.ReadHBufCL();
    if ( !tempBuf )
        {
        return NULL;
        }

    CleanupStack::PushL( tempBuf );

    // Assume aSource only contains 7-bit ASCII characters.
    // (ie. length of the string in UTF-8 is equal to the number of
    // characters)
    HBufC8* res = HBufC8::NewLC( tempBuf->Length() );
    TPtr8 resPtr( res->Des() );
    TPtr16 sourcePtr( tempBuf->Des() );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( resPtr, sourcePtr );
    CleanupStack::Pop(); // res
    CleanupStack::PopAndDestroy(); // tempBuf
    return res;
    }


//  End of File  
