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
* Description:  Stores video qulaity leve linformation read from resource file*
*/



// INCLUDE FILES

#include <utf.h>         // CnvUtfConverter
#include "CameraVariant.hrh"
#include "CamSettings.hrh"
#include "CamSettingsInternal.hrh"
#include "GSCamVideoQualityLevel.h"
#include "camlogging.h"
#include "CamUtility.h"


const TInt KQualityMaxStringLength = 64;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::CGSCamVideoQualityLevel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGSCamVideoQualityLevel::CGSCamVideoQualityLevel()
    {
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::ConstructFromResourceL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSCamVideoQualityLevel::ConstructFromResourceL( TResourceReader& aReader )
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
// CGSCamVideoQualityLevel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGSCamVideoQualityLevel::ConstructL( TVideoQualitySettings& aSettings )
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
// CGSCamVideoQualityLevel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSCamVideoQualityLevel* CGSCamVideoQualityLevel::NewL( TResourceReader& aReader )
    {
    CGSCamVideoQualityLevel* self = new( ELeave ) CGSCamVideoQualityLevel;
    
    CleanupStack::PushL( self );
    self->ConstructFromResourceL( aReader );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGSCamVideoQualityLevel* CGSCamVideoQualityLevel::NewL( TVideoQualitySettings& aSettings )
    {
    CGSCamVideoQualityLevel* self = new( ELeave ) CGSCamVideoQualityLevel;
    
    CleanupStack::PushL( self );
    self->ConstructL( aSettings );
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CGSCamVideoQualityLevel::~CGSCamVideoQualityLevel()
  {
  PRINT( _L("Camera => ~CGSCamVideoQualityLevel" ));
  delete iVideoMimeType;
  delete iPreferredSupplier;
  delete iVideoCodec;
  delete iAudioCodec;
  PRINT( _L("Camera <= ~CGSCamVideoQualityLevel" )); 
  }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::VideoResolution
// Return video resolution associated with this quality level
// -----------------------------------------------------------------------------
//
TVideoResolution CGSCamVideoQualityLevel::VideoResolution()
    {
    return iVideoResolution;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::FrameRate
// Return video frame rate associated with this quality level
// -----------------------------------------------------------------------------
//
TReal CGSCamVideoQualityLevel::FrameRate()
    {
    return iVideoFrameRate;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::VideoBitRate
// Return video bitrate associated with this quality level
// -----------------------------------------------------------------------------
//
TInt CGSCamVideoQualityLevel::VideoBitRate()
    {
    return iVideoBitRate;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::AudioBitRate
// Return audio bitrate associated with this quality level identifier
// -----------------------------------------------------------------------------
//
TInt CGSCamVideoQualityLevel::AudioBitRate()
    {
    return iAudioBitRate;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::VideoMimeType
// Return mime type required associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC8 CGSCamVideoQualityLevel::VideoMimeType()
    {
    return *iVideoMimeType;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::PreferredSupplier
// Return preferred supplier value associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC CGSCamVideoQualityLevel::PreferredSupplier()
    {
    return *iPreferredSupplier;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::VideoFileType
// Return video codec associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC8 CGSCamVideoQualityLevel::VideoFileType()
    {
    return *iVideoCodec;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::AudioType
// Return audio codec associated with this video quality level
// -----------------------------------------------------------------------------
//
TPtrC8 CGSCamVideoQualityLevel::AudioType()
    {
    return *iAudioCodec;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::VideoType
// Return file type identier associated with this video quality level
// either Mpeg4 or 3GP
// -----------------------------------------------------------------------------
//
TInt CGSCamVideoQualityLevel::VideoType()
    {
    return iVideoFileType;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::VideoLength
// Return length associated with this video quality level
// either max or short
// -----------------------------------------------------------------------------
//
TInt CGSCamVideoQualityLevel::VideoLength()
    {
    return iVideoLength;
    }

// -----------------------------------------------------------------------------
// CGSCamVideoQualityLevel::VideoQuality
// Return video quality level
// -----------------------------------------------------------------------------
//
TInt CGSCamVideoQualityLevel::VideoQuality()
    {
    return iQualityLevel;
    }
    
// ---------------------------------------------------------------------------
// CGSCamVideoQualityLevel::ReadHBufC8L
// Reads an LTEXT element from resources and returns
// it as HBufC8.
// ---------------------------------------------------------------------------
//
HBufC8* CGSCamVideoQualityLevel::ReadHBufC8L( TResourceReader& aReader ) const
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
