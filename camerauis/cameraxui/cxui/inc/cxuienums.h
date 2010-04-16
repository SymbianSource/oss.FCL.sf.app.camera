/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CXUIENUMS_H
#define CXUIENUMS_H


namespace CxUi
{
    enum CameraView
    {
        //! Viewfinder view for still capture
        StillPreCaptureView   = 0,

        //! Viewfinder view for video capture
        VideoPreCaptureView = 1,

        //! Preview for captured image
        StillPostCaptureView = 2,

        //! Preview for captured image
        VideoPostCaptureView = 3
    };

    enum StillSceneModes
    {
        //! Automatic
        StillSceneAuto   = 0,
        //! Portrait
        StillScenePortrait   = 1,
        //! Landscape
        StillSceneLandscape   = 2,
        //! Macro
        StillSceneMacro   = 3,
        //! Sports
        StillSceneSports   = 4,
        //! Night
        StillSceneNight   = 5,
        //! Night portrait
        StillSceneNightPortrait   = 6
    };
}

// Application layout strings
namespace CxUiLayout
{

    // Application layout xml files
    static const char *VIDEO_1ST_XML = ":/xml/view_video_precapture.docml";
    static const char *VIDEO_2ND_XML = ":/xml/view_video_precapture_secondary.docml";
    static const char *STILL_1ST_XML = ":/xml/view_still_precapture.docml";
    static const char *STILL_2ND_XML = ":/xml/view_still_precapture_secondary.docml";
    static const char *POSTCAPTURE_XML = ":/xml/view_postcapture.docml";

    static const char *SCENEMODE_SETTING_XML = ":/xml/setting_still_scenemode.docml";
    static const char *SCENEMODE_VIDEO_SETTING_XML = ":/xml/setting_video_scenemode.docml";

    static const char *STANDBY_POPUP_XML = ":/xml/standbymode_popup.docml";
    static const char *ERROR_POPUP_XML = ":/xml/errornote_popup.docml";
    static const char *SETTINGS_SLIDER_DIALOG_XML = ":/xml/setting_slider.docml";

    // Graphics
    static const char *TRANSPARENT_BACKGROUND_GRAPHIC = "qtg_fr_popup_trans";

    // Sections
    static const char *STILL_PRE_CAPTURE_WIDGETS_SECTION = "still_capture_with_widgets";
    static const char *VIDEO_PRE_CAPTURE_WIDGETS_SECTION = "video_capture_with_widgets";
    static const char *POST_CAPTURE_WIDGETS_SECTION = "post_capture_with_widgets";
    static const char *STILL_PRE_CAPTURE_EMBEDDED_SECTION = "still_embedded_mode";
    static const char *STILL_PRE_CAPTURE_STANDALONE_SECTION = "still_standalone_mode";
    static const char *VIDEO_PRE_CAPTURE_EMBEDDED_SECTION = "video_embedded_mode";
    static const char *VIDEO_PRE_CAPTURE_STANDALONE_SECTION = "video_standalone_mode";

    // Widgets
    // In order to be able to refer to a widget, we need to know its object name.
    static const char *STILL_PRE_CAPTURE_VIEW = "still_capture_view";
    static const char *STILL_PRE_CAPTURE_VIEW2 = "still_capture_view_2";
    static const char *STILL_PRE_CAPTURE_TOOLBAR = "toolbar";
    static const char *STILL_PRE_CAPTURE_FLASH_ACTION = "cxui_action_flash";
    static const char *STILL_PRE_CAPTURE_ROOT_WIDGET = "viewfinder_widget";
    static const char *STILL_PRE_CAPTURE_VIEWFINDER = "transparent";
    static const char *STILL_PRE_CAPTURE_ZOOM_SLIDER = "zoom";
    static const char *STILL_PRE_CAPTURE_IMAGE_QUALITY_ACTION = "cxui_action_image_quality";
    static const char *STILL_PRE_CAPTURE_GOTO_VIDEO_ACTION = "cxui_action_goto_video";
    static const char *STILL_PRE_CAPTURE_GOTO_2ND_CAMERA_ACTION = "cxui_action_goto_2_camera";
    static const char *STILL_PRE_CAPTURE_IMAGES_LEFT_LABEL = "images_remaining";
    static const char *STILL_PRE_CAPTURE_QUALITY_ICON = "quality_indicator";
    static const char *STILL_PRE_CAPTURE_POST_CAPTURE_ACTION = "cxui_action_stillpostcapture";
    static const char *STILL_PRE_CAPTURE_INDICATOR_CONTAINER = "indicatorContainer";
    static const char *STILL_PRE_CAPTURE_IMAGES_LEFT_CONTAINER = "images_left_container";

    static const char *STILL_PRE_CAPTURE_SELFTIMER_CONTAINER = "still_precapture_selftimer_container";
    static const char *STILL_PRE_CAPTURE_SELFTIMER_BUTTON_CONTAINER = "selftimer_button_container";
    static const char *STILL_PRE_CAPTURE_SELFTIMER_COUNTER = "still_selftimer_counter";
    static const char *STILL_PRE_CAPTURE_SELFTIMER_START_BUTTON = "still_selftimer_start_button";
    static const char *STILL_PRE_CAPTURE_SELFTIMER_CANCEL_BUTTON = "still_selftimer_cancel_button";

    static const char *VIDEO_PRE_CAPTURE_VIEW  = "video_capture_view";
    static const char *VIDEO_PRE_CAPTURE_VIEW2 = "video_capture_view_2";

    // video
    // In order to be able to refer to a widget, we need to know its object name.
    static const char *VIDEO_PRE_CAPTURE_TOOLBAR = "video_toolbar";
    static const char *VIDEO_PRE_CAPTURE_TOOLBAR_REC = "video_toolbar_recording";
    static const char *VIDEO_PRE_CAPTURE_TOOLBAR_PAUSED = "video_toolbar_paused";
    static const char *VIDEO_PRE_CAPTURE_FLASH_ACTION = "cxui_video_action_light";
    static const char *VIDEO_PRE_CAPTURE_VIEWFINDER = "video_viewfinder_widget";
    static const char *VIDEO_PRE_CAPTURE_ZOOM_SLIDER = "video_zoom";
    static const char *VIDEO_PRE_CAPTURE_RECORDING_ICON = "video_recording_icon";
    static const char *VIDEO_PRE_CAPTURE_ELAPSED_TIME_LABEL = "video_elapsed_time";
    static const char *VIDEO_PRE_CAPTURE_REMAINING_TIME_LABEL = "video_remaining_time";
    static const char *VIDEO_PRE_CAPTURE_TOGGLE_LIGHT_ACTION = "cxui_video_action_toggle_light";
    static const char *VIDEO_PRE_CAPTURE_DEFAULT_MENU = "video_view_menu";
    static const char *VIDEO_PRE_CAPTURE_VIDEO_QUALITY_ACTION = "cxui_video_action_quality";
    static const char *VIDEO_PRE_CAPTURE_GOTO_STILL_ACTION = "cxui_video_action_gotostill";
    static const char *VIDEO_PRE_CAPTURE_GOTO_2ND_CAMERA_ACTION = "cxui_video_action_goto_2_camera";
    static const char *VIDEO_PRE_CAPTURE_POST_CAPTURE_ACTION  = "cxui_video_action_showcapturedvideo";
    static const char *VIDEO_PRE_CAPTURE_INDICATOR_CONTAINER_TOP = "video_indicator_container_top";
    static const char *VIDEO_PRE_CAPTURE_INDICATOR_CONTAINER_BOTTOM = "video_indicator_container_bottom";

    static const char *VIDEO_PRE_CAPTURE_QUALITY_ICON = "video_quality_indicator";

    // standby
    static const char *STANDBY_POPUP = "standbymode_popup";
    static const char *STANDBY_TEXT_WIDGET = "standby_text";

    // error manager
    static const char *ERROR_POPUP = "errornote_popup";
    static const char *ERROR_TEXT_WIDGET = "errornote_text";
    static const char *ERROR_BUTTON_WIDGET = "errornote_button_exit";

    //post capture
    static const char *POSTCAPTURE_VIEW  = "postcapture_view";
    static const char *STILL_POST_CAPTURE_TOOLBAR = "still_postcapture_toolbar";
    static const char *VIDEO_POST_CAPTURE_TOOLBAR = "video_postcapture_toolbar";
    static const char *EMBEDDED_POST_CAPTURE_TOOLBAR = "embedded_postcapture_toolbar";
    static const char *POST_CAPTURE_SNAPSHOT_LABEL = "snapshot_label";

    // common settings dialog layout details
    static const char *SETTINGS_DIALOG_XML = ":/xml/setting.docml";
    static const char *SETTINGS_DIALOG_WIDGET = "settings_dialog";
    static const char *SETTINGS_DIALOG_HEADING_WIDGET = "settings_dialog_heading";
    static const char *SETTINGS_DIALOG_CONTENT_WIDGET = "settings_dialog_list";

    // common slider settings dialog layout details
    static const char *SETTINGS_SLIDER_DIALOG_WIDGET = "slider_settings_dialog";
    static const char *SETTINGS_SLIDER_DIALOG_HEADING_WIDGET = "slider_settings_dialog_heading";
    static const char *SETTINGS_SLIDER_DIALOG_CONTENT_WIDGET = "settings_dialog_slider";

    // scene setting
    static const char *STILL_SCENE_DIALOG  = "still_scenemode_popup";
    static const char *VIDEO_SCENE_DIALOG  = "video_scenemode_popup";
}

namespace CxUiInternal
{
    // Property for setting id string
    static const char *PROPERTY_KEY_SETTING_ID   = "settingskey";
    // Property for settings grid initiated settings
    static const char *PROPERTY_KEY_SETTING_GRID = "settingsgrid";
    static const char *PROPERTY_KEY_TRUE         = "1";
}

Q_DECLARE_METATYPE(CxUi::CameraView)

#endif // CXUIENUMS_H
