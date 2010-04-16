/*
* Copyright (c) 2009, Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation and/or its subsidiary(-ies).
*
* Contributors:
*
*
* Description:
* Implementation file for CxeQualityPresetsSymbian class
*
*/

#include <e32std.h> // For Symbian types used in mmsenginedomaincrkeys.h
#include <mmsenginedomaincrkeys.h>

#include "cxutils.h"
#include "cxenamespace.h"
#include "cxesettings.h"
#include "cxequalitydetails.h"
#include "cxequalitypresetssymbian.h"


// constants
namespace
{
    // Display IDs for cameras, used when requesting data from ICM
    const int   PRIMARY_CAMERA_DISPLAY_ID   = 2;
    const int   SECONDARY_CAMERA_DISPLAY_ID = 3;

    const int   ONE_MILLION    = 1000000;
    const qreal ASPECT_16_BY_9 = (16/9.0);
    const qreal DELTA_ERROR    = 0.20;

    // ICM "camcorderVisible" parameter value below this means sharing aka mms quality.
    const int   MMS_QUALITY_CAMCORDERVISIBLE_LIMIT = 200;
}


/* !
* Intializes ImagingConfigManager
*/
CxeQualityPresetsSymbian::CxeQualityPresetsSymbian(CxeSettings &settings)
    : mSettings(settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    TRAPD(err,  mIcm = CImagingConfigManager::NewL());

    if (err) {
        CX_DEBUG(("Error during ICM initialization error code = %d", err));
        mIcm = NULL;
    }

    CX_DEBUG_EXIT_FUNCTION();

}

/* !
* CxeQualityPresetsSymbian::~CxeQualityPresetsSymbian()
*/
CxeQualityPresetsSymbian::~CxeQualityPresetsSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mIcm;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
This function returns sorted list of image qualities from highest to lowest resolution.
i.e. first element in the list represent highest supported image resolution and so on.
@param cameraId The CameraIndex which defines which camera we are using primary/secondary.
Returns sorted list of image qualities in descending order.
*/
QList<CxeImageDetails> CxeQualityPresetsSymbian::imageQualityPresets(Cxe::CameraIndex cameraId)
{
    CX_DEBUG_ENTER_FUNCTION();

    QList<CxeImageDetails> presetList;

    if (!mIcm) {
        CX_DEBUG(("ICM not initialized, returning empty image qualities list"));
        return presetList;
    }
    int totalLevels = mIcm->NumberOfImageQualityLevels();

    CX_DEBUG(("Total image quality levels = %d", totalLevels));
    CArrayFixFlat<TUint>* levels= new CArrayFixFlat<TUint>(totalLevels);

    // Get camera display id based on camera index
    int displayId = cameraId == Cxe::SecondaryCameraIndex
                  ? SECONDARY_CAMERA_DISPLAY_ID : PRIMARY_CAMERA_DISPLAY_ID;

    TRAPD(err, mIcm->GetImageQualityLevelsL(*levels, displayId));

    if (err == KErrNone) {
        CX_DEBUG(( "Reading image quality sets one by one."));
        TImageQualitySet set;
        int numLevels = levels->Count();
        for(int i = 0; i < numLevels; i++) {
            mIcm->GetImageQualitySet(set, levels->At(i), displayId);
            if (set.iCamcorderVisible > 0) {
                // create new quality preset based on the quality set values
                CxeImageDetails newPreset = createImagePreset(set);

                // print debug prints
                debugPrints(newPreset);

                // append to the list of presets
                presetList.append(newPreset);
            }
        }
        CX_DEBUG(( "Sorting image qualities"));
        // Sorting result list according to height pixel size from highest to lowest
        // i.e. descending order keeping the highest resolution first.
        qSort(presetList.begin(), presetList.end(), qGreater<CxeImageDetails>());
    }

    delete levels;
    levels = NULL;

    CX_DEBUG_EXIT_FUNCTION();
    return presetList;
}



/*!
This function returns sorted list of video qualities from highest to lowest resolution.
i.e. first element in the list represent highest supported video resolution and so on.
@param cameraId The CameraIndex which defines which camera we are using primary/secondary.
Returns sorted list if image qualities in descending order.
*/
QList<CxeVideoDetails> CxeQualityPresetsSymbian::videoQualityPresets(Cxe::CameraIndex cameraId)
{
    CX_DEBUG_ENTER_FUNCTION();

    QList<CxeVideoDetails> presetList;

    if (!mIcm) {
        CX_DEBUG(("ICM not initialized, returning empty video qualities list"));
        return presetList;
    }
    int totalLevels = mIcm->NumberOfVideoQualityLevels();

    CX_DEBUG(("Total video quality levels = %d", totalLevels));
    CArrayFixFlat<TUint>* levels= new CArrayFixFlat<TUint>(totalLevels);

    int displayId = cameraId == Cxe::SecondaryCameraIndex
                  ? SECONDARY_CAMERA_DISPLAY_ID : PRIMARY_CAMERA_DISPLAY_ID;

    TRAPD(err, mIcm->GetVideoQualityLevelsL(*levels, displayId));

    if (err == KErrNone) {
        CX_DEBUG(( "Reading video quality sets one by one."));
        TVideoQualitySet set;
        int numLevels = levels->Count();
        for(int i = 0; i < numLevels; i++) {
            mIcm->GetVideoQualitySet(set, levels->At(i), displayId);

            // Disable all video sizes larger than VGA
            if (set.iCamcorderVisible > 0 && set.iVideoWidth <= 864) {
                // create new quality preset
                CxeVideoDetails newPreset = createVideoPreset(set);

                // print debug prints
                debugPrints(newPreset);

                // append to the list of presets
                presetList.append(newPreset);
            }
        }
        CX_DEBUG(( "Sorting image qualities"));
        // Sorting result list according to height pixel size from highest to lowest
        // i.e. descending order keeping the highest resolution first.
        qSort(presetList.begin(), presetList.end(), qGreater<CxeVideoDetails>());
    }

    delete levels;
    levels = NULL;

    // Get the average video bitrate scaler
    TCamcorderMMFPluginSettings mmfPluginSettings;
    mIcm->GetCamcorderMMFPluginSettings(mmfPluginSettings);
    mCMRAvgVideoBitRateScaler = mmfPluginSettings.iCMRAvgVideoBitRateScaler;

    CX_DEBUG_EXIT_FUNCTION();
    return presetList;
}



/*!
* Creates a new image preset based on TImageQualitySet values from ICM.
@ param set contains the ICM configuration data
@ returns CxeImageQuality struct
*/
CxeImageDetails CxeQualityPresetsSymbian::createImagePreset(TImageQualitySet set)
{
    CxeImageDetails newPreset;
    // set setting values from quality set
    newPreset.mWidth = set.iImageWidth;
    newPreset.mHeight = set.iImageHeight;
    newPreset.mImageFileExtension = toString(set.iImageFileExtension);
    newPreset.mEstimatedSize = set.iEstimatedSize;
    newPreset.mMpxCount = calculateMegaPixelCount(set.iImageWidth,
                                                  set.iImageHeight);
    newPreset.mAspectRatio = calculateAspectRatio(set.iImageWidth,
                                                  set.iImageHeight);

    return newPreset;
}



/*!
* Creates a new video preset based on TVideoQualitySet values from ICM.
*/
CxeVideoDetails CxeQualityPresetsSymbian::createVideoPreset(TVideoQualitySet set)
{
    CxeVideoDetails newPreset;
    // set setting values from quality set
    newPreset.mWidth = set.iVideoWidth;
    newPreset.mHeight = set.iVideoHeight;

    // Check if this is a sharing (mms) quality, and set size limit accordingly.
    if (set.iCamcorderVisible < MMS_QUALITY_CAMCORDERVISIBLE_LIMIT) {
        QVariant size;
        mSettings.get(KCRUidMmsEngine.iUid, KMmsEngineMaximumSendSize, Cxe::Repository, size);
        CX_DEBUG(("CxeQualityPresetsSymbian - Got MMS quality size limit: %d", size.toInt()));
        newPreset.mMaximumSizeInBytes = size.toInt();
    } else {
        // Zero means no limit.
        newPreset.mMaximumSizeInBytes = 0;
    }

    newPreset.mVideoBitRate = set.iVideoBitRate;
    newPreset.mAudioBitRate = set.iAudioBitRate;
    newPreset.mVideoFrameRate = set.iVideoFrameRate;
    newPreset.mVideoFileMimeType = toString(set.iVideoFileMimeType);
    newPreset.mPreferredSupplier = toString(set.iPreferredSupplier);
    newPreset.mVideoCodecMimeType = toString(set.iVideoCodecMimeType);
    newPreset.mAspectRatio = calculateAspectRatio(set.iVideoWidth,
                                                  set.iVideoHeight);

    // Convert FourCC value from TFourCC to ascii string
    const int KFourCCLength = 5; // 4 characters + '\0'
    TText8 fourCCBuf[KFourCCLength];
    TPtr8 fourCC(fourCCBuf, KFourCCLength);
    set.iAudioFourCCType.FourCC(&fourCC);
    fourCC.Append('\0');

    // set audiotype
    newPreset.mAudioType = toString(fourCCBuf);

    return newPreset;
}


/*!
* Returns Aspect ratio of the image.
*/
qreal CxeQualityPresetsSymbian::avgVideoBitRateScaler()
{
    CX_DEBUG_IN_FUNCTION();
    return mCMRAvgVideoBitRateScaler;
}


/*!
@ param width - image/video quality width
@ param height - image/video quality height
* Returns Aspect ratio of the image/video.
*/
Cxe::AspectRatio CxeQualityPresetsSymbian::calculateAspectRatio(int width, int height) const
{
    Cxe::AspectRatio aspectRatio = Cxe::AspectRatio4to3;

    qreal ratio = 0;
    if (height != 0) {
        ratio = (1.0 * width) / height;

        qreal delta16by9 = ratio - ASPECT_16_BY_9;
        if (abs(delta16by9) < DELTA_ERROR) {
            aspectRatio = Cxe::AspectRatio16to9;
        }
    }

    return aspectRatio;
}



/*!
* CxeQualityPresetsSymbian::calculateMegaPixelCount
@ param imageWidth refers to the image resolution width
@ param imageHeight refers to the image resolution height
@ returns megapixel count string
*/
QString
CxeQualityPresetsSymbian::calculateMegaPixelCount(int imageWidth, int imageHeight)
{
    QString mpxCountString;
    qreal size = imageWidth * imageHeight * 1.0;
    int mpxCount = (size/ONE_MILLION) * 10;

    if ((mpxCount % 10) == 0) {
        int value = mpxCount / 10;
        mpxCountString.setNum(value);
    } else {
        qreal value = mpxCount / 10.0;
        if ((mpxCount % 10) < 5) {
            mpxCountString.setNum(value, 'f', 1);
        } else {
            int temp = ceil(value);
            mpxCountString.setNum(temp);
        }
    }

    return mpxCountString;


}


/*!
Operator to sort values in ascending order.
@param s1 type of data to be sorted.
*/
bool CxeImageDetails::operator<(const CxeImageDetails& s1) const
{
    return mHeight < s1.mHeight;
}


/*!
Operator to sort values in ascending order.
@param s1 type of data to be sorted.
*/
bool CxeVideoDetails::operator<(const CxeVideoDetails& s1) const
{
    return mHeight < s1.mHeight;
}


/*
* Converts TUint8* to QString
*/
QString CxeQualityPresetsSymbian::toString(const TUint8* aData)
{
    return QString::fromLatin1((char*)aData);
}



/*!
* Helper method to enable debug prints.
@ param  Video quality preset values are printed out for debugging
*/
void CxeQualityPresetsSymbian::debugPrints(CxeVideoDetails preset)
{
    CX_DEBUG(("Video quality details"));
    CX_DEBUG(("Video resolution (%d,%d)", preset.mWidth, preset.mHeight));
    CX_DEBUG(("Audio bitrate = %d)", preset.mAudioBitRate));
    CX_DEBUG(("Video bitrate = %d)", preset.mVideoBitRate));
    CX_DEBUG(("Video frame rate = %f)", preset.mVideoFrameRate));
    CX_DEBUG(("Audio type: %s", preset.mAudioType.toAscii().data()));
    CX_DEBUG(("Video file MIME type: %s", preset.mVideoFileMimeType.toAscii().data()));
    CX_DEBUG(("Video preferred supplier: %s", preset.mPreferredSupplier.toAscii().data()));
    CX_DEBUG(("Video codec MIME type: %s", preset.mVideoCodecMimeType.toAscii().data()));
    QString aspectRatio;
    if (preset.mAspectRatio == Cxe::AspectRatio4to3) {
        aspectRatio = QString("4:3");
    } else if (preset.mAspectRatio == Cxe::AspectRatio16to9) {
        aspectRatio = QString("16:9");
    }
    CX_DEBUG(("Video aspect ratio: %s", aspectRatio.toAscii().data()));
}


/*!
* Helper method to enable debug prints.
@ param  Image quality preset values are printed out for debugging
*/
void CxeQualityPresetsSymbian::debugPrints(CxeImageDetails newPreset)
{
    CX_DEBUG(("Image quality details"));
    CX_DEBUG(("Image resolution (%d,%d)", newPreset.mWidth, newPreset.mHeight));
    CX_DEBUG(("Estimated size in bytes = %d)", newPreset.mEstimatedSize));
    CX_DEBUG(("Megapixels: %s", newPreset.mMpxCount.toAscii().data()));
    CX_DEBUG(("Image file extension: %s", newPreset.mImageFileExtension.toAscii().data()));

    QString aspectRatio;

    if (newPreset.mAspectRatio == Cxe::AspectRatio4to3) {
        aspectRatio = QString("4:3");
    } else {
        aspectRatio = QString("16:9");
    }
    CX_DEBUG(("Image aspect ratio: %s", aspectRatio.toAscii().data()));
}
